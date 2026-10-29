// CPU-01-STARTUP-SLOT-359 (lane M1): native model of startup constructor slot 359 (RVA 0x785f60, 296 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 359 (checker-pinned). Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by
// tools/lift/check_ctor_slot_359.py. NOTHING here executes, loads or calls an original RVA: vtable pointers,
// functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape: ONE console command built exactly like the slot 306 / 343 commands (init_fconcmd_object): 0xf2f0 name view,
// description view {imm length, 0, 0} borrowed from rodata, delegate block {self, 0x1990, 0, handler} staged on the
// stack, 0x20af0 with flags 2, vptr 0x1594dd0, the vector at +0x80 zeroed, the virtual call [0x1594dc0 + 0x20] =
// 0x20d60 (register-once), f590(description), f590(name), __cxa_atexit(0x4b0, obj, 0x1604218). Object 0x3255c30, command
// `giveItem`, description "Give the player the indicated item. Params: <item_index> <from_hard_list>" (0x49 bytes), handler identity tag 0x7836d0 (the handler
// body is NOT recovered and never called).
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot359Rva = 0x785f60;
inline constexpr std::size_t kSlot359Size = 296;
inline constexpr char kSlot359CommandName[] = "giveItem";
inline constexpr char kSlot359CommandDescription[] = "Give the player the indicated item. Params: <item_index> <from_hard_list>";
inline constexpr std::size_t kSlot359CommandDescriptionLength = 0x49;
inline constexpr std::uint64_t kSlot359HandlerTag = 0x7836d0;
inline constexpr std::uint64_t kSlot359ObjectRva = 0x3255c30;

struct Slot359Objects {
    ConCmdCommand command;   // 0x3255c30
};

struct Slot359Result {
    bool ok = false;
    FConCmdBuildResult command{};
};

[[nodiscard]] Slot359Result init_slot359_785f60(Slot359Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
