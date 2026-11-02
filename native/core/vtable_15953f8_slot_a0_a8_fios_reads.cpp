#include "vtable_15953f8_slot_a0_a8_fios_reads.hpp"

namespace sotc {
namespace {

bool fields_ready(const FiosReadCells& cells) {
    return cells.count != nullptr && cells.status != nullptr && cells.marker != nullptr;
}

}  // namespace

FiosReadResult vtable_15953f8_slot_a0_31870(const FiosReadCells& cells,
                                            const FiosReadHooks& hooks) noexcept {
    FiosReadResult out{};
    if (!fields_ready(cells)) {
        return out;  // every store and the read are unconditional in the retail body
    }
    *cells.marker = kVtable15953f8Marker;
    out.marker_written = true;

    if (hooks.readv != nullptr) {
        out.service_called = true;
        out.status = hooks.readv(nullptr, *cells.count, cells.buffer, cells.length);
    }
    *cells.status = out.status;
    out.ok = true;
    return out;
}

FiosReadResult vtable_15953f8_slot_a8_318b0(const FiosReadCells& cells,
                                            const FiosReadHooks& hooks) noexcept {
    FiosReadResult out{};
    if (!fields_ready(cells)) {
        return out;
    }
    *cells.marker = kVtable15953f8Marker;
    out.marker_written = true;

    if (hooks.preadv != nullptr) {
        out.service_called = true;
        out.status = hooks.preadv(nullptr, *cells.count, cells.buffer, cells.length, cells.offset);
    }
    *cells.status = out.status;
    out.ok = true;
    return out;
}

}  // namespace sotc
