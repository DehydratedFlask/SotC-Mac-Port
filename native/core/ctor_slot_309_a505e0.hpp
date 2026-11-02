// CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01 / -NATIVE-LIFT-01: native recovery of original
// constructor-table index 309 / RVA 0xa505e0 (1892 B, [0xa505e0, 0xa50d44)), ELF sha256
// d0723afb...5861d3. Offline analysis only: nothing here loads, relocates or executes an original RVA.
// Byte oracle: tools/lift/check_ctor_slot_309.py.
//
// ============================ characterization (from the pinned bytes) ============================
// Reached by DATA only: the startup table word at 0x2dd70e0 - 309*8 = 0x2dd6738 holds 0xa505e0, and no
// near call/jmp to it exists anywhere in the image (E1056). Body shape from the pinned llvm-objdump
// listing: 354 instructions, ONE conditional branch, 40 direct call sites over 10 distinct callees,
// three `callq *0x18(%rax)` functor-slot-3 sites, and the frame
//     push rbp; mov rsp,rbp; push r15/r14/r13/r12/rbx; and rsp,-0x20; sub rsp,0xe0
//     guard 0xc0(%rsp) <- *[0x15fe518]   (the GOT cell of __stack_chk_guard; R_X86_64_64, symbol 525)
//     ...
//     cmp 0xc0(%rsp),%rax ; jne 0xa50d3e ; lea -0x28(%rbp),%rsp; pop rbx..r15; pop rbp; ret
//   0xa50d3e: callq 0xf8f3a0 (__stack_chk_fail)
// Both imports resolve in local/re/imports.json: 0x15fe518 is symbol 525 / NID f7uOxY9mM1U =
// `__stack_chk_guard` (libkernel OBJECT, sites = 1) and the 0xf8f3a0 thunk is NID Ou3iL1abvng =
// `__stack_chk_fail` (GOT 0x15fffc0). The 12 registrations go through the 0xf8f390 thunk of
// `__cxa_atexit` (GOT 0x15fffb8), all with the same third argument 0x1604218.
// That `jne 0xa50d3e` is the body's only branch and it guards nothing but the canary compare, so the
// body is straight-line: 12 static object constructions, each followed by
// `__cxa_atexit(dtor, object, 0x1604218 __dso_handle)`, plus four debug-menu registrations.
//
// Callee histogram (site -> target, all 40 pinned): 0xf590 x17 (shared release), 0xf8f390 x12
// (__cxa_atexit PLT), 0x26ea0 x4 (debug-menu toggle ctor), 0x20af0 (ConCmd base ctor), 0x20d60
// (register-once), 0xf180 (empty SharedStr), 0xa4f4d0 / 0xa4fec0 / 0xa50280 (the three console-variable
// constructors, one caller each = this body), 0xf8f3a0 (__stack_chk_fail).
//
// The 12 objects, in registration order (the rsi object argument of `__cxa_atexit`):
//    1  0x32f79b8  qword zeroed                                     dtor 0xa46620  (bare cell; slot 311 shares the dtor)
//    2  0x32f79c8  +0x00..+0x17 zeroed                              dtor 0xa47cb0
//    3  0x32f79e8  "force_save_debug_condition" / "Debug the PS4 Save System", 0..5, flags 1,
//                  ctor 0xa4f4d0, vptr 0x15d1ba8, functor tag 0x15d1b78   dtor 0xa47d20
//    4  0x32f7ac0  name object: vptr 0x159e1b8, "+8 name view" of "sSaveManagerShowInfoLevel"
//                  dtor 0x2c84f0
//    5  0x32f7af0  bool "saveSmallMemory", empty description, flags 1, value 0 / default 0,
//                  functor tag 0x15d1c10, built by 0x20af0 + 0x20d60        dtor 0x20b0
//    6  0x32f7bc0  empty SharedStr (0xf180)                               dtor 0xf7b0
//    7  0x32f7be0  toggle "Save System"/"Force Save Debug Condition"      dtor 0x272d0  target 3
//    8  0x32f7c88  toggle "Save System"/"Fake Small Memory Available (10MB)" dtor 0x272d0  target 5
//    9  0x32f7d30  "saveLoadStallState" / "Stall before executing the specified save-load state",
//                  0..18, flags 1, ctor 0xa4fec0, vptr 0x15d1c60, functor tag 0x15d1c30  dtor 0xa48240
//   10  0x32f7e08  toggle "Save System"/"Load Stall State"               dtor 0x272d0  target 9
//   11  0x32f7eb0  "saveStoreStallState" / "Stall before executing the specified save-store state",
//                  0..20, flags 1, ctor 0xa50280, vptr 0x15d1cf8, functor tag 0x15d1cc8  dtor 0xa4ab50
//   12  0x32f7f88  toggle "Save System"/"Save Stall State"               dtor 0x272d0  target 11
// Every object stride is reproduced by Slot309Objects (static_assert below).
//
// The three `callq *0x18(%rax)` sites are the SAME call: rax is the caller's argument-block functor tag
// (0x15d1b78 / 0x15d1c30 / 0x15d1cc8, stored at 0xa0(%rsp) by the slot itself) and rdi is the 0x20 B
// functor copy at 0x80(%rsp); slot 3 of each of those three vtables is a bare `ret`
// (0xa4f4c0 / 0xa4feb0 / 0xa50270 = `c3`). They are functor destroys with no effect, exactly like
// slot 308's 0xc770. The release order around them is `f590(description)`, functor slot 3,
// `f590(name)`, then the atexit record.
//
// The 0x26ea0 toggle gate (`target->vptr[3]() == 1 && target->vptr[6]() == 3`) is answered from the
// pinned vtable slots, not from a hook: the three variable classes answer slot 3 = 1 and slot 6 = 1
// (0xa4d490/0xa4d760, 0xa4e190/0xa4e310, 0xa4e830/0xa4e9b0 are all `b8 01 00 00 00 c3`) and the bool
// class answers {1, 0} (0xb890/0xb930), so the default {1.0, 10.0} range is kept at all four toggles.
//
// ================================ modelled vs hook (G1 PARTIAL) =================================
// MODELLED (native, exact): the prologue/epilogue and canary guard; the two zeroed objects and their
// atexit records; the name object's four stores, its 0xf590 release and its atexit record; the bool
// variable through the shared native `init_bool_var_static` (0x20af0 + 0x20d60 + 0x20b0); the empty
// SharedStr through the native 0xf180 model; the four toggles through the native 0x26ea0 model plus
// the native 0x268b0 registration and 0x272d0 atexit record; all 12 atexit records in body order.
// HOOKS (no default, fail visible): the three console-variable constructors 0xa4f4d0 / 0xa4fec0 /
// 0xa50280. Their interiors are large (functor rebuild through `call *0x8(%rcx)` / `call *(%rax)`,
// a `0xa4f690` sub-initialiser, an internal `0x20d60`, view copies with `lock inc`) and are NOT
// modelled: the hook receives the pinned Slot309VarSpec (name/description/min/max/flags/vptr/functor
// tag/dtor) and owns the object interior. A null hook stops the fragment at that step with ok == false
// BEFORE that object's atexit record, so nothing downstream pretends to be built.
//
// Fail-visible contracts (all reported through `blocked` / `ok == false`, never a silent pass):
//   * a null or failing `var_ctor`;
//   * a functor tag whose slot 3 is not the pinned bare `ret` (`slot309_inert_functor_destroy`);
//   * a `0xf590` release that is not the exact inert no-op of a borrowed view;
//   * a failed 0x20d60 / 0x268b0 registration;
//   * a failed `__cxa_atexit` record;
//   * a `__stack_chk_guard` that changed across the body (the original would call __stack_chk_fail at
//     0xa50d3e). Without a host-supplied guard the comparison is not claimed: `canary_checked` stays
//     false and the result never reports a silent pass.
//
// WHY UNREGISTERED: because of the three hooks, `init_slot309_a505e0` cannot complete on the production
// startup path (StartupState supplies no var_ctor). Registering it would make run_constructors() stop
// at index 309, so the slot stays out of recovered_constructors() and the coverage cliff stays at
// 309 / 0xa505e0 (measured after this module landed: recovered_constructors().size() == 449 of
// kOriginalCtorCount == 573, first_unrecovered_constructor_index() == 309). This module is a
// verified model, not a registration.
//
// NOT modelled: the interiors of the three variable constructors, every destructor body (they are
// identity tags), every consumer of the 12 objects, and the runtime behaviour of the save manager.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"
#include "fmenu_ctors_slot306.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot309Rva = 0xa505e0;
inline constexpr std::size_t kSlot309BodySize = 1892;              // FDE [0xa505e0, 0xa50d44)
inline constexpr std::uint64_t kSlot309EndRva = 0xa50d44;
inline constexpr std::size_t kSlot309InstructionCount = 354;       // pinned listing; not re-derived
inline constexpr std::size_t kSlot309DirectCallSites = 40;
inline constexpr std::size_t kSlot309FunctorSiteCount = 3;
inline constexpr std::uint64_t kSlot309TableWordRva = 0x2dd6738;   // 0x2dd70e0 - 309*8
inline constexpr std::uint64_t kSlot309TableTopRva = 0x2dd70e0;    // index i at TOP - 8*i
inline constexpr std::uint32_t kSlot309TableIndex = 309;

