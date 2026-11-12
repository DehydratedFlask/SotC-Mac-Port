// CPU-01-STARTUP-BATCH-7: native model of startup constructor slot 396 (RVA 0x650f20, 2845 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 396. Every fact below is byte-pinned by tools/lift/check_ctor_slot_396.py (and the 0x27120 slider
// constructor by tools/lift/check_helper_27120.py). NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and
// destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order (all under the menu parent / caption `CP10-Anim`, item flags 0x30):
//   bool  shadowEnableRootFittingExpression          (0x3010280, value = default = 1) + 0x26db0 toggle (0x3010348)
//   bool  shadowEnableRootFittingExpressionFKLimiting (0x30103f0, 1)                  + 0x26db0 toggle (0x30104b8)
//   bool  shadowEnableRootFitting                    (0x3010560, 1)                  + 0x26db0 toggle (0x3010628)
//   bool  shadow_BlendRootFitting                    (0x30106d0, 0)                  + 0x26ea0 toggle `Enable blend root fitting` (0x3010798)
//   bool  cp10_drawRootFitting                       (0x3010840, 0)                  + 0x26db0 toggle (0x3010908)
//   float cp10_rootFittingExpressionUserFKDampen     (0x30109b0, bounded {0, 1}, 0.5) + 0x27120 slider (0x3010a88)
//   float cp10_rootFittingAltBoneDampenFactor        (0x3010b30, bounded {0, 1}, 0.75) + 0x27120 slider (0x3010c08)
//   bool  shadow_rootFittingEnableSelectiveRecomputeGlobal (0x3010cb0, 1)             + 0x26db0 toggle (0x3010d78)
// The bool variables use the static/literal shape (tag 0x15cc548), the floats 0xc300 with tag 0x15cc568; the sliders range {0.01, 0.1}.
//
// NOT established: what reads the variables at run time. "Root fitting" reading from the strings is INFERENCE only.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "helper_int_var_sites.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot396Rva = 0x650f20;
inline constexpr std::size_t kSlot396Size = 2845;
inline constexpr std::uint64_t kSlot396BoolFnTag = 0x15cc548;
inline constexpr std::uint64_t kSlot396FloatFnTag = 0x15cc568;
inline constexpr char kSlot396Parent[] = "CP10-Anim";
inline constexpr std::size_t kSlot396ParentLength = 9;
inline constexpr char kSlot396ToggleText[] = "Enable blend root fitting";
inline constexpr std::size_t kSlot396ToggleTextLength = 0x19;
inline constexpr std::uint32_t kSlot396ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot396VarFlags = 1;
inline constexpr std::uint32_t kSlot396FloatLoBits = 0x00000000;
inline constexpr std::uint32_t kSlot396FloatHiBits = 0x3f800000;
inline constexpr std::uint32_t kSlot396SliderLoBits = 0x3c23d70a;   // 0.01f at 0x126e888
inline constexpr std::uint32_t kSlot396SliderHiBits = 0x3dcccccd;   // 0.1f at 0x126e88c

inline constexpr std::size_t kSlot396BoolCount = 6;
inline constexpr char kSlot396BoolNames[kSlot396BoolCount][56] = {
    "shadowEnableRootFittingExpression", "shadowEnableRootFittingExpressionFKLimiting", "shadowEnableRootFitting",
    "shadow_BlendRootFitting", "cp10_drawRootFitting", "shadow_rootFittingEnableSelectiveRecomputeGlobal"};
inline constexpr std::size_t kSlot396BoolNameLengths[kSlot396BoolCount] = {0x21, 0x2b, 0x17, 0x17, 0x14, 0x30};
inline constexpr std::uint8_t kSlot396BoolValues[kSlot396BoolCount] = {1, 1, 1, 0, 0, 1};

inline constexpr std::size_t kSlot396FloatCount = 2;
inline constexpr char kSlot396FloatNames[kSlot396FloatCount][40] = {"cp10_rootFittingExpressionUserFKDampen",
                                                                      "cp10_rootFittingAltBoneDampenFactor"};
inline constexpr std::size_t kSlot396FloatNameLengths[kSlot396FloatCount] = {0x26, 0x23};
inline constexpr std::uint32_t kSlot396FloatValueBits[kSlot396FloatCount] = {0x3f000000, 0x3f400000};

inline constexpr std::uint64_t kSlot396Bool0Rva = 0x3010280;

struct Slot396Objects {
    ConCmdBoolVar bvar0;      // 0x3010280
    DebugMenuItem bitem0;     // 0x3010348
    ConCmdBoolVar bvar1;      // 0x30103f0
    DebugMenuItem bitem1;     // 0x30104b8
    ConCmdBoolVar bvar2;      // 0x3010560
    DebugMenuItem bitem2;     // 0x3010628
    ConCmdBoolVar bvar3;      // 0x30106d0
    DebugMenuItem bitem3;     // 0x3010798 (0x26ea0)
    ConCmdBoolVar bvar4;      // 0x3010840
    DebugMenuItem bitem4;     // 0x3010908
    ConCmdRangedVar fvar0;    // 0x30109b0
    DebugMenuItem fitem0;     // 0x3010a88
    ConCmdRangedVar fvar1;    // 0x3010b30
    DebugMenuItem fitem1;     // 0x3010c08
    ConCmdBoolVar bvar5;      // 0x3010cb0
    DebugMenuItem bitem5;     // 0x3010d78
};

struct Slot396Result {
    bool ok = false;
    BoolVarResult bvar[kSlot396BoolCount]{};
    MenuSiteResult bitem[kSlot396BoolCount]{};
    FloatVarResult fvar[kSlot396FloatCount]{};
    MenuSiteResult fitem[kSlot396FloatCount]{};
};

[[nodiscard]] Slot396Result init_slot396_650f20(Slot396Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
