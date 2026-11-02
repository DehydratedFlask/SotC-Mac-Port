// CPU-01-STARTUP-SLOT-326-CHARACTERIZATION-01: native recovery of original constructor-table index
// 326 / RVA 0x837f30 (7,055 B FDE [0x837f30, 0x839abf); 7,054 B reachable over 1,306 instructions,
// the last byte 0x839abe being the trailing nop), ELF sha256 d0723afb...5861d3.
// Offline analysis only: nothing here loads, relocates or executes an original RVA. Byte oracle:
// tools/lift/check_ctor_slot_326.py.
//
// ============================ characterization (from the pinned bytes) ============================
// Reached by DATA only: the startup table word at 0x2dd70e0 - 326*8 = **0x2dd66b0** holds 0x837f30.
// Table neighbours: entry 325 = 0x839f00 (0x2dd66b8) and entry 327 = 0x836330 (0x2dd66a8), both
// already registered.
// CORRECTION (2026-11-01): the task brief quoted the row as 0x2dd65d0. That address is entry **354**
// (0x79e180); 0x2dd70e0 - 0x2dd65d0 = 0xb10 = 354*8. The backwards-table arithmetic, the two
// registered neighbours and the recovered-row RVA all agree on 0x2dd66b0, which is what this model
// and its checker pin.
//
// Body shape from the pinned listing: 1,306 reachable instructions, **159 direct call sites over 10
// targets**, 13 indirect call sites (every one a `callq *0x18(%rax)`), 263 rip-relative operands (50
// of them stores over 45 distinct addresses, 113 distinct targets), and **exactly one conditional
// branch** -- 0x839aa8 `jne 0x839ab9`, the __stack_chk_guard compare -- plus the unpicked
// __stack_chk_fail tail at 0x839ab9. So the body is **two basic blocks**: the straight-line
// constructor run [0x837f30, 0x839ab8] (one `retq`) and the canary-failure tail. The frame:
//     push rbp; mov rsp,rbp; push r15/r14/r13/r12/rbx; and rsp,-0x20; sub rsp,0xe0
//     guard <- *[0x15fe518]  (the GOT cell holds the runtime address of __stack_chk_guard)
//     ... 1,290 instructions of constructor work ...
//     cmp [rsp+0xc0],%rax ; jne 0x839ab9 ; lea -0x28(%rbp),%rsp; pops; retq
// The straight-line shape is why this block is cheap to model per object but expensive to model as
// one function: 36 objects, each a fixed store script, in body order.
//
// Callee histogram (every one of the 159 sites is pinned by the checker):
//   0xf590    x71  shared release of a borrowed stack view (every site is an exact no-op)
//   0xf8f390  x36  __cxa_atexit PLT (third argument 0x1604218 = __dso_handle, one per object)
//   0x20d60   x15  register-once guard (two bool variables + thirteen float variables)
//   0xc300    x13  float-variable ctor (1 threshold + 12 eye colours)
//   0x27050   x12  debug-menu slider item ctor (0x30 flags {0.01, 0.1})
//   0x26c10   x5   debug-menu group item ctor (kind 1)
//   0x26ea0   x3   debug-menu toggle item ctor (kind 2)
//   0x20af0   x2   ConCmdBase ctor (the two bool variables)
//   0x26800   x1   kind-0 "MenuSeparator" label item ctor (see below)
//   0xf8f3a0  x1   __stack_chk_fail (the branch target only)
// The five item ctors tail-call 0x268b0, so no direct 0x268b0 site appears in this body.
//
// The 13 indirect sites (0x838240 / 0x838564 / 0x838674 / 0x838786 / 0x838890 / 0x83899d / 0x838ab2
// / 0x838bc5 / 0x838cdc / 0x838df0 / 0x838efe / 0x839015 / 0x83912a) are `callq *0x18(%rax)` with
// %rax = 0x15cec00, the *argument* functor tag of each 0xc300 site (the site copies the caller's
// argument block to +0x80 before the dispatch). Slot 3 of 0x15cec00 is 0xc4f0, a bare `ret`, so all
// thirteen are inert functor destroys of a stack temporary. The two argument tags this body uses are
// members of two *families*: 0x15cebe0 (the bool sites) heads 0xc2a0 copy / 0xc2c0 clone / 0xc2e0
// move / 0xc2f0 bare-ret destroy, and 0x15cec00 (the float sites) heads 0xc4a0 copy / 0xc4c0 clone /
// 0xc4e0 move / 0xc4f0 bare-ret destroy. Each table's second half repeats the other family's four
// bodies, and 0xc4a0..0xc4f0 plus 0x1594318 are exactly what the shared 0xc300 model already expects;
// the tag itself is nevertheless pinned here rather than assumed equal to 0x1594318.
//
// What the block IS. It is the **Colossus / Shadow eye-colour debug page**: two console bool
// variables, one console float variable, twelve console float variables for three RGBA colour
// vectors, one "Colossus" menu group with three toggles, one "Shadow Eye Color" group with a label
// row and three sub-groups of four sliders. The strings are the evidence:
//   names      shadow_debugEyeState (0x128e170, 0x14), forceColossusAngry (0x128e185, 0x12),
//              playerNotTouchingBossThreshold (0x128e198, 0x1e), eyeColor{Inactive,Blue,Red}{R,G,B,A}
//              (0x128e217 .. 0x128e2be)
//   groups     CP10 (0x128e1b7, 4) -> Colossus (0x128e1bc, 8); Shadow (0x128e2cb, 6) ->
//              Shadow Eye Color (0x128e2d2, 0x10) -> ... Inactive (0x128e2e3, 0x19) /
//              ... Blue (0x128e305, 0x15) / ... Red (0x128e31b, 0x14)
//   captions   Force Eyes Angry (0x128e1c5, 0x10), Debug Eye State (0x128e1d6, 0xf),
//              Player not touching colossus threshold (seconds) (0x128e1e6, 0x30), and the slider
//              labels R/G/B/A (0x128e2fd/0x128e2ff/0x128e301/0x128e303, one byte each)
//   empty description 0x128e348 (a zero byte at the head of the next string table)
//   "MenuSeparator" 0xf96530 (built by 0x26800 through 0xf1a0)
// The 0x2dce400 table the twelve eye-colour variables point their value cells at is three 4-float
// vectors: {1.0, 1.0, 1.0, 0.0} / {0.3, 0.7, 1.0, 1.0} / {1.0, 0.5, 0.3, 1.0}. The reading "RGBA
// colour vectors" is INFERENCE from the names and the 4-stride; the bytes are the fact.
//
// ================================ object map (verified strides) ================================
// Every original address below is consecutive, and the native aggregate reproduces all of them:
//   0x3291890  bool  shadow_debugEyeState     0xc8   (value cell 0x3291950 = obj+0xc0)
//   0x3291958  bool  forceColossusAngry       0xc8   (value cell 0x3291a18 = obj+0xc0)
//   0x3291a20  float playerNotTouching...     0xd8   (value cell 0x3291af0 = obj+0xd0, its own)
//   0x3291af8  group CP10 / Colossus          0xa8
//   0x3291ba0  toggle Force Eyes Angry        0xa8
//   0x3291c48  toggle Debug Eye State         0xa8
//   0x3291cf0  toggle Player not touching ... 0xa8
//   0x3291d98 .. 0x3292688  twelve eye-colour floats, **0xd0 apart**
//   0x3292758  group Shadow / Shadow Eye Color      0xa8
//   0x3292800  the 0x26800 kind-0 label item        0xa8
//   0x32928a8  group ... / ... Inactive             0xa8
//   0x3292950  0x32929f8  0x3292aa0  0x3292b48      slider R/G/B/A           0xa8 each
//   0x3292bf0  group ... / ... Blue                 0xa8
//   0x3292c98  0x3292d40  0x3292de8  0x3292e90      slider R/G/B/A           0xa8 each
//   0x3292f38  group ... / ... Red                  0xa8
//   0x3292fe0  0x3293088  0x3293130  0x32931d8      slider R/G/B/A           0xa8 each
//   0x3293280  end of the block (span 0x19f0)
// The eye-colour run is the one place where the natural library types do not fit: 0xc300 never
// writes its own +0xd0 dword, so the original lays the twelve objects 0xd0 apart and each object's
// +0xd0 is the NEXT object's vptr. `Slot326EyeVar` is therefore declared as the 0xd0 prefix of the
// c300 shape (same offsets up to +0xcf, no +0xd0 field), which is what makes the run exact. Writing
// the library `ConCmdRangedVar` here would be 8 B too wide per object and would silently shift every
// item after the run.
//
// ================================ modelled vs hook (G2) =================================
// MODELLED (native, exact):
//   * the frame, the canary guard and the epilogue;
//   * the two bool variables through the existing `init_bool_var_static` (class 0x1594218, argument
//     tag 0x15cebe0, value 0 / default 0, its own two releases, 0x20d60 and atexit 0x20b0);
//   * the threshold float through `slot326_float_c300_state` (the c300 net state) with value_ref =
//     its own +0xd0, both bounds [0, FLT_MAX] engaged, flags 1, argument tag 0x15cec00, then the
//     site's final vptr 0x15942b0 and the two 3.0f stores (+0xd0 value, +0x68 default);
//   * the twelve eye-colour floats through the same c300 net state on the 0xd0 shape, value_ref =
//     a cell of the mirrored 0x2dce400 table, both bounds [0, 2.0] engaged, final vptr 0x15943c0,
//     default = that cell.
//     Why the net state is reproduced here instead of calling the shared `init_ranged_float_var_c300`:
//     that model accepts exactly three argument tags (0x15d57e0 / 0x1594318 / 0x15d2040) and *refuses*
//     0x15cec00, which is the only tag these thirteen sites pass. Widening a shared contract was out of
//     scope for this task, so both object shapes go through `slot326_float_c300_state`, which refuses
//     any tag but 0x15cec00; the checker pins every one of its stores against the 0xc300 body
//     (+0x60, +0x6c, +0x70, +0x74, +0x78, +0x80, +0xb0, +0xc0, +0xc8 and the 0x1594128 mid vptr) rather
//     than against the other model;
//   * the five groups through `init_menu_group_26c10`, the three toggles through
//     `init_debug_menu_item_26ea0` (with the {slot3, slot6} answers below), the twelve sliders through
//     `init_debug_menu_item_27050` ({0.01, 0.1}), all registered through `register_menu_item_268b0`;
//   * the 0x26800 label item through `init_slot326_label_item_26800` (see the next paragraph);
//   * every 0xf590 release, every __cxa_atexit record, in body order.
// The 0x26800 item is a 173 B body that no earlier module had modelled: it is the 0x26b60 separator
// shape with one extra store. It builds the link, copies the caller's view into +0x28 with a counter
// acquire, sets kind 3, constructs "MenuSeparator" into +0x50 through the native 0xf1a0 model, the
// empty view at +0x70 through 0xf180, target 0, range {1.0, 10.0}, the caller's flags at +0xa0, and
// then **overwrites kind with 0** (0x26897) before tail-calling 0x268b0. The net kind is 0; the
// transient 3 is reproduced by the model because a host that inspects +0x48 mid-construction is
// looking at the same bytes the original writes.
//
// HOOK (the only one this block needs): `Slot326Hooks::string_alloc`, the allocator pair behind
// 0x12820 / 0x12af0 that the 0x26800 "MenuSeparator" text needs. This is the same documented
// malloc/free stand-in the registered slot 306 and slot 316 models use; `default_slot326_hooks()`
// installs it, and clearing it makes the fragment fail visibly at `label_item_build` (the original
// would have faulted). The engine allocator body is NOT recovered and is not claimed.
// Optional host input: `Slot326Hooks::stack_guard`; without it the guard compare is not claimed
// (`canary_checked` stays false) and the result never reports a silent pass.
//
// Fail-visible contracts (all reported through `blocked` / `ok == false`, never a silent pass):
//   * a null `string_alloc` allocator, or an allocation failure, at the 0x26800 label item;
//   * a variable spec that is not the pinned row for its index (refuse, write nothing);
//   * a float site whose spec/tag is not the pinned argument class 0x15cec00;
//   * a failed 0x20d60 registration;
//   * a 0x26ea0 target whose vptr tag is neither 0x1594218 nor 0x15942b0 (an unpinned class would
//     run unlifted virtual code);
//   * a reached 0x268b0 assert/gate site (only possible if a spec's flags changed);
//   * a failed __cxa_atexit record;
//   * a `__stack_chk_guard` that changed across the body.
//
// NOT modelled: every destructor body (0x20b0 bool, 0x20f0 ranged float, 0xad30 eye colour,
// 0x272d0 item, 0x27280 root). The argument-functor slot bodies 0xc4a0 (copy) / 0xc4c0 (clone) /
// 0xc4e0 (move) are byte-pinned and their net effect is reproduced by the collapsed install;
// 0xc4f0 (destroy) is a bare `ret`. None of them is executed. No eye colour, menu layout or gameplay
// behaviour is claimed; nothing here reads the 0x2dce400 table back out of the image.
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

