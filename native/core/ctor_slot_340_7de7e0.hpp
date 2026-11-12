// CPU-01-STARTUP-BATCH-10: native model of startup constructor slot 340 (RVA 0x7de7e0, 1800 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 340. Every body fact below is byte-pinned by tools/lift/check_ctor_slot_340.py. NOTHING here
// executes, loads or calls an original RVA: vtables, handler tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// This was the startup-table cliff after batch 9 ("two indirect calls, XL"). The static resolution of both indirect-call flags:
//   * four `callq *%r13` (0x7de8dd, 0x7de9bd, 0x7dea9d, 0x7deb82) and two `callq *0x38(%rsp)` (0x7dec85, 0x7dee8e). r13 is loaded ONCE at 0x7de8bf
//     (`movq 0x20(%rax), %r13`) with %rax = 0x1594dc0 (leaq at 0x7de888, same instruction pair as the slot-339 command), and is spilled to 0x38(%rsp)
//     at 0x7deb6a before r13 is reused for an object address. So all six calls dispatch through the console-command class table slot
//     [0x1594dc0 + 0x20], which the pinned RELA record resolves to 0x20d60 (the register-once body; `fcmd_vtable` extra). No indirect target
//     is left unresolved. This is the identical shape of slots 306 / 339 / 343 / 386 / 557 and is built by the existing init_fconcmd_object.
//   * the survey's "other indirect" label for the 5th/6th calls was the `*0x38(%rsp)` spill reload, not a second mechanism.
//
// Shape, in constructor order (object RVA):
//   c0..c3  0x325f090 + 0xa0 * n   four console commands `shadow_gameflag{chk,dump,set,cls}` (empty description, flags 2, handler tags
//                                  0x7db650 / 0x7db740 / 0x7db870 / 0x7db990 never called, vptr 0x1594dc0 + 0x10), atexit 0x4b0 each
//   cell    0x325f858              one NameHashCell {ptr "TimeAttackSetBestTime", fnv1a(name)} (the +0x0c bytes are never written)
//   c4      0x325fd20              console command `bp_giveAllOrbs` (description `Gives the user all the current difficulty orbs.`, handler 0x7ddc60)
//   g       0x325fdc0              debug-menu group: parent view `CP10`, caption `Shadow Cheat`, item flags 0x30 (0x26c10), atexit 0x272d0
//   t       0x325fe68              debug-menu toggle (0x26ea0): label `Shadow Cheat`, text `Give All Orbs`, flags 0x30, TARGET = c4 (the command
//                                  object), atexit 0x272d0
//   c5      0x325ff10              console command `bp_dumpCp3GameStateInfo` (description `Dump cp3 game state related information so it can be
//                                  inspected and compared`, handler 0x7dddd0), atexit 0x4b0
// Registration order: c0 c1 c2 c3 c4 c5 (the pending list); the menu root list gets g then t (the root destructor 0x27280 is recorded once, inside
// the first item). Atexit order: c0 c1 c2 c3 c4, 0x27280(root), g, t, c5. All strings are borrowed rodata views, so every 0xf590 release is an exact no-op.
//
// The toggle's range: 0x26ea0 narrows {1.0, 10.0} to {0.1, 0.5} only when target slot 3 == 1 and slot 6 == 3. A console command object has vptr
// 0x1594dd0 whose slot 3 (0x1594de8) is 0x22ae0 `xor eax,eax; ret`, so the gate is closed after the first virtual call and slot 6 is never read:
// the range stays {1.0, 10.0}. The slot-3 body is pinned by the checker (the same pin as slots 307 / 313 / 316).
// Meaning of the names (game-flag console commands, a time-attack best-time cell, a give-all-orbs cheat, a cp3 state dump) is INFERENCE from the
// strings only; no handler body is lifted or called. The __stack_chk_guard compare is a hook.
#pragma once

#include <cstddef>
#include <cstdint>

#include "ctor_shapes_343_356.hpp"
#include "ctor_slot_family_items.hpp"
#include "fnv_cstr_161fb0.hpp"
#include "hashed_name_ctors.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot340Rva = 0x7de7e0;
inline constexpr std::size_t kSlot340Size = 1800;
inline constexpr std::uint32_t kSlot340ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot340CommandGate3 = 0;   // 0x22ae0 `xor eax,eax; ret`; slot 6 is never reached

struct Slot340CommandSite {
    const char* name;
    const char* description;
    std::size_t description_length;
    std::uint64_t handler_tag;
    std::uint64_t object_rva;
};

inline constexpr std::size_t kSlot340CommandCount = 6;
inline constexpr Slot340CommandSite kSlot340Commands[kSlot340CommandCount] = {
    {"shadow_gameflagchk", "", 0, 0x7db650, 0x325f090},
    {"shadow_gameflagdump", "", 0, 0x7db740, 0x325f130},
    {"shadow_gameflagset", "", 0, 0x7db870, 0x325f1d0},
    {"shadow_gameflagcls", "", 0, 0x7db990, 0x325f270},
    {"bp_giveAllOrbs", "Gives the user all the current difficulty orbs.", 0x2f, 0x7ddc60, 0x325fd20},
    {"bp_dumpCp3GameStateInfo", "Dump cp3 game state related information so it can be inspected and compared", 0x4b, 0x7dddd0, 0x325ff10},
};

inline constexpr char kSlot340CellName[] = "TimeAttackSetBestTime";
inline constexpr std::uint64_t kSlot340CellRva = 0x325f858;
inline constexpr char kSlot340GroupParent[] = "CP10";
inline constexpr std::size_t kSlot340GroupParentLength = 4;
inline constexpr char kSlot340GroupCaption[] = "Shadow Cheat";
inline constexpr std::size_t kSlot340GroupCaptionLength = 0xc;
inline constexpr std::uint64_t kSlot340GroupRva = 0x325fdc0;
inline constexpr char kSlot340ToggleText[] = "Give All Orbs";
inline constexpr std::size_t kSlot340ToggleTextLength = 0xd;
inline constexpr std::uint64_t kSlot340ToggleRva = 0x325fe68;

struct Slot340Objects {
    ConCmdCommand cmd[kSlot340CommandCount];   // c0..c3, c4, c5 (scattered in the image; see kSlot340Commands[n].object_rva)
    NameHashCell cell;                         // 0x325f858
    DebugMenuItem group;                       // 0x325fdc0
    DebugMenuItem toggle;                      // 0x325fe68, target = cmd[4]
};

struct Slot340Result {
    bool ok = false;
    std::size_t rows_done = 0;   // 0..9: c0 c1 c2 c3 cell c4 group toggle c5
    FConCmdBuildResult commands[kSlot340CommandCount]{};
};

[[nodiscard]] Slot340Result init_slot340_7de7e0(Slot340Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
