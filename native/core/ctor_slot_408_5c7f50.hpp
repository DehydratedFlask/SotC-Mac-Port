// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 408 (RVA 0x5c7f50, 4750 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 408. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_408.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Render settings page (inference from the strings): a perspective-projection matrix, seven named setting holders, vector headers and the CP10_* / shadow_* / boss* console commands.
// Rows, in constructor order (25 objects at 0x2fda41c..): 0 bool variables (caller functor tag 0x0, dtor 0x20b0), 0 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x0, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// render settings page batch 16: perspective prelude, seven holders of four classes, three vector headers and fourteen commands
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
#include "helper_1580a0_perspective.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot408Rva = 0x5c7f50;
inline constexpr std::size_t kSlot408Size = 4750;
inline constexpr std::uint64_t kSlot408BoolFnTag = 0x0;
inline constexpr std::uint64_t kSlot408FloatFnTag = 0x0;
inline constexpr std::uint32_t kSlot408ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot408VarFlags = 1;
inline constexpr std::uint64_t kSlot408UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot408UFloatCount = 0;
inline constexpr std::size_t kSlot408BoolCount = 0;
inline constexpr std::size_t kSlot408FloatCount = 0;
inline constexpr std::size_t kSlot408RowCount = 25;
inline constexpr std::size_t kSlot408StringCount = 20;
inline constexpr std::uint64_t kSlot408IntFnTag = 0x0;
inline constexpr std::size_t kSlot408IntCount = 0;
inline constexpr std::size_t kSlot408LocalIntCount = 0;
inline constexpr std::size_t kSlot408SliderCount = 0;
inline constexpr std::uint64_t kSlot408UIntFnTag = 0x0;
inline constexpr std::size_t kSlot408UIntCount = 0;
inline constexpr std::size_t kSlot408CmdCount = 14;
inline constexpr std::size_t kSlot408StrVarCount = 0;
inline constexpr std::uint64_t kSlot408Object0Rva = 0x2fda41c;
inline constexpr std::uint64_t kSlot408HolderVptr = 0x15976b0;
inline constexpr std::uint64_t kSlot408HolderDtorRva = 0x8ef00;
inline constexpr std::uint64_t kSlot408HolderVptrB = 0x159e1b8;
inline constexpr std::uint64_t kSlot408HolderDtorRvaB = 0x2c84f0;
inline constexpr std::uint64_t kSlot408HolderVptrC = 0x15cb1b8;
inline constexpr std::uint64_t kSlot408HolderDtorRvaC = 0x5b5470;
inline constexpr std::uint64_t kSlot408HolderVptrD = 0x15cb1d0;
inline constexpr std::uint64_t kSlot408HolderDtorRvaD = 0x5b5490;
inline constexpr std::size_t kSlot408CellCount = 1;
inline constexpr std::size_t kSlot408HolderCount = 7;
inline constexpr std::size_t kSlot408SentinelCount = 1;
inline constexpr std::uint64_t kSlot408SentinelDtorRva = 0x5b55c0;
inline constexpr std::size_t kSlot408ZVecCount = 2;
inline constexpr std::uint32_t kSlot408PerspectiveFovBits = 0x3f490fdb;
inline constexpr std::uint32_t kSlot408PerspectiveAspectBits = 0x3fe38e39;
inline constexpr std::uint32_t kSlot408PerspectiveNearBits = 0x3f000000;
inline constexpr std::uint32_t kSlot408PerspectiveFarBits = 0x45834000;
inline constexpr std::uint32_t kSlot408PerspectiveFlag = 1;
inline constexpr char kSlot408CmdNames[kSlot408CmdCount][26] = {"CP10_ToggleProbes", "CP10_SwitchToPS4Mode", "shadow_giveAllItems", "shadow_giveAllNormalItems", "shadow_giveAllHardItems", "shadow_equipAllItems", "shadow_unequipAllItems", "shadow_setGameCompleted", "shadow_setHardMode", "shadow_setEasyMode", "shadow_jumpToBoss", "boss", "boss_outro", "unlockAllGalleryItems"};
inline constexpr std::size_t kSlot408CmdNameLengths[kSlot408CmdCount] = {0x11, 0x14, 0x13, 0x19, 0x17, 0x14, 0x16, 0x17, 0x12, 0x12, 0x11, 0x4, 0xa, 0x15};
inline constexpr std::int32_t kSlot408CmdDesc[kSlot408CmdCount] = {7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 17, 18, 19};
inline constexpr std::uint64_t kSlot408CmdHandler[kSlot408CmdCount] = {0x5b8ad0, 0x5b8da0, 0x5b9b80, 0x5b9ba0, 0x5b9bc0, 0x5b9be0, 0x5b9c70, 0x5b9db0, 0x5b9de0, 0x5b9e10, 0x5b9e70, 0x5b9ec0, 0x5b9f10, 0x5b9f60};
inline constexpr std::uint64_t kSlot408ZVecDtorRva[kSlot408ZVecCount] = {0x5b5530, 0x5c7c00};
inline constexpr char kSlot408MenuStrings[kSlot408StringCount][113] = {"showParticle", "showLegacyParticle", "showLegacyParticle_Overrides", "r_motionBlurEnable", "backgroundColor", "AO_Mode", "r_aaMethod", "Switches IBL and light probes.  CP10_ToggleProbes(bool) - TRUE=PS4 FALSE=PS3", "Switches between PS4 and PS3 render modes.  CP10_SwitchToPS4Mode(bool) - TRUE=PS4 FALSE=PS3", "Gives the user all items.", "Gives the user all Normal items.", "Gives the user all Hard items.", "Equip all items.", "Unequip all items.", "Marks the game as completed for unlockable purposes. Params: <number of times the player has completed the game>", "Sets the game to hard mode or normal mode. Params: <Type - 0 = Normal Mode, 1 = Hard Mode>", "Sets the game to easy mode or normal mode. Params: <Type - 0 = Normal Mode, 1 = Easy Mode>", "Jump to the selected boss 0-15. Params: <Boss Index (0-15)>", "Jump to the cutscene after a boss", "Automatically unlock all gallery items. Params: none"};
inline constexpr std::size_t kSlot408MenuStringLengths[kSlot408StringCount] = {0xc, 0x12, 0x1c, 0x12, 0xf, 0x7, 0xa, 0x4c, 0x5b, 0x19, 0x20, 0x1e, 0x10, 0x12, 0x70, 0x5a, 0x5a, 0x3b, 0x21, 0x34};

