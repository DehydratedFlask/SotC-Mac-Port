// CPU-01-AOT-CFG-SCALE-01-BATCH23 (slot 537): fixture for startup constructor index 537 (0x1324c0).
// Asserts the 8-byte zeroed word, the single __cxa_atexit record, the 40-half table at 0x2ee12f0 and
// the converter's behaviour at every boundary the pinned instructions define: signed zero,
// flush-to-zero, subnormal round-half-to-even, the 65520.0f saturation quirk, infinity and NaN payloads.
#include "l4_test_util.hpp"

#include <cstring>

#include "core/ctor_slot_537_1324c0.hpp"

namespace {
using namespace l4;
using sotc::slot537_f32_to_f16;

// The 40 stored halves, in store order. Derived from the port of 0x157e90 applied to
// kSlot537InputBits; tools/lift/check_ctor_slot_537.py recomputes them from the ELF's rodata with an
// independent Python port and fails if this table and the model ever disagree.
constexpr std::uint16_t kSlot537Halves[sotc::kSlot537Count] = {
    0x3f3f, 0xa842, 0xa8c0, 0xa953, 0x3ef3, 0xb2ee, 0xa8e0, 0xa76f,
    0x3d92, 0xb86a, 0xa6b8, 0xa185, 0x4317, 0x0000, 0x0000, 0x0000,
    0xac87, 0xab04, 0xa9d0, 0x2926, 0xac55, 0xaafb, 0xa828, 0x2952,
    0xaa4b, 0xa951, 0xa4da, 0x26c0, 0x0000, 0x0000, 0x0000, 0x0000,
    0xa90f, 0xa5a0, 0xa3c8, 0x0000, 0x3c00, 0x0000, 0x0000, 0x0000,
};

void section_constants() {
    CHECK(sotc::kSlot537Rva == 0x1324c0 && sotc::kSlot537Size == 795);
    CHECK(sotc::kSlot537TableWord == 0x2dd70e0 - 8 * 537);
    CHECK(sotc::kSlot537TableWord == 0x2dd6018);
    CHECK(sotc::kSlot537Object == 0x2ee12e0 && sotc::kSlot537Table == 0x2ee12f0);
    CHECK(sotc::kSlot537Count == 40 && sotc::kSlot537Stride == 2);
    CHECK(sotc::kSlot537Table + (sotc::kSlot537Count - 1) * sotc::kSlot537Stride == sotc::kSlot537LastHalf);
    CHECK(sotc::kSlot537LastHalf == 0x2ee133e);
    CHECK(sotc::kSlot537Dtor == 0x131f00 && sotc::kSlot537DsoHandle == 0x1604218);
    CHECK(sotc::kSlot537AtexitRva == 0xf8f390);
    CHECK(sotc::kSlot537ConverterRva == 0x157e90 && sotc::kSlot537ConverterSize == 203);

    // 29 rip-relative loads and 11 zero vectors, in call order.
    std::size_t zeros = 0;
    for (std::size_t i = 0; i < sotc::kSlot537Count; ++i) {
        if (sotc::kSlot537InputRvas[i] == 0) {
            ++zeros;
            CHECK(sotc::kSlot537InputBits[i] == 0);
        } else {
            CHECK(sotc::kSlot537InputRvas[i] >= 0xfa7718 && sotc::kSlot537InputRvas[i] <= 0xfa7788);
        }
    }
    CHECK(zeros == sotc::kSlot537ZeroInputs && zeros == 11);
    CHECK(sotc::kSlot537Count - zeros == 29);
    CHECK(sotc::kSlot537InputBits[36] == 0x3f800000u);   // the single 1.0f input
    CHECK(sotc::kSlot537InputRvas[36] == 0xfa7788);
}

void section_converter_zero_and_sign() {
    CHECK(slot537_f32_to_f16(0x00000000u) == 0x0000);   // +0.0
    CHECK(slot537_f32_to_f16(0x80000000u) == 0x8000);   // -0.0 keeps the sign
    CHECK(slot537_f32_to_f16(0x3f800000u) == 0x3c00);   // 1.0
    CHECK(slot537_f32_to_f16(0xbf800000u) == 0xbc00);   // -1.0
    CHECK(slot537_f32_to_f16(0x40000000u) == 0x4000);   // 2.0
}

void section_converter_rounding() {
    // 0x3f801000 is exactly halfway between 1.0 and 1.0 + 2^-10; ties-to-even keeps 1.0.
    CHECK(slot537_f32_to_f16(0x3f801000u) == 0x3c00);
    // 0x3f803000 is halfway between the next two; ties-to-even moves up to the even mantissa.
    CHECK(slot537_f32_to_f16(0x3f803000u) == 0x3c02);
    // Just below the first tie rounds down, just above it rounds up.
    CHECK(slot537_f32_to_f16(0x3f800fffu) == 0x3c00);
    CHECK(slot537_f32_to_f16(0x3f801001u) == 0x3c01);
    // Rounding is NOT truncation: 1.0 + 2^-10 + 2^-11 is above the tie and must carry.
    CHECK(slot537_f32_to_f16(0x3f803000u) == 0x3c02);
    CHECK(slot537_f32_to_f16(0x3f802000u) == 0x3c01);   // exact 1 + 2^-9 ... mantissa 0x2000
}

void section_converter_subnormal() {
    // 2^-24 is the smallest half subnormal.
    CHECK(slot537_f32_to_f16(0x33800000u) == 0x0001);
    // 2^-25 is exactly half of it: ties-to-even gives zero.
    CHECK(slot537_f32_to_f16(0x33000000u) == 0x0000);
    // 3 * 2^-26 is above that tie and rounds to 0x0001.
    CHECK(slot537_f32_to_f16(0x33400000u) == 0x0001);
    // 2^-14 is the smallest normal half: flush-to-zero does not apply yet.
    CHECK(slot537_f32_to_f16(0x38800000u) == 0x0400);
    // e <= 0x65 flushes to a signed zero, including the whole binary32 subnormal range.
    CHECK(slot537_f32_to_f16(0x00800000u) == 0x0000);   // 2^-126
    CHECK(slot537_f32_to_f16(0x00000001u) == 0x0000);   // smallest binary32 subnormal
    CHECK(slot537_f32_to_f16(0x80000001u) == 0x8000);   // ... with the sign preserved
}

void section_converter_overflow_and_nan() {
    CHECK(slot537_f32_to_f16(0x477fe000u) == 0x7bff);   // 65504.0, the largest finite half
    // 65520.0 has bit 13 set, so `mant + bit13 + 0xfff` carries into bit 23: the result saturates.
    CHECK(slot537_f32_to_f16(0x477ff000u) == 0x7c00);
    CHECK(slot537_f32_to_f16(0x477ffc00u) == 0x7c00);
    CHECK(slot537_f32_to_f16(0x477fe001u) == 0x7bff);   // just above 65504 still rounds down
    CHECK(slot537_f32_to_f16(0x47800000u) == 0x7c00);   // 65536.0
    CHECK(slot537_f32_to_f16(0x7f800000u) == 0x7c00);   // +inf
    CHECK(slot537_f32_to_f16(0xff800000u) == 0xfc00);   // -inf
    // NaN payload is the top ten mantissa bits; the smallest payloads force bit 0 so the result is
    // never plain infinity.
    CHECK(slot537_f32_to_f16(0x7fc00001u) == 0x7e00);
    CHECK(slot537_f32_to_f16(0x7f800001u) == 0x7c01);
    CHECK(slot537_f32_to_f16(0xffc00001u) == 0xfe00);
    CHECK(slot537_f32_to_f16(0x7fffffbfu) == 0x7fff);
}

void run_full() {
    StartupAtexitLog log;
    sotc::Slot537Objects o;
    poison(o);
    const sotc::Slot537Result res = sotc::init_slot537_1324c0(o, log, sotc::kSlot537InputBits);

    CHECK(res.ok);
    CHECK(res.halves_written == sotc::kSlot537Count);
    CHECK(res.atexit_records == 1);                   // one record, not one per conversion
    CHECK(log.size() == 1);
    CHECK(log.at(0).dtor_rva == sotc::kSlot537Dtor);
    CHECK(log.at(0).object == &o.header);
    CHECK(o.header == 0);

    for (std::size_t i = 0; i < sotc::kSlot537Count; ++i) {
        CHECK(o.halves[i] == kSlot537Halves[i]);
        // The model must reproduce a fresh application of the port, not a stored table.
        CHECK(o.halves[i] == slot537_f32_to_f16(sotc::kSlot537InputBits[i]));
    }

    // 30 distinct results among the 40: 11 zero inputs share one, and two values repeat.
    std::size_t distinct = 0;
    for (std::size_t i = 0; i < sotc::kSlot537Count; ++i) {
        bool first = true;
        for (std::size_t j = 0; j < i; ++j)
            if (kSlot537Halves[j] == kSlot537Halves[i]) first = false;
        if (first) ++distinct;
    }
    CHECK(distinct == 30);
}

void run_zero_inputs() {
    StartupAtexitLog log;
    sotc::Slot537Objects o;
    poison(o);
    CHECK(sotc::init_slot537_1324c0(o, log, sotc::kSlot537InputBits).ok);
    for (std::size_t i = 0; i < sotc::kSlot537Count; ++i)
        if (sotc::kSlot537InputBits[i] == 0) CHECK(o.halves[i] == 0x0000);
}

void run_refuses_null() {
    StartupAtexitLog log;
    sotc::Slot537Objects o;
    poison(o);
    const sotc::Slot537Result res = sotc::init_slot537_1324c0(o, log, nullptr);
    CHECK(!res.ok && res.halves_written == 0 && res.atexit_records == 0);
    CHECK(log.size() == 0);
    CHECK(o.header == 0xA5A5A5A5A5A5A5A5ull);
}

}  // namespace

int main() {
    section_constants();
    section_converter_zero_and_sign();
    section_converter_rounding();
    section_converter_subnormal();
    section_converter_overflow_and_nan();
    run_full();
    run_zero_inputs();
    run_refuses_null();
    std::printf("OK test_ctor_slot_537\n");
    return 0;
}
