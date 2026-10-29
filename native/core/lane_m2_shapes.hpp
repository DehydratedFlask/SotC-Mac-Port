// Lane M2 (startup constructor slots 389 393 403 407 413 415 419 421): the few construction shapes those
// slots need that the existing helpers (core/ctor_shapes_343_356, core/ctor_slots_322_334, core/concmd_vars)
// do not expose. Original ELF sha256 d0723afb...5861d3. Every claim here is byte-pinned by the slot checkers
// tools/lift/check_ctor_slot_<idx>.py. NOTHING here executes, loads or calls an original RVA; vtable pointers,
// functor tags and destructor RVAs are IDENTITY TAGS (destructors are recorded in the atexit log, never run).
//
//   * init_bool_var_flagged: the bool-variable static initialiser with a caller-chosen `flags` argument to
//     0x20af0. core/concmd_vars init_bool_var_static hard-codes flags = 1; slot 421 passes `mov ecx, 8`.
//     The sequence and every store are otherwise identical (re-implemented here because the shared helper
//     cannot be edited from this lane).
//   * float arg functor 0x15cb148 (slot 413): its four slots are the lifted float functor bodies
//     0xc4a0/0xc4c0/0xc4e0/0xc4f0 (checker-pinned), exactly like 0x15ce318 / 0x15d57e0. The shared
//     init_float_var_static gates on a fixed list of such tables; this wrapper accepts 0x15cb148 and passes the
//     already-accepted equivalent 0x15ce318 to it. That substitution is output-neutral: the 0xc300 constructor
//     never stores the caller's functor tag (it always installs 0x1594318) and the destroy slot is a bare ret.
//   * int arg functor 0x15cb128 (slot 415): four slots 0xc720/0xc740/0xc760/0xc770 (checker-pinned), same
//     reasoning against the 0x3a980 worker (installs 0x15944a0). init_unbounded_int_var_site reproduces the
//     slot 334 sequence without the menu item.
#pragma once

#include <cstddef>
#include <cstdint>

#include "ctor_shapes_343_356.hpp"
#include "ctor_slots_322_334.hpp"

namespace sotc::lane_m2 {

inline constexpr std::uint64_t kFloatArgFnVtbl_15cb148 = 0x15cb148;   // slot 413 caller functor
inline constexpr std::uint64_t kIntArgFnTable_15cb128 = 0x15cb128;    // slot 415 caller functor

// ---- bool variable with explicit constructor flags -----------------------------------------------------------
// [obj] = 0x1594218 ; 0x20af0(flags) ; mid vptr, value_ref, zero69/zero70, payload, tag, aux ; f590(desc) ;
// [obj] = 0x1594218 ; value ; default ; 0x20d60 ; f590(name) ; __cxa_atexit(0x20b0). NUL-terminated borrowed
// name, empty borrowed description.
[[nodiscard]] BoolVarResult init_bool_var_flagged(ConCmdBoolVar& var, const char* name, std::uint64_t fn_tag,
                                                  std::uint32_t flags, ConCmdRegistry& registry,
                                                  std::uint8_t value, std::uint8_t default_value) noexcept;

// ---- float variable through 0xc300 with the slot 413 caller functor ---------------------------------------------
// True for the argument functors this lane accepts (the shared list plus 0x15cb148).
[[nodiscard]] bool is_m2_float_arg_functor(std::uint64_t vtbl) noexcept;

// Same contract as init_float_var_static; fails closed (nothing written) for an unknown functor.
[[nodiscard]] FloatVarResult init_float_var_site(ConCmdRangedVar& var, const FloatVarSpec& spec,
                                                 ConCmdRegistry& registry) noexcept;

// ---- unbounded int variable through the 0x3a980 worker ------------------------------------------------------------
struct IntVarSpec {
    const char* name;               // borrowed, NUL-terminated
    const char* description;        // borrowed, NUL-terminated (empty at the observed site)
    std::uint64_t arg_fn_table;     // caller argument functor table
    std::uint32_t flags;            // 0x20af0 flags (8 at slot 415)
    std::uint32_t value;            // dword +0xd0
    std::uint32_t default_value;    // dword +0x68
};

struct IntVarResult {
    bool ok = false;
    bool worker_ok = false;             // known argument functor, worker ran
    bool description_inert = false;     // f590(description) was an exact no-op
    bool functor_destroy_ok = false;    // [argfn + 0x18] is the bare-ret destroy slot
    ConCmdRegisterResult registration{};
    bool name_inert = false;
    bool atexit_recorded = false;       // dtor 0x18a60 recorded, not run
};

[[nodiscard]] bool is_m2_int_arg_functor(std::uint64_t table) noexcept;

// [obj] = 0x1594c08 ; 0x3a980 ; f590(description) ; functor destroy ; [obj] = 0x1594c08 ; value ; default ;
// 0x20d60 ; f590(name) ; __cxa_atexit(0x18a60). Fails closed before any write for an unknown functor.
[[nodiscard]] IntVarResult init_unbounded_int_var_site(ConCmdRangedVar& var, const IntVarSpec& spec,
                                                       ConCmdRegistry& registry) noexcept;

}  // namespace sotc::lane_m2
