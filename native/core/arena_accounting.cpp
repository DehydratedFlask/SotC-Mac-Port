#include "arena_accounting.hpp"

namespace sotc {
namespace {

std::uint64_t read_u64_le(std::span<const std::byte> raw,
                          std::size_t offset) noexcept {
    std::uint64_t value = 0;
    for (std::size_t i = 0; i < sizeof(value); ++i) {
        value |= static_cast<std::uint64_t>(std::to_integer<unsigned char>(raw[offset + i]))
                 << (i * 8);
    }
    return value;
}

void write_u64_le(std::span<std::byte> raw, std::size_t offset,
                  std::uint64_t value) noexcept {
    for (std::size_t i = 0; i < sizeof(value); ++i) {
        raw[offset + i] = std::byte{static_cast<unsigned char>(value >> (i * 8))};
    }
}

}  // namespace

bool apply_f19e00_accounting(std::span<std::byte> raw,
                             std::uint64_t requested_bytes,
                             std::uint32_t mode) noexcept {
    if (raw.size() < kF19E00RawBytes) return false;

    const std::uint64_t next = read_u64_le(raw, kF19E00Offset80) + requested_bytes;
    write_u64_le(raw, kF19E00Offset80, next);

    if (next > read_u64_le(raw, kF19E00Offset88)) {
        write_u64_le(raw, kF19E00Offset88, next);
    }

    if (mode == 2) {
        const std::uint64_t value = read_u64_le(raw, kF19E00Offset90) - requested_bytes;
        write_u64_le(raw, kF19E00Offset90, value);
    }

    return true;
}

}  // namespace sotc
