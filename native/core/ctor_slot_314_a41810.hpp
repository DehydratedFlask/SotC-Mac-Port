// CPU-01-STARTUP-SLOT-314-CHARACTERIZATION-01 / -NATIVE-LIFT-01: native recovery of original
// constructor-table index 314 / RVA 0xa41810 (4,329 B FDE [0xa41810, 0xa428f9); 4,328 B reachable over
// 779 instructions, the byte at 0xa428f8 being the trailing nop), ELF sha256 d0723afb...5861d3.
// Offline analysis only: nothing here loads, relocates or executes an original RVA.
// Byte oracle: tools/lift/check_ctor_slot_314.py.
//
// ============================ characterization (from the pinned bytes) ============================
// Reached by DATA only: the startup table word at 0x2dd70e0 - 314*8 = 0x2dd6710 holds 0xa41810.
// Body shape from the pinned listing: 779 reachable instructions in ONE block (the only branch is the
// epilogue `jne 0xa428f3` at 0xa428e2), 84 direct call sites over 9 targets, 8 `callq *0x18(%rax)`
// functor-slot-3 sites, 196 rip-relative operands (63 of them stores / 56 distinct addresses), zero
// outside-image references, and the frame
//     push rbp; mov rsp,rbp; push r15/r14/r13/r12/rbx; and rsp,-0x20; sub rsp,0xe0
//     guard <- *[0x15fe518]  (the GOT cell holds the runtime address of __stack_chk_guard)
//     ...
//     cmp [rsp+0xc0],%rax ; jne 0xa428f3 ; lea -0x28(%rbp),%rsp; pops; ret
//   0xa428f3: callq 0xf8f3a0 (__stack_chk_fail); the FDE ends at 0xa428f9 (the byte at
//   0xa428f8 is the trailing nop).
//
// This is the **physics debug block**: one thread-named menu page ("PhysXToRenderThread"), ten console
// variables and ten debug-menu toggles, all under the menu caption "Physics". The strings are the
// evidence: `physics_EnableUpdate` (0x12c2774, 0x14) / `physics_LockSimulationAt30Hz` (0x12c27a7,
// 0x1d) / `physics_clothMaxDistanceScalar` (0x12c27dc, 0x1e) / `physics_clothDampening` (0x12c2811,
// 0x16) / `physics_clothWindScalar` (0x12c2838, 0x17) / `physX_connectToPhysicsDebugger` (0x12c2862,
// 0x1e) / `physX_enableDebugDraw` (0x12c289d, 0x15) / `physX_drawClothCollision` (0x12c28c5, 0x18) /
// `physX_drawClothMaxDistance` (0x12c28f3, 0x1a) / `physX_drawClothMaxDistanceInward` (0x12c2926,
// 0x20), the page/caption names `PhysXToRenderThread` (0x12c2760, 0x13) and `Physics` (0x12c2789, 7),
// the ten captions 0x12c2791..0x12c2947, and the empty description 0x12c2c83.
//
// Callee histogram (every one of the 84 sites is pinned by the checker):
//   0xf590    x35 (shared release of a borrowed stack view; every site is an exact no-op)
//   0xf8f390  x22 (__cxa_atexit PLT, third argument 0x1604218 = __dso_handle, one per object)
//   0x26ea0   x10 (menu TOGGLE ctor; its tail call to 0x268b0 registers the item)
//   0x20d60   x5  (register-once guard of the five in-body variables)
//   0xa41570  x5  (bool variable WITH an on-change functor; calls 0x20d60 itself)
//   0xc300    x3  (bounded float variable ctor; already native as core/concmd_vars)
//   0x20af0   x2  (ConCmdBase ctor of the two inline bool variables)
//   0x475c0   x1  (the 87 B page BASE ctor: vptr 0x1595900, name, link head at +0x38, +0x70 = 0;
//                    the body then stores the derived vptr 0x1594ca0)
//   0xf8f3a0  x1  (__stack_chk_fail, the branch target only)
//
// The 8 indirect sites are `callq *0x18(%rax)` where %rax is the row's *argument-functor tag*: four
// dispatch through 0x15d1770 (the float argument class) and four through 0x15d1750 (the bool
// argument class). Both tables are `.data.rel.ro` RELATIVE slices whose slot 3 is a bare `ret`
// (0xc4f0 / 0xc2f0), so every one of the eight is an inert functor destroy of a stack temporary.
// The five 0xa41570 calls add eleven more collapsed dispatches each (clone/copy/copy/copy/destroy,
// twice, plus the destroy of the leftover temporary, all through the same two tags), for 63 collapsed
// dispatches in the whole slot.
//
// The 22 __cxa_atexit records, in body order (site, dtor, object RVA; object offsets are + each
// field's offset inside Slot314Objects), are kSlot314AtexitRows below: page 0x32f5af0 (0x1ce70),
// then for each of the ten pairs the variable (0x20b0 bool / 0x20f0 float) immediately followed by
// its toggle (0x272d0), with the extra 0xb0 B zero cell 0x32f6538 (0xa3d9e0) between the fifth pair
// and the sixth variable.
//
// ================================ object map (verified strides) ================================
// Every original address is consecutive; the aggregate below reproduces the strides exactly:
//   0x32f5af0  page        0x2e8   (ends exactly where the first variable starts)
//   0x32f5dd8  var 0       0xc8    0x32f5ea0  item 0  0xa8
//   0x32f5f48  var 1       0xc8    0x32f6010  item 1  0xa8
//   0x32f60b8  var 2       0xd8    0x32f6190  item 2  0xa8
//   0x32f6238  var 3       0xd8    0x32f6310  item 3  0xa8
//   0x32f63b8  var 4       0xd8    0x32f6490  item 4  0xa8
//   0x32f6538  cell        0xb0    (only qword +0x00 is written: 0)
//   0x32f65e8  var 5       0xc8    0x32f66b0  item 5  0xa8   ... pairs 9 end at 0x32f6d18,
// which is exactly the first object of constructor index 313 (core/ctor_slot_313_a447a0). The
// adjacency is a pinned cross-check, not an inference.
//
// The page object's post-constructor block is a raw store pattern and is modelled as such (field
// meanings are NOT claimed): +0x80 = 0, +0x88 = 0, +0x90..+0xaf = 0, then twelve 0x30 B
// self-referential nodes at +0xb0 + 0x30*k whose {next, prev} both point at the node itself, each
// followed by 16 zero bytes at node+0x20 (all but the last), and a dword 1 at +0x2e0 written by the
// first variable's step.
//
// ================================ modelled vs hook (G2) =================================
// MODELLED (native, exact): the prologue/epilogue and canary guard; the page through
// `init_slot314_page_475c0` (the 87 B page ctor) and `init_slot314_page_stores` (the 0xb0..0x2df
// pattern); the page's atexit record; the two inline bool variables through the existing
// `init_bool_var_static` model (they use the same 0x1594218 class with functor tag 0x15d1750); the
// three bounded float variables through the existing `init_ranged_float_var_c300` model plus their
// description release, inert functor destroy, final vptr, value/default pair, 0x20d60 registration
// and atexit record; the five callback bool variables through `init_slot314_bool_var_callback` (net
// state of 0x475c0-free 0xa41570: two argument-functor copies, the ten dispatches collapsed, the
// final functor tag 0x1594280, the object's own value cell, registration and atexit); the ten
// toggles through the existing 0x26ea0 + 0x268b0 models with their gate answers; all 22 atexit
// records in body order.
// HOOK (optional override, no default in the struct): `Slot314Hooks::page_ctor` (0x475c0) and
// `Slot314Hooks::bool_callback_ctor` (0xa41570). `default_slot314_hooks()` installs both native
// models; a null or failing hook stops the fragment at that object's stage BEFORE its atexit record.
//
// Fail-visible contracts (all reported through `blocked` / `ok == false`, never a silent pass):
//   * a null or failing `page_ctor` / `bool_callback_ctor`;
//   * a `0xf590` release that is not the exact inert no-op of a borrowed view;
//   * a failed 0x20d60 / 0x268b0 registration;
//   * a failed `__cxa_atexit` record;
//   * a `__stack_chk_guard` that changed across the body (the original would call __stack_chk_fail at
//     0xa428f3). Without a host-supplied guard the comparison is not claimed: `canary_checked` stays
//     false and the result never reports a silent pass.
//   * an unknown variable shape, or a 0x26ea0 target whose vptr tag is not one of the two pinned
//     classes (the toggle's range branch would then be unpinned).
//
// NOT modelled: every destructor body (0x1ce70 page, 0x20b0 bool, 0x20f0 float, 0x272d0 item,
// 0xa3d9e0 cell) and the five on-change callback tags the 0xa41570 sites carry as DATA
// (0xa40160 / 0xa40230 / 0xa40300 / 0xa403c0 / 0xa40480 at q[3], 0x274710 at q[1]). They are pinned
// identity values, recorded and never executed. Nothing consumes the page or the ten variables yet,
// so no physics/cloth/PhysX behaviour is claimed.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"
#include "fmenu_ctors_slot306.hpp"
#include "l2_slot_common.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot314Rva = 0xa41810;
inline constexpr std::size_t kSlot314BodySize = 4329;          // FDE [0xa41810, 0xa428f9)
inline constexpr std::uint64_t kSlot314EndRva = 0xa428f9;
inline constexpr std::size_t kSlot314ReachableBytes = 4328;    // the last reached instruction ends here
inline constexpr std::size_t kSlot314InstructionCount = 779;
inline constexpr std::size_t kSlot314DirectCallSites = 84;
inline constexpr std::size_t kSlot314IndirectCallSites = 8;
inline constexpr std::size_t kSlot314RipOperands = 196;
inline constexpr std::size_t kSlot314RipTargets = 119;
inline constexpr std::size_t kSlot314StoreSites = 63;
inline constexpr std::size_t kSlot314StoreTargets = 56;
inline constexpr std::uint64_t kSlot314TableTopRva = 0x2dd70e0;    // index i at TOP - 8*i
inline constexpr std::uint64_t kSlot314TableWordRva = 0x2dd6710;   // 0x2dd70e0 - 314*8
inline constexpr std::uint32_t kSlot314TableIndex = 314;
inline constexpr std::uint32_t kSlot314NextUnrecoveredIndex = 335;     // 335 / 0x7edf10
inline constexpr std::uint64_t kSlot314NextUnrecoveredRva = 0x7edf10;

