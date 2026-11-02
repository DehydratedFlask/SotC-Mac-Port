// CPU-01-STARTUP-SLOT-316-CHARACTERIZATION-01 / -NATIVE-LIFT-01: native recovery of original
// constructor-table index 316 / RVA 0xa33280 (2,561 B FDE [0xa33280, 0xa33c81); 2,560 B reachable over
// 473 instructions, the last byte 0xa33c80 being the trailing nop), ELF sha256 d0723afb...5861d3.
// Offline analysis only: nothing here loads, relocates or executes an original RVA. Byte oracle:
// tools/lift/check_ctor_slot_316.py.
//
// ============================ characterization (from the pinned bytes) ============================
// Reached by DATA only: the startup table word at 0x2dd70e0 - 316*8 = 0x2dd6700 holds 0xa33280.
// Body shape from the pinned listing: 473 reachable instructions, 54 direct call sites over 9
// targets, 5 indirect call sites (four `callq *0x18(%rax)`, one `callq *0x20(%rax)`), 88
// rip-relative operands (11 of them stores over 9 distinct addresses), 62 distinct rip targets,
// zero outside-image references, exactly two conditional branches (0xa338a7 `je`, the functor
// self-pointer fix-up, which this body never takes because the queued callback is non-zero, and
// 0xa33c6a `jne`, the __stack_chk_guard compare), and the frame
//     push rbp; mov rsp,rbp; push r15/r14/r13/r12/rbx; and rsp,-0x20; sub rsp,0xe0
//     guard <- *[0x15fe518]  (the GOT cell holds the runtime address of __stack_chk_guard)
//     ...
//     cmp [rsp+0xc0],%rax ; jne 0xa33c7b ; lea -0x28(%rbp),%rsp; pops; ret
//   0xa33c7b: callq 0xf8f3a0 (__stack_chk_fail); the FDE ends exactly at 0xa33c81.
// Normal path = three basic blocks (entry, the a338a9..a338b0 skip, its join at a338b1) plus the
// unpicked __stack_chk_fail tail at 0xa33c7b.
//
// This is the **Localization block**: four language console variables, one bool variable, one
// console command and six "Localization" debug-menu toggles, then the process-wide
// "!!!Unlocalized!!!" string. The strings are the evidence: `loc_text` (0x12c1820, 8) /
// `loc_subtitles` (0x12c1848, 0xd) / `loc_speech` (0x12c187a, 0xa) / `loc_system` (0x12c18a6, 0xa),
// their descriptions `Change language for Text usage` (0x12c1829, 0x1e) / `... for Subtitles usage`
// (0x12c1856, 0x23) / `... for Speech usage` (0x12c1885, 0x20) / `... for System usage`
// (0x12c18b1, 0x20), `sLanguageUnlocalizedAsEnglish` (0x12c18d2, 0x1d), `loc_reload` (0x12c18f0,
// 0xa) with `Reloads localization data from disk` (0x12c18fb, 0x23), the parent `Localization`
// (0x12c191f, 0xc), the six captions (0x12c192c..0x12c1990), the empty description 0x12c1bfb, and
// `!!!Unlocalized!!!` (0x12c1bb2). The neighbouring rodata is the language-name table
// (`global, ar, zh, cs, da, nl, en, fi, fr, de, el, it, ja...`), which is the `[1, 26]` bound below.
//
// Callee histogram (every one of the 54 sites is pinned by the checker):
//   0xf590    x24 (shared release of a borrowed stack view; every site is an exact no-op)
//   0xf8f390  x14 (__cxa_atexit PLT, third argument 0x1604218 = __dso_handle, one per object)
//   0x26ea0   x6  (menu TOGGLE ctor; its tail call to 0x268b0 registers the item)
//   0xa324c0  x4  (language-variable wrapper; calls the leaf 0xa32680 and 0x20d60 itself)
//   0x20af0   x2  (ConCmdBase ctor: the bool variable and the command)
//   0x20d60   x1  (the bool variable's register-once)
//   0xf2f0    x1  (name view of the command, `loc_reload`)
//   0xf1a0    x1  (SharedStr ctor of "!!!Unlocalized!!!")
//   0xf8f3a0  x1  (__stack_chk_fail, the branch target only)
//
// The 5 indirect sites, resolved through .data.rel.ro RELATIVE relocations:
//   * four `callq *0x18(%rax)` (0xa333c5 / 0xa334cf / 0xa335d3 / 0xa336de) execute immediately after
//     `movq 0xb0(%rsp),%rax`, and [rsp+0xb0] is the row's argument-functor tag 0x15d1188. Slot 3 of
//     that table is 0xa32490, a bare `ret`, so each is an inert functor destroy of a stack temporary.
//   * one `callq *0x20(%rax)` (0xa338cb) with %rax = 0x1594dc0 (the command class table); slot
//     [0x1594dc0 + 0x20] = 0x20d60, the register-once guard. This is the same dispatch shape as
//     core/fconcmd_object_slot306, which already models it.
// The 4 x 0xa324c0 calls add eleven more collapsed dispatches each (six in the wrapper, five in the
// leaf, all through 0x15d1188 slots 0/1/3), so the whole slot performs 4*12 = 48 collapsed
// dispatches (the eleventh being the row-local destroy above).
//
// The 14 __cxa_atexit records, in body order (site, dtor, object RVA; object offsets are + each
// field's offset inside Slot316Objects), are kSlot316AtexitRows below: the 8 B pointer cell
// 0x32f4fe0 (0xa2c760) first, then for each of the four language variables its own record (0xa2c850),
// then the bool variable (0x20b0), the command (0x4b0), the six toggles (0x272d0) and finally the
// "!!!Unlocalized!!!" string (0xf7b0).
//
// ================================ object map (verified strides) ================================
// Every original address is consecutive except one documented gap:
//   0x32f4fe0  cell        8 B   (*cell = 0; the atexit 0xa2c760 frees the pointer it holds)
//   0x32f4fe8  var text    0xd8  0x32f50c0  var subtitles  0x32f5198  var speech
//   0x32f5270  var system  0xd8  (ends exactly where the bool variable starts)
//   0x32f5348  bool var    0xc8  (+0x60 = 0x32f5408 = obj+0xc0: the value cell is the object's own)
//   0x32f5410  command     0xa0
//   0x32f54b0 .. 0x32f57f8  six toggles, 0xa8 each (ending at 0x32f58a0)
//   0x32f58a0  0x58 B      NOT this body's: 0xa2d7be stores a dword there and 0xa306a2/0xa33... use
//                          it as an argument. The aggregate keeps it as an inert gap so the original
//                          addresses are reproduced; nothing in this module reads or writes it.
//   0x32f58f8  SharedStr  0x20 (0xf1a0 from "!!!Unlocalized!!!"; atexit 0xf7b0)
// Adjacency to 0x32f58a0 is a pinned FACT of the address map, not a claim about that object.
//
// ================================ modelled vs hook (G2) =================================
// MODELLED (native, exact): the prologue/epilogue and canary guard; the pointer-cell zero and its
// atexit record; the four language variables through `init_slot316_language_var` (the net state of
// the 436 B wrapper 0xa324c0 + the 352 B leaf 0xa32680: base ctor, mid vptr 0x15d1120, value cell,
// both bounds engaged at [1, 26], the 32 B argument copy with the q[3] self-pointer rule, the
// collapsed 11-dispatch functor install, the final vptr 0x15d11b8, the value/default pair read twice
// from the caller's cell, 0x20d60 registration) plus each site's description release, functor destroy,
// name release and atexit record; the bool variable through the existing `init_bool_var_static`
// (class 0x1594218, functor tag 0x15d1220, value 0); the command through the existing
// `init_fconcmd_object` (0x1594dd0, flags 2, delegate handler 0xa2cad0, register via
// [0x1594dc0+0x20] = 0x20d60, atexit 0x4b0); the six toggles through the existing 0x26ea0 + 0x268b0
// models with their gate answers; all 14 atexit records in body order.
// HOOK (optional override, no default in the struct): `Slot316Hooks::language_var_ctor` (0xa324c0 +
// 0xa32680). `default_slot316_hooks()` installs the native model; a null or failing hook stops the
// fragment at that variable's stage BEFORE its atexit record.
// HOST STAND-IN (named, not lifted): `Slot316Hooks::string_alloc` — the allocator pair behind
// 0x12820 / 0x12af0 that 0xf1a0 needs for a non-empty string. `default_slot316_hooks()` installs the
// same documented malloc/free stand-in that registered slot 306 uses; clearing it makes the fragment
// fail visibly at `unlocalized_build`. The engine allocator body is NOT recovered and is not claimed.
//
// Fail-visible contracts (all reported through `blocked` / `ok == false`, never a silent pass):
//   * a null or failing `language_var_ctor`, or a spec that is not the pinned row for its index;
//   * a `0xf590` release that is not the exact inert no-op of a borrowed view;
//   * a failed 0x20d60 / 0x268b0 registration;
//   * a failed `__cxa_atexit` record;
//   * a 0x26ea0 target whose vptr tag is not one of the three pinned classes;
//   * a failed 0xf1a0 build (missing allocator or allocation failure);
//   * a `__stack_chk_guard` that changed across the body (the original would call __stack_chk_fail at
//     0xa33c7b). Without a host-supplied guard the comparison is not claimed: `canary_checked` stays
//     false and the result never reports a silent pass.
//
// NOT modelled: every destructor body (0xa2c760 cell, 0xa2c850 variable, 0x20b0 bool, 0x4b0 command,
// 0x272d0 item, 0xf7b0 string) and the four per-site callbacks the language functors carry as DATA
// (0xa2c790 / 0xa2c890 / 0xa2c950 / 0xa2ca10 at q[3], plus the 0xa324a0 invoker thunk at q[1]).
// They are pinned identity tags, recorded and never executed, and nothing here implements the
// language-reload behaviour those callbacks describe. No localization, language-selection or
// reload behaviour is claimed.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"
#include "fconcmd_object_slot306.hpp"
#include "l2_slot_common.hpp"
#include "shared_str_ctor_f1a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot316Rva = 0xa33280;
inline constexpr std::size_t kSlot316BodySize = 2561;          // FDE [0xa33280, 0xa33c81)
inline constexpr std::uint64_t kSlot316EndRva = 0xa33c81;
inline constexpr std::size_t kSlot316ReachableBytes = 2560;    // the last reached instruction ends here
inline constexpr std::size_t kSlot316InstructionCount = 473;
inline constexpr std::size_t kSlot316DirectCallSites = 54;
inline constexpr std::size_t kSlot316IndirectCallSites = 5;
inline constexpr std::size_t kSlot316RipOperands = 88;
inline constexpr std::size_t kSlot316RipTargets = 62;
inline constexpr std::size_t kSlot316StoreSites = 11;
inline constexpr std::size_t kSlot316StoreTargets = 9;
inline constexpr std::size_t kSlot316BranchCount = 2;
inline constexpr std::uint64_t kSlot316TableTopRva = 0x2dd70e0;    // index i at TOP - 8*i
inline constexpr std::uint64_t kSlot316TableWordRva = 0x2dd6700;   // 0x2dd70e0 - 316*8
inline constexpr std::uint32_t kSlot316TableIndex = 316;
// The next index that is still unrecovered once this one is registered. Indices 317 (0xa2c390) and
// 318 (0xa20e80) are ALREADY registered startup slots, so the cliff moves to the next real gap in the
// 573-row table, which is index 326.
inline constexpr std::uint32_t kSlot316NextUnrecoveredIndex = 326;
inline constexpr std::uint64_t kSlot316NextUnrecoveredRva = 0x837f30;

