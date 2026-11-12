// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 451 (RVA 0x49d230, 5126 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 451. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_451.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Debug page (inference from the strings): the cp10_testUI test console variables, two dword-ranged variables, two critical-section objects, a node-holder slot and one command.
// Rows, in constructor order (23 objects at 0x2ef8f38..): 8 bool variables (caller functor tag 0x15bfc10, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// debug page batch 17: test console variables, dword-ranged variables, critical-section objects and a node holder
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
#include "helper_49ca60_dword_var_site.hpp"
#include "critsec_3b310.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot451Rva = 0x49d230;
inline constexpr std::size_t kSlot451Size = 5126;
inline constexpr std::uint64_t kSlot451BoolFnTag = 0x15bfc10;
inline constexpr std::uint64_t kSlot451FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot451ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot451VarFlags = 1;
inline constexpr std::uint64_t kSlot451UFloatFnTag = 0x15bfc50;
inline constexpr std::size_t kSlot451UFloatCount = 4;
inline constexpr std::size_t kSlot451BoolCount = 8;
inline constexpr std::size_t kSlot451FloatCount = 0;
inline constexpr std::size_t kSlot451RowCount = 23;
inline constexpr std::size_t kSlot451StringCount = 18;
inline constexpr std::uint64_t kSlot451IntFnTag = 0x0;
inline constexpr std::size_t kSlot451IntCount = 0;
inline constexpr std::size_t kSlot451LocalIntCount = 0;
inline constexpr std::size_t kSlot451SliderCount = 0;
inline constexpr std::uint64_t kSlot451UIntFnTag = 0x15bfc30;
inline constexpr std::size_t kSlot451UIntCount = 5;
inline constexpr std::size_t kSlot451CmdCount = 1;
inline constexpr std::size_t kSlot451StrVarCount = 0;
inline constexpr std::uint64_t kSlot451Object0Rva = 0x2ef8f38;
inline constexpr std::size_t kSlot451DwrCount = 2;
inline constexpr std::size_t kSlot451CritCount = 2;
inline constexpr std::size_t kSlot451CritStoreCount = 15;
inline constexpr std::size_t kSlot451CellCount = 0;
inline constexpr std::size_t kSlot451HolderCount = 0;
inline constexpr std::size_t kSlot451SentinelCount = 0;
inline constexpr std::size_t kSlot451ZeroPtrCount = 1;
inline constexpr std::uint64_t kSlot451ZeroPtrDtorRva = 0x499e90;
inline constexpr char kSlot451BoolNames[kSlot451BoolCount][41] = {"cp10_testUI_simGameCompleted", "cp10_testUI_simSOTCPlayerHasAllItems", "cp10_testUI_photoModeEnabled", "cp10_testUI_photoModeUIEnabled", "cp10_testUI_simCanContinue", "cp10_testUI_simCanLoad", "cp10_testUI_showHUD", "cp10_testUI_showTutorial"};
inline constexpr std::size_t kSlot451BoolNameLengths[kSlot451BoolCount] = {0x1c, 0x24, 0x1c, 0x1e, 0x1a, 0x16, 0x13, 0x18};
inline constexpr std::uint8_t kSlot451BoolValues[kSlot451BoolCount] = {0, 0, 0, 1, 0, 0, 1, 1};
inline constexpr std::int32_t kSlot451BoolDesc[kSlot451BoolCount] = {0, 1, 11, 12, 14, 14, 15, 16};
inline constexpr std::uint8_t kSlot451BoolEarly[kSlot451BoolCount] = {0, 0, 0, 0, 0, 0, 0, 0};
inline constexpr char kSlot451UFloatNames[kSlot451UFloatCount][41] = {"cp10_testUI_playerHealth_UpgradeRatio", "cp10_testUI_playerGrip_UpgradeRatio", "cp10_testUI_grip", "cp10_testUI_skipPercent"};
inline constexpr std::size_t kSlot451UFloatNameLengths[kSlot451UFloatCount] = {0x25, 0x23, 0x10, 0x17};
inline constexpr std::uint32_t kSlot451UFloatValueBits[kSlot451UFloatCount] = {0x3f800000, 0x3f800000, 0x3f800000, 0x00000000};
inline constexpr std::uint8_t kSlot451UFloatEarly[kSlot451UFloatCount] = {0, 0, 0, 1};
inline constexpr std::int32_t kSlot451UFloatDesc[kSlot451UFloatCount] = {6, 7, 10, -1};
inline constexpr char kSlot451UIntNames[kSlot451UIntCount][41] = {"cp10_testUI_simSOTCPlayerHasWhistleArrow", "cp10_testUI_simSOTCPlayerHasLizardDetect", "cp10_testUI_simSOTCPlayerHasFruitMap", "cp10_testUI_playerHealth", "cp10_testUI_bossHealth"};
inline constexpr std::size_t kSlot451UIntNameLengths[kSlot451UIntCount] = {0x28, 0x28, 0x24, 0x18, 0x16};
inline constexpr std::int32_t kSlot451UIntValue[kSlot451UIntCount] = {1, 1, 1, 100, 100};
inline constexpr std::uint8_t kSlot451UIntEarly[kSlot451UIntCount] = {0, 0, 0, 0, 0};
inline constexpr std::int32_t kSlot451UIntDesc[kSlot451UIntCount] = {2, 3, 4, 5, 8};
// Dword-ranged variables (0x49ca60 / 0x49cc60): per-site name, description (borrowed view), flags, the REAL argument-functor tag (the constructor's own class), the dword range, the initial value and default,
// the final vtable pointer (a literal lea of the site) and the atexit destructor (identity tag). Variant 0 = 0x49ca60, 1 = 0x49cc60.
inline constexpr char kSlot451DwrNames[kSlot451DwrCount][41] = {"cp10_testUI_curWeapon", "cp10_testUI_photoModeSelection"};
inline constexpr std::size_t kSlot451DwrNameLengths[kSlot451DwrCount] = {0x15, 0x1e};
inline constexpr std::uint8_t kSlot451DwrVariant[kSlot451DwrCount] = {0, 1};
inline constexpr std::int32_t kSlot451DwrDesc[kSlot451DwrCount] = {9, 13};
inline constexpr std::uint64_t kSlot451DwrFnTag[kSlot451DwrCount] = {0x15bfce8, 0x15bfd80};
inline constexpr std::int32_t kSlot451DwrMin[kSlot451DwrCount] = {0, 0};
inline constexpr std::int32_t kSlot451DwrMax[kSlot451DwrCount] = {10, 4};
inline constexpr std::int32_t kSlot451DwrValue[kSlot451DwrCount] = {1, 0};
inline constexpr std::int32_t kSlot451DwrDefault[kSlot451DwrCount] = {1, 0};
inline constexpr std::uint64_t kSlot451DwrFinalVptr[kSlot451DwrCount] = {0x15bfc80, 0x15bfd18};
inline constexpr std::uint64_t kSlot451DwrDtorRva[kSlot451DwrCount] = {0x498b20, 0x498b60};
// Critical-section objects (0x3b310 at +0 -> the 8-byte opaque handle, then the observed field stores in order; 0x499e50 / 0x499e60 are atexit destructors, IDENTITY TAGS). Stores are flattened per row:
// offset, width and value (a store wider than 8 bytes is an all-zero vector store). Nothing else in the object is written.
inline constexpr std::uint32_t kSlot451CritSize[kSlot451CritCount] = {0xd8, 0x30};
inline constexpr std::uint64_t kSlot451CritDtorRva[kSlot451CritCount] = {0x499e50, 0x499e60};
inline constexpr std::size_t kSlot451CritStoreFirst[kSlot451CritCount + 1] = {0, 12, 15};
inline constexpr std::uint32_t kSlot451CritStoreOffset[kSlot451CritStoreCount] = {0x8, 0x28, 0x38, 0x48, 0x58, 0x68, 0x78, 0x88, 0x98, 0xa8, 0xb8, 0xc8, 0x8, 0x18, 0x28};
inline constexpr std::uint32_t kSlot451CritStoreWidth[kSlot451CritStoreCount] = {32, 8, 16, 8, 16, 8, 16, 8, 16, 8, 16, 8, 16, 8, 4};
inline constexpr std::uint64_t kSlot451CritStoreValue[kSlot451CritStoreCount] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3f800000};
inline constexpr char kSlot451CmdNames[kSlot451CmdCount][41] = {"cp10_testUI_setCaption"};
inline constexpr std::size_t kSlot451CmdNameLengths[kSlot451CmdCount] = {0x16};
inline constexpr std::int32_t kSlot451CmdDesc[kSlot451CmdCount] = {17};
inline constexpr std::uint64_t kSlot451CmdHandler[kSlot451CmdCount] = {0x499ee0};
inline constexpr char kSlot451MenuStrings[kSlot451StringCount][156] = {"Indicates that the game is completed for UI purposes. Ignored unless the preview executable is running (not ICO or Shadow).", "Indicates that the player has all of the special items. Ignored unless the preview executable is running (not ICO or Shadow).", "Indicates that the player has the whistling arrow item. Ignored unless the preview executable is running (not ICO or Shadow).", "Indicates that the player has the lizard detection stone. Ignored unless the preview executable is running (not ICO or Shadow).", "Indicates that the player has the fruit tree map. Ignored unless the preview executable is running (not ICO or Shadow).", "Player health for test purposes. Ignored unless the preview executable is running (not ICO or Shadow).", "Health upgrades for test purposes. Ignored unless the preview executable is running (not ICO or Shadow).", "Grip upgrades for test purposes. Ignored unless the preview executable is running (not ICO or Shadow).", "Boss health for test purposes. Ignored unless the preview executable is running (not ICO or Shadow).", "Current weapon for test purposes. Ignored unless the preview executable is running (not ICO or Shadow).", "Grip for test purposes. Ignored unless the preview executable is running (not ICO or Shadow).", "Indicates that photo mode is enabled. Ignored unless the preview executable is running (not ICO or Shadow).", "Indicates that within photo mode the UI is enabled. Ignored unless the preview executable is running (not ICO or Shadow).", "Photo mode selection for test purposes. Ignored unless the preview executable is running (not ICO or Shadow).", "Indicates that the game has a continue slot. Ignored unless the preview executable is running (not ICO or Shadow).", "Indicates that the game has the hud enabled. Ignored unless the preview executable is running (not ICO or Shadow).", "Indicates that the game tutorials enabled. Ignored unless the preview executable is running (not ICO or Shadow).", "Sets the localized text for the caption area for test purposes. Ignored unless the preview executable is running (not ICO or Shadow). Params: <locstringid>"};
inline constexpr std::size_t kSlot451MenuStringLengths[kSlot451StringCount] = {0x7b, 0x7d, 0x7d, 0x7f, 0x77, 0x66, 0x68, 0x66, 0x64, 0x67, 0x5d, 0x6b, 0x79, 0x6d, 0x72, 0x72, 0x70, 0x9b};

