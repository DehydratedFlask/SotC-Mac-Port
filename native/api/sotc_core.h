/*
 * sotc_core.h - versioned C ABI of the native Shadow of the Colossus game core (APP-01-CORE-ABI-01).
 *
 * HONESTY CONTRACT
 *   The core has NO engine yet. sotc_core_start() therefore never reports SOTC_STATE_RUNNING: it
 *   returns SOTC_ERR_ENGINE_NOT_IMPLEMENTED and leaves the handle in SOTC_STATE_FAILED. The value
 *   SOTC_STATE_RUNNING exists so that a launcher can be written against the final state machine,
 *   but no code path in this library can enter it until a real engine is wired in behind
 *   start(). Every "engine_*" / "implemented" field below reports measured fact, not intent.
 *
 * ABI RULES
 *   - Plain C99, no allocation crosses the boundary: strings are copied into caller buffers or
 *     fixed-size char arrays inside the result structs.
 *   - Every out-struct begins with `uint32_t struct_size`. The caller sets it to sizeof(struct) it
 *     compiled against. The library requires >= the v1 size, writes only the v1 prefix it knows,
 *     and stores the v1 size back. A larger (newer) caller struct is therefore accepted.
 *   - Version is MAJOR.MINOR. A MAJOR change breaks the ABI; MINOR only appends. sotc_core_create()
 *     rejects a caller whose MAJOR differs or whose MINOR is newer than the library's.
 *   - A handle is thread-safe: every call takes an internal lock. State-change callbacks are
 *     delivered after the lock is released, on the thread that made the call, in order for that
 *     call. They may call back into the core.
 *   - ARM64 macOS only. No x86, no guest CPU, no original executable is loaded or referenced.
 */
#ifndef SOTC_CORE_H
#define SOTC_CORE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) || defined(__clang__)
#define SOTC_API __attribute__((visibility("default")))
#else
#define SOTC_API
#endif

#define SOTC_CORE_ABI_MAJOR 1u
#define SOTC_CORE_ABI_MINOR 0u
/* Packed as (major << 16) | minor. */
#define SOTC_CORE_ABI_VERSION ((SOTC_CORE_ABI_MAJOR << 16) | SOTC_CORE_ABI_MINOR)

/* Same six archives, same order, as launcher AssetValidator.requiredArchives. */
#define SOTC_ARCHIVE_COUNT 6u
#define SOTC_NAME_CAPACITY 32u
#define SOTC_DETAIL_CAPACITY 160u
#define SOTC_SUBSYSTEM_COUNT 5u

typedef enum sotc_error {
  SOTC_OK = 0,
  SOTC_ERR_INVALID_ARGUMENT = 1,        /* NULL/too-small argument, or struct_size < v1 */
  SOTC_ERR_BAD_STATE = 2,               /* call not legal in the current state */
  SOTC_ERR_ASSETS_NOT_READY = 3,        /* start requested but validation found problems */
  SOTC_ERR_ENGINE_NOT_IMPLEMENTED = 4,  /* start(): the core has no engine; NEVER success */
  SOTC_ERR_IO = 5,                      /* assets directory itself unusable (not a directory) */
  SOTC_ERR_BUFFER_TOO_SMALL = 6,        /* reserved: copy helpers truncate and report instead */
  SOTC_ERR_ABI_MISMATCH = 7,            /* caller compiled against an incompatible ABI */
  SOTC_ERR_INTERNAL = 8                 /* allocation or unexpected C++ exception */
} sotc_error;

/* Lifecycle, from the launcher's point of view (ROADMAP APP-01). */
typedef enum sotc_state {
  SOTC_STATE_IDLE = 0,        /* created, or last validation found problems */
  SOTC_STATE_VALIDATING = 1,  /* transient, visible only to callbacks and concurrent status() */
  SOTC_STATE_READY = 2,       /* assets validated; engine availability is reported separately */
  SOTC_STATE_STARTING = 3,    /* transient, visible only to callbacks and concurrent status() */
  SOTC_STATE_RUNNING = 4,     /* UNREACHABLE today: no engine exists */
  SOTC_STATE_STOPPING = 5,
  SOTC_STATE_FAILED = 6       /* start() failed; sotc_core_status().last_error says why */
} sotc_state;

