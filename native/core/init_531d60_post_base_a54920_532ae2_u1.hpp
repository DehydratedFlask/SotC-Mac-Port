// Init `0x531d60` continue E third call-site OBS `0x532ae2..0x532ae7` (5 B):
//   call 0xa54920   ; save-data ctor 3 ("save_%s_mem_used")
//
// After E537 `mov_rsp4b8_rcx_532ada_u1` end @ `0x532ae2`. Block-3 descriptor prep
// (vmovq through mov [rsp+0x4B8],rcx) NOT modelled as pinned units in `continue_e`;
// this u1 runs the ctor call only. NOT modelled: 0xa54920 body; post-call vcall
// @ `0x532af2`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseA54920_532ae2_U1Rva = 0x532ae2;
inline constexpr std::size_t kInit531d60PostBaseA54920_532ae2_U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseA54920_532ae2_U1EndRva = 0x532ae7;
inline constexpr std::uint64_t kInit531d60PostBaseA54920_532ae2_U1TargetRva = 0xa54920;
inline constexpr std::uint64_t kInit531d60PostBaseA54920_532ae2_U1Name3Rva = 0x2efda50;

struct Init531d60PostBaseA54920_532ae2_U1Hooks {
    void* parent = nullptr;
    void* descriptor = nullptr;
    void (*call_a54920)(void* parent, void* name_struct, void* descriptor) = nullptr;
};

struct Init531d60PostBaseA54920_532ae2_U1Result {
    bool ok = false;
    int call_a54920_count = 0;
    bool a54920_hook_supplied = false;
};

[[nodiscard]] Init531d60PostBaseA54920_532ae2_U1Result init_531d60_post_base_a54920_532ae2_u1(
    const Init531d60PostBaseA54920_532ae2_U1Hooks& hooks = {}) noexcept;

}  // namespace sotc
