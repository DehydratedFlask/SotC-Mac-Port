// CPU-01-STARTUP-SLOT-468: native model of startup constructor slot 468
// (RVA 0x3c3a90, 4,772 B; FDE [0x3c3a90, 0x3c4d34), 12 nops to 0x3c4d40; sha
// 26e0067a...1c7d82). The constructor-table word 0x2dd70e0 - 8 * 468 = 0x2dd6240 holds 0x3c3a90.
//
// Body order (every address below is read from the pinned ELF):
//   1. three 0x50-byte app-info delegate entries at 0x2ef6408 / 0x2ef6458 / 0x2ef64a8:
//      RequestAutomationCommands (25, handler 0x3c0200), LoadAutomationPlanRemote (24,
//      0x3c0220), SetAutomationIndex (18, 0x3c0240); built by the shared 0x4b560 / 0xf590
//      sequence with __cxa_atexit(0x4b660).
//   2. five 0x90-byte scripted-command entries at 0x2ef64f8 ... 0x2ef6738, built by
//      0x3c2a00 and registered through 0x3c0840 when 0x2ef63f8 is non-null:
//      wait, conCommand, print, waitForUser, waitForResourceLoading. Each carries three
//      {entry+0x88, identity tag} descriptors (tags 0x3c1580..0x3c1f30) and
//      __cxa_atexit(0x3c15e0).
//   3. ten 0xa0-byte commands at 0x2ef67c8 ... 0x2ef6d68: automate_userdone, automate,
//      automate_go, automate_pause, automate_rewind, automate_clear, automate_loadstring,
//      automate_loadfile, automate_throwerror, writetofile. All use the 0x20af0 base with
//      vptr 0x1594dd0, tail {self, 0x1990, 0, handler}, register through 0x20d60 and record
//      __cxa_atexit(0x4b0). automate_rewind additionally reserves one 0x50-byte token
//      (0x20540), writes {kind=0, "pause" SharedStr, empty SharedStr at +0x28, byte +0x48=1},
//      frees the stack view (0xf590), registers through vtable slot +0x10 (0x20d60), calls the
//      identity cell 0x15b0e10 +0x18 (0x3c3a80), then releases the two stack views.
//
// 18 atexit rows in body order: 3 x 0x4b660, 5 x 0x3c15e0, 10 x 0x4b0.
// 10 command registrations (nine direct calls through %r13 = 0x20d60, one through the command
// vtable's +0x10 slot), 3 app-info queue inserts and 5 optional RB registry inserts.
// No menu registration exists in this body. Vtables, functor tags, callbacks and destructors
// are identity data and are never executed by this model.
//
// The 0x20540 reserve and the inline push (the 0x20640 grow path is the branch not taken after
// the successful reserve) are represented with caller-owned token storage; the byte-level
// checker pins both bodies and all three call sites, and the token fields are exact.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "lane_m5_app_entry.hpp"
#include "scripted_entry_3c2a00.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot468Rva = 0x3c3a90;
inline constexpr std::size_t kSlot468Size = 4772;
inline constexpr std::uint64_t kSlot468TableWord = 0x2dd6240;

inline constexpr std::uint64_t kSlot468AppBase = 0x2ef6408;
inline constexpr std::uint64_t kSlot468EntryBase = 0x2ef64f8;
inline constexpr std::uint64_t kSlot468CommandBase = 0x2ef67c8;
inline constexpr std::size_t kSlot468AppCount = 3;
inline constexpr std::size_t kSlot468EntryCount = 5;
inline constexpr std::size_t kSlot468CommandCount = 10;

inline constexpr std::uint64_t kSlot468AppDtor = 0x4b660;
inline constexpr std::uint64_t kSlot468EntryDtor = 0x3c15e0;
inline constexpr std::uint64_t kSlot468CommandDtor = 0x4b0;
inline constexpr std::uint64_t kSlot468CommandVptr = 0x1594dd0;
inline constexpr std::uint64_t kSlot468CommandVtable = 0x1594dc0;
inline constexpr std::uint64_t kSlot468CommandRegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot468CommandTail8 = 0x1990;
inline constexpr std::uint32_t kSlot468CommandFlags = 2;

