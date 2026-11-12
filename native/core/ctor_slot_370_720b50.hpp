// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 370 (RVA 0x720b50, 52949 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 370. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_370.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Head-tracking and player tuning page (inference from the strings): 131 bounded floats and 36 bools (shadow_headTrack_*), 5 unbounded floats, 2 local-int wrapper sites, 3 console commands, the startupArea string variable, a name holder, 10 stat cells and 43 menu rows.
// Rows, in constructor order (232 objects at 0x3245580..): 36 bool variables (caller functor tag 0x15ccff8, dtor 0x20b0), 131 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x15cd018, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// Menu: Shadow Player / Shadow Controls / Hair / Pat / Swim / Collectibles toggles and 14 sliders.
//
// NOT established: what reads the variables at run time. Any reading of the names is INFERENCE only.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "ctor_slot_539_121940.hpp"   // NameHolder30
#include "string_var_257a0.hpp"
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

inline constexpr std::uint64_t kSlot370Rva = 0x720b50;
inline constexpr std::size_t kSlot370Size = 52949;
inline constexpr std::uint64_t kSlot370BoolFnTag = 0x15ccff8;
inline constexpr std::uint64_t kSlot370FloatFnTag = 0x15cd018;
inline constexpr std::uint32_t kSlot370ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot370VarFlags = 1;
inline constexpr std::uint64_t kSlot370UFloatFnTag = 0x15cd018;
inline constexpr std::size_t kSlot370UFloatCount = 5;
inline constexpr std::size_t kSlot370BoolCount = 36;
inline constexpr std::size_t kSlot370FloatCount = 131;
inline constexpr std::size_t kSlot370RowCount = 232;
inline constexpr std::size_t kSlot370StringCount = 27;
inline constexpr std::uint64_t kSlot370IntFnTag = 0x0;
inline constexpr std::size_t kSlot370IntCount = 0;
inline constexpr std::size_t kSlot370LocalIntCount = 2;
inline constexpr std::size_t kSlot370SliderCount = 14;
inline constexpr std::uint64_t kSlot370UIntFnTag = 0x0;
inline constexpr std::size_t kSlot370UIntCount = 0;
inline constexpr std::size_t kSlot370CmdCount = 3;
inline constexpr std::size_t kSlot370StrVarCount = 1;
inline constexpr std::uint64_t kSlot370Object0Rva = 0x3245580;
inline constexpr std::uint64_t kSlot370HolderVptr = 0x15a0a48;
inline constexpr std::uint64_t kSlot370HolderDtorRva = 0x2b5050;
inline constexpr std::size_t kSlot370CellCount = 10;
inline constexpr std::size_t kSlot370HolderCount = 1;
inline constexpr std::size_t kSlot370SentinelCount = 0;
inline constexpr char kSlot370BoolNames[kSlot370BoolCount][60] = {"shadow_headTrack_debug", "shadow_headTrack_active_swordHoist", "shadow_headTrack_active_lookAroundMode", "shadow_headTrack_active_lookAroundMode_springs", "shadow_headTrack_passive_torch", "shadow_headTrack_passive_boss", "shadow_headTrack_passive_horse", "shadow_headTrack_passive_horse_popFix", "shadow_headTrack_passive_fruit", "shadow_headTrack_passive_lizard", "shadow_headTrack_passive_bird", "shadow_headTrack_passive_food", "shadow_headTrack_passive_lookAt", "shadow_headTrack_passive_lookAt_disableDuringDirectedCam", "shadow_headTrack_shared_respectEnableLookIkAnimChannel", "shadow_headTrack_shared_respectSubAnimationInterestAttr", "shadow_headTrack_shared_constraints_overrideLgeacy", "shadow_headTrack_shared_constraints_useDynamicConstraints", "shadow_player_debugRecoveryStats", "shadow_playerDebugFaint", "shadow_forceHorsePitchForAnims", "shadow_playerEnableCrouchGridBlend", "shadow_playerEnableRunSlopeGridBlend", "shadow_playerEnableRootFittingSpringBlend", "shadow_playerEnableSwimBoost", "shadow_playerDebugSwimBoost", "shadow_playerDebugContextPrompts", "shadow_unlimitedGrip", "shadow_printFidgetTime", "shadow_reparentArrowToHandL", "shadow_debugBowAimFitIk", "shadow_playerHairAdditiveDebugPrint", "shadow_enableAimingAccelerationCurve", "cp10_debugPlayerDamageReactions", "shadow_CollectiblesDebugPrintValid", "shadowEnableGetUpFromFaintTutorial"};
inline constexpr std::size_t kSlot370BoolNameLengths[kSlot370BoolCount] = {0x16, 0x22, 0x26, 0x2e, 0x1e, 0x1d, 0x1e, 0x25, 0x1e, 0x1f, 0x1d, 0x1d, 0x1f, 0x38, 0x36, 0x37, 0x32, 0x39, 0x20, 0x17, 0x1e, 0x22, 0x24, 0x29, 0x1c, 0x1b, 0x20, 0x14, 0x16, 0x1b, 0x17, 0x23, 0x24, 0x1f, 0x22, 0x22};
inline constexpr std::uint8_t kSlot370BoolValues[kSlot370BoolCount] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0};
inline constexpr char kSlot370FloatNames[kSlot370FloatCount][60] = {"shadow_headTrack_active_swordHoist_interpMaxVel", "shadow_headTrack_active_swordHoist_interpCoefficient1", "shadow_headTrack_active_lookAroundMode_dotThreshold", "shadow_headTrack_active_lookAroundMode_interpMaxVel", "shadow_headTrack_active_lookAroundMode_interpCoefficient1", "shadow_headTrack_active_boss_interpMaxVel", "shadow_headTrack_active_boss_interpCoefficient1", "shadow_headTrack_active_horse_interpMaxVel", "shadow_headTrack_active_horse_interpCoefficient1", "shadow_headTrack_passive_torch_maxSpeed", "shadow_headTrack_passive_torch_yawThreshold", "shadow_headTrack_passive_torch_pitchMinThreshold", "shadow_headTrack_passive_torch_pitchMaxThreshold", "shadow_headTrack_passive_torch_minDistance", "shadow_headTrack_passive_torch_maxDistance", "shadow_headTrack_passive_torch_interpMaxVel", "shadow_headTrack_passive_torch_interpCoefficient1", "shadow_headTrack_passive_boss_maxSpeed", "shadow_headTrack_passive_boss_yawThreshold", "shadow_headTrack_passive_boss_pitchMinThreshold", "shadow_headTrack_passive_boss_pitchMaxThreshold", "shadow_headTrack_passive_boss_minDistance", "shadow_headTrack_passive_boss_maxDistance", "shadow_headTrack_passive_boss_interpMaxVel", "shadow_headTrack_passive_boss_interpCoefficient1", "shadow_headTrack_passive_horse_maxSpeed", "shadow_headTrack_passive_horse_yawThreshold", "shadow_headTrack_passive_horse_pitchMinThreshold", "shadow_headTrack_passive_horse_pitchMaxThreshold", "shadow_headTrack_passive_horse_minDistance", "shadow_headTrack_passive_horse_maxDistance", "shadow_headTrack_passive_horse_interpMaxVel", "shadow_headTrack_passive_horse_interpCoefficient1", "shadow_headTrack_passive_fruit_minDistance", "shadow_headTrack_passive_fruit_maxDistance", "shadow_headTrack_passive_fruit_yawThreshold", "shadow_headTrack_passive_fruit_pitchMinThreshold", "shadow_headTrack_passive_fruit_pitchMaxThreshold", "shadow_headTrack_passive_fruit_interpMaxVel", "shadow_headTrack_passive_fruit_interpCoefficient1", "shadow_headTrack_passive_lizard_minDistance", "shadow_headTrack_passive_lizard_maxDistance", "shadow_headTrack_passive_lizard_yawThreshold", "shadow_headTrack_passive_lizard_pitchMinThreshold", "shadow_headTrack_passive_lizard_pitchMaxThreshold", "shadow_headTrack_passive_lizard_interpMaxVel", "shadow_headTrack_passive_lizard_interpCoefficient1", "shadow_headTrack_passive_bird_minDistance", "shadow_headTrack_passive_bird_maxDistance", "shadow_headTrack_passive_bird_yawThreshold", "shadow_headTrack_passive_bird_pitchMinThreshold", "shadow_headTrack_passive_bird_pitchMaxThreshold", "shadow_headTrack_passive_bird_interpMaxVel", "shadow_headTrack_passive_bird_interpCoefficient1", "shadow_headTrack_passive_food_minDistance", "shadow_headTrack_passive_food_maxDistance", "shadow_headTrack_passive_food_yawThreshold", "shadow_headTrack_passive_food_pitchMinThreshold", "shadow_headTrack_passive_food_pitchMaxThreshold", "shadow_headTrack_passive_food_interpMaxVel", "shadow_headTrack_passive_food_interpCoefficient1", "shadow_headTrack_passive_lookAt_maxSpeed", "shadow_headTrack_passive_lookAt_yawThreshold", "shadow_headTrack_passive_lookAt_pitchMinThreshold", "shadow_headTrack_passive_lookAt_pitchMaxThreshold", "shadow_headTrack_passive_lookAt_interpMaxVel", "shadow_headTrack_passive_lookAt_interpCoefficient1", "shadow_headTrack_passive_lookAt_maxTimeWithoutCamInput", "shadow_headTrack_passive_lookAt_maxTimeWithoutCamInputHorse", "shadow_headTrack_passive_lookAt_horseViewMinSpeed", "shadow_headTrack_shared_constraints_pitchMin", "shadow_headTrack_shared_constraints_pitchMax", "shadow_headTrack_shared_constraints_rollMinMax", "shadow_headTrack_shared_constraints_yawMinMax", "shadow_headTrack_shared_constraints_softYawOffset", "shadow_headTrack_shared_constraints_softPitchOffset", "shadow_headTrack_shared_constraints_curPitchMultiplier", "shadow_headTrack_shared_constraints_curRollMultiplier", "shadow_headTrack_shared_constraints_curYawMultiplier", "shadow_headTrack_shared_interpolation_initMaxVel", "shadow_headTrack_shared_interpolation_initCoefficient1", "shadow_headTrack_shared_interpolation_curMaxVel", "shadow_headTrack_shared_interpolation_curCoefficient1", "shadow_player_swordOfLife_hpRegenAdditionPercent", "shadow_player_icoTabbard_hpRegenAdditionPercent", "shadow_player_swordOfDormin_hpRegenSubtractionPercent", "shadow_deadZoneAiming", "shadow_maxHorsePitchForAnims", "shadow_forceHorsePitchForAnimsBlend", "shadow_horsePitchBlendForMountBckAnim2", "shadow_horsePitchBlendForMountBckAnim1", "shadow_playerToeDiffBlendForCrouch", "shadow_playerRunSlopeGridBlendCoe1", "shadow_playerRunSlopeGridBlendCoe2", "shadow_playerSwimBoostChangeScale", "shadow_playerSwimBoostRootVelScale", "shadow_playerSwimBoostAnimePlayScale", "shadow_playerSwimBoostTurnSpeedCoeff1", "shadow_playerSwimBoostTurnSpeedCoeff2", "shadow_minInitialFidgetTime", "shadow_maxInitialFidgetTime", "shadow_minBetweenFidgetTime", "shadow_maxBetweenFidgetTime", "shadow_reparentArrowBlendThreshold", "shadow_playerEasyFaintTimeScale", "shadow_playerEasyFaintEscapeInputScale", "shadow_playerHorsePatNear2Dist", "shadow_playerHorsePatNearDist", "shadow_playerHorsePatMidDist", "shadow_playerHorsePatFarDist", "shadow_playerHorsePatStepMargin", "shadow_playerHorsePatHandMargin", "shadow_playerHorseRightSideMargin", "shadow_playerHorsePatDebugTestDist", "shadow_playerHairAdditiveMinPitch", "shadow_playerHairAdditiveMaxPitch", "shadow_playerHairAdditiveDebugBlend", "cp10_playerClothPhysicsInterpSpeed", "cp10_playerClothDesiredInertiaDampening_Default", "cp10_playerClothDesiredPhysicsDampening_Default", "cp10_playerClothDesiredInertiaDampening_OnBoss", "cp10_playerClothDesiredPhysicsDampening_OnBoss", "cp10_playerClothDesiredInertiaDampening_OnHorse", "cp10_playerClothDesiredPhysicsDampening_OnHorse", "shadowFastHorseMountMinSpeed", "shadowFastHorseMountMaxX", "shadowFastHorseMountMinY", "shadowFastHorseMountMaxY", "shadowFastHorseMountMinZ", "shadowFastHorseMountMaxZ", "shadowFastHorseMountMaxAngle"};
inline constexpr std::size_t kSlot370FloatNameLengths[kSlot370FloatCount] = {0x2f, 0x35, 0x33, 0x33, 0x39, 0x29, 0x2f, 0x2a, 0x30, 0x27, 0x2b, 0x30, 0x30, 0x2a, 0x2a, 0x2b, 0x31, 0x26, 0x2a, 0x2f, 0x2f, 0x29, 0x29, 0x2a, 0x30, 0x27, 0x2b, 0x30, 0x30, 0x2a, 0x2a, 0x2b, 0x31, 0x2a, 0x2a, 0x2b, 0x30, 0x30, 0x2b, 0x31, 0x2b, 0x2b, 0x2c, 0x31, 0x31, 0x2c, 0x32, 0x29, 0x29, 0x2a, 0x2f, 0x2f, 0x2a, 0x30, 0x29, 0x29, 0x2a, 0x2f, 0x2f, 0x2a, 0x30, 0x28, 0x2c, 0x31, 0x31, 0x2c, 0x32, 0x36, 0x3b, 0x31, 0x2c, 0x2c, 0x2e, 0x2d, 0x31, 0x33, 0x36, 0x35, 0x34, 0x30, 0x36, 0x2f, 0x35, 0x30, 0x2f, 0x35, 0x15, 0x1c, 0x23, 0x26, 0x26, 0x22, 0x22, 0x22, 0x21, 0x22, 0x24, 0x25, 0x25, 0x1b, 0x1b, 0x1b, 0x1b, 0x22, 0x1f, 0x26, 0x1e, 0x1d, 0x1c, 0x1c, 0x1f, 0x1f, 0x21, 0x22, 0x21, 0x21, 0x23, 0x22, 0x2f, 0x2f, 0x2e, 0x2e, 0x2f, 0x2f, 0x1c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1c};
inline constexpr std::uint32_t kSlot370FloatLoBits[kSlot370FloatCount] = {0x00000000, 0x00000000, 0xbf800000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xc2b40000, 0xc2b40000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xc2b40000, 0xc2b40000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xc2b40000, 0xc2b40000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xc2b40000, 0xc2b40000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xc2b40000, 0xc2b40000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xc2b40000, 0xc2b40000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xc2b40000, 0xc2b40000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xc2b40000, 0xc2b40000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xc3340000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xbf800000, 0x00000000, 0x00000000, 0x00000000, 0x3dcccccd, 0x3dcccccd, 0x3c23d70a, 0x3c23d70a, 0x3c23d70a, 0x3c23d70a, 0x3c23d70a, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xc2c80000, 0x00000000, 0xbf800000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xbf800000, 0xbf800000, 0xbf800000, 0xbf800000, 0xc3340000};
inline constexpr std::uint32_t kSlot370FloatHiBits[kSlot370FloatCount] = {0x7f7fffff, 0x7f7fffff, 0x3f800000, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x43340000, 0x42b40000, 0x42b40000, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x43340000, 0x42b40000, 0x42b40000, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x43340000, 0x42b40000, 0x42b40000, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x43340000, 0x42b40000, 0x42b40000, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x43340000, 0x42b40000, 0x42b40000, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x43340000, 0x42b40000, 0x42b40000, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x43340000, 0x42b40000, 0x42b40000, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x43340000, 0x42b40000, 0x42b40000, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x00000000, 0x43340000, 0x43340000, 0x43340000, 0x42b40000, 0x42b40000, 0x42b40000, 0x42b40000, 0x42b40000, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x7f7fffff, 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000, 0x42960000, 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000, 0x42200000, 0x42200000, 0x3f800000, 0x41200000, 0x41200000, 0x41200000, 0x41200000, 0x42f00000, 0x42f00000, 0x42f00000, 0x42f00000, 0x3f800000, 0x41200000, 0x41200000, 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000, 0x00000000, 0x42c80000, 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000, 0x41a00000, 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000, 0x43340000};
inline constexpr std::uint32_t kSlot370FloatValueBits[kSlot370FloatCount] = {0x43480000, 0x40800000, 0x3f000000, 0x440e8000, 0x41900000, 0x44048000, 0x41700000, 0x43d20000, 0x41800000, 0x42480000, 0x42be0000, 0xc20c0000, 0x41700000, 0x00000000, 0x00000000, 0x43d20000, 0x41400000, 0x42480000, 0x42be0000, 0xc20c0000, 0x42340000, 0x3f800000, 0x00000000, 0x43d20000, 0x41400000, 0x42480000, 0x42b40000, 0xc20c0000, 0x42340000, 0x3f800000, 0x41000000, 0x43d20000, 0x40c00000, 0x3f000000, 0x41600000, 0x42f00000, 0xc2aa0000, 0x42aa0000, 0x43b40000, 0x40a00000, 0x3f000000, 0x41400000, 0x42be0000, 0xc2700000, 0x42700000, 0x43960000, 0x40c00000, 0x3dcccccd, 0x41400000, 0x43070000, 0xc2700000, 0x42a00000, 0x43c80000, 0x41400000, 0x3e4ccccd, 0x41600000, 0x428c0000, 0xc2a00000, 0x42200000, 0x43480000, 0x41000000, 0x42480000, 0x42c80000, 0xc2200000, 0x42480000, 0x435c0000, 0x40c00000, 0x40c00000, 0x3f800000, 0x40400000, 0xc1a00000, 0x42480000, 0x41a00000, 0x42700000, 0x41f00000, 0x41a00000, 0x3f800000, 0x3f800000, 0x3f800000, 0x43898000, 0x41000000, 0x43480000, 0x41200000, 0x3e19999a, 0x3e19999a, 0x3e19999a, 0x3e4ccccd, 0x42480000, 0x00000000, 0x3f800000, 0x3ee8ba2e, 0x3f800000, 0x41a00000, 0x40a00000, 0x3eb33333, 0x3e99999a, 0x3e99999a, 0x40a00000, 0x40000000, 0x41700000, 0x41f00000, 0x40a00000, 0x41700000, 0x3f666666, 0x3f555555, 0x3f8ccccd, 0x3dd2f1aa, 0x3e428f5c, 0x3e800000, 0x3ec28f5c, 0x3e083127, 0x3d27ef9e, 0x00000000, 0x00000000, 0xc1c80000, 0x42700000, 0x00000000, 0x3e99999a, 0x3ecccccd, 0x3ecccccd, 0x3e99999a, 0x3f4ccccd, 0x3e99999a, 0x3ecccccd, 0x41000000, 0x3f333333, 0xbf400000, 0xbe99999a, 0xbf000000, 0x3ecccccd, 0x42dc0000};
inline constexpr char kSlot370UFloatNames[kSlot370UFloatCount][60] = {"shadow_headTrack_shared_constraints_crouchYawMultiplier", "shadow_headTrack_shared_constraints_crouchPitchMultiplier", "shadow_headTrack_shared_constraints_crouchRollMultiplier", "shadow_playerSaveHealAmountPerSecond", "shadow_playerSaveGripAmountPerSecond"};
inline constexpr std::size_t kSlot370UFloatNameLengths[kSlot370UFloatCount] = {0x37, 0x39, 0x38, 0x24, 0x24};
inline constexpr std::uint32_t kSlot370UFloatValueBits[kSlot370UFloatCount] = {0x3f400000, 0x3f000000, 0x3f000000, 0x3f000000, 0x3f000000};
inline constexpr std::uint8_t kSlot370UFloatEarly[kSlot370UFloatCount] = {1, 1, 1, 1, 1};
inline constexpr std::int32_t kSlot370UFloatDesc[kSlot370UFloatCount] = {-1, -1, -1, -1, -1};
inline constexpr char kSlot370CmdNames[kSlot370CmdCount][60] = {"shadow_EasyMode_SetPlayerDamageClassMultiplier", "startupLocation", "startupScriptSpawnLocation"};
inline constexpr std::size_t kSlot370CmdNameLengths[kSlot370CmdCount] = {0x2e, 0xf, 0x1a};
inline constexpr std::int32_t kSlot370CmdDesc[kSlot370CmdCount] = {16, 18, 19};
inline constexpr std::uint64_t kSlot370CmdHandler[kSlot370CmdCount] = {0x70fe20, 0x7105a0, 0x710660};
inline constexpr char kSlot370StrNames[kSlot370StrVarCount][60] = {"startupArea"};
inline constexpr std::size_t kSlot370StrNameLengths[kSlot370StrVarCount] = {0xb};
inline constexpr std::int32_t kSlot370StrInit[kSlot370StrVarCount] = {20};
// Local-int wrapper sites (0x620e10 family): wrapper RVA, name, init value, range, flags and the call site's own argument-functor tag; the description is the static empty string 0xf730 (never released).
// Per-site description: nullptr = the static empty string 0xf730 (never released); a literal = a hand-built borrowed view that the site RELEASES between the wrapper and the functor destroy (the 7ec160 order, release_description = true).
inline constexpr const char* kSlot370LintDesc[kSlot370LocalIntCount] = {nullptr, nullptr};
inline constexpr std::size_t kSlot370LintDescLength[kSlot370LocalIntCount] = {0, 0};
inline constexpr LocalIntSiteSpec kSlot370LocalIntSites[kSlot370LocalIntCount] = {{0x720430, "shadow_playerHeadTrackBehavior", 30, 0, 0, 2, 1, 0x15ccf60, false}, {0x7207f0, "shadow_playerBreathlessBehavior", 31, 0, 0, 2, 1, 0x15cd038, false}};
inline constexpr char kSlot370MenuStrings[kSlot370StringCount][112] = {"BowShotsTaken", "HarpoonsThrown", "Shadow Player", "Debug Damage Reactions", "Min Time Initial Fidget", "Max Time Initial Fidget", "Min Time Between Fidgets", "Max Time Between Fidgets", "Breathless Behavior", "Shadow Player Hair", "Shadow Player Swim", "Shadow Player Pat", "Shadow Controls", "Collectibles", "A11_Torch_Lit", "HighestSurvivedFall", "Sets a damage multiplier for damage class that affects the player. Params: <damage def name> <multiplier value>", "DeathCount", "Specify the startup location (x, y, z)", "Specify the startup location relative to the specified startupScript (x, y, z)", "", "poiPlayerStartingPositions", "FruitEaten", "PoisonedApplesEaten", "LizardTailsCollected", "ShinyLizardTailsCollected", "CurrentShinyLizardTailsCollected"};
inline constexpr std::size_t kSlot370MenuStringLengths[kSlot370StringCount] = {0xd, 0xe, 0xd, 0x16, 0x17, 0x17, 0x18, 0x18, 0x13, 0x12, 0x12, 0x11, 0xf, 0xc, 0xd, 0x13, 0x6f, 0xa, 0x26, 0x4e, 0x0, 0x1a, 0xa, 0x13, 0x14, 0x19, 0x20};
inline constexpr std::uint32_t kSlot370SliderLoBits[kSlot370SliderCount] = {0x3c23d70a, 0x3c23d70a, 0x3c23d70a, 0x3c23d70a, 0x3c23d70a, 0x3c23d70a, 0x3a83126f, 0x3a83126f, 0x3a83126f, 0x3a83126f, 0x3a83126f, 0x3a83126f, 0x3a83126f, 0x3a83126f};
inline constexpr std::uint32_t kSlot370SliderHiBits[kSlot370SliderCount] = {0x3dcccccd, 0x3dcccccd, 0x3dcccccd, 0x3dcccccd, 0x3dcccccd, 0x3dcccccd, 0x3c23d70a, 0x3c23d70a, 0x3c23d70a, 0x3c23d70a, 0x3c23d70a, 0x3c23d70a, 0x3c23d70a, 0x3c23d70a};

