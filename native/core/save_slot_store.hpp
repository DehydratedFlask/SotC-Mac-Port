// Host file-system seam + slot identity for the native save transaction.
//
// Layout under SaveStore::root (the only files this slice reads or writes):
//   SAVEDATA0<suffix>/USR-DATA        exact container the game's loader opens ("%s/USR-DATA")
//   SAVEDATA0<suffix>/USR-DATA.bak    previous committed generation (UmountWithBackup analogue)
//   SAVEDATA0<suffix>/USR-META        native metadata wrapper (E604); the guest never opens it
//   SAVEDATA0<suffix>/icon0.png       advisory SaveIcon bytes
//
// The game never opens a metadata or icon file through USR-DATA; those are separate platform calls
// (SetParam / SaveIcon) and remain host-supplied. Nothing here is a claim that the game can read a
// native file: the container is synthetic-fixture-verified only.
//
// Paths handed to SaveIoOps are already joined with SaveStore::root and use '/'. Every operation
// pointer may be null; the transaction reports `unsupported` instead of crashing. A SaveIoOps is a
// plain function-pointer table on purpose: the partial-write / rename-failure / BUSY fixtures need
// deterministic injection without virtuals, exceptions or test-only #ifdefs.
#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace sotc {

inline constexpr std::string_view kSaveDirPrefix = "SAVEDATA0";
inline constexpr std::string_view kSaveDataName = "USR-DATA";
inline constexpr std::string_view kSaveBackupSuffix = ".bak";
inline constexpr std::string_view kSaveMetaName = "USR-META";
inline constexpr std::string_view kSaveIconName = "icon0.png";
inline constexpr std::size_t kSaveMaxSuffixBytes = 24;

enum class SaveIoError : std::uint8_t {
    none,
    not_found,        // ENOENT
    already_exists,   // mkdir on an existing directory
    no_space,         // ENOSPC during an atomic write
    denied,           // EACCES
    busy,             // platform BUSY analogue (0x809f0013); retried, then failed_generic
    io_open,
    io_read,
    io_write,
    io_fsync,
    io_rename,
    io_mkdir,
    io_dir_read,
    unsupported,      // the operation pointer is null or the root is unusable
};
[[nodiscard]] const char* to_string(SaveIoError error) noexcept;

struct SaveIoResult {
    SaveIoError error = SaveIoError::none;
    int os_errno = 0;
    [[nodiscard]] bool ok() const noexcept { return error == SaveIoError::none; }
};

struct SaveIoOps {
    void* ctx = nullptr;
    SaveIoResult (*read_file)(void*, std::string_view, std::vector<std::byte>& out) = nullptr;
    SaveIoResult (*write_file_atomic)(void*, std::string_view, std::span<const std::byte>) = nullptr;
    SaveIoResult (*remove_file)(void*, std::string_view) = nullptr;
    SaveIoResult (*make_dir)(void*, std::string_view) = nullptr;
    SaveIoResult (*list_dir)(void*, std::string_view, std::vector<std::string>& names) = nullptr;
    SaveIoResult (*file_size)(void*, std::string_view, std::uint64_t& size) = nullptr;
};

// POSIX implementation. read_file rejects a file larger than 16 + 0x6400000 bytes from fstat
// before allocating; write_file_atomic forwards to sotc::write_file_atomic (unique same-directory
// temp, fsync, rename, directory fsync). ctx is unused. No operation sleeps.
[[nodiscard]] SaveIoOps save_io_posix() noexcept;

struct SaveStore {
    std::string root;   // absolute or relative; save_store_init creates it with 0700
    SaveIoOps ops{};
};

// Creates root and every missing parent through ops.make_dir. An existing directory is success.
[[nodiscard]] SaveIoResult save_store_init(const SaveStore& store);

// "SAVEDATA0" + suffix, the directory name the game's format string would produce.
[[nodiscard]] std::string save_slot_dir(std::string_view suffix);
// "SAVEDATA0" + suffix + "/" + file.
[[nodiscard]] std::string save_slot_path(std::string_view suffix, std::string_view file);
// SaveStore::root + '/' + save_slot_path. Trailing slashes on root are collapsed.
[[nodiscard]] std::string save_store_path(const SaveStore& store, std::string_view suffix,
                                          std::string_view file);
// SaveStore::root + '/' + save_slot_dir(suffix).
[[nodiscard]] std::string save_store_dir(const SaveStore& store, std::string_view suffix);

// 1..kSaveMaxSuffixBytes bytes from [0-9A-Za-z_-]. Deliberately excludes '/', '.', "..", NUL and
// any byte that would escape the slot directory. The draft's claim that 24 bytes "leaves room in a
// 32-byte PS4 dir name" is NOT verified; the limit is a native policy, not a platform fact.
[[nodiscard]] bool save_slot_suffix_valid(std::string_view suffix) noexcept;

// ---- deterministic in-memory implementation (fixture + failure injection) --------------------
// The map is the whole persistent state. write_file_atomic emulates the POSIX contract: it stores
// the bytes under a temp name and only then moves them onto the final name, so an injected partial
// write / rename failure never modifies the final file and never leaves a temp behind.
struct SaveMemoryFs {
    std::map<std::string, std::vector<std::byte>, std::less<>> files;
    std::set<std::string, std::less<>> dirs;

    // Failure injection. Call counters are 1-based and count every operation, so a test can fail
    // the second write (the data write) while letting the first (the backup copy) succeed.
    SaveIoError read_error = SaveIoError::none;      // every read
    unsigned busy_reads = 0;                          // first N reads return busy
    SaveIoError write_error = SaveIoError::none;      // every atomic write
    unsigned fail_write_on_call = 0;                  // this 1-based call returns io_write
    std::size_t partial_write_bytes = 0;              // >0: store N bytes, remove the temp, fail
    unsigned partial_write_on_call = 0;               // on this 1-based call
    SaveIoError rename_error = SaveIoError::none;     // every atomic write's rename step
    unsigned fail_rename_on_call = 0;                 // on this 1-based call
    SaveIoError remove_error = SaveIoError::none;
    unsigned fail_remove_on_call = 0;                 // this 1-based call returns io_write
    SaveIoError list_error = SaveIoError::none;

    unsigned read_calls = 0;
    unsigned write_calls = 0;
    unsigned remove_calls = 0;
    unsigned list_calls = 0;

    [[nodiscard]] const std::vector<std::byte>* find(std::string_view path) const noexcept;
    [[nodiscard]] bool has(std::string_view path) const noexcept { return find(path) != nullptr; }
    void put(std::string path, std::span<const std::byte> bytes);
};
[[nodiscard]] SaveIoOps save_io_memory(SaveMemoryFs& fs) noexcept;

// A SaveIoOps with every pointer null (the "missing hook" fixture).
[[nodiscard]] SaveIoOps save_io_none() noexcept;

}  // namespace sotc
