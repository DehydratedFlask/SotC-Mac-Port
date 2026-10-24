// Fixture for the native ResourceWrapper / ResourcePool / ResourceRef model.
// Covers the report's runtime-checker patterns (local/re/evidence/resource-pool/report.md §7.3) and the
// visible failure surfaces (§6.3). Keys are real 0xf1a0 SharedStr allocations with a test deallocator.
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <new>
#include <vector>

#include "core/fnv_span_161f80.hpp"
#include "core/resource_pool.hpp"
#include "core/shared_str_ctor_f1a0.hpp"
#include "core/shared_str_release_f590.hpp"

using namespace sotc;

namespace {

int g_checks = 0;

#define CHECK(c)                                                       \
    do {                                                               \
        ++g_checks;                                                    \
        if (!(c)) {                                                    \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); \
            std::exit(1);                                              \
        }                                                              \
    } while (0)

struct TestKeyDeallocator final : SharedStrDeallocatorF590 {
    int data_frees = 0;
    int control_frees = 0;

    void deallocate(const void* allocation, SharedStrDeallocationKindF590 kind) noexcept override {
        CHECK(allocation != nullptr);
        if (kind == SharedStrDeallocationKindF590::data) {
            ++data_frees;
        } else {
            ++control_frees;
        }
        std::free(const_cast<void*>(allocation));
    }
};

TestKeyDeallocator g_key_dealloc;

int g_alloc_calls = 0;
int g_free_calls = 0;
int g_manager_calls = 0;
int g_state_write_calls = 0;
int g_payload_creates = 0;
int g_payload_destroys = 0;
std::uint32_t g_last_state = 0;

void* test_allocate(std::size_t size) {
    void* const p = std::malloc(size);
    CHECK(p != nullptr);
    ++g_alloc_calls;
    return p;
}

bool test_deallocate(void* ptr) {
    ++g_free_calls;
    std::free(ptr);
    return true;
}

void* test_make_payload(const HashedStr&) {
    ++g_payload_creates;
    void* const p = std::malloc(8);
    CHECK(p != nullptr);
    return p;
}

bool test_destroy_payload(void* payload) {
    ++g_payload_destroys;
    std::free(payload);
    return true;
}

bool test_state_write(ResourceWrapper&, std::uint32_t state) {
    ++g_state_write_calls;
    g_last_state = state;
    return true;
}

ResourcePoolHooks g_hooks{};
const ResourcePoolHooks* g_manager_hooks = &g_hooks;

bool test_manager_delete(ResourceWrapper& wrapper) {
    ++g_manager_calls;
    return destroy_resource_wrapper(wrapper, *g_manager_hooks);
}

void* test_str_alloc(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) {
    void* const p = std::malloc(size);
    CHECK(p != nullptr);
    return p;
}

bool test_str_free(void* ptr, std::uint32_t) {
    std::free(ptr);
    return true;
}

SharedStrCtorHooks g_str_hooks{};

HashedStr make_key(const char* text, std::uint32_t hash) {
    HashedStr key{};
    CHECK(init_shared_str_f1a0(key.str, text, g_str_hooks));
    key.hash = hash;
    return key;
}

std::int64_t ctl_count(const HashedStr& key) {
    CHECK(key.str.ctl != nullptr);
    return std::atomic_ref<std::int64_t>(*key.str.ctl).load(std::memory_order_seq_cst);
}

void release_test_key(HashedStr& key) {
    if (key.str.ctl == nullptr) return;
    release_shared_str_f590(key.str, g_key_dealloc);
    key = HashedStr{};
}

void free_leaked_wrapper(ResourceWrapper* wrapper) {
    CHECK(wrapper != nullptr);
    if (wrapper->key.str.ctl != nullptr) {
        release_shared_str_f590(wrapper->key.str, g_key_dealloc);
    }
    if (wrapper->owner_id.str.ctl != nullptr) {
        release_shared_str_f590(wrapper->owner_id.str, g_key_dealloc);
    }
    wrapper->~ResourceWrapper();
    CHECK(test_deallocate(wrapper));
}

