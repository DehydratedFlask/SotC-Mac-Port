// Fixture checks for the first SaveData transaction slice: progress object codec, native slot
// store seam, and the synchronous load/store/backup transaction.
//
// Everything here is synthetic. No game save was read or written; no guest code is executed.
// The case ids (F01..F29) refer to local/re/evidence/savedata-contract/report.md §6.2.
#include <algorithm>
#include <bit>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string_view>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include "core/save_chunk_codec.hpp"
#include "core/save_chunk_header.hpp"
#include "core/save_checksum.hpp"
#include "core/save_progress_object.hpp"
#include "core/save_slot_store.hpp"
#include "core/save_transaction.hpp"

#define CHECK(c)                                                                        \
    do {                                                                                \
        if (!(c)) {                                                                     \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);            \
            std::exit(1);                                                               \
        }                                                                               \
    } while (0)

#define CHECK_MSG(c, msg)                                                               \
    do {                                                                                \
        if (!(c)) {                                                                     \
            std::fprintf(stderr, "FAIL %s:%d %s (%s)\n", __FILE__, __LINE__, #c, msg);  \
            std::exit(1);                                                               \
        }                                                                               \
    } while (0)

using Bytes = std::vector<std::byte>;
using sotc::ChunkWalkPolicy;
using sotc::ProgressError;
using sotc::ProgressV1;
using sotc::SaveIoError;
using sotc::SaveMemoryFs;
using sotc::SaveOptions;
using sotc::SaveResult;
using sotc::SaveStep;
using sotc::SaveStore;
using sotc::SaveUi;
using sotc::SaveUiKind;
using sotc::SaveUiRequest;
using sotc::SaveUiResponse;

namespace {

// ---- helpers ---------------------------------------------------------------------------------
Bytes B(std::initializer_list<unsigned> values) {
    Bytes out;
    for (const unsigned v : values) out.push_back(static_cast<std::byte>(v & 0xffu));
    return out;
}

std::uint32_t get32(const Bytes& bytes, std::size_t offset) {
    CHECK(offset + 4 <= bytes.size());
    std::uint32_t v = 0;
    for (int i = 0; i != 4; ++i) {
        v |= std::to_integer<std::uint32_t>(bytes[offset + static_cast<std::size_t>(i)])
             << (8 * i);
    }
    return v;
}

void put32(Bytes& bytes, std::size_t offset, std::uint32_t value) {
    CHECK(offset + 4 <= bytes.size());
    for (int i = 0; i != 4; ++i) {
        bytes[offset + static_cast<std::size_t>(i)] =
            static_cast<std::byte>((value >> (8 * i)) & 0xffu);
    }
}

void put64(Bytes& bytes, std::size_t offset, std::uint64_t value) {
    CHECK(offset + 8 <= bytes.size());
    for (int i = 0; i != 8; ++i) {
        bytes[offset + static_cast<std::size_t>(i)] =
            static_cast<std::byte>((value >> (8 * i)) & 0xffu);
    }
}

void check_status(const sotc::SaveStatus& status, SaveResult result, SaveStep step,
                  std::uint32_t sce, int os_errno, const char* what) {
    if (status.result != result || status.step != step || status.sce_status != sce ||
        (os_errno >= 0 && status.os_errno != os_errno)) {
        std::fprintf(stderr,
                     "FAIL %s: got result=%s step=%s sce=0x%08x errno=%d; "
                     "want result=%s step=%s sce=0x%08x errno=%d\n",
                     what, sotc::to_string(status.result), sotc::to_string(status.step),
                     status.sce_status, status.os_errno, sotc::to_string(result),
                     sotc::to_string(step), sce, os_errno);
        std::exit(1);
    }
}

void check_object_status(const sotc::ObjectStatus& status, SaveResult result,
                         ProgressError error, const char* what) {
    if (status.result != result || status.error != error) {
        std::fprintf(stderr, "FAIL %s: got result=%s error=%s; want result=%s error=%s\n", what,
                     sotc::to_string(status.result), sotc::to_string(status.error),
                     sotc::to_string(result), sotc::to_string(error));
        std::exit(1);
    }
}

Bytes make_record(std::uint32_t hash, std::uint16_t tag, const Bytes& payload) {
    sotc::SaveChunkHeader header;
    header.name_hash = hash;
    header.tag = tag;
    header.payload_length = static_cast<std::uint32_t>(payload.size());
    const auto encoded = sotc::encode_save_chunk_header(header);
    Bytes out(encoded.begin(), encoded.end());
    out.insert(out.end(), payload.begin(), payload.end());
    return out;
}

Bytes image_of(const Bytes& body, std::uint32_t version = 1, std::uint32_t descriptor = 4) {
    Bytes image(16 + body.size());
    put32(image, 0, version);
    put32(image, 4, sotc::fnv1a32(body, sotc::kFnv1a32OffsetBasis));
    put32(image, 8, static_cast<std::uint32_t>(body.size()));
    put32(image, 12, descriptor);
    std::copy(body.begin(), body.end(), image.begin() + 16);
    return image;
}

// F01/F06 golden vector: game_data[i] = i & 0xff, unlock[i] = i % 3, flags derived from the blob
// (the report's "all flags false" contradicts a blob whose bytes 1..5 are nonzero; the encoder's
// flag-wins rule would rewrite the blob, so the golden object uses the consistent derivation).
ProgressV1 golden() {
    ProgressV1 object = sotc::make_progress_v1();
    for (std::size_t i = 0; i != object.game_data.size(); ++i) {
        object.game_data[i] = static_cast<std::byte>(i & 0xffu);
    }
    for (std::size_t i = 0; i != object.unlock.size(); ++i) {
        object.unlock[i] = static_cast<std::uint8_t>(i % 3);
    }
    const auto flags = sotc::flag_specs();
    for (std::size_t i = 0; i != flags.size(); ++i) {
        bool bit = false;
        CHECK(sotc::get_flag_bit(object.game_data, flags[i].flag_index, bit));
        object.flags[i] = bit;
    }
    object.words = {1, 2, 3, 4, 5};
    object.booleans = {true, false, true};
    object.floats = {1.0f, -0.5f};
    object.ids = {0x11223344u, 0x55667788u};
    return object;
}

bool same_object(const ProgressV1& a, const ProgressV1& b) {
    return a.game_data == b.game_data && a.unlock == b.unlock && a.flags == b.flags &&
           a.words == b.words && a.booleans == b.booleans && a.floats == b.floats && a.ids == b.ids;
}

Bytes body_of(const ProgressV1& object) {
    Bytes body;
    const sotc::ObjectStatus encoded = sotc::encode_progress_body(object, body);
    CHECK_MSG(encoded.ok(), sotc::to_string(encoded.error));
    return body;
}

Bytes golden_body() { return body_of(golden()); }

struct MemSlot {
    SaveMemoryFs fs;
    SaveStore store;

    MemSlot() {
        store.root = "/saves";
        store.ops = sotc::save_io_memory(fs);
        CHECK(sotc::save_store_init(store).ok());
    }

    [[nodiscard]] std::string path(std::string_view suffix, std::string_view file) const {
        return sotc::save_store_path(store, suffix, file);
    }
    [[nodiscard]] std::string data_path(std::string_view suffix = "0") const {
        return path(suffix, sotc::kSaveDataName);
    }
    [[nodiscard]] std::string backup_path(std::string_view suffix = "0") const {
        return path(suffix, "USR-DATA.bak");
    }
};

// ---- object layer ----------------------------------------------------------------------------
void test_result_ordinals_and_layout() {
    // F01: the game's own 1-based ESaveResult ordinals.
    CHECK(static_cast<int>(SaveResult::success) == 1);
    CHECK(static_cast<int>(SaveResult::canceled) == 2);
    CHECK(static_cast<int>(SaveResult::failed_no_space) == 3);
    CHECK(static_cast<int>(SaveResult::failed_file_not_found) == 4);
    CHECK(static_cast<int>(SaveResult::failed_mount) == 5);
    CHECK(static_cast<int>(SaveResult::failed_corrupt) == 6);
    CHECK(static_cast<int>(SaveResult::failed_generic) == 7);
    CHECK(std::string(sotc::to_string(SaveResult::failed_corrupt)) == "failed_corrupt");

    CHECK(sotc::kProgressGameDataBytes == 0x3cd8);
    CHECK(sotc::kProgressGameDataBytes == 15576);
    CHECK(sotc::kProgressFlagBlockBytes == 624);
    CHECK(sotc::kProgressGameDataOffset == 624);
    CHECK(sotc::kProgressGameDataChunkBytes == 15596);
    CHECK(sotc::kProgressCountOffset == 16371);
    CHECK(sotc::kProgressArrayOffset == 16387);
    CHECK(sotc::kProgressIndexedOffset == 16407);
    CHECK(sotc::kProgressIndexedChunkBytes == 14);
    CHECK(sotc::progress_body_size(0) == 20005);
    CHECK(sotc::progress_body_size(2) == 20013);
    CHECK(sizeof(ProgressV1::unlock) == 257);
    CHECK(sizeof(ProgressV1::flags) == 48);
    CHECK(sizeof(ProgressV1::words) == 20);
    CHECK(sizeof(ProgressV1::booleans) == 3);
    CHECK(sizeof(ProgressV1::floats) == 8);

    // A default-constructed object is deliberately invalid; make_progress_v1() sizes the blob.
    ProgressV1 empty;
    check_object_status(sotc::progress_check(empty), SaveResult::failed_corrupt,
                        ProgressError::game_data_size, "default object");
    CHECK(sotc::progress_check(sotc::make_progress_v1()).ok());
}

void test_golden_body_F01_F06() {
    const ProgressV1 object = golden();
    const Bytes body = golden_body();

    // Term-by-term breakdown: 48*13 + (12+8+0x3cd8) + 5*16 + 3*13 + 2*16 + 16 + (20+4N)
    // + 257*14 = 20005 + 4N. The contract report's "19748 + 4N" drops the 257 one-byte varint
    // string lengths; see the transaction report's correction table.
    CHECK(body.size() == 20013);
    CHECK(body.size() == sotc::progress_body_size(object.ids.size()));
    // Independently re-derived, byte-for-byte, by
    // local/re/evidence/savedata-transaction/verify_golden.py.
    CHECK(sotc::fnv1a32(body, sotc::kFnv1a32OffsetBasis) == 0x5ce4db99u);

    // Flag chunk #1: flag_specs()[0] hash 0x10a37023, magic BEEF, tag 13, length 1.
    CHECK(Bytes(body.begin(), body.begin() + 13) ==
          B({0x23, 0x70, 0xa3, 0x10, 0xef, 0xbe, 0x0d, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00}));

    // GameData record at 624: hash f24056ef, tag 13, payload length 0x3ce0, u64 prefix 0x3cd8.
    CHECK(Bytes(body.begin() + 624, body.begin() + 644) ==
          B({0xef, 0x56, 0x40, 0xf2, 0xef, 0xbe, 0x0d, 0x00, 0xe0, 0x3c, 0x00, 0x00,
             0xd8, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}));
    // Blob bytes 0..14 are untouched by the flag rewrite (the first flag byte is 15).
    for (std::size_t i = 0; i != 15; ++i) {
        CHECK(body[644 + i] == std::byte(i));
    }

    // Count chunk (tag 4) at 16371: payload 2.
    CHECK(get32(body, 16371) == 0x2a708ecfu);
    CHECK(get32(body, 16371 + 8) == 4);
    CHECK(get32(body, 16371 + 12) == 2);
    // Array chunk at 16387: payload length 8 + 4N, u64 prefix 8, then the two LE u32 ids.
    CHECK(get32(body, 16387) == 0xfb61bc89u);
    CHECK(get32(body, 16387 + 8) == 16);
    CHECK(get32(body, 16387 + 12 + 8) == 0x11223344u);
    CHECK(get32(body, 16387 + 12 + 12) == 0x55667788u);

    // Indexed block starts at 16407 + 4N; row 0 is hash 0x31ebde09, tag 12, payload "0".
    const std::size_t indexed = 16407 + 4 * object.ids.size();
    CHECK(get32(body, indexed) == sotc::indexed_chunk_hash(0));
    CHECK(Bytes(body.begin() + static_cast<std::ptrdiff_t>(indexed),
                body.begin() + static_cast<std::ptrdiff_t>(indexed + 14)) ==
          B({0x09, 0xde, 0xeb, 0x31, 0xef, 0xbe, 0x0c, 0x00, 0x02, 0x00, 0x00, 0x00,
             0x01, 0x30}));

    // Every flag / indexed hash in the body is the table hash (F25).
    std::size_t pos = 0;
    for (std::size_t i = 0; i != 48; ++i) {
        CHECK(get32(body, pos) == sotc::flag_specs()[i].name_hash);
        CHECK(get32(body, pos + 8) == 1);
        pos += 13;
    }
    CHECK(pos == 624);
    pos = indexed;
    for (std::uint32_t i = 0; i != sotc::kIndexedChunkCount; ++i) {
        CHECK(get32(body, pos) == sotc::indexed_chunk_hash(i));
        CHECK(get32(body, pos + 8) == 2);
        pos += 14;
    }
    CHECK(pos == body.size());

    ProgressV1 decoded;
    const sotc::ObjectStatus status = sotc::decode_progress_body(body, decoded);
    CHECK(status.ok());
    CHECK(same_object(decoded, object));
    CHECK(std::bit_cast<std::uint32_t>(decoded.floats[0]) ==
          std::bit_cast<std::uint32_t>(1.0f));
    CHECK(std::bit_cast<std::uint32_t>(decoded.floats[1]) ==
          std::bit_cast<std::uint32_t>(-0.5f));

    // Outer header {1, FNV(body), size, 4}.
    const Bytes image = image_of(body);
    CHECK(get32(image, 0) == 1);
    CHECK(get32(image, 4) == sotc::fnv1a32(body, sotc::kFnv1a32OffsetBasis));
    CHECK(get32(image, 8) == body.size());
    CHECK(get32(image, 12) == 4);
}

void test_flag_conflict_F24() {
    ProgressV1 object = golden();
    object.flags[0] = !object.flags[0];
    const sotc::ObjectStatus checked = sotc::progress_check(object);
    check_object_status(checked, SaveResult::failed_corrupt, ProgressError::flag_conflict,
                        "flag conflict");
    CHECK(checked.name_hash == sotc::flag_specs()[0].name_hash);

    // The writer's rule: flags win, the blob is rewritten, the caller's object is not modified.
    const Bytes original_blob = object.game_data;
    Bytes body;
    CHECK(sotc::encode_progress_body(object, body).ok());
    CHECK(object.game_data == original_blob);
    ProgressV1 decoded;
    CHECK(sotc::decode_progress_body(body, decoded).ok());
    CHECK(decoded.flags == object.flags);
    CHECK(decoded.game_data != original_blob);
    bool bit = !object.flags[0];
    CHECK(sotc::get_flag_bit(decoded.game_data, sotc::flag_specs()[0].flag_index, bit));
    CHECK(bit == object.flags[0]);

    // Writer preconditions are visible failures, not silently clamped values.
    ProgressV1 bad_size = golden();
    bad_size.game_data.pop_back();
    Bytes ignored;
    check_object_status(sotc::encode_progress_body(bad_size, ignored),
                        SaveResult::failed_generic, ProgressError::game_data_size,
                        "short game_data");
    ProgressV1 bad_unlock = golden();
    bad_unlock.unlock[7] = 3;
    check_object_status(sotc::progress_check(bad_unlock), SaveResult::failed_corrupt,
                        ProgressError::unlock_out_of_range, "unlock 3");
    check_object_status(sotc::encode_progress_body(bad_unlock, ignored),
                        SaveResult::failed_generic, ProgressError::payload_rejected,
                        "unlock 3 writer");
}

void test_decode_policies_F17_to_F21() {
    const Bytes body = golden_body();
    ProgressV1 out;

    // Empty body is an error in both policies (the original would dereference NULL).
    check_object_status(sotc::decode_progress_body(std::span<const std::byte>{}, out),
                        SaveResult::failed_corrupt, ProgressError::empty_body, "empty");
    check_object_status(
        sotc::decode_progress_body(std::span<const std::byte>{}, out, ChunkWalkPolicy::original),
        SaveResult::failed_corrupt, ProgressError::empty_body, "empty original");

    // Duplicate chunk (the GameData record appended again): strict rejects, original first-wins.
    Bytes duplicate = body;
    duplicate.insert(duplicate.end(), body.begin() + 624, body.begin() + 16220);
    check_object_status(sotc::decode_progress_body(duplicate, out), SaveResult::failed_corrupt,
                        ProgressError::duplicate_chunk, "duplicate strict");
    CHECK(out.game_data.empty());   // out is unchanged on failure
    ProgressV1 duplicate_original;
    CHECK(sotc::decode_progress_body(duplicate, duplicate_original, ChunkWalkPolicy::original).ok());
    CHECK(same_object(duplicate_original, golden()));

    // Unknown chunk: strict rejects, original ignores.
    Bytes unknown = body;
    const Bytes unknown_record = make_record(0xaabbccddu, 13, B({1}));
    unknown.insert(unknown.end(), unknown_record.begin(), unknown_record.end());
    check_object_status(sotc::decode_progress_body(unknown, out), SaveResult::failed_corrupt,
                        ProgressError::unknown_chunk, "unknown strict");
    ProgressV1 unknown_original;
    CHECK(sotc::decode_progress_body(unknown, unknown_original, ChunkWalkPolicy::original).ok());

    // Trailing garbage: < 12 bytes is trailing_bytes in strict, ignored in original.
    Bytes trailing = body;
    trailing.insert(trailing.end(), {std::byte{0xff}, std::byte{0xff}, std::byte{0xff}});
    check_object_status(sotc::decode_progress_body(trailing, out), SaveResult::failed_corrupt,
                        ProgressError::trailing_bytes, "trailing short");
    ProgressV1 trailing_original;
    CHECK(sotc::decode_progress_body(trailing, trailing_original, ChunkWalkPolicy::original).ok());

    // Trailing garbage with >= 12 bytes and a bad magic byte: strict bad magic, original stops.
    Bytes bad_magic = body;
    for (int i = 0; i != 12; ++i) bad_magic.push_back(std::byte{0xaa});
    check_object_status(sotc::decode_progress_body(bad_magic, out), SaveResult::failed_corrupt,
                        ProgressError::bad_chunk_magic, "trailing bad magic");
    ProgressV1 bad_magic_original;
    CHECK(sotc::decode_progress_body(bad_magic, bad_magic_original, ChunkWalkPolicy::original).ok());

    // Overlong record: payload length past the end of the body is an error in both policies.
    Bytes overrun = body;
    const Bytes overrun_record = make_record(0x12345678u, 13, B({1}));
    overrun.insert(overrun.end(), overrun_record.begin(), overrun_record.end());
    put32(overrun, overrun.size() - 5, 0x0000ffffu);   // header length field of the new record
    check_object_status(sotc::decode_progress_body(overrun, out), SaveResult::failed_corrupt,
                        ProgressError::chunk_overrun, "overrun strict");
    check_object_status(
        sotc::decode_progress_body(overrun, out, ChunkWalkPolicy::original),
        SaveResult::failed_corrupt, ProgressError::chunk_overrun, "overrun original");

    // A missing required chunk is only an error under strict tiling.
    Bytes missing = body;
    missing.erase(missing.begin() + 624, missing.begin() + 16220);   // drop GameData
    check_object_status(sotc::decode_progress_body(missing, out), SaveResult::failed_corrupt,
                        ProgressError::chunk_missing, "missing GameData");
    ProgressV1 missing_original;
    CHECK(sotc::decode_progress_body(missing, missing_original, ChunkWalkPolicy::original).ok());
    CHECK(missing_original.flags == golden().flags);
    CHECK(sotc::progress_check(missing_original).ok());
    // Only the 48 flag bits (bytes 15..29) can differ from the zero default blob.
    for (std::size_t i = 0; i != missing_original.game_data.size(); ++i) {
        if (i >= 15 && i <= 29) continue;
        CHECK(missing_original.game_data[i] == std::byte{0});
    }
}

void test_payload_rule_errors_F22_F23() {
    const Bytes body = golden_body();
    ProgressV1 out;

    // GameData u64 prefix 0x3cd7 (one byte short) -> blob_length_mismatch.
    Bytes short_blob = body;
    put64(short_blob, 624 + 12, 0x3cd7);
    const sotc::ObjectStatus blob = sotc::decode_progress_body(short_blob, out);
    check_object_status(blob, SaveResult::failed_corrupt, ProgressError::payload_rejected,
                        "short blob");
    CHECK(blob.codec == sotc::CodecError::blob_length_mismatch);
    CHECK(blob.name_hash == 0xf24056efu);

    // Count chunk says 3 while the array prefix says 4*2.
    Bytes count_mismatch = body;
    put32(count_mismatch, 16371 + 12, 3);
    const sotc::ObjectStatus count = sotc::decode_progress_body(count_mismatch, out);
    check_object_status(count, SaveResult::failed_corrupt, ProgressError::payload_rejected,
                        "count mismatch");
    CHECK(count.codec == sotc::CodecError::array_count_mismatch);
    CHECK(count.name_hash == 0xfb61bc89u);

    // Indexed value 3 is outside E7 {0,1,2}.
    Bytes bad_unlock = body;
    const std::size_t first_indexed = 16407 + 4 * 2;
    bad_unlock[first_indexed + 13] = std::byte{'3'};
    const sotc::ObjectStatus unlock = sotc::decode_progress_body(bad_unlock, out);
    check_object_status(unlock, SaveResult::failed_corrupt, ProgressError::payload_rejected,
                        "unlock digit");
    CHECK(unlock.codec == sotc::CodecError::enum_value_not_in_set);
    CHECK(unlock.name_hash == sotc::indexed_chunk_hash(0));

    // Flag chunk retagged as tag 8: strict tag_mismatch, original keeps the blob default.
    Bytes retag = body;
    retag[6] = std::byte{0x08};
    retag[7] = std::byte{0x00};
    const sotc::ObjectStatus tag = sotc::decode_progress_body(retag, out);
    check_object_status(tag, SaveResult::failed_corrupt, ProgressError::payload_rejected,
                        "flag retag");
    CHECK(tag.codec == sotc::CodecError::tag_mismatch);
    CHECK(tag.name_hash == sotc::flag_specs()[0].name_hash);
    ProgressV1 retag_original;
    CHECK(sotc::decode_progress_body(retag, retag_original, ChunkWalkPolicy::original).ok());
    CHECK(same_object(retag_original, golden()));

    // Original-policy payload leniency: the same chunk with one trailing payload byte.
    Bytes trailing_payload = body;
    // Rebuild only the flag chunk with a 2-byte payload and a matching length field.
    const std::size_t flag_record_end = 13;
    CHECK(get32(trailing_payload, 8) == 1);
    put32(trailing_payload, 8, 2);
    trailing_payload.insert(trailing_payload.begin() + static_cast<std::ptrdiff_t>(flag_record_end),
                            std::byte{0x00});
    ProgressV1 ignore;
    CHECK(sotc::decode_progress_body(trailing_payload, ignore, ChunkWalkPolicy::strict,
                                     sotc::ReadMode::ignore_trailing)
              .ok());
    check_object_status(sotc::decode_progress_body(trailing_payload, out),
                        SaveResult::failed_corrupt, ProgressError::payload_rejected,
                        "flag trailing payload strict");
}

// ---- store / transaction ---------------------------------------------------------------------
struct UiProbe {
    enum class Mode { cancel, choose, confirm } mode = Mode::cancel;
    std::size_t chosen = 0;
    bool saw_active = false;
    SaveUiKind last_kind = SaveUiKind::choose_slot;
    std::size_t slot_count = 0;
};

bool ui_present(void* ctx, const SaveUiRequest& request, SaveUiResponse& response) {
    auto& probe = *static_cast<UiProbe*>(ctx);
    probe.last_kind = request.kind;
    probe.slot_count = request.slots == nullptr ? 0 : request.slots->size();
    probe.saw_active = sotc::save_ui_active(SaveStore{});
    if (probe.mode == UiProbe::Mode::cancel) {
        response.cancel = true;
        return true;
    }
    if (probe.mode == UiProbe::Mode::choose) response.chosen_slot = probe.chosen;
    return true;
}

void test_memory_round_trip_F01() {
    MemSlot slot;
    const Bytes image = image_of(golden_body());
    check_status(sotc::save_store(slot.store, "0", golden()), SaveResult::success, SaveStep::none,
                 0, -1, "F01 store");

    const auto* stored = slot.fs.find(slot.data_path());
    CHECK(stored != nullptr);
    CHECK(*stored == image);

    ProgressV1 loaded;
    check_status(sotc::save_load(slot.store, "0", loaded), SaveResult::success, SaveStep::none, 0,
                 -1, "F01 load");
    CHECK(same_object(loaded, golden()));

    std::vector<sotc::SaveSlotInfo> slots;
    check_status(sotc::save_enumerate(slot.store, slots), SaveResult::success, SaveStep::none, 0,
                 -1, "F01 enumerate");
    CHECK(slots.size() == 1);
    CHECK(slots[0].dir_name == "SAVEDATA00");
    CHECK(slots[0].game_suffix == "0");
    CHECK(slots[0].committed_bytes == image.size());
    CHECK(slots[0].body_size == 20013);
    CHECK(slots[0].body_hash == sotc::fnv1a32(golden_body(), sotc::kFnv1a32OffsetBasis));
    CHECK(!slots[0].has_backup);

    // A second store captures the first generation; has_backup becomes true.
    ProgressV1 second = golden();
    second.words[0] = 0xdeadbeefu;
    check_status(sotc::save_store(slot.store, "0", second), SaveResult::success, SaveStep::none, 0,
                 -1, "F01 store 2");
    CHECK(slot.fs.has(slot.backup_path()));
    CHECK(*slot.fs.find(slot.backup_path()) == image);
    ProgressV1 reloaded;
    check_status(sotc::save_load(slot.store, "0", reloaded), SaveResult::success, SaveStep::none,
                 0, -1, "F01 load 2");
    CHECK(same_object(reloaded, second));
}

void test_posix_round_trip_F02() {
    const char* tmp = std::getenv("TMPDIR");
    std::string tmpl = std::string(tmp != nullptr && *tmp != '\0' ? tmp : "/tmp") +
                       "/sotc_save_tx_XXXXXX";
    const char* made = ::mkdtemp(tmpl.data());
    CHECK(made != nullptr);

    SaveStore store;
    store.root = std::string(made) + "/saves";
    store.ops = sotc::save_io_posix();
    CHECK(sotc::save_store_init(store).ok());

    const Bytes image_a = image_of(golden_body());
    check_status(sotc::save_store(store, "0", golden()), SaveResult::success, SaveStep::none, 0,
                 -1, "F02 posix store");
    const std::string data_path = sotc::save_store_path(store, "0", sotc::kSaveDataName);
    const std::string backup_path = sotc::save_store_path(store, "0", "USR-DATA.bak");
    struct stat st {};
    CHECK(::stat(data_path.c_str(), &st) == 0 && S_ISREG(st.st_mode));

    ProgressV1 second = golden();
    second.ids.push_back(0x99aabbccu);
    check_status(sotc::save_store(store, "0", second), SaveResult::success, SaveStep::none, 0, -1,
                 "F02 posix store 2");
    CHECK(::stat(backup_path.c_str(), &st) == 0);

    // Read both files back byte-for-byte without using the store.
    const auto slurp = [](const std::string& path) {
        Bytes out;
        FILE* f = std::fopen(path.c_str(), "rb");
        CHECK(f != nullptr);
        std::byte buffer[4096];
        std::size_t n = 0;
        while ((n = std::fread(buffer, 1, sizeof buffer, f)) != 0) {
            out.insert(out.end(), buffer, buffer + n);
        }
        std::fclose(f);
        return out;
    };
    CHECK(slurp(backup_path) == image_a);
    CHECK(slurp(data_path) == image_of(body_of(second)));

    ProgressV1 loaded;
    check_status(sotc::save_load(store, "0", loaded), SaveResult::success, SaveStep::none, 0, -1,
                 "F02 posix load");
    CHECK(same_object(loaded, second));

    bool has_backup = false;
    check_status(sotc::save_check_backup(store, "0", has_backup), SaveResult::success,
                 SaveStep::none, 0, -1, "F02 check backup");
    CHECK(has_backup);

    const std::string cleanup = "rm -rf '" + std::string(made) + "'";
    CHECK(std::system(cleanup.c_str()) == 0);
}

void test_load_corruption_F03_to_F09() {
    const Bytes good_body = golden_body();
    const Bytes good_image = image_of(good_body);

    {
        MemSlot slot;   // F10: USR-DATA absent
        ProgressV1 out;
        check_status(sotc::save_load(slot.store, "0", out), SaveResult::failed_file_not_found,
                     SaveStep::read_file, 0x809f000eu, ENOENT, "F10 missing");
        // The original's fopen failure covers EACCES too (same raw code / ordinal).
        slot.fs.read_error = SaveIoError::denied;
        check_status(sotc::save_load(slot.store, "0", out), SaveResult::failed_file_not_found,
                     SaveStep::read_file, 0x809f000eu, -1, "F10 denied");
    }
    {
        MemSlot slot;
        for (std::size_t n = 0; n != 16; ++n) {   // F05: every short header length
            slot.fs.put(slot.data_path(), std::span<const std::byte>(good_image).first(n));
            ProgressV1 out;
            check_status(sotc::save_load(slot.store, "0", out), SaveResult::failed_generic,
                         SaveStep::read_header, 0x809f000bu, -1, "F05 short header");
        }
        slot.fs.put(slot.data_path(), good_image);
    }
    {
        MemSlot slot;   // F06: version != 1
        slot.fs.put(slot.data_path(), image_of(good_body, 2, 4));
        ProgressV1 out;
        check_status(sotc::save_load(slot.store, "0", out), SaveResult::failed_corrupt,
                     SaveStep::read_header, 0x809f000fu, -1, "F06 version");
    }
    {
        MemSlot slot;   // F07: body_size 0x6400001 with only 16 bytes present
        Bytes oversize(16, std::byte{0});
        put32(oversize, 0, 1);
        put32(oversize, 8, 0x6400001u);
        put32(oversize, 12, 4);
        slot.fs.put(slot.data_path(), oversize);
        ProgressV1 out;
        const unsigned before = slot.fs.read_calls;
        check_status(sotc::save_load(slot.store, "0", out), SaveResult::failed_corrupt,
                     SaveStep::read_header, 0x809f000fu, -1, "F07 oversize");
        CHECK(slot.fs.read_calls == before + 1);   // one read of 16 bytes, no body read
    }
    {
        MemSlot slot;   // F08: body truncated by one byte
        Bytes truncated(good_image.begin(), good_image.end() - 1);
        slot.fs.put(slot.data_path(), truncated);
        ProgressV1 out;
        check_status(sotc::save_load(slot.store, "0", out), SaveResult::failed_corrupt,
                     SaveStep::read_body, 0x809f000fu, -1, "F08 short body");
    }
    {
        MemSlot slot;   // F03: body byte flipped, header hash untouched
        Bytes flipped = good_image;
        flipped.back() ^= std::byte{0x01};
        slot.fs.put(slot.data_path(), flipped);
        ProgressV1 out;
        check_status(sotc::save_load(slot.store, "0", out), SaveResult::failed_corrupt,
                     SaveStep::verify_hash, 0x809f000fu, -1, "F03 body flip");
        CHECK(out.game_data.empty());
    }
    {
        MemSlot slot;   // F04: header hash flipped
        Bytes flipped = good_image;
        flipped[4] ^= std::byte{0x01};
        slot.fs.put(slot.data_path(), flipped);
        ProgressV1 out;
        check_status(sotc::save_load(slot.store, "0", out), SaveResult::failed_corrupt,
                     SaveStep::verify_hash, 0x809f000fu, -1, "F04 header hash");
    }
    {
        MemSlot slot;   // F09: descriptor 0xd in a progress slot
        slot.fs.put(slot.data_path(), image_of(good_body, 1, 0xd));
        ProgressV1 out;
        check_status(sotc::save_load(slot.store, "0", out), SaveResult::failed_corrupt,
                     SaveStep::verify_descriptor, 0x809f000fu, -1, "F09 descriptor");
    }
    {
        MemSlot slot;   // Trailing file bytes after header+body are ignored (original freads size).
        Bytes trailing = good_image;
        trailing.insert(trailing.end(), {std::byte{0xde}, std::byte{0xad}});
        slot.fs.put(slot.data_path(), trailing);
        ProgressV1 out;
        check_status(sotc::save_load(slot.store, "0", out), SaveResult::success, SaveStep::none, 0,
                     -1, "trailing file bytes");
        CHECK(same_object(out, golden()));
    }
}

void test_no_space_F11() {
    MemSlot slot;
    SaveOptions options;
    options.free_space_limit = 100;
    check_status(sotc::save_store(slot.store, "0", golden(), options),
                 SaveResult::failed_no_space, SaveStep::precheck, 0x809f000au, -1, "F11 no space");
    CHECK(!slot.fs.has(slot.data_path()));
    CHECK(slot.fs.write_calls == 0);

    // The backup copy is counted too: after a successful first store, a limit that fits the image
    // but not image + backup must fail before touching USR-DATA.
    MemSlot second;
    CHECK(sotc::save_store(second.store, "0", golden()).ok());
    const Bytes before = *second.fs.find(second.data_path());
    SaveOptions tight;
    tight.free_space_limit = before.size() + 10;
    check_status(sotc::save_store(second.store, "0", golden(), tight),
                 SaveResult::failed_no_space, SaveStep::precheck, 0x809f000au, -1,
                 "F11 backup room");
    CHECK(*second.fs.find(second.data_path()) == before);
}

void test_write_failures_F12_to_F14_F27() {
    const Bytes image_a = image_of(golden_body());

    {
        MemSlot slot;   // F13: partial write of the very first (data) write
        slot.fs.partial_write_on_call = 1;
        slot.fs.partial_write_bytes = 100;
        check_status(sotc::save_store(slot.store, "0", golden()), SaveResult::failed_generic,
                     SaveStep::write_data, 0x809f000bu, -1, "F13 partial");
        CHECK(!slot.fs.has(slot.data_path()));
        CHECK(!slot.fs.has(slot.data_path() + ".tmp"));
    }
    {
        MemSlot slot;   // F14: rename failure on the first data write
        slot.fs.fail_rename_on_call = 1;
        check_status(sotc::save_store(slot.store, "0", golden()), SaveResult::failed_generic,
                     SaveStep::write_data, 0x809f000bu, -1, "F14 rename");
        CHECK(!slot.fs.has(slot.data_path()));
        CHECK(!slot.fs.has(slot.data_path() + ".tmp"));
    }
    {
        MemSlot slot;   // F13 on a second store: the previous generation survives byte-identically
        CHECK(sotc::save_store(slot.store, "0", golden()).ok());
        ProgressV1 second = golden();
        second.words[0] = 77;
        slot.fs.partial_write_on_call = slot.fs.write_calls + 2;   // backup = +1, data = +2
        slot.fs.partial_write_bytes = 100;
        check_status(sotc::save_store(slot.store, "0", second), SaveResult::failed_generic,
                     SaveStep::write_data, 0x809f000bu, -1, "F13 second store");
        CHECK(*slot.fs.find(slot.data_path()) == image_a);
        CHECK(*slot.fs.find(slot.backup_path()) == image_a);   // the pre-commit copy is valid
        CHECK(!slot.fs.has(slot.data_path() + ".tmp"));
        // The previous generation is still loadable (F27 transactional).
        ProgressV1 loaded;
        check_status(sotc::save_load(slot.store, "0", loaded), SaveResult::success, SaveStep::none,
                     0, -1, "F27 load after failed store");
        CHECK(same_object(loaded, golden()));
    }
    {
        MemSlot slot;   // F27: fail the second store's data write, first generation loadable
        CHECK(sotc::save_store(slot.store, "0", golden()).ok());
        slot.fs.fail_write_on_call = slot.fs.write_calls + 2;
        ProgressV1 second = golden();
        second.ids.clear();
        check_status(sotc::save_store(slot.store, "0", second), SaveResult::failed_generic,
                     SaveStep::write_data, 0x809f000bu, -1, "F27 write");
        ProgressV1 loaded;
        CHECK(sotc::save_load(slot.store, "0", loaded).ok());
        CHECK(same_object(loaded, golden()));
    }
    {
        MemSlot slot;   // ENOSPC injected at the operation layer keeps result 3 and 0x809f000a
        slot.fs.write_error = SaveIoError::no_space;
        check_status(sotc::save_store(slot.store, "0", golden()), SaveResult::failed_no_space,
                     SaveStep::write_data, 0x809f000au, -1, "ENOSPC write");
        CHECK(!slot.fs.has(slot.data_path()));
    }
}

void test_backup_chain_F15_F16() {
    const Bytes image_a = image_of(golden_body());
    MemSlot slot;
    CHECK(sotc::save_store(slot.store, "0", golden()).ok());
    ProgressV1 second = golden();
    second.words[1] = 0x01020304u;
    CHECK(sotc::save_store(slot.store, "0", second).ok());
    CHECK(*slot.fs.find(slot.backup_path()) == image_a);

    // Corrupt USR-DATA: load fails, the backup is still good, restore returns generation A.
    Bytes broken = *slot.fs.find(slot.data_path());
    broken[broken.size() - 1] ^= std::byte{0x40};
    slot.fs.put(slot.data_path(), broken);
    ProgressV1 out;
    check_status(sotc::save_load(slot.store, "0", out), SaveResult::failed_corrupt,
                 SaveStep::verify_hash, 0x809f000fu, -1, "F15 corrupt live");
    bool has_backup = false;
    check_status(sotc::save_check_backup(slot.store, "0", has_backup), SaveResult::success,
                 SaveStep::none, 0, -1, "F15 check");
    CHECK(has_backup);
    check_status(sotc::save_restore_backup(slot.store, "0"), SaveResult::success, SaveStep::none,
                 0, -1, "F15 restore");
    CHECK(*slot.fs.find(slot.data_path()) == image_a);
    check_status(sotc::save_load(slot.store, "0", out), SaveResult::success, SaveStep::none, 0, -1,
                 "F15 load restored");
    CHECK(same_object(out, golden()));

    // F16: no backup at all.
    {
        MemSlot empty;
        check_status(sotc::save_restore_backup(empty.store, "0"),
                     SaveResult::failed_file_not_found, SaveStep::backup_restore, 0x809f0008u,
                     ENOENT, "F16 restore no backup");
        bool none = true;
        check_status(sotc::save_check_backup(empty.store, "0", none), SaveResult::success,
                     SaveStep::none, 0, -1, "F16 check none");
        CHECK(!none);
    }
    // A corrupt backup is reported as absent and never restored over a good USR-DATA.
    {
        MemSlot corrupt;
        CHECK(sotc::save_store(corrupt.store, "0", golden()).ok());
        corrupt.fs.put(corrupt.backup_path(), B({1, 2, 3}));
        bool bad = true;
        check_status(sotc::save_check_backup(corrupt.store, "0", bad), SaveResult::success,
                     SaveStep::none, 0, -1, "corrupt backup check");
        CHECK(!bad);
        check_status(sotc::save_restore_backup(corrupt.store, "0"), SaveResult::failed_corrupt,
                     SaveStep::backup_restore, 0x809f000fu, -1, "corrupt backup restore");
        CHECK(*corrupt.fs.find(corrupt.data_path()) == image_a);
    }
    // A corrupt backup that is a valid container but a bad object body is also refused.
    {
        MemSlot corrupt;
        CHECK(sotc::save_store(corrupt.store, "0", golden()).ok());
        Bytes bad_body = golden_body();
        bad_body.pop_back();
        corrupt.fs.put(corrupt.backup_path(), image_of(bad_body));
        check_status(sotc::save_restore_backup(corrupt.store, "0"), SaveResult::failed_corrupt,
                     SaveStep::backup_restore, 0x809f000fu, -1, "truncated backup restore");
    }
}

void test_cancel_and_ui_F26() {
    MemSlot slot;
    CHECK(sotc::save_store(slot.store, "0", golden()).ok());
    const Bytes image_a = image_of(golden_body());
    const unsigned writes_before = slot.fs.write_calls;

    ProgressV1 second = golden();
    second.words[4] = 1234;
    SaveOptions options;
    options.use_ui = true;

    UiProbe probe;
    probe.mode = UiProbe::Mode::cancel;
    SaveUi ui;
    ui.ctx = &probe;
    ui.present = &ui_present;
    check_status(sotc::save_store(slot.store, "0", second, options, &ui), SaveResult::canceled,
                 SaveStep::dialog, 0, -1, "F26 cancel");
    CHECK(probe.last_kind == SaveUiKind::confirm_overwrite);
    CHECK(probe.saw_active);
    CHECK(*slot.fs.find(slot.data_path()) == image_a);
    CHECK(slot.fs.write_calls == writes_before);   // no backup, no data write

    // use_ui without a usable hook is canceled, never a silent slot pick.
    check_status(sotc::save_store(slot.store, "0", second, options, nullptr),
                 SaveResult::canceled, SaveStep::dialog, 0, -1, "F26 no ui");
    check_status(sotc::save_load(slot.store, "0", second, options, nullptr),
                 SaveResult::canceled, SaveStep::dialog, 0, -1, "F26 no ui load");

    // A UI without confirm (fresh slot) is not asked at all.
    MemSlot fresh;
    UiProbe confirm;
    confirm.mode = UiProbe::Mode::confirm;
    SaveUi confirm_ui;
    confirm_ui.ctx = &confirm;
    confirm_ui.present = &ui_present;
    check_status(sotc::save_store(fresh.store, "0", golden(), options, &confirm_ui),
                 SaveResult::success, SaveStep::none, 0, -1, "fresh slot no dialog");
    CHECK(confirm.last_kind == SaveUiKind::choose_slot);   // default; present was never called
}

void test_choose_slot() {
    MemSlot slot;
    CHECK(sotc::save_store(slot.store, "0", golden()).ok());
    ProgressV1 other = golden();
    other.ids = {7, 8, 9};
    other.words[0] = 0xabcdef01u;
    CHECK(sotc::save_store(slot.store, "1", other).ok());

    UiProbe probe;
    probe.mode = UiProbe::Mode::choose;
    probe.chosen = 1;
    SaveUi ui;
    ui.ctx = &probe;
    ui.present = &ui_present;

    SaveOptions options;
    options.use_ui = true;
    ProgressV1 loaded;
    check_status(sotc::save_load(slot.store, "0", loaded, options, &ui), SaveResult::success,
                 SaveStep::none, 0, -1, "choose slot 1");
    CHECK(probe.last_kind == SaveUiKind::choose_slot);
    CHECK(probe.slot_count == 2);
    CHECK(same_object(loaded, other));

    probe.chosen = 2;   // out of range
    check_status(sotc::save_load(slot.store, "0", loaded, options, &ui),
                 SaveResult::failed_generic, SaveStep::dialog, 0, -1, "choose slot oob");

    MemSlot empty;   // no slots at all -> the original's empty enumeration result 4
    check_status(sotc::save_load(empty.store, "0", loaded, options, &ui),
                 SaveResult::failed_file_not_found, SaveStep::enumerate, 0, -1, "choose no slots");
}

void test_busy_retries_F29() {
    MemSlot slot;
    CHECK(sotc::save_store(slot.store, "0", golden()).ok());
    slot.fs.read_calls = 0;
    slot.fs.busy_reads = 10;
    SaveOptions options;
    options.max_busy_retries = 2;
    ProgressV1 out;
    check_status(sotc::save_load(slot.store, "0", out, options), SaveResult::failed_generic,
                 SaveStep::read_file, 0x809f0013u, -1, "F29 busy");
    CHECK(slot.fs.read_calls == 3);   // 1 attempt + 2 retries, then a visible failure

    slot.fs.read_calls = 0;
    slot.fs.busy_reads = 2;   // the third attempt succeeds
    check_status(sotc::save_load(slot.store, "0", out, options), SaveResult::success,
                 SaveStep::none, 0, -1, "F29 busy then ok");
    CHECK(slot.fs.read_calls == 3);
}

void test_missing_ops_and_roots() {
    SaveStore none;
    none.root = "/saves";
    none.ops = sotc::save_io_none();
    std::vector<sotc::SaveSlotInfo> slots;
    check_status(sotc::save_enumerate(none, slots), SaveResult::failed_generic,
                 SaveStep::enumerate, 0, -1, "no ops enumerate");
    ProgressV1 out;
    check_status(sotc::save_load(none, "0", out), SaveResult::failed_generic, SaveStep::read_file,
                 0, -1, "no ops load");
    check_status(sotc::save_store(none, "0", golden()), SaveResult::failed_generic,
                 SaveStep::precheck, 0, -1, "no ops store");

    // Only the mount seam is missing: everything before it succeeds, then a visible mount failure.
    SaveMemoryFs fs;
    sotc::SaveIoOps partial = sotc::save_io_memory(fs);    partial.make_dir = nullptr;
    SaveStore no_mount;
    no_mount.root = "/saves";
    no_mount.ops = partial;
    check_status(sotc::save_store(no_mount, "0", golden()), SaveResult::failed_generic,
                 SaveStep::mount, 0, -1, "no mount op");
    CHECK(!fs.has("/saves/SAVEDATA00/USR-DATA"));
    bool has_backup = true;
    check_status(sotc::save_check_backup(none, "0", has_backup), SaveResult::failed_generic,
                 SaveStep::backup_check, 0, -1, "no ops check");
    CHECK(!has_backup);
    check_status(sotc::save_restore_backup(none, "0"), SaveResult::failed_generic,
                 SaveStep::backup_restore, 0, -1, "no ops restore");

    SaveStore no_root;
    no_root.ops = sotc::save_io_posix();
    CHECK(sotc::save_store_init(no_root).error == SaveIoError::unsupported);

    // An empty root would write relative to the process CWD: every entry point refuses it.
    SaveMemoryFs rootless_fs;
    SaveStore rootless;
    rootless.ops = sotc::save_io_memory(rootless_fs);
    ProgressV1 rootless_out;
    check_status(sotc::save_load(rootless, "0", rootless_out), SaveResult::failed_generic,
                 SaveStep::validate, 0, -1, "empty root load");
    check_status(sotc::save_store(rootless, "0", golden()), SaveResult::failed_generic,
                 SaveStep::validate, 0, -1, "empty root store");
    check_status(sotc::save_enumerate(rootless, slots), SaveResult::failed_generic,
                 SaveStep::validate, 0, -1, "empty root enumerate");
    bool rootless_backup = true;
    check_status(sotc::save_check_backup(rootless, "0", rootless_backup),
                 SaveResult::failed_generic, SaveStep::validate, 0, -1, "empty root check");
    check_status(sotc::save_restore_backup(rootless, "0"), SaveResult::failed_generic,
                 SaveStep::validate, 0, -1, "empty root restore");
    CHECK(rootless_fs.files.empty());

    // A suffix that could escape the slot directory is refused before any I/O.
    MemSlot guarded;
    check_status(sotc::save_store(guarded.store, "a/b", golden()), SaveResult::failed_generic,
                 SaveStep::validate, 0, -1, "bad suffix store");
    CHECK(guarded.fs.files.empty());

    // A missing root is an empty enumeration, not an error.
    SaveStore absent;
    absent.root = "/definitely/not/here";
    SaveMemoryFs scratch;
    absent.ops = sotc::save_io_memory(scratch);
    CHECK(sotc::save_enumerate(absent, slots).ok());
    CHECK(slots.empty());
}

void test_suffix_validation() {
    CHECK(sotc::save_slot_suffix_valid("0"));
    CHECK(sotc::save_slot_suffix_valid("ABC_09-x"));
    CHECK(sotc::save_slot_suffix_valid(std::string(24, 'a')));
    CHECK(!sotc::save_slot_suffix_valid(""));
    CHECK(!sotc::save_slot_suffix_valid(std::string(25, 'a')));
    CHECK(!sotc::save_slot_suffix_valid("a/b"));
    CHECK(!sotc::save_slot_suffix_valid(".."));
    CHECK(!sotc::save_slot_suffix_valid("a.b"));
    CHECK(!sotc::save_slot_suffix_valid("a b"));
    CHECK(!sotc::save_slot_suffix_valid("\xc3\xa9"));
    CHECK(sotc::save_slot_dir("0") == "SAVEDATA00");
    CHECK(sotc::save_slot_path("0", "USR-DATA") == "SAVEDATA00/USR-DATA");
    SaveStore store;
    store.root = "/root/";
    CHECK(sotc::save_store_path(store, "0", "USR-DATA") == "/root/SAVEDATA00/USR-DATA");
    CHECK(sotc::to_string(SaveIoError::no_space) == std::string("no_space"));
}

void test_enumerate_two_slots() {
    MemSlot slot;
    CHECK(sotc::save_store(slot.store, "0", golden()).ok());
    ProgressV1 second = golden();
    second.ids.clear();
    CHECK(sotc::save_store(slot.store, "1", second).ok());
    CHECK(sotc::save_store(slot.store, "1", second).ok());   // creates .bak for slot 1

    std::vector<sotc::SaveSlotInfo> slots;
    check_status(sotc::save_enumerate(slot.store, slots), SaveResult::success, SaveStep::none, 0,
                 -1, "enumerate two");
    CHECK(slots.size() == 2);
    CHECK(slots[0].dir_name == "SAVEDATA00" && slots[0].game_suffix == "0");
    CHECK(slots[1].dir_name == "SAVEDATA01" && slots[1].game_suffix == "1");
    CHECK(!slots[0].has_backup);
    CHECK(slots[1].has_backup);
    CHECK(slots[1].body_size == sotc::progress_body_size(0));

    // A stray file that is not a slot, and a SAVEDATA dir without USR-DATA, are not listed.
    slot.fs.put("/saves/README", B({'x'}));
    slot.fs.files["/saves/SAVEDATA09/USR-META"] = B({0});
    slots.clear();
    CHECK(sotc::save_enumerate(slot.store, slots).ok());
    CHECK(slots.size() == 2);
}

void test_keep_backup_false() {
    MemSlot slot;
    CHECK(sotc::save_store(slot.store, "0", golden()).ok());
    CHECK(sotc::save_store(slot.store, "0", golden()).ok());
    CHECK(slot.fs.has(slot.backup_path()));
    SaveOptions options;
    options.keep_backup = false;
    CHECK(sotc::save_store(slot.store, "0", golden(), options).ok());
    CHECK(!slot.fs.has(slot.backup_path()));
    bool has_backup = true;
    CHECK(sotc::save_check_backup(slot.store, "0", has_backup).ok());
    CHECK(!has_backup);
}

void test_restore_write_failure() {
    MemSlot slot;
    CHECK(sotc::save_store(slot.store, "0", golden()).ok());
    CHECK(sotc::save_store(slot.store, "0", golden()).ok());
    const Bytes before = *slot.fs.find(slot.data_path());
    slot.fs.write_error = SaveIoError::io_write;
    check_status(sotc::save_restore_backup(slot.store, "0"), SaveResult::failed_generic,
                 SaveStep::backup_restore, 0x809f000bu, -1, "restore write failure");
    CHECK(*slot.fs.find(slot.data_path()) == before);
}

void test_posix_permission_denied_F12() {
    if (::geteuid() == 0) {
        std::puts("save_transaction: skipping F12 (running as root)");
        return;
    }
    const char* tmp = std::getenv("TMPDIR");
    std::string tmpl = std::string(tmp != nullptr && *tmp != '\0' ? tmp : "/tmp") +
                       "/sotc_save_tx_perm_XXXXXX";
    const char* made = ::mkdtemp(tmpl.data());
    CHECK(made != nullptr);
    SaveStore store;
    store.root = std::string(made) + "/saves";
    store.ops = sotc::save_io_posix();
    CHECK(sotc::save_store_init(store).ok());
    const std::string slot_dir = sotc::save_store_dir(store, "0");
    CHECK(::mkdir(slot_dir.c_str(), 0700) == 0);
    CHECK(::chmod(slot_dir.c_str(), 0555) == 0);
    const sotc::SaveStatus status = sotc::save_store(store, "0", golden());
    (void)::chmod(slot_dir.c_str(), 0700);
    check_status(status, SaveResult::failed_generic, SaveStep::write_data, 0x809f000bu, EACCES,
                 "F12 EACCES");
    struct stat st {};
    CHECK(::stat(sotc::save_store_path(store, "0", sotc::kSaveDataName).c_str(), &st) != 0);

    const std::string cleanup = "rm -rf '" + std::string(made) + "'";
    CHECK(std::system(cleanup.c_str()) == 0);
}

}  // namespace

int main() {
    test_result_ordinals_and_layout();
    test_golden_body_F01_F06();
    test_flag_conflict_F24();
    test_decode_policies_F17_to_F21();
    test_payload_rule_errors_F22_F23();
    test_memory_round_trip_F01();
    test_posix_round_trip_F02();
    test_posix_permission_denied_F12();
    test_load_corruption_F03_to_F09();
    test_no_space_F11();
    test_write_failures_F12_to_F14_F27();
    test_backup_chain_F15_F16();
    test_cancel_and_ui_F26();
    test_choose_slot();
    test_busy_retries_F29();
    test_missing_ops_and_roots();
    test_suffix_validation();
    test_enumerate_two_slots();
    test_keep_backup_false();
    test_restore_write_failure();
    std::puts("save_transaction: all checks passed");
    return 0;
}