// Frame / guard.
inline constexpr std::uint64_t kSlot309CanaryCellRva = 0x15fe518;  // GOT cell of __stack_chk_guard
inline constexpr std::uint32_t kSlot309CanaryFrameOffset = 0xc0;   // 0xc0(%rsp)
inline constexpr std::uint64_t kSlot309StackChkFailRva = 0xf8f3a0;
inline constexpr std::uint64_t kSlot309BranchRva = 0xa50d2d;       // jne 0xa50d3e (only branch)
inline constexpr std::uint64_t kSlot309BranchTargetRva = 0xa50d3e;
inline constexpr std::uint64_t kSlot309StackChkFailCallRva = 0xa50d3e;  // callq 0xf8f3a0 at the branch target
inline constexpr std::uint32_t kSlot309FrameBytes = 0xe0;          // sub rsp,0xe0 (after and rsp,-0x20)

// Shared callees (identity tags).
inline constexpr std::uint64_t kSlot309AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot309DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot309ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot309EmptyViewCtorRva = 0xf180;
inline constexpr std::uint64_t kSlot309ConCmdBaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot309ConCmdRegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot309MenuToggleCtorRva = 0x26ea0;
inline constexpr std::uint64_t kSlot309MenuRegisterRva = 0x268b0;
inline constexpr std::uint64_t kSlot309MenuRootDtorRva = 0x27280;   // recorded by 0x268b0, not by this body
inline constexpr std::uint64_t kSlot309MenuItemDtorRva = 0x272d0;
inline constexpr std::uint64_t kSlot309VarCtor1Rva = 0xa4f4d0;
inline constexpr std::uint64_t kSlot309VarCtor2Rva = 0xa4fec0;
inline constexpr std::uint64_t kSlot309VarCtor3Rva = 0xa50280;