// Frame / guard.
inline constexpr std::uint64_t kSlot314CanaryCellRva = 0x15fe518;   // GOT cell holding &__stack_chk_guard
inline constexpr std::uint32_t kSlot314CanaryFrameOffset = 0xc0;    // [rsp+0xc0]
inline constexpr std::uint64_t kSlot314StackChkFailRva = 0xf8f3a0;
inline constexpr std::uint64_t kSlot314BranchRva = 0xa428e2;        // jne 0xa428f3 (the only branch)
inline constexpr std::uint64_t kSlot314BranchTargetRva = 0xa428f3;
inline constexpr std::uint32_t kSlot314FrameBytes = 0xe0;           // sub rsp,0xe0 (after and rsp,-0x20)

// Shared callees.
inline constexpr std::uint64_t kSlot314AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot314DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot314ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot314ConCmdBaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot314ConCmdRegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot314MenuToggleCtorRva = 0x26ea0;
inline constexpr std::uint64_t kSlot314MenuRegisterRva = 0x268b0;
inline constexpr std::uint64_t kSlot314FloatVarCtorRva = 0xc300;
inline constexpr std::uint64_t kSlot314BoolCallbackCtorRva = 0xa41570;
inline constexpr std::uint64_t kSlot314PageCtorRva = 0x475c0;
inline constexpr std::size_t kSlot314PageCtorSize = 87;
inline constexpr std::size_t kSlot314BoolCallbackCtorSize = 669;   // FDE of 0xa41570

