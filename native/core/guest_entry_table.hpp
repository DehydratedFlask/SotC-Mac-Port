// Guest-address -> entry-kind table for the native runtime (CPU-01-ENTRY-MAP-CLOSURE-01).
//
// Generated data lives in guest_entry_table.inc: every distinct RVA our native headers name,
// typed against the pinned ELF's `.eh_frame_hdr` (55,790 FDE ranges) and PT_LOAD map. See
// docs/ENTRY_CLOSURE.md for the measured counts and docs/ENTRY_MAP.md for the source map.
//
// A `FunctionStart` is the only kind a dispatcher may treat as a call entry. `FunctionInterior`
// addresses are branch labels/literals inside a function, `Plt` entries are import stubs, and the
// data kinds are not code at all.

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

struct GuestEntry {
    std::uint64_t rva;
    GuestEntryKind kind;
};

struct GuestEntryCounts {
    std::uint32_t entries;
    std::uint32_t function_start;
    std::uint32_t function_interior;
    std::uint32_t plt;
    std::uint32_t executable_data;
    std::uint32_t image_data;
    std::uint32_t outside_image;
};

// nullptr when the RVA is not in the map.
const GuestEntry* guest_entry_find(std::uint64_t rva) noexcept;
const GuestEntryCounts& guest_entry_counts() noexcept;
const char* guest_entry_kind_name(GuestEntryKind kind) noexcept;

}  // namespace sotc
