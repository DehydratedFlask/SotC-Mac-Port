// Fixture tests for native/core/save_chunk_codec. Everything here is SYNTHETIC: golden vectors are
// written by hand from the byte layout in docs/checkpoints/2026-10-21-agentH-sav01-profile-chunks.md
// or produced by an independent Python builder (values below). No game byte is involved and no real
// save compatibility is claimed.
//
// Independent oracles: hand-written wire bytes; a reference FNV loop in this file; the whole
// "profile-like" body golden (size 20747, body hash 0xea9615e8, whole-image FNV 0xd29e50bd) was
// computed by a separate Python script that re-implements the layout from the spec text, not from
// this library.
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <set>
#include <string>
#include <variant>
#include <vector>

#include "core/save_checksum.hpp"
#include "core/save_chunk_codec.hpp"
#include "core/save_chunk_header.hpp"
#include "core/save_stream.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

using Bytes = std::vector<std::byte>;

Bytes B(std::initializer_list<unsigned> v) {
    Bytes out;
    for (const unsigned x : v) out.push_back(static_cast<std::byte>(x));
    return out;
}

std::uint32_t ref_fnv(const Bytes& bytes, std::uint32_t h = 0x811c9dc5u) {
    for (const auto b : bytes) h = (h ^ std::to_integer<std::uint32_t>(b)) * 0x01000193u;
    return h;
}

SaveStreamChunk make_chunk(std::uint16_t tag, Bytes payload, std::uint32_t hash = 1) {
    SaveStreamChunk c;
    c.name_hash = hash;
    c.tag = tag;
    c.payload = std::move(payload);
    return c;
}

// Raw container image with a CORRECT outer hash over `body`, so chunk-level problems are reported
// by the chunk parser rather than by the hash check.
Bytes raw_stream(std::uint32_t descriptor, const Bytes& body) {
    Bytes out;
    const auto put32 = [&out](std::uint32_t v) {
        for (int i = 0; i != 4; ++i) out.push_back(static_cast<std::byte>((v >> (8 * i)) & 0xffu));
    };
    put32(1);
    put32(ref_fnv(body));
    put32(static_cast<std::uint32_t>(body.size()));
    put32(descriptor);
    out.insert(out.end(), body.begin(), body.end());
    return out;
}

Bytes chunk_bytes(std::uint32_t hash, std::uint16_t tag, const Bytes& payload, std::uint32_t declared_len) {
    Bytes out;
    const auto h = encode_save_chunk_header({hash, tag, declared_len});
    out.insert(out.end(), h.begin(), h.end());
    out.insert(out.end(), payload.begin(), payload.end());
    return out;
}

// ---- deterministic synthetic profile-like value generator (mirrored by the Python oracle) ------
std::uint32_t enum_value_for(const ChunkSpec& s, std::uint32_t k) {
    static const std::array<std::uint32_t, 4> e1{1, 2, 3, 4};
    static const std::array<std::uint32_t, 4> e6{0, 1, 2, 3};
    switch (s.enum_set) {
        case EnumSet::e1: return e1[k % 4];
        case EnumSet::e3:
        case EnumSet::e5:
        case EnumSet::e7: return k % 3;
        case EnumSet::e4: return k % 2;
        case EnumSet::e6: return e6[k % 4];
        case EnumSet::e2: return k % 4;
        case EnumSet::none: return k * 1000003u;
    }
    return 0;
}

ChunkValue value_for(const ChunkSpec& s, std::uint32_t k) {
    switch (s.kind) {
        case PayloadKind::Bool8: return ChunkValue{k % 3 == 0};
        case PayloadKind::Word32:
        case PayloadKind::Word32Tag4Unresolved: return ChunkValue{(k * 0x01010101u) ^ 0xA5A5A5A5u};
        case PayloadKind::F32: return ChunkValue{static_cast<float>(k) * 0.5f};
        case PayloadKind::Str: {
            std::string text;
            for (std::uint32_t i = 0; i != (k * 7) % 200; ++i) text.push_back(static_cast<char>('a' + i % 26));
            return ChunkValue{std::move(text)};
        }
        case PayloadKind::EnumStrDecimal: return ChunkValue{enum_value_for(s, k)};
        case PayloadKind::BlobU64: {
            Bytes blob(static_cast<std::size_t>(s.fixed_length));
            for (std::size_t i = 0; i != blob.size(); ++i) blob[i] = static_cast<std::byte>((i * 7 + k) & 0xffu);
            return ChunkValue{std::move(blob)};
        }
        case PayloadKind::ArrayU32U64: return ChunkValue{std::vector<std::uint32_t>{k, k + 1}};
    }
    return {};
}

// ---- independent decoder helpers --------------------------------------------------------------
void check_all_truncations_fail(const SaveStreamChunk& good, auto decode) {
    CHECK(decode(good).ok());
    for (std::size_t n = 0; n != good.payload.size(); ++n) {
        SaveStreamChunk cut = good;
        cut.payload.resize(n);
        CHECK(!decode(cut).ok());   // every strict prefix must be rejected, never read out of bounds
    }
    SaveStreamChunk longer = good;
    longer.payload.push_back(std::byte{0});
    CHECK(decode(longer).error == CodecError::trailing_bytes);
}

void test_primitives_golden() {
    // varint boundaries
    struct V { std::uint32_t value; Bytes wire; };
    const std::vector<V> varints = {
        {0, B({0x00})},
        {1, B({0x01})},
        {127, B({0x7f})},
        {128, B({0x80, 0x01})},
        {300, B({0xac, 0x02})},
        {16383, B({0xff, 0x7f})},
        {16384, B({0x80, 0x80, 0x01})},
        {0x1fffff, B({0xff, 0xff, 0x7f})},
        {0x200000, B({0x80, 0x80, 0x80, 0x01})},
        {0x0fffffff, B({0xff, 0xff, 0xff, 0x7f})},
        {0x10000000, B({0x80, 0x80, 0x80, 0x80, 0x01})},
        {0xffffffffu, B({0xff, 0xff, 0xff, 0xff, 0x0f})},
    };
    for (const auto& v : varints) {
        PayloadWriter w;
        w.put_varint_u32(v.value);
        CHECK(w.bytes() == v.wire);
        PayloadReader r(v.wire);
        std::uint32_t got = 0;
        CHECK(r.read_varint_u32(got) == CodecError::none);
        CHECK(got == v.value && r.remaining() == 0);
    }
    // varint failures
    {
        std::uint32_t got = 0;
        const Bytes six = B({0x80, 0x80, 0x80, 0x80, 0x80, 0x01});
        PayloadReader r6(six);
        CHECK(r6.read_varint_u32(got) == CodecError::varint_overlong);
        const Bytes overflow = B({0xff, 0xff, 0xff, 0xff, 0x1f});   // 5th byte carries bit 32+
        PayloadReader ro(overflow);
        CHECK(ro.read_varint_u32(got) == CodecError::varint_overlong);
        const Bytes cont = B({0x80});                                 // continue bit, no next byte
        PayloadReader rc(cont);
        CHECK(rc.read_varint_u32(got) == CodecError::short_read);
        PayloadReader re(std::span<const std::byte>{});
        CHECK(re.read_varint_u32(got) == CodecError::short_read);
        // non-canonical zero padding is accepted (fixture policy, original behaviour unknown)
        const Bytes padded = B({0x80, 0x00});
        PayloadReader rp(padded);
        CHECK(rp.read_varint_u32(got) == CodecError::none && got == 0);
    }
    // little-endian fixed-width values
    {
        PayloadWriter w;
        w.put_u32(0x11223344u);
        w.put_u64(0x0102030405060708ull);
        w.put_f32(1.0f);
        w.put_f32(-2.5f);
        const Bytes expect = B({0x44, 0x33, 0x22, 0x11, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,
                                0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x20, 0xc0});
        CHECK(w.bytes() == expect);
        PayloadReader r(expect);
        std::uint32_t a = 0; std::uint64_t b = 0; float c = 0, d = 0;
        CHECK(r.read_u32(a) == CodecError::none && a == 0x11223344u);
        CHECK(r.read_u64(b) == CodecError::none && b == 0x0102030405060708ull);
        CHECK(r.read_f32(c) == CodecError::none && c == 1.0f);
        CHECK(r.read_f32(d) == CodecError::none && d == -2.5f);
        CHECK(r.remaining() == 0);
        std::uint8_t u = 0;
        CHECK(r.read_u8(u) == CodecError::short_read);
        std::uint32_t z = 0; std::uint64_t y = 0;
        CHECK(r.read_u32(z) == CodecError::short_read && r.read_u64(y) == CodecError::short_read);
        std::span<const std::byte> view;
        CHECK(r.read_bytes(1, view) == CodecError::short_read);
        CHECK(r.read_bytes(0, view) == CodecError::none && view.empty());
    }
    // NaN payload bits survive the f32 path
    {
        const std::uint32_t nan_bits = 0x7fc12345u;
        const auto payload = encode_f32_payload(std::bit_cast<float>(nan_bits));
        CHECK(payload == B({0x45, 0x23, 0xc1, 0x7f}));
        const auto d = decode_f32_chunk(make_chunk(kTagF32, payload));
        CHECK(d.ok() && std::bit_cast<std::uint32_t>(d.value) == nan_bits);
        const auto neg_zero = decode_f32_chunk(make_chunk(kTagF32, B({0, 0, 0, 0x80})));
        CHECK(neg_zero.ok() && std::bit_cast<std::uint32_t>(neg_zero.value) == 0x80000000u);
    }
}

