// 0xa2ddc0 / 0x162020: path-normalizing FNV-1a (ASCII toupper + '\\'->'/', null->0).
// Agrees with save_chunk ascii_upper_and_slash on non-null ASCII; ELF tie in check_f162020.py.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "core/fnv_cstr_161fb0.hpp"
#include "core/fnv_path_cstr_162020.hpp"
#include "core/save_chunk_codec.hpp"

#define CHECK(c)                                                                         \
    do {                                                                                 \
        if (!(c)) {                                                                      \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);              \
            std::exit(1);                                                                \
        }                                                                                \
    } while (0)

namespace {
std::uint32_t oracle(const std::string& s, std::uint32_t seed) {
    std::uint32_t h = seed;
    for (unsigned char c : s) {
        unsigned char folded = (c == 0xffu) ? 0xffu : c;
        if (folded != 0xffu && folded >= 'a' && folded <= 'z') {
            folded = static_cast<unsigned char>(folded - 'a' + 'A');
        }
        if (folded == '\\') folded = '/';
        h = (h ^ folded) * 0x01000193u;
    }
    return h;
}
}  // namespace

int main() {
    using namespace sotc;
    CHECK(kFnvOffsetBasisA2ddc0 == 0x811c9dc5u);
    CHECK(kFnvPrime162020 == 0x01000193u);

    CHECK(fnv_cstr_a2ddc0(nullptr) == 0);
    CHECK(fnv_path_cstr_162020(nullptr, 0x12345678u) == 0);
    CHECK(fnv_cstr_a2ddc0("") == 0x811c9dc5u);
    CHECK(fnv_path_cstr_162020("", 0xdeadbeefu) == 0xdeadbeefu);

    // ASCII upper fold: hashes the uppercased form (FNV-1a of "FOOBAR" == 0x9de507a8).
    CHECK(fnv_cstr_a2ddc0("foobar") == fnv_cstr_a2ddc0("FOOBAR"));
    CHECK(fnv_cstr_a2ddc0("foobar") == 0x9de507a8u);
    CHECK(fnv_cstr_a2ddc0("foobar") != 0xbf9cf968u);  // that digest is unsigned FNV of lowercase
    CHECK(fnv_cstr_a2ddc0("a\\b") == fnv_cstr_a2ddc0("A/B"));
    CHECK(fnv_cstr_a2ddc0("a\\b") == fnv_cstr_a2ddc0("a/b"));

    // Agrees with the save-chunk ascii_upper_and_slash policy on ordinary names.
    CHECK(fnv_cstr_a2ddc0("abc") == chunk_name_hash("abc", NameHashPolicy::ascii_upper_and_slash));
    CHECK(fnv_cstr_a2ddc0("a\\b") == chunk_name_hash("a\\b", NameHashPolicy::ascii_upper_and_slash));

    // Distinct from the sign-extending NUL FNV (0x161fb0) on high bytes and on ASCII case.
    CHECK(fnv_cstr_a2ddc0("foobar") != fnv_cstr_52a990("foobar"));
    CHECK(fnv_cstr_a2ddc0("\x80") != fnv_cstr_52a990("\x80"));
    CHECK(fnv_cstr_a2ddc0("\x80") == (0x811c9dc5u ^ 0x80u) * 0x01000193u);

    // Byte 0xff bypasses the table (original cmpq $0xff) and is hashed as 0xff.
    CHECK(fnv_cstr_a2ddc0("\xff") == (0x811c9dc5u ^ 0xffu) * 0x01000193u);

    const std::vector<std::string> samples = {
        "x", "TimeSinceLastProfileSave", "a\\b/c", std::string("\x80"), std::string("\xff"),
        std::string("Ab\xe9z"), std::string(200, 'm'),
    };
    for (const auto& s : samples) {
        for (std::uint32_t seed : {0u, 1u, 0x811c9dc5u, 0xffffffffu}) {
            CHECK(fnv_path_cstr_162020(s.c_str(), seed) == oracle(s, seed));
        }
        CHECK(fnv_cstr_a2ddc0(s.c_str()) == oracle(s, 0x811c9dc5u));
    }

    std::puts("fnv_path_cstr_162020: all checks passed");
    return 0;
}
