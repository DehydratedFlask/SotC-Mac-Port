// CPU-01-STARTUP-SLOT-497: native model of startup constructor index 497 / RVA 0x262a80.
//
// Original ELF sha256 d0723afb...5861d3. Body [0x262a80, 0x264e50), 0x23d0 B (9,168); the last real
// instruction is `call __stack_chk_fail` at 0x264e46, followed by five 0x90 alignment bytes. Body
// sha256 and every operand are pinned by tools/lift/check_ctor_slot_497.py. Constructor-table word
// 0x2dd70e0 - 8 * 497 = 0x2dd6158 holds 0x262a80. Offline only: NO original RVA is executed.
//
// Body facts (read end to end from the pinned bytes; every one is oracle-pinned):
//   * 1,742 real instructions, 225 direct calls, 7 indirect calls (`call *0x18(%rax)` x7), 10
//     branches (6 je, 3 jmp, 1 jne), one stack canary (0x15fe518) and the `call __stack_chk_fail`
//     tail at 0x264e46.
//   * direct calls: 87 x 0xf590 (borrowed view release), 54 x 0xf8f390 (__cxa_atexit),
//     20 x 0x20d60 (register), 15 x 0x20af0 (ConCmd base ctor), 10 x 0x26ea0 (toggle item),
//     7 x 0xa81700 (named-record registrar), 4 x 0xf2f0 (view from a literal), 4 x 0xf730 (empty),
//     3 x 0x26800 (label item), 3 x 0x26c10 (group item), 3 x 0x4b560 (entry registration),
//     2 x 0x6aa00 (unbounded float), 2 x 0x257a0 (string variable), 2 x 0x26b60 (separator),
//     1 each 0x8d650 (cell = 1), 0xc580 (ranged int), 0xc300 (bounded float), 0x3a980 (unbounded
//     int), 0x261b50 / 0x261f10 (local ranged int), 0x27050 (slider item), 0x323d0 (TSC store),
//     0xf8f3a0 (__stack_chk_fail).
//   * indirect calls: seven `call *0x18(%rax)` argument-functor destroy slots, all bare `ret`
//     (0x159c758-family tables); they are modelled as the caller-tag validation, never executed.
//
// ================================ page map (verified offsets) ================================
//   +0x0000  0x008  cell 0x2eec4d8: `*cell = 1` (0x8d650) writes four bytes; +4..+8 stay holes
//   +0x0008  0x020  page header 0x2eec4e0: {0x780, 0x438, 0, hole, qword 0, -1.0f, hole}
//   +0x0028  0x020  view 0x2eec500: `Bluepoint Engine` (0xf2f0, length 0)
//   +0x0048  0x1430 24 console variables (see kSlot497Vars)
//   +0x1478  0x0bd0 19 debug-menu items 0x2eed950.. (see kSlot497Menus)
//   +0x20f0  0x050  zero-filled container 0x2eee5c8 (three ymm zero stores)
//   +0x2140  0x008  TSC stamp 0x2eee618 (0x323d0)
//   +0x2148  0x188  seven 0x38 B named records 0x2eee620.. (a81700, atexit 0xa81950)
//   +0x22d0  0x008  qword 0x2eee7a8 zeroed at 0x264b6c; +0x22d8..+0x22e0 is a hole
//   +0x22e0  0x0f0  three 0x50 B registration entries 0x2eee7b8 / 0x2eee808 / 0x2eee858
// Total 0x23d0 B, ending exactly at 0x2eee8a8, the first byte after the body's last global store.
//
// The written-byte map is explicit in slot497_byte_written: 0x1c9a (7,322) bytes written and
// 0x736 (1,846) holes. The earlier draft totals 0x1dff / 0x5d1 were WRONG: they counted the
// functor inline buffers (+0x90..0xa0 at bool vars, +0xa0..0xb0 at ranged vars), the +0x110
// padding at string vars, and the 8-byte cell tail as written. None of those bytes has a store in
// the pinned body or in a called constructor. This header is corrected to the ELF.
//
// ================================ families ================================
// VARIABLES (constructor order; object sizes 0xc8 bool / 0xd8 ranged / 0x148 string):
//   5 bools `r_fps`, `show_memory`, `show_globalTime`, `show_timeManager`, `show_buildversion`
//   (value/default 1,1,0,0,0); ranged int `show_profileInfo` (0xc580, 0..2, value 0); bools
//   `exitGame`, `restartGame` (0); float `gameSpeed` (0x6aa00, 1.0); bool `singleStep` (0); float
//   `gameMaxDeltaTime` (0x6aa00, 0.1); bounded float `gameDeltaTimeCorrectionTolorance` (0xc300,
//   0..1, 0.5); int `randomSeed` (0x3a980, -1); string vars `loadScripting` and
//   `StartupScriptFromEditor` (0x257a0, empty); seven bools `snapshotGameInfo`, `logGameInfo`,
//   `snapshotAssets`, `logGameMessages`, `logGameMessagesEventLog`, `snapshotBugData`,
//   `disableRumble` (0); local ranged ints `TargetFramesPerSeconds` (0..3) and `VSyncWindow` (0..5)
//   through 0x261b50 / 0x261f10.
// MENU (0x2eed950, 0xa8 stride): label 26800 (empty, flags 0x608), Game Speed slider 27050
//   (0.01..0.5, target gameSpeed), Restart Game and `|Exit Game` toggles, group `Debug` (26c10),
//   labels 26800 (Debug flags 0x208, Messages flags 0x220), group `Debug` under `Debug Game`,
//   toggles Display Frame Rate / Memory / Global Time / Build Version, separator 26b60, toggles
//   Target FPS / VSync Window, separator 26b60, Profile Info under `Profile`, Display Time Manager
//   under `Systems`.
// RECORDS: seven `Main loop:` timing records (Wall time kind 0 flag 1; the other six kind 1 flag 1).
// ENTRIES: CookedAssetStatusInternal, SendLivePropertyUpdate, SendMessageToComponent (0x4b560,
//   atexit 0x4b660).
//
// ================================ modelled vs hook (G2) ================================
// MODELLED (native, exact): every store the page's own code performs into the 0x23d0 block: the
// cell, page header, view, all 24 variable records (base 0x60 + value/default/range/args/functor
// and the string value view), all 19 menu records (link/registered/label/kind/text/empty/target/
// range/flags), the zero container, the TSC stamp, the seven named records and the three 0x50 B
// entries. The written-byte map is explicit in slot497_byte_written (0x1c9a written, 0x736 holes).
// HOOKS (mandatory; a null hook blocks before the first store):
//   * register_var: default native 0x20d60 (queues into the pending list while the manager cell is
//     null, the whole startup path).
//   * register_menu: default root-list append (0x268b0 root path); increments the root count and
//     links the item; the root path deliberately leaves item.registered == 0.
//   * register_record: default native 0xa81700 with the host allocator contract.
//   * register_entry: default records the entry's registered byte and returns; the 0x4b560
//     manager-list insert (0x4b3f0) is NOT lifted and stays a hook contract.
//   * atexit: records every destructor RVA; nothing is run.
//   * release_view: 0xf590 on borrowed stack views; exactly inert.
//   * destroy_arg_functor: the seven indirect `call *0x18(%rax)` destroy slots; default accepts.
//   * store_tsc: default native 0x323d0.
// NOT modelled: what reads the variables at run time, the 0x4b560 manager-list insert, the entry
// callbacks' targets, the engine allocator behind 0xa81700, and the `MenuSeparator` text allocated
// by 0x26800 / 0x26b60 (the allocation is a host input through SharedStrCtorHooks).
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"
#include "helper_a81700_named_record.hpp"
#include "shared_str_ctor_f1a0.hpp"
#include "store_tsc_323d0.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot497Rva = 0x262a80;
inline constexpr std::size_t kSlot497Size = 0x23d0;             // 9,168 B
inline constexpr std::uint64_t kSlot497EndRva = 0x264e50;
inline constexpr std::size_t kSlot497InstructionCount = 1742;
inline constexpr std::size_t kSlot497DirectCallSites = 225;
inline constexpr std::size_t kSlot497IndirectCallSites = 7;
inline constexpr std::size_t kSlot497BranchSites = 10;
inline constexpr std::uint64_t kSlot497TableWordRva = 0x2dd6158;   // 0x2dd70e0 - 497*8
inline constexpr std::uint32_t kSlot497TableIndex = 497;
inline constexpr std::uint8_t kSlot497PadByte = 0x90;