typedef enum sotc_archive_status {
  SOTC_ARCHIVE_OK = 0,
  SOTC_ARCHIVE_MISSING = 1,         /* no regular file (a directory of that name counts as missing) */
  SOTC_ARCHIVE_UNREADABLE = 2,
  SOTC_ARCHIVE_BAD_MAGIC = 3        /* first four bytes are not "PSAR" */
} sotc_archive_status;

typedef enum sotc_subsystem_status {
  SOTC_SUBSYSTEM_NOT_IMPLEMENTED = 0,
  SOTC_SUBSYSTEM_PARTIAL = 1,       /* some recovered code exists; the subsystem is not usable */
  SOTC_SUBSYSTEM_IMPLEMENTED = 2,   /* nothing reports this today */
  SOTC_SUBSYSTEM_NOT_LINKED = 3     /* this build of the ABI library cannot measure it */
} sotc_subsystem_status;

typedef struct sotc_archive_check {
  char name[SOTC_NAME_CAPACITY]; /* e.g. "pak0.psarc" */
  uint32_t status;               /* sotc_archive_status */
  uint64_t size_bytes;           /* 0 unless the file exists as a regular file */
} sotc_archive_check;

typedef struct sotc_validation_result {
  uint32_t struct_size;
  uint32_t error;                   /* verdict: SOTC_OK (assets ready), SOTC_ERR_ASSETS_NOT_READY
                                       (archive problems), SOTC_ERR_IO (dir is not a directory) */
  uint32_t assets_ready;            /* 1 iff all SOTC_ARCHIVE_COUNT archives are SOTC_ARCHIVE_OK */
  uint32_t engine_ready;            /* ALWAYS 0 today (no engine) */
  uint32_t start_would_succeed;     /* assets_ready && engine_ready, hence 0 today */
  uint32_t problem_count;           /* archives whose status != OK */
  uint32_t engine_error;            /* SOTC_ERR_ENGINE_NOT_IMPLEMENTED while engine_ready == 0 */
  sotc_archive_check archives[SOTC_ARCHIVE_COUNT]; /* required order */
  char detail[SOTC_DETAIL_CAPACITY]; /* first problem, or engine-not-implemented note */
} sotc_validation_result;

/* Constructor-runner prefix facts (native/core/startup_ctors). Meaningful only when linked. */
typedef struct sotc_ctor_prefix {
  uint32_t linked;           /* 1 iff the ABI library was built against the native core library */
  uint32_t slots_registered; /* entries in recovered_constructors(), including explicit blockers */
  uint32_t slots_with_callback;
  uint32_t completed;        /* slots the runner executed before stopping */
  uint32_t run_ok;           /* 1 iff the runner reached the end of the table without a blocker */
  uint32_t callback_failed;
  uint32_t blocked_on_rva;   /* original RVA of the first unrecovered slot; 0 if run_ok */
} sotc_ctor_prefix;

typedef struct sotc_status {
  uint32_t struct_size;
  uint32_t state;            /* sotc_state */
  uint32_t last_error;       /* sotc_error of the most recent failed call on this handle */
  uint32_t assets_validated; /* a validation has completed on this handle */
  uint32_t assets_ready;     /* the most recent validation on this handle passed */
  uint32_t engine_implemented; /* ALWAYS 0 today */
  sotc_ctor_prefix ctor;
} sotc_status;

typedef struct sotc_subsystem_report {
  char name[SOTC_NAME_CAPACITY];
  uint32_t status; /* sotc_subsystem_status */
  char detail[SOTC_DETAIL_CAPACITY];
} sotc_subsystem_report;

