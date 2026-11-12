// CPU-01-STARTUP-SLOT-505: fixture for startup constructor index 505 (0x236030) and its two
// helpers 0x1621b0 / 0x162970. Proves the 2,704-byte table copy, the 8-byte final-vec3 tail, the
// 24-byte BSS seed copy, all 226 bounds updates, exact VMINSS/VMAXSS NaN and signed-zero rules,
// full-object writes (no 0x5a remains), and the null-source fail-closed stage.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_505_236030.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kSentinel = 0x5a;

std::uint32_t bits(float value) noexcept {
    std::uint32_t out;
    std::memcpy(&out, &value, sizeof out);
    return out;
}

float from_bits(std::uint32_t value) noexcept {
    float out;
    std::memcpy(&out, &value, sizeof out);
    return out;
}

template <class T>
void fill(T& obj) { std::memset(static_cast<void*>(&obj), kSentinel, sizeof obj); }

template <class T>
bool all_sentinel(const T& obj) {
    const auto* bytes = reinterpret_cast<const std::uint8_t*>(&obj);
    for (std::size_t i = 0; i < sizeof obj; ++i) if (bytes[i] != kSentinel) return false;
    return true;
}

template <class T>
bool no_sentinel(const T& obj) {
    const auto* bytes = reinterpret_cast<const std::uint8_t*>(&obj);
    for (std::size_t i = 0; i < sizeof obj; ++i) if (bytes[i] == kSentinel) return false;
    return true;
}

// All source floats use bit patterns without a 0x5a byte, so no_sentinel(object) proves that every
// byte of the 0xab0-byte object was written by the constructor.
std::array<std::uint8_t, kSlot505SourceCopySize> make_source() {
    alignas(4) std::array<std::uint8_t, kSlot505SourceCopySize> source{};
    auto* floats = reinterpret_cast<float*>(source.data());
    for (std::size_t i = 0; i < kSlot505SourceCopySize / sizeof(float); ++i) {
        floats[i] = 1.0f;
    }
    floats[0] = -2.0f;
    floats[1] = 3.0f;
    floats[2] = -4.0f;
    floats[3] = 5.0f;
    floats[4] = -6.0f;
    floats[5] = 7.0f;
    floats[675] = 10.0f;   // the final source float; the tail adds y/z of the last vec3
    return source;
}

BoundsRecord reference_bounds(const Slot505Objects& points, BoundsRecord bounds) noexcept {
    for (const BoundsVec3& p : points.points) {
        bounds.min.x = bounds.min.x < p.x ? bounds.min.x : p.x;
        bounds.min.y = bounds.min.y < p.y ? bounds.min.y : p.y;
        bounds.min.z = bounds.min.z < p.z ? bounds.min.z : p.z;
        bounds.max.x = bounds.max.x > p.x ? bounds.max.x : p.x;
        bounds.max.y = bounds.max.y > p.y ? bounds.max.y : p.y;
        bounds.max.z = bounds.max.z > p.z ? bounds.max.z : p.z;
    }
    return bounds;
}

Slot505Objects expected_object(const std::array<std::uint8_t, kSlot505SourceCopySize>& source,
                               BoundsRecord seed) {
    Slot505Objects expected{};
    std::memcpy(expected.points.data(), source.data(), kSlot505SourceCopySize);
    const std::uint64_t tail = kSlot505TailQword;
    std::memcpy(reinterpret_cast<std::uint8_t*>(expected.points.data()) + kSlot505TailOffset,
                &tail, sizeof tail);
    expected.bounds = reference_bounds(expected, seed);
    return expected;
}

void section_constants() {
    CHECK(kSlot505Rva == 0x236030 && kSlot505Size == 1476);
    CHECK(kSlot505Rva + kSlot505Size == 0x2365f4);
    CHECK(kSlot505TableWordRva == 0x2dd6118);
    CHECK(kSlot505SourceRva == 0xfc0580 && kSlot505SourceCopySize == 0xa90);
    CHECK(kSlot505TableRva == 0x2ee9820 && kSlot505TableSize == 0xa98);
    CHECK(kSlot505PointCount == 226 && kSlot505PointStride == 0xc);
    CHECK(kSlot505TailOffset == 0xa90 && kSlot505TailQword == 0x3f5415c8be50f81aull);
    CHECK(kSlot505BoundsRva == 0x2eea2b8 && kSlot505ObjectSize == 0xab0);
    CHECK(kSlot505LoopStep == 0xc && kSlot505LoopLimit == 0xa98);
    CHECK(kSlot505SeedCellRva == 0x2ee3ca0);
    CHECK(kSlot505MinMaxRva == 0x1621b0 && kSlot505SeedHelperRva == 0x162970);
    CHECK(kSlot505RetRva == 0x2365f3 && kSlot505PadByte == 0x90);
    CHECK(kSlot505BoundsRva == kSlot505TableRva + kSlot505TableSize);
    CHECK(kSlot505TableRva + kSlot505ObjectSize == 0x2eea2d0);
    CHECK(sizeof(Slot505Objects) == 0xab0);
    CHECK(offsetof(Slot505Objects, points) == 0);
    CHECK(offsetof(Slot505Objects, bounds) == 0xa98);
    CHECK(sizeof(BoundsVec3) == 12 && sizeof(BoundsRecord) == 24);
    CHECK(kBoundsMinMax1621b0Rva == 0x1621b0 && kBoundsMinMax1621b0Size == 87);
    CHECK(kBoundsSeed162970Rva == 0x162970 && kBoundsSeed162970Size == 8);
}

