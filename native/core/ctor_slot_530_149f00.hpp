// CPU-01-STARTUP-SLOT-530: native model of startup constructor slot 530 (RVA 0x149f00).
//
// Original ELF sha256 d0723afb...5861d3. FDE [0x149f00, 0x14aac1), 3,009 B; constructor-table word
// 0x2dd70e0 - 8 * 530 = 0x2dd6050 holds 0x149f00. Body sha256
// 7b94f30cc533ee5a5ab43bda557baa5c7ab37e250318835682c7619dd8df11d4 (recomputed by the oracle).
//
// The "Resources" debug page: five bool variables, one ranged int variable, one console command, one
// menu group and seven toggles, then a stack-guard epilogue. Shape (all addresses verified from the
// pinned bytes; every callback/vtable/destructor is an IDENTITY TAG, never executed):
//
//   0x149f3f..0x149f46  cell 0x2ee2d00 = 0; __cxa_atexit(0x1442f0, cell)
//   0x149f4b..0x149fbc  two 0x18-byte zero cells at 0x2ee2d08 / 0x2ee2d20 (bytes +4..+7 NOT written)
//   0x149fe3..0x14a07e  bool 0 0x2ee2d38  res_showResourceInfo            (20, 0xfa92d8)
//   0x14a083..0x14a175  bool 1 0x2ee2e00  res_showResourceStats           (21, 0xfa92ed)
//   0x14a17a..0x14a26c  bool 2 0x2ee2ec8  res_showResourceSyncWarnings    (28, 0xfa9303)
//   0x14a271..0x14a363  bool 3 0x2ee2f90  res_printHighLevelResourceLoads (31, 0xfa9320)
//   0x14a368..0x14a45a  bool 4 0x2ee3058  res_printAllFileLoads           (21, 0xfa9340)
//                       each: 0x20af0 flags 1, mid vptr 0x15940b0, value_ref = &value, functor tag
//                       0x15985b8, final vptr 0x1594218, value/default 0, 0x20d60, f590 x2, atexit 0x20b0.
//   0x14a45f..0x14a573  ranged int 0x2ee3120 res_maxLoadingMemoryMiB (23, 0xfa9356) through 0xc580:
//                       pre-store [obj] = 0x1594c08, min 0, max 0x7fffffff, flags 1, argument functor
//                       tag 0x15985d8 (its four slots are 0xc720 / 0xc740 / 0xc760 / 0xc770, the same
//                       bodies as 0x15944a0), empty description; then f590(empty), the bare-ret functor
//                       destroy 0xc770, final vptr 0x1594c08, value/default 0x1e, 0x20d60, f590(name),
//                       atexit 0x18a60.
//   0x14a578..0x14a670  command 0x2ee31f8 res_resetLoadingStats (21, 0xfa936e), description
//                       "Reset Loading Stats" (19, 0xfa9384), flags 2, vptr 0x1594dd0, delegate
//                       {self, 0x1990, 0, 0x144320}, vector +0x80/+0x88/+0x90 zeroed, +0x98 NOT
//                       written, register through [0x1594dc0+0x20] = 0x20d60, f590 x2, atexit 0x4b0.
//   0x14a675..0x14a6f6  group 0x2ee3298 parent "Systems" (7, 0xfa9398), caption "Resources"
//                       (9, 0xfa93a0), flags 0x30, 0x26c10, 0x268b0, f590 x2, atexit 0x272d0.
//   0x14a6fb..0x14aa93  seven toggles 0x2ee3340 + 0xa8 * i, label "Resources" for every one:
//                       0 "Show Loading Info" (17)            -> bool 0
//                       1 "Show Resource Stats" (19)          -> bool 1
//                       2 "Reset Loading Stats" (19)          -> command
//                       3 "Max Loading Memory MiBs" (23)      -> ranged int
//                       4 "Show SyncBuild Warnings" (23)      -> bool 2
//                       5 "Print HighLevel Loads to TTY" (28) -> bool 3
//                       6 "Print All File Loads to TTY" (27)  -> bool 4
//   0x14aa98..0x14aabf  stack-guard compare; 0x14aabb __stack_chk_fail; 0x14aac0 trailing nop.
//
// Object map, consecutive: cell 8 B, zero cells 0x18 + 0x18, five 0xc8 bool vars (0x2ee2d38 ...
// 0x2ee3058), 0xd8 ranged int (0x2ee3120), 0xa0 command (0x2ee31f8), 0xa8 group (0x2ee3298), seven
// 0xa8 items (0x2ee3340 ... 0x2ee3730). End 0x2ee37d8.
//
// The five bool names and the four string literals are the only text this body builds; the command
// name is measured by 0xf2f0 and the rest are borrowed stack views. The destructors 0x1442f0, 0x20b0,
// 0x18a60, 0x4b0, 0x272d0 and the handler 0x144320 are identity tags. Nothing here loads, relocates,
// or executes an original RVA.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"
#include "fconcmd_object_slot306.hpp"
#include "fmenu_ctors_slot306.hpp"
#include "l2_slot_common.hpp"
#include "ranged_int_var_c580.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot530Rva = 0x149f00;
inline constexpr std::size_t kSlot530BodySize = 0xbc1;   // 3,009 B
inline constexpr std::uint64_t kSlot530TableWordRva = 0x2dd6050;
inline constexpr std::uint64_t kSlot530CanaryCellRva = 0x15fe518;
inline constexpr std::uint32_t kSlot530CanaryOffset = 0xc0;
inline constexpr std::uint64_t kSlot530StackChkFailRva = 0xf8f3a0;
inline constexpr std::uint64_t kSlot530CanaryBranchRva = 0x14aaaa;   // jne 0x14aabb
inline constexpr std::uint64_t kSlot530CanaryTargetRva = 0x14aabb;
inline constexpr std::uint32_t kSlot530FrameBytes = 0xe0;

