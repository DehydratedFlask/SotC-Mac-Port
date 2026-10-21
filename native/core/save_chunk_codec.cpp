#include "core/save_chunk_codec.hpp"

#include <bit>
#include <cstring>

#include "core/save_chunk_header.hpp"

namespace sotc {
namespace {

constexpr std::uint32_t kFnvPrime = 0x01000193u;

// ---- spec tables (transcribed from agentH sections 4-6; counts are static_asserted) -----------
constexpr std::array<ChunkSpec, 33> kProfileSpecs{{
    {0x31542cc5u, 13, PayloadKind::BlobU64, EnumSet::none, 0x9cu},  // 1
    {0x6a83b721u, 12, PayloadKind::EnumStrDecimal, EnumSet::e1, 0},  // 2
    {0x087a6c73u, 13, PayloadKind::Bool8, EnumSet::none, 0},  // 3
    {0xf7154f95u, 13, PayloadKind::Bool8, EnumSet::none, 0},  // 4
    {0x6ea5bf45u, 13, PayloadKind::Bool8, EnumSet::none, 0},  // 5
    {0xf99a0ae3u, 8, PayloadKind::Word32, EnumSet::none, 0},  // 6
    {0x8b3584f3u, 8, PayloadKind::Word32, EnumSet::none, 0},  // 7
    {0xf068962fu, 8, PayloadKind::Word32, EnumSet::none, 0},  // 8
    {0xcb27377eu, 4, PayloadKind::Word32Tag4Unresolved, EnumSet::none, 0},  // 9
    {0x37c9d370u, 13, PayloadKind::Bool8, EnumSet::none, 0},  // 10
    {0xb0ddb3f5u, 12, PayloadKind::Str, EnumSet::none, 0},  // 11
    {0xec13f330u, 13, PayloadKind::Bool8, EnumSet::none, 0},  // 12
    {0x0c226ce0u, 13, PayloadKind::Bool8, EnumSet::none, 0},  // 13
    {0x16d0eb49u, 12, PayloadKind::EnumStrDecimal, EnumSet::e3, 0},  // 14
    {0xdeda656cu, 12, PayloadKind::EnumStrDecimal, EnumSet::e3, 0},  // 15
    {0x0bfe9eefu, 12, PayloadKind::EnumStrDecimal, EnumSet::e3, 0},  // 16
    {0xad062d8fu, 12, PayloadKind::EnumStrDecimal, EnumSet::e4, 0},  // 17
    {0x60a41121u, 12, PayloadKind::EnumStrDecimal, EnumSet::e3, 0},  // 18
    {0x92eaa017u, 12, PayloadKind::EnumStrDecimal, EnumSet::e5, 0},  // 19
    {0x5b6713deu, 10, PayloadKind::F32, EnumSet::none, 0},  // 20
    {0xce3222c6u, 10, PayloadKind::F32, EnumSet::none, 0},  // 21
    {0x2985559eu, 10, PayloadKind::F32, EnumSet::none, 0},  // 22
    {0x77d20933u, 10, PayloadKind::F32, EnumSet::none, 0},  // 23
    {0x97b66846u, 12, PayloadKind::EnumStrDecimal, EnumSet::e6, 0},  // 24
    {0x1478d8e2u, 13, PayloadKind::Bool8, EnumSet::none, 0},  // 25
    {0x60da6c9eu, 10, PayloadKind::F32, EnumSet::none, 0},  // 26
    {0x63c5981fu, 10, PayloadKind::F32, EnumSet::none, 0},  // 27
    {0x6bfe3e86u, 10, PayloadKind::F32, EnumSet::none, 0},  // 28
    {0x8bfceba4u, 12, PayloadKind::Str, EnumSet::none, 0},  // 29
    {0xe85a6984u, 10, PayloadKind::F32, EnumSet::none, 0},  // 30
    {0xdc913ac6u, 12, PayloadKind::Str, EnumSet::none, 0},  // 31
    {0xb1b4ea0cu, 12, PayloadKind::EnumStrDecimal, EnumSet::none, 0},  // 32
    {0x9a50a3bcu, 12, PayloadKind::EnumStrDecimal, EnumSet::none, 0},  // 33
}};

constexpr std::array<ChunkSpec, 13> kProgressSpecs{{
    {0xf24056efu, 13, PayloadKind::BlobU64, EnumSet::none, 0x3cd8u},
    {0x8c36c89bu, 8, PayloadKind::Word32, EnumSet::none, 0},
    {0x9b650877u, 8, PayloadKind::Word32, EnumSet::none, 0},
    {0x0ccea4ffu, 13, PayloadKind::Bool8, EnumSet::none, 0},
    {0x1d2abf79u, 13, PayloadKind::Bool8, EnumSet::none, 0},
    {0xbf73d5a9u, 13, PayloadKind::Bool8, EnumSet::none, 0},
    {0xe2922fb0u, 8, PayloadKind::Word32, EnumSet::none, 0},
    {0x79108b37u, 8, PayloadKind::Word32, EnumSet::none, 0},
    {0xd57202c7u, 8, PayloadKind::Word32, EnumSet::none, 0},
    {0x0acace09u, 10, PayloadKind::F32, EnumSet::none, 0},
    {0x025bf0e9u, 10, PayloadKind::F32, EnumSet::none, 0},
    {0x2a708ecfu, 4, PayloadKind::Word32Tag4Unresolved, EnumSet::none, 0},  // element count N
    {0xfb61bc89u, 13, PayloadKind::ArrayU32U64, EnumSet::none, 0},          // prefix == 4N
}};

constexpr std::array<FlagSpec, kFlagChunkCount> kFlags{{
    {0x7d, 0x10a37023u},
    {0x7c, 0x927711d9u},
    {0x78, 0xfd9dc6cdu},
    {0x80, 0xba3e8241u},
    {0x79, 0x95f710ebu},
    {0x7e, 0x11a371b6u},
    {0x86, 0xec9dae76u},
    {0x85, 0x4a87cd7au},
    {0x7a, 0xc01c3c76u},
    {0x83, 0xd9c55811u},
    {0x81, 0x85f0e1e4u},
    {0x82, 0x5189c431u},
    {0x7b, 0xd281cf5au},
    {0x84, 0x7cadc4efu},
    {0x7f, 0x12a37349u},
    {0x87, 0xc187e05fu},
    {0xa5, 0xb4de1ce1u},
    {0xa6, 0x9409ff84u},
    {0xa7, 0xf3265c03u},
    {0xa8, 0xaa10c206u},
    {0xa9, 0xc503c3b5u},
    {0xaa, 0xf837dbf8u},
    {0xab, 0xe96c15a7u},
    {0xac, 0xca6e6e9au},
    {0xad, 0x229511f9u},
    {0xae, 0xfa59a5b5u},
    {0xaf, 0xdf66a406u},
    {0xb0, 0x1ec1f7a7u},
    {0xb1, 0x2d8dbdf8u},
    {0xb2, 0xea33fee1u},
    {0xb3, 0x73e4cf02u},
    {0xb4, 0x287c3e03u},
    {0xd9, 0x1a3c4083u},
    {0xda, 0xd13ea686u},
    {0xdb, 0x0140810du},
    {0xdc, 0x633609a8u},
    {0xdd, 0xba217017u},
    {0xde, 0xd7285f9au},
    {0xdf, 0xf4490491u},
    {0xe0, 0x96b5972cu},
    {0xe1, 0x5cfdc54bu},
    {0xe2, 0x6424d235u},
    {0xe3, 0x23dbcdeeu},
    {0xe4, 0x440c13abu},
    {0xe5, 0x12f1808cu},
    {0xe6, 0x2f1ce079u},
    {0xe7, 0xaae7f6c2u},
    {0xe8, 0x2ede573fu},
}};

static_assert(kProfileSpecs.size() == 33);
static_assert(kProgressSpecs.size() == 13);

// Bit set of allowed values per enum set (E1 {1,2,3,4}, E2 {0..3}, E3 {0,1,2}, E4 {0,1},
// E5 {0,1,2}, E6 {0..3}, E7 {0,1,2}).
constexpr std::uint32_t enum_mask(EnumSet set) noexcept {
    switch (set) {
        case EnumSet::e1: return 0x1eu;
        case EnumSet::e2: return 0x0fu;
        case EnumSet::e3: return 0x07u;
        case EnumSet::e4: return 0x03u;
        case EnumSet::e5: return 0x07u;
        case EnumSet::e6: return 0x0fu;
        case EnumSet::e7: return 0x07u;
        case EnumSet::none: return 0xffffffffu;
    }
    return 0;
}

constexpr std::uint32_t fold_decimal(std::uint32_t state, std::uint32_t value) noexcept {
    char digits[10] = {};
    int n = 0;
    do {
        digits[n++] = static_cast<char>('0' + value % 10);
        value /= 10;
    } while (value != 0);
    for (int i = n - 1; i >= 0; --i) {
        state = (state ^ static_cast<std::uint32_t>(static_cast<unsigned char>(digits[i]))) * kFnvPrime;
    }
    return state;
}

static_assert(fold_decimal(kIndexPrefixState, 0) == 0x31ebde09u);
static_assert(fold_decimal(kIndexPrefixState, 1) == 0x30ebdc76u);
static_assert(fold_decimal(kIndexPrefixState, 256) == 0x79b2a7bcu);

std::byte to_byte(std::uint32_t v) noexcept { return static_cast<std::byte>(v & 0xffu); }

std::uint32_t load_u32(const std::byte* p) noexcept {
    std::uint32_t v = 0;
    for (int i = 0; i != 4; ++i) v |= std::to_integer<std::uint32_t>(p[i]) << (8 * i);
    return v;
}

std::uint64_t load_u64(const std::byte* p) noexcept {
    std::uint64_t v = 0;
    for (int i = 0; i != 8; ++i) v |= std::to_integer<std::uint64_t>(p[i]) << (8 * i);
    return v;
}

CodecError finish(const PayloadReader& reader, ReadMode mode) noexcept {
    if (mode == ReadMode::strict && reader.remaining() != 0) return CodecError::trailing_bytes;
    return CodecError::none;
}

CodecError check_tag(const SaveStreamChunk& chunk, std::uint16_t tag) noexcept {
    return chunk.tag == tag ? CodecError::none : CodecError::tag_mismatch;
}

template <class T>
Decoded<T> fail(CodecError error) {
    return Decoded<T>{error, T{}};
}

}  // namespace

const char* to_string(CodecError error) noexcept {
    switch (error) {
        case CodecError::none: return "none";
        case CodecError::short_read: return "short_read";
        case CodecError::trailing_bytes: return "trailing_bytes";
        case CodecError::varint_overlong: return "varint_overlong";
        case CodecError::string_length_exceeds_payload: return "string_length_exceeds_payload";
        case CodecError::blob_length_mismatch: return "blob_length_mismatch";
        case CodecError::blob_length_exceeds_payload: return "blob_length_exceeds_payload";
        case CodecError::array_count_mismatch: return "array_count_mismatch";
        case CodecError::decimal_empty: return "decimal_empty";
        case CodecError::decimal_not_digit: return "decimal_not_digit";
        case CodecError::decimal_overflow: return "decimal_overflow";
        case CodecError::enum_value_not_in_set: return "enum_value_not_in_set";
        case CodecError::payload_too_large: return "payload_too_large";
        case CodecError::tag_mismatch: return "tag_mismatch";
        case CodecError::chunk_missing: return "chunk_missing";
        case CodecError::wrong_value_type: return "wrong_value_type";
        case CodecError::stream_rejected: return "stream_rejected";
        case CodecError::empty_body: return "empty_body";
        case CodecError::descriptor_mismatch: return "descriptor_mismatch";
        case CodecError::container_rejected: return "container_rejected";
    }
    return "unknown";
}

std::uint32_t chunk_name_hash(std::string_view name, NameHashPolicy policy) noexcept {
    if (policy == NameHashPolicy::ascii_upper) return save_chunk_name_hash(name);
    std::uint32_t hash = kFnv1a32OffsetBasis;
    for (const char raw : name) {
        unsigned char byte = static_cast<unsigned char>(raw);
        if (policy == NameHashPolicy::ascii_upper_and_slash) {
            if (byte >= 'a' && byte <= 'z') byte = static_cast<unsigned char>(byte - 'a' + 'A');
            if (byte == '\\') byte = '/';
        }
        hash = (hash ^ byte) * kFnvPrime;
    }
    return hash;
}

// ---- PayloadReader ----------------------------------------------------------------------------
CodecError PayloadReader::read_u8(std::uint8_t& out) noexcept {
    if (remaining() < 1) return CodecError::short_read;
    out = std::to_integer<std::uint8_t>(data_[pos_++]);
    return CodecError::none;
}

CodecError PayloadReader::read_u32(std::uint32_t& out) noexcept {
    if (remaining() < 4) return CodecError::short_read;
    out = load_u32(data_.data() + pos_);
    pos_ += 4;
    return CodecError::none;
}

CodecError PayloadReader::read_u64(std::uint64_t& out) noexcept {
    if (remaining() < 8) return CodecError::short_read;
    out = load_u64(data_.data() + pos_);
    pos_ += 8;
    return CodecError::none;
}

CodecError PayloadReader::read_f32(float& out) noexcept {
    std::uint32_t bits = 0;
    if (const auto e = read_u32(bits); e != CodecError::none) return e;
    out = std::bit_cast<float>(bits);
    return CodecError::none;
}

CodecError PayloadReader::read_varint_u32(std::uint32_t& out) noexcept {
    // 7-bit groups, least significant first, bit 7 = continue. At most 5 bytes; the fifth byte may
    // only carry the top 4 bits of a u32. Non-canonical (zero-padded) forms are accepted.
    std::uint32_t value = 0;
    for (int i = 0; i != 5; ++i) {
        if (remaining() < 1) return CodecError::short_read;
        const unsigned byte = std::to_integer<unsigned>(data_[pos_++]);
        if (i == 4 && (byte & 0xf0u) != 0) return CodecError::varint_overlong;
        value |= static_cast<std::uint32_t>(byte & 0x7fu) << (7 * i);
        if ((byte & 0x80u) == 0) {
            out = value;
            return CodecError::none;
        }
    }
    return CodecError::varint_overlong;
}

CodecError PayloadReader::read_bytes(std::size_t count, std::span<const std::byte>& out) noexcept {
    if (remaining() < count) return CodecError::short_read;
    out = data_.subspan(pos_, count);
    pos_ += count;
    return CodecError::none;
}

// ---- PayloadWriter ----------------------------------------------------------------------------
void PayloadWriter::put_u8(std::uint8_t value) { out_.push_back(static_cast<std::byte>(value)); }

void PayloadWriter::put_u32(std::uint32_t value) {
    for (int i = 0; i != 4; ++i) out_.push_back(to_byte(value >> (8 * i)));
}

void PayloadWriter::put_u64(std::uint64_t value) {
    for (int i = 0; i != 8; ++i) out_.push_back(static_cast<std::byte>((value >> (8 * i)) & 0xffu));
}

void PayloadWriter::put_f32(float value) { put_u32(std::bit_cast<std::uint32_t>(value)); }

void PayloadWriter::put_varint_u32(std::uint32_t value) {
    while (value >= 0x80u) {
        out_.push_back(to_byte((value & 0x7fu) | 0x80u));
        value >>= 7;
    }
    out_.push_back(to_byte(value));
}

void PayloadWriter::put_bytes(std::span<const std::byte> bytes) {
    out_.insert(out_.end(), bytes.begin(), bytes.end());
}

// ---- encoders ---------------------------------------------------------------------------------
std::vector<std::byte> encode_bool_payload(bool value) {
    PayloadWriter w;
    w.put_u8(value ? 1 : 0);
    return w.take();
}

std::vector<std::byte> encode_word32_payload(std::uint32_t bits) {
    PayloadWriter w;
    w.put_u32(bits);
    return w.take();
}

std::vector<std::byte> encode_f32_payload(float value) {
    PayloadWriter w;
    w.put_f32(value);
    return w.take();
}

CodecError encode_string_payload(std::string_view text, std::vector<std::byte>& out) {
    if (text.size() > 0xffffffffu) return CodecError::payload_too_large;
    PayloadWriter w;
    w.put_varint_u32(static_cast<std::uint32_t>(text.size()));
    for (const char c : text) w.put_u8(static_cast<std::uint8_t>(static_cast<unsigned char>(c)));
    out = w.take();
    return CodecError::none;
}

std::vector<std::byte> encode_enum_decimal_payload(std::uint32_t value) {
    const std::string text = std::to_string(value);
    std::vector<std::byte> out;
    (void)encode_string_payload(text, out);   // cannot fail: at most 10 characters
    return out;
}

std::vector<std::byte> encode_blob_payload(std::span<const std::byte> blob) {
    PayloadWriter w;
    w.put_u64(blob.size());
    w.put_bytes(blob);
    return w.take();
}

std::vector<std::byte> encode_u32_array_payload(std::span<const std::uint32_t> values) {
    PayloadWriter w;
    w.put_u64(static_cast<std::uint64_t>(values.size()) * 4u);
    for (const auto v : values) w.put_u32(v);
    return w.take();
}

// ---- decoders ---------------------------------------------------------------------------------
Decoded<bool> decode_bool_chunk(const SaveStreamChunk& chunk, ReadMode mode) noexcept {
    if (const auto e = check_tag(chunk, kTagBytes); e != CodecError::none) return fail<bool>(e);
    PayloadReader r(chunk.payload);
    std::uint8_t b = 0;
    if (const auto e = r.read_u8(b); e != CodecError::none) return fail<bool>(e);
    if (const auto e = finish(r, mode); e != CodecError::none) return fail<bool>(e);
    return {CodecError::none, b != 0};   // original: any nonzero byte is true
}

namespace {
Decoded<std::uint32_t> decode_word32(const SaveStreamChunk& chunk, std::uint16_t tag,
                                     ReadMode mode) noexcept {
    if (const auto e = check_tag(chunk, tag); e != CodecError::none) return fail<std::uint32_t>(e);
    PayloadReader r(chunk.payload);
    std::uint32_t v = 0;
    if (const auto e = r.read_u32(v); e != CodecError::none) return fail<std::uint32_t>(e);
    if (const auto e = finish(r, mode); e != CodecError::none) return fail<std::uint32_t>(e);
    return {CodecError::none, v};
}
}  // namespace

Decoded<std::uint32_t> decode_word32_tag8_chunk(const SaveStreamChunk& chunk, ReadMode mode) noexcept {
    return decode_word32(chunk, kTagWord32Tag8, mode);
}

Decoded<std::uint32_t> decode_word32_tag4_unresolved_chunk(const SaveStreamChunk& chunk,
                                                           ReadMode mode) noexcept {
    return decode_word32(chunk, kTagWord32Tag4, mode);
}

Decoded<float> decode_f32_chunk(const SaveStreamChunk& chunk, ReadMode mode) noexcept {
    if (const auto e = check_tag(chunk, kTagF32); e != CodecError::none) return fail<float>(e);
    PayloadReader r(chunk.payload);
    float v = 0;
    if (const auto e = r.read_f32(v); e != CodecError::none) return fail<float>(e);
    if (const auto e = finish(r, mode); e != CodecError::none) return fail<float>(e);
    return {CodecError::none, v};
}

namespace {
Decoded<std::span<const std::byte>> decode_string_bytes(const SaveStreamChunk& chunk, ReadMode mode) {
    using R = Decoded<std::span<const std::byte>>;
    if (const auto e = check_tag(chunk, kTagString); e != CodecError::none) return R{e, {}};
    PayloadReader r(chunk.payload);
    std::uint32_t len = 0;
    if (const auto e = r.read_varint_u32(len); e != CodecError::none) return R{e, {}};
    if (len > r.remaining()) return R{CodecError::string_length_exceeds_payload, {}};
    std::span<const std::byte> bytes;
    if (const auto e = r.read_bytes(len, bytes); e != CodecError::none) return R{e, {}};
    if (const auto e = finish(r, mode); e != CodecError::none) return R{e, {}};
    return R{CodecError::none, bytes};
}
}  // namespace

Decoded<std::string> decode_string_chunk(const SaveStreamChunk& chunk, ReadMode mode) {
    const auto bytes = decode_string_bytes(chunk, mode);
    if (!bytes.ok()) return fail<std::string>(bytes.error);
    std::string out;
    out.reserve(bytes.value.size());
    for (const auto b : bytes.value) out.push_back(static_cast<char>(std::to_integer<unsigned char>(b)));
    return {CodecError::none, std::move(out)};
}

bool enum_set_contains(EnumSet set, std::uint32_t value) noexcept {
    if (set == EnumSet::none) return true;
    return value < 32 && ((enum_mask(set) >> value) & 1u) != 0;
}

Decoded<std::uint32_t> decode_enum_decimal_chunk(const SaveStreamChunk& chunk, EnumSet set,
                                                 ReadMode mode) {
    const auto text = decode_string_bytes(chunk, mode);
    if (!text.ok()) return fail<std::uint32_t>(text.error);
    if (text.value.empty()) return fail<std::uint32_t>(CodecError::decimal_empty);
    std::uint64_t value = 0;
    for (const auto b : text.value) {
        const unsigned c = std::to_integer<unsigned>(b);
        if (c < '0' || c > '9') return fail<std::uint32_t>(CodecError::decimal_not_digit);
        value = value * 10 + (c - '0');
        if (value > 0xffffffffull) return fail<std::uint32_t>(CodecError::decimal_overflow);
    }
    const auto v = static_cast<std::uint32_t>(value);
    if (!enum_set_contains(set, v)) return fail<std::uint32_t>(CodecError::enum_value_not_in_set);
    return {CodecError::none, v};
}

Decoded<std::span<const std::byte>> decode_blob_chunk(const SaveStreamChunk& chunk,
                                                      RequiredLength required, ReadMode mode) noexcept {
    using R = Decoded<std::span<const std::byte>>;
    if (const auto e = check_tag(chunk, kTagBytes); e != CodecError::none) return R{e, {}};
    PayloadReader r(chunk.payload);
    std::uint64_t len = 0;
    if (const auto e = r.read_u64(len); e != CodecError::none) return R{e, {}};
    if (required.has && len != required.value) return R{CodecError::blob_length_mismatch, {}};
    if (len > r.remaining()) return R{CodecError::blob_length_exceeds_payload, {}};
    std::span<const std::byte> bytes;
    if (const auto e = r.read_bytes(static_cast<std::size_t>(len), bytes); e != CodecError::none) {
        return R{e, {}};
    }
    if (const auto e = finish(r, mode); e != CodecError::none) return R{e, {}};
    return R{CodecError::none, bytes};
}

Decoded<std::vector<std::uint32_t>> decode_u32_array_chunk(const SaveStreamChunk& chunk,
                                                           std::uint32_t element_count, ReadMode mode) {
    using R = Decoded<std::vector<std::uint32_t>>;
    if (const auto e = check_tag(chunk, kTagBytes); e != CodecError::none) return R{e, {}};
    PayloadReader r(chunk.payload);
    std::uint64_t len = 0;
    if (const auto e = r.read_u64(len); e != CodecError::none) return R{e, {}};
    if (len != static_cast<std::uint64_t>(element_count) * 4u) return R{CodecError::array_count_mismatch, {}};
    if (len > r.remaining()) return R{CodecError::blob_length_exceeds_payload, {}};
    std::vector<std::uint32_t> out(element_count);
    for (auto& v : out) {
        if (const auto e = r.read_u32(v); e != CodecError::none) return R{e, {}};
    }
    if (const auto e = finish(r, mode); e != CodecError::none) return R{e, {}};
    return R{CodecError::none, std::move(out)};
}

// ---- lookup -----------------------------------------------------------------------------------
ChunkLookup lookup_chunk(const ParsedSaveStream& stream, std::uint32_t name_hash,
                         std::uint16_t expected_tag) noexcept {
    const SaveStreamChunk* chunk = stream.find_first(name_hash);
    if (chunk == nullptr) return {LookupStatus::missing, nullptr};
    if (chunk->tag != expected_tag) return {LookupStatus::tag_mismatch, chunk};
    return {LookupStatus::found, chunk};
}

// ---- spec tables ------------------------------------------------------------------------------
std::span<const ChunkSpec> profile_named_chunk_specs() noexcept { return kProfileSpecs; }
std::span<const ChunkSpec> progress_named_chunk_specs() noexcept { return kProgressSpecs; }
std::span<const FlagSpec> flag_specs() noexcept { return kFlags; }

std::uint32_t indexed_chunk_hash(std::uint32_t index) noexcept {
    return fold_decimal(kIndexPrefixState, index);
}

ChunkSpec indexed_chunk_spec(std::uint32_t index) noexcept {
    return {indexed_chunk_hash(index), kTagString, PayloadKind::EnumStrDecimal, EnumSet::e7, 0};
}

ChunkSpec flag_chunk_spec(const FlagSpec& flag) noexcept {
    return {flag.name_hash, kTagBytes, PayloadKind::Bool8, EnumSet::none, 0};
}

bool get_flag_bit(std::span<const std::byte> blob, std::uint32_t flag_index, bool& out) noexcept {
    const std::size_t byte_index = flag_index >> 3;
    if (byte_index >= blob.size()) return false;
    out = ((std::to_integer<unsigned>(blob[byte_index]) >> (flag_index & 7u)) & 1u) != 0;
    return true;
}

bool set_flag_bit(std::span<std::byte> blob, std::uint32_t flag_index, bool value) noexcept {
    const std::size_t byte_index = flag_index >> 3;
    if (byte_index >= blob.size()) return false;
    const unsigned mask = 1u << (flag_index & 7u);
    unsigned b = std::to_integer<unsigned>(blob[byte_index]);
    b = value ? (b | mask) : (b & ~mask);
    blob[byte_index] = static_cast<std::byte>(b);
    return true;
}

// ---- spec driven values -----------------------------------------------------------------------
CodecStatus append_chunk_by_spec(SaveStreamWriter& writer, const ChunkSpec& spec, const ChunkValue& value) {
    std::vector<std::byte> payload;
    switch (spec.kind) {
        case PayloadKind::Bool8: {
            const bool* v = std::get_if<bool>(&value);
            if (v == nullptr) return {CodecError::wrong_value_type, SaveStreamError::none};
            payload = encode_bool_payload(*v);
            break;
        }
        case PayloadKind::Word32:
        case PayloadKind::Word32Tag4Unresolved: {
            const auto* v = std::get_if<std::uint32_t>(&value);
            if (v == nullptr) return {CodecError::wrong_value_type, SaveStreamError::none};
            payload = encode_word32_payload(*v);
            break;
        }
        case PayloadKind::F32: {
            const auto* v = std::get_if<float>(&value);
            if (v == nullptr) return {CodecError::wrong_value_type, SaveStreamError::none};
            payload = encode_f32_payload(*v);
            break;
        }
        case PayloadKind::Str: {
            const auto* v = std::get_if<std::string>(&value);
            if (v == nullptr) return {CodecError::wrong_value_type, SaveStreamError::none};
            if (const auto e = encode_string_payload(*v, payload); e != CodecError::none) {
                return {e, SaveStreamError::none};
            }
            break;
        }
        case PayloadKind::EnumStrDecimal: {
            const auto* v = std::get_if<std::uint32_t>(&value);
            if (v == nullptr) return {CodecError::wrong_value_type, SaveStreamError::none};
            if (!enum_set_contains(spec.enum_set, *v)) {
                return {CodecError::enum_value_not_in_set, SaveStreamError::none};
            }
            payload = encode_enum_decimal_payload(*v);
            break;
        }
        case PayloadKind::BlobU64: {
            const auto* v = std::get_if<std::vector<std::byte>>(&value);
            if (v == nullptr) return {CodecError::wrong_value_type, SaveStreamError::none};
            if (spec.fixed_length != 0 && v->size() != spec.fixed_length) {
                return {CodecError::blob_length_mismatch, SaveStreamError::none};
            }
            payload = encode_blob_payload(*v);
            break;
        }
        case PayloadKind::ArrayU32U64: {
            const auto* v = std::get_if<std::vector<std::uint32_t>>(&value);
            if (v == nullptr) return {CodecError::wrong_value_type, SaveStreamError::none};
            payload = encode_u32_array_payload(*v);
            break;
        }
    }
    const SaveStreamError err = writer.append_chunk_hashed(spec.name_hash, spec.tag, payload);
    if (err != SaveStreamError::none) return {CodecError::stream_rejected, err};
    return {};
}

Decoded<ChunkValue> decode_chunk_by_spec(const SaveStreamChunk& chunk, const ChunkSpec& spec,
                                         ReadMode mode, std::uint32_t array_count) {
    if (chunk.tag != spec.tag) return fail<ChunkValue>(CodecError::tag_mismatch);
    const auto wrap = [](auto&& d) -> Decoded<ChunkValue> {
        if (!d.ok()) return Decoded<ChunkValue>{d.error, ChunkValue{}};
        return Decoded<ChunkValue>{CodecError::none, ChunkValue{std::move(d.value)}};
    };
    switch (spec.kind) {
        case PayloadKind::Bool8: return wrap(decode_bool_chunk(chunk, mode));
        case PayloadKind::Word32: return wrap(decode_word32_tag8_chunk(chunk, mode));
        case PayloadKind::Word32Tag4Unresolved: return wrap(decode_word32_tag4_unresolved_chunk(chunk, mode));
        case PayloadKind::F32: return wrap(decode_f32_chunk(chunk, mode));
        case PayloadKind::Str: return wrap(decode_string_chunk(chunk, mode));
        case PayloadKind::EnumStrDecimal: return wrap(decode_enum_decimal_chunk(chunk, spec.enum_set, mode));
        case PayloadKind::BlobU64: {
            RequiredLength req;
            if (spec.fixed_length != 0) req = {true, spec.fixed_length};
            const auto d = decode_blob_chunk(chunk, req, mode);
            if (!d.ok()) return fail<ChunkValue>(d.error);
            return {CodecError::none, ChunkValue{std::vector<std::byte>(d.value.begin(), d.value.end())}};
        }
        case PayloadKind::ArrayU32U64: return wrap(decode_u32_array_chunk(chunk, array_count, mode));
    }
    return fail<ChunkValue>(CodecError::wrong_value_type);
}

Decoded<ChunkValue> read_chunk_by_spec(const ParsedSaveStream& stream, const ChunkSpec& spec,
                                       ReadMode mode, std::uint32_t array_count) {
    const ChunkLookup found = lookup_chunk(stream, spec.name_hash, spec.tag);
    if (found.status == LookupStatus::missing) return fail<ChunkValue>(CodecError::chunk_missing);
    if (found.status == LookupStatus::tag_mismatch) return fail<ChunkValue>(CodecError::tag_mismatch);
    return decode_chunk_by_spec(*found.chunk, spec, mode, array_count);
}

// ---- loader-level descriptor gate -------------------------------------------------------------
CodecError check_descriptor_id(const SaveStreamHeader& header, std::uint32_t expected) noexcept {
    return header.descriptor_id == expected ? CodecError::none : CodecError::descriptor_mismatch;
}

LoadGateResult validate_for_loader(std::span<const std::byte> bytes, std::uint32_t expected_descriptor_id) {
    LoadGateResult out;
    SaveStreamReadResult parsed = parse_save_stream(bytes);
    if (parsed.error != SaveStreamError::none) {
        out.error = CodecError::container_rejected;
        out.stream_error = parsed.error;
        return out;
    }
    if (const auto e = check_descriptor_id(parsed.stream.header, expected_descriptor_id); e != CodecError::none) {
        out.error = e;
        return out;
    }
    if (parsed.stream.header.body_size == 0 || parsed.stream.chunks.empty()) {
        out.error = CodecError::empty_body;
        return out;
    }
    out.stream = std::move(parsed.stream);
    return out;
}

}  // namespace sotc