// Identity tags (original vtable address points; never dispatched through here).
// 0x475c0 installs the BASE tag (0x15958f0 + 0x10); the body's first store installs the derived one.
inline constexpr std::uint64_t kSlot314PageCtorVptr = 0x1595900;    // lea 0x15958f0 + 0x10
inline constexpr std::uint64_t kSlot314PageVptr = 0x1594ca0;        // lea 0x1594c90 + 0x10 (body)
inline constexpr std::uint64_t kSlot314BoolMidVptr = 0x15940b0;     // 0x15940a0 + 0x10
inline constexpr std::uint64_t kSlot314BoolFinalVptr = 0x1594218;   // 0x1594208 + 0x10
inline constexpr std::uint64_t kSlot314FloatMidVptr = 0x1594128;    // 0x1594118 + 0x10
inline constexpr std::uint64_t kSlot314FloatFinalVptr = 0x15942b0;  // 0x15942a0 + 0x10
inline constexpr std::uint64_t kSlot314FloatFnVtbl = 0x1594318;     // what 0xc4a0/0xc4c0 store
inline constexpr std::uint64_t kSlot314BoolCallbackFnVtbl = 0x1594280;  // what 0xc2a0/0xc2c0 store
// The two argument-functor classes the body dispatches through (`callq *0x18(%rax)`).
inline constexpr std::uint64_t kSlot314BoolArgFnVtbl = 0x15d1750;   // slots 0xc2a0/0xc2c0/0xc2e0/0xc2f0
inline constexpr std::uint64_t kSlot314FloatArgFnVtbl = 0x15d1770;  // slots 0xc4a0/0xc4c0/0xc4e0/0xc4f0
inline constexpr std::uint64_t kSlot314BoolArgDestroyRva = 0xc2f0;  // bare `ret`
inline constexpr std::uint64_t kSlot314FloatArgDestroyRva = 0xc4f0; // bare `ret`
// 0x26ea0 gate answers, read from each target's final vptr tag (never a hook): slot 3 / slot 6.
inline constexpr std::uint32_t kSlot314BoolGate3 = 1;    // 0xb890 `mov eax,1; ret`
inline constexpr std::uint32_t kSlot314BoolGate6 = 0;    // 0xb930 `xor eax,eax; ret`
inline constexpr std::uint32_t kSlot314FloatGate3 = 1;   // 0xbba0 `mov eax,1; ret`
inline constexpr std::uint32_t kSlot314FloatGate6 = 3;   // 0xbbf0 `mov eax,3; ret`

// Destructors (identity tags, recorded by __cxa_atexit, never run).
inline constexpr std::uint64_t kSlot314PageDtorRva = 0x1ce70;
inline constexpr std::uint64_t kSlot314BoolVarDtorRva = 0x20b0;
inline constexpr std::uint64_t kSlot314FloatVarDtorRva = 0x20f0;
inline constexpr std::uint64_t kSlot314CellDtorRva = 0xa3d9e0;

// Original object addresses. The aggregate below reproduces every stride; these are the identity
// values the checker pins.
inline constexpr std::uint64_t kSlot314PageRva = 0x32f5af0;
inline constexpr std::size_t kSlot314PageSize = 0x2e8;
inline constexpr std::uint64_t kSlot314CellRva = 0x32f6538;
inline constexpr std::size_t kSlot314CellSize = 0xb0;
inline constexpr std::uint64_t kSlot314LastObjectRva = 0x32f6c70;   // item 9; + 0xa8 = 0x32f6d18
inline constexpr std::uint64_t kSlot314NextCtorFirstObjectRva = 0x32f6d18;  // slot 313's cell

// Strings of the block: identity address + pinned length (the native views are literals).
inline constexpr char kSlot314PageName[] = "PhysXToRenderThread";      // 0x12c2760, 0x13
inline constexpr char kSlot314MenuParent[] = "Physics";                // 0x12c2789, 7
inline constexpr char kSlot314EmptyText[] = "";                        // 0x12c2c83
inline constexpr std::uint64_t kSlot314PageNameRva = 0x12c2760;
inline constexpr std::uint64_t kSlot314MenuParentRva = 0x12c2789;
inline constexpr std::uint64_t kSlot314EmptyTextRva = 0x12c2c83;
inline constexpr std::uint32_t kSlot314ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot314BoolVarFlags = 1;
inline constexpr std::uint32_t kSlot314BoolCallbackFlags = 3;