inline constexpr std::uint64_t kSlot326Rva = 0x837f30;
inline constexpr std::size_t kSlot326BodySize = 7055;          // FDE [0x837f30, 0x839abf)
inline constexpr std::uint64_t kSlot326EndRva = 0x839abf;
inline constexpr std::size_t kSlot326ReachableBytes = 7054;    // the last reached byte is the trailing nop
inline constexpr std::size_t kSlot326InstructionCount = 1306;
inline constexpr std::size_t kSlot326DirectCallSites = 159;
inline constexpr std::size_t kSlot326DirectCallees = 10;
inline constexpr std::size_t kSlot326IndirectCallSites = 13;
inline constexpr std::size_t kSlot326RipOperands = 263;
inline constexpr std::size_t kSlot326RipTargets = 113;
inline constexpr std::size_t kSlot326StoreSites = 50;
inline constexpr std::size_t kSlot326StoreTargets = 45;
inline constexpr std::size_t kSlot326BranchCount = 1;
inline constexpr std::size_t kSlot326BasicBlocks = 2;
inline constexpr std::uint64_t kSlot326TableTopRva = 0x2dd70e0;   // index i at TOP - 8*i
inline constexpr std::uint64_t kSlot326TableWordRva = 0x2dd66b0;  // 0x2dd70e0 - 326*8
inline constexpr std::uint32_t kSlot326TableIndex = 326;
// The next index that is still unrecovered once this one is registered: 327 and 328 are already
// registered startup slots, so the cliff moves to index 329.
inline constexpr std::uint32_t kSlot326NextUnrecoveredIndex = 329;
inline constexpr std::uint64_t kSlot326NextUnrecoveredRva = 0x829e90;

