// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 572 (RVA 0x1a20, 1668 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 572. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_572.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Player-position debug console commands (inference from the strings): printPlayerPos / printPlayerAndCameraPos / setPlayerPos / Pos / toggleDebugCam plus the CP10 menu rows (two toggles and a Kill the active Boss command item).
// Rows, in constructor order (8 objects at 0x2dd7100..): 0 bool variables (caller functor tag 0x0, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// debug page batch 14: five console commands, two 0x26ea0 toggles and a 0x26f90 command item
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

namespace sotc {

inline constexpr std::uint64_t kSlot572Rva = 0x1a20;
inline constexpr std::size_t kSlot572Size = 1668;
inline constexpr std::uint64_t kSlot572BoolFnTag = 0x0;
inline constexpr std::uint64_t kSlot572FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot572ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot572VarFlags = 1;
inline constexpr std::uint64_t kSlot572UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot572UFloatCount = 0;
inline constexpr std::size_t kSlot572BoolCount = 0;
inline constexpr std::size_t kSlot572FloatCount = 0;
inline constexpr std::size_t kSlot572RowCount = 8;
inline constexpr std::size_t kSlot572StringCount = 9;
inline constexpr std::uint64_t kSlot572IntFnTag = 0x0;
inline constexpr std::size_t kSlot572IntCount = 0;
inline constexpr std::size_t kSlot572LocalIntCount = 0;
inline constexpr std::size_t kSlot572SliderCount = 0;
inline constexpr std::uint64_t kSlot572UIntFnTag = 0x0;
inline constexpr std::size_t kSlot572UIntCount = 0;
inline constexpr std::size_t kSlot572CmdCount = 5;
inline constexpr std::size_t kSlot572StrVarCount = 0;
inline constexpr std::uint64_t kSlot572Object0Rva = 0x2dd7100;
inline constexpr std::size_t kSlot572CellCount = 0;
inline constexpr std::size_t kSlot572HolderCount = 0;
inline constexpr std::size_t kSlot572SentinelCount = 0;
inline constexpr char kSlot572CmdNames[kSlot572CmdCount][24] = {"printPlayerPos", "printPlayerAndCameraPos", "setPlayerPos", "Pos", "toggleDebugCam"};
inline constexpr std::size_t kSlot572CmdNameLengths[kSlot572CmdCount] = {0xe, 0x17, 0xc, 0x3, 0xe};
inline constexpr std::int32_t kSlot572CmdDesc[kSlot572CmdCount] = {0, 1, 2, 2, 3};
inline constexpr std::uint64_t kSlot572CmdHandler[kSlot572CmdCount] = {0x330, 0x530, 0x570, 0xa70, 0xa90};
inline constexpr char kSlot572MenuStrings[kSlot572StringCount][135] = {"Prints the player's current orientation to the console. Params: None", "Prints the player and camera's current orientation to the console. Params: None", "Sets the player to the indicated position and direction. Params: <posX> <posY> <posZ> [<rotX> <rotY> <rotZ>]", "Toggles the debug camera. If toggled on the camera will be placed at the location of the current game camera. Params: [<enable state>]", "CP10", "Set Player Position to Camera Position", "Toggle the Debug Camera", "Kill the active Boss", "killboss"};
inline constexpr std::size_t kSlot572MenuStringLengths[kSlot572StringCount] = {0x44, 0x4f, 0x6c, 0x86, 0x4, 0x26, 0x17, 0x14, 0x8};

struct Slot572Objects {
    ConCmdCommand r00;   // 0x2dd7100 cmd
    ConCmdCommand r01;   // 0x2dd71a0 cmd
    ConCmdCommand r02;   // 0x2dd7240 cmd
    ConCmdCommand r03;   // 0x2dd72e0 cmd
    ConCmdCommand r04;   // 0x2dd7380 cmd
    DebugMenuItem r05;   // 0x2dd7420 toge
    DebugMenuItem r06;   // 0x2dd74c8 toge
    DebugMenuItem r07;   // 0x2dd7570 cmdi
    const void* external_target_0 = nullptr;   // identity of the object at 0x2dfcbd0 (built by another slot; the host supplies it, the model only stores the pointer)
};

struct Slot572Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot572Result init_slot572_1a20(Slot572Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
