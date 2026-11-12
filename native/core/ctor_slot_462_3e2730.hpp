// CPU-01-AOT-CFG-SCALE-01-BATCH27 (slot 462): native model of startup constructor slot 462
// (RVA 0x3e2730, 2,580 B; FDE [0x3e2730, 0x3e3144), then 12 nops to the next FDE 0x3e3150).
// Original ELF sha256 d0723afb...5861d3; constructor-table word 0x2dd70e0 - 8 * 462 = 0x2dd6270
// holds 0x3e2730.
//
// ---- body (every RVA below is read from the instructions) ------------------------------------
// Stack-guarded (0x15fe518 at 0x3e2748 -> [rsp+0xe0], fail 0xf8f3a0 at 0x3e313e) debug page for the
// point-of-interest / presentation commands. Body order:
//   1. 0x2ef7420: one zeroed qword; __cxa_atexit(0x3e01e0).
//   2. 0x2ef7428: 0x20-byte borrowed view {0xfe92e0 "Presentation" (12), 0, 0};
//      __cxa_atexit(0xf7b0).
//   3. 0x2ef7448: {0xfe92ed "Clear Active Point of Interest" (30), 0, 0};
//      __cxa_atexit(0xf7b0).
//   4. 0x2ef7468: command "activatePOI" (name built by 0xf2f0 from 0xfe930c, 11), description
//      0xfe9318 (0x33), flags 2, vptr 0x1594dd0, callback tag 0x3e0600; register 0x20d60;
//      f590(description), f590(name); __cxa_atexit(0x4b0). The +0x60 tail copies the 32-byte stack
//      block {self, 0x1990, 0x2512a0, callback} and +0x80..0x97 is zeroed.
//   5. 0x2ef7508: command "clearPOI" (0xfe934c, 8), description 0xfe9355 (0x20), flags 2, callback
//      0x3e0810; same tail and __cxa_atexit(0x4b0).
//   6. 0x2ef75a8: bool variable "showPoiDescription" (0xfe9376, 18), empty description, flags 1.
//      Stores the pre-vptr 0x1594218, runs 0x20af0, stores mid-vptr 0x15940b0, +0x60 = &value,
//      +0x69 dword 0, +0x70..0x8f zero, +0xa0 = 0, +0xb0 = 0x15b18c8, +0xb8 = 0; f590(description);
//      stores the pre-vptr 0x1594218 again at 0x3e2ac1, value +0xc0 = 0 and +0x68 = 0; register;
//      f590(name); __cxa_atexit(0x20b0). The final vptr is 0x1594218, not 0x15940b0 (byte evidence
//      at 0x3e2ac1: `4c 89 35 e0 4a b1 02`).
//   7. 0x2ef7670: 0x30-byte string holder, vptr 0x15976b0, view {0xfe9389 "renderErrorMsgs" (15)},
//      +0x28 = 0; __cxa_atexit(0x8ef00).
//   8. 0x2ef76a0: 0xd0-byte functor command "presentationShowRenderTarget" (0xfe9399, 28), empty
//      description, flags 1, low = 0, high = 0x24, spec 0x2ea8ee8 (zero-fill .bss), view at
//      rsp+0x80 with q18 = 0, vtable 0x15b18e8, tag 0; built by 0x6af60 (concmd_functor_6af60).
//      After it returns: f590(description), a call through the view vtable +0x18 (destroy), f590(name);
//      __cxa_atexit(0x54ba0).
//   9. 0x2ef7770: command "poiClearErrorMessages" (0xfe93b7, 21), description
//      0xfe93cd "Clear Error Messages" (0x14), flags 2, callback 0x3e0840; register through
//      0x1594dc0 +0x20 then f590(description), f590(name); __cxa_atexit(0x4b0).
//  10. 0x2ef7810: 0x26ea0 toggle labelled "Show Description" (0xfe93e2, 16) under the
//      0x2ef7428 parent view, target 0x2ef75a8, flags 0x30; f590(label); __cxa_atexit(0x272d0).
//  11. 0x2ef78b8: 0x26f90 command item under 0x2ef7428, command view 0xfe93cd (20), target
//      0x2dfcbd0, caption view 0xfe93b7 (21), flags 0x30; f590(caption), f590(command);
//      __cxa_atexit(0x272d0).
//  12. 0x2ef7960: 0x26ea0 toggle "Show Render Target" (0xfe93f3, 18) under 0x2ef7428, target
//      0x2ef76a0, flags 0x30; f590(label); __cxa_atexit(0x272d0). The two virtual slots 0x26ea0
//      calls on the functor-command target are 0x1596980 +0x18 = 0x5c6b0 and +0x30 = 0x5c830,
//      both `b8 01 00 00 00 c3` (return 1), so the default range {1.0, 10.0} is kept.
//  13. 0x2ef7a08: 0x26c10 group "Shadow Boss Jump" (0xfe9406, 16) under 0x2ef7428, flags 0x30;
//      f590(caption); __cxa_atexit(0x272d0).
//  14. 0x2ef7ab0: command "poiGroup" (0xfe94ad, 4), description 0xfe94b6 (0x3d), flags 2,
//      callback 0x3e1cb0; f590(description), f590(name); __cxa_atexit(0x4b0).
//  15. 0x2ef7b50: command "poi" (0xfe94f4, 3), description 0xfe94f8 (0x82), flags 2, callback
//      0x3e1e10; f590(description), f590(name); __cxa_atexit(0x4b0).
//  16. canary compare against [rsp+0xe0]; fail 0xf8f3a0.
//
// 15 __cxa_atexit records in body order: 0x3e01e0, 0xf7b0, 0xf7b0, 0x4b0, 0x4b0, 0x20b0, 0x8ef00,
// 0x54ba0, 0x4b0, 0x272d0, 0x272d0, 0x272d0, 0x272d0, 0x4b0, 0x4b0.
// 7 registrations through 0x20d60: the six commands and the bool variable. 4 menu items.
// The page identity (point-of-interest / presentation) is INFERENCE from the strings; every store,
// call and offset is read from the instructions. Destructors, vptr values, callback tags and the two
// RELRO qwords copied from 0x15fe5e0 are identity data and are never executed.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_functor_6af60.hpp"
#include "debug_menu_item.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot462Rva = 0x3e2730;
inline constexpr std::size_t kSlot462Size = 2580;
inline constexpr std::uint64_t kSlot462TableWord = 0x2dd6270;