inline constexpr std::uint64_t kSlot530CellRva = 0x2ee2d00;
inline constexpr std::uint64_t kSlot530CellDtorRva = 0x1442f0;      // identity tag
inline constexpr std::uint64_t kSlot530ZeroCell0Rva = 0x2ee2d08;
inline constexpr std::uint64_t kSlot530ZeroCell1Rva = 0x2ee2d20;
inline constexpr std::size_t kSlot530ZeroCellSize = 0x18;
inline constexpr std::uint64_t kSlot530Bool0Rva = 0x2ee2d38;
inline constexpr std::size_t kSlot530BoolCount = 5;
inline constexpr std::size_t kSlot530BoolStride = 0xc8;
inline constexpr std::uint64_t kSlot530RangedRva = 0x2ee3120;
inline constexpr std::uint64_t kSlot530CommandRva = 0x2ee31f8;
inline constexpr std::uint64_t kSlot530GroupRva = 0x2ee3298;
inline constexpr std::uint64_t kSlot530Toggle0Rva = 0x2ee3340;
inline constexpr std::size_t kSlot530ItemStride = 0xa8;
inline constexpr std::size_t kSlot530ItemCount = 7;
inline constexpr std::uint64_t kSlot530EndRva = 0x2ee37d8;

inline constexpr std::uint64_t kSlot530AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot530DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot530ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot530ConCmdBaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot530ConCmdRegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot530MenuToggleCtorRva = 0x26ea0;
inline constexpr std::uint64_t kSlot530MenuGroupCtorRva = 0x26c10;
inline constexpr std::uint64_t kSlot530MenuRegisterRva = 0x268b0;
inline constexpr std::uint64_t kSlot530MenuItemDtorRva = 0x272d0;
inline constexpr std::uint64_t kSlot530BoolDtorRva = 0x20b0;
inline constexpr std::uint64_t kSlot530CommandDtorRva = 0x4b0;
inline constexpr std::uint64_t kSlot530RangedDtorRva = 0x18a60;
inline constexpr std::uint64_t kSlot530RangedCtorRva = 0xc580;
inline constexpr std::uint64_t kSlot530BoolFnVtbl = 0x15985b8;
inline constexpr std::uint64_t kSlot530RangedArgFnVtbl = 0x15985d8;
inline constexpr std::uint32_t kSlot530VarFlags = 1;
inline constexpr std::uint32_t kSlot530CommandFlags = 2;
inline constexpr std::uint32_t kSlot530ItemFlags = 0x30;
inline constexpr std::int32_t kSlot530RangedMin = 0;
inline constexpr std::int32_t kSlot530RangedMax = 0x7fffffff;
inline constexpr std::uint32_t kSlot530RangedValue = 0x1e;
inline constexpr std::uint32_t kSlot530RangedDefault = 0x1e;
inline constexpr std::uint8_t kSlot530BoolValue = 0;
inline constexpr std::uint8_t kSlot530BoolDefault = 0;
inline constexpr std::uint64_t kSlot530CommandHandler = 0x144320;   // identity tag
inline constexpr std::uint64_t kSlot530CommandTail8 = 0x1990;
inline constexpr std::uint64_t kSlot530CommandThunkCellRva = 0x15fe5e0;
inline constexpr std::uint64_t kSlot530CommandVtableRva = 0x1594dc0;
inline constexpr std::uint64_t kSlot530CommandVptrRva = 0x1594dd0;
inline constexpr std::uint32_t kSlot530CommandGate3 = 0;            // 0x22ae0 xor eax,eax; ret
inline constexpr std::size_t kSlot530AtexitCount = 16;
inline constexpr std::size_t kSlot530PendingCount = 7;              // 5 bools + 1 ranged + 1 command
inline constexpr std::size_t kSlot530MenuCount = 8;                 // 1 group + 7 toggles

