// CPU-01-STARTUP-SLOTS-M5 (lane M5): native model of startup constructor slot 516 (RVA 0x1b97b0, 177 bytes).
// Original ELF sha256 d0723afb...5861d3; the constructor-table word for slot 516 is at 0x2dd70e0 - 516 * 8. Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_516.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape (the slot 310 `titleid` string-variable site, alone): ONE string console variable `network_PlayerName`
// (length 18, object 0x2ee6780, 0x148 B) built by 0x257a0(obj, name view {0xfaf050, 0x12, 0, 0}, init view
// {0xfaf063, 0, 0, 0} = the empty string), then 0xf590 on the init view FIRST, then on the name view (both
// borrowed, inert), then __cxa_atexit(0x23c90, obj, &dso). OBSERVATION only: the initial value is the empty
// string; what reads or sets the player name is NOT modelled here.
#pragma once

#include "string_var_257a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot516Rva = 0x1b97b0;
inline constexpr std::size_t kSlot516Size = 177;
inline constexpr char kSlot516Name[] = "network_PlayerName";
inline constexpr std::uint64_t kSlot516ObjectRva = 0x2ee6780;

enum class Slot516Stage : std::uint8_t { none, string_var, view_release, atexit };

struct Slot516Objects {
    ConCmdStringVar var;   // 0x2ee6780
};

struct Slot516Result {
    bool ok = false;
    Slot516Stage blocked = Slot516Stage::none;
    StringVarResult var{};
};

[[nodiscard]] Slot516Result init_slot516_1b97b0(Slot516Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
