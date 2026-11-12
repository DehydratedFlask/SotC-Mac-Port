// Native slices of the 0x5b9f90 debug-menu page builder.
//
// Original: 0x5b9f90..0x5c56b1, 46,881 B, 7,685 instructions (pinned ELF sha256
// d0723afb...5861d3). The body has exactly one `ret` (0x5c56aa) and exactly one backward branch
// (0x5c1e5b -> 0x5c1d50, 33 iterations); it makes 1,136 direct calls to 25 distinct targets and 53
// indirect calls.
// Every fact below was recovered from the pinned bytes by tools/lift/slice_5b9f90.py; the committed
// map is docs/5B9F90_SLICES.md (with docs/5b9f90_slices.json as the machine-readable copy).
//
// Slice map, delimited by the rare callees and the single loop nest:
//
//   S0 0x5b9f90..0x5ba040  frame + canary + group "Shadow" under "CP10"          EXECUTED here
//   S1 0x5ba041..0x5ba045  0x2170 page builder ("Shadow Misc")                   contract
//   S2 0x5ba046..0x5ba04a  0x5cf140 page builder                                 contract
//   S3 0x5ba04b..0x5bc3d3  six pages, see kInit5b9f90Pages                      EXECUTED here
//   S4 0x5bc3d4..0x5bc3dd  0x5dbf40 + 0x7f8930 pair                              blocked
//   S5 0x5bc3de..0x5c1195  "Stereo 3D" / "Dynamic 3D" / wormhole pages           blocked
//   S6 0x5c1196..0x5c119f  0x5c94b0 + 0xb25840 pair                              blocked
//   S7 0x5c11a0..0x5c1d4f  "Shadow Sound" / "Shadow Cursor" / "Layers" pages     blocked
//   S8 0x5c1d50..0x5c1e60  the single loop nest (33 iterations)                    blocked
//   S9 0x5c1e61..0x5c56b0  "Performance" / "Transparency" / "Texture" / ...      blocked
//
// Executed natively here: S0 (0x5b9f90, 177 B) plus all six S3 pages, 0x5ba04b..0x5bc3d4 (9,097 B):
//   S3.1 "Shadow Boss Jump"        0x5ba04b, 2,663 B
//   S3.2 "Shadow Boss Outro Jump"  0x5baab2, 2,572 B
//   S3.3 "Shadow Cheat"            0x5bb4be, 2,160 B
//   S3.4 "Shadow Trophy"           0x5bbd2e, 1,609 B
//   S3.5 "Shadow Render"           0x5bc377,    80 B
//   S3.6 "MLAA"                    0x5bc3c7,    13 B
// Total 9,274 B of the 46,881 B body (19.8 %), 60 menu items and 60 registrations (plus the four
// S3.4 ConCmd registrations). Execution stops
// at 0x5bc3d4 (S4, the 0x5dbf40 + 0x7f8930 pair), which is reported as the resume offset; the
// remaining 37,607 B are refused, never inferred. The two intervening page builders 0x2170/0x5cf140
// are named host contracts (their bodies are not lifted), so the slices that contain them are marked
// `contract`, not executed.
//
// Constructors and registration are the existing native models (fmenu_ctors_slot306.*,
// debug_menu_item.*, menu_item_26db0.*): 0x26c10 group (kind 1), 0x26f90 command (kind 2), 0x26ea0
// toggle through a DebugMenuTarget identity, 0x27050 float range, 0x26db0 named path bound to a
// bool variable, 0x268b0 registration; 0xf590 view release is the host hook (every view here is a
// borrowed literal, so no storage is released). Target pointers are identity tags stored verbatim
// and never dereferenced. Nothing in this file executes an original RVA; G1 PARTIAL — no boot,
// audio, save or Metal gate is claimed.
//
// S3.3 toggles read two virtual slots of the target variable. The eight global bool variables
// (0x3265a20, 0x324c408, 0x3265ae8, 0x2fed798, 0x2fed0b8, 0x2fed298, 0x2fed338, 0x325dd78) are
// constructed by unlifted engine init code, so the host supplies one bool stand-in per target and
// the target identity is the measured pair (slot3 = 1, slot6 = 0) observed for every bool variable.
//
// S3.4 builds four bool variables inside the body: 0x20af0 (ConCmdBase ctor), then the bool-class
// stores (vptr 0x1594348, value_ref, default value, functor tag 0x15cb1d8), the __cxa_atexit record
// with dtor 0xacf0, 0x20d60 registration and the 0x26db0 named item. The whole block is guarded by
// one Itanium guard byte per variable (0x2fed8f8, 0x2fed9c0, 0x2feda88, 0x2fedb50); the host owns
// the guard bytes and the variable storage.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"
#include "fmenu_ctors_slot306.hpp"
#include "init_post_call_trace.hpp"
#include "menu_item_26db0.hpp"
#include "shared_str_ctor_f1a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit5b9f90Rva = 0x5B9F90;
inline constexpr std::uint64_t kInit5b9f90EndRva = 0x5C56B1;   // exclusive; the ret is at 0x5c56aa
inline constexpr std::size_t kInit5b9f90Size = 0xB721;         // 46,881 B
inline constexpr std::size_t kInit5b9f90Instructions = 7645;   // objdump-aligned, [0x5b9f90, 0x5c56b1)
inline constexpr std::size_t kInit5b9f90ExecutedBytes = 46876;  // whole body except the S1 5 B call site (0x2170 contract)
// Native code now executed by the same run, counting the executed callee bodies (S2 + S4.1).
inline constexpr std::size_t kInit5b9f90TotalExecutedBytes = 108325;   // 46,876 + 5,526 + 7,533 + 23,673 + 24,169 + 548 (S4.2)
inline constexpr std::uint64_t kInit5b9f90SingleRetRva = 0x5C56AA;
inline constexpr std::uint64_t kInit5b9f90LoopHeadRva = 0x5C1D50;
inline constexpr std::uint64_t kInit5b9f90LoopTailRva = 0x5C1E5B;   // jne 0x5c1d50, 33 iterations
inline constexpr std::uint64_t kInit5b9f90ResumeRva = 0x5BA041;   // S1 (0x2170) contract
inline constexpr std::uint32_t kInit5b9f90ItemFlags = 0x30;       // every item in this body

// The two page builders that sit between S0 and S3.
inline constexpr std::uint64_t kInit5b9f90PageBuilderShadowMiscRva = 0x2170;
inline constexpr std::uint64_t kInit5b9f90PageBuilderSecondRva = 0x5CF140;

// Identity tags of the command targets (stored, never dereferenced).
inline constexpr std::uint64_t kInit5b9f90BossJumpTargetRva = 0x2FED5B8;
inline constexpr std::uint64_t kInit5b9f90BossOutroJumpTargetRva = 0x2FED6F8;

// The bool-variable class observed at the four S3.4 definitions (a different class from the
// slot-299/301 shape: final vptr 0x1594348, functor tag 0x15cb1d8, atexit dtor 0xacf0).
inline constexpr std::uint64_t kInit5b9f90BoolVarVptr = 0x1594348;
inline constexpr std::uint64_t kInit5b9f90BoolVarFnVptr = 0x15CB1D8;
inline constexpr std::uint64_t kInit5b9f90BoolVarDtorRva = 0xACF0;

enum class Init5b9f90SliceStatus : std::uint8_t {
    executed,   // fully lifted and executed by run_init_5b9f90_slices
    partial,    // only some of its pages are lifted; the pages carry their own status
    contract,   // named host seam; the callee body is not lifted
    blocked,    // not lifted; reason in `note`
};

struct Init5b9f90Slice {
    const char* id;
    std::uint64_t start_rva;
    std::uint64_t end_rva;   // exclusive
    std::size_t instructions;
    std::uint16_t call_count;
    Init5b9f90SliceStatus status;
    const char* note;
};

// Coarse map (the ten slices in the header comment). Instruction and call counts are measured by
// tools/lift/slice_5b9f90.py; tools/lift/check_init_5b9f90_slices.py re-derives the boundaries from
// the bytes.
// S4.1: the 0x5dbf40 "Particle" page body (7,533 B, 1,243 instructions, 177 direct calls, single
// `ret` at 0x5ddcad, no backward branch). It is decomposed at its constructor clusters: one group,
// seven bounded variables (two 0xc580 ranged ints, five 0xc300 floats) each with a 0x27120 slider,
// and twelve 0x20af0 bool variables. The jne/je pairs in the body are the per-cluster `f8f3d0`
// already-built guards; on the first run every guard is clear, so the observed path visits every
// cluster once.
inline constexpr std::uint64_t kInit5dbf40Rva = 0x5DBF40;
inline constexpr std::uint64_t kInit5dbf40EndRva = 0x5DDCAD;   // the ret byte, exclusive
inline constexpr std::size_t kInit5dbf40Size = 0x1D6D;         // 7,533 B
inline constexpr std::uint64_t kInit5dbf40RetRva = 0x5DDCAD;
inline constexpr std::size_t kInit5dbf40Insns = 1243;
inline constexpr std::uint64_t kInit5dbf40BoolVarFnVptr = 0x15CBDC0;
inline constexpr std::uint64_t kInit5dbf40RangedIntTag = 0x15C8450;   // int-family caller tag

// S5.1: the first page of S5, `0x5bc413..0x5bc78c` (889 B, 149 instructions): group "Stereo 3D"
// under "CP10" and the two bounded floats "Auto Focal Distance" (0..500) and "Auto Distortion"
// (0..1) with their 0x27120 sliders. S5.2 executes the rest of S5.
inline constexpr std::uint64_t kInit5b9f90S5Rva = 0x5BC3DE;
inline constexpr std::uint64_t kInit5b9f90S5_1EndRva = 0x5BC78C;
inline constexpr std::size_t kInit5b9f90S5_1Size = 0x379;   // 889 B
inline constexpr std::uint64_t kInit5b9f90S5_2Rva = 0x5BC78C;
inline constexpr std::uint64_t kInit5b9f90S5_2EndRva = 0x5C1196;
inline constexpr std::size_t kInit5b9f90S5_2Size = 0x4A0A;   // 18,954 B

// S4.2: the 0x7f8930 guard helper (26 instructions, one call at 0x7f895b to 0x7f8610, one `tls`
// canary compare) needs 0x7f8610's 688-instruction body and the two host globals at 0x3288478 and
// 0x3288480; it stays blocked with that measured reason.
inline constexpr std::uint64_t kInit7f8930Rva = 0x7F8930;
inline constexpr std::uint64_t kInit7f8930EndRva = 0x7F898A;
inline constexpr std::uint64_t kInit7f8610Rva = 0x7F8610;

inline constexpr Init5b9f90Slice kInit5b9f90Slices[] = {
    {"S0", 0x5B9F90, 0x5BA041, 33, 3, Init5b9f90SliceStatus::executed,
     "frame, canary, group \"Shadow\" under \"CP10\""},
    {"S1", 0x5BA041, 0x5BA046, 1, 1, Init5b9f90SliceStatus::contract,
     "0x2170 page builder (\"Shadow Misc\"): body not lifted, named host contract"},
    {"S2", 0x5BA046, 0x5BA04B, 1, 1, Init5b9f90SliceStatus::executed,
     "0x5cf140 \"Shadow Credits\" body executed natively (5,526 B)"},
    {"S3", 0x5BA04B, 0x5BC3D4, 1460, 245, Init5b9f90SliceStatus::executed,
     "six pages, all executed; ends at 0x5bc3d4"},
    {"S4", 0x5BC3D4, 0x5BC3DE, 2, 2, Init5b9f90SliceStatus::partial,
     "S4.1 0x5dbf40 executed (7,533 B); S4.2 0x7f8930 blocked on 0x7f8610"},
    {"S5", 0x5BC3DE, 0x5C1196, 3245, 452, Init5b9f90SliceStatus::executed,
     "S5.1 \"Stereo 3D\" + S5.2 \"Dynamic 3D\"/wormhole pages execute (19,896 B incl. prologue)"},
    {"S6", 0x5C1196, 0x5C11A0, 2, 2, Init5b9f90SliceStatus::executed,
     "0x5c94b0 (\"Shadow UI/HUD/Map\") + 0xb25840 page bodies executed"},
    {"S7", 0x5C11A0, 0x5C1D50, 491, 75, Init5b9f90SliceStatus::executed,
     "\"Shadow Sound\" 5 bools, \"Shadow Cursor\" int, \"Layers\" bool"},
    {"S8", 0x5C1D50, 0x5C1E61, 54, 8, Init5b9f90SliceStatus::executed,
     "33-iteration loop over the 0x2dcaa90 name table: 33 layer bools"},
    {"S9", 0x5C1E61, 0x5C56B1, 2356, 347, Init5b9f90SliceStatus::executed,
     "Performance/Transparency/Texture/Fullscreen pages: 40 items"},
};

