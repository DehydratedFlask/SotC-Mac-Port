// The two terminal paths of the `0xe21420` decision contract.
//
// `0xe21420` is one 681-byte body with two continuation points that its guard ladder selects:
//
//   **found (0xe2167e, 75 B)** — pinned bytes:
//     rax = *(rbx)                          ; ctx
//     rdi = *(ctx + 0x8)                    ; provider
//     rax = provider->vtable[0x38](provider, name)      ; second lookup
//     r15 = 0x1b
//     if (result == null) -> return 0x1b
//     rcx = *(ctx); r8 = *(rcx + 0x148)      ; ctx vtable slot 0x148
//     rdx = result; rsi = arg2; ecx = (uint8)arg4
//     jmp *r8                                ; tail call, returns its value
//
//   **missing (0xe214f7, 191 B)**:
//     rdi = *(ctx + 0x8); r13 = provider->vtable[0xb0](provider, 0x48)   ; allocate 0x48 bytes
//     *(r13)      = 0x15e2d70                ; a vtable one slot into 0x15e2d60
//     *(r13 + 0x8) = provider
//     zero 32 B at +0x10 and +0x30; byte +0x40 = +0x41 = +0x42 = 1
//     r15 = 0x1b
//     if (r13 == null) -> return 0x1b
//     if (arg3 == null) -> the 0xe215ce sub-tree (a different scan; not modelled here)
//     else rescan provider->slot88(i) with strcmp, then provider->vtable[0xc0](r13) and r15 = 0x14
//
//   **null-name sub-tree (0xe215ce, 102 B)** — entered when `arg3` is null and the record exists:
//     primary = provider->vtable[0x88](provider, 0)
//     if (record->owner != null) {
//       if (record->+0x38 != null && strcmp(record->+0x38, primary) == 0 && record->+0x42 != 0)
//         record->owner->vtable[0xd0](owner, record->+0x38)
//       primary = record->owner->vtable[0xc8](owner, primary)
//     }
//     record->+0x38 = primary; record->+0x42 = 1;  -> shared exit
//
//   **first-scan match (0xe21634, 32 B)**: `record->vtable[0x30](record, provider->slot88(index))`
//   then the shared exit.
//
//   **shared exit (0xe21654, 42 B)**: `ctx->vtable[0x148](ctx, arg2, record, (uint8)arg4)`.
//
// What is modelled: all four continuations at the decision/call level. What is NOT modelled: the
// callee bodies behind slots 0x30/0x88/0xc8/0xd0/0x148, the per-element bodies of the scans, and
// any consumer of the constructed record.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kE21420Found18000Rva = 0xe2167e;
inline constexpr std::size_t kE21420Found18000Size = 0x4b;
inline constexpr std::uint64_t kE21420Missing18000Rva = 0xe214f7;
inline constexpr std::size_t kE21420Missing18000Size = 0xbf;
inline constexpr std::uint64_t kE21420InnerOffset = 0x08;
inline constexpr std::uint64_t kE21420MissingVtable = 0x15e2d70;
inline constexpr std::uint32_t kE21420LookupSlot = 0x38;
inline constexpr std::uint32_t kE21420AllocateSlot = 0xb0;
inline constexpr std::uint32_t kE21420ReleaseSlot = 0xc0;
inline constexpr std::uint32_t kE21420TailSlot = 0x148;
inline constexpr std::uint32_t kE21420NameSlot = 0x88;
inline constexpr std::uint32_t kE21420CountSlot = 0x78;
inline constexpr std::uint32_t kE21420AllocationSize = 0x48;
inline constexpr std::int32_t kE21420StatusMissing = 0x1b;
inline constexpr std::int32_t kE21420StatusScanExhausted = 0x14;
// Byte offsets inside the constructed 0x48-byte record.
inline constexpr std::uint32_t kE21420RecordVtable = 0x00;
inline constexpr std::uint32_t kE21420RecordOwner = 0x08;
inline constexpr std::uint32_t kE21420RecordZeroA = 0x10;
inline constexpr std::uint32_t kE21420RecordZeroB = 0x30;
inline constexpr std::uint32_t kE21420RecordFlags = 0x40;
inline constexpr std::uint64_t kE21420NullName18000Rva = 0xe215ce;
inline constexpr std::size_t kE21420NullName18000Size = 0x66;
inline constexpr std::uint64_t kE21420Match18000Rva = 0xe21634;
inline constexpr std::size_t kE21420Match18000Size = 0x20;
inline constexpr std::uint64_t kE21420Exit18000Rva = 0xe21654;
inline constexpr std::size_t kE21420Exit18000Size = 0x2a;
inline constexpr std::uint32_t kE21420RecordPrimary = 0x38;
inline constexpr std::uint32_t kE21420RecordReady = 0x42;
inline constexpr std::uint32_t kE21420RecordHookSlot = 0x30;
inline constexpr std::uint32_t kE21420PrimarySlot = 0x88;
inline constexpr std::uint32_t kE21420CompareSlot = 0xc8;
inline constexpr std::uint32_t kE21420NotifySlot = 0xd0;

