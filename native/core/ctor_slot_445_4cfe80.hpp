// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 445 (RVA 0x4cfe80, 2016 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 445. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_445.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// UI option page (inference from the strings): four name holders, the uiForceAllowExtendedVideoModes bool and 79 hashed UI option string ids stored as dwords.
// Rows, in constructor order (6 objects at 0x2efb058..): 1 bool variables (caller functor tag 0x15c1950, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// UI option page batch 20: four name holders, a bool and a hash-to-dword table
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
#include "hash_store_table.hpp"
#include "fnv_path_cstr_162020.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot445Rva = 0x4cfe80;
inline constexpr std::size_t kSlot445Size = 2016;
inline constexpr std::uint64_t kSlot445BoolFnTag = 0x15c1950;
inline constexpr std::uint64_t kSlot445FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot445ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot445VarFlags = 1;
inline constexpr std::uint64_t kSlot445UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot445UFloatCount = 0;
inline constexpr std::size_t kSlot445BoolCount = 1;
inline constexpr std::size_t kSlot445FloatCount = 0;
inline constexpr std::size_t kSlot445RowCount = 6;
inline constexpr std::size_t kSlot445StringCount = 4;
inline constexpr std::uint64_t kSlot445IntFnTag = 0x0;
inline constexpr std::size_t kSlot445IntCount = 0;
inline constexpr std::size_t kSlot445LocalIntCount = 0;
inline constexpr std::size_t kSlot445SliderCount = 0;
inline constexpr std::uint64_t kSlot445UIntFnTag = 0x0;
inline constexpr std::size_t kSlot445UIntCount = 0;
inline constexpr std::size_t kSlot445CmdCount = 0;
inline constexpr std::size_t kSlot445StrVarCount = 0;
inline constexpr std::uint64_t kSlot445Object0Rva = 0x2efb058;
inline constexpr std::uint64_t kSlot445HolderVptr = 0x159bab8;
inline constexpr std::uint64_t kSlot445HolderDtorRva = 0x4cb9e0;
inline constexpr std::uint64_t kSlot445HolderVptrB = 0x159bb30;
inline constexpr std::uint64_t kSlot445HolderDtorRvaB = 0x4cba00;
inline constexpr std::size_t kSlot445CellCount = 1;
inline constexpr std::size_t kSlot445HolderCount = 4;
inline constexpr std::size_t kSlot445SentinelCount = 0;
inline constexpr std::size_t kSlot445HashStoreCount = 79;
inline constexpr std::size_t kSlot445HashTableSize = 0x164;
inline constexpr std::uint64_t kSlot445HashTableRva = 0x2efb1e0;
inline constexpr std::size_t kSlot445HashStringCount = 74;
inline constexpr std::uint64_t kSlot445HashFirstCallRva = 0x4d0146;
inline constexpr std::uint64_t kSlot445HashLastStoreRva = 0x4d063f;
inline constexpr char kSlot445BoolNames[kSlot445BoolCount][31] = {"uiForceAllowExtendedVideoModes"};
inline constexpr std::size_t kSlot445BoolNameLengths[kSlot445BoolCount] = {0x1e};
inline constexpr std::uint8_t kSlot445BoolValues[kSlot445BoolCount] = {0};
inline constexpr std::int32_t kSlot445BoolDesc[kSlot445BoolCount] = {-1};
inline constexpr std::uint8_t kSlot445BoolEarly[kSlot445BoolCount] = {1};
inline constexpr char kSlot445MenuStrings[kSlot445StringCount][16] = {"r_Wants4K", "r_WantsHDR", "r_Wants4K_Game", "r_WantsHDR_Game"};
inline constexpr std::size_t kSlot445MenuStringLengths[kSlot445StringCount] = {0x9, 0xa, 0xe, 0xf};