// ---- variable / toggle rows ----------------------------------------------------------------
enum class Slot314VarShape : std::uint8_t { bool_inline, ranged_float, bool_callback };

struct Slot314VarSpec {
    std::uint32_t index;             // 0..9, body order
    Slot314VarShape shape;
    const char* name;
    std::uint32_t name_len;
    std::uint64_t name_rva;
    std::uint64_t object_rva;
    std::uint64_t dtor_rva;
    std::uint32_t flags;
    std::uint64_t fn_tag;            // bool_inline: [obj+0xb0]; float/callback: the argument tag
    std::uint32_t float_min_bits;    // ranged_float only
    std::uint32_t float_max_bits;
    std::uint32_t float_value_bits;  // value == default at every one of the three sites
    std::uint8_t bool_value;         // bool shapes
    std::uint8_t bool_default;
    std::uint64_t arg_q1;            // bool_callback only (0x274710 at every site)
    std::uint64_t arg_q3;            // bool_callback only: the per-site on-change tag
};

inline constexpr std::size_t kSlot314VarCount = 10;
inline constexpr std::array<Slot314VarSpec, kSlot314VarCount> kSlot314Vars = {{
    // physics_EnableUpdate: inline bool, value 1, default 1 (dword 1 also lands at page+0x2e0).
    {0, Slot314VarShape::bool_inline, "physics_EnableUpdate", 0x14, 0x12c2774, 0x32f5dd8,
     kSlot314BoolVarDtorRva, kSlot314BoolVarFlags, kSlot314BoolArgFnVtbl, 0, 0, 0, 1, 1, 0, 0},
    // physics_LockSimulationAt30Hz: inline bool, value 0, default 0.
    {1, Slot314VarShape::bool_inline, "physics_LockSimulationAt30Hz", 0x1c, 0x12c27a7, 0x32f5f48,
     kSlot314BoolVarDtorRva, kSlot314BoolVarFlags, kSlot314BoolArgFnVtbl, 0, 0, 0, 0, 0, 0, 0},
    // physics_clothMaxDistanceScalar: 0xc300, range {0, 1}, value 1.
    {2, Slot314VarShape::ranged_float, "physics_clothMaxDistanceScalar", 0x1e, 0x12c27dc, 0x32f60b8,
     kSlot314FloatVarDtorRva, kSlot314BoolVarFlags, kSlot314FloatArgFnVtbl, 0x00000000u, 0x3f800000u,
     0x3f800000u, 0, 0, 0, 0},
    // physics_clothDampening: 0xc300, range {-1, 1}, value -1.
    {3, Slot314VarShape::ranged_float, "physics_clothDampening", 0x16, 0x12c2811, 0x32f6238,
     kSlot314FloatVarDtorRva, kSlot314BoolVarFlags, kSlot314FloatArgFnVtbl, 0xbf800000u, 0x3f800000u,
     0xbf800000u, 0, 0, 0, 0},
    // physics_clothWindScalar: 0xc300, range {0, 100}, value 1.
    {4, Slot314VarShape::ranged_float, "physics_clothWindScalar", 0x17, 0x12c2838, 0x32f63b8,
     kSlot314FloatVarDtorRva, kSlot314BoolVarFlags, kSlot314FloatArgFnVtbl, 0x00000000u, 0x42c80000u,
     0x3f800000u, 0, 0, 0, 0},
    // physX_connectToPhysicsDebugger: 0xa41570, value 0, callback tag 0xa40160.
    {5, Slot314VarShape::bool_callback, "physX_connectToPhysicsDebugger", 0x1e, 0x12c2862, 0x32f65e8,
     kSlot314BoolVarDtorRva, kSlot314BoolCallbackFlags, kSlot314BoolArgFnVtbl, 0, 0, 0, 0, 0,
     0x274710, 0xa40160},
    {6, Slot314VarShape::bool_callback, "physX_enableDebugDraw", 0x15, 0x12c289d, 0x32f6758,
     kSlot314BoolVarDtorRva, kSlot314BoolCallbackFlags, kSlot314BoolArgFnVtbl, 0, 0, 0, 0, 0,
     0x274710, 0xa40230},
    {7, Slot314VarShape::bool_callback, "physX_drawClothCollision", 0x18, 0x12c28c5, 0x32f68c8,
     kSlot314BoolVarDtorRva, kSlot314BoolCallbackFlags, kSlot314BoolArgFnVtbl, 0, 0, 0, 0, 0,
     0x274710, 0xa40300},
    {8, Slot314VarShape::bool_callback, "physX_drawClothMaxDistance", 0x1a, 0x12c28f3, 0x32f6a38,
     kSlot314BoolVarDtorRva, kSlot314BoolCallbackFlags, kSlot314BoolArgFnVtbl, 0, 0, 0, 0, 0,
     0x274710, 0xa403c0},
    {9, Slot314VarShape::bool_callback, "physX_drawClothMaxDistanceInward", 0x20, 0x12c2926, 0x32f6ba8,
     kSlot314BoolVarDtorRva, kSlot314BoolCallbackFlags, kSlot314BoolArgFnVtbl, 0, 0, 0, 0, 0,
     0x274710, 0xa40480},
}};
static_assert(sizeof kSlot314PageName - 1 == 0x13 && sizeof kSlot314MenuParent - 1 == 7);

