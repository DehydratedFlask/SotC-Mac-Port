#include "core/save_transaction.hpp"

#include <algorithm>
#include <cstdint>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "core/save_checksum.hpp"
#include "core/save_chunk_header.hpp"
#include "core/save_stream.hpp"

namespace sotc {
namespace {

// Raw TSceResult values observed at the corresponding step (report §4.3). 0x809f0008 is the
// internal "no backup" code; the original does not surface it as an ESaveResult.
constexpr std::uint32_t kSceNoSpace = 0x809f000au;
constexpr std::uint32_t kSceShortIo = 0x809f000bu;
constexpr std::uint32_t kSceFileNotFound = 0x809f000eu;
constexpr std::uint32_t kSceCorrupt = 0x809f000fu;
constexpr std::uint32_t kSceBusy = 0x809f0013u;
constexpr std::uint32_t kSceNoBackup = 0x809f0008u;

thread_local unsigned t_ui_depth = 0;

std::uint32_t get_u32(const std::byte* p) noexcept {
    std::uint32_t v = 0;
    for (int i = 0; i != 4; ++i) v |= std::to_integer<std::uint32_t>(p[i]) << (8 * i);
    return v;
}

void put_u32(std::byte* p, std::uint32_t v) noexcept {
    for (int i = 0; i != 4; ++i) p[i] = static_cast<std::byte>((v >> (8 * i)) & 0xffu);
}

// The 16-byte outer header. Same layout as core/save_stream.hpp; decoded here so the transaction
// can attribute a failure to read_header / read_body / verify_hash individually.
SaveStreamHeader decode_header(std::span<const std::byte> bytes) noexcept {
    SaveStreamHeader header;
    header.version = get_u32(bytes.data() + 0);
    header.body_hash = get_u32(bytes.data() + 4);
    header.body_size = get_u32(bytes.data() + 8);
    header.descriptor_id = get_u32(bytes.data() + 12);
    return header;
}

void encode_header(std::byte* out, const SaveStreamHeader& header) noexcept {
    put_u32(out + 0, header.version);
    put_u32(out + 4, header.body_hash);
    put_u32(out + 8, header.body_size);
    put_u32(out + 12, header.descriptor_id);
}

SaveStatus status(SaveResult result, SaveStep step, std::uint32_t sce = 0, int os_errno = 0) {
    SaveStatus out;
    out.result = result;
    out.step = step;
    out.sce_status = sce;
    out.os_errno = os_errno;
    return out;
}

std::string backup_file_name() {
    std::string out(kSaveDataName);
    out.append(kSaveBackupSuffix);
    return out;
}

template <class Fn>
SaveIoResult with_retries(const SaveOptions& options, Fn&& fn) {
    SaveIoResult result{};
    for (unsigned attempt = 0; attempt <= options.max_busy_retries; ++attempt) {
        result = fn();
        if (result.error != SaveIoError::busy) return result;
    }
    return result;   // bounded: the original's unbounded `while busy: sleep(1)` is not copied
}

SaveIoResult read_slot(const SaveStore& store, const SaveOptions& options, std::string_view suffix,
                       std::string_view file, std::vector<std::byte>& out) {
    if (store.ops.read_file == nullptr) return {SaveIoError::unsupported, 0};
    const std::string path = save_store_path(store, suffix, file);
    return with_retries(options, [&] { return store.ops.read_file(store.ops.ctx, path, out); });
}

SaveIoResult write_slot(const SaveStore& store, const SaveOptions& options, std::string_view suffix,
                        std::string_view file, std::span<const std::byte> bytes) {
    if (store.ops.write_file_atomic == nullptr) return {SaveIoError::unsupported, 0};
    const std::string path = save_store_path(store, suffix, file);
    return with_retries(options, [&] {
        return store.ops.write_file_atomic(store.ops.ctx, path, bytes);
    });
}

SaveIoResult size_of_slot(const SaveStore& store, const SaveOptions& options,
                          std::string_view suffix, std::string_view file, std::uint64_t& size) {
    if (store.ops.file_size == nullptr) return {SaveIoError::unsupported, 0};
    const std::string path = save_store_path(store, suffix, file);
    return with_retries(options, [&] { return store.ops.file_size(store.ops.ctx, path, size); });
}

SaveIoResult remove_slot(const SaveStore& store, const SaveOptions& options,
                         std::string_view suffix, std::string_view file) {
    if (store.ops.remove_file == nullptr) return {SaveIoError::unsupported, 0};
    const std::string path = save_store_path(store, suffix, file);
    return with_retries(options, [&] { return store.ops.remove_file(store.ops.ctx, path); });
}

// Validates one complete USR-DATA image. The order is the original's observable order:
// short header -> version -> size ceiling -> short body -> body hash -> descriptor -> chunks.
SaveStatus parse_image(std::span<const std::byte> bytes, ProgressV1* out) {
    if (bytes.size() < kSaveStreamHeaderSize) {
        return status(SaveResult::failed_generic, SaveStep::read_header, kSceShortIo);
    }
    const SaveStreamHeader header = decode_header(bytes);
    if (header.version != kSaveStreamVersion) {
        return status(SaveResult::failed_corrupt, SaveStep::read_header, kSceCorrupt);
    }
    if (header.body_size > kSaveStreamMaxBodySize) {
        // Checked before the body is looked at; no body-sized allocation happens.
        return status(SaveResult::failed_corrupt, SaveStep::read_header, kSceCorrupt);
    }
    if (bytes.size() - kSaveStreamHeaderSize < header.body_size) {
        return status(SaveResult::failed_corrupt, SaveStep::read_body, kSceCorrupt);
    }
    const std::span<const std::byte> body =
        bytes.subspan(kSaveStreamHeaderSize, header.body_size);
    if (fnv1a32(body, kFnv1a32OffsetBasis) != header.body_hash) {
        return status(SaveResult::failed_corrupt, SaveStep::verify_hash, kSceCorrupt);
    }
    if (header.descriptor_id != kProgressDescriptorId) {
        return status(SaveResult::failed_corrupt, SaveStep::verify_descriptor, kSceCorrupt);
    }
    ProgressV1 scratch;
    ProgressV1& destination = out == nullptr ? scratch : *out;
    const ObjectStatus decoded = decode_progress_body(body, destination);
    if (!decoded.ok()) {
        return status(SaveResult::failed_corrupt, SaveStep::parse_body, kSceCorrupt);
    }
    return status(SaveResult::success, SaveStep::none);
}

SaveStatus write_failure(const SaveIoResult& result, SaveStep step) {
    // The original has no rename; the closest observed code for a write-family failure is the
    // short-write code 0x809f000b. ENOSPC keeps its own result and code.
    if (result.error == SaveIoError::unsupported) {
        return status(SaveResult::failed_generic, step);
    }
    if (result.error == SaveIoError::no_space) {
        return status(SaveResult::failed_no_space, step, kSceNoSpace, result.os_errno);
    }
    if (result.error == SaveIoError::busy) {
        return status(SaveResult::failed_generic, step, kSceBusy, result.os_errno);
    }
    return status(SaveResult::failed_generic, step, kSceShortIo, result.os_errno);
}

}  // namespace

const char* to_string(SaveStep step) noexcept {
    switch (step) {
        case SaveStep::none: return "none";
        case SaveStep::validate: return "validate";
        case SaveStep::enumerate: return "enumerate";
        case SaveStep::dialog: return "dialog";
        case SaveStep::mount: return "mount";
        case SaveStep::precheck: return "precheck";
        case SaveStep::read_file: return "read_file";
        case SaveStep::read_header: return "read_header";
        case SaveStep::read_body: return "read_body";
        case SaveStep::verify_hash: return "verify_hash";
        case SaveStep::verify_descriptor: return "verify_descriptor";
        case SaveStep::parse_body: return "parse_body";
        case SaveStep::encode_body: return "encode_body";
        case SaveStep::backup_capture: return "backup_capture";
        case SaveStep::write_data: return "write_data";
        case SaveStep::backup_check: return "backup_check";
        case SaveStep::backup_restore: return "backup_restore";
        case SaveStep::write_meta: return "write_meta";
        case SaveStep::write_icon: return "write_icon";
        case SaveStep::delete_slot: return "delete_slot";
    }
    return "unknown";
}

const char* to_string(SaveUiKind kind) noexcept {
    switch (kind) {
        case SaveUiKind::choose_slot: return "choose_slot";
        case SaveUiKind::confirm_overwrite: return "confirm_overwrite";
        case SaveUiKind::corrupt_offer_restore: return "corrupt_offer_restore";
    }
    return "unknown";
}

SaveStatus save_enumerate(const SaveStore& store, std::vector<SaveSlotInfo>& out) {
    out.clear();
    if (store.root.empty()) return status(SaveResult::failed_generic, SaveStep::validate);
    if (store.ops.list_dir == nullptr) {
        return status(SaveResult::failed_generic, SaveStep::enumerate);
    }
    const SaveOptions defaults;
    std::vector<std::string> names;
    const SaveIoResult listed = with_retries(defaults, [&] {
        names.clear();
        return store.ops.list_dir(store.ops.ctx, store.root, names);
    });
    if (listed.error == SaveIoError::not_found) return status(SaveResult::success, SaveStep::none);
    if (!listed.ok()) {
        return status(SaveResult::failed_generic, SaveStep::enumerate,
                      listed.error == SaveIoError::busy ? kSceBusy : 0, listed.os_errno);
    }

    std::set<std::string> listed_once;
    for (const std::string& name : names) {
        const std::string_view view = name;
        if (view.size() <= kSaveDirPrefix.size() || !view.starts_with(kSaveDirPrefix)) continue;
        const std::string_view suffix = view.substr(kSaveDirPrefix.size());
        if (!save_slot_suffix_valid(suffix)) continue;
        if (!listed_once.insert(name).second) continue;

        std::vector<std::byte> bytes;
        const SaveIoResult read = read_slot(store, defaults, suffix, kSaveDataName, bytes);
        if (read.error == SaveIoError::not_found) continue;   // not a slot we wrote
        if (!read.ok()) {
            return status(SaveResult::failed_generic, SaveStep::enumerate, 0, read.os_errno);
        }
        SaveSlotInfo info;
        info.dir_name = name;
        info.game_suffix = std::string(suffix);
        info.committed_bytes = bytes.size();
        if (bytes.size() >= kSaveStreamHeaderSize) {
            const SaveStreamHeader header = decode_header(bytes);
            info.body_size = header.body_size;
            info.body_hash = header.body_hash;
        }
        bool has_backup = false;
        const SaveStatus checked = save_check_backup(store, suffix, has_backup);
        if (!checked.ok()) return checked;
        info.has_backup = has_backup;

        // USR-META is advisory: a bad magic/version or a missing file is not an enumerate error.
        std::vector<std::byte> meta_bytes;
        const SaveIoResult meta_read =
            read_slot(store, defaults, suffix, kSaveMetaName, meta_bytes);
        if (meta_read.ok()) {
            std::uint32_t stored_hash = 0;
            std::uint32_t descriptor = 0;
            std::vector<std::byte> block;
            if (decode_meta_file(meta_bytes, stored_hash, descriptor, block) ==
                    SaveMetaFileError::none &&
                decode_save_meta(block, info.meta) == SaveMetaError::none) {
                info.has_meta = true;
                info.meta_matches_data = stored_hash == info.body_hash;
            }
        } else if (meta_read.error != SaveIoError::not_found) {
            return status(SaveResult::failed_generic, SaveStep::enumerate, 0,
                          meta_read.os_errno);
        }
        out.push_back(std::move(info));
    }
    std::sort(out.begin(), out.end(), [](const SaveSlotInfo& a, const SaveSlotInfo& b) {
        return a.dir_name < b.dir_name;
    });
    return status(SaveResult::success, SaveStep::none);
}

SaveStatus save_load(const SaveStore& store, std::string_view suffix, ProgressV1& out,
                     const SaveOptions& options, const SaveUi* ui) {
    if (store.root.empty() || !save_slot_suffix_valid(suffix)) {
        return status(SaveResult::failed_generic, SaveStep::validate);
    }
    std::string chosen(suffix);
    if (options.use_ui) {
        std::vector<SaveSlotInfo> slots;
        const SaveStatus listed = save_enumerate(store, slots);
        if (!listed.ok()) return listed;
        if (slots.empty()) {
            // The original's empty DirNameSearch result is ESaveResult 4 (not found).
            return status(SaveResult::failed_file_not_found, SaveStep::enumerate);
        }
        if (ui == nullptr || ui->present == nullptr) {
            return status(SaveResult::canceled, SaveStep::dialog);
        }
        SaveUiRequest request;
        request.kind = SaveUiKind::choose_slot;
        request.storing = false;
        request.slots = &slots;
        request.target = suffix;
        SaveUiResponse response;
        ++t_ui_depth;
        const bool presented = ui->present(ui->ctx, request, response);
        --t_ui_depth;
        if (!presented || response.cancel) return status(SaveResult::canceled, SaveStep::dialog);
        if (response.chosen_slot >= slots.size()) {
            return status(SaveResult::failed_generic, SaveStep::dialog);
        }
        chosen = slots[response.chosen_slot].game_suffix;
        if (!save_slot_suffix_valid(chosen)) {
            return status(SaveResult::failed_generic, SaveStep::dialog);
        }
    }

    std::vector<std::byte> bytes;
    const SaveIoResult read = read_slot(store, options, chosen, kSaveDataName, bytes);
    if (!read.ok()) {
        // The original folds every fopen failure into 0x809f000e / result 4. ENOENT, EACCES and
        // other open failures land here; a short/failed read after a successful open does not.
        if (read.error == SaveIoError::not_found || read.error == SaveIoError::denied ||
            read.error == SaveIoError::io_open) {
            return status(SaveResult::failed_file_not_found, SaveStep::read_file, kSceFileNotFound,
                          read.os_errno);
        }
        std::uint32_t sce = kSceShortIo;
        if (read.error == SaveIoError::busy) sce = kSceBusy;
        if (read.error == SaveIoError::unsupported) sce = 0;
        return status(SaveResult::failed_generic, SaveStep::read_file, sce, read.os_errno);
    }
    return parse_image(bytes, &out);
}

SaveStatus save_store_impl(const SaveStore& store, std::string_view suffix,
                           const ProgressV1& object, const SaveAttachments& attachments,
                           const SaveOptions& options, const SaveUi* ui) {
    if (store.root.empty() || !save_slot_suffix_valid(suffix)) {
        return status(SaveResult::failed_generic, SaveStep::validate);
    }

    std::vector<std::byte> body;
    const ObjectStatus encoded = encode_progress_body(object, body);
    if (!encoded.ok()) return status(encoded.result, SaveStep::encode_body);

    // Advisory attachments are validated before any I/O: a bad text or an oversized icon must not
    // write a partial bundle.
    std::vector<std::byte> meta_block;
    if (attachments.meta != nullptr) {
        const SaveMetaError meta_error = encode_save_meta(*attachments.meta, meta_block);
        if (meta_error != SaveMetaError::none) {
            return status(SaveResult::failed_generic, SaveStep::validate);
        }
    }
    if (attachments.icon != nullptr && !save_icon_ok(*attachments.icon)) {
        return status(SaveResult::failed_generic, SaveStep::validate);
    }

    std::vector<std::byte> image(kSaveStreamHeaderSize + body.size());
    SaveStreamHeader header;
    header.version = kSaveStreamVersion;
    header.body_hash = fnv1a32(body, kFnv1a32OffsetBasis);
    header.body_size = static_cast<std::uint32_t>(body.size());
    header.descriptor_id = kProgressDescriptorId;
    encode_header(image.data(), header);
    std::copy(body.begin(), body.end(), image.begin() + static_cast<std::ptrdiff_t>(kSaveStreamHeaderSize));

    // Sizes first: the overwrite dialog and the free-space policy both need them.
    const std::string backup_name = backup_file_name();
    std::uint64_t data_size = 0;
    std::uint64_t backup_size = 0;
    bool has_data = false;
    bool has_backup = false;
    {
        const SaveIoResult sized = size_of_slot(store, options, suffix, kSaveDataName, data_size);
        if (sized.ok()) {
            has_data = true;
        } else if (sized.error != SaveIoError::not_found) {
            return status(SaveResult::failed_generic, SaveStep::precheck, 0, sized.os_errno);
        }
        const SaveIoResult sized_backup =
            size_of_slot(store, options, suffix, backup_name, backup_size);
        if (sized_backup.ok()) {
            has_backup = true;
        } else if (sized_backup.error != SaveIoError::not_found) {
            return status(SaveResult::failed_generic, SaveStep::precheck, 0, sized_backup.os_errno);
        }
    }

    if (options.use_ui) {
        if (ui == nullptr || ui->present == nullptr) {
            return status(SaveResult::canceled, SaveStep::dialog);
        }
        if (has_data) {
            SaveUiRequest request;
            request.kind = SaveUiKind::confirm_overwrite;
            request.storing = true;
            request.target = suffix;
            SaveUiResponse response;
            ++t_ui_depth;
            const bool presented = ui->present(ui->ctx, request, response);
            --t_ui_depth;
            if (!presented || response.cancel) {
                return status(SaveResult::canceled, SaveStep::dialog);
            }
        }
    }

    if (options.free_space_limit != 0) {
        if (data_size > UINT64_MAX - backup_size) {
            return status(SaveResult::failed_no_space, SaveStep::precheck, kSceNoSpace);
        }
        const std::uint64_t used = data_size + backup_size;
        std::uint64_t needed = image.size();
        if (options.keep_backup && has_data) {
            if (data_size > UINT64_MAX - needed) {
                return status(SaveResult::failed_no_space, SaveStep::precheck, kSceNoSpace);
            }
            needed += data_size;   // the .bak copy also consumes space
        }
        if (used > options.free_space_limit || needed > options.free_space_limit - used) {
            return status(SaveResult::failed_no_space, SaveStep::precheck, kSceNoSpace);
        }
    }

    // Mount analogue: the slot directory must exist before the atomic write. The original's
    // sceSaveDataMount2 creates it after its free-space pre-check; here it is one make_dir call.
    if (store.ops.make_dir == nullptr) {
        return status(SaveResult::failed_generic, SaveStep::mount);
    }
    const std::string slot_dir = save_store_dir(store, suffix);
    const SaveIoResult made = with_retries(options, [&] {
        return store.ops.make_dir(store.ops.ctx, slot_dir);
    });
    if (!made.ok() && made.error != SaveIoError::already_exists) {
        return status(SaveResult::failed_generic, SaveStep::mount, 0, made.os_errno);
    }

    // Capture the previous generation BEFORE the commit. A crash or a failed write therefore
    // leaves the old USR-DATA (and possibly an identical .bak), never nothing. The original
    // deletes the old save first (C14); that hazard is not copied.
    if (options.keep_backup && has_data) {
        std::vector<std::byte> previous;
        const SaveIoResult read = read_slot(store, options, suffix, kSaveDataName, previous);
        if (!read.ok()) {
            return status(SaveResult::failed_generic, SaveStep::backup_capture, 0, read.os_errno);
        }
        const SaveIoResult wrote = write_slot(store, options, suffix, backup_name, previous);
        if (!wrote.ok()) return write_failure(wrote, SaveStep::backup_capture);
    } else if (!options.keep_backup && has_backup) {
        const SaveIoResult removed = remove_slot(store, options, suffix, backup_name);
        if (!removed.ok() && removed.error != SaveIoError::not_found) {
            return write_failure(removed, SaveStep::backup_capture);
        }
    }

    const SaveIoResult wrote = write_slot(store, options, suffix, kSaveDataName, image);
    if (!wrote.ok()) return write_failure(wrote, SaveStep::write_data);

    // The data generation is now committed. Advisory metadata/icon failures keep this true.
    SaveStatus committed = status(SaveResult::success, SaveStep::none);
    committed.data_committed = true;

    if (!meta_block.empty()) {
        const std::vector<std::byte> meta_file = encode_meta_file(
            meta_block, header.body_hash, kProgressDescriptorId);
        if (meta_file.empty()) {
            return status(SaveResult::failed_generic, SaveStep::write_meta);
        }
        const SaveIoResult meta_wrote =
            write_slot(store, options, suffix, kSaveMetaName, meta_file);
        if (!meta_wrote.ok()) {
            SaveStatus failed = write_failure(meta_wrote, SaveStep::write_meta);
            failed.data_committed = true;
            return failed;
        }
    }
    if (attachments.icon != nullptr && !attachments.icon->bytes.empty()) {
        const SaveIoResult icon_wrote = write_slot(store, options, suffix, kSaveIconName,
                                                   attachments.icon->bytes);
        if (!icon_wrote.ok()) {
            SaveStatus failed = write_failure(icon_wrote, SaveStep::write_icon);
            failed.data_committed = true;
            return failed;
        }
    }
    return committed;
}

SaveStatus save_store(const SaveStore& store, std::string_view suffix, const ProgressV1& object,
                      const SaveOptions& options, const SaveUi* ui) {
    return save_store_impl(store, suffix, object, SaveAttachments{}, options, ui);
}

SaveStatus save_store(const SaveStore& store, std::string_view suffix, const ProgressV1& object,
                      const SaveAttachments& attachments, const SaveOptions& options,
                      const SaveUi* ui) {
    return save_store_impl(store, suffix, object, attachments, options, ui);
}

SaveStatus save_check_backup(const SaveStore& store, std::string_view suffix, bool& has_backup) {
    has_backup = false;
    if (store.root.empty() || !save_slot_suffix_valid(suffix)) {
        return status(SaveResult::failed_generic, SaveStep::validate);
    }
    const SaveOptions defaults;
    std::vector<std::byte> bytes;
    const SaveIoResult read =
        read_slot(store, defaults, suffix, backup_file_name(), bytes);
    if (read.error == SaveIoError::not_found) {
        // The contract maps missing *and* invalid backups to has_backup = false, success.
        return status(SaveResult::success, SaveStep::none);
    }
    if (!read.ok()) {
        return status(SaveResult::failed_generic, SaveStep::backup_check,
                      read.error == SaveIoError::busy ? kSceBusy : 0, read.os_errno);
    }
    has_backup = parse_image(bytes, nullptr).ok();
    return status(SaveResult::success, SaveStep::none);
}

SaveStatus save_restore_backup(const SaveStore& store, std::string_view suffix) {
    if (store.root.empty() || !save_slot_suffix_valid(suffix)) {
        return status(SaveResult::failed_generic, SaveStep::validate);
    }
    const SaveOptions defaults;
    std::vector<std::byte> bytes;
    const SaveIoResult read =
        read_slot(store, defaults, suffix, backup_file_name(), bytes);
    if (read.error == SaveIoError::not_found) {
        // "No backup" is internal in the original (0x809f0008); it surfaces as result 4 here
        // because the caller asked for a restore of something that does not exist.
        return status(SaveResult::failed_file_not_found, SaveStep::backup_restore, kSceNoBackup,
                      read.os_errno);
    }
    if (!read.ok()) {
        return status(SaveResult::failed_generic, SaveStep::backup_restore,
                      read.error == SaveIoError::busy ? kSceBusy : 0, read.os_errno);
    }
    if (!parse_image(bytes, nullptr).ok()) {
        // A corrupt backup is never copied over a possibly-good USR-DATA.
        return status(SaveResult::failed_corrupt, SaveStep::backup_restore, kSceCorrupt);
    }
    const SaveIoResult wrote = write_slot(store, defaults, suffix, kSaveDataName, bytes);
    if (!wrote.ok()) return write_failure(wrote, SaveStep::backup_restore);
    return status(SaveResult::success, SaveStep::none);
}

SaveStatus save_delete(const SaveStore& store, std::string_view suffix,
                       const SaveOptions& options) {
    (void)options;
    if (store.root.empty() || !save_slot_suffix_valid(suffix)) {
        return status(SaveResult::failed_generic, SaveStep::validate);
    }
    if (store.ops.file_size == nullptr || store.ops.remove_file == nullptr) {
        return status(SaveResult::failed_generic, SaveStep::delete_slot);
    }
    std::uint64_t size = 0;
    const std::string data_path = save_store_path(store, suffix, kSaveDataName);
    const SaveIoResult sized = store.ops.file_size(store.ops.ctx, data_path, size);
    if (sized.error == SaveIoError::not_found) {
        return status(SaveResult::failed_file_not_found, SaveStep::delete_slot, kSceFileNotFound);
    }
    if (!sized.ok()) {
        return status(SaveResult::failed_generic, SaveStep::delete_slot,
                      sized.error == SaveIoError::busy ? kSceBusy : 0, sized.os_errno);
    }

    // Remove the satellite files first; USR-DATA is the commit point. A BUSY is not retried
    // (0xa4bae0..0xa4bafe), so the first busy maps to failed_generic.
    const std::string_view satellites[] = {
        kSaveMetaName,  // stale metadata is worse than none
        kSaveIconName,
    };
    for (const std::string_view name : satellites) {
        const std::string path = save_store_path(store, suffix, name);
        const SaveIoResult removed = store.ops.remove_file(store.ops.ctx, path);
        if (!removed.ok() && removed.error != SaveIoError::not_found) {
            return status(SaveResult::failed_generic, SaveStep::delete_slot,
                          removed.error == SaveIoError::busy ? kSceBusy : 0, removed.os_errno);
        }
    }
    {
        std::string backup_path = data_path;
        backup_path.append(kSaveBackupSuffix);
        const SaveIoResult removed = store.ops.remove_file(store.ops.ctx, backup_path);
        if (!removed.ok() && removed.error != SaveIoError::not_found) {
            return status(SaveResult::failed_generic, SaveStep::delete_slot,
                          removed.error == SaveIoError::busy ? kSceBusy : 0, removed.os_errno);
        }
    }
    {
        std::string meta_backup_path = save_store_path(store, suffix, kSaveMetaName);
        meta_backup_path.append(kSaveBackupSuffix);
        const SaveIoResult removed = store.ops.remove_file(store.ops.ctx, meta_backup_path);
        if (!removed.ok() && removed.error != SaveIoError::not_found) {
            return status(SaveResult::failed_generic, SaveStep::delete_slot,
                          removed.error == SaveIoError::busy ? kSceBusy : 0, removed.os_errno);
        }
    }
    const SaveIoResult removed = store.ops.remove_file(store.ops.ctx, data_path);
    if (!removed.ok()) {
        SaveStatus failed = status(SaveResult::failed_generic, SaveStep::delete_slot,
                                   removed.error == SaveIoError::busy ? kSceBusy : 0,
                                   removed.os_errno);
        return failed;
    }
    return status(SaveResult::success, SaveStep::none);
}

SaveResult map_sce_result(std::uint32_t raw) noexcept {
    switch (raw) {
        case 0x809F0004u: return SaveResult::failed_mount;
        case 0x809F000Au: return SaveResult::failed_no_space;
        case 0x809F000Eu: return SaveResult::failed_file_not_found;
        case 0x809F000Fu: return SaveResult::failed_corrupt;
        default: return SaveResult::failed_generic;
    }
}

SaveDialogSignal classify_dialog_status(std::uint32_t raw) noexcept {
    switch (raw) {
        case 0: return SaveDialogSignal::poll_again;
        case 2: return SaveDialogSignal::close_progress;
        case 3: return SaveDialogSignal::get_result;
        case 0x80B80008u: return SaveDialogSignal::initialize_busy;
        default: return SaveDialogSignal::poll_again;
    }
}

bool save_ui_active(const SaveStore& store) noexcept {
    (void)store;
    return t_ui_depth != 0;
}

}  // namespace sotc
