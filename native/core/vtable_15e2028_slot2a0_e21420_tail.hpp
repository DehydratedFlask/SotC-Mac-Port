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
// What is modelled: the found path in full (lookup, null gate, tail call) and the missing path's
// record construction plus its status and the two observable exits. What is NOT modelled: the
// `arg3 == null` sub-tree at 0xe215ce..0xe2167e, the rescan's per-element bodies beyond the
// strcmp calls, and any consumer of the constructed record.
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

[[nodiscard]] E21420MissingResult e21420_missing(const E21420MissingCells& cells,
                                                 const E21420MissingHooks& hooks) noexcept;

}  // namespace sotc
