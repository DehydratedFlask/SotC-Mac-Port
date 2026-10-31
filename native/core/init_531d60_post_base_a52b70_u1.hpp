// Init `0x531d60` merge call-site OBS `0x53290b..0x532910` (5 B):
//   call 0xa52b70   ; first call after continue_b fall-through jmp to 0x532904
//
// Prep `0x532904..0x53290b` (7 B: movzx esi,r14b; mov rdi,r15) NOT modelled.
// NOT modelled: 0xa52b70 body beyond hook. PR #5 cite-only.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseA52b70U1Rva = 0x53290b;
inline constexpr std::size_t kInit531d60PostBaseA52b70U1Size = 5;
inline constexpr std::uint64_t kInit531d60PostBaseA52b70U1EndRva = 0x532910;
inline constexpr std::uint64_t kInit531d60PostBaseA52b70U1TargetRva = 0xa52b70;
inline constexpr std::uint64_t kInit531d60PostBaseA52b70U1PrepRva = 0x532904;
inline constexpr std::size_t kInit531d60PostBaseA52b70U1PrepSize = 7;
inline constexpr std::uint64_t kInit531d60PostBaseA52b70MergeJmpRva = 0x5327b1;

struct Init531d60PostBaseA52b70U1Hooks {
    std::uint8_t incoming_r14b = 0;
    const void* incoming_r15 = nullptr;
    void (*call_a52b70)(void* rdi, std::uint8_t esi) = nullptr;
};

struct Init531d60PostBaseA52b70U1Result {
    bool ok = false;
    int call_a52b70_count = 0;
    std::uint8_t esi_arg = 0;
    void* rdi_arg = nullptr;
};

[[nodiscard]] Init531d60PostBaseA52b70U1Result init_531d60_post_base_a52b70_u1(
    const Init531d60PostBaseA52b70U1Hooks& hooks = {}) noexcept;

}  // namespace sotc
