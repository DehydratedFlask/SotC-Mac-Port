// File helpers for the save stream container: atomic write (temp + fsync + rename) and read with
// error enums. Fixture-level only; no game save is read or written. Power-loss durability is NOT
// tested (see docs/VALIDATION.md G2 [B06]/[B07]).
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include "core/save_stream.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)
#define CHECK_ERR(got, want) do { const auto g_ = (got); if (g_ != (want)) { std::fprintf(stderr, "FAIL %s:%d %s: got %s want %s\n", __FILE__, __LINE__, #got, sotc::to_string(g_), sotc::to_string(want)); std::exit(1); } } while (0)

using sotc::SaveStreamError;
using Bytes = std::vector<std::byte>;

namespace {

std::string g_dir;

Bytes B(std::initializer_list<unsigned> v) {
    Bytes out;
    for (const unsigned x : v) out.push_back(static_cast<std::byte>(x));
    return out;
}

Bytes make_stream(unsigned seed) {
    sotc::SaveStreamWriter w(0xd);
    CHECK_ERR(w.append_chunk("alpha", 4, B({seed, 2, 3})), SaveStreamError::none);
    CHECK_ERR(w.append_chunk("beta", 13, B({seed + 1, 0xef, 0xbe})), SaveStreamError::none);
    return w.serialize();
}

Bytes slurp(const std::string& path) {
    Bytes out;
    FILE* f = std::fopen(path.c_str(), "rb");
    CHECK(f != nullptr);
    std::byte buf[4096];
    std::size_t n;
    while ((n = std::fread(buf, 1, sizeof buf, f)) != 0) out.insert(out.end(), buf, buf + n);
    std::fclose(f);
    return out;
}

void spit(const std::string& path, const Bytes& bytes) {
    FILE* f = std::fopen(path.c_str(), "wb");
    CHECK(f != nullptr);
    CHECK(std::fwrite(bytes.data(), 1, bytes.size(), f) == bytes.size());
    std::fclose(f);
}

bool exists(const std::string& path) {
    struct stat st;
    return ::stat(path.c_str(), &st) == 0;
}

std::size_t dir_entries(const std::string& dir) {
    std::size_t n = 0;
    DIR* d = ::opendir(dir.c_str());
    CHECK(d != nullptr);
    while (const dirent* e = ::readdir(d)) {
        const std::string name = e->d_name;
        if (name != "." && name != "..") ++n;
    }
    ::closedir(d);
    return n;
}

void test_write_read_round_trip() {
    const std::string path = g_dir + "/slot0.sav";
    const Bytes img = make_stream(1);
    const auto w = sotc::write_file_atomic(path, img);
    CHECK_ERR(w.error, SaveStreamError::none);
    CHECK(slurp(path) == img);
    CHECK(!exists(path + ".tmp"));                       // temp renamed away
    struct stat st;
    CHECK(::stat(path.c_str(), &st) == 0 && (st.st_mode & 0777) == 0644);
    const auto r = sotc::read_save_stream_file(path);
    CHECK_ERR(r.error, SaveStreamError::none);
    CHECK(r.stream.chunks.size() == 2 && r.stream.header.descriptor_id == 0xd);
    CHECK(r.stream.chunks[0].payload == B({1, 2, 3}));
}

void test_overwrite_replaces() {
    const std::string path = g_dir + "/slot1.sav";
    const Bytes a = make_stream(10), b = make_stream(20);
    CHECK_ERR(sotc::write_file_atomic(path, a).error, SaveStreamError::none);
    CHECK_ERR(sotc::write_file_atomic(path, b).error, SaveStreamError::none);
    CHECK(slurp(path) == b && a != b);
    CHECK(!exists(path + ".tmp"));
    // A stale temp from an earlier crash is overwritten, not an error.
    spit(path + ".tmp", B({9, 9, 9}));
    CHECK_ERR(sotc::write_file_atomic(path, a).error, SaveStreamError::none);
    CHECK(slurp(path) == a && !exists(path + ".tmp"));
}

void test_empty_and_zero_length() {
    const std::string path = g_dir + "/empty.bin";
    CHECK_ERR(sotc::write_file_atomic(path, {}).error, SaveStreamError::none);
    CHECK(slurp(path).empty());
    CHECK_ERR(sotc::read_save_stream_file(path).error, SaveStreamError::truncated_header);
}

void test_missing_and_bad_targets() {
    CHECK_ERR(sotc::read_save_stream_file(g_dir + "/nope.sav").error, SaveStreamError::file_not_found);
    const auto missing = sotc::read_save_stream_file(g_dir + "/nope.sav");
    CHECK(missing.os_errno == ENOENT);
    // Reading a directory is an io error, not a crash.
    CHECK_ERR(sotc::read_save_stream_file(g_dir).error, SaveStreamError::io_read);
    // Writing into a missing directory fails at open and leaves nothing behind.
    const auto w = sotc::write_file_atomic(g_dir + "/no_such_dir/x.sav", make_stream(1));
    CHECK_ERR(w.error, SaveStreamError::io_open);
    CHECK(w.os_errno == ENOENT);
}

void test_rename_failure_cleans_up() {
    // Target is a non-empty directory: rename(file, dir) fails; the temp must be removed.
    const std::string dir = g_dir + "/blocker.sav";
    CHECK(::mkdir(dir.c_str(), 0755) == 0);
    spit(dir + "/inner", B({1}));
    const std::size_t before = dir_entries(g_dir);
    const auto w = sotc::write_file_atomic(dir, make_stream(1));
    CHECK_ERR(w.error, SaveStreamError::io_rename);
    CHECK(w.os_errno != 0);
    CHECK(!exists(dir + ".tmp"));
    CHECK(dir_entries(g_dir) == before);
    CHECK(slurp(dir + "/inner") == B({1}));
}

void test_unwritable_directory_preserves_original() {
    if (::geteuid() == 0) {
        std::puts("save_stream_file: skipping unwritable-dir test (running as root)");
        return;
    }
    const std::string sub = g_dir + "/ro";
    CHECK(::mkdir(sub.c_str(), 0755) == 0);
    const std::string path = sub + "/slot.sav";
    const Bytes original = make_stream(5);
    CHECK_ERR(sotc::write_file_atomic(path, original).error, SaveStreamError::none);
    CHECK(::chmod(sub.c_str(), 0555) == 0);
    const auto w = sotc::write_file_atomic(path, make_stream(6));
    CHECK(::chmod(sub.c_str(), 0755) == 0);
    CHECK_ERR(w.error, SaveStreamError::io_open);
    CHECK(w.os_errno == EACCES);
    CHECK(slurp(path) == original);                      // failed save never damages the old one
    CHECK(!exists(path + ".tmp"));
}

void test_truncated_file_every_offset() {
    const Bytes img = make_stream(3);
    const std::string path = g_dir + "/trunc.sav";
    for (std::size_t n = 0; n != img.size(); ++n) {
        spit(path, Bytes(img.begin(), img.begin() + static_cast<std::ptrdiff_t>(n)));
        CHECK_ERR(sotc::read_save_stream_file(path).error,
                  n < 16 ? SaveStreamError::truncated_header : SaveStreamError::truncated_body);
    }
}

void test_corrupt_file_rejected() {
    const Bytes img = make_stream(4);
    const std::string path = g_dir + "/corrupt.sav";
    for (std::size_t pos = 4; pos != 12; ++pos) {        // hash + size bytes, every byte flipped
        Bytes b = img;
        b[pos] = static_cast<std::byte>(std::to_integer<unsigned>(b[pos]) ^ 0xff);
        spit(path, b);
        CHECK(sotc::read_save_stream_file(path).error != SaveStreamError::none);
    }
    Bytes b = img;
    b.back() = static_cast<std::byte>(std::to_integer<unsigned>(b.back()) ^ 1);
    spit(path, b);
    CHECK_ERR(sotc::read_save_stream_file(path).error, SaveStreamError::bad_hash);
}

void test_oversize_file() {
    // Sparse file one byte past header + max body: rejected from fstat without reading it.
    const std::string path = g_dir + "/huge.sav";
    const int fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    CHECK(fd >= 0);
    CHECK(::ftruncate(fd, static_cast<off_t>(16 + sotc::kSaveStreamMaxBodySize + 1)) == 0);
    ::close(fd);
    CHECK_ERR(sotc::read_save_stream_file(path).error, SaveStreamError::file_too_large);
}

void test_duplicate_policy_via_file() {
    sotc::SaveStreamWriter w(1);
    CHECK_ERR(w.append_chunk("dup", 4, B({1})), SaveStreamError::none);
    CHECK_ERR(w.append_chunk("dup", 4, B({2})), SaveStreamError::none);
    const std::string path = g_dir + "/dup.sav";
    CHECK_ERR(sotc::write_file_atomic(path, w.serialize()).error, SaveStreamError::none);
    CHECK_ERR(sotc::read_save_stream_file(path).error, SaveStreamError::none);
    CHECK_ERR(sotc::read_save_stream_file(path, sotc::DuplicatePolicy::reject).error,
              SaveStreamError::reader_duplicate_name);
}

}  // namespace

int main() {
    const char* tmp_root = std::getenv("TMPDIR");
    std::string tmpl = std::string(tmp_root && *tmp_root ? tmp_root : "/tmp") + "/sotc_save_stream_XXXXXX";
    const char* made = ::mkdtemp(tmpl.data());
    CHECK(made != nullptr);
    g_dir = made;
    test_write_read_round_trip();
    test_overwrite_replaces();
    test_empty_and_zero_length();
    test_missing_and_bad_targets();
    test_rename_failure_cleans_up();
    test_unwritable_directory_preserves_original();
    test_truncated_file_every_offset();
    test_corrupt_file_rejected();
    test_oversize_file();
    test_duplicate_policy_via_file();
    // Best-effort cleanup of the private temp directory.
    const std::string cmd = "rm -rf '" + g_dir + "'";
    (void)std::system(cmd.c_str());
    std::puts("save_stream_file: all checks passed");
    return 0;
}