enum class E21420TailStatus : std::uint8_t {
    ok = 0,
    no_provider,
    lookup_missing,   // the second lookup returned null
    tail_missing,     // the ctx vtable slot was not supplied
};

const char* e21420_tail_status_name(E21420TailStatus status) noexcept;

struct E21420FoundCells {
    void* context = nullptr;    // rbx: the ctx whose +0x8 is the provider
    void* provider = nullptr;   // *(ctx + 0x8)
    const char* name = nullptr; // r12
    void* arg2 = nullptr;       // r14
    std::uint8_t arg4_byte = 0; // the byte taken from the saved 4th argument
};

struct E21420FoundHooks {
    void* (*lookup)(void* provider, const char* name) = nullptr;             // slot 0x38
    std::int32_t (*tail)(void* context, void* arg2, void* object,
                         std::uint8_t arg4_byte) = nullptr;                 // slot 0x148
};

struct E21420FoundResult {
    bool ok = false;
    E21420TailStatus status = E21420TailStatus::no_provider;
    std::int32_t code = 0;      // 0x1b when the lookup is empty, else the tail's value
    bool lookup_called = false;
    bool tail_called = false;
};

[[nodiscard]] E21420FoundResult e21420_found(const E21420FoundCells& cells,
                                             const E21420FoundHooks& hooks) noexcept;

struct E21420MissingCells {
    void* context = nullptr;
    void* provider = nullptr;
    const char* name = nullptr;   // r12; null takes the 0xe215ce sub-tree
    void* record = nullptr;       // host storage for the 0x48-byte record (fail-visible if null)
};

struct E21420MissingHooks {
    void* (*allocate)(void* provider, std::uint32_t size) = nullptr;   // slot 0xb0
    std::int32_t (*count)(void* provider) = nullptr;                   // slot 0x78
    const char* (*name_at)(void* provider, std::int32_t index) = nullptr;  // slot 0x88
    void (*release)(void* record) = nullptr;                           // slot 0xc0
    // The arg3 == null sub-tree at 0xe215ce..0xe2167e is out of scope; it is reported, not run.
    bool (*null_name_subtree)(void* record) = nullptr;
};

struct E21420MissingResult {
    bool ok = false;
    E21420TailStatus status = E21420TailStatus::no_provider;
    std::int32_t code = kE21420StatusMissing;
    bool record_constructed = false;
    bool rescan_matched = false;
    bool release_called = false;
    bool null_name_subtree = false;
    std::int32_t scanned = 0;
};

// The shared exit: `ctx->vtable[0x148](ctx, arg2, record, (uint8)arg4)`. Every modelled continuation
// ends here, which is why the record pointer is the third argument in all of them.
struct E21420ExitCells {
    void* context = nullptr;
    void* arg2 = nullptr;
    void* record = nullptr;
    std::uint8_t arg4_byte = 0;
};

struct E21420ExitHooks {
    std::int32_t (*tail)(void* context, void* arg2, void* record, std::uint8_t arg4_byte) = nullptr;
};

struct E21420ExitResult {
    bool ok = false;
    bool tail_called = false;
    std::int32_t code = 0;
};

[[nodiscard]] E21420ExitResult e21420_exit(const E21420ExitCells& cells,
                                           const E21420ExitHooks& hooks) noexcept;

// The `arg3 == null` sub-tree: primary lookup, the optional compare/notify pair, the record update.
struct E21420NullNameCells {
    void* provider = nullptr;
    std::uint8_t* record = nullptr;   // the 0x48-byte record built by the missing path
};

struct E21420NullNameHooks {
    const char* (*primary)(void* provider, std::int32_t index) = nullptr;   // slot 0x88
    void (*notify)(void* owner, const char* value) = nullptr;               // slot 0xd0
    const char* (*compare)(void* owner, const char* value) = nullptr;       // slot 0xc8
};

struct E21420NullNameResult {
    bool ok = false;
    bool owner_present = false;
    bool notify_called = false;
    bool compare_called = false;
    bool ready_flag_written = false;
};

[[nodiscard]] E21420NullNameResult e21420_null_name(const E21420NullNameCells& cells,
                                                    const E21420NullNameHooks& hooks) noexcept;

// The first-scan match: `record->vtable[0x30](record, provider->slot88(index))`.
struct E21420MatchResult {
    bool ok = false;
    bool hook_called = false;
};

[[nodiscard]] E21420MatchResult e21420_match(void* record, const char* value,
                                             void (*record_hook)(void* record, const char* value)) noexcept;

[[nodiscard]] E21420MissingResult e21420_missing(const E21420MissingCells& cells,
                                                 const E21420MissingHooks& hooks) noexcept;

}  // namespace sotc
