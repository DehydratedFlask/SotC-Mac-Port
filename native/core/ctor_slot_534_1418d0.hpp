// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 534 (RVA 0x1418d0, 6207 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 534. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_534.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Debug page (inference from the strings): texture streaming variables, one unbounded inline enum, a zero cell and two commands.
// Rows, in constructor order (24 objects at 0x2ee1530..): 6 bool variables (caller functor tag 0x1598330, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// debug page batch 17: texture streaming variables, an unbounded inline enum, a zero cell and two commands
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
#include "helper_140b00_twin_site.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot534Rva = 0x1418d0;
inline constexpr std::size_t kSlot534Size = 6207;
inline constexpr std::uint64_t kSlot534BoolFnTag = 0x1598330;
inline constexpr std::uint64_t kSlot534FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot534ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot534VarFlags = 1;
inline constexpr std::uint64_t kSlot534UFloatFnTag = 0x1598408;
inline constexpr std::size_t kSlot534UFloatCount = 2;
inline constexpr std::size_t kSlot534BoolCount = 6;
inline constexpr std::size_t kSlot534FloatCount = 0;
inline constexpr std::size_t kSlot534RowCount = 24;
inline constexpr std::size_t kSlot534StringCount = 5;
inline constexpr std::uint64_t kSlot534IntFnTag = 0x15983e8;
inline constexpr std::size_t kSlot534IntCount = 1;
inline constexpr std::size_t kSlot534LocalIntCount = 0;
inline constexpr std::size_t kSlot534SliderCount = 0;
inline constexpr std::uint64_t kSlot534UIntTwinFinalVptr = 0x1598360;
inline constexpr std::uint64_t kSlot534UIntTwinDtorRva = 0x13a090;
inline constexpr std::uint64_t kSlot534UIntFnTag = 0x15983e8;
inline constexpr std::size_t kSlot534UIntCount = 11;
inline constexpr std::size_t kSlot534CmdCount = 2;
inline constexpr std::size_t kSlot534StrVarCount = 0;
inline constexpr std::uint64_t kSlot534Object0Rva = 0x2ee1530;
inline constexpr std::size_t kSlot534CellCount = 0;
inline constexpr std::size_t kSlot534HolderCount = 0;
inline constexpr std::size_t kSlot534SentinelCount = 0;
inline constexpr std::size_t kSlot534EnumCount = 1;
inline constexpr std::uint64_t kSlot534EnumMidVptr = 0x15982c8;
inline constexpr std::uint64_t kSlot534EnumFinalVptr = 0x1598438;
inline constexpr std::uint64_t kSlot534EnumFnTag = 0x15984a0;
inline constexpr std::uint64_t kSlot534EnumDtorRva = 0x140010;
inline constexpr std::uint32_t kSlot534EnumBounded = 0;
inline constexpr std::uint32_t kSlot534EnumSlot3Result = 1;
inline constexpr std::uint32_t kSlot534EnumSlot6Result = 1;
inline constexpr std::size_t kSlot534ZeroCellCount = 1;
inline constexpr std::uint64_t kSlot534ZeroCellDtorRva = 0x13a0d0;
inline constexpr std::size_t kSlot534ZeroCellWords = 1;
inline constexpr char kSlot534BoolNames[kSlot534BoolCount][34] = {"enableTextureStreaming", "showTextures", "overlayTextureNames", "overlayTextureStats", "showTexturePools", "cacheUnusedTextures"};
inline constexpr std::size_t kSlot534BoolNameLengths[kSlot534BoolCount] = {0x16, 0xc, 0x13, 0x13, 0x10, 0x13};
inline constexpr std::uint8_t kSlot534BoolValues[kSlot534BoolCount] = {1, 1, 1, 1, 0, 1};
inline constexpr std::int32_t kSlot534BoolDesc[kSlot534BoolCount] = {-1, -1, 0, 1, -1, -1};
inline constexpr std::uint8_t kSlot534BoolEarly[kSlot534BoolCount] = {1, 1, 0, 0, 1, 1};
inline constexpr char kSlot534UFloatNames[kSlot534UFloatCount][34] = {"overlayTextScale", "showTextureScale"};
inline constexpr std::size_t kSlot534UFloatNameLengths[kSlot534UFloatCount] = {0x10, 0x10};
inline constexpr std::uint32_t kSlot534UFloatValueBits[kSlot534UFloatCount] = {0x3f266666, 0x3f800000};
inline constexpr std::uint8_t kSlot534UFloatEarly[kSlot534UFloatCount] = {0, 0};
inline constexpr std::int32_t kSlot534UFloatDesc[kSlot534UFloatCount] = {2, 2};
inline constexpr char kSlot534IntNames[kSlot534IntCount][34] = {"numMipRequestsPerFrame"};
inline constexpr std::size_t kSlot534IntNameLengths[kSlot534IntCount] = {0x16};
inline constexpr std::int32_t kSlot534IntMin[kSlot534IntCount] = {1};
inline constexpr std::int32_t kSlot534IntMax[kSlot534IntCount] = {256};
inline constexpr std::int32_t kSlot534IntValue[kSlot534IntCount] = {256};
inline constexpr char kSlot534EnumNames[kSlot534EnumCount][34] = {"textureUnloadHighMode"};
inline constexpr std::size_t kSlot534EnumNameLengths[kSlot534EnumCount] = {0x15};
inline constexpr std::int32_t kSlot534EnumMin[kSlot534EnumCount] = {0};
inline constexpr std::int32_t kSlot534EnumMax[kSlot534EnumCount] = {0};
inline constexpr std::int32_t kSlot534EnumValue[kSlot534EnumCount] = {2};
inline constexpr char kSlot534UIntNames[kSlot534UIntCount][34] = {"showTexturesMode", "ForceSortMode", "showTexturePageIndex", "watchTextureId", "showTextureStats", "texturePoolBaseSizeMiBs", "texturePoolSizeMiBs", "texturePoolReserveSystemMemMiBs", "texturePoolLowMemRecoverySizeMiBs", "textureMinUnusedFramesForUnload", "textureMaxLoadsInFlight"};
inline constexpr std::size_t kSlot534UIntNameLengths[kSlot534UIntCount] = {0x10, 0xd, 0x14, 0xe, 0x10, 0x17, 0x13, 0x1f, 0x21, 0x1f, 0x17};
inline constexpr std::int32_t kSlot534UIntValue[kSlot534UIntCount] = {0, 0, 0, -1, 0, 1024, 5120, 220, 0, 5, 4};
inline constexpr std::uint8_t kSlot534UIntEarly[kSlot534UIntCount] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
inline constexpr std::uint8_t kSlot534UIntTwin[kSlot534UIntCount] = {1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0};
inline constexpr char kSlot534CmdNames[kSlot534CmdCount][34] = {"tx_unload_high", "tx_dump_stats"};
inline constexpr std::size_t kSlot534CmdNameLengths[kSlot534CmdCount] = {0xe, 0xd};
inline constexpr std::int32_t kSlot534CmdDesc[kSlot534CmdCount] = {3, 4};
inline constexpr std::uint64_t kSlot534CmdHandler[kSlot534CmdCount] = {0x13ef40, 0x13ef60};
inline constexpr char kSlot534MenuStrings[kSlot534StringCount][55] = {"Overlays each texture's name when showTextures==true.", "Overlays each texture's stats when showTextures==true.", "The text scale for overlays.", "Unload high mips", "Dump texture stats"};
inline constexpr std::size_t kSlot534MenuStringLengths[kSlot534StringCount] = {0x35, 0x36, 0x1c, 0x10, 0x12};

