// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): native model of startup constructor slot 442 (RVA 0x4e3760, 779 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 442. Every body fact below is
// byte-pinned by tools/lift/check_ctor_slot_442.py. NOTHING here executes, loads or calls an original RVA: vtables,
// functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: bool + float + bool grip-fill test variables. Variables, in order (object RVA, kind, caller functor tag, value):
//   v0  0x2efbc90  bool_late    tag 0x15c27d0  value 0x0  `cp10_testUI_testGripFillPercent`
//   v1  0x2efbd58  float_late   tag 0x15c27f0  value 0x3f800000  `cp10_testUI_testGripFillPercentSpeed`
//   v2  0x2efbe30  bool_late    tag 0x15c27d0  value 0x0  `cp10_testUI_testGripAttackCircle`
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

inline constexpr std::uint64_t kSlot442Rva = 0x4e3760;
inline constexpr std::size_t kSlot442Size = 779;
inline constexpr char kSlot442V0Name[] = "cp10_testUI_testGripFillPercent";
inline constexpr std::size_t kSlot442V0NameLength = 0x1f;
inline constexpr char kSlot442V0Description[] = "Automatically cycles through fill percentages.";
inline constexpr std::size_t kSlot442V0DescriptionLength = 0x2e;
inline constexpr std::uint64_t kSlot442V0FnTag = 0x15c27d0;
inline constexpr std::uint32_t kSlot442V0Value = 0x0;
inline constexpr std::uint64_t kSlot442V0Rva = 0x2efbc90;
inline constexpr char kSlot442V1Name[] = "cp10_testUI_testGripFillPercentSpeed";
inline constexpr std::size_t kSlot442V1NameLength = 0x24;
inline constexpr char kSlot442V1Description[] = "How fast to automatically cycle through fill percentages.";
inline constexpr std::size_t kSlot442V1DescriptionLength = 0x39;
inline constexpr std::uint64_t kSlot442V1FnTag = 0x15c27f0;
inline constexpr std::uint32_t kSlot442V1Value = 0x3f800000;
inline constexpr std::uint64_t kSlot442V1Rva = 0x2efbd58;
inline constexpr char kSlot442V2Name[] = "cp10_testUI_testGripAttackCircle";
inline constexpr std::size_t kSlot442V2NameLength = 0x20;
inline constexpr char kSlot442V2Description[] = "Automatically cycles through attack circle charging.";
inline constexpr std::size_t kSlot442V2DescriptionLength = 0x34;
inline constexpr std::uint64_t kSlot442V2FnTag = 0x15c27d0;
inline constexpr std::uint32_t kSlot442V2Value = 0x0;
inline constexpr std::uint64_t kSlot442V2Rva = 0x2efbe30;

struct Slot442Objects {
    ConCmdBoolVar v0;   // 0x2efbc90
    ConCmdRangedVar v1;   // 0x2efbd58
    ConCmdBoolVar v2;   // 0x2efbe30
};

struct Slot442Result {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;   // first float-wrapper refusal, if any
    BoolVarResult v0{};
    HelperFloatSiteResult v1{};
    BoolVarResult v2{};
};

[[nodiscard]] Slot442Result init_slot442_4e3760(Slot442Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