void test_layout() {
    CHECK(sizeof(ResourceWrapper) == 0x100);
    CHECK(alignof(ResourceWrapper) == 8);
    CHECK(offsetof(ResourceWrapper, header00) == 0x00);
    CHECK(offsetof(ResourceWrapper, list_next) == 0x08);
    CHECK(offsetof(ResourceWrapper, list_prev) == 0x10);
    CHECK(offsetof(ResourceWrapper, payload) == 0x20);
    CHECK(offsetof(ResourceWrapper, refs) == 0x30);
    CHECK(offsetof(ResourceWrapper, deps) == 0x40);
    CHECK(offsetof(ResourceWrapper, guard) == 0x50);
    CHECK(offsetof(ResourceWrapper, flags60) == 0x60);
    CHECK(offsetof(ResourceWrapper, pool) == 0x68);
    CHECK(offsetof(ResourceWrapper, key) == 0x70);
    CHECK(offsetof(ResourceWrapper, state) == 0x98);
    CHECK(offsetof(ResourceWrapper, children) == 0xa8);
    CHECK(offsetof(ResourceWrapper, owner_id) == 0xc8);
    CHECK(offsetof(ResourceWrapper, f0) == 0xf0);
    CHECK(offsetof(ResourceWrapper, f4) == 0xf4);
    CHECK(offsetof(ResourceWrapper, f8) == 0xf8);
    CHECK(sizeof(ResourceRef) == 0x10);
    CHECK(offsetof(ResourceRef, ptr) == 0x00);
    CHECK(offsetof(ResourceRef, owned) == 0x08);
    CHECK(kResourceWrapperBytes == 0x100);
    CHECK(kResourceWrapperInitialState == 11);
}

void test_comparator() {
    ResourcePool pool{&g_key_dealloc};
    HashedStr a = make_key("alpha", 0x77777777u);
    HashedStr a2 = make_key("alpha", 0x77777777u);
    HashedStr b = make_key("beta", 0x77777777u);
    HashedStr low = make_key("zeta", 0x77777776u);
    HashedStr high = make_key("alpha", 0x77777778u);

    const ResourceIdLess less{};
    CHECK(!less(a, a2));
    CHECK(!less(a2, a));
    CHECK(less(a, b));   // equal hash, strcmp alpha < beta
    CHECK(!less(b, a));
    CHECK(less(low, a));  // hash order dominates the text
    CHECK(less(a, high));
    CHECK(less(low, high));

    ResourceRef ra{};
    ResourceRef rc{};
    ResourceRef rd{};
    ResourceRef re{};
    CHECK(pool.get_resource(a, ra, g_hooks));
    CHECK(pool.get_resource(b, rc, g_hooks));
    CHECK(pool.get_resource(high, rd, g_hooks));
    CHECK(pool.get_resource(low, re, g_hooks));
    CHECK(pool.size() == 4);

    // Equal hash + equal text is a hit; equal hash + different text was a miss.
    ResourceRef hit{};
    CHECK(pool.get_resource(a2, hit, g_hooks));
    CHECK(hit.ptr == ra.ptr);
    CHECK(pool.size() == 4);
    CHECK(release_resource_ref(hit, g_hooks));

    // In-order iteration: low, a, b, high.
    std::vector<const ResourceWrapper*> order;
    std::uint32_t last_hash = 0;
    bool first = true;
    for (const auto& entry : pool.entries()) {
        if (!first) CHECK(last_hash <= entry.first.hash);
        last_hash = entry.first.hash;
        first = false;
        order.push_back(entry.second);
    }
    CHECK(order.size() == 4);
    CHECK(order[0] == re.ptr);
    CHECK(order[1] == ra.ptr);
    CHECK(order[2] == rc.ptr);
    CHECK(order[3] == rd.ptr);

    CHECK(release_resource_ref(ra, g_hooks));
    CHECK(release_resource_ref(rc, g_hooks));
    CHECK(release_resource_ref(rd, g_hooks));
    CHECK(release_resource_ref(re, g_hooks));
    CHECK(pool.size() == 0);

    release_test_key(a);
    release_test_key(a2);
    release_test_key(b);
    release_test_key(low);
    release_test_key(high);
}

