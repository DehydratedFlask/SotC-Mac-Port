// One synchronous save, load, backup check, backup restore or slot enumeration for one progress
// slot. Mirrors FUN_00a4ab90 / FUN_00a48280 / FUN_00a4a940 / FUN_00a47d60 at the *observable*
// level: the ordered platform-call effects and the final result. The original is a 16 ms-tick
// worker state machine driven by FUN_00a4d330; tick scheduling is not observable and is not
// modelled (the report says so explicitly in §1.2), so this API is synchronous.
//
// Deliberate divergences from the original, all required by the contract analysis:
//   * no unbounded BUSY retry loop: every operation is retried at most max_busy_retries times,
//     then the transaction reports failed_generic (C13);
//   * no delete-before-write: the previous generation is copied to USR-DATA.bak and the new file
//     replaces USR-DATA through a temp + rename, so a failed save leaves the old file readable
//     (C14);
//   * the mount pre-check counts the backup copy as well and the requested size is the real image
//     size, not the original's fixed 3 MiB block request (C12).
//
// Result ordinals are the game's own 1-based values (core/save_progress_object.hpp). A missing
// operation, a refused write or an unsupported path is always a visible non-success; this layer
// never turns "could not do it" into success.
//
// NOT MODELLED (contracts left to a later slice or the host application):
//   * the dialog parameter builder FUN_00a4a600, the nine sceSaveDataDialog* thunks and the
//     10/20/40/50/70/100 progress milestones: only slot choice and overwrite confirmation are
//     surfaced through SaveUi; SaveUiKind::corrupt_offer_restore and progress_percent are never
//     produced here;
//   * sceSaveDataDirNameSearch: save_enumerate reads the native store instead;
//   * the platform dialog milestones and the corrupt-offer-restore flow;
//   * the 15 576-byte blob's field semantics.
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "core/save_meta.hpp"
#include "core/save_progress_object.hpp"
#include "core/save_slot_store.hpp"