void test_typed_payload_golden_and_truncation() {
    // bool: one byte, no prefix; any nonzero byte reads as true
    CHECK(encode_bool_payload(true) == B({1}));
    CHECK(encode_bool_payload(false) == B({0}));
    CHECK(decode_bool_chunk(make_chunk(13, B({0}))).value == false);
    CHECK(decode_bool_chunk(make_chunk(13, B({1}))).value == true);
    CHECK(decode_bool_chunk(make_chunk(13, B({0x80}))).value == true);
    check_all_truncations_fail(make_chunk(13, B({1})), [](const SaveStreamChunk& c) { return decode_bool_chunk(c); });
    CHECK(decode_bool_chunk(make_chunk(13, B({1, 0})), ReadMode::ignore_trailing).ok());
    CHECK(decode_bool_chunk(make_chunk(13, B({}))).error == CodecError::short_read);

    // word32 (tag 8) / tag 4 unresolved
    CHECK(encode_word32_payload(0x11223344u) == B({0x44, 0x33, 0x22, 0x11}));
    check_all_truncations_fail(make_chunk(8, B({1, 2, 3, 4})),
                               [](const SaveStreamChunk& c) { return decode_word32_tag8_chunk(c); });
    check_all_truncations_fail(make_chunk(4, B({1, 2, 3, 4})),
                               [](const SaveStreamChunk& c) { return decode_word32_tag4_unresolved_chunk(c); });
    CHECK(decode_word32_tag8_chunk(make_chunk(8, B({0xff, 0xff, 0xff, 0xff}))).value == 0xffffffffu);

    // f32
    check_all_truncations_fail(make_chunk(10, B({0, 0, 0x80, 0x3f})),
                               [](const SaveStreamChunk& c) { return decode_f32_chunk(c); });

    // string
    Bytes sp;
    CHECK(encode_string_payload("abc", sp) == CodecError::none && sp == B({3, 'a', 'b', 'c'}));
    CHECK(encode_string_payload("", sp) == CodecError::none && sp == B({0}));
    const auto empty = decode_string_chunk(make_chunk(12, B({0})));
    CHECK(empty.ok() && empty.value.empty());
    check_all_truncations_fail(make_chunk(12, B({3, 'a', 'b', 'c'})),
                               [](const SaveStreamChunk& c) { return decode_string_chunk(c); });
    CHECK(decode_string_chunk(make_chunk(12, B({4, 'a', 'b', 'c'}))).error == CodecError::string_length_exceeds_payload);
    CHECK(decode_string_chunk(make_chunk(12, B({0xff, 0xff, 0xff, 0xff, 0x0f}))).error
          == CodecError::string_length_exceeds_payload);
    CHECK(decode_string_chunk(make_chunk(12, B({3, 'a', 'b', 'c', 'd'})), ReadMode::ignore_trailing).value == "abc");
    // embedded NUL and high bytes are plain bytes (not NUL-terminated format)
    const auto weird = decode_string_chunk(make_chunk(12, B({3, 'a', 0, 0xff})));
    CHECK(weird.ok() && weird.value.size() == 3 && weird.value[1] == '\0');
    // a 200 byte string needs a two byte varint (128+)
    {
        const std::string text(200, 'x');
        CHECK(encode_string_payload(text, sp) == CodecError::none);
        CHECK(sp.size() == 202 && sp[0] == std::byte{0xc8} && sp[1] == std::byte{0x01});
        CHECK(decode_string_chunk(make_chunk(12, sp)).value == text);
    }

    // enum decimal
    CHECK(encode_enum_decimal_payload(2) == B({1, '2'}));
    CHECK(encode_enum_decimal_payload(4294967295u) == B({10, '4', '2', '9', '4', '9', '6', '7', '2', '9', '5'}));
    CHECK(decode_enum_decimal_chunk(make_chunk(12, B({1, '2'})), EnumSet::e1).value == 2);
    CHECK(decode_enum_decimal_chunk(make_chunk(12, B({3, '0', '0', '7'})), EnumSet::none).value == 7);   // leading zeros ok
    CHECK(decode_enum_decimal_chunk(make_chunk(12, B({0})), EnumSet::none).error == CodecError::decimal_empty);
    CHECK(decode_enum_decimal_chunk(make_chunk(12, B({2, '1', 'a'})), EnumSet::none).error == CodecError::decimal_not_digit);
    CHECK(decode_enum_decimal_chunk(make_chunk(12, B({2, '-', '1'})), EnumSet::none).error == CodecError::decimal_not_digit);
    CHECK(decode_enum_decimal_chunk(make_chunk(12, B({2, ' ', '1'})), EnumSet::none).error == CodecError::decimal_not_digit);
    // decimal-only: any name-like text is refused, not mapped to a default
    CHECK(decode_enum_decimal_chunk(make_chunk(12, B({3, 'a', 'b', 'c'})), EnumSet::e1).error == CodecError::decimal_not_digit);
    CHECK(decode_enum_decimal_chunk(make_chunk(12, B({10, '4', '2', '9', '4', '9', '6', '7', '2', '9', '6'})),
                                    EnumSet::none).error == CodecError::decimal_overflow);
    CHECK(decode_enum_decimal_chunk(make_chunk(12, B({10, '4', '2', '9', '4', '9', '6', '7', '2', '9', '5'})),
                                    EnumSet::none).value == 4294967295u);
    check_all_truncations_fail(make_chunk(12, B({1, '2'})),
                               [](const SaveStreamChunk& c) { return decode_enum_decimal_chunk(c, EnumSet::e1); });

    // enum value sets (values only): E1 {1..4}, E2/E6 {0..3}, E3/E5/E7 {0,1,2}, E4 {0,1}
    const struct { EnumSet set; std::uint32_t lo, hi; } sets[] = {
        {EnumSet::e1, 1, 4}, {EnumSet::e2, 0, 3}, {EnumSet::e3, 0, 2}, {EnumSet::e4, 0, 1},
        {EnumSet::e5, 0, 2}, {EnumSet::e6, 0, 3}, {EnumSet::e7, 0, 2},
    };
    for (const auto& s : sets) {
        for (std::uint32_t v = 0; v != 40; ++v) {
            CHECK(enum_set_contains(s.set, v) == (v >= s.lo && v <= s.hi));
        }
        CHECK(!enum_set_contains(s.set, 0xffffffffu));
        CHECK(!enum_set_contains(s.set, 32));
    }
    CHECK(enum_set_contains(EnumSet::none, 0) && enum_set_contains(EnumSet::none, 0xffffffffu));
    CHECK(decode_enum_decimal_chunk(make_chunk(12, B({1, '0'})), EnumSet::e1).error == CodecError::enum_value_not_in_set);
    CHECK(decode_enum_decimal_chunk(make_chunk(12, B({1, '3'})), EnumSet::e4).error == CodecError::enum_value_not_in_set);

    // u64-prefixed blob: prefix is INSIDE payload_length (payload = 8 + size)
    {
        const Bytes blob = B({0xaa, 0xbb});
        const Bytes p = encode_blob_payload(blob);
        CHECK(p == B({2, 0, 0, 0, 0, 0, 0, 0, 0xaa, 0xbb}));
        CHECK(p.size() == 8 + blob.size());
        const SaveStreamChunk blob_chunk = make_chunk(13, p);   // the returned span aliases this chunk
        const auto d = decode_blob_chunk(blob_chunk);
        CHECK(d.ok() && d.value.size() == 2 && d.value[0] == std::byte{0xaa} && d.value[1] == std::byte{0xbb});
        check_all_truncations_fail(make_chunk(13, p), [](const SaveStreamChunk& c) { return decode_blob_chunk(c); });
        CHECK(decode_blob_chunk(make_chunk(13, B({}))).error == CodecError::short_read);
        // prefix larger than what is there
        CHECK(decode_blob_chunk(make_chunk(13, B({3, 0, 0, 0, 0, 0, 0, 0, 1, 2}))).error
              == CodecError::blob_length_exceeds_payload);
        // absurd 64-bit prefix is bounded, never wrapped to size_t arithmetic
        CHECK(decode_blob_chunk(make_chunk(13, B({0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}))).error
              == CodecError::blob_length_exceeds_payload);
        // required length (profile param blob must be exactly 0x9c, game data 0x3cd8)
        CHECK(decode_blob_chunk(make_chunk(13, p), RequiredLength{true, 2}).ok());
        CHECK(decode_blob_chunk(make_chunk(13, p), RequiredLength{true, 3}).error == CodecError::blob_length_mismatch);
        // required-length mismatch is reported before the size check
        CHECK(decode_blob_chunk(make_chunk(13, B({9, 0, 0, 0, 0, 0, 0, 0})), RequiredLength{true, 2}).error
              == CodecError::blob_length_mismatch);
        // zero length blob
        const SaveStreamChunk empty_blob = make_chunk(13, encode_blob_payload({}));
        const auto z = decode_blob_chunk(empty_blob);
        CHECK(z.ok() && z.value.empty());
        // a bool chunk is a tag 13 chunk too: a 1 byte payload is not a blob (short read of prefix)
        CHECK(decode_blob_chunk(make_chunk(13, B({1}))).error == CodecError::short_read);
    }

    // u32 array: prefix == 4N, elements raw LE
    {
        const std::vector<std::uint32_t> v{1, 0x01020304u};
        const Bytes p = encode_u32_array_payload(v);
        CHECK(p == B({8, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 4, 3, 2, 1}));
        const auto d = decode_u32_array_chunk(make_chunk(13, p), 2);
        CHECK(d.ok() && d.value == v);
        CHECK(decode_u32_array_chunk(make_chunk(13, p), 1).error == CodecError::array_count_mismatch);
        CHECK(decode_u32_array_chunk(make_chunk(13, p), 3).error == CodecError::array_count_mismatch);
        CHECK(decode_u32_array_chunk(make_chunk(13, p), 0xffffffffu).error == CodecError::array_count_mismatch);
        check_all_truncations_fail(make_chunk(13, p), [](const SaveStreamChunk& c) { return decode_u32_array_chunk(c, 2); });
        const auto zero = decode_u32_array_chunk(make_chunk(13, encode_u32_array_payload({})), 0);
        CHECK(zero.ok() && zero.value.empty());
        // prefix says 8 but only 4 bytes follow
        CHECK(decode_u32_array_chunk(make_chunk(13, B({8, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0})), 2).error
              == CodecError::blob_length_exceeds_payload);
    }

    // tag mismatch for every decoder
    CHECK(decode_bool_chunk(make_chunk(12, B({1}))).error == CodecError::tag_mismatch);
    CHECK(decode_f32_chunk(make_chunk(8, B({0, 0, 0, 0}))).error == CodecError::tag_mismatch);
    CHECK(decode_string_chunk(make_chunk(13, B({0}))).error == CodecError::tag_mismatch);
    CHECK(decode_enum_decimal_chunk(make_chunk(13, B({1, '1'})), EnumSet::none).error == CodecError::tag_mismatch);
    CHECK(decode_blob_chunk(make_chunk(12, encode_blob_payload({}))).error == CodecError::tag_mismatch);
    CHECK(decode_u32_array_chunk(make_chunk(12, encode_u32_array_payload({})), 0).error == CodecError::tag_mismatch);
}