// Sentinel container (0x3941b0 is its atexit destructor, an IDENTITY TAG). The original writes {[0x00] = [0x08] = [0x20] = this + 0x28, [0x10] = this + 0xa8} and nothing else:
// a self-referential header whose sentinel node starts at +0x28 and whose 0x80 B area ends at +0xa8 (the size and the element type are INFERENCE; +0x18 and the area are never written).
struct Slot408Sentinel {
    std::uintptr_t q00;   // = this + 0x28
    std::uintptr_t q08;   // = this + 0x28
    std::uintptr_t q10;   // = this + 0xa8
    std::uintptr_t q18;   // never written
    std::uintptr_t q20;   // = this + 0x28
    std::uint8_t area[0x80];   // never written
};
static_assert(sizeof(Slot408Sentinel) == 0xa8);

// Zero-filled container (0x1c5090 is its atexit destructor, an IDENTITY TAG): the original zeroes [+0x08, +0x18) and [+0x18, +0x20) and nothing else (+0x00 is never written; the object may extend beyond +0x20: INFERENCE).
struct Slot408Zero {
    std::uint64_t q00;   // never written
    std::uint64_t q08;   // = 0
    std::uint64_t q10;   // = 0
    std::uint64_t q18;   // = 0
};
static_assert(sizeof(Slot408Zero) == 0x20);

// Perspective block (0x1580a0 writes all sixteen floats of a row-major matrix at 0x2fda41c; the process-wide cell is the original's .bss, the host owns the storage).
struct Slot408Perspective {
    float m[16];
};
static_assert(sizeof(Slot408Perspective) == 0x40);

struct Slot408Objects {
    std::uint8_t gap00[0x4];
    Slot408Perspective r00;   // 0x2fda41c pre
    std::uint8_t gap01[0x4c];
    NameHolder30 r01;   // 0x2fda4a8 holder
    NameHolder30 r02;   // 0x2fda4d8 holder
    NameHolder30 r03;   // 0x2fda508 holder
    NameHolder30 r04;   // 0x2fda538 holder
    NameHolder30 r05;   // 0x2fda568 holder
    NameHolder30 r06;   // 0x2fda598 holder
    NameHolder30 r07;   // 0x2fda5c8 holder
    std::uint8_t gap08[0x388];
    Slot408Zero r08;   // 0x2fda980 zvec
    Slot408Sentinel r09;   // 0x2fda9a0 sent
    std::uint8_t gap10[0x124f0];
    ConCmdCommand r10;   // 0x2fecf38 cmd
    ConCmdCommand r11;   // 0x2fecfd8 cmd
    std::uint8_t gap12[0x40];
    ConCmdCommand r12;   // 0x2fed0b8 cmd
    ConCmdCommand r13;   // 0x2fed158 cmd
    ConCmdCommand r14;   // 0x2fed1f8 cmd
    ConCmdCommand r15;   // 0x2fed298 cmd
    ConCmdCommand r16;   // 0x2fed338 cmd
    ConCmdCommand r17;   // 0x2fed3d8 cmd
    ConCmdCommand r18;   // 0x2fed478 cmd
    ConCmdCommand r19;   // 0x2fed518 cmd
    ConCmdCommand r20;   // 0x2fed5b8 cmd
    ConCmdCommand r21;   // 0x2fed658 cmd
    ConCmdCommand r22;   // 0x2fed6f8 cmd
    ConCmdCommand r23;   // 0x2fed798 cmd
    std::uint8_t gap24[0x52b8];
    Slot408Zero r24;   // 0x2ff2af0 zvec
    TanfFn perspective_tanf = nullptr;   // HOST INPUT: libc tanf (the original reaches it through the PLT thunk 0xf8f410); nullptr refuses the perspective row. See helper_1580a0_perspective.
};

struct Slot408Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot408Result init_slot408_5c7f50(Slot408Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
