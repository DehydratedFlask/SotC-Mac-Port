// CPU-01-STARTUP-BATCH-7: native model of startup constructor slot 336 (RVA 0x7ec4c0, 1880 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 336. Every fact below is byte-pinned by tools/lift/check_ctor_slot_336.py (and the local enum
// constructor by tools/lift/check_helper_7ec160.py). NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and
// destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order (the object block starts at 0x3264fb0):
//   (1) ENUM-style ranged int variable `attract_mode_type` (0x7ec160 through helper_init_enum_var_site, range 0..3, flags 1, value = default = 0,
//       caller functor tag 0x15ce538, description `Which attract mode to play. Default will play the system selected one.`, object 0x3264fb0,
//       atexit 0x7ea160);
//   (2) bool variable `cp10_dontAllowAttractModeToFinish` (static/literal shape, tag 0x15ce5d0, value = default = 1, object 0x3265088, atexit 0x20b0);
//   (3) RANGED int variable `cp10_ForceAttractModeSpawnPlaceId` (0xc580 through helper_c580_ranged_int, range -1..0x1a, flags 1, value = default
//       = 0, tag 0x15ce5f0, object 0x3265150, atexit 0x18a60);
//   (4) rand() (libc thunk 0xf90130) -> 0x158340(0x3265228, value): u32 at +0 = the value, byte +4 = 0 (a HOST CHOICE, as at slot 540);
//   (5) a 0x26c10 group (item 0x3265230, parent `CP10`, caption `AttractMode`, flags 0x30) and three 0x26ea0 toggles under `AttractMode`:
//       `Type` -> variable (1) (item 0x32652d8), `Allow AttractMode to terminate` -> variable (2) (item 0x3265380),
//       `AttractMode Spawn PlaceId (-1 = Random)` -> variable (3) (item 0x3265428); each flags 0x30, atexit 0x272d0;
//   (6) a 0x30 byte name holder (object 0x32654d0: class pointer 0x15976a0 + 0x10, borrowed view `shadowDemoMode`, tail 0; atexit 0x8ef00);
//   (7) UNBOUNDED float variable `shadowDemoMode_minLoadTime` (0x6aa00 through helper_6aa00_unbounded_float, flags 1, value = default = 5.0f,
//       tag 0x15ce610, object 0x3265500, atexit 0x20f0);
//   (8) a 9 byte cell (0x32655f0): handle = 0x14d880(), flag = 0, 0x14ccb0(handle) (retain), atexit 0x7ea1a0 (a release-if-flag + 0x14ccc0 body,
//       an IDENTITY TAG here).
//
// Menu targets: variable (1) has the final class 0x15ce568 whose slots 3 / 6 are both `mov eax,1; ret`, the bool class answers (1, 0) and the
// int class (1, 2): none is (1, 3), so all three toggles keep the {1.0, 10.0} default range.
//
// HOOKS (documented, fail closed): rand() is the host generator unless supplied; the cell needs the lifted getter / retain (production wiring is
// default_slot336_hooks()); with either cleared the fragment reports ok == false at the cell and leaves it untouched. The __stack_chk_guard
// compare is a hook. NOT established: what reads these variables at run time (the attract-mode reading is INFERENCE from the strings only).
#pragma once

#include <cstddef>
#include <cstdint>