void test_tag4_vs_tag8_stays_unresolved() {
    // Identical 4 byte payload under tag 8 and tag 4: separate kinds, no cross reading.
    const Bytes four = B({0x78, 0x56, 0x34, 0x12});
    CHECK(decode_word32_tag8_chunk(make_chunk(8, four)).value == 0x12345678u);
    CHECK(decode_word32_tag4_unresolved_chunk(make_chunk(4, four)).value == 0x12345678u);
    CHECK(decode_word32_tag8_chunk(make_chunk(4, four)).error == CodecError::tag_mismatch);
    CHECK(decode_word32_tag4_unresolved_chunk(make_chunk(8, four)).error == CodecError::tag_mismatch);
    CHECK(kind_is_unresolved(PayloadKind::Word32Tag4Unresolved));
    CHECK(!kind_is_unresolved(PayloadKind::Word32));
    // Exactly one unresolved row in each table (profile #9 0xcb27377e, progress element count 0x2a708ecf).
    int profile_unresolved = 0;
    for (const auto& s : profile_named_chunk_specs()) {
        if (kind_is_unresolved(s.kind)) {
            ++profile_unresolved;
            CHECK(s.name_hash == 0xcb27377eu && s.tag == kTagWord32Tag4);
        }
    }
    CHECK(profile_unresolved == 1);
    int progress_unresolved = 0;
    for (const auto& s : progress_named_chunk_specs()) {
        if (kind_is_unresolved(s.kind)) {
            ++progress_unresolved;
            CHECK(s.name_hash == 0x2a708ecfu && s.tag == kTagWord32Tag4);
        }
    }
    CHECK(progress_unresolved == 1);
    // A stream carrying the unresolved row under tag 8 is rejected by the spec reader, not "fixed".
    const ChunkSpec* row9 = nullptr;
    for (const auto& s : profile_named_chunk_specs()) if (s.name_hash == 0xcb27377eu) row9 = &s;
    CHECK(row9 != nullptr);
    SaveStreamWriter w(kProfileDescriptorId);
    CHECK(w.append_chunk_hashed(0xcb27377eu, 8, four) == SaveStreamError::none);
    const auto parsed = parse_save_stream(w.serialize());
    CHECK(parsed.error == SaveStreamError::none);
    CHECK(read_chunk_by_spec(parsed.stream, *row9).error == CodecError::tag_mismatch);
}

