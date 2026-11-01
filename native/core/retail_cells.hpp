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

}  // namespace sotc
