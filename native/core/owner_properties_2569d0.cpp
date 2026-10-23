#include "owner_properties_2569d0.hpp"

namespace sotc {
namespace {

constexpr std::uint8_t kPathGate1Head = 0;
constexpr std::uint8_t kPathGate0Head = 1;
constexpr std::uint8_t kPathTail = 2;
constexpr std::uint8_t kPathFinal = 3;
constexpr std::uint8_t kPathGate0Pending = 5;
constexpr std::uint8_t kPathGate1Pending = 6;

bool emit_row(const detail::OwnerPropertyRow& row, OwnerPropertySink& sink) noexcept {
    if (row.map == 0) {
        return sink.insert_a(row.key, row.i, row.f, row.tag);
    }
    return sink.insert_b(row.key, row.lo, row.hi, row.v, row.tag);
}

bool emit_path(std::uint8_t path, OwnerPropertySink& sink, OwnerPropertiesResult& out) noexcept {
    for (const detail::OwnerPropertyRow& row : detail::kOwnerProperties) {
        if (row.path != path) {
            continue;
        }
        if (!emit_row(row, sink)) {
            out.failed_key = row.key;
            return false;
        }
        ++out.inserted;
    }
    return true;
}

}  // namespace

OwnerMode2569d0 classify_owner_mode(bool retail_build, std::int32_t mode_field) noexcept {
    if (mode_field == 2) {
        return OwnerMode2569d0::mode2_unreachable;
    }
    if (mode_field != 1) {
        return OwnerMode2569d0::none;
    }
    return retail_build ? OwnerMode2569d0::retail_gate1 : OwnerMode2569d0::retail_gate0;
}

OwnerPropertiesResult apply_owner_properties(OwnerMode2569d0 mode,
                                             OwnerPropertySink& sink) noexcept {
    OwnerPropertiesResult out{};
    switch (mode) {
        case OwnerMode2569d0::none:
            out.ok = true;
            return out;
        case OwnerMode2569d0::mode2_unreachable:
            return out;  // explicit failure: this path is not executed from 0x2569d0
        case OwnerMode2569d0::retail_gate1:
            out.ok = emit_path(kPathGate1Head, sink, out) &&
                     emit_path(kPathGate1Pending, sink, out) &&
                     emit_path(kPathTail, sink, out) && emit_path(kPathFinal, sink, out);
            return out;
        case OwnerMode2569d0::retail_gate0:
            out.ok = emit_path(kPathGate0Head, sink, out) &&
                     emit_path(kPathGate0Pending, sink, out) &&
                     emit_path(kPathTail, sink, out) && emit_path(kPathFinal, sink, out);
            return out;
    }
    return out;
}

}  // namespace sotc
