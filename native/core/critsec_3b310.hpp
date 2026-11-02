// RE-02-CRITSEC-3B310-3B380-NATIVE-01: host model of PS4 critical-section helpers at 0x3b310 / 0x3b380.
// Offline analysis only; nothing here executes an original RVA.
//
// ============================== OBSERVED (pinned bytes, FDE 0x3b310 + 107) ==============================
// The body is a straight-line shim with **exactly five** relative calls and no conditional branch:
//
//   0x3b335  call 0xf8f920  scePthreadMutexattrInit(&attr)                 ; attr = rsp scratch
//   0x3b33a  movl $0x2, %esi
//   0x3b342  call 0xf8f930  scePthreadMutexattrSettype(&attr, 2)
//   0x3b347  leaq "CritSec" (0xf97deb), %rdx
//   0x3b354  call 0xf8f8d0  scePthreadMutexInit(obj, &attr, "CritSec")     ; obj = the incoming rdi
//   0x3b35c  call 0xf8f910  scePthreadMutexattrDestroy(&attr)
//   0x3b375  call 0xf8f3a0  __stack_chk_fail                               ; canary guard, only on mismatch
//
// Every return code is discarded (`testq %rdi,%rdi`-style checks do not exist); the imported symbols are
// identified by the pinned import manifest (tools/lift/lane_m5_check.critsec_extra pins the four PLT
// targets and the "CritSec" bytes).
//
// 0x3b380 is a 5-byte FDE and one relative jump: `jmp 0xf8f8c0` = scePthreadMutexDestroy. It forwards rdi
// unchanged, so the game-facing cell it destroys is exactly the cell 0x3b310 filled.
//
// ABSENCE OF PRIORITY INHERITANCE IS BYTE EVIDENCE, NOT AN INFERENCE: the complete call census of 0x3b310
// is {attrInit, attrSettype, mutexInit, attrDestroy, stack_chk_fail}. There is no
// scePthreadMutexattrSetprotocol and no scePthreadMutexattrSetprioceiling call, so no PRIO_INHERIT and no
// PRIO_PROTECT attribute can be requested by this shim. (core/recursive_mutex.hpp, by contrast, calls
// pthread_mutexattr_setprotocol(PTHREAD_PRIO_INHERIT); CritSec deliberately does not.)
//
// ============================== LAYOUT: 8-BYTE OPAQUE HANDLE ==========================================
// The shim performs **no store** to the caller buffer; it hands rdi straight to scePthreadMutexInit as the
// mutex out-parameter, so the game object holds the libkernel mutex handle and the API writes it. Two
// independent byte facts fix the width at 8:
//
//   slot 535 (0x138e20): the handle cell 0x2ee14f0 is passed to 0x3b310, registered with atexit as
//       `0x3b380(0x2ee14f0)`, and the *next* game field starts at 0x2ee14f8 — exactly 8 bytes later.
//   slot 414 (0x57b9b0): after 0x3b310(0x2f35898), the constructor writes u32 0 at 0x2f358a0 (= base + 8)
//       and 16 zero bytes at base + 0x10, so the handle window is [base, base + 8).
//
// ============================== INFERENCE (not PS4-primary) ==========================================
// Type 2 is read as PTHREAD_MUTEX_RECURSIVE. That is the FreeBSD-derived (PS4) numbering, not the glibc
// one (Linux uses 1), so the mapping is an inference from the pinned constant 2 plus the platform ABI.
// The host model therefore uses PTHREAD_MUTEX_RECURSIVE; the debug name "CritSec" has no host counterpart.
//
// ============================== HOST MODEL ===========================================================
// The game-facing field is exactly 8 bytes: an opaque process-local token. The host pthread object lives in
// a sidecar registry keyed by that token and is never copied or moved after pthread_mutex_init
// (E1043/E1044). Every lock/unlock/destroy operation takes a lease on the sidecar and touches pthread state
// outside the registry lock, so a blocking waiter cannot deadlock the owner's unlock.
#pragma once

#include <cstddef>
#include <cstdint>

#include "ctor_tiny_misc_slots.hpp"

namespace sotc {

inline constexpr std::uint64_t kCritSecCtorRva = 0x3b310;
inline constexpr std::uint64_t kCritSecDtorRva = 0x3b380;
inline constexpr std::size_t kCritSecHandleBytes = 8;

// Byte-pinned shape of the two bodies (checked by tools/lift/check_critsec_3b310_3b380.py).
inline constexpr std::size_t kCritSecCtorFdeLength = 107;
inline constexpr std::size_t kCritSecDtorFdeLength = 5;
inline constexpr std::uint32_t kCritSecSettypeValue = 2;      // esi of scePthreadMutexattrSettype
inline constexpr std::uint64_t kCritSecNameRva = 0xf97deb;    // "CritSec"
inline constexpr bool kCritSecUsesPriorityInheritance = false;

// Models 0x3b310(obj): writes an 8-byte opaque handle at `handle_bytes`. Returns 0 on success, errno on
// failure, EALREADY only when the cell already holds a *live* token (a real double initialisation).
//
// The retail shim never reads the field - it passes it straight to scePthreadMutexInit, which overwrites
// it - so a non-zero cell that is not a live token is overwritten, not rejected. Null `handle_bytes`
// => EINVAL.
//
// Failure atomicity (E1055 review): on every failure return no sidecar is registered and no pthread mutex
// stays initialized, so a caller may retry. The retail shim has no such branches at all; these are host
// detection only and must never be read as recovered retail error semantics.
[[nodiscard]] int critsec_init_at(void* handle_bytes) noexcept;

// Models 0x3b380(obj): destroys the sidecar for the handle at `handle_bytes` and zeroes the 8 bytes.
// Returns 0 on success. EINVAL if null/empty/unknown; EBUSY if held or used by another in-flight
// operation. The host rejects and retains the handle in those cases; the retail destroy behaviour
// under contention is not established.
[[nodiscard]] int critsec_destroy_at(void* handle_bytes) noexcept;

// Lock/unlock for host tests and future consumers (not observed in the 0x3b310/0x3b380 shims).
[[nodiscard]] int critsec_lock_at(const void* handle_bytes) noexcept;
[[nodiscard]] int critsec_unlock_at(const void* handle_bytes) noexcept;

// Host diagnostic (no retail counterpart): number of live sidecars. Fixtures use it to prove that a
// failed init or a rejected destroy leaves no registry growth.
[[nodiscard]] std::size_t critsec_live_sidecars() noexcept;

// Thin wrapper for an embedded 8-byte handle (slot 535 / hook targets).
struct CritSecHandle {
    std::uint8_t bytes[kCritSecHandleBytes]{};

    [[nodiscard]] int initialize() noexcept { return critsec_init_at(bytes); }
    [[nodiscard]] int destroy() noexcept { return critsec_destroy_at(bytes); }
    [[nodiscard]] int lock() noexcept { return critsec_lock_at(bytes); }
    [[nodiscard]] int unlock() noexcept { return critsec_unlock_at(bytes); }
    [[nodiscard]] int close() noexcept { return destroy(); }
};

// Default hooks wiring critsec_init_at / critsec_destroy_at for Slot414 / tiny slot 535 paths.
[[nodiscard]] Slot414Hooks default_critsec_slot414_hooks() noexcept;

}  // namespace sotc
