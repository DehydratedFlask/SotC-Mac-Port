// Native recovery of original helpers RVA 0xb30cf0 and 0xb30d00 (ELF sha256 d0723afb...5861d3), task CPU-01.
// Two call-free leaves built on `lock xadd dword [rdi], eax`; both return the NEW value of the cell:
//   0xb30cf0 (12 bytes): mov eax,-1; lock xadd [rdi],eax; dec eax; ret      -> atomic decrement-and-fetch
//   0xb30d00 (10 bytes): mov eax,esi; lock xadd [rdi],eax; lea eax,[rax+rsi]; ret -> atomic add-and-fetch
// (sibling 0xb30ce0, `mov eax,1; ...; inc eax; ret`, is the increment-and-fetch form, not lifted here).
// rdi is the address of a 4-aligned int32 cell (offset 0 of the pointed-to object); the old value is
// computed by hardware in eax and the helper adds the delta back, wrapping modulo 2^32. Callers' use of
// the cells is not analysed, so the names state the operation only.
#pragma once
#include <atomic>
#include <cstdint>

namespace sotc {

// 0xb30cf0: returns (old - 1), the value now stored; seq_cst (locked RMW).
std::int32_t atomic_dec_fetch_b30cf0(std::atomic<std::int32_t>& cell) noexcept;

// 0xb30d00: returns (old + delta) mod 2^32, the value now stored; seq_cst (locked RMW).
std::int32_t atomic_add_fetch_b30d00(std::atomic<std::int32_t>& cell, std::int32_t delta) noexcept;

}  // namespace sotc