// Original object addresses (identity only; the native stand-ins live in Slot309Objects).
inline constexpr std::uint64_t kSlot309CellRva = 0x32f79b8;
inline constexpr std::uint64_t kSlot309ClearedRva = 0x32f79c8;
inline constexpr std::uint64_t kSlot309Var1Rva = 0x32f79e8;
inline constexpr std::uint64_t kSlot309NameObjectRva = 0x32f7ac0;
inline constexpr std::uint64_t kSlot309BoolVarRva = 0x32f7af0;
inline constexpr std::uint64_t kSlot309EmptyViewRva = 0x32f7bc0;
inline constexpr std::uint64_t kSlot309Toggle1Rva = 0x32f7be0;
inline constexpr std::uint64_t kSlot309Toggle2Rva = 0x32f7c88;
inline constexpr std::uint64_t kSlot309Var2Rva = 0x32f7d30;
inline constexpr std::uint64_t kSlot309Toggle3Rva = 0x32f7e08;
inline constexpr std::uint64_t kSlot309Var3Rva = 0x32f7eb0;
inline constexpr std::uint64_t kSlot309Toggle4Rva = 0x32f7f88;
inline constexpr std::size_t kSlot309VarBytes = 0xd8;              // stride of each variable object
inline constexpr std::size_t kSlot309BoolVarStride = 0xd0;         // +0xc8..+0xcf never written
inline constexpr std::size_t kSlot309MenuItemStride = 0xa8;        // 0xa4 B item + 4 B pad

