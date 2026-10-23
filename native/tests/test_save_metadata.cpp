// Fixture for the E604 metadata/icon/delete slice: the 0x530 block, the native USR-META wrapper,
// the advisory icon and save_delete. No platform metadata, no real save, no dialog.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "core/save_checksum.hpp"
#include "core/save_meta.hpp"
#include "core/save_slot_store.hpp"
#include "core/save_transaction.hpp"

using namespace sotc;

namespace {

int g_checks = 0;

#define CHECK(c)                                                                     \
    do {                                                                             \
        ++g_checks;                                                                  \
        if (!(c)) {                                                                  \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);          \
            std::exit(1);                                                            \
        }                                                                            \
    } while (0)

SaveMeta sample_meta() {
    SaveMeta meta;
    meta.title = "SOTC-NATIVE-TITLE";
    meta.subtitle = "SLOT-0";
    meta.detail = "synthetic-detail";
    meta.user_param = 0x00010003u;
    return meta;
}

SaveStore make_store(SaveMemoryFs& fs) {
    SaveStore store;
    store.root = "/mem";
    store.ops = save_io_memory(fs);
    return store;
}

std::string data_path(const std::string& suffix) {
    return "/mem/" + save_slot_path(suffix, kSaveDataName);
}

std::string meta_path(const std::string& suffix) {
    return "/mem/" + save_slot_path(suffix, kSaveMetaName);
}

std::string icon_path(const std::string& suffix) {
    return "/mem/" + save_slot_path(suffix, kSaveIconName);
}

}  // namespace