// Frame / guard.
inline constexpr std::uint64_t kSlot326CanaryCellRva = 0x15fe518;   // GOT cell holding &__stack_chk_guard
inline constexpr std::uint32_t kSlot326CanaryFrameOffset = 0xc0;    // [rsp+0xc0]
inline constexpr std::uint64_t kSlot326StackChkFailRva = 0xf8f3a0;
inline constexpr std::uint64_t kSlot326BranchRva = 0x839aa8;        // jne 0x839ab9 (the guard compare)
inline constexpr std::uint64_t kSlot326BranchTargetRva = 0x839ab9;
inline constexpr std::uint64_t kSlot326EpilogueRva = 0x839aaa;      // lea -0x28(%rbp),%rsp
inline constexpr std::uint32_t kSlot326FrameBytes = 0xe0;           // sub rsp,0xe0 (after and rsp,-0x20)

// Shared callees.
inline constexpr std::uint64_t kSlot326AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot326DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot326ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot326ConCmdBaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot326ConCmdRegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot326FloatVarCtorRva = 0xc300;
inline constexpr std::size_t kSlot326FloatVarCtorSize = 0x160;
inline constexpr std::uint64_t kSlot326MenuGroupCtorRva = 0x26c10;
inline constexpr std::uint64_t kSlot326MenuToggleCtorRva = 0x26ea0;
inline constexpr std::uint64_t kSlot326MenuSliderCtorRva = 0x27050;
inline constexpr std::uint64_t kSlot326MenuRegisterRva = 0x268b0;
inline constexpr std::uint64_t kSlot326LabelItemCtorRva = 0x26800;
inline constexpr std::size_t kSlot326LabelItemCtorSize = 173;
inline constexpr std::uint64_t kSlot326LabelItemCtorEndRva = 0x268ad;
inline constexpr std::uint64_t kSlot326SeparatorTextRva = 0xf96530;   // "MenuSeparator"
inline constexpr char kSlot326SeparatorText[] = "MenuSeparator";
inline constexpr std::uint64_t kSlot326EmptyViewCtorRva = 0xf180;
inline constexpr std::uint64_t kSlot326SharedStrCtorRva = 0xf1a0;

// Destructors (identity tags recorded with __cxa_atexit, never executed).
inline constexpr std::uint64_t kSlot326BoolVarDtorRva = 0x20b0;
inline constexpr std::uint64_t kSlot326FloatVarDtorRva = 0x20f0;
inline constexpr std::uint64_t kSlot326EyeVarDtorRva = 0xad30;
inline constexpr std::uint64_t kSlot326MenuItemDtorRva = 0x272d0;

// Identity tags (original vtable address points; never dispatched through here).
inline constexpr std::uint64_t kSlot326BoolMidVptr = 0x15940b0;        // 0x15940a0 + 0x10
inline constexpr std::uint64_t kSlot326BoolFinalVptr = 0x1594218;      // 0x1594208 + 0x10
inline constexpr std::uint64_t kSlot326FloatMidVptr = 0x1594128;       // 0x1594118 + 0x10
inline constexpr std::uint64_t kSlot326FloatFinalVptr = 0x15942b0;     // 0x15942a0 + 0x10
inline constexpr std::uint64_t kSlot326EyeFinalVptr = 0x15943c0;       // 0x15943b0 + 0x10
inline constexpr std::uint64_t kSlot326BoolArgFnVtbl = 0x15cebe0;      // the two bool sites' argument tag
inline constexpr std::uint64_t kSlot326FloatArgFnVtbl = 0x15cec00;     // the thirteen float sites' argument tag
inline constexpr std::uint64_t kSlot326FloatFnCopyRva = 0xc4a0;        // slot 0 of both argument tables
inline constexpr std::uint64_t kSlot326FloatFnCloneRva = 0xc4c0;       // slot 1
inline constexpr std::uint64_t kSlot326FloatFnMoveRva = 0xc4e0;        // slot 2
inline constexpr std::uint64_t kSlot326FloatFnDestroyRva = 0xc4f0;     // slot 3: one `retq`

// The 0x26ea0 gate answers of the two classes this body targets, read from the target's own vptr tag.
inline constexpr std::uint64_t kSlot326BoolGate3Rva = 0xb890;   // mov eax,1; ret
inline constexpr std::uint64_t kSlot326BoolGate6Rva = 0xb930;   // xor eax,eax; ret
inline constexpr std::uint64_t kSlot326FloatGate3Rva = 0xbba0;  // mov eax,1; ret
inline constexpr std::uint64_t kSlot326FloatGate6Rva = 0xbbf0;  // mov eax,3; ret
inline constexpr std::uint32_t kSlot326BoolGate3 = 1;
inline constexpr std::uint32_t kSlot326BoolGate6 = 0;    // so a bool toggle keeps the range {1.0, 10.0}
inline constexpr std::uint32_t kSlot326FloatGate3 = 1;
inline constexpr std::uint32_t kSlot326FloatGate6 = 3;   // so the float toggle's range becomes {0.1, 0.5}

// Per-site constants.
inline constexpr std::uint32_t kSlot326VarFlags = 1;         // r9d at every 0xc300 site and ecx at 0x20af0
inline constexpr std::uint32_t kSlot326ItemFlags = 0x30;     // every group / toggle / slider of this body
inline constexpr std::uint32_t kSlot326LabelItemFlags = 0;   // the 0x26800 item: `xorl %ecx,%ecx`
inline constexpr std::uint32_t kSlot326ThresholdMinBits = 0x00000000u;   // 0.0f
inline constexpr std::uint32_t kSlot326ThresholdMaxBits = 0x7f7fffffu;   // FLT_MAX
inline constexpr std::uint32_t kSlot326EyeMinBits = 0x00000000u;         // 0.0f
inline constexpr std::uint32_t kSlot326EyeMaxBits = 0x40000000u;         // 2.0f
inline constexpr std::uint32_t kSlot326ThresholdValueBits = 0x40400000u; // 3.0f, stored twice
inline constexpr std::uint32_t kSlot326SliderMinBits = 0x3c23d70au;      // 0.01f  (0x128e134)
inline constexpr std::uint32_t kSlot326SliderMaxBits = 0x3dcccccd;       // 0.1f   (0x128e138)
inline constexpr float kSlot326LabelRangeMin = 1.0f;                     // 0x26800 stores 0x3f800000
inline constexpr float kSlot326LabelRangeMax = 10.0f;                    // 0x26800 stores 0x41200000
inline constexpr std::uint32_t kSlot326GroupKind = 1;
inline constexpr std::uint32_t kSlot326ItemKind = 2;
inline constexpr std::uint32_t kSlot326LabelKind = 0;        // the net kind of the 0x26800 item
inline constexpr std::uint32_t kSlot326LabelTransientKind = 3;  // 0x26855 stores 3, 0x26897 overwrites it