// Frame / guard.
inline constexpr std::uint64_t kSlot316CanaryCellRva = 0x15fe518;   // GOT cell holding &__stack_chk_guard
inline constexpr std::uint32_t kSlot316CanaryFrameOffset = 0xc0;    // [rsp+0xc0]
inline constexpr std::uint64_t kSlot316StackChkFailRva = 0xf8f3a0;
inline constexpr std::uint64_t kSlot316BranchRva = 0xa33c6a;        // jne 0xa33c7b (the guard compare)
inline constexpr std::uint64_t kSlot316BranchTargetRva = 0xa33c7b;
inline constexpr std::uint64_t kSlot316InnerBranchRva = 0xa338a7;   // je 0xa338b1 (never taken here)
inline constexpr std::uint64_t kSlot316InnerBranchTargetRva = 0xa338b1;
inline constexpr std::uint32_t kSlot316FrameBytes = 0xe0;           // sub rsp,0xe0 (after and rsp,-0x20)

// Shared callees.
inline constexpr std::uint64_t kSlot316AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot316DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot316ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot316ConCmdBaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot316ConCmdRegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot316MenuToggleCtorRva = 0x26ea0;
inline constexpr std::uint64_t kSlot316MenuRegisterRva = 0x268b0;
inline constexpr std::uint64_t kSlot316MenuItemDtorRva = 0x272d0;
inline constexpr std::uint64_t kSlot316NameViewRva = 0xf2f0;
inline constexpr std::uint64_t kSlot316BoolVarDtorRva = 0x20b0;      // = kBoolVarDtorRva
inline constexpr std::uint64_t kSlot316CommandDtorRva = 0x4b0;       // = kFConCmdDtorRva
inline constexpr std::uint64_t kSlot316CellDtorRva = 0xa2c760;
inline constexpr std::uint64_t kSlot316LanguageVarDtorRva = 0xa2c850;
inline constexpr std::uint64_t kSlot316UnlocalizedDtorRva = 0xf7b0;
// The language-variable constructor pair.
inline constexpr std::uint64_t kSlot316LanguageVarWrapperRva = 0xa324c0;
inline constexpr std::size_t kSlot316LanguageVarWrapperSize = 436;   // FDE
inline constexpr std::size_t kSlot316LanguageVarWrapperInstructions = 99;
inline constexpr std::uint64_t kSlot316LanguageVarLeafRva = 0xa32680;
inline constexpr std::size_t kSlot316LanguageVarLeafSize = 352;      // FDE
inline constexpr std::size_t kSlot316LanguageVarLeafInstructions = 90;
inline constexpr std::uint64_t kSlot316SharedStrCtorRva = 0xf1a0;