// The two zeroed objects' destructors and the name-object / empty-view tags.
inline constexpr std::uint64_t kSlot309CellDtorRva = 0xa46620;
inline constexpr std::uint64_t kSlot309ClearedDtorRva = 0xa47cb0;
inline constexpr std::uint64_t kSlot309NameObjectDtorRva = 0x2c84f0;
inline constexpr std::uint64_t kSlot309NameObjectVptr = 0x159e1b8;   // .data.rel.ro 0x159e1a8 + 0x10
inline constexpr std::uint64_t kSlot309EmptyViewDtorRva = 0xf7b0;    // SharedStr dtor

// The three console-variable classes (vtable address points and the two 0x26ea0 gate slots).
inline constexpr std::uint64_t kSlot309Var1Vptr = 0x15d1ba8;   // 0x15d1b98 + 0x10
inline constexpr std::uint64_t kSlot309Var2Vptr = 0x15d1c60;   // 0x15d1c50 + 0x10
inline constexpr std::uint64_t kSlot309Var3Vptr = 0x15d1cf8;   // 0x15d1ce8 + 0x10
inline constexpr std::uint64_t kSlot309Var1Slot3Rva = 0xa4d490;   // mov eax,1; ret
inline constexpr std::uint64_t kSlot309Var1Slot6Rva = 0xa4d760;   // mov eax,1; ret
inline constexpr std::uint64_t kSlot309Var2Slot3Rva = 0xa4e190;   // mov eax,1; ret
inline constexpr std::uint64_t kSlot309Var2Slot6Rva = 0xa4e310;   // mov eax,1; ret
inline constexpr std::uint64_t kSlot309Var3Slot3Rva = 0xa4e830;   // mov eax,1; ret
inline constexpr std::uint64_t kSlot309Var3Slot6Rva = 0xa4e9b0;   // mov eax,1; ret
inline constexpr std::uint32_t kSlot309VarSlot3Value = 1;
inline constexpr std::uint32_t kSlot309VarSlot6Value = 1;

// The three argument-block functor tags and their (bare `ret`) slot-3 bodies.
inline constexpr std::uint64_t kSlot309Var1FunctorTag = 0x15d1b78;
inline constexpr std::uint64_t kSlot309Var2FunctorTag = 0x15d1c30;
inline constexpr std::uint64_t kSlot309Var3FunctorTag = 0x15d1cc8;
inline constexpr std::uint64_t kSlot309Var1FunctorSlot3Rva = 0xa4f4c0;   // `c3`
inline constexpr std::uint64_t kSlot309Var2FunctorSlot3Rva = 0xa4feb0;   // `c3`
inline constexpr std::uint64_t kSlot309Var3FunctorSlot3Rva = 0xa50270;   // `c3`
inline constexpr std::uint64_t kSlot309FunctorSlotOffset = 0x18;         // `callq *0x18(%rax)`

// The bool variable's argument-block functor tag (0x15d1c10, distinct from the three above; its
// slot-3 body is what `init_bool_var_static` does not need because the original never calls it).
inline constexpr std::uint64_t kSlot309BoolVarFnTag = 0x15d1c10;
inline constexpr std::uint64_t kSlot309BoolVarFnSlot3Rva = 0xc2f0;       // `c3`; this body never calls it