// S4 is decomposed at its two call sites. S4.1 executes the full 0x5dbf40 "Particle" page body;
// S4.2 is the 0x7f8930 guard helper, which needs 0x7f8610 (688 instructions in its measured window)
// and the host globals 0x3288478/0x3288480, so it stays blocked with that reason.
struct Init5b9f90SubSlice {
    const char* id;
    std::uint64_t call_site_rva;
    std::uint64_t callee_rva;
    std::uint64_t callee_end_rva;
    Init5b9f90SliceStatus status;
    const char* reason;
};

inline constexpr Init5b9f90SubSlice kInit5b9f90SubSlices[] = {
    {"S4.1", 0x5BC3D4, kInit5dbf40Rva, kInit5dbf40EndRva, Init5b9f90SliceStatus::executed,
     "0x5dbf40 \"Particle\" page body, 7,533 B, 1,243 insns, 177 calls, 20 items"},
    {"S4.2", 0x5BC3D9, kInit7f8930Rva, kInit7f8930EndRva, Init5b9f90SliceStatus::blocked,
     "0x7f8930/0x7f8610 label builder needs the debug-tool globals 0x3288478/0x3288480 (host state)"},
    {"S5.1", 0x5BC413, kInit5b9f90S5Rva, kInit5b9f90S5_1EndRva, Init5b9f90SliceStatus::executed,
     "\"Stereo 3D\" page: group + 2 bounded floats/sliders, 889 B, 149 insns"},
    {"S5.2", 0x5BC78C, kInit5b9f90S5_2Rva, kInit5b9f90S5_2EndRva, Init5b9f90SliceStatus::executed,
     "\"Dynamic 3D\" + \"Shadow Boss Worm Hole Effect\": 2 groups, 9 bools, 35 bounded vars, 7 separators, 3 commands"},
};

// One menu item exactly as the body builds it. `caption` is the item text (for a named variable it
// is the variable name, needed by the 0x20af0 ctor before the 0x26db0 item reads it back).
enum class Init5b9f90ItemKind : std::uint8_t {
    command,          // 0x26f90: caption, console string, target tag
    toggle,           // 0x26ea0: caption + a target variable identity
    float_item,       // 0x27050: caption, target tag, [range_min, range_max]
    named_bool_var,   // 0x20af0 + 0x20d60 + 0x26db0: a guarded bool variable and its item
};

struct Init5b9f90ItemSpec {
    Init5b9f90ItemKind kind;
    std::uint64_t site_rva;
    const char* caption;
    std::uint8_t caption_len;
    const char* command;          // command items only (nullptr otherwise)
    std::uint8_t command_len;
    std::uint64_t target_rva;     // command target / toggle variable / float variable / var cell
    float range_min;              // float items only
    float range_max;
    std::uint64_t guard_rva;      // named_bool_var only
};

// Shadow Boss Jump: group at 0x5ba0a3, 16 command items, target 0x2fed5b8.
inline constexpr Init5b9f90ItemSpec kInit5b9f90BossJumpItems[] = {
    {Init5b9f90ItemKind::command, 0x5BA12D, "Boss 1  (Minotaur A)", 20, "0", 1, 0x2FED5B8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BA1B8, "Boss 2  (Mammoth)", 17, "1", 1, 0x2FED5B8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BA243, "Boss 3  (Knight)", 16, "2", 1, 0x2FED5B8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BA2CE, "Boss 4  (Kirin / Giraffe)", 25, "3", 1, 0x2FED5B8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BA359, "Boss 5  (Bird)", 14, "4", 1, 0x2FED5B8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BA3E4, "Boss 6  (Minotaur B)", 20, "5", 1, 0x2FED5B8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BA46F, "Boss 7  (Eel / Unagi)", 21, "6", 1, 0x2FED5B8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BA4FA, "Boss 8  (Gecko / Yamori)", 24, "7", 1, 0x2FED5B8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BA585, "Boss 9  (Tortoise / Kame)", 25, "8", 1, 0x2FED5B8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BA610, "Boss 10 (Narga)", 15, "9", 1, 0x2FED5B8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BA69B, "Boss 11 (Leo)", 13, "10", 2, 0x2FED5B8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BA726, "Boss 12 (Poseidon)", 18, "11", 2, 0x2FED5B8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BA7B1, "Boss 13 (Flying Snake)", 22, "12", 2, 0x2FED5B8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BA83C, "Boss 14 (Cerberus)", 18, "13", 2, 0x2FED5B8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BA8D1, "Boss 15 (Minotaur C)", 20, "14", 2, 0x2FED5B8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BA963, "Boss 16 (Wizard / Evis)", 23, "15", 2, 0x2FED5B8, 0, 0, 0},
};

// Shadow Boss Outro Jump: group at 0x5baab2, 16 command items, target 0x2fed6f8.
inline constexpr Init5b9f90ItemSpec kInit5b9f90BossOutroJumpItems[] = {
    {Init5b9f90ItemKind::command, 0x5BAB3C, "Boss Outro 1  (Minotaur A)", 26, "0", 1, 0x2FED6F8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BABC7, "Boss Outro 2  (Mammoth)", 23, "1", 1, 0x2FED6F8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BAC52, "Boss Outro 3  (Knight)", 22, "2", 1, 0x2FED6F8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BACDD, "Boss Outro 4  (Kirin / Giraffe)", 31, "3", 1, 0x2FED6F8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BAD68, "Boss Outro 5  (Bird)", 20, "4", 1, 0x2FED6F8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BADF3, "Boss Outro 6  (Minotaur B)", 26, "5", 1, 0x2FED6F8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BAE7E, "Boss Outro 7  (Eel / Unagi)", 27, "6", 1, 0x2FED6F8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BAF09, "Boss Outro 8  (Gecko / Yamori)", 30, "7", 1, 0x2FED6F8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BAF94, "Boss Outro 9  (Tortoise / Kame)", 31, "8", 1, 0x2FED6F8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BB01F, "Boss Outro 10 (Narga)", 21, "9", 1, 0x2FED6F8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BB0AA, "Boss Outro 11 (Leo)", 19, "10", 2, 0x2FED6F8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BB135, "Boss Outro 12 (Poseidon)", 24, "11", 2, 0x2FED6F8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BB1C0, "Boss Outro 13 (Flying Snake)", 28, "12", 2, 0x2FED6F8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BB24B, "Boss Outro 14 (Cerberus)", 24, "13", 2, 0x2FED6F8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BB2E0, "Boss Outro 15 (Minotaur C)", 26, "14", 2, 0x2FED6F8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BB372, "Boss Outro 16 (Wizard / Evis)", 29, "15", 2, 0x2FED6F8, 0, 0, 0},
};

// Shadow Cheat: 17 items in body order (8 toggles, 5 commands, 4 float ranges). The float constant
// pair is 0x12662fc = 0.01f (xmm0) and 0x1266300 = 0.1f (xmm1) at every one of the four sites.
inline constexpr Init5b9f90ItemSpec kInit5b9f90CheatItems[] = {
    {Init5b9f90ItemKind::toggle, 0x5BB516, "No_Damage", 9, nullptr, 0, 0x3265A20, 0, 0, 0},
    {Init5b9f90ItemKind::toggle, 0x5BB56E, "Unlimited_Grip", 14, nullptr, 0, 0x324C408, 0, 0, 0},
    {Init5b9f90ItemKind::toggle, 0x5BB5C6, "Max_Attack_Damage", 17, nullptr, 0, 0x3265AE8, 0, 0, 0},
    {Init5b9f90ItemKind::toggle, 0x5BB61E, "Unlock All Gallery Items", 24, nullptr, 0, 0x2FED798, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BB6A8, "Set Game Completed", 18, "1", 1, 0x2FED3D8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BB733, "Clear Game Completed", 20, "0", 1, 0x2FED3D8, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BB7BE, "Set Game Hard Mode", 18, "1", 1, 0x2FED478, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BB849, "Set Game Normal Mode", 20, "0", 1, 0x2FED478, 0, 0, 0},
    {Init5b9f90ItemKind::command, 0x5BB8D1, "Set Game Easy Mode", 18, "1", 1, 0x2FED518, 0, 0, 0},
    {Init5b9f90ItemKind::toggle, 0x5BB931, "Give All Items", 14, nullptr, 0, 0x2FED0B8, 0, 0, 0},
    {Init5b9f90ItemKind::toggle, 0x5BB989, "Equip All Items", 15, nullptr, 0, 0x2FED298, 0, 0, 0},
    {Init5b9f90ItemKind::toggle, 0x5BB9E1, "Unequip All Items", 17, nullptr, 0, 0x2FED338, 0, 0, 0},
    {Init5b9f90ItemKind::toggle, 0x5BBA39, "Enable Debug Fly", 16, nullptr, 0, 0x325DD78, 0, 0, 0},
    {Init5b9f90ItemKind::float_item, 0x5BBAA1, "30Hz Acceleration Scalar", 24, nullptr, 0, 0x300FEA8,
     0.01F, 0.1F, 0},
    {Init5b9f90ItemKind::float_item, 0x5BBB0C, "30Hz Acceleration Scalar Easy Mode", 34, nullptr, 0,
     0x300FF80, 0.01F, 0.1F, 0},
    {Init5b9f90ItemKind::float_item, 0x5BBB77, "60Hz Acceleration Scalar", 24, nullptr, 0, 0x3010058,
     0.01F, 0.1F, 0},
    {Init5b9f90ItemKind::float_item, 0x5BBBE2, "60Hz Acceleration Scalar Easy Mode", 34, nullptr, 0,
     0x3010130, 0.01F, 0.1F, 0},
};

// Shadow Trophy: four guarded bool variables, each with its own item.
inline constexpr Init5b9f90ItemSpec kInit5b9f90TrophyItems[] = {
    {Init5b9f90ItemKind::named_bool_var, 0x5BBDCE, "Show Remaining Fruit Trees", 26, nullptr, 0,
     0x2FED838, 0, 0, 0x2FED8F8},
    {Init5b9f90ItemKind::named_bool_var, 0x5BBF40, "Log Gex Tails Cut", 17, nullptr, 0, 0x2FED900, 0,
     0, 0x2FED9C0},
    {Init5b9f90ItemKind::named_bool_var, 0x5BC0B2, "Debug Horse Stunts", 18, nullptr, 0, 0x2FED9C8, 0,
     0, 0x2FEDA88},
    {Init5b9f90ItemKind::named_bool_var, 0x5BC224, "Show Visited Save Points", 24, nullptr, 0,
     0x2FEDA90, 0, 0, 0x2FEDB50},
};

inline constexpr std::size_t kInit5b9f90BossJumpItemsCount =
    sizeof(kInit5b9f90BossJumpItems) / sizeof(kInit5b9f90BossJumpItems[0]);
inline constexpr std::size_t kInit5b9f90BossOutroJumpItemsCount =
    sizeof(kInit5b9f90BossOutroJumpItems) / sizeof(kInit5b9f90BossOutroJumpItems[0]);
inline constexpr std::size_t kInit5b9f90CheatItemsCount =
    sizeof(kInit5b9f90CheatItems) / sizeof(kInit5b9f90CheatItems[0]);
inline constexpr std::size_t kInit5b9f90TrophyItemsCount =
    sizeof(kInit5b9f90TrophyItems) / sizeof(kInit5b9f90TrophyItems[0]);

// The six pages inside S3, in body order. `start` includes the page-name view construction.
// `destroy_count` is the number of 0x272d0 releases the body issues after the page (items first, in
// reverse order, then the group); 0 means the page's items are left standing.
struct Init5b9f90PageSpec {
    const char* id;
    const char* parent;        // parent path view (literal)
    std::uint8_t parent_len;
    const char* caption;       // group caption view (literal)
    std::uint8_t caption_len;
    std::uint64_t start_rva;
    std::uint64_t end_rva;     // exclusive
    std::uint64_t group_site_rva;
    const Init5b9f90ItemSpec* items;
    std::uint16_t item_count;
    std::uint16_t destroy_count;
    Init5b9f90SliceStatus status;
};

inline constexpr Init5b9f90PageSpec kInit5b9f90Pages[] = {
    {"S3.1", "Shadow", 6, "Shadow Boss Jump", 16, 0x5BA04B, 0x5BAAB2, 0x5BA0A3,
     kInit5b9f90BossJumpItems, 16, 0, Init5b9f90SliceStatus::executed},
    {"S3.2", "Shadow", 6, "Shadow Boss Outro Jump", 22, 0x5BAAB2, 0x5BB4BE, 0x5BAAB2,
     kInit5b9f90BossOutroJumpItems, 16, 0, Init5b9f90SliceStatus::executed},
    {"S3.3", "CP10", 4, "Shadow Cheat", 12, 0x5BB4BE, 0x5BBD2E, 0x5BB4BE, kInit5b9f90CheatItems,
     17, 18, Init5b9f90SliceStatus::executed},
    {"S3.4", "Shadow", 6, "Shadow Trophy", 13, 0x5BBD2E, 0x5BC377, 0x5BBD2E,
     kInit5b9f90TrophyItems, 4, 5, Init5b9f90SliceStatus::executed},
    {"S3.5", "Shadow", 6, "Shadow Render", 13, 0x5BC377, 0x5BC3C7, 0x5BC377, nullptr, 0, 0,
     Init5b9f90SliceStatus::executed},
    {"S3.6", "Shadow Render", 13, "MLAA", 4, 0x5BC3C7, 0x5BC3D4, 0x5BC3C7, nullptr, 0, 0,
     Init5b9f90SliceStatus::executed},
};

