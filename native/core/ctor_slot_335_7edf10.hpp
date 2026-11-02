// CPU-01-STARTUP-SLOT-335-CHARACTERIZATION-01: native recovery of original constructor-table index
// 335 / RVA 0x7edf10 (2,725 B FDE [0x7edf10, 0x7ee9b5); 2,724 B reachable over 485 instructions,
// the last FDE byte 0x7ee9b4 being the trailing nop), ELF sha256 d0723afb...5861d3.
// Offline analysis only: nothing here loads, relocates or executes an original RVA. Byte oracle:
// tools/lift/check_ctor_slot_335.py.
//
// ============================ characterization (from the pinned bytes) ============================
// Reached by DATA only: the startup table word at 0x2dd70e0 - 335*8 = **0x2dd6668** holds 0x7edf10.
// Table neighbours: entry 334 = 0x7fc530 (0x2dd6670) and entry 336 = 0x7ec4c0 (0x2dd6660), both
// already registered. Both agree with the backwards arithmetic 0x2dd70e0 - 8*i.
//
// Body shape from the pinned listing: 485 reachable instructions, **58 direct call sites over 11
// targets**, 6 indirect call sites (every one `callq *0x18(%rax)`), 77 rip-relative operands (44
// distinct targets; **none of them a store**), and **exactly one conditional branch** -- 0x7ee99e
// `jne 0x7ee9af`, the __stack_chk_guard compare -- plus the __stack_chk_fail tail at 0x7ee9af. So
// the body is **two basic blocks**: the straight-line constructor run [0x7edf10, 0x7ee99d] (one
// `retq` at 0x7ee9ae) and the canary-failure tail. The frame:
//     push rbp; mov rsp,rbp; push r15/r14/r13/r12/rbx; and rsp,-0x20; sub rsp,0x100
//     guard <- *[0x15fe518]  (the GOT cell holds the runtime address of __stack_chk_guard)
//     ... 480 instructions of constructor work ...
//     cmp [rsp+0xe0],%rax ; jne 0x7ee9af ; lea -0x28(%rbp),%rsp; pops; retq
// Every object of the block is loaded into a register first (`leaq <obj>,%rbx`), which is why this
// body has **zero** rip-relative stores: every store goes through %rbx / %rsp, and the object
// addresses are pinned as rip LEA operands instead.
//
// Callee histogram (every one of the 58 sites is pinned by the checker):
//   0xf590    x22  shared release of a borrowed stack view (every site is an exact no-op)
//   0xf8f390  x11  __cxa_atexit PLT (third argument 0x1604218 = __dso_handle, one per object)
//   0x20d60   x7   register-once guard (4 float variables, 3 bool variables; the two bounded
//                  variables are registered inside 0x7edd80 instead)
//   0x6aa00   x4   UNBOUNDED float variable ctor (native: init_unbounded_float_var_6aa00 shape)
//   0x20af0   x3   ConCmdBase ctor (the three bool variables)
//   0x52a990  x2   FNV-1a 32 of a NUL-terminated name (tail jump into 0x161fb0)
//   0x7dbca0  x2   `&value_array[index]`, the array at 0x325f410 (15 B: movslq/lea/lea/ret)
//   0xf2f0    x2   SharedStr from a NUL-terminated C string: {ptr, strlen, 0, 0} (40 B)
//   0x7edd80  x2   bounded float variable through 0xc300 + final vptr + default = *cell + 0x20d60
//   0x27050   x2   debug-menu slider item ctor (0x30 flags, explicit float range)
//   0xf8f3a0  x1   __stack_chk_fail (the branch target only)
//
// The 6 indirect sites (0x7ee008 / 0x7ee101 / 0x7ee1f3 / 0x7ee2ef / 0x7ee722 / 0x7ee8c8) are
// `callq *0x18(%rax)` with %rax = **0x15ce630**, the float argument-functor table of the four 0x6aa00
// sites (slot 0 0xc4a0 copy / slot 1 0xc4c0 clone / slot 2 0xc4e0 move / slot 3 0xc4f0 **bare ret**).
// The two `0x7edd80` sites reuse that same tag, so all six dispatches are inert functor destroys of a
// stack temporary. The three bool sites use table **0x15ce650** = {0xc2a0, 0xc2c0, 0xc2e0, 0xc2f0},
// whose slot 3 0xc2f0 is also a bare `ret`; those dispatches happen inside 0x20af0's inline shape.
//
// What the block IS. It is the **"Shadow Cheat" assist / cheat debug page**: four Easy-Mode float
// multipliers, three cheat bools, two name-hash records for the health/grip bars, and two bounded
// float sliders under the "Shadow Cheat" group. The strings are the evidence:
//   names    shadow_EasyMode_playerDamageMult (0x128ae40, 0x20),
//            shadow_EasyMode_playerHealMult (0x128ae61, 0x1e),
//            shadow_EasyMode_gripLossMult (0x128ae80, 0x1c),
//            shadow_EasyMode_gripGainMult (0x128ae9d, 0x1c),
//            shadow_noDamage (0x128aeba, 0xf), shadow_maxAttackDamage (0x128aeca, 0x16),
//            shadow_printPlayerNowPower (0x128aee1, 0x1a),
//            Max_HP_Value (0x128af20, 0xc), Max_Grip_Value (0x128af48, 0xe)
//   hashed   WanderHealthBarMax (0x128aefc), WanderGripBarMax (0x128af0f) -- NUL-terminated, hashed
//            with FNV-1a 32 basis 0x811c9dc5 and stored as `{name, hash}` records
//   group    "Shadow Cheat" (0x128af2e, 0xc) -- the parent of BOTH sliders
//   captions "Max HP Value" (0x128af3b, 0xc) and "Max_Grip_Value" (0x128af48, 0xe) -- note the two
//            spellings differ in the image and are reproduced verbatim
//   empty    the description view points at 0x128af2d, the second of two consecutive NULs
//   floats   100.0 / 570.0 (HP slider bounds and the HP variable's min/max),
//            100.0 / 700.0 (grip variable's min/max), 10.0 / 100.0 (HP slider display range),
//            1.0 / 100.0 (grip slider display range)
// The `0x52a990` hash keys (`WanderHealthBarMax` / `WanderGripBarMax`) name the game-side cells the
// two bounded variables drive; the value cells themselves are `0x325f410[11]` and `[12]`. The
// reading "these two sliders set the player's health/grip bar maxima" is INFERENCE from those names;
// the bytes and the indices are the fact.
//
// The two callback bodies 0x7ed280 (70 B) and 0x7ed2d0 (99 B) are stored -- never called -- as
// qword 3 of the two 0x7edd80 argument blocks, next to qword 1 = the member-function thunk 0x7edd60
// (20 B: `jmpq *0x18(%rdi)` with the four argument registers shifted). They clamp the incoming float
// (0x7ed280: `min(v, 570.0)`; 0x7ed2d0: `clamp(v, 100.0, 700.0)`) and forward it to 0x572900, a
// sorted-vector lookup keyed by the hash record. Nothing in this body executes either of them, so the
// model keeps them as identity tags and the tables stay pinned, not run.
//
// ================================ object map (verified strides) ================================
// The block is one contiguous .bss run [0x32656c0, 0x3265f98) = 0x8d8 bytes; the native aggregate
// reproduces every address (static_assert below):
//   0x32656c0  unbounded float shadow_EasyMode_playerDamageMult   0xd8  value=default=1.0
//   0x3265798  unbounded float shadow_EasyMode_playerHealMult     0xd8  value=default=1.2
//   0x3265870  unbounded float shadow_EasyMode_gripLossMult       0xd8  value=default=0.75
//   0x3265948  unbounded float shadow_EasyMode_gripGainMult       0xd8  value=default=1.2
//   0x3265a20  bool shadow_noDamage                               0xc8  value=default=0
//   0x3265ae8  bool shadow_maxAttackDamage                        0xc8  value=default=0
//   0x3265bb0  bool shadow_printPlayerNowPower                    0xc8  value=default=0
//   0x3265c80  {name, hash} record for "WanderHealthBarMax"       0x18 (16 used, 8 pad)
//   0x3265c98  {name, hash} record for "WanderGripBarMax"         0x10
//   0x3265ca8  bounded float Max_HP_Value                         **0xd0**  [100, 570]
//   0x3265d78  slider item "Shadow Cheat" / "Max HP Value"        0xa8  range {10, 100}
//   0x3265e20  bounded float Max_Grip_Value                       **0xd0**  [100, 700]
//   0x3265ef0  slider item "Shadow Cheat" / "Max_Grip_Value"      0xa8  range {1, 100}
//   0x3265f98  end of the block (the next object, outside this slot)
// The two bounded variables are the second place (after slot 326's eye colours) where the natural
// library type does not fit: 0xc300 never writes its own +0xd0 dword and 0x7edd80 does not either, so
// the original lays these objects **0xd0** apart and each object's +0xd0 is the NEXT object's vptr.
// `Slot335BoundedVar` is therefore declared as the 0xd0 prefix of the c300 shape, exactly like
// `Slot326EyeVar`; using the 0xd8 `ConCmdRangedVar` here would be 8 B too wide per object and would
// silently shift both sliders.
//
// ================================ modelled vs hook (G2) =================================
// MODELLED (native, exact):
//   * the frame, the canary guard and the epilogue;
//   * the four unbounded floats: 0x6aa00's net state (`slot335_unbounded_float_6aa00`), then the
//     site's final vptr 0x15942b0, value into +0xd0, default into +0x68, 0x20d60, both 0xf590
//     releases and the 0x20f0 atexit row;
//   * the three bools through the shared `init_bool_var_static` (class 0x1594218, argument tag
//     0x15ce650, value 0 / default 0, its own two releases, 0x20d60 and 0x20b0 atexit);
//   * the two FNV-1a 32 name hashes (`slot335_fnv1a32`, the 0x52a990 + 0x161fb0 pair) into the two
//     `{name, hash}` records;
//   * the two bounded floats through `slot335_bounded_var_state` (the 0xc300 net state) and
//     `slot335_bounded_var_7edd80` (the 385 B helper: it calls 0xc300 with flags 1, overwrites the
//     vptr with 0x15943c0, stores `default = *value_cell`, and registers through 0x20d60);
//   * the two sliders through `init_debug_menu_item_27050` with their pinned display ranges, the
//     0x268b0 registration, the text-then-label release order and the 0x272d0 atexit row;
//   * every 0xf590 release and every __cxa_atexit record, in body order.
//
// HOOK: **none is required.** Unlike slots 306 / 316 / 326 / 329 this block builds no SharedStr that
// owns storage: `0xf2f0` (modelled here) only measures the literal with strlen and stores
// {ptr, len, 0, 0}, so there is no allocator contract to stand in for. The only host input is
// `Slot335Hooks::stack_guard`, the __stack_chk_guard source: without it the guard compare is not
// claimed (`canary_checked` stays false) and a changed guard fails the fragment at `canary`.
//
// Fail-visible contracts (all reported through `blocked` / `ok == false`, never a silent pass):
//   * a float/bounded variable spec whose argument tag is not 0x15ce630 (an unlifted functor would
//     run after most of the stores, so NOTHING is written and the stage fails);
//   * a null value cell, or a value-cell index outside the mirrored 0x325f410 window;
//   * a null or empty hashed name (0x52a990 would fault / 0x161fb0 would return the basis);
//   * a hashed name that is not the pinned row for its index;
//   * a failed 0x20d60 registration (both the direct sites and the one inside 0x7edd80);
//   * a reached 0x268b0 assert/gate site (only possible if a spec's flags changed);
//   * a failed __cxa_atexit record;
//   * a `__stack_chk_guard` that changed across the body.
//
// NOT modelled: every destructor body (0x20f0 float, 0x20b0 bool, 0xad30 bounded-float, 0x272d0
// item, 0x27280 root) and the two change callbacks 0x7ed280 / 0x7ed2d0 with their 0x7edd60 thunk,
// 0x7dbc30 store body and 0x572900 lookup. The functor slot bodies 0xc4a0 (copy) / 0xc4c0 (clone) /
// 0xc4e0 (move) are byte-pinned and their net effect is reproduced by the collapsed install; 0xc4f0
// and 0xc2f0 (destroy) are a bare `ret`. None of them is executed. No cheat, health, grip or gameplay
// behaviour is claimed, and nothing here reads or writes the real 0x325f410 array: the model works on
// a host-owned mirror of the two cells the body addresses.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"
#include "fmenu_ctors_slot306.hpp"
#include "l2_slot_common.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot335Rva = 0x7edf10;
inline constexpr std::size_t kSlot335BodySize = 2725;          // FDE [0x7edf10, 0x7ee9b5)
inline constexpr std::uint64_t kSlot335EndRva = 0x7ee9b5;
inline constexpr std::size_t kSlot335ReachableBytes = 2724;    // the last reached byte is the trailing nop
inline constexpr std::size_t kSlot335InstructionCount = 485;
inline constexpr std::size_t kSlot335DirectCallSites = 58;
inline constexpr std::size_t kSlot335DirectCallees = 11;
inline constexpr std::size_t kSlot335IndirectCallSites = 6;
inline constexpr std::size_t kSlot335RipOperands = 77;
inline constexpr std::size_t kSlot335RipTargets = 44;
inline constexpr std::size_t kSlot335RipStoreSites = 0;        // every store is register-based
inline constexpr std::size_t kSlot335BranchCount = 1;
inline constexpr std::size_t kSlot335BasicBlocks = 2;
inline constexpr std::uint64_t kSlot335TableTopRva = 0x2dd70e0;   // index i at TOP - 8*i
inline constexpr std::uint64_t kSlot335TableWordRva = 0x2dd6668;  // 0x2dd70e0 - 335*8
inline constexpr std::uint32_t kSlot335TableIndex = 335;
// The next index that is still unrecovered once this one is registered: 334 was already registered
// and 335 was the cliff, so the cliff moves to index 336.
inline constexpr std::uint32_t kSlot335NextUnrecoveredIndex = 336;
inline constexpr std::uint64_t kSlot335NextUnrecoveredRva = 0x7ec4c0;

