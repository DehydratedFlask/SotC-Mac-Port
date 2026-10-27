// Native recovery of nine startup constructor slots (L3-SLOTS-322-334; original ELF sha256
// d0723afb...5861d3; table word of slot i = u64 at TABLE_START - 8*i):
//
//   slot  RVA       size  contents
//   322   0x849e30  386   bool var `shadow_autoEquipNewWeapons` (value 1) + 0x26db0 menu item "Shadow Items"
//   325   0x839f00  455   bool var `shadow_debugHints` (value 0) + 0x26ea0 toggle "Colossus"/"Hint debug"
//   327   0x836330  455   bool var `colossusCanSeePlayer` (value 1) + 0x26ea0 toggle "Colossus"/"Can See Player"
//   328   0x82e040  519   bool vars `shadow_showBossScrModeStatus`, `shadow_showBossMotKind` (value 0)
//   330   0x810a50  299   bool var `shadow_debugDrawHorsePassageDir` (value 0)
//   331   0x809640  512   bool var `CP10_DebugWormIk` + 0x26c10 group "CP10-Anim"/"Shadow IK" + 0x26db0 item
//   332   0x805440  694   two (bool var, 0x26db0 item) pairs: `shadow_gazeIkEnableSelectiveRecomputeGlobal`
//                         under "Shadow IK" and `cp10_enableGazeIk` under "CP10-Anim" (both value 1)
//   333   0x7fd6d0  550   console commands `shadow_timeAttackSetCompletionCount` and
//                         `shadow_timeAttackSetRetroactiveCompletionCount` (slot-306 command object shape)
//   334   0x7fc530  481   unbounded int var `gMaxSimaltaneousAreaLoadRequests` (value/default 0x10, worker
//                         0x3a980) + 0x26ea0 toggle "Shadow Misc"/"MaxSimaltaneousAreaLoadRequests"
//
// Shared shapes. Slots 322..332 only repeat the shapes already recovered for slot 303 and the slot-306
// menu constructors, so they reuse core/concmd_vars (init_bool_var_static), core/debug_menu_item
// (0x26ea0), core/fmenu_ctors_slot306 (0x26c10, 0x268b0) and the new core/menu_item_26db0 (0x26db0,
// byte-pinned by tools/lift/check_ctor_slot_322.py). All name/caption strings are BORROWED views
// (control == nullptr) over rodata literals, so every 0xf590 on them is an exact inert release.
//
// Provenance of every number below: tools/lift/slotfam_interp.py symbolically walks each straight-line
// body (calls are only recorded) and tools/lift/slotfam_check.py compares the walk with
// tools/lift/slotfam_specs.py and with these constants.
//
// Slot 333 reuses the slot-306 console-command object (core/fconcmd_object_slot306): same vtable
// 0x1594dc0, invoker thunk 0x1990 (cell 0x15fe5e0), indirect register through [0x1594dc0+0x20] = 0x20d60,
// atexit 0x4b0. The two handler addresses are IDENTITY TAGS; their bodies are not lifted here and are
// never executed, so the commands are constructed and registered but running them is NOT implemented
// (a host must supply a handler hook through FConCmdInvokeHooks).
//
// Slot 334 uses the unbounded int-variable worker 0x3a980 (0x142 B). It is the same code as the ranged
// worker 0xb0e060 apart from its rip-relative operands, but it never engages the min/max bounds (bytes
// +0x70/+0x78 are written 0). The callee is native here: init_unbounded_int_var_worker_3a980. The
// functor tables 0x15ce8c0 / 0x1595660 / 0x15944a0 share the slot bodies 0xc720 (copy), 0xc740 (clone),
// 0xc760 (move) and 0xc770 (destroy: bare ret); copy and clone store the table 0x15944a0, which is
// therefore the tag the object ends with. The int class (vptr 0x1594c08) answers 1 / 2 for the two
// virtual calls of 0x26ea0, so the `slot3 == 1 && slot6 == 3` narrowing does not apply.
//
// Nothing here consumes the stored values: no boss debug draw, hint overlay, IK or area-loading
// behaviour is claimed. Original function addresses are identity tags; none is executed.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "ctor_slot_family_items.hpp"
#include "debug_menu_item.hpp"
#include "fconcmd_object_slot306.hpp"