// Identity tags (original vtable address points; never dispatched through here).
inline constexpr std::uint64_t kSlot316LanguageVarMidVptr = 0x15d1120;    // 0x15d1110 + 0x10 (leaf)
inline constexpr std::uint64_t kSlot316LanguageVarFinalVptr = 0x15d11b8;  // 0x15d11a8 + 0x10 (wrapper)
inline constexpr std::uint64_t kSlot316LanguageFnVtbl = 0x15d1188;        // the argument/functor class
inline constexpr std::uint64_t kSlot316LanguageThunkRva = 0xa324a0;       // q[1]: 5-arg invoker thunk
inline constexpr std::uint64_t kSlot316LanguageFnCopyRva = 0xa32440;      // slot 0
inline constexpr std::uint64_t kSlot316LanguageFnCloneRva = 0xa32460;     // slot 1
inline constexpr std::uint64_t kSlot316LanguageFnMoveRva = 0xa32480;      // slot 2
inline constexpr std::uint64_t kSlot316LanguageFnDestroyRva = 0xa32490;   // slot 3: bare `ret`
inline constexpr std::uint64_t kSlot316LanguageGate3Rva = 0xa31d90;       // mov eax,1; ret
inline constexpr std::uint64_t kSlot316LanguageGate6Rva = 0xa31f10;       // mov eax,1; ret
inline constexpr std::uint32_t kSlot316LanguageGate3 = 1;
inline constexpr std::uint32_t kSlot316LanguageGate6 = 1;
inline constexpr std::uint64_t kSlot316BoolFinalVptr = 0x1594218;         // = kBoolVarFinalVptr
inline constexpr std::uint64_t kSlot316BoolFnTag = 0x15d1220;             // the functor tag of the site
inline constexpr std::uint32_t kSlot316BoolGate3 = 1;                     // 0xb890 `mov eax,1; ret`
inline constexpr std::uint32_t kSlot316BoolGate6 = 0;                     // 0xb930 `xor eax,eax; ret`
inline constexpr std::uint64_t kSlot316CommandVptr = 0x1594dd0;           // = kFConCmdVptr
inline constexpr std::uint32_t kSlot316CommandGate3 = 0;                  // 0x22ae0 `xor eax,eax; ret`

