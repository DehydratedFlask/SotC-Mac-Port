// Implementation of the versioned C ABI declared in sotc_core.h (APP-01-CORE-ABI-01).
//
// There is deliberately no engine behind start(): it always fails with
// SOTC_ERR_ENGINE_NOT_IMPLEMENTED and the state machine below has no edge into RUNNING.
#include "sotc_core.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <memory>
#include <mutex>
#include <new>
#include <string>
#include <utility>
#include <vector>

#if SOTC_API_WITH_CORE_LIB
#include "core/startup_ctors.hpp"
#endif

namespace {

constexpr const char* kArchiveNames[SOTC_ARCHIVE_COUNT] = {
    "cachedFiles.psarc", "pak0.psarc", "pak1.psarc", "pak2.psarc", "pak3.psarc", "pakN.psarc",
};

template <size_t N>
void copy_text(char (&dst)[N], const std::string& src) {
  const size_t n = src.size() < N - 1 ? src.size() : N - 1;
  std::memcpy(dst, src.data(), n);
  dst[n] = '\0';
}

size_t copy_out(const std::string& src, char* buf, size_t cap) {
  if (buf != nullptr && cap > 0) {
    const size_t n = src.size() < cap - 1 ? src.size() : cap - 1;
    std::memcpy(buf, src.data(), n);
    buf[n] = '\0';
  }
  return src.size();
}

const char* error_message(uint32_t code) {
  switch (code) {
    case SOTC_OK: return "ok";
    case SOTC_ERR_INVALID_ARGUMENT:
      return "invalid argument (null pointer, empty path, or struct_size smaller than the v1 layout)";
    case SOTC_ERR_BAD_STATE: return "operation is not valid in the current core state";
    case SOTC_ERR_ASSETS_NOT_READY:
      return "the most recent asset validation found problems; fix the assets and validate again";
    case SOTC_ERR_ENGINE_NOT_IMPLEMENTED:
      return "the native game engine is not implemented in this build; the game cannot be started";
    case SOTC_ERR_IO: return "the assets path is not a usable directory";
    case SOTC_ERR_BUFFER_TOO_SMALL: return "output buffer too small";
    case SOTC_ERR_ABI_MISMATCH: return "caller was built against an incompatible core ABI version";
    case SOTC_ERR_INTERNAL: return "internal error (allocation failure or unexpected exception)";
    default: return "unknown error code";
  }
}

// ---- constructor-runner prefix (only measurable when the native core library is linked) ----

sotc_ctor_prefix measure_ctor_prefix() noexcept {
  sotc_ctor_prefix p;
  std::memset(&p, 0, sizeof p);
#if SOTC_API_WITH_CORE_LIB
  static std::once_flag once;
  static sotc_ctor_prefix cached;
  std::call_once(once, [] {
    std::memset(&cached, 0, sizeof cached);
    cached.linked = 1;
    try {
      const auto slots = sotc::recovered_constructors();
      cached.slots_registered = static_cast<uint32_t>(slots.size());
      for (const sotc::CtorSlot& s : slots)
        if (s.run != nullptr) ++cached.slots_with_callback;
      auto state = std::make_unique<sotc::StartupState>();  // heap: the state is large
      const sotc::CtorRunResult r = sotc::run_constructors(slots, *state);
      cached.completed = static_cast<uint32_t>(r.completed);
      cached.run_ok = r.ok() ? 1u : 0u;
      cached.callback_failed = r.callback_failed ? 1u : 0u;
      cached.blocked_on_rva = r.blocked_on_rva;
    } catch (...) {
      // Leave counters at zero; linked stays 1 so the failure is visible as "completed 0, not ok".
    }
  });
  p = cached;
#endif
  return p;
}

std::string ctor_note(const sotc_ctor_prefix& c) {
  char b[160];
  if (!c.linked) {
    std::snprintf(b, sizeof b, "constructor runner not linked into this ABI build");
  } else if (c.run_ok) {
    std::snprintf(b, sizeof b, "%u of %u registered constructor slots ran; table end reached", c.completed,
                  c.slots_registered);
  } else {
    std::snprintf(b, sizeof b, "%u constructor slots ran, then blocked at original RVA 0x%x", c.completed,
                  c.blocked_on_rva);
  }
  return b;
}

// ---- coverage detail: the constructor table measured against the original 573-slot list ----

std::string ctor_coverage_detail(const sotc_ctor_prefix& c) {
  if (!c.linked) {
    return "ABI library built without the native core library; not measured";
  }
#if SOTC_API_WITH_CORE_LIB
  constexpr std::uint32_t kOriginalSlots = 573;
  bool present[kOriginalSlots] = {};
  std::uint32_t recovered = 0;
  for (const sotc::CtorSlot& slot : sotc::recovered_constructors()) {
    if (slot.original_index < kOriginalSlots && !present[slot.original_index]) {
      present[slot.original_index] = true;
      ++recovered;
    }
  }
  bool has_first = false;
  std::uint32_t first = 0;
  for (std::uint32_t i = 0; i < kOriginalSlots; ++i) {
    if (!present[i]) {
      has_first = true;
      first = i;
      break;
    }
  }
  const std::string first_text = has_first ? std::to_string(first) : std::string("none");
  std::string detail = std::to_string(recovered) + " of " + std::to_string(kOriginalSlots) +
                       " original constructor slots recovered; first unrecovered index " + first_text + "; ";
  if (c.run_ok) {
    detail += std::to_string(c.completed) + " registered slots ran OK";
  } else if (c.callback_failed) {
    char rva[16];
    std::snprintf(rva, sizeof rva, "0x%x", c.blocked_on_rva);
    detail += std::to_string(c.completed) + " registered slots ran, a callback failed at original RVA " + rva;
  } else {
    char rva[16];
    std::snprintf(rva, sizeof rva, "0x%x", c.blocked_on_rva);
    detail += std::to_string(c.completed) + " registered slots ran, blocked at original RVA " + rva;
  }
  return detail;
#else
  return "ABI library built without the native core library; not measured";
#endif
}

// ---- asset validation (same checks and wording as the launcher's AssetValidator) ----

void run_validation(const std::string& dir, sotc_validation_result& r) {
  std::memset(&r, 0, sizeof r);
  r.struct_size = sizeof r;
  r.engine_ready = 0;
  r.start_would_succeed = 0;
  r.engine_error = SOTC_ERR_ENGINE_NOT_IMPLEMENTED;

  struct stat dst;
  const bool dir_ok = ::stat(dir.c_str(), &dst) == 0 && S_ISDIR(dst.st_mode);
  std::string first_problem;
  for (uint32_t i = 0; i < SOTC_ARCHIVE_COUNT; ++i) {
    sotc_archive_check& a = r.archives[i];
    std::snprintf(a.name, sizeof a.name, "%s", kArchiveNames[i]);
    a.status = SOTC_ARCHIVE_OK;
    std::string problem;
    struct stat st;
    const std::string path = dir + "/" + kArchiveNames[i];
    if (!dir_ok || ::stat(path.c_str(), &st) != 0 || !S_ISREG(st.st_mode)) {
      a.status = SOTC_ARCHIVE_MISSING;
      problem = std::string(kArchiveNames[i]) + " not found";
    } else {
      a.size_bytes = static_cast<uint64_t>(st.st_size);
      const int fd = ::open(path.c_str(), O_RDONLY);
      if (fd < 0) {
        a.status = SOTC_ARCHIVE_UNREADABLE;
        problem = std::string(kArchiveNames[i]) + " is not readable";
      } else {
        char magic[4] = {0, 0, 0, 0};
        const ssize_t got = ::read(fd, magic, sizeof magic);
        ::close(fd);
        if (got != 4 || std::memcmp(magic, "PSAR", 4) != 0) {
          a.status = SOTC_ARCHIVE_BAD_MAGIC;
          problem = std::string(kArchiveNames[i]) + " is not a PSARC archive";
        }
      }
    }
    if (a.status != SOTC_ARCHIVE_OK) {
      ++r.problem_count;
      if (first_problem.empty()) first_problem = problem;
    }
  }
  r.assets_ready = r.problem_count == 0 ? 1u : 0u;
  if (!dir_ok) {
    r.error = SOTC_ERR_IO;
    copy_text(r.detail, "assets path is not a directory: " + dir);
  } else if (r.assets_ready) {
    r.error = SOTC_OK;
    copy_text(r.detail, "assets ready; native engine not implemented, so start() will fail");
  } else {
    r.error = SOTC_ERR_ASSETS_NOT_READY;
    copy_text(r.detail, first_problem);
  }
}

bool v1_out_ok(const void* out, uint32_t struct_size, size_t v1_size) {
  return out != nullptr && struct_size >= v1_size;
}

}  // namespace

