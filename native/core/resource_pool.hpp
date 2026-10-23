// Native model of the 0x100 B `ResourceWrapper`, the non-owning `ResourcePool` map and `ResourceRef`
// (private audit: local/re/evidence/resource-pool/report.md §3-§7; ELF sha256 d0723afb...861d3).
//
// Modelled from the machine code (offline analysis only; no guest code is executed):
//   ResourceWrapper has no vtable: ctor 0x14c8a0 stores 0 at +0x00 and never stores a vtable address.
//   +0x30 refs   : 0x14ccb0 `lock inc dword [rdi+0x30]`.
//   +0x40 deps   : 0x14cf20 `lock xadd +1` before every check; 0x14d2d0 `lock xadd -1`.
//   +0x70 key    : copy_hashed_14c3a0 (control block +1).
//   +0x98 state  : 11 after construction; +0xf4 is also 11.
//   +0xa8 children: vector<ResourceRef> (0x10 B stride).
//   pool map     : std::map<HashedStr, ResourceWrapper*, ResourceIdLess>; the value is a raw,
//                  non-owning pointer. In-order iteration is observable (0x14eb10).
//   comparator   : 0x14c850 unsigned u32 hash, then strcmp (not memcmp) of the key data.
//
// `owned` means "this ref holds exactly one +0x40 dependency reference" (set by 0x95580/0x140d90 after
// RequestState, cleared by 0x14d2d0). GetResource/TryGetResource always return owned == false.
//
// Hooks: allocation (0x17070/0x170b0), payload lifecycle (the payload is opaque; GetResource leaves it
// null), manager deletion (*0x2ee2d00 slots +0x28/+0x30) and the engine state notification (pool
// vtable slot 13 -> manager vtable[3]). A missing or failed hook is a visible false, never a fake
// success.
//
// NOT modelled: the pool's recursive mutex (+0x70), the second dependency tree (+0x58/+0x60), the
// ResourceManager/loader bodies, the guest allocator bodies, and the map node's 0x17090/0x170c0
// allocation (std::map uses the host allocator). The model is single-threaded; the atomics keep the
// two counters' exact operations.
#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <map>
#include <vector>

#include "hashed_string_14c3a0.hpp"
#include "shared_str_release_f590.hpp"