// Strings (identity addresses + pinned lengths; the native views are literals).
inline constexpr std::uint64_t kSlot309Var1NameRva = 0x12c33a0;
inline constexpr std::uint64_t kSlot309Var1DescRva = 0x12c33bb;
inline constexpr std::uint64_t kSlot309NameObjectTextRva = 0x12c33d5;
inline constexpr std::uint64_t kSlot309BoolVarNameRva = 0x12c33ef;
inline constexpr std::uint64_t kSlot309ToggleLabelRva = 0x12c33ff;
inline constexpr std::uint64_t kSlot309Toggle1TextRva = 0x12c340b;
inline constexpr std::uint64_t kSlot309Toggle2TextRva = 0x12c3426;
inline constexpr std::uint64_t kSlot309Var2NameRva = 0x12c34d1;
inline constexpr std::uint64_t kSlot309Var2DescRva = 0x12c34e4;
inline constexpr std::uint64_t kSlot309Toggle3TextRva = 0x12c3519;
inline constexpr std::uint64_t kSlot309Var3NameRva = 0x12c3598;
inline constexpr std::uint64_t kSlot309Var3DescRva = 0x12c35ac;
inline constexpr std::uint64_t kSlot309Toggle4TextRva = 0x12c35e2;
inline constexpr std::uint64_t kSlot309EmptyDescRva = 0x12c3fc0;   // the bool var's empty description

inline constexpr char kSlot309Var1Name[] = "force_save_debug_condition";                   // 0x1a
inline constexpr char kSlot309Var1Desc[] = "Debug the PS4 Save System";                   // 0x19
inline constexpr char kSlot309NameObjectText[] = "sSaveManagerShowInfoLevel";             // 0x19
inline constexpr char kSlot309BoolVarName[] = "saveSmallMemory";                          // 0xf
inline constexpr char kSlot309ToggleLabel[] = "Save System";                              // 0xb
inline constexpr char kSlot309Toggle1Text[] = "Force Save Debug Condition";               // 0x1a
inline constexpr char kSlot309Toggle2Text[] = "Fake Small Memory Available (10MB)";       // 0x22
inline constexpr char kSlot309Var2Name[] = "saveLoadStallState";                          // 0x12
inline constexpr char kSlot309Var2Desc[] = "Stall before executing the specified save-load state";   // 0x34
inline constexpr char kSlot309Toggle3Text[] = "Load Stall State";                         // 0x10
inline constexpr char kSlot309Var3Name[] = "saveStoreStallState";                         // 0x13
inline constexpr char kSlot309Var3Desc[] = "Stall before executing the specified save-store state";  // 0x35
inline constexpr char kSlot309Toggle4Text[] = "Save Stall State";                         // 0x10
static_assert(sizeof kSlot309Var1Name - 1 == 0x1a && sizeof kSlot309Var1Desc - 1 == 0x19);
static_assert(sizeof kSlot309NameObjectText - 1 == 0x19 && sizeof kSlot309BoolVarName - 1 == 0xf);
static_assert(sizeof kSlot309ToggleLabel - 1 == 0xb && sizeof kSlot309Toggle1Text - 1 == 0x1a);
static_assert(sizeof kSlot309Toggle2Text - 1 == 0x22 && sizeof kSlot309Var2Name - 1 == 0x12);
static_assert(sizeof kSlot309Var2Desc - 1 == 0x34 && sizeof kSlot309Toggle3Text - 1 == 0x10);
static_assert(sizeof kSlot309Var3Name - 1 == 0x13 && sizeof kSlot309Var3Desc - 1 == 0x35);
static_assert(sizeof kSlot309Toggle4Text - 1 == 0x10);

inline constexpr std::uint32_t kSlot309VarFlags = 1;      // r9d at every variable constructor
inline constexpr std::uint32_t kSlot309ItemFlags = 0x30;  // r8d at every toggle
inline constexpr std::uint8_t kSlot309BoolValue = 0;      // +0xc0
inline constexpr std::uint8_t kSlot309BoolDefault = 0;    // +0x68

