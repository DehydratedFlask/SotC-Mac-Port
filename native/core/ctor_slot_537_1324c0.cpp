// CPU-01-AOT-CFG-SCALE-01-BATCH23 (slot 537): see ctor_slot_537_1324c0.hpp.
#include "ctor_slot_537_1324c0.hpp"

namespace sotc {
namespace {

// x86 shift semantics, mirrored so the port cannot drift from the pinned instructions: the count is
// masked to 5 bits and `sar` is arithmetic on a 32-bit register.
constexpr std::uint32_t shl32(std::uint32_t value, std::uint32_t count) noexcept {
    return value << (count & 31u);
}
constexpr std::uint32_t shr32(std::uint32_t value, std::uint32_t count) noexcept {
    return value >> (count & 31u);
}
constexpr std::uint32_t sar32(std::uint32_t value, std::uint32_t count) noexcept {
    const std::uint32_t n = count & 31u;
    const std::int32_t signed_value = static_cast<std::int32_t>(value);
    // Arithmetic shift: defined in C++20 and performed as such by every supported compiler before it.
    return static_cast<std::uint32_t>(signed_value >> n);
}

}  // namespace

std::uint16_t slot537_f32_to_f16(std::uint32_t bits) noexcept {
    const std::uint32_t exp = (bits >> 23) & 0xffu;          // bextr 0x817
    const std::uint32_t mant = bits & 0x7fffffu;             // and esi, 0x7fffff
    const std::uint32_t sign = (bits >> 16) & 0x8000u;       // shr 0x10 / and 0x8000
    const std::int32_t ecx = static_cast<std::int32_t>(exp) - 0x70;

    if (ecx <= 0) {
        // 0x157eda: e <= 0x70.
        if (ecx < -0xa) return static_cast<std::uint16_t>(sign);   // 0x157f54: signed zero
        const std::uint32_t m = mant | 0x800000u;                  // restore the implicit bit
        const std::uint32_t shift_lo = 0x7du - exp;                // 0x7d - e
        const std::uint32_t shift_hi = 0x7eu - exp;                // 0x7e - e
        std::uint32_t round = shl32(1u, shift_lo);                 // 1 << (0x7d - e)
        std::uint32_t value = shr32(m, shift_hi);                  // m >> (0x7e - e), before the add
        round += m;                                                // addl %esi,%edi
        value &= 1u;                                               // and $1
        value = round - 1u + value;                                // lea -1(rdx,rdi)
        value = sar32(value, shift_hi);                            // sarl (0x7e - e)
        return static_cast<std::uint16_t>(sign | value);
    }

    if (ecx == 0x8f) {
        // 0x157ec0: e == 0xff, infinity or NaN.
        if (mant == 0) return static_cast<std::uint16_t>(sign | 0x7c00u);
        const std::uint32_t payload = mant >> 13;
        const std::uint32_t force_low = (payload == 0) ? 1u : 0u;  // sete %cl
        return static_cast<std::uint16_t>(sign | payload | force_low | 0x7c00u);
    }

    // 0x157f12: normal path, round half to even.
    std::uint32_t result_exp = exp - 0x6fu;                    // e - 0x6f, the "carry" hypothesis
    const std::uint32_t low_bit = (bits >> 13) & 1u;           // bextr 0x10d
    const std::uint32_t d = mant + low_bit + 0xfffu;           // mant + lsb + 0xfff
    const std::uint32_t carry = d & 0x800000u;                 // did the mantissa carry out?
    if (carry == 0) result_exp = exp - 0x70u;                   // cmove ecx, r9d
    if (static_cast<std::int32_t>(result_exp) >= 0x1f) return static_cast<std::uint16_t>(sign | 0x7c00u);
    result_exp <<= 10;                                          // shl $0xa
    const std::uint32_t shifted = d >> 13;                      // shr $0xd
    const std::uint32_t payload = (carry == 0) ? shifted : 0u;  // dropped when the carry landed
    return static_cast<std::uint16_t>(sign | payload | result_exp);
}

Slot537Result init_slot537_1324c0(Slot537Objects& objects, StartupAtexitLog& log,
                                  const std::uint32_t* bits) noexcept {
    Slot537Result out{};
    if (bits == nullptr) return out;

    // movq $0, (0x2ee12e0): the registered object is this 8-byte word.
    objects.header = 0;
    if (!log.record(kSlot537Dtor, &objects.header)) return out;   // __cxa_atexit(0x131f00, obj, dso)
    ++out.atexit_records;

    for (std::size_t i = 0; i < kSlot537Count; ++i) {
        // call 0x157e90, then `movw %ax, 0x2ee12f0 + 2 * i` with the result of this very call.
        objects.halves[i] = slot537_f32_to_f16(bits[i]);
        ++out.halves_written;
    }

    out.ok = out.atexit_records == 1 && out.halves_written == kSlot537Count;
    return out;
}

}  // namespace sotc