// Frame / guard.
inline constexpr std::uint64_t kSlot335CanaryCellRva = 0x15fe518;   // GOT cell holding &__stack_chk_guard
inline constexpr std::uint32_t kSlot335CanaryFrameOffset = 0xe0;    // [rsp+0xe0]
inline constexpr std::uint64_t kSlot335StackChkFailRva = 0xf8f3a0;
inline constexpr std::uint64_t kSlot335BranchRva = 0x7ee99e;        // jne 0x7ee9af (the guard compare)
inline constexpr std::uint64_t kSlot335BranchTargetRva = 0x7ee9af;
inline constexpr std::uint64_t kSlot335EpilogueRva = 0x7ee9a0;      // lea -0x28(%rbp),%rsp
inline constexpr std::uint32_t kSlot335FrameBytes = 0x100;          // sub rsp,0x100 (after and rsp,-0x20)

// Shared callees.
inline constexpr std::uint64_t kSlot335AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot335DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot335ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot335BaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot335RegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot335UnboundedCtorRva = 0x6aa00;
inline constexpr std::size_t kSlot335UnboundedCtorSize = 323;      // FDE [0x6aa00, 0x6ab43)
inline constexpr std::uint64_t kSlot335BoundedCtorRva = 0xc300;
inline constexpr std::size_t kSlot335BoundedCtorSize = 352;
inline constexpr std::uint64_t kSlot335SliderCtorRva = 0x27050;
inline constexpr std::size_t kSlot335SliderCtorSize = 208;
inline constexpr std::uint64_t kSlot335MenuRegisterRva = 0x268b0;
inline constexpr std::uint64_t kSlot335HashRva = 0x52a990;          // FNV-1a 32 tail-jump ...
inline constexpr std::size_t kSlot335HashSize = 10;
inline constexpr std::uint64_t kSlot335HashBodyRva = 0x161fb0;      // ... into this body
inline constexpr std::uint32_t kSlot335FnvBasis = 0x811c9dc5;
inline constexpr std::uint32_t kSlot335FnvPrime = 0x01000193;
inline constexpr std::uint64_t kSlot335ValueCellRva = 0x7dbca0;     // `&value_array[index]`
inline constexpr std::size_t kSlot335ValueCellSize = 15;
inline constexpr std::uint64_t kSlot335ValueArrayRva = 0x325f410;   // the .bss array 0x7dbca0 indexes
inline constexpr std::uint64_t kSlot335BorrowedStrRva = 0xf2f0;     // {ptr, strlen, 0, 0}
inline constexpr std::size_t kSlot335BorrowedStrSize = 40;
inline constexpr std::uint64_t kSlot335StrlenRva = 0xf8f3b0;        // the strlen PLT 0xf2f0 calls
inline constexpr std::uint64_t kSlot335BoundedHelperRva = 0x7edd80;
inline constexpr std::size_t kSlot335BoundedHelperSize = 386;       // FDE; 385 B of code + nop
inline constexpr std::uint64_t kSlot335CallbackThunkRva = 0x7edd60; // `jmpq *0x18(%rdi)` (20 B)
inline constexpr std::uint64_t kSlot335CallbackHpRva = 0x7ed280;    // 70 B, stored only
inline constexpr std::uint64_t kSlot335CallbackGripRva = 0x7ed2d0;  // 99 B, stored only
inline constexpr std::uint64_t kSlot335CallbackStoreRva = 0x7dbc30; // 15 B, called by the callbacks
inline constexpr std::uint64_t kSlot335CallbackLookupRva = 0x572900;// the sorted-vector lookup

