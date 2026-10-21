// Tests for the C ABI (native/api/sotc_core.h). The central assertion, repeated on purpose:
// start() NEVER succeeds and NEVER reports RUNNING, because the core has no engine.
#include <sys/stat.h>
#include <unistd.h>

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "sotc_core.h"

#if SOTC_TEST_CORE_LINKED
#include "core/startup_ctors.hpp"
#endif

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

const char* kArchives[SOTC_ARCHIVE_COUNT] = {"cachedFiles.psarc", "pak0.psarc", "pak1.psarc",
                                             "pak2.psarc",        "pak3.psarc", "pakN.psarc"};

std::string make_dir() {
  const char* base = std::getenv("TMPDIR");
  std::string tmpl = std::string(base && *base ? base : "/tmp") + "/sotc_api_test_XXXXXX";
  std::vector<char> b(tmpl.begin(), tmpl.end());
  b.push_back('\0');
  CHECK(mkdtemp(b.data()) != nullptr);
  return b.data();
}

void write_file(const std::string& path, const std::string& bytes) {
  FILE* f = std::fopen(path.c_str(), "wb");
  CHECK(f != nullptr);
  if (!bytes.empty()) CHECK(std::fwrite(bytes.data(), 1, bytes.size(), f) == bytes.size());
  std::fclose(f);
}

void write_all_good(const std::string& dir) {
  for (const char* n : kArchives) write_file(dir + "/" + n, "PSAR0123");
}

void rm_rf(const std::string& dir) {
  for (const char* n : kArchives) {
    const std::string p = dir + "/" + n;
    chmod(p.c_str(), 0600);
    if (unlink(p.c_str()) != 0) rmdir(p.c_str());
  }
  rmdir(dir.c_str());
}

sotc_validation_result blank_result() {
  sotc_validation_result r;
  std::memset(&r, 0xAB, sizeof r);
  r.struct_size = sizeof r;
  return r;
}

struct StateLog {
  std::vector<std::pair<sotc_state, sotc_state>> changes;
  bool saw_running = false;
};
void log_state(void* user, sotc_state from, sotc_state to) {
  auto* l = static_cast<StateLog*>(user);
  l->changes.emplace_back(from, to);
  if (from == SOTC_STATE_RUNNING || to == SOTC_STATE_RUNNING) l->saw_running = true;
}

sotc_core* make_core() {
  sotc_core* c = nullptr;
  CHECK(sotc_core_create(SOTC_CORE_ABI_VERSION, &c) == SOTC_OK && c != nullptr);
  return c;
}

sotc_status get_status(sotc_core* c) {
  sotc_status s;
  std::memset(&s, 0, sizeof s);
  s.struct_size = sizeof s;
  CHECK(sotc_core_status(c, &s) == SOTC_OK);
  CHECK(s.struct_size == sizeof(sotc_status));
  CHECK(s.engine_implemented == 0);
  CHECK(s.state != SOTC_STATE_RUNNING);
  return s;
}

void test_version_and_messages() {
  CHECK(sotc_core_abi_version() == SOTC_CORE_ABI_VERSION);
  CHECK(sotc_core_abi_version() >> 16 == SOTC_CORE_ABI_MAJOR);
  CHECK(std::strcmp(sotc_core_error_name(SOTC_OK), "SOTC_OK") == 0);
  CHECK(std::strcmp(sotc_core_error_name(SOTC_ERR_ENGINE_NOT_IMPLEMENTED), "SOTC_ERR_ENGINE_NOT_IMPLEMENTED") == 0);
  CHECK(std::strcmp(sotc_core_error_name(999), "SOTC_ERR_UNKNOWN") == 0);
  CHECK(std::strcmp(sotc_core_state_name(SOTC_STATE_FAILED), "failed") == 0);
  CHECK(std::strcmp(sotc_core_state_name(77), "unknown") == 0);

  char buf[256];
  const size_t full = sotc_core_error_message(SOTC_ERR_ENGINE_NOT_IMPLEMENTED, buf, sizeof buf);
  CHECK(full == std::strlen(buf) && full > 10);
  CHECK(std::strstr(buf, "not implemented") != nullptr);
  // Truncation: NUL-terminated, returns the untruncated length.
  char small[8];
  std::memset(small, 'x', sizeof small);
  CHECK(sotc_core_error_message(SOTC_ERR_ENGINE_NOT_IMPLEMENTED, small, sizeof small) == full);
  CHECK(small[7] == '\0' && std::strlen(small) == 7);
  // Length query with no buffer, and cap 0 leaves the buffer untouched.
  CHECK(sotc_core_error_message(SOTC_ERR_ENGINE_NOT_IMPLEMENTED, nullptr, 0) == full);
  char untouched = 'q';
  CHECK(sotc_core_error_message(SOTC_ERR_BAD_STATE, &untouched, 0) > 0 && untouched == 'q');
  CHECK(sotc_core_error_message(4242, buf, sizeof buf) > 0 && std::strstr(buf, "unknown") != nullptr);
  // Every defined code has a distinct non-empty message.
  std::vector<std::string> seen;
  for (int c = SOTC_OK; c <= SOTC_ERR_INTERNAL; ++c) {
    char m[256];
    CHECK(sotc_core_error_message(static_cast<sotc_error>(c), m, sizeof m) > 0);
    for (const auto& s : seen) CHECK(s != m);
    seen.push_back(m);
  }
}

