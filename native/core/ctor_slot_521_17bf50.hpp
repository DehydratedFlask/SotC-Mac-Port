// CPU-01-STARTUP-BATCH-8: native model of startup constructor slot 521 (RVA 0x17bf50, 1674 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 521. Every body fact below is byte-pinned by tools/lift/check_ctor_slot_521.py. NOTHING here
// executes, loads or calls an original RVA: vtables, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape (no menu item, no group): 6 bool and 1 unbounded float console variables, in order (object RVA, kind, value = default):
//   fvar0  0x2ee3e18  float  0x3f800000 `fontDebugScale`
//   bvar0  0x2ee3ef0  bool   0          `fontDisableKerning`
//   bvar1  0x2ee3fb8  bool   0          `fontShowCharExtents`
//   bvar2  0x2ee4080  bool   0          `fontForceRightToLeft`
//   bvar3  0x2ee4148  bool   1          `fontAllow2Pass`
//   bvar4  0x2ee4240  bool   1          `fontExternalElementOffsetByEmptyHeading`
//   bvar5  0x2ee4308  bool   1          `fontExternalElementOffsetByEmptyFooting`
// Bool rows are the static/literal bool shape (caller functor tag 0x1598810); float rows go through helper_init_unbounded_float_var_site (0x6aa00 through
// helper_6aa00_unbounded_float, caller functor tag 0x15987f0, late description release): the slot requires HelperRefusal::none, i.e. the tag is a
// member of the 68-table float family. All strings are borrowed rodata views, so every 0xf590 release is an exact no-op. The meaning of the variables
// beyond their names (font debug flags and a text scale) is INFERENCE from the strings only; no consumer is lifted. The __stack_chk_guard compare is a hook.
#pragma once

#include <cstddef>
#include <cstdint>

#include "helper_int_var_sites.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot521Rva = 0x17bf50;
inline constexpr std::size_t kSlot521Size = 1674;
inline constexpr std::uint64_t kSlot521BoolFnTag = 0x1598810;
inline constexpr std::uint64_t kSlot521FloatFnTag = 0x15987f0;

inline constexpr std::size_t kSlot521BoolCount = 6;
inline constexpr char kSlot521BoolNames[kSlot521BoolCount][40] = {"fontDisableKerning", "fontShowCharExtents", "fontForceRightToLeft", "fontAllow2Pass", "fontExternalElementOffsetByEmptyHeading", "fontExternalElementOffsetByEmptyFooting"};
inline constexpr std::size_t kSlot521BoolNameLengths[kSlot521BoolCount] = {0x12, 0x13, 0x14, 0xe, 0x27, 0x27};
inline constexpr char kSlot521BoolDescriptions[kSlot521BoolCount][79] = {"Disables kerning for all font text.", "Disables blending for fonts so quad boundaries can be seen.", "Makes all text render as though the characters were 'right to left' charcters.", "Allow 2 pass font rendering where applicable.", "", ""};
inline constexpr std::size_t kSlot521BoolDescriptionLengths[kSlot521BoolCount] = {0x23, 0x3b, 0x4e, 0x2d, 0x0, 0x0};
inline constexpr std::uint8_t kSlot521BoolValues[kSlot521BoolCount] = {0, 0, 0, 1, 1, 1};
inline constexpr std::size_t kSlot521FloatCount = 1;
inline constexpr char kSlot521FloatNames[kSlot521FloatCount][15] = {"fontDebugScale"};
inline constexpr std::size_t kSlot521FloatNameLengths[kSlot521FloatCount] = {0xe};
inline constexpr char kSlot521FloatDescriptions[kSlot521FloatCount][102] = {"Scale applied to all text. Can be used to shrink and grow all the text in the game for debug purposes"};
inline constexpr std::size_t kSlot521FloatDescriptionLengths[kSlot521FloatCount] = {0x65};
inline constexpr std::uint32_t kSlot521FloatValueBits[kSlot521FloatCount] = {0x3f800000};

inline constexpr std::uint64_t kSlot521Obj0Rva = 0x2ee3e18;

struct Slot521Objects {
    ConCmdRangedVar fvar0;   // 0x2ee3e18
    ConCmdBoolVar bvar0;   // 0x2ee3ef0
    ConCmdBoolVar bvar1;   // 0x2ee3fb8
    ConCmdBoolVar bvar2;   // 0x2ee4080
    ConCmdBoolVar bvar3;   // 0x2ee4148
    std::uint8_t pad_bvar4[0x30];   // gap 0x2ee4210..0x2ee4240
    ConCmdBoolVar bvar4;   // 0x2ee4240
    ConCmdBoolVar bvar5;   // 0x2ee4308
};

struct Slot521Result {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;   // first float-wrapper refusal, if any
    BoolVarResult bvar[kSlot521BoolCount]{};
    HelperFloatSiteResult fvar[kSlot521FloatCount]{};
};

[[nodiscard]] Slot521Result init_slot521_17bf50(Slot521Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
