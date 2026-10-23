#include "resource_pool.hpp"

#include <atomic>
#include <cstring>
#include <new>

namespace sotc {
namespace {

// 0xf590's data/control callback. A missing deallocator is only visible when a free is actually
// required (previous control count == 1); a plain decrement needs no hook.
class KeyDeallocator final : public SharedStrDeallocatorF590 {
public:
    explicit KeyDeallocator(SharedStrDeallocatorF590* inner) noexcept : inner_(inner) {}

    [[nodiscard]] bool failed() const noexcept { return failed_; }

    void deallocate(const void* allocation, SharedStrDeallocationKindF590 kind) noexcept override {
        if (inner_ == nullptr) {
            failed_ = true;
            return;
        }
        inner_->deallocate(allocation, kind);
    }

private:
    SharedStrDeallocatorF590* inner_ = nullptr;
    bool failed_ = false;
};

[[nodiscard]] bool release_key(HashedStr& key, SharedStrDeallocatorF590* deallocator) noexcept {
    if (key.str.ctl == nullptr) return true;
    KeyDeallocator adapter{deallocator};
    release_shared_str_f590(key.str, adapter);
    return !adapter.failed();
}

struct NullWrapperHolder {
    ResourceWrapper wrapper;

    NullWrapperHolder() noexcept : wrapper(HashedStr{}, nullptr) {
        // 0x14d880's first call returns the object with one permanent reference.
        wrapper.refs.store(1, std::memory_order_seq_cst);
    }
};

// Inline destroy of 0x14ccc0; the caller holds the +0x50 guard. Clears the guard before the destructor
// and the deallocation so a hook that frees the storage is never followed by a write.
[[nodiscard]] bool destroy_locked(ResourceWrapper& wrapper,
                                  const ResourcePoolHooks& hooks) noexcept;

}  // namespace

ResourceWrapper& null_resource_wrapper() noexcept {
    static NullWrapperHolder holder;
    return holder.wrapper;
}

ResourceWrapper::ResourceWrapper(const HashedStr& key, ResourcePool* pool) noexcept : pool(pool) {
    list_next = this;
    list_prev = this;
    copy_hashed_14c3a0(this->key, key);
    state = kResourceWrapperInitialState;
    f4 = kResourceWrapperInitialState;
}

bool ResourceIdLess::operator()(const HashedStr& a, const HashedStr& b) const noexcept {
    if (a.hash != b.hash) return a.hash < b.hash;
    const char* const a_text =
        a.str.data != nullptr ? reinterpret_cast<const char*>(a.str.data) : "";
    const char* const b_text =
        b.str.data != nullptr ? reinterpret_cast<const char*>(b.str.data) : "";
    return std::strcmp(a_text, b_text) < 0;
}

void add_reference(ResourceWrapper& wrapper) noexcept {
    (void)wrapper.refs.fetch_add(1, std::memory_order_seq_cst);
}

bool release_dependency(ResourceWrapper& wrapper, const ResourcePoolHooks& hooks) noexcept {
    const std::uint32_t previous = wrapper.deps.fetch_sub(1, std::memory_order_seq_cst);
    if (previous != 1) return true;
    if (wrapper.pool == nullptr) return true;

    std::int64_t expected = 0;
    if (!wrapper.guard.compare_exchange_strong(expected, -1, std::memory_order_seq_cst)) return false;

    if (wrapper.deps.load(std::memory_order_seq_cst) != 0) {
        wrapper.guard.store(0, std::memory_order_release);
        return true;
    }

    bool ok = true;
    if (wrapper.state == kResourceWrapperInitialState && wrapper.payload != nullptr) {
        // The original calls manager slot +0x28 (DeleteResource); the manager body is a hook.
        if (hooks.destroy_payload == nullptr || !hooks.destroy_payload(wrapper.payload)) {
            ok = false;
        } else {
            wrapper.payload = nullptr;
        }
    }
    for (std::size_t i = 0; i < wrapper.children.size(); ++i) {
        ResourceRef& child = wrapper.children[i];
        if (!child.owned) continue;
        if (child.ptr == nullptr || !release_dependency(*child.ptr, hooks)) {
            ok = false;
            break;
        }
        child.owned = false;
    }
    wrapper.guard.store(0, std::memory_order_release);
    return ok;
}

bool request_state(ResourceWrapper& wrapper, std::uint32_t priority, const HashedStr& requester,
                   const ResourcePoolHooks& hooks) noexcept {
    // The owner-id copy at 0x14d132 and the child re-request at 0x14d0ee are unreachable in this
    // build: r12d is set to 1 at 0x14cf34 and the test at 0x14d0e5 always jumps over both.
    (void)requester;

    // 0x14cf3f: the only +0x40 increaser, before every check.
    (void)wrapper.deps.fetch_add(1, std::memory_order_seq_cst);

    // 0x14cf4a: signed `jle` on `state <= priority`. Sites 1-8 (priority 13, state 11) take this exit.
    if (static_cast<std::int32_t>(wrapper.state) <= static_cast<std::int32_t>(priority)) return true;

    // 0x14cf5b: signed `jle` on the (wrapped) dependency count.
    if (static_cast<std::int32_t>(wrapper.deps.load(std::memory_order_seq_cst)) <= 0) return true;
    // Report §6.3: a loaded payload needs the blocked state machine, so the native model refuses.
    if (wrapper.payload != nullptr) return false;
    // 0x14cfe7: the null wrapper singleton returns without a transition.
    if (&wrapper == &null_resource_wrapper()) return true;
    // 0x14cff0: flags60 != 0 returns without a transition.
    if (wrapper.flags60 != 0) return true;
    if (wrapper.pool == nullptr) return false;

    std::int64_t expected = 0;
    if (!wrapper.guard.compare_exchange_strong(expected, -1, std::memory_order_seq_cst)) return false;

    // 0x14d050..0x14d0df re-checks the same conditions after taking the guard.
    if (static_cast<std::int32_t>(wrapper.deps.load(std::memory_order_seq_cst)) <= 0 ||
        wrapper.payload != nullptr || &wrapper == &null_resource_wrapper() ||
        wrapper.flags60 != 0) {
        wrapper.guard.store(0, std::memory_order_release);
        return true;
    }

    bool ok = true;
    if (priority == kResourceWrapperLockPriority13) {
        // 0x14d193: state = state < 10 ? state : 5, then the engine notification.
        const std::uint32_t next = wrapper.state < 10 ? wrapper.state : 5;
        if (hooks.state_write == nullptr) {
            wrapper.guard.store(0, std::memory_order_release);
            return false;
        }
        wrapper.state = next;
        ok = hooks.state_write(wrapper, next);
    } else if (priority <= 9) {
        // 0x14d1af: state = priority, then the engine notification. Site 9 writes 9 here.
        if (hooks.state_write == nullptr) {
            wrapper.guard.store(0, std::memory_order_release);
            return false;
        }
        wrapper.state = priority;
        ok = hooks.state_write(wrapper, priority);
    } else if (priority == 12) {
        // 0x14d1f4: 8 if state == 11, no engine notification. The state == 11 row is dominated by the
        // earlier `state <= priority` return, so this write is unreachable from a fresh wrapper.
        if (wrapper.state == kResourceWrapperInitialState) wrapper.state = 8;
    } else {
        // 0x14d1af -> 0x14d1ef -> epilogue: priorities 10, 11 and > 13 write no state and notify nobody.
    }
    wrapper.guard.store(0, std::memory_order_release);
    return ok;
}

bool remove_reference(ResourceWrapper& wrapper, const ResourcePoolHooks& hooks) noexcept {
    const std::uint32_t previous = wrapper.refs.fetch_sub(1, std::memory_order_seq_cst);
    if (previous != 1) return true;

    // 0x14ccf6: pool slot 9 is called only when the pool back-pointer is non-null.
    if (wrapper.pool != nullptr && !wrapper.pool->erase(&wrapper)) return false;
    // 0x14cd0c: resurrection check after the callback.
    if (wrapper.refs.load(std::memory_order_seq_cst) != 0) return true;

    std::int64_t expected = 0;
    if (!wrapper.guard.compare_exchange_strong(expected, -1, std::memory_order_seq_cst)) return false;

    if (hooks.manager_delete != nullptr) {
        // The manager owns destruction. Clear the guard first: a hook that frees the storage must not
        // be followed by a write (the original clears it after the call because its manager defers).
        wrapper.guard.store(0, std::memory_order_release);
        return hooks.manager_delete(wrapper);
    }
    return destroy_locked(wrapper, hooks);
}

namespace {

bool destroy_locked(ResourceWrapper& wrapper, const ResourcePoolHooks& hooks) noexcept {
    if (hooks.deallocate == nullptr) {
        wrapper.guard.store(0, std::memory_order_release);
        return false;
    }
    if (wrapper.payload != nullptr && hooks.destroy_payload == nullptr) {
        wrapper.guard.store(0, std::memory_order_release);
        return false;
    }

    SharedStrDeallocatorF590* const key_deallocator =
        wrapper.pool != nullptr ? wrapper.pool->key_deallocator() : nullptr;

    bool ok = true;
    if (wrapper.payload != nullptr) {
        if (!hooks.destroy_payload(wrapper.payload)) {
            ok = false;
        } else {
            wrapper.payload = nullptr;
        }
    }
    // 0x14ce2c: release the owner-id holder first, then the children, then the own key.
    if (!release_key(wrapper.owner_id, key_deallocator)) ok = false;
    if (ok) {
        for (std::size_t i = 0; i < wrapper.children.size(); ++i) {
            ResourceRef& child = wrapper.children[i];
            if (child.ptr == nullptr) {
                ok = false;
                break;
            }
            if (child.owned) {
                if (!release_dependency(*child.ptr, hooks)) {
                    ok = false;
                    break;
                }
                child.owned = false;
            }
            if (!remove_reference(*child.ptr, hooks)) {
                ok = false;
                break;
            }
        }
    }
    if (!ok) {
        wrapper.guard.store(0, std::memory_order_release);
        return false;
    }
    if (!release_key(wrapper.key, key_deallocator)) {
        wrapper.guard.store(0, std::memory_order_release);
        return false;
    }
    wrapper.guard.store(0, std::memory_order_release);
    wrapper.~ResourceWrapper();
    return hooks.deallocate(&wrapper);
}

}  // namespace

bool destroy_resource_wrapper(ResourceWrapper& wrapper, const ResourcePoolHooks& hooks) noexcept {
    std::int64_t expected = 0;
    if (!wrapper.guard.compare_exchange_strong(expected, -1, std::memory_order_seq_cst)) return false;
    return destroy_locked(wrapper, hooks);
}

bool resource_wrapper_set_payload(ResourceWrapper& wrapper, const HashedStr& key,
                                  const ResourcePoolHooks& hooks) noexcept {
    if (wrapper.payload != nullptr) return false;
    if (hooks.make_payload == nullptr) return false;
    void* const payload = hooks.make_payload(key);
    if (payload == nullptr) return false;
    wrapper.payload = payload;
    return true;
}

bool lock_resource_ref(ResourceRef& ref, std::uint32_t priority, const HashedStr& requester,
                       const ResourcePoolHooks& hooks) noexcept {
    if (ref.owned) return true;
    if (ref.ptr == nullptr) return false;
    if (!request_state(*ref.ptr, priority, requester, hooks)) return false;
    ref.owned = true;
    return true;
}

bool release_resource_ref(ResourceRef& ref, const ResourcePoolHooks& hooks) noexcept {
    bool ok = true;
    if (ref.owned && ref.ptr != nullptr) {
        ok = release_dependency(*ref.ptr, hooks);
        ref.owned = false;
    }
    if (ref.ptr != nullptr) {
        ok = remove_reference(*ref.ptr, hooks) && ok;
        ref.ptr = nullptr;
    }
    return ok;
}

ResourcePool::ResourcePool(SharedStrDeallocatorF590* key_deallocator) noexcept
    : key_deallocator_(key_deallocator) {}

ResourcePool::~ResourcePool() {
    for (auto& entry : map_) {
        // Raw field copy: the node's key reference is transferred to `key` and released once.
        HashedStr key = entry.first;
        (void)release_key(key, key_deallocator_);
    }
}

bool ResourcePool::get_resource(const HashedStr& key, ResourceRef& out,
                                const ResourcePoolHooks& hooks) noexcept {
    auto it = map_.lower_bound(key);
    if (it != map_.end() && !ResourceIdLess{}(key, it->first)) {
        add_reference(*it->second);
        out = ResourceRef{it->second, false};
        return true;
    }

    if (hooks.allocate == nullptr) return false;
    void* const storage = hooks.allocate(kResourceWrapperBytes);
    if (storage == nullptr) return false;

    auto* const wrapper = new (storage) ResourceWrapper(key, this);
    add_reference(*wrapper);  // 0x14e374: the returned reference

    // 0x150360 copies the key into the node (0x14c3a0, control +1). `node_key` is a raw struct copy
    // until emplace; the +1 is then owned by the map node and released by erase().
    HashedStr node_key{};
    copy_hashed_14c3a0(node_key, key);
    const auto inserted = map_.emplace(node_key, wrapper);
    if (!inserted.second) {
        HashedStr undo = node_key;
        (void)release_node_key(undo);
        (void)release_node_key(wrapper->key);
        wrapper->~ResourceWrapper();
        if (hooks.deallocate != nullptr) (void)hooks.deallocate(storage);
        return false;
    }
    out = ResourceRef{wrapper, false};
    return true;
}

bool ResourcePool::try_get_resource(const HashedStr& key, ResourceRef& out,
                                    const ResourcePoolHooks& hooks) noexcept {
    (void)hooks;
    auto it = map_.lower_bound(key);
    if (it != map_.end() && !ResourceIdLess{}(key, it->first)) {
        add_reference(*it->second);
        out = ResourceRef{it->second, false};
        return true;
    }
    // 0x14e440's miss returns the null wrapper with one added reference and never inserts.
    ResourceWrapper& null_wrapper = null_resource_wrapper();
    add_reference(null_wrapper);
    out = ResourceRef{&null_wrapper, false};
    return true;
}

bool ResourcePool::erase(ResourceWrapper* wrapper) noexcept {
    if (wrapper == nullptr) return false;
    // 0x14e9b0: erase only when the external count is already zero.
    if (wrapper->refs.load(std::memory_order_seq_cst) != 0) return false;

    auto it = map_.lower_bound(wrapper->key);
    if (it == map_.end() || ResourceIdLess{}(wrapper->key, it->first)) return false;
    if (it->second != wrapper) return false;

    HashedStr node_key = it->first;
    map_.erase(it);
    return release_node_key(node_key);
}

std::size_t ResourcePool::size() const noexcept { return map_.size(); }

const ResourcePool::Map& ResourcePool::entries() const noexcept { return map_; }

SharedStrDeallocatorF590* ResourcePool::key_deallocator() const noexcept { return key_deallocator_; }

bool ResourcePool::release_node_key(HashedStr& key) noexcept {
    return release_key(key, key_deallocator_);
}

}  // namespace sotc
