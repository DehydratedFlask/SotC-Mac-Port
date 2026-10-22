// Codec fixture tests for the 12-byte save tagged-chunk header. Nothing here claims compatibility
// with a real save: the payload is not modelled and the tag values are opaque.
//
// Hash oracle: the FNV-1a-32 vectors below ("a" = 0xe40c292c, "foobar" = 0xbf9cf968) are the
// published FNV reference vectors for the 32-bit FNV-1a offset basis 0x811c9dc5. The test also
// carries its own byte-at-a-time FNV loop, and the implementation's uppercase mapping is checked
// against that loop (not against a value produced by the implementation).
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <span>
#include <string>
#include <string_view>

#include "core/save_chunk_header.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

// Independent FNV-1a-32 over raw bytes (no uppercase mapping, no project code).
std::uint32_t fnv1a32_reference(std::string_view bytes) {
    std::uint32_t hash = 0x811c9dc5u;
    for (const unsigned char byte : bytes) {
        hash = (hash ^ byte) * 0x01000193u;
    }
    return hash;
}

std::string uppercase_ascii(std::string_view name) {
    std::string out;
    out.reserve(name.size());
    for (const char c : name) {
        out.push_back((c >= 'a' && c <= 'z') ? static_cast<char>(c - 'a' + 'A') : c);
    }
    return out;
}

std::array<std::byte, 12> bytes_of(const char* text) {
    std::array<std::byte, 12> out{};
    for (std::size_t i = 0; i != 12; ++i) out[i] = static_cast<std::byte>(text[i]);
    return out;
}

}  // namespace