inline constexpr std::uint64_t kSlot497BlockRva = 0x2eec4d8;
inline constexpr std::size_t kSlot497BlockSize = 0x23d0;
inline constexpr std::size_t kSlot497BlockWritten = 0x1c9a;
inline constexpr std::size_t kSlot497BlockHoles = 0x736;

inline constexpr std::size_t kSlot497CellOffset = 0x0000;
inline constexpr std::size_t kSlot497PageOffset = 0x0008;
inline constexpr std::size_t kSlot497ViewOffset = 0x0028;
inline constexpr std::size_t kSlot497VarOffset = 0x0048;
inline constexpr std::size_t kSlot497MenuOffset = 0x1478;
inline constexpr std::size_t kSlot497ZeroOffset = 0x20f0;
inline constexpr std::size_t kSlot497TscOffset = 0x2140;
inline constexpr std::size_t kSlot497RecordOffset = 0x2148;
inline constexpr std::size_t kSlot497TailQwordOffset = 0x22d0;
inline constexpr std::size_t kSlot497EntryOffset = 0x22e0;

inline constexpr std::size_t kSlot497CellSize = 0x08;
inline constexpr std::size_t kSlot497PageSize = 0x20;
inline constexpr std::size_t kSlot497ViewSize = 0x20;
inline constexpr std::size_t kSlot497BoolVarSize = 0xc8;
inline constexpr std::size_t kSlot497RangedVarSize = 0xd8;
inline constexpr std::size_t kSlot497StringVarSize = 0x148;
inline constexpr std::size_t kSlot497MenuItemSize = 0xa8;
inline constexpr std::size_t kSlot497MenuItemPayload = 0xa4;
inline constexpr std::size_t kSlot497ZeroSize = 0x50;
inline constexpr std::size_t kSlot497RecordSize = 0x38;
inline constexpr std::size_t kSlot497EntrySize = 0x50;

