#include "helper_1f220_ring_header.hpp"

namespace sotc {

void ring_header_init_1f220(void* object) noexcept {
    auto* base = static_cast<std::uint8_t*>(object);
    std::uint8_t* const begin = base + kRingHeader1f220StorageOffset;   // 48 8d 47 28: lea rax, [rdi + 0x28]
    std::uint8_t* const end = base + kRingHeader1f220EndOffset;         // 48 8d 8f 28 26 00 00: lea rcx, [rdi + 0x2628]
    auto store = [base](std::size_t offset, std::uint8_t* value) { *reinterpret_cast<std::uint8_t**>(base + offset) = value; };
    store(0x20, begin);
    store(0x08, begin);
    store(0x00, begin);
    store(0x10, end);
}

}  // namespace sotc
