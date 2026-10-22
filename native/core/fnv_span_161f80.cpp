#include "fnv_span_161f80.hpp"

namespace sotc {

std::uint32_t fnv_span_161f80(const ByteSpan& span, std::uint32_t seed) noexcept {
    if (span.size <= 0) return seed;   // `test rax,rax ; jle` is a signed compare; seed returned unchanged
    std::uint32_t h = seed;
    const std::uint8_t* p = span.data;
    for (std::int64_t i = 0; i != span.size; ++i) {
        // movzx edx, byte [rcx] ; xor edx, esi ; imul esi, edx, 0x1000193
        h = (static_cast<std::uint32_t>(p[i]) ^ h) * kFnvPrime161f80;
    }
    return h;
}

}  // namespace sotc
