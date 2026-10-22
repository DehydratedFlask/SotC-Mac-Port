// 0x52a990 / 0x161fb0: seeded FNV-1a over a NUL-terminated string, bytes sign-extended, null -> 0.
// Independent oracle: a separately written 64-bit-arithmetic FNV with the sign extension spelled out,
// published FNV-1a vectors for ASCII, the differences from the zero-extended save checksum for high
// bytes, null/empty edge cases and an unmodified-input check. --dump <arg>... (arg = "n" for a null
// pointer or "s:<hex bytes>", "s:" is the empty string) feeds tools/lift/check_f52a990.py, which
// ties these outputs to the pinned machine code.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <span>
#include <string>
#include <vector>

#include "core/fnv_cstr_161fb0.hpp"
#include "core/save_checksum.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {
std::uint32_t oracle(const std::string& s, std::uint32_t seed) {
    std::uint64_t h = seed;
    for (unsigned char b : s) {
        const std::uint64_t ext = b >= 0x80 ? (0xffffffffu - 0xffu + b) : b;   // sign extension to 32 bits
        h = ((h ^ ext) * 0x01000193ull) & 0xffffffffull;
    }
    return static_cast<std::uint32_t>(h);
}
}  // namespace

int main(int argc, char** argv) {
    using namespace sotc;
    CHECK(kFnvOffsetBasis52a990 == 0x811c9dc5u);

    // Published FNV-1a 32-bit vectors (ASCII, so sign/zero extension agree).
    CHECK(fnv_cstr_52a990("") == 0x811c9dc5u);
    CHECK(fnv_cstr_52a990("a") == 0xe40c292cu);
    CHECK(fnv_cstr_52a990("foobar") == 0xbf9cf968u);

    // Null pointer -> 0 regardless of the seed; empty string -> the seed.
    CHECK(fnv_cstr_52a990(nullptr) == 0);
    CHECK(fnv_cstr_161fb0(nullptr, 0x12345678u) == 0);
    CHECK(fnv_cstr_161fb0("", 0x12345678u) == 0x12345678u);
    CHECK(fnv_cstr_161fb0("", 0) == 0);

    // Oracle comparison over ASCII and high-byte strings, several seeds.
    const std::vector<std::string> samples = {
        "x", "InSaveVolume", "Motion *getAnim(AnimObjDef *)", std::string("\x80"), std::string("\xff"),
        std::string("ab\xe3\x81\x82z"), std::string("\x7f\x80\x81"), std::string(300, 'q'),
    };
    for (const auto& s : samples)
        for (std::uint32_t seed : {0u, 1u, 0x811c9dc5u, 0xffffffffu, 0xdeadbeefu}) {
            CHECK(fnv_cstr_161fb0(s.c_str(), seed) == oracle(s, seed));
        }
    for (const auto& s : samples) CHECK(fnv_cstr_52a990(s.c_str()) == oracle(s, 0x811c9dc5u));

    // Sign extension is observable: a single 0x80 byte xors as 0xffffff80, unlike zero extension.
    CHECK(fnv_cstr_52a990("\x80") == (0x811c9dc5u ^ 0xffffff80u) * 0x01000193u);
    CHECK(fnv_cstr_52a990("\x80") != (0x811c9dc5u ^ 0x80u) * 0x01000193u);
    {
        const std::byte raw[1] = {std::byte{0x80}};
        CHECK(fnv1a32(std::span<const std::byte>(raw, 1), 0x811c9dc5u) != fnv_cstr_52a990("\x80"));
        const std::byte ascii[3] = {std::byte{'a'}, std::byte{'b'}, std::byte{'c'}};
        CHECK(fnv1a32(std::span<const std::byte>(ascii, 3), 0x811c9dc5u) == fnv_cstr_52a990("abc"));
    }

    // Reads only up to the first NUL; the bytes after it are ignored.
    const char with_tail[] = {'a', 'b', '\0', 'z', 'z', '\0'};
    CHECK(fnv_cstr_52a990(with_tail) == fnv_cstr_52a990("ab"));

    if (argc > 1 && std::strcmp(argv[1], "--dump") == 0) {
        std::printf("[");
        for (int i = 2; i < argc; ++i) {
            const std::string a = argv[i];
            std::string s;
            const char* p = nullptr;
            if (a != "n") {
                CHECK(a.rfind("s:", 0) == 0 && a.size() % 2 == 0);
                for (std::size_t k = 2; k < a.size(); k += 2) s.push_back(static_cast<char>(std::stoi(a.substr(k, 2), nullptr, 16)));
                p = s.c_str();
            }
            std::printf("%s{\"h52a990\":%u,\"h161fb0_seed12345678\":%u}", i > 2 ? "," : "", fnv_cstr_52a990(p),
                        fnv_cstr_161fb0(p, 0x12345678u));
        }
        std::printf("]\n");
    } else {
        std::puts("fnv_cstr: all checks passed");
    }
    return 0;
}