namespace sotc {

// ---- slot 322 / RVA 0x849e30 (386 B) ----------------------------------------
inline constexpr std::uint64_t kSlot322Rva = 0x849e30;
inline constexpr std::size_t kSlot322BodySize = 386;
inline constexpr std::uint64_t kSlot322VarRva = 0x3294d20;   // name view 0x128ede0, description 0x128ee13
inline constexpr std::uint64_t kSlot322VarFnTag = 0x15cec90;   // functor vtable tag (identity only)
inline constexpr std::uint8_t kSlot322VarValue = 1;
inline constexpr std::uint8_t kSlot322VarDefault = 1;
inline constexpr char kSlot322VarName[] = "shadow_autoEquipNewWeapons";
inline constexpr std::uint64_t kSlot322ItemRva = 0x3294de8;   // parent view 0x128edfb; caption = target name
inline constexpr char kSlot322ItemParent[] = "Shadow Items";
inline constexpr std::uint32_t kSlot322ItemFlags = 0x30;
struct Slot322Objects {
    ConCmdBoolVar var{};   // 0x3294d20
    DebugMenuItem item{};   // 0x3294de8
};
struct Slot322Result {
    bool ok = false;
    BoolVarResult var{};
    FamilyItemResult item{};
};
[[nodiscard]] Slot322Result init_slot322_849e30(Slot322Objects& objects, ConCmdRegistry& concmd,
                                       DebugMenuRegistry& menu) noexcept;

// ---- slot 325 / RVA 0x839f00 (455 B) ----------------------------------------
inline constexpr std::uint64_t kSlot325Rva = 0x839f00;
inline constexpr std::size_t kSlot325BodySize = 455;
inline constexpr std::uint64_t kSlot325VarRva = 0x3293288;   // name view 0x128e3f8, description 0x128e47a
inline constexpr std::uint64_t kSlot325VarFnTag = 0x15cec20;   // functor vtable tag (identity only)
inline constexpr std::uint8_t kSlot325VarValue = 0;
inline constexpr std::uint8_t kSlot325VarDefault = 0;
inline constexpr char kSlot325VarName[] = "shadow_debugHints";
inline constexpr std::uint64_t kSlot325ItemRva = 0x3293350;   // label 0x128e40a, text 0x128e413
inline constexpr char kSlot325ItemLabel[] = "Colossus";
inline constexpr char kSlot325ItemText[] = "Hint debug";
inline constexpr std::uint32_t kSlot325ItemFlags = 0x30;
struct Slot325Objects {
    ConCmdBoolVar var{};   // 0x3293288
    DebugMenuItem item{};   // 0x3293350
};
struct Slot325Result {
    bool ok = false;
    BoolVarResult var{};
    FamilyItemResult item{};
};
[[nodiscard]] Slot325Result init_slot325_839f00(Slot325Objects& objects, ConCmdRegistry& concmd,
                                       DebugMenuRegistry& menu) noexcept;

// ---- slot 327 / RVA 0x836330 (455 B) ----------------------------------------
inline constexpr std::uint64_t kSlot327Rva = 0x836330;
inline constexpr std::size_t kSlot327BodySize = 455;
inline constexpr std::uint64_t kSlot327VarRva = 0x3291608;   // name view 0x128e080, description 0x128e0b8
inline constexpr std::uint64_t kSlot327VarFnTag = 0x15cebc0;   // functor vtable tag (identity only)
inline constexpr std::uint8_t kSlot327VarValue = 1;
inline constexpr std::uint8_t kSlot327VarDefault = 1;
inline constexpr char kSlot327VarName[] = "colossusCanSeePlayer";
inline constexpr std::uint64_t kSlot327ItemRva = 0x32916d0;   // label 0x128e095, text 0x128e09e
inline constexpr char kSlot327ItemLabel[] = "Colossus";
inline constexpr char kSlot327ItemText[] = "Can See Player";
inline constexpr std::uint32_t kSlot327ItemFlags = 0x30;
struct Slot327Objects {
    ConCmdBoolVar var{};   // 0x3291608
    DebugMenuItem item{};   // 0x32916d0
};
struct Slot327Result {
    bool ok = false;
    BoolVarResult var{};
    FamilyItemResult item{};
};
[[nodiscard]] Slot327Result init_slot327_836330(Slot327Objects& objects, ConCmdRegistry& concmd,
                                       DebugMenuRegistry& menu) noexcept;

// ---- slot 328 / RVA 0x82e040 (519 B) ----------------------------------------
inline constexpr std::uint64_t kSlot328Rva = 0x82e040;
inline constexpr std::size_t kSlot328BodySize = 519;
inline constexpr std::uint64_t kSlot328VarARva = 0x3291470;   // name view 0x128de60, description 0x128dec9
inline constexpr std::uint64_t kSlot328VarAFnTag = 0x15ceba0;   // functor vtable tag (identity only)
inline constexpr std::uint8_t kSlot328VarAValue = 0;
inline constexpr std::uint8_t kSlot328VarADefault = 0;
inline constexpr char kSlot328VarAName[] = "shadow_showBossScrModeStatus";
inline constexpr std::uint64_t kSlot328VarBRva = 0x3291538;   // name view 0x128de7d, description 0x128dec9
inline constexpr std::uint64_t kSlot328VarBFnTag = 0x15ceba0;   // functor vtable tag (identity only)
inline constexpr std::uint8_t kSlot328VarBValue = 0;
inline constexpr std::uint8_t kSlot328VarBDefault = 0;
inline constexpr char kSlot328VarBName[] = "shadow_showBossMotKind";
struct Slot328Objects {
    ConCmdBoolVar var_a{};   // 0x3291470
    ConCmdBoolVar var_b{};   // 0x3291538
};
struct Slot328Result {
    bool ok = false;
    BoolVarResult var_a{};
    BoolVarResult var_b{};
};
[[nodiscard]] Slot328Result init_slot328_82e040(Slot328Objects& objects, ConCmdRegistry& concmd) noexcept;

// ---- slot 330 / RVA 0x810a50 (299 B) ----------------------------------------
inline constexpr std::uint64_t kSlot330Rva = 0x810a50;
inline constexpr std::size_t kSlot330BodySize = 299;
inline constexpr std::uint64_t kSlot330VarRva = 0x3288cd0;   // name view 0x128cef0, description 0x128cf1b
inline constexpr std::uint64_t kSlot330VarFnTag = 0x15ce920;   // functor vtable tag (identity only)
inline constexpr std::uint8_t kSlot330VarValue = 0;
inline constexpr std::uint8_t kSlot330VarDefault = 0;
inline constexpr char kSlot330VarName[] = "shadow_debugDrawHorsePassageDir";
struct Slot330Objects {
    ConCmdBoolVar var{};   // 0x3288cd0
};
struct Slot330Result {
    bool ok = false;
    BoolVarResult var{};
};
[[nodiscard]] Slot330Result init_slot330_810a50(Slot330Objects& objects, ConCmdRegistry& concmd) noexcept;

// ---- slot 331 / RVA 0x809640 (512 B) ----------------------------------------
inline constexpr std::uint64_t kSlot331Rva = 0x809640;
inline constexpr std::size_t kSlot331BodySize = 512;
inline constexpr std::uint64_t kSlot331VarRva = 0x3288a98;   // name view 0x128cbe0, description 0x128cc10
inline constexpr std::uint64_t kSlot331VarFnTag = 0x15ce900;   // functor vtable tag (identity only)
inline constexpr std::uint8_t kSlot331VarValue = 0;
inline constexpr std::uint8_t kSlot331VarDefault = 0;
inline constexpr char kSlot331VarName[] = "CP10_DebugWormIk";
inline constexpr std::uint64_t kSlot331GroupRva = 0x3288b60;   // parent view 0x128cbf1, caption 0x128cbfb
inline constexpr char kSlot331GroupParent[] = "CP10-Anim";
inline constexpr char kSlot331GroupCaption[] = "Shadow IK";
inline constexpr std::uint32_t kSlot331GroupFlags = 0x30;
inline constexpr std::uint64_t kSlot331ItemRva = 0x3288c08;   // parent view 0x128cbfb; caption = target name
inline constexpr char kSlot331ItemParent[] = "Shadow IK";
inline constexpr std::uint32_t kSlot331ItemFlags = 0x30;
struct Slot331Objects {
    ConCmdBoolVar var{};   // 0x3288a98
    DebugMenuItem group{};   // 0x3288b60
    DebugMenuItem item{};   // 0x3288c08
};
struct Slot331Result {
    bool ok = false;
    BoolVarResult var{};
    FamilyItemResult group{};
    FamilyItemResult item{};
};
[[nodiscard]] Slot331Result init_slot331_809640(Slot331Objects& objects, ConCmdRegistry& concmd,
                                       DebugMenuRegistry& menu) noexcept;

// ---- slot 332 / RVA 0x805440 (694 B) ----------------------------------------
inline constexpr std::uint64_t kSlot332Rva = 0x805440;
inline constexpr std::size_t kSlot332BodySize = 694;
inline constexpr std::uint64_t kSlot332VarARva = 0x32887b0;   // name view 0x128ca40, description 0x128ca9d
inline constexpr std::uint64_t kSlot332VarAFnTag = 0x15ce8e0;   // functor vtable tag (identity only)
inline constexpr std::uint8_t kSlot332VarAValue = 1;
inline constexpr std::uint8_t kSlot332VarADefault = 1;
inline constexpr char kSlot332VarAName[] = "shadow_gazeIkEnableSelectiveRecomputeGlobal";
inline constexpr std::uint64_t kSlot332ItemARva = 0x3288878;   // parent view 0x128ca6c; caption = target name
inline constexpr char kSlot332ItemAParent[] = "Shadow IK";
inline constexpr std::uint32_t kSlot332ItemAFlags = 0x30;
inline constexpr std::uint64_t kSlot332VarBRva = 0x3288920;   // name view 0x128ca76, description 0x128ca9d
inline constexpr std::uint64_t kSlot332VarBFnTag = 0x15ce8e0;   // functor vtable tag (identity only)
inline constexpr std::uint8_t kSlot332VarBValue = 1;
inline constexpr std::uint8_t kSlot332VarBDefault = 1;
inline constexpr char kSlot332VarBName[] = "cp10_enableGazeIk";
inline constexpr std::uint64_t kSlot332ItemBRva = 0x32889e8;   // parent view 0x128ca88; caption = target name
inline constexpr char kSlot332ItemBParent[] = "CP10-Anim";
inline constexpr std::uint32_t kSlot332ItemBFlags = 0x30;
struct Slot332Objects {
    ConCmdBoolVar var_a{};   // 0x32887b0
    DebugMenuItem item_a{};   // 0x3288878
    ConCmdBoolVar var_b{};   // 0x3288920
    DebugMenuItem item_b{};   // 0x32889e8
};
struct Slot332Result {
    bool ok = false;
    BoolVarResult var_a{};
    FamilyItemResult item_a{};
    BoolVarResult var_b{};
    FamilyItemResult item_b{};
};
[[nodiscard]] Slot332Result init_slot332_805440(Slot332Objects& objects, ConCmdRegistry& concmd,
                                       DebugMenuRegistry& menu) noexcept;

// ---- slot 333 / RVA 0x7fd6d0 (550 B) ----------------------------------------
inline constexpr std::uint64_t kSlot333Rva = 0x7fd6d0;
inline constexpr std::size_t kSlot333BodySize = 550;
inline constexpr std::uint64_t kSlot333CmdARva = 0x3288640;        // name 0x128c588, description 0x128c5ac
inline constexpr std::uint64_t kSlot333CmdAHandler = 0x7fd4d0;    // identity tag, never called
inline constexpr char kSlot333CmdAName[] = "shadow_timeAttackSetCompletionCount";
inline constexpr char kSlot333CmdADescription[] =
    "Sets how many colossi have been defeated in time attack mode. +shadow_timeAttackSetCompletionCount <number completed>";
inline constexpr std::uint64_t kSlot333CmdBRva = 0x32886e0;        // name 0x128c622, description 0x128c651
inline constexpr std::uint64_t kSlot333CmdBHandler = 0x7fd590;    // identity tag, never called
inline constexpr char kSlot333CmdBName[] = "shadow_timeAttackSetRetroactiveCompletionCount";
inline constexpr char kSlot333CmdBDescription[] =
    "Sets how many colossi have been defeated in time attack mode and retroactively unlocks the time attack rewards. +shadow_timeAttackSetRetroactiveCompletionCount <number completed>";
static_assert(sizeof kSlot333CmdADescription - 1 == 0x75 && sizeof kSlot333CmdBDescription - 1 == 0xb2,
              "description lengths are the immediates stored into the stack views (0x75 / 0xb2)");
struct Slot333Objects {
    ConCmdCommand cmd_a{};   // 0x3288640
    ConCmdCommand cmd_b{};   // 0x32886e0
};
struct Slot333Result {
    bool ok = false;
    FConCmdBuildResult cmd_a{};
    FConCmdBuildResult cmd_b{};
};
[[nodiscard]] Slot333Result init_slot333_7fd6d0(Slot333Objects& objects, ConCmdRegistry& concmd) noexcept;

// ---- slot 334 / RVA 0x7fc530 (481 B) ----------------------------------------
inline constexpr std::uint64_t kSlot334Rva = 0x7fc530;
inline constexpr std::size_t kSlot334BodySize = 481;
inline constexpr std::uint64_t kSlot334VarRva = 0x32882f8;         // name view 0x128c1c0, description 0x128c2f4
inline constexpr std::uint64_t kSlot334ItemRva = 0x32883d0;        // label 0x128c1e1, text 0x128c1ed
inline constexpr std::uint64_t kSlot334ArgFnTable = 0x15ce8c0;   // functor table of the argument block (+0x40)
inline constexpr std::uint32_t kSlot334CtorFlags = 1;
inline constexpr std::uint32_t kSlot334Value = 0x10;        // dword +0xd0
inline constexpr std::uint32_t kSlot334Default = 0x10;      // dword +0x68
inline constexpr std::uint64_t kSlot334DtorRva = 0x18a60;       // atexit destructor of the variable
inline constexpr std::uint32_t kSlot334ItemFlags = 0x30;
inline constexpr char kSlot334Name[] = "gMaxSimaltaneousAreaLoadRequests";
inline constexpr char kSlot334Label[] = "Shadow Misc";
inline constexpr char kSlot334Text[] = "MaxSimaltaneousAreaLoadRequests";

inline constexpr std::uint64_t kUnboundedIntWorkerRva = 0x3a980;
inline constexpr std::size_t kUnboundedIntWorkerSize = 0x142;
inline constexpr std::uint64_t kUnboundedIntWorkerVptr = 0x15941a0;       // table 0x1594190 + 0x10
inline constexpr std::uint64_t kUnboundedIntFinalVptr = 0x1594c08;        // table 0x1594bf8 + 0x10
inline constexpr std::uint64_t kUnboundedIntSeedFnTable = 0x1595660;      // +0xc0 before the functor install
inline constexpr std::uint64_t kUnboundedIntFnTable = 0x15944a0;          // +0xc0 after the install (copy/clone body)
inline constexpr std::uint64_t kUnboundedIntFnTableB = 0x15ce8c0;         // second lifted table (slot 334's argument)
inline constexpr std::uint32_t kUnboundedIntSlot3Result = 1;              // 0xbf70: mov eax,1; ret
inline constexpr std::uint32_t kUnboundedIntSlot6Result = 2;              // 0xbfc0: mov eax,2; ret

// The three tables whose four slot bodies (0xc720 / 0xc740 / 0xc760 / 0xc770) are lifted.
[[nodiscard]] bool unbounded_int_fn_table_lifted(std::uint64_t table) noexcept;

// 0x3a980 (this=rdi, name=rsi, value cell=this+0xd0, flags=ecx, block=r8, description=r9). Leaves the
// worker vptr in place and engages no bounds. false (nothing written) when the block's functor table
// is not one of the three lifted tables: the original would run an unread clone slot first.
[[nodiscard]] bool init_unbounded_int_var_worker_3a980(ConCmdRangedVar& var, const SharedStr& name,
                                                       std::uint32_t flags, const RangedVarArgs& block,
                                                       const SharedStr& description) noexcept;

// `0x26ea0` virtual-slot results for the int class (1 / 2); known only for vptr 0x1594c08.
[[nodiscard]] DebugMenuTarget unbounded_int_menu_target(const ConCmdRangedVar& var) noexcept;

struct Slot334Objects {
    ConCmdRangedVar var{};      // 0x32882f8
    DebugMenuItem item{};       // 0x32883d0
};
struct Slot334Result {
    bool ok = false;
    bool worker_ok = false;
    bool description_inert = false;
    bool arg_functor_destroyed = false;
    ConCmdRegisterResult registration{};
    bool name_inert = false;
    bool var_atexit_recorded = false;
    FamilyItemResult item{};
};
[[nodiscard]] Slot334Result init_slot334_7fc530(Slot334Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
