// Native recovery of the two ranged INT console-variable constructors used by startup slot 306
// (the `sndx_` sound-engine debug variables), original ELF sha256 d0723afb...5861d3. Names are ours.
// Task CPU-01-SLOT306-INTVAR-CTORS-01. Offline analysis only; no original code is executed and no
// original RVA is called. Byte-pinned by tools/lift/check_fa624a0_intvars.py.
//
// Two wrappers with one shape, each tail-composed with a worker (all four bodies read end to end):
//
//   shape               wrapper   worker    final vptr  worker vptr  functor table  class dtor (atexit)
//   listener (2 sites)  0xa624a0  0xa62660  0x15d1fd8   0x15d1ea0    0x15d1fa8      0xa5bea0
//   show_active (1)     0xa62860  0xa62a20  0x15d20b0   0x15d1f18    0x15d2080      0xa5bee0
//
// The two wrappers (0x1b3 B) and the two workers (0x15f B) are the same code; the checker proves it by
// masking every rip-relative operand (7 per wrapper, 6 per worker) and requiring the rest to be equal,
// then resolving the operands to the constants in the table above.
//
// Prototype (SysV): wrapper(this=rdi, name=rsi -> SharedStr, &default=rdx, &min=rcx, &max=r8, flags=r9d,
//                           args=[rsp+8] -> RangedVarArgs-shaped 0x50 B block, description=[rsp+0x10] -> SharedStr)
// Worker: the same, but rdx = value cell (this+0xd0) and the block is the wrapper's LOCAL copy.
//
// Wrapper sequence (0xa624a0..0xa62653):
//   [this] = final_vptr                                  ; overwritten by the base ctor, restored at the end
//   local block: q[0..3] = args.q[0..3]; if (q[3] != 0) local.q[0] = &local.q[0];
//   local functor: payload 0, tag W (= the table above), aux 0;  value cell pointer = this+0xd0
//   tmp1 <- clone(args.fn)   via args.fn_vptr slot 1   ; tmp1.aux = args.fn_aux
//   tmp2 <- copy(tmp1)       via tmp1.tag    slot 0
//   tmp1 <- copy(local.fn)   via local.tag   slot 0
//   local.fn <- copy(tmp2)   via tmp2.tag    slot 0
//   swap(local.aux, tmp1.aux);  destroy(tmp1) via tmp1.tag slot 3 (bare ret)
//   worker(this, name, &this.value, &min, &max, flags, &local, description)
//   destroy(local.fn) via local.tag slot 3 (bare ret)
//   [this] = final_vptr; [this+0xd0] = *default; [this+0x68] = *default; call 0x20d60(this) (register)
// Worker sequence (0xa62660..0xa6277f):
//   0x20af0(this, name, description, flags)              ; the shared ConCmd base constructor
//   [this] = worker_vptr; [this+0x60] = value cell; byte[+0x6c]=0; byte[+0x70]=1; dword[+0x6c]=*min;
//   byte[+0x74]=0; byte[+0x78]=1; dword[+0x74]=*max; ymm[+0x80]=0; [+0xc0]=W; [+0xb0]=0; [+0xc8]=0
//   [+0x88..0x98) = block.q[1..2]; [+0x80] = block.q[0]; [+0x98] = block.q[3];
//   if (block.q[3] != 0) [+0x80] = this+0x80
//   same four-step functor install as above, onto this+0xa0 (payload +0xb0, tag +0xc0, aux +0xc8)
// Both quadruples of functor slots (copy 0xa62440/0xa62800, clone 0xa62460/0xa62820, move
// 0xa62480/0xa62840, destroy 0xa62490/0xa62850) are lifted trivially: copy and clone store
// `dst+0x10 = src+0x10; dst+0x20 = <own table>`, destroy is a bare `ret`. That is why the tag the
// object ends up with is the tag of the CALLER'S argument functor (A), not necessarily W.
//
// Resulting layout: identical to ConCmdRangedVar (core/concmd_vars.hpp, the float 0xc300 shape): both
// bounds engaged (bytes +0x70 and +0x78 == 1), min at +0x6c, max at +0x74, default at +0x68, value at
// +0xd0, `value_ref` (+0x60) == &value, functor payload +0xb0 / tag +0xc0 / aux +0xc8. Bytes the original
// never writes (+0x6d.. is covered by the dword; +0x71..0x73, +0x79..0x7f, +0xa0..0xaf, +0xb8) stay
// untouched here too.
//
// Registration is performed BY THE CONSTRUCTOR (direct call 0x20d60 at 0xa62627 / 0xa629e7), not by the
// slot-306 body: the 16 direct 0x20d60 calls of slot 306 are the 5 floats and 11 bools, the 3 indirect
// ones are the commands, and the 3 int vars register here. On the startup path the manager cell is
// null, so registration appends to the pending list (core/concmd_base).
//
// Vtable slots reached later through these objects (all four are `mov eax,1; ret`, checker-pinned):
// class vptr+0x18 (slot 3) and vptr+0x30 (slot 6) = 0xa60ea0/0xa60ef0 (listener) and 0xa61940/0xa61990
// (show_active). They are the two virtual calls of the debug-menu toggle constructor 0x26ea0, so
// `int_var_menu_target` reports {slot3 = 1, slot6 = 1}: the `slot3 == 1 && slot6 == 3` narrowing does
// NOT apply and the item keeps its default range {1.0, 10.0}.
//
// MODELLED (native): both wrappers, both workers, the 8 functor-slot bodies, the 0x20af0 base
// constructor and 0x20d60 registration (reused), the caller epilogue of the three slot-306 sites (argument
// functor destroy, 0xf590 on the name view, `__cxa_atexit` record), the 0x26ea0 virtual-slot results.
//
// HOOKS / CONTRACTS (explicit; nothing is silently skipped):
//   * `IntVarCtorResult::unlifted_functor`: an argument functor whose table is not 0x15d1fa8 or
//     0x15d2080 would run an unread clone slot (`call [tag+8]`) before any object write. The native
//     model refuses: ok == false and the object is left untouched. The three slot-306 sites use the
//     lifted tables only (0x15d1fa8 twice, 0x15d2080 once).
//   * the non-null ConCmdManager insert of 0x20d80 stays the existing `ConCmdManagerContract::insert`
//     hook (a non-null manager without it reports ok == false).
//   * class destructors 0xa5bea0 / 0xa5bee0 (and deleting dtors 0xa627c0 / 0xa62b80) are PINNED, never
//     executed; the atexit log records dtor RVA and object only.
//   * `__stack_chk_fail` (cold, noreturn) is not modelled; vptr stores that the base constructor
//     overwrites (the wrapper's first store) are not observable and are documented, not replayed.
//
// NOT modelled / unknown: the consumers of the three values (sound-engine listener orientation /
// position / "show active" debug draw) and the console-command read path.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"
#include "hashed_string_14c3a0.hpp"

