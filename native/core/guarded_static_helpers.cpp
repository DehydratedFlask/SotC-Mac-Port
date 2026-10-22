#include "guarded_static_helpers.hpp"

#include <cstring>

namespace sotc {

Pair128 pair_zero_51230() noexcept { return Pair128{0, 0}; }   // xor eax,eax ; xor edx,edx

const Pair128& static_pair_a77960() noexcept {
    static const Pair128 value = pair_zero_51230();   // guard acquire ; call 0x51230 ; 2 stores ; guard release
    return value;
}

std::uint8_t static_flag_268d40() noexcept {
    static const std::uint8_t value = 0xff;           // guard acquire ; mov byte [..],0xff ; guard release
    return value;
}

void init_ctor_78c760(Pair128& cell) noexcept {
    std::memcpy(&cell, &static_pair_a77960(), sizeof(Pair128));   // vmovups xmm0,[rax] ; vmovups [rcx],xmm0
}

void init_ctor_268ce0(std::uint8_t& cell) noexcept { cell = static_flag_268d40(); }   // mov [cell],al

}  // namespace sotc
