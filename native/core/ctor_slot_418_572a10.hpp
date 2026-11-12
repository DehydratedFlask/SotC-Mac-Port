// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 418 (RVA 0x572a10, 4726 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 418. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_418.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Player-statistics page (inference from the strings): 179 name-hash cells (stunt / win / riding / held counters).
// Rows, in constructor order (179 objects at 0x2f11880..): 0 bool variables (caller functor tag 0x0, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// Menu-less: cells only.
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

inline constexpr std::uint64_t kSlot418Rva = 0x572a10;
inline constexpr std::size_t kSlot418Size = 4726;
inline constexpr std::uint64_t kSlot418BoolFnTag = 0x0;
inline constexpr std::uint64_t kSlot418FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot418ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot418VarFlags = 1;
inline constexpr std::uint64_t kSlot418UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot418UFloatCount = 0;
inline constexpr std::size_t kSlot418BoolCount = 0;
inline constexpr std::size_t kSlot418FloatCount = 0;
inline constexpr std::size_t kSlot418RowCount = 179;
inline constexpr std::size_t kSlot418StringCount = 179;
inline constexpr std::uint64_t kSlot418IntFnTag = 0x0;
inline constexpr std::size_t kSlot418IntCount = 0;
inline constexpr std::size_t kSlot418LocalIntCount = 0;
inline constexpr std::size_t kSlot418SliderCount = 0;
inline constexpr std::uint64_t kSlot418UIntFnTag = 0x0;
inline constexpr std::size_t kSlot418UIntCount = 0;
inline constexpr std::size_t kSlot418CmdCount = 0;
inline constexpr std::uint64_t kSlot418Object0Rva = 0x2f11880;
inline constexpr std::size_t kSlot418CellCount = 179;
inline constexpr std::size_t kSlot418HolderCount = 0;
inline constexpr char kSlot418MenuStrings[kSlot418StringCount][33] = {"AgroStuntCount", "UniqueAgroStuntCount", "WinCountMINOTAUR_A", "WinCountMAMMOTH", "WinCountKNIGHT", "WinCountKIRIN", "WinCountBIRD", "WinCountMINOTAUR_B", "WinCountEEL", "WinCountYAMORI", "WinCountTORTOISE", "WinCountNARGA", "WinCountLEO", "WinCountPOSEIDON", "WinCountSNAKE", "WinCountCERBERUS", "WinCountMINOTAUR_C", "WinCountEVIS", "WinCountCampaignEasyMINOTAUR_A", "WinCountCampaignEasyMAMMOTH", "WinCountCampaignEasyKNIGHT", "WinCountCampaignEasyKIRIN", "WinCountCampaignEasyBIRD", "WinCountCampaignEasyMINOTAUR_B", "WinCountCampaignEasyEEL", "WinCountCampaignEasyYAMORI", "WinCountCampaignEasyTORTOISE", "WinCountCampaignEasyNARGA", "WinCountCampaignEasyLEO", "WinCountCampaignEasyPOSEIDON", "WinCountCampaignEasySNAKE", "WinCountCampaignEasyCERBERUS", "WinCountCampaignEasyMINOTAUR_C", "WinCountCampaignEasyEVIS", "WinCountCampaignNormalMINOTAUR_A", "WinCountCampaignNormalMAMMOTH", "WinCountCampaignNormalKNIGHT", "WinCountCampaignNormalKIRIN", "WinCountCampaignNormalBIRD", "WinCountCampaignNormalMINOTAUR_B", "WinCountCampaignNormalEEL", "WinCountCampaignNormalYAMORI", "WinCountCampaignNormalTORTOISE", "WinCountCampaignNormalNARGA", "WinCountCampaignNormalLEO", "WinCountCampaignNormalPOSEIDON", "WinCountCampaignNormalSNAKE", "WinCountCampaignNormalCERBERUS", "WinCountCampaignNormalMINOTAUR_C", "WinCountCampaignNormalEVIS", "WinCountCampaignHardMINOTAUR_A", "WinCountCampaignHardMAMMOTH", "WinCountCampaignHardKNIGHT", "WinCountCampaignHardKIRIN", "WinCountCampaignHardBIRD", "WinCountCampaignHardMINOTAUR_B", "WinCountCampaignHardEEL", "WinCountCampaignHardYAMORI", "WinCountCampaignHardTORTOISE", "WinCountCampaignHardNARGA", "WinCountCampaignHardLEO", "WinCountCampaignHardPOSEIDON", "WinCountCampaignHardSNAKE", "WinCountCampaignHardCERBERUS", "WinCountCampaignHardMINOTAUR_C", "WinCountCampaignHardEVIS", "WinCountTimeAttackMINOTAUR_A", "WinCountTimeAttackMAMMOTH", "WinCountTimeAttackKNIGHT", "WinCountTimeAttackKIRIN", "WinCountTimeAttackBIRD", "WinCountTimeAttackMINOTAUR_B", "WinCountTimeAttackEEL", "WinCountTimeAttackYAMORI", "WinCountTimeAttackTORTOISE", "WinCountTimeAttackNARGA", "WinCountTimeAttackLEO", "WinCountTimeAttackPOSEIDON", "WinCountTimeAttackSNAKE", "WinCountTimeAttackCERBERUS", "WinCountTimeAttackMINOTAUR_C", "WinCountTimeAttackEVIS", "WinCountReplayMINOTAUR_A", "WinCountReplayMAMMOTH", "WinCountReplayKNIGHT", "WinCountReplayKIRIN", "WinCountReplayBIRD", "WinCountReplayMINOTAUR_B", "WinCountReplayEEL", "WinCountReplayYAMORI", "WinCountReplayTORTOISE", "WinCountReplayNARGA", "WinCountReplayLEO", "WinCountReplayPOSEIDON", "WinCountReplaySNAKE", "WinCountReplayCERBERUS", "WinCountReplayMINOTAUR_C", "WinCountReplayEVIS", "FastestDefeat_EASY_MINOTAUR_A", "FastestDefeat_NORMAL_MINOTAUR_A", "FastestDefeat_HARD_MINOTAUR_A", "FastestDefeat_EASY_MAMMOTH", "FastestDefeat_NORMAL_MAMMOTH", "FastestDefeat_HARD_MAMMOTH", "FastestDefeat_EASY_KNIGHT", "FastestDefeat_NORMAL_KNIGHT", "FastestDefeat_HARD_KNIGHT", "FastestDefeat_EASY_KIRIN", "FastestDefeat_NORMAL_KIRIN", "FastestDefeat_HARD_KIRIN", "FastestDefeat_EASY_BIRD", "FastestDefeat_NORMAL_BIRD", "FastestDefeat_HARD_BIRD", "FastestDefeat_EASY_MINOTAUR_B", "FastestDefeat_NORMAL_MINOTAUR_B", "FastestDefeat_HARD_MINOTAUR_B", "FastestDefeat_EASY_EEL", "FastestDefeat_NORMAL_EEL", "FastestDefeat_HARD_EEL", "FastestDefeat_EASY_YAMORI", "FastestDefeat_NORMAL_YAMORI", "FastestDefeat_HARD_YAMORI", "FastestDefeat_EASY_TORTOISE", "FastestDefeat_NORMAL_TORTOISE", "FastestDefeat_HARD_TORTOISE", "FastestDefeat_EASY_NARGA", "FastestDefeat_NORMAL_NARGA", "FastestDefeat_HARD_NARGA", "FastestDefeat_EASY_LEO", "FastestDefeat_NORMAL_LEO", "FastestDefeat_HARD_LEO", "FastestDefeat_EASY_POSEIDON", "FastestDefeat_NORMAL_POSEIDON", "FastestDefeat_HARD_POSEIDON", "FastestDefeat_EASY_SNAKE", "FastestDefeat_NORMAL_SNAKE", "FastestDefeat_HARD_SNAKE", "FastestDefeat_EASY_CERBERUS", "FastestDefeat_NORMAL_CERBERUS", "FastestDefeat_HARD_CERBERUS", "FastestDefeat_EASY_MINOTAUR_C", "FastestDefeat_NORMAL_MINOTAUR_C", "FastestDefeat_HARD_MINOTAUR_C", "FastestDefeat_EASY_EVIS", "FastestDefeat_NORMAL_EVIS", "FastestDefeat_HARD_EVIS", "PrayedAtShrinesCount", "IsPraying", "UniqueShrinesVisited", "SpecialItemCount", "Collected_WhistlingArrow", "Collected_CloakOfForce", "Collected_MaskOfStrength", "Collected_LizardDetectionStone", "Collected_WhiteAgro", "Collected_MaskOfPower", "Collected_CloakOfDeception", "Collected_FlashArrow", "Collected_HarpoonOfThunder", "Collected_SwordOfTheSun", "Collected_ShamansCloak", "Collected_ShamansMask", "Collected_FruitTreeMap", "Collected_ClothOfDesperation", "Collected_CorruptWander", "Collected_QueensSword", "Discovered_SecretGarden", "JustCollectedItem", "LizardsKilled", "RidingFishTime", "LongestRidingFishTime", "FishHeldCount", "RidingHawkTime", "LongestRidingHawkTime", "HawkHeldCount", "RidingDoveTime", "LongestRidingDoveTime", "DoveHeldCount", "IsResistingTheLight"};
inline constexpr std::size_t kSlot418MenuStringLengths[kSlot418StringCount] = {0xe, 0x14, 0x12, 0xf, 0xe, 0xd, 0xc, 0x12, 0xb, 0xe, 0x10, 0xd, 0xb, 0x10, 0xd, 0x10, 0x12, 0xc, 0x1e, 0x1b, 0x1a, 0x19, 0x18, 0x1e, 0x17, 0x1a, 0x1c, 0x19, 0x17, 0x1c, 0x19, 0x1c, 0x1e, 0x18, 0x20, 0x1d, 0x1c, 0x1b, 0x1a, 0x20, 0x19, 0x1c, 0x1e, 0x1b, 0x19, 0x1e, 0x1b, 0x1e, 0x20, 0x1a, 0x1e, 0x1b, 0x1a, 0x19, 0x18, 0x1e, 0x17, 0x1a, 0x1c, 0x19, 0x17, 0x1c, 0x19, 0x1c, 0x1e, 0x18, 0x1c, 0x19, 0x18, 0x17, 0x16, 0x1c, 0x15, 0x18, 0x1a, 0x17, 0x15, 0x1a, 0x17, 0x1a, 0x1c, 0x16, 0x18, 0x15, 0x14, 0x13, 0x12, 0x18, 0x11, 0x14, 0x16, 0x13, 0x11, 0x16, 0x13, 0x16, 0x18, 0x12, 0x1d, 0x1f, 0x1d, 0x1a, 0x1c, 0x1a, 0x19, 0x1b, 0x19, 0x18, 0x1a, 0x18, 0x17, 0x19, 0x17, 0x1d, 0x1f, 0x1d, 0x16, 0x18, 0x16, 0x19, 0x1b, 0x19, 0x1b, 0x1d, 0x1b, 0x18, 0x1a, 0x18, 0x16, 0x18, 0x16, 0x1b, 0x1d, 0x1b, 0x18, 0x1a, 0x18, 0x1b, 0x1d, 0x1b, 0x1d, 0x1f, 0x1d, 0x17, 0x19, 0x17, 0x14, 0x9, 0x14, 0x10, 0x18, 0x16, 0x18, 0x1e, 0x13, 0x15, 0x1a, 0x14, 0x1a, 0x17, 0x16, 0x15, 0x16, 0x1c, 0x17, 0x15, 0x17, 0x11, 0xd, 0xe, 0x15, 0xd, 0xe, 0x15, 0xd, 0xe, 0x15, 0xd, 0x13};