inline constexpr std::uint64_t kSlot468ReserveRva = 0x20540;
inline constexpr std::size_t kSlot468ReserveSize = 253;
inline constexpr std::uint64_t kSlot468PushRva = 0x20640;
inline constexpr std::size_t kSlot468PushSize = 342;
inline constexpr std::uint64_t kSlot468EmptyViewRva = 0xf180;
inline constexpr std::size_t kSlot468EmptyViewSize = 28;
inline constexpr std::uint64_t kSlot468TokenEmptyData = 0x2dd9f78;
inline constexpr std::uint64_t kSlot468RewindCell = 0x15b0e10;
inline constexpr std::uint64_t kSlot468RewindSecondaryHook = 0x3c3a80;

struct Slot468AppSpec {
    const char* name;
    std::size_t name_length;
    std::uint64_t handler;
    std::uint64_t object;
};

inline constexpr Slot468AppSpec kSlot468Apps[kSlot468AppCount] = {
    {"RequestAutomationCommands", 25, 0x3c0200, 0x2ef6408},
    {"LoadAutomationPlanRemote", 24, 0x3c0220, 0x2ef6458},
    {"SetAutomationIndex", 18, 0x3c0240, 0x2ef64a8},
};

struct Slot468EntrySpec {
    const char* name;
    std::size_t name_length;
    const char* description;
    std::size_t description_length;
    std::uint64_t fns[3];
    std::uint64_t object;
};

inline constexpr Slot468EntrySpec kSlot468Entries[kSlot468EntryCount] = {
    {"wait", 4, "Waits the indicated number of seconds. Params: <seconds to wait>", 64,
     {0x3c1580, 0x3c15b0, 0x3c15d0}, 0x2ef64f8},
    {"conCommand", 10, "Execute the indicated console command. Params: <console command text>", 69,
     {0x3c15f0, 0x3c16b0, 0x3c16c0}, 0x2ef6588},
    {"print", 5, "Prints text to the console. Params <text to print>", 50,
     {0x3c16d0, 0x3c1be0, 0x3c1bf0}, 0x2ef6618},
    {"waitForUser", 11,
     "Wait until the user performs some task. User signals they are done by firing the console command "
     "automate_userdone. Params: None",
     128, {0x3c1c00, 0x3c1d00, 0x3c1e00}, 0x2ef66a8},
    {"waitForResourceLoading", 22, "Wait until the Resource Loader finishes. Params: None", 53,
     {0x3c1f00, 0x3c1f10, 0x3c1f30}, 0x2ef6738},
};

struct Slot468CommandSpec {
    const char* name;
    std::size_t name_length;
    const char* description;
    std::size_t description_length;
    std::uint64_t callback;
    bool rewind;
    std::uint64_t object;
};

inline constexpr Slot468CommandSpec kSlot468Commands[kSlot468CommandCount] = {
    {"automate_userdone", 17,
     "If the automation system is waiting for the user to do something with waitForUser, this indicates "
     "the user is done and that automation can continue. Params: None",
     161, 0x3c1f40, false, 0x2ef67c8},
    {"automate", 8,
     "Puts a command in the current automation plan. Params: <automation command or console command>", 94,
     0x3c1f70, false, 0x2ef6868},
    {"automate_go", 11, "Starts the current automation. Params: None", 43, 0x3c2020, false, 0x2ef6908},
    {"automate_pause", 14, "Pauses the current automation. Params: None", 43, 0x3c2040, false, 0x2ef69a8},
    {"automate_rewind", 15, "Rewinds the current automation. Params: None", 44, 0x3c2060, true,
     0x2ef6a48},
    {"automate_clear", 14, "Clears the current automation plan. Params: None", 48, 0x3c20c0, false,
     0x2ef6ae8},
    {"automate_loadstring", 19, "Loads a string as an automation plan. Params: <string to load>", 62,
     0x3c20e0, false, 0x2ef6b88},
    {"automate_loadfile", 17, "Loads a text file as an automation plan. Params: <name of file to load>", 71,
     0x3c2190, false, 0x2ef6c28},
    {"automate_throwerror", 19, "Produces an automation error for testing. Params: None", 54,
     0x3c22a0, false, 0x2ef6cc8},
    {"writetofile", 11,
     "Writes data to file. Useful for signalling completion. Params: <name of file to write to> <text to "
     "write out>",
     109, 0x3c22b0, false, 0x2ef6d68},
};

