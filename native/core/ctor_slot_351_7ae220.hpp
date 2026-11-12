// CPU-01-STARTUP-BATCH-8: native model of startup constructor slot 351 (RVA 0x7ae220, 2706 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 351. Every body fact below is byte-pinned by tools/lift/check_ctor_slot_351.py (the 0x27120 slider constructor by
// tools/lift/check_helper_27120.py). NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS,
// recorded (the atexit log) and never run.
//
// Shape, in order (menu parent / caption `Shadow Camera`, item flags 0x30):
//   bool   shadow_disableCameraAutoCenter                  (0x325ace0, 1)   tag 0x15ce220   (no toggle)
//   bool   shadow_retractLookAtFromCollision               (0x325ada8, 1)   tag 0x15ce220   (no toggle)
//   float  ...RetractSpringScaleTgt / Cur, ...InterpSpringScaleTgt / Cur, ...AutoRotateTowardPlayerThreshold, cameraColKeepNorm, cameraColKeepBoss
//          (0x325ae70 .. 0x325b380, stride 0xd8; bounded 0xc300, caller functor tag 0x15ce240; ranges/defaults in the tables below)
//   0x27120 sliders over the LAST TWO floats (0x325b458, 0x325b500; text = the variable's own name, range {0.01, 0.1} from .rodata 0x12869d8 / 0x12869dc)
// NOT established: what reads the variables at run time. The "shadow camera" reading from the strings is INFERENCE only.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "helper_int_var_sites.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot351Rva = 0x7ae220;
inline constexpr std::size_t kSlot351Size = 2706;
inline constexpr std::uint64_t kSlot351BoolFnTag = 0x15ce220;
inline constexpr std::uint64_t kSlot351FloatFnTag = 0x15ce240;
inline constexpr char kSlot351Parent[] = "Shadow Camera";
inline constexpr std::size_t kSlot351ParentLength = 0xd;
inline constexpr std::uint32_t kSlot351ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot351VarFlags = 1;
inline constexpr std::uint32_t kSlot351SliderLoBits = 0x3c23d70a;   // 0.01f at 0x12869d8
inline constexpr std::uint32_t kSlot351SliderHiBits = 0x3dcccccd;   // 0.1f at 0x12869dc

inline constexpr std::size_t kSlot351BoolCount = 2;
inline constexpr char kSlot351BoolNames[kSlot351BoolCount][40] = {"shadow_disableCameraAutoCenter", "shadow_retractLookAtFromCollision"};
inline constexpr std::size_t kSlot351BoolNameLengths[kSlot351BoolCount] = {0x1e, 0x21};
inline constexpr std::uint8_t kSlot351BoolValues[kSlot351BoolCount] = {1, 1};

inline constexpr std::size_t kSlot351FloatCount = 7;
inline constexpr char kSlot351FloatNames[kSlot351FloatCount][56] = {"shadow_cameraLookAtCollisionRetractSpringScaleTgt", "shadow_cameraLookAtCollisionRetractSpringScaleCur", "shadow_CameraLookAtCollidingInterpSpringScaleTgt", "shadow_CameraLookAtCollidingInterpSpringScaleCur", "shadow_CameraAutoRotateTowardPlayerThreshold", "shadow_cameraColKeepNorm", "shadow_cameraColKeepBoss"};
inline constexpr std::size_t kSlot351FloatNameLengths[kSlot351FloatCount] = {0x31, 0x31, 0x30, 0x30, 0x2c, 0x18, 0x18};
inline constexpr std::uint32_t kSlot351FloatLoBits[kSlot351FloatCount] = {0x3dcccccd, 0x3dcccccd, 0x3dcccccd, 0x3dcccccd, 0x0, 0x3dcccccd, 0x3dcccccd};
inline constexpr std::uint32_t kSlot351FloatHiBits[kSlot351FloatCount] = {0x41800000, 0x41800000, 0x41800000, 0x41800000, 0x43340000, 0x3f000000, 0x3f000000};
inline constexpr std::uint32_t kSlot351FloatValueBits[kSlot351FloatCount] = {0x40000000, 0x41000000, 0x41000000, 0x3f800000, 0x43070000, 0x3dcccccd, 0x3ecccccd};

inline constexpr std::uint64_t kSlot351Obj0Rva = 0x325ace0;

struct Slot351Objects {
    ConCmdBoolVar bvar0;       // 0x325ace0
    ConCmdBoolVar bvar1;       // 0x325ada8
    ConCmdRangedVar fvar0;     // 0x325ae70
    ConCmdRangedVar fvar1;     // 0x325af48
    ConCmdRangedVar fvar2;     // 0x325b020
    ConCmdRangedVar fvar3;     // 0x325b0f8
    ConCmdRangedVar fvar4;     // 0x325b1d0
    ConCmdRangedVar fvar5;     // 0x325b2a8
    ConCmdRangedVar fvar6;     // 0x325b380
    DebugMenuItem sitem0;      // 0x325b458 (slider over fvar5)
    DebugMenuItem sitem1;      // 0x325b500 (slider over fvar6)
};

struct Slot351Result {
    bool ok = false;
    BoolVarResult bvar[kSlot351BoolCount]{};
    FloatVarResult fvar[kSlot351FloatCount]{};
    MenuSiteResult sitem[2]{};
};

[[nodiscard]] Slot351Result init_slot351_7ae220(Slot351Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