// Destructors (identity tags recorded with __cxa_atexit, never executed).
inline constexpr std::uint64_t kSlot335FloatVarDtorRva = 0x20f0;
inline constexpr std::uint64_t kSlot335BoolVarDtorRva = 0x20b0;
inline constexpr std::uint64_t kSlot335BoundedVarDtorRva = 0xad30;
inline constexpr std::uint64_t kSlot335MenuItemDtorRva = 0x272d0;

// Identity tags (original vtable address points; never dispatched through here).
inline constexpr std::uint64_t kSlot335BoolMidVptr = 0x15940b0;      // 0x15940a0 + 0x10
inline constexpr std::uint64_t kSlot335BoolFinalVptr = 0x1594218;    // 0x1594208 + 0x10
inline constexpr std::uint64_t kSlot335FloatMidVptr = 0x1594128;     // 0x1594118 + 0x10
inline constexpr std::uint64_t kSlot335FloatFinalVptr = 0x15942b0;   // 0x15942a0 + 0x10
inline constexpr std::uint64_t kSlot335BoundedFinalVptr = 0x15943c0; // 0x15943b0 + 0x10 (0x7edd80 stores it)
inline constexpr std::uint64_t kSlot335FloatClassFnVtbl = 0x1594318; // the class functor the dance installs
inline constexpr std::uint64_t kSlot335FloatArgFnVtbl = 0x15ce630;   // the six float/bounded sites' argument tag
inline constexpr std::uint64_t kSlot335BoolArgFnVtbl = 0x15ce650;    // the three bool sites' argument tag
inline constexpr std::uint64_t kSlot335FloatFnCopyRva = 0xc4a0;      // slot 0 of both float tables
inline constexpr std::uint64_t kSlot335FloatFnCloneRva = 0xc4c0;     // slot 1
inline constexpr std::uint64_t kSlot335FloatFnMoveRva = 0xc4e0;      // slot 2
inline constexpr std::uint64_t kSlot335FloatFnDestroyRva = 0xc4f0;   // slot 3: one `retq`
inline constexpr std::uint64_t kSlot335BoolFnCopyRva = 0xc2a0;       // slot 0 of the bool table
inline constexpr std::uint64_t kSlot335BoolFnCloneRva = 0xc2c0;      // slot 1
inline constexpr std::uint64_t kSlot335BoolFnMoveRva = 0xc2e0;       // slot 2
inline constexpr std::uint64_t kSlot335BoolFnDestroyRva = 0xc2f0;    // slot 3: one `retq`

