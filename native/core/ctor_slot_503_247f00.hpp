// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 503 (RVA 0x247f00, 3185 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 503. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_503.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// UI scene debug page (inference from the strings): a NULL-named record, three bool/float override variables, nine ui* console commands and a Render draw toggle.
// Rows, in constructor order (15 objects at 0x2eea480..): 3 bool variables (caller functor tag 0x159bae0, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// debug page batch 15: null-name record, three variables, nine commands and a 0x26ea0 toggle
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
#include "helper_f8f630_strncpy.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot503Rva = 0x247f00;
inline constexpr std::size_t kSlot503Size = 3185;
inline constexpr std::uint64_t kSlot503BoolFnTag = 0x159bae0;
inline constexpr std::uint64_t kSlot503FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot503ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot503VarFlags = 1;
inline constexpr std::uint64_t kSlot503UFloatFnTag = 0x159bb00;
inline constexpr std::size_t kSlot503UFloatCount = 1;
inline constexpr std::size_t kSlot503BoolCount = 3;
inline constexpr std::size_t kSlot503FloatCount = 0;
inline constexpr std::size_t kSlot503RowCount = 15;
inline constexpr std::size_t kSlot503StringCount = 15;
inline constexpr std::uint64_t kSlot503IntFnTag = 0x0;
inline constexpr std::size_t kSlot503IntCount = 0;
inline constexpr std::size_t kSlot503LocalIntCount = 0;
inline constexpr std::size_t kSlot503SliderCount = 0;
inline constexpr std::uint64_t kSlot503UIntFnTag = 0x0;
inline constexpr std::size_t kSlot503UIntCount = 0;
inline constexpr std::size_t kSlot503CmdCount = 9;
inline constexpr std::size_t kSlot503StrVarCount = 0;
inline constexpr std::uint64_t kSlot503Object0Rva = 0x2eea480;
inline constexpr std::size_t kSlot503CellCount = 1;
inline constexpr std::size_t kSlot503HolderCount = 0;
inline constexpr std::size_t kSlot503SentinelCount = 0;
inline constexpr char kSlot503BoolNames[kSlot503BoolCount][33] = {"uiOverrideTransitions", "uiOverrideTargetFramerateInMenus", "uiDrawDisable"};
inline constexpr std::size_t kSlot503BoolNameLengths[kSlot503BoolCount] = {0x15, 0x20, 0xd};
inline constexpr std::uint8_t kSlot503BoolValues[kSlot503BoolCount] = {0, 1, 0};
inline constexpr std::int32_t kSlot503BoolDesc[kSlot503BoolCount] = {0, 2, 12};
inline constexpr std::uint8_t kSlot503BoolEarly[kSlot503BoolCount] = {0, 0, 0};
inline constexpr char kSlot503UFloatNames[kSlot503UFloatCount][33] = {"uiOverrideTransitionDuration"};
inline constexpr std::size_t kSlot503UFloatNameLengths[kSlot503UFloatCount] = {0x1c};
inline constexpr std::uint32_t kSlot503UFloatValueBits[kSlot503UFloatCount] = {0x41200000};
inline constexpr std::uint8_t kSlot503UFloatEarly[kSlot503UFloatCount] = {0};
inline constexpr std::int32_t kSlot503UFloatDesc[kSlot503UFloatCount] = {1};
inline constexpr char kSlot503CmdNames[kSlot503CmdCount][33] = {"uiPushScene", "uiPushSceneConfig", "uiPopScene", "uiPopAllScenes", "uiShowHudScene", "uiHideHudScene", "uiInputDisabled", "uiInputEnabled", "uiRepositionAll"};
inline constexpr std::size_t kSlot503CmdNameLengths[kSlot503CmdCount] = {0xb, 0x11, 0xa, 0xe, 0xe, 0xe, 0xf, 0xe, 0xf};
inline constexpr std::int32_t kSlot503CmdDesc[kSlot503CmdCount] = {3, 4, 5, 6, 7, 8, 9, 10, 11};
inline constexpr std::uint64_t kSlot503CmdHandler[kSlot503CmdCount] = {0x245490, 0x245650, 0x2457e0, 0x2458b0, 0x245980, 0x245b80, 0x245d80, 0x245e50, 0x245f20};
inline constexpr char kSlot503MenuStrings[kSlot503StringCount][94] = {"Instead of using transition times from data, use value found in uiOverrideTransitionDuration.", "If uiOverrideTransitions is true this value will be used for all transition times.", "If enabled the UI is allowed to change the target framerate in certain menus.", "Pushes a UI scene onto the menu stack. Params: <scene name>", "Pushes a UI scene configuration onto the menu stack. Params: <config name>", "Pops the top of the menu stack. Params: None", "Pops all the scenes off the menu stack. Params: None", "Shows the indicated scene as a piece of the HUD. Params: <scene name> [transition time]", "Hides the indicated scene as a piece of the HUD. Params: <scene name> [transition time]", "Disables UI input.", "Enable UI input.", "Reposition all UI elements.", "Prevents UI from drawing.", "Render", "Disable UI Draw"};
inline constexpr std::size_t kSlot503MenuStringLengths[kSlot503StringCount] = {0x5d, 0x52, 0x4d, 0x3b, 0x4a, 0x2c, 0x34, 0x57, 0x57, 0x12, 0x10, 0x1b, 0x19, 0x6, 0xf};

struct Slot503Objects {
    NullNameCell247f47 r00;   // 0x2eea480 ncell
    std::uint8_t gap01[0x4];
    ConCmdBoolVar r01;   // 0x2eea498 bool
    ConCmdRangedVar r02;   // 0x2eea560 ufloat
    ConCmdBoolVar r03;   // 0x2eea638 bool
    ConCmdCommand r04;   // 0x2eea700 cmd
    ConCmdCommand r05;   // 0x2eea7a0 cmd
    ConCmdCommand r06;   // 0x2eea840 cmd
    ConCmdCommand r07;   // 0x2eea8e0 cmd
    ConCmdCommand r08;   // 0x2eea980 cmd
    ConCmdCommand r09;   // 0x2eeaa20 cmd
    ConCmdCommand r10;   // 0x2eeaac0 cmd
    ConCmdCommand r11;   // 0x2eeab60 cmd
    ConCmdCommand r12;   // 0x2eeac00 cmd
    ConCmdBoolVar r13;   // 0x2eeaca0 bool
    DebugMenuItem r14;   // 0x2eead68 toge
};

struct Slot503Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot503Result init_slot503_247f00(Slot503Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
