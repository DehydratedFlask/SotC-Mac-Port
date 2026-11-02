// CPU-01-STARTUP-SLOT-329-CHARACTERIZATION-01: native recovery of original constructor-table index
// 329 / RVA 0x7edf10 (4,082 B FDE [0x7edf10, 0x82ae82); 4,081 B of code + the trailing nop at
// 0x82ae81), ELF sha256 d0723afb...5861d3. Offline analysis only: nothing here loads, relocates or
// executes an original RVA. Byte oracle: tools/lift/check_ctor_slot_329.py.
//
// ============================ characterization (from the pinned bytes) ============================
// Reached by DATA only: the startup table word at 0x2dd70e0 - 329*8 = **0x2dd6698** holds 0x7edf10.
// Table neighbours: entry 328 = 0x82e040 (0x2dd66a0) and entry 330 = 0x810a50 (0x2dd6690), both
// already registered.
// CORRECTION (2026-11-01): the task brief quoted the row as 0x2dd65b8. That address is entry **357**
// (0x78c760): 0x2dd70e0 - 0x2dd65b8 = 0xb28 = 357*8. The backwards-table arithmetic, the two
// registered neighbours and the recovered-row RVA all agree on 0x2dd6698, which is what this model
// and its checker pin. (Slot 326's brief carried the same off-by-one-table-row error; see its header.)
//
// Body shape from the pinned listing: 774 instructions (the 774th is the trailing nop), **97 direct
// call sites over 11 targets**, 3 indirect call sites (every one a `callq *0x18(%rax)`), 124
// rip-relative operands (69 distinct targets; **11 of them stores over 9 cells** -- only the FIRST
// bool variable is addressed rip-relative, every later object is reached through a register base),
// and **exactly one conditional branch** -- 0x82ae6b `jne 0x82ae7c`, the __stack_chk_guard compare --
// plus the unpicked __stack_chk_fail tail at 0x82ae7c. So the body is **two basic blocks**: the
// straight-line constructor run [0x7edf10, 0x82ae7b] (one `retq`) and the canary-failure tail. The
// frame:
//     push rbp; mov rsp,rbp; push r15/r14/r13/r12/rbx; and rsp,-0x20; sub rsp,0xe0
//     guard <- *[0x15fe518]  (the GOT cell holds the runtime address of __stack_chk_guard)
//     ... 760 instructions of constructor work ...
//     cmp [rsp+0xc0],%rax ; jne 0x82ae7c ; lea -0x28(%rbp),%rsp; pops; retq
//
// Callee histogram (every one of the 97 sites is pinned by the checker):
//   0xf590    x43  shared release of a borrowed stack view (every site is an exact no-op)
//   0xf8f390  x22  __cxa_atexit PLT (third argument 0x1604218 = __dso_handle, one per object)
//   0x26ea0   x10  debug-menu toggle item ctor (kind 2)
//   0x20d60   x9   register-once guard (one per console variable except the two wrapped ranges)
//   0x20af0   x7   ConCmdBase ctor (the seven bool variables)
//   0x8298d0  x1   local ranged-variable leaf A (ranged class P, see below)
//   0x829b30  x1   local ranged-variable wrapper B (ranged class C, see below)
//   0xc300    x1   float-variable ctor (the one float)
//   0x26c10   x1   debug-menu group item ctor (kind 1)
//   0x26b60   x1   debug-menu separator item ctor (kind 3)
//   0xf8f3a0  x1   __stack_chk_fail (the branch target only)
// The nine 0x20d60 calls are the seven bool variables plus the two range sites whose constructors do
// not register (leaf A / wrapper B call it themselves, and 0xc300 does too, which is why the three
// range sites do not add a ninth and tenth direct site).
//
// The 3 indirect sites (0x82a186 / 0x82a467 / 0x82a563) are `callq *0x18(%rax)` with %rax = the
// *argument* functor tag of the range site (the site copies the caller's argument block to the stack
// and dispatches its slot 3 after the constructor returns). The tags and their slot-3 bodies:
//   0x15ceac8 -> 0x829ac0  (one `c3`)
//   0x15ceae8 -> 0x829b20  (one `c3`)
//   0x15ceb80 -> 0xc4f0    (one `c3`)
// so all three are inert destroys of a stack temporary. 0x15ceac8 / 0x15ceae8 are the two local
// four-slot functor tables {copy, clone, move, bare-ret destroy}; 0x15ceb80 is the float family
// {0xc4a0, 0xc4c0, 0xc4e0, 0xc4f0} whose four bodies are byte-identical to 0x1594318's.
//
// ==================== the two local ranged-variable constructors ====================
// This is the only startup slot so far that carries its OWN console-variable constructor bodies
// instead of calling 0xc300 / 0xb0e060 / the 0xa4f4d0 family:
//   leaf A    0x8298d0 (352 B FDE, 90 instructions)  used directly by the slot for ranged P
//   wrapper B 0x829b30 (436 B FDE, 99 instructions)  used by the slot for ranged C
//   leaf B    0x829cf0 (352 B FDE, 90 instructions)  called only by wrapper B
//   dtor A    0x81be00 (54 B)   final slot 0 of class P (the atexit rdi)
//   dtor A'   0x829a30 (62 B)   final slot 1 of class P (non-deleting)
//   dtor C    0x81be40 (54 B)   final slot 0 of class C
//   dtor C'   0x829e50 (62 B)   final slot 1 of class C
//   functor A 0x829a70 copy / 0x829a90 clone / 0x829ab0 move / 0x829ac0 destroy (bare ret)
//   functor C 0x829ad0 copy / 0x829af0 clone / 0x829b10 move / 0x829b20 destroy (bare ret)
// A full-image rel32 census gives each of 0x8298d0 / 0x829b30 exactly one caller (0x82a165 /
// 0x82a446, both inside this body) and 0x829cf0 exactly one caller (0x829c87, inside wrapper B).
// The leaves are the shape already native as `init_ranged_bound_var_a4f4d0`'s leaf (0x20af0, mid
// vptr, value_ref, both bounds engaged, the argument copy, the functor dance) parameterised by
// (mid vptr, class functor tag); the wrapper is that template's wrapper (build a local argument
// block under the class tag, call the leaf, re-store the final vptr, copy *value_ref into +0xd0 and
// +0x68, then 0x20d60). The model reproduces the net state of both with the shared
// `install_ranged_fn_from_args`, which is exactly what the composed wrapper+leaf leaves behind.
//
// The two class identities (vtable address points, pinned through .data.rel.ro RELATIVE relocations):
//   class P   mid 0x15ce950 (0x15ce940+0x10)   final 0x15cea60 (0x15cea50+0x10)   fn tag 0x15ceac8
//             final slot 0 = 0x81be00  slot 1 = 0x829a30  slot 2 = 0x20d60
//             slot 3 = 0x8254c0 -> 1   slot 6 = 0x825510 -> 1
//   class C   mid 0x15ce9c8 (0x15ce9b8+0x10)   final 0x15ceb18 (0x15ceb08+0x10)   fn tag 0x15ceae8
//             final slot 0 = 0x81be40  slot 1 = 0x829e50  slot 2 = 0x20d60
//             slot 3 = 0x825f40 -> 1   slot 6 = 0x8261e0 -> 1
// Both slot-3/slot-6 answers are 1, so the 0x26ea0 toggle on either range keeps its default range
// {1.0, 10.0} (the {0.1, 0.5} narrowing needs slot3 == 1 AND slot6 == 3).
//
// What the block IS. It is the **Shadow Path / path-finding debug page**: seven console bools, two
// integer ranges and one float range, one "Shadow"/"Shadow Path" menu group, one separator row and
// ten toggles -- one toggle per variable except the two range variables that own the two range
// menus. The strings are the evidence (every one is a byte-pinned rip operand):
//   names      enablePathFinding (0x128d690, 0x11), shadow_pathDebugDrawEnable (0x128d6a2, 0x1a),
//              shadow_pathDebugDrawPathDataEnable (0x128d6bd, 0x22),
//              shadow_pathDebugDrawAttainRangeEnable (0x128d6e0, 0x25),
//              shadow_pathEnableEditing (0x128d706, 0x18), shadow_pathDebugDrawCharaType
//              (0x128d71f, 0x1d), shadow_pathDebugTranslationSpeed (0x128d784, 0x20),
//              shadow_pathDebugDisplayAllNames (0x128d7a5, 0x1f),
//              shadow_enableSpeculativeAsyncRaycasts (0x128d7c5, 0x25),
//              shadow_debugSpeculativeAsyncRaycasts (0x128d7eb, 0x24)
//   groups     Shadow (0x128d810, 6) -> Shadow Path (0x128d817, 0xb)
//   captions   Debug Draw Enable (0x128d823, 0x11), Debug Draw Data Enable (0x128d835, 0x16),
//              Debug Draw Attain Range Enable (0x128d84c, 0x1e), Debug Draw Chara Type
//              (0x128d86b, 0x15), Enable Path Finding (0x128d881, 0x13), Enable Editing
//              (0x128d895, 0xe), Editing Trans Speed (0x128d8a4, 0x13), Show All Names
//              (0x128d8b8, 0xe), Speculative Async Raycasts Enable (0x128d8c7, 0x21),
//              Speculative Async Raycasts Debug (0x128d8e9, 0x20)
//   description 0x128dc9a (a zero byte: the empty description of nine of the ten variables); the one
//              non-empty description is 0x128d73d, 0x46 B "Which attract mode to play. Default will
//              play the system selected one." -- carried by shadow_pathDebugDrawCharaType. The text
//              reads like an attract-mode description, but the bytes put it at that site; it is
//              recorded as the byte fact and not interpreted.
//   enum text  0x128d9fc..0x128dc12: "true"/"false", "Off"/"Proximity"/"All" with their
//              EPathDataDrawType:: spellings, "%d", "Boss"/"Horse"/"SmallAnimal"/"SmallAnimalDemo"/
//              "MAX" with their EPathObjectCharaType:: spellings, "%.4f", and the module's own
//              __FILE__ "d:\dev\cp10\Main\Code\Game\CP10\Shadow\sources\gamecore\path\src\
//              pathmanager.cpp" with the assert "void bp_doDrawPathPoint(PathDataAddr *, int)".
//              That pathmanager.cpp string is what names the subsystem; the model does not execute
//              the assert and claims no path-finding behaviour.
// "MenuSeparator" (0xf96530) is the separator row's built text, through the native 0xf1a0 model.
//
// ================================ object map (verified strides) ================================
// The ten variables and twelve menu items are one contiguous block, 0x3288d98 .. 0x3289d78 (0xfe0):
//   0x3288d98  bool   enablePathFinding                0xc8  (value cell = own +0xc0)
//   0x3288e60  bool   shadow_pathDebugDrawEnable       0xc8
//   0x3288f28  range  shadow_pathDebugDrawPathDataEnable   0xd8 (class P)
//   0x3289000  bool   shadow_pathDebugDrawAttainRangeEnable 0xc8
//   0x32890c8  bool   shadow_pathEnableEditing          0xc8
//   0x3289190  range  shadow_pathDebugDrawCharaType     0xd8 (class C)
//   0x3289268  float  shadow_pathDebugTranslationSpeed  0xd8 (0xc300)
//   0x3289340  bool   shadow_pathDebugDisplayAllNames   0xc8
//   0x3289408  bool   shadow_enableSpeculativeAsyncRaycasts 0xc8
//   0x32894d0  bool   shadow_debugSpeculativeAsyncRaycasts  0xc8
//   0x3289598  group  "Shadow" -> "Shadow Path"         0xa8
//   0x3289640 .. 0x3289cd0  eleven more items, **0xa8 apart** (separator at 0x3289988)
//   0x3289d78  end of the block
// The range objects are 0xd8 apart like 0xc300's, not 0xd0: leaf A writes +0xd0 (its own value
// cell) and +0xc8 (functor aux), so the object really is the `ConCmdRangedVar` shape.
//
// ================================ modelled vs hook (G2) =================================
// MODELLED (native, exact):
//   * the frame, the canary guard and the epilogue;
//   * the seven bool variables through the existing `init_bool_var_static` (class 0x1594218,
//     argument tag 0x15cea30, value/default per row, its own two 0xf590 releases, 0x20d60 and the
//     __cxa_atexit(0x20b0) record);
//   * the two integer ranges through `slot329_ranged_var_state` (the net state of leaf A / the
//     wrapper B + leaf B composition: 0x20af0 with flags 1, the row's mid vptr, value_ref, both
//     bounds engaged from the caller's cells, the argument copy, `install_ranged_fn_from_args`
//     under the row's class tag), then the site's own final vptr restore, its `*value_ref` copy into
//     +0xd0 and +0x68, 0x20d60 and the __cxa_atexit record of 0x81be00 / 0x81be40;
//   * the one float through `slot329_float_var_state` (0xc300 via `init_ranged_float_var_c300`,
//     bounds [0, 1000.0f], value/default 1.0f, class 0x15942b0, atexit 0x20f0);
//   * the group through `init_menu_group_26c10`, the separator through `init_menu_separator_26b60`
//     and the ten toggles through `init_debug_menu_item_26ea0`, all registered through
//     `register_menu_item_268b0` and each followed by `record_menu_item_dtor_272d0`;
//   * every 0xf590 release, every __cxa_atexit record, and the three inert `callq *0x18(%rax)`
//     destroys, in body order.
// HOOK (the only one this block needs): `Slot329Hooks::separator_alloc`, the allocator pair behind
// 0x12820 / 0x12af0 that 0x26b60 needs for "MenuSeparator". This is the same documented malloc/free
// stand-in the registered slot 306 / 316 / 326 models use; `default_slot329_hooks()` installs it, and
// clearing it makes the fragment fail visibly at `item_ctor` (the original would have faulted). The
// engine allocator body is NOT recovered and is not claimed.
// Optional host input: `Slot329Hooks::stack_guard`; without it the guard compare is not claimed
// (`canary_checked` stays false) and the result never reports a silent pass.
//
// Fail-visible contracts (all reported through `blocked` / `ok == false`, never a silent pass):
//   * a null `separator_alloc` allocator, or an allocation failure, at the separator row;
//   * a range site whose argument functor tag is not the pinned row's class tag (refuse, write
//     nothing: the original would dispatch that table's slot bodies);
//   * a `value_ref` whose cell the site cannot read (null -> refuse, write nothing);
//   * a failed 0x20d60 registration;
//   * a 0x26ea0 target whose vptr tag is not one of the four pinned classes (an unpinned class
//     would run unlifted virtual code);
//   * a reached 0x268b0 assert/gate site (only possible if a row's flags changed);
//   * a failed __cxa_atexit record;
//   * a `__stack_chk_guard` that changed across the body.
//
// NOT modelled: every destructor body (0x20b0 bool, 0x20f0 float, 0x81be00/0x81be40 range,
// 0x829a30/0x829e50 range non-deleting, 0x272d0 item) and the functor slot bodies -- they are pinned
// identity recorders that are never executed. No path-finding, navigation, AI or gameplay behaviour
// is claimed; nothing here reads the module's string table back out of the image.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"
#include "fmenu_ctors_slot306.hpp"
#include "l2_slot_common.hpp"
#include "shared_str_ctor_f1a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot329Rva = 0x7edf10;
inline constexpr std::size_t kSlot329BodySize = 4082;          // FDE [0x7edf10, 0x82ae82)
inline constexpr std::uint64_t kSlot329EndRva = 0x82ae82;
inline constexpr std::size_t kSlot329ReachableBytes = 4081;    // through the callq at 0x82ae7c
inline constexpr std::size_t kSlot329InstructionCount = 774;   // the 774th is the trailing nop
inline constexpr std::size_t kSlot329DirectCallSites = 97;
inline constexpr std::size_t kSlot329DirectCallees = 11;
inline constexpr std::size_t kSlot329IndirectCallSites = 3;
inline constexpr std::size_t kSlot329RipOperands = 124;
inline constexpr std::size_t kSlot329RipTargets = 69;
inline constexpr std::size_t kSlot329StoreSites = 11;
inline constexpr std::size_t kSlot329StoreTargets = 9;
inline constexpr std::size_t kSlot329BranchCount = 1;
inline constexpr std::size_t kSlot329BasicBlocks = 2;
inline constexpr std::uint64_t kSlot329TableTopRva = 0x2dd70e0;   // index i at TOP - 8*i
inline constexpr std::uint64_t kSlot329TableWordRva = 0x2dd6698;  // 0x2dd70e0 - 329*8
inline constexpr std::uint32_t kSlot329TableIndex = 329;
// The next index that is still unrecovered once this one is registered: 330..334 are already
// registered startup slots, so the cliff moves to index 335 (0x7edf10).
inline constexpr std::uint32_t kSlot329NextUnrecoveredIndex = 335;
inline constexpr std::uint64_t kSlot329NextUnrecoveredRva = 0x7edf10;