// Bounds and value of the four language variables: [1, 26] with the unset sentinel -1.
inline constexpr std::uint32_t kSlot316LanguageMin = 1;
inline constexpr std::uint32_t kSlot316LanguageMax = 26;      // 0x1a, the language-name table length
inline constexpr std::uint32_t kSlot316LanguageUnset = 0xffffffffu;
inline constexpr std::uint32_t kSlot316VarFlags = 1;
inline constexpr std::uint32_t kSlot316CommandFlags = 2;      // = kFConCmdFlags
inline constexpr std::uint32_t kSlot316ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot316LanguageDispatches = 11;   // wrapper (6) + leaf (5)
inline constexpr std::uint32_t kSlot316LanguageRowLocalDispatches = 1;
inline constexpr std::uint32_t kSlot316FunctorDispatches = 48;   // 4 * (11 + 1), see below
static_assert(kSlot316FunctorDispatches ==
              4 * (kSlot316LanguageDispatches + kSlot316LanguageRowLocalDispatches));

// Strings of the block: identity address + pinned length (the native views are literals).
inline constexpr char kSlot316MenuParent[] = "Localization";                       // 0x12c191f, 0xc
inline constexpr char kSlot316EmptyText[] = "";                                    // 0x12c1bfb
inline constexpr char kSlot316UnlocalizedText[] = "!!!Unlocalized!!!";            // 0x12c1bb2
inline constexpr std::uint64_t kSlot316MenuParentRva = 0x12c191f;
inline constexpr std::uint64_t kSlot316EmptyTextRva = 0x12c1bfb;
inline constexpr std::uint64_t kSlot316UnlocalizedTextRva = 0x12c1bb2;
static_assert(sizeof kSlot316MenuParent - 1 == 0xc);
static_assert(sizeof kSlot316UnlocalizedText - 1 == 17);