struct sotc_core {
  struct Change { sotc_state from, to; };

  std::mutex m;
  sotc_state state = SOTC_STATE_IDLE;
  uint32_t last_error = SOTC_OK;
  std::string last_message;
  bool validated = false;
  bool assets_ready = false;
  std::string assets_dir;
  sotc_state_callback cb = nullptr;
  void* cb_user = nullptr;
  std::vector<Change> pending;  // guarded by m; delivered by flush() after unlock

  // Caller holds m.
  void set_state(sotc_state next) {
    if (next == state) return;
    pending.push_back({state, next});
    state = next;
  }
  void fail(uint32_t code, std::string msg) {
    last_error = code;
    last_message = std::move(msg);
  }
  // Takes ownership of the lock, releases it, then delivers queued callbacks in order.
  void flush(std::unique_lock<std::mutex>& lock) {
    std::vector<Change> todo;
    todo.swap(pending);
    const sotc_state_callback f = cb;
    void* const user = cb_user;
    lock.unlock();
    if (f != nullptr)
      for (const Change& c : todo) f(user, c.from, c.to);
  }
};

extern "C" {

uint32_t sotc_core_abi_version(void) { return SOTC_CORE_ABI_VERSION; }

const char* sotc_core_error_name(uint32_t code) {
  switch (code) {
    case SOTC_OK: return "SOTC_OK";
    case SOTC_ERR_INVALID_ARGUMENT: return "SOTC_ERR_INVALID_ARGUMENT";
    case SOTC_ERR_BAD_STATE: return "SOTC_ERR_BAD_STATE";
    case SOTC_ERR_ASSETS_NOT_READY: return "SOTC_ERR_ASSETS_NOT_READY";
    case SOTC_ERR_ENGINE_NOT_IMPLEMENTED: return "SOTC_ERR_ENGINE_NOT_IMPLEMENTED";
    case SOTC_ERR_IO: return "SOTC_ERR_IO";
    case SOTC_ERR_BUFFER_TOO_SMALL: return "SOTC_ERR_BUFFER_TOO_SMALL";
    case SOTC_ERR_ABI_MISMATCH: return "SOTC_ERR_ABI_MISMATCH";
    case SOTC_ERR_INTERNAL: return "SOTC_ERR_INTERNAL";
    default: return "SOTC_ERR_UNKNOWN";
  }
}

const char* sotc_core_state_name(uint32_t state) {
  switch (state) {
    case SOTC_STATE_IDLE: return "idle";
    case SOTC_STATE_VALIDATING: return "validating";
    case SOTC_STATE_READY: return "ready";
    case SOTC_STATE_STARTING: return "starting";
    case SOTC_STATE_RUNNING: return "running";
    case SOTC_STATE_STOPPING: return "stopping";
    case SOTC_STATE_FAILED: return "failed";
    default: return "unknown";
  }
}

size_t sotc_core_error_message(uint32_t code, char* buf, size_t cap) {
  return copy_out(error_message(code), buf, cap);
}

sotc_error sotc_core_create(uint32_t abi_version_expected, sotc_core** out_core) {
  if (out_core == nullptr) return SOTC_ERR_INVALID_ARGUMENT;
  *out_core = nullptr;
  const uint32_t major = abi_version_expected >> 16;
  const uint32_t minor = abi_version_expected & 0xffffu;
  if (major != SOTC_CORE_ABI_MAJOR || minor > SOTC_CORE_ABI_MINOR) return SOTC_ERR_ABI_MISMATCH;
  try {
    *out_core = new sotc_core();
  } catch (...) {
    return SOTC_ERR_INTERNAL;
  }
  return SOTC_OK;
}

void sotc_core_destroy(sotc_core* core) {
  if (core == nullptr) return;
  (void)sotc_core_stop(core);
  delete core;
}

sotc_error sotc_core_set_state_callback(sotc_core* core, sotc_state_callback cb, void* user) {
  if (core == nullptr) return SOTC_ERR_INVALID_ARGUMENT;
  std::lock_guard<std::mutex> g(core->m);
  core->cb = cb;
  core->cb_user = user;
  return SOTC_OK;
}

sotc_error sotc_core_validate(sotc_core* core, const char* assets_dir, sotc_validation_result* out) {
  if (assets_dir == nullptr || assets_dir[0] == '\0' || out == nullptr ||
      out->struct_size < sizeof(sotc_validation_result))
    return SOTC_ERR_INVALID_ARGUMENT;
  try {
    const std::string dir(assets_dir);
    if (core == nullptr) {
      run_validation(dir, *out);
      return SOTC_OK;
    }
    {
      std::unique_lock<std::mutex> lock(core->m);
      switch (core->state) {
        case SOTC_STATE_IDLE:
        case SOTC_STATE_READY:
        case SOTC_STATE_FAILED: break;
        default:
          core->fail(SOTC_ERR_BAD_STATE, "validate() is not valid while the core is " +
                                             std::string(sotc_core_state_name(core->state)));
          return SOTC_ERR_BAD_STATE;
      }
      core->set_state(SOTC_STATE_VALIDATING);
      core->flush(lock);
    }
    sotc_validation_result r;
    run_validation(dir, r);  // filesystem I/O happens outside the lock
    {
      std::unique_lock<std::mutex> lock(core->m);
      core->validated = true;
      core->assets_ready = r.assets_ready != 0;
      core->assets_dir = dir;
      if (core->assets_ready) {
        core->last_error = SOTC_OK;
        core->last_message.clear();
      } else {
        core->fail(r.error, r.detail);
      }
      core->set_state(core->assets_ready ? SOTC_STATE_READY : SOTC_STATE_IDLE);
      core->flush(lock);
    }
    *out = r;
    return SOTC_OK;
  } catch (...) {
    return SOTC_ERR_INTERNAL;
  }
}

sotc_error sotc_core_start(sotc_core* core) {
  if (core == nullptr) return SOTC_ERR_INVALID_ARGUMENT;
  try {
    std::unique_lock<std::mutex> lock(core->m);
    if (core->state == SOTC_STATE_IDLE && core->validated && !core->assets_ready) {
      core->fail(SOTC_ERR_ASSETS_NOT_READY, "start() refused: " + core->last_message);
      return SOTC_ERR_ASSETS_NOT_READY;
    }
    if (core->state != SOTC_STATE_READY && !(core->state == SOTC_STATE_FAILED && core->assets_ready)) {
      core->fail(SOTC_ERR_BAD_STATE, "start() is not valid while the core is " +
                                         std::string(sotc_core_state_name(core->state)) +
                                         "; validate the assets first");
      return SOTC_ERR_BAD_STATE;
    }
    core->set_state(SOTC_STATE_STARTING);
    // An engine would be brought up here. None exists, so this is the only possible outcome and
    // the machine goes to FAILED, never RUNNING.
    core->fail(SOTC_ERR_ENGINE_NOT_IMPLEMENTED,
               std::string(error_message(SOTC_ERR_ENGINE_NOT_IMPLEMENTED)) + " (" +
                   ctor_note(measure_ctor_prefix()) + ")");
    core->set_state(SOTC_STATE_FAILED);
    core->flush(lock);
    return SOTC_ERR_ENGINE_NOT_IMPLEMENTED;
  } catch (...) {
    return SOTC_ERR_INTERNAL;
  }
}

sotc_error sotc_core_stop(sotc_core* core) {
  if (core == nullptr) return SOTC_ERR_INVALID_ARGUMENT;
  try {
    std::unique_lock<std::mutex> lock(core->m);
    if (core->state == SOTC_STATE_RUNNING || core->state == SOTC_STATE_STARTING) {
      // Unreachable today (no engine); kept so the machine is complete for a future engine.
      core->set_state(SOTC_STATE_STOPPING);
      core->set_state(SOTC_STATE_READY);
    }
    core->flush(lock);
    return SOTC_OK;
  } catch (...) {
    return SOTC_ERR_INTERNAL;
  }
}

sotc_error sotc_core_status(sotc_core* core, sotc_status* out) {
  if (core == nullptr || !v1_out_ok(out, out ? out->struct_size : 0, sizeof(sotc_status)))
    return SOTC_ERR_INVALID_ARGUMENT;
  const sotc_ctor_prefix ctor = measure_ctor_prefix();
  std::lock_guard<std::mutex> g(core->m);
  std::memset(out, 0, sizeof *out);
  out->struct_size = sizeof *out;
  out->state = core->state;
  out->last_error = core->last_error;
  out->assets_validated = core->validated ? 1u : 0u;
  out->assets_ready = core->assets_ready ? 1u : 0u;
  out->engine_implemented = 0;
  out->ctor = ctor;
  return SOTC_OK;
}

size_t sotc_core_last_error_message(sotc_core* core, char* buf, size_t cap) {
  if (core == nullptr) return copy_out("", buf, cap);
  std::lock_guard<std::mutex> g(core->m);
  return copy_out(core->last_message, buf, cap);
}

sotc_error sotc_core_coverage(sotc_coverage_report* out) {
  if (!v1_out_ok(out, out ? out->struct_size : 0, sizeof(sotc_coverage_report)))
    return SOTC_ERR_INVALID_ARGUMENT;
  std::memset(out, 0, sizeof *out);
  out->struct_size = sizeof *out;
  out->abi_version = SOTC_CORE_ABI_VERSION;
  out->engine_implemented = 0;
  out->subsystem_count = SOTC_SUBSYSTEM_COUNT;
  out->ctor = measure_ctor_prefix();

  struct Row { const char* name; uint32_t status; std::string detail; };
  const Row rows[SOTC_SUBSYSTEM_COUNT] = {
      {"startup_constructors", out->ctor.linked ? SOTC_SUBSYSTEM_PARTIAL : SOTC_SUBSYSTEM_NOT_LINKED,
       ctor_coverage_detail(out->ctor)},
      {"engine_boot", SOTC_SUBSYSTEM_NOT_IMPLEMENTED,
       "no engine; sotc_core_start() always returns SOTC_ERR_ENGINE_NOT_IMPLEMENTED"},
      {"metal_renderer", SOTC_SUBSYSTEM_NOT_IMPLEMENTED,
       "no Metal renderer: the offscreen capture provider exists but no game frame is rendered; "
       "G3 NOT RUN"},
      {"audio", SOTC_SUBSYSTEM_NOT_IMPLEMENTED, "no audio output or mixing code in native/"},
      {"save_load", SOTC_SUBSYSTEM_PARTIAL,
       "fixture-verified USR-DATA transaction and metadata (E601/E604/E606); no platform save or real PS4 "
       "acceptance"},
  };
  for (uint32_t i = 0; i < SOTC_SUBSYSTEM_COUNT; ++i) {
    copy_text(out->subsystems[i].name, rows[i].name);
    out->subsystems[i].status = rows[i].status;
    copy_text(out->subsystems[i].detail, rows[i].detail);
  }
  return SOTC_OK;
}

}  // extern "C"
