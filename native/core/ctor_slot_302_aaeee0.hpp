// Original constructor-table index 302 / RVA 0xaaeee0: ten zeroed eight-byte handles,
// ten cleanup registrations, then one identity vptr publication. ELF d0723afb...5861d3;
// 343-byte body, 8-byte zero helper 0xaaf040, 52-byte cleanup 0xaaf050.
//
// Cleanup registrations are observations, never original executable pointers. The cleanup's
// null path is inert; its live path calls 0xaaf260 and frees buffer+object via 0x170b0.
// That live teardown remains unimplemented. This constructor creates no live pointee and
// does not claim teardown, allocation, or the consumers of the published identity vptr.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot302Rva = 0xaaeee0;
inline constexpr std::size_t kSlot302BodySize = 343;
inline constexpr std::size_t kSlot302HandleCount = 10;
inline constexpr std::uint64_t kSlot302FirstHandleRva = 0x3305440;
inline constexpr std::uint64_t kSlot302IdentityCellRva = 0x3305490;
inline constexpr std::uint64_t kSlot302IdentityVptr = 0x15d4638;
inline constexpr std::uint64_t kSlot302ZeroRva = 0xaaf040;
inline constexpr std::uint64_t kSlot302DtorRva = 0xaaf050;
inline constexpr std::size_t kSlot302DtorSize = 52;

struct Slot302Objects {
    std::array<void*, kSlot302HandleCount> handles{};
    std::uint64_t identity_vptr = 0;  // identity only; never dispatched
};
static_assert(offsetof(Slot302Objects, identity_vptr) == 0x50);

// Resets each handle immediately before recording its cleanup, preserving source order.
// Host log allocation failure is visible as false; all later source stores still occur.
[[nodiscard]] bool init_slot302_aaeee0(Slot302Objects& objects, StartupAtexitLog& cleanup) noexcept;

}  // namespace sotc
