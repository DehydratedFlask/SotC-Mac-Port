// CPU-01-STARTUP-SLOTS-M5 (lane M5): native model of startup constructor slot 509 (RVA 0x2196f0, 431 bytes).
// Original ELF sha256 d0723afb...5861d3; the constructor-table word for slot 509 is at 0x2dd70e0 - 509 * 8. Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_509.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape (the slot 345 shape): bool variable `showBroadcastEvents` (object 0x2ee86b0, tag 0x159b170, value 0, default 0) + a 0x26ea0
// toggle (item 0x2ee8778, flags 0x30) labelled `Game Manager` / `Show Broadcast Events`. Releases: text first, then label;
// then atexit(0x272d0). OBSERVATION only: what reads the variable is NOT modelled.
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot509Rva = 0x2196f0;
inline constexpr std::size_t kSlot509Size = 431;
inline constexpr char kSlot509Name[] = "showBroadcastEvents";
inline constexpr char kSlot509Label[] = "Game Manager";
inline constexpr char kSlot509Text[] = "Show Broadcast Events";
inline constexpr std::uint64_t kSlot509FnTag = 0x159b170;
inline constexpr std::uint64_t kSlot509VarRva = 0x2ee86b0;
inline constexpr std::uint64_t kSlot509ItemRva = 0x2ee8778;

struct Slot509Objects {
    ConCmdBoolVar var;      // 0x2ee86b0
    DebugMenuItem item;     // 0x2ee8778
};

struct Slot509Result {
    bool ok = false;
    BoolVarResult var{};
    MenuSiteResult item{};
};

[[nodiscard]] Slot509Result init_slot509_2196f0(Slot509Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