namespace sotc {

struct ResourceWrapper;
class ResourcePool;

inline constexpr std::uint64_t kResourceWrapperCtor14c8a0Rva = 0x14C8A0;
inline constexpr std::size_t kResourceWrapperCtor14c8a0Size = 243;
inline constexpr std::uint64_t kResourceWrapperAddRef14ccb0Rva = 0x14CCB0;
inline constexpr std::size_t kResourceWrapperAddRef14ccb0Size = 5;
inline constexpr std::uint64_t kResourceWrapperRemoveRef14ccc0Rva = 0x14CCC0;
inline constexpr std::size_t kResourceWrapperRemoveRef14ccc0Size = 563;
inline constexpr std::uint64_t kResourceWrapperRequestState14cf20Rva = 0x14CF20;
inline constexpr std::size_t kResourceWrapperRequestState14cf20Size = 748;
inline constexpr std::uint64_t kResourceWrapperReleaseDep14d2d0Rva = 0x14D2D0;
inline constexpr std::size_t kResourceWrapperReleaseDep14d2d0Size = 289;
inline constexpr std::uint64_t kResourceIdLess14c850Rva = 0x14C850;
inline constexpr std::size_t kResourceIdLess14c850Size = 32;
inline constexpr std::uint64_t kResourcePoolGetResource14e260Rva = 0x14E260;
inline constexpr std::size_t kResourcePoolGetResource14e260Size = 466;
inline constexpr std::uint64_t kResourcePoolTryGetResource14e440Rva = 0x14E440;
inline constexpr std::size_t kResourcePoolTryGetResource14e440Size = 284;
inline constexpr std::uint64_t kResourcePoolMarkDead14e9b0Rva = 0x14E9B0;
inline constexpr std::size_t kResourcePoolMarkDead14e9b0Size = 335;
inline constexpr std::uint64_t kNullResourceWrapper14d880Rva = 0x14D880;
inline constexpr std::size_t kNullResourceWrapper14d880Size = 121;

inline constexpr std::size_t kResourceWrapperBytes = 0x100;
inline constexpr std::uint32_t kResourceWrapperInitialState = 11;
inline constexpr std::uint32_t kResourceWrapperLockPriority13 = 13;
inline constexpr std::uint32_t kResourceWrapperLockPriority9 = 9;

// 0x10 B: {ResourceWrapper* ptr @0x00, bool owned @0x08}. The original children-vector stride is 0x10.
struct ResourceRef {
    ResourceWrapper* ptr = nullptr;  // +0x00
    bool owned = false;              // +0x08; holds one +0x40 dependency reference
};
static_assert(sizeof(ResourceRef) == 0x10);
static_assert(offsetof(ResourceRef, ptr) == 0x00);
static_assert(offsetof(ResourceRef, owned) == 0x08);

// 0x14c850: ascending unsigned u32 hash, then strcmp over the string data. Two different buffers with
// equal text compare equal; an embedded NUL terminates, exactly like the original strcmp.
struct ResourceIdLess {
    [[nodiscard]] bool operator()(const HashedStr& a, const HashedStr& b) const noexcept;
};

// 0x100 B, non-polymorphic. Holes are native zeros: the original leaves some bytes unwritten, which is
// not observable; the modelled fields are byte-exact.
struct ResourceWrapper {
    void* header00 = nullptr;                 // +0x00 ctor stores 0; no vtable is ever stored
    ResourceWrapper* list_next = nullptr;     // +0x08 intrusive list head, self after construction
    ResourceWrapper* list_prev = nullptr;     // +0x10
    std::uint64_t hole18 = 0;                 // +0x18 (the 16-byte vmovups at +0x18 covers +0x20 too)
    void* payload = nullptr;                  // +0x20 opaque owned payload; null on GetResource
    std::uint64_t hole28 = 0;                 // +0x28
    std::atomic<std::uint32_t> refs{0};       // +0x30 external refcount (AddReference)
    std::uint64_t hole38 = 0;                 // +0x38
    std::atomic<std::uint32_t> deps{0};       // +0x40 dependency/lock count
    std::uint32_t hole44 = 0;                 // +0x44
    std::uint32_t hole48 = 0;                 // +0x48
    std::uint32_t hole4c = 0;                 // +0x4c
    std::atomic<std::int64_t> guard{0};       // +0x50 teardown guard, CAS 0 -> -1
    std::uint64_t hole58 = 0;                 // +0x58
    std::uint32_t flags60 = 0;                // +0x60 blocks RequestState when non-zero
    std::uint32_t hole64 = 0;                 // +0x64
    ResourcePool* pool = nullptr;             // +0x68 pool back-pointer
    HashedStr key{};                          // +0x70 key copy (hash @0x70, SharedStr @0x78)
    std::uint32_t state = 0;                  // +0x98 11 after construction
    std::uint32_t hole9c = 0;                 // +0x9c
    std::uint64_t holea0 = 0;                 // +0xa0
    std::vector<ResourceRef> children{};      // +0xa8
    std::uint64_t holec0 = 0;                 // +0xc0 (zeroed by the 32-byte vmovups at +0xa8)
    HashedStr owner_id{};                     // +0xc8 requester/owner id copy
    std::uint32_t f0 = 0;                     // +0xf0
    std::uint32_t f4 = 0;                     // +0xf4 11 after construction
    std::uint8_t f8 = 0;                      // +0xf8
    std::uint8_t holef9[7] = {};              // +0xf9