int main() {
    // ---- M01: golden 0x530 block and container (independent Python re-derivation). ----
    {
        std::vector<std::byte> block;
        CHECK(encode_save_meta(sample_meta(), block) == SaveMetaError::none);
        CHECK(block.size() == 0x530);
        CHECK(fnv1a32(block, 0x811C9DC5u) == 0x6bd7d637u);
        const std::vector<std::byte> file = encode_meta_file(block, 0x6d3a0905u, 4);
        CHECK(file.size() == 0x550);
        CHECK(fnv1a32(file, 0x811C9DC5u) == 0x57959728u);
        CHECK(std::memcmp(file.data(), "SOTCMETA", 8) == 0);
    }

    // ---- M02: text boundaries (slot-1 accepted, slot rejected). ----
    {
        std::vector<std::byte> block;
        SaveMeta meta = sample_meta();
        meta.title.assign(0x7F, 'T');
        CHECK(encode_save_meta(meta, block) == SaveMetaError::none);
        meta.title.assign(0x80, 'T');
        CHECK(encode_save_meta(meta, block) == SaveMetaError::text_too_long);
        meta = sample_meta();
        meta.subtitle.assign(0x80, 'S');
        CHECK(encode_save_meta(meta, block) == SaveMetaError::text_too_long);
        meta = sample_meta();
        meta.detail.assign(0x3FF, 'D');
        CHECK(encode_save_meta(meta, block) == SaveMetaError::none);
        meta.detail.assign(0x400, 'D');
        CHECK(encode_save_meta(meta, block) == SaveMetaError::text_too_long);
    }

    // ---- M03: embedded NUL is rejected, never truncated. ----
    {
        std::vector<std::byte> block;
        SaveMeta meta = sample_meta();
        meta.title = std::string("a\0b", 3);
        CHECK(encode_save_meta(meta, block) == SaveMetaError::text_has_nul);
    }

    // ---- M04: userParam LE at +0x500, reserved at +0x504, texts at their slots. ----
    {
        SaveMeta meta = sample_meta();
        meta.reserved[0] = std::byte{0xAB};
        meta.reserved[0x2B] = std::byte{0xCD};
        std::vector<std::byte> block;
        CHECK(encode_save_meta(meta, block) == SaveMetaError::none);
        SaveMeta decoded;
        CHECK(decode_save_meta(block, decoded) == SaveMetaError::none);
        CHECK(decoded.title == meta.title);
        CHECK(decoded.subtitle == meta.subtitle);
        CHECK(decoded.detail == meta.detail);
        CHECK(decoded.user_param == meta.user_param);
        CHECK(decoded.reserved[0] == std::byte{0xAB});
        CHECK(decoded.reserved[0x2B] == std::byte{0xCD});
        CHECK(block[0x500] == std::byte{0x03});
        CHECK(block[0x501] == std::byte{0x00});
        // The last byte of the block is reserved, not text.
        CHECK(block[0x52F] == std::byte{0xCD});
    }

    // ---- M05: container strictness. ----
    {
        std::vector<std::byte> block;
        CHECK(encode_save_meta(sample_meta(), block) == SaveMetaError::none);
        std::vector<std::byte> file = encode_meta_file(block, 0x11223344u, 4);
        std::uint32_t hash = 0;
        std::uint32_t descriptor = 0;
        std::vector<std::byte> out;
        CHECK(decode_meta_file(file, hash, descriptor, out) == SaveMetaFileError::none);
        CHECK(hash == 0x11223344u);
        CHECK(descriptor == 4);
        CHECK(out == block);

        std::vector<std::byte> bad = file;
        bad[0] = std::byte{'X'};
        CHECK(decode_meta_file(bad, hash, descriptor, out) == SaveMetaFileError::bad_magic);
        bad = file;
        bad[8] = std::byte{2};
        CHECK(decode_meta_file(bad, hash, descriptor, out) == SaveMetaFileError::bad_version);
        bad = file;
        bad[0x18] = std::byte{0};
        CHECK(decode_meta_file(bad, hash, descriptor, out) == SaveMetaFileError::bad_param_bytes);
        CHECK(decode_meta_file(std::span<const std::byte>(file).first(0x10), hash, descriptor, out) ==
              SaveMetaFileError::bad_size);
    }

    // ---- M06/M07: attachments are written; absent attachments write nothing. ----
    {
        SaveMemoryFs fs;
        SaveStore store = make_store(fs);
        ProgressV1 progress = make_progress_v1();
        SaveMeta meta = sample_meta();
        SaveIcon icon;
        icon.bytes = {std::byte{0x89}, std::byte{'P'}, std::byte{'N'}, std::byte{'G'}};
        icon.resource_name = "$SCEA_SAVEINFO_ProgressIcon";
        SaveAttachments attachments{&meta, &icon};
        const SaveStatus stored = save_store(store, "0", progress, attachments);
        CHECK(stored.ok());
        CHECK(stored.data_committed);
        CHECK(fs.has(data_path("0")));
        CHECK(fs.has(meta_path("0")));
        CHECK(fs.has(icon_path("0")));
        CHECK(fs.find(meta_path("0"))->size() == 0x550);

        std::vector<SaveSlotInfo> slots;
        CHECK(save_enumerate(store, slots).ok());
        CHECK(slots.size() == 1);
        CHECK(slots[0].has_meta);
        CHECK(slots[0].meta_matches_data);
        CHECK(slots[0].meta.title == meta.title);

        SaveMemoryFs plain;
        SaveStore plain_store = make_store(plain);
        CHECK(save_store(plain_store, "0", progress).ok());
        CHECK(plain.has(data_path("0")));
        CHECK(!plain.has(meta_path("0")));
        CHECK(!plain.has(icon_path("0")));
    }

    // ---- M08: invalid attachments write nothing at all. ----
    {
        SaveMemoryFs fs;
        SaveStore store = make_store(fs);
        ProgressV1 progress = make_progress_v1();
        SaveMeta meta = sample_meta();
        meta.title.assign(0x80, 'T');
        SaveAttachments bad{&meta, nullptr};
        const SaveStatus stored = save_store(store, "0", progress, bad);
        CHECK(!stored.ok());
        CHECK(stored.step == SaveStep::validate);
        CHECK(!stored.data_committed);
        CHECK(fs.files.empty());
        CHECK(fs.dirs.empty());

        SaveIcon huge;
        huge.bytes.assign(kSaveIconMaxBytes + 1, std::byte{1});
        SaveMeta good = sample_meta();
        SaveAttachments bad_icon{&good, &huge};
        const SaveStatus icon_stored = save_store(store, "0", progress, bad_icon);
        CHECK(!icon_stored.ok());
        CHECK(!icon_stored.data_committed);
        CHECK(fs.files.empty());
    }

    // ---- M09: a failed USR-META write keeps the data committed. ----
    {
        SaveMemoryFs fs;
        SaveStore store = make_store(fs);
        fs.fail_write_on_call = 2;  // 1 = USR-DATA, 2 = USR-META
        ProgressV1 progress = make_progress_v1();
        SaveMeta meta = sample_meta();
        const SaveAttachments attachments{&meta, nullptr};
        const SaveStatus stored = save_store(store, "0", progress, attachments);
        CHECK(!stored.ok());
        CHECK(stored.step == SaveStep::write_meta);
        CHECK(stored.data_committed);
        CHECK(fs.has(data_path("0")));
        CHECK(!fs.has(meta_path("0")));
    }

    // ---- M10: a failed icon write keeps the data and the metadata committed. ----
    {
        SaveMemoryFs fs;
        SaveStore store = make_store(fs);
        fs.fail_write_on_call = 3;  // 3 = icon
        ProgressV1 progress = make_progress_v1();
        SaveMeta meta = sample_meta();
        SaveIcon icon;
        icon.bytes = {std::byte{1}};
        const SaveAttachments attachments{&meta, &icon};
        const SaveStatus stored = save_store(store, "0", progress, attachments);
        CHECK(!stored.ok());
        CHECK(stored.step == SaveStep::write_icon);
        CHECK(stored.data_committed);
        CHECK(fs.has(data_path("0")));
        CHECK(fs.has(meta_path("0")));
        CHECK(!fs.has(icon_path("0")));
    }

    // ---- M11: delete removes satellites, backup and data. ----
    {
        SaveMemoryFs fs;
        SaveStore store = make_store(fs);
        ProgressV1 progress = make_progress_v1();
        SaveMeta meta = sample_meta();
        SaveIcon icon;
        icon.bytes = {std::byte{2}};
        CHECK(save_store(store, "0", progress, SaveAttachments{&meta, &icon}).ok());
        fs.put("/mem/" + save_slot_path("0", "USR-DATA.bak"), std::span<const std::byte>{});
        CHECK(save_delete(store, "0").ok());
        CHECK(fs.files.empty());
    }

    // ---- M12: deleting a missing slot is result 4 with 0x809f000e. ----
    {
        SaveMemoryFs fs;
        SaveStore store = make_store(fs);
        const SaveStatus deleted = save_delete(store, "9");
        CHECK(deleted.result == SaveResult::failed_file_not_found);
        CHECK(deleted.step == SaveStep::delete_slot);
        CHECK(deleted.sce_status == 0x809F000Eu);
    }

    // ---- M13: BUSY on delete is not retried: one remove, result 7 + 0x809f0013. ----
    {
        SaveMemoryFs fs;
        SaveStore store = make_store(fs);
        fs.put(data_path("0"), std::span<const std::byte>{});
        fs.remove_error = SaveIoError::busy;
        const SaveStatus deleted = save_delete(store, "0");
        CHECK(deleted.result == SaveResult::failed_generic);
        CHECK(deleted.step == SaveStep::delete_slot);
        CHECK(deleted.sce_status == 0x809F0013u);
        CHECK(fs.remove_calls == 1);
    }

    // ---- M14: a failed final data removal leaves the slot loadable. ----
    {
        SaveMemoryFs fs;
        SaveStore store = make_store(fs);
        ProgressV1 progress = make_progress_v1();
        SaveMeta meta = sample_meta();
        SaveIcon icon;
        icon.bytes = {std::byte{3}};
        CHECK(save_store(store, "0", progress, SaveAttachments{&meta, &icon}).ok());
        fs.fail_remove_on_call = 5;  // meta, icon, data.bak, meta.bak, data
        const SaveStatus deleted = save_delete(store, "0");
        CHECK(!deleted.ok());
        CHECK(deleted.step == SaveStep::delete_slot);
        CHECK(fs.has(data_path("0")));
        CHECK(!fs.has(meta_path("0")));
        CHECK(!fs.has(icon_path("0")));
        ProgressV1 loaded;
        CHECK(save_load(store, "0", loaded).ok());
    }

    // ---- M15: pinned result and dialog tables. ----
    {
        CHECK(map_sce_result(0x809F0004u) == SaveResult::failed_mount);
        CHECK(map_sce_result(0x809F000Au) == SaveResult::failed_no_space);
        CHECK(map_sce_result(0x809F000Eu) == SaveResult::failed_file_not_found);
        CHECK(map_sce_result(0x809F000Fu) == SaveResult::failed_corrupt);
        CHECK(map_sce_result(0x809F0008u) == SaveResult::failed_generic);
        CHECK(map_sce_result(0x809F0013u) == SaveResult::failed_generic);
        CHECK(classify_dialog_status(0) == SaveDialogSignal::poll_again);
        CHECK(classify_dialog_status(2) == SaveDialogSignal::close_progress);
        CHECK(classify_dialog_status(3) == SaveDialogSignal::get_result);
        CHECK(classify_dialog_status(0x80B80008u) == SaveDialogSignal::initialize_busy);
    }

    // ---- M16: metadata mismatch is reported, not an error; bad metadata is ignored. ----
    {
        SaveMemoryFs fs;
        SaveStore store = make_store(fs);
        ProgressV1 progress = make_progress_v1();
        SaveMeta meta = sample_meta();
        CHECK(save_store(store, "0", progress, SaveAttachments{&meta, nullptr}).ok());
        std::vector<std::byte> block;
        CHECK(encode_save_meta(meta, block) == SaveMetaError::none);
        std::vector<std::byte> wrong = encode_meta_file(block, 0xDEADBEEFu, 4);
        fs.put(meta_path("0"), wrong);
        std::vector<SaveSlotInfo> slots;
        CHECK(save_enumerate(store, slots).ok());
        CHECK(slots.size() == 1);
        CHECK(slots[0].has_meta);
        CHECK(!slots[0].meta_matches_data);

        fs.put(meta_path("0"), std::vector<std::byte>{std::byte{1}, std::byte{2}});
        CHECK(save_enumerate(store, slots).ok());
        CHECK(slots.size() == 1);
        CHECK(!slots[0].has_meta);
    }

    std::printf("test_save_metadata: %d checks passed\n", g_checks);
    return 0;
}