#include "ctor_release_family_slots.hpp"
#include "ctor_slot_family_items.hpp"
#include "helper_7ec160_enum_var.hpp"
#include "helper_int_var_sites.hpp"
#include "lane_m6_shapes.hpp"
#include "registry_object_14c9a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot336Rva = 0x7ec4c0;
inline constexpr std::size_t kSlot336Size = 1880;
inline constexpr std::uint64_t kSlot336EnumFnTag = 0x15ce538;
inline constexpr std::uint64_t kSlot336BoolFnTag = 0x15ce5d0;
inline constexpr std::uint64_t kSlot336IntFnTag = 0x15ce5f0;
inline constexpr std::uint64_t kSlot336FloatFnTag = 0x15ce610;
inline constexpr std::uint64_t kSlot336HolderVptr = 0x15976b0;      // 0x15976a0 + 0x10
inline constexpr std::uint64_t kSlot336HolderDtorRva = 0x8ef00;
inline constexpr std::uint64_t kSlot336CellDtorRva = 0x7ea1a0;
inline constexpr char kSlot336Var0Name[] = "attract_mode_type";
inline constexpr std::size_t kSlot336Var0NameLength = 0x11;
inline constexpr char kSlot336Var0Description[] = "Which attract mode to play. Default will play the system selected one.";
inline constexpr std::size_t kSlot336Var0DescriptionLength = 0x46;
inline constexpr std::int32_t kSlot336Var0Min = 0;
inline constexpr std::int32_t kSlot336Var0Max = 3;
inline constexpr char kSlot336Var1Name[] = "cp10_dontAllowAttractModeToFinish";
inline constexpr std::size_t kSlot336Var1NameLength = 0x21;
inline constexpr char kSlot336Var2Name[] = "cp10_ForceAttractModeSpawnPlaceId";
inline constexpr std::size_t kSlot336Var2NameLength = 0x21;
inline constexpr std::int32_t kSlot336Var2Min = -1;
inline constexpr std::int32_t kSlot336Var2Max = 0x1a;
inline constexpr char kSlot336GroupParent[] = "CP10";
inline constexpr std::size_t kSlot336GroupParentLength = 4;
inline constexpr char kSlot336GroupCaption[] = "AttractMode";
inline constexpr std::size_t kSlot336GroupCaptionLength = 0xb;
inline constexpr char kSlot336Item0Text[] = "Type";
inline constexpr std::size_t kSlot336Item0TextLength = 4;
inline constexpr char kSlot336Item1Text[] = "Allow AttractMode to terminate";
inline constexpr std::size_t kSlot336Item1TextLength = 0x1e;
inline constexpr char kSlot336Item2Text[] = "AttractMode Spawn PlaceId (-1 = Random)";
inline constexpr std::size_t kSlot336Item2TextLength = 0x27;
inline constexpr std::uint32_t kSlot336ItemFlags = 0x30;
inline constexpr char kSlot336HolderName[] = "shadowDemoMode";
inline constexpr std::size_t kSlot336HolderNameLength = 0xe;
inline constexpr char kSlot336Var3Name[] = "shadowDemoMode_minLoadTime";
inline constexpr std::size_t kSlot336Var3NameLength = 0x1a;
inline constexpr std::uint32_t kSlot336Var3ValueBits = 0x40a00000;   // 5.0f
inline constexpr std::uint64_t kSlot336Var0Rva = 0x3264fb0;
inline constexpr std::uint64_t kSlot336Var1Rva = 0x3265088;
inline constexpr std::uint64_t kSlot336Var2Rva = 0x3265150;
inline constexpr std::uint64_t kSlot336RandCellRva = 0x3265228;
inline constexpr std::uint64_t kSlot336GroupRva = 0x3265230;
inline constexpr std::uint64_t kSlot336Item0Rva = 0x32652d8;
inline constexpr std::uint64_t kSlot336Item1Rva = 0x3265380;
inline constexpr std::uint64_t kSlot336Item2Rva = 0x3265428;
inline constexpr std::uint64_t kSlot336HolderRva = 0x32654d0;
inline constexpr std::uint64_t kSlot336Var3Rva = 0x3265500;
inline constexpr std::uint64_t kSlot336CellRva = 0x32655f0;

struct Slot336Cell {
    Object14c9a0* handle = nullptr;   // +0x00 = 0x14d880()
    std::uint8_t flag = 0;            // +0x08 = 0
    std::uint8_t gap09[7]{};          // never written
};
static_assert(sizeof(Slot336Cell) == 0x10);

struct Slot336Objects {
    ConCmdRangedVar var0;               // 0x3264fb0
    ConCmdBoolVar var1;                 // 0x3265088
    ConCmdRangedVar var2;               // 0x3265150
    std::uint8_t rand_cell[8];          // 0x3265228: u32 value, byte +4 cleared
    DebugMenuItem group;                // 0x3265230
    DebugMenuItem item0;                // 0x32652d8
    DebugMenuItem item1;                // 0x3265380
    DebugMenuItem item2;                // 0x3265428
    ReleaseFamilyNameObject holder;     // 0x32654d0
    ConCmdRangedVar var3;               // 0x3265500
    std::uint8_t gap628[0x18];          // never written
    Slot336Cell cell;                   // 0x32655f0
};

using Slot336RandSource = int (*)() noexcept;

struct Slot336Hooks {
    Slot336RandSource rand_source = nullptr;                    // null => std::rand (host choice)
    Object14c9a0* (*acquire_14d880)() = nullptr;                // null => the cell is untouched (ok == false)
    void (*retain_14ccb0)(void* handle) = nullptr;              // null => the cell is untouched (ok == false)
};

// Production wiring: std::rand, getter_14d880, add_ref_14ccb0.
[[nodiscard]] Slot336Hooks default_slot336_hooks() noexcept;

struct Slot336Result {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;   // the first wrapper refusal, if any
    HelperIntSiteResult var0{};
    BoolVarResult var1{};
    HelperIntSiteResult var2{};
    std::uint32_t rand_value = 0;
    FamilyItemResult group{};
    MenuSiteResult item0{};
    MenuSiteResult item1{};
    MenuSiteResult item2{};
    bool holder_ok = false;             // holder built, f590 inert, (0x8ef00, holder) recorded
    HelperFloatSiteResult var3{};
    bool cell_acquired = false;
    bool cell_retained = false;
    bool cell_atexit_recorded = false;  // (0x7ea1a0, cell) recorded
};

[[nodiscard]] Slot336Result init_slot336_7ec4c0(Slot336Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                                const Slot336Hooks& hooks) noexcept;

}  // namespace sotc