inline constexpr std::size_t kSlot497VarCount = 24;
inline constexpr std::size_t kSlot497MenuCount = 19;
inline constexpr std::size_t kSlot497RecordCount = 7;
inline constexpr std::size_t kSlot497EntryCount = 3;

inline constexpr std::uint64_t kSlot497DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot497CanaryRva = 0x15fe518;
inline constexpr std::uint64_t kSlot497StackChkFailRva = 0xf8f3a0;
inline constexpr std::uint64_t kSlot497AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot497ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot497RegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot497BaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot497RangedIntCtorRva = 0xc580;
inline constexpr std::uint64_t kSlot497UnboundedFloatCtorRva = 0x6aa00;
inline constexpr std::uint64_t kSlot497BoundedFloatCtorRva = 0xc300;
inline constexpr std::uint64_t kSlot497UnboundedIntCtorRva = 0x3a980;
inline constexpr std::uint64_t kSlot497StringVarCtorRva = 0x257a0;
inline constexpr std::uint64_t kSlot497LocalIntLowCtorRva = 0x261b50;
inline constexpr std::uint64_t kSlot497LocalIntHighCtorRva = 0x261f10;
inline constexpr std::uint64_t kSlot497ToggleCtorRva = 0x26ea0;
inline constexpr std::uint64_t kSlot497GroupCtorRva = 0x26c10;
inline constexpr std::uint64_t kSlot497LabelCtorRva = 0x26800;
inline constexpr std::uint64_t kSlot497SeparatorCtorRva = 0x26b60;
inline constexpr std::uint64_t kSlot497SliderCtorRva = 0x27050;
inline constexpr std::uint64_t kSlot497NamedRecordCtorRva = 0xa81700;
inline constexpr std::uint64_t kSlot497EntryRegisterRva = 0x4b560;
inline constexpr std::uint64_t kSlot497EntryLinkRva = 0x4b3f0;
inline constexpr std::uint64_t kSlot497TscRva = 0x323d0;
inline constexpr std::uint64_t kSlot497CellSetRva = 0x8d650;
inline constexpr std::uint64_t kSlot497EmptyViewRva = 0xf730;
inline constexpr std::uint64_t kSlot497ViewCtorRva = 0xf2f0;

