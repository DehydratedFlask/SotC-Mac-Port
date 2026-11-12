// CPU-01-HELPER-CALLEE-MODELS-04: native model of the out-of-line bool console-variable constructor 0x274730 (0x295 B = 661, FDE 661,
// [0x274730, 0x2749c5)). Original ELF sha256 d0723afb...5861d3; every fact is byte-pinned by tools/lift/check_helper_274730.py. NOTHING here
// executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded and never run.
//
// It is the bool sibling of the ranged-int wrapper 0x7ec160 / 0xc580: the same 0xc1 B ConCmdBoolVar object as the inlined static bool constructors
// (init_bool_var_static), but built from a caller-provided functor argument block instead of zeros, with a FIXED empty description and flags == 1.
//   SysV args: rdi = this (ConCmdBoolVar), rsi = name (SharedStr*, forwarded to 0x20af0), rdx = initial (uint8_t*), rcx = args (RangedVarArgs-shaped).
// Observed order (all offsets relative to this):
//   [this] = 0x1594208 + 0x10 = 0x1594218   pre-store of the FINAL vptr (r15), then
//   0x20af0(this, name, desc{ptr 0xfc521a, len 0}, 1)                          the 0x60 B base; desc is a stack view with no control counter
//   [this] = 0x15940a0 + 0x10 = 0x15940b0 ; [+0x60] = this + 0xc0 (value_ref) ; dword [+0x69] = 0 ; qword [+0xa0] = 0 ; qword [+0xb8] = 0
//   [+0xb0] = 0x159ca60   a bool-family table (staging class; overwritten below)
//   ymm [+0x70] <- args.q[0..3] ; if args.q[3] != 0 then [+0x70] = this + 0x70      (the stack copy is made self-referential first and then re-copied,
//                                                                                     so the final self reference points into the object)
//   functor install (clone / copy / copy / copy / aux swap / bare-ret destroy through the family slots) = net effect
//        [+0xa0] = args.fn_payload ; [+0xb0] = 0x1594280 ; [+0xb8] = args.fn_aux
//     0xc2a0 / 0xc2c0 store the class constant 0x1594280, so the staging class 0x159ca60 does NOT survive: the final class is the constant 0x1594280.
//   f590(desc) (inert) ; call [0x159ca60-staging-class + 0x18] on the stack temporary = 0xc2f0 (bare ret)
//   [this] = 0x1594218 ; byte [+0xc0] = byte [+0x68] = *initial ; 0x20d60(this)
// The callee does NOT run __cxa_atexit and does NOT release the caller's name; the call sites do (and call [args.fn_vptr + 0x18] on their own block).
//
// Admission. The callee dereferences the caller's class `[args.fn_vptr + 8]` (clone). Only a table in the bool functor family
// (helper_bool_functor_tags.hpp: all 143 relocation-derived tables, each = {0xc2a0, 0xc2c0, 0xc2e0, 0xc2f0}) has a lifted slot 1, so any other tag is
// refused with nothing written (the original would run an unread slot).
//
// NOT established: what q[3] (an `.text` RVA at the observed sites, e.g. 0x26fcf0) is called with at run time; that is a callback pointer carried
// identity-only (the debug-tool path is not gameplay-reachable). The class destructor 0x20b0 is recorded by the call sites, never run.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_vars.hpp"
#include "helper_bool_functor_tags.hpp"
#include "helper_functor_tags.hpp"

namespace sotc {

inline constexpr std::uint64_t kBoolVarCtorRva = 0x274730;
inline constexpr std::size_t kBoolVarCtorSize = 661;
inline constexpr std::uint64_t kBoolVarCtorStagingClass = 0x159ca60;     // lea at +0x33 / +0x154 ; family member, not the final class
inline constexpr std::uint64_t kBoolVarCtorFinalFnClass = kBoolFunctorClassTag;   // 0x1594280, stored by the family copy / clone slots
inline constexpr std::uint64_t kBoolVarCtorEmptyDescRva = 0xfc521a;      // the description view's data pointer (a NUL byte)
inline constexpr std::uint32_t kBoolVarCtorFlags = 1;                    // movl $1, %ecx before 0x20af0
inline constexpr std::uint64_t kBoolVarCtorPreVptr = 0x1594218;          // 0x1594208 + 0x10, r15
inline constexpr std::uint64_t kBoolVarCtorMidVptr = 0x15940b0;          // 0x15940a0 + 0x10, kBoolVarMidVptr

struct BoolVar274730Result {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;
    bool desc_inert = false;                 // f590 on the stack description view was an exact no-op
    bool arg_destroy_ok = false;             // `call [0x159ca60 + 0x18]` is the bare-ret 0xc2f0
    ConCmdRegisterResult registration{};
};

// 0x274730. `args` is the caller's block: q[4] at +0, functor object at +0x20 (payload at +0x30, class at +0x40, aux at +0x48).
[[nodiscard]] BoolVar274730Result helper_274730_bool_var(ConCmdBoolVar& var, const SharedStr& name, const std::uint8_t* initial,
                                                         const RangedVarArgs& args, ConCmdRegistry& registry) noexcept;

// One observed call site's parameters. The block is described by value; q[3] != 0 is an identity-only callback pointer (the self-reference rule).
struct BoolVar274730SiteSpec {
    const char* name;
    std::size_t name_length;
    std::uint8_t initial;                    // byte stored into [var+0xc0] and [var+0x68]
    std::uint64_t arg_fn_tag;                // the call site's REAL argument-functor class tag (bool family)
    std::uint64_t q[4];                      // args.q[0..3]
    std::uint64_t fn_payload;                // args +0x30
    std::uint64_t fn_aux;                    // args +0x48
};

struct BoolVar274730SiteResult {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;
    bool ctor_ok = false;
    bool functor_destroy_ok = false;         // `call [argfn + 0x18]` is the bare-ret destroy (admitted bool family tag)
    bool views_inert = false;                // f590(desc) inside the callee and f590(name) at the site were exact no-ops
    ConCmdRegisterResult registration{};
    bool atexit_recorded = false;            // dtor 0x20b0 recorded, not run
};

// The whole call site in the observed order (slot 493 first site, 0x2777a4):
//   0x274730 ; call [argfn + 0x18] ; f590(name) ; __cxa_atexit(0x20b0, obj)
// A refused tag leaves `var` untouched and records nothing.
[[nodiscard]] BoolVar274730SiteResult helper_init_bool_var_274730_site(ConCmdBoolVar& var, const BoolVar274730SiteSpec& spec,
                                                                       ConCmdRegistry& registry) noexcept;

}  // namespace sotc