// One `__cxa_atexit` registration of the body, in body order (pinned sites are tool-checked).
struct Slot309AtexitRow {
    std::uint32_t step;          // 1..12
    std::uint64_t site_rva;      // the `callq 0xf8f390`
    std::uint64_t dtor_rva;      // rdi (identity tag, never run)
    std::uint64_t object_rva;    // rsi (identity only)
};

inline constexpr std::size_t kSlot309AtexitRowCount = 12;
inline constexpr std::array<Slot309AtexitRow, kSlot309AtexitRowCount> kSlot309AtexitRows = {{
    {1, 0xa5062d, 0xa46620, 0x32f79b8},
    {2, 0xa5065d, 0xa47cb0, 0x32f79c8},
    {3, 0xa50754, 0xa47d20, 0x32f79e8},
    {4, 0xa507d6, 0x2c84f0, 0x32f7ac0},
    {5, 0xa508e2, 0x20b0, 0x32f7af0},
    {6, 0xa50903, 0xf7b0, 0x32f7bc0},
    {7, 0xa50998, 0x272d0, 0x32f7be0},
    {8, 0xa50a15, 0x272d0, 0x32f7c88},
    {9, 0xa50b13, 0xa48240, 0x32f7d30},
    {10, 0xa50b97, 0x272d0, 0x32f7e08},
    {11, 0xa50c92, 0xa4ab50, 0x32f7eb0},
    {12, 0xa50d16, 0x272d0, 0x32f7f88},
}};

// What the hook of a console-variable constructor receives: the pinned argument identity only.
struct Slot309VarSpec {
    std::uint64_t ctor_rva;        // 0xa4f4d0 / 0xa4fec0 / 0xa50280 (identity only)
    std::uint64_t vptr_tag;        // the address point the ctor installs at object+0
    std::uint64_t functor_tag;     // the caller's argument-block functor vtable (identity)
    std::uint64_t dtor_rva;        // the __cxa_atexit destructor of this object (identity)
    const char* name;              // name view data
    std::uint32_t name_len;
    const char* description;       // second view data
    std::uint32_t description_len;
    std::int32_t initial_value;    // *(rdx) of the original call
    std::int32_t min_value;        // *(rcx)
    std::int32_t max_value;        // *(r8)
    std::uint32_t flags;           // r9d
};

inline constexpr std::array<Slot309VarSpec, 3> kSlot309Vars = {{
    {kSlot309VarCtor1Rva, kSlot309Var1Vptr, kSlot309Var1FunctorTag, 0xa47d20,
     kSlot309Var1Name, 0x1a, kSlot309Var1Desc, 0x19, 0, 0, 5, kSlot309VarFlags},
    {kSlot309VarCtor2Rva, kSlot309Var2Vptr, kSlot309Var2FunctorTag, 0xa48240,
     kSlot309Var2Name, 0x12, kSlot309Var2Desc, 0x34, 0, 0, 0x12, kSlot309VarFlags},
    {kSlot309VarCtor3Rva, kSlot309Var3Vptr, kSlot309Var3FunctorTag, 0xa4ab50,
     kSlot309Var3Name, 0x13, kSlot309Var3Desc, 0x35, 0, 0, 0x14, kSlot309VarFlags},
}};

// One `callq *0x18(%rax)` site: the functor class identity and the body it dispatches to.
struct Slot309FunctorSite {
    std::uint64_t site_rva;        // 0xa5073c / 0xa50af4 / 0xa50c73
    std::uint64_t tag_rva;         // rax
    std::uint64_t slot3_rva;       // [[tag] + 0x18] - a bare `ret` at all three sites
    bool slot3_is_bare_ret;        // pinned true; false blocks the fragment
};

inline constexpr std::array<Slot309FunctorSite, kSlot309FunctorSiteCount> kSlot309FunctorSites = {{
    {0xa5073c, kSlot309Var1FunctorTag, kSlot309Var1FunctorSlot3Rva, true},
    {0xa50af4, kSlot309Var2FunctorTag, kSlot309Var2FunctorSlot3Rva, true},
    {0xa50c73, kSlot309Var3FunctorTag, kSlot309Var3FunctorSlot3Rva, true},
}};