void test_create() {
  sotc_core* c = nullptr;
  CHECK(sotc_core_create(SOTC_CORE_ABI_VERSION, nullptr) == SOTC_ERR_INVALID_ARGUMENT);
  CHECK(sotc_core_create(((SOTC_CORE_ABI_MAJOR + 1) << 16), &c) == SOTC_ERR_ABI_MISMATCH && c == nullptr);
  CHECK(sotc_core_create((SOTC_CORE_ABI_MAJOR << 16) | (SOTC_CORE_ABI_MINOR + 1), &c) == SOTC_ERR_ABI_MISMATCH);
  CHECK(sotc_core_create(0, &c) == SOTC_ERR_ABI_MISMATCH);
  CHECK(sotc_core_create(SOTC_CORE_ABI_VERSION, &c) == SOTC_OK && c != nullptr);
  const sotc_status s = get_status(c);
  CHECK(s.state == SOTC_STATE_IDLE && s.assets_validated == 0 && s.assets_ready == 0 && s.last_error == SOTC_OK);
  sotc_core_destroy(c);
  sotc_core_destroy(nullptr);  // no-op
  CHECK(sotc_core_status(nullptr, nullptr) == SOTC_ERR_INVALID_ARGUMENT);
  CHECK(sotc_core_start(nullptr) == SOTC_ERR_INVALID_ARGUMENT);
  CHECK(sotc_core_stop(nullptr) == SOTC_ERR_INVALID_ARGUMENT);
}