inline constexpr std::uint64_t kSlot462ZeroCell = 0x2ef7420;
inline constexpr std::uint64_t kSlot462Presentation = 0x2ef7428;
inline constexpr std::uint64_t kSlot462ClearActive = 0x2ef7448;
inline constexpr std::uint64_t kSlot462Activate = 0x2ef7468;
inline constexpr std::uint64_t kSlot462Clear = 0x2ef7508;
inline constexpr std::uint64_t kSlot462ShowDescription = 0x2ef75a8;
inline constexpr std::uint64_t kSlot462ShowDescriptionValue = 0x2ef7668;
inline constexpr std::uint64_t kSlot462RenderErrors = 0x2ef7670;
inline constexpr std::uint64_t kSlot462ShowRenderTarget = 0x2ef76a0;
inline constexpr std::uint64_t kSlot462ClearErrors = 0x2ef7770;
inline constexpr std::uint64_t kSlot462ItemShowDescription = 0x2ef7810;
inline constexpr std::uint64_t kSlot462ItemClearErrors = 0x2ef78b8;
inline constexpr std::uint64_t kSlot462ItemShowTarget = 0x2ef7960;
inline constexpr std::uint64_t kSlot462ItemShadowJump = 0x2ef7a08;
inline constexpr std::uint64_t kSlot462PoiGroup = 0x2ef7ab0;
inline constexpr std::uint64_t kSlot462Poi = 0x2ef7b50;

inline constexpr std::uint64_t kSlot462CommandVptr = 0x1594dd0;        // vtable 0x1594dc0
inline constexpr std::uint64_t kSlot462CommandVtable = 0x1594dc0;
inline constexpr std::uint64_t kSlot462BoolMidVptr = 0x15940b0;        // 0x15940a0 + 0x10
inline constexpr std::uint64_t kSlot462BoolFinalVptr = 0x1594218;      // 0x1594208 + 0x10
inline constexpr std::uint64_t kSlot462StringVptr = 0x15976b0;         // 0x15976a0 + 0x10
inline constexpr std::uint64_t kSlot462FunctorVptr = kConCmdFunctorFinalVptr;
inline constexpr std::uint64_t kSlot462FunctorTagVptr = kConCmdFunctorTagVptr;
inline constexpr std::uint64_t kSlot462FunctorViewVtable = 0x15b18e8;
inline constexpr std::uint64_t kSlot462FunctorSpec = 0x2ea8ee8;        // .bss, word 0
inline constexpr std::uint64_t kSlot462BoolFnTag = 0x15b18c8;
inline constexpr std::uint64_t kSlot462CommandTailCell = 0x15fe5e0;    // RELRO pair copied to +0x68
inline constexpr std::uint64_t kSlot462CommandTail8 = 0x1990;
inline constexpr std::uint64_t kSlot462CommandTail10 = 0x2512a0;
inline constexpr std::uint32_t kSlot462CommandFlags = 2;
inline constexpr std::uint32_t kSlot462VarFlags = 1;
inline constexpr std::uint32_t kSlot462MenuFlags = 0x30;
inline constexpr std::uint32_t kSlot462FunctorLow = 0;
inline constexpr std::uint32_t kSlot462FunctorHigh = 0x24;
inline constexpr std::uint64_t kSlot462TargetGlobal = 0x2dfcbd0;       // opaque 0x26f90 target