// Per-site constants.
inline constexpr std::uint32_t kSlot335VarFlags = 1;        // ecx / r9d at every variable site
inline constexpr std::uint32_t kSlot335ItemFlags = 0x30;    // r8d at both slider sites
inline constexpr std::uint8_t kSlot335BoolValue = 0;        // byte [obj+0xc0] at all three bools
inline constexpr std::uint8_t kSlot335BoolDefault = 0;      // byte [obj+0x68]
inline constexpr std::uint32_t kSlot335HpCellIndex = 11;    // 0x325f410 + 4*11 = 0x325f43c
inline constexpr std::uint32_t kSlot335GripCellIndex = 12;  // 0x325f410 + 4*12 = 0x325f440
inline constexpr std::uint32_t kSlot335ValueArrayWindow = 16;  // the mirrored window of 0x325f410
// The bounded variables' bounds: the sites materialise them as IMMEDIATE dword stores into two stack
// cells (0x58(%rsp) = max, 0x5c(%rsp) = min) and pass those cells by address to 0x7edd80.
inline constexpr std::uint32_t kSlot335HpMinBits = 0x42c80000u;    // 100.0f
inline constexpr std::uint32_t kSlot335HpMaxBits = 0x440e8000u;    // 570.0f
inline constexpr std::uint32_t kSlot335GripMinBits = 0x42c80000u;  // 100.0f
inline constexpr std::uint32_t kSlot335GripMaxBits = 0x442f0000u;  // 700.0f
inline constexpr float kSlot335HpMin = 100.0f;
inline constexpr float kSlot335HpMax = 570.0f;
inline constexpr float kSlot335GripMin = 100.0f;
inline constexpr float kSlot335GripMax = 700.0f;
// The two sliders' DISPLAY ranges (xmm0 = min, xmm1 = max at 0x27050).
inline constexpr std::uint64_t kSlot335HpRangeMinRva = 0x128ade0;    // 10.0f
inline constexpr std::uint64_t kSlot335HpRangeMaxRva = 0x128ade4;    // 100.0f
inline constexpr std::uint64_t kSlot335GripRangeMinRva = 0x128ade8;  // 1.0f
inline constexpr float kSlot335HpRangeMin = 10.0f;
inline constexpr float kSlot335HpRangeMax = 100.0f;
inline constexpr float kSlot335GripRangeMin = 1.0f;
inline constexpr float kSlot335GripRangeMax = 100.0f;
inline constexpr std::uint32_t kSlot335DamageMultBits = 0x3f800000u;  // 1.0f
inline constexpr std::uint32_t kSlot335HealMultBits = 0x3f99999au;    // 1.2f
inline constexpr std::uint32_t kSlot335GripLossBits = 0x3f400000u;    // 0.75f
inline constexpr std::uint32_t kSlot335GripGainBits = 0x3f99999au;    // 1.2f