int main() {
    using namespace sotc;

    // --- published FNV-1a-32 vectors, checked through the independent loop first --------------
    CHECK(fnv1a32_reference("") == 0x811c9dc5u);
    CHECK(fnv1a32_reference("a") == 0xe40c292cu);
    CHECK(fnv1a32_reference("foobar") == 0xbf9cf968u);

    // --- exact 12-byte wire layout ------------------------------------------------------------
    SaveChunkHeader header;
    header.name_hash = 0x11223344u;
    header.tag = 0x5566u;
    header.payload_length = 0x778899aau;
    const auto wire = encode_save_chunk_header(header);
    CHECK(wire.size() == kSaveChunkHeaderSize);
    const auto expected = bytes_of("\x44\x33\x22\x11\xef\xbe\x66\x55\xaa\x99\x88\x77");
    CHECK(wire == expected);
    // Magic bytes are EF BE at offsets 4..5 (little-endian 0xBEEF).
    CHECK(std::to_integer<unsigned>(wire[4]) == 0xEFu);
    CHECK(std::to_integer<unsigned>(wire[5]) == 0xBEu);

    // --- roundtrip ---------------------------------------------------------------------------
    const auto parsed = parse_save_chunk_header(wire);
    CHECK(parsed.error == ChunkParseError::none);
    CHECK(parsed.header.name_hash == header.name_hash);
    CHECK(parsed.header.tag == header.tag);
    CHECK(parsed.header.payload_length == header.payload_length);
    // Trailing payload bytes are ignored by the header codec.
    std::array<std::byte, 20> with_payload{};
    std::memcpy(with_payload.data(), wire.data(), wire.size());
    with_payload[12] = std::byte{0xAA};
    const auto parsed_long = parse_save_chunk_header(with_payload);
    CHECK(parsed_long.error == ChunkParseError::none);
    CHECK(parsed_long.header.name_hash == header.name_hash);
    CHECK(parsed_long.header.tag == header.tag);
    CHECK(parsed_long.header.payload_length == header.payload_length);

    // --- truncation at every length 0..11 ----------------------------------------------------
    for (std::size_t length = 0; length != kSaveChunkHeaderSize; ++length) {
        const auto truncated = parse_save_chunk_header(
            std::span<const std::byte>(wire.data(), length));
        CHECK(truncated.error == ChunkParseError::truncated_header);
        CHECK(truncated.header.name_hash == 0 && truncated.header.tag == 0
              && truncated.header.payload_length == 0);
    }

    // --- wrong magic -------------------------------------------------------------------------
    for (const std::uint16_t magic : std::array<std::uint16_t, 4>{0x0000u, 0xBEEEu, 0xEFBEu, 0x1234u}) {
        auto bad = wire;
        bad[4] = static_cast<std::byte>(magic & 0xffu);
        bad[5] = static_cast<std::byte>((magic >> 8) & 0xffu);
        const auto rejected = parse_save_chunk_header(bad);
        CHECK(rejected.error == ChunkParseError::bad_magic);
        CHECK(rejected.header.name_hash == 0);
        CHECK(rejected.header.tag == 0);
        CHECK(rejected.header.payload_length == 0);
    }
    // A header that is both truncated and wrong-magic reports truncation first.
    auto short_bad = wire;
    short_bad[4] = std::byte{0};
    const auto short_bad_result = parse_save_chunk_header(std::span<const std::byte>(short_bad.data(), 5));
    CHECK(short_bad_result.error == ChunkParseError::truncated_header);

    // --- boundary payload lengths ------------------------------------------------------------
    SaveChunkHeader zero;
    zero.name_hash = 1;
    zero.tag = 4;
    zero.payload_length = 0;
    const auto zero_wire = encode_save_chunk_header(zero);
    CHECK(zero_wire == bytes_of("\x01\x00\x00\x00\xef\xbe\x04\x00\x00\x00\x00\x00"));
    const auto zero_parsed = parse_save_chunk_header(zero_wire);
    CHECK(zero_parsed.error == ChunkParseError::none && zero_parsed.header.payload_length == 0);

    SaveChunkHeader maximal;
    maximal.name_hash = std::numeric_limits<std::uint32_t>::max();
    maximal.tag = std::numeric_limits<std::uint16_t>::max();
    maximal.payload_length = std::numeric_limits<std::uint32_t>::max();
    const auto max_wire = encode_save_chunk_header(maximal);
    const auto max_parsed = parse_save_chunk_header(max_wire);
    CHECK(max_parsed.error == ChunkParseError::none);
    CHECK(max_parsed.header.name_hash == 0xffffffffu);
    CHECK(max_parsed.header.tag == 0xffffu);
    CHECK(max_parsed.header.payload_length == 0xffffffffu);
    CHECK(max_wire == bytes_of("\xff\xff\xff\xff\xef\xbe\xff\xff\xff\xff\xff\xff"));

    // --- name hash: implementation vs the independent loop -----------------------------------
    CHECK(save_chunk_name_hash("") == 0x811c9dc5u);   // empty name leaves the seed untouched
    CHECK(save_chunk_name_hash("foobar") == fnv1a32_reference("FOOBAR"));
    CHECK(save_chunk_name_hash("FOOBAR") == fnv1a32_reference("FOOBAR"));
    CHECK(save_chunk_name_hash("FoObAr") == save_chunk_name_hash("foobar"));
    CHECK(save_chunk_name_hash("abc") == save_chunk_name_hash("ABC"));
    CHECK(save_chunk_name_hash("abc") != save_chunk_name_hash("abd"));
    // Non-ASCII bytes are hashed unchanged (documented limitation: _Getptoupper is not modelled).
    CHECK(save_chunk_name_hash("\xC3\xA9") == fnv1a32_reference("\xC3\xA9"));
    CHECK(save_chunk_name_hash("_") == fnv1a32_reference("_"));
    for (const std::string_view name : {"level", "ProgressData", "x", "player_name_00"}) {
        CHECK(save_chunk_name_hash(name) == fnv1a32_reference(uppercase_ascii(name)));
    }

    std::puts("save_chunk_header: all checks passed");
    return 0;
}
