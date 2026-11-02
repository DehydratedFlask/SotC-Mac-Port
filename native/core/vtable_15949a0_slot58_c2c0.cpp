#include "vtable_15949a0_slot58_c2c0.hpp"

#include <cstring>

namespace sotc {

Vtable15949a0Slot58_c2c0Result vtable_15949a0_slot58_c2c0(
    const Vtable15949a0Slot58_c2c0Cells& cells) noexcept {
    Vtable15949a0Slot58_c2c0Result out{};
    if (cells.object == nullptr || cells.source == nullptr) {
        return out;  // both operands are dereferenced unconditionally
    }

    // Store order matches the body: stamp first, then the source read, then the copy.
    const std::uint64_t stamp = kVtable15949a0Slot58_c2c0StampAddress;
    std::memcpy(cells.object + kVtable15949a0Slot58_c2c0StampOffset, &stamp, sizeof(stamp));

    std::uint64_t copied = 0;
    std::memcpy(&copied, cells.source + kVtable15949a0Slot58_c2c0SourceOffset, sizeof(copied));

    std::uint64_t previous = 0;
    std::memcpy(&previous, cells.object + kVtable15949a0Slot58_c2c0CopyOffset, sizeof(previous));

    std::memcpy(cells.object + kVtable15949a0Slot58_c2c0CopyOffset, &copied, sizeof(copied));

    out.stamp = stamp;
    out.copied = copied;
    out.previous = previous;
    out.ok = true;
    return out;
}

}  // namespace sotc