void test_pure_validation() {
  const std::string dir = make_dir();
  sotc_validation_result r = blank_result();

  // Empty directory: all six missing, in the required order, with the launcher's wording.
  CHECK(sotc_core_validate(nullptr, dir.c_str(), &r) == SOTC_OK);
  CHECK(r.assets_ready == 0 && r.problem_count == SOTC_ARCHIVE_COUNT && r.error == SOTC_ERR_ASSETS_NOT_READY);
  for (uint32_t i = 0; i < SOTC_ARCHIVE_COUNT; ++i) {
    CHECK(std::strcmp(r.archives[i].name, kArchives[i]) == 0);
    CHECK(r.archives[i].status == SOTC_ARCHIVE_MISSING && r.archives[i].size_bytes == 0);
  }
  CHECK(std::string(r.detail) == "cachedFiles.psarc not found");

  // Complete set: assets ready, engine NOT ready, start would not succeed.
  write_all_good(dir);
  r = blank_result();
  CHECK(sotc_core_validate(nullptr, dir.c_str(), &r) == SOTC_OK);
  CHECK(r.assets_ready == 1 && r.problem_count == 0 && r.error == SOTC_OK);
  CHECK(r.engine_ready == 0 && r.start_would_succeed == 0);
  CHECK(r.engine_error == SOTC_ERR_ENGINE_NOT_IMPLEMENTED);
  CHECK(std::strstr(r.detail, "engine not implemented") != nullptr);
  for (const auto& a : r.archives) CHECK(a.status == SOTC_ARCHIVE_OK && a.size_bytes == 8);

  // Bad magic, short file, directory wearing an archive name, missing, per-archive ordering.
  write_file(dir + "/pak0.psarc", "XXXXabcd");
  write_file(dir + "/pak1.psarc", "PS");
  unlink((dir + "/pak2.psarc").c_str());
  mkdir((dir + "/pak2.psarc").c_str(), 0700);
  unlink((dir + "/pakN.psarc").c_str());
  r = blank_result();
  CHECK(sotc_core_validate(nullptr, dir.c_str(), &r) == SOTC_OK);
  CHECK(r.assets_ready == 0 && r.problem_count == 4);
  CHECK(r.archives[0].status == SOTC_ARCHIVE_OK);
  CHECK(r.archives[1].status == SOTC_ARCHIVE_BAD_MAGIC && r.archives[1].size_bytes == 8);
  CHECK(r.archives[2].status == SOTC_ARCHIVE_BAD_MAGIC && r.archives[2].size_bytes == 2);
  CHECK(r.archives[3].status == SOTC_ARCHIVE_MISSING && r.archives[3].size_bytes == 0);
  CHECK(r.archives[4].status == SOTC_ARCHIVE_OK);
  CHECK(r.archives[5].status == SOTC_ARCHIVE_MISSING);
  CHECK(std::string(r.detail) == "pak0.psarc is not a PSARC archive");  // first problem wins
  CHECK(r.start_would_succeed == 0);

  if (geteuid() != 0) {  // root ignores mode bits
    write_file(dir + "/pak0.psarc", "PSAR0123");
    CHECK(chmod((dir + "/pak0.psarc").c_str(), 0000) == 0);
    r = blank_result();
    CHECK(sotc_core_validate(nullptr, dir.c_str(), &r) == SOTC_OK);
    CHECK(r.archives[1].status == SOTC_ARCHIVE_UNREADABLE);
  }
  rm_rf(dir);

  // Directory that is not there, and a path that is a regular file.
  r = blank_result();
  CHECK(sotc_core_validate(nullptr, "/nonexistent/sotc_api_test_dir", &r) == SOTC_OK);
  CHECK(r.assets_ready == 0 && r.error == SOTC_ERR_IO && r.problem_count == SOTC_ARCHIVE_COUNT);
  CHECK(std::strstr(r.detail, "not a directory") != nullptr);

  // Argument and struct_size checks.
  CHECK(sotc_core_validate(nullptr, nullptr, &r) == SOTC_ERR_INVALID_ARGUMENT);
  CHECK(sotc_core_validate(nullptr, "", &r) == SOTC_ERR_INVALID_ARGUMENT);
  CHECK(sotc_core_validate(nullptr, "/tmp", nullptr) == SOTC_ERR_INVALID_ARGUMENT);
  r = blank_result();
  r.struct_size = sizeof r - 1;
  CHECK(sotc_core_validate(nullptr, "/tmp", &r) == SOTC_ERR_INVALID_ARGUMENT);
  // A larger (newer) caller struct is accepted and only the v1 prefix is written.
  struct Big { sotc_validation_result v; unsigned char tail[16]; } big;
  std::memset(&big, 0x5A, sizeof big);
  big.v.struct_size = sizeof big;
  CHECK(sotc_core_validate(nullptr, "/nonexistent", &big.v) == SOTC_OK);
  CHECK(big.v.struct_size == sizeof(sotc_validation_result));
  for (unsigned char b : big.tail) CHECK(b == 0x5A);
}

