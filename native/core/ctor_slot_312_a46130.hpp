// CPU-01-STARTUP-SLOT-312-CHARACTERIZATION-01 / -NATIVE-LIFT-01: native recovery of original
// constructor-table index 312 / RVA 0xa46130 (1,250 B FDE [0xa46130, 0xa46612); 238 reachable
// instructions, 1,249 B reachable), ELF sha256 d0723afb...5861d3. Offline analysis only: nothing
// here loads, relocates or executes an original RVA. Byte oracle: tools/lift/check_ctor_slot_312.py.
//
// ============================ characterization (from the pinned bytes) ============================
// Reached by DATA only: the startup table word at 0x2dd70e0 - 312*8 = 0x2dd6720 holds 0xa46130.
// Body shape from the pinned llvm-objdump listing: 238 reachable instructions, ONE conditional
// branch (0xa465f8 `jne 0xa4660c`, the __stack_chk_guard compare), 30 direct call sites over 9
// distinct callees, two `callq *0x18(%rax)` functor-slot-3 sites (0xa4626c / 0xa46320), 49
// rip-relative operands (8 of them stores), zero outside-image references, and the frame
//     push rbp; mov rsp,rbp; push r15/r14/r13/r12/rbx; sub rsp,0x88
//     guard 0xc0.. -> -0x30(%rbp) <- *[0x15fe518]   (__stack_chk_guard GOT cell)
//     ...
//     cmp -0x30(%rbp),%rax ; jne 0xa4660c ; lea -0x28(%rbp),%rsp; pop rbx..r15; pop rbp; ret
//   0xa4660c: callq 0xf8f3a0 (__stack_chk_fail) then the body's last byte (0xa46611) is a nop.
//
// This is the **Entitlement / Platform Services title-info block**, the sibling of slot 310
// (`titleid`): it builds three console variables and four debug-menu items, then one 0x80 B record.
// The strings are the evidence: `titleskuid` (0x12c2e80, 0xa) / `DEFAULT` (0x12c2e8b, 7),
// `titleAgeRestriction` (0x12c2e93, 0x13), `titleRegion` (0x12c2ea7, 0xb), `Platform Services`
// (0x12c2eb3, 0x11), `TitleInfo` (0x12c2ec5, 9), `Sku Name` (0x12c2ecf, 8), `Age Restriction`
// (0x12c2ed8, 0xf), `Region` (0x12c2ee8, 6); the neighbouring rodata also carries the
// `PS4EntitlementWorker.cpp` path and the `ETitleRegion::*` names.
//
// Callee histogram (site -> target, all 30 pinned by the checker):
//   0xf590 x12 (shared release of a borrowed stack view; every site is an exact no-op)
//   0xf8f390 x8 (__cxa_atexit PLT, all with third argument 0x1604218) + 0xf8f3a0 x1 (guard fail)
//   0x257a0 x1  string-variable ctor `titleskuid` (already native: core/string_var_257a0)
//   0xa45ba0 x1 class-A int-variable wrapper (titleAgeRestriction)
//   0xa45de0 x1 class-B int-variable wrapper (titleRegion)
//   0x26c10 x1  menu GROUP ctor ("Platform Services" / "TitleInfo")
//   0x26ea0 x3  menu TOGGLE ctor (Sku Name / Age Restriction / Region)
//   0xf180 x2   empty SharedStr ctor (the record's two views)
//
// The eight __cxa_atexit records, in body order (site, dtor, object):
//   1 0xa461d3 0x23c90  0x32f6f58  string variable            (dtor sets vptr 0x1594ef0, releases +0x128/+0x68)
//   2 0xa46287 0x18a60  0x32f70a0  int variable, class A       (dtor destroys the functor at +0xa0, 0x20b50)
//   3 0xa46338 0xa44a80 0x32f7178  int variable, class B       (same shape, class B tag)
//   4 0xa463c6 0x272d0  0x32f7250  menu group item
//   5 0xa46447 0x272d0  0x32f72f8  toggle "Sku Name"
//   6 0xa464cf 0x272d0  0x32f73a0  toggle "Age Restriction"
//   7 0xa46557 0x272d0  0x32f7448  toggle "Region"
//   8 0xa465e5 0xa44ac0 0x32f74f0  the 0x80 B record          (dtor releases +0x60, +0x40, then this)
//
// ---------------------------- the two int variables, and the E1022 caveat ----------------------------
// `0xa45ba0` / `0xa45de0` are 442 B WRAPPERS of the 323 B leaves `0x3a980` / `0xa45fa0`. That pair is
// the same template as the `0xa4f4d0` family (see core/ranged_bound_var_a4f4d0.hpp) and the same
// "unbounded int" shape already native for slots 334 / 486 / 489 (core/ctor_slots_322_334,
// init_unbounded_int_var_worker_3a980) -- with three differences that are pinned here:
//   * the caller's argument block is the 0x50 B `RangedVarArgs` (q[0..3], source functor at +0x20,
//     fn vptr at +0x40, fn aux at +0x48), not the stack-passed slot-309 form;
//   * the value cell is the object's OWN +0xd0 (`lea rsi,[rbx+0xd0]` -> rdx), so +0x60 is a
//     self-pointer here -- unlike slot 309, nothing dangles;
//   * both bounds are disengaged (the leaf writes byte 0 at +0x6c/+0x70/+0x74/+0x78 and no dword).
// Class rows (all identity tags, pinned through .data.rel.ro RELATIVE relocations):
//   class A (titleAgeRestriction, wrapper 0xa45ba0, leaf 0x3a980)
//     final vptr 0x1594c08, mid vptr 0x15941a0, seed tag 0x1595660,
//     argument tag 0x15d18a0 -> slots 0xc720 copy / 0xc740 clone / 0xc760 move / 0xc770 bare-ret destroy,
//     final functor tag 0x15944a0 (what the copy body stores), dtor 0x18a60,
//     0x26ea0 gate answers {slot3 = 0xbf70 -> 1, slot6 = 0xbfc0 -> 2}.
//   class B (titleRegion, wrapper 0xa45de0, leaf 0xa45fa0)
//     final vptr 0x15d18f0, mid vptr 0x15d1818, seed tag 0x15d18c0,
//     argument tag 0x15d18c0 -> slots 0xa45d60 / 0xa45d80 / 0xa45da0 / 0xa45db0 (byte-identical
//     bodies to the 0xc720 family except the tag they store: 0x15d18c0),
//     final functor tag 0x15d18c0, dtor 0xa44a80,
//     0x26ea0 gate answers {slot3 = 0xa44f20 -> 1, slot6 = 0xa451f0 -> 1}.
// Both wrappers pass flags = 1 and the value cell holds the caller's stack temporary: -1 for
// titleAgeRestriction (`movl $0xffffffff,-0xa4(%rbp)`) and 0 for titleRegion. The wrapper reads that
// cell twice after the leaf and stores it to +0xd0 and +0x68, so value == default at both sites.
//
// ======================= the E1022 indirect-site accounting, resolved =======================
// E1022 recorded "at least 18 unresolved nested indirect sites in selected descendants". Every one of
// them is a functor-vtable slot call of this template, and this module pins each target body:
//     row-local                        2 sites  (0xa4626c, 0xa46320 -> 0xc770 / 0xa45db0, bare `ret`)
//     wrapper + leaf, class A         11 sites  (6 + 5; targets 0xc720 / 0xc740 / 0xc760 / 0xc770)
//     wrapper + leaf, class B         11 sites  (6 + 5; targets 0xa45d60 / 0xa45d80 / 0xa45da0 / 0xa45db0)
//     already native via 0x257a0      1 site   (0x25856 -> 0x25aa0, bare `ret`)
//     already native via 0x258f0      5 sites  (0x1595068 slots 0x25a50 / 0x25a70 / 0x25a90 / 0x25aa0)
// The bodies of 0xc720 / 0xc740 / 0xc760 / 0xc770 are 20/20/9/1 B: `copy` stores the source payload
// at dst+0x10 and the class tag at dst+0x20; `clone` stores the source tag at dst+0x20 and then the
// payload; `move` copies the payload; `destroy` is one `c3`. The 0xa45d6x/8x/a0/b0 four are the same
// four bodies with tag 0x15d18c0. So the dispatch is fully determined and is modelled as its net
// state, exactly like core/ranged_bound_var_a4f4d0 does for the three slot-309 instantiations.
// The four 0x26ea0 gate reads per item are answered from the pinned vptr tags, not from a hook.
//
// ================================ modelled vs hook (G2) =================================
// MODELLED (native, exact): the prologue/epilogue and canary guard; the `titleskuid` string variable
// (existing native 0x257a0/0x258f0 model) with both stack-view releases and its atexit record; the two
// int variables through `init_slot312_int_var` (net state of wrapper + leaf, 11 collapsed dispatches
// each) with their description release, final vptr, value/default pair, 0x20d60 registration and atexit
// records; the menu group through the native 0x26c10 model + 0x268b0 registration; the three toggles
// through the native 0x26ea0 model + registration; the record's eight stores, its two empty views and
// its atexit record; all eight atexit records in body order.
// HOOK (optional override, no default): `Slot312Hooks::var_ctor` still receives the pinned
// Slot312IntVarSpec and owns the object interior. `default_slot312_hooks()` installs the native model;
// a null or failing hook stops the fragment at that variable's stage BEFORE its atexit record.
//
// Fail-visible contracts (all reported through `blocked` / `ok == false`, never a silent pass):
//   * a null or failing `var_ctor` (the default hook fails when the class row does not match);
//   * a `0xf590` release that is not the exact inert no-op of a borrowed view;
//   * a failed 0x20d60 / 0x268b0 registration;
//   * a failed `__cxa_atexit` record;
//   * a `__stack_chk_guard` that changed across the body (the original would call __stack_chk_fail at
//     0xa4660c). Without a host-supplied guard the comparison is not claimed: `canary_checked` stays
//     false and the result never reports a silent pass.
//
// NOT modelled: every destructor body (0x23c90 / 0x18a60 / 0xa44a80 / 0x272d0 / 0xa44ac0) and the
// four callbacks the two argument blocks carry as DATA (0x52dab0 / 0xa45dc0 at q[1], 0xa44a40 /
// 0xa44a60 at q[3]); they are pinned identity tags, recorded and never executed. Nothing consumes the
// three variables or the record yet, so no title/entitlement/PSN behaviour is claimed.
//
// The record 0x32f74f0 (0x80 B) is modelled as its raw stores: a borrowed "DEFAULT" view, dword 0 at
// +0x20, {1, 7, 0xffffffff, 0xffffffff} at +0x24, dword 0 at +0x34, 0xffffffff at +0x38, 0x72 at
// +0x3c, then two empty SharedStr at +0x40 and +0x60. The field meanings are NOT claimed.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"
#include "fmenu_ctors_slot306.hpp"
#include "l2_slot_common.hpp"
#include "string_var_257a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot312Rva = 0xa46130;
inline constexpr std::size_t kSlot312BodySize = 1250;             // FDE [0xa46130, 0xa46612)
inline constexpr std::uint64_t kSlot312EndRva = 0xa46612;
inline constexpr std::size_t kSlot312ReachableBytes = 1249;       // last reachable insn ends at 0xa46611
inline constexpr std::size_t kSlot312InstructionCount = 238;      // pinned listing; not re-derived here
inline constexpr std::size_t kSlot312DirectCallSites = 30;
inline constexpr std::size_t kSlot312IndirectCallSites = 2;
inline constexpr std::size_t kSlot312RipOperands = 49;
inline constexpr std::size_t kSlot312StoreSites = 8;
inline constexpr std::uint64_t kSlot312TableTopRva = 0x2dd70e0;   // index i at TOP - 8*i
inline constexpr std::uint64_t kSlot312TableWordRva = 0x2dd6720;  // 0x2dd70e0 - 312*8
inline constexpr std::uint32_t kSlot312TableIndex = 312;
inline constexpr std::uint32_t kSlot312NextUnrecoveredIndex = 314;      // 314 / 0xa41810
inline constexpr std::uint64_t kSlot312NextUnrecoveredRva = 0xa41810;