// Frame / guard.
inline constexpr std::uint64_t kSlot329CanaryCellRva = 0x15fe518;   // GOT cell holding &__stack_chk_guard
inline constexpr std::uint32_t kSlot329CanaryFrameOffset = 0xc0;    // [rsp+0xc0]
inline constexpr std::uint64_t kSlot329StackChkFailRva = 0xf8f3a0;
inline constexpr std::uint64_t kSlot329BranchRva = 0x82ae6b;        // jne 0x82ae7c (the guard compare)
inline constexpr std::uint64_t kSlot329BranchTargetRva = 0x82ae7c;
inline constexpr std::uint64_t kSlot329EpilogueRva = 0x82ae6d;      // lea -0x28(%rbp),%rsp
inline constexpr std::uint32_t kSlot329FrameBytes = 0xe0;           // sub rsp,0xe0 (after and rsp,-0x20)

// Shared callees.
inline constexpr std::uint64_t kSlot329AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot329DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot329ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot329BaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot329RegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot329ToggleCtorRva = 0x26ea0;
inline constexpr std::uint64_t kSlot329GroupCtorRva = 0x26c10;
inline constexpr std::uint64_t kSlot329SeparatorCtorRva = 0x26b60;
inline constexpr std::uint64_t kSlot329FloatCtorRva = 0xc300;
inline constexpr std::size_t kSlot329FloatCtorSize = 0x160;