inline constexpr std::uint64_t kSlot462ZeroCellDtor = 0x3e01e0;
inline constexpr std::uint64_t kSlot462ViewDtor = 0xf7b0;
inline constexpr std::uint64_t kSlot462CommandDtor = 0x4b0;
inline constexpr std::uint64_t kSlot462BoolDtor = 0x20b0;
inline constexpr std::uint64_t kSlot462StringDtor = 0x8ef00;
inline constexpr std::uint64_t kSlot462FunctorDtor = 0x54ba0;
inline constexpr std::uint64_t kSlot462MenuItemDtor = 0x272d0;
inline constexpr std::uint64_t kSlot462DsoHandle = 0x1604218;
inline constexpr std::uint64_t kSlot462AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot462CanaryCell = 0x15fe518;
inline constexpr std::uint64_t kSlot462StackChkFailRva = 0xf8f3a0;

inline constexpr std::uint64_t kSlot462ActivateCallback = 0x3e0600;
inline constexpr std::uint64_t kSlot462ClearCallback = 0x3e0810;
inline constexpr std::uint64_t kSlot462ClearErrorsCallback = 0x3e0840;
inline constexpr std::uint64_t kSlot462PoiGroupCallback = 0x3e1cb0;
inline constexpr std::uint64_t kSlot462PoiCallback = 0x3e1e10;

inline constexpr const char* kSlot462PresentationText = "Presentation";
inline constexpr std::size_t kSlot462PresentationLength = 0xc;
inline constexpr const char* kSlot462ClearActiveText = "Clear Active Point of Interest";
inline constexpr std::size_t kSlot462ClearActiveLength = 0x1e;
inline constexpr const char* kSlot462ActivateName = "activatePOI";
inline constexpr std::size_t kSlot462ActivateNameLength = 0xb;      // built from the C string 0xfe930c
inline constexpr const char* kSlot462ClearName = "clearPOI";
inline constexpr std::size_t kSlot462ClearNameLength = 8;
inline constexpr const char* kSlot462ShowDescriptionName = "showPoiDescription";
inline constexpr std::size_t kSlot462ShowDescriptionNameLength = 0x12;
inline constexpr const char* kSlot462RenderErrorsText = "renderErrorMsgs";
inline constexpr std::size_t kSlot462RenderErrorsLength = 0xf;
inline constexpr const char* kSlot462ShowTargetName = "presentationShowRenderTarget";
inline constexpr std::size_t kSlot462ShowTargetNameLength = 0x1c;
inline constexpr const char* kSlot462ClearErrorsName = "poiClearErrorMessages";
inline constexpr std::size_t kSlot462ClearErrorsNameLength = 0x15;
inline constexpr const char* kSlot462ClearErrorsText = "Clear Error Messages";
inline constexpr std::size_t kSlot462ClearErrorsTextLength = 0x14;
inline constexpr const char* kSlot462ItemShowDescriptionText = "Show Description";
inline constexpr std::size_t kSlot462ItemShowDescriptionLength = 0x10;
inline constexpr const char* kSlot462ItemShowTargetText = "Show Render Target";
inline constexpr std::size_t kSlot462ItemShowTargetLength = 0x12;
inline constexpr const char* kSlot462ItemShadowJumpText = "Shadow Boss Jump";
inline constexpr std::size_t kSlot462ItemShadowJumpLength = 0x10;
inline constexpr const char* kSlot462PoiGroupName = "poiGroup";
inline constexpr std::size_t kSlot462PoiGroupNameLength = 8;
inline constexpr const char* kSlot462PoiName = "poi";
inline constexpr std::size_t kSlot462PoiNameLength = 3;
inline constexpr const char* kSlot462ActivateDescription =
    "Sets the poi as active. +activatePOI <group> <name>";