void test_miss_hit_and_remove() {
    ResourcePool pool{&g_key_dealloc};
    HashedStr k1 = make_key("$/Models/_cmn/shadowmodels.crmp", 0x11111111u);
    CHECK(ctl_count(k1) == 1);
    const int alloc_before = g_alloc_calls;
    const int manager_start = g_manager_calls;

    // Miss: one wrapper, key copied into the wrapper and the map node, one returned reference.
    ResourceRef r1{};
    CHECK(pool.get_resource(k1, r1, g_hooks));
    CHECK(pool.size() == 1);
    CHECK(r1.ptr != nullptr);
    CHECK(!r1.owned);
    CHECK(r1.ptr->refs.load() == 1);
    CHECK(r1.ptr->deps.load() == 0);
    CHECK(r1.ptr->state == 11);
    CHECK(r1.ptr->f4 == 11);
    CHECK(r1.ptr->pool == &pool);
    CHECK(r1.ptr->payload == nullptr);
    CHECK(r1.ptr->children.empty());
    CHECK(r1.ptr->list_next == r1.ptr);
    CHECK(r1.ptr->list_prev == r1.ptr);
    CHECK(r1.ptr->header00 == nullptr);
    CHECK(r1.ptr->guard.load() == 0);
    CHECK(r1.ptr->flags60 == 0);
    CHECK(r1.ptr->f0 == 0);
    CHECK(r1.ptr->f8 == 0);
    CHECK(r1.ptr->key.hash == k1.hash);
    CHECK(r1.ptr->key.str.data == k1.str.data);
    CHECK(r1.ptr->key.str.ctl == k1.str.ctl);
    CHECK(ctl_count(k1) == 3);  // caller + wrapper key + map node key
    CHECK(g_alloc_calls == alloc_before + 1);
    CHECK(pool.entries().begin()->second == r1.ptr);
    CHECK(pool.entries().begin()->first.hash == k1.hash);

    // Hit: same pointer, one more +0x30, no new wrapper and no key copy.
    ResourceRef r2{};
    CHECK(pool.get_resource(k1, r2, g_hooks));
    CHECK(r2.ptr == r1.ptr);
    CHECK(!r2.owned);
    CHECK(r2.ptr->refs.load() == 2);
    CHECK(pool.size() == 1);
    CHECK(ctl_count(k1) == 3);
    CHECK(g_alloc_calls == alloc_before + 1);
    CHECK(release_resource_ref(r2, g_hooks));
    CHECK(r2.ptr == nullptr);
    CHECK(r1.ptr->refs.load() == 1);
    CHECK(g_manager_calls == manager_start);

    // From 2 only decrements.
    add_reference(*r1.ptr);
    CHECK(r1.ptr->refs.load() == 2);
    CHECK(remove_reference(*r1.ptr, g_hooks));
    CHECK(r1.ptr->refs.load() == 1);
    CHECK(pool.size() == 1);
    CHECK(g_manager_calls == manager_start);

    // From 1 erases the node and delegates to the manager hook.
    const int manager_before = g_manager_calls;
    const int free_before = g_free_calls;
    CHECK(remove_reference(*r1.ptr, g_hooks));
    CHECK(pool.size() == 0);
    CHECK(g_manager_calls == manager_before + 1);
    CHECK(g_free_calls == free_before + 1);
    CHECK(ctl_count(k1) == 1);  // caller only: node key and wrapper key were released
    r1.ptr = nullptr;
    release_test_key(k1);
}