// Frame / guard (same shape as slot 309, different frame size).
inline constexpr std::uint64_t kSlot312CanaryCellRva = 0x15fe518;  // GOT cell of __stack_chk_guard
inline constexpr std::uint32_t kSlot312CanaryFrameOffset = 0x30;   // -0x30(%rbp)
inline constexpr std::uint64_t kSlot312StackChkFailRva = 0xf8f3a0;
inline constexpr std::uint64_t kSlot312BranchRva = 0xa465f8;       // jne 0xa4660c (only branch)
inline constexpr std::uint64_t kSlot312BranchTargetRva = 0xa4660c;
inline constexpr std::uint32_t kSlot312FrameBytes = 0x88;          // sub rsp,0x88

// Shared callees.
inline constexpr std::uint64_t kSlot312AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot312DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot312ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot312ConCmdRegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot312MenuGroupCtorRva = 0x26c10;
inline constexpr std::uint64_t kSlot312MenuToggleCtorRva = 0x26ea0;
inline constexpr std::uint64_t kSlot312MenuRegisterRva = 0x268b0;
inline constexpr std::uint64_t kSlot312MenuItemDtorRva = 0x272d0;
inline constexpr std::uint64_t kSlot312EmptyViewCtorRva = 0xf180;
inline constexpr std::uint64_t kSlot312StringVarCtorRva = 0x257a0;

