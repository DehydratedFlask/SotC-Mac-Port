#include "helper_f8f630_strncpy.hpp"

namespace sotc {

char* strncpy_f8f630(char* dst, const char* src, std::size_t n) noexcept {
    std::size_t i = 0;
    for (; i < n && src[i] != '\0'; ++i) dst[i] = src[i];   // copy until NUL or n bytes
    for (; i < n; ++i) dst[i] = '\0';                       // pad to exactly n bytes
    return dst;
}

void store_null_name_cell_247f47(NullNameCell247f47& cell) noexcept {
    cell.id = 0;                                                                   // movl $0, (%rbx)
    (void)strncpy_f8f630(cell.name, "NULL", kNullNameCellCopyLength);              // 0x247f47: the literal at 0xfc15f3
    cell.name[kNullNameCellTerminatorOffset - offsetof(NullNameCell247f47, name)] = '\0';   // movb $0, 0x13(%rbx)
}

}  // namespace sotc