struct Slot314ItemSpec {
    std::uint32_t index;         // 0..9, body order (same pair index as its variable)
    const char* caption;         // the view 0x26ea0 receives in rdx
    std::uint32_t caption_len;
    std::uint64_t caption_rva;
    std::uint64_t parent_rva;    // "Physics" at every site
    std::uint32_t parent_len;
    std::uint64_t object_rva;
    std::uint32_t target_var;    // index into kSlot314Vars
    std::uint32_t flags;
};

inline constexpr std::size_t kSlot314ItemCount = 10;
inline constexpr std::array<Slot314ItemSpec, kSlot314ItemCount> kSlot314Items = {{
    {0, "Enable Physics Update", 0x15, 0x12c2791, kSlot314MenuParentRva, 7, 0x32f5ea0, 0, kSlot314ItemFlags},
    {1, "Lock Simulation at 30Hz", 0x17, 0x12c27c4, kSlot314MenuParentRva, 7, 0x32f6010, 1, kSlot314ItemFlags},
    {2, "Cloth Max Dist Scalar", 0x15, 0x12c27fb, kSlot314MenuParentRva, 7, 0x32f6190, 2, kSlot314ItemFlags},
    {3, "Cloth Dampening", 0xf, 0x12c2828, kSlot314MenuParentRva, 7, 0x32f6310, 3, kSlot314ItemFlags},
    {4, "Cloth Wind Scalar", 0x11, 0x12c2850, kSlot314MenuParentRva, 7, 0x32f6490, 4, kSlot314ItemFlags},
    {5, "Connect To Physics Debugger", 0x1b, 0x12c2881, kSlot314MenuParentRva, 7, 0x32f66b0, 5, kSlot314ItemFlags},
    {6, "Enable Debug Draw", 0x11, 0x12c28b3, kSlot314MenuParentRva, 7, 0x32f6820, 6, kSlot314ItemFlags},
    {7, "Draw Cloth Collision", 0x14, 0x12c28de, kSlot314MenuParentRva, 7, 0x32f6990, 7, kSlot314ItemFlags},
    {8, "Draw Cloth Max Distance", 0x17, 0x12c290e, kSlot314MenuParentRva, 7, 0x32f6b00, 8, kSlot314ItemFlags},
    {9, "Draw Cloth Max Distance Inward", 0x1e, 0x12c2947, kSlot314MenuParentRva, 7, 0x32f6c70, 9,
     kSlot314ItemFlags},
}};

// One `__cxa_atexit` registration of the body, in body order (every site is tool-pinned).
struct Slot314AtexitRow {
    std::uint32_t step;        // 1..22
    std::uint64_t site_rva;    // the `callq 0xf8f390`
    std::uint64_t dtor_rva;    // rdi (identity tag, never run)
    std::uint64_t object_rva;  // rsi (identity only)
};
inline constexpr std::size_t kSlot314AtexitRowCount = 22;
inline constexpr std::array<Slot314AtexitRow, kSlot314AtexitRowCount> kSlot314AtexitRows = {{
    {1, 0xa41a1a, kSlot314PageDtorRva, 0x32f5af0},
    {2, 0xa41b31, kSlot314BoolVarDtorRva, 0x32f5dd8},
    {3, 0xa41bbd, kDebugMenuItemDtorRva, 0x32f5ea0},
    {4, 0xa41cb7, kSlot314BoolVarDtorRva, 0x32f5f48},
    {5, 0xa41d3b, kDebugMenuItemDtorRva, 0x32f6010},
    {6, 0xa41e60, kSlot314FloatVarDtorRva, 0x32f60b8},
    {7, 0xa41ee8, kDebugMenuItemDtorRva, 0x32f6190},
    {8, 0xa42006, kSlot314FloatVarDtorRva, 0x32f6238},
    {9, 0xa4208e, kDebugMenuItemDtorRva, 0x32f6310},
    {10, 0xa421a5, kSlot314FloatVarDtorRva, 0x32f63b8},
    {11, 0xa42229, kDebugMenuItemDtorRva, 0x32f6490},
    {12, 0xa42246, kSlot314CellDtorRva, 0x32f6538},
    {13, 0xa42348, kSlot314BoolVarDtorRva, 0x32f65e8},
    {14, 0xa423cf, kDebugMenuItemDtorRva, 0x32f66b0},
    {15, 0xa4248a, kSlot314BoolVarDtorRva, 0x32f6758},
    {16, 0xa4250e, kDebugMenuItemDtorRva, 0x32f6820},
    {17, 0xa425c9, kSlot314BoolVarDtorRva, 0x32f68c8},
    {18, 0xa4264d, kDebugMenuItemDtorRva, 0x32f6990},
    {19, 0xa42708, kSlot314BoolVarDtorRva, 0x32f6a38},
    {20, 0xa4278c, kDebugMenuItemDtorRva, 0x32f6b00},
    {21, 0xa42847, kSlot314BoolVarDtorRva, 0x32f6ba8},
    {22, 0xa428cb, kDebugMenuItemDtorRva, 0x32f6c70},
}};