void test_dependency_recursion() {
    ResourcePool pool{&g_key_dealloc};
    HashedStr ka = make_key("parent", 0x22222222u);
    HashedStr kb = make_key("child", 0x33333333u);
    const HashedStr zero{};

    ResourceRef ra{};
    ResourceRef rb{};
    CHECK(pool.get_resource(ka, ra, g_hooks));
    CHECK(pool.get_resource(kb, rb, g_hooks));
    CHECK(request_state(*ra.ptr, 13, zero, g_hooks));  // deps 1, early return
    CHECK(request_state(*rb.ptr, 13, zero, g_hooks));  // deps 1, early return
    CHECK(ra.ptr->deps.load() == 1);
    CHECK(rb.ptr->deps.load() == 1);

    add_reference(*rb.ptr);  // the children vector's reference
    CHECK(rb.ptr->refs.load() == 2);
    ra.ptr->children.push_back(ResourceRef{rb.ptr, true});

    const std::uint32_t ra_refs = ra.ptr->refs.load();
    const std::uint32_t rb_refs = rb.ptr->refs.load();
    const int manager_before = g_manager_calls;
    const int free_before = g_free_calls;

    CHECK(release_dependency(*ra.ptr, g_hooks));
    CHECK(ra.ptr->deps.load() == 0);
    CHECK(rb.ptr->deps.load() == 0);
    CHECK(ra.ptr->refs.load() == ra_refs);  // never frees the wrapper
    CHECK(rb.ptr->refs.load() == rb_refs);
    CHECK(g_manager_calls == manager_before);
    CHECK(g_free_calls == free_before);
    CHECK(pool.size() == 2);
    CHECK(ra.ptr->children.size() == 1);
    CHECK(!ra.ptr->children[0].owned);
    CHECK(ra.ptr->children[0].ptr == rb.ptr);

    CHECK(release_resource_ref(ra, g_hooks));  // A's inline destroy removes B's child reference
    CHECK(ra.ptr == nullptr);
    CHECK(pool.size() == 1);
    CHECK(rb.ptr->refs.load() == 1);
    CHECK(release_resource_ref(rb, g_hooks));
    CHECK(rb.ptr == nullptr);
    CHECK(pool.size() == 0);
    CHECK(g_manager_calls == manager_before + 2);

    release_test_key(ka);
    release_test_key(kb);
}

void test_request_state() {
    ResourcePool pool{&g_key_dealloc};
    HashedStr k = make_key("state", 0x44444444u);
    const HashedStr zero{};
    ResourceRef r{};
    CHECK(pool.get_resource(k, r, g_hooks));
    g_state_write_calls = 0;

    // Sites 1-8: state 11 <= priority 13 -> +0x40 only.
    CHECK(request_state(*r.ptr, 13, zero, g_hooks));
    CHECK(r.ptr->deps.load() == 1);
    CHECK(r.ptr->state == 11);
    CHECK(g_state_write_calls == 0);

    // Site 9: state 11 > priority 9 -> state = 9 and one notification.
    CHECK(request_state(*r.ptr, 9, zero, g_hooks));
    CHECK(r.ptr->deps.load() == 2);
    CHECK(r.ptr->state == 9);
    CHECK(g_state_write_calls == 1);
    CHECK(g_last_state == 9);

    // Idempotent early return at state 9, priority 9.
    CHECK(request_state(*r.ptr, 9, zero, g_hooks));
    CHECK(r.ptr->deps.load() == 3);
    CHECK(g_state_write_calls == 1);

    // Priority 10 with a higher state: no write, no notification (0x14d1af -> epilogue).
    r.ptr->state = 11;
    CHECK(request_state(*r.ptr, 10, zero, g_hooks));
    CHECK(r.ptr->state == 11);
    CHECK(r.ptr->deps.load() == 4);
    CHECK(g_state_write_calls == 1);

    // Priority 13 from state 14: state = 5 + notification.
    r.ptr->state = 14;
    CHECK(request_state(*r.ptr, 13, zero, g_hooks));
    CHECK(r.ptr->state == 5);
    CHECK(r.ptr->deps.load() == 5);
    CHECK(g_state_write_calls == 2);
    CHECK(g_last_state == 5);

    // Priority 12 from state 13: no write (the state == 11 row is dominated by the early return).
    r.ptr->state = 13;
    CHECK(request_state(*r.ptr, 12, zero, g_hooks));
    CHECK(r.ptr->state == 13);
    CHECK(r.ptr->deps.load() == 6);
    CHECK(g_state_write_calls == 2);

    // The null wrapper returns after +0x40 without a transition.
    const std::uint32_t null_deps = null_resource_wrapper().deps.load();
    CHECK(request_state(null_resource_wrapper(), 9, zero, g_hooks));
    CHECK(null_resource_wrapper().deps.load() == null_deps + 1);
    CHECK(null_resource_wrapper().state == 11);
    CHECK(g_state_write_calls == 2);

    // A missing state_write hook on a transition is a visible false.
    r.ptr->state = 11;
    ResourcePoolHooks no_state = g_hooks;
    no_state.state_write = nullptr;
    CHECK(!request_state(*r.ptr, 9, zero, no_state));
    CHECK(r.ptr->deps.load() == 7);
    CHECK(r.ptr->state == 11);

    // A live payload needs the blocked state machine (report §6.3).
    CHECK(resource_wrapper_set_payload(*r.ptr, k, g_hooks));
    CHECK(r.ptr->payload != nullptr);
    CHECK(!request_state(*r.ptr, 9, zero, g_hooks));
    CHECK(r.ptr->deps.load() == 8);
    CHECK(r.ptr->state == 11);
    CHECK(g_hooks.destroy_payload(r.ptr->payload));
    r.ptr->payload = nullptr;

    CHECK(release_resource_ref(r, g_hooks));
    CHECK(r.ptr == nullptr);
    CHECK(pool.size() == 0);
    release_test_key(k);
}