// S2: the 0x5cf140 "Shadow Credits" page builder, executed natively as part of the same run.
// Body 0x5cf140..0x5d06d6 (end-exclusive 0x5d06d6, 5,526 B), single `ret` at 0x5d06d6, 932
// instructions, 132 direct calls, no backward branch. Its six separators, three named variables
// (one bool var built by 0x5d41c0 named `cp10_creditsUseNewList`), ten bounded float variables
// built by 0xc300 and ten 0x27120 sliders are modelled with the same ctors as S3.
inline constexpr std::uint64_t kInit5cf140Rva = 0x5CF140;
inline constexpr std::uint64_t kInit5cf140EndRva = 0x5D06D6;
inline constexpr std::size_t kInit5cf140Size = 0x1596;   // 5,526 B
inline constexpr std::uint64_t kInit5cf140RetRva = 0x5D06D6;
inline constexpr std::uint64_t kInit5cf140BoolVarDtorRva = 0xAD30;   // ranged-var atexit dtor



enum class Init5cf140ItemKind : std::uint8_t {
    group,
    command,
    separator,
    named_external,   // 0x26db0 over a variable constructed by unlifted code (name read from the ELF)
    bool_var,         // 0x20af0 + 0x20d60 + 0x26db0 (same class as S3.4)
    ranged_slider,    // 0xc300 bounded float variable + 0x27120 slider
    ranged_int_slider,  // 0xc580 bounded int variable + 0x27120 slider (S4.1)
};

struct Init5cf140ItemSpec {
    Init5cf140ItemKind kind;
    std::uint64_t site_rva;      // the constructor call (0x20af0 / 0xc300 for the variable kinds)
    const char* parent;
    std::uint8_t parent_len;
    const char* text;            // caption, or the variable name for the variable kinds
    std::uint8_t text_len;
    const char* command;         // command items only
    std::uint8_t command_len;
    std::uint64_t target_rva;    // command target / variable cell
    std::uint64_t guard_rva;     // bool_var / ranged_slider guard byte
    float lo, hi;                // 0x27120 slider range
    float var_lo, var_hi;        // 0xc300 bounded range
    std::uint64_t slider_site_rva;
};

inline constexpr Init5cf140ItemSpec kInit5cf140Items[] = {
    {Init5cf140ItemKind::group, 0x5CF1C6, "Shadow", 6, "Shadow Credits", 14, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::command, 0x5CF23A, "Shadow Credits", 14, "Jump to credits level", 21,
     "BP_JumpToCredits", 16, 0x2DFCBD0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::separator, 0x5CF261, "Shadow Credits", 14, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::named_external, 0x5CF27D, "Shadow Credits", 14, "cp10_creditsUseNewList",
     22, nullptr, 0, 0x2FF59C0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5CF315, "Shadow Credits", 14, "Apply adjustments", 17, nullptr, 0,
     0x2FF5A90, 0x2FF5B50, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5CF487, "Shadow Credits", 14, "Credit Pause", 12, nullptr, 0,
     0x2FF5B58, 0x2FF5C18, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::separator, 0x5CF57B, "Shadow Credits", 14, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::separator, 0x5CF592, "Shadow Credits", 14, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_slider, 0x5CF69E, "Shadow Credits", 14, "Font Scale", 10, nullptr, 0,
     0x2FF5C20, 0x2FF5CF0, 0.01F, 0.1F, 0.0F, 4.0F, 0x5CF73D},
    {Init5cf140ItemKind::separator, 0x5CF75C, "Shadow Credits", 14, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_slider, 0x5CF868, "Shadow Credits", 14, "Scroller Speed", 14, nullptr, 0,
     0x2FF5CF8, 0x2FF5DC8, 0.1F, 1.0F, -20.0F, 20.0F, 0x5CF904},
    {Init5cf140ItemKind::ranged_slider, 0x5CFA09, "Shadow Credits", 14, "Scroller Width", 14, nullptr, 0,
     0x2FF5DD0, 0x2FF5EA0, 0.01F, 0.1F, -2.0F, 2.0F, 0x5CFAAA},
    {Init5cf140ItemKind::ranged_slider, 0x5CFBAF, "Shadow Credits", 14, "Scroller Offset", 15, nullptr, 0,
     0x2FF5EA8, 0x2FF5F78, 0.01F, 0.1F, -2.0F, 2.0F, 0x5CFC50},
    {Init5cf140ItemKind::ranged_slider, 0x5CFD55, "Shadow Credits", 14, "Scroller Left Line Delay", 24,
     nullptr, 0, 0x2FF5F80, 0x2FF6050, 0.5F, 10.0F, 0.0F, 1000.0F, 0x5CFDF6},
    {Init5cf140ItemKind::ranged_slider, 0x5CFEFC, "Shadow Credits", 14, "Scroller Right Line Delay", 25,
     nullptr, 0, 0x2FF6058, 0x2FF6128, 0.5F, 10.0F, 0.0F, 10000.0F, 0x5CFFA0},
    {Init5cf140ItemKind::separator, 0x5CFFBF, "Shadow Credits", 14, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_slider, 0x5D00BF, "Shadow Credits", 14, "Fade top offset", 15, nullptr, 0,
     0x2FF6130, 0x2FF6200, 0.01F, 0.1F, -1.0F, 1.0F, 0x5D015B},
    {Init5cf140ItemKind::ranged_slider, 0x5D0248, "Shadow Credits", 14, "Fade top height", 15, nullptr, 0,
     0x2FF6208, 0x2FF62D8, 0.01F, 0.1F, 0.0F, 1.0F, 0x5D02EC},
    {Init5cf140ItemKind::separator, 0x5D030B, "Shadow Credits", 14, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_slider, 0x5D03FF, "Shadow Credits", 14, "Fade bottom offset", 18,
     nullptr, 0, 0x2FF62E0, 0x2FF63B0, 0.01F, 0.1F, -1.0F, 1.0F, 0x5D049B},
    {Init5cf140ItemKind::ranged_slider, 0x5D0586, "Shadow Credits", 14, "Fade bottom height", 18,
     nullptr, 0, 0x2FF63B8, 0x2FF6488, 0.01F, 0.1F, 0.0F, 1.0F, 0x5D062A},
};

inline constexpr std::size_t kInit5cf140ItemsCount =
    sizeof(kInit5cf140Items) / sizeof(kInit5cf140Items[0]);

// S4.1 item table, in body order. `slider_site_rva` is the 0x27120 call for the bounded variables
// and the 0x26db0 call for the bool variables. The group's page guard (0x3004a50) and the last bool
// variable's guard are recorded in the map; the host owns all guard storage.
inline constexpr Init5cf140ItemSpec kInit5dbf40Items[] = {
    {Init5cf140ItemKind::group, 0x5DBFAF, "Shadow Render", 13, "Particle", 8, nullptr, 0, 0x3004A50, 0,
     0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_int_slider, 0x5DC0BB, "Particle", 8, "Soft Particle Mode", 18, nullptr, 0,
     0x3004980, 0x3004B28, 1.0F, 1.0F, -1.0F, 2.0F, 0x5DC153},
    {Init5cf140ItemKind::ranged_slider, 0x5DC258, "Particle", 8, "Soft Particle Fixed Fade", 24, nullptr,
     0, 0x3004A58, 0x3004C00, 1.0F, 1.0F, 0.05F, 5.0F, 0x5DC2F9},
    {Init5cf140ItemKind::ranged_slider, 0x5DC3FE, "Particle", 8, "Refract Min Fade", 16, nullptr, 0,
     0x3004B30, 0x3004CD8, 1.0F, 1.0F, 0.0F, 50.0F, 0x5DC49F},
    {Init5cf140ItemKind::ranged_slider, 0x5DC5A4, "Particle", 8, "Refract Max Fade", 16, nullptr, 0,
     0x3004C08, 0x3004DB0, 1.0F, 1.0F, 0.01F, 50.0F, 0x5DC645},
    {Init5cf140ItemKind::ranged_slider, 0x5DC74A, "Particle", 8, "Camera Min Fade", 15, nullptr, 0,
     0x3004CE0, 0x3004E88, 1.0F, 1.0F, 0.0F, 50.0F, 0x5DC7EB},
    {Init5cf140ItemKind::ranged_slider, 0x5DC8F0, "Particle", 8, "Camera Max Fade", 15, nullptr, 0,
     0x3004DB8, 0x3004F50, 1.0F, 1.0F, 0.01F, 50.0F, 0x5DC991},
    {Init5cf140ItemKind::bool_var, 0x5DCA31, "Particle", 8, "Draw Extend", 11, nullptr, 0, 0x3004E90,
     0x3005018, 0, 0, 0, 0, 0x5DCB03},
    {Init5cf140ItemKind::bool_var, 0x5DCBA3, "Particle", 8, "Draw Extend2", 12, nullptr, 0, 0x3004F58,
     0x30050E0, 0, 0, 0, 0, 0x5DCC75},
    {Init5cf140ItemKind::bool_var, 0x5DCD15, "Particle", 8, "Draw Blur0", 10, nullptr, 0, 0x3005020,
     0x30051A8, 0, 0, 0, 0, 0x5DCDE7},
    {Init5cf140ItemKind::bool_var, 0x5DCE87, "Particle", 8, "Draw Blur1", 10, nullptr, 0, 0x30050E8,
     0x3005270, 0, 0, 0, 0, 0x5DCF59},
    {Init5cf140ItemKind::bool_var, 0x5DCFF9, "Particle", 8, "Draw FakeVol", 12, nullptr, 0, 0x30051B0,
     0x3005338, 0, 0, 0, 0, 0x5DD0CB},
    {Init5cf140ItemKind::bool_var, 0x5DD16B, "Particle", 8, "Draw Anim", 9, nullptr, 0, 0x3005278,
     0x3005400, 0, 0, 0, 0, 0x5DD23D},
    {Init5cf140ItemKind::bool_var, 0x5DD2DD, "Particle", 8, "Draw RandAxis", 13, nullptr, 0, 0x3005340,
     0x30054C8, 0, 0, 0, 0, 0x5DD3AF},
    {Init5cf140ItemKind::bool_var, 0x5DD44F, "Particle", 8, "Draw Refract", 12, nullptr, 0, 0x3005408,
     0x30055A0, 0, 0, 0, 0, 0x5DD521},
    {Init5cf140ItemKind::ranged_int_slider, 0x5DD626, "Particle", 8, "Debug Particle Index", 20, nullptr,
     0, 0x30054D0, 0x3005668, 1.0F, 1.0F, -1.0F, 100.0F, 0x5DD6C3},
    {Init5cf140ItemKind::bool_var, 0x5DD757, "Particle", 8, "Trim Particles", 14, nullptr, 0, 0x30055A8,
     0x3005730, 0, 0, 0, 0, 0x5DD829},
    {Init5cf140ItemKind::bool_var, 0x5DD8BD, "Particle", 8, "Optimize Identical", 18, nullptr, 0,
     0x3005670, 0x30057F8, 0, 0, 0, 0, 0x5DD98F},
    {Init5cf140ItemKind::bool_var, 0x5DDA23, "Particle", 8, "Optimize Null", 13, nullptr, 0, 0x3005738,
     0x30058C0, 0, 0, 0, 0, 0x5DDAF5},
    {Init5cf140ItemKind::bool_var, 0x5DDB87, "Particle", 8, "Particle Offscreen Render", 25, nullptr,
     0, 0x3005800, 0, 0, 0, 0, 0, 0x5DDC5C},
};

inline constexpr std::size_t kInit5dbf40ItemsCount =
    sizeof(kInit5dbf40Items) / sizeof(kInit5dbf40Items[0]);

inline constexpr Init5cf140ItemSpec kInit5b9f90S5_1Items[] = {
    {Init5cf140ItemKind::group, 0x5BC413, "CP10", 4, "Stereo 3D", 9, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_slider, 0x5BC51F, "Stereo 3D", 9, "Auto Focal Distance", 19, nullptr, 0,
     0x2FEDB58, 0x2FEDC28, 1.0F, 1.0F, 0.0F, 500.0F, 0x5BC5B2},
    {Init5cf140ItemKind::ranged_slider, 0x5BC6B7, "Stereo 3D", 9, "Auto Distortion", 15, nullptr, 0,
     0x2FEDC30, 0x2FEDD00, 1.0F, 1.0F, 0.0F, 1.0F, 0x5BC753},
};

inline constexpr std::size_t kInit5b9f90S5_1ItemsCount =
    sizeof(kInit5b9f90S5_1Items) / sizeof(kInit5b9f90S5_1Items[0]);