// Original objects (identity only; the native stand-ins live in Slot312Objects).
inline constexpr std::uint64_t kSlot312TitleSkuRva = 0x32f6f58;        // ConCmdStringVar, 0x148
inline constexpr std::uint64_t kSlot312AgeRestrictionRva = 0x32f70a0;  // ConCmdRangedVar, 0xd8
inline constexpr std::uint64_t kSlot312RegionRva = 0x32f7178;          // ConCmdRangedVar, 0xd8
inline constexpr std::uint64_t kSlot312GroupRva = 0x32f7250;           // DebugMenuItem, 0xa8
inline constexpr std::uint64_t kSlot312ToggleSkuRva = 0x32f72f8;       // DebugMenuItem, 0xa8
inline constexpr std::uint64_t kSlot312ToggleAgeRva = 0x32f73a0;       // DebugMenuItem, 0xa8
inline constexpr std::uint64_t kSlot312ToggleRegionRva = 0x32f7448;    // DebugMenuItem, 0xa8
inline constexpr std::uint64_t kSlot312RecordRva = 0x32f74f0;          // the 0x80 B title record

// Destructors (identity tags, recorded by __cxa_atexit, never run).
inline constexpr std::uint64_t kSlot312StringVarDtorRva = 0x23c90;
inline constexpr std::uint64_t kSlot312RecordDtorRva = 0xa44ac0;
inline constexpr std::uint64_t kSlot312RecordSize = 0x80;