struct Slot530Cell {
    std::uint64_t word;   // +0x00
};
static_assert(sizeof(Slot530Cell) == 8);

// The two 0x18-byte cells the body only partially writes: dword +0x00 and 16 bytes +0x08; bytes
// +0x04..+0x07 are NOT touched by the original and stay as the host left them.
struct Slot530ZeroCell {
    std::uint32_t word;     // +0x00 = 0
    std::uint8_t gap04[4];  // +0x04 never written
    std::uint64_t zero8;    // +0x08 = 0
    std::uint64_t zero10;   // +0x10 = 0
};
static_assert(sizeof(Slot530ZeroCell) == kSlot530ZeroCellSize &&
              offsetof(Slot530ZeroCell, gap04) == 4 && offsetof(Slot530ZeroCell, zero8) == 8);

struct Slot530BoolSpec {
    const char* name;
    std::uint32_t name_length;
    std::uint64_t name_rva;
    std::uint64_t object_rva;
    std::uint64_t atexit_site_rva;
};

// Body order. Each name is a borrowed view; the shared empty description is 0xfa9cac.
inline constexpr std::array<Slot530BoolSpec, kSlot530BoolCount> kSlot530Bools{{
    {"res_showResourceInfo", 0x14, 0xfa92d8, 0x2ee2d38, 0x14a07e},
    {"res_showResourceStats", 0x15, 0xfa92ed, 0x2ee2e00, 0x14a175},
    {"res_showResourceSyncWarnings", 0x1c, 0xfa9303, 0x2ee2ec8, 0x14a26c},
    {"res_printHighLevelResourceLoads", 0x1f, 0xfa9320, 0x2ee2f90, 0x14a363},
    {"res_printAllFileLoads", 0x15, 0xfa9340, 0x2ee3058, 0x14a45a},
}};

struct Slot530RangedSpec {
    const char* name;
    std::uint32_t name_length;
    std::uint64_t name_rva;
    std::uint64_t object_rva;
    std::uint64_t atexit_site_rva;
};
inline constexpr Slot530RangedSpec kSlot530Ranged{
    "res_maxLoadingMemoryMiB", 0x17, 0xfa9356, 0x2ee3120, 0x14a573};

struct Slot530CommandSpec {
    const char* name;
    const char* description;
    std::uint32_t description_length;
    std::uint64_t name_rva;
    std::uint64_t description_rva;
    std::uint64_t object_rva;
    std::uint64_t atexit_site_rva;
};
inline constexpr Slot530CommandSpec kSlot530Command{
    "res_resetLoadingStats", "Reset Loading Stats", 0x13, 0xfa936e, 0xfa9384, 0x2ee31f8, 0x14a670};

inline constexpr char kSlot530EmptyText[] = "";              // 0xfa9cac
inline constexpr char kSlot530Parent[] = "Systems";          // 0xfa9398, 7
inline constexpr char kSlot530Label[] = "Resources";         // 0xfa93a0, 9
inline constexpr std::uint64_t kSlot530EmptyTextRva = 0xfa9cac;
inline constexpr std::uint64_t kSlot530ParentRva = 0xfa9398;
inline constexpr std::uint64_t kSlot530LabelRva = 0xfa93a0;
static_assert(sizeof kSlot530Parent - 1 == 7 && sizeof kSlot530Label - 1 == 9);