// ---- the four language variables ------------------------------------------------------------
struct Slot316LanguageVarSpec {
    std::uint32_t index;             // 0..3, body order
    const char* name;
    std::uint32_t name_len;
    std::uint64_t name_rva;
    const char* description;
    std::uint32_t description_len;
    std::uint64_t description_rva;
    std::uint64_t object_rva;
    std::uint64_t callback_rva;      // functor q[3] (identity tag, never called)
    std::uint64_t atexit_site_rva;   // this row's `callq 0xf8f390`
};

inline constexpr std::size_t kSlot316LanguageVarCount = 4;
inline constexpr std::array<Slot316LanguageVarSpec, kSlot316LanguageVarCount> kSlot316LanguageVars = {{
    {0, "loc_text", 8, 0x12c1820, "Change language for Text usage", 0x1e, 0x12c1829, 0x32f4fe8,
     0xa2c790, 0xa333e4},
    {1, "loc_subtitles", 0xd, 0x12c1848, "Change language for Subtitles usage", 0x23, 0x12c1856,
     0x32f50c0, 0xa2c890, 0xa334e7},
    {2, "loc_speech", 0xa, 0x12c187a, "Change language for Speech usage", 0x20, 0x12c1885, 0x32f5198,
     0xa2c950, 0xa335f5},
    {3, "loc_system", 0xa, 0x12c18a6, "Change language for System usage", 0x20, 0x12c18b1, 0x32f5270,
     0xa2ca10, 0xa336f9},
}};

// ---- the bool variable, the command and the six toggles ---------------------------------------
struct Slot316BoolVarSpec {
    const char* name;
    std::uint32_t name_len;
    std::uint64_t name_rva;
    std::uint64_t object_rva;
    std::uint64_t fn_tag;
    std::uint8_t value;
    std::uint8_t default_value;
    std::uint64_t atexit_site_rva;
};
inline constexpr Slot316BoolVarSpec kSlot316BoolVar = {
    "sLanguageUnlocalizedAsEnglish", 0x1d, 0x12c18d2, 0x32f5348, kSlot316BoolFnTag, 0, 0, 0xa33805};

struct Slot316CommandSpec {
    const char* name;                  // 0xf2f0 measures it
    const char* description;
    std::uint32_t description_len;
    std::uint64_t name_rva;
    std::uint64_t description_rva;
    std::uint64_t object_rva;
    std::uint64_t handler_tag;         // identity tag only (0xa2cad0 writes the pointer cell)
    std::uint64_t atexit_site_rva;
};
inline constexpr Slot316CommandSpec kSlot316Command = {
    "loc_reload", "Reloads localization data from disk", 0x23, 0x12c18f0, 0x12c18fb, 0x32f5410,
    0xa2cad0, 0xa338f6};

// Which object a toggle points at: 0..3 the language variables, 4 the bool, 5 the command.
struct Slot316ItemSpec {
    std::uint32_t index;
    const char* caption;
    std::uint32_t caption_len;
    std::uint64_t caption_rva;
    std::uint64_t object_rva;
    std::uint32_t target;              // Slot316Target
    std::uint64_t atexit_site_rva;
};
enum class Slot316Target : std::uint8_t { language_var, bool_var, command };
struct Slot316ItemRow {
    Slot316ItemSpec spec;
    Slot316Target kind;
};
inline constexpr std::size_t kSlot316ItemCount = 6;
inline constexpr std::array<Slot316ItemRow, kSlot316ItemCount> kSlot316Items = {{
    {{0, "Language Text", 0xd, 0x12c192c, 0x32f54b0, 0, 0xa33981}, Slot316Target::language_var},
    {{1, "Language Subtitles", 0x12, 0x12c193a, 0x32f5558, 1, 0xa33a08}, Slot316Target::language_var},
    {{2, "Language Speech", 0xf, 0x12c194d, 0x32f5600, 2, 0xa33a8f}, Slot316Target::language_var},
    {{3, "Language System", 0xf, 0x12c195d, 0x32f56a8, 3, 0xa33b1a}, Slot316Target::language_var},
    {{4, "Use English instead of Unlocalized", 0x22, 0x12c196d, 0x32f5750, 4, 0xa33ba4},
     Slot316Target::bool_var},
    {{5, "Reload", 6, 0x12c1990, 0x32f57f8, 5, 0xa33c2b}, Slot316Target::command},
}};
// The `target` column and the `kind` column cannot drift apart.
static_assert(kSlot316Items[0].spec.target == 0 && kSlot316Items[1].spec.target == 1 &&
              kSlot316Items[2].spec.target == 2 && kSlot316Items[3].spec.target == 3 &&
              kSlot316Items[4].spec.target == 4 && kSlot316Items[5].spec.target == 5,
              "each toggle names its own object index");
