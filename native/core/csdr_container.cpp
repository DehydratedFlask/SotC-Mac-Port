#include "csdr_container.hpp"

#include <array>
#include <cstring>
#include <new>
#include <stdexcept>
#include <utility>

namespace sotc::csdr {
namespace {
constexpr std::array<std::uint8_t, 7> signature{0x4f, 0x72, 0x62, 0x53, 0x68, 0x64, 0x72};
constexpr std::uint32_t token = 0xbeeb03ff;
constexpr std::uint32_t endpgm = 0xbf810000;
constexpr std::size_t info_size = 28;

bool fits(std::size_t offset, std::size_t length, std::size_t size) noexcept {
    return offset <= size && length <= size - offset;
}

std::uint32_t u32(std::span<const std::uint8_t> bytes, std::size_t offset) noexcept {
    return std::uint32_t{bytes[offset]} | (std::uint32_t{bytes[offset + 1]} << 8) |
           (std::uint32_t{bytes[offset + 2]} << 16) | (std::uint32_t{bytes[offset + 3]} << 24);
}

std::uint64_t u64(std::span<const std::uint8_t> bytes, std::size_t offset) noexcept {
    return std::uint64_t{u32(bytes, offset)} | (std::uint64_t{u32(bytes, offset + 4)} << 32);
}

Result parse_impl(std::span<const std::uint8_t> bytes) {
    Result out;
    auto fail = [&](Status status, std::size_t offset) {
        out.status = status;
        out.error_offset = offset;
        out.programs.clear();
        return std::move(out);
    };
    if (bytes.data() == nullptr && !bytes.empty()) return fail(Status::invalid_input, 0);
    if (bytes.size() < 16) return fail(Status::too_short, bytes.size());
    out.declared_count = u32(bytes, 0);

    std::vector<std::size_t> infos;
    std::vector<std::size_t> tokens;
    // Byte-wise scan matches the observed format without assuming host alignment/endianness.
    // Bounded subtraction precedes all reads, additions and subspan construction.
    for (std::size_t at = 0; fits(at, 4, bytes.size()); ++at) {
        if (u32(bytes, at) == token && fits(at, 8, bytes.size())) tokens.push_back(at);
        if (fits(at, signature.size(), bytes.size()) &&
            std::memcmp(bytes.data() + at, signature.data(), signature.size()) == 0)
            infos.push_back(at);
    }
    out.observed_count = infos.size();
    if (infos.empty()) return fail(Status::no_programs, 4);
    // The only observed count exception occurs at GS-named paths and has numeric types 5/6.
    // This recognizes a rejection diagnostic, never an accepted-count exception or stage ABI.
    const bool gs_exception = out.declared_count == 1 && infos.size() == 2 &&
        fits(infos[0], info_size, bytes.size()) && fits(infos[1], info_size, bytes.size()) &&
        ((u32(bytes, infos[0] + 8) >> 2) & 15) == 5 &&
        ((u32(bytes, infos[1] + 8) >> 2) & 15) == 6;
    std::size_t previous_end = 4;
    for (const auto at : infos) {
        if (!fits(at, info_size, bytes.size())) return fail(Status::truncated_binary_info, at);
        Program program;
        program.info_offset = at;
        auto& info = program.info;
        const auto word = u32(bytes, at + 8);
        info.version = bytes[at + 7];
        info.pssl_or_cg = (word & 1) != 0;
        info.cached = (word & 2) != 0;
        info.type = (word >> 2) & 15;
        info.source_type = (word >> 6) & 3;
        info.length = word >> 8;
        info.chunk_usage_base_offset_in_dw = bytes[at + 12];
        info.num_input_usage_slots = bytes[at + 13];
        info.usage_flags = bytes[at + 14];
        info.reserved = bytes[at + 15];
        info.shader_hash = u64(bytes, at + 16);
        info.crc32 = u32(bytes, at + 24);
        if (info.version != 7) return fail(Status::unsupported_version, at + 7);
        if (info.source_type != 1) return fail(Status::unsupported_source_type, at + 8);
        switch (info.type) {
        case 0: case 1: case 2: case 4: case 7: case 8: break;
        default:
            if (!gs_exception) return fail(Status::unsupported_shader_type, at + 8);
        }

        std::size_t candidates = 0;
        for (const auto candidate : tokens) {
            if (candidate >= at || at - candidate < 8) continue;
            const auto distance = at - candidate;
            const auto immediate = u32(bytes, candidate + 4);
            // Equivalent to token+(imm+1)*8 == info, without unchecked multiply/add overflow.
            if (distance % 8 == 0 && std::uint64_t{distance / 8} ==
                                           std::uint64_t{immediate} + 1) {
                ++candidates;
                program.code_offset = candidate;
                program.token_immediate = immediate;
            }
        }
        if (candidates == 0) return fail(Status::missing_token, at);
        if (candidates != 1) return fail(Status::ambiguous_token, at);
        if (program.code_offset < previous_end || program.code_offset % 4 || at % 4)
            return fail(Status::invalid_span, program.code_offset);
        if (info.length < 12 || info.length % 4 || info.length > at - program.code_offset)
            return fail(Status::invalid_length, at + 8);
        if (u32(bytes, program.code_offset + info.length - 4) != endpgm)
            return fail(Status::missing_endpgm, program.code_offset + info.length - 4);
        previous_end = at + info_size; // fits(at, info_size, size) already proved this addition.
        out.programs.push_back(program);
    }
    if (out.declared_count != out.observed_count) {
        if (gs_exception)
            return fail(Status::unsupported_gs_double_program, 0);
        return fail(Status::count_mismatch, 0);
    }
    out.status = Status::ok;
    return out;
}
} // namespace

Result parse(std::span<const std::uint8_t> bytes) noexcept {
    try {
        auto result = parse_impl(bytes);
        // A prefix is only a rejection fallback: raw little-endian counts can have exactly
        // these bytes too (1676, 2700, 1740, 2764). A complete valid raw structure wins.
        if (!result && bytes.data() != nullptr && bytes.size() >= 2 &&
            (bytes[0] == 0xcc ||
             (bytes[0] == 0x8c && (bytes[1] == 0x06 || bytes[1] == 0x0a)))) {
            result.status = Status::unsupported_compression;
            result.error_offset = 0;
        }
        return result;
    } catch (const std::bad_alloc&) {
        return {Status::allocation_failed, 0, 0, 0, {}};
    } catch (const std::length_error&) {
        return {Status::allocation_failed, 0, 0, 0, {}};
    }
}

Result parse(const std::uint8_t* data, std::size_t size) noexcept {
    if (data == nullptr && size != 0) return {Status::invalid_input, 0, 0, 0, {}};
    return parse(std::span<const std::uint8_t>{data, size});
}

std::span<const std::uint8_t> code_bytes(std::span<const std::uint8_t> bytes,
                                         const Program& program) noexcept {
    if ((bytes.data() == nullptr && !bytes.empty()) ||
        !fits(program.code_offset, program.info.length, bytes.size()) ||
        !fits(program.info_offset, info_size, bytes.size()) ||
        program.code_offset > program.info_offset ||
        program.info.length > program.info_offset - program.code_offset)
        return {};
    return bytes.subspan(program.code_offset, program.info.length);
}

const char* status_name(Status status) noexcept {
#define CSDR_STATUS(value) case Status::value: return #value
    switch (status) {
    CSDR_STATUS(ok);
    CSDR_STATUS(invalid_input);
    CSDR_STATUS(too_short);
    CSDR_STATUS(unsupported_compression);
    CSDR_STATUS(no_programs);
    CSDR_STATUS(truncated_binary_info);
    CSDR_STATUS(unsupported_version);
    CSDR_STATUS(unsupported_source_type);
    CSDR_STATUS(unsupported_shader_type);
    CSDR_STATUS(missing_token);
    CSDR_STATUS(ambiguous_token);
    CSDR_STATUS(invalid_span);
    CSDR_STATUS(invalid_length);
    CSDR_STATUS(missing_endpgm);
    CSDR_STATUS(count_mismatch);
    CSDR_STATUS(unsupported_gs_double_program);
    CSDR_STATUS(allocation_failed);
    }
#undef CSDR_STATUS
    return "unknown_status";
}
} // namespace sotc::csdr
