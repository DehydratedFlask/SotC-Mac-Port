#include "core/save_slot_store.hpp"

#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "core/save_stream.hpp"

namespace sotc {
namespace {

// Host-safety ceiling for a whole USR-DATA file. A valid container is 16 + body bytes with
// body <= 0x6400000, so this only rejects files far larger than any save. The original would
// fread exactly the declared body and ignore trailing bytes; this cap is a native policy.
constexpr std::uint64_t kMaxReadBytes =
    static_cast<std::uint64_t>(kSaveStreamHeaderSize) + kSaveStreamMaxBodySize;

std::string join_path(std::string_view root, std::string_view rest) {
    std::string out(root);
    while (out.size() > 1 && out.back() == '/') out.pop_back();
    if (out.empty()) return std::string(rest);
    if (out.back() != '/') out.push_back('/');
    out.append(rest);
    return out;
}

// ---- POSIX ------------------------------------------------------------------------------------
SaveIoResult posix_read(void*, std::string_view path, std::vector<std::byte>& out) {
    const std::string name(path);
    int fd;
    do { fd = ::open(name.c_str(), O_RDONLY); } while (fd < 0 && errno == EINTR);
    if (fd < 0) {
        const int err = errno;
        if (err == ENOENT || err == ENOTDIR) return {SaveIoError::not_found, err};
        if (err == EACCES) return {SaveIoError::denied, err};
        return {SaveIoError::io_open, err};
    }
    struct stat st {};
    if (::fstat(fd, &st) != 0) {
        const int err = errno;
        ::close(fd);
        return {SaveIoError::io_read, err};
    }
    if (!S_ISREG(st.st_mode)) {
        ::close(fd);
        return {SaveIoError::io_read, EISDIR};
    }
    if (st.st_size < 0 || static_cast<std::uint64_t>(st.st_size) > kMaxReadBytes) {
        ::close(fd);
        return {SaveIoError::io_read, EFBIG};
    }
    std::vector<std::byte> bytes(static_cast<std::size_t>(st.st_size));
    std::size_t done = 0;
    while (done != bytes.size()) {
        const ssize_t n = ::read(fd, bytes.data() + done, bytes.size() - done);
        if (n < 0) {
            if (errno == EINTR) continue;
            const int err = errno;
            ::close(fd);
            return {SaveIoError::io_read, err};
        }
        if (n == 0) break;   // shrank underneath us; the transaction reports a short body
        done += static_cast<std::size_t>(n);
    }
    ::close(fd);
    bytes.resize(done);
    out = std::move(bytes);
    return {};
}

SaveIoResult posix_write_atomic(void*, std::string_view path,
                                std::span<const std::byte> bytes) {
    const SaveStreamIoResult result = write_file_atomic(std::string(path), bytes);
    switch (result.error) {
        case SaveStreamError::none: return {};
        case SaveStreamError::io_open:
            return {result.os_errno == EACCES ? SaveIoError::denied : SaveIoError::io_open,
                    result.os_errno};
        case SaveStreamError::io_write:
            return {result.os_errno == ENOSPC ? SaveIoError::no_space : SaveIoError::io_write,
                    result.os_errno};
        case SaveStreamError::io_fsync: return {SaveIoError::io_fsync, result.os_errno};
        case SaveStreamError::io_rename: return {SaveIoError::io_rename, result.os_errno};
        default: return {SaveIoError::io_write, result.os_errno};
    }
}

SaveIoResult posix_remove(void*, std::string_view path) {
    if (::unlink(std::string(path).c_str()) != 0) {
        const int err = errno;
        if (err == ENOENT) return {SaveIoError::not_found, err};
        if (err == EACCES) return {SaveIoError::denied, err};
        return {SaveIoError::io_write, err};
    }
    return {};
}

SaveIoResult posix_mkdir(void*, std::string_view path) {
    const std::string name(path);
    if (::mkdir(name.c_str(), 0700) != 0) {
        const int err = errno;
        if (err == EEXIST) {
            struct stat st {};
            if (::stat(name.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
                return {SaveIoError::already_exists, err};
            }
            return {SaveIoError::io_mkdir, ENOTDIR};
        }
        if (err == EACCES) return {SaveIoError::denied, err};
        if (err == ENOSPC) return {SaveIoError::no_space, err};
        return {SaveIoError::io_mkdir, err};
    }
    return {};
}

SaveIoResult posix_list(void*, std::string_view path, std::vector<std::string>& names) {
    const std::string name(path);
    DIR* dir = ::opendir(name.c_str());
    if (dir == nullptr) {
        const int err = errno;
        if (err == ENOENT) return {SaveIoError::not_found, err};
        if (err == EACCES) return {SaveIoError::denied, err};
        return {SaveIoError::io_dir_read, err};
    }
    names.clear();
    while (const dirent* entry = ::readdir(dir)) {
        const std::string child = entry->d_name;
        if (child == "." || child == "..") continue;
        names.push_back(child);
    }
    ::closedir(dir);
    return {};
}

SaveIoResult posix_file_size(void*, std::string_view path, std::uint64_t& size) {
    struct stat st {};
    if (::stat(std::string(path).c_str(), &st) != 0) {
        const int err = errno;
        if (err == ENOENT) return {SaveIoError::not_found, err};
        if (err == EACCES) return {SaveIoError::denied, err};
        return {SaveIoError::io_read, err};
    }
    if (!S_ISREG(st.st_mode)) return {SaveIoError::io_read, EISDIR};
    size = static_cast<std::uint64_t>(st.st_size);
    return {};
}

// ---- in-memory --------------------------------------------------------------------------------
SaveIoResult memory_read(void* ctx, std::string_view path, std::vector<std::byte>& out) {
    auto& fs = *static_cast<SaveMemoryFs*>(ctx);
    const unsigned call = ++fs.read_calls;
    if (fs.read_error != SaveIoError::none) {
        return {fs.read_error, fs.read_error == SaveIoError::not_found ? ENOENT : 0};
    }
    if (call <= fs.busy_reads) return {SaveIoError::busy, 0};
    const auto it = fs.files.find(path);
    if (it == fs.files.end()) return {SaveIoError::not_found, ENOENT};
    out = it->second;
    return {};
}

SaveIoResult memory_write_atomic(void* ctx, std::string_view path,
                                 std::span<const std::byte> bytes) {
    auto& fs = *static_cast<SaveMemoryFs*>(ctx);
    const unsigned call = ++fs.write_calls;
    if (fs.write_error != SaveIoError::none) return {fs.write_error, 0};
    const std::string final_path(path);
    const std::string temp = final_path + ".tmp";
    if (fs.partial_write_on_call == call) {
        const std::size_t keep = std::min(fs.partial_write_bytes, bytes.size());
        fs.files[temp].assign(bytes.begin(), bytes.begin() + static_cast<std::ptrdiff_t>(keep));
        fs.files.erase(temp);   // the real temp writer unlinks its temp on failure
        return {SaveIoError::io_write, 0};
    }
    if (fs.fail_write_on_call == call) return {SaveIoError::io_write, 0};
    if (fs.rename_error != SaveIoError::none) return {fs.rename_error, 0};
    if (fs.fail_rename_on_call == call) return {SaveIoError::io_rename, 0};
    // Same ordering as the POSIX helper: the destination name is only touched by the final move.
    auto& slot = fs.files[temp];
    slot.assign(bytes.begin(), bytes.end());
    fs.files[final_path] = std::move(slot);
    fs.files.erase(temp);
    return {};
}

SaveIoResult memory_remove(void* ctx, std::string_view path) {
    auto& fs = *static_cast<SaveMemoryFs*>(ctx);
    ++fs.remove_calls;
    if (fs.remove_error != SaveIoError::none) return {fs.remove_error, 0};
    if (fs.fail_remove_on_call != 0 && fs.remove_calls == fs.fail_remove_on_call) {
        return {SaveIoError::io_write, 0};
    }
    if (fs.files.erase(std::string(path)) == 0) return {SaveIoError::not_found, ENOENT};
    return {};
}

SaveIoResult memory_mkdir(void* ctx, std::string_view path) {
    auto& fs = *static_cast<SaveMemoryFs*>(ctx);
    const auto [it, inserted] = fs.dirs.insert(std::string(path));
    (void)it;
    return inserted ? SaveIoResult{} : SaveIoResult{SaveIoError::already_exists, EEXIST};
}

SaveIoResult memory_list(void* ctx, std::string_view path, std::vector<std::string>& names) {
    auto& fs = *static_cast<SaveMemoryFs*>(ctx);
    ++fs.list_calls;
    if (fs.list_error != SaveIoError::none) return {fs.list_error, 0};
    const std::string dir(path);
    const std::string prefix = dir + "/";
    std::set<std::string> children;
    for (auto it = fs.files.lower_bound(prefix); it != fs.files.end(); ++it) {
        const std::string_view key = it->first;
        if (!key.starts_with(prefix)) break;
        const std::string_view rest = key.substr(prefix.size());
        children.insert(std::string(rest.substr(0, rest.find('/'))));
    }
    for (const std::string& known : fs.dirs) {
        const std::string_view key = known;
        if (!key.starts_with(prefix)) continue;
        const std::string_view rest = key.substr(prefix.size());
        children.insert(std::string(rest.substr(0, rest.find('/'))));
    }
    if (children.empty() && fs.dirs.find(dir) == fs.dirs.end()) {
        return {SaveIoError::not_found, ENOENT};
    }
    names.assign(children.begin(), children.end());
    return {};
}

SaveIoResult memory_file_size(void* ctx, std::string_view path, std::uint64_t& size) {
    auto& fs = *static_cast<SaveMemoryFs*>(ctx);
    const auto it = fs.files.find(path);
    if (it == fs.files.end()) return {SaveIoError::not_found, ENOENT};
    size = it->second.size();
    return {};
}

}  // namespace

const char* to_string(SaveIoError error) noexcept {
    switch (error) {
        case SaveIoError::none: return "none";
        case SaveIoError::not_found: return "not_found";
        case SaveIoError::already_exists: return "already_exists";
        case SaveIoError::no_space: return "no_space";
        case SaveIoError::denied: return "denied";
        case SaveIoError::busy: return "busy";
        case SaveIoError::io_open: return "io_open";
        case SaveIoError::io_read: return "io_read";
        case SaveIoError::io_write: return "io_write";
        case SaveIoError::io_fsync: return "io_fsync";
        case SaveIoError::io_rename: return "io_rename";
        case SaveIoError::io_mkdir: return "io_mkdir";
        case SaveIoError::io_dir_read: return "io_dir_read";
        case SaveIoError::unsupported: return "unsupported";
    }
    return "unknown";
}

SaveIoOps save_io_posix() noexcept {
    SaveIoOps ops;
    ops.read_file = &posix_read;
    ops.write_file_atomic = &posix_write_atomic;
    ops.remove_file = &posix_remove;
    ops.make_dir = &posix_mkdir;
    ops.list_dir = &posix_list;
    ops.file_size = &posix_file_size;
    return ops;
}

SaveIoOps save_io_memory(SaveMemoryFs& fs) noexcept {
    SaveIoOps ops;
    ops.ctx = &fs;
    ops.read_file = &memory_read;
    ops.write_file_atomic = &memory_write_atomic;
    ops.remove_file = &memory_remove;
    ops.make_dir = &memory_mkdir;
    ops.list_dir = &memory_list;
    ops.file_size = &memory_file_size;
    return ops;
}

SaveIoOps save_io_none() noexcept { return SaveIoOps{}; }

const std::vector<std::byte>* SaveMemoryFs::find(std::string_view path) const noexcept {
    const auto it = files.find(path);
    return it == files.end() ? nullptr : &it->second;
}

void SaveMemoryFs::put(std::string path, std::span<const std::byte> bytes) {
    files[std::move(path)].assign(bytes.begin(), bytes.end());
}

std::string save_slot_dir(std::string_view suffix) {
    std::string out(kSaveDirPrefix);
    out.append(suffix);
    return out;
}

std::string save_slot_path(std::string_view suffix, std::string_view file) {
    std::string out = save_slot_dir(suffix);
    out.push_back('/');
    out.append(file);
    return out;
}

std::string save_store_path(const SaveStore& store, std::string_view suffix,
                            std::string_view file) {
    return join_path(store.root, save_slot_path(suffix, file));
}

std::string save_store_dir(const SaveStore& store, std::string_view suffix) {
    return join_path(store.root, save_slot_dir(suffix));
}

bool save_slot_suffix_valid(std::string_view suffix) noexcept {
    if (suffix.empty() || suffix.size() > kSaveMaxSuffixBytes) return false;
    for (const char c : suffix) {
        const unsigned char b = static_cast<unsigned char>(c);
        const bool ok = (b >= '0' && b <= '9') || (b >= 'A' && b <= 'Z') ||
                        (b >= 'a' && b <= 'z') || b == '_' || b == '-';
        if (!ok) return false;
    }
    return true;
}

SaveIoResult save_store_init(const SaveStore& store) {
    if (store.root.empty()) return {SaveIoError::unsupported, 0};
    if (store.ops.make_dir == nullptr) return {SaveIoError::unsupported, 0};
    std::string path;
    std::size_t index = 0;
    if (store.root.front() == '/') {
        path = "/";
        index = 1;
    }
    while (index <= store.root.size()) {
        std::size_t slash = store.root.find('/', index);
        if (slash == std::string::npos) slash = store.root.size();
        const std::string_view part(store.root.data() + index, slash - index);
        if (!part.empty()) {
            path = join_path(path, part);
            const SaveIoResult result = store.ops.make_dir(store.ops.ctx, path);
            if (!result.ok() && result.error != SaveIoError::already_exists) return result;
        }
        if (slash == store.root.size()) break;
        index = slash + 1;
    }
    return {};
}

}  // namespace sotc