void section_helper_semantics() {
    const std::uint32_t nan_bits = 0x7fc12345u;
    const float nan = from_bits(nan_bits);
    const float positive_zero = from_bits(0x00000000u);
    const float negative_zero = from_bits(0x80000000u);

    // VMINSS/VMAXSS: any NaN returns the second (incoming) operand, preserving its payload.
    CHECK(bits(bounds_minss(2.0f, nan)) == nan_bits);
    CHECK(bits(bounds_maxss(2.0f, nan)) == nan_bits);
    CHECK(bounds_minss(nan, 2.0f) == 2.0f);
    CHECK(bounds_maxss(nan, 2.0f) == 2.0f);

    // Equality returns the second operand, so signed zero is not collapsed.
    CHECK(bits(bounds_minss(positive_zero, negative_zero)) == 0x80000000u);
    CHECK(bits(bounds_minss(negative_zero, positive_zero)) == 0x00000000u);
    CHECK(bits(bounds_maxss(negative_zero, positive_zero)) == 0x00000000u);
    CHECK(bits(bounds_maxss(positive_zero, negative_zero)) == 0x80000000u);

    BoundsRecord bounds{{1.0f, 2.0f, 3.0f}, {4.0f, 5.0f, 6.0f}};
    const BoundsVec3 point{-1.0f, nan, positive_zero};
    bounds_minmax_1621b0(bounds, point);
    CHECK(bounds.min.x == -1.0f && bounds.max.x == 4.0f);
    CHECK(bits(bounds.min.y) == nan_bits && bits(bounds.max.y) == nan_bits);
    CHECK(bits(bounds.min.z) == 0x00000000u && bounds.max.z == 6.0f);

    const BoundsRecord& seed = bounds_seed_162970();
    const BoundsRecord zero{};
    CHECK(&seed == &bounds_seed_162970());
    CHECK(std::memcmp(&seed, &zero, sizeof seed) == 0);
}

void section_default_seed() {
    const auto source = make_source();
    const auto source_copy = source;
    Slot505Objects object;
    fill(object);
    const Slot505Result result = init_slot505_236030(object, source.data());
    CHECK(result.ok && result.blocked == Slot505Stage::none);
    CHECK(result.source_copied && result.tail_written && result.seed_copied);
    CHECK(result.points == 226);

    // The source is copied, not modified.
    CHECK(std::memcmp(source.data(), source_copy.data(), source.size()) == 0);
    // Every object byte is written; the sentinel fill leaves no hole.
    CHECK(no_sentinel(object));
    const Slot505Objects expected = expected_object(source, BoundsRecord{});
    CHECK(std::memcmp(&object, &expected, sizeof object) == 0);

    // The final vec3 is source float 675 plus the two immediate dwords.
    CHECK(bits(object.points[225].x) == bits(10.0f));
    CHECK(bits(object.points[225].y) == 0xbe50f81au);
    CHECK(bits(object.points[225].z) == 0x3f5415c8u);
    CHECK(object.bounds.min.x == -2.0f && object.bounds.min.y == -6.0f &&
          object.bounds.min.z == -4.0f);
    CHECK(object.bounds.max.x == 10.0f && object.bounds.max.y == 3.0f &&
          object.bounds.max.z == 7.0f);
}

void section_explicit_seed() {
    const auto source = make_source();
    const BoundsRecord seed{{-100.0f, -200.0f, -300.0f}, {100.0f, 200.0f, 300.0f}};
    Slot505Hooks hooks{&seed};
    Slot505Objects object;
    fill(object);
    const Slot505Result result = init_slot505_236030(object, source.data(), hooks);
    CHECK(result.ok && result.seed_copied && result.points == 226);
    CHECK(no_sentinel(object));
    const Slot505Objects expected = expected_object(source, seed);
    CHECK(std::memcmp(&object, &expected, sizeof object) == 0);
    CHECK(object.bounds.min.x == -100.0f && object.bounds.min.y == -200.0f);
    CHECK(object.bounds.max.x == 100.0f && object.bounds.max.y == 200.0f);
}

void section_null_source() {
    // A null source is the fail-closed stage; the object must remain untouched.
    Slot505Objects object;
    fill(object);
    const Slot505Result result = init_slot505_236030(object, nullptr);
    CHECK(!result.ok && result.blocked == Slot505Stage::source);
    CHECK(!result.source_copied && !result.tail_written && !result.seed_copied);
    CHECK(result.points == 0);
    CHECK(all_sentinel(object));
}

void section_stage_names() {
    CHECK(std::strcmp(slot505_stage_name(Slot505Stage::none), "none") == 0);
    CHECK(std::strcmp(slot505_stage_name(Slot505Stage::source), "source") == 0);
}

}  // namespace

int main() {
    section_constants();
    section_helper_semantics();
    section_default_seed();
    section_explicit_seed();
    section_null_source();
    section_stage_names();
    std::puts("ctor_slot_505: all checks passed");
    return 0;
}
