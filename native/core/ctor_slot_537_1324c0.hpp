// CPU-01-AOT-CFG-SCALE-01-BATCH23 (slot 537): native model of startup constructor slot 537
// (RVA 0x1324c0, 795 B; FDE [0x1324c0, 0x1327db)). Original ELF sha256 d0723afb...5861d3; the
// constructor-table word at 0x2dd70e0 - 8 * 537 = 0x2dd6018 holds 0x1324c0.
// Every fact below was read end to end from the pinned bytes; the byte oracle is
// tools/lift/check_ctor_slot_537.py. NOTHING here executes, loads or calls an original RVA:
// the destructor RVA and the __dso_handle are IDENTITY TAGS, recorded in the atexit log and never run.
//
// ================================ observed shape ================================
// Frame: push rbp; mov rsp,rbp; ... pop rbp; ret. No local frame and no conditional branch: one
// straight-line run of 40 conversions.
//
// Preamble:
//   lea  rsi, 0x2ee12e0 ; movq $0, (rsi)      ; an 8-byte word, zeroed
//   lea  rdi, 0x131f00  ; lea rdx, 0x1604218  ; __cxa_atexit(dtor, object, __dso_handle)
//   call 0xf8f390
//
// Then 40 groups `call 0x157e90` / `movw %ax, 0x2ee12f0 + 2 * i`. The scheduler interleaves them:
// group i's store is issued after group i+1's `vmovss`, so the stored value is still the RESULT OF
// CALL i, and the store order is exactly i = 0..39 (verified by decoding all 40 stores). Each
// `vmovss` is rip-relative into .rodata; 11 of the 40 inputs are `vxorps %xmm0,%xmm0,%xmm0`, i.e. a
// +0.0f bit pattern. The last store is at 0x2ee133e, so the table is 80 B at 0x2ee12f0.
//
// ============================== the converter, 0x157e90 ==============================
// 203 B, no call and no stack use: input is the low 32 bits of xmm0 (a binary32 bit pattern),
// output is %ax (a binary16 bit pattern). Read instruction by instruction:
//
//   r9d = (bits >> 23) & 0xff        ; bextr 0x817, i.e. the exponent field e
//   esi = bits & 0x7fffff            ; mantissa
//   eax = (bits >> 16) & 0x8000      ; sign, already placed at bit 15
//   ecx = e - 0x70                   ; the half exponent, biased by the difference of the biases
//   if (int32) ecx <= 0     -> subnormal / zero path  (0x157eda)
//   if ecx == 0x8f          -> e == 0xff, inf / NaN path (0x157ec0)
//   otherwise               -> normal path (0x157f12)
//
// Normal path. `r9d = e - 0x6f; d = mant + bit13 + 0xfff` where bit13 is bit 13 of the whole word,
// i.e. the LSB of the half's mantissa field. Adding 0xfff plus that LSB is round-half-to-EVEN. If bit 23 of `d` sets, the
// mantissa carried into the exponent: r9d stays e-0x6f (one higher) and the mantissa bits are
// DROPPED (the `cmove`/`or` pair emits 0 instead of the shifted value, so the carry is not
// double-counted). Otherwise r9d becomes e-0x70 and `d >> 13` is the 10-bit mantissa. If r9d >= 0x1f
// the result saturates to 0x7c00 (infinity). The body therefore rounds, it does NOT truncate.
//
// Subnormal path (`e <= 0x70`). `e < 0x66` (i.e. ecx < -0xa) returns the sign alone: a signed zero,
// a flush-to-zero floor. For 0x66 <= e <= 0x70 the implicit bit is restored (`esi |= 0x800000`) and
// `(m + (1 << (0x7d - e)) - 1 + ((m >> (0x7e - e)) & 1)) >> (0x7e - e)` reproduces round-half-to-even
// at the subnormal spacing, again dropping the carry (the `sar` moves it into bit 10 and the value is
// ORed as-is). Every reachable shift here is 14..24, so the x86 `cl & 31` masking never engages.
//
// inf / NaN path (`e == 0xff`). Mantissa 0 returns sign | 0x7c00. Otherwise the payload is
// `sign | 0x7c00 | (mant >> 13) | (1 if (mant >> 13) == 0 else 0)`: a NaN whose payload is the top
// ten mantissa bits, with bit 0 forced for the smallest payloads so the result is never plain inf.
//
// Consequence worth pinning: this is round-half-to-even, NOT truncation, and NOT a call to any host
// float16 routine. It saturates at 0x7c00 instead of raising, and it defines its own NaN payload
// formula. The fixture asserts each boundary the instructions encode (the 0x3f801000 tie, the
// 0x33800000 subnormal floor, 0x477fe000 as the largest finite half, 0x477ff000 saturating because
// its bit 13 sets the carry, and the 0x7f800001 / 0x7fc00001 payloads).
//
// Interpretations: a table of 40 tuning constants converted once and cached as halves. What reads
// 0x2ee12f0 at run time is NOT established; that reading is INFERENCE only.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot537Rva = 0x1324c0;
inline constexpr std::size_t kSlot537Size = 795;
inline constexpr std::uint64_t kSlot537TableWord = 0x2dd6018;   // 0x2dd70e0 - 8 * 537

