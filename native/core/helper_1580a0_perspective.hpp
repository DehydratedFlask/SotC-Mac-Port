// CPU-01-HELPER-CALLEE-MODELS-04: native model of the perspective-projection builder 0x1580a0 (0xc0 B, FDE 192, [0x1580a0, 0x158160)). Original
// ELF sha256 d0723afb...5861d3; every fact below is byte-pinned by tools/lift/check_helper_1580a0.py. NOTHING here executes, loads or calls an
// original RVA: the only external effect is libc `tanf`, which the original reaches through the PLT thunk 0xf8f410 (GOT 0x15ffff8, NID-proven
// by the checker) and which this model takes as an explicit host hook (`TanfFn`).
//
// Signature recovered from the prologue (SysV; rdi, rsi, edx, xmm0..xmm2):
//   rdi  = out, a 16-float row-major matrix, every element written (returned in rax)
//   rsi  = fov*, a pointer to one float (the full field-of-view angle in radians; the callee halves it)
//   xmm0 = aspect          xmm1 = near plane   xmm2 = far plane      edx (low byte only, `test r14b, r14b`) = reversed-depth flag
// Semantics, evaluated in single precision in exactly the instruction order (no fused multiply-add exists in the body):
//   t = tanf(*fov * 0.5f)                      (0.5f is the dword at 0xfaad0c)
//   c = 1.0f / t                               (1.0f is the dword at 0xfaad10)
//   m[0] = c / aspect, m[5] = c, m[11] = 1.0f, m[1..4] = m[6..9] = m[12] = m[13] = m[15] = 0.0f
//   flag != 0 : d = near - far, m[10] = near / d,  m[14] = -(near * far) / d       (-0.0 mask xor at 0xfaad30)
//   flag == 0 : d = far - near, m[10] = far / d,   m[14] = -(near * far) / d
// i.e. a left-handed projection whose m[11] == 1 takes the view depth; flag == 0 is the conventional depth range [0,1] near -> far and flag != 0 swaps
// the two planes (reversed depth). INFERENCE: the names near / far / reversed come from the observed values (0.5 and 4200.0 at the slot 408 site)
// and the camera fields the other callers read; the arithmetic itself is OBSERVED.
//
// Direct callers (all `call rel32`, no tail jmp and no relocation addend names 0x1580a0): 0xa7038, 0xa8978, 0xb1110, 0x1198c7, 0x119f79, 0x20e79a
// and 0x5c8082 (startup slot 408, 0x5c7f50). Only the last is modelled as a site; the rest are identity-only.
//
// Fail-visible: a missing `tanf` hook, a null out or a null fov pointer is refused with `ok == false` and nothing written (the original has no such
// guard; it would fault, so the refusal is the faithful "not reachable" rendering). Division by zero follows IEEE-754 single precision as on the
// original SSE unit (no trap).
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kPerspective1580a0Rva = 0x1580a0;
inline constexpr std::size_t kPerspective1580a0Size = 0xc0;
inline constexpr std::uint64_t kPerspectiveTanfPltRva = 0xf8f410;      // PLT thunk of libc tanf (NID ZE6RNL+eLbk)
inline constexpr std::uint64_t kPerspectiveTanfGotRva = 0x15ffff8;
inline constexpr std::uint32_t kPerspectiveHalfBits = 0x3f000000;      // 0.5f at 0xfaad0c
inline constexpr std::uint32_t kPerspectiveOneBits = 0x3f800000;       // 1.0f at 0xfaad10 (also the m[11] immediate)
inline constexpr std::uint32_t kPerspectiveSignBits = 0x80000000;      // sign mask at 0xfaad30
inline constexpr std::size_t kPerspectiveMatrixFloats = 16;

// Slot 408 (0x5c7f50) call site 0x5c8082: out = 0x2fda41c, fov cell = [rsp+0x60] = 0x3f490fdb (pi/4), aspect = [0x126633c], near = [0x1266340],
// far = [0x1266344], edx = 1.
inline constexpr std::uint64_t kSlot408PerspectiveCallRva = 0x5c8082;
inline constexpr std::uint64_t kSlot408PerspectiveOutRva = 0x2fda41c;
inline constexpr std::uint32_t kSlot408FovBits = 0x3f490fdb;
inline constexpr std::uint32_t kSlot408AspectBits = 0x3fe38e39;        // 16/9
inline constexpr std::uint32_t kSlot408NearBits = 0x3f000000;          // 0.5
inline constexpr std::uint32_t kSlot408FarBits = 0x45834000;           // 4200.0
inline constexpr std::uint32_t kSlot408Flag = 1;

using TanfFn = float (*)(float);

enum class PerspectiveRefusal : std::uint8_t { none, no_tanf_hook, null_out, null_fov };

struct PerspectiveResult {
    bool ok = false;
    PerspectiveRefusal refusal = PerspectiveRefusal::none;
    float tan_argument = 0.0f;   // fov * 0.5f, what the original hands to tanf
    float tan_result = 0.0f;
};

// 0x1580a0. Writes all 16 elements of `out` on success; nothing on refusal. `flag` is the full edx register, only its low byte matters.
[[nodiscard]] PerspectiveResult helper_1580a0_perspective(float* out, const float* fov, float aspect, float near_plane, float far_plane,
                                                          std::uint32_t flag, TanfFn tanf_hook) noexcept;

// The slot 408 call: the constants above through 0x1580a0 into `out` (the original writes the process-wide cell 0x2fda41c; the host owns the storage).
[[nodiscard]] PerspectiveResult slot408_perspective_site(float* out, TanfFn tanf_hook) noexcept;

}  // namespace sotc
