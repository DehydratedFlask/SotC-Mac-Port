// Native leaf for original helper RVA 0x357f0 (8 B; ELF sha256 d0723afb…861d3):
//   movzx eax, sil; xchg dword [rdi+0x20], eax; ret
// Called from base Initialize 0x255130 with sil=0 and rdi=*0x2ea4718.
// Returns the previous dword at +0x20 (xchg result in eax). Seq_cst via lock-free
// exchange on an atomic view; bytes outside +0x20..+0x23 untouched.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::size_t kXchgStore357f0Offset = 0x20;

// Models 0x357f0. `obj` must address at least +0x24 writable bytes.
[[nodiscard]] std::uint32_t xchg_store_u32_357f0(void* obj, std::uint8_t value) noexcept;

}  // namespace sotc