inline constexpr std::uint64_t kSlot537Object = 0x2ee12e0;      // the zeroed 8-byte word
inline constexpr std::uint64_t kSlot537Table = 0x2ee12f0;       // the 40 halves
inline constexpr std::size_t kSlot537Count = 40;
inline constexpr std::size_t kSlot537Stride = 2;
inline constexpr std::uint64_t kSlot537LastHalf = 0x2ee133e;    // Table + 2 * 39

inline constexpr std::uint64_t kSlot537Dtor = 0x131f00;         // registered destructor, identity only
inline constexpr std::uint64_t kSlot537DsoHandle = 0x1604218;
inline constexpr std::uint64_t kSlot537AtexitRva = 0xf8f390;

inline constexpr std::uint64_t kSlot537ConverterRva = 0x157e90;
inline constexpr std::size_t kSlot537ConverterSize = 203;

// Input provenance, in call order: 0 marks a `vxorps` input (bit pattern +0.0f), otherwise the RVA
// of the float word the body loads. 29 loads + 11 zeros = 40. Both arrays are pinned independently
// against the ELF by the checker.
inline constexpr std::uint64_t kSlot537InputRvas[kSlot537Count] = {
    0xfa7718, 0xfa771c, 0xfa7720, 0xfa7724, 0xfa7728, 0xfa772c, 0xfa7730, 0xfa7734,
    0xfa7738, 0xfa773c, 0xfa7740, 0xfa7744, 0xfa7748, 0x000000, 0x000000, 0x000000,
    0xfa774c, 0xfa7750, 0xfa7754, 0xfa7758, 0xfa775c, 0xfa7760, 0xfa7764, 0xfa7768,
    0xfa776c, 0xfa7770, 0xfa7774, 0xfa7778, 0x000000, 0x000000, 0x000000, 0x000000,
    0xfa777c, 0xfa7780, 0xfa7784, 0x000000, 0xfa7788, 0x000000, 0x000000, 0x000000,
};
inline constexpr std::uint32_t kSlot537InputBits[kSlot537Count] = {
    0x3fe7dfe3u, 0xbd083ba3u, 0xbd1800a8u, 0xbd2a64c3u, 0x3fde5fd9u, 0xbe5dbf48u, 0xbd1c0443u, 0xbcede54bu,
    0x3fb24010u, 0xbf0d4025u, 0xbcd70a3du, 0xbc309e99u, 0x4062dfceu, 0x00000000u, 0x00000000u, 0x00000000u,
    0xbd90e022u, 0xbd608073u, 0xbd39ffd6u, 0x3d24bdbau, 0xbd8aa10eu, 0xbd5f5ad9u, 0xbd04ff43u, 0x3d2a3ad2u,
    0xbd495bffu, 0xbd2a1b5cu, 0xbc9b3d08u, 0x3cd805e6u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
    0xbd21dfb9u, 0xbcb3fa6eu, 0xbc79096cu, 0x00000000u, 0x3f800000u, 0x00000000u, 0x00000000u, 0x00000000u,
};
inline constexpr std::size_t kSlot537ZeroInputs = 11;

// The port of 0x157e90. `bits` is a binary32 bit pattern; the result is a binary16 bit pattern, the
// same value the helper returns in %ax. Every branch, mask, bias and shift below mirrors one
// instruction; see the derivation above.
[[nodiscard]] std::uint16_t slot537_f32_to_f16(std::uint32_t bits) noexcept;

struct Slot537Objects {
    std::uint64_t header = 0;                        // 0x2ee12e0, the word the body zeroes
    std::uint16_t halves[kSlot537Count]{};           // 0x2ee12f0 + 2 * i
};

struct Slot537Result {
    bool ok = false;
    std::size_t halves_written = 0;
    std::size_t atexit_records = 0;
};

// Runs the 40 conversions and the single __cxa_atexit registration. `bits` must expose
// kSlot537Count binary32 bit patterns in body order; the model never stores the input table.
[[nodiscard]] Slot537Result init_slot537_1324c0(Slot537Objects& objects, StartupAtexitLog& log,
                                                const std::uint32_t* bits) noexcept;

}  // namespace sotc
