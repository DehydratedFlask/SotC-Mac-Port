// CPU-01-STARTUP-SLOT-568: native model of startup constructor slot 568 (RVA 0x207e0).
//
// Original ELF sha256 d0723afb...5861d3. FDE [0x207e0, 0x20aee), 782 B, then two nops to
// 0x20af0. Constructor-table word 0x2dd70e0 - 8 * 568 = 0x2dd5f20 holds 0x207e0. Body sha256
// 95b7a07e...4719b.
//
// The body builds ONE 0xa0-byte console command at 0x2dfca88:
//   name "LoadCmdsFromFile" (0xf2f0, 17), description 0xf959ec (49), flags 2, vptr
//   0x1594dd0 (0x1594dc0 + 0x10), tail {self, 0x1990, 0x2512a0, 0x1fe80}; the tail callback
//   word is non-zero, so the self field is rewritten from the stack self to &tail0.
//   vector at +0x80 is zeroed, then 0x20540(vec, 2) reserves two 0x50-byte tokens; both pushes
//   take the inline branch (end < cap), so the 0x20640 grow path is NOT taken.
//   token 0: kind 4, text "file" (4), empty view (0xf180); the empty view is then compared with
//            the 0xf730 static, released inertly and replaced by that static. Its +0x48 byte is
//            NOT written by this body (host storage is initialised, but that is not a store).
//   token 1: kind 0, text "inline" (6), empty view (0xf180), +0x48 byte = 0.
//   register through command vtable +0x10 = 0x20d60; call vtable 0x1594d60 +0x18 = 0x207d0
//   (a one-byte `ret`) on the 0x30-byte stack scratch at rsp+0xb0; release description and name;
//   __cxa_atexit(0x4b0, command, 0x1604218) once.
//
// The vector's 0x50-byte storage is host-owned (the original reserves it through 0x20540). The
// stores into each token are exact; the first token's +0x48 is deliberately not written. Vtable
// entries, the 0x2512a0 tail word, the 0x1fe80 handler, 0x20350 and 0x207d0 are identity data and
// are never executed. No original RVA is loaded, relocated or run.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "hashed_string_14c3a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot568Rva = 0x207e0;
inline constexpr std::size_t kSlot568Size = 0x30e;
inline constexpr std::uint64_t kSlot568TableWord = 0x2dd5f20;
inline constexpr std::uint64_t kSlot568CommandObject = 0x2dfca88;

inline constexpr std::uint64_t kSlot568NameViewRva = 0xf2f0;
inline constexpr std::size_t kSlot568NameViewSize = 0x28;
inline constexpr std::uint64_t kSlot568EmptyViewRva = 0xf180;
inline constexpr std::size_t kSlot568EmptyViewSize = 28;
inline constexpr std::uint64_t kSlot568EmptyStaticRva = 0xf730;
inline constexpr std::size_t kSlot568EmptyStaticSize = 0x72;
inline constexpr std::uint64_t kSlot568ReserveRva = 0x20540;
inline constexpr std::size_t kSlot568ReserveSize = 253;
inline constexpr std::uint64_t kSlot568PushRva = 0x20640;
inline constexpr std::size_t kSlot568PushSize = 342;
inline constexpr std::uint64_t kSlot568EmptyObject = 0x2dd9f50;
inline constexpr std::uint64_t kSlot568EmptyData = 0x2dd9f78;

inline constexpr std::uint64_t kSlot568CommandVptr = 0x1594dd0;
inline constexpr std::uint64_t kSlot568CommandVtable = 0x1594dc0;
inline constexpr std::uint64_t kSlot568CommandRegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot568CommandTail8 = 0x1990;
inline constexpr std::uint64_t kSlot568CommandTail10 = 0x2512a0;
inline constexpr std::uint64_t kSlot568CommandHandler = 0x1fe80;
inline constexpr std::uint64_t kSlot568CommandDtor = 0x4b0;
inline constexpr std::uint32_t kSlot568CommandFlags = 2;
inline constexpr std::size_t kSlot568CommandSize = 0xa0;
inline constexpr std::size_t kSlot568TokenCount = 2;

inline constexpr std::uint64_t kSlot568ScratchVtable = 0x1594d60;
inline constexpr std::uint64_t kSlot568ScratchTag = 0x20350;
inline constexpr std::uint64_t kSlot568NoopRva = 0x207d0;
inline constexpr std::size_t kSlot568ScratchSize = 0x30;