// Identity vtable / functor tags and destructors (pinned, never dispatched or run).
inline constexpr std::uint64_t kSlot497BoolFinalVptr = 0x1594218;
inline constexpr std::uint64_t kSlot497BoolMidVptr = 0x15940b0;
inline constexpr std::uint64_t kSlot497BoolFnVtbl = 0x159c758;
inline constexpr std::uint64_t kSlot497RangedFinalVptr = 0x1594c08;
inline constexpr std::uint64_t kSlot497RangedMidVptr = 0x15941a0;
inline constexpr std::uint64_t kSlot497RangedFnVtbl = 0x15944a0;
inline constexpr std::uint64_t kSlot497FloatFinalVptr = 0x15942b0;
inline constexpr std::uint64_t kSlot497FloatMidVptr = 0x1594128;
inline constexpr std::uint64_t kSlot497FloatFnVtbl = 0x1594318;
inline constexpr std::uint64_t kSlot497StringFinalVptr = 0x1594ef0;
inline constexpr std::uint64_t kSlot497StringFnVtbl = 0x1595068;
inline constexpr std::uint64_t kSlot497LocalLowFinalVptr = 0x159c808;
inline constexpr std::uint64_t kSlot497LocalLowMidVptr = 0x159c628;
inline constexpr std::uint64_t kSlot497LocalLowFnVtbl = 0x159c7d8;
inline constexpr std::uint64_t kSlot497LocalHighFinalVptr = 0x159c8a0;
inline constexpr std::uint64_t kSlot497LocalHighMidVptr = 0x159c6a0;
inline constexpr std::uint64_t kSlot497LocalHighFnVtbl = 0x159c870;
inline constexpr std::uint64_t kSlot497PageDtorRva = 0x253430;
inline constexpr std::uint64_t kSlot497RangedIntDtorRva = 0x18a60;
inline constexpr std::uint64_t kSlot497FloatDtorRva = 0x20f0;
inline constexpr std::uint64_t kSlot497StringDtorRva = 0x23c90;
inline constexpr std::uint64_t kSlot497LocalLowDtorRva = 0x253440;
inline constexpr std::uint64_t kSlot497LocalHighDtorRva = 0x253480;
inline constexpr std::uint64_t kSlot497MenuItemDtorRva = 0x272d0;
inline constexpr std::uint64_t kSlot497RecordDtorRva = 0xa81950;
inline constexpr std::uint64_t kSlot497EntryDtorRva = 0x4b660;

inline constexpr char kSlot497PageName[] = "Bluepoint Engine";
inline constexpr std::size_t kSlot497PageNameLength = 0;   // the view length is 0, the pointer names it

inline constexpr std::uint32_t kSlot497VarFlags = 1;
inline constexpr std::uint32_t kSlot497MenuFlagsTop = 0x608;
inline constexpr std::uint32_t kSlot497MenuFlagsDebug = 0x208;
inline constexpr std::uint32_t kSlot497MenuFlagsMessages = 0x220;
inline constexpr std::uint32_t kSlot497MenuFlagsGroup = 0x20;
inline constexpr std::uint32_t kSlot497MenuFlagsItem = 0x30;
inline constexpr std::uint32_t kSlot497NoTarget = 0xffffffffu;
inline constexpr std::uint64_t kSlot497EntryCallbackRva = 0x25ddd0;
inline constexpr std::uint64_t kSlot497EntryKeyRva = 0x15fe800;