// The block's own ranged-variable constructors (one caller each; see the header comment).
inline constexpr std::uint64_t kSlot329RangedLeafARva = 0x8298d0;
inline constexpr std::size_t kSlot329RangedLeafASize = 352;
inline constexpr std::uint64_t kSlot329RangedWrapperRva = 0x829b30;
inline constexpr std::size_t kSlot329RangedWrapperSize = 436;
inline constexpr std::uint64_t kSlot329RangedLeafBRva = 0x829cf0;
inline constexpr std::size_t kSlot329RangedLeafBSize = 352;

// Destructors (identity tags recorded with __cxa_atexit, never executed).
inline constexpr std::uint64_t kSlot329BoolVarDtorRva = 0x20b0;
inline constexpr std::uint64_t kSlot329FloatVarDtorRva = 0x20f0;
inline constexpr std::uint64_t kSlot329MenuItemDtorRva = 0x272d0;
inline constexpr std::uint64_t kSlot329PathDataDtorRva = 0x81be00;
inline constexpr std::uint64_t kSlot329PathDataNonDeletingDtorRva = 0x829a30;
inline constexpr std::uint64_t kSlot329CharaDtorRva = 0x81be40;
inline constexpr std::uint64_t kSlot329CharaNonDeletingDtorRva = 0x829e50;

