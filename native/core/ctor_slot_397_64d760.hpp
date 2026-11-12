// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): native model of startup constructor slot 397 (RVA 0x64d760, 1036 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 397. Every body fact below is
// byte-pinned by tools/lift/check_ctor_slot_397.py. NOTHING here executes, loads or calls an original RVA: vtables,
// functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: four unbounded float console variables (player acceleration scalars). Variables, in order (object RVA, kind, caller functor tag, value):
//   v0  0x300fea8  float_early  tag 0x15cc528  value 0x3f400000  `playerAccelerationScalar30Hz`
//   v1  0x300ff80  float_early  tag 0x15cc528  value 0x3f2b851f  `playerAccelerationScalarEasyMode30Hz`
//   v2  0x3010058  float_early  tag 0x15cc528  value 0x3f11eb85  `playerAccelerationScalar60Hz`
//   v3  0x3010130  float_early  tag 0x15cc528  value 0x3f11eb85  `playerAccelerationScalarEasyMode60Hz`
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

inline constexpr std::uint64_t kSlot397Rva = 0x64d760;
inline constexpr std::size_t kSlot397Size = 1036;
inline constexpr char kSlot397V0Name[] = "playerAccelerationScalar30Hz";
inline constexpr std::size_t kSlot397V0NameLength = 0x1c;
inline constexpr std::size_t kSlot397V0DescriptionLength = 0x0;
inline constexpr std::uint64_t kSlot397V0FnTag = 0x15cc528;
inline constexpr std::uint32_t kSlot397V0Value = 0x3f400000;
inline constexpr std::uint64_t kSlot397V0Rva = 0x300fea8;
inline constexpr char kSlot397V1Name[] = "playerAccelerationScalarEasyMode30Hz";
inline constexpr std::size_t kSlot397V1NameLength = 0x24;
inline constexpr std::size_t kSlot397V1DescriptionLength = 0x0;
inline constexpr std::uint64_t kSlot397V1FnTag = 0x15cc528;
inline constexpr std::uint32_t kSlot397V1Value = 0x3f2b851f;
inline constexpr std::uint64_t kSlot397V1Rva = 0x300ff80;
inline constexpr char kSlot397V2Name[] = "playerAccelerationScalar60Hz";
inline constexpr std::size_t kSlot397V2NameLength = 0x1c;
inline constexpr std::size_t kSlot397V2DescriptionLength = 0x0;
inline constexpr std::uint64_t kSlot397V2FnTag = 0x15cc528;
inline constexpr std::uint32_t kSlot397V2Value = 0x3f11eb85;
inline constexpr std::uint64_t kSlot397V2Rva = 0x3010058;
inline constexpr char kSlot397V3Name[] = "playerAccelerationScalarEasyMode60Hz";
inline constexpr std::size_t kSlot397V3NameLength = 0x24;
inline constexpr std::size_t kSlot397V3DescriptionLength = 0x0;
inline constexpr std::uint64_t kSlot397V3FnTag = 0x15cc528;
inline constexpr std::uint32_t kSlot397V3Value = 0x3f11eb85;
inline constexpr std::uint64_t kSlot397V3Rva = 0x3010130;

struct Slot397Objects {
    ConCmdRangedVar v0;   // 0x300fea8
    ConCmdRangedVar v1;   // 0x300ff80
    ConCmdRangedVar v2;   // 0x3010058
    ConCmdRangedVar v3;   // 0x3010130
};

struct Slot397Result {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;   // first float-wrapper refusal, if any
    HelperFloatSiteResult v0{};
    HelperFloatSiteResult v1{};
    HelperFloatSiteResult v2{};
    HelperFloatSiteResult v3{};
};

[[nodiscard]] Slot397Result init_slot397_64d760(Slot397Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
