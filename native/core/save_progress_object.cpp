#include "core/save_progress_object.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <set>
#include <utility>

#include "core/save_chunk_header.hpp"
#include "core/save_checksum.hpp"

namespace sotc {
namespace {

constexpr std::size_t kNamedCount = 13;

ObjectStatus corrupt(ProgressError error, std::uint32_t name_hash = 0) noexcept {
    ObjectStatus s;
    s.result = SaveResult::failed_corrupt;
    s.error = error;
    s.name_hash = name_hash;
    return s;
}

ObjectStatus payload_failure(std::uint32_t name_hash, CodecError codec) noexcept {
    ObjectStatus s = corrupt(ProgressError::payload_rejected, name_hash);
    s.codec = codec;
    return s;
}

ObjectStatus writer_failure(ProgressError error, std::uint32_t name_hash, CodecError codec,
                            SaveStreamError stream) noexcept {
    ObjectStatus s;
    s.result = SaveResult::failed_generic;
    s.error = error;
    s.name_hash = name_hash;
    s.codec = codec;
    s.stream = stream;
    return s;
}

// Walks the tagged-chunk stream. Copies payloads: bodies are ~20 KB, and owning them keeps the
// resolved index trivially valid.
ObjectStatus walk_records(std::span<const std::byte> body, ChunkWalkPolicy policy,
                          std::vector<SaveStreamChunk>& out) {
    std::size_t pos = 0;
    while (pos < body.size()) {
        const std::size_t left = body.size() - pos;
        if (left < kSaveChunkHeaderSize) {
            if (policy == ChunkWalkPolicy::original) break;   // trailing partial record ignored
            return corrupt(ProgressError::trailing_bytes);
        }
        const ChunkParseResult parsed =
            parse_save_chunk_header(body.subspan(pos, kSaveChunkHeaderSize));
        if (parsed.error == ChunkParseError::bad_magic) {
            // The original stops at the first record without the 0xBEEF magic and accepts what it
            // already read (report C7). Strict tiling treats that as corruption.
            if (policy == ChunkWalkPolicy::original) break;
            return corrupt(ProgressError::bad_chunk_magic);
        }
        if (parsed.error != ChunkParseError::none) {
            // Unreachable with 12 bytes, kept so the enum is exhaustively handled.
            return corrupt(ProgressError::truncated_chunk_header);
        }
        const std::size_t payload_length = parsed.header.payload_length;
        if (payload_length > left - kSaveChunkHeaderSize) {
            return corrupt(ProgressError::chunk_overrun, parsed.header.name_hash);
        }
        SaveStreamChunk chunk;
        chunk.name_hash = parsed.header.name_hash;
        chunk.tag = parsed.header.tag;
        const auto payload = body.subspan(pos + kSaveChunkHeaderSize, payload_length);
        chunk.payload.assign(payload.begin(), payload.end());
        out.push_back(std::move(chunk));
        pos += kSaveChunkHeaderSize + payload_length;
    }
    if (out.empty()) {
        // The original dereferences NULL on an empty body; never reproduced.
        return corrupt(ProgressError::empty_body);
    }
    return {};
}

struct Resolved {
    std::vector<SaveStreamChunk> chunks;
    std::array<std::int32_t, kNamedCount> named{};
    std::array<std::int32_t, kFlagChunkCount> flags{};
    std::array<std::int32_t, kIndexedChunkCount> indexed{};

    Resolved() {
        named.fill(-1);
        flags.fill(-1);
        indexed.fill(-1);
    }