// Identity tags (original vtable address points; never dispatched through here).
inline constexpr std::uint64_t kSlot329BoolMidVptr = 0x15940b0;        // 0x15940a0 + 0x10
inline constexpr std::uint64_t kSlot329BoolFinalVptr = 0x1594218;      // 0x1594208 + 0x10
inline constexpr std::uint64_t kSlot329BoolArgFnVtbl = 0x15cea30;      // the seven bool sites' tag
inline constexpr std::uint64_t kSlot329NodeFnCopyRva = 0xc2a0;         // slot 0 of 0x15cea30
inline constexpr std::uint64_t kSlot329NodeFnCloneRva = 0xc2c0;        // slot 1
inline constexpr std::uint64_t kSlot329NodeFnMoveRva = 0xc2e0;         // slot 2
inline constexpr std::uint64_t kSlot329NodeFnDestroyRva = 0xc2f0;      // slot 3: one `retq`
inline constexpr std::uint64_t kSlot329FloatMidVptr = 0x1594128;       // 0x1594118 + 0x10
inline constexpr std::uint64_t kSlot329FloatFinalVptr = 0x15942b0;     // 0x15942a0 + 0x10
inline constexpr std::uint64_t kSlot329FloatClassFnVtbl = 0x1594318;   // the 0xc300 class tag
inline constexpr std::uint64_t kSlot329FloatArgFnVtbl = 0x15ceb80;     // the site's argument tag
inline constexpr std::uint64_t kSlot329FloatFnCopyRva = 0xc4a0;        // slot 0
inline constexpr std::uint64_t kSlot329FloatFnCloneRva = 0xc4c0;       // slot 1
inline constexpr std::uint64_t kSlot329FloatFnMoveRva = 0xc4e0;        // slot 2
inline constexpr std::uint64_t kSlot329FloatFnDestroyRva = 0xc4f0;     // slot 3: one `retq`
// Class P (ranged, leaf A): mid / final / functor tag.
inline constexpr std::uint64_t kSlot329PathDataMidVptr = 0x15ce950;    // 0x15ce940 + 0x10
inline constexpr std::uint64_t kSlot329PathDataFinalVptr = 0x15cea60;  // 0x15cea50 + 0x10
inline constexpr std::uint64_t kSlot329PathDataArgFnVtbl = 0x15ceac8;
inline constexpr std::uint64_t kSlot329PathDataFnCopyRva = 0x829a70;
inline constexpr std::uint64_t kSlot329PathDataFnCloneRva = 0x829a90;
inline constexpr std::uint64_t kSlot329PathDataFnMoveRva = 0x829ab0;
inline constexpr std::uint64_t kSlot329PathDataFnDestroyRva = 0x829ac0;
// Class C (ranged, wrapper B + leaf B): mid / final / functor tag.
inline constexpr std::uint64_t kSlot329CharaMidVptr = 0x15ce9c8;       // 0x15ce9b8 + 0x10
inline constexpr std::uint64_t kSlot329CharaFinalVptr = 0x15ceb18;     // 0x15ceb08 + 0x10
inline constexpr std::uint64_t kSlot329CharaArgFnVtbl = 0x15ceae8;
inline constexpr std::uint64_t kSlot329CharaFnCopyRva = 0x829ad0;
inline constexpr std::uint64_t kSlot329CharaFnCloneRva = 0x829af0;
inline constexpr std::uint64_t kSlot329CharaFnMoveRva = 0x829b10;
inline constexpr std::uint64_t kSlot329CharaFnDestroyRva = 0x829b20;

// The 0x26ea0 gate answers of the four classes this body targets, read from the target's vptr tag.
inline constexpr std::uint64_t kSlot329BoolGate3Rva = 0xb890;     // mov eax,1; ret
inline constexpr std::uint64_t kSlot329BoolGate6Rva = 0xb930;     // xor eax,eax; ret
inline constexpr std::uint64_t kSlot329FloatGate3Rva = 0xbba0;    // mov eax,1; ret
inline constexpr std::uint64_t kSlot329FloatGate6Rva = 0xbbf0;    // mov eax,3; ret
inline constexpr std::uint64_t kSlot329PathDataGate3Rva = 0x8254c0;   // mov eax,1; ret
inline constexpr std::uint64_t kSlot329PathDataGate6Rva = 0x825510;   // mov eax,1; ret
inline constexpr std::uint64_t kSlot329CharaGate3Rva = 0x825f40;      // mov eax,1; ret
inline constexpr std::uint64_t kSlot329CharaGate6Rva = 0x8261e0;      // mov eax,1; ret
inline constexpr std::uint32_t kSlot329BoolGate3 = 1;
inline constexpr std::uint32_t kSlot329BoolGate6 = 0;
inline constexpr std::uint32_t kSlot329FloatGate3 = 1;
inline constexpr std::uint32_t kSlot329FloatGate6 = 3;   // so the float toggle narrows to {0.1, 0.5}
inline constexpr std::uint32_t kSlot329PathDataGate3 = 1;
inline constexpr std::uint32_t kSlot329PathDataGate6 = 1;
inline constexpr std::uint32_t kSlot329CharaGate3 = 1;
inline constexpr std::uint32_t kSlot329CharaGate6 = 1;

// Per-site constants.
inline constexpr std::uint32_t kSlot329VarFlags = 1;         // r9d at every 0xc300 / leaf / wrapper site
inline constexpr std::uint32_t kSlot329ItemFlags = 0x30;     // every group / separator / toggle of this body
inline constexpr std::uint32_t kSlot329PathDataMin = 0;      // the site's 0x1c(%rsp) cell
inline constexpr std::uint32_t kSlot329PathDataMax = 2;      // the site's 0x18(%rsp) cell
inline constexpr std::uint32_t kSlot329PathDataValue = 2;    // stored into +0xd0 and +0x68
inline constexpr std::uint32_t kSlot329CharaMin = 0;
inline constexpr std::uint32_t kSlot329CharaMax = 3;
inline constexpr std::uint32_t kSlot329CharaValue = 1;
inline constexpr std::uint32_t kSlot329TranslationMinBits = 0x00000000u;     // 0.0f
inline constexpr std::uint32_t kSlot329TranslationMaxBits = 0x447a0000u;     // 1000.0f
inline constexpr std::uint32_t kSlot329TranslationValueBits = 0x3f800000u;   // 1.0f, stored twice

