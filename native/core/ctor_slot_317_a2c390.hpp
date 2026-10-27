// CPU-01-STARTUP-SLOT-317-01: native recovery of original constructor-table index 317 / RVA 0xa2c390
// (969 B, [0xa2c390, 0xa2c759)), ELF sha256 d0723afb...5861d3: the "Controller" block of the
// "Platform Services" debug menu: the `vibration_large_scalar` and `vibration_small_scalar` UNBOUNDED float
// variables (0x6aa00, value = default = 0.5f), a "Controller" group and two toggle items. Every byte below was
// read from the pinned image with xcrun llvm-objdump and is pinned by tools/lift/check_ctor_slot_317.py.
// Nothing here executes an original RVA; vtable pointers, destructors and functor tags are identity tags.
//
// Sequence (.bss addresses):
//   1. large     0x32f4c38 = vibration_large_scalar (name view 0x12c1520/0x16, empty description 0x12c159e):
//                [obj] = 0x15942b0; 0x6aa00(obj, name, obj+0xd0, flags 1, args = zero block with functor
//                tag 0x15d10f0, description); 0xf590(description); caller functor destroy (slot 3 = 0xc4f0,
//                bare ret); [obj] = 0x15942b0; value = default = 0x3f000000; 0x20d60 (register);
//                0xf590(name); __cxa_atexit(0x20f0)      [shape: core/ctor_shapes_343_356 init_float_var_static]
//   2. small     0x32f4d10 = vibration_small_scalar (0x12c1537/0x16): the same sequence
//   3. group     0x32f4de8 = 0x26c10(parent "Platform Services" (0x12c154e, 0x11), caption "Controller"
//                (0x12c1560, 0xa), flags 0x30); 0xf590 x2; __cxa_atexit(0x272d0)
//   4. toggle 1  0x32f4e90 = 0x26ea0(label "Controller", text "Large Vibration Scalar" (0x12c156b, 0x16),
//                target = large, flags 0x30); 0xf590 x2; __cxa_atexit(0x272d0)
//   5. toggle 2  0x32f4f38 = 0x26ea0(label "Controller", text "Small Vibration Scalar" (0x12c1582, 0x16),
//                target = small, flags 0x30); 0xf590 x2; __cxa_atexit(0x272d0)
// The float class 0x15942b0 answers slot 3 = 1 (0xbba0) and slot 6 = 3 (0xbbf0), so 0x26ea0 NARROWS the
// toggle range to {0.1, 0.5} (this is the first slot module whose toggles take the narrowed branch).
//
// The caller's argument functor tag 0x15d10f0 is not in core/ctor_shapes_343_356's accept list; its four
// slots are the same bodies (0xc4a0 / 0xc4c0 / 0xc4e0 / 0xc4f0) as the accepted tag 0x15d57e0, which the
// checker proves from the ELF, so the shared helper is driven with the equivalent accepted tag. The tag is
// never stored in the object (the class tag 0x1594318 is), so the substitution is unobservable.
// The consumers of the two scalars (controller rumble) are NOT modelled: no vibration behaviour is claimed.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "ctor_shapes_343_356.hpp"
#include "debug_menu_item.hpp"
#include "fmenu_ctors_slot306.hpp"
#include "l2_slot_common.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot317Rva = 0xa2c390;
inline constexpr std::size_t kSlot317BodySize = 969;
inline constexpr std::uint64_t kSlot317LargeRva = 0x32f4c38;
inline constexpr std::uint64_t kSlot317SmallRva = 0x32f4d10;
inline constexpr std::uint64_t kSlot317GroupRva = 0x32f4de8;
inline constexpr std::uint64_t kSlot317Toggle1Rva = 0x32f4e90;
inline constexpr std::uint64_t kSlot317Toggle2Rva = 0x32f4f38;
inline constexpr std::uint64_t kSlot317ArgFnVtbl = 0x15d10f0;            // the site's real functor tag
inline constexpr std::uint64_t kSlot317ArgFnEquivalent = 0x15d57e0;      // accepted tag with identical slots
inline constexpr std::uint32_t kSlot317DefaultBits = 0x3f000000;         // 0.5f
inline constexpr std::uint32_t kSlot317VarFlags = 1;
inline constexpr std::uint32_t kSlot317ItemFlags = 0x30;
inline constexpr std::uint64_t kFloatClassSlot3Rva = 0xbba0;             // mov eax,1; ret
inline constexpr std::uint64_t kFloatClassSlot6Rva = 0xbbf0;             // mov eax,3; ret
inline constexpr std::uint32_t kFloatClassSlot3Result = 1;
inline constexpr std::uint32_t kFloatClassSlot6Result = 3;
inline constexpr char kSlot317LargeName[] = "vibration_large_scalar";   // 0x12c1520, 0x16
inline constexpr char kSlot317SmallName[] = "vibration_small_scalar";   // 0x12c1537, 0x16
inline constexpr char kSlot317Parent[] = "Platform Services";           // 0x12c154e, 0x11
inline constexpr char kSlot317Caption[] = "Controller";                 // 0x12c1560, 0xa
inline constexpr char kSlot317Toggle1Text[] = "Large Vibration Scalar"; // 0x12c156b, 0x16
inline constexpr char kSlot317Toggle2Text[] = "Small Vibration Scalar"; // 0x12c1582, 0x16
static_assert(sizeof kSlot317LargeName - 1 == 0x16 && sizeof kSlot317SmallName - 1 == 0x16);
static_assert(sizeof kSlot317Parent - 1 == 0x11 && sizeof kSlot317Caption - 1 == 0xa);
static_assert(sizeof kSlot317Toggle1Text - 1 == 0x16 && sizeof kSlot317Toggle2Text - 1 == 0x16);

// The two virtual-slot results 0x26ea0 reads from a variable of the float class 0x15942b0 (known only when
// the variable's vptr is exactly that class).
[[nodiscard]] DebugMenuTarget float_var_menu_target(const ConCmdRangedVar& var) noexcept;

struct Slot317Objects {
    ConCmdRangedVar large{};     // 0x32f4c38
    ConCmdRangedVar small{};     // 0x32f4d10
    DebugMenuItem group{};       // 0x32f4de8
    DebugMenuItem toggle1{};     // 0x32f4e90
    DebugMenuItem toggle2{};     // 0x32f4f38
};

enum class Slot317Stage : std::uint8_t {
    none,
    large_build,
    small_build,
    group_ctor,
    group_registration,
    group_view_release,
    group_atexit,
    toggle1_ctor,
    toggle1_registration,
    toggle1_view_release,
    toggle1_atexit,
    toggle2_ctor,
    toggle2_registration,
    toggle2_view_release,
    toggle2_atexit,
};
[[nodiscard]] const char* slot317_stage_name(Slot317Stage stage) noexcept;

struct Slot317Result {
    bool ok = false;
    Slot317Stage blocked = Slot317Stage::none;
    FloatVarResult large{};
    FloatVarResult small{};
    MenuRegisterResult group_registration{};
    MenuRegisterResult toggle1_registration{};
    MenuRegisterResult toggle2_registration{};
};

[[nodiscard]] Slot317Result init_slot317_a2c390(Slot317Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu,
                                                const DebugMenuAssertHooks& hooks = {}) noexcept;

}  // namespace sotc