    // 0x14c8a0: zero header, self list head, pool back-pointer, key copy (control +1), state 11.
    // The owner-id member stays the zero .bss id the original copies from 0x2ed3640.
    ResourceWrapper(const HashedStr& key, ResourcePool* pool) noexcept;
    ~ResourceWrapper() = default;

    ResourceWrapper(const ResourceWrapper&) = delete;
    ResourceWrapper& operator=(const ResourceWrapper&) = delete;
    ResourceWrapper(ResourceWrapper&&) = delete;
    ResourceWrapper& operator=(ResourceWrapper&&) = delete;
};
static_assert(sizeof(ResourceWrapper) == 0x100);
static_assert(alignof(ResourceWrapper) == 8);
static_assert(offsetof(ResourceWrapper, header00) == 0x00);
static_assert(offsetof(ResourceWrapper, list_next) == 0x08);
static_assert(offsetof(ResourceWrapper, list_prev) == 0x10);
static_assert(offsetof(ResourceWrapper, payload) == 0x20);
static_assert(offsetof(ResourceWrapper, refs) == 0x30);
static_assert(offsetof(ResourceWrapper, deps) == 0x40);
static_assert(offsetof(ResourceWrapper, guard) == 0x50);
static_assert(offsetof(ResourceWrapper, flags60) == 0x60);
static_assert(offsetof(ResourceWrapper, pool) == 0x68);
static_assert(offsetof(ResourceWrapper, key) == 0x70);
static_assert(offsetof(ResourceWrapper, state) == 0x98);
static_assert(offsetof(ResourceWrapper, children) == 0xa8);
static_assert(offsetof(ResourceWrapper, owner_id) == 0xc8);
static_assert(offsetof(ResourceWrapper, f0) == 0xf0);
static_assert(offsetof(ResourceWrapper, f4) == 0xf4);
static_assert(offsetof(ResourceWrapper, f8) == 0xf8);

// Host seams. A null pointer where the original would call a real function is a visible false.
struct ResourcePoolHooks {
    // 0x17070(0x100) / 0x170b0(ptr): the wrapper's guest allocation seam. The returned storage must
    // be suitably aligned for ResourceWrapper (alignof 8).
    void* (*allocate)(std::size_t size) = nullptr;
    bool (*deallocate)(void* ptr) = nullptr;
    // Payload at +0x20. The original GetResource leaves it null; this factory is the explicit host
    // seam for the blocked loader. A live payload makes RequestState's proven subset fail (report
    // §6.3), and destroy_payload is required whenever a payload must be deleted.
    void* (*make_payload)(const HashedStr& key) = nullptr;
    bool (*destroy_payload)(void* payload) = nullptr;
    // *0x2ee2d00 slots +0x28 (DeleteResource) / +0x30 (DeleteResourceWrapper). The hook owns the
    // wrapper's destruction; remove_reference does not touch the wrapper after a successful call.
    bool (*manager_delete)(ResourceWrapper& wrapper) = nullptr;
    // Engine notification after the proven state transition (pool vtable slot 13 -> manager vtable[3]).
    bool (*state_write)(ResourceWrapper& wrapper, std::uint32_t state) = nullptr;
};

// 0x14d880's singleton. refs starts at 1 (the getter's permanent reference); TryGetResource adds one.
[[nodiscard]] ResourceWrapper& null_resource_wrapper() noexcept;

// 0x14ccb0: `lock inc dword [rdi+0x30]`.
void add_reference(ResourceWrapper& wrapper) noexcept;

// 0x14ccc0: decrement +0x30. At zero erase the pool map node (slot 9) and either delegate to
// manager_delete or run the inline destroy. Never touches the map when wrapper.pool is null.
[[nodiscard]] bool remove_reference(ResourceWrapper& wrapper,
                                    const ResourcePoolHooks& hooks) noexcept;

// 0x14d2d0: decrement +0x40. At zero delete a live payload and recurse the children's owned refs; the
// wrapper is never freed and +0x30 is never touched.
[[nodiscard]] bool release_dependency(ResourceWrapper& wrapper,
                                      const ResourcePoolHooks& hooks) noexcept;

// 0x14cf20: `lock xadd +1` on +0x40 unconditionally, then the proven `state <= priority` early return
// and the state-write table (0x14d193..0x14d209). The owner-id copy and child re-request are dead in
// this build (r12d is always 1 at 0x14d0e5), so `requester` is not read.
[[nodiscard]] bool request_state(ResourceWrapper& wrapper, std::uint32_t priority,
                                 const HashedStr& requester,
                                 const ResourcePoolHooks& hooks) noexcept;

// Inline destroy of 0x14ccc0 (also usable from a manager_delete hook): payload, owner key, children,
// own key, destructor, deallocate. Acquires the +0x50 guard; a missing deallocate/destroy_payload or a
// failed child release reports false and leaves the wrapper alive.
[[nodiscard]] bool destroy_resource_wrapper(ResourceWrapper& wrapper,
                                            const ResourcePoolHooks& hooks) noexcept;

// Host seam for the payload factory. Fails when a payload already exists, the factory is missing, or
// the factory returns null.
[[nodiscard]] bool resource_wrapper_set_payload(ResourceWrapper& wrapper, const HashedStr& key,
                                                const ResourcePoolHooks& hooks) noexcept;

// 0x95580 (priority 13) / 0x140d90 (slot, priority): `if (!ref.owned) { RequestState(priority); owned=1 }`.
// Idempotent; a failed RequestState leaves owned false.
[[nodiscard]] bool lock_resource_ref(ResourceRef& ref, std::uint32_t priority,
                                     const HashedStr& requester,
                                     const ResourcePoolHooks& hooks) noexcept;

// ResourceRef destructor rule: `if (owned) ReleaseDependency; owned=0; RemoveReference`.
[[nodiscard]] bool release_resource_ref(ResourceRef& ref,
                                        const ResourcePoolHooks& hooks) noexcept;

// Non-owning `std::map<HashedStr, ResourceWrapper*, ResourceIdLess>` (the original map at pool+0x10).
// `key_deallocator` releases the node-key/own-key/owner-id control blocks through the existing 0xf590
// lift; a null deallocator is only acceptable when no free is required.
class ResourcePool {
public:
    using Map = std::map<HashedStr, ResourceWrapper*, ResourceIdLess>;