// Class rows of the two int variables (see the header comment for the full pin list).
struct Slot312IntVarClass {
    std::uint64_t wrapper_rva;
    std::uint64_t leaf_rva;
    std::uint64_t final_vptr;      // [obj] written by the wrapper before and after the leaf
    std::uint64_t mid_vptr;        // [obj] written by the leaf
    std::uint64_t seed_fn_tag;     // [obj+0xc0] the leaf stores before the functor install
    std::uint64_t arg_fn_tag;      // the caller block's fn vptr (slot 0/1/3 bodies)
    std::uint64_t final_fn_tag;    // [obj+0xc0] after the install (what the copy body stores)
    std::uint64_t dtor_rva;        // the caller's __cxa_atexit rdi
    std::uint32_t slot3;           // 0x26ea0 gate: [vptr+0x18]() result
    std::uint32_t slot6;           // 0x26ea0 gate: [vptr+0x30]() result
};
inline constexpr std::size_t kSlot312IntVarClassCount = 2;
inline constexpr std::array<Slot312IntVarClass, kSlot312IntVarClassCount> kSlot312IntVarClasses = {{
    {0xa45ba0, 0x3a980, 0x1594c08, 0x15941a0, 0x1595660, 0x15d18a0, 0x15944a0, 0x18a60, 1, 2},
    {0xa45de0, 0xa45fa0, 0x15d18f0, 0x15d1818, 0x15d18c0, 0x15d18c0, 0x15d18c0, 0xa44a80, 1, 1},
}};
// One original int-variable construction performs 11 functor-vtable dispatches (6 in the wrapper,
// 5 in the leaf). The model performs their net state and reports this count.
inline constexpr std::uint32_t kSlot312VarFunctorDispatches = 11;
inline constexpr std::uint32_t kSlot312RowLocalFunctorSites = 2;
inline constexpr std::uint32_t kSlot312CtorFlags = 1;
inline constexpr std::size_t kSlot312WrapperSize = 0x1ba;   // 442 B FDE of both wrappers
inline constexpr std::size_t kSlot312LeafSize = 0x143;      // 323 B FDE of both leaves