static_assert(kSlot316Items[0].kind == Slot316Target::language_var &&
              kSlot316Items[3].kind == Slot316Target::language_var &&
              kSlot316Items[4].kind == Slot316Target::bool_var &&
              kSlot316Items[5].kind == Slot316Target::command,
              "the toggle target classes are pinned");

// ---- the 8 B pointer cell ---------------------------------------------------------------------
inline constexpr std::uint64_t kSlot316CellRva = 0x32f4fe0;
struct Slot316Cell {
    std::uint64_t word;              // +0x00 0; the atexit dtor 0xa2c760 frees the pointer it holds
};
static_assert(sizeof(Slot316Cell) == 8);

// ---- objects ----------------------------------------------------------------------------------
inline constexpr std::uint64_t kSlot316UnlocalizedRva = 0x32f58f8;
inline constexpr std::size_t kSlot316ForeignGapRva = 0x32f58a0;    // 0x58 B, NOT written by this body
inline constexpr std::size_t kSlot316ForeignGapSize = 0x58;

struct Slot316Objects {
    Slot316Cell cell{};                     // 0x32f4fe0  +0x000
    ConCmdRangedVar var_text{};             // 0x32f4fe8  +0x008
    ConCmdRangedVar var_subtitles{};        // 0x32f50c0  +0x0e0
    ConCmdRangedVar var_speech{};           // 0x32f5198  +0x1b8
    ConCmdRangedVar var_system{};           // 0x32f5270  +0x290
    ConCmdBoolVar var_english{};            // 0x32f5348  +0x368
    ConCmdCommand command{};                // 0x32f5410  +0x430
    DebugMenuItem item_text{};              // 0x32f54b0  +0x4d0
    DebugMenuItem item_subtitles{};         // 0x32f5558  +0x578
    DebugMenuItem item_speech{};            // 0x32f5600  +0x620
    DebugMenuItem item_system{};            // 0x32f56a8  +0x6c8
    DebugMenuItem item_english{};           // 0x32f5750  +0x770
    DebugMenuItem item_reload{};            // 0x32f57f8  +0x818
    std::uint8_t foreign_58a0[kSlot316ForeignGapSize]{};   // 0x32f58a0  +0x8c0 (another block's object)
    SharedStr unlocalized{};                // 0x32f58f8  +0x918
};
static_assert(sizeof(Slot316Objects) == 0x938, "Slot316Objects must reproduce the original strides");
static_assert(offsetof(Slot316Objects, cell) == 0x000 && offsetof(Slot316Objects, var_text) == 0x008 &&
              offsetof(Slot316Objects, var_subtitles) == 0x0e0 &&
              offsetof(Slot316Objects, var_speech) == 0x1b8 &&
              offsetof(Slot316Objects, var_system) == 0x290 &&
              offsetof(Slot316Objects, var_english) == 0x368 &&
              offsetof(Slot316Objects, command) == 0x430 && offsetof(Slot316Objects, item_text) == 0x4d0 &&
              offsetof(Slot316Objects, item_subtitles) == 0x578 &&
              offsetof(Slot316Objects, item_speech) == 0x620 &&
              offsetof(Slot316Objects, item_system) == 0x6c8 &&
              offsetof(Slot316Objects, item_english) == 0x770 &&
              offsetof(Slot316Objects, item_reload) == 0x818 &&
              offsetof(Slot316Objects, foreign_58a0) == 0x8c0 &&
              offsetof(Slot316Objects, unlocalized) == 0x918);
