// Native recovery of original constructor-table index 300 / RVA 0xacc880 (ELF sha256
// d0723afb...5861d3). 21-byte static-cleanup thunk:
//   lea rdx,[0x1604218]   ; DSO handle
//   lea rdi,[0xacc5b0]    ; destructor: one byte, `ret`
//   xor esi,esi           ; argument: nullptr
//   jmp 0xf8f390          ; __cxa_atexit
// It writes no object and cannot fail. The native model records exactly one inert cleanup entry;
// the registered destructor is a bare `ret`, so running it is a no-op with no state to release.
#pragma once
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kSlot300Rva = 0xacc880;
inline constexpr std::size_t kSlot300BodySize = 21;
inline constexpr std::uint64_t kSlot300DtorRva = 0xacc5b0;   // single `ret` (0xc3)
inline constexpr std::size_t kSlot300DtorSize = 1;
inline constexpr std::uint64_t kSlot300DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot300AtexitRva = 0xf8f390;

// One original call registers one cleanup. The counter is observable state for the fixture; it is
// not a claim about the host C++ runtime.
struct Slot300Cleanup {
    std::size_t registered = 0;
};

// Models 0xacc880. Always true; increments `registered`.
[[nodiscard]] bool init_slot300_acc880(Slot300Cleanup& cleanup) noexcept;

}  // namespace sotc
