#include "const_table_ee1130.hpp"

namespace sotc {
namespace {

// Little-endian byte images of the qwords/vectors stored by 0xee1130, in original store order.
constexpr std::array<std::uint8_t, 8> kZero{0, 0, 0, 0, 0, 0, 0, 0};
constexpr std::array<std::uint8_t, 8> kAllOnes{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
constexpr std::array<std::uint8_t, 8> kHighAllOnes{0x00, 0x00, 0x00, 0x00,
                                                    0xff, 0xff, 0xff, 0xff};
constexpr std::array<std::uint8_t, 8> kLowAllOnes{0xff, 0xff, 0xff, 0xff,
                                                  0x00, 0x00, 0x00, 0x00};
constexpr std::array<std::uint8_t, 8> kNegOnePair{0x00, 0x00, 0x80, 0xbf,
                                                  0x00, 0x00, 0x80, 0xbf};
constexpr std::array<std::uint8_t, 8> kOneNegOne{0x00, 0x00, 0x80, 0xbf,
                                                 0x00, 0x00, 0x80, 0x3f};
constexpr std::array<std::uint8_t, 8> kFloatMaxZero{0x00, 0x00, 0x00, 0x00,
                                                    0xff, 0xff, 0x7f, 0x7f};
constexpr std::array<std::uint8_t, 8> kNegFloatMaxPair{0xff, 0xff, 0x7f, 0xff,
                                                       0xff, 0xff, 0x7f, 0xff};
constexpr std::array<std::uint8_t, 8> kZeroNegFloatMax{0xff, 0xff, 0x7f, 0xff,
                                                       0x00, 0x00, 0x00, 0x00};
constexpr std::array<std::uint8_t, 16> kSmallFloat4{
    0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x34,
    0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x34};

void put(ConstTableEe1130& table, std::size_t offset, const auto& bytes) noexcept {
    for (std::size_t i = 0; i < bytes.size(); ++i)
        table[offset + i] = bytes[i];
}

}  // namespace

void init_const_table_ee1130(ConstTableEe1130& table) noexcept {
    put(table, 0x00, kZero);
    put(table, 0x08, kHighAllOnes);     // rax = [0x1323b50]
    put(table, 0x10, kAllOnes);
    put(table, 0x18, kZero);
    put(table, 0x20, kAllOnes);
    put(table, 0x28, kLowAllOnes);      // vmovss [0x1323bdc] = 0xffffffff
    put(table, 0x30, kHighAllOnes);     // rax (unchanged)
    put(table, 0x38, kAllOnes);
    put(table, 0x40, kSmallFloat4);     // vmovups [0x1323b60]
    put(table, 0x50, kNegOnePair);      // rdx = [0x1323b70]
    put(table, 0x58, kOneNegOne);       // rcx = [0x1323b80]
    put(table, 0x60, kZero);
    put(table, 0x68, kFloatMaxZero);    // rdx = [0x1323b90]
    put(table, 0x70, kNegFloatMaxPair); // rcx = [0x1323ba0]
    put(table, 0x78, kZeroNegFloatMax); // vmovss [0x1323be0] = 0xff7fffff
}

}  // namespace sotc
