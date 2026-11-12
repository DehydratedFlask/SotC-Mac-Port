// CPU-01-STARTUP-SLOT-556: native model of startup constructor slot 556 (RVA 0x4ccb0).
//
// Original ELF sha256 d0723afb...5861d3. FDE [0x4ccb0, 0x4d1c4), 1,300 B; constructor-table word
// 0x2dd70e0 - 8 * 556 = 0x2dd5f80 holds 0x4ccb0. Body sha256
// f23d9fed626fdaffcda28e56b91cd9744e2c503e65ffe71921cfdab986284953.
//
// The body first zeroes the application-info singleton cell 0x2ea8978 and registers
// __cxa_atexit(0x4a660, &cell, &__dso_handle). It then builds FOUR 0x50-byte named app-info entries
// (0x2ea8988, 0x2ea89d8, 0x2ea8a28, 0x2ea8a78), each through the shared lane-M5 sequence:
// 0xf2f0 name, entry stores, 0x4b560 queue insert, 0xf590 stack-view release, 0x4b660 atexit.
//
// Finally it builds ONE 0xa0-byte console command at 0x2ea8ac8:
//   name "sendEmail" (9, 0xf99636), description 0xf99640 (51), flags 2,
//   vptr 0x1594dd0 (0x1594dc0 + 0x10), tail {self, 0x1990, 0, 0x4b980}; +0x80/+0x88/+0x90 are
//   zeroed, +0x98 is NOT written. Registration is through vtable +0x20 = 0x1594de0 -> 0x20d60.
//   Description then name are released, then __cxa_atexit(0x4b0, command, &__dso_handle).
//
// The four app entries use the existing lane_m5::AppInfoQueueEntry layout and 0x4b560/0x4b660
// contracts. The handler tags 0x4b650, 0x4b670, 0x4b720 and 0x4b870 are identity data; the command
// tail handler 0x4b980 is identity data too. None is executed. 0x4a660 is an identity destructor tag.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "lane_m5_app_entry.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot556Rva = 0x4ccb0;
inline constexpr std::size_t kSlot556Size = 0x514;
inline constexpr std::uint64_t kSlot556TableWord = 0x2dd5f80;

inline constexpr std::uint64_t kSlot556SingletonCellRva = 0x2ea8978;
inline constexpr std::uint64_t kSlot556SingletonDtorRva = 0x4a660;
inline constexpr std::uint64_t kSlot556Entry0Rva = 0x2ea8988;
inline constexpr std::size_t kSlot556EntryStride = 0x50;
inline constexpr std::size_t kSlot556AppCount = 4;
inline constexpr std::uint64_t kSlot556CommandRva = 0x2ea8ac8;

inline constexpr std::uint64_t kSlot556EntryRegisterRva = 0x4b560;
inline constexpr std::uint64_t kSlot556EntryDtorRva = 0x4b660;
inline constexpr std::uint64_t kSlot556DelegateInvokerRva = 0x4cca0;
inline constexpr std::uint64_t kSlot556DelegateInvokerCellRva = 0x15fe800;
static_assert(kSlot556EntryRegisterRva == lane_m5::kEntryRegisterRva);
static_assert(kSlot556EntryDtorRva == lane_m5::kEntryDtorRva);
static_assert(kSlot556DelegateInvokerRva == lane_m5::kDelegateInvokerRva);
static_assert(kSlot556DelegateInvokerCellRva == lane_m5::kDelegateInvokerCellRva);

inline constexpr std::uint64_t kSlot556CommandVtableRva = 0x1594dc0;
inline constexpr std::uint64_t kSlot556CommandVptrRva = 0x1594dd0;
inline constexpr std::uint64_t kSlot556CommandRegisterSlotRva = 0x1594de0;
inline constexpr std::uint64_t kSlot556CommandRegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot556CommandDtorRva = 0x4b0;
inline constexpr std::uint32_t kSlot556CommandFlags = 2;
inline constexpr std::size_t kSlot556CommandSize = 0xa0;
inline constexpr std::uint64_t kSlot556CommandTail8 = 0x1990;
inline constexpr std::uint64_t kSlot556CommandTail10 = 0;
inline constexpr std::uint64_t kSlot556CommandHandler = 0x4b980;

struct Slot556AppSpec {
    const char* name;
    std::size_t name_length;
    std::uint64_t handler;
};

// Body order: each name is built by 0xf2f0 at the stated literal and registered through 0x4b560.
inline constexpr Slot556AppSpec kSlot556Apps[] = {
    {"PingAllConnections", 18, 0x4b650},
    {"SendEngineDispatchCmd", 21, 0x4b670},
    {"SendConCommand", 14, 0x4b720},
    {"RequestListOfConCommands", 24, 0x4b870},
};
static_assert(sizeof(kSlot556Apps) / sizeof(kSlot556Apps[0]) == kSlot556AppCount);

inline constexpr const char* kSlot556CommandName = "sendEmail";
inline constexpr std::size_t kSlot556CommandNameLength = 9;
inline constexpr const char* kSlot556CommandDescription =
    "Sends an email. Params: <to> <subject> <opt: body>]";
inline constexpr std::size_t kSlot556CommandDescriptionLength = 51;

// The command is the 0xa0-byte FConCmd shape used by the earlier command pages.
struct Slot556Command {
    ConCmdBase base;              // +0x00
    std::uint64_t tail0;          // +0x60 self pointer after the callback test
    std::uint64_t tail8;          // +0x68 0x1990
    std::uint64_t tail10;         // +0x70 0
    std::uint64_t tail18;         // +0x78 0x4b980 handler identity
    std::uint64_t vec_begin;      // +0x80 zeroed
    std::uint64_t vec_end;        // +0x88 zeroed
    std::uint64_t vec_cap;        // +0x90 zeroed
    std::uint64_t gap98;          // +0x98 NOT written
};
static_assert(sizeof(Slot556Command) == 0xa0 && offsetof(Slot556Command, tail0) == 0x60 &&
              offsetof(Slot556Command, vec_begin) == 0x80 && offsetof(Slot556Command, gap98) == 0x98);

struct Slot556Objects {
    lane_m5::AppInfoQueueEntry apps[kSlot556AppCount];  // 0x2ea8988 + 0x50 * i
    Slot556Command command;                             // 0x2ea8ac8
};
static_assert(offsetof(Slot556Objects, command) == 4 * 0x50);
static_assert(sizeof(Slot556Objects) == 4 * 0x50 + 0xa0);

struct Slot556Result {
    bool ok = false;
    bool canary_matched = false;
    bool singleton_cleared = false;
    bool singleton_atexit_recorded = false;
    std::size_t apps = 0;
    std::size_t app_registrations = 0;
    std::size_t app_atexit_records = 0;
    bool command_registered = false;
    bool command_vector_cleared = false;
    bool command_gap98_written = false;
    std::size_t command_registrations = 0;
    std::size_t command_atexit_records = 0;
    std::size_t atexit_records = 0;
};

// `queue` is the shared application-info queue ([0x2ea8978] singleton + [0x2ea8980] head, owned by
// StartupState). The body zeroes the singleton cell before the four entries are queued.
[[nodiscard]] Slot556Result init_slot556_4ccb0(Slot556Objects& objects, lane_m5::AppInfoQueue& queue,
                                               ConCmdRegistry& registry) noexcept;

}  // namespace sotc
