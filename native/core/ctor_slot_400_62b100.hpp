// CPU-01-STARTUP-BATCH-8: native model of startup constructor slot 400 (RVA 0x62b100, 1624 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 400. Every body fact below is byte-pinned by tools/lift/check_ctor_slot_400.py (the menu sites by the shared
// 0x26db0 / 0x268b0 checks). NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS,
// recorded (the atexit log) and never run.
//
// Shape, in order (bool = static/literal shape, caller functor tag 0x15cc448, value = default; toggle = 0x26db0 over that bool, item flags 0x30, text = the variable name):
//   bool   shadow_ikEnableSelectiveGazeAddMotion      (0x300e048, 1)
//   bool   cp10_enableSelectiveAddMotion              (0x300e110, 1)
//   bool   cp10_enableGazeAddMotion                   (0x300e1d8, 1)
//   toggle for bool 0 under `Shadow IK` (0x300e2a0)
//   toggle for bool 1 under `CP10-Anim` (0x300e348)
//   toggle for bool 2 under `CP10-Anim` (0x300e3f0)
//   bool   cp10_enableAdditiveAnims                   (0x300e498, 1)
//   bool   cp10_enableDrivenShareRatios               (0x300e560, 1)
//   toggle for bool 3 under `CP10-Anim` (0x300e628)
//   toggle for bool 4 under `CP10-Anim` (0x300e6d0)
// NOT established: what reads the variables at run time (IK / additive-motion switches is INFERENCE from the strings).
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot400Rva = 0x62b100;
inline constexpr std::size_t kSlot400Size = 1624;
inline constexpr std::uint64_t kSlot400BoolFnTag = 0x15cc448;
inline constexpr std::uint32_t kSlot400ItemFlags = 0x30;

inline constexpr std::size_t kSlot400BoolCount = 5;
inline constexpr char kSlot400BoolNames[kSlot400BoolCount][38] = {"shadow_ikEnableSelectiveGazeAddMotion", "cp10_enableSelectiveAddMotion", "cp10_enableGazeAddMotion", "cp10_enableAdditiveAnims", "cp10_enableDrivenShareRatios"};
inline constexpr std::size_t kSlot400BoolNameLengths[kSlot400BoolCount] = {0x25, 0x1d, 0x18, 0x18, 0x1c};
inline constexpr std::uint8_t kSlot400BoolValues[kSlot400BoolCount] = {1, 1, 1, 1, 1};
inline constexpr char kSlot400Parents[kSlot400BoolCount][10] = {"Shadow IK", "CP10-Anim", "CP10-Anim", "CP10-Anim", "CP10-Anim"};
inline constexpr std::size_t kSlot400ParentLengths[kSlot400BoolCount] = {0x9, 0x9, 0x9, 0x9, 0x9};

inline constexpr std::uint64_t kSlot400Obj0Rva = 0x300e048;

struct Slot400Objects {
    ConCmdBoolVar bvar0;   // 0x300e048
    ConCmdBoolVar bvar1;   // 0x300e110
    ConCmdBoolVar bvar2;   // 0x300e1d8
    DebugMenuItem bitem0;   // 0x300e2a0
    DebugMenuItem bitem1;   // 0x300e348
    DebugMenuItem bitem2;   // 0x300e3f0
    ConCmdBoolVar bvar3;   // 0x300e498
    ConCmdBoolVar bvar4;   // 0x300e560
    DebugMenuItem bitem3;   // 0x300e628
    DebugMenuItem bitem4;   // 0x300e6d0
};

struct Slot400Result {
    bool ok = false;
    BoolVarResult bvar[kSlot400BoolCount]{};
    MenuSiteResult bitem[kSlot400BoolCount]{};
};

[[nodiscard]] Slot400Result init_slot400_62b100(Slot400Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
