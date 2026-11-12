// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 381 (RVA 0x6cd5b0, 3059 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 381. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_381.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Combat-tuning page (inference from the strings): five shadow_weaponAttackMultiplier_* bounded floats, shadow_InstantKillPercentage / EasyMode damage multipliers, three EasyMode console commands and six NameHashCell achievement counters.
// Rows, in constructor order (17 objects at 0x323a0d0..): 0 bool variables (caller functor tag 0x0, dtor 0x20b0), 5 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x15cc838, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// No menu rows.
//
// NOT established: what reads the variables at run time. Any reading of the names is INFERENCE only.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "ctor_slot_539_121940.hpp"   // NameHolder30
#include "ctor_slot_family_items.hpp"
#include "fnv_cstr_161fb0.hpp"
#include "hashed_name_ctors.hpp"
#include "hashed_string_14c3a0.hpp"
#include "helper_620e10_local_int_var.hpp"
#include "helper_int_var_sites.hpp"
#include "l2_slot_common.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot381Rva = 0x6cd5b0;
inline constexpr std::size_t kSlot381Size = 3059;
inline constexpr std::uint64_t kSlot381BoolFnTag = 0x0;
inline constexpr std::uint64_t kSlot381FloatFnTag = 0x15cc838;
inline constexpr std::uint32_t kSlot381ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot381VarFlags = 1;
inline constexpr std::uint64_t kSlot381UFloatFnTag = 0x15cc838;
inline constexpr std::size_t kSlot381UFloatCount = 3;
inline constexpr std::size_t kSlot381BoolCount = 0;
inline constexpr std::size_t kSlot381FloatCount = 5;
inline constexpr std::size_t kSlot381RowCount = 17;
inline constexpr std::size_t kSlot381StringCount = 10;
inline constexpr std::uint64_t kSlot381IntFnTag = 0x0;
inline constexpr std::size_t kSlot381IntCount = 0;
inline constexpr std::size_t kSlot381LocalIntCount = 0;
inline constexpr std::size_t kSlot381SliderCount = 0;
inline constexpr std::uint64_t kSlot381UIntFnTag = 0x0;
inline constexpr std::size_t kSlot381UIntCount = 0;
inline constexpr std::size_t kSlot381CmdCount = 3;
inline constexpr std::uint64_t kSlot381Object0Rva = 0x323a0d0;
inline constexpr std::size_t kSlot381CellCount = 6;
inline constexpr std::size_t kSlot381HolderCount = 0;
inline constexpr char kSlot381FloatNames[kSlot381FloatCount][44] = {"shadow_weaponAttackMultiplier_queensSword", "shadow_weaponAttackMultiplier_sunSword", "shadow_weaponAttackMultiplier_swordOfLife", "shadow_weaponAttackMultiplier_swordOfDormin", "shadow_weaponAttackMultiplier_bow"};
inline constexpr std::size_t kSlot381FloatNameLengths[kSlot381FloatCount] = {0x29, 0x26, 0x29, 0x2b, 0x21};
inline constexpr std::uint32_t kSlot381FloatLoBits[kSlot381FloatCount] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
inline constexpr std::uint32_t kSlot381FloatHiBits[kSlot381FloatCount] = {0x40a00000, 0x40a00000, 0x40a00000, 0x40a00000, 0x40a00000};
inline constexpr std::uint32_t kSlot381FloatValueBits[kSlot381FloatCount] = {0x40400000, 0x3f000000, 0x3f800000, 0x4049999a, 0x3f800000};
inline constexpr char kSlot381UFloatNames[kSlot381UFloatCount][44] = {"shadow_InstantKillPercentage", "shadow_EasyMode_damageAgainstBossMult", "shadow_EasyMode_damageAgainstBossMultBow"};
inline constexpr std::size_t kSlot381UFloatNameLengths[kSlot381UFloatCount] = {0x1c, 0x25, 0x28};
inline constexpr std::uint32_t kSlot381UFloatValueBits[kSlot381UFloatCount] = {0x3d4ccccd, 0x3fa00000, 0x3f800000};
inline constexpr std::uint8_t kSlot381UFloatEarly[kSlot381UFloatCount] = {0, 1, 1};
inline constexpr std::int32_t kSlot381UFloatDesc[kSlot381UFloatCount] = {0, -1, -1};
inline constexpr char kSlot381CmdNames[kSlot381CmdCount][44] = {"shadow_EasyMode_SetBossVitalPointDamageMult", "shadow_EasyMode_SetBossWeakPointDamageMult", "shadow_EasyMode_ResetAllBossPartMultipliers"};
inline constexpr std::size_t kSlot381CmdNameLengths[kSlot381CmdCount] = {0x2b, 0x2a, 0x2b};
inline constexpr std::int32_t kSlot381CmdDesc[kSlot381CmdCount] = {1, 2, 3};
inline constexpr std::uint64_t kSlot381CmdHandler[kSlot381CmdCount] = {0x6ca4a0, 0x6ca7f0, 0x6cab50};
inline constexpr char kSlot381MenuStrings[kSlot381StringCount][130] = {"If the boss hit points are less than this percentage, the next stab will always kill no matter what.", "Sets a damage multiplier for a boss' vital point. Params: <boss index> <vital index> <multiplier value> [weapon type: Melee, Bow]", "Sets a damage multiplier for a boss' weak point. Params: <boss index> <vital index> <multiplier value> [weapon type: Melee, Bow]", "Resets all the boss part specific damage multipliers to 1.0. Params: None", "JumpStabColossusKill", "SkilledDefeatKNIGHT", "SkilledDefeatEEL", "YamoriTurnOverCount", "SkilledDefeatYAMORI", "QueenSwordColossusKill"};
inline constexpr std::size_t kSlot381MenuStringLengths[kSlot381StringCount] = {0x64, 0x81, 0x80, 0x49, 0x14, 0x13, 0x10, 0x13, 0x13, 0x16};

struct Slot381Objects {
    ConCmdRangedVar r00;   // 0x323a0d0 float
    ConCmdRangedVar r01;   // 0x323a1a8 float
    ConCmdRangedVar r02;   // 0x323a280 float
    ConCmdRangedVar r03;   // 0x323a358 float
    ConCmdRangedVar r04;   // 0x323a430 float
    ConCmdRangedVar r05;   // 0x323a508 ufloat
    ConCmdRangedVar r06;   // 0x323a5e0 ufloat
    ConCmdRangedVar r07;   // 0x323a6b8 ufloat
    ConCmdCommand r08;   // 0x323a790 cmd
    ConCmdCommand r09;   // 0x323a830 cmd
    ConCmdCommand r10;   // 0x323a8d0 cmd
    NameHashCell r11;   // 0x323a970 cell
    NameHashCell r12;   // 0x323a980 cell
    NameHashCell r13;   // 0x323a990 cell
    NameHashCell r14;   // 0x323a9a0 cell
    NameHashCell r15;   // 0x323a9b0 cell
    NameHashCell r16;   // 0x323a9c0 cell
};

struct Slot381Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot381Result init_slot381_6cd5b0(Slot381Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
