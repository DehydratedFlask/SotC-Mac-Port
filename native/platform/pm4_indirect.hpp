// PM4 indirect-buffer traversal and policy-driven draw extraction.
//
// A type-3 packet whose opcode the caller classifies as "indirect" opens a nested command stream.
// The packet format does not fix where the address and size live in the payload, so the caller
// supplies that layout. This module does the traversal: depth limit, cycle detection, unreadable
// buffers, and a flat list of typed draws carrying their nesting depth.
//
// No opcode or register meaning is built in. The game imports `sceGnmDrawIndexAuto`,
// `sceGnmDrawIndexIndirect`, `sceGnmDispatchIndirect` and friends (see
// `docs/GPU_COMMAND_SURFACE.md`), but their PM4 encodings are not yet byte-verified, so the policy
// table is supplied by the caller and the fixture uses a synthetic table.
#pragma once

#include "pm4_decoder.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace sotc::pm4 {

enum class DrawKind : std::uint8_t {
    none = 0,
    index_auto = 1,
    index = 2,
    index_offset = 3,
    index_indirect = 4,
    dispatch = 5,
    dispatch_indirect = 6,
};

const char* draw_kind_name(DrawKind kind) noexcept;

struct OpcodeRule {
    std::uint16_t opcode = 0;
    DrawKind kind = DrawKind::none;
    // Payload dword index holding the draw count; 0xFF means "no count field".
    std::uint8_t count_field = 0xFF;
    // Non-zero: this opcode opens a nested command buffer.
    bool indirect = false;
    // Payload dword indexes for the nested buffer address (low, high) and size in dwords.
    std::uint8_t address_field = 0xFF;
    std::uint8_t size_field = 0xFF;
};

struct Policy {
    const OpcodeRule* rules = nullptr;
    std::size_t count = 0;
};

struct GuestReader {
    bool (*read)(void* user, std::uint64_t address, std::uint32_t* out, std::size_t words) = nullptr;
    void* user = nullptr;
};

struct DrawCommand {
    DrawKind kind = DrawKind::none;
    std::uint16_t opcode = 0;
    std::uint32_t count = 0;
    std::uint8_t depth = 0;
    std::size_t header_index = 0;  // index inside the buffer the packet came from
    std::uint64_t buffer_address = 0;
};

enum class TraversalStatus : std::uint8_t {
    ok = 0,
    empty_buffer,
    invalid_policy,
    reader_missing,
    depth_exceeded,
    cycle_detected,
    unreadable_indirect,
    bad_indirect_payload,
    decode_error,
};

const char* traversal_status_name(TraversalStatus status) noexcept;

struct TraversalLimits {
    std::uint8_t max_depth = 4;
    std::uint32_t max_buffers = 64;
    std::uint32_t max_draws = 4096;
};

struct TraversalStats {
    std::uint32_t buffers = 0;
    std::uint32_t indirects = 0;
    std::uint32_t draws = 0;
    std::uint32_t cycles = 0;
    std::uint32_t unreadable = 0;
    std::uint8_t max_depth_seen = 0;
};

struct TraversalResult {
    TraversalStatus status = TraversalStatus::ok;
    TraversalStats stats{};
    std::vector<DrawCommand> draws;
    std::uint64_t stop_address = 0;  // buffer that failed, when applicable
};

// Walks `root` with the caller's policy. Reads nested buffers through `reader`. A buffer whose
// address is already on the current path is a cycle, not a repeat visit.
[[nodiscard]] TraversalResult traverse(const std::uint32_t* root, std::size_t root_words,
                                       std::uint64_t root_address, const Policy& policy,
                                       const GuestReader& reader,
                                       TraversalLimits limits = {}) noexcept;

}  // namespace sotc::pm4