inline constexpr const char* kSlot568Name = "LoadCmdsFromFile";
inline constexpr std::size_t kSlot568NameLength = 16;
inline constexpr const char* kSlot568Description = "Load and dispatch ConVars/ConCommands from a file";
inline constexpr std::size_t kSlot568DescriptionLength = 0x31;
inline constexpr const char* kSlot568FileText = "file";
inline constexpr std::size_t kSlot568FileLength = 4;
inline constexpr const char* kSlot568InlineText = "inline";
inline constexpr std::size_t kSlot568InlineLength = 6;

// One 0x50-byte element of the command's +0x80 vector.
struct Slot568Token {
    std::uint32_t kind;          // +0x00 (4 / 0)
    std::uint8_t pad04[4];       // +0x04 never written
    SharedStr text;              // +0x08
    SharedStr empty;             // +0x28 (0xf180, then normalized through 0xf730)
    std::uint8_t flag48;         // +0x48 token 0: never written; token 1: 0
    std::uint8_t pad49[7];       // +0x49..+0x4f never written
};
static_assert(sizeof(Slot568Token) == 0x50 && offsetof(Slot568Token, text) == 0x08 &&
              offsetof(Slot568Token, empty) == 0x28 && offsetof(Slot568Token, flag48) == 0x48);

// 0xa0-byte command: ConCmdBase + the 0x40-byte tail + the vector at +0x80.
struct Slot568Command {
    ConCmdBase base;              // +0x00
    std::uint64_t tail0;          // +0x60 self pointer after the callback test
    std::uint64_t tail8;          // +0x68 0x1990
    std::uint64_t tail10;         // +0x70 0x2512a0
    std::uint64_t tail18;         // +0x78 0x1fe80 handler identity
    Slot568Token* vec_begin;      // +0x80
    Slot568Token* vec_end;        // +0x88
    Slot568Token* vec_cap;        // +0x90
    std::uint64_t gap98;          // +0x98 never written
};
static_assert(sizeof(Slot568Command) == 0xa0 && offsetof(Slot568Command, tail0) == 0x60 &&
              offsetof(Slot568Command, vec_begin) == 0x80 && offsetof(Slot568Command, gap98) == 0x98);

// The body also constructs a 0x30-byte stack scratch at rsp+0xb0 whose +0x20 is the vtable
// 0x1594d60 and +0x28 is 0x20350. Only the vtable's +0x18 slot (0x207d0, `ret`) is called.
struct Slot568Scratch {
    std::uint64_t gap00[4];       // +0x00..+0x1f not written
    std::uint64_t vtable;         // +0x20 = 0x1594d60
    std::uint64_t tag;            // +0x28 = 0x20350
};
static_assert(sizeof(Slot568Scratch) == 0x30 && offsetof(Slot568Scratch, vtable) == 0x20);

struct Slot568Objects {
    Slot568Command command;       // 0x2dfca88
    Slot568Token tokens[kSlot568TokenCount];  // host storage for the 0x20540 reserve
    Slot568Scratch scratch;       // stack scratch at rsp+0xb0
};
static_assert(offsetof(Slot568Objects, tokens) == 0xa0);

struct Slot568Hooks {
    // The 0x1594d60 + 0x18 call lands on the one-byte `ret` 0x207d0. A host may observe it; null
    // records the no-op without executing guest code. The hook sees the stack scratch only.
    bool (*noop_vcall)(void* context, Slot568Scratch& scratch) noexcept = nullptr;
    void* noop_context = nullptr;
};

struct Slot568Result {
    bool ok = false;
    bool canary_matched = false;
    bool reserved = false;
    bool inline_push_first = false;
    bool inline_push_second = false;
    bool grow_path = false;          // 0x20640 is pinned but not taken
    bool empty_normalized = false;   // token 0: compare + f590 + copy from 0xf730
    bool first_flag_written = false; // token 0 +0x48 is not stored by the body
    bool second_flag_written = true;
    bool registered = false;
    bool noop_vcall = false;
    std::size_t reserve_count = 0;
    std::size_t tokens = 0;
    std::size_t registrations = 0;
    std::size_t atexit_records = 0;
};

[[nodiscard]] Slot568Result init_slot568_207e0(Slot568Objects& objects, ConCmdRegistry& registry,
                                              const Slot568Hooks& hooks = {}) noexcept;

}  // namespace sotc
