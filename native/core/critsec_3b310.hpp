// RE-02-CRITSEC-3B310-3B380-NATIVE-01: host model of PS4 critical-section helpers at 0x3b310 / 0x3b380.
// Offline analysis only; nothing here executes an original RVA.
//
// OBS (pinned ELF, E1032 / lane_m5_check critsec_extra): 0x3b310 calls attr-init, settype(2),
// mutex-init(obj, attr, "CritSec"), attr-destroy; import return codes are discarded. 0x3b380 tail-jumps
// to scePthreadMutexDestroy.
//
// INFERENCE (not PS4-primary): type 2 is treated as PTHREAD_MUTEX_RECURSIVE on a FreeBSD-derived ABI.
// The debug name "CritSec" has no host counterpart. This module uses a plain recursive mutex with
// **no** PTHREAD_PRIO_INHERIT (unlike core/recursive_mutex.hpp).
//
// Layout: the game-facing field is exactly 8 bytes — an opaque token. The host pthread object lives in
// a process-local sidecar registry keyed by that token (Luna/E1033 sidecar proposal).
#pragma once

#include <cstddef>
#include <cstdint>

#include "ctor_tiny_misc_slots.hpp"

namespace sotc {

inline constexpr std::uint64_t kCritSecCtorRva = 0x3b310;
inline constexpr std::uint64_t kCritSecDtorRva = 0x3b380;
inline constexpr std::size_t kCritSecHandleBytes = 8;

// Models 0x3b310(obj): writes an 8-byte opaque handle at `handle_bytes`. Returns 0 on success, errno on
// failure, EALREADY if already initialized. Null `handle_bytes` => EINVAL.
[[nodiscard]] int critsec_init_at(void* handle_bytes) noexcept;

// Models 0x3b380(obj): destroys the sidecar for the handle at `handle_bytes` and zeroes the 8 bytes.
// Returns 0 on success. EINVAL if null/empty/unknown; EBUSY if held or used by another in-flight
// operation. The host rejects and retains the handle in those cases; the retail destroy behavior
// under contention is not established.
[[nodiscard]] int critsec_destroy_at(void* handle_bytes) noexcept;

// Lock/unlock for host tests and future consumers (not observed in the 0x3b310/0x3b380 shims).
[[nodiscard]] int critsec_lock_at(const void* handle_bytes) noexcept;
[[nodiscard]] int critsec_unlock_at(const void* handle_bytes) noexcept;

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