// Strings of the block: identity address + pinned length (the native views are literals / measured).
inline constexpr char kSlot335DamageMultName[] = "shadow_EasyMode_playerDamageMult";   // 0x128ae40, 0x20
inline constexpr char kSlot335HealMultName[] = "shadow_EasyMode_playerHealMult";       // 0x128ae61, 0x1e
inline constexpr char kSlot335GripLossName[] = "shadow_EasyMode_gripLossMult";         // 0x128ae80, 0x1c
inline constexpr char kSlot335GripGainName[] = "shadow_EasyMode_gripGainMult";         // 0x128ae9d, 0x1c
inline constexpr char kSlot335NoDamageName[] = "shadow_noDamage";                      // 0x128aeba, 0xf
inline constexpr char kSlot335MaxAttackName[] = "shadow_maxAttackDamage";              // 0x128aeca, 0x16
inline constexpr char kSlot335PrintPowerName[] = "shadow_printPlayerNowPower";         // 0x128aee1, 0x1a
inline constexpr char kSlot335HealthHashName[] = "WanderHealthBarMax";                 // 0x128aefc (NUL-terminated)
inline constexpr char kSlot335GripHashName[] = "WanderGripBarMax";                     // 0x128af0f (NUL-terminated)
inline constexpr char kSlot335HpVarName[] = "Max_HP_Value";                            // 0x128af20, 0xc
inline constexpr char kSlot335GripVarName[] = "Max_Grip_Value";                        // 0x128af48, 0xe
inline constexpr char kSlot335GroupName[] = "Shadow Cheat";                            // 0x128af2e, 0xc
inline constexpr char kSlot335HpCaption[] = "Max HP Value";                            // 0x128af3b, 0xc
inline constexpr char kSlot335EmptyText[] = "";                                        // 0x128af2d, first byte 0
inline constexpr std::uint64_t kSlot335EmptyTextRva = 0x128af2d;
static_assert(sizeof kSlot335DamageMultName - 1 == 0x20);
static_assert(sizeof kSlot335HealMultName - 1 == 0x1e);
static_assert(sizeof kSlot335GripLossName - 1 == 0x1c);
static_assert(sizeof kSlot335GripGainName - 1 == 0x1c);
static_assert(sizeof kSlot335NoDamageName - 1 == 0xf);
static_assert(sizeof kSlot335MaxAttackName - 1 == 0x16);
static_assert(sizeof kSlot335PrintPowerName - 1 == 0x1a);
static_assert(sizeof kSlot335HpVarName - 1 == 0xc);
static_assert(sizeof kSlot335GripVarName - 1 == 0xe);
static_assert(sizeof kSlot335GroupName - 1 == 0xc);
static_assert(sizeof kSlot335HpCaption - 1 == 0xc);
static_assert(sizeof kSlot335HealthHashName - 1 == 18);
static_assert(sizeof kSlot335GripHashName - 1 == 16);
static_assert(sizeof kSlot335EmptyText - 1 == 0);

inline constexpr std::size_t kSlot335UnboundedCount = 4;
inline constexpr std::size_t kSlot335BoolCount = 3;
inline constexpr std::size_t kSlot335HashCount = 2;
inline constexpr std::size_t kSlot335BoundedCount = 2;
inline constexpr std::size_t kSlot335VarCount = 9;          // 4 unbounded + 3 bool + 2 bounded
inline constexpr std::size_t kSlot335ItemCount = 2;
inline constexpr std::size_t kSlot335AtexitRowCount = 11;
inline constexpr std::size_t kSlot335FunctorSiteCount = 6;
inline constexpr std::size_t kSlot335ReleaseCount = 22;

// ---- the 0xd0 bounded-variable shape ------------------------------------------------------------
// The c300 shape truncated at +0xd0: every field 0xc300 writes is here at the library type's offset,
// and the library type's +0xd0 `value` dword is deliberately absent because in the original layout
// that dword is the NEXT object's vptr (see the object map).
struct Slot335BoundedVar {
    ConCmdBase base;               // +0x00
    void* value_ref;               // +0x60
    std::uint32_t default_value;   // +0x68
    std::uint8_t min_value[4];     // +0x6c
    std::uint8_t min_engaged;      // +0x70
    std::uint8_t gap71[3];         // +0x71 never written
    std::uint8_t max_value[4];     // +0x74
    std::uint8_t max_engaged;      // +0x78
    std::uint8_t gap79[7];         // +0x79 never written
    std::uint64_t args[4];         // +0x80 the caller's argument qwords
    std::uint8_t fn_inline[0x10];  // +0xa0 never written
    std::uint64_t fn_payload;      // +0xb0
    std::uint64_t gapb8;           // +0xb8 never written
    std::uint64_t fn_vptr;         // +0xc0
    std::uint64_t fn_aux;          // +0xc8
};
static_assert(sizeof(Slot335BoundedVar) == 0xd0, "the bounded objects are 0xd0 apart");
static_assert(offsetof(Slot335BoundedVar, value_ref) == 0x60 &&
              offsetof(Slot335BoundedVar, default_value) == 0x68 &&
              offsetof(Slot335BoundedVar, min_value) == 0x6c &&
              offsetof(Slot335BoundedVar, min_engaged) == 0x70 &&
              offsetof(Slot335BoundedVar, max_value) == 0x74 &&
              offsetof(Slot335BoundedVar, max_engaged) == 0x78 &&
              offsetof(Slot335BoundedVar, args) == 0x80 &&
              offsetof(Slot335BoundedVar, fn_inline) == 0xa0 &&
              offsetof(Slot335BoundedVar, fn_payload) == 0xb0 &&
              offsetof(Slot335BoundedVar, fn_vptr) == 0xc0 &&
              offsetof(Slot335BoundedVar, fn_aux) == 0xc8);

// ---- the {name, hash} record (0x3265c80 / 0x3265c98) ---------------------------------------------
struct Slot335HashRecord {
    const char* name;         // +0x00
    std::uint32_t hash;       // +0x08  FNV-1a 32 of `name` (0x52a990)
    std::uint32_t pad0c;      // +0x0c never written
};
static_assert(sizeof(Slot335HashRecord) == 0x10);

