#include "frontier_47860_locked_store.hpp"

#include <cstring>

namespace sotc {

Frontier47860Result frontier_47860_locked_store(const Frontier47860Cells& cells, std::uint64_t value,
                                                const Frontier47860Hooks& hooks) noexcept {
    Frontier47860Result out{};
    if (cells.object == nullptr) {
        return out;
    }
    std::uint64_t* lock = nullptr;
    std::memcpy(&lock, cells.object + kFrontier47860LockPointerOffset, sizeof(lock));
    std::uint8_t* const field = cells.object + kFrontier47860StoredFieldOffset;

    if (lock == nullptr) {  // 0x478cd
        std::memcpy(field, &value, sizeof(value));
        out.stored = true;
        out.ok = true;
        return out;
    }
    if ((reinterpret_cast<std::uintptr_t>(lock) & 7u) != 0) {
        out.misaligned_lock = true;
        return out;
    }

    out.locked = true;
    std::uint64_t attempt = 1;  // rbx
    for (;;) {
        // `cmp qword [r12],0; jne` then `lock cmpxchg [r12], -1` with rax = 0.
        if (__atomic_load_n(lock, __ATOMIC_RELAXED) == kFrontier47860LockFree) {
            std::uint64_t expected = kFrontier47860LockFree;
            if (__atomic_compare_exchange_n(lock, &expected, kFrontier47860LockHeld, false,
                                            __ATOMIC_SEQ_CST, __ATOMIC_RELAXED)) {
                break;
            }
        }
        ++out.failed_attempts;
        if (out.failed_attempts >= kFrontier47860MaxAttempts) {
            out.spin_overflow = true;
            return out;
        }
        if (attempt >= kFrontier47860SpinThreshold) {
            if (hooks.usleep != nullptr) {
                hooks.usleep(kFrontier47860UsleepMicroseconds, hooks.user);
            }
            ++out.usleep_calls;
        }
        if (hooks.pause != nullptr) {
            hooks.pause(attempt, hooks.user);
        }
        ++attempt;
    }

    __atomic_thread_fence(__ATOMIC_SEQ_CST);  // lfence
    std::memcpy(field, &value, sizeof(value));
    out.stored = true;
    __atomic_thread_fence(__ATOMIC_SEQ_CST);  // sfence
    __atomic_exchange_n(lock, kFrontier47860LockFree, __ATOMIC_SEQ_CST);  // xchg [r12], 0
    out.ok = true;
    return out;
}

}  // namespace sotc
