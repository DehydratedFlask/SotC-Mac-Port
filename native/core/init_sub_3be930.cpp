#include "init_sub_3be930.hpp"

#include <cstring>

namespace sotc {
namespace {

// Stable host identity for OBS address 0x2dd9f78 (lea target; not a load).
alignas(8) std::uint8_t g_slot_vtable_2dd9f78_blob[8]{};

constexpr std::uint64_t kSingles[15] = {
    0x98b0,  0x13150, 0x1c9f0, 0x26290, 0x2fb30, 0x393d0, 0x42c70, 0x4c510,
    0x55db0, 0x5f650, 0x68ef0, 0x72790, 0x7c030, 0x858d0, 0x8f170,
};
constexpr std::uint64_t kBanks[15] = {
    0x9948,  0x131e8, 0x1ca88, 0x26328, 0x2fbc8, 0x39468, 0x42d08, 0x4c5a8,
    0x55e48, 0x5f6e8, 0x68f88, 0x72828, 0x7c0c8, 0x85968, 0x8f208,
};

}  // namespace

void* slot_vtable_2dd9f78() noexcept { return g_slot_vtable_2dd9f78_blob; }

void init_slot_f180(void* at) noexcept {
    if (at == nullptr) {
        return;
    }
    auto* p = static_cast<std::uint8_t*>(at);
    void* vt = slot_vtable_2dd9f78();
    std::memcpy(p, &vt, sizeof(void*));
    std::memset(p + 8, 0, 16);
    std::memset(p + 0x18, 0, 8);
}

void init_block_1d9c0(void* obj) noexcept {
    if (obj == nullptr) {
        return;
    }
    auto* b = static_cast<std::uint8_t*>(obj);
    // OBS: call 0xf180(obj+8)
    init_slot_f180(b + 8);
    void* mid = b + 0x58;
    void* end = b + 0x98;
    std::memcpy(b + 0x50, &mid, sizeof(void*));
    std::memcpy(b + 0x30, &mid, sizeof(void*));
    std::memcpy(b + 0x40, &end, sizeof(void*));
    std::memset(b + 0x00, 0, 4);
    std::memset(b + 0x04, 0, 4);
    std::memset(b + 0x28, 0, 8);
    std::memcpy(b + 0x38, &mid, sizeof(void*));
}

void init_sub_3be930(void* at_plus_68) noexcept {
    if (at_plus_68 == nullptr) {
        return;
    }
    auto* base = static_cast<std::uint8_t*>(at_plus_68);
    base[0] = 0;
    std::memset(base + 4, 0, 4);

    // OBS: call 0x1d9c0(base+0x10); then loop off=0xa8; off!=0x98a8; off+=0x98
    init_block_1d9c0(base + 0x10);
    for (std::uint64_t off = 0xa8; off != 0x98a8; off += 0x98) {
        init_block_1d9c0(base + off);
    }

    // OBS: 15× (single + bank of 256×0x98)
    for (std::size_t i = 0; i < 15; ++i) {
        init_block_1d9c0(base + kSingles[i]);
        for (std::uint64_t off = 0; off != 0x9800; off += 0x98) {
            init_block_1d9c0(base + kBanks[i] + off);
        }
    }

    std::memset(base + 0x98a10, 0, 4);
    std::memset(base + 0x98a18, 0, 8);
}

}  // namespace sotc
