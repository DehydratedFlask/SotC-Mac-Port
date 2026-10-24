// Native lift of the 46-byte owner destructor `0x1bdae0`.
//
// ELF sha256 d0723afb…861d3. Bytes `0x1bdae0..0x1bdb0e`:
//   push rbp; mov rbp,rsp; push r14; push rbx
//   r14 = rdi                      ; rdi = &slot (holder of one owned pointer)
//   rbx = [r14]                    ; owned pointer
//   test rbx,rbx ; je  0x1bdb09    ; null => nothing to do
//   rdi = rbx ; call 0x1bede0      ; destroy the owned object (body NOT lifted)
//   rdi = rbx ; call 0x170b0       ; operator delete (body NOT lifted)
//   [r14] = 0                      ; clear the slot
//   pop rbx/r14/rbp ; ret
//
// It is registered as an `__cxa_atexit` handler by the singleton installer
// (`0x1c00e0` in the `0x1bdb10` family) and is the teardown half of those owners.
//
// NOT modelled: the bodies of `0x1bede0` and `0x170b0`; the object's own type; whether a cleared
// slot can be re-installed later. G1 PARTIAL — the holder semantics are exact, the callees are
// contracts.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kOwnerDtor1bdae0Rva = 0x1BDAE0;
inline constexpr std::size_t kOwnerDtor1bdae0Size = 46;
inline constexpr std::uint64_t kOwnerDtor1bdae0EndRva = 0x1BDB0E;
inline constexpr std::uint64_t kOwnerDtor1bdae0DestroyRva = 0x1BEDE0;
inline constexpr std::uint64_t kOwnerDtor1bdae0FreeRva = 0x170B0;

struct OwnerDtor1bdae0Hooks {
    // 0x1bede0(owned) — destroy the object. Null => the destruction is reported as not performed.
    void (*destroy)(void* owned) = nullptr;
    // 0x170b0(owned) — operator delete. Null => the free is reported as not performed.
    void (*deallocate)(void* owned) = nullptr;
};

struct OwnerDtor1bdae0Result {
    bool ok = false;          // true when the slot was already empty, or both hooks ran
    bool was_empty = false;
    bool destroyed = false;
    bool deallocated = false;
    bool slot_cleared = false;
    void* owned = nullptr;    // pointer observed in the slot before teardown
};

// Runs the recovered teardown on `slot`. A non-null owned pointer with a missing hook is a visible
// failure (ok == false); the slot is only cleared after both steps succeed.
[[nodiscard]] OwnerDtor1bdae0Result owner_dtor_1bdae0(void** slot,
                                                     const OwnerDtor1bdae0Hooks& hooks) noexcept;

}  // namespace sotc
