// File helpers for the save stream container: atomic write (temp + fsync + rename) and read with
// error enums. Fixture-level only; no game save is read or written. Power-loss durability is NOT
// tested (see docs/VALIDATION.md G2 [B06]/[B07]).
#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <cstdarg>
#include <signal.h>
#include <string>
#include <sys/resource.h>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include "core/save_stream.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)
#define CHECK_ERR(got, want) do { const auto g_ = (got); if (g_ != (want)) { std::fprintf(stderr, "FAIL %s:%d %s: got %s want %s\n", __FILE__, __LINE__, #got, sotc::to_string(g_), sotc::to_string(want)); std::exit(1); } } while (0)

using sotc::SaveStreamError;
using Bytes = std::vector<std::byte>;

// Test-side fault seam for close(2). sotc_core is a static library, so the symbol below, defined in
// this executable, is what write_file_atomic's ::close binds to at link time (no production hook).
// When armed it really releases the descriptor and then reports EIO, which is how a deferred write
// error surfaces from close on a network or full filesystem. Only the next close() is affected.
namespace { std::atomic<int> g_fail_next_close{0}; }
extern "C" int close(int fd) {
    using CloseFn = int (*)(int);
    static const CloseFn real = reinterpret_cast<CloseFn>(::dlsym(RTLD_NEXT, "close"));
    const int rc = real(fd);
    if (g_fail_next_close.exchange(0) != 0 && rc == 0) {
        errno = EIO;
        return -1;
    }
    return rc;
}

// Test-side durability seam for fsync(2), fcntl(F_FULLFSYNC) and rename(2), same link-time mechanism
// as close() above. Unarmed it is a pure pass-through. Armed it records every sync/rename in call
// order (fd kind and identity come from fstat, so a sync of a directory is told from a sync of the
// temp file) and can inject sync failures. Only the calling thread's armed window is meaningful: the
// durability tests are single-threaded and disarm before any reader thread starts.
namespace sync_seam {
enum class Op { fsync, fullfsync, rename };
struct Event {
    Op op = Op::fsync;
    bool is_dir = false, is_reg = false;
    dev_t dev = 0;
    ino_t ino = 0;
    int rc = 0;
};
enum Fault { kNone = 0, kFileSyncFails, kFullFsyncUnsupported, kDirFsyncFails };
constexpr unsigned kMaxEvents = 32;
std::atomic<bool> armed{false};
std::atomic<int> fault{kNone};
std::atomic<unsigned> count{0};
Event events[kMaxEvents];
char last_rename_from[1024];   // source path of the most recent rename seen while armed

void record(Op op, int fd, int rc) {
    Event e;
    e.op = op;
    e.rc = rc;
    if (fd >= 0) {
        struct stat st {};
        if (::fstat(fd, &st) == 0) {
            e.is_dir = S_ISDIR(st.st_mode);
            e.is_reg = S_ISREG(st.st_mode);
            e.dev = st.st_dev;
            e.ino = st.st_ino;
        }
    }
    const unsigned i = count.fetch_add(1);
    if (i < kMaxEvents) events[i] = e;
}

bool is_dir_fd(int fd) {
    struct stat st {};
    return ::fstat(fd, &st) == 0 && S_ISDIR(st.st_mode);
}
}  // namespace sync_seam

extern "C" int fsync(int fd) {
    using Fn = int (*)(int);
    static const Fn real = reinterpret_cast<Fn>(::dlsym(RTLD_NEXT, "fsync"));
    if (!sync_seam::armed.load()) return real(fd);
    const int f = sync_seam::fault.load();
    int rc;
    if (f == sync_seam::kFileSyncFails && !sync_seam::is_dir_fd(fd)) { errno = EIO; rc = -1; }
    else if (f == sync_seam::kDirFsyncFails && sync_seam::is_dir_fd(fd)) { errno = EIO; rc = -1; }
    else rc = real(fd);
    const int saved = errno;
    sync_seam::record(sync_seam::Op::fsync, fd, rc);
    errno = saved;
    return rc;
}