// One of the eight row-local `callq *0x18(%rax)` sites: (site, argument tag, slot-3 body).
struct Slot314FunctorSite {
    std::uint64_t site_rva;
    std::uint64_t tag_rva;
    std::uint64_t slot3_rva;
    bool slot3_is_bare_ret;      // pinned true; false blocks the fragment
};
inline constexpr std::size_t kSlot314FunctorSiteCount = 8;
inline constexpr std::array<Slot314FunctorSite, kSlot314FunctorSiteCount> kSlot314FunctorSites = {{
    {0xa41e1b, kSlot314FloatArgFnVtbl, kSlot314FloatArgDestroyRva, true},
    {0xa41fc4, kSlot314FloatArgFnVtbl, kSlot314FloatArgDestroyRva, true},
    {0xa42167, kSlot314FloatArgFnVtbl, kSlot314FloatArgDestroyRva, true},
    {0xa4232d, kSlot314BoolArgFnVtbl, kSlot314BoolArgDestroyRva, true},
    {0xa42472, kSlot314BoolArgFnVtbl, kSlot314BoolArgDestroyRva, true},
    {0xa425b1, kSlot314BoolArgFnVtbl, kSlot314BoolArgDestroyRva, true},
    {0xa426f0, kSlot314BoolArgFnVtbl, kSlot314BoolArgDestroyRva, true},
    {0xa4282f, kSlot314BoolArgFnVtbl, kSlot314BoolArgDestroyRva, true},
}};
// The row-local sites and 0xa41570's sites use the same two tags; 0xa41570 repeats the four-dispatch
// install twice and destroys the leftover temporary, so the whole slot performs 8 + 5*11 = 63.
inline constexpr std::uint32_t kSlot314BoolCallbackDispatches = 11;
inline constexpr std::uint32_t kSlot314FunctorDispatches = 63;

// ---- native stand-ins -------------------------------------------------------------------------
// The page's twelve self-referential nodes are a raw store pattern (meanings not claimed).
struct Slot314Node {
    void* next;                     // +0x00 written as &node
    void* prev;                     // +0x08 written as &node
    std::uint8_t gap10[0x10];       // +0x10 never written
    std::uint8_t zero20[0x10];      // +0x20 zeroed for every node but the last
};
static_assert(sizeof(Slot314Node) == 0x30);
struct Slot314NodeTail {
    void* next;                     // +0x00 written as &node
    void* prev;                     // +0x08 written as &node
    std::uint8_t gap10[0x10];       // +0x10 never written
};
static_assert(sizeof(Slot314NodeTail) == 0x20);

inline constexpr std::size_t kSlot314NodeCount = 12;

struct Slot314Page {                // 0x32f5af0, 0x2e8 B
    std::uint64_t vptr;             // +0x000  ctor tag 0x1595900, then the body's 0x1594ca0
    std::uint8_t byte08;            // +0x008  0
    std::uint8_t byte09;            // +0x009  1
    std::uint8_t gap0a[6];          // +0x00a never written
    std::uint64_t name;             // +0x010  the page name pointer
    std::uint32_t word18;           // +0x018  0
    std::uint8_t zero1c[0x24];      // +0x01c..+0x03f zeroed by the ctor (includes the +0x38 head)
    std::uint64_t link40;           // +0x040  &page + 0x38
    std::uint64_t link48;           // +0x048  &page + 0x38
    std::uint64_t word50;           // +0x050  0
    std::uint32_t word58;           // +0x058  0
    std::uint8_t gap5c[4];
    std::uint64_t word60;           // +0x060  0
    std::uint8_t gap68[8];          // +0x068 never written
    std::uint64_t word70;           // +0x070  0 (ctor)
    std::uint8_t gap78[8];          // +0x078 never written
    std::uint64_t word80;           // +0x080  0 (body)
    std::uint32_t word88;           // +0x088  0 (body)
    std::uint8_t gap8c[4];
    std::uint8_t zero90[0x20];      // +0x090..+0x0af zeroed by the body
    std::array<Slot314Node, 11> nodes;   // +0x0b0 .. +0x2bf
    Slot314NodeTail last_node;      // +0x2c0
    std::uint32_t tail2e0;          // +0x2e0  dword 1, written by variable 0's step
    std::uint8_t gap2e4[4];         // +0x2e4 never written
};
static_assert(sizeof(Slot314Page) == kSlot314PageSize);
static_assert(offsetof(Slot314Page, name) == 0x10 && offsetof(Slot314Page, zero1c) == 0x1c &&
              offsetof(Slot314Page, link40) == 0x40 &&
              offsetof(Slot314Page, link48) == 0x48 && offsetof(Slot314Page, word50) == 0x50 &&
              offsetof(Slot314Page, word58) == 0x58 && offsetof(Slot314Page, word60) == 0x60 &&
              offsetof(Slot314Page, word70) == 0x70 && offsetof(Slot314Page, word80) == 0x80 &&
              offsetof(Slot314Page, word88) == 0x88 && offsetof(Slot314Page, zero90) == 0x90 &&
              offsetof(Slot314Page, nodes) == 0xb0 && offsetof(Slot314Page, last_node) == 0x2c0 &&
              offsetof(Slot314Page, tail2e0) == 0x2e0);