// Hash-to-dword store table (batch 20, inference only: UI option / button-code string ids). The original runs 79 times `lea rdi, <rodata C string> ; call 0xa2ddc0 ; mov [rbx+d], eax` (0xa2ddc0 = the path-folding FNV hash,
// native fnv_cstr_a2ddc0); the 0x164 B window at 0x2efb1e0 is NOT one array: bytes between the written dwords are NEVER written. Each record is one site (call_rva / store_rva = identity tags, never run).
// The expected hash of every site is an INDEPENDENT oracle (Python port of 0x162020, ASCII upper-case policy) used by the fixture test only; the model computes the hash natively.
struct Slot445HashTable {
    alignas(4) std::uint8_t bytes[0x164];
};
static_assert(sizeof(Slot445HashTable) == 0x164);
inline constexpr HashStoreRecord kSlot445HashStores[kSlot445HashStoreCount] = {
    {0x4d0146, 0x4d0159, 0x0, 0},
    {0x4d015b, 0x4d0167, 0x4, 1},
    {0x4d016a, 0x4d017d, 0x10, 2},
    {0x4d017f, 0x4d018b, 0x14, 3},
    {0x4d018e, 0x4d019a, 0x18, 4},
    {0x4d019d, 0x4d01a9, 0x1c, 5},
    {0x4d01ac, 0x4d01b8, 0x20, 6},
    {0x4d01bb, 0x4d01c7, 0x24, 7},
    {0x4d01ca, 0x4d01d6, 0x28, 8},
    {0x4d01d9, 0x4d01e5, 0x2c, 9},
    {0x4d01e8, 0x4d01f4, 0x30, 10},
    {0x4d01f7, 0x4d0203, 0x34, 11},
    {0x4d0206, 0x4d0212, 0x38, 12},
    {0x4d0218, 0x4d0220, 0x3c, 13},
    {0x4d0223, 0x4d022f, 0x40, 13},
    {0x4d0235, 0x4d0244, 0x50, 14},
    {0x4d0246, 0x4d0252, 0x54, 14},
    {0x4d0255, 0x4d0261, 0x58, 15},
    {0x4d0264, 0x4d0270, 0x5c, 16},
    {0x4d0273, 0x4d0286, 0x60, 17},
    {0x4d0288, 0x4d0294, 0x64, 18},
    {0x4d0297, 0x4d02a3, 0x68, 19},
    {0x4d02a6, 0x4d02b2, 0x6c, 20},
    {0x4d02b5, 0x4d02c1, 0x70, 21},
    {0x4d02c4, 0x4d02d0, 0x74, 22},
    {0x4d02d3, 0x4d02df, 0x78, 23},
    {0x4d02e2, 0x4d02ee, 0x7c, 24},
    {0x4d02f1, 0x4d02fd, 0x80, 25},
    {0x4d0300, 0x4d030c, 0x84, 26},
    {0x4d030f, 0x4d031b, 0x88, 27},
    {0x4d031e, 0x4d032a, 0x8c, 28},
    {0x4d032d, 0x4d0339, 0x90, 29},
    {0x4d033c, 0x4d0348, 0x94, 30},
    {0x4d034b, 0x4d0357, 0x98, 31},
    {0x4d035a, 0x4d0366, 0x9c, 32},
    {0x4d0369, 0x4d0375, 0xa0, 33},
    {0x4d0378, 0x4d0384, 0xa4, 34},
    {0x4d0387, 0x4d0393, 0xa8, 35},
    {0x4d0396, 0x4d03a2, 0xac, 36},
    {0x4d03a5, 0x4d03b1, 0xb0, 37},
    {0x4d03b4, 0x4d03c7, 0xc0, 38},
    {0x4d03c9, 0x4d03d5, 0xc4, 39},
    {0x4d03d8, 0x4d03e4, 0xc8, 40},
    {0x4d03e7, 0x4d03f3, 0xcc, 41},
    {0x4d03f6, 0x4d0402, 0xd0, 42},
    {0x4d0408, 0x4d041b, 0xd4, 43},
    {0x4d041d, 0x4d0429, 0xd8, 44},
    {0x4d042c, 0x4d0438, 0xdc, 45},
    {0x4d043b, 0x4d044e, 0xe0, 46},
    {0x4d0450, 0x4d045c, 0xe4, 47},
    {0x4d045f, 0x4d046b, 0xe8, 48},
    {0x4d046e, 0x4d047a, 0xec, 49},
    {0x4d047d, 0x4d0489, 0xf0, 50},
    {0x4d048c, 0x4d049f, 0xf4, 51},
    {0x4d04a1, 0x4d04ad, 0xf8, 52},
    {0x4d04b0, 0x4d04c3, 0x100, 53},
    {0x4d04c5, 0x4d04d1, 0x104, 54},
    {0x4d04d4, 0x4d04e0, 0x108, 55},
    {0x4d04e3, 0x4d04ef, 0x10c, 56},
    {0x4d04f2, 0x4d0505, 0x110, 57},
    {0x4d0507, 0x4d0513, 0x114, 58},
    {0x4d0516, 0x4d0522, 0x118, 59},
    {0x4d0525, 0x4d0538, 0x120, 60},
    {0x4d053a, 0x4d0546, 0x124, 61},
    {0x4d0549, 0x4d0555, 0x128, 62},
    {0x4d0558, 0x4d0564, 0x12c, 63},
    {0x4d0567, 0x4d057a, 0x130, 64},
    {0x4d057c, 0x4d0584, 0x134, 65},
    {0x4d0587, 0x4d059a, 0x138, 43},
    {0x4d059c, 0x4d05a8, 0x13c, 66},
    {0x4d05ab, 0x4d05b7, 0x140, 67},
    {0x4d05ba, 0x4d05cd, 0x144, 68},
    {0x4d05cf, 0x4d05db, 0x148, 69},
    {0x4d05de, 0x4d05ea, 0x14c, 70},
    {0x4d05f0, 0x4d0606, 0x150, 71},
    {0x4d0608, 0x4d0610, 0x154, 72},
    {0x4d0613, 0x4d0626, 0x158, 71},
    {0x4d0628, 0x4d0630, 0x15c, 73},
    {0x4d0633, 0x4d063f, 0x160, 72},
};
inline constexpr std::uint32_t kSlot445HashStringRva[kSlot445HashStringCount] = {0x10053f3, 0x100540c, 0x1005427, 0x100543d, 0x1005453, 0x1005469, 0x100547f, 0x1005495, 0x10054ab, 0x10054c8, 0x10054e6, 0x1005502, 0x100551c, 0x1005536, 0x100554b, 0x1005564, 0x100557c, 0x1005593, 0x10055ac, 0x10055cc, 0x10055e2, 0x10055fb, 0x1005612, 0x1005632, 0x100565a, 0x100567f, 0x100569e, 0x10056bc, 0x10056d3, 0x10056ea, 0x1005710, 0x1005737, 0x100574f, 0x100576c, 0x100578a, 0x10057a8, 0x10057d1, 0x10057f9, 0x1005815, 0x100582d, 0x1005846, 0x1005864, 0x100587c, 0x100589a, 0x10058a8, 0x10058b5, 0x10058bd, 0x10058d9, 0x10058f3, 0x1005917, 0x100593a, 0x100595c, 0x100597a, 0x1005997, 0x10059ba, 0x10059df, 0x1005a10, 0x1005a37, 0x1005a61, 0x1005a8a, 0x1005aab, 0x1005aca, 0x1005af5, 0x1005b1a, 0x1005b3a, 0x1005b5a, 0x1005b7d, 0x1005b9a, 0x1005bb4, 0x1005bcb, 0x1005be1, 0x1005bf9, 0x1005c10, 0x1005c28};
inline constexpr std::uint32_t kSlot445HashStringOffset[kSlot445HashStringCount] = {0x0, 0x19, 0x34, 0x4a, 0x60, 0x76, 0x8c, 0xa2, 0xb8, 0xd5, 0xf3, 0x10f, 0x129, 0x143, 0x158, 0x171, 0x189, 0x1a0, 0x1b9, 0x1d9, 0x1ef, 0x208, 0x21f, 0x23f, 0x267, 0x28c, 0x2ab, 0x2c9, 0x2e0, 0x2f7, 0x31d, 0x344, 0x35c, 0x379, 0x397, 0x3b5, 0x3de, 0x406, 0x422, 0x43a, 0x453, 0x471, 0x489, 0x4a7, 0x4b5, 0x4c2, 0x4ca, 0x4e6, 0x500, 0x524, 0x547, 0x569, 0x587, 0x5a4, 0x5c7, 0x5ec, 0x61d, 0x644, 0x66e, 0x697, 0x6b8, 0x6d7, 0x702, 0x727, 0x747, 0x767, 0x78a, 0x7a7, 0x7c1, 0x7d8, 0x7ee, 0x806, 0x81d, 0x835};
inline constexpr std::uint32_t kSlot445HashExpected[kSlot445HashStoreCount] = {0xb31c9fe7, 0x635c23e9, 0x3af438e4, 0xa32af696, 0x3cf43c0a, 0x3bf43a77, 0xa12af370, 0xa42af829, 0xcc405430, 0x56361845, 0xbc95de41, 0x6be639ed, 0x1ce4179a, 0x7beb1523, 0x7beb1523, 0xd76b4bab, 0xd76b4bab, 0x7ddf5bc3, 0xe232ee28, 0x8f108a7a, 0xda3fc8e3, 0x5f9d6fd9, 0x19c271dc, 0x2fa5733a, 0xf7fcb13b, 0x15d897f7, 0xa58e36e9, 0x9386318f, 0x6b65b04b, 0x8ea75f7b, 0xbeefe72c, 0x1ff5f952, 0x34926d79, 0x80c99132, 0xe01ed45e, 0xe60230b5, 0x236592bf, 0xbcaaceb9, 0x4c20fff1, 0xa91c4e1d, 0x66f63665, 0x3c27efb3, 0x80cfb9d7, 0xc8feba2d, 0x5faca2db, 0x22afbeea, 0x6fb6b350, 0xd8a43bfb, 0xfdb81853, 0x27e7a8e2, 0x9e855275, 0xc599faba, 0xb68a3ebf, 0x953ffb91, 0x2724ddbc, 0x5a0e9e9f, 0xb8747228, 0x8fb3a907, 0x2e23ceb0, 0x2bd440f8, 0xe1f1d6f6, 0x2985fd1f, 0x8af7a867, 0xee861b39, 0x2cb59aae, 0xe6549ea1, 0x4579c6ea, 0xc92ac25f, 0x22afbeea, 0x6aceca99, 0x3af553a7, 0x7c395c1d, 0x9dde8661, 0x5a377faf, 0x7e57a96a, 0xf6b84d97, 0x7e57a96a, 0x391f9776, 0xf6b84d97};
inline constexpr char kSlot445HashStringBlob[] =
    "UI_OPTION_CONTROL_NORMAL\0"
    "UI_OPTION_CONTROL_INVERTED\0"
    "UI_BUTTON_CODE_PS4_L3\0"
    "UI_BUTTON_CODE_PS4_R3\0"
    "UI_BUTTON_CODE_PS4_L1\0"
    "UI_BUTTON_CODE_PS4_L2\0"
    "UI_BUTTON_CODE_PS4_R1\0"
    "UI_BUTTON_CODE_PS4_R2\0"
    "UI_BUTTON_CODE_PS4_DPAD_LEFT\0"
    "UI_BUTTON_CODE_PS4_DPAD_RIGHT\0"
    "UI_BUTTON_CODE_PS4_TRIANGLE\0"
    "UI_BUTTON_CODE_PS4_CIRCLE\0"
    "UI_BUTTON_CODE_PS4_SQUARE\0"
    "UI_BUTTON_CODE_PS4_X\0"
    "UI_ITEM_STATUS_NOT_FOUND\0"
    "UI_ITEM_STATUS_DISABLED\0"
    "UI_ITEM_STATUS_ENABLED\0"
    "UI_OPTION_DIAGRAM_ACTION\0"
    "UI_OPTION_DIAGRAM_CONTROL_HORSE\0"
    "UI_OPTION_DIAGRAM_AIM\0"
    "UI_OPTION_DIAGRAM_ATTACK\0"
    "UI_OPTION_DIAGRAM_GRAB\0"
    "UI_OPTION_DIAGRAM_VIEW_COLOSSUS\0"
    "UI_OPTION_DIAGRAM_CHANGE_WEAPON_FORWARD\0"
    "UI_OPTION_DIAGRAM_CHANGE_WEAPON_BACK\0"
    "UI_OPTION_DIAGRAM_CAMERA_RESET\0"
    "UI_OPTION_DIAGRAM_CAMERA_ZOOM\0"
    "UI_OPTION_DIAGRAM_ROLL\0"
    "UI_OPTION_DIAGRAM_JUMP\0"
    "UI_OPTION_DIAGRAM_CHANGE_WEAPON_MELEE\0"
    "UI_OPTION_DIAGRAM_CHANGE_WEAPON_RANGED\0"
    "UI_OPTION_DIAGRAM_PAUSE\0"
    "UI_OPTION_DIAGRAM_PHOTO_MODE\0"
    "UI_OPTION_DIAGRAM_MOVE_CAMERA\0"
    "UI_OPTION_DIAGRAM_MOVE_PLAYER\0"
    "UI_OPTION_DIAGRAM_CHANGE_WEAPON_TO_SWORD\0"
    "UI_OPTION_DIAGRAM_CHANGE_WEAPON_TO_HAND\0"
    "UI_OPTION_DIAGRAM_WORLD_MAP\0"
    "UI_OPTION_SCHEME_CUSTOM\0"
    "UI_OPTION_SCHEME_DEFAULT\0"
    "UI_OPTION_SCHEME_MODERN_REMAP\0"
    "UI_OPTION_SCHEME_LEGACY\0"
    "UI_OPTION_SCHEME_LEGACY_REMAP\0"
    "UI_TOGGLE_OFF\0"
    "UI_TOGGLE_ON\0"
    "UI_AUTO\0"
    "UI_OPTION_AUDIO_DEVICE_AUTO\0"
    "UI_OPTION_AUDIO_DEVICE_TV\0"
    "UI_OPTION_AUDIO_DEVICE_HOME_THEATER\0"
    "UI_OPTION_AUDIO_DEVICE_STUDIO_PREF\0"
    "UI_OPTION_AUDIO_DEVICE_HEADPHONES\0"
    "UI_OPTION_AUDIO_CHANNELS_AUTO\0"
    "UI_OPTION_AUDIO_CHANNELS_TWO\0"
    "UI_OPTION_AUDIO_CENTERCHANNEL_AUTO\0"
    "UI_OPTION_AUDIO_CENTERCHANNEL_DIALOG\0"
    "UI_OPTION_AUDIO_CENTERCHANNEL_DIALOG_AND_EFFECTS\0"
    "UI_OPTION_AUDIO_CENTERCHANNEL_DISABLED\0"
    "UI_OPTION_AUDIO_DYNAMIC_RANGE_HOME_CINEMA\0"
    "UI_OPTION_AUDIO_DYNAMIC_RANGE_HEADPHONES\0"
    "UI_OPTION_AUDIO_DYNAMIC_RANGE_TV\0"
    "UI_OPTION_AUDIO_CONTROLLER_OFF\0"
    "UI_OPTION_AUDIO_CONTROLLER_CONTROLLER_ONLY\0"
    "UI_OPTION_AUDIO_CONTROLLER_MAIN_ONLY\0"
    "UI_OPTION_AUDIO_CONTROLLER_BOTH\0"
    "UI_OPTION_PS4_PRO_MODE_4K_30FPS\0"
    "UI_OPTION_PS4_PRO_MODE_1080P_60FPS\0"
    "UI_OPTION_HUD_MODE_GLOBAL_ON\0"
    "UI_OPTION_HUD_MODE_LEGACY\0"
    "UI_OPTION_HUD_MODE_OFF\0"
    "UI_OPTION_HUD_MODE_ON\0"
    "UI_OPTION_HUD_MODE_AUTO\0"
    "UI_OPTION_HUD_POS_LEFT\0"
    "UI_OPTION_HUD_POS_RIGHT\0"
    "UI_OPTION_HUD_POS_CENTER\0";
static_assert(sizeof(kSlot445HashStringBlob) == 2127);   // the pool plus the literal's own terminator

struct Slot445Objects {
    NameHolder30 r00;   // 0x2efb058 holder
    NameHolder30 r01;   // 0x2efb088 holder
    NameHolder30 r02;   // 0x2efb0b8 holder
    NameHolder30 r03;   // 0x2efb0e8 holder
    ConCmdBoolVar r04;   // 0x2efb118 bool
    Slot445HashTable r05;   // 0x2efb1e0 hstab
};

struct Slot445Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot445Result init_slot445_4cfe80(Slot445Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
