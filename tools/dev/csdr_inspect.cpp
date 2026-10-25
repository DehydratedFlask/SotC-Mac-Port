#include "core/csdr_container.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

// Offline importer diagnostic. stdout is metadata JSON only, never shader bytes.
// --stream accepts repeated [little-endian u32 size][size bytes] frames and emits one line each.
// A malformed container is a named JSON result; malformed transport/IO exits 2.
namespace {
constexpr std::uint32_t max_bytes = 64 * 1024 * 1024;
void emit(const std::vector<std::uint8_t>& bytes) {
    const auto result = sotc::csdr::parse(bytes);
    std::cout << "{\"status\":\"" << sotc::csdr::status_name(result.status)
              << "\",\"error_offset\":" << result.error_offset
              << ",\"declared_count\":" << result.declared_count
              << ",\"observed_count\":" << result.observed_count << ",\"programs\":[";
    bool comma = false;
    for (const auto& program : result.programs) {
        if (comma) std::cout << ',';
        comma = true;
        const auto& info = program.info;
        std::cout << "{\"code_offset\":" << program.code_offset
                  << ",\"info_offset\":" << program.info_offset
                  << ",\"token_immediate\":" << program.token_immediate
                  << ",\"version\":" << unsigned(info.version)
                  << ",\"type\":" << unsigned(info.type)
                  << ",\"source_type\":" << unsigned(info.source_type)
                  << ",\"pssl_or_cg\":" << (info.pssl_or_cg ? "true" : "false")
                  << ",\"cached\":" << (info.cached ? "true" : "false")
                  << ",\"length\":" << info.length
                  << ",\"chunk_usage_base_offset_in_dw\":" << unsigned(info.chunk_usage_base_offset_in_dw)
                  << ",\"num_input_usage_slots\":" << unsigned(info.num_input_usage_slots)
                  << ",\"usage_flags\":" << unsigned(info.usage_flags)
                  << ",\"reserved\":" << unsigned(info.reserved)
                  << ",\"shader_hash\":" << info.shader_hash
                  << ",\"crc32\":" << info.crc32 << '}';
    }
    std::cout << "]}\n" << std::flush;
}

int stream() {
    for (;;) {
        std::array<std::uint8_t, 4> head{};
        std::cin.read(reinterpret_cast<char*>(head.data()), head.size());
        if (std::cin.gcount() == 0 && std::cin.eof()) return 0;
        if (std::cin.gcount() != 4) {
            std::cerr << "truncated stream frame length\n";
            return 2;
        }
        const std::uint32_t size = std::uint32_t{head[0]} | (std::uint32_t{head[1]} << 8) |
                                   (std::uint32_t{head[2]} << 16) | (std::uint32_t{head[3]} << 24);
        if (size > max_bytes) {
            std::cerr << "stream frame exceeds 64 MiB diagnostic limit\n";
            return 2;
        }
        std::vector<std::uint8_t> bytes(size);
        if (size) {
            std::cin.read(reinterpret_cast<char*>(bytes.data()), size);
            if (std::cin.gcount() != size) {
                std::cerr << "truncated stream frame payload\n";
                return 2;
            }
        }
        emit(bytes);
    }
}
} // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: csdr_inspect FILE | --stream\n";
        return 2;
    }
    if (std::strcmp(argv[1], "--stream") == 0) return stream();
    std::ifstream input(argv[1], std::ios::binary | std::ios::ate);
    const auto size = input.tellg();
    if (!input || size < 0 || size > max_bytes) {
        std::cerr << "cannot read input (64 MiB diagnostic limit)\n";
        return 2;
    }
    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size));
    input.seekg(0);
    if (!bytes.empty() && !input.read(reinterpret_cast<char*>(bytes.data()), bytes.size())) {
        std::cerr << "input read failed\n";
        return 2;
    }
    emit(bytes);
    return sotc::csdr::parse(bytes) ? 0 : 1;
}