static_assert(kSlot316CellRva + 8 == kSlot316LanguageVars[0].object_rva &&
              kSlot316LanguageVars[0].object_rva + 0xd8 == kSlot316LanguageVars[1].object_rva &&
              kSlot316LanguageVars[1].object_rva + 0xd8 == kSlot316LanguageVars[2].object_rva &&
              kSlot316LanguageVars[2].object_rva + 0xd8 == kSlot316LanguageVars[3].object_rva &&
              kSlot316LanguageVars[3].object_rva + 0xd8 == kSlot316BoolVar.object_rva &&
              kSlot316BoolVar.object_rva + 0xc8 == kSlot316Command.object_rva &&
              kSlot316Command.object_rva + 0xa0 == kSlot316Items[0].spec.object_rva &&
              kSlot316Items[5].spec.object_rva + 0xa8 == kSlot316ForeignGapRva &&
              kSlot316ForeignGapRva + kSlot316ForeignGapSize == kSlot316UnlocalizedRva,
              "the original object addresses follow the pinned strides");

// One `__cxa_atexit` registration of the body, in body order (every site is tool-pinned).
struct Slot316AtexitRow {
    std::uint32_t step;        // 1..14
    std::uint64_t site_rva;    // the `callq 0xf8f390`
    std::uint64_t dtor_rva;    // rdi (identity tag, never run)
    std::uint64_t object_rva;  // rsi (identity only)
};
inline constexpr std::size_t kSlot316AtexitRowCount = 14;
inline constexpr std::array<Slot316AtexitRow, kSlot316AtexitRowCount> kSlot316AtexitRows = {{
    {1, 0xa332c6, kSlot316CellDtorRva, 0x32f4fe0},
    {2, 0xa333e4, kSlot316LanguageVarDtorRva, 0x32f4fe8},
    {3, 0xa334e7, kSlot316LanguageVarDtorRva, 0x32f50c0},
    {4, 0xa335f5, kSlot316LanguageVarDtorRva, 0x32f5198},
    {5, 0xa336f9, kSlot316LanguageVarDtorRva, 0x32f5270},
    {6, 0xa33805, kSlot316BoolVarDtorRva, 0x32f5348},
    {7, 0xa338f6, kSlot316CommandDtorRva, 0x32f5410},
    {8, 0xa33981, kSlot316MenuItemDtorRva, 0x32f54b0},
    {9, 0xa33a08, kSlot316MenuItemDtorRva, 0x32f5558},
    {10, 0xa33a8f, kSlot316MenuItemDtorRva, 0x32f5600},
    {11, 0xa33b1a, kSlot316MenuItemDtorRva, 0x32f56a8},
    {12, 0xa33ba4, kSlot316MenuItemDtorRva, 0x32f5750},
    {13, 0xa33c2b, kSlot316MenuItemDtorRva, 0x32f57f8},
    {14, 0xa33c53, kSlot316UnlocalizedDtorRva, 0x32f58f8},
}};

// One of the four row-local `callq *0x18(%rax)` sites: (site, argument tag, slot-3 body).
struct Slot316FunctorSite {
    std::uint64_t site_rva;
    std::uint64_t tag_rva;
    std::uint64_t slot3_rva;
    bool slot3_is_bare_ret;      // pinned true; false blocks the fragment
};
inline constexpr std::size_t kSlot316FunctorSiteCount = 4;
inline constexpr std::array<Slot316FunctorSite, kSlot316FunctorSiteCount> kSlot316FunctorSites = {{
    {0xa333c5, kSlot316LanguageFnVtbl, kSlot316LanguageFnDestroyRva, true},
    {0xa334cf, kSlot316LanguageFnVtbl, kSlot316LanguageFnDestroyRva, true},
    {0xa335d3, kSlot316LanguageFnVtbl, kSlot316LanguageFnDestroyRva, true},
    {0xa336de, kSlot316LanguageFnVtbl, kSlot316LanguageFnDestroyRva, true},
}};
// The one `callq *0x20(%rax)` site: it dispatches through the command class table (NOT its vptr).
inline constexpr std::uint64_t kSlot316CommandRegisterSiteRva = 0xa338cb;
inline constexpr std::uint64_t kSlot316CommandTableRva = 0x1594dc0;
inline constexpr std::uint32_t kSlot316CommandRegisterSlot = 0x20;

// ---- stages / results ---------------------------------------------------------------------------
enum class Slot316Stage : std::uint8_t {
    none,
    cell_atexit,
    language_var_build,
    language_var_view_release,
    language_var_atexit,
    bool_var_build,
    bool_var_atexit,
    command_build,
    command_atexit,
    item_ctor,
    item_registration,
    item_view_release,
    item_atexit,
    unlocalized_build,
    unlocalized_atexit,
    canary,
};
[[nodiscard]] const char* slot316_stage_name(Slot316Stage stage) noexcept;