struct Slot418Objects {
    NameHashCell r00;   // 0x2f11880 cell
    NameHashCell r01;   // 0x2f11890 cell
    std::uint8_t gap02[0x60];
    NameHashCell r02;   // 0x2f11900 cell
    NameHashCell r03;   // 0x2f11910 cell
    NameHashCell r04;   // 0x2f11920 cell
    NameHashCell r05;   // 0x2f11930 cell
    NameHashCell r06;   // 0x2f11940 cell
    NameHashCell r07;   // 0x2f11950 cell
    NameHashCell r08;   // 0x2f11960 cell
    NameHashCell r09;   // 0x2f11970 cell
    NameHashCell r10;   // 0x2f11980 cell
    NameHashCell r11;   // 0x2f11990 cell
    NameHashCell r12;   // 0x2f119a0 cell
    NameHashCell r13;   // 0x2f119b0 cell
    NameHashCell r14;   // 0x2f119c0 cell
    NameHashCell r15;   // 0x2f119d0 cell
    NameHashCell r16;   // 0x2f119e0 cell
    NameHashCell r17;   // 0x2f119f0 cell
    NameHashCell r18;   // 0x2f11a00 cell
    NameHashCell r19;   // 0x2f11a10 cell
    NameHashCell r20;   // 0x2f11a20 cell
    NameHashCell r21;   // 0x2f11a30 cell
    NameHashCell r22;   // 0x2f11a40 cell
    NameHashCell r23;   // 0x2f11a50 cell
    NameHashCell r24;   // 0x2f11a60 cell
    NameHashCell r25;   // 0x2f11a70 cell
    NameHashCell r26;   // 0x2f11a80 cell
    NameHashCell r27;   // 0x2f11a90 cell
    NameHashCell r28;   // 0x2f11aa0 cell
    NameHashCell r29;   // 0x2f11ab0 cell
    NameHashCell r30;   // 0x2f11ac0 cell
    NameHashCell r31;   // 0x2f11ad0 cell
    NameHashCell r32;   // 0x2f11ae0 cell
    NameHashCell r33;   // 0x2f11af0 cell
    NameHashCell r34;   // 0x2f11b00 cell
    NameHashCell r35;   // 0x2f11b10 cell
    NameHashCell r36;   // 0x2f11b20 cell
    NameHashCell r37;   // 0x2f11b30 cell
    NameHashCell r38;   // 0x2f11b40 cell
    NameHashCell r39;   // 0x2f11b50 cell
    NameHashCell r40;   // 0x2f11b60 cell
    NameHashCell r41;   // 0x2f11b70 cell
    NameHashCell r42;   // 0x2f11b80 cell
    NameHashCell r43;   // 0x2f11b90 cell
    NameHashCell r44;   // 0x2f11ba0 cell
    NameHashCell r45;   // 0x2f11bb0 cell
    NameHashCell r46;   // 0x2f11bc0 cell
    NameHashCell r47;   // 0x2f11bd0 cell
    NameHashCell r48;   // 0x2f11be0 cell
    NameHashCell r49;   // 0x2f11bf0 cell
    NameHashCell r50;   // 0x2f11c00 cell
    NameHashCell r51;   // 0x2f11c10 cell
    NameHashCell r52;   // 0x2f11c20 cell
    NameHashCell r53;   // 0x2f11c30 cell
    NameHashCell r54;   // 0x2f11c40 cell
    NameHashCell r55;   // 0x2f11c50 cell
    NameHashCell r56;   // 0x2f11c60 cell
    NameHashCell r57;   // 0x2f11c70 cell
    NameHashCell r58;   // 0x2f11c80 cell
    NameHashCell r59;   // 0x2f11c90 cell
    NameHashCell r60;   // 0x2f11ca0 cell
    NameHashCell r61;   // 0x2f11cb0 cell
    NameHashCell r62;   // 0x2f11cc0 cell
    NameHashCell r63;   // 0x2f11cd0 cell
    NameHashCell r64;   // 0x2f11ce0 cell
    NameHashCell r65;   // 0x2f11cf0 cell
    NameHashCell r66;   // 0x2f11d00 cell
    NameHashCell r67;   // 0x2f11d10 cell
    NameHashCell r68;   // 0x2f11d20 cell
    NameHashCell r69;   // 0x2f11d30 cell
    NameHashCell r70;   // 0x2f11d40 cell
    NameHashCell r71;   // 0x2f11d50 cell
    NameHashCell r72;   // 0x2f11d60 cell
    NameHashCell r73;   // 0x2f11d70 cell
    NameHashCell r74;   // 0x2f11d80 cell
    NameHashCell r75;   // 0x2f11d90 cell
    NameHashCell r76;   // 0x2f11da0 cell
    NameHashCell r77;   // 0x2f11db0 cell
    NameHashCell r78;   // 0x2f11dc0 cell
    NameHashCell r79;   // 0x2f11dd0 cell
    NameHashCell r80;   // 0x2f11de0 cell
    NameHashCell r81;   // 0x2f11df0 cell
    NameHashCell r82;   // 0x2f11e00 cell
    NameHashCell r83;   // 0x2f11e10 cell
    NameHashCell r84;   // 0x2f11e20 cell
    NameHashCell r85;   // 0x2f11e30 cell
    NameHashCell r86;   // 0x2f11e40 cell
    NameHashCell r87;   // 0x2f11e50 cell
    NameHashCell r88;   // 0x2f11e60 cell
    NameHashCell r89;   // 0x2f11e70 cell
    NameHashCell r90;   // 0x2f11e80 cell
    NameHashCell r91;   // 0x2f11e90 cell
    NameHashCell r92;   // 0x2f11ea0 cell
    NameHashCell r93;   // 0x2f11eb0 cell
    NameHashCell r94;   // 0x2f11ec0 cell
    NameHashCell r95;   // 0x2f11ed0 cell
    NameHashCell r96;   // 0x2f11ee0 cell
    NameHashCell r97;   // 0x2f11ef0 cell
    NameHashCell r98;   // 0x2f11f00 cell
    NameHashCell r99;   // 0x2f11f10 cell
    NameHashCell r100;   // 0x2f11f20 cell
    NameHashCell r101;   // 0x2f11f30 cell
    NameHashCell r102;   // 0x2f11f40 cell
    NameHashCell r103;   // 0x2f11f50 cell
    NameHashCell r104;   // 0x2f11f60 cell
    NameHashCell r105;   // 0x2f11f70 cell
    NameHashCell r106;   // 0x2f11f80 cell
    NameHashCell r107;   // 0x2f11f90 cell
    NameHashCell r108;   // 0x2f11fa0 cell
    NameHashCell r109;   // 0x2f11fb0 cell
    NameHashCell r110;   // 0x2f11fc0 cell
    NameHashCell r111;   // 0x2f11fd0 cell
    NameHashCell r112;   // 0x2f11fe0 cell
    NameHashCell r113;   // 0x2f11ff0 cell
    NameHashCell r114;   // 0x2f12000 cell
    NameHashCell r115;   // 0x2f12010 cell
    NameHashCell r116;   // 0x2f12020 cell
    NameHashCell r117;   // 0x2f12030 cell
    NameHashCell r118;   // 0x2f12040 cell
    NameHashCell r119;   // 0x2f12050 cell
    NameHashCell r120;   // 0x2f12060 cell
    NameHashCell r121;   // 0x2f12070 cell
    NameHashCell r122;   // 0x2f12080 cell
    NameHashCell r123;   // 0x2f12090 cell
    NameHashCell r124;   // 0x2f120a0 cell
    NameHashCell r125;   // 0x2f120b0 cell
    NameHashCell r126;   // 0x2f120c0 cell
    NameHashCell r127;   // 0x2f120d0 cell
    NameHashCell r128;   // 0x2f120e0 cell
    NameHashCell r129;   // 0x2f120f0 cell
    NameHashCell r130;   // 0x2f12100 cell
    NameHashCell r131;   // 0x2f12110 cell
    NameHashCell r132;   // 0x2f12120 cell
    NameHashCell r133;   // 0x2f12130 cell
    NameHashCell r134;   // 0x2f12140 cell
    NameHashCell r135;   // 0x2f12150 cell
    NameHashCell r136;   // 0x2f12160 cell
    NameHashCell r137;   // 0x2f12170 cell
    NameHashCell r138;   // 0x2f12180 cell
    NameHashCell r139;   // 0x2f12190 cell
    NameHashCell r140;   // 0x2f121a0 cell
    NameHashCell r141;   // 0x2f121b0 cell
    NameHashCell r142;   // 0x2f121c0 cell
    NameHashCell r143;   // 0x2f121d0 cell
    NameHashCell r144;   // 0x2f121e0 cell
    NameHashCell r145;   // 0x2f121f0 cell
    std::uint8_t gap146[0x8];
    NameHashCell r146;   // 0x2f12208 cell
    NameHashCell r147;   // 0x2f12218 cell
    NameHashCell r148;   // 0x2f12228 cell
    NameHashCell r149;   // 0x2f12238 cell
    NameHashCell r150;   // 0x2f12248 cell
    NameHashCell r151;   // 0x2f12258 cell
    NameHashCell r152;   // 0x2f12268 cell
    NameHashCell r153;   // 0x2f12278 cell
    NameHashCell r154;   // 0x2f12288 cell
    NameHashCell r155;   // 0x2f12298 cell
    NameHashCell r156;   // 0x2f122a8 cell
    NameHashCell r157;   // 0x2f122b8 cell
    NameHashCell r158;   // 0x2f122c8 cell
    NameHashCell r159;   // 0x2f122d8 cell
    NameHashCell r160;   // 0x2f122e8 cell
    NameHashCell r161;   // 0x2f122f8 cell
    NameHashCell r162;   // 0x2f12308 cell
    NameHashCell r163;   // 0x2f12318 cell
    NameHashCell r164;   // 0x2f12328 cell
    NameHashCell r165;   // 0x2f12338 cell
    NameHashCell r166;   // 0x2f12348 cell
    NameHashCell r167;   // 0x2f12358 cell
    NameHashCell r168;   // 0x2f12368 cell
    std::uint8_t gap169[0x10];
    NameHashCell r169;   // 0x2f12388 cell
    NameHashCell r170;   // 0x2f12398 cell
    NameHashCell r171;   // 0x2f123a8 cell
    NameHashCell r172;   // 0x2f123b8 cell
    NameHashCell r173;   // 0x2f123c8 cell
    NameHashCell r174;   // 0x2f123d8 cell
    NameHashCell r175;   // 0x2f123e8 cell
    NameHashCell r176;   // 0x2f123f8 cell
    NameHashCell r177;   // 0x2f12408 cell
    std::uint8_t gap178[0x60];
    NameHashCell r178;   // 0x2f12478 cell
};

struct Slot418Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot418Result init_slot418_572a10(Slot418Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
