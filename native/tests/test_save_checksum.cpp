#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <span>

#include "core/save_checksum.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main() {
    constexpr std::uint32_t offset_basis = 0x811c9dc5u;
    const std::span<const std::byte> empty;
    CHECK(sotc::fnv1a32(empty, offset_basis) == 0x811c9dc5u);
    CHECK(sotc::fnv1a32(empty, 0x12345678u) == 0x12345678u);

    constexpr std::array a{std::byte{0x61}};
    constexpr std::array foo{std::byte{0x66}, std::byte{0x6f}, std::byte{0x6f}};
    constexpr std::array bar{std::byte{0x62}, std::byte{0x61}, std::byte{0x72}};
    constexpr std::array foobar{std::byte{0x66}, std::byte{0x6f}, std::byte{0x6f},
                                std::byte{0x62}, std::byte{0x61}, std::byte{0x72}};
    CHECK(sotc::fnv1a32(a, offset_basis) == 0xe40c292cu);
    CHECK(sotc::fnv1a32(foobar, offset_basis) == 0xbf9cf968u);

    constexpr std::array custom{std::byte{0x01}, std::byte{0x23}, std::byte{0x45}, std::byte{0x67},
                                std::byte{0x89}, std::byte{0xab}, std::byte{0xcd}, std::byte{0xef}};
    CHECK(sotc::fnv1a32(custom, 0x12345678u) == 0x705ef638u);

    constexpr std::array high_bits{std::byte{0x80}, std::byte{0xff}, std::byte{0x7f}};
    CHECK(sotc::fnv1a32(high_bits, offset_basis) == 0xbbbb958du);
    constexpr std::array with_nul{std::byte{0x00}, std::byte{0x61}, std::byte{0x00}};
    CHECK(sotc::fnv1a32(with_nul, offset_basis) == 0x659c64aeu);
    constexpr std::array wrapping{std::byte{0xff}, std::byte{0x80}, std::byte{0x00}, std::byte{0x7f},
                                  std::byte{0x55}, std::byte{0xaa}, std::byte{0x01}};
    CHECK(sotc::fnv1a32(wrapping, 0xfffffff0u) == 0x45478e30u);

    const std::uint32_t after_foo = sotc::fnv1a32(foo, offset_basis);
    CHECK(after_foo == 0xa9f37ed7u);
    CHECK(sotc::fnv1a32(bar, after_foo) == 0xbf9cf968u);
    CHECK(sotc::fnv1a32(foobar, offset_basis) == sotc::fnv1a32(bar, after_foo));

    std::puts("save_checksum: all checks passed");
    return 0;
}