// Strings of the block: identity address + pinned length (the native views are literals / measured).
inline constexpr char kSlot329EmptyText[] = "";                    // 0x128dc9a, a zero byte
inline constexpr std::uint64_t kSlot329EmptyTextRva = 0x128dc9a;
inline constexpr std::size_t kSlot329VarCount = 10;
inline constexpr std::size_t kSlot329ItemCount = 12;               // 1 group + 10 toggles + 1 separator
inline constexpr std::size_t kSlot329ToggleCount = 10;
inline constexpr std::size_t kSlot329AtexitRowCount = 22;
inline constexpr std::size_t kSlot329FunctorSiteCount = 3;
// Indirect functor dispatches one construction performs: 5 in leaf A / leaf B, 6 in wrapper B, 5 in
// 0xc300, plus the site's own `callq *0x18(%rax)` per range site. The model collapses them to the
// net state and reports the count instead of pretending to run them.
inline constexpr std::uint32_t kSlot329PathDataDispatches = 6;   // 5 leaf A + 1 site
inline constexpr std::uint32_t kSlot329CharaDispatches = 12;     // 6 wrapper B + 5 leaf B + 1 site
inline constexpr std::uint32_t kSlot329TranslationDispatches = 6;  // 5 in 0xc300 + 1 site
inline constexpr std::uint32_t kSlot329FunctorDispatches = 24;     // the three rows above, summed
static_assert(kSlot329FunctorDispatches ==
                  kSlot329PathDataDispatches + kSlot329CharaDispatches + kSlot329TranslationDispatches,
              "the collapsed dispatch count is the three range rows");

// ---- the two ranged-variable instantiations ------------------------------------------------
// A row that is not exactly one of these two is refused (its vtable slots would not be the pinned
// bodies). `wrapped` says whether the slot inlines the wrapper (P) or calls it (C); the model
// reproduces the same net state either way but keeps the stage order the site really has.
struct Slot329RangedClass {
    std::uint64_t mid_vptr;
    std::uint64_t final_vptr;
    std::uint64_t fn_vtbl;
    std::uint64_t functor_destroy_rva;   // [[fn_vtbl] + 0x18], a bare `ret`
    std::uint64_t dtor_rva;              // final slot 0 (the __cxa_atexit rdi)
    std::uint64_t non_deleting_dtor_rva; // final slot 1
    std::uint32_t gate3;                 // final slot 3 answer
    std::uint32_t gate6;                 // final slot 6 answer
    std::uint64_t gate3_rva;
    std::uint64_t gate6_rva;
    std::uint32_t functor_dispatches;    // wrapper + leaf slot calls the model collapses
    bool wrapped;                        // false: the site inlines the wrapper (class P)
};
inline constexpr std::size_t kSlot329RangedClassCount = 2;
inline constexpr std::array<Slot329RangedClass, kSlot329RangedClassCount> kSlot329RangedClasses = {{
    {kSlot329PathDataMidVptr, kSlot329PathDataFinalVptr, kSlot329PathDataArgFnVtbl,
     kSlot329PathDataFnDestroyRva, kSlot329PathDataDtorRva, kSlot329PathDataNonDeletingDtorRva,
     kSlot329PathDataGate3, kSlot329PathDataGate6, kSlot329PathDataGate3Rva, kSlot329PathDataGate6Rva,
     5, false},
    {kSlot329CharaMidVptr, kSlot329CharaFinalVptr, kSlot329CharaArgFnVtbl, kSlot329CharaFnDestroyRva,
     kSlot329CharaDtorRva, kSlot329CharaNonDeletingDtorRva, kSlot329CharaGate3, kSlot329CharaGate6,
     kSlot329CharaGate3Rva, kSlot329CharaGate6Rva, 11, true},
}};
[[nodiscard]] const Slot329RangedClass* slot329_ranged_class_for(std::uint64_t fn_vtbl) noexcept;

// ---- the ten console variables ---------------------------------------------------------------
enum class Slot329VarKind : std::uint8_t { bool_var, ranged_int, ranged_float };

struct Slot329VarSpec {
    const char* name;
    std::uint32_t name_len;
    std::uint64_t name_rva;
    const char* description;        // nullptr => the empty view at 0x128dc9a
    std::uint32_t description_len;
    std::uint64_t description_rva;
    std::uint64_t object_rva;
    Slot329VarKind kind;
    std::uint32_t min_cell;         // raw 4-byte cell (bool / float rows do not store it)
    std::uint32_t max_cell;
    std::uint32_t value;            // byte at +0xc0 (bool) or dword at +0xd0 (range)
    std::uint32_t default_value;    // byte at +0x68 (bool) or dword at +0x68 (range)
    std::uint64_t final_vptr;
    std::uint64_t fn_vtbl;          // the argument functor tag of the site
    std::uint64_t atexit_site_rva;
    std::uint64_t dtor_rva;
};
inline constexpr std::array<Slot329VarSpec, kSlot329VarCount> kSlot329Vars = {{
    {"enablePathFinding", 0x11, 0x128d690, nullptr, 0, kSlot329EmptyTextRva, 0x3288d98,
     Slot329VarKind::bool_var, 0, 0, 1, 1, kSlot329BoolFinalVptr, kSlot329BoolArgFnVtbl, 0x829fc7,
     kSlot329BoolVarDtorRva},
    {"shadow_pathDebugDrawEnable", 0x1a, 0x128d6a2, nullptr, 0, kSlot329EmptyTextRva, 0x3288e60,
     Slot329VarKind::bool_var, 0, 0, 0, 0, kSlot329BoolFinalVptr, kSlot329BoolArgFnVtbl, 0x82a0a6,
     kSlot329BoolVarDtorRva},
    {"shadow_pathDebugDrawPathDataEnable", 0x22, 0x128d6bd, nullptr, 0, kSlot329EmptyTextRva,
     0x3288f28, Slot329VarKind::ranged_int, kSlot329PathDataMin, kSlot329PathDataMax,
     kSlot329PathDataValue, kSlot329PathDataValue, kSlot329PathDataFinalVptr,
     kSlot329PathDataArgFnVtbl, 0x82a1c1, kSlot329PathDataDtorRva},
    {"shadow_pathDebugDrawAttainRangeEnable", 0x25, 0x128d6e0, nullptr, 0, kSlot329EmptyTextRva,
     0x3289000, Slot329VarKind::bool_var, 0, 0, 0, 0, kSlot329BoolFinalVptr, kSlot329BoolArgFnVtbl,
     0x82a2ac, kSlot329BoolVarDtorRva},
    {"shadow_pathEnableEditing", 0x18, 0x128d706, nullptr, 0, kSlot329EmptyTextRva, 0x32890c8,
     Slot329VarKind::bool_var, 0, 0, 0, 0, kSlot329BoolFinalVptr, kSlot329BoolArgFnVtbl, 0x82a38e,
     kSlot329BoolVarDtorRva},
    // The one row with a non-empty description; the 0x829b30 wrapper is the one the slot calls.
    {"shadow_pathDebugDrawCharaType", 0x1d, 0x128d71f, "Which attract mode to play. Default will play the system selected one.",
     0x46, 0x128d73d, 0x3289190, Slot329VarKind::ranged_int, kSlot329CharaMin, kSlot329CharaMax,
     kSlot329CharaValue, kSlot329CharaValue, kSlot329CharaFinalVptr, kSlot329CharaArgFnVtbl,
     0x82a483, kSlot329CharaDtorRva},
    {"shadow_pathDebugTranslationSpeed", 0x20, 0x128d784, nullptr, 0, kSlot329EmptyTextRva,
     0x3289268, Slot329VarKind::ranged_float, kSlot329TranslationMinBits,
     kSlot329TranslationMaxBits, kSlot329TranslationValueBits, kSlot329TranslationValueBits,
     kSlot329FloatFinalVptr, kSlot329FloatArgFnVtbl, 0x82a59d, kSlot329FloatVarDtorRva},
    {"shadow_pathDebugDisplayAllNames", 0x1f, 0x128d7a5, nullptr, 0, kSlot329EmptyTextRva, 0x3289340,
     Slot329VarKind::bool_var, 0, 0, 0, 0, kSlot329BoolFinalVptr, kSlot329BoolArgFnVtbl, 0x82a684,
     kSlot329BoolVarDtorRva},
    {"shadow_enableSpeculativeAsyncRaycasts", 0x25, 0x128d7c5, nullptr, 0, kSlot329EmptyTextRva,
     0x3289408, Slot329VarKind::bool_var, 0, 0, 1, 1, kSlot329BoolFinalVptr, kSlot329BoolArgFnVtbl,
     0x82a766, kSlot329BoolVarDtorRva},
    {"shadow_debugSpeculativeAsyncRaycasts", 0x24, 0x128d7eb, nullptr, 0, kSlot329EmptyTextRva,
     0x32894d0, Slot329VarKind::bool_var, 0, 0, 0, 0, kSlot329BoolFinalVptr, kSlot329BoolArgFnVtbl,
     0x82a849, kSlot329BoolVarDtorRva},
}};

