// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01: template-driven native model of the smallest uniform cluster of
// "debug page" startup constructors. Original ELF sha256 d0723afb...5861d3.
//
// ================================ the cluster and why it is one shape ================================
// The startup-gap survey (docs/STARTUP_GAP_SURVEY.md) measures 100 of the 113 open gaps as one
// debug-page template. The batch modelled here is the sub-cluster that is *byte-for-byte the same
// statement sequence*, so one parameterised model serves it with per-slot DATA and no per-slot code:
//
//   idx  RVA        FDE B  insn  shape
//   361  0x777390   528    107   one bounded float variable + one 0x27050 slider item
//   467  0x3ca110   537    107   one bounded float variable + one 0x27050 slider item
//   379  0x6d8d20   512    105   one bounded float variable + one 0x26ea0 toggle item
//   536  0x1342d0   659    124   two bounded float variables, no item
//
// Every one of the four bodies is, in this order and with no other statement:
//   1. prologue `push rbp; mov rsp,rbp; push r15/r14/r13/r12/rbx; and rsp,-0x20; sub rsp,0xe0`;
//      `guard <- *[0x15fe518]` at [rsp+0xc0];
//   2. for each variable row:
//        `[obj] = 0x15942b0` (0x15942a0 + 0x10), `callq 0xc300(obj, name, &obj.value, &min, &max, 1,
//        args, description)` where the caller argument block is {q[0..3] = 0, payload 0, tag = the
//        row's site tag, aux 0}; then `callq 0xf590(description)`; then `callq *0x18(%rax)` with
//        %rax = the same tag (its slot 3 is the bare `ret` 0xc4f0); then `[obj] = 0x15942b0`,
//        `dword [obj+0xd0] = value`, `dword [obj+0x68] = value`; `callq 0x20d60(obj)`; then
//        `callq 0xf590(name)`; then `__cxa_atexit(0x20f0, obj, 0x1604218)`;
//   3. optionally one debug-menu item on the LAST variable row:
//        0x27050(item, label, text, target = that variable, 0x30, lo, hi)   (slider, explicit range) or
//        0x26ea0(item, label, text, target = that variable, 0x30)            (toggle, gate range)
//      followed by 0x268b0 (the ctor tail call), `callq 0xf590(text)`, `callq 0xf590(label)` and
//      `__cxa_atexit(0x272d0, item, 0x1604218)`;
//   4. `cmpq 0xc0(%rsp), %rax; jne <__stack_chk_fail>; lea -0x28(%rbp),%rsp; pops; ret`.
//
// The per-slot numbers (table word, FDE range, body digest, every direct call site, every rip-relative
// operand, every string, every class/functor relocation) are pinned by tools/lift/check_ctor_slot_<N>.py.
// The four bodies differ ONLY in: the object addresses (0xd8/0xa8 strides in every one of them), the
// name/description/label/text pointers and their pinned lengths, the min/max/value IEEE words, the
// caller argument-functor tag, the menu item kind and its (label, text, range), and whether the value
// store is register-based or rip-relative. All of that is data in the per-slot tables.
//
// ================================ modelled vs hook (G2) =================================
// Everything is MODELLED: the model is a composition of shapes that are already native and checker-pinned
//   * 0xc300 bounded float variable  core/ctor_shapes_343_356 init_float_var_static (bounded row)
//   * 0x20d60 register-once guard    core/concmd_base
//   * 0xf590 borrowed-view release   core/concmd_base release_inert_view_f590
//   * 0x27050 slider item            core/debug_menu_item init_debug_menu_item_27050
//   * 0x26ea0 toggle item            core/debug_menu_item init_debug_menu_item_26ea0
//   * 0x268b0 menu registration      core/debug_menu_item register_debug_menu_item_268b0
//   * __cxa_atexit                   core/concmd_base StartupAtexitLog (destructors recorded, never run)
// The only hook is the __stack_chk_guard source: it is the toolchain's runtime secret, so a host that
// does not supply it gets `canary_checked == false` instead of a silent pass.
//
// The three caller argument-functor tags this batch introduces (0x15cda18 at 361, 0x15b0e90 at 467,
// 0x15981a0 at 536) are PROVEN EQUIVALENT to the already-lifted float tag: their four .data.rel.ro
// slots resolve to the same bodies 0xc4a0 (copy) / 0xc4c0 (clone) / 0xc4e0 (move) / 0xc4f0 (bare-ret
// destroy), which the per-slot checkers pin. The constructor reads the caller tag only to select those
// bodies and never stores it (the object receives the class tag from the copy/clone bodies), so the
// model validates the REAL tag and then hands `kRangedFloatFnVtbl` to the shared shape.
//
// The debug-menu gate of the float class is read from the pinned relocations, not from a hook:
// 0x15942b0 + 0x18 -> 0xbba0 (`mov eax,1; ret`) and 0x15942b0 + 0x30 -> 0xbbf0 (`mov eax,3; ret`), so
// the 0x26ea0 narrowing `slot3 == 1 && slot6 == 3` HOLDS and slot 379's toggle carries the {0.1, 0.5}
// range (the 0x27050 slider sites pass their range explicitly and read no gate).
//
// Fail-visible contracts (reported through `blocked` / `ok == false`, never a silent pass): a script
// larger than the object stand-ins or with an out-of-range item target (`shape`, before any write); an
// unpinned caller tag or a refused 0xc300 construction (`var_ctor`); a non-inert 0xf590 release
// (`var_view` / `item_view`); a failed 0x20d60 / 0x268b0 registration (`var_register` /
// `item_register`); a failed __cxa_atexit record; a __stack_chk_guard that changed across the body
// (`canary`). The original is unconditional, so the first failing step stops the fragment and nothing
// after it is claimed.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "ctor_shapes_343_356.hpp"   // FloatVarResult / MenuSiteResult (the shared site results)
#include "debug_menu_item.hpp"