void test_try_get_and_slots() {
    ResourcePool pool{&g_key_dealloc};
    HashedStr k = make_key("try", 0x55555555u);
    const HashedStr zero{};
    const std::size_t before = pool.size();

    // TryGetResource miss: the null wrapper with one added reference, no insertion.
    const std::uint32_t null_refs = null_resource_wrapper().refs.load();
    ResourceRef miss{};
    CHECK(pool.try_get_resource(k, miss, g_hooks));
    CHECK(miss.ptr == &null_resource_wrapper());
    CHECK(!miss.owned);
    CHECK(pool.size() == before);
    CHECK(null_resource_wrapper().refs.load() == null_refs + 1);
    CHECK(release_resource_ref(miss, g_hooks));
    CHECK(miss.ptr == nullptr);
    CHECK(null_resource_wrapper().refs.load() == null_refs);
    CHECK(pool.size() == before);

    // TryGetResource hit.
    ResourceRef slot{};
    CHECK(pool.get_resource(k, slot, g_hooks));
    CHECK(slot.ptr->refs.load() == 1);
    ResourceRef hit{};
    CHECK(pool.try_get_resource(k, hit, g_hooks));
    CHECK(hit.ptr == slot.ptr);
    CHECK(!hit.owned);
    CHECK(slot.ptr->refs.load() == 2);
    CHECK(release_resource_ref(hit, g_hooks));
    CHECK(slot.ptr->refs.load() == 1);

    // Sites 1-8: Lock(13) is idempotent and takes one +0x40 without a state write.
    g_state_write_calls = 0;
    CHECK(lock_resource_ref(slot, 13, zero, g_hooks));
    CHECK(slot.owned);
    CHECK(slot.ptr->deps.load() == 1);
    CHECK(slot.ptr->state == 11);
    CHECK(g_state_write_calls == 0);
    CHECK(lock_resource_ref(slot, 13, zero, g_hooks));
    CHECK(slot.ptr->deps.load() == 1);

    const int manager_before = g_manager_calls;
    CHECK(release_resource_ref(slot, g_hooks));
    CHECK(slot.ptr == nullptr);
    CHECK(pool.size() == before);
    CHECK(g_manager_calls == manager_before + 1);

    // Site 9: Lock(9) writes state 9 and notifies once.
    ResourceRef slot9{};
    CHECK(pool.get_resource(k, slot9, g_hooks));
    CHECK(lock_resource_ref(slot9, 9, zero, g_hooks));
    CHECK(slot9.owned);
    CHECK(slot9.ptr->deps.load() == 1);
    CHECK(slot9.ptr->state == 9);
    CHECK(g_state_write_calls == 1);
    CHECK(g_last_state == 9);
    CHECK(release_resource_ref(slot9, g_hooks));
    CHECK(slot9.ptr == nullptr);
    CHECK(pool.size() == before);

    release_test_key(k);
}

