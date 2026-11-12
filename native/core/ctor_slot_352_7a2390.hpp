// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 352 (RVA 0x7a2390, 2290 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 352. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_352.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Camera debug page (menu-less): six bounded floats, one ranged int and one unbounded float (dolly spring, camera rewind, bow-on-horseback look-at scale; inference from the strings).
// Rows, in constructor order (8 objects at 0x325a5c8..): 0 bool variables (caller functor tag 0x0, dtor 0x20b0), 6 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x15ce1e0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// No menu rows: the page only registers console variables.
//
// NOT established: what reads the variables at run time. Any reading of the names is INFERENCE only.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "ctor_slot_539_121940.hpp"   // NameHolder30
#include "ctor_slot_family_items.hpp"
#include "fnv_cstr_161fb0.hpp"
#include "hashed_name_ctors.hpp"
#include "helper_int_var_sites.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot352Rva = 0x7a2390;
inline constexpr std::size_t kSlot352Size = 2290;
inline constexpr std::uint64_t kSlot352BoolFnTag = 0x0;
inline constexpr std::uint64_t kSlot352FloatFnTag = 0x15ce1e0;
inline constexpr std::uint32_t kSlot352ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot352VarFlags = 1;
inline constexpr std::uint64_t kSlot352UFloatFnTag = 0x15ce1e0;
inline constexpr std::size_t kSlot352UFloatCount = 1;
inline constexpr std::size_t kSlot352BoolCount = 0;
inline constexpr std::size_t kSlot352FloatCount = 6;
inline constexpr std::size_t kSlot352RowCount = 8;
inline constexpr std::size_t kSlot352StringCount = 0;
inline constexpr std::uint64_t kSlot352IntFnTag = 0x15ce200;
inline constexpr std::size_t kSlot352IntCount = 1;
inline constexpr std::size_t kSlot352SliderCount = 0;
inline constexpr std::uint64_t kSlot352Object0Rva = 0x325a5c8;
inline constexpr std::size_t kSlot352CellCount = 0;
inline constexpr std::size_t kSlot352HolderCount = 0;
inline constexpr char kSlot352FloatNames[kSlot352FloatCount][39] = {"nico_PhotoModeDollySpringCoe1", "nico_PhotoModeDollySpringCoe2", "nico_PhotoModeDollySpringCoeBlendSpeed", "shadow_cameraRewindSpeedScaleCoe1", "shadow_cameraRewindSpeedScaleCoe2", "shadow_cameraScaleDistToLookAt"};
inline constexpr std::size_t kSlot352FloatNameLengths[kSlot352FloatCount] = {0x1d, 0x1d, 0x26, 0x21, 0x21, 0x1e};
inline constexpr std::uint32_t kSlot352FloatLoBits[kSlot352FloatCount] = {0x3dcccccd, 0x3dcccccd, 0x3dcccccd, 0x3dcccccd, 0x3dcccccd, 0x3dcccccd};
inline constexpr std::uint32_t kSlot352FloatHiBits[kSlot352FloatCount] = {0x42200000, 0x42200000, 0x40800000, 0x41000000, 0x41000000, 0x41200000};
inline constexpr std::uint32_t kSlot352FloatValueBits[kSlot352FloatCount] = {0x41a00000, 0x41a00000, 0x40000000, 0x40800000, 0x40800000, 0x3f800000};
inline constexpr char kSlot352UFloatNames[kSlot352UFloatCount][39] = {"shadow_bowDistToLookAtScaleOnHorseback"};
inline constexpr std::size_t kSlot352UFloatNameLengths[kSlot352UFloatCount] = {0x26};
inline constexpr std::uint32_t kSlot352UFloatValueBits[kSlot352UFloatCount] = {0x3fe00000};
inline constexpr std::uint8_t kSlot352UFloatEarly[kSlot352UFloatCount] = {1};
inline constexpr std::int32_t kSlot352UFloatDesc[kSlot352UFloatCount] = {-1};
inline constexpr char kSlot352IntNames[kSlot352IntCount][39] = {"shadow_tightenCameraRewind"};
inline constexpr std::size_t kSlot352IntNameLengths[kSlot352IntCount] = {0x1a};
inline constexpr std::int32_t kSlot352IntMin[kSlot352IntCount] = {0};
inline constexpr std::int32_t kSlot352IntMax[kSlot352IntCount] = {2};
inline constexpr std::int32_t kSlot352IntValue[kSlot352IntCount] = {1};
inline constexpr char kSlot352MenuStrings[1][1] = {""};   // no strings in this slot
inline constexpr std::size_t kSlot352MenuStringLengths[1] = {0};

struct Slot352Objects {
    ConCmdRangedVar r00;   // 0x325a5c8 float
    ConCmdRangedVar r01;   // 0x325a6a0 float
    ConCmdRangedVar r02;   // 0x325a778 float
    ConCmdRangedVar r03;   // 0x325a850 int
    ConCmdRangedVar r04;   // 0x325a928 float
    ConCmdRangedVar r05;   // 0x325aa00 float
    ConCmdRangedVar r06;   // 0x325aad8 float
    ConCmdRangedVar r07;   // 0x325abb0 ufloat
};

struct Slot352Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot352Result init_slot352_7a2390(Slot352Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
