// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): native model of startup constructor slot 450 (RVA 0x4af4b0, 808 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 450. Every body fact below is
// byte-pinned by tools/lift/check_ctor_slot_450.py. NOTHING here executes, loads or calls an original RVA: vtables,
// functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: three unbounded float console variables (UI logo / main-menu timings). Variables, in order (object RVA, kind, caller functor tag, value):
//   v0  0x2efa178  float_early  tag 0x15c05f0  value 0x3f800000  `uiLogoFadeInFromWhiteTime`
//   v1  0x2efa250  float_early  tag 0x15c05f0  value 0x3fc00000  `uiLogoFadeOutTime`
//   v2  0x2efa328  float_early  tag 0x15c05f0  value 0x3f800000  `uiMainMenuStartDelay`
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

inline constexpr std::uint64_t kSlot450Rva = 0x4af4b0;
inline constexpr std::size_t kSlot450Size = 808;
inline constexpr char kSlot450V0Name[] = "uiLogoFadeInFromWhiteTime";
inline constexpr std::size_t kSlot450V0NameLength = 0x19;
inline constexpr std::size_t kSlot450V0DescriptionLength = 0x0;
inline constexpr std::uint64_t kSlot450V0FnTag = 0x15c05f0;
inline constexpr std::uint32_t kSlot450V0Value = 0x3f800000;
inline constexpr std::uint64_t kSlot450V0Rva = 0x2efa178;
inline constexpr char kSlot450V1Name[] = "uiLogoFadeOutTime";
inline constexpr std::size_t kSlot450V1NameLength = 0x11;
inline constexpr std::size_t kSlot450V1DescriptionLength = 0x0;
inline constexpr std::uint64_t kSlot450V1FnTag = 0x15c05f0;
inline constexpr std::uint32_t kSlot450V1Value = 0x3fc00000;
inline constexpr std::uint64_t kSlot450V1Rva = 0x2efa250;
inline constexpr char kSlot450V2Name[] = "uiMainMenuStartDelay";
inline constexpr std::size_t kSlot450V2NameLength = 0x14;
inline constexpr std::size_t kSlot450V2DescriptionLength = 0x0;
inline constexpr std::uint64_t kSlot450V2FnTag = 0x15c05f0;
inline constexpr std::uint32_t kSlot450V2Value = 0x3f800000;
inline constexpr std::uint64_t kSlot450V2Rva = 0x2efa328;

struct Slot450Objects {
    ConCmdRangedVar v0;   // 0x2efa178
    ConCmdRangedVar v1;   // 0x2efa250
    ConCmdRangedVar v2;   // 0x2efa328
};

struct Slot450Result {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;   // first float-wrapper refusal, if any
    HelperFloatSiteResult v0{};
    HelperFloatSiteResult v1{};
    HelperFloatSiteResult v2{};
};

[[nodiscard]] Slot450Result init_slot450_4af4b0(Slot450Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
