// CPU-01-STARTUP-SLOTS-M4 (lane M4): native model of startup constructor slot 486 (RVA 0x2d5e00, 462 bytes).
// Original ELF sha256 d0723afb...5861d3; the constructor-table word for slot 486 is at 0x2dd70e0 - 486 * 8. Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_486.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order: (1) unbounded int variable `SM_debugging` (object 0x2eefa70, length 12, value 0, default 0,
// flags 1, empty description, argument-functor tag 0x15a2248): the 0x3a980 worker, f590(description), the
// bare-ret destroy through the tag's slot 3, 0x20d60, f590(name), __cxa_atexit(0x18a60) -- native as
// lane_m4::init_int_var_site; (2) string variable `SM_debugIdentifier` (object 0x2eefb48 = the int variable + 0xd8,
// length 18) built by 0x257a0 with an EMPTY initial/default value view (the same rodata byte 0xfce360 the int
// variable used as its description): f590(init view), f590(name), __cxa_atexit(0x23c90) -- native as
// init_string_var_257a0 (the same sequence as the `titleid` variable of slot 310). No debug-menu item follows.
// OBSERVATION only: what reads the variables (a "SM" debug switch and identifier, by their names) is NOT modelled.
#pragma once

#include "lane_m4_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot486Rva = 0x2d5e00;
inline constexpr std::size_t kSlot486Size = 462;
inline constexpr char kSlot486IntName[] = "SM_debugging";
inline constexpr char kSlot486StringName[] = "SM_debugIdentifier";
inline constexpr std::uint64_t kSlot486ArgFnTag = 0x15a2248;
inline constexpr std::uint64_t kSlot486IntRva = 0x2eefa70;
inline constexpr std::uint64_t kSlot486StringRva = 0x2eefb48;
inline constexpr std::uint32_t kSlot486Value = 0;
inline constexpr std::uint32_t kSlot486Default = 0;

struct Slot486Objects {
    ConCmdRangedVar int_var;        // 0x2eefa70
    ConCmdStringVar string_var;     // 0x2eefb48
};

struct Slot486Result {
    bool ok = false;
    lane_m4::IntVarSiteResult int_var{};
    StringVarResult string_var{};
    bool string_views_inert = false;      // the two 0xf590 calls after 0x257a0
    bool string_atexit_recorded = false;  // dtor 0x23c90 recorded, not run
};

[[nodiscard]] Slot486Result init_slot486_2d5e00(Slot486Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
