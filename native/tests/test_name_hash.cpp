#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "core/name_hash.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

// All vectors are synthetic (computed with an independent Python reference of the decoded
// algorithm); none are original game strings.
int main() {
    using sotc::name_hash;
    using sotc::name_hash_inline;

    CHECK(name_hash(nullptr) == 0u);
    CHECK(name_hash("") == 5381u);
    CHECK(name_hash_inline("") == 5381u);
    CHECK(name_hash("a") == 0x0002b5c4u);
    CHECK(name_hash("abc") == 0x0b873285u);
    CHECK(name_hash("The quick brown fox") == 0x1556a66eu);
    CHECK(name_hash_inline("The quick brown fox") == 0x1556a66eu);

    // XOR (not add) of the final byte: differing low bits move the hash in a non-additive way.
    CHECK(name_hash("tex_0") == 0x0ae9a623u);
    CHECK(name_hash("tex_1") == 0x0ae9a622u);
    CHECK(name_hash("tex_2") == 0x0ae9a621u);
    CHECK(name_hash("tex_3") == 0x0ae9a620u);

    // Raw hash 0 is remapped to 1 only by the dedicated routine.
    CHECK(name_hash("zeawiolf") == 1u);
    CHECK(name_hash_inline("zeawiolf") == 0u);

    // Bytes >= 0x80: dedicated routine sign-extends, inline form zero-extends.
    CHECK(name_hash("\x80") == 0xfffd4a25u);
    CHECK(name_hash_inline("\x80") == 0x0002b525u);
    CHECK(name_hash("\xff\x7f") == 0xffa695e5u);
    CHECK(name_hash_inline("\xff\x7f") == 0x005960e5u);
    CHECK(name_hash("ab\xc3\xa9") == 0x7c6d8e6cu);
    CHECK(name_hash_inline("ab\xc3\xa9") == 0x7c6d766cu);

    // Prefix independence from trailing bytes after the NUL.
    CHECK(name_hash("abc\0def") == name_hash("abc"));
    CHECK(name_hash_inline("abc\0def") == 0x0b873285u);

    std::puts("name_hash: ok");
    return 0;
}
