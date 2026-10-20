#include "arena_page_debit.hpp"

namespace sotc {

void arena_debit_pages(void* arena, std::uint64_t bytes) noexcept {
    if (!arena) return;
    const auto pages = static_cast<std::uint32_t>(bytes >> 16);  // OBS shr rax, 0x10
    arena_page_count(arena) -= pages;  // OBS sub dword [r15+0x70], eax
}

}  // namespace sotc