// Opaque bytes of a critical-section object: only the 8 byte handle at +0 (0x3b310) and the listed stores are ever written; the rest is never touched.
template <std::size_t N>
struct Slot451Crit {
    alignas(8) std::uint8_t b[N];
};

struct Slot451Objects {
    ConCmdBoolVar r00;   // 0x2ef8f38 bool
    ConCmdBoolVar r01;   // 0x2ef9000 bool
    ConCmdRangedVar r02;   // 0x2ef90c8 uint
    ConCmdRangedVar r03;   // 0x2ef91a0 uint
    ConCmdRangedVar r04;   // 0x2ef9278 uint
    ConCmdRangedVar r05;   // 0x2ef9350 uint
    ConCmdRangedVar r06;   // 0x2ef9428 ufloat
    ConCmdRangedVar r07;   // 0x2ef9500 ufloat
    ConCmdRangedVar r08;   // 0x2ef95d8 uint
    ConCmdRangedVar r09;   // 0x2ef96b0 dwr
    ConCmdRangedVar r10;   // 0x2ef9788 ufloat
    ConCmdBoolVar r11;   // 0x2ef9860 bool
    ConCmdBoolVar r12;   // 0x2ef9928 bool
    ConCmdRangedVar r13;   // 0x2ef99f0 dwr
    ConCmdBoolVar r14;   // 0x2ef9ac8 bool
    ConCmdBoolVar r15;   // 0x2ef9b90 bool
    ConCmdBoolVar r16;   // 0x2ef9c58 bool
    ConCmdBoolVar r17;   // 0x2ef9d20 bool
    ConCmdRangedVar r18;   // 0x2ef9de8 ufloat
    Slot451Crit<0xd8> r19;   // 0x2ef9ec0 crit
    Slot451Crit<0x30> r20;   // 0x2ef9f98 crit
    std::uint64_t r21;   // 0x2ef9fc8 zq
    ConCmdCommand r22;   // 0x2ef9fd0 cmd
};

struct Slot451Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot451Result init_slot451_49d230(Slot451Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
