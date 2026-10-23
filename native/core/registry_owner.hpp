// Native owner for the application-info registry container and its list-3 delegates.
//
// Recovered shape (E561/E562/E563):
//   `0x2569d0` allocates 0x60 B through `0x17070`, runs the `0x1bdb10` ctor, destroys any previous
//   container through `0x1bede0` + `0x170b0`, stores the new pointer into `*(0x2ee68c8)`, and
//   registers four callbacks. Only the fourth is a list-3 delegate: `{receiver = app object,
//   fn = 0x25a4d0, adjust = 0}`. `0x1290` later registers `0xb10` the same way.
//
//   `0x1bdf30(container, ...)` walks list-3 from `sentinel.prev` and follows `node.prev` after each
//   call (0x1bdf66 `mov rbx,[rdi+0x40]`, 0x1bdf79 `mov rbx,[rbx]`). Because `0x1beeb0` pushes at
//   the front, `sentinel.prev` is the OLDEST node, so dispatch is FIFO in registration order.
//   This corrects the earlier LIFO inference in the worker report.
//
// This module is the native mechanism, not a full lift of either 0x2569d0 or 0x1bdf30:
//   - it stores native function pointers (never original RVAs) in the delegate node's `invoke` slot;
//   - it models only the list-3 loop of 0x1bdf30 (the function has further work after that loop);
//   - it does not model the other three callback installs, the member initialisation before
//     `0x256f98`, the previous-container teardown internals, or the container dtor `0x1bede0`.
// G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

#include "app_info_registry.hpp"
#include "insert_1beeb0.hpp"

namespace sotc {

inline constexpr std::uint64_t kRegistryOwnerConstructRva = 0x256F98;
inline constexpr std::uint64_t kRegistryOwnerCtorRva = 0x1BDB10;
inline constexpr std::uint64_t kRegistryOwnerDispatchRva = 0x1BDF30;
inline constexpr std::uint64_t kRegistryOwnerProductInstallerRva = 0x25A4D0;
inline constexpr std::uint64_t kRegistryOwnerGameInstallerRva = 0xB10;

// Native delegate: `fn(receiver, registry)`. The receiver is the value the original stores at
// node+0x10 (the app object for both known delegates); `adjust` is always 0 in the observed sites.
using RegistryDelegateFn = void (*)(void* receiver, AppInfoRegistry& registry);

struct RegistryDelegate {
    void* receiver = nullptr;
    RegistryDelegateFn fn = nullptr;
};

struct RegistryOwnerDispatchResult {
    bool ok = false;
    int called = 0;              // native delegates invoked
    int skipped_identity = 0;    // nodes whose `fn` is a non-zero original identity, never called
    Node1beeb0* failed_node = nullptr;  // null native `fn` stops the walk
};

// Owns one 0x60 B registry container and the allocation seam for its list-3 nodes. The container is
// constructed eagerly (0x1bdb10). Delegate nodes are heap-allocated through `alloc`; a missing
// allocator is a visible add failure.
class RegistryOwner {
public:
    explicit RegistryOwner(const Insert1beeb0Alloc& alloc = {}) noexcept;
    ~RegistryOwner();

    RegistryOwner(const RegistryOwner&) = delete;
    RegistryOwner& operator=(const RegistryOwner&) = delete;

    [[nodiscard]] AppInfoRegistry& registry() noexcept { return registry_; }
    [[nodiscard]] const AppInfoRegistry& registry() const noexcept { return registry_; }

    // Appends a list-3 delegate in registration order (0x1beeb0 push-front under the hood).
    // `fn` must be a native function; original RVAs are never stored or called.
    [[nodiscard]] Insert1beeb0Result add_delegate(const RegistryDelegate& delegate) noexcept;

    // Number of list-3 nodes (container count at +0x50).
    [[nodiscard]] std::uint64_t delegate_count() const noexcept;

    // Runs the recovered list-3 traversal: oldest first, `node.prev` after each call, receiver =
    // node[0x10] + node[0x20]. Native delegates are tagged by `fn == 0`; a node with a non-zero
    // `fn` is an identity-only descriptor from `0x1290` (its invoke value is an original RVA) and is
    // skipped, never called. A null `invoke` on a native delegate stops the walk with ok == false.
    [[nodiscard]] RegistryOwnerDispatchResult dispatch() noexcept;

    // Frees every list-3 node through the owner's seam and reinitialises the list-3 sentinel.
    // Test/teardown helper; the original uses the unlifted 0x1bede0 container destructor.
    void clear_delegates() noexcept;

private:
    AppInfoRegistry registry_{};
    Insert1beeb0Alloc alloc_{};
};

}  // namespace sotc
