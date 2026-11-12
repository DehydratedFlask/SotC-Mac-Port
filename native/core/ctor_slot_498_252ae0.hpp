// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 498 (RVA 0x252ae0, 2379 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 498. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_498.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Debug page (inference from the strings): console variables, four menu items, a separator and a 24-record member-hash table.
// Rows, in constructor order (10 objects at 0x2eebd20..): 2 bool variables (caller functor tag 0x159c528, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// debug page batch 16: variables, four items, a separator and a member-hash table
//
// NOT established: what reads the variables at run time. Any reading of the names is INFERENCE only.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "ctor_slot_539_121940.hpp"   // NameHolder30
#include "string_var_257a0.hpp"
#include "startup_row_kinds.hpp"
#include "ctor_slot_family_items.hpp"
#include "fnv_cstr_161fb0.hpp"
#include "hashed_name_ctors.hpp"
#include "hashed_string_14c3a0.hpp"
#include "helper_620e10_local_int_var.hpp"
#include "helper_a81700_named_record.hpp"
#include "helper_int_var_sites.hpp"
#include "l2_slot_common.hpp"
#include "lane_m6_shapes.hpp"
#include "helper_498_member_hashes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot498Rva = 0x252ae0;
inline constexpr std::size_t kSlot498Size = 2379;
inline constexpr std::uint64_t kSlot498BoolFnTag = 0x159c528;
inline constexpr std::uint64_t kSlot498FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot498ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot498VarFlags = 1;
inline constexpr std::uint64_t kSlot498UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot498UFloatCount = 0;
inline constexpr std::size_t kSlot498BoolCount = 2;
inline constexpr std::size_t kSlot498FloatCount = 0;
inline constexpr std::size_t kSlot498RowCount = 10;
inline constexpr std::size_t kSlot498StringCount = 1;
inline constexpr std::uint64_t kSlot498IntFnTag = 0x159c548;
inline constexpr std::size_t kSlot498IntCount = 1;
inline constexpr std::size_t kSlot498LocalIntCount = 0;
inline constexpr std::size_t kSlot498SliderCount = 0;
inline constexpr std::uint64_t kSlot498UIntFnTag = 0x159c548;
inline constexpr std::size_t kSlot498UIntCount = 1;
inline constexpr std::size_t kSlot498CmdCount = 0;
inline constexpr std::size_t kSlot498StrVarCount = 0;
inline constexpr std::uint64_t kSlot498Object0Rva = 0x2eebd20;
inline constexpr std::size_t kSlot498CellCount = 1;
inline constexpr std::size_t kSlot498HolderCount = 0;
inline constexpr std::size_t kSlot498SentinelCount = 0;
inline constexpr std::size_t kSlot498MemberCount = 24;
inline constexpr char kSlot498BoolNames[kSlot498BoolCount][31] = {"enableOptimizePass", "showOptimizePassStats"};
inline constexpr std::size_t kSlot498BoolNameLengths[kSlot498BoolCount] = {0x12, 0x15};
inline constexpr std::uint8_t kSlot498BoolValues[kSlot498BoolCount] = {1, 0};
inline constexpr std::int32_t kSlot498BoolDesc[kSlot498BoolCount] = {-1, -1};
inline constexpr std::uint8_t kSlot498BoolEarly[kSlot498BoolCount] = {1, 1};
inline constexpr char kSlot498IntNames[kSlot498IntCount][31] = {"optimizeSubmissionMethod"};
inline constexpr std::size_t kSlot498IntNameLengths[kSlot498IntCount] = {0x18};
inline constexpr std::int32_t kSlot498IntMin[kSlot498IntCount] = {0};
inline constexpr std::int32_t kSlot498IntMax[kSlot498IntCount] = {1};
inline constexpr std::int32_t kSlot498IntValue[kSlot498IntCount] = {0};
inline constexpr char kSlot498UIntNames[kSlot498UIntCount][31] = {"showOptimizePassStatsThreshold"};
inline constexpr std::size_t kSlot498UIntNameLengths[kSlot498UIntCount] = {0x1e};
inline constexpr std::int32_t kSlot498UIntValue[kSlot498UIntCount] = {0};
inline constexpr std::uint8_t kSlot498UIntEarly[kSlot498UIntCount] = {1};
inline constexpr char kSlot498MenuStrings[kSlot498StringCount][9] = {"Optimize"};
inline constexpr std::size_t kSlot498MenuStringLengths[kSlot498StringCount] = {0x8};
inline constexpr std::uint32_t kSlot498MemberLoRva[kSlot498MemberCount] = {0x3295d0, 0x311530, 0x309b20, 0x373e70, 0x297310, 0x250060, 0x297b40, 0x36aca0, 0x381840, 0x298390, 0x36c920, 0x31fe70, 0x320730, 0x3b9440, 0x249bf0, 0x3a8570, 0x3a93f0, 0x3a8cb0, 0x2c0690, 0x2c1130, 0x2c1ae0, 0x3425e0, 0x2a23c0, 0x2a2c60};
inline constexpr std::uint32_t kSlot498MemberHiRva[kSlot498MemberCount] = {0x3295e0, 0x311540, 0x309b30, 0x373e80, 0x297320, 0x250070, 0x297b50, 0x36acb0, 0x381850, 0x2983a0, 0x36c930, 0x31fe80, 0x320740, 0x3b9450, 0x249c00, 0x3a8580, 0x3a9400, 0x3a8cc0, 0x2c06a0, 0x2c1140, 0x2c1af0, 0x3425f0, 0x2a23d0, 0x2a2c70};
inline constexpr std::uint32_t kSlot498MemberLo[kSlot498MemberCount] = {0x3afc714, 0x39d9cae8, 0xd1edf694, 0xaadb73f3, 0xaadb73f3, 0xaadb73f3, 0xaadb73f3, 0xaadb73f3, 0xaadb73f3, 0xaadb73f3, 0xaadb73f3, 0x3d2f4f61, 0x3d2f4f61, 0x3d2f4f61, 0x2a0f60ad, 0x51a4998d, 0x51a4998d, 0x51a4998d, 0xd5edd0d2, 0xd5edd0d2, 0xd5edd0d2, 0xec9e749b, 0xec9e749b, 0xec9e749b};
inline constexpr std::uint32_t kSlot498MemberHi[kSlot498MemberCount] = {0x30e40eda, 0x355373d, 0x51b85a57, 0x8cacdb54, 0xc19bcbce, 0xfa0e9428, 0x7664dc20, 0xb6429662, 0x69f98817, 0x1900d54d, 0x17a73db4, 0xbdff2f49, 0xf28988bd, 0x9f0d08fd, 0x525563b1, 0x30e40eda, 0xdd5981f, 0x525563b1, 0x30e40eda, 0xdd5981f, 0x525563b1, 0xf05050d6, 0x852af535, 0x413ce7a5};
inline constexpr std::uint32_t kSlot498MemberSize[kSlot498MemberCount] = {0x28, 0x8, 0x24, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x8, 0x8, 0x8, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x2, 0x2, 0x2};

