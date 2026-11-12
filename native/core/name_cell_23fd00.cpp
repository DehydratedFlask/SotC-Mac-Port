#include "name_cell_23fd00.hpp"

#include "fnv_path_cstr_162020.hpp"

namespace sotc {

namespace {

// strncpy(cell.text, source, 15): copy up to 15 bytes, stop at the terminator and zero the rest of the 15.
void copy_padded(char* destination, const char* source) noexcept {
    std::size_t n = 0;
    for (; n < kNameCell23fd00CopyLength && source[n] != '\0'; ++n) destination[n] = source[n];
    for (; n < kNameCell23fd00CopyLength; ++n) destination[n] = '\0';
}

}  // namespace

void init_name_cell_23fd00(HashedName20& cell, const char* source) noexcept {
    if (source != nullptr) {
        cell.hash = fnv_path_cstr_162020(source, kFnvOffsetBasisA2ddc0);
        copy_padded(cell.text, source);
    } else {
        cell.hash = 0;
        copy_padded(cell.text, kNameCell23fd00NullText);
    }
    cell.text[15] = '\0';   // movb $0, 0x13(%r14)
}

}  // namespace sotc