    [[nodiscard]] const SaveStreamChunk* named_chunk(std::size_t i) const noexcept {
        return named[i] < 0 ? nullptr : &chunks[static_cast<std::size_t>(named[i])];
    }
};

// First match wins for every name hash. Strict mode additionally rejects duplicates, unknown
// hashes and missing rows; original mode keeps the destination default for all three.
ObjectStatus resolve(std::span<const std::byte> body, ChunkWalkPolicy policy, Resolved& out) {
    ObjectStatus walked = walk_records(body, policy, out.chunks);
    if (!walked.ok()) return walked;

    const auto specs = progress_named_chunk_specs();
    const auto flag_rows = flag_specs();
    static_assert(kNamedCount == 13);

    std::set<std::uint32_t> known;
    for (const ChunkSpec& spec : specs) known.insert(spec.name_hash);
    for (const FlagSpec& flag : flag_rows) known.insert(flag.name_hash);
    for (std::uint32_t i = 0; i != kIndexedChunkCount; ++i) known.insert(indexed_chunk_hash(i));

    std::set<std::uint32_t> seen;
    for (std::size_t index = 0; index != out.chunks.size(); ++index) {
        const SaveStreamChunk& chunk = out.chunks[index];
        if (!seen.insert(chunk.name_hash).second) {
            if (policy == ChunkWalkPolicy::strict) {
                return corrupt(ProgressError::duplicate_chunk, chunk.name_hash);
            }
            continue;
        }
        if (known.find(chunk.name_hash) == known.end()) {
            if (policy == ChunkWalkPolicy::strict) {
                return corrupt(ProgressError::unknown_chunk, chunk.name_hash);
            }
            continue;
        }
        bool placed = false;
        for (std::size_t i = 0; i != specs.size(); ++i) {
            if (specs[i].name_hash != chunk.name_hash) continue;
            // The original keeps the prior value when the first matching chunk has another tag.
            if (policy == ChunkWalkPolicy::original && chunk.tag != specs[i].tag) break;
            out.named[i] = static_cast<std::int32_t>(index);
            placed = true;
            break;
        }
        if (placed) continue;
        for (std::size_t i = 0; i != flag_rows.size(); ++i) {
            if (flag_rows[i].name_hash != chunk.name_hash) continue;
            if (policy == ChunkWalkPolicy::original && chunk.tag != kTagBytes) break;
            out.flags[i] = static_cast<std::int32_t>(index);
            placed = true;
            break;
        }
        if (placed) continue;
        for (std::uint32_t i = 0; i != kIndexedChunkCount; ++i) {
            if (indexed_chunk_hash(i) != chunk.name_hash) continue;
            if (policy == ChunkWalkPolicy::original && chunk.tag != kTagString) break;
            out.indexed[i] = static_cast<std::int32_t>(index);
            break;
        }
    }

    if (policy == ChunkWalkPolicy::strict) {
        for (std::size_t i = 0; i != specs.size(); ++i) {
            if (out.named[i] < 0) return corrupt(ProgressError::chunk_missing, specs[i].name_hash);
        }
        for (std::size_t i = 0; i != flag_rows.size(); ++i) {
            if (out.flags[i] < 0) {
                return corrupt(ProgressError::chunk_missing, flag_rows[i].name_hash);
            }
        }
        for (std::uint32_t i = 0; i != kIndexedChunkCount; ++i) {
            if (out.indexed[i] < 0) {
                return corrupt(ProgressError::chunk_missing, indexed_chunk_hash(i));
            }
        }
    }
    return {};
}

}  // namespace

const char* to_string(SaveResult result) noexcept {
    switch (result) {
        case SaveResult::success: return "success";
        case SaveResult::canceled: return "canceled";
        case SaveResult::failed_no_space: return "failed_no_space";
        case SaveResult::failed_file_not_found: return "failed_file_not_found";
        case SaveResult::failed_mount: return "failed_mount";
        case SaveResult::failed_corrupt: return "failed_corrupt";
        case SaveResult::failed_generic: return "failed_generic";
    }
    return "unknown";
}

const char* to_string(ProgressError error) noexcept {
    switch (error) {
        case ProgressError::none: return "none";
        case ProgressError::empty_body: return "empty_body";
        case ProgressError::truncated_chunk_header: return "truncated_chunk_header";
        case ProgressError::bad_chunk_magic: return "bad_chunk_magic";
        case ProgressError::chunk_overrun: return "chunk_overrun";
        case ProgressError::duplicate_chunk: return "duplicate_chunk";
        case ProgressError::unknown_chunk: return "unknown_chunk";
        case ProgressError::chunk_missing: return "chunk_missing";
        case ProgressError::trailing_bytes: return "trailing_bytes";
        case ProgressError::game_data_size: return "game_data_size";
        case ProgressError::count_mismatch: return "count_mismatch";
        case ProgressError::flag_conflict: return "flag_conflict";
        case ProgressError::unlock_out_of_range: return "unlock_out_of_range";
        case ProgressError::tag_mismatch: return "tag_mismatch";
        case ProgressError::payload_rejected: return "payload_rejected";
        case ProgressError::stream_rejected: return "stream_rejected";
        case ProgressError::layout_mismatch: return "layout_mismatch";
    }
    return "unknown";
}

const char* to_string(ChunkWalkPolicy policy) noexcept {
    switch (policy) {
        case ChunkWalkPolicy::strict: return "strict";
        case ChunkWalkPolicy::original: return "original";
    }
    return "unknown";
}

ProgressV1 make_progress_v1() {
    ProgressV1 object;
    object.game_data.assign(kProgressGameDataBytes, std::byte{0});
    return object;
}

ObjectStatus progress_check(const ProgressV1& object) noexcept {
    if (object.game_data.size() != kProgressGameDataBytes) {
        return corrupt(ProgressError::game_data_size);
    }
    const auto flag_rows = flag_specs();
    for (std::size_t i = 0; i != flag_rows.size(); ++i) {
        bool bit = false;
        if (!get_flag_bit(object.game_data, flag_rows[i].flag_index, bit)) {
            return corrupt(ProgressError::flag_conflict, flag_rows[i].name_hash);
        }
        if (bit != object.flags[i]) {
            return corrupt(ProgressError::flag_conflict, flag_rows[i].name_hash);
        }
    }
    for (std::uint32_t i = 0; i != kIndexedChunkCount; ++i) {
        if (object.unlock[i] > 2) {
            return corrupt(ProgressError::unlock_out_of_range, indexed_chunk_hash(i));
        }
    }
    if (object.ids.size() > 0x3fffffffu) {
        return corrupt(ProgressError::count_mismatch, progress_named_chunk_specs()[11].name_hash);
    }
    return {};
}

ObjectStatus encode_progress_body(const ProgressV1& object, std::vector<std::byte>& body) {
    if (object.game_data.size() != kProgressGameDataBytes) {
        return writer_failure(ProgressError::game_data_size, 0, CodecError::none,
                              SaveStreamError::none);
    }
    if (object.ids.size() > 0x3fffffffu) {
        return writer_failure(ProgressError::count_mismatch,
                              progress_named_chunk_specs()[11].name_hash, CodecError::none,
                              SaveStreamError::none);
    }

    // The writer's rule (contract C9/F24): `flags` wins and the blob copy is rewritten so an
    // internally inconsistent pair can never reach the wire. The caller's object is untouched.
    std::vector<std::byte> blob = object.game_data;
    const auto flag_rows = flag_specs();
    for (std::size_t i = 0; i != flag_rows.size(); ++i) {
        if (!set_flag_bit(blob, flag_rows[i].flag_index, object.flags[i])) {
            return writer_failure(ProgressError::flag_conflict, flag_rows[i].name_hash,
                                  CodecError::none, SaveStreamError::none);
        }
    }

    SaveStreamWriter writer(kProgressDescriptorId);
    ObjectStatus status;

    const auto append = [&](const ChunkSpec& spec, ChunkValue value) -> bool {
        const CodecStatus appended = append_chunk_by_spec(writer, spec, value);
        if (appended.ok()) return true;
        status = writer_failure(
            appended.error == CodecError::stream_rejected ? ProgressError::stream_rejected
                                                          : ProgressError::payload_rejected,
            spec.name_hash, appended.error, appended.stream_error);
        return false;
    };

    for (std::size_t i = 0; i != flag_rows.size(); ++i) {
        if (!append(flag_chunk_spec(flag_rows[i]), ChunkValue{object.flags[i]})) return status;
    }

    const auto specs = progress_named_chunk_specs();
    if (!append(specs[0], ChunkValue{std::move(blob)})) return status;
    if (!append(specs[1], ChunkValue{object.words[0]})) return status;
    if (!append(specs[2], ChunkValue{object.words[1]})) return status;
    if (!append(specs[3], ChunkValue{object.booleans[0]})) return status;
    if (!append(specs[4], ChunkValue{object.booleans[1]})) return status;
    if (!append(specs[5], ChunkValue{object.booleans[2]})) return status;
    if (!append(specs[6], ChunkValue{object.words[2]})) return status;
    if (!append(specs[7], ChunkValue{object.words[3]})) return status;
    if (!append(specs[8], ChunkValue{object.words[4]})) return status;
    if (!append(specs[9], ChunkValue{object.floats[0]})) return status;
    if (!append(specs[10], ChunkValue{object.floats[1]})) return status;
    if (!append(specs[11], ChunkValue{static_cast<std::uint32_t>(object.ids.size())})) return status;
    if (!append(specs[12], ChunkValue{object.ids})) return status;

    for (std::uint32_t i = 0; i != kIndexedChunkCount; ++i) {
        if (!append(indexed_chunk_spec(i), ChunkValue{static_cast<std::uint32_t>(object.unlock[i])})) {
            return status;
        }
    }

    if (writer.body_size() != progress_body_size(object.ids.size())) {
        return writer_failure(ProgressError::layout_mismatch, 0, CodecError::none,
                              SaveStreamError::none);
    }
    const std::vector<std::byte> image = writer.serialize();
    body.assign(image.begin() + static_cast<std::ptrdiff_t>(kSaveStreamHeaderSize), image.end());
    return {};
}

ObjectStatus decode_progress_body(std::span<const std::byte> body, ProgressV1& out,
                                  ChunkWalkPolicy policy, ReadMode mode) {
    Resolved resolved;
    const ObjectStatus resolved_status = resolve(body, policy, resolved);
    if (!resolved_status.ok()) return resolved_status;

    const auto specs = progress_named_chunk_specs();
    const auto flag_rows = flag_specs();
    const bool strict = policy == ChunkWalkPolicy::strict;
    ProgressV1 value = make_progress_v1();

    // GameData first: the flag defaults below come from the blob (the reader's "keep prior value"
    // analogue), so a missing flag chunk leaves the blob bit exactly as the original does.
    if (const SaveStreamChunk* chunk = resolved.named_chunk(0)) {
        const Decoded<ChunkValue> decoded = decode_chunk_by_spec(*chunk, specs[0], mode, 0);
        if (!decoded.ok()) return payload_failure(chunk->name_hash, decoded.error);
        value.game_data = std::get<std::vector<std::byte>>(decoded.value);
    } else if (strict) {
        return corrupt(ProgressError::chunk_missing, specs[0].name_hash);
    }

    for (std::size_t i = 0; i != flag_rows.size(); ++i) {
        bool bit = false;
        if (!get_flag_bit(value.game_data, flag_rows[i].flag_index, bit)) {
            return corrupt(ProgressError::flag_conflict, flag_rows[i].name_hash);
        }
        value.flags[i] = bit;   // default from the blob
        const SaveStreamChunk* chunk =
            resolved.flags[i] < 0 ? nullptr : &resolved.chunks[static_cast<std::size_t>(resolved.flags[i])];
        if (chunk == nullptr) {
            if (strict) return corrupt(ProgressError::chunk_missing, flag_rows[i].name_hash);
            continue;
        }
        const Decoded<ChunkValue> decoded =
            decode_chunk_by_spec(*chunk, flag_chunk_spec(flag_rows[i]), mode, 0);
        if (!decoded.ok()) return payload_failure(chunk->name_hash, decoded.error);
        value.flags[i] = std::get<bool>(decoded.value);   // chunk wins (C9)
        // C9: the loader stores the chunk value into the blob bit too. With a provided prior blob
        // this is a no-op; with a fresh zero blob it keeps the decoded object self-consistent.
        if (!set_flag_bit(value.game_data, flag_rows[i].flag_index, value.flags[i])) {
            return corrupt(ProgressError::flag_conflict, flag_rows[i].name_hash);
        }
    }

    const auto read_word = [&](std::size_t spec_index, std::uint32_t& dst) -> ObjectStatus {
        const SaveStreamChunk* chunk = resolved.named_chunk(spec_index);
        if (chunk == nullptr) {
            return strict ? corrupt(ProgressError::chunk_missing, specs[spec_index].name_hash)
                          : ObjectStatus{};
        }
        const Decoded<ChunkValue> decoded =
            decode_chunk_by_spec(*chunk, specs[spec_index], mode, 0);
        if (!decoded.ok()) return payload_failure(chunk->name_hash, decoded.error);
        dst = std::get<std::uint32_t>(decoded.value);
        return {};
    };
    const auto read_boolean = [&](std::size_t spec_index, bool& dst) -> ObjectStatus {
        const SaveStreamChunk* chunk = resolved.named_chunk(spec_index);
        if (chunk == nullptr) {
            return strict ? corrupt(ProgressError::chunk_missing, specs[spec_index].name_hash)
                          : ObjectStatus{};
        }
        const Decoded<ChunkValue> decoded =
            decode_chunk_by_spec(*chunk, specs[spec_index], mode, 0);
        if (!decoded.ok()) return payload_failure(chunk->name_hash, decoded.error);
        dst = std::get<bool>(decoded.value);
        return {};
    };
    const auto read_float = [&](std::size_t spec_index, float& dst) -> ObjectStatus {
        const SaveStreamChunk* chunk = resolved.named_chunk(spec_index);
        if (chunk == nullptr) {
            return strict ? corrupt(ProgressError::chunk_missing, specs[spec_index].name_hash)
                          : ObjectStatus{};
        }
        const Decoded<ChunkValue> decoded =
            decode_chunk_by_spec(*chunk, specs[spec_index], mode, 0);
        if (!decoded.ok()) return payload_failure(chunk->name_hash, decoded.error);
        dst = std::get<float>(decoded.value);
        return {};
    };

    ObjectStatus step = read_word(1, value.words[0]);
    if (!step.ok()) return step;
    step = read_word(2, value.words[1]);
    if (!step.ok()) return step;
    step = read_boolean(3, value.booleans[0]);
    if (!step.ok()) return step;
    step = read_boolean(4, value.booleans[1]);
    if (!step.ok()) return step;
    step = read_boolean(5, value.booleans[2]);
    if (!step.ok()) return step;
    step = read_word(6, value.words[2]);
    if (!step.ok()) return step;
    step = read_word(7, value.words[3]);
    if (!step.ok()) return step;
    step = read_word(8, value.words[4]);
    if (!step.ok()) return step;
    step = read_float(9, value.floats[0]);
    if (!step.ok()) return step;
    step = read_float(10, value.floats[1]);
    if (!step.ok()) return step;

    std::uint32_t id_count = 0;
    step = read_word(11, id_count);
    if (!step.ok()) return step;

    if (const SaveStreamChunk* chunk = resolved.named_chunk(12)) {
        const Decoded<ChunkValue> decoded =
            decode_chunk_by_spec(*chunk, specs[12], mode, id_count);
        if (!decoded.ok()) return payload_failure(chunk->name_hash, decoded.error);
        value.ids = std::get<std::vector<std::uint32_t>>(decoded.value);
    } else if (strict) {
        return corrupt(ProgressError::chunk_missing, specs[12].name_hash);
    }

    for (std::uint32_t i = 0; i != kIndexedChunkCount; ++i) {
        const ChunkSpec spec = indexed_chunk_spec(i);
        const std::int32_t index = resolved.indexed[i];
        if (index < 0) {
            if (strict) return corrupt(ProgressError::chunk_missing, spec.name_hash);
            continue;
        }
        const SaveStreamChunk& chunk = resolved.chunks[static_cast<std::size_t>(index)];
        const Decoded<ChunkValue> decoded = decode_chunk_by_spec(chunk, spec, mode, 0);
        if (!decoded.ok()) return payload_failure(chunk.name_hash, decoded.error);
        value.unlock[i] = static_cast<std::uint8_t>(std::get<std::uint32_t>(decoded.value));
    }

    const ObjectStatus checked = progress_check(value);
    if (!checked.ok()) return checked;
    out = std::move(value);
    return {};
}

}  // namespace sotc