// fcntl is variadic. The third argument is forwarded as one pointer-sized word, which carries int and
// pointer arguments alike on arm64; commands without an argument ignore it.
extern "C" int fcntl(int fd, int cmd, ...) {
    using Fn = int (*)(int, int, ...);
    static const Fn real = reinterpret_cast<Fn>(::dlsym(RTLD_NEXT, "fcntl"));
    va_list ap;
    va_start(ap, cmd);
    void* arg = va_arg(ap, void*);
    va_end(ap);
#ifdef F_FULLFSYNC
    if (cmd == F_FULLFSYNC && sync_seam::armed.load()) {
        const int f = sync_seam::fault.load();
        int rc;
        if ((f == sync_seam::kFileSyncFails || f == sync_seam::kFullFsyncUnsupported) &&
            !sync_seam::is_dir_fd(fd)) { errno = f == sync_seam::kFileSyncFails ? EIO : ENOTSUP; rc = -1; }
        else rc = real(fd, cmd, arg);
        const int saved = errno;
        sync_seam::record(sync_seam::Op::fullfsync, fd, rc);
        errno = saved;
        return rc;
    }
#endif
    return real(fd, cmd, arg);
}

extern "C" int rename(const char* from, const char* to) {
    using Fn = int (*)(const char*, const char*);
    static const Fn real = reinterpret_cast<Fn>(::dlsym(RTLD_NEXT, "rename"));
    const int rc = real(from, to);
    if (sync_seam::armed.load()) {
        const int saved = errno;
        sync_seam::record(sync_seam::Op::rename, -1, rc);
        std::snprintf(sync_seam::last_rename_from, sizeof sync_seam::last_rename_from, "%s", from);
        errno = saved;
    }
    return rc;
}

// Test-side seam for arc4random(3), same link-time mechanism. write_file_atomic names its temp
// "<dest>.tmp.<arc4random()>.<arc4random()>", so a scripted sequence lets a test predict the exact
// temp name and pre-create a colliding file. Unarmed it forwards to the real generator. Armed it
// returns the script in order (or, in repeat mode, the first value forever); asking for more values
// than a non-repeating script holds falls back to the real generator and is counted as an overrun
// so the test can reject it. The armed window is single-threaded (the writer runs on the caller).
namespace rand_seam {
std::atomic<bool> armed{false};
std::vector<std::uint32_t> script;
bool repeat = false;
std::atomic<unsigned> calls{0};
std::atomic<unsigned> overrun{0};
}  // namespace rand_seam

extern "C" uint32_t arc4random(void) {
    using Fn = uint32_t (*)(void);
    static const Fn real = reinterpret_cast<Fn>(::dlsym(RTLD_NEXT, "arc4random"));
    if (!rand_seam::armed.load()) return real();
    const unsigned i = rand_seam::calls.fetch_add(1);
    if (rand_seam::repeat) return rand_seam::script[0];
    if (i < rand_seam::script.size()) return rand_seam::script[i];
    rand_seam::overrun.fetch_add(1);
    return real();
}

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
    // A stale temp from an earlier invocation belongs to that invocation.
    const Bytes stale = B({9, 9, 9});
    spit(path + ".tmp", stale);
    const std::size_t before = dir_entries(g_dir);
    CHECK_ERR(sotc::write_file_atomic(path, a).error, SaveStreamError::none);
    CHECK(slurp(path) == a && slurp(path + ".tmp") == stale);
    CHECK(dir_entries(g_dir) == before);
}

void test_stale_temp_alias(bool hardlink, bool aliases_original) {
    const std::string path = g_dir + "/stale-" + (hardlink ? "hard" : "sym") +
                             (aliases_original ? "-original.sav" : "-other.sav");
    const std::string old_inode = path + ".previous";
    const std::string other = path + ".other";
    const Bytes original = make_stream(31);
    const Bytes unrelated = B({0xa1, 0xa2, 0xa3, 0xa4});
    const Bytes replacement = make_stream(41);
    spit(path, original);
    // Keep an independent name for the old save inode to detect pre-rename truncation.
    CHECK(::link(path.c_str(), old_inode.c_str()) == 0);
    spit(other, unrelated);
    const std::string target = aliases_original ? path : other;
    const std::string stale = path + ".tmp";
    CHECK((hardlink ? ::link(target.c_str(), stale.c_str()) :
                      ::symlink(target.c_str(), stale.c_str())) == 0);
    struct stat before_st {};
    CHECK(::lstat(stale.c_str(), &before_st) == 0);
    const std::size_t before_count = dir_entries(g_dir);

    CHECK_ERR(sotc::write_file_atomic(path, replacement).error, SaveStreamError::none);
    CHECK(slurp(path) == replacement);
    CHECK(slurp(old_inode) == original);
    CHECK(slurp(other) == unrelated);
    struct stat after_st {}, save_st {};
    CHECK(::lstat(stale.c_str(), &after_st) == 0);
    CHECK(after_st.st_dev == before_st.st_dev && after_st.st_ino == before_st.st_ino &&
          after_st.st_mode == before_st.st_mode);
    CHECK(::lstat(path.c_str(), &save_st) == 0 && S_ISREG(save_st.st_mode));
    CHECK(dir_entries(g_dir) == before_count);  // invocation temp was renamed away
    if (hardlink) CHECK(slurp(stale) == (aliases_original ? original : unrelated));
    else {
        std::vector<char> link_text(target.size() + 1);
        CHECK(::readlink(stale.c_str(), link_text.data(), link_text.size()) ==
              static_cast<ssize_t>(target.size()));
        CHECK(std::string(link_text.data(), target.size()) == target);
    }
}