void test_name_hash_policy() {
    // Policy, not claim: ascii_upper == save_chunk_name_hash; verbatim does not fold case.
    CHECK(chunk_name_hash("abc") == save_chunk_name_hash("abc"));
    CHECK(chunk_name_hash("abc") == chunk_name_hash("ABC"));
    CHECK(chunk_name_hash("abc") == chunk_name_hash("aBc"));
    CHECK(chunk_name_hash("abc", NameHashPolicy::verbatim) != chunk_name_hash("ABC", NameHashPolicy::verbatim));
    CHECK(chunk_name_hash("ABC", NameHashPolicy::verbatim) == chunk_name_hash("ABC"));
    // independent loop on uppercase text
    CHECK(chunk_name_hash("ABC") == ref_fnv(B({'A', 'B', 'C'})));
    CHECK(chunk_name_hash("abc", NameHashPolicy::verbatim) == ref_fnv(B({'a', 'b', 'c'})));
    CHECK(chunk_name_hash("") == 0x811c9dc5u);
    // backslash substitution is verified in the original hash loop but only in the slash policy
    CHECK(chunk_name_hash("a\\b", NameHashPolicy::ascii_upper_and_slash) == ref_fnv(B({'A', '/', 'B'})));
    CHECK(chunk_name_hash("a/b", NameHashPolicy::ascii_upper_and_slash) == chunk_name_hash("a\\b", NameHashPolicy::ascii_upper_and_slash));
    CHECK(chunk_name_hash("a\\b") != chunk_name_hash("a/b"));   // default policy keeps the backslash
    CHECK(chunk_name_hash("abc", NameHashPolicy::ascii_upper_and_slash) == chunk_name_hash("abc"));
    // non-ASCII bytes are hashed unchanged under every policy
    CHECK(chunk_name_hash("\xe9", NameHashPolicy::ascii_upper) == ref_fnv(B({0xe9})));
    CHECK(chunk_name_hash("\xe9", NameHashPolicy::ascii_upper_and_slash) == ref_fnv(B({0xe9})));
    // the incremental property used for the indexed block: fold(hash(prefix), digits) == hash(prefix+digits)
    const std::uint32_t prefix_state = chunk_name_hash("ABCDEFGH");
    for (std::uint32_t i : {0u, 7u, 10u, 99u, 100u, 256u}) {
        std::uint32_t h = prefix_state;
        for (const char c : std::to_string(i)) h = (h ^ static_cast<unsigned char>(c)) * 0x01000193u;
        CHECK(h == chunk_name_hash("abcdefgh" + std::to_string(i)));
    }
}

void test_spec_tables() {
    const auto profile = profile_named_chunk_specs();
    const auto progress = progress_named_chunk_specs();
    const auto flags = flag_specs();
    CHECK(profile.size() == 33);
    CHECK(progress.size() == 13);
    CHECK(flags.size() == kFlagChunkCount && flags.size() == 48);
    CHECK(kIndexedChunkCount == 257);

    // golden anchors from the spec text: first and last rows
    CHECK(profile.front().name_hash == 0x31542cc5u && profile.front().tag == 13
          && profile.front().kind == PayloadKind::BlobU64 && profile.front().fixed_length == 0x9c);
    CHECK(profile.back().name_hash == 0x9a50a3bcu && profile.back().tag == 12
          && profile.back().kind == PayloadKind::EnumStrDecimal);
    CHECK(profile[8].name_hash == 0xcb27377eu && profile[8].tag == 4);
    CHECK(progress.front().name_hash == 0xf24056efu && progress.front().fixed_length == 0x3cd8);
    CHECK(progress.back().name_hash == 0xfb61bc89u && progress.back().kind == PayloadKind::ArrayU32U64);
    CHECK(flags.front().flag_index == 0x7d && flags.front().name_hash == 0x10a37023u);
    CHECK(flags.back().flag_index == 0xe8 && flags.back().name_hash == 0x2ede573fu);

    // per-kind counts from the spec's "Counts" line: 7 bool, 4 four-byte ints (3 x tag 8 + 1 x tag 4),
    // 8 floats, 1 blob, 13 strings/enum strings
    int bools = 0, w32 = 0, w32u = 0, f32s = 0, blobs = 0, strs = 0;
    for (const auto& s : profile) {
        switch (s.kind) {
            case PayloadKind::Bool8: ++bools; CHECK(s.tag == 13); break;
            case PayloadKind::Word32: ++w32; CHECK(s.tag == 8); break;
            case PayloadKind::Word32Tag4Unresolved: ++w32u; CHECK(s.tag == 4); break;
            case PayloadKind::F32: ++f32s; CHECK(s.tag == 10); break;
            case PayloadKind::BlobU64: ++blobs; CHECK(s.tag == 13); break;
            case PayloadKind::Str:
            case PayloadKind::EnumStrDecimal: ++strs; CHECK(s.tag == 12); break;
            case PayloadKind::ArrayU32U64: CHECK(false); break;
        }
    }
    CHECK(bools == 7 && w32 == 3 && w32u == 1 && f32s == 8 && blobs == 1 && strs == 13);
    // enum rows 2,14-19,24 carry value sets; two registry rows carry none; row 11 and 29/31 are plain strings
    CHECK(profile[1].enum_set == EnumSet::e1);
    CHECK(profile[13].enum_set == EnumSet::e3 && profile[16].enum_set == EnumSet::e4 && profile[18].enum_set == EnumSet::e5);
    CHECK(profile[23].enum_set == EnumSet::e6);
    CHECK(profile[10].kind == PayloadKind::Str && profile[28].kind == PayloadKind::Str && profile[30].kind == PayloadKind::Str);
    CHECK(profile[31].kind == PayloadKind::EnumStrDecimal && profile[31].enum_set == EnumSet::none);

    // all hashes pairwise distinct across profile + progress + flags + indexed block
    std::set<std::uint32_t> seen;
    std::size_t total = 0;
    for (const auto& s : profile) { seen.insert(s.name_hash); ++total; }
    for (const auto& s : progress) { seen.insert(s.name_hash); ++total; }
    for (const auto& f : flags) { seen.insert(f.name_hash); ++total; }
    for (std::uint32_t i = 0; i != kIndexedChunkCount; ++i) { seen.insert(indexed_chunk_hash(i)); ++total; }
    CHECK(total == 33 + 13 + 48 + 257);
    CHECK(seen.size() == total);

    // flag index runs and flag chunk spec
    for (const auto& f : flags) {
        const bool ok = (f.flag_index >= 0x78 && f.flag_index <= 0x87) || (f.flag_index >= 0xa5 && f.flag_index <= 0xb4)
                        || (f.flag_index >= 0xd9 && f.flag_index <= 0xe8);
        CHECK(ok);
        const ChunkSpec s = flag_chunk_spec(f);
        CHECK(s.name_hash == f.name_hash && s.tag == 13 && s.kind == PayloadKind::Bool8);
    }
    std::set<std::uint32_t> idx;
    for (const auto& f : flags) idx.insert(f.flag_index);
    CHECK(idx.size() == 48);

    // indexed hashes: spec anchors and an independent fold
    CHECK(indexed_chunk_hash(0) == 0x31ebde09u);
    CHECK(indexed_chunk_hash(1) == 0x30ebdc76u);
    CHECK(indexed_chunk_hash(256) == 0x79b2a7bcu);
    std::uint32_t digest = 0x811c9dc5u;
    for (std::uint32_t i = 0; i != kIndexedChunkCount; ++i) {
        std::uint32_t h = kIndexPrefixState;
        for (const char c : std::to_string(i)) h = (h ^ static_cast<unsigned char>(c)) * 0x01000193u;
        CHECK(h == indexed_chunk_hash(i));
        for (int k = 0; k != 4; ++k) digest = (digest ^ ((h >> (8 * k)) & 0xffu)) * 0x01000193u;
    }
    CHECK(digest == 0x11ef85a7u);   // FNV-1a-32 over the 257 hashes as LE u32 (Python-computed)
    const ChunkSpec ix = indexed_chunk_spec(5);
    CHECK(ix.tag == 12 && ix.kind == PayloadKind::EnumStrDecimal && ix.enum_set == EnumSet::e7 && ix.name_hash == indexed_chunk_hash(5));
}