// One int-variable site of the body.
struct Slot312IntVarSpec {
    std::uint64_t wrapper_rva;     // 0xa45ba0 / 0xa45de0
    std::uint64_t leaf_rva;        // 0x3a980 / 0xa45fa0
    std::uint64_t dtor_rva;        // 0x18a60 / 0xa44a80
    const char* name;              // name view data
    std::uint32_t name_len;
    std::uint64_t arg_q1;          // second qword of the caller block (resolved GOT value)
    std::uint64_t arg_q3;          // fourth qword (the value-changed callback tag)
    std::uint32_t value;           // the caller's stack cell, loaded twice after the leaf
};

inline constexpr std::array<Slot312IntVarSpec, 2> kSlot312IntVars = {{
    {0xa45ba0, 0x3a980, 0x18a60, "titleAgeRestriction", 0x13, 0x52dab0, 0xa44a40, 0xffffffffu},
    {0xa45de0, 0xa45fa0, 0xa44a80, "titleRegion", 0xb, 0xa45dc0, 0xa44a60, 0u},
}};

// Strings of the group (identity addresses + pinned lengths; the native views are literals).
inline constexpr char kSlot312SkuName[] = "titleskuid";                 // 0x12c2e80, 0xa
inline constexpr char kSlot312SkuDefault[] = "DEFAULT";                 // 0x12c2e8b, 7
inline constexpr char kSlot312AgeName[] = "titleAgeRestriction";        // 0x12c2e93, 0x13
inline constexpr char kSlot312RegionName[] = "titleRegion";             // 0x12c2ea7, 0xb
inline constexpr char kSlot312GroupParent[] = "Platform Services";      // 0x12c2eb3, 0x11
inline constexpr char kSlot312MenuLabel[] = "TitleInfo";                // 0x12c2ec5, 9
inline constexpr char kSlot312ToggleSkuText[] = "Sku Name";             // 0x12c2ecf, 8
inline constexpr char kSlot312ToggleAgeText[] = "Age Restriction";      // 0x12c2ed8, 0xf
inline constexpr char kSlot312ToggleRegionText[] = "Region";            // 0x12c2ee8, 6
inline constexpr char kSlot312EmptyText[] = "";                         // 0x12c2fa6 (the vars' description)
static_assert(sizeof kSlot312SkuName - 1 == 0xa && sizeof kSlot312SkuDefault - 1 == 7);
static_assert(sizeof kSlot312AgeName - 1 == 0x13 && sizeof kSlot312RegionName - 1 == 0xb);
static_assert(sizeof kSlot312GroupParent - 1 == 0x11 && sizeof kSlot312MenuLabel - 1 == 9);
static_assert(sizeof kSlot312ToggleSkuText - 1 == 8 && sizeof kSlot312ToggleAgeText - 1 == 0xf);
static_assert(sizeof kSlot312ToggleRegionText - 1 == 6);

