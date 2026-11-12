// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): native model of startup constructor slot 460 (RVA 0x41c340, 857 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 460. Every body fact below is
// byte-pinned by tools/lift/check_ctor_slot_460.py. NOTHING here executes, loads or calls an original RVA: vtables,
// functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: bool drawWaypointNode + two unbounded floats (waypoint debug draw). Variables, in order (object RVA, kind, caller functor tag, value):
//   v0  0x2ef7cb8  bool_static  tag 0x15b5750  value 0x0  `drawWaypointNode`
//   v1  0x2ef7d80  float_early  tag 0x15b5770  value 0x0  `drawWaypointSegment`
//   v2  0x2ef7e58  float_early  tag 0x15b5770  value 0x3f800000  `drawWaypointSample`
// Float rows go through the family wrapper helper_6aa00_unbounded_float (via helper_init_unbounded_float_var_site),
// which returns HelperRefusal::none only for a tag that is a member of the 68-table float family; bool rows are the
// lane M4/M6 bool-variable shapes. All strings are borrowed rodata views, so every 0xf590 release is an exact no-op.
// Meaning of the variables beyond their names is NOT established (INFERENCE only; no consumer is lifted).
// The __stack_chk_guard compare is a hook (not modelled).
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_vars.hpp"
#include "helper_int_var_sites.hpp"
#include "lane_m4_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot460Rva = 0x41c340;
inline constexpr std::size_t kSlot460Size = 857;
inline constexpr char kSlot460V0Name[] = "drawWaypointNode";
inline constexpr std::size_t kSlot460V0NameLength = 0x10;
inline constexpr std::size_t kSlot460V0DescriptionLength = 0x0;
inline constexpr std::uint64_t kSlot460V0FnTag = 0x15b5750;
inline constexpr std::uint32_t kSlot460V0Value = 0x0;
inline constexpr std::uint64_t kSlot460V0Rva = 0x2ef7cb8;
inline constexpr char kSlot460V1Name[] = "drawWaypointSegment";
inline constexpr std::size_t kSlot460V1NameLength = 0x13;
inline constexpr std::size_t kSlot460V1DescriptionLength = 0x0;
inline constexpr std::uint64_t kSlot460V1FnTag = 0x15b5770;
inline constexpr std::uint32_t kSlot460V1Value = 0x0;
inline constexpr std::uint64_t kSlot460V1Rva = 0x2ef7d80;
inline constexpr char kSlot460V2Name[] = "drawWaypointSample";
inline constexpr std::size_t kSlot460V2NameLength = 0x12;
inline constexpr std::size_t kSlot460V2DescriptionLength = 0x0;
inline constexpr std::uint64_t kSlot460V2FnTag = 0x15b5770;
inline constexpr std::uint32_t kSlot460V2Value = 0x3f800000;
inline constexpr std::uint64_t kSlot460V2Rva = 0x2ef7e58;

struct Slot460Objects {
    ConCmdBoolVar v0;   // 0x2ef7cb8
    ConCmdRangedVar v1;   // 0x2ef7d80
    ConCmdRangedVar v2;   // 0x2ef7e58
};

struct Slot460Result {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;   // first float-wrapper refusal, if any
    BoolVarResult v0{};
    HelperFloatSiteResult v1{};
    HelperFloatSiteResult v2{};
};

[[nodiscard]] Slot460Result init_slot460_41c340(Slot460Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
