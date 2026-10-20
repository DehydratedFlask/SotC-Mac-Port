// SRV-01: free-list region split surgery from original RVA 0x1a8b6..0x1a8fa
// (runs on a fresh node returned by 0x19660). Not a lift of 0x19660 itself —
// that function's replenish path calls 0x19e00. See check_f1a8b6_split.py.
#pragma once

#include <cstdint>
#include <cstring>
#include <optional>

#include "free_list_carve.hpp"

namespace sotc {

struct RegionNode {
    static constexpr std::size_t kBytes = 0x70;
    static constexpr std::size_t kSizeOffset = 0x58;
    static constexpr std::size_t kStartOffset = 0x60;
    static constexpr std::size_t kFlagAOffset = 0x68;
    static constexpr std::size_t kFlagBOffset = 0x69;
    static constexpr std::size_t kPtrOffset = 0x40;
    static constexpr std::size_t kSentinelOffset = 0x48;
    static constexpr std::size_t kCapOffset = 0x50;

    alignas(8) std::uint8_t raw[kBytes]{};

    std::uint64_t size() const noexcept {
        std::uint64_t v; std::memcpy(&v, raw + kSizeOffset, 8); return v;
    }
    std::uint64_t start() const noexcept {
        std::uint64_t v; std::memcpy(&v, raw + kStartOffset, 8); return v;
    }
    std::uint8_t flag_a() const noexcept { return raw[kFlagAOffset]; }
    std::uint8_t flag_b() const noexcept { return raw[kFlagBOffset]; }
    std::uint64_t ptr_field() const noexcept {
        std::uint64_t v; std::memcpy(&v, raw + kPtrOffset, 8); return v;
    }
    std::uint32_t sentinel() const noexcept {
        std::uint32_t v; std::memcpy(&v, raw + kSentinelOffset, 4); return v;
    }
    std::uint64_t capacity() const noexcept {
        std::uint64_t v; std::memcpy(&v, raw + kCapOffset, 8); return v;
    }

    void set_size(std::uint64_t v) noexcept { std::memcpy(raw + kSizeOffset, &v, 8); }
    void set_start(std::uint64_t v) noexcept { std::memcpy(raw + kStartOffset, &v, 8); }
    void set_flag_a(std::uint8_t v) noexcept { raw[kFlagAOffset] = v; }
    void set_capacity(std::uint64_t v) noexcept { std::memcpy(raw + kCapOffset, &v, 8); }
};

bool split_free_at_lead(RegionNode& free, RegionNode& neu, std::uint64_t lead) noexcept;

std::optional<CarveFit> split_using_carve_fit(RegionNode& free, RegionNode& neu,
                                              std::uint64_t req_size,
                                              std::uint32_t alignment) noexcept;

}  // namespace sotc