enum class Slot497VarType : std::uint8_t {
    bool_var,
    ranged_int,
    unbounded_float,
    bounded_float,
    unbounded_int,
    string_var,
    local_int_low,
    local_int_high,
};

struct Slot497VarSpec {
    Slot497VarType type;
    std::uint32_t offset;       // PAGE-relative
    const char* name;
    std::uint8_t name_len;
    std::uint32_t value;        // byte for bool, IEEE bits for floats, int32 bits otherwise
    std::uint32_t min_value;    // int32 / float bits; 0xffffffff for "disengaged"
    std::uint32_t max_value;
    std::uint64_t fn_tag;       // functor class tag stored at +0xc0
    std::uint64_t dtor_rva;
    std::uint64_t final_vptr;
    std::uint16_t size;
};

inline constexpr std::array<Slot497VarSpec, kSlot497VarCount> kSlot497Vars = {{
    {Slot497VarType::bool_var, 0x0048, "r_fps", 5, 1, 0, 0, 0x159c758, 0x20b0, 0x1594218, 0xc8},
    {Slot497VarType::bool_var, 0x0110, "show_memory", 11, 1, 0, 0, 0x159c758, 0x20b0, 0x1594218, 0xc8},
    {Slot497VarType::bool_var, 0x01d8, "show_globalTime", 15, 0, 0, 0, 0x159c758, 0x20b0, 0x1594218, 0xc8},
    {Slot497VarType::bool_var, 0x02a0, "show_timeManager", 16, 0, 0, 0, 0x159c758, 0x20b0, 0x1594218, 0xc8},
    {Slot497VarType::bool_var, 0x0368, "show_buildversion", 17, 0, 0, 0, 0x159c758, 0x20b0, 0x1594218, 0xc8},
    {Slot497VarType::ranged_int, 0x0430, "show_profileInfo", 16, 0, 0, 2, 0x15944a0, 0x18a60, 0x1594c08, 0xd8},
    {Slot497VarType::bool_var, 0x0508, "exitGame", 8, 0, 0, 0, 0x159c758, 0x20b0, 0x1594218, 0xc8},
    {Slot497VarType::bool_var, 0x05d0, "restartGame", 11, 0, 0, 0, 0x159c758, 0x20b0, 0x1594218, 0xc8},
    {Slot497VarType::unbounded_float, 0x0698, "gameSpeed", 9, 0x3f800000u, 0, 0, 0x1594318, 0x20f0, 0x15942b0, 0xd8},
    {Slot497VarType::bool_var, 0x0770, "singleStep", 10, 0, 0, 0, 0x159c758, 0x20b0, 0x1594218, 0xc8},
    {Slot497VarType::unbounded_float, 0x0838, "gameMaxDeltaTime", 16, 0x3dcccccd, 0, 0, 0x1594318, 0x20f0, 0x15942b0, 0xd8},
    {Slot497VarType::bounded_float, 0x0910, "gameDeltaTimeCorrectionTolorance", 32, 0x3f000000u, 0,
     0x3f800000u, 0x1594318, 0x20f0, 0x15942b0, 0xd8},
    {Slot497VarType::unbounded_int, 0x09e8, "randomSeed", 10, 0xffffffffu, 0, 0, 0x15944a0, 0x18a60, 0x1594c08, 0xd8},
    {Slot497VarType::string_var, 0x0ac0, "loadScripting", 13, 0, 0, 0, 0x1595068, 0x23c90, 0x1594ef0, 0x148},
    {Slot497VarType::string_var, 0x0c08, "StartupScriptFromEditor", 23, 0, 0, 0, 0x1595068, 0x23c90, 0x1594ef0, 0x148},
    {Slot497VarType::bool_var, 0x0d50, "snapshotGameInfo", 16, 0, 0, 0, 0x159c758, 0x20b0, 0x1594218, 0xc8},
    {Slot497VarType::bool_var, 0x0e18, "logGameInfo", 12, 0, 0, 0, 0x159c758, 0x20b0, 0x1594218, 0xc8},
    {Slot497VarType::bool_var, 0x0ee0, "snapshotAssets", 14, 0, 0, 0, 0x159c758, 0x20b0, 0x1594218, 0xc8},
    {Slot497VarType::bool_var, 0x0fa8, "logGameMessages", 15, 0, 0, 0, 0x159c758, 0x20b0, 0x1594218, 0xc8},
    {Slot497VarType::bool_var, 0x1070, "logGameMessagesEventLog", 27, 0, 0, 0, 0x159c758, 0x20b0, 0x1594218, 0xc8},
    {Slot497VarType::bool_var, 0x1138, "snapshotBugData", 16, 0, 0, 0, 0x159c758, 0x20b0, 0x1594218, 0xc8},
    {Slot497VarType::bool_var, 0x1200, "disableRumble", 13, 0, 0, 0, 0x159c758, 0x20b0, 0x1594218, 0xc8},
    {Slot497VarType::local_int_low, 0x12c8, "TargetFramesPerSeconds", 22, 0, 0, 3, 0x159c7d8, 0x253440, 0x159c808, 0xd8},
    {Slot497VarType::local_int_high, 0x13a0, "VSyncWindow", 11, 0, 0, 5, 0x159c870, 0x253480, 0x159c8a0, 0xd8},
}};