// Member-hash block (helper_498_member_hashes): the original calls 24 pairs of six byte getters (`mov eax, imm32 ; ret`, lo at rva, hi at rva + 0x10) and stores {lo, hi, size} into 24 consecutive 12 byte records.
// What the hashes are is INFERENCE only; the model copies them and does not interpret them. The block is 0x120 bytes at 0x2eec3b0.
struct Slot498MemberBlock {
    std::uint32_t dw[3 * kSlot498MemberCount];
};
static_assert(sizeof(Slot498MemberBlock) == 12 * kSlot498MemberCount);

struct Slot498Objects {
    ConCmdBoolVar r00;   // 0x2eebd20 bool
    ConCmdBoolVar r01;   // 0x2eebde8 bool
    ConCmdRangedVar r02;   // 0x2eebeb0 uint
    ConCmdRangedVar r03;   // 0x2eebf88 int
    DebugMenuItem r04;   // 0x2eec060 tog
    DebugMenuItem r05;   // 0x2eec108 tog
    DebugMenuItem r06;   // 0x2eec1b0 tog
    DebugMenuItem r07;   // 0x2eec258 tog
    DebugMenuItem r08;   // 0x2eec300 sep
    std::uint8_t gap09[0x8];
    Slot498MemberBlock r09;   // 0x2eec3b0 gettable
    SharedStrCtorHooks separator_alloc{};   // HOST INPUT: the allocator behind 0xf1a0 ("MenuSeparator"; 0x12820 / 0x12af0 are not lifted); empty refuses the separator rows. See default_family_separator_alloc().
};

struct Slot498Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot498Result init_slot498_252ae0(Slot498Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