// Strings of the block: identity address + pinned length (the native views are literals / measured).
inline constexpr char kSlot326EmptyText[] = "";                   // 0x128e348, the first byte is 0
inline constexpr std::uint64_t kSlot326EmptyTextRva = 0x128e348;
inline constexpr std::size_t kSlot326BoolCount = 2;
inline constexpr std::size_t kSlot326FloatCount = 13;             // 1 threshold + 12 eye colours
inline constexpr std::size_t kSlot326EyeCount = 12;
inline constexpr std::size_t kSlot326ItemCount = 21;              // 5 groups + 3 toggles + 12 sliders + 1 label
inline constexpr std::size_t kSlot326AtexitRowCount = 36;
inline constexpr std::size_t kSlot326FunctorSiteCount = 13;

// ---- the image's 0x2dce400 colour table, mirrored ---------------------------------------------
// The twelve eye-colour variables point their +0x60 value cell into three 4-float vectors. Nothing
// in this body ever writes that table (0xc300 only stores the pointer), so the native model mirrors
// it byte-for-byte and points value_ref at the mirror instead of at an image address.
struct Slot326EyeValues {
    std::uint32_t inactive[4];   // 0x2dce400: 1.0, 1.0, 1.0, 0.0
    std::uint32_t blue[4];       // 0x2dce410: 0.3, 0.7, 1.0, 1.0
    std::uint32_t red[4];        // 0x2dce420: 1.0, 0.5, 0.3, 1.0
};
inline constexpr std::uint64_t kSlot326EyeValueTableRva = 0x2dce400;
inline constexpr Slot326EyeValues kSlot326EyeValueBits = {
    {0x3f800000u, 0x3f800000u, 0x3f800000u, 0x00000000u},
    {0x3e99999au, 0x3f333333u, 0x3f800000u, 0x3f800000u},
    {0x3f800000u, 0x3f000000u, 0x3e99999au, 0x3f800000u},
};

// ---- the two bool variables -------------------------------------------------------------------
struct Slot326BoolSpec {
    const char* name;
    std::uint32_t name_len;
    std::uint64_t name_rva;
    std::uint64_t object_rva;
    std::uint8_t value;
    std::uint8_t default_value;
    std::uint64_t atexit_site_rva;
};
inline constexpr std::array<Slot326BoolSpec, kSlot326BoolCount> kSlot326Bools = {{
    {"shadow_debugEyeState", 20, 0x128e170, 0x3291890, 0, 0, 0x838062},
    {"forceColossusAngry", 18, 0x128e185, 0x3291958, 0, 0, 0x83815c},
}};

// ---- the thirteen float variables -------------------------------------------------------------
// `eye_row` selects the mirrored colour vector (0..2) and `eye_channel` the float inside it (0..3).
// The threshold row carries its own value cell instead (its object's +0xd0) and writes it.
struct Slot326FloatSpec {
    std::uint32_t index;            // 0 = the threshold, 1..12 = the eye colours in body order
    const char* name;
    std::uint32_t name_len;
    std::uint64_t name_rva;
    std::uint64_t object_rva;
    bool eye_colour;                // true: the 0xd0 shape; false: the 0xd8 `ConCmdRangedVar` shape
    std::uint8_t eye_row;           // valid when eye_colour
    std::uint8_t eye_channel;       // valid when eye_colour
    std::uint64_t value_ref_rva;    // the original +0x60 target (its own +0xd0, or 0x2dce400 + 4k)
    std::uint32_t min_bits;
    std::uint32_t max_bits;
    bool writes_value_cell;         // the site stores value_bits at +0xd0 (threshold only)
    std::uint32_t value_bits;
    std::uint32_t default_bits;     // the site stores this at +0x68
    std::uint64_t final_vptr;       // the class the site installs after the ctor
    std::uint64_t atexit_site_rva;
    std::uint64_t dtor_rva;
};
inline constexpr std::array<Slot326FloatSpec, kSlot326FloatCount> kSlot326Floats = {{
    {0, "playerNotTouchingBossThreshold", 30, 0x128e198, 0x3291a20, false, 0, 0, 0x3291af0,
     kSlot326ThresholdMinBits, kSlot326ThresholdMaxBits, true, kSlot326ThresholdValueBits,
     kSlot326ThresholdValueBits, kSlot326FloatFinalVptr, 0x83827b, kSlot326FloatVarDtorRva},
    {1, "eyeColorInactiveR", 17, 0x128e217, 0x3291d98, true, 0, 0, 0x2dce400, kSlot326EyeMinBits,
     kSlot326EyeMaxBits, false, 0, 0x3f800000u, kSlot326EyeFinalVptr, 0x8385a5, kSlot326EyeVarDtorRva},
    {2, "eyeColorInactiveG", 17, 0x128e229, 0x3291e68, true, 0, 1, 0x2dce404, kSlot326EyeMinBits,
     kSlot326EyeMaxBits, false, 0, 0x3f800000u, kSlot326EyeFinalVptr, 0x8386b2, kSlot326EyeVarDtorRva},
    {3, "eyeColorInactiveB", 17, 0x128e23b, 0x3291f38, true, 0, 2, 0x2dce408, kSlot326EyeMinBits,
     kSlot326EyeMaxBits, false, 0, 0x3f800000u, kSlot326EyeFinalVptr, 0x8387bd, kSlot326EyeVarDtorRva},
    {4, "eyeColorInactiveA", 17, 0x128e24d, 0x3292008, true, 0, 3, 0x2dce40c, kSlot326EyeMinBits,
     kSlot326EyeMaxBits, false, 0, 0x00000000u, kSlot326EyeFinalVptr, 0x8388c7, kSlot326EyeVarDtorRva},
    {5, "eyeColorBlueR", 13, 0x128e25f, 0x32920d8, true, 1, 0, 0x2dce410, kSlot326EyeMinBits,
     kSlot326EyeMaxBits, false, 0, 0x3e99999au, kSlot326EyeFinalVptr, 0x8389dc, kSlot326EyeVarDtorRva},
    {6, "eyeColorBlueG", 13, 0x128e26d, 0x32921a8, true, 1, 1, 0x2dce414, kSlot326EyeMinBits,
     kSlot326EyeMaxBits, false, 0, 0x3f333333u, kSlot326EyeFinalVptr, 0x838af2, kSlot326EyeVarDtorRva},
    {7, "eyeColorBlueB", 13, 0x128e27b, 0x3292278, true, 1, 2, 0x2dce418, kSlot326EyeMinBits,
     kSlot326EyeMaxBits, false, 0, 0x3f800000u, kSlot326EyeFinalVptr, 0x838c03, kSlot326EyeVarDtorRva},
    {8, "eyeColorBlueA", 13, 0x128e289, 0x3292348, true, 1, 3, 0x2dce41c, kSlot326EyeMinBits,
     kSlot326EyeMaxBits, false, 0, 0x3f800000u, kSlot326EyeFinalVptr, 0x838d16, kSlot326EyeVarDtorRva},
    {9, "eyeColorRedR", 12, 0x128e297, 0x3292418, true, 2, 0, 0x2dce420, kSlot326EyeMinBits,
     kSlot326EyeMaxBits, false, 0, 0x3f800000u, kSlot326EyeFinalVptr, 0x838e28, kSlot326EyeVarDtorRva},
    {10, "eyeColorRedG", 12, 0x128e2a4, 0x32924e8, true, 2, 1, 0x2dce424, kSlot326EyeMinBits,
     kSlot326EyeMaxBits, false, 0, 0x3f000000u, kSlot326EyeFinalVptr, 0x838f42, kSlot326EyeVarDtorRva},
    {11, "eyeColorRedB", 12, 0x128e2b1, 0x32925b8, true, 2, 2, 0x2dce428, kSlot326EyeMinBits,
     kSlot326EyeMaxBits, false, 0, 0x3e99999au, kSlot326EyeFinalVptr, 0x839053, kSlot326EyeVarDtorRva},
    {12, "eyeColorRedA", 12, 0x128e2be, 0x3292688, true, 2, 3, 0x2dce42c, kSlot326EyeMinBits,
     kSlot326EyeMaxBits, false, 0, 0x3f800000u, kSlot326EyeFinalVptr, 0x839162, kSlot326EyeVarDtorRva},
}};

