// CPU-01-STARTUP-BATCH-8: native model of startup constructor slot 337 (RVA 0x7e81d0, 1849 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 337. Every body fact below is byte-pinned by tools/lift/check_ctor_slot_337.py. NOTHING here
// executes, loads or calls an original RVA: vtables, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape (no menu item, no group): two bool console variables, five unbounded float console variables, one 0x30 byte name holder. In order
// (object RVA, caller functor tag, value = default):
//   b0  0x3264980  bool   tag 0x15ce480  1            `wormholeReturnToCathedralFast`
//   b1  0x3264a48  bool   tag 0x15ce480  0            `wormholePauseWorldDuringFade`
//   f0  0x3264b10  float  tag 0x15ce4a0  0x40000000   `wormholeFadeOutSeconds`
//   f1  0x3264be8  float  tag 0x15ce4a0  0x40b9999a   `wormholeFadeOutDelaySeconds`
//   f2  0x3264cc0  float  tag 0x15ce4a0  0x3ff33333   `wormholeFadeOutDelaySecondsSwimming`
//   f3  0x3264d98  float  tag 0x15ce4a0  0x40c00000   `wormholeFadeInSeconds`
//   f4  0x3264e70  float  tag 0x15ce4a0  0x3f800000   `wormholeFadeInDelaySeconds`
//   h0  0x3264f48  0x30 B holder, vptr 0x15976a0 + 0x10, borrowed view `shadowDemoMode`, tail 0, atexit 0x8ef00
// Bool rows are the static/literal bool-variable shape (description empty). Float rows go through helper_init_unbounded_float_var_site
// (0x6aa00 through helper_6aa00_unbounded_float, early description release): the slot requires HelperRefusal::none, i.e. the caller tag is a
// member of the 68-table float family. All strings are borrowed rodata views, so every 0xf590 release is an exact no-op. The holder shape is the
// one of slots 539 / 550 (0x8ef00 is hash-pinned by their checkers and by this one). Meaning of the variables beyond their names (a "wormhole" fade
// timing page, holder named shadowDemoMode) is INFERENCE from the strings only; no consumer is lifted. The __stack_chk_guard compare is a hook.
#pragma once

#include <cstddef>
#include <cstdint>

#include "ctor_slot_539_121940.hpp"   // NameHolder30
#include "helper_int_var_sites.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot337Rva = 0x7e81d0;
inline constexpr std::size_t kSlot337Size = 1849;
inline constexpr std::uint64_t kSlot337BoolFnTag = 0x15ce480;
inline constexpr std::uint64_t kSlot337FloatFnTag = 0x15ce4a0;
inline constexpr std::uint64_t kSlot337HolderDtorRva = 0x8ef00;
inline constexpr std::uint64_t kSlot337HolderVptr = 0x15976b0;   // 0x15976a0 + 0x10 (identity tag)
inline constexpr char kSlot337HolderName[] = "shadowDemoMode";
inline constexpr std::size_t kSlot337HolderNameLength = 0xe;

inline constexpr std::size_t kSlot337BoolCount = 2;
inline constexpr char kSlot337BoolNames[kSlot337BoolCount][32] = {"wormholeReturnToCathedralFast", "wormholePauseWorldDuringFade"};
inline constexpr std::size_t kSlot337BoolNameLengths[kSlot337BoolCount] = {0x1d, 0x1c};
inline constexpr std::uint8_t kSlot337BoolValues[kSlot337BoolCount] = {1, 0};

inline constexpr std::size_t kSlot337FloatCount = 5;
inline constexpr char kSlot337FloatNames[kSlot337FloatCount][40] = {"wormholeFadeOutSeconds", "wormholeFadeOutDelaySeconds",
                                                                     "wormholeFadeOutDelaySecondsSwimming", "wormholeFadeInSeconds",
                                                                     "wormholeFadeInDelaySeconds"};
inline constexpr std::size_t kSlot337FloatNameLengths[kSlot337FloatCount] = {0x16, 0x1b, 0x23, 0x15, 0x1a};
inline constexpr std::uint32_t kSlot337FloatValueBits[kSlot337FloatCount] = {0x40000000, 0x40b9999a, 0x3ff33333, 0x40c00000, 0x3f800000};

inline constexpr std::uint64_t kSlot337Bool0Rva = 0x3264980;
inline constexpr std::uint64_t kSlot337Float0Rva = 0x3264b10;
inline constexpr std::uint64_t kSlot337HolderRva = 0x3264f48;

struct Slot337Objects {
    ConCmdBoolVar bvar0;       // 0x3264980
    ConCmdBoolVar bvar1;       // 0x3264a48
    ConCmdRangedVar fvar0;     // 0x3264b10
    ConCmdRangedVar fvar1;     // 0x3264be8
    ConCmdRangedVar fvar2;     // 0x3264cc0
    ConCmdRangedVar fvar3;     // 0x3264d98
    ConCmdRangedVar fvar4;     // 0x3264e70
    NameHolder30 holder;       // 0x3264f48
};

struct Slot337Result {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;   // first float-wrapper refusal, if any
    BoolVarResult bvar[kSlot337BoolCount]{};
    HelperFloatSiteResult fvar[kSlot337FloatCount]{};
    bool holder_ok = false;                        // holder built, f590 inert, (0x8ef00, holder) recorded
};

[[nodiscard]] Slot337Result init_slot337_7e81d0(Slot337Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
