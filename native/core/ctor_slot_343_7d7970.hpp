// CPU-01-STARTUP-SLOTS-343-356 (lane L4): native model of startup constructor slot 343 (RVA 0x7d7970, 378 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dda750 + (306 - 343) * 8. Every body fact
// below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_343.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: two zeroed 24-byte {begin,end,cap} statics (0x325eb28 / 0x325eb48), each with __cxa_atexit(0x7d3af0,
// cell) -- the destructor frees the storage when begin is non-null (pinned, never run) -- then the console
// command `fireCaption` (object 0x325eb70, description `Fires any caption by name. Params: <caption name>`,
// handler tag 0x7d6420 never called, flags 2) built exactly like the slot 306 commands.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot343Rva = 0x7d7970;
inline constexpr std::size_t kSlot343Size = 378;
inline constexpr char kSlot343CommandName[] = "fireCaption";
inline constexpr char kSlot343CommandDescription[] = "Fires any caption by name. Params: <caption name>";
inline constexpr std::size_t kSlot343CommandDescriptionLength = 0x31;
inline constexpr std::uint64_t kSlot343HandlerTag = 0x7d6420;

struct Slot343Objects {
    ZeroVector24 cell_a;     // 0x325eb28
    ZeroVector24 cell_b;     // 0x325eb48
    ConCmdCommand command;   // 0x325eb70
};

struct Slot343Result {
    bool ok = false;
    bool cell_a_atexit = false;
    bool cell_b_atexit = false;
    FConCmdBuildResult command{};
};

[[nodiscard]] Slot343Result init_slot343_7d7970(Slot343Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