void test_flag_bits() {
    Bytes blob(0x40, std::byte{0});
    bool v = true;
    CHECK(get_flag_bit(blob, 0x7d, v) && !v);
    CHECK(set_flag_bit(blob, 0x7d, true));
    // flag 0x7d -> byte 0xf, bit 5
    CHECK(blob[0xf] == std::byte{0x20});
    CHECK(get_flag_bit(blob, 0x7d, v) && v);
    CHECK(set_flag_bit(blob, 0x78, true) && blob[0xf] == std::byte{0x21});   // 0x78 -> byte 15 bit 0
    CHECK(set_flag_bit(blob, 0x7d, false) && blob[0xf] == std::byte{0x01});
    CHECK(set_flag_bit(blob, 0xe8, true) && blob[29] == std::byte{0x01});    // 0xe8 -> byte 29 bit 0
    // out of range is refused, never wrapped or clamped
    CHECK(!set_flag_bit(blob, 0x200, true));
    CHECK(!get_flag_bit(blob, 0x200, v));
    CHECK(!get_flag_bit(std::span<const std::byte>{}, 0, v));
    CHECK(get_flag_bit(blob, 0x1ff, v));   // byte 63, last valid
    CHECK(!get_flag_bit(blob, 0x200, v));
}

void test_lookup_and_duplicates() {
    SaveStreamWriter w(kProfileDescriptorId);
    CHECK(w.append_chunk_hashed(0x1111u, 13, B({1})) == SaveStreamError::none);
    CHECK(w.append_chunk_hashed(0x2222u, 13, B({0})) == SaveStreamError::none);
    CHECK(w.append_chunk_hashed(0x1111u, 13, B({0})) == SaveStreamError::none);   // duplicate (default policy: allowed)
    CHECK(w.append_chunk_hashed(0x3333u, 10, B({0, 0, 0x80, 0x3f})) == SaveStreamError::none);
    CHECK(w.append_chunk_hashed(0x3333u, 13, B({0})) == SaveStreamError::none);   // later dup with another tag
    const auto parsed = parse_save_stream(w.serialize());
    CHECK(parsed.error == SaveStreamError::none && parsed.stream.chunks.size() == 5);
    // first match wins
    const auto a = lookup_chunk(parsed.stream, 0x1111u, 13);
    CHECK(a.status == LookupStatus::found && a.chunk == &parsed.stream.chunks[0]);
    CHECK(decode_bool_chunk(*a.chunk).value == true);
    CHECK(lookup_chunk(parsed.stream, 0x9999u, 13).status == LookupStatus::missing);
    CHECK(lookup_chunk(parsed.stream, 0x9999u, 13).chunk == nullptr);
    // tag mismatch on the first match is NOT retried against a later duplicate (original behaviour)
    const auto t = lookup_chunk(parsed.stream, 0x3333u, 13);
    CHECK(t.status == LookupStatus::tag_mismatch && t.chunk == &parsed.stream.chunks[3]);
    CHECK(lookup_chunk(parsed.stream, 0x3333u, 10).status == LookupStatus::found);
    // spec reader maps these to errors the caller can treat as "keep default"
    const ChunkSpec bool_spec{0x1111u, 13, PayloadKind::Bool8, EnumSet::none, 0};
    CHECK(read_chunk_by_spec(parsed.stream, bool_spec).value == ChunkValue{true});
    const ChunkSpec missing{0x9999u, 13, PayloadKind::Bool8, EnumSet::none, 0};
    CHECK(read_chunk_by_spec(parsed.stream, missing).error == CodecError::chunk_missing);
    const ChunkSpec retag{0x3333u, 13, PayloadKind::Bool8, EnumSet::none, 0};
    CHECK(read_chunk_by_spec(parsed.stream, retag).error == CodecError::tag_mismatch);
    // order is preserved on re-serialisation of a duplicate-bearing stream
    CHECK(w.serialize() == w.serialize());

    // writer-side reject policy surfaces as stream_rejected + the container's error
    SaveStreamWriter strict(kProfileDescriptorId, DuplicatePolicy::reject);
    const ChunkSpec s{0x1111u, 13, PayloadKind::Bool8, EnumSet::none, 0};
    CHECK(append_chunk_by_spec(strict, s, ChunkValue{true}).ok());
    const CodecStatus dup = append_chunk_by_spec(strict, s, ChunkValue{false});
    CHECK(dup.error == CodecError::stream_rejected && dup.stream_error == SaveStreamError::duplicate_name);
    CHECK(strict.chunk_count() == 1);
}

