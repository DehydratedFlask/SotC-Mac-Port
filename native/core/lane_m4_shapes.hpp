// Lane M4 shared site shapes for startup constructor slots 463 465 475 476 478 479 482 486 489 490
// (original ELF sha256 d0723afb...5861d3). Names are ours. Every shape below is the inline text of a
// constructor-table body read end to end from the pinned bytes and byte-pinned by the per-slot checkers
// tools/lift/check_ctor_slot_<N>.py. NOTHING here executes, loads or calls an original RVA: vtable pointers,
// functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Two shapes are new on top of the existing helpers (init_bool_var_site, init_toggle_site_26db0,
// init_string_var_257a0, init_unbounded_int_var_worker_3a980):
//
// 1. Late-release bool variable (slots 475, 478). Same object as init_bool_var_static but with a NON-EMPTY
//    description, no 0x15940b0 mid vptr, and BOTH stack views released after 0x20d60:
//      [obj] = 0x1594218 ; 0x20af0(obj, name, desc, 1) ; +0x60 = &value ; dword +0x69 = 0 ; ymm +0x70 = 0 ;
//      +0xa0 = 0 ; +0xb0 = functor tag ; +0xb8 = 0 ; [obj] = 0x1594218 ; +0xc0 = value ; +0x68 = default ;
//      0x20d60(obj) ; f590(desc) ; f590(name) ; __cxa_atexit(0x20b0, obj, &dso).
//    (The existing shape releases the description BEFORE 0x20d60 and the name after it.)
//
// 2. Unbounded int variable site (slots 486, 489): the 0x3a980 worker with the caller's argument-functor
//    block {zero q[4], payload 0, tag, aux 0}, then f590(desc), the bare-ret destroy through the block's
//    tag slot 3 (+0x18), [obj] = 0x1594c08, value (dword +0xd0) and default (dword +0x68), 0x20d60(obj),
//    f590(name), __cxa_atexit(0x18a60, obj, &dso). Same sequence as startup slot 334 minus its menu item.
//    The argument tags 0x159fae0 (slot 489) and 0x15a2248 (slot 486) have four slot relocations byte-pinned
//    to the same lifted bodies 0xc720 / 0xc740 / 0xc760 / 0xc770 as 0x15ce8c0; they are admitted directly by
//    the shared worker whitelist. The worker's modeled copy/clone path leaves the resulting class tag
//    0x15944a0, while the slot-specific checkers pin the caller tables.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "ctor_slots_322_334.hpp"
#include "string_var_257a0.hpp"

namespace sotc::lane_m4 {

inline constexpr std::uint64_t kIntVarDtorRva = 0x18a60;        // atexit destructor of an int variable
inline constexpr std::uint32_t kIntVarCtorFlags = 1;
inline constexpr std::uint64_t kIntArgTagSlot489 = 0x159fae0;   // argument-functor table, slot 489
inline constexpr std::uint64_t kIntArgTagSlot486 = 0x15a2248;   // argument-functor table, slot 486

// Borrowed view (control == nullptr) over `length` bytes of `text`.
[[nodiscard]] SharedStr borrowed_view_of(const char* text, std::size_t length) noexcept;

// Late-release bool variable site (slots 475, 478). See the header comment for the exact order.
[[nodiscard]] BoolVarResult init_bool_var_late_release(ConCmdBoolVar& var, const SharedStr& name,
                                                       const SharedStr& description, std::uint64_t fn_tag,
                                                       ConCmdRegistry& registry, std::uint8_t value,
                                                       std::uint8_t default_value) noexcept;

struct IntVarSiteResult {
    bool ok = false;
    bool worker_ok = false;
    bool description_inert = false;
    bool arg_functor_destroyed = false;   // `call [tag + 0x18]` resolved to the bare ret 0xc770
    ConCmdRegisterResult registration{};
    bool name_inert = false;
    bool atexit_recorded = false;
};

// Unbounded int variable site (slots 486, 489). `false`/nothing written for an unlifted argument tag.
[[nodiscard]] IntVarSiteResult init_int_var_site(ConCmdRangedVar& var, const SharedStr& name,
                                                 const SharedStr& description, std::uint64_t arg_fn_tag,
                                                 ConCmdRegistry& registry, std::uint32_t value,
                                                 std::uint32_t default_value) noexcept;

}  // namespace sotc::lane_m4