struct Slot530GroupSpec {
    const char* parent;
    std::uint32_t parent_length;
    std::uint64_t parent_rva;
    const char* caption;
    std::uint32_t caption_length;
    std::uint64_t caption_rva;
    std::uint64_t object_rva;
    std::uint64_t atexit_site_rva;
};
inline constexpr Slot530GroupSpec kSlot530Group{
    "Systems", 7, 0xfa9398, "Resources", 9, 0xfa93a0, 0x2ee3298, 0x14a6f6};

enum class Slot530Target : std::uint8_t { bool_var, ranged_int, command };

struct Slot530ItemSpec {
    const char* text;
    std::uint32_t text_length;
    std::uint64_t text_rva;
    Slot530Target target;
    std::uint32_t target_index;   // the bool index for bool_var targets
    std::uint64_t object_rva;
    std::uint64_t atexit_site_rva;
};
inline constexpr std::array<Slot530ItemSpec, kSlot530ItemCount> kSlot530Items{{
    {"Show Loading Info", 0x11, 0xfa93aa, Slot530Target::bool_var, 0, 0x2ee3340, 0x14a77a},
    {"Show Resource Stats", 0x13, 0xfa93bc, Slot530Target::bool_var, 1, 0x2ee33e8, 0x14a802},
    {"Reset Loading Stats", 0x13, 0xfa9384, Slot530Target::command, 0, 0x2ee3490, 0x14a883},
    {"Max Loading Memory MiBs", 0x17, 0xfa93d0, Slot530Target::ranged_int, 0, 0x2ee3538, 0x14a904},
    {"Show SyncBuild Warnings", 0x17, 0xfa93e8, Slot530Target::bool_var, 2, 0x2ee35e0, 0x14a98b},
    {"Print HighLevel Loads to TTY", 0x1c, 0xfa9400, Slot530Target::bool_var, 3, 0x2ee3688, 0x14aa0f},
    {"Print All File Loads to TTY", 0x1b, 0xfa941d, Slot530Target::bool_var, 4, 0x2ee3730, 0x14aa93},
}};

// One `__cxa_atexit` registration of the body, in body order (every site is tool-pinned).
struct Slot530AtexitRow {
    std::uint32_t step;
    std::uint64_t site_rva;
    std::uint64_t dtor_rva;
    std::uint64_t object_rva;
};
inline constexpr std::array<Slot530AtexitRow, kSlot530AtexitCount> kSlot530AtexitRows{{
    {1, 0x149f46, kSlot530CellDtorRva, 0x2ee2d00},
    {2, 0x14a07e, kSlot530BoolDtorRva, 0x2ee2d38},
    {3, 0x14a175, kSlot530BoolDtorRva, 0x2ee2e00},
    {4, 0x14a26c, kSlot530BoolDtorRva, 0x2ee2ec8},
    {5, 0x14a363, kSlot530BoolDtorRva, 0x2ee2f90},
    {6, 0x14a45a, kSlot530BoolDtorRva, 0x2ee3058},
    {7, 0x14a573, kSlot530RangedDtorRva, 0x2ee3120},
    {8, 0x14a670, kSlot530CommandDtorRva, 0x2ee31f8},
    {9, 0x14a6f6, kSlot530MenuItemDtorRva, 0x2ee3298},
    {10, 0x14a77a, kSlot530MenuItemDtorRva, 0x2ee3340},
    {11, 0x14a802, kSlot530MenuItemDtorRva, 0x2ee33e8},
    {12, 0x14a883, kSlot530MenuItemDtorRva, 0x2ee3490},
    {13, 0x14a904, kSlot530MenuItemDtorRva, 0x2ee3538},
    {14, 0x14a98b, kSlot530MenuItemDtorRva, 0x2ee35e0},
    {15, 0x14aa0f, kSlot530MenuItemDtorRva, 0x2ee3688},
    {16, 0x14aa93, kSlot530MenuItemDtorRva, 0x2ee3730},
}};