void test_creation_respects_umask() {
    const std::string path = g_dir + "/private-mode.sav";
    const mode_t previous = ::umask(0077);
    const auto w = sotc::write_file_atomic(path, make_stream(1));
    ::umask(previous);
    CHECK_ERR(w.error, SaveStreamError::none);
    struct stat st {};
    CHECK(::stat(path.c_str(), &st) == 0 && (st.st_mode & 0777) == 0600);
}

void test_symlink_destination_replaces_link_not_referent() {
    const std::string path = g_dir + "/slot-link.sav";
    const std::string referent = g_dir + "/referent.sav";
    const Bytes original = B({0x51, 0x52, 0x53});
    const Bytes replacement = make_stream(21);
    spit(referent, original);
    CHECK(::symlink(referent.c_str(), path.c_str()) == 0);

    CHECK_ERR(sotc::write_file_atomic(path, replacement).error, SaveStreamError::none);
    struct stat st {};
    CHECK(::lstat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode));
    CHECK(slurp(path) == replacement);
    CHECK(slurp(referent) == original);
    struct stat tmp_st {};
    CHECK(::lstat((path + ".tmp").c_str(), &tmp_st) != 0 && errno == ENOENT);
}

void test_symlink_source_read_follows_referent() {
    const std::string path = g_dir + "/slot-source-link.sav";
    const std::string referent = g_dir + "/source-referent.sav";
    spit(referent, make_stream(11));
    CHECK(::symlink(referent.c_str(), path.c_str()) == 0);

    const auto r = sotc::read_save_stream_file(path);
    CHECK_ERR(r.error, SaveStreamError::none);
    CHECK(r.stream.header.descriptor_id == 0xd);
    CHECK(r.stream.chunks.size() == 2);
    CHECK(r.stream.chunks[0].payload == B({11, 2, 3}));
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
    const auto directory = sotc::read_save_stream_file(g_dir);
    CHECK_ERR(directory.error, SaveStreamError::io_read);
    CHECK(directory.os_errno == EISDIR);
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
    const std::string stale = dir + ".tmp";
    const std::string unique_stale = dir + ".tmp.left-by-another-writer";
    const Bytes preserved = B({0x71, 0x72, 0x73});
    spit(stale, preserved);
    spit(unique_stale, preserved);
    const std::size_t before = dir_entries(g_dir);
    const auto w = sotc::write_file_atomic(dir, make_stream(1));
    CHECK_ERR(w.error, SaveStreamError::io_rename);
    CHECK(w.os_errno != 0);
    CHECK(slurp(stale) == preserved);
    CHECK(slurp(unique_stale) == preserved);
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

    // The exact maximum passes the inclusive file-size gate; zero version bytes fail parsing next.
    const std::string exact_path = g_dir + "/exact-limit.sav";
    const int exact_fd = ::open(exact_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    CHECK(exact_fd >= 0);
    CHECK(::ftruncate(exact_fd, static_cast<off_t>(
        sotc::kSaveStreamHeaderSize + sotc::kSaveStreamMaxBodySize)) == 0);
    ::close(exact_fd);
    CHECK_ERR(sotc::read_save_stream_file(exact_path).error, SaveStreamError::bad_version);
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

// Collect names in dir that look like writer temp files for `base`.
std::vector<std::string> stray_temps(const std::string& dir, const std::string& base) {
    std::vector<std::string> out;
    DIR* d = ::opendir(dir.c_str());
    CHECK(d != nullptr);
    while (const dirent* e = ::readdir(d)) {
        const std::string name = e->d_name;
        if (name.rfind(base + ".tmp", 0) == 0) out.push_back(name);
    }
    ::closedir(d);
    return out;
}

// Lowers RLIMIT_FSIZE (and ignores SIGXFSZ so write returns EFBIG instead of killing us) for the
// lifetime of the object. A write that crosses the limit is first short, then fails with EFBIG.
struct FsizeLimit {
    rlimit saved{};
    void (*saved_handler)(int) = SIG_DFL;
    explicit FsizeLimit(rlim_t bytes) {
        CHECK(::getrlimit(RLIMIT_FSIZE, &saved) == 0);
        saved_handler = ::signal(SIGXFSZ, SIG_IGN);
        rlimit lowered = saved;
        lowered.rlim_cur = bytes;
        CHECK(::setrlimit(RLIMIT_FSIZE, &lowered) == 0);
    }
    ~FsizeLimit() {
        (void)::setrlimit(RLIMIT_FSIZE, &saved);
        ::signal(SIGXFSZ, saved_handler);
    }
};

void test_write_failure_cleans_up_and_never_reports_short_write() {
    const std::string path = g_dir + "/efbig.sav";
    const Bytes original = make_stream(51);
    CHECK_ERR(sotc::write_file_atomic(path, original).error, SaveStreamError::none);
    const Bytes big = make_stream(52);
    CHECK(big.size() > 24);
    const std::size_t before = dir_entries(g_dir);
    sotc::SaveStreamIoResult w;
    {
        FsizeLimit limit(24);   // first write() is short (24 bytes), the retry fails with EFBIG
        w = sotc::write_file_atomic(path, big);
    }
    // A short write must be resumed, never counted as complete, and the failure must surface.
    CHECK_ERR(w.error, SaveStreamError::io_write);
    CHECK(w.os_errno == EFBIG);
    CHECK(slurp(path) == original);                          // old save untouched
    CHECK(stray_temps(g_dir, "efbig.sav").empty());          // aborted temp unlinked
    CHECK(dir_entries(g_dir) == before);
    // With the limit gone the same call succeeds.
    CHECK_ERR(sotc::write_file_atomic(path, big).error, SaveStreamError::none);
    CHECK(slurp(path) == big);
}

void test_close_failure_cleans_up() {
    const std::string path = g_dir + "/closefail.sav";
    const Bytes original = make_stream(61);
    CHECK_ERR(sotc::write_file_atomic(path, original).error, SaveStreamError::none);
    const std::size_t before = dir_entries(g_dir);
    g_fail_next_close.store(1);
    const auto w = sotc::write_file_atomic(path, make_stream(62));
    CHECK(g_fail_next_close.load() == 0);                    // the seam was consumed by the writer
    CHECK_ERR(w.error, SaveStreamError::io_write);
    CHECK(w.os_errno == EIO);
    CHECK(slurp(path) == original);                          // unsynced temp was never renamed in
    CHECK(stray_temps(g_dir, "closefail.sav").empty());
    CHECK(dir_entries(g_dir) == before);
}

// RAII arm/disarm of the sync seam; clears the trace and any injected fault on both edges.
struct SyncTrace {
    explicit SyncTrace(int fault = sync_seam::kNone) {
        sync_seam::count.store(0);
        sync_seam::fault.store(fault);
        sync_seam::armed.store(true);
    }
    ~SyncTrace() { stop(); }
    void stop() {
        sync_seam::armed.store(false);
        sync_seam::fault.store(sync_seam::kNone);
    }
    unsigned size() const { return sync_seam::count.load(); }
    const sync_seam::Event& at(unsigned i) const { CHECK(i < size() && i < sync_seam::kMaxEvents); return sync_seam::events[i]; }
    // Index of the first event matching, or size() when absent.
    template <class P> unsigned find(P pred, unsigned from = 0) const {
        for (unsigned i = from; i < size() && i < sync_seam::kMaxEvents; ++i) if (pred(sync_seam::events[i])) return i;
        return size();
    }
};

bool is_file_sync(const sync_seam::Event& e) { return e.op != sync_seam::Op::rename && e.is_reg; }
bool is_dir_sync(const sync_seam::Event& e) { return e.op == sync_seam::Op::fsync && e.is_dir; }
bool is_rename(const sync_seam::Event& e) { return e.op == sync_seam::Op::rename; }
bool is_ok_file_sync(const sync_seam::Event& e) { return is_file_sync(e) && e.rc == 0; }

// The commit protocol: the temp file's data is forced to stable storage (F_FULLFSYNC or fsync)
// BEFORE rename publishes it, and the directory entry is fsynced AFTER rename. Observable here only
// through the syscall order; actual power-loss durability is not tested.
void test_fsync_ordering_file_before_rename_dir_after() {
    const std::string path = g_dir + "/durable.sav";
    const Bytes img = make_stream(81);
    SyncTrace t;
    const auto w = sotc::write_file_atomic(path, img);
    t.stop();
    CHECK_ERR(w.error, SaveStreamError::none);
    CHECK(slurp(path) == img);

    const unsigned ren = t.find(is_rename);
    CHECK(ren < t.size() && t.at(ren).rc == 0);
    CHECK(t.find(is_rename, ren + 1) == t.size());               // exactly one rename
    const unsigned fs = t.find(is_ok_file_sync);                 // successful sync of a regular file
    CHECK(fs < t.size());
    CHECK(fs < ren);                                             // file synced before it is published
    // The synced regular file is the one that was renamed into place, not some other descriptor.
    struct stat dest {}, dir_st {};
    CHECK(::stat(path.c_str(), &dest) == 0 && ::stat(g_dir.c_str(), &dir_st) == 0);
    CHECK(t.at(fs).dev == dest.st_dev && t.at(fs).ino == dest.st_ino);
    const unsigned ds = t.find(is_dir_sync, ren + 1);            // directory fsync after the rename
    CHECK(ds < t.size());
    CHECK(t.at(ds).dev == dir_st.st_dev && t.at(ds).ino == dir_st.st_ino && t.at(ds).rc == 0);
    CHECK(t.find(is_dir_sync) == ds);                            // none before the rename either
}

// F_FULLFSYNC is not supported everywhere: when it fails the writer must fall back to fsync of the
// same temp file, still before rename, and succeed.
void test_fullfsync_unsupported_falls_back_to_fsync() {
    const std::string path = g_dir + "/fallback.sav";
    const Bytes img = make_stream(82);
    SyncTrace t(sync_seam::kFullFsyncUnsupported);
    const auto w = sotc::write_file_atomic(path, img);
    t.stop();
    CHECK_ERR(w.error, SaveStreamError::none);
    CHECK(slurp(path) == img);
    const unsigned ren = t.find(is_rename);
    CHECK(ren < t.size() && t.at(ren).rc == 0);
    const unsigned full = t.find([](const sync_seam::Event& e) {
        return e.op == sync_seam::Op::fullfsync && e.is_reg && e.rc == -1; });
    const unsigned fs = t.find([](const sync_seam::Event& e) {
        return e.op == sync_seam::Op::fsync && e.is_reg && e.rc == 0; });
    CHECK(full < t.size() && fs < t.size());
    CHECK(full < fs && fs < ren);
    CHECK(t.find(is_dir_sync, ren + 1) < t.size());
    CHECK(stray_temps(g_dir, "fallback.sav").empty());
}

// A failed sync of the temp file must abort before rename: surface io_fsync, keep the old save, and
// leave no temp behind. The directory is not synced for a commit that never happened.
void test_file_sync_failure_aborts_before_rename() {
    const std::string path = g_dir + "/syncfail.sav";
    const Bytes original = make_stream(83);
    CHECK_ERR(sotc::write_file_atomic(path, original).error, SaveStreamError::none);
    const std::size_t before = dir_entries(g_dir);
    SyncTrace t(sync_seam::kFileSyncFails);
    const auto w = sotc::write_file_atomic(path, make_stream(84));
    t.stop();
    CHECK_ERR(w.error, SaveStreamError::io_fsync);
    CHECK(w.os_errno == EIO);
    CHECK(slurp(path) == original);
    CHECK(t.find(is_rename) == t.size());
    CHECK(t.find(is_dir_sync) == t.size());
    CHECK(t.find(is_file_sync) < t.size());                      // the sync really was attempted
    CHECK(stray_temps(g_dir, "syncfail.sav").empty());
    CHECK(dir_entries(g_dir) == before);
}

// Directory fsync is best effort: it must be attempted after the rename, and its failure must not be
// reported or undo the committed save.
void test_dir_fsync_failure_is_best_effort() {
    const std::string path = g_dir + "/dirsyncfail.sav";
    const Bytes img = make_stream(85);
    SyncTrace t(sync_seam::kDirFsyncFails);
    const auto w = sotc::write_file_atomic(path, img);
    t.stop();
    CHECK_ERR(w.error, SaveStreamError::none);
    CHECK(w.os_errno == 0);
    CHECK(slurp(path) == img);
    const unsigned ren = t.find(is_rename);
    CHECK(ren < t.size() && t.at(ren).rc == 0);
    const unsigned ds = t.find(is_dir_sync, ren + 1);
    CHECK(ds < t.size() && t.at(ds).rc == -1);                   // attempted, and it did fail
    CHECK(t.find(is_ok_file_sync) < ren);
    CHECK(stray_temps(g_dir, "dirsyncfail.sav").empty());
}

// Readers must see the old or the new complete save at all times: the destination is replaced by
// rename, never unlinked first. Two reader threads spin on the destination while the main thread
// overwrites it repeatedly. Bounded by overwrite count and wall time.
void test_concurrent_reader_never_sees_gap_or_partial() {
    const std::string path = g_dir + "/concurrent.sav";
    const Bytes a = make_stream(71), b = make_stream(72);
    CHECK_ERR(sotc::write_file_atomic(path, a).error, SaveStreamError::none);

    std::atomic<bool> stop{false};
    std::atomic<unsigned long> reads{0}, bad{0};
    std::atomic<int> first_bad_error{0}, first_bad_errno{0};
    auto reader = [&] {
        while (!stop.load(std::memory_order_relaxed)) {
            const auto r = sotc::read_save_stream_file(path);
            reads.fetch_add(1, std::memory_order_relaxed);
            const bool ok = r.error == SaveStreamError::none && r.stream.chunks.size() == 2 &&
                            (r.stream.chunks[0].payload == B({71, 2, 3}) ||
                             r.stream.chunks[0].payload == B({72, 2, 3}));
            if (!ok && bad.fetch_add(1) == 0) {
                first_bad_error.store(static_cast<int>(r.error));
                first_bad_errno.store(r.os_errno);
            }
        }
    };
    std::thread t1(reader), t2(reader);

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(1500);
    unsigned writes = 0;
    bool write_ok = true;
    while (writes < 400 && (writes < 20 || std::chrono::steady_clock::now() < deadline)) {
        write_ok = sotc::write_file_atomic(path, (writes & 1) ? a : b).error == SaveStreamError::none;
        if (!write_ok || bad.load() != 0) break;
        ++writes;
    }
    stop.store(true);
    t1.join();
    t2.join();
    CHECK(write_ok);
    if (bad.load() != 0)
        std::fprintf(stderr, "concurrent reader: %lu bad of %lu reads; first error=%d errno=%d\n",
                     bad.load(), reads.load(), first_bad_error.load(), first_bad_errno.load());
    CHECK(bad.load() == 0);
    CHECK(reads.load() > 0 && writes >= 20);
    CHECK(stray_temps(g_dir, "concurrent.sav").empty());
}

// RAII arm/disarm of the arc4random seam. `values` is the scripted sequence; with repeat the first
// value is returned forever (retry-exhaustion tests).
struct RandScript {
    explicit RandScript(std::vector<std::uint32_t> values, bool repeat_first = false) {
        CHECK(!values.empty());
        rand_seam::script = std::move(values);
        rand_seam::repeat = repeat_first;
        rand_seam::calls.store(0);
        rand_seam::overrun.store(0);
        rand_seam::armed.store(true);
    }
    ~RandScript() { stop(); }
    void stop() { rand_seam::armed.store(false); }
    unsigned calls() const { return rand_seam::calls.load(); }
    unsigned overrun() const { return rand_seam::overrun.load(); }
};

std::string temp_name(const std::string& dest, std::uint32_t a, std::uint32_t b) {
    return dest + ".tmp." + std::to_string(a) + "." + std::to_string(b);
}

struct Identity {
    dev_t dev = 0; ino_t ino = 0; mode_t mode = 0; off_t size = 0;
    bool operator==(const Identity& o) const {
        return dev == o.dev && ino == o.ino && mode == o.mode && size == o.size;
    }
};
Identity ident(const std::string& path) {
    struct stat st {};
    CHECK(::lstat(path.c_str(), &st) == 0);
    return {st.st_dev, st.st_ino, st.st_mode, st.st_size};
}
bool absent(const std::string& path) {
    struct stat st {};
    return ::lstat(path.c_str(), &st) != 0 && errno == ENOENT;
}

enum class Squatter { regular, dangling_symlink, symlink_to_file, hardlink_to_dest };

// Another writer (or an attacker) already owns the exact temp name this invocation draws first. The
// writer must create its temp exclusively: never open, truncate, write through or rename the
// squatter (nor follow a symlink at that name), must draw a fresh name, and must still commit.
void test_temp_name_collision_is_never_clobbered(Squatter kind, unsigned collisions) {
    const char* tag = kind == Squatter::regular ? "reg" : kind == Squatter::dangling_symlink ? "dangle" :
                      kind == Squatter::symlink_to_file ? "symfile" : "hard";
    const std::string dest = g_dir + "/excl-" + tag + "-" + std::to_string(collisions) + ".sav";
    const std::string referent = dest + ".referent";   // what a symlink squatter points at
    const std::string dangling_target = dest + ".must-not-exist";
    const Bytes original = make_stream(91), replacement = make_stream(92);
    const Bytes sentinel = B({0xde, 0xad, 0xbe, 0xef, 0x01});
    const Bytes referent_bytes = B({0x5e, 0x4e, 0x3e});
    spit(dest, original);
    if (kind == Squatter::symlink_to_file) spit(referent, referent_bytes);

    // Script: colliding pairs (1,2), (3,4), ... then one free pair. Squatters sit on the colliding ones.
    std::vector<std::uint32_t> script;
    std::vector<std::string> squatted;
    for (unsigned c = 0; c != collisions; ++c) {
        script.push_back(1000 + 2 * c);
        script.push_back(1001 + 2 * c);
        squatted.push_back(temp_name(dest, script[2 * c], script[2 * c + 1]));
    }
    const std::uint32_t free_a = 7001, free_b = 7002;
    script.push_back(free_a);
    script.push_back(free_b);

    std::vector<Identity> before_id;
    for (const auto& s : squatted) {
        switch (kind) {
            case Squatter::regular: spit(s, sentinel); break;
            case Squatter::dangling_symlink: CHECK(::symlink(dangling_target.c_str(), s.c_str()) == 0); break;
            case Squatter::symlink_to_file: CHECK(::symlink(referent.c_str(), s.c_str()) == 0); break;
            case Squatter::hardlink_to_dest: CHECK(::link(dest.c_str(), s.c_str()) == 0); break;
        }
        before_id.push_back(ident(s));
    }
    const Identity dest_before = ident(dest);   // for hardlink_to_dest the squatter shares this inode
    const std::size_t entries_before = dir_entries(g_dir);

    sotc::SaveStreamIoResult w;
    unsigned calls = 0, overrun = 0;
    {
        RandScript rs(script);
        SyncTrace t;   // records the rename source path
        w = sotc::write_file_atomic(dest, replacement);
        t.stop();
        rs.stop();
        calls = rs.calls();
        overrun = rs.overrun();
    }
    CHECK_ERR(w.error, SaveStreamError::none);
    CHECK(w.os_errno == 0);
    CHECK(overrun == 0);
    CHECK(slurp(dest) == replacement);
    for (std::size_t i = 0; i != squatted.size(); ++i) {
        CHECK(ident(squatted[i]) == before_id[i]);    // same inode, type, mode and size: not touched
        switch (kind) {
            case Squatter::regular: CHECK(slurp(squatted[i]) == sentinel); break;
            case Squatter::dangling_symlink: break;
            case Squatter::symlink_to_file: CHECK(slurp(referent) == referent_bytes); break;
            case Squatter::hardlink_to_dest: CHECK(slurp(squatted[i]) == original); break;
        }
    }
    if (kind == Squatter::dangling_symlink) CHECK(absent(dangling_target));   // link not followed
    if (kind == Squatter::hardlink_to_dest) {
        // The old save inode survives under the squatter's name with its original bytes, and the
        // destination is a different inode now.
        CHECK(ident(squatted[0]).ino == dest_before.ino);
        CHECK(ident(dest).ino != dest_before.ino);
    }
    // One fresh pair of draws per attempt: collisions colliding attempts plus the winning one.
    CHECK(calls == 2 * (collisions + 1));
    // The committed temp is the first non-colliding name, built from two draws in order.
    CHECK(std::string(sync_seam::last_rename_from) == temp_name(dest, free_a, free_b));
    CHECK(absent(temp_name(dest, free_a, free_b)));            // committed temp renamed away
    // Only the squatters remain as temp-looking names: nothing of ours was left behind.
    const std::string base = dest.substr(dest.find_last_of('/') + 1);
    CHECK(stray_temps(g_dir, base).size() == squatted.size());
    CHECK(dir_entries(g_dir) == entries_before);
}

// Every attempt collides: the writer gives up after a bounded number of attempts (128, two draws
// each) with io_open/EEXIST, leaves the squatter and the destination untouched, and creates nothing.
void test_temp_name_collision_retry_exhaustion() {
    const std::string dest = g_dir + "/excl-exhaust.sav";
    const Bytes original = make_stream(93), replacement = make_stream(94);
    const Bytes sentinel = B({0xca, 0xfe, 0xba, 0xbe});
    constexpr std::uint32_t kConst = 43981;   // every draw returns this, so every attempt names the same file
    const std::string squatter = temp_name(dest, kConst, kConst);
    spit(dest, original);
    spit(squatter, sentinel);
    const Identity dest_before = ident(dest), squat_before = ident(squatter);
    const std::size_t entries_before = dir_entries(g_dir);

    sotc::SaveStreamIoResult w;
    unsigned calls = 0, overrun = 0;
    {
        RandScript rs({kConst}, /*repeat_first=*/true);
        SyncTrace t;
        w = sotc::write_file_atomic(dest, replacement);
        t.stop();
        rs.stop();
        calls = rs.calls();
        overrun = rs.overrun();
    }
    CHECK_ERR(w.error, SaveStreamError::io_open);
    CHECK(w.os_errno == EEXIST);
    CHECK(overrun == 0);
    CHECK(calls == 2 * 128);                       // exactly 128 attempts, two draws each, no more
    CHECK(sync_seam::count.load() == 0);           // nothing was synced or renamed
    CHECK(slurp(dest) == original && ident(dest) == dest_before);
    CHECK(slurp(squatter) == sentinel && ident(squatter) == squat_before);
    CHECK(dir_entries(g_dir) == entries_before);
    // Same call with the generator unarmed succeeds: the failure was the forced collision only.
    CHECK_ERR(sotc::write_file_atomic(dest, replacement).error, SaveStreamError::none);
    CHECK(slurp(dest) == replacement && slurp(squatter) == sentinel);
}

}  // namespace

int main() {
    const char* tmp_root = std::getenv("TMPDIR");
    std::string tmpl = std::string(tmp_root && *tmp_root ? tmp_root : "/tmp") + "/sotc_save_stream_XXXXXX";
    const char* made = ::mkdtemp(tmpl.data());
    CHECK(made != nullptr);
    g_dir = made;
    test_stale_temp_alias(false, false);
    test_stale_temp_alias(true, false);
    test_stale_temp_alias(false, true);
    test_stale_temp_alias(true, true);
    test_write_read_round_trip();
    test_overwrite_replaces();
    test_creation_respects_umask();
    test_symlink_destination_replaces_link_not_referent();
    test_symlink_source_read_follows_referent();
    test_empty_and_zero_length();
    test_missing_and_bad_targets();
    test_rename_failure_cleans_up();
    test_unwritable_directory_preserves_original();
    test_truncated_file_every_offset();
    test_corrupt_file_rejected();
    test_oversize_file();
    test_duplicate_policy_via_file();
    test_write_failure_cleans_up_and_never_reports_short_write();
    test_close_failure_cleans_up();
    test_fsync_ordering_file_before_rename_dir_after();
    test_fullfsync_unsupported_falls_back_to_fsync();
    test_file_sync_failure_aborts_before_rename();
    test_dir_fsync_failure_is_best_effort();
    test_concurrent_reader_never_sees_gap_or_partial();
    test_temp_name_collision_is_never_clobbered(Squatter::regular, 1);
    test_temp_name_collision_is_never_clobbered(Squatter::regular, 2);
    test_temp_name_collision_is_never_clobbered(Squatter::dangling_symlink, 1);
    test_temp_name_collision_is_never_clobbered(Squatter::symlink_to_file, 1);
    test_temp_name_collision_is_never_clobbered(Squatter::hardlink_to_dest, 1);
    test_temp_name_collision_retry_exhaustion();
    // Best-effort cleanup of the private temp directory.
    const std::string cmd = "rm -rf '" + g_dir + "'";
    (void)std::system(cmd.c_str());
    std::puts("save_stream_file: all checks passed");
    return 0;
}