inline constexpr std::uint64_t kSlot312GroupParentRva = 0x12c2eb3;
inline constexpr std::uint64_t kSlot312MenuLabelRva = 0x12c2ec5;
inline constexpr std::uint64_t kSlot312SkuTextRva = 0x12c2ecf;
inline constexpr std::uint64_t kSlot312AgeTextRva = 0x12c2ed8;
inline constexpr std::uint64_t kSlot312RegionTextRva = 0x12c2ee8;
inline constexpr std::uint64_t kSlot312VarDescRva = 0x12c2fa6;
inline constexpr std::uint32_t kSlot312ItemFlags = 0x30;

// The 0x80 B record's raw stores (0x32f74f0). Field meanings are not claimed.
inline constexpr std::uint64_t kSlot312RecordNameRva = 0x12c2e8b;   // borrowed "DEFAULT" view
inline constexpr std::uint32_t kSlot312RecordWord20 = 0x0;
inline constexpr std::array<std::uint32_t, 4> kSlot312RecordBlock24 = {{0x1, 0x7, 0xffffffffu, 0xffffffffu}};
inline constexpr std::uint64_t kSlot312RecordBlockRva = 0x12c2e60;  // the 16 B literal the body copies
inline constexpr std::uint32_t kSlot312RecordWord34 = 0x0;
inline constexpr std::uint32_t kSlot312RecordWord38 = 0xffffffffu;
inline constexpr std::uint32_t kSlot312RecordWord3c = 0x72;

// One `__cxa_atexit` registration of the body, in body order (pinned sites are tool-checked).
struct Slot312AtexitRow {
    std::uint32_t step;          // 1..8
    std::uint64_t site_rva;      // the `callq 0xf8f390`
    std::uint64_t dtor_rva;      // rdi (identity tag, never run)
    std::uint64_t object_rva;    // rsi (identity only)
};
inline constexpr std::size_t kSlot312AtexitRowCount = 8;
inline constexpr std::array<Slot312AtexitRow, kSlot312AtexitRowCount> kSlot312AtexitRows = {{
    {1, 0xa461d3, 0x23c90, 0x32f6f58},
    {2, 0xa46287, 0x18a60, 0x32f70a0},
    {3, 0xa46338, 0xa44a80, 0x32f7178},
    {4, 0xa463c6, 0x272d0, 0x32f7250},
    {5, 0xa46447, 0x272d0, 0x32f72f8},
    {6, 0xa464cf, 0x272d0, 0x32f73a0},
    {7, 0xa46557, 0x272d0, 0x32f7448},
    {8, 0xa465e5, 0xa44ac0, 0x32f74f0},
}};

// The two row-local `callq *0x18(%rax)` sites: the caller's argument functor copy, destroyed through
// the class tag's slot 3 (a bare `ret` at both).
struct Slot312FunctorSite {
    std::uint64_t site_rva;
    std::uint64_t tag_rva;
    std::uint64_t slot3_rva;
    bool slot3_is_bare_ret;      // pinned true; false blocks the fragment
};
inline constexpr std::size_t kSlot312FunctorSiteCount = 2;
inline constexpr std::array<Slot312FunctorSite, kSlot312FunctorSiteCount> kSlot312FunctorSites = {{
    {0xa4626c, 0x15d18a0, 0xc770, true},
    {0xa46320, 0x15d18c0, 0xa45db0, true},
}};

