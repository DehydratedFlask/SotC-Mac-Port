// Native model of the registry-owner slot replace at `0x256f98..0x256fd2`.
//
// Recovered sequence (E567):
//   new = 0x17070(0x60); 0x1bdb10(new)
//   old = *(0x2ee68c8)
//   if (old != new) { 0x1bede0(old); 0x170b0(old) }
//   *(0x2ee68c8) = new
//
// A native `RegistryOwner` is a value object, so the slot uses unique ownership instead of the raw
// allocator: creating the new owner first, tearing the previous one down through the lifted
// `container_dtor_1bede0`, then replacing the pointer reproduces the observable replace order. The
// previous owner's destructor sees the lists already reset by the teardown, so no node is freed
// twice.
//
// NOT modelled: the 18 other slots written by 0x2569d0, the static atexit registration `0x1c00e0`,
// the 0x17070/0x170b0 bodies, and any concurrency (the original has no lock here).
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include "container_dtor_1bede0.hpp"
#include "registry_owner.hpp"

namespace sotc {

inline constexpr std::uint64_t kRegistrySlotRva = 0x2EE68C8;
inline constexpr std::uint64_t kRegistrySlotAllocRva = 0x17070;
inline constexpr std::uint64_t kRegistrySlotFreeRva = 0x170B0;
inline constexpr std::uint64_t kRegistrySlotReplaceRva = 0x256F98;
inline constexpr std::size_t kRegistrySlotOwnerBytes = 0x60;

struct RegistrySlotHooks {
    // Allocation seam for the new owner's list-3 delegate nodes.
    Insert1beeb0Alloc delegates{};
    // Teardown of the previous owner: one tag-2 free hook for both node sizes.
    ContainerDtor1bede0Hooks dtor{};
};

struct RegistrySlotInstallResult {
    bool ok = false;
    bool created = false;
    bool replaced_previous = false;
    int freed_previous_nodes = 0;
    int previous_release_failures = 0;
};

// Replaces `slot` with a freshly constructed RegistryOwner. The previous owner is torn down through
// container_dtor_1bede0 before the replacement. A missing dtor free hook or an allocation failure
// leaves the previous owner untouched and reports ok == false.
class RegistrySlot {
public:
    RegistrySlot() noexcept = default;
    RegistrySlot(const RegistrySlot&) = delete;
    RegistrySlot& operator=(const RegistrySlot&) = delete;

    [[nodiscard]] RegistrySlotInstallResult install(const RegistrySlotHooks& hooks) noexcept;

    [[nodiscard]] RegistryOwner* get() noexcept { return owner_.get(); }
    [[nodiscard]] const RegistryOwner* get() const noexcept { return owner_.get(); }
    [[nodiscard]] int replace_count() const noexcept { return replace_count_; }

private:
    std::unique_ptr<RegistryOwner> owner_{};
    int replace_count_ = 0;
};

}  // namespace sotc