namespace sotc {

enum class IntVarShape : std::uint8_t {
    listener,      // wrapper 0xa624a0 / worker 0xa62660
    show_active,   // wrapper 0xa62860 / worker 0xa62a20
};

// ---- ELF-tied constants (tools/lift/check_fa624a0_intvars.py compares each with the image) ----
inline constexpr std::uint64_t kIntVarWrapperListenerRva = 0xa624a0;
inline constexpr std::uint64_t kIntVarWorkerListenerRva = 0xa62660;
inline constexpr std::uint64_t kIntVarWrapperShowActiveRva = 0xa62860;
inline constexpr std::uint64_t kIntVarWorkerShowActiveRva = 0xa62a20;
inline constexpr std::size_t kIntVarWrapperSize = 0x1b3;
inline constexpr std::size_t kIntVarWorkerSize = 0x15f;

inline constexpr std::uint64_t kIntVarFinalVptrListener = 0x15d1fd8;      // table 0x15d1fc8 + 0x10
inline constexpr std::uint64_t kIntVarWorkerVptrListener = 0x15d1ea0;     // table 0x15d1e90 + 0x10
inline constexpr std::uint64_t kIntVarFnTableListener = 0x15d1fa8;
inline constexpr std::uint64_t kIntVarDtorListenerRva = 0xa5bea0;
inline constexpr std::uint64_t kIntVarFinalVptrShowActive = 0x15d20b0;    // table 0x15d20a0 + 0x10
inline constexpr std::uint64_t kIntVarWorkerVptrShowActive = 0x15d1f18;   // table 0x15d1f08 + 0x10
inline constexpr std::uint64_t kIntVarFnTableShowActive = 0x15d2080;
inline constexpr std::uint64_t kIntVarDtorShowActiveRva = 0xa5bee0;
inline constexpr std::size_t kIntVarDtorSize = 0x36;

// Functor slot bodies (copy / clone / move-payload / bare-ret destroy) of the two tables.
inline constexpr std::uint64_t kIntVarFnCopyListenerRva = 0xa62440;
inline constexpr std::uint64_t kIntVarFnCloneListenerRva = 0xa62460;
inline constexpr std::uint64_t kIntVarFnMoveListenerRva = 0xa62480;
inline constexpr std::uint64_t kIntVarFnDestroyListenerRva = 0xa62490;
inline constexpr std::uint64_t kIntVarFnCopyShowActiveRva = 0xa62800;
inline constexpr std::uint64_t kIntVarFnCloneShowActiveRva = 0xa62820;
inline constexpr std::uint64_t kIntVarFnMoveShowActiveRva = 0xa62840;
inline constexpr std::uint64_t kIntVarFnDestroyShowActiveRva = 0xa62850;

// Virtual slots 3 / 6 of the two final classes (each `mov eax,1; ret`).
inline constexpr std::uint64_t kIntVarSlot3ListenerRva = 0xa60ea0;
inline constexpr std::uint64_t kIntVarSlot6ListenerRva = 0xa60ef0;
inline constexpr std::uint64_t kIntVarSlot3ShowActiveRva = 0xa61940;
inline constexpr std::uint64_t kIntVarSlot6ShowActiveRva = 0xa61990;
inline constexpr std::uint32_t kIntVarSlot3Result = 1;
inline constexpr std::uint32_t kIntVarSlot6Result = 1;

inline constexpr std::uint32_t kIntVarCtorFlags = 1;   // r9d at all three slot-306 sites

struct IntVarShapeInfo {
    std::uint64_t wrapper_rva;
    std::uint64_t worker_rva;
    std::uint64_t final_vptr;
    std::uint64_t worker_vptr;
    std::uint64_t fn_table;        // W: the table the wrapper seeds its local functor and the object with
    std::uint64_t dtor_rva;        // atexit destructor
};
[[nodiscard]] const IntVarShapeInfo& int_var_shape_info(IntVarShape shape) noexcept;

// True for the two functor tables whose four slot bodies are lifted.
[[nodiscard]] bool int_var_fn_table_lifted(std::uint64_t table) noexcept;

struct IntVarCtorResult {
    bool ok = false;                 // every stage completed
    bool unlifted_functor = false;   // refused before any write: argument functor table not lifted
    ConCmdRegisterResult registration{};
    std::uint64_t installed_fn_table = 0;   // the object's +0xc0 tag after construction (A)
};

// Worker 0xa62660 / 0xa62a20 only: leaves vptr = worker_vptr and does NOT touch +0x68 / +0xd0 /
// registration. `block` is the wrapper's local copy (same layout as the caller's argument block).
// `registered` fields untouched except what the base constructor writes.
[[nodiscard]] bool init_int_var_worker(IntVarShape shape, ConCmdRangedVar& var, const SharedStr& name,
                                       std::int32_t min_value, std::int32_t max_value,
                                       std::uint32_t flags, const RangedVarArgs& block,
                                       const SharedStr& description) noexcept;

// Wrapper 0xa624a0 / 0xa62860 including the registration. See the header for the exact sequence.
[[nodiscard]] IntVarCtorResult init_int_var(IntVarShape shape, ConCmdRangedVar& var,
                                            const SharedStr& name, std::int32_t default_value,
                                            std::int32_t min_value, std::int32_t max_value,
                                            std::uint32_t flags, const RangedVarArgs& args,
                                            const SharedStr& description,
                                            ConCmdRegistry& registry) noexcept;

// `0x26ea0` virtual-slot results for an int-var target. known == false (fail-closed) unless the
// object's vptr is one of the two final vptrs above.
[[nodiscard]] DebugMenuTarget int_var_menu_target(const ConCmdRangedVar& var) noexcept;

// ---- the three slot-306 construction sites ----
struct Slot306IntVarSite {
    const char* name;              // registered console name (ELF literal)
    std::uint32_t name_len;
    std::uint64_t name_literal_rva;
    std::uint64_t call_site_rva;   // the `call` to the wrapper
    std::uint64_t global_rva;      // the object's global address
    IntVarShape shape;
    std::int32_t default_value;
    std::int32_t min_value;
    std::int32_t max_value;
};

inline constexpr std::array<Slot306IntVarSite, 3> kSlot306IntVarSites{{
    {"sndx_listenerOrientation", 24, 0x12c4a90, 0xa63497, 0x32f8ff8, IntVarShape::listener, 0, 0, 1},
    {"sndx_listenerPosition", 21, 0x12c4aa9, 0xa63567, 0x32f90d0, IntVarShape::listener, 0, 0, 1},
    {"sndx_showActive", 15, 0x12c4bb7, 0xa640c7, 0x32f9a78, IntVarShape::show_active, 0, 0, 2},
}};

struct IntVarSiteResult {
    bool ok = false;
    IntVarCtorResult ctor{};
    bool arg_functor_destroyed = false;   // `call [tag+0x18]` on the caller's argument functor (bare ret)
    bool name_release_inert = false;      // `call 0xf590` on the borrowed name view: exact no-op
    bool atexit_recorded = false;         // __cxa_atexit(dtor, object, dso) recorded, not run
};

// One site, exactly as the slot-306 body does it: build the borrowed name view and the zero argument
// block (functor tag = the shape's table, payload/aux 0), call the wrapper with flags 1 and the shared
// empty description (0xf730), then destroy the argument functor, release the name view and record the
// atexit destructor. `index` is 0..2 into kSlot306IntVarSites; anything else yields ok == false.
[[nodiscard]] IntVarSiteResult init_slot306_int_var(std::size_t index, ConCmdRangedVar& var,
                                                    ConCmdRegistry& registry) noexcept;

}  // namespace sotc