enum class Slot497MenuType : std::uint8_t {
    label_26800,
    slider_27050,
    toggle_26ea0,
    group_26c10,
    separator_26b60,
};

struct Slot497MenuSpec {
    Slot497MenuType type;
    std::uint32_t offset;          // PAGE-relative
    const char* parent;            // label view at +0x28
    std::uint8_t parent_len;
    const char* text;              // text view at +0x50; empty for separators/labels
    std::uint8_t text_len;
    std::uint32_t target_offset;   // PAGE-relative target, kSlot497NoTarget for none
    std::uint32_t flags;
    float range_min;
    float range_max;
};

inline constexpr std::array<Slot497MenuSpec, kSlot497MenuCount> kSlot497Menus = {{
    {Slot497MenuType::label_26800, 0x1478, "", 0, "", 0, kSlot497NoTarget, 0x608, 1.0f, 10.0f},
    {Slot497MenuType::slider_27050, 0x1520, "", 0, "Game Speed", 10, 0x0698, 0x30, 0.01f, 0.5f},
    {Slot497MenuType::toggle_26ea0, 0x15c8, "", 0, "Restart Game", 12, 0x05d0, 0x30, 1.0f, 10.0f},
    {Slot497MenuType::toggle_26ea0, 0x1670, "", 0, "|Exit Game", 10, 0x0508, 0x30, 1.0f, 10.0f},
    {Slot497MenuType::group_26c10, 0x1718, "", 0, "Debug", 5, kSlot497NoTarget, 0x20, 1.0f, 10.0f},
    {Slot497MenuType::label_26800, 0x17c0, "Debug", 5, "", 0, kSlot497NoTarget, 0x208, 1.0f, 10.0f},
    {Slot497MenuType::group_26c10, 0x1868, "Debug", 5, "Messages", 8, kSlot497NoTarget, 0x20, 1.0f, 10.0f},
    {Slot497MenuType::label_26800, 0x1910, "Messages", 8, "", 0, kSlot497NoTarget, 0x220, 1.0f, 10.0f},
    {Slot497MenuType::group_26c10, 0x19b8, "Debug Game", 10, "Debug", 5, kSlot497NoTarget, 0x20, 1.0f, 10.0f},
    {Slot497MenuType::toggle_26ea0, 0x1a60, "Debug", 5, "Display Frame Rate", 18, 0x0048, 0x30, 1.0f, 10.0f},
    {Slot497MenuType::toggle_26ea0, 0x1b08, "Debug", 5, "Display Memory", 14, 0x0110, 0x30, 1.0f, 10.0f},
    {Slot497MenuType::toggle_26ea0, 0x1bb0, "Debug", 5, "Display Global Time", 19, 0x01d8, 0x30, 1.0f, 10.0f},
    {Slot497MenuType::toggle_26ea0, 0x1c58, "Debug", 5, "Display Build Version", 21, 0x0368, 0x30, 1.0f, 10.0f},
    {Slot497MenuType::separator_26b60, 0x1d00, "Debug", 5, "", 0, kSlot497NoTarget, 0x30, 1.0f, 10.0f},
    {Slot497MenuType::toggle_26ea0, 0x1da8, "Debug", 5, "Target FPS", 10, 0x12c8, 0x30, 1.0f, 10.0f},
    {Slot497MenuType::toggle_26ea0, 0x1e50, "Debug", 5, "VSync Window", 12, 0x13a0, 0x30, 1.0f, 10.0f},
    {Slot497MenuType::separator_26b60, 0x1ef8, "Debug", 5, "", 0, kSlot497NoTarget, 0x30, 1.0f, 10.0f},
    {Slot497MenuType::toggle_26ea0, 0x1fa0, "Profile", 7, "Profile Info", 12, 0x0430, 0x30, 1.0f, 10.0f},
    {Slot497MenuType::toggle_26ea0, 0x2048, "Systems", 7, "Display Time Manager", 20, 0x02a0, 0x30, 1.0f, 10.0f},
}};

