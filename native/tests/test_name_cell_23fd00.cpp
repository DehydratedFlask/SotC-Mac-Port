// CPU-01-STARTUP-BATCH-10: fixture for the 0x23fd00 name-cell initialiser (hash + 15 byte padded text). NULL source -> hash 0, text "NULL".
#include "l4_test_util.hpp"

#include "core/fnv_path_cstr_162020.hpp"
#include "core/name_cell_23fd00.hpp"

namespace {
using namespace l4;

void section_null() {
    HashedName20 c;
    poison(c);
    init_name_cell_23fd00(c, nullptr);
    CHECK(c.hash == 0);
    CHECK(std::memcmp(c.text, "NULL", 5) == 0);
    for (std::size_t i = 4; i < sizeof c.text; ++i) CHECK(c.text[i] == 0);   // strncpy zero-pads, byte 0x13 is a terminator
    CHECK(kNameCell23fd00CopyLength == 15 && sizeof(HashedName20) == 20);
}

void section_text() {
    HashedName20 c;
    poison(c);
    init_name_cell_23fd00(c, "Abc");
    CHECK(c.hash == fnv_path_cstr_162020("Abc", 0x811c9dc5u) && c.hash == fnv_cstr_a2ddc0("Abc"));
    CHECK(std::memcmp(c.text, "Abc", 4) == 0);
    for (std::size_t i = 3; i < sizeof c.text; ++i) CHECK(c.text[i] == 0);
    // the hash folds case and path separators (0x162020), the text does not
    HashedName20 d;
    init_name_cell_23fd00(d, "ABC");
    CHECK(d.hash == c.hash && std::memcmp(d.text, "ABC", 4) == 0);
    // a source longer than 15 bytes keeps its first 15 bytes and is terminated by the unconditional store at +0x13
    poison(c);
    init_name_cell_23fd00(c, "0123456789abcdefXYZ");
    CHECK(std::memcmp(c.text, "0123456789abcde", 15) == 0 && c.text[15] == 0);
    CHECK(c.hash == fnv_path_cstr_162020("0123456789abcdefXYZ", 0x811c9dc5u));
    // exactly 15 bytes: copied whole, terminator from the store
    poison(c);
    init_name_cell_23fd00(c, "0123456789abcde");
    CHECK(std::memcmp(c.text, "0123456789abcde", 15) == 0 && c.text[15] == 0);
    // empty string: hash = the seed, text all zero (NOT the NULL default)
    poison(c);
    init_name_cell_23fd00(c, "");
    CHECK(c.hash == 0x811c9dc5u);
    for (char ch : c.text) CHECK(ch == 0);
}

}  // namespace

int main() {
    section_null();
    section_text();
    std::puts("OK test_name_cell_23fd00");
    return 0;
}
