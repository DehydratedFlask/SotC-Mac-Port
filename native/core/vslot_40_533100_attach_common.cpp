#include "vslot_40_533100_attach_common.hpp"

#include "resource_id_14c2f0.hpp"

#include <atomic>
#include <cstring>

namespace sotc {
namespace {

// 0x53356e..0x53357d: zero the 0x20 B holder's pointer field, then 0xf590.
[[nodiscard]] bool release_site_key(HashedStr& key, const SharedReleaseF590Hooks& release,
                                    Vslot40AttachCommonResult& out) noexcept {
    const SharedHolderF590 holder{const_cast<std::uint8_t*>(key.str.data), nullptr, nullptr,
                                  reinterpret_cast<std::atomic<std::uint64_t>*>(key.str.ctl)};
    out.key_release = shared_release_f590(holder, release);
    ++out.dtor_f590_count;
    return out.key_release.ok && out.key_release.had_control &&
           (out.key_release.previous == 1 || out.key_release.previous == 2 ||
            out.key_release.previous == 3) &&
           out.key_release.was_last == (out.key_release.previous == 1);
}

}  // namespace

void vslot_40_533100_init_slots(void* f08, void* self) noexcept {
    static constexpr std::uint64_t kF08Offsets[] = {0x00, 0x10, 0x20, 0x40, 0x50, 0x60, 0x70};
    static constexpr std::uint64_t kSelfOffsets[] = {0x3CB8, 0x3CC8};
    auto init_one = [](void* base, std::uint64_t off) noexcept {
        if (base == nullptr) return;
        auto* const slot =
            reinterpret_cast<LockedResourceRef*>(static_cast<std::uint8_t*>(base) + off);
        if (slot->ref.ptr == nullptr) slot->ref.ptr = &null_resource_wrapper();
        slot->ref.owned = false;
    };
    for (const std::uint64_t off : kF08Offsets) init_one(f08, off);
    for (const std::uint64_t off : kSelfOffsets) init_one(self, off);
}

Vslot40AttachCommonResult vslot_40_533100_attach_common(
    const char* literal, std::uint64_t slot_off,
    const Vslot40AttachCommonHooks& hooks) noexcept {
    Vslot40AttachCommonResult out{};

    if (literal == nullptr) {
        out.error = "missing literal";
        return out;
    }
    if (hooks.slot_base == nullptr) {
        out.error = "missing slot base";
        return out;
    }
    if (hooks.pool == nullptr) {
        out.error = "missing ResourcePool";
        return out;
    }
    auto* const slot = reinterpret_cast<LockedResourceRef*>(
        static_cast<std::uint8_t*>(hooks.slot_base) + slot_off);

    // 0x533461: the real ResourceId key (`****` rewritten to the platform tag before FNV).
    HashedStr key{};
    ResourceIdCtorHooks key_hooks{};
    key_hooks.str = hooks.key_alloc;
    key_hooks.platform_tag = hooks.platform_tag;
    if (!resource_id_ctor_14c2f0(key, literal, key_hooks)) {
        out.error = "key";
        return out;
    }
    out.key_built = true;
    out.key_size = key.str.size;
    out.key_capacity = key.str.aux;
    out.key_hash = key.hash;

    // 0x533470: ResourcePool::GetResource. A hit adds one +0x30; a miss allocates the wrapper with
    // one reference and inserts the non-owning map node.
    ResourceRef ref{};
    if (!hooks.pool->get_resource(key, ref, hooks.pool_hooks) || ref.ptr == nullptr) {
        out.error = "get_resource";
        (void)release_site_key(key, hooks.release, out);
        return out;
    }
    out.get_ok = true;
    out.wrapper = ref.ptr;

    // 0x53347a: AddReference(W). The `if (ref.owned)` RequestState(13) block at 0x53347f is dead
    // because GetResource always returns owned == false in this build.
    add_reference(*ref.ptr);
    ++out.add_reference_count;

    // 0x5334f9: join. `prev_slot` is read through the same slot the assign writes.
    out.prev_slot = slot->ref.ptr;
    if (!slot->assign(*ref.ptr, hooks.pool_hooks)) {
        out.error = "slot assign";
        (void)remove_reference(*ref.ptr, hooks.pool_hooks);
        ++out.remove_reference_count;
        (void)release_site_key(key, hooks.release, out);
        return out;
    }
    out.slot_assigned = true;

    // 0x53354a and 0x533565: the two RemoveReference calls. The `if (used)` and `if (ref.owned)`
    // blocks between them are dead for the same reason as above.
    if (!remove_reference(*ref.ptr, hooks.pool_hooks)) {
        out.error = "remove 1";
    }
    ++out.remove_reference_count;
    if (out.error[0] == '\0' && !remove_reference(*ref.ptr, hooks.pool_hooks)) {
        out.error = "remove 2";
    }
    ++out.remove_reference_count;

    // 0x53356e..0x53357d: release the caller's key. On a miss the map node holds the other
    // reference (control 2); on a hit the caller's key is the last one (control 1).
    const bool release_ok = release_site_key(key, hooks.release, out);

    // 0x53358c (sites 1-8) or the 0x53403c tail (site 9): Lock(slot, priority).
    if (hooks.lock_after_cleanup) {
        out.lock_ok = slot->lock(hooks.lock_priority, HashedStr{}, hooks.pool_hooks);
    } else {
        out.lock_ok = true;  // the site's 0x140d90 tail owns this step
    }

    out.refs_after = slot->ref.ptr != nullptr ? slot->ref.ptr->refs.load() : 0;
    out.deps_after = slot->ref.ptr != nullptr ? slot->ref.ptr->deps.load() : 0;
    out.state_after = slot->ref.ptr != nullptr ? slot->ref.ptr->state : 0;

    out.ok = out.error[0] == '\0' && release_ok && out.lock_ok && out.slot_assigned &&
             out.add_reference_count == 1 && out.remove_reference_count == 2 &&
             (hooks.lock_after_cleanup ? slot->ref.owned : !slot->ref.owned);
    return out;
}

}  // namespace sotc