struct Slot497RecordSpec {
    const char* name;
    std::uint8_t name_len;
    std::uint32_t kind;
    std::uint8_t flag;
};

inline constexpr std::array<Slot497RecordSpec, kSlot497RecordCount> kSlot497Records = {{
    {"Main loop: Wall time", 20, 0, 1},
    {"Main loop: Desired frame time", 29, 1, 1},
    {"Main loop: Frame time", 21, 1, 1},
    {"Main loop: Sleep time", 21, 1, 1},
    {"Main loop: ProcessGameLoop", 26, 1, 1},
    {"Main loop: BPEGameLoop", 22, 1, 1},
    {"Main loop: Render", 17, 1, 1},
}};

struct Slot497EntrySpec {
    const char* name;
    std::uint8_t name_len;
};

inline constexpr std::array<Slot497EntrySpec, kSlot497EntryCount> kSlot497Entries = {{
    {"CookedAssetStatusInternal", 25},
    {"SendLivePropertyUpdate", 22},
    {"SendMessageToComponent", 22},
}};

// The 0x50-byte entry: registered byte, the name view, a 16-byte zeroed link pair, then the
// callback/link words the body stores from 0x25ddd0 and the dso-relative cell at 0x15fe800.
struct Slot497Entry {
    std::uint8_t registered;      // +0x00
    std::uint8_t gap01[7];        // +0x01 never written
    SharedStr name;               // +0x08
    std::uint64_t link_next;      // +0x28 = 0
    std::uint64_t link_self;      // +0x30
    std::uint64_t key;            // +0x38
    std::uint64_t callback;       // +0x40 = 0x25ddd0
    std::uint64_t reserved;       // +0x48 = 0
};
static_assert(sizeof(Slot497Entry) == kSlot497EntrySize &&
              offsetof(Slot497Entry, name) == 0x08 && offsetof(Slot497Entry, link_next) == 0x28 &&
              offsetof(Slot497Entry, link_self) == 0x30 && offsetof(Slot497Entry, callback) == 0x40);