// ---- per-row specs ------------------------------------------------------------------------------
struct Slot335UnboundedSpec {
    std::uint32_t index;
    const char* name;
    std::uint32_t name_len;
    std::uint64_t name_rva;
    std::uint64_t object_rva;
    std::uint32_t value_bits;     // stored into +0xd0 and +0x68
    std::uint64_t ctor_site_rva;
    std::uint64_t atexit_site_rva;
    std::uint64_t functor_site_rva;
};
inline constexpr std::array<Slot335UnboundedSpec, kSlot335UnboundedCount> kSlot335Unbounded = {{
    {0, kSlot335DamageMultName, 0x20, 0x128ae40, 0x32656c0, kSlot335DamageMultBits, 0x7edff0, 0x7ee040, 0x7ee008},
    {1, kSlot335HealMultName, 0x1e, 0x128ae61, 0x3265798, kSlot335HealMultBits, 0x7ee0e4, 0x7ee139, 0x7ee101},
    {2, kSlot335GripLossName, 0x1c, 0x128ae80, 0x3265870, kSlot335GripLossBits, 0x7ee1d6, 0x7ee22b, 0x7ee1f3},
    {3, kSlot335GripGainName, 0x1c, 0x128ae9d, 0x3265948, kSlot335GripGainBits, 0x7ee2d7, 0x7ee32a, 0x7ee2ef},
}};

struct Slot335BoolSpec {
    std::uint32_t index;
    const char* name;
    std::uint32_t name_len;
    std::uint64_t name_rva;
    std::uint64_t object_rva;
    std::uint64_t ctor_site_rva;
    std::uint64_t atexit_site_rva;
};
inline constexpr std::array<Slot335BoolSpec, kSlot335BoolCount> kSlot335Bools = {{
    {0, kSlot335NoDamageName, 0x0f, 0x128aeba, 0x3265a20, 0x7ee3a2, 0x7ee42b},
    {1, kSlot335MaxAttackName, 0x16, 0x128aeca, 0x3265ae8, 0x7ee498, 0x7ee516},
    {2, kSlot335PrintPowerName, 0x1a, 0x128aee1, 0x3265bb0, 0x7ee583, 0x7ee601},
}};

struct Slot335HashSpec {
    std::uint32_t index;
    const char* name;
    std::uint64_t name_rva;
    std::uint64_t record_rva;
    std::uint64_t call_site_rva;
};
inline constexpr std::array<Slot335HashSpec, kSlot335HashCount> kSlot335Hashes = {{
    {0, kSlot335HealthHashName, 0x128aefc, 0x3265c80, 0x7ee617},
    {1, kSlot335GripHashName, 0x128af0f, 0x3265c98, 0x7ee630},
}};

struct Slot335BoundedSpec {
    std::uint32_t index;
    const char* name;
    std::uint32_t name_len;
    std::uint64_t name_rva;
    std::uint64_t object_rva;
    std::uint32_t cell_index;      // into the mirrored 0x325f410
    std::uint64_t cell_rva;
    float min_value;
    float max_value;
    std::uint32_t min_bits;        // the immediate the site stores into 0x5c(%rsp)
    std::uint32_t max_bits;        // the immediate the site stores into 0x58(%rsp)
    std::uint64_t call_site_rva;   // the 0x7edd80 call
    std::uint64_t atexit_site_rva;
    std::uint64_t functor_site_rva;
    std::uint64_t callback_rva;    // stored as args.q[3], never called
};
inline constexpr std::array<Slot335BoundedSpec, kSlot335BoundedCount> kSlot335Bounded = {{
    {0, kSlot335HpVarName, 0x0c, 0x128af20, 0x3265ca8, kSlot335HpCellIndex, 0x325f43c,
     kSlot335HpMin, kSlot335HpMax, kSlot335HpMinBits, kSlot335HpMaxBits, 0x7ee705, 0x7ee741,
     0x7ee722, kSlot335CallbackHpRva},
    {1, kSlot335GripVarName, 0x0e, 0x128af48, 0x3265e20, kSlot335GripCellIndex, 0x325f440,
     kSlot335GripMin, kSlot335GripMax, kSlot335GripMinBits, kSlot335GripMaxBits, 0x7ee8ab,
     0x7ee8e7, 0x7ee8c8, kSlot335CallbackGripRva},
}};

struct Slot335ItemSpec {
    std::uint32_t index;
    const char* label;            // view A (+0x28): the parent path ("Shadow Cheat")
    std::uint32_t label_len;
    std::uint64_t label_rva;
    const char* text;             // view B (+0x50): the caption
    std::uint32_t text_len;
    std::uint64_t text_rva;
    std::uint64_t object_rva;
    std::uint32_t target_bounded; // into kSlot335Bounded
    std::uint32_t flags;
    float range_min;
    float range_max;
    std::uint64_t ctor_site_rva;
    std::uint64_t atexit_site_rva;
};
inline constexpr std::array<Slot335ItemSpec, kSlot335ItemCount> kSlot335Items = {{
    {0, kSlot335GroupName, 0x0c, 0x128af2e, kSlot335HpCaption, 0x0c, 0x128af3b, 0x3265d78, 0,
     kSlot335ItemFlags, kSlot335HpRangeMin, kSlot335HpRangeMax, 0x7ee7bf, 0x7ee7e1},
    {1, kSlot335GroupName, 0x0c, 0x128af2e, kSlot335GripVarName, 0x0e, 0x128af48, 0x3265ef0, 1,
     kSlot335ItemFlags, kSlot335GripRangeMin, kSlot335GripRangeMax, 0x7ee965, 0x7ee987},
}};

// ---- objects ------------------------------------------------------------------------------------
inline constexpr std::uint64_t kSlot335BlockRva = 0x32656c0;
inline constexpr std::size_t kSlot335BlockBytes = 0x8d8;         // 0x32656c0 .. 0x3265f98
inline constexpr std::uint64_t kSlot335BlockEndRva = 0x3265f98;

