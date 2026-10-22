#include "xchg_store_u32_357f0.hpp"

namespace sotc {

std::uint32_t xchg_store_u32_357f0(void* obj, std::uint8_t value) noexcept {
    auto* cell = reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(obj) +
                                                  kXchgStore357f0Offset);
    // OBS: movzx eax,sil ; xchg dword [rdi+0x20], eax — xchg-with-mem is implicitly locked.
    return __atomic_exchange_n(cell, static_cast<std::uint32_t>(value), __ATOMIC_SEQ_CST);
}

}  // namespace sotc
