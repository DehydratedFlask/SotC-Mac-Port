// Native recovery of original ctor RVA 0xee1130 (ctor index 21; ELF sha256 d0723afb...5861d3).
//
// Body: 182 bytes, 26 instructions, one return, no calls, no indirect transfers, no stack frame.
// It performs 15 straight-line constant stores into one 128-byte .bss block at 0x33540f0,
// taking the values from eight rodata cells at 0x1323b50..0x1323be0. The observed values are
// float/mask constants (0, ±1.0f, ±FLT_MAX, 4x0x34000000, all-ones masks); their semantic role is
// NOT recovered, so this is a byte-exact data lift, not a typed model of the block.
//
// Store map (offsets are relative to the native table; original address = 0x33540f0 + offset):
//   +0x00 8B  0x0000000000000000        vmovlps xmm0 (=0)
//   +0x08 8B  0xffffffff00000000        rax = [0x1323b50]
//   +0x10 8B  0xffffffffffffffff        vmovq xmm1 (all ones)
//   +0x18 8B  0x0000000000000000        vmovlps xmm0
//   +0x20 8B  0xffffffffffffffff        vmovq xmm1
//   +0x28 8B  0x00000000ffffffff        vmovlps xmm2 (vmovss [0x1323bdc] = 0xffffffff)
//   +0x30 8B  0xffffffff00000000        rax
//   +0x38 8B  0xffffffffffffffff        vmovq xmm1
//   +0x40 16B 0x34000000 x4             vmovups xmm1 from [0x1323b60]
//   +0x50 8B  0xbf800000bf800000        rdx = [0x1323b70] (two -1.0f)
//   +0x58 8B  0x3f800000bf800000        rcx = [0x1323b80] (1.0f, -1.0f)
//   +0x60 8B  0x0000000000000000        vmovlps xmm0
//   +0x68 8B  0x7f7fffff00000000        rdx = [0x1323b90] (FLT_MAX, 0)
//   +0x70 8B  0xff7fffffff7fffff        rcx = [0x1323ba0] (two -FLT_MAX)
//   +0x78 8B  0x00000000ff7fffff        vmovlps xmm0 (vmovss [0x1323be0] = 0xff7fffff)
// Register reloads at 0xee117d (xmm0), 0xee119b (xmm1), 0xee11aa (rdx) and 0xee11b1 (rcx) are
// modelled in that order; using the initial register values would produce wrong bytes.
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::size_t kConstTableEe1130Bytes = 0x80;

using ConstTableEe1130 = std::array<std::uint8_t, kConstTableEe1130Bytes>;

// Byte-exact form of the 15 stores. The meaning of the cells is unknown; do not attach semantics.
void init_const_table_ee1130(ConstTableEe1130& table) noexcept;

}  // namespace sotc