// ---- native stand-ins -------------------------------------------------------------------------
struct Slot309Cell {                       // 0x32f79b8, stride 0x10
    std::uint64_t value = 0;               // +0x00 written by the body
    std::uint64_t gap08 = 0;               // +0x08 never written
};

struct Slot309Cleared {                    // 0x32f79c8, stride 0x20
    std::array<std::uint8_t, 0x18> zero{}; // +0x00..+0x17 written
    std::uint64_t gap18 = 0;               // +0x18 never written
};

struct Slot309VarObject {                  // 0xd8 stand-in for a console variable
    alignas(8) std::array<std::uint8_t, kSlot309VarBytes> bytes{};
};

struct Slot309NameObject {                 // 0x30, the release-family name_object shape
    std::uint64_t vptr = 0;                // +0x00
    SharedStr name{};                      // +0x08  borrowed view {data, len, 0, 0}
    std::uint64_t tail28 = 0;              // +0x28
};

struct Slot309BoolVarBlock {               // 0xd0 stride: 0xc8 for the object, 8 never written
    ConCmdBoolVar var{};                   // +0x00
    std::uint64_t gap_c8 = 0;              // +0xc8..+0xcf never written
};

struct Slot309Objects {
    Slot309Cell cell{};                    // 0x32f79b8  +0x000
    Slot309Cleared cleared{};              // 0x32f79c8  +0x010
    Slot309VarObject var1{};               // 0x32f79e8  +0x030
    Slot309NameObject name_object{};       // 0x32f7ac0  +0x108
    Slot309BoolVarBlock bool_var{};        // 0x32f7af0  +0x138
    SharedStr empty_view{};                // 0x32f7bc0  +0x208
    DebugMenuItem toggle1{};               // 0x32f7be0  +0x228
    DebugMenuItem toggle2{};               // 0x32f7c88  +0x2d0
    Slot309VarObject var2{};               // 0x32f7d30  +0x378
    DebugMenuItem toggle3{};               // 0x32f7e08  +0x450
    Slot309VarObject var3{};               // 0x32f7eb0  +0x4f8
    DebugMenuItem toggle4{};               // 0x32f7f88  +0x5d0
};
static_assert(offsetof(Slot309Objects, cleared) == 0x010 &&
              offsetof(Slot309Objects, var1) == 0x030 &&
              offsetof(Slot309Objects, name_object) == 0x108 &&
              offsetof(Slot309Objects, bool_var) == 0x138 &&
              offsetof(Slot309Objects, empty_view) == 0x208 &&
              offsetof(Slot309Objects, toggle1) == 0x228 &&
              offsetof(Slot309Objects, toggle2) == 0x2d0 &&
              offsetof(Slot309Objects, var2) == 0x378 &&
              offsetof(Slot309Objects, toggle3) == 0x450 &&
              offsetof(Slot309Objects, var3) == 0x4f8 &&
              offsetof(Slot309Objects, toggle4) == 0x5d0,
              "Slot309Objects must reproduce the original .bss strides");
static_assert(sizeof(Slot309Cell) == 0x10 && sizeof(Slot309Cleared) == 0x20 &&
              sizeof(Slot309VarObject) == 0xd8 && sizeof(Slot309NameObject) == 0x30 &&
              sizeof(Slot309BoolVarBlock) == 0xd0 && sizeof(DebugMenuItem) == 0xa8,
              "stand-in sizes");
static_assert(kSlot309CellRva + 0x010 == kSlot309ClearedRva &&
              kSlot309ClearedRva + 0x020 == kSlot309Var1Rva &&
              kSlot309Var1Rva + 0xd8 == kSlot309NameObjectRva &&
              kSlot309NameObjectRva + 0x30 == kSlot309BoolVarRva &&
              kSlot309BoolVarRva + 0xd0 == kSlot309EmptyViewRva &&
              kSlot309EmptyViewRva + 0x20 == kSlot309Toggle1Rva &&
              kSlot309Toggle1Rva + 0xa8 == kSlot309Toggle2Rva &&
              kSlot309Toggle2Rva + 0xa8 == kSlot309Var2Rva &&
              kSlot309Var2Rva + 0xd8 == kSlot309Toggle3Rva &&
              kSlot309Toggle3Rva + 0xa8 == kSlot309Var3Rva &&
              kSlot309Var3Rva + 0xd8 == kSlot309Toggle4Rva,
              "the original object addresses follow the same strides");