// ---- the twenty-one menu items ----------------------------------------------------------------
enum class Slot326ItemKind : std::uint8_t { group, toggle, slider, label };
enum class Slot326TargetKind : std::uint8_t { none, bool_var, float_var };

struct Slot326ItemSpec {
    Slot326ItemKind kind;
    const char* label;              // view A (+0x28): the parent path
    std::uint32_t label_len;
    std::uint64_t label_rva;
    const char* text;               // view B (+0x50): the caption (null for the 0x26800 label item)
    std::uint32_t text_len;
    std::uint64_t text_rva;
    std::uint64_t object_rva;
    Slot326TargetKind target_kind;
    std::uint32_t target_index;     // into kSlot326Bools / kSlot326Floats
    std::uint32_t flags;
    std::uint64_t atexit_site_rva;
};

inline constexpr char kSlot326GroupColossus[] = "Colossus";            // 0x128e1bc, 8
inline constexpr char kSlot326GroupCrown[] = "CP10";                   // 0x128e1b7, 4
inline constexpr char kSlot326GroupShadow[] = "Shadow";                // 0x128e2cb, 6
inline constexpr char kSlot326GroupEyeColor[] = "Shadow Eye Color";    // 0x128e2d2, 0x10
inline constexpr char kSlot326GroupInactive[] = "Shadow Eye Color Inactive";  // 0x128e2e3, 0x19
inline constexpr char kSlot326GroupBlue[] = "Shadow Eye Color Blue";   // 0x128e305, 0x15
inline constexpr char kSlot326GroupRed[] = "Shadow Eye Color Red";     // 0x128e31b, 0x14
inline constexpr char kSlot326ToggleEyesAngry[] = "Force Eyes Angry";  // 0x128e1c5, 0x10
inline constexpr char kSlot326ToggleEyeState[] = "Debug Eye State";    // 0x128e1d6, 0xf
inline constexpr char kSlot326ToggleThreshold[] =
    "Player not touching colossus threshold (seconds)";                // 0x128e1e6, 0x30
inline constexpr char kSlot326ChannelR[] = "R";                        // 0x128e2fd, 1
inline constexpr char kSlot326ChannelG[] = "G";                        // 0x128e2ff, 1
inline constexpr char kSlot326ChannelB[] = "B";                        // 0x128e301, 1
inline constexpr char kSlot326ChannelA[] = "A";                        // 0x128e303, 1
static_assert(sizeof kSlot326GroupColossus - 1 == 8);
static_assert(sizeof kSlot326GroupCrown - 1 == 4);
static_assert(sizeof kSlot326GroupShadow - 1 == 6);
static_assert(sizeof kSlot326GroupEyeColor - 1 == 0x10);
static_assert(sizeof kSlot326GroupInactive - 1 == 0x19);
static_assert(sizeof kSlot326GroupBlue - 1 == 0x15);
static_assert(sizeof kSlot326GroupRed - 1 == 0x14);
static_assert(sizeof kSlot326ToggleEyesAngry - 1 == 0x10);
static_assert(sizeof kSlot326ToggleEyeState - 1 == 0xf);
static_assert(sizeof kSlot326ToggleThreshold - 1 == 0x30);
static_assert(sizeof kSlot326SeparatorText - 1 == 0xd);
static_assert(sizeof kSlot326EmptyText - 1 == 0);