// The aggregate is exactly the 0x8d8-byte original run. `hash_gap` is the 8 bytes between the two
// {name, hash} records (0x3265c80 + 0x18 = 0x3265c98); nothing in the image writes them.
struct Slot335Objects {
    std::array<ConCmdRangedVar, kSlot335UnboundedCount> unbounded{};  // +0x000 0x32656c0, 0xd8 each
    std::array<ConCmdBoolVar, kSlot335BoolCount> bools{};            // +0x360 0x3265a20, 0xc8 each
    std::uint8_t bool_gap[8]{};                                      // +0x5b8 never written
    Slot335HashRecord hash_health{};                                 // +0x5c0 0x3265c80
    std::uint8_t hash_gap[8]{};                                      // +0x5d0 never written
    Slot335HashRecord hash_grip{};                                   // +0x5d8 0x3265c98
    Slot335BoundedVar bounded_hp{};                                  // +0x5e8 0x3265ca8
    DebugMenuItem slider_hp{};                                       // +0x6b8 0x3265d78
    Slot335BoundedVar bounded_grip{};                                // +0x760 0x3265e20
    DebugMenuItem slider_grip{};                                     // +0x830 0x3265ef0
};
static_assert(offsetof(Slot335Objects, unbounded) == 0x000 &&
              offsetof(Slot335Objects, bools) == 0x360 &&
              offsetof(Slot335Objects, bool_gap) == 0x5b8 &&
              offsetof(Slot335Objects, hash_health) == 0x5c0 &&
              offsetof(Slot335Objects, hash_gap) == 0x5d0 &&
              offsetof(Slot335Objects, hash_grip) == 0x5d8 &&
              offsetof(Slot335Objects, bounded_hp) == 0x5e8 &&
              offsetof(Slot335Objects, slider_hp) == 0x6b8 &&
              offsetof(Slot335Objects, bounded_grip) == 0x760 &&
              offsetof(Slot335Objects, slider_grip) == 0x830,
              "the original object addresses are reproduced");
static_assert(offsetof(Slot335Objects, slider_grip) + sizeof(DebugMenuItem) == kSlot335BlockBytes,
              "the block spans 0x8d8 bytes");
static_assert(kSlot335BlockRva + kSlot335BlockBytes == kSlot335BlockEndRva,
              "the block ends where the next object starts");
static_assert(sizeof(ConCmdRangedVar) == 0xd8 && sizeof(ConCmdBoolVar) == 0xc8 &&
              sizeof(DebugMenuItem) == 0xa8);

// The host-owned mirror of the 0x325f410 array window this body addresses (indices 11 and 12 are the
// only ones it touches; the array itself is .bss and its real length is not pinned).
struct Slot335ValueCells {
    std::uint32_t cell[kSlot335ValueArrayWindow]{};
};
static_assert(sizeof(Slot335ValueCells) == 4 * kSlot335ValueArrayWindow);

// One `__cxa_atexit` registration of the body, in body order (every site is tool-pinned).
struct Slot335AtexitRow {
    std::uint32_t step;        // 1..11
    std::uint64_t site_rva;    // the `callq 0xf8f390`
    std::uint64_t dtor_rva;    // rdi (identity tag, never run)
    std::uint64_t object_rva;  // rsi (identity only)
};
inline constexpr std::array<Slot335AtexitRow, kSlot335AtexitRowCount> kSlot335AtexitRows = {{
    {1, 0x7ee040, kSlot335FloatVarDtorRva, 0x32656c0},
    {2, 0x7ee139, kSlot335FloatVarDtorRva, 0x3265798},
    {3, 0x7ee22b, kSlot335FloatVarDtorRva, 0x3265870},
    {4, 0x7ee32a, kSlot335FloatVarDtorRva, 0x3265948},
    {5, 0x7ee42b, kSlot335BoolVarDtorRva, 0x3265a20},
    {6, 0x7ee516, kSlot335BoolVarDtorRva, 0x3265ae8},
    {7, 0x7ee601, kSlot335BoolVarDtorRva, 0x3265bb0},
    {8, 0x7ee741, kSlot335BoundedVarDtorRva, 0x3265ca8},
    {9, 0x7ee7e1, kSlot335MenuItemDtorRva, 0x3265d78},
    {10, 0x7ee8e7, kSlot335BoundedVarDtorRva, 0x3265e20},
    {11, 0x7ee987, kSlot335MenuItemDtorRva, 0x3265ef0},
}};

// One of the six `callq *0x18(%rax)` sites: (site, argument tag, slot-3 body).
struct Slot335FunctorSite {
    std::uint64_t site_rva;
    std::uint64_t tag_rva;
    std::uint64_t slot3_rva;
    bool slot3_is_bare_ret;      // pinned true; false blocks the fragment
};
inline constexpr std::array<Slot335FunctorSite, kSlot335FunctorSiteCount> kSlot335FunctorSites = {{
    {0x7ee008, kSlot335FloatArgFnVtbl, kSlot335FloatFnDestroyRva, true},
    {0x7ee101, kSlot335FloatArgFnVtbl, kSlot335FloatFnDestroyRva, true},
    {0x7ee1f3, kSlot335FloatArgFnVtbl, kSlot335FloatFnDestroyRva, true},
    {0x7ee2ef, kSlot335FloatArgFnVtbl, kSlot335FloatFnDestroyRva, true},
    {0x7ee722, kSlot335FloatArgFnVtbl, kSlot335FloatFnDestroyRva, true},
    {0x7ee8c8, kSlot335FloatArgFnVtbl, kSlot335FloatFnDestroyRva, true},
}};

