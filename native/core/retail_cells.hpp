// Adapters that turn guest addresses into the host cells the lifted models expect.
//
// Each modelled target reads a few fields of an object (a once flag, a vtable slot pointer, a source
// qword, a key string). These helpers resolve those addresses through a `GuestWindow`, so a retail
// call frame can drive the native model without the model ever dereferencing a guest address itself.
//
// The offsets come from the pinned bodies and are the same constants their headers declare; a checker
// cross-checks the two, so an adapter cannot drift from the body it marshals.
#pragma once

#include "guest_memory.hpp"

#include "vtable_15949a0_slot88_187c0.hpp"
#include "vtable_1594ae8_slot28_13d20.hpp"
#include "vtable_1594dc0_slot20_20d60.hpp"
#include "vtable_15955c0_slot08_358c0.hpp"
#include "vtable_1598e10_slot18_1bf710.hpp"
#include "vtable_15d1678_slot_68_a40e20.hpp"
#include "vtable_15d1678_slot_e8_c2e0.hpp"
#include "vtable_15d1d80_slot08_c740.hpp"
#include "vtable_15e0b48_slot28_ddf4b0.hpp"
#include "vtable_15d1678_slot280_a460f0.hpp"
#include "vtable_15e2d60_slot_d0_e24900.hpp"
#include "vtable_15e1ee0_slot_b0_e19980.hpp"
#include "vtable_15e2028_slot70_e1a5c0.hpp"
#include "vtable_15ed2a0_slot_c0_e26f90.hpp"
#include "vtable_15f2f08_slot38_e16a00.hpp"
#include "vtable_159c908_slot58_2687b0.hpp"

#include <cstdint>