struct Slot530Objects {
    Slot530Cell cell{};                          // 0x2ee2d00  +0x000
    Slot530ZeroCell zero0{};                     // 0x2ee2d08  +0x008
    Slot530ZeroCell zero1{};                     // 0x2ee2d20  +0x020
    std::array<ConCmdBoolVar, kSlot530BoolCount> bools{};   // 0x2ee2d38 +0x038, stride 0xc8
    ConCmdRangedVar ranged{};                    // 0x2ee3120  +0x420
    ConCmdCommand command{};                     // 0x2ee31f8  +0x4f8
    DebugMenuItem group{};                       // 0x2ee3298  +0x598
    std::array<DebugMenuItem, kSlot530ItemCount> items{};   // 0x2ee3340 +0x640, stride 0xa8
};
static_assert(offsetof(Slot530Objects, cell) == 0x000 && offsetof(Slot530Objects, zero0) == 0x008 &&
              offsetof(Slot530Objects, zero1) == 0x020 &&
              offsetof(Slot530Objects, bools) == 0x038 &&
              offsetof(Slot530Objects, ranged) == 0x420 &&
              offsetof(Slot530Objects, command) == 0x4f8 &&
              offsetof(Slot530Objects, group) == 0x598 &&
              offsetof(Slot530Objects, items) == 0x640);
static_assert(sizeof(Slot530Objects) == 0xad8);
static_assert(kSlot530CellRva + sizeof(Slot530Cell) == kSlot530ZeroCell0Rva &&
              kSlot530ZeroCell0Rva + kSlot530ZeroCellSize == kSlot530ZeroCell1Rva &&
              kSlot530ZeroCell1Rva + kSlot530ZeroCellSize == kSlot530Bool0Rva &&
              kSlot530Bool0Rva + kSlot530BoolCount * kSlot530BoolStride == kSlot530RangedRva &&
              kSlot530RangedRva + sizeof(ConCmdRangedVar) == kSlot530CommandRva &&
              kSlot530CommandRva + sizeof(ConCmdCommand) == kSlot530GroupRva &&
              kSlot530GroupRva + sizeof(DebugMenuItem) == kSlot530Toggle0Rva &&
              kSlot530Toggle0Rva + kSlot530ItemCount * kSlot530ItemStride == kSlot530EndRva,
              "the original .bss addresses follow the pinned strides");

struct Slot530Hooks {
    // __stack_chk_guard source (0x15fe518). Null => the comparison is not claimed; a changed guard
    // fails the fragment at stage `canary`.
    const std::uint64_t* stack_guard = nullptr;
};

enum class Slot530Stage : std::uint8_t {
    none,
    cell_atexit,
    zero_cells,
    bool_build,
    bool_atexit,
    ranged_build,
    ranged_registration,
    ranged_view_release,
    ranged_atexit,
    command_build,
    command_atexit,
    group_ctor,
    group_registration,
    group_view_release,
    group_atexit,
    item_ctor,
    item_registration,
    item_view_release,
    item_atexit,
    canary,
};
[[nodiscard]] const char* slot530_stage_name(Slot530Stage stage) noexcept;

struct Slot530Result {
    bool ok = false;
    Slot530Stage blocked = Slot530Stage::none;
    std::uint32_t blocked_index = 0;
    std::size_t atexit_records = 0;
    std::array<BoolVarResult, kSlot530BoolCount> bools{};
    std::size_t bools_built = 0;
    bool ranged_built = false;
    bool command_built = false;
    std::size_t menu_items = 0;
    ConCmdRegisterResult ranged_registration{};
    FConCmdBuildResult command{};
    std::array<MenuRegisterResult, kSlot530ItemCount> item_registrations{};
    MenuRegisterResult group_registration{};
    bool views_inert = true;
    bool args_functor_inert = false;
    bool canary_checked = false;
    bool canary_matched = false;
};

// The 0x26ea0 gate answer of the three target classes this body uses. Any other vptr tag blocks the
// fragment instead of guessing a range.
[[nodiscard]] DebugMenuTarget slot530_menu_target(const ConCmdBase& base, Slot530Target kind,
                                                  std::uint32_t target_index,
                                                  const Slot530Objects& objects) noexcept;

// Runs original constructor 530 natively. Unconditional in the original, so the first failing step
// stops the slot; every failure names its stage and row index.
[[nodiscard]] Slot530Result init_slot530_149f00(Slot530Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu,
                                                const Slot530Hooks& hooks = {}) noexcept;

}  // namespace sotc
