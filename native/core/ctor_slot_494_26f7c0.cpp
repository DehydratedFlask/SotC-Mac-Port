#include "ctor_slot_494_26f7c0.hpp"

#include <cstdlib>
#include <cstring>

namespace sotc {
namespace {

void* heap_allocate(std::size_t size) { return std::malloc(size); }
void heap_release(void* pointer) { std::free(pointer); }

// Largest element counts accepted by the original grow helpers (the movabs immediates): 0x1fffffffffffffff 8-byte
// pointers, 0x0aaaaaaaaaaaaaaa 24-byte nodes, 0x07ffffffffffffff 32-byte nodes.
constexpr std::size_t kMaxPointers = 0x1fffffffffffffffull;
constexpr std::size_t kMaxNodes24 = 0x0aaaaaaaaaaaaaaaull;
constexpr std::size_t kMaxNodes32 = 0x07ffffffffffffffull;

// 0x26e4d0 / 0x26e8f0 (pointer vector) and 0x26e5f0 / 0x26ea10 (node vector): "make room for `count` more elements".
// A free-capacity check first; then new capacity = max(size + count, cap + cap / 2), an allocation of that many
// elements, a memmove of the live elements, a release of the old block (when non-null) and the three pointers rewritten.
}  // namespace

bool slot494_reserve_more(std::byte*& begin, std::byte*& end, std::byte*& cap, std::size_t element_size,
                          std::size_t max_elements, std::size_t count, const Slot494PoolHooks& hooks) noexcept {
    const std::size_t spare = static_cast<std::size_t>(cap - end) / element_size;
    if (spare >= count) return true;
    const std::size_t size = static_cast<std::size_t>(end - begin) / element_size;
    if (max_elements - size < count) return false;                       // 0x26e5d0 / 0x26e750: length_error throw
    const std::size_t capacity = static_cast<std::size_t>(cap - begin) / element_size;
    const std::size_t want = size + count;
    std::size_t grown = capacity + (capacity >> 1);
    if (max_elements - (capacity >> 1) < capacity) grown = 0;
    if (grown < want) grown = want;
    std::byte* storage = nullptr;
    if (grown != 0) {
        if (grown > max_elements) return false;                          // 0xf8f5f0: bad_alloc throw
        if (hooks.allocate == nullptr) return false;
        storage = static_cast<std::byte*>(hooks.allocate(grown * element_size));
        if (storage == nullptr) return false;
    }
    const std::size_t live = static_cast<std::size_t>(end - begin);
    if (live != 0) std::memmove(storage, begin, live);                   // 0xf8f480 (memmove import)
    if (begin != nullptr) {
        if (hooks.release == nullptr) return false;
        hooks.release(begin);
    }
    cap = storage + grown * element_size;
    end = storage + size * element_size;
    begin = storage;
    return true;
}

namespace {

template <class Node>
void init_value(Node* node) noexcept;

template <>
void init_value<Slot494Node0>(Slot494Node0* node) noexcept {
    node->next = nullptr;                                                // movq $0, (%rsi)
    node->state = kSlot494NodeStateInit;                                 // movl $0xffffffff, 8(%rsi)
    node->aux = 0;                                                       // movq $0, 0x10(%rsi)
}

template <>
void init_value<Slot494Node1>(Slot494Node1* node) noexcept {
    node->next = nullptr;
    node->state = kSlot494NodeStateInit;
    node->aux = 0;
    node->state2 = kSlot494NodeStateInit;                                // movl $0xffffffff, 0x18(%rsi)
}

template <class Node>
bool build_pool(Slot494Registry& registry, std::size_t max_nodes, const Slot494PoolHooks& hooks) noexcept {
    registry.begin = nullptr;                                            // vmovups xmm0, 0x8(%r14); movq $0, 0x18(%r14)
    registry.end = nullptr;
    registry.cap = nullptr;
    if (hooks.allocate == nullptr) return false;
    auto* pool = static_cast<Slot494PoolBlock*>(hooks.allocate(kSlot494PoolBlockSize));   // 0x17070(0x28)
    if (pool == nullptr) return false;
    pool->nodes_begin = nullptr;
    pool->nodes_end = nullptr;
    pool->nodes_cap = nullptr;
    if (!slot494_reserve_more(pool->nodes_begin, pool->nodes_end, pool->nodes_cap, sizeof(Node), max_nodes, kSlot494NodeCount, hooks))
        return false;
    const std::size_t have = static_cast<std::size_t>(pool->nodes_end - pool->nodes_begin) / sizeof(Node);
    const std::size_t missing = kSlot494NodeCount - have;
    auto* tail = reinterpret_cast<Node*>(pool->nodes_end);
    for (std::size_t i = 0; i < missing; ++i) init_value(tail + i);
    pool->nodes_end += missing * sizeof(Node);
    pool->free_head = nullptr;                                           // movq $0, 0x20(%rbx)
    auto* first = reinterpret_cast<Node*>(pool->nodes_begin);
    Node* chain = nullptr;
    for (std::size_t i = kSlot494NodeCount; i-- > 0;) {                  // last node first: 0x17fe8 / 0x1ffe0 = 0xfff * stride
        first[i].state = 0;                                              // movl $0, 8(%rdx)
        first[i].next = chain;                                           // movq %rsi, (%rdx)
        chain = first + i;
    }
    pool->free_head = pool->nodes_begin;                                 // movq %rax, 0x20(%rbx)
    // push_back(pool): grow the pointer vector by one when full, then store and advance.
    if (registry.end == registry.cap) {
        std::byte* vb = reinterpret_cast<std::byte*>(registry.begin);
        std::byte* ve = reinterpret_cast<std::byte*>(registry.end);
        std::byte* vc = reinterpret_cast<std::byte*>(registry.cap);
        const bool grown = slot494_reserve_more(vb, ve, vc, sizeof(void*), kMaxPointers, 1, hooks);
        registry.begin = reinterpret_cast<Slot494PoolBlock**>(vb);
        registry.end = reinterpret_cast<Slot494PoolBlock**>(ve);
        registry.cap = reinterpret_cast<Slot494PoolBlock**>(vc);
        if (!grown) return false;
    }
    *registry.end = pool;
    ++registry.end;
    return true;
}

}  // namespace

Slot494PoolHooks slot494_default_pool_hooks() noexcept {
    Slot494PoolHooks hooks{};
    hooks.allocate = &heap_allocate;
    hooks.release = &heap_release;
    return hooks;
}

bool slot494_build_registry(Slot494Registry& registry, int node_kind, const Slot494PoolHooks& hooks) noexcept {
    return node_kind == 0 ? build_pool<Slot494Node0>(registry, kMaxNodes24, hooks)
                          : build_pool<Slot494Node1>(registry, kMaxNodes32, hooks);
}

bool slot494_destroy_registry(Slot494Registry& registry, const Slot494PoolHooks& hooks) noexcept {
    Slot494PoolBlock** at = registry.begin;
    while (at != registry.end) {
        Slot494PoolBlock* pool = *at;
        if (pool != nullptr) {
            if (pool->nodes_begin != nullptr) {
                if (hooks.release == nullptr) return false;
                hooks.release(pool->nodes_begin);
            }
            if (hooks.release == nullptr) return false;
            hooks.release(pool);
        }
        ++at;
    }
    registry.end = registry.begin;                                       // movq %rbx, 0x10(%r14)
    if (registry.begin != nullptr) {
        if (hooks.release == nullptr) return false;
        hooks.release(registry.begin);
        registry.begin = nullptr;                                        // vmovups xmm0 -> 0x8(%r14); movq $0, 0x18(%r14)
        registry.end = nullptr;
        registry.cap = nullptr;
    }
    return true;
}

Slot494Result init_slot494_pools_26f7c0(Slot494Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot494Result out{};
    out.built0 = slot494_build_registry(objects.r0, 0, objects.hooks);
    if (!out.built0) return out;
    out.atexit0 = concmd.atexit_log.record(kSlot494PoolsDtor0Rva, &objects.r0);
    if (!out.atexit0) return out;
    out.built1 = slot494_build_registry(objects.r1, 1, objects.hooks);
    if (!out.built1) return out;
    out.atexit1 = concmd.atexit_log.record(kSlot494PoolsDtor1Rva, &objects.r1);
    out.ok = out.atexit1;
    return out;
}

}  // namespace sotc