void test_handle_lifecycle_never_runs() {
  const std::string dir = make_dir();
  sotc_core* c = make_core();
  StateLog log;
  CHECK(sotc_core_set_state_callback(c, log_state, &log) == SOTC_OK);

  // start before any validation: bad state, not "assets not ready".
  CHECK(sotc_core_start(c) == SOTC_ERR_BAD_STATE);
  CHECK(get_status(c).state == SOTC_STATE_IDLE && get_status(c).last_error == SOTC_ERR_BAD_STATE);
  char msg[256];
  CHECK(sotc_core_last_error_message(c, msg, sizeof msg) > 0);

  // Validation with problems: back to IDLE, start now says assets not ready.
  sotc_validation_result r = blank_result();
  CHECK(sotc_core_validate(c, dir.c_str(), &r) == SOTC_OK && r.assets_ready == 0);
  sotc_status s = get_status(c);
  CHECK(s.state == SOTC_STATE_IDLE && s.assets_validated == 1 && s.assets_ready == 0);
  CHECK(sotc_core_start(c) == SOTC_ERR_ASSETS_NOT_READY);
  CHECK(get_status(c).state == SOTC_STATE_IDLE);

  // Good assets: READY, with the exact transition sequence reported to the callback.
  write_all_good(dir);
  log.changes.clear();
  r = blank_result();
  CHECK(sotc_core_validate(c, dir.c_str(), &r) == SOTC_OK && r.assets_ready == 1);
  CHECK(r.start_would_succeed == 0);
  s = get_status(c);
  CHECK(s.state == SOTC_STATE_READY && s.assets_ready == 1 && s.last_error == SOTC_OK);
  CHECK(log.changes.size() == 2);
  CHECK(log.changes[0] == std::make_pair(SOTC_STATE_IDLE, SOTC_STATE_VALIDATING));
  CHECK(log.changes[1] == std::make_pair(SOTC_STATE_VALIDATING, SOTC_STATE_READY));

  // START: distinct engine-not-implemented error, FAILED state, never RUNNING.
  log.changes.clear();
  CHECK(sotc_core_start(c) == SOTC_ERR_ENGINE_NOT_IMPLEMENTED);
  s = get_status(c);
  CHECK(s.state == SOTC_STATE_FAILED && s.last_error == SOTC_ERR_ENGINE_NOT_IMPLEMENTED);
  CHECK(s.state != SOTC_STATE_RUNNING && s.engine_implemented == 0);
  CHECK(log.changes.size() == 2);
  CHECK(log.changes[0] == std::make_pair(SOTC_STATE_READY, SOTC_STATE_STARTING));
  CHECK(log.changes[1] == std::make_pair(SOTC_STATE_STARTING, SOTC_STATE_FAILED));
  CHECK(!log.saw_running);
  const size_t n = sotc_core_last_error_message(c, msg, sizeof msg);
  CHECK(n == std::strlen(msg) && std::strstr(msg, "not implemented") != nullptr);

  // Retrying from FAILED gives the identical honest answer, 200 times, never RUNNING.
  for (int i = 0; i < 200; ++i) {
    CHECK(sotc_core_start(c) == SOTC_ERR_ENGINE_NOT_IMPLEMENTED);
    CHECK(get_status(c).state == SOTC_STATE_FAILED);
  }
  CHECK(!log.saw_running);

  // stop() is an idempotent no-op here: nothing was running, so FAILED is preserved.
  CHECK(sotc_core_stop(c) == SOTC_OK);
  CHECK(sotc_core_stop(c) == SOTC_OK);
  CHECK(get_status(c).state == SOTC_STATE_FAILED);

  // Re-validate from FAILED returns to READY; failed validation after that drops to IDLE.
  CHECK(sotc_core_validate(c, dir.c_str(), &r) == SOTC_OK && r.assets_ready == 1);
  CHECK(get_status(c).state == SOTC_STATE_READY);
  CHECK(sotc_core_stop(c) == SOTC_OK && get_status(c).state == SOTC_STATE_READY);
  unlink((dir + "/pak3.psarc").c_str());
  CHECK(sotc_core_validate(c, dir.c_str(), &r) == SOTC_OK && r.assets_ready == 0);
  CHECK(get_status(c).state == SOTC_STATE_IDLE);
  CHECK(sotc_core_start(c) == SOTC_ERR_ASSETS_NOT_READY);

  // Clearing the callback stops notifications.
  CHECK(sotc_core_set_state_callback(c, nullptr, nullptr) == SOTC_OK);
  const size_t before = log.changes.size();
  write_file(dir + "/pak3.psarc", "PSAR");
  CHECK(sotc_core_validate(c, dir.c_str(), &r) == SOTC_OK);
  CHECK(log.changes.size() == before);
  CHECK(!log.saw_running);

  sotc_core_destroy(c);
  rm_rf(dir);
}

void test_validate_arguments_do_not_change_state() {
  sotc_core* c = make_core();
  sotc_validation_result r = blank_result();
  CHECK(sotc_core_validate(c, nullptr, &r) == SOTC_ERR_INVALID_ARGUMENT);
  CHECK(sotc_core_validate(c, "/tmp", nullptr) == SOTC_ERR_INVALID_ARGUMENT);
  CHECK(get_status(c).state == SOTC_STATE_IDLE && get_status(c).assets_validated == 0);
  sotc_core_destroy(c);
}

void test_concurrent_calls_never_report_running() {
  const std::string dir = make_dir();
  write_all_good(dir);
  sotc_core* c = make_core();
  std::atomic<bool> saw_running{false};
  std::atomic<int> start_ok{0};
  auto cb = [](void* user, sotc_state, sotc_state to) {
    if (to == SOTC_STATE_RUNNING) static_cast<std::atomic<bool>*>(user)->store(true);
  };
  CHECK(sotc_core_set_state_callback(c, cb, &saw_running) == SOTC_OK);
  std::vector<std::thread> ts;
  for (int t = 0; t < 4; ++t) {
    ts.emplace_back([&, t] {
      for (int i = 0; i < 200; ++i) {
        sotc_validation_result r = blank_result();
        switch ((i + t) % 4) {
          case 0: (void)sotc_core_validate(c, dir.c_str(), &r); break;
          case 1: if (sotc_core_start(c) == SOTC_OK) start_ok++; break;
          case 2: (void)sotc_core_stop(c); break;
          default: {
            sotc_status s;
            std::memset(&s, 0, sizeof s);
            s.struct_size = sizeof s;
            if (sotc_core_status(c, &s) == SOTC_OK && s.state == SOTC_STATE_RUNNING) saw_running = true;
          }
        }
      }
    });
  }
  for (auto& t : ts) t.join();
  CHECK(!saw_running.load());
  CHECK(start_ok.load() == 0);
  sotc_core_destroy(c);
  rm_rf(dir);
}