// ---- stages / results ---------------------------------------------------------------------------
enum class Slot335Stage : std::uint8_t {
    none,
    unbounded_var_build,
    unbounded_var_registration,
    unbounded_var_view_release,
    unbounded_var_atexit,
    bool_var_build,
    bool_var_registration,
    bool_var_view_release,
    bool_var_atexit,
    hash_lookup,
    bounded_var_build,
    bounded_var_registration,
    bounded_var_view_release,
    bounded_var_atexit,
    item_ctor,
    item_registration,
    item_view_release,
    item_atexit,
    canary,
};
[[nodiscard]] const char* slot335_stage_name(Slot335Stage stage) noexcept;

struct Slot335Hooks {
    // The only host input this block needs: the __stack_chk_guard source (0x15fe518). Null => the
    // comparison is not performed and `canary_checked` stays false; a changed guard fails the
    // fragment at stage `canary`. There is deliberately no allocator hook: every view this body
    // builds is borrowed (0xf2f0 stores {ptr, strlen, 0, 0}), so no native body is stood in for.
    const std::uint64_t* stack_guard = nullptr;
};

// The production hooks. Nothing to install; `stack_guard` stays null unless the host supplies it.
[[nodiscard]] Slot335Hooks default_slot335_hooks() noexcept;

// 0x52a990 + 0x161fb0: FNV-1a 32 over a NUL-terminated string. `movsbl` sign-extends each byte
// before the XOR, so the model does too. A null `text` is refused by the caller (the original would
// fault in 0x161fb0); an empty string returns the basis.
[[nodiscard]] std::uint32_t slot335_fnv1a32(const char* text) noexcept;

// 0x7dbca0: `&value_array[index]`. Refuses an index outside the mirrored window (the original has no
// bound at all, but the model will not invent cells the block never addresses).
[[nodiscard]] std::uint32_t* slot335_value_cell(Slot335ValueCells& cells, std::uint32_t index) noexcept;

// 0xf2f0: {ptr, strlen(ptr), 0, 0}. A borrowed view: no counter, no allocation.
void slot335_borrowed_str(SharedStr& out, const char* text) noexcept;

// The c300 net state for a 0xd0 bounded variable. Refuses (writes nothing) when the argument tag is
// not the pinned 0x15ce630.
[[nodiscard]] bool slot335_bounded_var_state(Slot335BoundedVar& var, const SharedStr& name,
                                             void* value_ref, float min_value, float max_value,
                                             std::uint32_t flags, const RangedVarArgs& args,
                                             const SharedStr& description) noexcept;

// 0x6aa00's net state (no bounds engaged). Refuses any tag but 0x15ce630.
[[nodiscard]] bool slot335_unbounded_float_6aa00(ConCmdRangedVar& var, const SharedStr& name,
                                                 void* value_ref, std::uint32_t flags,
                                                 const RangedVarArgs& args,
                                                 const SharedStr& description) noexcept;

// 0x7edd80 (385 B): bounded variable through 0xc300, then final vptr 0x15943c0,
// `default = *value_cell`, then 0x20d60. Refuses a null cell or an unpinned argument tag before the
// first store (the original would fault / would run unlifted slot code).
[[nodiscard]] bool slot335_bounded_var_7edd80(Slot335BoundedVar& var, const SharedStr& name,
                                              const std::uint32_t* value_cell, float min_value,
                                              float max_value, const RangedVarArgs& args,
                                              const SharedStr& description,
                                              ConCmdRegistry& registry) noexcept;

// The lifted `callq *0x18(%rax)` for the six sites: a bare `ret` has no effect.
[[nodiscard]] bool slot335_inert_functor_destroy(const Slot335FunctorSite& site) noexcept;

// The stand-ins the aggregate holds, by index.
[[nodiscard]] ConCmdBase& slot335_unbounded_base(Slot335Objects& objects, std::uint32_t index) noexcept;
[[nodiscard]] ConCmdBase& slot335_bool_base(Slot335Objects& objects, std::uint32_t index) noexcept;
[[nodiscard]] Slot335BoundedVar& slot335_bounded_var(Slot335Objects& objects, std::uint32_t index) noexcept;
[[nodiscard]] Slot335HashRecord& slot335_hash_record(Slot335Objects& objects, std::uint32_t index) noexcept;
[[nodiscard]] DebugMenuItem& slot335_item(Slot335Objects& objects, std::uint32_t index) noexcept;

struct Slot335VarResult {
    bool ok = false;
    bool name_inert = false;        // the 0xf590 on the stack name view
    bool description_inert = false; // the 0xf590 on the stack description view
    bool functor_inert = false;     // the site-local `[tag+0x18]` is the pinned bare ret
    ConCmdRegisterResult registration{};
};

struct Slot335Result {
    bool ok = false;
    Slot335Stage blocked = Slot335Stage::none;
    std::uint32_t blocked_index = 0;               // the row index for the per-object stages
    std::uint32_t atexit_records = 0;              // this body's 11 __cxa_atexit registrations
    std::uint32_t functors_destroyed = 0;          // the six `callq *0x18(%rax)` sites
    std::uint32_t vars_built = 0;                  // 4 unbounded + 3 bool + 2 bounded
    std::uint32_t menu_items = 0;                  // 2
    std::uint32_t hashes_built = 0;                // 2
    std::array<Slot335VarResult, kSlot335UnboundedCount> unbounded{};
    std::array<BoolVarResult, kSlot335BoolCount> bools{};
    std::array<Slot335VarResult, kSlot335BoundedCount> bounded{};
    std::array<std::uint32_t, kSlot335HashCount> hashes{};
    std::array<MenuRegisterResult, kSlot335ItemCount> item_registrations{};
    bool views_inert = true;                       // every 0xf590 call of this body was a no-op
    bool canary_checked = false;                   // false when the host supplied no guard
    bool canary_matched = false;
};

// Runs original constructor 335 natively. The original is unconditional, so the first failing step
// stops the slot (nothing after it is claimed); every failure names its stage and row index.
[[nodiscard]] Slot335Result init_slot335_7edf10(Slot335Objects& objects, Slot335ValueCells& cells,
                                               ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                               const Slot335Hooks& hooks = {}) noexcept;

}  // namespace sotc