void test_failures() {
    ResourcePool pool{&g_key_dealloc};
    HashedStr k = make_key("fail", 0x66666666u);
    const HashedStr zero{};
    const std::size_t before = pool.size();
    ResourceRef r{};

    // Missing / failing allocator.
    ResourcePoolHooks no_alloc = g_hooks;
    no_alloc.allocate = nullptr;
    CHECK(!pool.get_resource(k, r, no_alloc));
    CHECK(r.ptr == nullptr);
    CHECK(pool.size() == before);

    ResourcePoolHooks null_alloc = g_hooks;
    null_alloc.allocate = [](std::size_t) -> void* { return nullptr; };
    CHECK(!pool.get_resource(k, r, null_alloc));
    CHECK(r.ptr == nullptr);
    CHECK(pool.size() == before);

    // Missing payload factory.
    CHECK(pool.get_resource(k, r, g_hooks));
    ResourcePoolHooks no_factory = g_hooks;
    no_factory.make_payload = nullptr;
    CHECK(!resource_wrapper_set_payload(*r.ptr, k, no_factory));
    CHECK(r.ptr->payload == nullptr);

    // Missing manager + missing deallocate: false, the node is erased, the wrapper stays alive.
    ResourcePoolHooks no_destroy = g_hooks;
    no_destroy.manager_delete = nullptr;
    no_destroy.deallocate = nullptr;
    const int manager_before = g_manager_calls;
    CHECK(!remove_reference(*r.ptr, no_destroy));
    CHECK(pool.size() == before);
    CHECK(g_manager_calls == manager_before);
    CHECK(r.ptr->refs.load() == 0);
    free_leaked_wrapper(r.ptr);
    r.ptr = nullptr;

    // Manager present but the wrapper free contract missing: the manager hook reports false.
    CHECK(pool.get_resource(k, r, g_hooks));
    ResourcePoolHooks no_free = g_hooks;
    no_free.deallocate = nullptr;
    const int manager_second = g_manager_calls;
    const ResourcePoolHooks* const saved_manager_hooks = g_manager_hooks;
    g_manager_hooks = &no_free;
    CHECK(!remove_reference(*r.ptr, no_free));
    g_manager_hooks = saved_manager_hooks;
    CHECK(g_manager_calls == manager_second + 1);
    CHECK(pool.size() == before);
    free_leaked_wrapper(r.ptr);
    r.ptr = nullptr;

    // Missing destroy_payload at dependency zero: false, the wrapper is not freed.
    CHECK(pool.get_resource(k, r, g_hooks));
    CHECK(request_state(*r.ptr, 13, zero, g_hooks));  // deps 1 while the payload is still null
    CHECK(resource_wrapper_set_payload(*r.ptr, k, g_hooks));
    CHECK(r.ptr->payload != nullptr);
    const std::uint32_t refs_before = r.ptr->refs.load();
    ResourcePoolHooks no_payload_destroy = g_hooks;
    no_payload_destroy.destroy_payload = nullptr;
    CHECK(!release_dependency(*r.ptr, no_payload_destroy));
    CHECK(r.ptr->deps.load() == 0);
    CHECK(r.ptr->refs.load() == refs_before);
    CHECK(pool.size() == 1);
    CHECK(r.ptr->payload != nullptr);
    CHECK(g_hooks.destroy_payload(r.ptr->payload));
    r.ptr->payload = nullptr;
    CHECK(release_resource_ref(r, g_hooks));
    CHECK(pool.size() == before);
    CHECK(r.ptr == nullptr);

    release_test_key(k);
}

}  // namespace

int main() {
    g_str_hooks.alloc.allocate = test_str_alloc;
    g_str_hooks.alloc.deallocate = test_str_free;

    g_hooks.allocate = test_allocate;
    g_hooks.deallocate = test_deallocate;
    g_hooks.make_payload = test_make_payload;
    g_hooks.destroy_payload = test_destroy_payload;
    g_hooks.manager_delete = test_manager_delete;
    g_hooks.state_write = test_state_write;

    test_layout();
    test_comparator();
    test_miss_hit_and_remove();
    test_dependency_recursion();
    test_request_state();
    test_try_get_and_slots();
    test_failures();

    CHECK(g_alloc_calls == g_free_calls);
    CHECK(g_key_dealloc.data_frees == g_key_dealloc.control_frees);
    CHECK(g_key_dealloc.data_frees > 0);
    CHECK(g_payload_creates == g_payload_destroys);

    std::printf(
        "test_resource_pool: %d checks passed (%d wrappers allocated/freed, %d key blocks freed, "
        "%d manager deletes, %d state writes, %d payloads)\n",
        g_checks, g_alloc_calls, g_key_dealloc.data_frees, g_manager_calls, g_state_write_calls,
        g_payload_creates);
    return 0;
}
