// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): native model of startup constructor slot 504 (RVA 0x23f4d0, 562 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 504. Every body fact below is
// byte-pinned by tools/lift/check_ctor_slot_504.py. NOTHING here executes, loads or calls an original RVA: vtables,
// functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order: (1) the UNBOUNDED float console variable `uiOverrideSafeFrame` (object 0x2eea2d8, flags 1,
// value = default = 0.0f, caller functor tag 0x159ba68) built by 0x6aa00 through the family wrapper
// helper_6aa00_unbounded_float (the tag is a member of the 68-table float family; the wrapper returns
// HelperRefusal::none only for such a tag), call [argfn + 0x18] (bare ret), [obj] = 0x15942a0 + 0x10, value/default
// stores, 0x20d60, f590(description), f590(name), __cxa_atexit(0x20f0, var); (2) the bool console variable
// `uiShowSafeFrame` (object 0x2eea3b0, tag 0x159ba88 stored directly at +0xb0, value 0 / default 0, no menu item)
// built by 0x20af0 with the late-release order (stores, 0x20d60, f590(description), f590(name), atexit 0x20b0).
// The description strings are borrowed rodata views, so the four 0xf590 releases are exact no-ops.
// NOT established: who reads the two variables (a safe-frame percentage override and a debug overlay switch, by
// their names; INFERENCE only, no consumer is lifted). The __stack_chk_guard compare is a hook (not modelled).
#pragma once

#include <cstddef>
#include <cstdint>

#include "helper_6aa00_unbounded_float.hpp"
#include "lane_m4_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot504Rva = 0x23f4d0;
inline constexpr std::size_t kSlot504Size = 562;
inline constexpr std::uint64_t kSlot504FloatFnTag = 0x159ba68;
inline constexpr std::uint64_t kSlot504BoolFnTag = 0x159ba88;
inline constexpr char kSlot504FloatName[] = "uiOverrideSafeFrame";
inline constexpr std::size_t kSlot504FloatNameLength = 0x13;
inline constexpr char kSlot504FloatDescription[] =
    "If non zero this value will be used for the safe frame percentage. Sony's config app tops out at 0.9.";
inline constexpr std::size_t kSlot504FloatDescriptionLength = 0x65;
inline constexpr char kSlot504BoolName[] = "uiShowSafeFrame";
inline constexpr std::size_t kSlot504BoolNameLength = 0xf;
inline constexpr char kSlot504BoolDescription[] = "If true the safe frame will be overlaid onto the screen for reference";
inline constexpr std::size_t kSlot504BoolDescriptionLength = 0x45;
inline constexpr std::uint64_t kSlot504FloatRva = 0x2eea2d8;
inline constexpr std::uint64_t kSlot504BoolRva = 0x2eea3b0;

struct Slot504Objects {
    ConCmdRangedVar float_var;   // 0x2eea2d8
    ConCmdBoolVar bool_var;      // 0x2eea3b0
};

struct Slot504Result {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;
    bool float_ctor_ok = false;
    bool float_destroy_ok = false;       // `call [argfn + 0x18]` is the bare-ret destroy
    bool float_views_inert = false;
    ConCmdRegisterResult float_registration{};
    bool float_atexit_recorded = false;  // 0x20f0 recorded, not run
    BoolVarResult bool_var{};
};

[[nodiscard]] Slot504Result init_slot504_23f4d0(Slot504Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