// S5.2: the 0x5bc78c..0x5c1196 "Dynamic 3D" and "Shadow Boss Worm Hole Effect" pages
// (18,954 B, 3,085 instructions): 2 groups, 9 bool variables, 35 bounded variables + sliders
// (28 0xc300 floats, 7 0xc580 ints), 7 separators and 3 0x26f90 keyframe commands.

inline constexpr Init5cf140ItemSpec kInit5b9f90S5_2Items[] = {
    {Init5cf140ItemKind::group, 0x5BC78C, "Stereo 3D", 9, "Dynamic 3D", 10, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5BC824, "Stereo 3D", 9, "Focus On Objects", 16, nullptr, 0, 0x2fedd08, 0x2fede90, 0, 0, 0, 0, 0x5BC8F6},
    {Init5cf140ItemKind::bool_var, 0x5BC996, "Stereo 3D", 9, "Show Crosshair in 3D", 20, nullptr, 0, 0x2feddd0, 0x2fedf58, 0, 0, 0, 0, 0x5BCA68},
    {Init5cf140ItemKind::bool_var, 0x5BCB08, "Stereo 3D", 9, "Dynamic S3D", 11, nullptr, 0, 0x2fede98, 0x2fee020, 0, 0, 0, 0, 0x5BCBDA},
    {Init5cf140ItemKind::bool_var, 0x5BCC7A, "Stereo 3D", 9, "Clamp Max Separation", 20, nullptr, 0, 0x2fedf60, 0x2fee0e8, 0, 0, 0, 0, 0x5BCD4C},
    {Init5cf140ItemKind::bool_var, 0x5BCDEC, "Stereo 3D", 9, "Debug Clamp Max Separation", 26, nullptr, 0, 0x2fee028, 0x2fee1c0, 0, 0, 0, 0, 0x5BCEBE},
    {Init5cf140ItemKind::ranged_slider, 0x5BCFC3, "Stereo 3D", 9, "Underwater Distortion Scale", 27, nullptr, 0, 0x2fee0f0, 0x2fee298, 1.0F, 1.0F, 0.0F, 1.0F, 0x5BD067},
    {Init5cf140ItemKind::separator, 0x5BD086, "Stereo 3D", 9, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_int_slider, 0x5BD192, "Stereo 3D", 9, "Extraction border size", 22, nullptr, 0, 0x2fee1c8, 0x2fee370, 1.0F, 1.0F, 16, 256, 0x5BD229},
    {Init5cf140ItemKind::ranged_slider, 0x5BD32E, "Stereo 3D", 9, "Extraction border weight", 24, nullptr, 0, 0x2fee2a0, 0x2fee448, 1.0F, 1.0F, 0.0F, 10.0F, 0x5BD3CF},
    {Init5cf140ItemKind::ranged_slider, 0x5BD4D4, "Stereo 3D", 9, "Auto Distance Step%", 19, nullptr, 0, 0x2fee378, 0x2fee520, 1.0F, 1.0F, 0.0F, 1.0F, 0x5BD575},
    {Init5cf140ItemKind::ranged_slider, 0x5BD67A, "Stereo 3D", 9, "Auto Distortion Step%", 21, nullptr, 0, 0x2fee450, 0x2fee5f8, 1.0F, 1.0F, 0.0F, 1.0F, 0x5BD71E},
    {Init5cf140ItemKind::separator, 0x5BD73D, "Stereo 3D", 9, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_slider, 0x5BD849, "Stereo 3D", 9, "Near Focal Distance", 19, nullptr, 0, 0x2fee528, 0x2fee6d0, 1.0F, 1.0F, 0.0F, 100.0F, 0x5BD8E4},
    {Init5cf140ItemKind::ranged_slider, 0x5BD9E9, "Stereo 3D", 9, "Near Weight 1", 13, nullptr, 0, 0x2fee600, 0x2fee7a8, 1.0F, 1.0F, 0.0F, 20.0F, 0x5BDA8A},
    {Init5cf140ItemKind::ranged_slider, 0x5BDB90, "Stereo 3D", 9, "Near Weight 2", 13, nullptr, 0, 0x2fee6d8, 0x2fee880, 1.0F, 1.0F, 0.0F, 20.0F, 0x5BDC31},
    {Init5cf140ItemKind::ranged_slider, 0x5BDD37, "Stereo 3D", 9, "Near Weight 3", 13, nullptr, 0, 0x2fee7b0, 0x2fee958, 1.0F, 1.0F, 0.0F, 20.0F, 0x5BDDD8},
    {Init5cf140ItemKind::ranged_slider, 0x5BDEDD, "Stereo 3D", 9, "Near Distortion Near", 20, nullptr, 0, 0x2fee888, 0x2feea30, 1.0F, 1.0F, 0.0F, 1.0F, 0x5BDF7E},
    {Init5cf140ItemKind::ranged_slider, 0x5BE084, "Stereo 3D", 9, "Near Distortion Mid", 19, nullptr, 0, 0x2fee960, 0x2feeb08, 1.0F, 1.0F, 0.0F, 1.0F, 0x5BE125},
    {Init5cf140ItemKind::ranged_slider, 0x5BE22B, "Stereo 3D", 9, "Near Distortion Far", 19, nullptr, 0, 0x2feea38, 0x2feebe0, 1.0F, 1.0F, 0.0F, 1.0F, 0x5BE2CF},
    {Init5cf140ItemKind::separator, 0x5BE2EE, "Stereo 3D", 9, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_slider, 0x5BE3FC, "Stereo 3D", 9, "Far Focal Distance", 18, nullptr, 0, 0x2feeb10, 0x2feecb8, 1.0F, 1.0F, 0.0F, 100.0F, 0x5BE499},
    {Init5cf140ItemKind::ranged_slider, 0x5BE59F, "Stereo 3D", 9, "Far Weight 1", 12, nullptr, 0, 0x2feebe8, 0x2feed90, 1.0F, 1.0F, 0.0F, 20.0F, 0x5BE640},
    {Init5cf140ItemKind::ranged_slider, 0x5BE746, "Stereo 3D", 9, "Far Weight 2", 12, nullptr, 0, 0x2feecc0, 0x2feee68, 1.0F, 1.0F, 0.0F, 20.0F, 0x5BE7E7},
    {Init5cf140ItemKind::ranged_slider, 0x5BE8ED, "Stereo 3D", 9, "Far Weight 3", 12, nullptr, 0, 0x2feed98, 0x2feef40, 1.0F, 1.0F, 0.0F, 20.0F, 0x5BE98E},
    {Init5cf140ItemKind::ranged_slider, 0x5BEA94, "Stereo 3D", 9, "Far Distortion Near", 19, nullptr, 0, 0x2feee70, 0x2fef018, 1.0F, 1.0F, 0.0F, 1.0F, 0x5BEB35},
    {Init5cf140ItemKind::ranged_slider, 0x5BEC3B, "Stereo 3D", 9, "Far Distortion Mid", 18, nullptr, 0, 0x2feef48, 0x2fef0f0, 1.0F, 1.0F, 0.0F, 1.0F, 0x5BECDC},
    {Init5cf140ItemKind::ranged_slider, 0x5BEDE2, "Stereo 3D", 9, "Far Distortion Far", 18, nullptr, 0, 0x2fef020, 0x2fef1c8, 1.0F, 1.0F, 0.0F, 1.0F, 0x5BEE86},
    {Init5cf140ItemKind::separator, 0x5BEEE9, "Stereo 3D", 9, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_int_slider, 0x5BEFE6, "Stereo 3D", 9, "Cinematic Control", 17, nullptr, 0, 0x2fef0f8, 0x2fef2a0, 1.0F, 1.0F, 0, 1, 0x5BF07E},
    {Init5cf140ItemKind::ranged_slider, 0x5BF183, "Stereo 3D", 9, "Cinematic Focal Distance", 24, nullptr, 0, 0x2fef1d0, 0x2fef378, 1.0F, 1.0F, 0.0F, 500.0F, 0x5BF21B},
    {Init5cf140ItemKind::ranged_slider, 0x5BF320, "Stereo 3D", 9, "Cinematic Distortion", 20, nullptr, 0, 0x2fef2a8, 0x2fef450, 1.0F, 1.0F, 0.0F, 1.0F, 0x5BF3B8},
    {Init5cf140ItemKind::ranged_slider, 0x5BF4BD, "Stereo 3D", 9, "Cinematic FOV Adjustment", 24, nullptr, 0, 0x2fef380, 0x2fef528, 1.0F, 1.0F, -3.0F, 3.0F, 0x5BF555},
    {Init5cf140ItemKind::ranged_slider, 0x5BF65A, "Stereo 3D", 9, "Cinematic Near Plane Adj", 24, nullptr, 0, 0x2fef458, 0x2fef600, 1.0F, 1.0F, 0.05000000074505806F, 10.0F, 0x5BF6F2},
    {Init5cf140ItemKind::ranged_int_slider, 0x5BF7F7, "Stereo 3D", 9, "Cinematic Character Motion Blur", 31, nullptr, 0, 0x2fef530, 0x2fef6c8, 1.0F, 1.0F, 0, 2, 0x5BF896},
    {Init5cf140ItemKind::separator, 0x5BF8B5, "Stereo 3D", 9, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5BF94D, "Stereo 3D", 9, "Manual Control", 14, nullptr, 0, 0x2fef608, 0x2fef7a0, 0, 0, 0, 0, 0x5BFA1F},
    {Init5cf140ItemKind::ranged_slider, 0x5BFB24, "Stereo 3D", 9, "Manual Focal Distance", 21, nullptr, 0, 0x2fef6d0, 0x2fef878, 1.0F, 1.0F, 0.0F, 500.0F, 0x5BFBC5},
    {Init5cf140ItemKind::ranged_slider, 0x5BFCCA, "Stereo 3D", 9, "Manual Distortion", 17, nullptr, 0, 0x2fef7a8, 0x2fef950, 1.0F, 1.0F, 0.0F, 1.0F, 0x5BFD6B},
    {Init5cf140ItemKind::ranged_slider, 0x5BFE70, "Stereo 3D", 9, "Manual FOV Adjustment", 21, nullptr, 0, 0x2fef880, 0x2fefa28, 1.0F, 1.0F, -3.0F, 3.0F, 0x5BFF11},
    {Init5cf140ItemKind::ranged_slider, 0x5C0016, "Stereo 3D", 9, "Manual Near Plane Adj", 21, nullptr, 0, 0x2fef958, 0x2fefb00, 1.0F, 1.0F, 0.05000000074505806F, 10.0F, 0x5C00B7},
    {Init5cf140ItemKind::ranged_int_slider, 0x5C01BC, "Stereo 3D", 9, "Manual Character Motion Blur", 28, nullptr, 0, 0x2fefa30, 0x0, 1.0F, 1.0F, 0, 2, 0x5C025C},
    {Init5cf140ItemKind::separator, 0x5C027B, "Stereo 3D", 9, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::command, 0x5C02FD, "Stereo 3D", 9, "Create Keyframe", 15, "BP_CreateCinematicKeyframe", 26, 0x2DFCBD0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::command, 0x5C0388, "Stereo 3D", 9, "Save Keyframe File", 18, "BP_SaveCinematicKeyframes", 25, 0x2DFCBD0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5C0430, "Stereo 3D", 9, "Pause on First Frame", 20, nullptr, 0, 0x2fefb08, 0x2fefc90, 0, 0, 0, 0, 0x5C0502},
    {Init5cf140ItemKind::bool_var, 0x5C05A2, "Stereo 3D", 9, "Pause on Last Frame", 19, nullptr, 0, 0x2fefbd0, 0x2fefd58, 0, 0, 0, 0, 0x5C0674},
    {Init5cf140ItemKind::bool_var, 0x5C0714, "Stereo 3D", 9, "Pause on EVERY Frame", 20, nullptr, 0, 0x2fefc98, 0x0, 0, 0, 0, 0, 0x5C07E9},
    {Init5cf140ItemKind::command, 0x5C0870, "Stereo 3D", 9, "Reset All Keyframes", 19, "BP_DeleteCinematicKeyframes", 27, 0x2DFCBD0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::separator, 0x5C0897, "Stereo 3D", 9, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_int_slider, 0x5C09A3, "Stereo 3D", 9, "Cinematic Camera Stage", 22, nullptr, 0, 0x2fefd60, 0x2feff08, 1.0F, 1.0F, -1.0F, 2147483647.0F, 0x5C0A37},
    {Init5cf140ItemKind::ranged_int_slider, 0x5C0B3C, "Stereo 3D", 9, "Cinematic Camera Frame", 22, nullptr, 0, 0x2fefe38, 0x2feffe0, 1.0F, 1.0F, 0.0F, 2147483647.0F, 0x5C0BD5},
    {Init5cf140ItemKind::ranged_int_slider, 0x5C0CDA, "Stereo 3D", 9, "Cinematic Camera Frame Max", 26, nullptr, 0, 0x2feff10, 0x2ff00b8, 1.0F, 1.0F, 0.0F, 2147483647.0F, 0x5C0D76},
    {Init5cf140ItemKind::group, 0x5C0DAF, "Stereo 3D", 9, "Shadow Boss Worm Hole Effect", 28, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_slider, 0x5C0EBB, "Stereo 3D", 9, "Focal Distance", 14, nullptr, 0, 0x2feffe8, 0x2ff0190, 1.0F, 1.0F, -500.0F, 500.0F, 0x5C0F56},
    {Init5cf140ItemKind::ranged_slider, 0x5C105B, "Stereo 3D", 9, "Distortion", 10, nullptr, 0, 0x2ff00c0, 0x0, 1.0F, 1.0F, 0.0F, 1.0F, 0x5C10FF},
};