typedef struct sotc_coverage_report {
  uint32_t struct_size;
  uint32_t abi_version;
  uint32_t engine_implemented;   /* ALWAYS 0 today */
  uint32_t subsystem_count;      /* SOTC_SUBSYSTEM_COUNT */
  sotc_ctor_prefix ctor;
  sotc_subsystem_report subsystems[SOTC_SUBSYSTEM_COUNT];
} sotc_coverage_report;

typedef struct sotc_core sotc_core; /* opaque handle */

/* Called after a state change, outside the core's lock. `from != to`. */
typedef void (*sotc_state_callback)(void* user, sotc_state from, sotc_state to);

/* ---- version and messages (no handle needed) ---- */
SOTC_API uint32_t sotc_core_abi_version(void); /* SOTC_CORE_ABI_VERSION of the library */
/* The code/state parameters are plain uint32_t (pass a sotc_error / sotc_state value) so that an
 * out-of-range value from a C caller is well defined instead of undefined behaviour. */
SOTC_API const char* sotc_core_error_name(uint32_t code);  /* static, e.g. "SOTC_ERR_BAD_STATE" */
SOTC_API const char* sotc_core_state_name(uint32_t state); /* static, e.g. "ready" */
/* Copy the fixed human message for `code` into buf (always NUL-terminated when cap > 0,
 * truncated if needed). Returns the untruncated length, like snprintf. buf may be NULL. */
SOTC_API size_t sotc_core_error_message(uint32_t code, char* buf, size_t cap);

/* ---- lifecycle ---- */
/* `abi_version_expected` is the SOTC_CORE_ABI_VERSION the caller compiled against. */
SOTC_API sotc_error sotc_core_create(uint32_t abi_version_expected, sotc_core** out_core);
/* Releases the handle. Stops first if needed. NULL is a no-op. Not callable concurrently with
 * other calls on the same handle. */
SOTC_API void sotc_core_destroy(sotc_core* core);
/* At most one callback; pass NULL to clear. */
SOTC_API sotc_error sotc_core_set_state_callback(sotc_core* core, sotc_state_callback cb, void* user);

/* Check the six archives in assets_dir. With core == NULL this is a pure check. With a handle it
 * also moves it IDLE/READY/FAILED -> VALIDATING -> READY (all archives OK) or IDLE (problems)
 * and remembers the directory for start(). Returns SOTC_OK whenever the check itself ran, even if
 * the assets are not ready (read result->assets_ready); BAD_STATE while STARTING/RUNNING/STOPPING. */
SOTC_API sotc_error sotc_core_validate(sotc_core* core, const char* assets_dir, sotc_validation_result* out);

/* READY or FAILED (with assets still ready) -> STARTING -> FAILED, returning
 * SOTC_ERR_ENGINE_NOT_IMPLEMENTED. NEVER returns SOTC_OK and never enters RUNNING while the core
 * has no engine. IDLE after a failed validation returns SOTC_ERR_ASSETS_NOT_READY; any other
 * state returns SOTC_ERR_BAD_STATE. */
SOTC_API sotc_error sotc_core_start(sotc_core* core);

/* Idempotent. RUNNING/STARTING -> STOPPING -> READY. In every other state it is an accepted no-op
 * that changes nothing (there is nothing to stop). Returns SOTC_OK. */
SOTC_API sotc_error sotc_core_stop(sotc_core* core);

SOTC_API sotc_error sotc_core_status(sotc_core* core, sotc_status* out);
/* Detail text for the most recent failure on this handle ("" if none). Returns untruncated length. */
SOTC_API size_t sotc_core_last_error_message(sotc_core* core, char* buf, size_t cap);

/* ---- coverage (no handle needed) ---- */
SOTC_API sotc_error sotc_core_coverage(sotc_coverage_report* out);

#ifdef __cplusplus
}
#endif
#endif /* SOTC_CORE_H */