// ---- native stand-ins -------------------------------------------------------------------------
struct Slot312Record {                     // 0x32f74f0, 0x80 B
    SharedStr name{};                      // +0x00 borrowed "DEFAULT" view
    std::uint32_t word20 = 0;              // +0x20 dword store of 0
    std::array<std::uint32_t, 4> block24{};  // +0x24 the 16 B copy of 0x12c2e60
    std::uint32_t word34 = 0;              // +0x34 dword store of 0
    std::uint32_t word38 = 0;              // +0x38 dword store of 0xffffffff
    std::uint32_t word3c = 0;              // +0x3c dword store of 0x72
    SharedStr empty40{};                   // +0x40 (0xf180)
    SharedStr empty60{};                   // +0x60 (0xf180)
};
static_assert(sizeof(Slot312Record) == 0x80 && offsetof(Slot312Record, word20) == 0x20 &&
              offsetof(Slot312Record, block24) == 0x24 && offsetof(Slot312Record, word34) == 0x34 &&
              offsetof(Slot312Record, word38) == 0x38 && offsetof(Slot312Record, word3c) == 0x3c &&
              offsetof(Slot312Record, empty40) == 0x40 && offsetof(Slot312Record, empty60) == 0x60);

struct Slot312Objects {
    ConCmdStringVar title_sku{};      // 0x32f6f58  +0x0008
    ConCmdRangedVar age_restriction{};  // 0x32f70a0  +0x148
    ConCmdRangedVar region{};         // 0x32f7178  +0x220
    DebugMenuItem group{};            // 0x32f7250  +0x2f8
    DebugMenuItem toggle_sku{};       // 0x32f72f8  +0x3a0
    DebugMenuItem toggle_age{};       // 0x32f73a0  +0x448
    DebugMenuItem toggle_region{};    // 0x32f7448  +0x4f0
    Slot312Record record{};           // 0x32f74f0  +0x598
};
static_assert(offsetof(Slot312Objects, age_restriction) == 0x148 &&
              offsetof(Slot312Objects, region) == 0x220 && offsetof(Slot312Objects, group) == 0x2f8 &&
              offsetof(Slot312Objects, toggle_sku) == 0x3a0 &&
              offsetof(Slot312Objects, toggle_age) == 0x448 &&
              offsetof(Slot312Objects, toggle_region) == 0x4f0 &&
              offsetof(Slot312Objects, record) == 0x598 && sizeof(Slot312Objects) == 0x618,
              "Slot312Objects must reproduce the original .bss strides");
static_assert(kSlot312TitleSkuRva + 0x148 == kSlot312AgeRestrictionRva &&
              kSlot312AgeRestrictionRva + 0xd8 == kSlot312RegionRva &&
              kSlot312RegionRva + 0xd8 == kSlot312GroupRva &&
              kSlot312GroupRva + 0xa8 == kSlot312ToggleSkuRva &&
              kSlot312ToggleSkuRva + 0xa8 == kSlot312ToggleAgeRva &&
              kSlot312ToggleAgeRva + 0xa8 == kSlot312ToggleRegionRva &&
              kSlot312ToggleRegionRva + 0xa8 == kSlot312RecordRva,
              "the original object addresses follow the same strides");

enum class Slot312Stage : std::uint8_t {
    none,
    title_sku_build,
    title_sku_view_release,
    title_sku_atexit,
    age_restriction_build,
    age_restriction_view_release,
    age_restriction_atexit,
    region_build,
    region_view_release,
    region_atexit,
    group_ctor,
    group_registration,
    group_view_release,
    group_atexit,
    toggle_sku_ctor,
    toggle_sku_registration,
    toggle_sku_view_release,
    toggle_sku_atexit,
    toggle_age_ctor,
    toggle_age_registration,
    toggle_age_view_release,
    toggle_age_atexit,
    toggle_region_ctor,
    toggle_region_registration,
    toggle_region_view_release,
    toggle_region_atexit,
    record_stores,
    record_views,
    record_atexit,
    canary,
};
[[nodiscard]] const char* slot312_stage_name(Slot312Stage stage) noexcept;

