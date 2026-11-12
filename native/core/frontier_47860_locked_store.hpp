// Function `0x47860` (0x80 B): store a qword at `this+0x28` under the object's spin lock.
//
// This is the real target of the two call sites withdrawn in E1095. Both reload an object vptr stored
// as `lea TABLE(%rip); add $0x10`, so `call *0x50(%rax)` reads `0x1594b48` (site `0x11462`, table
// `0x1594ae8 + 0x60`) and `0x1595bc0` (site `0x113d6`, table `0x1595b60 + 0x60`); each qword is
// `0x47860` with one type-8 RELA (`tools/lift/check_frontier_target_models.py`). Whether the table
// rows return is the integrator's decision; this file is only the body model.
//
// Characterised from the ELF bytes (E1100): 0x80 B, 45 instructions, 1 direct call (`0x3b570`, a 5 B
// `jmp` to PLT `0xf8fa20` = `sceKernelUsleep`), no indirect transfer, no jump table, frame
// `push rbp; mov rbp,rsp` plus r12-r15/rbx. `lock cmpxchg`, `xchg`, `lfence`, `sfence`, `pause`.
//
//   void f(Object* this /*rdi*/, u64 value /*rsi*/) {
//       u64* lock = *(u64**)(this + 0x70);          // 0x13d60 stores `this + 0x60` there
//       if (lock == nullptr) { *(u64*)(this + 0x28) = value; return; }   // 0x478cd: no atomics
//       for (rbx = 1;; ++rbx) {
//           if (*lock == 0 && cmpxchg(lock, expected 0, desired ~0ull)) break;   // test, then CAS
//           if (rbx >= 0x2711) sceKernelUsleep(1);   // only on the failure path
//           pause;
//       }
//       lfence; *(u64*)(this + 0x28) = value; sfence; xchg(lock, 0);   // release, old value discarded
//   }
//
// The lock word is 0 = free, all-ones = held. The return value is not defined by the body (the lock
// path leaves the exchanged -1 in `rax`, the null-lock path leaves the caller's `rax`), so none is
// modelled. Fences are modelled as sequentially consistent barriers, which is stronger than
// `lfence`/`sfence` and never weaker.
//
// Fail-visible choices: the retail loop has no exit while the lock stays held, so the model gives up
// after `kFrontier47860MaxAttempts` failed attempts and reports `spin_overflow` without storing. A
// lock pointer that is not 8-aligned is refused rather than accessed.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kFrontier47860Rva = 0x47860;
inline constexpr std::size_t kFrontier47860Size = 0x80;
inline constexpr std::uint64_t kFrontier47860EndAddress = 0x478e0;
inline constexpr std::uint64_t kFrontier47860SlotAddress0 = 0x1594b48;  // 0x1594ae8 + 0x60
inline constexpr std::uint64_t kFrontier47860SlotAddress1 = 0x1595bc0;  // 0x1595b60 + 0x60
inline constexpr std::uint32_t kFrontier47860StoredFieldOffset = 0x28;
inline constexpr std::uint32_t kFrontier47860LockPointerOffset = 0x70;
inline constexpr std::uint64_t kFrontier47860UsleepPlt = 0xf8fa20;      // sceKernelUsleep via 0x3b570
inline constexpr std::uint64_t kFrontier47860UsleepThunkRva = 0x3b570;
inline constexpr std::uint64_t kFrontier47860SpinThreshold = 0x2711;
inline constexpr std::uint32_t kFrontier47860UsleepMicroseconds = 1;
inline constexpr std::uint64_t kFrontier47860LockFree = 0;
inline constexpr std::uint64_t kFrontier47860LockHeld = ~0ull;
// Not retail: bound on failed attempts so a held lock is reported instead of hanging the host.
inline constexpr std::uint64_t kFrontier47860MaxAttempts = 0x2711 + 64;

struct Frontier47860Cells {
    // Host storage for the object; at least `LockPointerOffset + 8` bytes, 8-aligned. Null is
    // fail-visible. The qword at +0x70 is a host pointer to the lock word (or null).
    std::uint8_t* object = nullptr;
};

struct Frontier47860Hooks {
    // The `pause` instruction after every failed attempt; `attempt` is `rbx` (1-based). Optional:
    // a host may use it to yield or, in a fixture, to release the lock.
    void (*pause)(std::uint64_t attempt, void* user) = nullptr;
    // `0x3b570` -> `sceKernelUsleep(1)`, called only when `attempt >= 0x2711`. Optional.
    void (*usleep)(std::uint32_t microseconds, void* user) = nullptr;
    void* user = nullptr;
};

struct Frontier47860Result {
    bool ok = false;
    bool locked = false;          // the lock path ran (false when the lock pointer was null)
    bool stored = false;          // the qword at +0x28 was written
    bool spin_overflow = false;   // gave up after MaxAttempts failed attempts; nothing stored
    bool misaligned_lock = false; // lock pointer not 8-aligned; nothing touched
    std::uint64_t failed_attempts = 0;
    std::uint64_t usleep_calls = 0;
};

[[nodiscard]] Frontier47860Result frontier_47860_locked_store(
    const Frontier47860Cells& cells, std::uint64_t value,
    const Frontier47860Hooks& hooks = {}) noexcept;

}  // namespace sotc
