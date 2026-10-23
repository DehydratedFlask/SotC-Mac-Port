// Init `0x531d60` remainder — trivial callees only (appmain triage next after registry fragment).
//
// ELF sha256 d0723afb…861d3. OBS sites in Init mid-path (before registry @ 0x532b0c):
//   one-byte `ret` stubs: 0x56dca0, 0x56db70, 0x5b3d00, 0x5b3cf0, 0x56fad0
//   flag pair 0x4c4950 → cells 0x2efa890 (dil) + 0x2efa891 (=1)
//   byte setters: 0xa52b70 [rdi+0x01], 0xa599e0 [rdi+0x29], 0xa431e0 [rdi+0x51]
//   splash hide PLT 0xf90310 (sceSystemServiceHideSplashScreen) → native no-op contract
//
// NOT modelled: 0x1fc80/0x1e4c0 pad path (0xf08 singleton + base Initialize 0x255130 are earlier slices),
// pool/listener half of 0x52a220, remainder of Init. No leaf-vec; not idx 95; not in
// recovered_constructors(). PR #5 heap untouched (cite-only).
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

// Process-wide cells modelling original BSS pair at 0x2efa890 / 0x2efa891.
[[nodiscard]] std::uint8_t& init_flag_cell_2efa890() noexcept;
[[nodiscard]] std::uint8_t& init_flag_cell_2efa891() noexcept;

// 0x4c4950 — 21 B: store dil → 0x2efa890; mov byte [0x2efa891], 1.
void set_init_flag_pair_4c4950(std::uint8_t flag) noexcept;

// 0xa52b70 / 0xa599e0 / 0xa431e0 — 5 B each: mov [rdi+off], sil; ret.
void set_byte_a52b70(void* obj, std::uint8_t v) noexcept;  // +0x01
void set_byte_a599e0(void* obj, std::uint8_t v) noexcept;  // +0x29
void set_byte_a431e0(void* obj, std::uint8_t v) noexcept;  // +0x51

// One-byte `ret` stubs called from Init early/mid path (OBS sizes = 1).
void ret_nop_56dca0() noexcept;
void ret_nop_56db70() noexcept;
void ret_nop_5b3d00() noexcept;
void ret_nop_5b3cf0() noexcept;
void ret_nop_56fad0() noexcept;

// PLT 0xf90310 → Sony splash hide. Native contract: no-op success (no Sony module).
void hide_splash_screen_f90310() noexcept;

// Offsets for setter ELF-tie / fixture poison checks.
inline constexpr std::size_t kSetterOffA52b70 = 0x01;
inline constexpr std::size_t kSetterOffA599e0 = 0x29;
inline constexpr std::size_t kSetterOffA431e0 = 0x51;

}  // namespace sotc