void test_append_by_spec_errors_leave_writer_unchanged() {
    SaveStreamWriter w(kProfileDescriptorId);
    const ChunkSpec b{0xaaaa0001u, 13, PayloadKind::Bool8, EnumSet::none, 0};
    const ChunkSpec word{0xaaaa0002u, 8, PayloadKind::Word32, EnumSet::none, 0};
    const ChunkSpec f{0xaaaa0003u, 10, PayloadKind::F32, EnumSet::none, 0};
    const ChunkSpec str{0xaaaa0004u, 12, PayloadKind::Str, EnumSet::none, 0};
    const ChunkSpec en{0xaaaa0005u, 12, PayloadKind::EnumStrDecimal, EnumSet::e3, 0};
    const ChunkSpec blob{0xaaaa0006u, 13, PayloadKind::BlobU64, EnumSet::none, 4};
    const ChunkSpec arr{0xaaaa0007u, 13, PayloadKind::ArrayU32U64, EnumSet::none, 0};
    // wrong alternative for every kind
    CHECK(append_chunk_by_spec(w, b, ChunkValue{1.0f}).error == CodecError::wrong_value_type);
    CHECK(append_chunk_by_spec(w, word, ChunkValue{true}).error == CodecError::wrong_value_type);
    CHECK(append_chunk_by_spec(w, f, ChunkValue{std::uint32_t{1}}).error == CodecError::wrong_value_type);
    CHECK(append_chunk_by_spec(w, str, ChunkValue{std::uint32_t{1}}).error == CodecError::wrong_value_type);
    CHECK(append_chunk_by_spec(w, en, ChunkValue{std::string("1")}).error == CodecError::wrong_value_type);
    CHECK(append_chunk_by_spec(w, blob, ChunkValue{std::string("abcd")}).error == CodecError::wrong_value_type);
    CHECK(append_chunk_by_spec(w, arr, ChunkValue{Bytes{}}).error == CodecError::wrong_value_type);
    CHECK(append_chunk_by_spec(w, b, ChunkValue{}).error == CodecError::wrong_value_type);   // monostate
    // value-level errors
    CHECK(append_chunk_by_spec(w, en, ChunkValue{std::uint32_t{3}}).error == CodecError::enum_value_not_in_set);
    CHECK(append_chunk_by_spec(w, blob, ChunkValue{Bytes(3)}).error == CodecError::blob_length_mismatch);
    CHECK(append_chunk_by_spec(w, blob, ChunkValue{Bytes(5)}).error == CodecError::blob_length_mismatch);
    CHECK(w.chunk_count() == 0 && w.body_size() == 0);   // nothing was appended by any failure
    // and the matching good values succeed
    CHECK(append_chunk_by_spec(w, en, ChunkValue{std::uint32_t{2}}).ok());
    CHECK(append_chunk_by_spec(w, blob, ChunkValue{Bytes(4)}).ok());
    CHECK(w.chunk_count() == 2);
    // writer size limit surfaces as stream_rejected / body_too_large
    SaveStreamWriter tiny(kProfileDescriptorId, DuplicatePolicy::allow, 20);
    CHECK(append_chunk_by_spec(tiny, b, ChunkValue{true}).ok());   // 13 bytes
    const auto over = append_chunk_by_spec(tiny, word, ChunkValue{std::uint32_t{1}});   // +16 > 20
    CHECK(over.error == CodecError::stream_rejected && over.stream_error == SaveStreamError::body_too_large);
}

// Wire layout of whole chunks inside the container, byte for byte.
void test_container_golden_bytes() {
    SaveStreamWriter w(kProgressDescriptorId);
    CHECK(append_chunk_by_spec(w, ChunkSpec{0x0ccea4ffu, 13, PayloadKind::Bool8, EnumSet::none, 0}, ChunkValue{true}).ok());
    CHECK(append_chunk_by_spec(w, ChunkSpec{0x8c36c89bu, 8, PayloadKind::Word32, EnumSet::none, 0},
                               ChunkValue{std::uint32_t{0x11223344u}}).ok());
    const Bytes img = w.serialize();
    const Bytes body = B({
        0xff, 0xa4, 0xce, 0x0c, 0xef, 0xbe, 0x0d, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,           // bool chunk, 13 bytes
        0x9b, 0xc8, 0x36, 0x8c, 0xef, 0xbe, 0x08, 0x00, 0x04, 0x00, 0x00, 0x00, 0x44, 0x33, 0x22, 0x11,   // u32 chunk
    });
    CHECK(body.size() == 29);
    CHECK(img.size() == 16 + body.size());
    CHECK(Bytes(img.begin() + 16, img.end()) == body);
    // outer header: version 1, body hash, body size, descriptor id 4 (little-endian)
    CHECK(Bytes(img.begin(), img.begin() + 4) == B({1, 0, 0, 0}));
    CHECK(Bytes(img.begin() + 8, img.begin() + 16) == B({29, 0, 0, 0, 4, 0, 0, 0}));
    const std::uint32_t hash = ref_fnv(body);
    CHECK(Bytes(img.begin() + 4, img.begin() + 8)
          == B({hash & 0xffu, (hash >> 8) & 0xffu, (hash >> 16) & 0xffu, hash >> 24}));

    // blob record: payload_length = 8 + blob size, so the 156 byte param blob is 0xa4 long
    SaveStreamWriter wb(kProfileDescriptorId);
    CHECK(append_chunk_by_spec(wb, profile_named_chunk_specs()[0], value_for(profile_named_chunk_specs()[0], 0)).ok());
    CHECK(wb.body_size() == 12 + 8 + 0x9c);
    const Bytes ib = wb.serialize();
    CHECK(Bytes(ib.begin() + 16, ib.begin() + 16 + 12 + 8)
          == B({0xc5, 0x2c, 0x54, 0x31, 0xef, 0xbe, 0x0d, 0x00, 0xa4, 0x00, 0x00, 0x00,
                0x9c, 0, 0, 0, 0, 0, 0, 0}));
    // the progress game-data record is 12 + 8 + 0x3cd8 = 15596 bytes (earlier checkpoint figure)
    SaveStreamWriter wg(kProgressDescriptorId);
    CHECK(append_chunk_by_spec(wg, progress_named_chunk_specs()[0], value_for(progress_named_chunk_specs()[0], 0)).ok());
    CHECK(wg.body_size() == 15596);
    // a spec-written string / enum / float chunk, wire bytes by hand
    SaveStreamWriter wm(kProfileDescriptorId);
    CHECK(append_chunk_by_spec(wm, ChunkSpec{0x01020304u, 12, PayloadKind::Str, EnumSet::none, 0},
                               ChunkValue{std::string("hi")}).ok());
    CHECK(append_chunk_by_spec(wm, ChunkSpec{0x05060708u, 12, PayloadKind::EnumStrDecimal, EnumSet::e1, 0},
                               ChunkValue{std::uint32_t{4}}).ok());
    CHECK(append_chunk_by_spec(wm, ChunkSpec{0x0a0b0c0du, 10, PayloadKind::F32, EnumSet::none, 0},
                               ChunkValue{1.0f}).ok());
    const Bytes im = wm.serialize();
    CHECK(Bytes(im.begin() + 16, im.end()) == B({
        0x04, 0x03, 0x02, 0x01, 0xef, 0xbe, 0x0c, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 'h', 'i',
        0x08, 0x07, 0x06, 0x05, 0xef, 0xbe, 0x0c, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, '4',
        0x0d, 0x0c, 0x0b, 0x0a, 0xef, 0xbe, 0x0a, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f}));
}

struct SyntheticProfile {
    Bytes image;
    std::vector<std::pair<ChunkSpec, ChunkValue>> rows;   // in write order, game-data blob last
    std::uint32_t array_count = 0;
};

SyntheticProfile build_synthetic_profile() {
    SyntheticProfile out;
    SaveStreamWriter w(kProfileDescriptorId);
    const auto add = [&](const ChunkSpec& s, ChunkValue v) {
        CHECK(append_chunk_by_spec(w, s, v).ok());
        out.rows.emplace_back(s, std::move(v));
    };
    std::uint32_t k = 0;
    for (const auto& s : profile_named_chunk_specs()) add(s, value_for(s, k++));
    for (std::uint32_t i = 0; i != kIndexedChunkCount; ++i) {
        add(indexed_chunk_spec(i), ChunkValue{i % 3});
    }
    // 48 bool flag chunks, plus a game-data-shaped blob whose bit i mirrors flag i
    Bytes game_data(0x3cd8);
    for (std::size_t i = 0; i != game_data.size(); ++i) game_data[i] = static_cast<std::byte>((i * 13 + 5) & 0xffu);
    std::uint32_t j = 0;
    for (const auto& f : flag_specs()) {
        const bool live = (j++ % 2) == 0;
        CHECK(set_flag_bit(game_data, f.flag_index, live));
        add(flag_chunk_spec(f), ChunkValue{live});
    }
    add(progress_named_chunk_specs()[0], ChunkValue{game_data});
    out.image = w.serialize();
    return out;
}