namespace sotc {

enum class SaveStep : std::uint8_t {
    none,
    validate,          // suffix / argument check
    enumerate,         // slot list for the UI or save_enumerate
    dialog,            // SaveUi::present returned cancel / absent UI
    mount,             // slot-directory creation (sceSaveDataMount2 analogue)
    precheck,          // free-space policy before any write
    read_file,         // read USR-DATA / USR-DATA.bak
    read_header,       // 16-byte outer header checks (version, size limit)
    read_body,         // declared body length vs bytes available
    verify_hash,       // FNV-1a-32 over the body
    verify_descriptor, // outer dword3 == 4
    parse_body,        // chunk walk / object rules
    encode_body,       // ProgressV1 -> body
    backup_capture,    // copy the pre-commit USR-DATA to USR-DATA.bak
    write_data,        // atomic replace of USR-DATA
    backup_check,      // validate .bak
    backup_restore,    // copy .bak over USR-DATA
    write_meta,        // advisory USR-META write after the data commit
    write_icon,        // advisory icon0.png write after the data commit
    delete_slot,       // USR-DATA removal (the commit point of save_delete)
};
[[nodiscard]] const char* to_string(SaveStep step) noexcept;

struct SaveStatus {
    SaveResult result = SaveResult::success;
    SaveStep step = SaveStep::none;   // step that produced `result`
    std::uint32_t sce_status = 0;     // observed 0x809fxxxx for that step, else 0
    int os_errno = 0;                 // host errno when an I/O operation failed
    // True when the new USR-DATA generation is present and loadable, even if an advisory
    // metadata/icon write failed afterwards.
    bool data_committed = false;
    [[nodiscard]] bool ok() const noexcept { return result == SaveResult::success; }
};

struct SaveOptions {
    // Ask the supplied SaveUi for a slot (load) or an overwrite confirmation (store). If use_ui is
    // set and no usable SaveUi is supplied, the transaction reports canceled (2): it does not
    // silently pick a slot.
    bool use_ui = false;
    // Keep the pre-commit generation as USR-DATA.bak. false removes a stale backup.
    bool keep_backup = true;
    // Native free-space policy in bytes: used + new image + backup copy must fit. 0 disables the
    // check. Default is the original's 0xa00000 ceiling.
    std::uint64_t free_space_limit = 0xa00000;
    // BUSY is retried at most this many times (0 = one attempt), then failed_generic. Never
    // unbounded. The original's 16 ms tick / sleep is not modelled.
    unsigned max_busy_retries = 5;
};

struct SaveSlotInfo {
    std::string dir_name;      // exact directory name under root, e.g. "SAVEDATA00"
    std::string game_suffix;   // the "%s" in "SAVEDATA0%s", e.g. "0"
    std::uint64_t committed_bytes = 0;   // USR-DATA file size
    std::uint64_t body_size = 0;         // outer header dword2, 0 when the file is too short
    std::uint32_t body_hash = 0;         // outer header dword1, 0 when the file is too short
    bool has_backup = false;             // USR-DATA.bak exists and validates
    SaveMeta meta{};                     // decoded USR-META block; default when absent
    bool has_meta = false;               // USR-META present and well formed
    bool meta_matches_data = false;      // stored data_hash == the USR-DATA header hash
};

// ---- dialog hook ----------------------------------------------------------------------------
// Replaces FUN_00a4a600 and the Dialog* thunks. The transaction calls it on the calling thread;
// save_ui_active() is true only while the call is being serviced.
enum class SaveUiKind : std::uint8_t {
    choose_slot,             // load: slots is non-null; chosen_slot must be < slots->size()
    confirm_overwrite,       // store: target names the slot that already has USR-DATA
    corrupt_offer_restore,   // NOT produced in this slice; reserved for the corrupt-data flow
};
[[nodiscard]] const char* to_string(SaveUiKind kind) noexcept;

struct SaveUiRequest {
    SaveUiKind kind = SaveUiKind::choose_slot;
    bool storing = false;                 // true = save, false = load
    const std::vector<SaveSlotInfo>* slots = nullptr;   // choose_slot only
    std::string_view target;              // slot being confirmed / described
    int progress_percent = 0;             // always 0 in this slice (milestones are a contract)
};

struct SaveUiResponse {
    bool cancel = false;
    std::size_t chosen_slot = 0;
    bool restore_backup = false;          // consumed only by the corrupt_offer_restore flow
};

struct SaveUi {
    void* ctx = nullptr;
    // Return false to mean "no UI available"; the transaction then reports canceled (2).
    bool (*present)(void*, const SaveUiRequest&, SaveUiResponse&) = nullptr;
};

// ---- transaction ----------------------------------------------------------------------------
// Lists SAVEDATA0<suffix> directories that contain a USR-DATA file, sorted by dir_name.
// A missing root is success with an empty list. It does NOT fail on a corrupt slot: the slot is
// listed with the header fields it has, so a UI can show it and load can report the corruption.
[[nodiscard]] SaveStatus save_enumerate(const SaveStore& store, std::vector<SaveSlotInfo>& out);

// Read + validate + decode. Order: suffix -> (optional UI) -> read -> short header -> version ->
// size ceiling -> short body -> hash -> descriptor 4 -> chunk/ObjectCheck. Trailing bytes after
// header+body are ignored (the original freads exactly body_size).
[[nodiscard]] SaveStatus save_load(const SaveStore& store, std::string_view suffix, ProgressV1& out,
                                   const SaveOptions& options = {}, const SaveUi* ui = nullptr);

// Encode -> outer header {1, FNV(body), size, 4} -> (optional overwrite UI) -> free-space
// pre-check -> create the slot directory -> capture USR-DATA as .bak -> atomic replace. The
// previous generation is untouched until the final rename, and never deleted before the write.
[[nodiscard]] SaveStatus save_store(const SaveStore& store, std::string_view suffix,
                                    const ProgressV1& object, const SaveOptions& options = {},
                                    const SaveUi* ui = nullptr);

// Optional advisory files written after the data commit. A failure is reported (failed_generic +
// step) but `data_committed` stays true: the save itself is not lost.
struct SaveAttachments {
    const SaveMeta* meta = nullptr;  // nullptr = write no metadata
    const SaveIcon* icon = nullptr;  // nullptr or empty bytes = write no icon
};

[[nodiscard]] SaveStatus save_store(const SaveStore& store, std::string_view suffix,
                                    const ProgressV1& object, const SaveAttachments& attachments,
                                    const SaveOptions& options = {}, const SaveUi* ui = nullptr);

// Removes one slot: USR-DATA.bak, USR-META/USR-META.bak, icon0.png, then USR-DATA (the commit).
// No BUSY retry. Missing slot -> failed_file_not_found (4) with 0x809f000e.
[[nodiscard]] SaveStatus save_delete(const SaveStore& store, std::string_view suffix,
                                     const SaveOptions& options = {});

// Byte-pinned observers: the raw sce result table (0x12c3370) and the dialog status classes.
[[nodiscard]] SaveResult map_sce_result(std::uint32_t raw) noexcept;

enum class SaveDialogSignal : std::uint32_t {
    poll_again = 0,
    close_progress = 2,
    get_result = 3,
    initialize_busy = 0x80b80008u,
};
[[nodiscard]] SaveDialogSignal classify_dialog_status(std::uint32_t raw) noexcept;

// Missing or invalid .bak -> success with has_backup = false. A missing op or an I/O failure is a
// visible non-success (failed_generic / backup_check).
[[nodiscard]] SaveStatus save_check_backup(const SaveStore& store, std::string_view suffix,
                                           bool& has_backup);

// Validate .bak, then atomically copy it over USR-DATA. No backup:
// failed_file_not_found (4) with sce_status 0x809f0008. A corrupt backup is not restored:
// failed_corrupt (6) with 0x809f000f.
[[nodiscard]] SaveStatus save_restore_backup(const SaveStore& store, std::string_view suffix);

// FUN_00a4d400 analogue: true while a SaveUi::present call is being serviced on this thread.
// The game's only use is to poll whether its dialog is up; there is no asynchronous dialog owner.
[[nodiscard]] bool save_ui_active(const SaveStore& store) noexcept;

}  // namespace sotc
