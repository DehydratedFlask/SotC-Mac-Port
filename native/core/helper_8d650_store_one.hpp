// CPU-01-HELPER-CALLEE-MODELS-03: native model of the 7 byte leaf 0x8d650 (ELF sha256 d0723afb...5861d3), called once by
// startup constructor slot 497 (0x262a80, at 0x262ab1, rdi = the display-configuration cell 0x2eec4d8):
//
//   0x8d650: c7 07 01 00 00 00   mov dword [rdi], 1
//   0x8d656: c3                  ret
//
// It reads nothing, calls nothing and writes exactly the four bytes at `rdi` (rax is not written, flags are not read).
// The meaning of the cell is NOT recovered here (INFERENCE only: slot 497 stores 0x780 / 0x438 -- 1920 / 1080 -- at the
// two dwords that follow, so the cell is plausibly an "enabled" flag of a display record). Byte-pinned by
// tools/lift/check_helper_8d650.py; nothing here executes or calls an original RVA.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kStoreOne8d650Rva = 0x8d650;
inline constexpr std::size_t kStoreOne8d650Size = 7;
inline constexpr std::uint32_t kStoreOne8d650Value = 1;

// 0x8d650: `*cell = 1`. Only those four bytes are written.
void store_one_8d650(std::uint32_t& cell) noexcept;

}  // namespace sotc