void test_full_profile_like_round_trip() {
    const SyntheticProfile sp = build_synthetic_profile();
    CHECK(sp.rows.size() == 33 + 257 + 48 + 1);

    // Python-computed goldens for the whole synthetic image (see the file header comment)
    CHECK(sp.image.size() == 16 + 20747);
    CHECK(fnv1a32(std::span<const std::byte>(sp.image).subspan(16), 0x811c9dc5u) == 0xea9615e8u);
    CHECK(ref_fnv(sp.image) == 0xd29e50bdu);
    // independent size formula: 12 bytes per header plus the payload sizes by kind
    std::size_t expect_body = 0;
    for (const auto& [spec, value] : sp.rows) {
        expect_body += 12;
        switch (spec.kind) {
            case PayloadKind::Bool8: expect_body += 1; break;
            case PayloadKind::Word32:
            case PayloadKind::Word32Tag4Unresolved:
            case PayloadKind::F32: expect_body += 4; break;
            case PayloadKind::Str: {
                const auto n = std::get<std::string>(value).size();
                expect_body += (n < 128 ? 1 : 2) + n;
                break;
            }
            case PayloadKind::EnumStrDecimal: expect_body += 1 + std::to_string(std::get<std::uint32_t>(value)).size(); break;
            case PayloadKind::BlobU64: expect_body += 8 + std::get<Bytes>(value).size(); break;
            case PayloadKind::ArrayU32U64: CHECK(false); break;
        }
    }
    CHECK(expect_body == 20747);

    // loader gate with the right descriptor
    const LoadGateResult gate = validate_for_loader(sp.image, kProfileDescriptorId);
    CHECK(gate.error == CodecError::none && gate.stream_error == SaveStreamError::none);
    CHECK(gate.stream.chunks.size() == sp.rows.size());
    CHECK(gate.stream.header.descriptor_id == 0xd);

    // decode every chunk by spec and compare with what was written (order-independent lookup)
    for (const auto& [spec, value] : sp.rows) {
        const auto got = read_chunk_by_spec(gate.stream, spec);
        CHECK(got.ok());
        CHECK(got.value == value);
    }
    // flag bits of the decoded game-data blob match the 48 decoded bool chunks
    const auto game = read_chunk_by_spec(gate.stream, progress_named_chunk_specs()[0]);
    CHECK(game.ok());
    const Bytes& gd = std::get<Bytes>(game.value);
    CHECK(gd.size() == 0x3cd8);
    for (const auto& f : flag_specs()) {
        const auto flag = read_chunk_by_spec(gate.stream, flag_chunk_spec(f));
        bool bit = !std::get<bool>(flag.value);
        CHECK(get_flag_bit(gd, f.flag_index, bit));
        CHECK(bit == std::get<bool>(flag.value));
    }

    // re-encode the decoded values: the image must be byte-identical (order preserved)
    SaveStreamWriter again(kProfileDescriptorId);
    for (const auto& c : gate.stream.chunks) {
        CHECK(again.append_chunk_hashed(c.name_hash, c.tag, c.payload) == SaveStreamError::none);
    }
    CHECK(again.serialize() == sp.image);
    SaveStreamWriter from_decoded(kProfileDescriptorId);
    for (const auto& [spec, value] : sp.rows) {
        const auto got = read_chunk_by_spec(gate.stream, spec);
        CHECK(append_chunk_by_spec(from_decoded, spec, got.value).ok());
    }
    CHECK(from_decoded.serialize() == sp.image);

    // Wrong descriptor: container accepts (dword 3 is unhashed), the loader gate refuses.
    const LoadGateResult wrong = validate_for_loader(sp.image, kProgressDescriptorId);
    CHECK(wrong.error == CodecError::descriptor_mismatch);
    CHECK(parse_save_stream(sp.image).error == SaveStreamError::none);
    CHECK(check_descriptor_id(gate.stream.header, kProfileDescriptorId) == CodecError::none);
    CHECK(check_descriptor_id(gate.stream.header, kProgressDescriptorId) == CodecError::descriptor_mismatch);
    CHECK(check_descriptor_id(gate.stream.header, 0) == CodecError::descriptor_mismatch);
    // flip every bit of dword 3: still parses, gate rejects every one of them
    for (std::size_t byte = 12; byte != 16; ++byte) {
        for (unsigned bit = 0; bit != 8; ++bit) {
            Bytes flipped = sp.image;
            flipped[byte] ^= static_cast<std::byte>(1u << bit);
            CHECK(parse_save_stream(flipped).error == SaveStreamError::none);
            CHECK(validate_for_loader(flipped, kProfileDescriptorId).error == CodecError::descriptor_mismatch);
        }
    }

    // truncation at every offset is rejected before any chunk is exposed
    for (std::size_t n = 0; n < sp.image.size(); n += 1) {
        const LoadGateResult cut = validate_for_loader(std::span<const std::byte>(sp.image.data(), n), kProfileDescriptorId);
        CHECK(cut.error == CodecError::container_rejected);
        CHECK(cut.stream.chunks.empty());
        if (n < 16) CHECK(cut.stream_error == SaveStreamError::truncated_header);
        else CHECK(cut.stream_error == SaveStreamError::truncated_body);
    }
    // one extra trailing byte is rejected (fixture policy, stricter than the original)
    Bytes longer = sp.image;
    longer.push_back(std::byte{0});
    CHECK(validate_for_loader(longer, kProfileDescriptorId).stream_error == SaveStreamError::trailing_bytes);
}

