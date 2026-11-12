// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 395 (RVA 0x65af60, 1525 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 395. Every fact below is byte-pinned by tools/lift/check_ctor_slot_395.py. NOTHING here executes,
// loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order:
//   bool  shadow_playerEnableFilterAverageSlope        (0x3010e40, value = default = 1, caller functor tag 0x15cc588)
//   float shadow_playerAverageSlopeFilterDeadZone      (0x3010f08, bounded {0, 5},  value = default = 3.0, tag 0x15cc5a8)
//   float shadow_playerAverageSlopeFilterSpringCoe1    (0x3010fe0, bounded {0, 10}, 3.0)
//   float shadow_playerAverageSlopeFilterSpringCoe2    (0x30110b8, bounded {0, 10}, 3.0)
//   four 0x26db0 toggles under the menu parent `Shadow Player` (flags 0x30), one per variable in the same order
//   (objects 0x3011190 / 0x3011238 / 0x30112e0 / 0x3011388). The three float toggles are the first 0x26db0 sites whose TARGET is a float
//   variable: the original routine is class-agnostic and reads the target's own virtual slots 3 / 6; the float class answers 1 / 3, so the
//   item range narrows to {0.1, 0.5} (see helper_26db0_named_item.hpp).
//
// NOT established: what reads the variables at run time. "Average slope filter" reading from the strings is INFERENCE only.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "helper_26db0_named_item.hpp"
#include "helper_int_var_sites.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot395Rva = 0x65af60;
inline constexpr std::size_t kSlot395Size = 1525;
inline constexpr std::uint64_t kSlot395BoolFnTag = 0x15cc588;
inline constexpr std::uint64_t kSlot395FloatFnTag = 0x15cc5a8;
inline constexpr char kSlot395Parent[] = "Shadow Player";
inline constexpr std::size_t kSlot395ParentLength = 0xd;
inline constexpr std::uint32_t kSlot395ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot395VarFlags = 1;
inline constexpr char kSlot395BoolName[] = "shadow_playerEnableFilterAverageSlope";
inline constexpr std::size_t kSlot395BoolNameLength = 0x25;
inline constexpr std::uint8_t kSlot395BoolValue = 1;

inline constexpr std::size_t kSlot395FloatCount = 3;
inline constexpr char kSlot395FloatNames[kSlot395FloatCount][48] = {"shadow_playerAverageSlopeFilterDeadZone",
                                                                      "shadow_playerAverageSlopeFilterSpringCoe1",
                                                                      "shadow_playerAverageSlopeFilterSpringCoe2"};
inline constexpr std::size_t kSlot395FloatNameLengths[kSlot395FloatCount] = {0x27, 0x29, 0x29};
inline constexpr std::uint32_t kSlot395FloatLoBits = 0x00000000;
inline constexpr std::uint32_t kSlot395FloatHiBits[kSlot395FloatCount] = {0x40a00000, 0x41200000, 0x41200000};   // 5.0, 10.0, 10.0
inline constexpr std::uint32_t kSlot395FloatValueBits = 0x40400000;                                              // 3.0 (value = default)

inline constexpr std::uint64_t kSlot395Bool0Rva = 0x3010e40;

struct Slot395Objects {
    ConCmdBoolVar bvar;        // 0x3010e40
    ConCmdRangedVar fvar0;     // 0x3010f08
    ConCmdRangedVar fvar1;     // 0x3010fe0
    ConCmdRangedVar fvar2;     // 0x30110b8
    DebugMenuItem bitem;       // 0x3011190 (target: the bool)
    DebugMenuItem fitem0;      // 0x3011238
    DebugMenuItem fitem1;      // 0x30112e0
    DebugMenuItem fitem2;      // 0x3011388
};

struct Slot395Result {
    bool ok = false;
    BoolVarResult bvar{};
    FloatVarResult fvar[kSlot395FloatCount]{};
    MenuSiteResult bitem{};
    MenuSiteResult fitem[kSlot395FloatCount]{};
};

// The first `0x26db0` site over a FLOAT target: 0x26db0(item, parent, flags 0x30), 0x268b0 (tail), 0xf590(parent), atexit(0x272d0, item).
// Refuses (item untouched) when the target is not the lifted float class.
[[nodiscard]] MenuSiteResult init_toggle_site_26db0_float(DebugMenuItem& item, const ConCmdRangedVar& target, const char* parent,
                                                          std::size_t parent_length, ConCmdRegistry& registry,
                                                          DebugMenuRegistry& menu) noexcept;

[[nodiscard]] Slot395Result init_slot395_65af60(Slot395Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
