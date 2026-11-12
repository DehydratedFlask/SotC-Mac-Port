// Function `0x318f0` (8 B): "the Fios op finished without error" predicate.
//
// Real target of the withdrawn `vptr+0xa0` calls (`0x312de`, `0x3134e`): `0x15954a8` holds `0x318f0`
// (one type-8 RELA, `tools/lift/check_frontier_target_models.py`). Body, byte-exact:
//   `83 7f 5c 00` cmpl $0, 0x5c(%rdi);  `0f 94 c0` sete %al;  `c3` retq
// i.e. `return *(i32*)(this + 0x5c) == 0;`. The dword at +0x5c is written by `0x31900` from
// `sceFiosOpGetError`, so this reads "the last completed op reported no error" (inferred from the
// two bodies; the field name is not otherwise evidenced). Only `%al` is defined by the body; the
// other bits of `rax` are whatever the caller left, and no caller of this slot reads them.
// 3 instructions (private listing). Memory effects: one 4-byte read, no writes, no calls, no indirect transfer, no stack frame.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kFrontier318f0Rva = 0x318f0;
inline constexpr std::size_t kFrontier318f0Size = 0x8;
inline constexpr std::uint64_t kFrontier318f0EndAddress = 0x318f8;
inline constexpr std::uint64_t kFrontier318f0SlotAddress = 0x15954a8;  // 0x15953f8 + 0x10 + 0xa0
inline constexpr std::uint32_t kFrontier318f0ErrorOffset = 0x5c;

struct Frontier318f0Cells {
    const std::uint8_t* object = nullptr;  // at least 0x60 bytes; null is fail-visible
};

struct Frontier318f0Result {
    bool ok = false;
    bool value = false;          // the returned `al`
    std::int32_t error = 0;      // the dword read at +0x5c
};

[[nodiscard]] Frontier318f0Result frontier_318f0_fios_succeeded(
    const Frontier318f0Cells& cells = {}) noexcept;

}  // namespace sotc