namespace sotc {

// 0x20d60: once flag at self+0x18, global cell at 0x2dfcbc8.
struct Marshal20d60 {
    bool ok = false;
    GuestStatus status = GuestStatus::no_window;
    Vtable1594dc0Slot20_20d60Cells cells{};
};

[[nodiscard]] Marshal20d60 marshal_20d60(GuestWindow& window, std::uint64_t self,
                                         std::uint64_t global_address) noexcept;

// 0x358c0: table at self+0x20, copy at self+0x10, source qword at source+0x10.
struct Marshal358c0 {
    bool ok = false;
    GuestStatus status = GuestStatus::no_window;
    Vtable15955c0Slot08_358c0Cells cells{};
};

[[nodiscard]] Marshal358c0 marshal_358c0(GuestWindow& window, std::uint64_t self,
                                         std::uint64_t source) noexcept;

// 0x13d20: dword field at self+0x1c.
struct Marshal13d20 {
    bool ok = false;
    GuestStatus status = GuestStatus::no_window;
    Vtable1594ae8Slot28_13d20Cells cells{};
};

[[nodiscard]] Marshal13d20 marshal_13d20(GuestWindow& window, std::uint64_t self) noexcept;

// 0x187c0: the model takes only `self`; the adapter exists so callers have one uniform surface.
struct Marshal187c0 {
    bool ok = false;
    GuestStatus status = GuestStatus::no_window;
    Vtable15949a0Slot88_187c0Cells cells{};
};

[[nodiscard]] Marshal187c0 marshal_187c0(GuestWindow& window, std::uint64_t self) noexcept;

// 0x2687b0 and 0x1bf710: the teardown models take only `self`; the adapter verifies the address is
// inside the window and hands the guest pointer through.
struct Marshal2687b0 {
    bool ok = false;
    GuestStatus status = GuestStatus::no_window;
    Vtable159c908Slot58_2687b0Cells cells{};
};

[[nodiscard]] Marshal2687b0 marshal_2687b0(GuestWindow& window, std::uint64_t self) noexcept;

struct Marshal1bf710 {
    bool ok = false;
    GuestStatus status = GuestStatus::no_window;
    Vtable1598e10Slot18_1bf710Cells cells{};
};

[[nodiscard]] Marshal1bf710 marshal_1bf710(GuestWindow& window, std::uint64_t self) noexcept;

// 0xc2e0 and 0xc740: self + source copies. Offsets come from each model header.
struct Marshalc2e0 {
    bool ok = false;
    GuestStatus status = GuestStatus::no_window;
    Vtable15d1678SlotE8_c2e0Cells cells{};
};

[[nodiscard]] Marshalc2e0 marshal_c2e0(GuestWindow& window, std::uint64_t self,
                                       std::uint64_t source) noexcept;

struct Marshalc740 {
    bool ok = false;
    GuestStatus status = GuestStatus::no_window;
    Vtable15d1d80Slot08_c740Cells cells{};
};

[[nodiscard]] Marshalc740 marshal_c740(GuestWindow& window, std::uint64_t self,
                                       std::uint64_t source) noexcept;

// 0xe16a00: the model reads the vtable pointer at `*this` and forwards one argument.
struct MarshalE16a00 {
    bool ok = false;
    GuestStatus status = GuestStatus::no_window;
    Vtable15f2f08Slot38_e16a00Cells cells{};
};

[[nodiscard]] MarshalE16a00 marshal_e16a00(GuestWindow& window, std::uint64_t self,
                                           std::uint64_t argument) noexcept;

// 0xddf4b0: manager dispatch; the model only needs `self`.
struct MarshalDdf4b0 {
    bool ok = false;
    GuestStatus status = GuestStatus::no_window;
    Vtable15e0b48Slot28_ddf4b0Cells cells{};
};

[[nodiscard]] MarshalDdf4b0 marshal_ddf4b0(GuestWindow& window, std::uint64_t self) noexcept;

// 0xa460f0: teardown; needs the vtable cell at `*self` and the helper pointer at `self+0xc0`.
struct MarshalA460f0 {
    bool ok = false;
    GuestStatus status = GuestStatus::no_window;
    Vtable15d1678Slot280_a460f0Cells cells{};
};

[[nodiscard]] MarshalA460f0 marshal_a460f0(GuestWindow& window, std::uint64_t self) noexcept;

// 0xe24900: sub-dispatch; reads the sub pointer at `self+0x8` and stores through `out`.
struct MarshalE24900 {
    bool ok = false;
    GuestStatus status = GuestStatus::no_window;
    Vtable15e2d60SlotD0_e24900Cells cells{};
};

[[nodiscard]] MarshalE24900 marshal_e24900(GuestWindow& window, std::uint64_t self,
                                           std::uint64_t out) noexcept;

// 0xe26f90: status gate. `self`/`arg2`/`arg3` come from the frame; the canary and the two scratch
// regions are host storage because the retail body uses its own stack and this layer has no guest
// stack mapping yet.
struct MarshalE26f90 {
    bool ok = false;
    GuestStatus status = GuestStatus::no_window;
    Vtable15ed2a0SlotC0_e26f90Cells cells{};
};

[[nodiscard]] MarshalE26f90 marshal_e26f90(GuestWindow& window, std::uint64_t self,
                                           std::uint64_t arg2, std::int32_t arg3,
                                           std::uint64_t* host_canary, void* host_buffer,
                                           void* host_local) noexcept;

// 0xe1a5c0: container scan. The array pointer lives at `self+0x8` and the count at `self+0x10`; the
// key is a guest C string.
struct MarshalE1a5c0 {
    bool ok = false;
    GuestStatus status = GuestStatus::no_window;
    Vtable15e2028Slot70_e1a5c0Cells cells{};
};

[[nodiscard]] MarshalE1a5c0 marshal_e1a5c0(GuestWindow& window, std::uint64_t self,
                                           std::uint64_t key, std::int32_t value) noexcept;

// 0xe19980: alignment-gated wrapper. Arguments come from the frame; the canary and the slow-path
// buffer are host storage (no guest stack mapping yet), and the fast-path store target is the guest
// address in `rdx` when it is 8-aligned.
struct MarshalE19980 {
    bool ok = false;
    GuestStatus status = GuestStatus::no_window;
    Vtable15e1ee0SlotB0_e19980Args args{};
    Vtable15e1ee0SlotB0_e19980Cells cells{};
};

[[nodiscard]] MarshalE19980 marshal_e19980(GuestWindow& window, std::uint64_t arg2,
                                           std::uint64_t arg3, std::uint64_t arg4,
                                           std::uint64_t arg5, std::uint64_t* host_canary,
                                           const void* const* value_cell) noexcept;

}  // namespace sotc