// ---- the twelve menu items -------------------------------------------------------------------
enum class Slot329ItemKind : std::uint8_t { group, toggle, separator };

struct Slot329ItemSpec {
    Slot329ItemKind kind;
    const char* label;              // view A (+0x28): the parent path
    std::uint32_t label_len;
    std::uint64_t label_rva;
    const char* text;               // view B (+0x50): the caption (null for the separator)
    std::uint32_t text_len;
    std::uint64_t text_rva;
    std::uint64_t object_rva;
    std::uint32_t target_index;     // into kSlot329Vars for a toggle, unused otherwise
    std::uint32_t flags;
    std::uint64_t atexit_site_rva;
};

inline constexpr char kSlot329GroupShadow[] = "Shadow";        // 0x128d810, 6
inline constexpr char kSlot329GroupPath[] = "Shadow Path";     // 0x128d817, 0xb
inline constexpr char kSlot329ToggleDrawEnable[] = "Debug Draw Enable";              // 0x11
inline constexpr char kSlot329ToggleDrawData[] = "Debug Draw Data Enable";           // 0x16
inline constexpr char kSlot329ToggleAttainRange[] = "Debug Draw Attain Range Enable"; // 0x1e
inline constexpr char kSlot329ToggleCharaType[] = "Debug Draw Chara Type";           // 0x15
inline constexpr char kSlot329TogglePathFinding[] = "Enable Path Finding";           // 0x13
inline constexpr char kSlot329ToggleEditing[] = "Enable Editing";                    // 0xe
inline constexpr char kSlot329ToggleTransSpeed[] = "Editing Trans Speed";            // 0x13
inline constexpr char kSlot329ToggleAllNames[] = "Show All Names";                   // 0xe
inline constexpr char kSlot329ToggleRayEnable[] = "Speculative Async Raycasts Enable";  // 0x21
inline constexpr char kSlot329ToggleRayDebug[] = "Speculative Async Raycasts Debug";   // 0x20
static_assert(sizeof kSlot329GroupShadow - 1 == 6);
static_assert(sizeof kSlot329GroupPath - 1 == 0xb);
static_assert(sizeof kSlot329ToggleDrawEnable - 1 == 0x11);
static_assert(sizeof kSlot329ToggleDrawData - 1 == 0x16);
static_assert(sizeof kSlot329ToggleAttainRange - 1 == 0x1e);
static_assert(sizeof kSlot329ToggleCharaType - 1 == 0x15);
static_assert(sizeof kSlot329TogglePathFinding - 1 == 0x13);
static_assert(sizeof kSlot329ToggleEditing - 1 == 0xe);
static_assert(sizeof kSlot329ToggleTransSpeed - 1 == 0x13);
static_assert(sizeof kSlot329ToggleAllNames - 1 == 0xe);
static_assert(sizeof kSlot329ToggleRayEnable - 1 == 0x21);
static_assert(sizeof kSlot329ToggleRayDebug - 1 == 0x20);
static_assert(sizeof kSlot329EmptyText - 1 == 0);