struct Slot312IntVarResult {
    bool ok = false;
    std::uint32_t functor_slot_calls = 0;   // kSlot312VarFunctorDispatches once the net state is modelled
    bool description_inert = false;         // the 0xf590 on the wrapper's stack description view
    bool local_functor_inert = false;       // `call [arg tag + 0x18]` is the pinned bare ret
    ConCmdRegisterResult registration{};
};

struct Slot312Hooks {
    // 0xa45ba0 / 0xa45de0. NO default in this struct: a missing hook stops the fragment at that
    // variable's stage. `default_slot312_hooks()` installs the native net-state model.
    bool (*var_ctor)(ConCmdRangedVar& var, const Slot312IntVarSpec& spec,
                     ConCmdRegistry& registry) noexcept = nullptr;
    // __stack_chk_guard source (0x15fe518). Null => the comparison is not performed and
    // `canary_checked` stays false; a changed guard fails the fragment at stage `canary`.
    const std::uint64_t* stack_guard = nullptr;
};

// The pinned class row whose wrapper_rva is `wrapper_rva`, or nullptr.
[[nodiscard]] const Slot312IntVarClass* slot312_int_var_class_for(std::uint64_t wrapper_rva) noexcept;

// The native model of one wrapper + leaf pair: prologue state of the wrapper, the leaf's stores, the
// argument copy with the q[3] self-pointer rule, the collapsed functor install, the description
// release, the final vptr, the value/default pair read twice from the caller's cell, and the 0x20d60
// registration. Refuses (writes nothing) on an unknown class row or a mismatched spec.
[[nodiscard]] Slot312IntVarResult init_slot312_int_var(ConCmdRangedVar& var, const Slot312IntVarSpec& spec,
                                                       ConCmdRegistry& registry) noexcept;

// The production hooks: the native int-variable model, no guard.
[[nodiscard]] Slot312Hooks default_slot312_hooks() noexcept;

// The 0x26ea0 gate answer of an int variable, read from the stand-in's own vptr tag (never a hook).
[[nodiscard]] DebugMenuTarget slot312_int_var_menu_target(const ConCmdRangedVar& var) noexcept;

// The lifted `callq *0x18(%rax)` for the two row-local sites: a bare `ret` has no effect. Returns
// false (visible) when the site's slot 3 is not the pinned bare ret.
[[nodiscard]] bool slot312_inert_functor_destroy(const Slot312FunctorSite& site) noexcept;

struct Slot312Result {
    bool ok = false;
    Slot312Stage blocked = Slot312Stage::none;
    std::uint32_t atexit_records = 0;          // this body's 8 __cxa_atexit registrations
    std::uint32_t functors_destroyed = 0;      // the two row-local `callq *0x18(%rax)` sites
    std::uint32_t var_functor_calls = 0;       // collapsed dispatches (11 per int variable)
    std::uint32_t vars_built = 0;              // 1 string + 2 int variables
    std::uint32_t menu_items = 0;              // one group + three toggles
    StringVarResult string_var{};
    std::array<Slot312IntVarResult, 2> int_vars{};
    MenuRegisterResult group_registration{};
    std::array<MenuRegisterResult, 3> toggle_registrations{};
    bool views_inert = true;                   // every 0xf590 call of this body was a no-op
    bool canary_checked = false;                // false when the host supplied no guard
    bool canary_matched = false;
};

// Runs original constructor 312 natively. The original is unconditional, so the first failing step
// stops the slot (nothing after it is claimed); every failure names its stage.
[[nodiscard]] Slot312Result init_slot312_a46130(Slot312Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu,
                                                const Slot312Hooks& hooks = {}) noexcept;

}  // namespace sotc