struct Slot316LanguageVarResult {
    bool ok = false;
    std::uint32_t functor_dispatches = 0;   // kSlot316LanguageDispatches once the net state is modelled
    bool description_inert = false;         // the 0xf590 on the stack description view
    bool name_inert = false;                // the 0xf590 on the stack name view
    bool local_functor_inert = false;       // the row-local `[tag+0x18]` is the pinned bare ret
    ConCmdRegisterResult registration{};
};

struct Slot316Hooks {
    // 0xa324c0 + 0xa32680. NO default in this struct: a missing hook stops the fragment at that
    // variable's `language_var_build` stage. `default_slot316_hooks()` installs the native model.
    Slot316LanguageVarResult (*language_var_ctor)(ConCmdRangedVar& var,
                                                  const Slot316LanguageVarSpec& spec,
                                                  ConCmdRegistry& registry) noexcept = nullptr;
    // The allocator pair behind 0x12820 / 0x12af0 that 0xf1a0 needs for "!!!Unlocalized!!!".
    // NOT lifted: `default_slot316_hooks()` installs the documented malloc/free stand-in (the same
    // convention as registered slot 306). Clearing it fails the fragment at `unlocalized_build`.
    SharedStrCtorHooks string_alloc{};
    // __stack_chk_guard source (0x15fe518). Null => the comparison is not performed and
    // `canary_checked` stays false; a changed guard fails the fragment at stage `canary`.
    const std::uint64_t* stack_guard = nullptr;
};

// The net state of 0xa324c0 + 0xa32680 for one pinned language-variable row: the base constructor,
// the leaf's stores (mid vptr, value cell, both bounds engaged at [1, 26], the argument block), the
// final vptr, the collapsed 11-dispatch functor install, the value/default pair read twice from the
// caller's cell, and the 0x20d60 registration. Refuses (writes nothing) when the spec is not the
// pinned row for its index.
[[nodiscard]] Slot316LanguageVarResult init_slot316_language_var(ConCmdRangedVar& var,
                                                                 const Slot316LanguageVarSpec& spec,
                                                                 ConCmdRegistry& registry) noexcept;

// The production hooks: the native language-variable model plus the malloc-backed string allocator.
[[nodiscard]] Slot316Hooks default_slot316_hooks() noexcept;

// The 0x26ea0 gate answer of a target object, read from its own vptr tag (never a hook). Knows the
// three classes this body targets: the language-variable class {1, 1}, the bool class {1, 0} and the
// command class {0, 0}. Any other tag is unknown and blocks the fragment.
[[nodiscard]] DebugMenuTarget slot316_menu_target(const ConCmdBase& cmd) noexcept;

// The lifted `callq *0x18(%rax)` for the four row-local sites: a bare `ret` has no effect.
[[nodiscard]] bool slot316_inert_functor_destroy(const Slot316FunctorSite& site) noexcept;

// The stand-ins the aggregate holds, by index (0..3 language vars, 4 the bool, 5 the command).
[[nodiscard]] ConCmdBase& slot316_target_base(Slot316Objects& objects, std::uint32_t index) noexcept;
[[nodiscard]] DebugMenuItem& slot316_item(Slot316Objects& objects, std::uint32_t index) noexcept;

struct Slot316Result {
    bool ok = false;
    Slot316Stage blocked = Slot316Stage::none;
    std::uint32_t blocked_index = 0;           // the row index for the per-object stages
    std::uint32_t atexit_records = 0;          // this body's 14 __cxa_atexit registrations
    std::uint32_t functors_destroyed = 0;      // the four row-local `callq *0x18(%rax)` sites
    std::uint32_t functor_dispatches = 0;      // all collapsed dispatches (48 at completion)
    std::uint32_t vars_built = 0;              // four language variables + the bool + the command
    std::uint32_t menu_items = 0;              // six toggles
    std::array<Slot316LanguageVarResult, kSlot316LanguageVarCount> language_vars{};
    BoolVarResult bool_var{};
    FConCmdBuildResult command{};
    std::array<DebugMenuRegisterResult, kSlot316ItemCount> item_registrations{};
    bool unlocalized_built = false;
    bool views_inert = true;                   // every 0xf590 call of this body was a no-op
    bool canary_checked = false;               // false when the host supplied no guard
    bool canary_matched = false;
};

// Runs original constructor 316 natively. The original is unconditional, so the first failing step
// stops the slot (nothing after it is claimed); every failure names its stage and row index.
[[nodiscard]] Slot316Result init_slot316_a33280(Slot316Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu,
                                                const Slot316Hooks& hooks = {}) noexcept;

}  // namespace sotc