inline constexpr std::size_t kInit5b9f90S5_2ItemsCount =
    sizeof(kInit5b9f90S5_2Items) / sizeof(kInit5b9f90S5_2Items[0]);

// Init5c94b0Items: 0x5c94b0..0x5cf12a (23674 B), 76 items
inline constexpr std::uint64_t kInit5c94b0Rva = 0x5C94B0;
inline constexpr std::uint64_t kInit5c94b0EndRva = 0x5CF12A;
inline constexpr std::size_t kInit5c94b0Size = 23674;

inline constexpr Init5cf140ItemSpec Init5c94b0Items[] = {
    {Init5cf140ItemKind::group, 0x5C9536, "Shadow", 6, "Shadow UI", 9, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_slider, 0x5C964A, "Shadow UI", 9, "Save/Load Dialog - Game Time - Spacing", 38, nullptr, 0, 0x2ff2b60, 0x2ff2cf8, 1.0F, 1.0F, 0.0F, 1.0F, 0x5C96E9},
    {Init5cf140ItemKind::separator, 0x5C9708, "Shadow UI", 9, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5C97A0, "Shadow UI", 9, "Save/Load Dialog - Game Time - Fake Enable", 42, nullptr, 0, 0x2ff2c38, 0x2ff2dd0, 0, 0, 0, 0, 0x5C9872},
    {Init5cf140ItemKind::ranged_int_slider, 0x5C9977, "Shadow UI", 9, "Save/Load Dialog - Game Time - Fake Hours", 41, nullptr, 0, 0x2ff2d00, 0x2ff2ea8, 1.0F, 1.0F, 0.0F, 999.0F, 0x5C9A18},
    {Init5cf140ItemKind::ranged_int_slider, 0x5C9B1D, "Shadow UI", 9, "Save/Load Dialog - Game Time - Fake Mins", 40, nullptr, 0, 0x2ff2dd8, 0x2ff2f80, 1.0F, 1.0F, 0.0F, 59.0F, 0x5C9BBE},
    {Init5cf140ItemKind::ranged_int_slider, 0x5C9CC3, "Shadow UI", 9, "Save/Load Dialog - Game Time - Fake Secs", 40, nullptr, 0, 0x2ff2eb0, 0x2ff3048, 1.0F, 1.0F, 0.0F, 59.0F, 0x5C9D67},
    {Init5cf140ItemKind::group, 0x5C9DF2, "Shadow", 6, "Shadow HUD", 10, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5C9E92, "Shadow HUD", 10, "Apply pixel 1:1 scale", 21, nullptr, 0, 0x2ff2f88, 0x2ff3110, 0, 0, 0, 0, 0x5C9F64},
    {Init5cf140ItemKind::bool_var, 0x5CA004, "Shadow HUD", 10, "Show grid textures", 18, nullptr, 0, 0x2ff3050, 0x2ff31d8, 0, 0, 0, 0, 0x5CA0D9},
    {Init5cf140ItemKind::separator, 0x5CA0F8, "Shadow HUD", 10, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5CA190, "Shadow HUD", 10, "Weapon sight enable pixel snap", 30, nullptr, 0, 0x2ff3118, 0x2ff32a0, 0, 0, 0, 0, 0x5CA262},
    {Init5cf140ItemKind::bool_var, 0x5CA302, "Shadow HUD", 10, "Weapon sight linear texture filter", 34, nullptr, 0, 0x2ff31e0, 0x2ff3368, 0, 0, 0, 0, 0x5CA3D4},
    {Init5cf140ItemKind::bool_var, 0x5CA474, "Shadow HUD", 10, "Weapon sight apply stereo 3d", 28, nullptr, 0, 0x2ff32a8, 0x2ff3440, 0, 0, 0, 0, 0x5CA546},
    {Init5cf140ItemKind::ranged_slider, 0x5CA64C, "Shadow HUD", 10, "Weapon Sight Scale X", 20, nullptr, 0, 0x2ff3370, 0x2ff3518, 1.0F, 1.0F, 0.10000000149011612F, 4.0F, 0x5CA6ED},
    {Init5cf140ItemKind::ranged_slider, 0x5CA7F3, "Shadow HUD", 10, "Weapon Sight Scale Y", 20, nullptr, 0, 0x2ff3448, 0x2ff35e0, 1.0F, 1.0F, 0.10000000149011612F, 4.0F, 0x5CA897},
    {Init5cf140ItemKind::separator, 0x5CA8B6, "Shadow HUD", 10, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5CA94E, "Shadow HUD", 10, "Health bar apply fixup", 22, nullptr, 0, 0x2ff3520, 0x2ff36a8, 0, 0, 0, 0, 0x5CAA20},
    {Init5cf140ItemKind::bool_var, 0x5CAAC0, "Shadow HUD", 10, "Health bar show glow", 20, nullptr, 0, 0x2ff35e8, 0x2ff3770, 0, 0, 0, 0, 0x5CAB95},
    {Init5cf140ItemKind::separator, 0x5CABB4, "Shadow HUD", 10, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5CAC4C, "Shadow HUD", 10, "Icon enable pixel snap", 22, nullptr, 0, 0x2ff36b0, 0x2ff3838, 0, 0, 0, 0, 0x5CAD1E},
    {Init5cf140ItemKind::bool_var, 0x5CADBE, "Shadow HUD", 10, "Icon linear texture filter", 26, nullptr, 0, 0x2ff3778, 0x2ff3910, 0, 0, 0, 0, 0x5CAE90},
    {Init5cf140ItemKind::ranged_slider, 0x5CAF95, "Shadow HUD", 10, "Icon size scale x", 17, nullptr, 0, 0x2ff3840, 0x2ff39e8, 1.0F, 1.0F, 0.10000000149011612F, 4.0F, 0x5CB036},
    {Init5cf140ItemKind::ranged_slider, 0x5CB13C, "Shadow HUD", 10, "Icon size scale y", 17, nullptr, 0, 0x2ff3918, 0x2ff3ac0, 1.0F, 1.0F, 0.10000000149011612F, 4.0F, 0x5CB1DD},
    {Init5cf140ItemKind::ranged_slider, 0x5CB2E3, "Shadow HUD", 10, "Icon offset scale x", 19, nullptr, 0, 0x2ff39f0, 0x2ff3b98, 1.0F, 1.0F, 0.0F, 2.0F, 0x5CB384},
    {Init5cf140ItemKind::ranged_slider, 0x5CB48A, "Shadow HUD", 10, "Icon offset scale y", 19, nullptr, 0, 0x2ff3ac8, 0x2ff3c60, 1.0F, 1.0F, 0.0F, 2.0F, 0x5CB52E},
    {Init5cf140ItemKind::separator, 0x5CB54D, "Shadow HUD", 10, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5CB5E5, "Shadow HUD", 10, "Caption enable pixel snap", 25, nullptr, 0, 0x2ff3ba0, 0x2ff3d28, 0, 0, 0, 0, 0x5CB6B7},
    {Init5cf140ItemKind::bool_var, 0x5CB757, "Shadow HUD", 10, "Caption linear texture filter", 29, nullptr, 0, 0x2ff3c68, 0x2ff3e00, 0, 0, 0, 0, 0x5CB829},
    {Init5cf140ItemKind::ranged_slider, 0x5CB92F, "Shadow HUD", 10, "Caption size scale x", 20, nullptr, 0, 0x2ff3d30, 0x2ff3ed8, 1.0F, 1.0F, 0.10000000149011612F, 4.0F, 0x5CB9D0},
    {Init5cf140ItemKind::ranged_slider, 0x5CBAD6, "Shadow HUD", 10, "Caption size scale y", 20, nullptr, 0, 0x2ff3e08, 0x2ff3fb0, 1.0F, 1.0F, 0.10000000149011612F, 4.0F, 0x5CBB77},
    {Init5cf140ItemKind::ranged_slider, 0x5CBC7D, "Shadow HUD", 10, "Caption offset scale x", 22, nullptr, 0, 0x2ff3ee0, 0x2ff4088, 1.0F, 1.0F, 0.0F, 2.0F, 0x5CBD1E},
    {Init5cf140ItemKind::ranged_slider, 0x5CBE24, "Shadow HUD", 10, "Caption offset scale y", 22, nullptr, 0, 0x2ff3fb8, 0x2ff4150, 1.0F, 1.0F, 0.0F, 2.0F, 0x5CBEC8},
    {Init5cf140ItemKind::separator, 0x5CBEE7, "Shadow HUD", 10, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5CBF7F, "Shadow HUD", 10, "Anti-aliased lines", 18, nullptr, 0, 0x2ff4090, 0x2ff4228, 0, 0, 0, 0, 0x5CC051},
    {Init5cf140ItemKind::ranged_slider, 0x5CC156, "Shadow HUD", 10, "Grip circle size scale", 22, nullptr, 0, 0x2ff4158, 0x2ff4300, 1.0F, 1.0F, 0.0F, 4.0F, 0x5CC1F7},
    {Init5cf140ItemKind::ranged_slider, 0x5CC2FC, "Shadow HUD", 10, "Grip circle ratio scale", 23, nullptr, 0, 0x2ff4230, 0x2ff43d8, 1.0F, 1.0F, 0.0F, 10.0F, 0x5CC39D},
    {Init5cf140ItemKind::ranged_slider, 0x5CC4A2, "Shadow HUD", 10, "Grip circle line size scale", 27, nullptr, 0, 0x2ff4308, 0x2ff44a0, 1.0F, 1.0F, 0.0F, 10.0F, 0x5CC546},
    {Init5cf140ItemKind::separator, 0x5CC565, "Shadow HUD", 10, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5CC609, "Shadow HUD", 10, "Show Hud", 8, nullptr, 0, 0x2ff43e0, 0x2ff4568, 0, 0, 0, 0, 0x5CC6D4},
    {Init5cf140ItemKind::bool_var, 0x5CC774, "Shadow HUD", 10, "Show Weapon Sight", 17, nullptr, 0, 0x2ff44a8, 0x2ff4630, 0, 0, 0, 0, 0x5CC846},
    {Init5cf140ItemKind::bool_var, 0x5CC8F2, "Shadow HUD", 10, "Show Captions", 13, nullptr, 0, 0x2ff4570, 0x0, 0, 0, 0, 0, 0x5CC9C0},
    {Init5cf140ItemKind::separator, 0x5CC9E2, "Shadow HUD", 10, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::group, 0x5CCAB1, "Shadow", 6, "Shadow Map", 10, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5CCB51, "Shadow Map", 10, "Show grid map textures", 22, nullptr, 0, 0x2ff4638, 0x2ff47c0, 0, 0, 0, 0, 0x5CCC23},
    {Init5cf140ItemKind::bool_var, 0x5CCCC3, "Shadow Map", 10, "Show clouds", 11, nullptr, 0, 0x2ff4700, 0x2ff4888, 0, 0, 0, 0, 0x5CCD95},
    {Init5cf140ItemKind::bool_var, 0x5CCE35, "Shadow Map", 10, "Show dead bosses", 16, nullptr, 0, 0x2ff47c8, 0x2ff4950, 0, 0, 0, 0, 0x5CCF07},
    {Init5cf140ItemKind::bool_var, 0x5CCFA7, "Shadow Map", 10, "Show fruit", 10, nullptr, 0, 0x2ff4890, 0x2ff4a18, 0, 0, 0, 0, 0x5CD079},
    {Init5cf140ItemKind::bool_var, 0x5CD119, "Shadow Map", 10, "Show lizards", 12, nullptr, 0, 0x2ff4958, 0x2ff4ae0, 0, 0, 0, 0, 0x5CD1EB},
    {Init5cf140ItemKind::bool_var, 0x5CD28B, "Shadow Map", 10, "Show shrines", 12, nullptr, 0, 0x2ff4a20, 0x2ff4ba8, 0, 0, 0, 0, 0x5CD360},
    {Init5cf140ItemKind::separator, 0x5CD37F, "Shadow Map", 10, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5CD417, "Shadow Map", 10, "Enable adjustments", 18, nullptr, 0, 0x2ff4ae8, 0x2ff4c70, 0, 0, 0, 0, 0x5CD4EC},
    {Init5cf140ItemKind::separator, 0x5CD50B, "Shadow Map", 10, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5CD5A3, "Shadow Map", 10, "Apply stereo offset", 19, nullptr, 0, 0x2ff4bb0, 0x2ff4d48, 0, 0, 0, 0, 0x5CD675},
    {Init5cf140ItemKind::ranged_slider, 0x5CD77A, "Shadow Map", 10, "Stereo min intensity", 20, nullptr, 0, 0x2ff4c78, 0x2ff4e20, 1.0F, 1.0F, 0.0F, 2.0F, 0x5CD823},
    {Init5cf140ItemKind::ranged_slider, 0x5CD926, "Shadow Map", 10, "Stereo max intensity", 20, nullptr, 0, 0x2ff4d50, 0x2ff4ef8, 1.0F, 1.0F, 0.0F, 2.0F, 0x5CD9CF},
    {Init5cf140ItemKind::separator, 0x5CD9EE, "Shadow Map", 10, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_slider, 0x5CDAF3, "Shadow Map", 10, "Background offset", 17, nullptr, 0, 0x2ff4e28, 0x2ff4fd0, 1.0F, 1.0F, -50.0F, 50.0F, 0x5CDB91},
    {Init5cf140ItemKind::separator, 0x5CDBB0, "Shadow Map", 10, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_slider, 0x5CDCB7, "Shadow Map", 10, "Icon offset", 11, nullptr, 0, 0x2ff4f00, 0x2ff5098, 1.0F, 1.0F, -50.0F, 50.0F, 0x5CDD52},
    {Init5cf140ItemKind::bool_var, 0x5CDDF2, "Shadow Map", 10, "Icon apply scale", 16, nullptr, 0, 0x2ff4fd8, 0x2ff5170, 0, 0, 0, 0, 0x5CDEC4},
    {Init5cf140ItemKind::ranged_slider, 0x5CDFC9, "Shadow Map", 10, "Icon scale X", 12, nullptr, 0, 0x2ff50a0, 0x2ff5248, 1.0F, 1.0F, 0.009999999776482582F, 4.0F, 0x5CE06A},
    {Init5cf140ItemKind::ranged_slider, 0x5CE16F, "Shadow Map", 10, "Icon scale Y", 12, nullptr, 0, 0x2ff5178, 0x2ff5310, 1.0F, 1.0F, 0.009999999776482582F, 4.0F, 0x5CE213},
    {Init5cf140ItemKind::separator, 0x5CE232, "Shadow Map", 10, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5CE2CA, "Shadow Map", 10, "Player linear texture filter", 28, nullptr, 0, 0x2ff5250, 0x2ff53e8, 0, 0, 0, 0, 0x5CE39C},
    {Init5cf140ItemKind::ranged_slider, 0x5CE4A1, "Shadow Map", 10, "Player offset", 13, nullptr, 0, 0x2ff5318, 0x2ff54b0, 1.0F, 1.0F, -50.0F, 50.0F, 0x5CE542},
    {Init5cf140ItemKind::bool_var, 0x5CE5E2, "Shadow Map", 10, "Player apply scale", 18, nullptr, 0, 0x2ff53f0, 0x2ff5588, 0, 0, 0, 0, 0x5CE6B4},
    {Init5cf140ItemKind::ranged_slider, 0x5CE7B9, "Shadow Map", 10, "Player scale X", 14, nullptr, 0, 0x2ff54b8, 0x2ff5660, 1.0F, 1.0F, 0.009999999776482582F, 4.0F, 0x5CE85A},
    {Init5cf140ItemKind::ranged_slider, 0x5CE95F, "Shadow Map", 10, "Player scale Y", 14, nullptr, 0, 0x2ff5590, 0x2ff5738, 1.0F, 1.0F, 0.009999999776482582F, 4.0F, 0x5CEA03},
    {Init5cf140ItemKind::separator, 0x5CEA22, "Shadow Map", 10, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_slider, 0x5CEB2E, "Shadow Map", 10, "Player path offset", 18, nullptr, 0, 0x2ff5668, 0x2ff5800, 1.0F, 1.0F, -50.0F, 50.0F, 0x5CEBCC},
    {Init5cf140ItemKind::separator, 0x5CEBEB, "Shadow Map", 10, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5CEC77, "Shadow Map", 10, "Cloud enable offset", 19, nullptr, 0, 0x2ff5740, 0x2ff58d8, 0, 0, 0, 0, 0x5CED49},
    {Init5cf140ItemKind::ranged_slider, 0x5CEE36, "Shadow Map", 10, "Cloud min offset", 16, nullptr, 0, 0x2ff5808, 0x2ff59b0, 1.0F, 1.0F, -50.0F, 50.0F, 0x5CEED7},
    {Init5cf140ItemKind::ranged_slider, 0x5CEFC4, "Shadow Map", 10, "Cloud max offset", 16, nullptr, 0, 0x2ff58e0, 0x0, 1.0F, 1.0F, -50.0F, 50.0F, 0x5CF068},
    {Init5cf140ItemKind::separator, 0x5CF08A, "Shadow Map", 10, "", 0, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
};

inline constexpr std::size_t Init5c94b0ItemsCount =
    sizeof(Init5c94b0Items) / sizeof(Init5c94b0Items[0]);

// Initb25840Items: 0xb25840..0xb2b6aa (24170 B), 60 items
inline constexpr std::uint64_t kInitb25840Rva = 0xB25840;
inline constexpr std::uint64_t kInitb25840EndRva = 0xB2B6AA;
inline constexpr std::size_t kInitb25840Size = 24170;

inline constexpr Init5cf140ItemSpec Initb25840Items[] = {
    {Init5cf140ItemKind::group, 0xB258C6, "Shadow", 6, "Shadow S3D Dialog", 17, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::group, 0xB258FF, "Shadow S3D Dialog", 17, "Alert", 5, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_slider, 0xB25A0C, "Alert", 5, "Alert - Background X", 20, nullptr, 0, 0x334ac30, 0x334add8, 1.0F, 1.0F, -2.0F, 2.0F, 0xB25AA8},
    {Init5cf140ItemKind::ranged_slider, 0xB25BAE, "Alert", 5, "Alert - Background Y", 20, nullptr, 0, 0x334ad08, 0x334aeb0, 1.0F, 1.0F, -2.0F, 2.0F, 0xB25C4F},
    {Init5cf140ItemKind::ranged_slider, 0xB25D55, "Alert", 5, "Alert - Background W", 20, nullptr, 0, 0x334ade0, 0x334af88, 1.0F, 1.0F, -2.0F, 2.0F, 0xB25DF6},
    {Init5cf140ItemKind::ranged_slider, 0xB25EFC, "Alert", 5, "Alert - Background H", 20, nullptr, 0, 0x334aeb8, 0x334b060, 1.0F, 1.0F, -2.0F, 2.0F, 0xB25F9D},
    {Init5cf140ItemKind::ranged_slider, 0xB260A3, "Alert", 5, "Alert - Background U0", 21, nullptr, 0, 0x334af90, 0x334b138, 1.0F, 1.0F, -2.0F, 2.0F, 0xB26144},
    {Init5cf140ItemKind::ranged_slider, 0xB2624A, "Alert", 5, "Alert - Background V0", 21, nullptr, 0, 0x334b068, 0x334b210, 1.0F, 1.0F, -2.0F, 2.0F, 0xB262EB},
    {Init5cf140ItemKind::ranged_slider, 0xB263F1, "Alert", 5, "Alert - Background U1", 21, nullptr, 0, 0x334b140, 0x334b2e8, 1.0F, 1.0F, -2.0F, 2.0F, 0xB26492},
    {Init5cf140ItemKind::ranged_slider, 0xB26598, "Alert", 5, "Alert - Background V1", 21, nullptr, 0, 0x334b218, 0x334b3c0, 1.0F, 1.0F, -2.0F, 2.0F, 0xB26639},
    {Init5cf140ItemKind::ranged_slider, 0xB2673F, "Alert", 5, "Alert - Yes X", 13, nullptr, 0, 0x334b2f0, 0x334b498, 1.0F, 1.0F, -2.0F, 2.0F, 0xB267E0},
    {Init5cf140ItemKind::ranged_slider, 0xB268E6, "Alert", 5, "Alert - Yes Y", 13, nullptr, 0, 0x334b3c8, 0x334b570, 1.0F, 1.0F, -2.0F, 2.0F, 0xB26987},
    {Init5cf140ItemKind::ranged_slider, 0xB26A8D, "Alert", 5, "Alert - Yes W", 13, nullptr, 0, 0x334b4a0, 0x334b648, 1.0F, 1.0F, -2.0F, 2.0F, 0xB26B2E},
    {Init5cf140ItemKind::ranged_slider, 0xB26C34, "Alert", 5, "Alert - Yes H", 13, nullptr, 0, 0x334b578, 0x334b720, 1.0F, 1.0F, -2.0F, 2.0F, 0xB26CD5},
    {Init5cf140ItemKind::ranged_slider, 0xB26DDB, "Alert", 5, "Alert - Yes U0", 14, nullptr, 0, 0x334b650, 0x334b7f8, 1.0F, 1.0F, -2.0F, 2.0F, 0xB26E7C},
    {Init5cf140ItemKind::ranged_slider, 0xB26F82, "Alert", 5, "Alert - Yes V0", 14, nullptr, 0, 0x334b728, 0x334b8d0, 1.0F, 1.0F, -2.0F, 2.0F, 0xB27023},
    {Init5cf140ItemKind::ranged_slider, 0xB27129, "Alert", 5, "Alert - Yes U1", 14, nullptr, 0, 0x334b800, 0x334b9a8, 1.0F, 1.0F, -2.0F, 2.0F, 0xB271CA},
    {Init5cf140ItemKind::ranged_slider, 0xB272D0, "Alert", 5, "Alert - Yes V1", 14, nullptr, 0, 0x334b8d8, 0x334ba80, 1.0F, 1.0F, -2.0F, 2.0F, 0xB27371},
    {Init5cf140ItemKind::ranged_slider, 0xB27477, "Alert", 5, "Alert - No X", 12, nullptr, 0, 0x334b9b0, 0x334bb58, 1.0F, 1.0F, -2.0F, 2.0F, 0xB27518},
    {Init5cf140ItemKind::ranged_slider, 0xB2761E, "Alert", 5, "Alert - No Y", 12, nullptr, 0, 0x334ba88, 0x334bc30, 1.0F, 1.0F, -2.0F, 2.0F, 0xB276BF},
    {Init5cf140ItemKind::ranged_slider, 0xB277C5, "Alert", 5, "Alert - No W", 12, nullptr, 0, 0x334bb60, 0x334bd08, 1.0F, 1.0F, -2.0F, 2.0F, 0xB27866},
    {Init5cf140ItemKind::ranged_slider, 0xB2796C, "Alert", 5, "Alert - No H", 12, nullptr, 0, 0x334bc38, 0x334bde0, 1.0F, 1.0F, -2.0F, 2.0F, 0xB27A0D},
    {Init5cf140ItemKind::ranged_slider, 0xB27B13, "Alert", 5, "Alert - No U0", 13, nullptr, 0, 0x334bd10, 0x334beb8, 1.0F, 1.0F, -2.0F, 2.0F, 0xB27BB4},
    {Init5cf140ItemKind::ranged_slider, 0xB27CBA, "Alert", 5, "Alert - No V0", 13, nullptr, 0, 0x334bde8, 0x334bf90, 1.0F, 1.0F, -2.0F, 2.0F, 0xB27D5B},
    {Init5cf140ItemKind::ranged_slider, 0xB27E61, "Alert", 5, "Alert - No U1", 13, nullptr, 0, 0x334bec0, 0x334c068, 1.0F, 1.0F, -2.0F, 2.0F, 0xB27F02},
    {Init5cf140ItemKind::ranged_slider, 0xB28008, "Alert", 5, "Alert - No V1", 13, nullptr, 0, 0x334bf98, 0x334c140, 1.0F, 1.0F, -2.0F, 2.0F, 0xB280AC},
    {Init5cf140ItemKind::group, 0xB280FF, "Shadow S3D Dialog", 17, "Prompt", 6, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_slider, 0xB281FD, "Prompt", 6, "Prompt - Background X", 21, nullptr, 0, 0x334c070, 0x334c218, 1.0F, 1.0F, -2.0F, 2.0F, 0xB2829E},
    {Init5cf140ItemKind::ranged_slider, 0xB283A4, "Prompt", 6, "Prompt - Background Y", 21, nullptr, 0, 0x334c148, 0x334c2f0, 1.0F, 1.0F, -2.0F, 2.0F, 0xB28445},
    {Init5cf140ItemKind::ranged_slider, 0xB2854B, "Prompt", 6, "Prompt - Background W", 21, nullptr, 0, 0x334c220, 0x334c3c8, 1.0F, 1.0F, -2.0F, 2.0F, 0xB285EC},
    {Init5cf140ItemKind::ranged_slider, 0xB286F2, "Prompt", 6, "Prompt - Background H", 21, nullptr, 0, 0x334c2f8, 0x334c4a0, 1.0F, 1.0F, -2.0F, 2.0F, 0xB28793},
    {Init5cf140ItemKind::ranged_slider, 0xB28899, "Prompt", 6, "Prompt - Background U0", 22, nullptr, 0, 0x334c3d0, 0x334c578, 1.0F, 1.0F, -2.0F, 2.0F, 0xB2893A},
    {Init5cf140ItemKind::ranged_slider, 0xB28A40, "Prompt", 6, "Prompt - Background V0", 22, nullptr, 0, 0x334c4a8, 0x334c650, 1.0F, 1.0F, -2.0F, 2.0F, 0xB28AE1},
    {Init5cf140ItemKind::ranged_slider, 0xB28BEA, "Prompt", 6, "Prompt - Background U1", 22, nullptr, 0, 0x334c580, 0x334c728, 1.0F, 1.0F, -2.0F, 2.0F, 0xB28C8E},
    {Init5cf140ItemKind::ranged_slider, 0xB28D97, "Prompt", 6, "Prompt - Background V1", 22, nullptr, 0, 0x334c658, 0x334c800, 1.0F, 1.0F, -2.0F, 2.0F, 0xB28E3B},
    {Init5cf140ItemKind::ranged_slider, 0xB28F44, "Prompt", 6, "Prompt - Yes X", 14, nullptr, 0, 0x334c730, 0x334c8d8, 1.0F, 1.0F, -2.0F, 2.0F, 0xB28FE8},
    {Init5cf140ItemKind::ranged_slider, 0xB290F1, "Prompt", 6, "Prompt - Yes Y", 14, nullptr, 0, 0x334c808, 0x334c9b0, 1.0F, 1.0F, -2.0F, 2.0F, 0xB29195},
    {Init5cf140ItemKind::ranged_slider, 0xB2929E, "Prompt", 6, "Prompt - Yes W", 14, nullptr, 0, 0x334c8e0, 0x334ca88, 1.0F, 1.0F, -2.0F, 2.0F, 0xB29342},
    {Init5cf140ItemKind::ranged_slider, 0xB2944B, "Prompt", 6, "Prompt - Yes H", 14, nullptr, 0, 0x334c9b8, 0x334cb60, 1.0F, 1.0F, -2.0F, 2.0F, 0xB294EF},
    {Init5cf140ItemKind::ranged_slider, 0xB295F8, "Prompt", 6, "Prompt - Yes U0", 15, nullptr, 0, 0x334ca90, 0x334cc38, 1.0F, 1.0F, -2.0F, 2.0F, 0xB2969C},
    {Init5cf140ItemKind::ranged_slider, 0xB297A5, "Prompt", 6, "Prompt - Yes V0", 15, nullptr, 0, 0x334cb68, 0x334cd10, 1.0F, 1.0F, -2.0F, 2.0F, 0xB29849},
    {Init5cf140ItemKind::ranged_slider, 0xB29952, "Prompt", 6, "Prompt - Yes U1", 15, nullptr, 0, 0x334cc40, 0x334cde8, 1.0F, 1.0F, -2.0F, 2.0F, 0xB299F6},
    {Init5cf140ItemKind::ranged_slider, 0xB29AFF, "Prompt", 6, "Prompt - Yes V1", 15, nullptr, 0, 0x334cd18, 0x334cec0, 1.0F, 1.0F, -2.0F, 2.0F, 0xB29BA3},
    {Init5cf140ItemKind::ranged_slider, 0xB29CAC, "Prompt", 6, "Prompt - No X", 13, nullptr, 0, 0x334cdf0, 0x334cf98, 1.0F, 1.0F, -2.0F, 2.0F, 0xB29D50},
    {Init5cf140ItemKind::ranged_slider, 0xB29E59, "Prompt", 6, "Prompt - No Y", 13, nullptr, 0, 0x334cec8, 0x334d070, 1.0F, 1.0F, -2.0F, 2.0F, 0xB29EFD},
    {Init5cf140ItemKind::ranged_slider, 0xB2A006, "Prompt", 6, "Prompt - No W", 13, nullptr, 0, 0x334cfa0, 0x334d148, 1.0F, 1.0F, -2.0F, 2.0F, 0xB2A0AA},
    {Init5cf140ItemKind::ranged_slider, 0xB2A1B3, "Prompt", 6, "Prompt - No H", 13, nullptr, 0, 0x334d078, 0x334d220, 1.0F, 1.0F, -2.0F, 2.0F, 0xB2A257},
    {Init5cf140ItemKind::ranged_slider, 0xB2A360, "Prompt", 6, "Prompt - No U0", 14, nullptr, 0, 0x334d150, 0x334d2f8, 1.0F, 1.0F, -2.0F, 2.0F, 0xB2A404},
    {Init5cf140ItemKind::ranged_slider, 0xB2A50D, "Prompt", 6, "Prompt - No V0", 14, nullptr, 0, 0x334d228, 0x334d3d0, 1.0F, 1.0F, -2.0F, 2.0F, 0xB2A5B1},
    {Init5cf140ItemKind::ranged_slider, 0xB2A6BA, "Prompt", 6, "Prompt - No U1", 14, nullptr, 0, 0x334d300, 0x334d4a8, 1.0F, 1.0F, -2.0F, 2.0F, 0xB2A75E},
    {Init5cf140ItemKind::ranged_slider, 0xB2A867, "Prompt", 6, "Prompt - No V1", 14, nullptr, 0, 0x334d3d8, 0x334d580, 1.0F, 1.0F, -2.0F, 2.0F, 0xB2A90E},
    {Init5cf140ItemKind::group, 0xB2A961, "Shadow S3D Dialog", 17, "Number", 6, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_slider, 0xB2AA5E, "Number", 6, "0 -X", 4, nullptr, 0, 0x334d4b0, 0x334d658, 1.0F, 1.0F, -2.0F, 2.0F, 0xB2AAFF},
    {Init5cf140ItemKind::ranged_slider, 0xB2AC05, "Number", 6, "0 -Y", 4, nullptr, 0, 0x334d588, 0x334d730, 1.0F, 1.0F, -2.0F, 2.0F, 0xB2ACA6},
    {Init5cf140ItemKind::ranged_slider, 0xB2ADAC, "Number", 6, "0 -W", 4, nullptr, 0, 0x334d660, 0x334d808, 1.0F, 1.0F, -2.0F, 2.0F, 0xB2AE4D},
    {Init5cf140ItemKind::ranged_slider, 0xB2AF53, "Number", 6, "0 -H", 4, nullptr, 0, 0x334d738, 0x334d8e0, 1.0F, 1.0F, -2.0F, 2.0F, 0xB2AFF4},
    {Init5cf140ItemKind::ranged_slider, 0xB2B0FA, "Number", 6, "0 -U0", 5, nullptr, 0, 0x334d810, 0x334d9b8, 1.0F, 1.0F, -2.0F, 2.0F, 0xB2B19B},
    {Init5cf140ItemKind::ranged_slider, 0xB2B289, "Number", 6, "0 -V0", 5, nullptr, 0, 0x334d8e8, 0x334da90, 1.0F, 1.0F, -2.0F, 2.0F, 0xB2B32A},
    {Init5cf140ItemKind::ranged_slider, 0xB2B418, "Number", 6, "0 -U1", 5, nullptr, 0, 0x334d9c0, 0x334db68, 1.0F, 1.0F, -2.0F, 2.0F, 0xB2B4B9},
    {Init5cf140ItemKind::ranged_slider, 0xB2B5A7, "Number", 6, "0 -V1", 5, nullptr, 0, 0x334da98, 0x0, 1.0F, 1.0F, -2.0F, 2.0F, 0xB2B64B},
};

inline constexpr std::size_t Initb25840ItemsCount =
    sizeof(Initb25840Items) / sizeof(Initb25840Items[0]);

// Init5c11a0Items: 0x5c11a0..0x5c1d50 (2992 B), 10 items
inline constexpr std::uint64_t kInit5c11a0Rva = 0x5C11A0;
inline constexpr std::uint64_t kInit5c11a0EndRva = 0x5C1D50;
inline constexpr std::size_t kInit5c11a0Size = 2992;

inline constexpr Init5cf140ItemSpec Init5c11a0Items[] = {
    {Init5cf140ItemKind::group, 0x5C11FC, "Shadow", 6, "Shadow Sound", 12, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5C129C, "Shadow Sound", 12, "Enable Streams", 14, nullptr, 0, 0x2ff0198, 0x2ff0320, 0, 0, 0, 0, 0x5C136E},
    {Init5cf140ItemKind::bool_var, 0x5C140E, "Shadow Sound", 12, "Enable SE", 9, nullptr, 0, 0x2ff0260, 0x2ff03e8, 0, 0, 0, 0, 0x5C14E0},
    {Init5cf140ItemKind::bool_var, 0x5C1580, "Shadow Sound", 12, "Show ADPCM Streams", 18, nullptr, 0, 0x2ff0328, 0x2ff04b0, 0, 0, 0, 0, 0x5C1652},
    {Init5cf140ItemKind::bool_var, 0x5C16F2, "Shadow Sound", 12, "Show SFX", 8, nullptr, 0, 0x2ff03f0, 0x2ff0578, 0, 0, 0, 0, 0x5C17C4},
    {Init5cf140ItemKind::bool_var, 0x5C1864, "Shadow Sound", 12, "Enable ADSR Envelopes", 21, nullptr, 0, 0x2ff04b8, 0x2ff0650, 0, 0, 0, 0, 0x5C1939},
    {Init5cf140ItemKind::group, 0x5C19B7, "Shadow", 6, "Shadow Cursor", 13, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_int_slider, 0x5C1AB5, "Shadow Cursor", 13, "Offset", 6, nullptr, 0, 0x2ff0580, 0x2ff0718, 1.0F, 1.0F, 0.0F, 2147483647.0F, 0x5C1B57},
    {Init5cf140ItemKind::group, 0x5C1BAA, "Shadow", 6, "Layers", 6, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5C1C42, "Layers", 6, "Show Draw Layers", 16, nullptr, 0, 0x2ff0658, 0x0, 0, 0, 0, 0, 0x5C1D14},
};

inline constexpr std::size_t Init5c11a0ItemsCount =
    sizeof(Init5c11a0Items) / sizeof(Init5c11a0Items[0]);

// Init5c1e61Items: 0x5c1e61..0x5c56b1 (14416 B), 40 items
inline constexpr std::uint64_t kInit5c1e61Rva = 0x5C1E61;
inline constexpr std::uint64_t kInit5c1e61EndRva = 0x5C56B1;
inline constexpr std::size_t kInit5c1e61Size = 14416;

inline constexpr Init5cf140ItemSpec Init5c1e61Items[] = {
    {Init5cf140ItemKind::group, 0x5C1EAC, "CP10", 4, "Performance", 11, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_int_slider, 0x5C1FAA, "Performance", 11, "Show Profile Info", 17, nullptr, 0, 0x2ff0720, 0x2ff08c8, 1.0F, 1.0F, 0.0F, 3.0F, 0x5C204D},
    {Init5cf140ItemKind::ranged_int_slider, 0x5C2152, "Performance", 11, "Profile Info Line Offset", 24, nullptr, 0, 0x2ff07f8, 0x2ff0990, 1.0F, 1.0F, 0.0F, 100.0F, 0x5C21F9},
    {Init5cf140ItemKind::bool_var, 0x5C229E, "Performance", 11, "Background Render Mode", 22, nullptr, 0, 0x2ff08d0, 0x2ff0a68, 0, 0, 0, 0, 0x5C2366},
    {Init5cf140ItemKind::ranged_int_slider, 0x5C246B, "Performance", 11, "Background Render Option", 24, nullptr, 0, 0x2ff0998, 0x2ff0b30, 1.0F, 1.0F, 0.0F, 1.0F, 0x5C2513},
    {Init5cf140ItemKind::bool_var, 0x5C25B3, "Performance", 11, "Lowres Shadows", 14, nullptr, 0, 0x2ff0a70, 0x2ff0bf8, 0, 0, 0, 0, 0x5C267B},
    {Init5cf140ItemKind::bool_var, 0x5C271B, "Performance", 11, "World ZPrepass", 14, nullptr, 0, 0x2ff0b38, 0x2ff0cc0, 0, 0, 0, 0, 0x5C27E3},
    {Init5cf140ItemKind::bool_var, 0x5C2883, "Performance", 11, "World ZCull Reload", 18, nullptr, 0, 0x2ff0c00, 0x2ff0d88, 0, 0, 0, 0, 0x5C294B},
    {Init5cf140ItemKind::bool_var, 0x5C29EB, "Performance", 11, "Character ZPrepass", 18, nullptr, 0, 0x2ff0cc8, 0x2ff0e50, 0, 0, 0, 0, 0x5C2AB3},
    {Init5cf140ItemKind::bool_var, 0x5C2B53, "Performance", 11, "Character ZCull Reload", 22, nullptr, 0, 0x2ff0d90, 0x2ff0f18, 0, 0, 0, 0, 0x5C2C1B},
    {Init5cf140ItemKind::bool_var, 0x5C2CBB, "Performance", 11, "Force Single Pass Shader", 24, nullptr, 0, 0x2ff0e58, 0x2ff0fe0, 0, 0, 0, 0, 0x5C2D83},
    {Init5cf140ItemKind::bool_var, 0x5C2E23, "Performance", 11, "Enable Mesh Rendering", 21, nullptr, 0, 0x2ff0f20, 0x2ff10b8, 0, 0, 0, 0, 0x5C2EEB},
    {Init5cf140ItemKind::ranged_int_slider, 0x5C2FF0, "Performance", 11, "Particle Alpha Test Optimization", 32, nullptr, 0, 0x2ff0fe8, 0x2ff1190, 1.0F, 1.0F, 4294967295.0F, 255.0F, 0x5C309C},
    {Init5cf140ItemKind::ranged_int_slider, 0x5C31A1, "Performance", 11, "Cinematic Scissor Override", 26, nullptr, 0, 0x2ff10c0, 0x2ff1268, 1.0F, 1.0F, 4294967295.0F, 1.0F, 0x5C3249},
    {Init5cf140ItemKind::ranged_slider, 0x5C334E, "Performance", 11, "Cinematic Scissor Factor", 24, nullptr, 0, 0x2ff1198, 0x2ff1340, 1.0F, 1.0F, 0.0F, 1.0F, 0x5C33FA},
    {Init5cf140ItemKind::ranged_slider, 0x5C34FF, "Performance", 11, "Cinematic Scissor Factor (Under Water)", 38, nullptr, 0, 0x2ff1270, 0x2ff1418, 1.0F, 1.0F, 0.0F, 1.0F, 0x5C35AB},
    {Init5cf140ItemKind::ranged_slider, 0x5C36B0, "Performance", 11, "Near Plane", 10, nullptr, 0, 0x2ff1348, 0x2ff14f0, 1.0F, 1.0F, 0.05000000074505806F, 10.0F, 0x5C375C},
    {Init5cf140ItemKind::ranged_slider, 0x5C3861, "Performance", 11, "Far Plane", 9, nullptr, 0, 0x2ff1420, 0x2ff15c8, 1.0F, 1.0F, 0.05000000074505806F, 65535.0F, 0x5C3910},
    {Init5cf140ItemKind::group, 0x5C3949, "Performance", 11, "Transparency", 12, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::ranged_int_slider, 0x5C3A51, "Transparency", 12, "World Alpha Fail Optimization", 29, nullptr, 0, 0x2ff14f8, 0x2ff1690, 1.0F, 1.0F, 0.0F, 2.0F, 0x5C3AF7},
    {Init5cf140ItemKind::bool_var, 0x5C3B97, "Transparency", 12, "Show World Alpha Fail Optimization Stats", 40, nullptr, 0, 0x2ff15d0, 0x2ff1758, 0, 0, 0, 0, 0x5C3C5F},
    {Init5cf140ItemKind::bool_var, 0x5C3CFF, "Transparency", 12, "Force Disable Depth Write", 25, nullptr, 0, 0x2ff1698, 0x2ff1820, 0, 0, 0, 0, 0x5C3DC7},
    {Init5cf140ItemKind::bool_var, 0x5C3E67, "Transparency", 12, "Force Depth Write", 17, nullptr, 0, 0x2ff1760, 0x2ff18f8, 0, 0, 0, 0, 0x5C3F2F},
    {Init5cf140ItemKind::ranged_int_slider, 0x5C4034, "Transparency", 12, "Mesh alpha blend optimization", 29, nullptr, 0, 0x2ff1828, 0x2ff19c0, 1.0F, 1.0F, 0.0F, 2.0F, 0x5C40E0},
    {Init5cf140ItemKind::group, 0x5C414D, "CP10", 4, "Texture", 7, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5C41E5, "Texture", 7, "Force Disable CLUT Lerp", 23, nullptr, 0, 0x2ff1900, 0x2ff1a88, 0, 0, 0, 0, 0x5C42AD},
    {Init5cf140ItemKind::bool_var, 0x5C434D, "Texture", 7, "Display CLUT anim stats", 23, nullptr, 0, 0x2ff19c8, 0x2ff1b50, 0, 0, 0, 0, 0x5C4415},
    {Init5cf140ItemKind::bool_var, 0x5C44B5, "Texture", 7, "Allow Texture Data Hints", 24, nullptr, 0, 0x2ff1a90, 0x2ff1c28, 0, 0, 0, 0, 0x5C457D},
    {Init5cf140ItemKind::ranged_int_slider, 0x5C4682, "Texture", 7, "Texture Filter", 14, nullptr, 0, 0x2ff1b58, 0x2ff1d00, 1.0F, 1.0F, 4294967295.0F, 3.0F, 0x5C472B},
    {Init5cf140ItemKind::ranged_int_slider, 0x5C4830, "Texture", 7, "Alpha Ref Override", 18, nullptr, 0, 0x2ff1c30, 0x2ff1dc8, 1.0F, 1.0F, 4294967295.0F, 255.0F, 0x5C48DD},
    {Init5cf140ItemKind::bool_var, 0x5C497D, "Texture", 7, "Texture Override LOD Offset", 27, nullptr, 0, 0x2ff1d08, 0x2ff1ea0, 0, 0, 0, 0, 0x5C4A45},
    {Init5cf140ItemKind::ranged_int_slider, 0x5C4B4A, "Texture", 7, "Texture LOD Offset", 18, nullptr, 0, 0x2ff1dd0, 0x2ff1f68, 1.0F, 1.0F, 4294967284.0F, 12.0F, 0x5C4BF6},
    {Init5cf140ItemKind::group, 0x5C4C49, "CP10", 4, "Fullscreen Effects Tweaks", 25, nullptr, 0, 0, 0, 0, 0, 0, 0, 0},
    {Init5cf140ItemKind::bool_var, 0x5C4CE1, "Fullscreen Effects Tweaks", 25, "Show Fullscreen Effects List", 28, nullptr, 0, 0x2ff1ea8, 0x2ff2040, 0, 0, 0, 0, 0x5C4DA9},
    {Init5cf140ItemKind::ranged_slider, 0x5C4EAE, "Fullscreen Effects Tweaks", 25, "Large Colossus Shake Blur Intensity", 35, nullptr, 0, 0x2ff1f70, 0x2ff2108, 1.0F, 1.0F, 0.10000000149011612F, 1.0F, 0x5C4F5B},
    {Init5cf140ItemKind::bool_var, 0x5C4FFB, "Fullscreen Effects Tweaks", 25, "Flash Arrow Blur Disable", 24, nullptr, 0, 0x2ff2048, 0x2ff21d0, 0, 0, 0, 0, 0x5C50C6},
    {Init5cf140ItemKind::bool_var, 0x5C5175, "Fullscreen Effects Tweaks", 25, "Show FPS", 8, nullptr, 0, 0x2ff2110, 0x2ff2298, 0, 0, 0, 0, 0x5C523D},
    {Init5cf140ItemKind::bool_var, 0x5C52D1, "Fullscreen Effects Tweaks", 25, "Show Memory", 11, nullptr, 0, 0x2ff21d8, 0x2ff2360, 0, 0, 0, 0, 0x5C53A0},
    {Init5cf140ItemKind::bool_var, 0x5C542D, "Fullscreen Effects Tweaks", 25, "Show Render Memory", 18, nullptr, 0, 0x2ff22a0, 0x2ff2428, 0, 0, 0, 0, 0x5C54FC},
    {Init5cf140ItemKind::bool_var, 0x5C558F, "Fullscreen Effects Tweaks", 25, "Spew Timescale Info", 19, nullptr, 0, 0x2ff2368, 0x0, 0, 0, 0, 0, 0x5C564C},
};

inline constexpr std::size_t Init5c1e61ItemsCount =
    sizeof(Init5c1e61Items) / sizeof(Init5c1e61Items[0]);

// S8: 0x5c1d50..0x5c1e61 (273 B, 54 instructions), the 33-iteration loop over the
// 0x2dcaa90 name table and the 0x2fdaa60 default bytes; each row is one layer bool.
inline constexpr std::uint64_t kInit5c1d50Rva = 0x5C1D50;
inline constexpr std::uint64_t kInit5c1d50EndRva = 0x5C1E61;
inline constexpr std::size_t kInit5c1d50Size = 273;

inline constexpr Init5cf140ItemSpec Init5c1d50Items[] = {
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Clear", 5, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "World BG", 8, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "World FG Solid", 14, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "World FG Trans", 14, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "World Comp", 10, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Character Solid", 15, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Character Trans", 15, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Shadow", 6, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Boss Effects", 12, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Water Refraction", 16, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Water Fog", 9, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Particle Before Water", 21, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Water Refl Model Opaque", 23, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Water Refl Model Trans", 22, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Reflection", 10, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Reflection with Water Effect", 28, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Reflection (Non-Water)", 22, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Water Surface", 13, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "3D Effects", 10, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Particles", 9, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "General Refraction", 18, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "PostFX", 6, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Particle After Water", 20, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Glow", 4, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "3D Display Debug", 16, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "3D Fonts", 8, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Sprite 1", 8, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Sprite 2", 8, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Fade", 4, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Over Fade", 9, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Debugging fonts", 15, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Window", 6, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
    {Init5cf140ItemKind::bool_var, 0x5C1DB9, "Layers", 6, "Finish", 6, nullptr, 0, 0, 0, 0, 0, 0, 0, 0x5C1E43},
};

inline constexpr std::size_t Init5c1d50ItemsCount =
    sizeof(Init5c1d50Items) / sizeof(Init5c1d50Items[0]);




struct Init5b9f90Hooks {
    void* host = nullptr;
    DebugMenuRegistry* menu = nullptr;
    StartupAtexitLog* atexit = nullptr;
    ConCmdRegistry* registry = nullptr;   // 0x20d60 for the S3.4 variables
    // 0x5b9fa8 loads [0x15fe518] and stores it at rsp+0x2780; the host supplies the token and owns
    // the cell (the epilogue comparison belongs to S9, which is not lifted).
    std::uint64_t* canary_cell = nullptr;
    std::uint64_t canary_token = 0;
    // The body builds its 0xa4-byte items in the caller's 0x27a0 frame and the root list keeps
    // pointers to them, so the host owns the storage (at least 61 slots for the executed slices).
    DebugMenuItem* item_storage = nullptr;
    std::size_t item_capacity = 0;
    // 0xf590 on a borrowed view.
    bool (*release_view)(void* host, const SharedStr& view) noexcept = nullptr;
    // 0x2170 / 0x5cf140. The host observes the call; the callee body is not lifted, so the slices
    // that contain these calls are reported as `contract`, never as executed native code.
    InitPostCallOutcome (*page_builder)(void* host, std::uint64_t rva, const char* name) noexcept = nullptr;
    // The eight S3.3 toggle variables are built by unlifted engine init; one host bool per target.
    bool* toggle_value_storage = nullptr;
    std::size_t toggle_value_capacity = 0;
    // S3.4: four bool variables, their guard bytes and their value bytes.
    ConCmdBoolVar* var_storage = nullptr;
    std::size_t var_capacity = 0;
    bool* var_value_storage = nullptr;
    bool* var_guard_storage = nullptr;
    std::size_t var_guard_capacity = 0;
    // S2/S4.1: bounded variables and their atexit guards.
    ConCmdRangedVar* ranged_storage = nullptr;
    std::size_t ranged_capacity = 0;
    float* ranged_value_storage = nullptr;
    std::int32_t* ranged_int_value_storage = nullptr;
    bool* ranged_guard_storage = nullptr;
    // 0x26b60 builds its text through 0xf1a0; the host supplies the allocator.
    SharedStrCtorHooks separator_alloc{};
};

struct Init5b9f90Result {
    bool ok = false;
    bool complete = false;          // always false: 37,607 B of the body remain unlifted
    const char* blocked = nullptr;
    std::uint64_t resume_rva = kInit5b9f90ResumeRva;
    int groups = 0;
    int commands = 0;
    int toggles = 0;
    int float_items = 0;
    int named_vars = 0;
    int separators = 0;
    int sliders = 0;
    int int_sliders = 0;
    std::size_t s4_items = 0;
    std::size_t s4_bytes = 0;
    bool s4_executed = false;
    std::size_t s5_items = 0;
    std::size_t s5_bytes = 0;
    bool s5_executed = false;
    std::size_t s4_2_label_len = 0;
    bool s4_2_executed = false;
    int registered = 0;
    int releases = 0;
    int destroys = 0;
    int contracts = 0;
    std::size_t s2_items = 0;
    bool s2_executed = false;
    int executed_slices = 0;
    std::size_t items_used = 0;
    bool canary_stored = false;
    // Executed sub-slice records handed to the runtime's post-call log (S0, S2, S3.1..S3.6, S4.1).
    std::array<InitPostCallRecord, 24> records{};
    std::size_t record_count = 0;
};

// Executes S0 and all six S3 pages in body order, with the two page builders as named contracts.
// Any missing input or model failure stops with a named `blocked` value; a partial build is never
// reported as ok.
[[nodiscard]] Init5b9f90Result run_init_5b9f90_slices(const Init5b9f90Hooks& hooks) noexcept;

}  // namespace sotc
