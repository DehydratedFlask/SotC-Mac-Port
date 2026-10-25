// Native recovery of original constructor-table index 299 / RVA 0xb0c7a0 (ELF sha256
// d0723afb...5861d3). 926-byte static initialiser (sha256 634d3052...b2f2462) of the debug-text
// scale settings. Four objects, in this order:
//
//   A  0x3305a30  float variable `globalDebugTextScale`        (0xc300, value cell A+0xd0)
//        name view {"globalDebugTextScale" (0x12cfb00), len 0x14}, description = empty view
//        range min 0.1 (0x3dcccccd), max 10.0 (0x41200000); args: four zero qwords, functor
//        {payload 0, vtable 0x15d57e0, aux 0}
//        [A] = 0x15942b0; value = 1.0f (0x3f800000) at A+0xd0 and default at A+0x68
//        call 0xf590(description); functor-destroy slot 0xc4f0 (bare ret); call 0x20d60(A);
//        call 0xf590(name); __cxa_atexit(0x20f0, A, dso)
//   B  0x3305b08  bool variable `globalDebugTextAdjustFor4K`   (shared bool-var shape, fn tag
//        0x15d5800, name len 0x1a, value 1 / default 1); atexit(0x20b0, B, dso)
//   C  0x3305bd0  debug-menu item (0x27050) label "Debug", text "Global Debug Text Scale",
//        target A, flags 0, range {0.1 (0x12cfac8), 1.0 (0x12cfacc)}; registered with a null
//        parent; atexit(0x272d0, C, dso)
//   D  0x3305c78  debug-menu item (0x26ea0) label "Debug", text "Global Debug Text Scale Adjust
//        for 4k" (len 0x25), target B, flags 0x30; the lifted bool-class slots return {1, 0}, so
//        the default range {1.0, 10.0} is kept; atexit(0x272d0, D, dso)
//
// The earlier analysis misread the maximum as 20.0 and the cluster as "debug-only with no
// modelable subsystem": the maximum is 10.0 and every callee is a lifted body. The description
// views are empty strings that point at NUL bytes in rodata (0x12cfb84), the label/text views
// point at the strings above; none carries a control counter.
//
// Registered destructors 0x20f0 / 0x20b0 / 0x272d0 are pinned, not executed; the debug-menu
// parent contract (a non-null parent) and the assert branches of 0x268b0 are visible failures.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot299Rva = 0xb0c7a0;
inline constexpr std::size_t kSlot299BodySize = 926;
inline constexpr std::uint64_t kSlot299ScaleRva = 0x3305a30;
inline constexpr std::uint64_t kSlot299AdjustRva = 0x3305b08;
inline constexpr std::uint64_t kSlot299ScaleItemRva = 0x3305bd0;
inline constexpr std::uint64_t kSlot299AdjustItemRva = 0x3305c78;
inline constexpr std::uint64_t kSlot299AdjustFnVtbl = 0x15d5800;   // functor vtable tag of B
inline constexpr std::uint32_t kSlot299AdjustItemFlags = 0x30;
inline constexpr char kSlot299ScaleName[] = "globalDebugTextScale";                 // len 0x14
inline constexpr char kSlot299AdjustName[] = "globalDebugTextAdjustFor4K";          // len 0x1a
inline constexpr char kSlot299Label[] = "Debug";                                    // len 5
inline constexpr char kSlot299ScaleText[] = "Global Debug Text Scale";              // len 0x17
inline constexpr char kSlot299AdjustText[] = "Global Debug Text Scale Adjust for 4k"; // len 0x25

struct Slot299Objects {
    ConCmdRangedVar scale{};        // A, 0x3305a30
    ConCmdBoolVar adjust{};         // B, 0x3305b08
    DebugMenuItem scale_item{};     // C, 0x3305bd0
    DebugMenuItem adjust_item{};    // D, 0x3305c78
};

struct Slot299Result {
    bool ok = false;
    bool scale_ctor_ok = false;
    bool scale_functor_destroy_noop = false;
    ConCmdRegisterResult scale_registration{};
    bool scale_views_inert = false;
    bool scale_atexit_recorded = false;
    BoolVarResult adjust{};
    bool scale_item_ctor_ok = false;
    DebugMenuRegisterResult scale_item_registration{};
    bool scale_item_views_inert = false;
    bool scale_item_atexit_recorded = false;
    bool adjust_item_ctor_ok = false;
    DebugMenuRegisterResult adjust_item_registration{};
    bool adjust_item_views_inert = false;
    bool adjust_item_atexit_recorded = false;
};

// Models 0xb0c7a0 against the shared ConCmd registry and the debug-menu registry.
[[nodiscard]] Slot299Result init_slot299_b0c7a0(Slot299Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
