// CPU-01-STARTUP-BATCH-8: native model of startup constructor slot 444 (RVA 0x4d3f10, 1276 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 444. Every body fact below is byte-pinned by tools/lift/check_ctor_slot_444.py. NOTHING here
// executes, loads or calls an original RVA: vtables, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape (no menu item, no group): 1 bool and 4 unbounded float console variables, in order (object RVA, kind, value = default):
//   fvar0  0x2efb348  float  0x3f333333 `uiPauseMenuOpacity`
//   fvar1  0x2efb420  float  0x3f4ccccd `uiPauseMenuIntensity`
//   fvar2  0x2efb4f8  float  0x3f733333 `uiWorldMapOpacity`
//   fvar3  0x2efb5d0  float  0x00000000 `uiWorldMapIntensity`
//   bvar0  0x2efb6a8  bool   0          `fontShowExternalElementExtents`
// Bool rows are the static/literal bool shape (caller functor tag 0x15c1be8); float rows go through helper_init_unbounded_float_var_site (0x6aa00 through
// helper_6aa00_unbounded_float, caller functor tag 0x15c1bc8, early description release): the slot requires HelperRefusal::none, i.e. the tag is a
// member of the 68-table float family. All strings are borrowed rodata views, so every 0xf590 release is an exact no-op. The meaning of the variables
// beyond their names (UI pause menu / world map opacity and intensity, a font extents toggle) is INFERENCE from the strings only; no consumer is lifted. The __stack_chk_guard compare is a hook.
#pragma once

#include <cstddef>
#include <cstdint>

#include "helper_int_var_sites.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot444Rva = 0x4d3f10;
inline constexpr std::size_t kSlot444Size = 1276;
inline constexpr std::uint64_t kSlot444BoolFnTag = 0x15c1be8;
inline constexpr std::uint64_t kSlot444FloatFnTag = 0x15c1bc8;

inline constexpr std::size_t kSlot444BoolCount = 1;
inline constexpr char kSlot444BoolNames[kSlot444BoolCount][31] = {"fontShowExternalElementExtents"};
inline constexpr std::size_t kSlot444BoolNameLengths[kSlot444BoolCount] = {0x1e};
inline constexpr char kSlot444BoolDescriptions[kSlot444BoolCount][1] = {""};
inline constexpr std::size_t kSlot444BoolDescriptionLengths[kSlot444BoolCount] = {0x0};
inline constexpr std::uint8_t kSlot444BoolValues[kSlot444BoolCount] = {0};
inline constexpr std::size_t kSlot444FloatCount = 4;
inline constexpr char kSlot444FloatNames[kSlot444FloatCount][21] = {"uiPauseMenuOpacity", "uiPauseMenuIntensity", "uiWorldMapOpacity", "uiWorldMapIntensity"};
inline constexpr std::size_t kSlot444FloatNameLengths[kSlot444FloatCount] = {0x12, 0x14, 0x11, 0x13};
inline constexpr char kSlot444FloatDescriptions[kSlot444FloatCount][1] = {"", "", "", ""};
inline constexpr std::size_t kSlot444FloatDescriptionLengths[kSlot444FloatCount] = {0x0, 0x0, 0x0, 0x0};
inline constexpr std::uint32_t kSlot444FloatValueBits[kSlot444FloatCount] = {0x3f333333, 0x3f4ccccd, 0x3f733333, 0x0};

inline constexpr std::uint64_t kSlot444Obj0Rva = 0x2efb348;

struct Slot444Objects {
    ConCmdRangedVar fvar0;   // 0x2efb348
    ConCmdRangedVar fvar1;   // 0x2efb420
    ConCmdRangedVar fvar2;   // 0x2efb4f8
    ConCmdRangedVar fvar3;   // 0x2efb5d0
    ConCmdBoolVar bvar0;   // 0x2efb6a8
};

struct Slot444Result {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;   // first float-wrapper refusal, if any
    BoolVarResult bvar[kSlot444BoolCount]{};
    HelperFloatSiteResult fvar[kSlot444FloatCount]{};
};

[[nodiscard]] Slot444Result init_slot444_4d3f10(Slot444Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