namespace sotc::debug_page {

// ---- the constructor table (index i at TOP - 8*i) ------------------------------------------------
inline constexpr std::uint64_t kCtorTableTopRva = 0x2dd70e0;

// ---- the original callees this cluster is built from (identity tags) ------------------------------
inline constexpr std::uint64_t kBoundedFloatCtorRva = 0xc300;   // variable row
inline constexpr std::uint64_t kFloatFnCopyRva = 0xc4a0;        // caller-tag slot 0
inline constexpr std::uint64_t kFloatFnCloneRva = 0xc4c0;       // caller-tag slot 1
inline constexpr std::uint64_t kFloatFnMoveRva = 0xc4e0;        // caller-tag slot 2
inline constexpr std::uint64_t kFloatFnDestroyRva = 0xc4f0;     // caller-tag slot 3: a bare `ret`
inline constexpr std::uint64_t kConCmdRegisterRva = 0x20d60;    // register-once guard
inline constexpr std::uint64_t kViewReleaseRva = 0xf590;        // shared release of a borrowed view
inline constexpr std::uint64_t kFloatVarDtorRva = 0x20f0;       // __cxa_atexit rdi of a variable row
inline constexpr std::uint64_t kSliderItemCtorRva = 0x27050;    // item row (explicit float range)
inline constexpr std::uint64_t kToggleItemCtorRva = 0x26ea0;    // item row (gate-derived range)
inline constexpr std::uint64_t kMenuItemRegisterRva = 0x268b0;  // the ctor's tail call
inline constexpr std::uint64_t kMenuItemDtorRva = 0x272d0;      // __cxa_atexit rdi of an item row
inline constexpr std::uint64_t kStackChkFailRva = 0xf8f3a0;
inline constexpr std::uint64_t kStackGuardCellRva = 0x15fe518;  // GOT cell of __stack_chk_guard
inline constexpr std::uint64_t kDsoHandleRva = 0x1604218;

// ---- the float variable class and its 0x26ea0 gate answer ----------------------------------------
inline constexpr std::uint64_t kFloatClassVptr = 0x15942b0;      // 0x15942a0 + 0x10
inline constexpr std::uint64_t kFloatClassSlot3Rva = 0xbba0;     // mov eax,1; ret
inline constexpr std::uint64_t kFloatClassSlot6Rva = 0xbbf0;     // mov eax,3; ret
inline constexpr std::uint32_t kFloatClassSlot3 = 1;
inline constexpr std::uint32_t kFloatClassSlot6 = 3;

// ---- strides and site constants (identical at every slot of the batch) ---------------------------
inline constexpr std::size_t kVarStride = 0xd8;    // one ConCmdRangedVar block
inline constexpr std::size_t kItemStride = 0xa8;   // one DebugMenuItem block (0xa4 + 4 pad)
inline constexpr std::uint32_t kVarFlags = 1;      // r9d of every 0xc300 site
inline constexpr std::uint32_t kItemFlags = 0x30;  // r8d of every 0x27050 / 0x26ea0 site
inline constexpr std::uint32_t kCanaryFrameOffset = 0xc0;

// The largest script this batch claims: slot 536 has two variable rows and no item, 361/467/379 have
// one variable row and one item. A larger script is refused at stage `shape`, before any write.
inline constexpr std::size_t kMaxScriptVars = 2;
inline constexpr std::size_t kMaxScriptItems = 1;

// ---- the caller argument-functor tags introduced by this batch -----------------------------------
inline constexpr std::uint64_t kArgFnVtblSlot361 = 0x15cda18;
inline constexpr std::uint64_t kArgFnVtblSlot467 = 0x15b0e90;
inline constexpr std::uint64_t kArgFnVtblSlot379 = 0x15cc898;
inline constexpr std::uint64_t kArgFnVtblSlot536 = 0x15981a0;

// True for the four tags above, whose four relocations are the float bodies 0xc4a0/0xc4c0/0xc4e0/0xc4f0
// (checker-pinned). The shared 0xc300 model's own accept list is checked separately.
[[nodiscard]] bool is_batch_float_site_tag(std::uint64_t tag) noexcept;

// ---- one variable row (one complete 0xc300 static-initialiser block) -----------------------------
struct FloatVarRow {
    std::uint64_t object_rva;      // identity only; the stand-in lives in the slot's object struct
    const char* name;              // borrowed view data (control == nullptr)
    std::uint32_t name_len;        // the immediate the original stores into the view
    const char* description;       // borrowed view data; empty at every row of this batch
    std::uint32_t description_len;
    std::uint32_t lo_bits;         // the min cell the site stores before the call
    std::uint32_t hi_bits;         // the max cell
    std::uint32_t value_bits;      // the value copied into +0xd0 and +0x68 after the functor destroy
    std::uint64_t arg_fn_vtbl;     // the REAL caller argument-functor tag of the site
    std::uint64_t ctor_site_rva;   // the `callq 0xc300` (identity)
    std::uint64_t functor_site_rva;// the `callq *0x18(%rax)` (identity)
    std::uint64_t register_site_rva;  // the `callq 0x20d60` (identity)
    std::uint64_t atexit_site_rva;    // the `callq __cxa_atexit` (identity)
};

// ---- one menu-item row (one complete 0x27050 / 0x26ea0 site tail) --------------------------------
enum class MenuKind : std::uint8_t {
    slider_27050,   // explicit float range taken from the row
    toggle_26ea0,   // range from the target class gate, never from the row
};

struct MenuRow {
    MenuKind kind;
    std::uint64_t object_rva;      // identity only
    std::uint32_t target_var;      // index into the script's variable rows
    const char* label;             // +0x28 view
    std::uint32_t label_len;
    const char* text;              // +0x50 view
    std::uint32_t text_len;
    float range_min;               // slider_27050 only
    float range_max;               // slider_27050 only
    std::uint64_t ctor_site_rva;   // the `callq 0x27050` / `callq 0x26ea0` (identity)
    std::uint64_t atexit_site_rva; // the `callq __cxa_atexit` (identity)
};

// ---- the per-slot script: everything that differs between the four bodies ------------------------
struct Script {
    std::uint64_t slot_rva;         // identity: the constructor-table word
    std::size_t body_size;          // FDE address_range
    std::size_t instruction_count;  // pinned listing
    std::size_t direct_call_sites;
    std::size_t functor_sites;      // `callq *0x18(%rax)` sites
    std::uint32_t table_index;
    std::uint32_t canary_frame_offset;
    std::uint64_t canary_branch_rva;         // the body's only conditional branch
    std::uint64_t canary_branch_target_rva;  // where it lands (the __stack_chk_fail call)
    std::span<const FloatVarRow> vars;
    std::span<const MenuRow> items;          // at most one in this batch
};

// The native stand-ins, in script order. The slot's aggregate owns the storage at the original strides.
struct Objects {
    std::span<ConCmdRangedVar> vars;
    std::span<DebugMenuItem> items;
};

enum class Stage : std::uint8_t {
    none,
    shape,           // the script does not fit the stand-ins / an item names a missing variable
    var_ctor,        // 0xc300 refused (unpinned caller tag) or stopped the block
    var_functor,     // the site tag's slot 3 is not the pinned bare ret 0xc4f0
    var_view,        // a 0xf590 release was not the inert no-op of a borrowed view
    var_register,    // 0x20d60 failed
    var_atexit,      // __cxa_atexit(0x20f0) failed
    item_ctor,       // 0x27050 / 0x26ea0 refused
    item_register,   // 0x268b0 failed
    item_view,       // a 0xf590 release was not the inert no-op of a borrowed view
    item_atexit,     // __cxa_atexit(0x272d0) failed
    canary,          // the __stack_chk_guard changed across the body
};
[[nodiscard]] const char* stage_name(Stage stage) noexcept;

struct Hooks {
    // __stack_chk_guard source. Null => the comparison is not performed and `canary_checked` stays
    // false; a changed guard fails the fragment at stage `canary`.
    const std::uint64_t* stack_guard = nullptr;
};

struct Result {
    bool ok = false;
    Stage blocked = Stage::none;
    std::uint32_t blocked_index = 0;      // the row the stage belongs to (0 when global)
    std::uint32_t vars_built = 0;
    std::uint32_t items_built = 0;
    std::uint32_t atexit_records = 0;     // this body's records, in body order
    std::uint32_t functors_destroyed = 0; // `callq *0x18(%rax)` sites that reached the bare ret
    std::array<FloatVarResult, kMaxScriptVars> vars{};
    std::array<MenuSiteResult, kMaxScriptItems> items{};
    bool canary_checked = false;          // false when the host supplied no guard
    bool canary_matched = false;
};

// Runs one member of the cluster natively. The original is unconditional, so the first failing step
// stops the fragment (nothing after it is claimed) and every failure names its stage and row.
[[nodiscard]] Result init_debug_page(const Script& script, const Objects& objects,
                                     ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                     const Hooks& hooks = {}) noexcept;

}  // namespace sotc::debug_page