// 0x50-byte token built by the inline push at 0x4761..0x4798 (the 0x20640 grow path builds the
// same shape through the 0x17090 allocator and 0xf180 empty-view constructor).
struct Slot468Token {
    std::uint32_t kind;       // +0x00 = 0
    std::uint8_t pad04[4];    // +0x04 not written by the inline store
    SharedStr text;           // +0x08 {0xfe7955 "pause", 5, 0, 0}
    SharedStr empty;          // +0x28 {0x2dd9f78, 0, 0, 0}
    std::uint8_t flag48;      // +0x48 = 1
    std::uint8_t pad49[7];    // +0x49..+0x4f not written
};
static_assert(sizeof(Slot468Token) == 0x50 && offsetof(Slot468Token, text) == 0x08 &&
              offsetof(Slot468Token, empty) == 0x28 && offsetof(Slot468Token, flag48) == 0x48);

// 0xa0-byte command: ConCmdBase + the 0x40-byte tail + the three-word vector at +0x80.
struct Slot468Command {
    ConCmdBase base;                     // +0x00
    std::uint64_t tail0;                 // +0x60 self pointer
    std::uint64_t tail8;                 // +0x68 0x1990
    std::uint64_t tail10;                // +0x70 0
    std::uint64_t tail18;                // +0x78 callback identity tag
    Slot468Token* vec_begin;             // +0x80
    Slot468Token* vec_end;               // +0x88
    Slot468Token* vec_cap;               // +0x90
    std::uint64_t gap98;                 // +0x98 never written
};
static_assert(sizeof(Slot468Command) == 0xa0 && offsetof(Slot468Command, tail0) == 0x60 &&
              offsetof(Slot468Command, vec_begin) == 0x80 && offsetof(Slot468Command, gap98) == 0x98);

struct Slot468Objects {
    lane_m5::AppInfoQueueEntry apps[kSlot468AppCount];   // +0x0000, stride 0x50
    ScriptedEntry entries[kSlot468EntryCount];           // +0x00f0, stride 0x90
    Slot468Command commands[kSlot468CommandCount];       // +0x03c0, stride 0xa0
    Slot468Token rewind_token;                           // host storage for the 0x50 heap token
};
static_assert(offsetof(Slot468Objects, apps) == 0x0000 && offsetof(Slot468Objects, entries) == 0x00f0 &&
              offsetof(Slot468Objects, commands) == 0x03c0 &&
              sizeof(Slot468Command) * kSlot468CommandCount == 0x640);

struct Slot468Hooks {
    // The call through 0x15b0e10 + 0x18 (0x3c3a80) is an identity call in the original. A host may
    // observe it; null records the identity call without executing guest code.
    bool (*rewind_secondary)(void* context, Slot468Command& command) noexcept = nullptr;
    void* rewind_secondary_context = nullptr;
    // Allocator/comparator for the optional 0x3c0840 registry path (0x2ef63f8 non-null).
    RbHooks entry_hooks{};
};

struct Slot468Result {
    bool ok = false;
    bool canary_matched = false;
    std::size_t apps = 0;
    std::size_t entries = 0;
    std::size_t commands = 0;
    std::size_t app_registrations = 0;
    std::size_t entry_inserts = 0;
    std::size_t command_registrations = 0;
    std::size_t atexit_records = 0;
    bool rewind_reserved = false;
    bool rewind_inline_push = false;
    bool rewind_grow_path = false;
    bool rewind_registered = false;
    bool rewind_secondary_called = false;
};

[[nodiscard]] Slot468Result init_slot468_3c3a90(Slot468Objects& objects,
                                                ConCmdRegistry& registry,
                                                lane_m5::AppInfoQueue& app_queue,
                                                const Slot468Hooks& hooks = {}) noexcept;

}  // namespace sotc