    explicit ResourcePool(SharedStrDeallocatorF590* key_deallocator = nullptr) noexcept;
    ~ResourcePool();

    ResourcePool(const ResourcePool&) = delete;
    ResourcePool& operator=(const ResourcePool&) = delete;
    ResourcePool(ResourcePool&&) = delete;
    ResourcePool& operator=(ResourcePool&&) = delete;

    // 0x14e260: hit adds one +0x30 and returns {ptr, false}; miss allocates a 0x100 B wrapper, copies
    // the key (control +1), inserts the non-owning node, and returns one reference.
    [[nodiscard]] bool get_resource(const HashedStr& key, ResourceRef& out,
                                    const ResourcePoolHooks& hooks) noexcept;

    // 0x14e440: hit as above; miss returns {null_resource_wrapper(), false} with one added reference
    // and never inserts.
    [[nodiscard]] bool try_get_resource(const HashedStr& key, ResourceRef& out,
                                        const ResourcePoolHooks& hooks) noexcept;

    // 0x14e9b0 (pool slot 9): only at +0x30 == 0; finds the node by the wrapper's key, erases it and
    // releases the node key. The value is not owned.
    [[nodiscard]] bool erase(ResourceWrapper* wrapper) noexcept;

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] const Map& entries() const noexcept;
    [[nodiscard]] SharedStrDeallocatorF590* key_deallocator() const noexcept;

private:
    [[nodiscard]] bool release_node_key(HashedStr& key) noexcept;

    Map map_{};
    SharedStrDeallocatorF590* key_deallocator_ = nullptr;
};

}  // namespace sotc