// The whole page. Bytes are addressed by the offsets above; accessors give the typed views.
struct alignas(8) Slot497Objects {
    std::array<std::uint8_t, kSlot497BlockSize> bytes{};
};
static_assert(sizeof(Slot497Objects) == kSlot497BlockSize);
static_assert(kSlot497BlockRva + kSlot497BlockSize == 0x2eee8a8);

enum class Slot497Stage : std::uint8_t {
    none,
    hooks,
    cell,
    page_header,
    view,
    page_atexit,
    var_build,
    var_register,
    var_atexit,
    menu_build,
    menu_register,
    menu_atexit,
    zero_container,
    tsc,
    record_build,
    record_register,
    record_atexit,
    tail_qword,
    entry_build,
    entry_register,
    entry_atexit,
    canary,
};

struct Slot497Result {
    bool ok = false;
    Slot497Stage blocked = Slot497Stage::none;
    std::size_t blocked_index = 0;
    bool cell_set = false;
    bool header_built = false;
    bool view_built = false;
    bool page_atexit = false;
    std::size_t vars_built = 0;
    std::size_t vars_registered = 0;
    std::size_t menus_built = 0;
    std::size_t menus_registered = 0;
    std::size_t records_built = 0;
    std::size_t records_registered = 0;
    std::size_t entries_built = 0;
    std::size_t entries_registered = 0;
    std::size_t atexit_records = 0;
    bool zeroed = false;
    bool tsc_stamped = false;
    bool tail_qword_zeroed = false;
    bool canary_checked = false;
};

// Mandatory hooks; a null pointer blocks at Slot497Stage::hooks before any store.
struct Slot497Hooks {
    bool (*register_var)(ConCmdBase& cmd, ConCmdRegistry& registry) noexcept = nullptr;
    bool (*register_menu)(DebugMenuItem& item, DebugMenuRegistry& menu,
                          StartupAtexitLog& atexit) noexcept = nullptr;
    bool (*register_record)(RecordRegistryCell& cell, NamedRecord& record, const SharedStr& name,
                            std::uint32_t kind, std::uint8_t flag,
                            const AllocDispatchContract& alloc) noexcept = nullptr;
    bool (*register_entry)(Slot497Entry& entry) noexcept = nullptr;
    bool (*atexit)(std::uint64_t dtor_rva, const void* object, ConCmdRegistry& registry) noexcept = nullptr;
    bool (*release_view)(const SharedStr& view) noexcept = nullptr;
    bool (*destroy_arg_functor)() noexcept = nullptr;
    void (*store_tsc)(void* dest) noexcept = nullptr;
    SharedStrCtorHooks menu_alloc{};
};

[[nodiscard]] Slot497Hooks default_slot497_hooks() noexcept;
[[nodiscard]] AllocDispatchContract default_slot497_record_alloc() noexcept;

// Typed views into the page (all offsets static_assert-checked below).
[[nodiscard]] ConCmdBase* slot497_var_base(Slot497Objects& objects, std::size_t index) noexcept;
[[nodiscard]] void* slot497_var_object(Slot497Objects& objects, std::size_t index) noexcept;
[[nodiscard]] DebugMenuItem* slot497_menu_item(Slot497Objects& objects, std::size_t index) noexcept;
[[nodiscard]] NamedRecord* slot497_record(Slot497Objects& objects, std::size_t index) noexcept;
[[nodiscard]] Slot497Entry* slot497_entry(Slot497Objects& objects, std::size_t index) noexcept;

// True when the body's own store program (including the writes performed by its direct callees
// into the page) covers this byte of the 0x23d0-byte block.
[[nodiscard]] bool slot497_byte_written(std::size_t offset) noexcept;
[[nodiscard]] const char* slot497_stage_name(Slot497Stage stage) noexcept;

[[nodiscard]] Slot497Result init_slot497_262a80(Slot497Objects& objects, ConCmdRegistry& registry,
                                               DebugMenuRegistry& menu, RecordRegistryCell& records,
                                               const Slot497Hooks& hooks) noexcept;

}  // namespace sotc