struct Slot370Objects {
    ConCmdRangedVar r00;   // 0x3245580 lint
    ConCmdBoolVar r01;   // 0x3245658 bool
    ConCmdBoolVar r02;   // 0x3245720 bool
    ConCmdRangedVar r03;   // 0x32457e8 float
    ConCmdRangedVar r04;   // 0x32458c0 float
    ConCmdBoolVar r05;   // 0x3245998 bool
    ConCmdBoolVar r06;   // 0x3245a60 bool
    ConCmdRangedVar r07;   // 0x3245b28 float
    ConCmdRangedVar r08;   // 0x3245c00 float
    ConCmdRangedVar r09;   // 0x3245cd8 float
    ConCmdRangedVar r10;   // 0x3245db0 float
    ConCmdRangedVar r11;   // 0x3245e88 float
    ConCmdRangedVar r12;   // 0x3245f60 float
    ConCmdRangedVar r13;   // 0x3246038 float
    ConCmdBoolVar r14;   // 0x3246110 bool
    ConCmdRangedVar r15;   // 0x32461d8 float
    ConCmdRangedVar r16;   // 0x32462b0 float
    ConCmdRangedVar r17;   // 0x3246388 float
    ConCmdRangedVar r18;   // 0x3246460 float
    ConCmdRangedVar r19;   // 0x3246538 float
    ConCmdRangedVar r20;   // 0x3246610 float
    ConCmdRangedVar r21;   // 0x32466e8 float
    ConCmdRangedVar r22;   // 0x32467c0 float
    ConCmdBoolVar r23;   // 0x3246898 bool
    ConCmdRangedVar r24;   // 0x3246960 float
    ConCmdRangedVar r25;   // 0x3246a38 float
    ConCmdRangedVar r26;   // 0x3246b10 float
    ConCmdRangedVar r27;   // 0x3246be8 float
    ConCmdRangedVar r28;   // 0x3246cc0 float
    ConCmdRangedVar r29;   // 0x3246d98 float
    ConCmdRangedVar r30;   // 0x3246e70 float
    ConCmdRangedVar r31;   // 0x3246f48 float
    ConCmdBoolVar r32;   // 0x3247020 bool
    ConCmdRangedVar r33;   // 0x32470e8 float
    ConCmdRangedVar r34;   // 0x32471c0 float
    ConCmdRangedVar r35;   // 0x3247298 float
    ConCmdRangedVar r36;   // 0x3247370 float
    ConCmdRangedVar r37;   // 0x3247448 float
    ConCmdRangedVar r38;   // 0x3247520 float
    ConCmdRangedVar r39;   // 0x32475f8 float
    ConCmdRangedVar r40;   // 0x32476d0 float
    ConCmdBoolVar r41;   // 0x32477a8 bool
    ConCmdBoolVar r42;   // 0x3247870 bool
    ConCmdRangedVar r43;   // 0x3247938 float
    ConCmdRangedVar r44;   // 0x3247a10 float
    ConCmdRangedVar r45;   // 0x3247ae8 float
    ConCmdRangedVar r46;   // 0x3247bc0 float
    ConCmdRangedVar r47;   // 0x3247c98 float
    ConCmdRangedVar r48;   // 0x3247d70 float
    ConCmdRangedVar r49;   // 0x3247e48 float
    ConCmdBoolVar r50;   // 0x3247f20 bool
    ConCmdRangedVar r51;   // 0x3247fe8 float
    ConCmdRangedVar r52;   // 0x32480c0 float
    ConCmdRangedVar r53;   // 0x3248198 float
    ConCmdRangedVar r54;   // 0x3248270 float
    ConCmdRangedVar r55;   // 0x3248348 float
    ConCmdRangedVar r56;   // 0x3248420 float
    ConCmdRangedVar r57;   // 0x32484f8 float
    ConCmdBoolVar r58;   // 0x32485d0 bool
    ConCmdRangedVar r59;   // 0x3248698 float
    ConCmdRangedVar r60;   // 0x3248770 float
    ConCmdRangedVar r61;   // 0x3248848 float
    ConCmdRangedVar r62;   // 0x3248920 float
    ConCmdRangedVar r63;   // 0x32489f8 float
    ConCmdRangedVar r64;   // 0x3248ad0 float
    ConCmdRangedVar r65;   // 0x3248ba8 float
    ConCmdBoolVar r66;   // 0x3248c80 bool
    ConCmdRangedVar r67;   // 0x3248d48 float
    ConCmdRangedVar r68;   // 0x3248e20 float
    ConCmdRangedVar r69;   // 0x3248ef8 float
    ConCmdRangedVar r70;   // 0x3248fd0 float
    ConCmdRangedVar r71;   // 0x32490a8 float
    ConCmdRangedVar r72;   // 0x3249180 float
    ConCmdRangedVar r73;   // 0x3249258 float
    ConCmdBoolVar r74;   // 0x3249330 bool
    ConCmdRangedVar r75;   // 0x32493f8 float
    ConCmdRangedVar r76;   // 0x32494d0 float
    ConCmdRangedVar r77;   // 0x32495a8 float
    ConCmdRangedVar r78;   // 0x3249680 float
    ConCmdRangedVar r79;   // 0x3249758 float
    ConCmdRangedVar r80;   // 0x3249830 float
    ConCmdBoolVar r81;   // 0x3249908 bool
    ConCmdRangedVar r82;   // 0x32499d0 float
    ConCmdRangedVar r83;   // 0x3249aa8 float
    ConCmdRangedVar r84;   // 0x3249b80 float
    ConCmdBoolVar r85;   // 0x3249c58 bool
    ConCmdBoolVar r86;   // 0x3249d20 bool
    ConCmdBoolVar r87;   // 0x3249de8 bool
    ConCmdRangedVar r88;   // 0x3249eb0 float
    ConCmdRangedVar r89;   // 0x3249f88 float
    ConCmdRangedVar r90;   // 0x324a060 float
    ConCmdRangedVar r91;   // 0x324a138 float
    ConCmdRangedVar r92;   // 0x324a210 float
    ConCmdRangedVar r93;   // 0x324a2e8 float
    ConCmdRangedVar r94;   // 0x324a3c0 float
    ConCmdRangedVar r95;   // 0x324a498 float
    ConCmdRangedVar r96;   // 0x324a570 float
    ConCmdBoolVar r97;   // 0x324a648 bool
    ConCmdRangedVar r98;   // 0x324a710 ufloat
    ConCmdRangedVar r99;   // 0x324a7e8 ufloat
    ConCmdRangedVar r100;   // 0x324a8c0 ufloat
    ConCmdRangedVar r101;   // 0x324a998 float
    ConCmdRangedVar r102;   // 0x324aa70 float
    ConCmdRangedVar r103;   // 0x324ab48 float
    ConCmdRangedVar r104;   // 0x324ac20 float
    ConCmdRangedVar r105;   // 0x324acf8 lint
    ConCmdRangedVar r106;   // 0x324add0 float
    ConCmdRangedVar r107;   // 0x324aea8 float
    ConCmdRangedVar r108;   // 0x324af80 float
    ConCmdBoolVar r109;   // 0x324b058 bool
    ConCmdRangedVar r110;   // 0x324b120 float
    ConCmdBoolVar r111;   // 0x324b1f8 bool
    ConCmdRangedVar r112;   // 0x324b2c0 float
    ConCmdRangedVar r113;   // 0x324b398 float
    ConCmdBoolVar r114;   // 0x324b470 bool
    ConCmdRangedVar r115;   // 0x324b538 float
    ConCmdRangedVar r116;   // 0x324b610 float
    ConCmdRangedVar r117;   // 0x324b6e8 float
    ConCmdBoolVar r118;   // 0x324b7c0 bool
    ConCmdBoolVar r119;   // 0x324b888 bool
    ConCmdRangedVar r120;   // 0x324b950 float
    ConCmdRangedVar r121;   // 0x324ba28 float
    ConCmdBoolVar r122;   // 0x324bb00 bool
    ConCmdBoolVar r123;   // 0x324bbc8 bool
    ConCmdBoolVar r124;   // 0x324bc90 bool
    ConCmdRangedVar r125;   // 0x324bd58 float
    ConCmdRangedVar r126;   // 0x324be30 float
    ConCmdRangedVar r127;   // 0x324bf08 float
    ConCmdRangedVar r128;   // 0x324bfe0 float
    ConCmdRangedVar r129;   // 0x324c0b8 float
    ConCmdRangedVar r130;   // 0x324c190 ufloat
    ConCmdRangedVar r131;   // 0x324c268 ufloat
    ConCmdBoolVar r132;   // 0x324c340 bool
    ConCmdBoolVar r133;   // 0x324c408 bool
    ConCmdRangedVar r134;   // 0x324c4d0 float
    ConCmdRangedVar r135;   // 0x324c5a8 float
    ConCmdRangedVar r136;   // 0x324c680 float
    ConCmdRangedVar r137;   // 0x324c758 float
    ConCmdBoolVar r138;   // 0x324c830 bool
    ConCmdBoolVar r139;   // 0x324c8f8 bool
    ConCmdRangedVar r140;   // 0x324c9c0 float
    ConCmdBoolVar r141;   // 0x324ca98 bool
    ConCmdRangedVar r142;   // 0x324cb60 float
    ConCmdRangedVar r143;   // 0x324cc38 float
    ConCmdRangedVar r144;   // 0x324cd10 float
    ConCmdRangedVar r145;   // 0x324cde8 float
    ConCmdRangedVar r146;   // 0x324cec0 float
    ConCmdRangedVar r147;   // 0x324cf98 float
    ConCmdRangedVar r148;   // 0x324d070 float
    ConCmdRangedVar r149;   // 0x324d148 float
    ConCmdRangedVar r150;   // 0x324d220 float
    ConCmdRangedVar r151;   // 0x324d2f8 float
    ConCmdRangedVar r152;   // 0x324d3d0 float
    ConCmdRangedVar r153;   // 0x324d4a8 float
    ConCmdRangedVar r154;   // 0x324d580 float
    ConCmdBoolVar r155;   // 0x324d658 bool
    ConCmdBoolVar r156;   // 0x324d720 bool
    ConCmdBoolVar r157;   // 0x324d7e8 bool
    ConCmdBoolVar r158;   // 0x324d8b0 bool
    NameHashCell r159;   // 0x324d978 cell
    NameHashCell r160;   // 0x324d988 cell
    DebugMenuItem r161;   // 0x324d998 toge
    DebugMenuItem r162;   // 0x324da40 toge
    DebugMenuItem r163;   // 0x324dae8 toge
    DebugMenuItem r164;   // 0x324db90 toge
    DebugMenuItem r165;   // 0x324dc38 toge
    DebugMenuItem r166;   // 0x324dce0 tog
    DebugMenuItem r167;   // 0x324dd88 tog
    DebugMenuItem r168;   // 0x324de30 tog
    DebugMenuItem r169;   // 0x324ded8 tog
    DebugMenuItem r170;   // 0x324df80 tog
    DebugMenuItem r171;   // 0x324e028 tog
    DebugMenuItem r172;   // 0x324e0d0 tog
    DebugMenuItem r173;   // 0x324e178 tog
    DebugMenuItem r174;   // 0x324e220 tog
    DebugMenuItem r175;   // 0x324e2c8 toge
    DebugMenuItem r176;   // 0x324e370 tog
    DebugMenuItem r177;   // 0x324e418 tog
    DebugMenuItem r178;   // 0x324e4c0 slider
    DebugMenuItem r179;   // 0x324e568 tog
    DebugMenuItem r180;   // 0x324e610 tog
    DebugMenuItem r181;   // 0x324e6b8 tog
    DebugMenuItem r182;   // 0x324e760 slider
    DebugMenuItem r183;   // 0x324e808 slider
    DebugMenuItem r184;   // 0x324e8b0 slider
    DebugMenuItem r185;   // 0x324e958 slider
    DebugMenuItem r186;   // 0x324ea00 slider
    DebugMenuItem r187;   // 0x324eaa8 slider
    DebugMenuItem r188;   // 0x324eb50 slider
    DebugMenuItem r189;   // 0x324ebf8 slider
    DebugMenuItem r190;   // 0x324eca0 slider
    DebugMenuItem r191;   // 0x324ed48 slider
    DebugMenuItem r192;   // 0x324edf0 slider
    DebugMenuItem r193;   // 0x324ee98 slider
    DebugMenuItem r194;   // 0x324ef40 slider
    DebugMenuItem r195;   // 0x324efe8 tog
    DebugMenuItem r196;   // 0x324f090 tog
    std::uint8_t gap197[0x20];
    ConCmdBoolVar r197;   // 0x324f158 bool
    NameHashCell r198;   // 0x324f220 cell
    NameHashCell r199;   // 0x324f230 cell
    ConCmdCommand r200;   // 0x324f240 cmd
    NameHashCell r201;   // 0x324f2e0 cell
    std::uint8_t gap202[0x20];
    ConCmdCommand r202;   // 0x324f310 cmd
    std::uint8_t gap203[0x20];
    ConCmdCommand r203;   // 0x324f3d0 cmd
    ConCmdStringVar r204;   // 0x324f470 str
    NameHolder30 r205;   // 0x324f5b8 holder
    ConCmdRangedVar r206;   // 0x324f5e8 float
    ConCmdRangedVar r207;   // 0x324f6c0 float
    ConCmdRangedVar r208;   // 0x324f798 float
    ConCmdRangedVar r209;   // 0x324f870 float
    ConCmdRangedVar r210;   // 0x324f948 float
    ConCmdRangedVar r211;   // 0x324fa20 float
    ConCmdRangedVar r212;   // 0x324faf8 float
    DebugMenuItem r213;   // 0x324fbd0 tog
    DebugMenuItem r214;   // 0x324fc78 tog
    DebugMenuItem r215;   // 0x324fd20 tog
    DebugMenuItem r216;   // 0x324fdc8 tog
    DebugMenuItem r217;   // 0x324fe70 tog
    DebugMenuItem r218;   // 0x324ff18 tog
    DebugMenuItem r219;   // 0x324ffc0 tog
    NameHashCell r220;   // 0x3250068 cell
    NameHashCell r221;   // 0x3250078 cell
    NameHashCell r222;   // 0x3250088 cell
    NameHashCell r223;   // 0x3250098 cell
    NameHashCell r224;   // 0x32500a8 cell
    ConCmdRangedVar r225;   // 0x32500b8 float
    ConCmdRangedVar r226;   // 0x3250190 float
    ConCmdRangedVar r227;   // 0x3250268 float
    ConCmdRangedVar r228;   // 0x3250340 float
    ConCmdRangedVar r229;   // 0x3250418 float
    ConCmdRangedVar r230;   // 0x32504f0 float
    ConCmdRangedVar r231;   // 0x32505c8 float
};

struct Slot370Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot370Result init_slot370_720b50(Slot370Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