void test_coverage() {
  sotc_coverage_report cov;
  std::memset(&cov, 0, sizeof cov);
  cov.struct_size = sizeof cov;
  CHECK(sotc_core_coverage(&cov) == SOTC_OK);
  CHECK(cov.struct_size == sizeof cov && cov.abi_version == SOTC_CORE_ABI_VERSION);
  CHECK(cov.engine_implemented == 0);
  CHECK(cov.subsystem_count == SOTC_SUBSYSTEM_COUNT);
  bool saw_engine = false;
  for (uint32_t i = 0; i < cov.subsystem_count; ++i) {
    const sotc_subsystem_report& s = cov.subsystems[i];
    CHECK(s.name[0] != '\0' && s.detail[0] != '\0');
    CHECK(s.status != SOTC_SUBSYSTEM_IMPLEMENTED);  // nothing may claim to be finished
    if (std::strcmp(s.name, "engine_boot") == 0) {
      saw_engine = true;
      CHECK(s.status == SOTC_SUBSYSTEM_NOT_IMPLEMENTED);
    }
    if (std::strcmp(s.name, "metal_renderer") == 0 || std::strcmp(s.name, "audio") == 0 ||
        std::strcmp(s.name, "save_load") == 0)
      CHECK(s.status == SOTC_SUBSYSTEM_NOT_IMPLEMENTED);
  }
  CHECK(saw_engine);
  CHECK(cov.ctor.run_ok == (cov.ctor.blocked_on_rva == 0 && !cov.ctor.callback_failed ? 1u : 0u) ||
        !cov.ctor.linked);
#if SOTC_TEST_CORE_LINKED
  CHECK(cov.ctor.linked == 1);
  CHECK(std::strcmp(cov.subsystems[0].name, "startup_constructors") == 0);
  CHECK(cov.subsystems[0].status == SOTC_SUBSYSTEM_PARTIAL);
  // Cross-check against a direct run of the runner: the ABI must report the same facts.
  const auto slots = sotc::recovered_constructors();
  auto state = std::make_unique<sotc::StartupState>();
  const sotc::CtorRunResult direct = sotc::run_constructors(slots, *state);
  CHECK(cov.ctor.slots_registered == slots.size());
  CHECK(cov.ctor.completed == direct.completed);
  CHECK(cov.ctor.blocked_on_rva == direct.blocked_on_rva);
  CHECK(cov.ctor.run_ok == (direct.ok() ? 1u : 0u));
  uint32_t with_cb = 0;
  for (const auto& sl : slots) with_cb += sl.run != nullptr;
  CHECK(cov.ctor.slots_with_callback == with_cb);
  CHECK(cov.ctor.completed <= cov.ctor.slots_registered);
  // Status carries the same prefix facts, and a ctor-run success still does not mean an engine.
  sotc_core* c = make_core();
  const sotc_status st = get_status(c);
  CHECK(st.ctor.linked == 1 && st.ctor.completed == cov.ctor.completed);
  sotc_core_destroy(c);
#else
  CHECK(cov.ctor.linked == 0 && cov.ctor.slots_registered == 0 && cov.ctor.completed == 0);
  CHECK(cov.subsystems[0].status == SOTC_SUBSYSTEM_NOT_LINKED);
#endif
  // Argument and struct_size checks.
  CHECK(sotc_core_coverage(nullptr) == SOTC_ERR_INVALID_ARGUMENT);
  cov.struct_size = sizeof cov - 1;
  CHECK(sotc_core_coverage(&cov) == SOTC_ERR_INVALID_ARGUMENT);
}

}  // namespace

int main() {
  test_version_and_messages();
  test_create();
  test_pure_validation();
  test_handle_lifecycle_never_runs();
  test_validate_arguments_do_not_change_state();
  test_concurrent_calls_never_report_running();
  test_coverage();
  std::puts("sotc_core_api: all checks passed");
  return 0;
}