enum class Slot309Stage : std::uint8_t {
    none,
    cell,
    cleared,
    var1_ctor,
    var1_functor,
    var1_release,
    var1_atexit,
    name_object,
    name_object_release,
    name_object_atexit,
    bool_var,
    empty_view,
    empty_view_atexit,
    toggle1_ctor,
    toggle1_registration,
    toggle1_release,
    toggle1_atexit,
    toggle2_ctor,
    toggle2_registration,
    toggle2_release,
    toggle2_atexit,
    var2_ctor,
    var2_functor,
    var2_release,
    var2_atexit,
    toggle3_ctor,
    toggle3_registration,
    toggle3_release,
    toggle3_atexit,
    var3_ctor,
    var3_functor,
    var3_release,
    var3_atexit,
    toggle4_ctor,
    toggle4_registration,
    toggle4_release,
    toggle4_atexit,
    canary,
};
[[nodiscard]] const char* slot309_stage_name(Slot309Stage stage) noexcept;

struct Slot309Hooks {
    // 0xa4f4d0 / 0xa4fec0 / 0xa50280. NO default: a missing hook stops the fragment before that
    // object's atexit record. The hook owns the object interior; the slot only re-checks the vptr tag.
    bool (*var_ctor)(Slot309VarObject& object, const Slot309VarSpec& spec) noexcept = nullptr;
    // `callq *0x18(%rax)`. Null => slot309_inert_functor_destroy (exact no-op when slot 3 is a bare ret).
    bool (*destroy_functor)(void* functor, const Slot309FunctorSite& site) noexcept = nullptr;
    // __stack_chk_guard source (0x15fe518). Null => the comparison is not performed and
    // `canary_checked` stays false; a changed guard fails the fragment at stage `canary`.
    const std::uint64_t* stack_guard = nullptr;
};

// The lifted `callq *0x18(%rax)` for the three pinned sites: a bare `ret` has no effect. Returns false
// (visible) when the site's slot 3 is not the pinned bare ret.
[[nodiscard]] bool slot309_inert_functor_destroy(void* functor, const Slot309FunctorSite& site) noexcept;

// The 0x26ea0 gate answer of a variable class, read from the stand-in's own vptr tag (never from a
// hook). An unknown tag reports known == false and makes the toggle ctor fail visibly.
[[nodiscard]] DebugMenuTarget slot309_var_menu_target(const Slot309VarObject& var) noexcept;

struct Slot309Result {
    bool ok = false;
    Slot309Stage blocked = Slot309Stage::none;
    std::uint32_t atexit_records = 0;        // this body's 12 __cxa_atexit registrations
    std::uint32_t functors_destroyed = 0;    // the three `callq *0x18(%rax)` sites
    std::uint32_t vars_built = 0;            // var_ctor hooks that completed (0..3)
    std::uint32_t menu_items = 0;            // toggles registered through 0x268b0 (0..4)
    BoolVarResult bool_var{};                // the shared 0x20af0/0x20d60/0x20b0 shape
    std::array<MenuRegisterResult, 4> toggle_registration{};
    bool name_object_released = false;       // the 0xf590 call on the stack view was an exact no-op
    bool views_inert = true;                 // every f590 call of this body was a no-op
    bool canary_checked = false;             // false when the host supplied no guard
    bool canary_matched = false;
};

// Runs original constructor 309 natively. The original is unconditional, so the first failing step
// stops the slot (nothing after it is claimed); every failure names its stage.
[[nodiscard]] Slot309Result init_slot309_a505e0(Slot309Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu,
                                                const Slot309Hooks& hooks = {}) noexcept;

}  // namespace sotc