inline constexpr std::array<Slot326ItemSpec, kSlot326ItemCount> kSlot326Items = {{
    // 0x3291af8: the page group. parent "CP10", caption "Colossus".
    {Slot326ItemKind::group, kSlot326GroupCrown, 4, 0x128e1b7, kSlot326GroupColossus, 8, 0x128e1bc,
     0x3291af8, Slot326TargetKind::none, 0, kSlot326ItemFlags, 0x838302},
    // 0x3291ba0: toggle on the forceColossusAngry bool.
    {Slot326ItemKind::toggle, kSlot326GroupColossus, 8, 0x128e1bc, kSlot326ToggleEyesAngry, 0x10,
     0x128e1c5, 0x3291ba0, Slot326TargetKind::bool_var, 1, kSlot326ItemFlags, 0x838386},
    // 0x3291c48: toggle on the shadow_debugEyeState bool.
    {Slot326ItemKind::toggle, kSlot326GroupColossus, 8, 0x128e1bc, kSlot326ToggleEyeState, 0xf,
     0x128e1d6, 0x3291c48, Slot326TargetKind::bool_var, 0, kSlot326ItemFlags, 0x838410},
    // 0x3291cf0: toggle on the threshold FLOAT -- the one target whose gate is {1, 3}.
    {Slot326ItemKind::toggle, kSlot326GroupColossus, 8, 0x128e1bc, kSlot326ToggleThreshold, 0x30,
     0x128e1e6, 0x3291cf0, Slot326TargetKind::float_var, 0, kSlot326ItemFlags, 0x838494},
    // 0x3292758: the "Shadow Eye Color" group under "Shadow".
    {Slot326ItemKind::group, kSlot326GroupShadow, 6, 0x128e2cb, kSlot326GroupEyeColor, 0x10,
     0x128e2d2, 0x3292758, Slot326TargetKind::none, 0, kSlot326ItemFlags, 0x8391e5},
    // 0x3292800: the 0x26800 kind-0 "MenuSeparator" row inside that group (flags 0).
    {Slot326ItemKind::label, kSlot326GroupEyeColor, 0x10, 0x128e2d2, nullptr, 0, 0, 0x3292800,
     Slot326TargetKind::none, 0, kSlot326LabelItemFlags, 0x83922e},
    // 0x32928a8: the Inactive sub-group and its four sliders.
    {Slot326ItemKind::group, kSlot326GroupEyeColor, 0x10, 0x128e2d2, kSlot326GroupInactive, 0x19,
     0x128e2e3, 0x32928a8, Slot326TargetKind::none, 0, kSlot326ItemFlags, 0x8392a7},
    {Slot326ItemKind::slider, kSlot326GroupInactive, 0x19, 0x128e2e3, kSlot326ChannelR, 1, 0x128e2fd,
     0x3292950, Slot326TargetKind::float_var, 1, kSlot326ItemFlags, 0x839338},
    {Slot326ItemKind::slider, kSlot326GroupInactive, 0x19, 0x128e2e3, kSlot326ChannelG, 1, 0x128e2ff,
     0x32929f8, Slot326TargetKind::float_var, 2, kSlot326ItemFlags, 0x8393c9},
    {Slot326ItemKind::slider, kSlot326GroupInactive, 0x19, 0x128e2e3, kSlot326ChannelB, 1, 0x128e301,
     0x3292aa0, Slot326TargetKind::float_var, 3, kSlot326ItemFlags, 0x83945d},
    {Slot326ItemKind::slider, kSlot326GroupInactive, 0x19, 0x128e2e3, kSlot326ChannelA, 1, 0x128e303,
     0x3292b48, Slot326TargetKind::float_var, 4, kSlot326ItemFlags, 0x8394f1},
    // 0x3292bf0: the Blue sub-group and its four sliders.
    {Slot326ItemKind::group, kSlot326GroupEyeColor, 0x10, 0x128e2d2, kSlot326GroupBlue, 0x15,
     0x128e305, 0x3292bf0, Slot326TargetKind::none, 0, kSlot326ItemFlags, 0x839571},
    {Slot326ItemKind::slider, kSlot326GroupBlue, 0x15, 0x128e305, kSlot326ChannelR, 1, 0x128e2fd,
     0x3292c98, Slot326TargetKind::float_var, 5, kSlot326ItemFlags, 0x839605},
    {Slot326ItemKind::slider, kSlot326GroupBlue, 0x15, 0x128e305, kSlot326ChannelG, 1, 0x128e2ff,
     0x3292d40, Slot326TargetKind::float_var, 6, kSlot326ItemFlags, 0x839699},
    {Slot326ItemKind::slider, kSlot326GroupBlue, 0x15, 0x128e305, kSlot326ChannelB, 1, 0x128e301,
     0x3292de8, Slot326TargetKind::float_var, 7, kSlot326ItemFlags, 0x83972d},
    {Slot326ItemKind::slider, kSlot326GroupBlue, 0x15, 0x128e305, kSlot326ChannelA, 1, 0x128e303,
     0x3292e90, Slot326TargetKind::float_var, 8, kSlot326ItemFlags, 0x8397c1},
    // 0x3292f38: the Red sub-group and its four sliders.
    {Slot326ItemKind::group, kSlot326GroupEyeColor, 0x10, 0x128e2d2, kSlot326GroupRed, 0x14,
     0x128e31b, 0x3292f38, Slot326TargetKind::none, 0, kSlot326ItemFlags, 0x839841},
    {Slot326ItemKind::slider, kSlot326GroupRed, 0x14, 0x128e31b, kSlot326ChannelR, 1, 0x128e2fd,
     0x3292fe0, Slot326TargetKind::float_var, 9, kSlot326ItemFlags, 0x8398d5},
    {Slot326ItemKind::slider, kSlot326GroupRed, 0x14, 0x128e31b, kSlot326ChannelG, 1, 0x128e2ff,
     0x3293088, Slot326TargetKind::float_var, 10, kSlot326ItemFlags, 0x839969},
    {Slot326ItemKind::slider, kSlot326GroupRed, 0x14, 0x128e31b, kSlot326ChannelB, 1, 0x128e301,
     0x3293130, Slot326TargetKind::float_var, 11, kSlot326ItemFlags, 0x8399fd},
    {Slot326ItemKind::slider, kSlot326GroupRed, 0x14, 0x128e31b, kSlot326ChannelA, 1, 0x128e303,
     0x32931d8, Slot326TargetKind::float_var, 12, kSlot326ItemFlags, 0x839a91},
}};

