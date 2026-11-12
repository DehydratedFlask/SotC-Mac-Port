// CPU-01-STARTUP-SLOT-505: see ctor_slot_505_236030.hpp.
#include "ctor_slot_505_236030.hpp"

#include <cstring>

namespace sotc {

const char* slot505_stage_name(Slot505Stage stage) noexcept {
    switch (stage) {
        case Slot505Stage::none: return "none";
        case Slot505Stage::source: return "source";
    }
    return "";
}

Slot505Result init_slot505_236030(Slot505Objects& o, const std::uint8_t* source,
                                  const Slot505Hooks& hooks) noexcept {
    Slot505Result out{};

    // 0x23603a: the original reads 0xfc0580 unconditionally. A null host source cannot model that
    // read, so fail closed before touching the object.
    if (source == nullptr) {
        out.blocked = Slot505Stage::source;
        return out;
    }

    // 0x23603a..0x236052 / 0x236064..0x236584: 84 32-byte chunks and the final 16-byte xmm chunk.
    std::memcpy(o.points.data(), source, kSlot505SourceCopySize);
    out.source_copied = true;

    // 0x23605a/0x236594: the qword completes the final vec3; it is not source data.
    const std::uint64_t tail = kSlot505TailQword;
    std::memcpy(reinterpret_cast<std::uint8_t*>(o.points.data()) + kSlot505TailOffset,
                &tail, sizeof tail);
    out.tail_written = true;

    // 0x162970/0x2365a0..0x2365bf: copy the 24-byte seed record to table + 0xa98.
    const BoundsRecord& seed = hooks.seed != nullptr ? *hooks.seed : bounds_seed_162970();
    o.bounds = seed;
    out.seed_copied = true;

    // 0x2365c7..0x2365e7: the loop body is exactly 226 calls at stride 0xc.
    for (std::size_t i = 0; i < kSlot505PointCount; ++i) {
        bounds_minmax_1621b0(o.bounds, o.points[i]);
    }
    out.points = kSlot505PointCount;

    out.blocked = Slot505Stage::none;
    out.ok = true;
    return out;
}

}  // namespace sotc