void test_loader_gate_hazards() {
    // Empty body: the original dereferences NULL on the first record; here it is an explicit error.
    // The container itself accepts an empty body (size 0, hash of nothing), so this is loader policy.
    const Bytes empty = raw_stream(kProfileDescriptorId, {});
    CHECK(empty.size() == 16);
    CHECK(parse_save_stream(empty).error == SaveStreamError::none);
    CHECK(validate_for_loader(empty, kProfileDescriptorId).error == CodecError::empty_body);
    CHECK(validate_for_loader(empty, kProgressDescriptorId).error == CodecError::descriptor_mismatch);   // descriptor first
    CHECK(validate_for_loader(empty, kProfileDescriptorId).stream.chunks.empty());
    CHECK(validate_for_loader({}, kProfileDescriptorId).stream_error == SaveStreamError::truncated_header);

    // Overlong record (original: out-of-bounds read) with a CORRECT outer hash: container error.
    const Bytes ok_payload = B({1});
    Bytes body = chunk_bytes(0xaaaa0001u, 13, ok_payload, 1);
    {
        const Bytes b2 = chunk_bytes(0xaaaa0002u, 13, ok_payload, 2);   // declares 2, carries 1
        const LoadGateResult r = validate_for_loader(raw_stream(kProfileDescriptorId, b2), kProfileDescriptorId);
        CHECK(r.error == CodecError::container_rejected && r.stream_error == SaveStreamError::chunk_overrun);
        CHECK(r.stream.chunks.empty());
        // declared length 0xffffffff (the back-patch placeholder of an unfinished chunk)
        const Bytes b3 = chunk_bytes(0xaaaa0002u, 13, ok_payload, 0xffffffffu);
        const LoadGateResult r3 = validate_for_loader(raw_stream(kProfileDescriptorId, b3), kProfileDescriptorId);
        CHECK(r3.error == CodecError::container_rejected && r3.stream_error == SaveStreamError::chunk_overrun);
        // overrun by one only in the SECOND chunk
        Bytes two = body;
        const Bytes second = chunk_bytes(0xaaaa0002u, 13, ok_payload, 2);
        two.insert(two.end(), second.begin(), second.end());
        CHECK(validate_for_loader(raw_stream(kProfileDescriptorId, two), kProfileDescriptorId).stream_error
              == SaveStreamError::chunk_overrun);
        // the same body with a correct length is accepted
        Bytes good = body;
        const Bytes second_ok = chunk_bytes(0xaaaa0002u, 13, ok_payload, 1);
        good.insert(good.end(), second_ok.begin(), second_ok.end());
        const LoadGateResult g = validate_for_loader(raw_stream(kProfileDescriptorId, good), kProfileDescriptorId);
        CHECK(g.error == CodecError::none && g.stream.chunks.size() == 2);
    }
    // Tail shorter than a chunk header, bad magic: strict container rejects (the original tolerated
    // garbage tails; import mode is not implemented)
    {
        Bytes tail = body;
        tail.push_back(std::byte{0});
        tail.push_back(std::byte{0});
        CHECK(validate_for_loader(raw_stream(kProfileDescriptorId, tail), kProfileDescriptorId).stream_error
              == SaveStreamError::truncated_chunk_header);
        Bytes bad_magic = chunk_bytes(0xaaaa0001u, 13, ok_payload, 1);
        bad_magic[4] = std::byte{0xee};
        CHECK(validate_for_loader(raw_stream(kProfileDescriptorId, bad_magic), kProfileDescriptorId).stream_error
              == SaveStreamError::bad_chunk_magic);
    }
    // The same overrun with a WRONG outer hash is reported as bad_hash (hash is checked first)
    {
        Bytes img = raw_stream(kProfileDescriptorId, chunk_bytes(0xaaaa0002u, 13, ok_payload, 2));
        img[4] ^= std::byte{1};
        CHECK(validate_for_loader(img, kProfileDescriptorId).stream_error == SaveStreamError::bad_hash);
    }
    // Hostile-looking payload sizes inside an otherwise valid chunk never read out of bounds.
    for (std::uint8_t first = 0; first != 255; ++first) {
        SaveStreamChunk c = make_chunk(13, B({first, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 1, 2, 3}));
        CHECK(!decode_blob_chunk(c).ok());
        c.tag = 12;
        c.payload = B({first, 0xff, 0xff, 0xff, 0xff, 0x0f, 1});
        (void)decode_string_chunk(c);   // result irrelevant; sanitizers watch bounds
    }
}

void test_decode_by_spec_every_kind() {
    // one spec per kind, written and read through the generic path, then truncated at every length
    const std::vector<ChunkSpec> specs = {
        {0xb001u, 13, PayloadKind::Bool8, EnumSet::none, 0},
        {0xb002u, 8, PayloadKind::Word32, EnumSet::none, 0},
        {0xb003u, 4, PayloadKind::Word32Tag4Unresolved, EnumSet::none, 0},
        {0xb004u, 10, PayloadKind::F32, EnumSet::none, 0},
        {0xb005u, 12, PayloadKind::Str, EnumSet::none, 0},
        {0xb006u, 12, PayloadKind::EnumStrDecimal, EnumSet::e6, 0},
        {0xb007u, 13, PayloadKind::BlobU64, EnumSet::none, 5},
        {0xb008u, 13, PayloadKind::ArrayU32U64, EnumSet::none, 0},
    };
    const std::vector<ChunkValue> values = {
        ChunkValue{true}, ChunkValue{std::uint32_t{0xdeadbeefu}}, ChunkValue{std::uint32_t{7}},
        ChunkValue{-0.25f}, ChunkValue{std::string("hello")}, ChunkValue{std::uint32_t{3}},
        ChunkValue{B({1, 2, 3, 4, 5})}, ChunkValue{std::vector<std::uint32_t>{9, 8, 7}},
    };
    for (std::size_t i = 0; i != specs.size(); ++i) {
        SaveStreamWriter w(kProfileDescriptorId);
        CHECK(append_chunk_by_spec(w, specs[i], values[i]).ok());
        const auto parsed = parse_save_stream(w.serialize());
        CHECK(parsed.error == SaveStreamError::none && parsed.stream.chunks.size() == 1);
        const SaveStreamChunk& c = parsed.stream.chunks[0];
        const std::uint32_t count = 3;
        const auto got = decode_chunk_by_spec(c, specs[i], ReadMode::strict, count);
        CHECK(got.ok() && got.value == values[i]);
        for (std::size_t n = 0; n != c.payload.size(); ++n) {
            SaveStreamChunk cut = c;
            cut.payload.resize(n);
            CHECK(!decode_chunk_by_spec(cut, specs[i], ReadMode::strict, count).ok());
            CHECK(!decode_chunk_by_spec(cut, specs[i], ReadMode::ignore_trailing, count).ok());
        }
        SaveStreamChunk longer = c;
        longer.payload.push_back(std::byte{0});
        CHECK(decode_chunk_by_spec(longer, specs[i], ReadMode::strict, count).error == CodecError::trailing_bytes);
        CHECK(decode_chunk_by_spec(longer, specs[i], ReadMode::ignore_trailing, count).ok());
        SaveStreamChunk retag = c;
        retag.tag = static_cast<std::uint16_t>(c.tag + 1);
        CHECK(decode_chunk_by_spec(retag, specs[i]).error == CodecError::tag_mismatch);
    }
    // the progress count/array pair: array length follows the count chunk
    SaveStreamWriter w(kProgressDescriptorId);
    const auto& prog = progress_named_chunk_specs();
    CHECK(append_chunk_by_spec(w, prog[11], ChunkValue{std::uint32_t{3}}).ok());
    CHECK(append_chunk_by_spec(w, prog[12], ChunkValue{std::vector<std::uint32_t>{1, 2, 3}}).ok());
    const auto parsed = parse_save_stream(w.serialize());
    const auto n = read_chunk_by_spec(parsed.stream, prog[11]);
    CHECK(n.ok());
    const auto arr = read_chunk_by_spec(parsed.stream, prog[12], ReadMode::strict, std::get<std::uint32_t>(n.value));
    CHECK(arr.ok() && std::get<std::vector<std::uint32_t>>(arr.value).size() == 3);
    CHECK(read_chunk_by_spec(parsed.stream, prog[12], ReadMode::strict, 2).error == CodecError::array_count_mismatch);
}

void test_to_string_total() {
    for (int e = static_cast<int>(CodecError::none); e <= static_cast<int>(CodecError::container_rejected); ++e) {
        const char* s = to_string(static_cast<CodecError>(e));
        CHECK(s != nullptr && std::strcmp(s, "unknown") != 0);
    }
}

}  // namespace

int main() {
    test_primitives_golden();
    test_typed_payload_golden_and_truncation();
    test_tag4_vs_tag8_stays_unresolved();
    test_name_hash_policy();
    test_spec_tables();
    test_flag_bits();
    test_lookup_and_duplicates();
    test_append_by_spec_errors_leave_writer_unchanged();
    test_container_golden_bytes();
    test_full_profile_like_round_trip();
    test_loader_gate_hazards();
    test_decode_by_spec_every_kind();
    test_to_string_total();
    std::puts("save_chunk_codec: all checks passed");
    return 0;
}