// Zero cell (0x13a0d0 is its atexit destructor, an IDENTITY TAG; the original deletes through the pointer at +0 and CLEARS the cell): the original zeroes the qword at +0 and nothing else.
struct Slot534ZeroCell {
    std::uint64_t q00;   // = 0
};
static_assert(sizeof(Slot534ZeroCell) == 0x8);

struct Slot534Objects {
    ConCmdBoolVar r00;   // 0x2ee1530 bool
    ConCmdBoolVar r01;   // 0x2ee15f8 bool
    ConCmdRangedVar r02;   // 0x2ee16c0 uint
    ConCmdRangedVar r03;   // 0x2ee1798 uint
    ConCmdRangedVar r04;   // 0x2ee1870 uint
    ConCmdBoolVar r05;   // 0x2ee1948 bool
    ConCmdBoolVar r06;   // 0x2ee1a10 bool
    ConCmdRangedVar r07;   // 0x2ee1ad8 ufloat
    ConCmdRangedVar r08;   // 0x2ee1bb0 ufloat
    ConCmdRangedVar r09;   // 0x2ee1c88 uint
    ConCmdRangedVar r10;   // 0x2ee1d60 uint
    ConCmdBoolVar r11;   // 0x2ee1e38 bool
    ConCmdBoolVar r12;   // 0x2ee1f00 bool
    ConCmdRangedVar r13;   // 0x2ee1fc8 int
    ConCmdRangedVar r14;   // 0x2ee20a0 uint
    ConCmdRangedVar r15;   // 0x2ee2178 uint
    ConCmdRangedVar r16;   // 0x2ee2250 uint
    ConCmdRangedVar r17;   // 0x2ee2328 uint
    ConCmdRangedVar r18;   // 0x2ee2400 uint
    ConCmdRangedVar r19;   // 0x2ee24d8 uint
    ConCmdRangedVar r20;   // 0x2ee25b0 ienum
    Slot534ZeroCell r21;   // 0x2ee2688 zcell
    std::uint8_t gap22[0x18];
    ConCmdCommand r22;   // 0x2ee26a8 cmd
    ConCmdCommand r23;   // 0x2ee2748 cmd
};

struct Slot534Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot534Result init_slot534_1418d0(Slot534Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
