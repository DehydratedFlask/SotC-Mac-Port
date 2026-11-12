// Guest-address -> entry kind/callable table for the native runtime
// (CPU-01-AOT-CFG-CALLABLE-01).
//
// Generated data lives in guest_entry_table.inc: every distinct RVA our native headers name, typed
// against the pinned ELF's `.eh_frame_hdr` (55,790 FDE ranges), the PT_LOAD map and the decoded entry
// bytes. `callable` records the measured entry shape: a normal SysV entry, a `jmp` thunk, a one-byte
// `ret` stub, or a non-entry row. See docs/ENTRY_CLOSURE.md for the counts and the indirect-site
// resolutions/blockers.
//
// A `RetailEntry` proves the ELF shape, not that a native function with the retail signature exists;
// `JmpThunk`/`RetStub` are real entries but alias another target / do nothing.

#pragma once

#include <cstdint>

namespace sotc {

enum class GuestEntryKind : std::uint8_t {
    FunctionStart = 1,
    FunctionInterior = 2,
    Plt = 3,
    ExecutableData = 4,
    ImageData = 5,
    OutsideImage = 6,
};

enum class GuestEntryCallable : std::uint8_t {
    RetailEntry = 1,
    JmpThunk = 2,
    RetStub = 3,
    NotCallable = 4,
};

struct GuestEntry {
    std::uint64_t rva;
    GuestEntryKind kind;
    GuestEntryCallable callable;
};

struct GuestEntryCounts {
    std::uint32_t entries;
    std::uint32_t function_start;
    std::uint32_t function_interior;
    std::uint32_t plt;
    std::uint32_t executable_data;
    std::uint32_t image_data;
    std::uint32_t outside_image;
    std::uint32_t retail_entry;
    std::uint32_t jmp_thunk;
    std::uint32_t ret_stub;
    std::uint32_t not_callable;
};

struct GuestIndirectSummary {
    std::uint32_t functions;
    std::uint32_t sites;
    std::uint32_t resolved;
    std::uint32_t blocked;
};

// nullptr when the RVA is not in the map.
const GuestEntry* guest_entry_find(std::uint64_t rva) noexcept;
const GuestEntryCounts& guest_entry_counts() noexcept;
const GuestIndirectSummary& guest_entry_indirect() noexcept;
const char* guest_entry_kind_name(GuestEntryKind kind) noexcept;
const char* guest_entry_callable_name(GuestEntryCallable callable) noexcept;

}  // namespace sotc