inline constexpr std::size_t kSlot462ActivateDescriptionLength = 0x33;
inline constexpr const char* kSlot462ClearDescription = "Sets no poi as active. +clearPOI";
inline constexpr std::size_t kSlot462ClearDescriptionLength = 0x20;
inline constexpr const char* kSlot462PoiGroupDescription =
    "Add a point of interest group. +poiGroup <name> <description>";
inline constexpr std::size_t kSlot462PoiGroupDescriptionLength = 0x3d;
inline constexpr const char* kSlot462PoiDescription =
    "Add a point of interest the debug camera can use to jump to. +poi <group> <name> <posx> <posy> <posz> <opt:size> <opt:description>";
inline constexpr std::size_t kSlot462PoiDescriptionLength = 0x82;

// 0x2ef7468 / 0x2ef7508 / 0x2ef7770 / 0x2ef7ab0 / 0x2ef7b50: ConCmdBase + the 0x40-byte tail.
struct Slot462Command {
    ConCmdBase base;             // +0x00
    std::uint64_t tail0;         // +0x60 self pointer when the callback tag is non-zero
    std::uint64_t tail8;         // +0x68 = 0x1990 (copied from 0x15fe5e0)
    std::uint64_t tail10;        // +0x70 = 0x2512a0
    std::uint64_t tail18;        // +0x78 callback tag
    std::uint64_t zero80[3];     // +0x80..0x97 = 0
    std::uint64_t gap98;         // +0x98 never written
};
static_assert(sizeof(Slot462Command) == 0xa0 && offsetof(Slot462Command, tail0) == 0x60 &&
              offsetof(Slot462Command, tail18) == 0x78 && offsetof(Slot462Command, zero80) == 0x80 &&
              offsetof(Slot462Command, gap98) == 0x98);

// 0x2ef7670: vptr + borrowed 0x20-byte view + one zero qword.
struct Slot462StringHolder {
    std::uint64_t vptr;   // +0x00
    SharedStr view;       // +0x08
    std::uint64_t zero28; // +0x28
};
static_assert(sizeof(Slot462StringHolder) == 0x30 && offsetof(Slot462StringHolder, view) == 0x08 &&
              offsetof(Slot462StringHolder, zero28) == 0x28);

struct Slot462Hooks {
    ConCmdFunctorHooks functor{};
    // The caller-side `call *0x18(view.vtable)` at 0x3e2c61, after f590(description). Its target is
    // runtime state; null skips it and records zero destructions.
    void* view_context = nullptr;
    bool (*destroy_view)(void* context, ConCmdFunctor& object) noexcept = nullptr;
};

struct Slot462Objects {
    std::uint64_t zero_cell{};                 // 0x2ef7420
    SharedStr presentation{};                  // 0x2ef7428
    SharedStr clear_active{};                  // 0x2ef7448
    Slot462Command activate{};                 // 0x2ef7468
    Slot462Command clear{};                    // 0x2ef7508
    ConCmdBoolVar show_description{};          // 0x2ef75a8
    Slot462StringHolder render_errors{};       // 0x2ef7670
    ConCmdFunctor show_render_target{};        // 0x2ef76a0
    Slot462Command clear_errors{};             // 0x2ef7770
    DebugMenuItem item_show_description{};     // 0x2ef7810
    DebugMenuItem item_clear_errors{};         // 0x2ef78b8
    DebugMenuItem item_show_target{};          // 0x2ef7960
    DebugMenuItem item_shadow_jump{};          // 0x2ef7a08
    Slot462Command poi_group{};                // 0x2ef7ab0
    Slot462Command poi{};                      // 0x2ef7b50
};

struct Slot462Result {
    bool ok = false;
    bool canary_matched = false;      // a native function has no cookie; recorded, always true
    std::size_t registered_commands = 0;   // 6 commands + the bool variable
    std::size_t menu_items = 0;
    std::size_t static_views = 0;
    std::size_t atexit_records = 0;
    std::size_t functor_transfers = 0;      // 5 when the host hook is present
    std::size_t view_destructions = 0;      // 1 when the host view-destroy hook is present
};

[[nodiscard]] Slot462Result init_slot462_3e2730(Slot462Objects& objects, ConCmdRegistry& registry,
                                                DebugMenuRegistry& menu,
                                                const Slot462Hooks& hooks = {}) noexcept;

}  // namespace sotc