inline constexpr std::array<Slot329ItemSpec, kSlot329ItemCount> kSlot329Items = {{
    // 0x3289598: the page group. parent "Shadow", caption "Shadow Path".
    {Slot329ItemKind::group, kSlot329GroupShadow, 6, 0x128d810, kSlot329GroupPath, 0xb, 0x128d817,
     0x3289598, 0, kSlot329ItemFlags, 0x82a8c9},
    {Slot329ItemKind::toggle, kSlot329GroupPath, 0xb, 0x128d817, kSlot329ToggleDrawEnable, 0x11,
     0x128d823, 0x3289640, 1, kSlot329ItemFlags, 0x82a94d},
    {Slot329ItemKind::toggle, kSlot329GroupPath, 0xb, 0x128d817, kSlot329ToggleDrawData, 0x16,
     0x128d835, 0x32896e8, 2, kSlot329ItemFlags, 0x82a9d5},
    {Slot329ItemKind::toggle, kSlot329GroupPath, 0xb, 0x128d817, kSlot329ToggleAttainRange, 0x1e,
     0x128d84c, 0x3289790, 3, kSlot329ItemFlags, 0x82aa5c},
    {Slot329ItemKind::toggle, kSlot329GroupPath, 0xb, 0x128d817, kSlot329ToggleCharaType, 0x15,
     0x128d86b, 0x3289838, 5, kSlot329ItemFlags, 0x82aae0},
    {Slot329ItemKind::toggle, kSlot329GroupPath, 0xb, 0x128d817, kSlot329TogglePathFinding, 0x13,
     0x128d881, 0x32898e0, 0, kSlot329ItemFlags, 0x82ab67},
    // 0x3289988: the kind-3 "MenuSeparator" row. 0x26b60 takes no caption (`xorl %edx,%edx`).
    {Slot329ItemKind::separator, kSlot329GroupPath, 0xb, 0x128d817, nullptr, 0, 0, 0x3289988, 0,
     kSlot329ItemFlags, 0x82abba},
    {Slot329ItemKind::toggle, kSlot329GroupPath, 0xb, 0x128d817, kSlot329ToggleEditing, 0xe,
     0x128d895, 0x3289a30, 4, kSlot329ItemFlags, 0x82ac41},
    {Slot329ItemKind::toggle, kSlot329GroupPath, 0xb, 0x128d817, kSlot329ToggleTransSpeed, 0x13,
     0x128d8a4, 0x3289ad8, 6, kSlot329ItemFlags, 0x82acc5},
    {Slot329ItemKind::toggle, kSlot329GroupPath, 0xb, 0x128d817, kSlot329ToggleAllNames, 0xe,
     0x128d8b8, 0x3289b80, 7, kSlot329ItemFlags, 0x82ad49},
    {Slot329ItemKind::toggle, kSlot329GroupPath, 0xb, 0x128d817, kSlot329ToggleRayEnable, 0x21,
     0x128d8c7, 0x3289c28, 8, kSlot329ItemFlags, 0x82add0},
    {Slot329ItemKind::toggle, kSlot329GroupPath, 0xb, 0x128d817, kSlot329ToggleRayDebug, 0x20,
     0x128d8e9, 0x3289cd0, 9, kSlot329ItemFlags, 0x82ae54},
}};

// ---- objects -----------------------------------------------------------------------------------
inline constexpr std::uint64_t kSlot329BlockRva = 0x3288d98;
inline constexpr std::size_t kSlot329BlockBytes = 0xfe0;          // 0x3288d98 .. 0x3289d78

struct Slot329Objects {
    ConCmdBoolVar bool_path_finding{};      // +0x000  0x3288d98
    ConCmdBoolVar bool_draw_enable{};       // +0x0c8  0x3288e60
    ConCmdRangedVar ranged_path_data{};     // +0x190  0x3288f28 (class P)
    ConCmdBoolVar bool_attain_range{};      // +0x268  0x3289000
    ConCmdBoolVar bool_enable_editing{};    // +0x330  0x32890c8
    ConCmdRangedVar ranged_chara_type{};    // +0x3f8  0x3289190 (class C)
    ConCmdRangedVar float_translation{};    // +0x4d0  0x3289268 (0xc300)
    ConCmdBoolVar bool_all_names{};         // +0x5a8  0x3289340
    ConCmdBoolVar bool_raycasts_enable{};   // +0x670  0x3289408
    ConCmdBoolVar bool_raycasts_debug{};    // +0x738  0x32894d0
    DebugMenuItem group{};                  // +0x800  0x3289598
    std::array<DebugMenuItem, kSlot329ItemCount - 1> items{};   // +0x8a8  0x3289640 .. 0x3289d78
};
static_assert(offsetof(Slot329Objects, bool_path_finding) == 0x000 &&
              offsetof(Slot329Objects, bool_draw_enable) == 0x0c8 &&
              offsetof(Slot329Objects, ranged_path_data) == 0x190 &&
              offsetof(Slot329Objects, bool_attain_range) == 0x268 &&
              offsetof(Slot329Objects, bool_enable_editing) == 0x330 &&
              offsetof(Slot329Objects, ranged_chara_type) == 0x3f8 &&
              offsetof(Slot329Objects, float_translation) == 0x4d0 &&
              offsetof(Slot329Objects, bool_all_names) == 0x5a8 &&
              offsetof(Slot329Objects, bool_raycasts_enable) == 0x670 &&
              offsetof(Slot329Objects, bool_raycasts_debug) == 0x738 &&
              offsetof(Slot329Objects, group) == 0x800 &&
              offsetof(Slot329Objects, items) == 0x8a8,
              "the original object addresses are reproduced");
static_assert(offsetof(Slot329Objects, items) + sizeof(std::array<DebugMenuItem, kSlot329ItemCount - 1>) ==
                  kSlot329BlockBytes,
              "the block spans 0xfe0 bytes");
static_assert(kSlot329BlockRva + kSlot329BlockBytes == 0x3289d78, "the block ends where the next one starts");

// One `__cxa_atexit` registration of the body, in body order (every site is tool-pinned).
struct Slot329AtexitRow {
    std::uint32_t step;        // 1..22
    std::uint64_t site_rva;    // the `callq 0xf8f390`
    std::uint64_t dtor_rva;    // rdi (identity tag, never run)
    std::uint64_t object_rva;  // rsi (identity only)
};
inline constexpr std::array<Slot329AtexitRow, kSlot329AtexitRowCount> kSlot329AtexitRows = {{
    {1, 0x829fc7, kSlot329BoolVarDtorRva, 0x3288d98},
    {2, 0x82a0a6, kSlot329BoolVarDtorRva, 0x3288e60},
    {3, 0x82a1c1, kSlot329PathDataDtorRva, 0x3288f28},
    {4, 0x82a2ac, kSlot329BoolVarDtorRva, 0x3289000},
    {5, 0x82a38e, kSlot329BoolVarDtorRva, 0x32890c8},
    {6, 0x82a483, kSlot329CharaDtorRva, 0x3289190},
    {7, 0x82a59d, kSlot329FloatVarDtorRva, 0x3289268},
    {8, 0x82a684, kSlot329BoolVarDtorRva, 0x3289340},
    {9, 0x82a766, kSlot329BoolVarDtorRva, 0x3289408},
    {10, 0x82a849, kSlot329BoolVarDtorRva, 0x32894d0},
    {11, 0x82a8c9, kSlot329MenuItemDtorRva, 0x3289598},
    {12, 0x82a94d, kSlot329MenuItemDtorRva, 0x3289640},
    {13, 0x82a9d5, kSlot329MenuItemDtorRva, 0x32896e8},
    {14, 0x82aa5c, kSlot329MenuItemDtorRva, 0x3289790},
    {15, 0x82aae0, kSlot329MenuItemDtorRva, 0x3289838},
    {16, 0x82ab67, kSlot329MenuItemDtorRva, 0x32898e0},
    {17, 0x82abba, kSlot329MenuItemDtorRva, 0x3289988},
    {18, 0x82ac41, kSlot329MenuItemDtorRva, 0x3289a30},
    {19, 0x82acc5, kSlot329MenuItemDtorRva, 0x3289ad8},
    {20, 0x82ad49, kSlot329MenuItemDtorRva, 0x3289b80},
    {21, 0x82add0, kSlot329MenuItemDtorRva, 0x3289c28},
    {22, 0x82ae54, kSlot329MenuItemDtorRva, 0x3289cd0},
}};