// 0xa41570's product: the bool variable layout with the on-change functor at +0x90. It is the same
// 0xc1/0xc8 shape as ConCmdBoolVar, but +0x70..+0x8f holds the copied argument block and +0x90..+0x9f
// the functor's inline buffer (never written by the original).
struct Slot314BoolVar {
    ConCmdBase base;                // +0x00
    void* value_ref;                // +0x60  &this->value (this+0xc0)
    std::uint8_t default_value;     // +0x68
    std::uint8_t zero69[4];         // +0x69
    std::uint8_t gap6d[3];          // +0x6d never written
    std::uint64_t args[4];          // +0x70  copy of the caller block (q[0] self pointer if q[3]!=0)
    std::uint8_t gap90[0x10];       // +0x90 never written (functor inline buffer)
    std::uint64_t fn_payload;       // +0xa0
    std::uint8_t gapa8[8];          // +0xa8 never written
    std::uint64_t fn_vptr;          // +0xb0  0x1594280 after the install
    std::uint64_t fn_aux;           // +0xb8
    std::uint8_t value;             // +0xc0
};
static_assert(sizeof(Slot314BoolVar) == 0xc8 && offsetof(Slot314BoolVar, value_ref) == 0x60 &&
              offsetof(Slot314BoolVar, default_value) == 0x68 && offsetof(Slot314BoolVar, zero69) == 0x69 &&
              offsetof(Slot314BoolVar, args) == 0x70 && offsetof(Slot314BoolVar, gap90) == 0x90 &&
              offsetof(Slot314BoolVar, fn_payload) == 0xa0 && offsetof(Slot314BoolVar, gapa8) == 0xa8 &&
              offsetof(Slot314BoolVar, fn_vptr) == 0xb0 && offsetof(Slot314BoolVar, fn_aux) == 0xb8 &&
              offsetof(Slot314BoolVar, value) == 0xc0);

// 0x32f6538: 0xb0 B of zero with a destructor. Only qword +0x00 is written by the body.
struct Slot314Cell {
    std::uint64_t word;             // +0x00 0
    std::uint8_t rest[0xa8];        // +0x08 never written
};
static_assert(sizeof(Slot314Cell) == kSlot314CellSize);

struct Slot314Objects {
    Slot314Page page{};             // 0x32f5af0
    ConCmdBoolVar var0{};           // 0x32f5dd8
    DebugMenuItem item0{};          // 0x32f5ea0
    ConCmdBoolVar var1{};           // 0x32f5f48
    DebugMenuItem item1{};          // 0x32f6010
    ConCmdRangedVar var2{};         // 0x32f60b8
    DebugMenuItem item2{};          // 0x32f6190
    ConCmdRangedVar var3{};         // 0x32f6238
    DebugMenuItem item3{};          // 0x32f6310
    ConCmdRangedVar var4{};         // 0x32f63b8
    DebugMenuItem item4{};          // 0x32f6490
    Slot314Cell cell{};             // 0x32f6538
    Slot314BoolVar var5{};          // 0x32f65e8
    DebugMenuItem item5{};          // 0x32f66b0
    Slot314BoolVar var6{};          // 0x32f6758
    DebugMenuItem item6{};          // 0x32f6820
    Slot314BoolVar var7{};          // 0x32f68c8
    DebugMenuItem item7{};          // 0x32f6990
    Slot314BoolVar var8{};          // 0x32f6a38
    DebugMenuItem item8{};          // 0x32f6b00
    Slot314BoolVar var9{};          // 0x32f6ba8
    DebugMenuItem item9{};          // 0x32f6c70
};
static_assert(sizeof(Slot314Objects) == 0x1228, "Slot314Objects must reproduce the original strides");
static_assert(offsetof(Slot314Objects, page) == 0x000 && offsetof(Slot314Objects, var0) == 0x2e8 &&
              offsetof(Slot314Objects, item0) == 0x3b0 && offsetof(Slot314Objects, var1) == 0x458 &&
              offsetof(Slot314Objects, item1) == 0x520 && offsetof(Slot314Objects, var2) == 0x5c8 &&
              offsetof(Slot314Objects, item2) == 0x6a0 && offsetof(Slot314Objects, var3) == 0x748 &&
              offsetof(Slot314Objects, item3) == 0x820 && offsetof(Slot314Objects, var4) == 0x8c8 &&
              offsetof(Slot314Objects, item4) == 0x9a0 && offsetof(Slot314Objects, cell) == 0xa48 &&
              offsetof(Slot314Objects, var5) == 0xaf8 && offsetof(Slot314Objects, item5) == 0xbc0 &&
              offsetof(Slot314Objects, var6) == 0xc68 && offsetof(Slot314Objects, item6) == 0xd30 &&
              offsetof(Slot314Objects, var7) == 0xdd8 && offsetof(Slot314Objects, item7) == 0xea0 &&
              offsetof(Slot314Objects, var8) == 0xf48 && offsetof(Slot314Objects, item8) == 0x1010 &&
              offsetof(Slot314Objects, var9) == 0x10b8 && offsetof(Slot314Objects, item9) == 0x1180);
static_assert(kSlot314PageRva + kSlot314PageSize == kSlot314Vars[0].object_rva &&
              kSlot314Vars[0].object_rva + 0xc8 == kSlot314Items[0].object_rva &&
              kSlot314Items[0].object_rva + 0xa8 == kSlot314Vars[1].object_rva &&
              kSlot314Vars[4].object_rva + 0xd8 == kSlot314Items[4].object_rva &&
              kSlot314Items[4].object_rva + 0xa8 == kSlot314CellRva &&
              kSlot314CellRva + kSlot314CellSize == kSlot314Vars[5].object_rva &&
              kSlot314Vars[9].object_rva == 0x32f6ba8 &&
              kSlot314Items[9].object_rva == kSlot314LastObjectRva &&
              kSlot314LastObjectRva + 0xa8 == kSlot314NextCtorFirstObjectRva,
              "the original object addresses follow the pinned strides");

