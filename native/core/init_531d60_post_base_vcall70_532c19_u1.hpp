// Init `0x531d60` registry virtual call +0x70 OBS `0x532c19..0x532c1c` (3 B):
//   ff 50 70   ; call [rax+0x70] with rdi = *(0x2ee2cf8), rsi = &caller->0x3ec0, edx = 0x63736472
//
// After E614 mov rax,[rdi] @ `0x532c16`. The callee body is NOT modelled; the host supplies a
// hook that receives the three argument registers so the call site stays observable.
// NOT modelled: E616 mov rbx,[r14+0x3ec0] @ `0x532c1c`. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kInit531d60PostBaseVcall70_532c19_U1Rva = 0x532c19;
inline constexpr std::size_t kInit531d60PostBaseVcall70_532c19_U1Size = 3;
inline constexpr std::uint64_t kInit531d60PostBaseVcall70_532c19_U1EndRva = 0x532c1c;
inline constexpr std::uint32_t kInit531d60PostBaseVcall70_532c19_U1SlotOff = 0x70;

struct Init531d60PostBaseVcall70_532c19_U1Cells {
    void* self = nullptr;   // rdi
    void* field = nullptr;  // rsi
    std::uint32_t tag = 0;  // edx
};

struct Init531d60PostBaseVcall70_532c19_U1Hooks {
    // Called once with the modelled argument registers; the callee body is out of scope.
    void (*call_vslot70)(void* self, void* field, std::uint32_t tag) = nullptr;
};

struct Init531d60PostBaseVcall70_532c19_U1Result {
    bool ok = false;
    int call_vslot70_532c19_count = 0;
};

[[nodiscard]] Init531d60PostBaseVcall70_532c19_U1Result init_531d60_post_base_vcall70_532c19_u1(
    const Init531d60PostBaseVcall70_532c19_U1Cells& cells = {},
    const Init531d60PostBaseVcall70_532c19_U1Hooks& hooks = {}) noexcept;

}  // namespace sotc
