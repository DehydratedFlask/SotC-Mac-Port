// Fixture tests for the save tagged-chunk stream container (outer 16-byte header + chunk body).
// Nothing here claims compatibility with a real save: payloads and tags are opaque, and the facts
// used are only those proved in the SAV-01 checkpoints (see core/save_stream.hpp).
//
// Oracles are independent of the implementation under test: expected wire bytes are written by
// hand/with a local byte-at-a-time FNV loop, never produced by save_stream or save_checksum.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "core/save_stream.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)
#define CHECK_ERR(got, want) do { const auto g_ = (got); if (g_ != (want)) { std::fprintf(stderr, "FAIL %s:%d %s: got %s want %s\n", __FILE__, __LINE__, #got, sotc::to_string(g_), sotc::to_string(want)); std::exit(1); } } while (0)

using sotc::DuplicatePolicy;
using sotc::SaveStreamError;
using Bytes = std::vector<std::byte>;

namespace {

std::uint32_t fnv_ref(const Bytes& bytes) {
    std::uint32_t h = 0x811c9dc5u;
    for (const std::byte b : bytes) h = (h ^ std::to_integer<std::uint32_t>(b)) * 0x01000193u;
    return h;
}

std::uint32_t name_ref(std::string_view name) {
    std::uint32_t h = 0x811c9dc5u;
    for (const char c : name) {
        unsigned char u = static_cast<unsigned char>(c);
        if (u >= 'a' && u <= 'z') u = static_cast<unsigned char>(u - 32);
        h = (h ^ u) * 0x01000193u;
    }
    return h;
}

Bytes B(std::initializer_list<unsigned> v) {
    Bytes out;
    for (const unsigned x : v) out.push_back(static_cast<std::byte>(x));
    return out;
}

void put32(Bytes& out, std::uint32_t v) {
    for (int i = 0; i != 4; ++i) out.push_back(static_cast<std::byte>((v >> (8 * i)) & 0xff));
}

// Independent chunk encoder (does not use encode_save_chunk_header).
Bytes chunk(std::uint32_t name_hash, std::uint16_t tag, const Bytes& payload) {
    Bytes out;
    put32(out, name_hash);
    out.push_back(std::byte{0xEF});
    out.push_back(std::byte{0xBE});
    out.push_back(static_cast<std::byte>(tag & 0xff));
    out.push_back(static_cast<std::byte>(tag >> 8));
    put32(out, static_cast<std::uint32_t>(payload.size()));
    out.insert(out.end(), payload.begin(), payload.end());
    return out;
}

// Independent outer wrapper with a CORRECT hash, for crafting chunk-level-bad bodies.
Bytes wrap(const Bytes& body, std::uint32_t dword3 = 0) {
    Bytes out;
    put32(out, 1);
    put32(out, fnv_ref(body));
    put32(out, static_cast<std::uint32_t>(body.size()));
    put32(out, dword3);
    out.insert(out.end(), body.begin(), body.end());
    return out;
}

sotc::SaveStreamReadResult parse(const Bytes& b, DuplicatePolicy p = DuplicatePolicy::allow) {
    return sotc::parse_save_stream(b, p);
}

Bytes sample_stream() {
    sotc::SaveStreamWriter w(0xd);
    CHECK_ERR(w.append_chunk("alpha", 4, B({1, 2, 3})), SaveStreamError::none);
    CHECK_ERR(w.append_chunk("Beta", 8, {}), SaveStreamError::none);
    CHECK_ERR(w.append_chunk("gamma_00", 13, B({0xff, 0x00, 0xef, 0xbe, 0x7f})), SaveStreamError::none);
    return w.serialize();
}

void test_wire_layout() {
    sotc::SaveStreamWriter w(4);
    const Bytes payload = B({0xde, 0xad, 0xbe, 0xef});
    CHECK_ERR(w.append_chunk_hashed(0x11223344u, 0x5566, payload), SaveStreamError::none);
    // Hand-written body: header 44 33 22 11 EF BE 66 55 04 00 00 00, then payload.
    const Bytes body = B({0x44, 0x33, 0x22, 0x11, 0xEF, 0xBE, 0x66, 0x55, 0x04, 0, 0, 0,
                          0xde, 0xad, 0xbe, 0xef});
    const std::uint32_t h = fnv_ref(body);
    Bytes expected = B({1, 0, 0, 0});
    put32(expected, h);
    expected.insert(expected.end(), {std::byte{16}, std::byte{0}, std::byte{0}, std::byte{0},
                                      std::byte{4}, std::byte{0}, std::byte{0}, std::byte{0}});
    expected.insert(expected.end(), body.begin(), body.end());
    CHECK(w.serialize() == expected);
    CHECK(w.body_size() == 16);
    CHECK(w.chunk_count() == 1);
    // The hash covers the body only: the outer header is not part of it.
    CHECK(h != fnv_ref(expected));
}

void test_name_hash_through_writer() {
    sotc::SaveStreamWriter w(1);
    CHECK_ERR(w.append_chunk("Progress", 4, {}), SaveStreamError::none);
    const Bytes out = w.serialize();
    Bytes expected_hdr;
    put32(expected_hdr, name_ref("PROGRESS"));
    for (int i = 0; i != 4; ++i) CHECK(out[16 + i] == expected_hdr[i]);
    CHECK(out[20] == std::byte{0xEF} && out[21] == std::byte{0xBE});
}

void test_round_trip() {
    const Bytes img = sample_stream();
    const auto r = parse(img);
    CHECK_ERR(r.error, SaveStreamError::none);
    CHECK(r.stream.header.version == 1);
    CHECK(r.stream.header.descriptor_id == 0xd);
    CHECK(r.stream.header.body_size == img.size() - 16);
    CHECK(r.stream.chunks.size() == 3);
    CHECK(r.stream.chunks[0].name_hash == name_ref("ALPHA"));
    CHECK(r.stream.chunks[0].tag == 4);
    CHECK(r.stream.chunks[0].payload == B({1, 2, 3}));
    CHECK(r.stream.chunks[1].name_hash == name_ref("BETA"));
    CHECK(r.stream.chunks[1].tag == 8);
    CHECK(r.stream.chunks[1].payload.empty());
    CHECK(r.stream.chunks[2].tag == 13);
    CHECK(r.stream.chunks[2].payload == B({0xff, 0x00, 0xef, 0xbe, 0x7f}));
    CHECK(r.stream.find_first("beta") == &r.stream.chunks[1]);   // case-insensitive via name hash
    CHECK(r.stream.find_first("nope") == nullptr);
    // Re-serialising from parsed chunks reproduces the bytes exactly.
    sotc::SaveStreamWriter w(r.stream.header.descriptor_id);
    for (const auto& c : r.stream.chunks) {
        CHECK_ERR(w.append_chunk_hashed(c.name_hash, c.tag, c.payload), SaveStreamError::none);
    }
    CHECK(w.serialize() == img);
}

void test_empty_body() {
    sotc::SaveStreamWriter w(7);
    const Bytes img = w.serialize();
    CHECK(img.size() == 16);
    const auto r = parse(img);
    CHECK_ERR(r.error, SaveStreamError::none);
    CHECK(r.stream.chunks.empty());
    CHECK(r.stream.header.body_hash == 0x811c9dc5u);   // hash of nothing is the seed
    CHECK(r.stream.header.descriptor_id == 7);
}

void test_boundary_payloads() {
    sotc::SaveStreamWriter w(0);
    Bytes big(100000);
    for (std::size_t i = 0; i != big.size(); ++i) big[i] = static_cast<std::byte>(i * 31 + 7);
    CHECK_ERR(w.append_chunk_hashed(0, 0, big), SaveStreamError::none);
    CHECK_ERR(w.append_chunk_hashed(0xffffffffu, 0xffff, {}), SaveStreamError::none);
    const auto r = parse(w.serialize());
    CHECK_ERR(r.error, SaveStreamError::none);
    CHECK(r.stream.chunks[0].payload == big);
    CHECK(r.stream.chunks[1].name_hash == 0xffffffffu && r.stream.chunks[1].tag == 0xffff);
}

void test_duplicate_policy() {
    // Writer default allows duplicates and keeps order.
    sotc::SaveStreamWriter allow(1);
    CHECK_ERR(allow.append_chunk("flag", 4, B({1})), SaveStreamError::none);
    CHECK_ERR(allow.append_chunk("FLAG", 4, B({2})), SaveStreamError::none);
    const Bytes img = allow.serialize();
    const auto r = parse(img);
    CHECK_ERR(r.error, SaveStreamError::none);
    CHECK(r.stream.chunks.size() == 2 && r.stream.chunks[0].payload == B({1}) &&
          r.stream.chunks[1].payload == B({2}));
    CHECK(r.stream.find_first("flag")->payload == B({1}));   // first wins
    // Reader opt-in reject.
    CHECK_ERR(parse(img, DuplicatePolicy::reject).error, SaveStreamError::reader_duplicate_name);
    // Writer opt-in reject leaves the writer unchanged.
    sotc::SaveStreamWriter rej(1, DuplicatePolicy::reject);
    CHECK_ERR(rej.append_chunk("flag", 4, B({1})), SaveStreamError::none);
    const std::size_t before = rej.body_size();
    CHECK_ERR(rej.append_chunk("Flag", 8, B({9, 9})), SaveStreamError::duplicate_name);
    CHECK(rej.body_size() == before && rej.chunk_count() == 1);
    CHECK_ERR(rej.append_chunk("other", 4, {}), SaveStreamError::none);
    CHECK_ERR(parse(rej.serialize(), DuplicatePolicy::reject).error, SaveStreamError::none);
}

void test_version() {
    const Bytes good = sample_stream();
    for (const std::uint32_t v : {0u, 2u, 3u, 0x01000000u, 0x80000001u, 0xffffffffu}) {
        Bytes b = good;
        for (int i = 0; i != 4; ++i) b[i] = static_cast<std::byte>((v >> (8 * i)) & 0xff);
        CHECK_ERR(parse(b).error, SaveStreamError::bad_version);
    }
    // Every single-byte change of the version dword is rejected as bad_version.
    for (int pos = 0; pos != 4; ++pos) {
        for (unsigned x = 1; x != 256; ++x) {
            Bytes b = good;
            b[pos] = static_cast<std::byte>(std::to_integer<unsigned>(b[pos]) ^ x);
            CHECK_ERR(parse(b).error, SaveStreamError::bad_version);
        }
    }
}

void test_header_corruption() {
    const Bytes good = sample_stream();
    // Bytes 4..7 = body hash, 8..11 = body size: every single-byte XOR (all 255 values) rejected.
    for (int pos = 4; pos != 12; ++pos) {
        for (unsigned x = 1; x != 256; ++x) {
            Bytes b = good;
            b[pos] = static_cast<std::byte>(std::to_integer<unsigned>(b[pos]) ^ x);
            const auto r = parse(b);
            CHECK(r.error != SaveStreamError::none);
            if (pos < 8) CHECK_ERR(r.error, SaveStreamError::bad_hash);
        }
    }
    // Every single-bit flip as well (subset of the above, kept as the literal spec statement).
    for (int pos = 4; pos != 12; ++pos) {
        for (int bit = 0; bit != 8; ++bit) {
            Bytes b = good;
            b[pos] = static_cast<std::byte>(std::to_integer<unsigned>(b[pos]) ^ (1u << bit));
            CHECK(parse(b).error != SaveStreamError::none);
        }
    }
}

void test_descriptor_id_not_hashed() {
    // PROVED: dword3 is not covered by the body hash and is not checked at load. The fixture
    // therefore accepts any dword3 and carries it through unchanged.
    const Bytes good = sample_stream();
    for (int pos = 12; pos != 16; ++pos) {
        Bytes b = good;
        b[pos] = static_cast<std::byte>(std::to_integer<unsigned>(b[pos]) ^ 0x5a);
        const auto r = parse(b);
        CHECK_ERR(r.error, SaveStreamError::none);
        CHECK(r.stream.header.descriptor_id != 0xd);
        CHECK(r.stream.chunks.size() == 3);
    }
}

void test_body_corruption() {
    const Bytes good = sample_stream();
    // FNV-1a is injective per byte position (xor then odd multiply), so every body flip is bad_hash.
    for (std::size_t pos = 16; pos != good.size(); ++pos) {
        for (unsigned x = 1; x != 256; ++x) {
            Bytes b = good;
            b[pos] = static_cast<std::byte>(std::to_integer<unsigned>(b[pos]) ^ x);
            CHECK_ERR(parse(b).error, SaveStreamError::bad_hash);
        }
    }
}

void test_truncation_every_offset() {
    const Bytes good = sample_stream();
    for (std::size_t n = 0; n != good.size(); ++n) {
        const Bytes cut(good.begin(), good.begin() + static_cast<std::ptrdiff_t>(n));
        const auto r = parse(cut);
        CHECK_ERR(r.error, n < 16 ? SaveStreamError::truncated_header : SaveStreamError::truncated_body);
    }
    CHECK_ERR(parse(good).error, SaveStreamError::none);
    // Empty-body stream truncated at every offset too.
    const Bytes empty = sotc::SaveStreamWriter(0).serialize();
    for (std::size_t n = 0; n != empty.size(); ++n) {
        const Bytes cut(empty.begin(), empty.begin() + static_cast<std::ptrdiff_t>(n));
        CHECK_ERR(parse(cut).error, SaveStreamError::truncated_header);
    }
}

void test_trailing_bytes() {
    Bytes b = sample_stream();
    b.push_back(std::byte{0});
    CHECK_ERR(parse(b).error, SaveStreamError::trailing_bytes);
}

void test_oversize() {
    auto header_only = [](std::uint32_t size) {
        Bytes b;
        put32(b, 1);
        put32(b, 0);
        put32(b, size);
        put32(b, 0);
        return b;
    };
    // Over the limit is rejected from the header alone, before any body check.
    for (const std::uint32_t s : {0x6400001u, 0x6400100u, 0x7fffffffu, 0x80000000u, 0xffffffffu}) {
        CHECK_ERR(parse(header_only(s)).error, SaveStreamError::size_too_large);
    }
    // Exactly at the limit is a size the loader accepts (here: body just missing).
    CHECK_ERR(parse(header_only(0x6400000u)).error, SaveStreamError::truncated_body);
    // A body at exactly the limit round-trips (one chunk of the remaining room).
    sotc::SaveStreamWriter w(1);
    const Bytes payload(sotc::kSaveStreamMaxBodySize - 12, std::byte{0x5a});
    CHECK_ERR(w.append_chunk("big", 4, payload), SaveStreamError::none);
    CHECK(w.body_size() == sotc::kSaveStreamMaxBodySize);
    // The writer refuses to cross the limit, even by one byte, and stays unchanged.
    CHECK_ERR(w.append_chunk_hashed(1, 1, {}), SaveStreamError::body_too_large);
    CHECK(w.body_size() == sotc::kSaveStreamMaxBodySize && w.chunk_count() == 1);
    const Bytes img = w.serialize();
    CHECK(img.size() == 16 + sotc::kSaveStreamMaxBodySize);
    const auto r = parse(img);
    CHECK_ERR(r.error, SaveStreamError::none);
    CHECK(r.stream.chunks.size() == 1 && r.stream.chunks[0].payload == payload);
    // A configured smaller limit is honoured; a larger one is clamped to 0x6400000.
    sotc::SaveStreamWriter small(1, DuplicatePolicy::allow, 20);
    CHECK_ERR(small.append_chunk_hashed(1, 1, Bytes(9)), SaveStreamError::body_too_large);
    CHECK_ERR(small.append_chunk_hashed(1, 1, Bytes(8)), SaveStreamError::none);
    CHECK_ERR(small.append_chunk_hashed(2, 1, {}), SaveStreamError::body_too_large);
    sotc::SaveStreamWriter huge(1, DuplicatePolicy::allow, ~std::size_t{0});
    CHECK_ERR(huge.append_chunk_hashed(1, 1, Bytes(sotc::kSaveStreamMaxBodySize - 11)),
              SaveStreamError::body_too_large);
    // A payload span larger than u32 is refused before any byte is read (pointer is never used).
    sotc::SaveStreamWriter w2(1);
    const std::byte dummy{};
    const std::span<const std::byte> fake(&dummy, std::size_t{0x100000000ull});
    CHECK_ERR(w2.append_chunk_hashed(1, 1, fake), SaveStreamError::payload_too_large);
    CHECK(w2.body_size() == 0);
}

void test_chunk_level_errors() {
    // All of these carry a CORRECT outer hash, so only chunk validation can reject them.
    const Bytes ok = chunk(0xabcdef01u, 4, B({1, 2}));
    CHECK_ERR(parse(wrap(ok)).error, SaveStreamError::none);

    // Partial chunk header after a valid chunk: every length 1..11.
    for (std::size_t extra = 1; extra != 12; ++extra) {
        Bytes body = ok;
        const Bytes next = chunk(1, 1, {});
        body.insert(body.end(), next.begin(), next.begin() + static_cast<std::ptrdiff_t>(extra));
        CHECK_ERR(parse(wrap(body)).error, SaveStreamError::truncated_chunk_header);
    }
    // Bad chunk magic.
    const unsigned bad_magics[4][2] = {{0, 0}, {0xEE, 0xBE}, {0xEF, 0xBF}, {0xBE, 0xEF}};
    for (const auto& m : bad_magics) {
        Bytes body = chunk(5, 4, {});
        body[4] = static_cast<std::byte>(m[0]);
        body[5] = static_cast<std::byte>(m[1]);
        CHECK_ERR(parse(wrap(body)).error, SaveStreamError::bad_chunk_magic);
    }
    // Payload length overruns the body: by one, and huge.
    for (const std::uint32_t len : {3u, 0x7fffffffu, 0xffffffffu}) {
        Bytes body = chunk(5, 4, B({1, 2}));
        for (int i = 0; i != 4; ++i) body[8 + i] = static_cast<std::byte>((len >> (8 * i)) & 0xff);
        CHECK_ERR(parse(wrap(body)).error, SaveStreamError::chunk_overrun);
    }
    // Payload shorter than declared by chunk boundary: second chunk header swallowed -> overrun/magic.
    Bytes body = chunk(5, 4, B({1, 2}));
    body[8] = std::byte{1};   // declares 1 byte payload, leaving a 1-byte stray tail
    CHECK_ERR(parse(wrap(body)).error, SaveStreamError::truncated_chunk_header);
}

void test_hash_checked_before_chunks() {
    // Corrupt body AND bad chunk magic: hash mismatch is reported first (fixture ordering).
    Bytes b = wrap(chunk(5, 4, {}));
    b[16 + 4] = std::byte{0};
    b[16 + 5] = std::byte{0};
    b[4] = static_cast<std::byte>(std::to_integer<unsigned>(b[4]) ^ 1);   // wrecks the hash only
    CHECK_ERR(parse(b).error, SaveStreamError::bad_hash);
}

void test_error_names() {
    CHECK(std::string(sotc::to_string(SaveStreamError::bad_hash)) == "bad_hash");
    CHECK(std::string(sotc::to_string(SaveStreamError::none)) == "none");
    CHECK(std::string(sotc::to_string(SaveStreamError::duplicate_name)) == "duplicate_name");
    CHECK(std::string(sotc::to_string(SaveStreamError::payload_too_large)) == "payload_too_large");
    CHECK(std::string(sotc::to_string(SaveStreamError::body_too_large)) == "body_too_large");
    CHECK(std::string(sotc::to_string(SaveStreamError::truncated_header)) == "truncated_header");
    CHECK(std::string(sotc::to_string(SaveStreamError::bad_version)) == "bad_version");
    CHECK(std::string(sotc::to_string(SaveStreamError::size_too_large)) == "size_too_large");
    CHECK(std::string(sotc::to_string(SaveStreamError::truncated_body)) == "truncated_body");
    CHECK(std::string(sotc::to_string(SaveStreamError::trailing_bytes)) == "trailing_bytes");
    CHECK(std::string(sotc::to_string(SaveStreamError::truncated_chunk_header)) == "truncated_chunk_header");
    CHECK(std::string(sotc::to_string(SaveStreamError::bad_chunk_magic)) == "bad_chunk_magic");
    CHECK(std::string(sotc::to_string(SaveStreamError::chunk_overrun)) == "chunk_overrun");
    CHECK(std::string(sotc::to_string(SaveStreamError::reader_duplicate_name)) == "reader_duplicate_name");
    CHECK(std::string(sotc::to_string(SaveStreamError::file_not_found)) == "file_not_found");
    CHECK(std::string(sotc::to_string(SaveStreamError::file_too_large)) == "file_too_large");
    CHECK(std::string(sotc::to_string(SaveStreamError::io_open)) == "io_open");
    CHECK(std::string(sotc::to_string(SaveStreamError::io_read)) == "io_read");
    CHECK(std::string(sotc::to_string(SaveStreamError::io_write)) == "io_write");
    CHECK(std::string(sotc::to_string(SaveStreamError::io_fsync)) == "io_fsync");
    CHECK(std::string(sotc::to_string(SaveStreamError::io_rename)) == "io_rename");
    CHECK(std::string(sotc::to_string(static_cast<SaveStreamError>(0xff))) == "unknown");
}

}  // namespace

int main() {
    test_wire_layout();
    test_name_hash_through_writer();
    test_round_trip();
    test_empty_body();
    test_boundary_payloads();
    test_duplicate_policy();
    test_version();
    test_header_corruption();
    test_descriptor_id_not_hashed();
    test_body_corruption();
    test_truncation_every_offset();
    test_trailing_bytes();
    test_oversize();
    test_chunk_level_errors();
    test_hash_checked_before_chunks();
    test_error_names();
    std::puts("save_stream: all checks passed");
    return 0;
}
