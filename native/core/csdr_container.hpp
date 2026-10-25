#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

// Importer-only structural reader for the observed raw .csdr format. It neither decodes nor
// executes shader instructions. Caller-owned input must remain valid while code_bytes() is used.
// Container headers, resource bindings, shader semantics and checksum algorithms remain unmodelled.
namespace sotc::csdr {

enum class Status {
    ok,
    invalid_input,
    too_short,
    unsupported_compression,
    no_programs,
    truncated_binary_info,
    unsupported_version,
    unsupported_source_type,
    unsupported_shader_type,
    missing_token,
    ambiguous_token,
    invalid_span,
    invalid_length,
    missing_endpgm,
    count_mismatch,
    unsupported_gs_double_program,
    allocation_failed,
};

struct BinaryInfo {
    std::uint8_t version{};
    std::uint8_t type{}; // Raw numeric metadata; this parser does not establish stage semantics.
    std::uint8_t source_type{};
    bool pssl_or_cg{};
    bool cached{};
    std::uint32_t length{};
    std::uint8_t chunk_usage_base_offset_in_dw{};
    std::uint8_t num_input_usage_slots{};
    std::uint8_t usage_flags{}; // Retained verbatim; usage tables are not decoded.
    std::uint8_t reserved{};
    std::uint64_t shader_hash{}; // Metadata only; not recomputed or authenticated.
    std::uint32_t crc32{};       // Metadata only; algorithm/coverage not established.
};

struct Program {
    std::size_t code_offset{}; // Includes the token dword and its immediate.
    std::size_t info_offset{};
    std::uint32_t token_immediate{};
    BinaryInfo info{};
};

struct Result {
    Status status{Status::invalid_input};
    std::size_t error_offset{};
    std::uint32_t declared_count{};
    std::size_t observed_count{};
    std::vector<Program> programs; // Empty on every failure: no partial imports.
    explicit operator bool() const noexcept { return status == Status::ok; }
};

// Only version 7/source_type 1 and the observed stage codes are supported. The GS container
// exception (declared=1, two valid programs with the observed type pair 5/6) is rejected with a
// distinct status, as observed in gs_main/gsmainvoxelrasterize paths. The names/semantics of 5/6
// are unestablished. This does not weaken the count invariant. Allocation failure is explicit.
// A valid complete raw structure takes precedence over a colliding known compression prefix;
// known block prefixes (8c06/8c0a or cc with any following byte) are rejection diagnostics only.
Result parse(std::span<const std::uint8_t> bytes) noexcept;
// Validates null/nonempty before constructing a span. Other pointer lifetime/bounds are caller-owned.
Result parse(const std::uint8_t* data, std::size_t size) noexcept;
// Rechecks bounds, including for caller-constructed Program values; never returns an invalid span.
std::span<const std::uint8_t> code_bytes(std::span<const std::uint8_t> bytes,
                                         const Program& program) noexcept;
const char* status_name(Status status) noexcept;

} // namespace sotc::csdr
