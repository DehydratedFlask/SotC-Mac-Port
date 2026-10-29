// CPU-01-STARTUP-SLOT-557 (lane m6): native model of startup constructor slot 557 (RVA 0x47490, 296 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 557. Every body fact below was
// read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_557.py. NOTHING here
// executes, loads or calls an original RVA: the vtable pointer, the handler 0x465a0 and the destructor 0x4b0 are
// IDENTITY TAGS, recorded (atexit log) and never run.
//
// Shape: the console command `mem` (object 0x2ea8868, description `Dump memory statistics`, handler tag 0x465a0
// never called, flags 2) built exactly like the slot 306 / slot 343 commands (0xf2f0 name, stack description view,
// 0x20af0, vtable store, delegate block, `call [vtbl + 0x20]` = register, two inert 0xf590 releases,
// __cxa_atexit(0x4b0)). Nothing else is constructed by this slot.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot557Rva = 0x47490;
inline constexpr std::size_t kSlot557Size = 296;
inline constexpr char kSlot557CommandName[] = "mem";
inline constexpr char kSlot557CommandDescription[] = "Dump memory statistics";
inline constexpr std::size_t kSlot557CommandDescriptionLength = 0x16;
inline constexpr std::uint64_t kSlot557HandlerTag = 0x465a0;

struct Slot557Objects {
    ConCmdCommand command;   // 0x2ea8868
};

struct Slot557Result {
    bool ok = false;
    FConCmdBuildResult command{};
};

[[nodiscard]] Slot557Result init_slot557_47490(Slot557Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