// ---- the 0xd0 eye-colour object ---------------------------------------------------------------
// The c300 shape truncated at +0xd0: every field 0xc300 writes is here with the library type's
// offset, and the library type's +0xd0 `value` dword is deliberately absent because in the original
// layout that dword is the NEXT object's vptr.
struct Slot326EyeVar {
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
static_assert(sizeof(Slot326EyeVar) == 0xd0, "the eye-colour objects are 0xd0 apart");
static_assert(offsetof(Slot326EyeVar, value_ref) == 0x60 && offsetof(Slot326EyeVar, default_value) == 0x68 &&
              offsetof(Slot326EyeVar, min_value) == 0x6c && offsetof(Slot326EyeVar, min_engaged) == 0x70 &&
              offsetof(Slot326EyeVar, max_value) == 0x74 && offsetof(Slot326EyeVar, max_engaged) == 0x78 &&
              offsetof(Slot326EyeVar, args) == 0x80 && offsetof(Slot326EyeVar, fn_inline) == 0xa0 &&
              offsetof(Slot326EyeVar, fn_payload) == 0xb0 && offsetof(Slot326EyeVar, fn_vptr) == 0xc0 &&
              offsetof(Slot326EyeVar, fn_aux) == 0xc8);

// ---- objects -----------------------------------------------------------------------------------
inline constexpr std::uint64_t kSlot326BlockRva = 0x3291890;
inline constexpr std::size_t kSlot326BlockBytes = 0x19f0;          // 0x3291890 .. 0x3293280

// The aggregate is exactly the 0x19f0-byte original run; the mirrored 0x2dce400 table lives outside it
// because it is not adjacent to this block in the image.
struct Slot326Objects {
    ConCmdBoolVar bool_eye_state{};         // 0x3291890  +0x000
    ConCmdBoolVar bool_force_angry{};       // 0x3291958  +0x0c8
    ConCmdRangedVar threshold{};            // 0x3291a20  +0x190
    DebugMenuItem group_colossus{};         // 0x3291af8  +0x268
    DebugMenuItem toggle_force_angry{};     // 0x3291ba0  +0x310
    DebugMenuItem toggle_eye_state{};       // 0x3291c48  +0x3b8
    DebugMenuItem toggle_threshold{};       // 0x3291cf0  +0x460
    std::array<Slot326EyeVar, kSlot326EyeCount> eye{};        // 0x3291d98  +0x508, 0xd0 apart
    DebugMenuItem group_eye_color{};        // 0x3292758  +0xec8
    DebugMenuItem item_label{};             // 0x3292800  +0xf70
    DebugMenuItem group_inactive{};         // 0x32928a8  +0x1018
    std::array<DebugMenuItem, 4> slider_inactive{};           // 0x3292950  +0x10c0
    DebugMenuItem group_blue{};             // 0x3292bf0  +0x1360
    std::array<DebugMenuItem, 4> slider_blue{};               // 0x3292c98  +0x1408
    DebugMenuItem group_red{};              // 0x3292f38  +0x16a8
    std::array<DebugMenuItem, 4> slider_red{};                // 0x3292fe0  +0x1750
};
static_assert(offsetof(Slot326Objects, bool_eye_state) == 0x000 &&
              offsetof(Slot326Objects, bool_force_angry) == 0x0c8 &&
              offsetof(Slot326Objects, threshold) == 0x190 &&
              offsetof(Slot326Objects, group_colossus) == 0x268 &&
              offsetof(Slot326Objects, toggle_force_angry) == 0x310 &&
              offsetof(Slot326Objects, toggle_eye_state) == 0x3b8 &&
              offsetof(Slot326Objects, toggle_threshold) == 0x460 &&
              offsetof(Slot326Objects, eye) == 0x508 &&
              offsetof(Slot326Objects, group_eye_color) == 0xec8 &&
              offsetof(Slot326Objects, item_label) == 0xf70 &&
              offsetof(Slot326Objects, group_inactive) == 0x1018 &&
              offsetof(Slot326Objects, slider_inactive) == 0x10c0 &&
              offsetof(Slot326Objects, group_blue) == 0x1360 &&
              offsetof(Slot326Objects, slider_blue) == 0x1408 &&
              offsetof(Slot326Objects, group_red) == 0x16a8 &&
              offsetof(Slot326Objects, slider_red) == 0x1750,
              "the original object addresses are reproduced");
static_assert(offsetof(Slot326Objects, slider_red) + sizeof(std::array<DebugMenuItem, 4>) -
                  offsetof(Slot326Objects, bool_eye_state) == kSlot326BlockBytes,
              "the block spans 0x19f0 bytes");
static_assert(kSlot326BlockRva + kSlot326BlockBytes == 0x3293280, "the block ends where the next one starts");

// One `__cxa_atexit` registration of the body, in body order (every site is tool-pinned).
struct Slot326AtexitRow {
    std::uint32_t step;        // 1..36
    std::uint64_t site_rva;    // the `callq 0xf8f390`
    std::uint64_t dtor_rva;    // rdi (identity tag, never run)
    std::uint64_t object_rva;  // rsi (identity only)
};
inline constexpr std::array<Slot326AtexitRow, kSlot326AtexitRowCount> kSlot326AtexitRows = {{
    {1, 0x838062, kSlot326BoolVarDtorRva, 0x3291890},
    {2, 0x83815c, kSlot326BoolVarDtorRva, 0x3291958},
    {3, 0x83827b, kSlot326FloatVarDtorRva, 0x3291a20},
    {4, 0x838302, kSlot326MenuItemDtorRva, 0x3291af8},
    {5, 0x838386, kSlot326MenuItemDtorRva, 0x3291ba0},
    {6, 0x838410, kSlot326MenuItemDtorRva, 0x3291c48},
    {7, 0x838494, kSlot326MenuItemDtorRva, 0x3291cf0},
    {8, 0x8385a5, kSlot326EyeVarDtorRva, 0x3291d98},
    {9, 0x8386b2, kSlot326EyeVarDtorRva, 0x3291e68},
    {10, 0x8387bd, kSlot326EyeVarDtorRva, 0x3291f38},
    {11, 0x8388c7, kSlot326EyeVarDtorRva, 0x3292008},
    {12, 0x8389dc, kSlot326EyeVarDtorRva, 0x32920d8},
    {13, 0x838af2, kSlot326EyeVarDtorRva, 0x32921a8},
    {14, 0x838c03, kSlot326EyeVarDtorRva, 0x3292278},
    {15, 0x838d16, kSlot326EyeVarDtorRva, 0x3292348},
    {16, 0x838e28, kSlot326EyeVarDtorRva, 0x3292418},
    {17, 0x838f42, kSlot326EyeVarDtorRva, 0x32924e8},
    {18, 0x839053, kSlot326EyeVarDtorRva, 0x32925b8},
    {19, 0x839162, kSlot326EyeVarDtorRva, 0x3292688},
    {20, 0x8391e5, kSlot326MenuItemDtorRva, 0x3292758},
    {21, 0x83922e, kSlot326MenuItemDtorRva, 0x3292800},
    {22, 0x8392a7, kSlot326MenuItemDtorRva, 0x32928a8},
    {23, 0x839338, kSlot326MenuItemDtorRva, 0x3292950},
    {24, 0x8393c9, kSlot326MenuItemDtorRva, 0x32929f8},
    {25, 0x83945d, kSlot326MenuItemDtorRva, 0x3292aa0},
    {26, 0x8394f1, kSlot326MenuItemDtorRva, 0x3292b48},
    {27, 0x839571, kSlot326MenuItemDtorRva, 0x3292bf0},
    {28, 0x839605, kSlot326MenuItemDtorRva, 0x3292c98},
    {29, 0x839699, kSlot326MenuItemDtorRva, 0x3292d40},
    {30, 0x83972d, kSlot326MenuItemDtorRva, 0x3292de8},
    {31, 0x8397c1, kSlot326MenuItemDtorRva, 0x3292e90},
    {32, 0x839841, kSlot326MenuItemDtorRva, 0x3292f38},
    {33, 0x8398d5, kSlot326MenuItemDtorRva, 0x3292fe0},
    {34, 0x839969, kSlot326MenuItemDtorRva, 0x3293088},
    {35, 0x8399fd, kSlot326MenuItemDtorRva, 0x3293130},
    {36, 0x839a91, kSlot326MenuItemDtorRva, 0x32931d8},
}};

// One of the thirteen row-local `callq *0x18(%rax)` sites: (site, argument tag, slot-3 body).
struct Slot326FunctorSite {
    std::uint64_t site_rva;
    std::uint64_t tag_rva;
    std::uint64_t slot3_rva;
    bool slot3_is_bare_ret;      // pinned true; false blocks the fragment
};
inline constexpr std::array<Slot326FunctorSite, kSlot326FunctorSiteCount> kSlot326FunctorSites = {{
    {0x838240, kSlot326FloatArgFnVtbl, kSlot326FloatFnDestroyRva, true},
    {0x838564, kSlot326FloatArgFnVtbl, kSlot326FloatFnDestroyRva, true},
    {0x838674, kSlot326FloatArgFnVtbl, kSlot326FloatFnDestroyRva, true},
    {0x838786, kSlot326FloatArgFnVtbl, kSlot326FloatFnDestroyRva, true},
    {0x838890, kSlot326FloatArgFnVtbl, kSlot326FloatFnDestroyRva, true},
    {0x83899d, kSlot326FloatArgFnVtbl, kSlot326FloatFnDestroyRva, true},
    {0x838ab2, kSlot326FloatArgFnVtbl, kSlot326FloatFnDestroyRva, true},
    {0x838bc5, kSlot326FloatArgFnVtbl, kSlot326FloatFnDestroyRva, true},
    {0x838cdc, kSlot326FloatArgFnVtbl, kSlot326FloatFnDestroyRva, true},
    {0x838df0, kSlot326FloatArgFnVtbl, kSlot326FloatFnDestroyRva, true},
    {0x838efe, kSlot326FloatArgFnVtbl, kSlot326FloatFnDestroyRva, true},
    {0x839015, kSlot326FloatArgFnVtbl, kSlot326FloatFnDestroyRva, true},
    {0x83912a, kSlot326FloatArgFnVtbl, kSlot326FloatFnDestroyRva, true},
}};

// ---- stages / results ---------------------------------------------------------------------------
enum class Slot326Stage : std::uint8_t {
    none,
    bool_var_build,
    bool_var_atexit,
    float_var_build,
    float_var_view_release,
    float_var_registration,
    float_var_atexit,
    item_ctor,
    item_registration,
    item_view_release,
    item_atexit,
    canary,
};
[[nodiscard]] const char* slot326_stage_name(Slot326Stage stage) noexcept;

struct Slot326Hooks {
    // The allocator pair behind 0x12820 / 0x12af0 that 0x26800 needs for "MenuSeparator".
    // NOT lifted: `default_slot326_hooks()` installs the documented malloc/free stand-in (the same
    // convention as the registered slot 306 and slot 316). Clearing it fails the fragment at
    // `item_ctor` for the label row.
    SharedStrCtorHooks string_alloc{};
    // __stack_chk_guard source (0x15fe518). Null => the comparison is not performed and
    // `canary_checked` stays false; a changed guard fails the fragment at stage `canary`.
    const std::uint64_t* stack_guard = nullptr;
};

// The net state of 0xc300 for one pinned float row. Refuses (writes nothing) when the argument tag
// is not the pinned 0x15cec00. Two shapes: `ConCmdRangedVar` (the threshold, 0xd8) and
// `Slot326EyeVar` (the eye colours, 0xd0). Identical stores; only the object width differs.
[[nodiscard]] bool slot326_float_c300_state(Slot326EyeVar& var, const Slot326FloatSpec& spec,
                                            const RangedVarArgs& args,
                                            Slot326EyeValues& values) noexcept;
[[nodiscard]] bool slot326_float_c300_state(ConCmdRangedVar& var, const Slot326FloatSpec& spec,
                                            const RangedVarArgs& args,
                                            Slot326EyeValues& values) noexcept;

// 0x26800 (173 B): the kind-0 "MenuSeparator" row. link, the caller's label view with its counter
// acquire, kind 3 then 0, "MenuSeparator" through the native 0xf1a0, the empty view through 0xf180,
// target 0, range {1.0, 10.0}, the caller's flags. Refuses before the first store when the allocator
// is missing or the allocation fails (the original would have faulted).
[[nodiscard]] bool init_slot326_label_item_26800(DebugMenuItem& item, const SharedStr& label,
                                                 std::uint32_t flags,
                                                 const SharedStrCtorHooks& hooks) noexcept;

// The production hooks: the malloc-backed "MenuSeparator" allocator.
[[nodiscard]] Slot326Hooks default_slot326_hooks() noexcept;

// The 0x26ea0 gate answer of a target object, read from its own vptr tag (never a hook). Knows the
// two classes this body targets: the bool variable class {1, 0} and the float variable class {1, 3}.
// Any other tag is unknown and blocks the fragment.
[[nodiscard]] DebugMenuTarget slot326_menu_target(const ConCmdBase& cmd) noexcept;

// The lifted `callq *0x18(%rax)` for the thirteen row-local sites: a bare `ret` has no effect.
[[nodiscard]] bool slot326_inert_functor_destroy(const Slot326FunctorSite& site) noexcept;

// The stand-ins the aggregate holds, by index.
[[nodiscard]] ConCmdBase& slot326_bool_base(Slot326Objects& objects, std::uint32_t index) noexcept;
[[nodiscard]] ConCmdBase& slot326_float_base(Slot326Objects& objects, std::uint32_t index) noexcept;
[[nodiscard]] DebugMenuItem& slot326_item(Slot326Objects& objects, std::uint32_t index) noexcept;
[[nodiscard]] std::uint32_t* slot326_value_cell(Slot326EyeValues& values,
                                                const Slot326FloatSpec& spec) noexcept;

struct Slot326FloatResult {
    bool ok = false;
    bool name_inert = false;        // the 0xf590 on the stack name view
    bool description_inert = false; // the 0xf590 on the stack description view
    bool functor_inert = false;     // the row-local `[tag+0x18]` is the pinned bare ret
    ConCmdRegisterResult registration{};
};

struct Slot326Result {
    bool ok = false;
    Slot326Stage blocked = Slot326Stage::none;
    std::uint32_t blocked_index = 0;               // the row index for the per-object stages
    std::uint32_t atexit_records = 0;              // this body's 36 __cxa_atexit registrations
    std::uint32_t functors_destroyed = 0;          // the thirteen `callq *0x18(%rax)` sites
    std::uint32_t vars_built = 0;                  // 2 bools + 13 floats
    std::uint32_t menu_items = 0;                  // 21
    std::array<BoolVarResult, kSlot326BoolCount> bool_vars{};
    std::array<Slot326FloatResult, kSlot326FloatCount> float_vars{};
    std::array<MenuRegisterResult, kSlot326ItemCount> item_registrations{};
    bool views_inert = true;                       // every 0xf590 call of this body was a no-op
    bool eye_values_mirrored = false;              // the 0x2dce400 table was seeded from the pinned bits
    bool canary_checked = false;                   // false when the host supplied no guard
    bool canary_matched = false;
};

// Runs original constructor 326 natively. The original is unconditional, so the first failing step
// stops the slot (nothing after it is claimed); every failure names its stage and row index.
[[nodiscard]] Slot326Result init_slot326_837f30(Slot326Objects& objects, Slot326EyeValues& eye_values,
                                                ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                                const Slot326Hooks& hooks = {}) noexcept;

}  // namespace sotc