enum class Slot314Stage : std::uint8_t {
    none,
    page_ctor,
    page_stores,
    page_atexit,
    page_tail,
    variable_build,
    variable_registration,
    variable_view_release,
    variable_atexit,
    item_ctor,
    item_registration,
    item_view_release,
    item_atexit,
    cell_atexit,
    canary,
};
[[nodiscard]] const char* slot314_stage_name(Slot314Stage stage) noexcept;

struct Slot314VarResult {
    bool ok = false;
    std::uint32_t functor_dispatches = 0;   // kSlot314BoolCallbackDispatches once modelled
    bool description_inert = false;         // the 0xf590 on the stack description view
    bool local_functor_inert = false;       // both `[tag+0x18]` destroys are the pinned bare ret
    ConCmdRegisterResult registration{};
};

struct Slot314PageResult {
    bool ok = false;
};

struct Slot314Hooks {
    // 0x475c0. NO default in this struct: a missing hook stops the fragment at `page_ctor`.
    // `default_slot314_hooks()` installs the native model.
    Slot314PageResult (*page_ctor)(Slot314Page& page, const SharedStr& name) noexcept = nullptr;
    // 0xa41570. NO default in this struct: a missing hook stops the fragment at that variable's
    // `variable_build` stage. `default_slot314_hooks()` installs the native net-state model.
    Slot314VarResult (*bool_callback_ctor)(Slot314BoolVar& var, const Slot314VarSpec& spec,
                                           ConCmdRegistry& registry) noexcept = nullptr;
    // __stack_chk_guard source (0x15fe518). Null => the comparison is not performed and
    // `canary_checked` stays false; a changed guard fails the fragment at stage `canary`.
    const std::uint64_t* stack_guard = nullptr;
};

// 0x475c0: the page BASE constructor (87 B). Writes +0x00..+0x70 only (the body then stores the
// derived vptr 0x1594ca0 and the +0x80..+0x2df pattern).
[[nodiscard]] Slot314PageResult init_slot314_page_475c0(Slot314Page& page, const SharedStr& name) noexcept;
// The body's own page stores: +0x80 = 0, +0x88 = 0, +0x90..+0xaf = 0, the twelve self-referential
// nodes, and the 16 zero bytes at node+0x20 for every node but the last.
void init_slot314_page_stores(Slot314Page& page) noexcept;
// 0xc300 with THIS body's argument tag 0x15d1770 (the shared `init_ranged_float_var_c300` accepts
// only three tags, none of them this one; the stores are identical). Refuses (writes nothing) when the
// spec is not a ranged_float row or the tag is not 0x15d1770.
[[nodiscard]] bool init_slot314_float_var_c300(ConCmdRangedVar& var, const Slot314VarSpec& spec) noexcept;
// 0xa41570: the bool variable with an on-change functor. Refuses (writes nothing) when the spec is not
// a bool_callback row or the argument tag is not the pinned class.
[[nodiscard]] Slot314VarResult init_slot314_bool_var_callback(Slot314BoolVar& var,
                                                              const Slot314VarSpec& spec,
                                                              ConCmdRegistry& registry) noexcept;
// The production hooks: both native models, no guard.
[[nodiscard]] Slot314Hooks default_slot314_hooks() noexcept;
// The 0x26ea0 gate answer of a variable, read from the stand-in's own vptr tag (never a hook).
[[nodiscard]] DebugMenuTarget slot314_var_menu_target(const ConCmdBase& cmd) noexcept;
// The lifted `callq *0x18(%rax)` for the eight row-local sites: a bare `ret` has no effect.
[[nodiscard]] bool slot314_inert_functor_destroy(const Slot314FunctorSite& site) noexcept;
// The stand-ins the aggregate holds, by index (0..4 bool/float, 5..9 callback).
[[nodiscard]] ConCmdBase& slot314_var_base(Slot314Objects& objects, std::uint32_t index) noexcept;
[[nodiscard]] DebugMenuItem& slot314_item(Slot314Objects& objects, std::uint32_t index) noexcept;

struct Slot314Result {
    bool ok = false;
    Slot314Stage blocked = Slot314Stage::none;
    std::uint32_t blocked_index = 0;           // the pair index 0..9 for the per-object stages
    std::uint32_t atexit_records = 0;          // this body's 22 __cxa_atexit registrations
    std::uint32_t functors_destroyed = 0;      // the eight row-local `callq *0x18(%rax)` sites
    std::uint32_t functor_dispatches = 0;      // all collapsed dispatches (63 at completion)
    std::uint32_t vars_built = 0;              // ten console variables
    std::uint32_t menu_items = 0;              // ten toggles
    Slot314PageResult page{};
    std::array<Slot314VarResult, kSlot314VarCount> var_results{};
    std::array<DebugMenuRegisterResult, kSlot314ItemCount> item_registrations{};
    bool views_inert = true;                   // every 0xf590 call of this body was a no-op
    bool canary_checked = false;               // false when the host supplied no guard
    bool canary_matched = false;
};

// Runs original constructor 314 natively. The original is unconditional, so the first failing step
// stops the slot (nothing after it is claimed); every failure names its stage and object index.
[[nodiscard]] Slot314Result init_slot314_a41810(Slot314Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu,
                                                const Slot314Hooks& hooks = {}) noexcept;

}  // namespace sotc
