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
inline constexpr std::size_t kInit5b9f90ExecutedBytes = 9274;  // S0 177 + S3 9,097
inline constexpr std::uint64_t kInit5b9f90SingleRetRva = 0x5C56AA;
inline constexpr std::uint64_t kInit5b9f90LoopHeadRva = 0x5C1D50;
inline constexpr std::uint64_t kInit5b9f90LoopTailRva = 0x5C1E5B;   // jne 0x5c1d50, 33 iterations
inline constexpr std::uint64_t kInit5b9f90ResumeRva = 0x5BC3D4;   // S4, not lifted
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
inline constexpr Init5b9f90Slice kInit5b9f90Slices[] = {
    {"S0", 0x5B9F90, 0x5BA041, 33, 3, Init5b9f90SliceStatus::executed,
     "frame, canary, group \"Shadow\" under \"CP10\""},
    {"S1", 0x5BA041, 0x5BA046, 1, 1, Init5b9f90SliceStatus::contract,
     "0x2170 page builder (\"Shadow Misc\"): body not lifted, named host contract"},
    {"S2", 0x5BA046, 0x5BA04B, 1, 1, Init5b9f90SliceStatus::contract,
     "0x5cf140 page builder: body not lifted, named host contract"},
    {"S3", 0x5BA04B, 0x5BC3D4, 1460, 245, Init5b9f90SliceStatus::executed,
     "six pages, all executed; ends at 0x5bc3d4"},
    {"S4", 0x5BC3D4, 0x5BC3DE, 2, 2, Init5b9f90SliceStatus::blocked,
     "0x5dbf40 + 0x7f8930: callee bodies not lifted"},
    {"S5", 0x5BC3DE, 0x5C1196, 3245, 452, Init5b9f90SliceStatus::blocked,
     "\"Stereo 3D\" / \"Dynamic 3D\" / wormhole pages; extraction pending"},
    {"S6", 0x5C1196, 0x5C11A0, 2, 2, Init5b9f90SliceStatus::blocked,
     "0x5c94b0 + 0xb25840: callee bodies not lifted"},
    {"S7", 0x5C11A0, 0x5C1D50, 491, 75, Init5b9f90SliceStatus::blocked,
     "\"Shadow Sound\" / \"Shadow Cursor\" / \"Layers\" pages; extraction pending"},
    {"S8", 0x5C1D50, 0x5C1E61, 54, 8, Init5b9f90SliceStatus::blocked,
     "the only loop nest (33 iterations over the 0x2dcaa90 table); body pending"},
    {"S9", 0x5C1E61, 0x5C56B1, 2356, 347, Init5b9f90SliceStatus::blocked,
     "\"Performance\" / \"Transparency\" / \"Texture\" / \"Fullscreen Effects Tweaks\"; pending"},
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
    int registered = 0;
    int releases = 0;
    int destroys = 0;
    int contracts = 0;
    int executed_slices = 0;
    std::size_t items_used = 0;
    bool canary_stored = false;
    // Executed sub-slice records handed to the runtime's post-call log (S0 and the six S3 pages).
    std::array<InitPostCallRecord, 8> records{};
    std::size_t record_count = 0;
};

// Executes S0 and all six S3 pages in body order, with the two page builders as named contracts.
// Any missing input or model failure stops with a named `blocked` value; a partial build is never
// reported as ok.
[[nodiscard]] Init5b9f90Result run_init_5b9f90_slices(const Init5b9f90Hooks& hooks) noexcept;

}  // namespace sotc