// One of the three row-local `callq *0x18(%rax)` sites: (site, argument tag, slot-3 body).
struct Slot329FunctorSite {
    std::uint64_t site_rva;
    std::uint64_t tag_rva;
    std::uint64_t slot3_rva;
    bool slot3_is_bare_ret;      // pinned true; false blocks the fragment
};
inline constexpr std::array<Slot329FunctorSite, kSlot329FunctorSiteCount> kSlot329FunctorSites = {{
    {0x82a186, kSlot329PathDataArgFnVtbl, kSlot329PathDataFnDestroyRva, true},
    {0x82a467, kSlot329CharaArgFnVtbl, kSlot329CharaFnDestroyRva, true},
    {0x82a563, kSlot329FloatArgFnVtbl, kSlot329FloatFnDestroyRva, true},
}};

// ---- stages / results ---------------------------------------------------------------------------
enum class Slot329Stage : std::uint8_t {
    none,
    bool_var_build,
    bool_var_registration,
    bool_var_view_release,
    bool_var_atexit,
    ranged_var_build,
    ranged_var_registration,
    ranged_var_view_release,
    ranged_var_atexit,
    item_ctor,
    item_registration,
    item_view_release,
    item_atexit,
    canary,
};
[[nodiscard]] const char* slot329_stage_name(Slot329Stage stage) noexcept;

struct Slot329Hooks {
    // The allocator pair behind 0x12820 / 0x12af0 that 0x26b60 needs for "MenuSeparator".
    // NOT lifted: `default_slot329_hooks()` installs the documented malloc/free stand-in (the same
    // convention as the registered slot 306 / 316 / 326). Clearing it fails the fragment at
    // `item_ctor` for the separator row.
    SharedStrCtorHooks separator_alloc{};
    // __stack_chk_guard source (0x15fe518). Null => the comparison is not performed and
    // `canary_checked` stays false; a changed guard fails the fragment at stage `canary`.
    const std::uint64_t* stack_guard = nullptr;
};

// The net state of leaf A (0x8298d0) or of the wrapper B + leaf B composition (0x829b30 /
// 0x829cf0). Refuses (writes nothing) when the argument tag is not the row's pinned class tag or the
// value cell is null (the original would fault on the two loads).
[[nodiscard]] bool slot329_ranged_var_state(ConCmdRangedVar& var, const Slot329RangedClass& cls,
                                            const SharedStr& name, void* value_ref,
                                            std::uint32_t min_cell, std::uint32_t max_cell,
                                            std::uint32_t flags, const RangedVarArgs& args,
                                            const SharedStr& description) noexcept;

// The net state of 0xc300 for the one float row. Refuses (writes nothing) when the argument tag is
// not the site's pinned 0x15ceb80. The four slot bodies of 0x15ceb80 are byte-identical to the
// accepted 0x15d57e0 / 0x1594318 / 0x15d2040 families, so the shared 0xc300 model is called with
// the equivalent tag; the object's own functor tag is the class tag 0x1594318 either way.
[[nodiscard]] bool slot329_float_var_state(ConCmdRangedVar& var, const SharedStr& name,
                                           void* value_ref, std::uint32_t min_bits,
                                           std::uint32_t max_bits, std::uint32_t flags,
                                           const RangedVarArgs& args,
                                           const SharedStr& description) noexcept;

// The production hooks: the malloc-backed "MenuSeparator" allocator.
[[nodiscard]] Slot329Hooks default_slot329_hooks() noexcept;

// The 0x26ea0 gate answer of a target object, read from its own vptr tag (never a hook). Knows the
// four classes this body targets: bool {1, 0}, float {1, 3}, class P {1, 1} and class C {1, 1}.
// Any other tag is unknown and blocks the fragment.
[[nodiscard]] DebugMenuTarget slot329_menu_target(const ConCmdBase& cmd) noexcept;

// The lifted `callq *0x18(%rax)` for the three row-local sites: a bare `ret` has no effect.
[[nodiscard]] bool slot329_inert_functor_destroy(const Slot329FunctorSite& site) noexcept;

// The stand-ins the aggregate holds, by index.
[[nodiscard]] ConCmdBase& slot329_var_base(Slot329Objects& objects, std::uint32_t index) noexcept;
[[nodiscard]] DebugMenuItem& slot329_item(Slot329Objects& objects, std::uint32_t index) noexcept;

struct Slot329VarResult {
    bool ok = false;
    bool description_inert = false;  // the 0xf590 on the stack description view
    bool name_inert = false;         // the 0xf590 on the stack name view
    bool functor_inert = false;      // the row-local `[tag+0x18]` is the pinned bare ret
    bool atexit_recorded = false;
    std::uint32_t functor_dispatches = 0;
    ConCmdRegisterResult registration{};
};

struct Slot329Result {
    bool ok = false;
    Slot329Stage blocked = Slot329Stage::none;
    std::uint32_t blocked_index = 0;               // the row index for the per-object stages
    std::uint32_t atexit_records = 0;              // this body's 22 __cxa_atexit registrations
    std::uint32_t functor_dispatches = 0;          // the collapsed range-site dispatches
    std::uint32_t functors_destroyed = 0;          // the three `callq *0x18(%rax)` sites
    std::uint32_t vars_built = 0;                  // 7 bools + 2 ranges + 1 float
    std::uint32_t menu_items = 0;                  // 12
    std::array<Slot329VarResult, kSlot329VarCount> vars{};
    std::array<MenuRegisterResult, kSlot329ItemCount> item_registrations{};
    bool views_inert = true;                       // every 0xf590 call of this body was a no-op
    bool canary_checked = false;                   // false when the host supplied no guard
    bool canary_matched = false;
};

// Runs original constructor 329 natively. The original is unconditional, so the first failing step
// stops the slot (nothing after it is claimed); every failure names its stage and row index.
[[nodiscard]] Slot329Result init_slot329_829e90(Slot329Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu,
                                                const Slot329Hooks& hooks = {}) noexcept;

}  // namespace sotc
