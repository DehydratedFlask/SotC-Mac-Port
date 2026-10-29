#include "ctor_slot_419_56fae0.hpp"

namespace sotc {

namespace {

constexpr std::uint32_t kF5 = 0x40a00000u;     // 5.0f
constexpr std::uint32_t kF2 = 0x40000000u;     // 2.0f
constexpr std::uint32_t kHalf = 0x3f000000u;   // 0.5f
constexpr std::uint32_t kThird = 0x3eaaaaabu;  // ~0.3333f

void store_record(SplashRecord419& r, std::uint32_t a, std::uint32_t b, std::uint32_t c, std::uint8_t f0c,
                  std::uint8_t f0d, const char* path) noexcept {
    r.a = a;
    r.b = b;
    r.c = c;
    r.f0c = f0c;
    r.f0d = f0d;
    r.zero10 = 0;
    r.path = path;
    r.minus1 = -1;
}

}  // namespace

Slot419Result init_slot419_56fae0(Slot419Objects& o, const StoreTsc323d0Hooks& tsc_hooks) noexcept {
    Slot419Result out{};
    store_record(o.records[0], kF5, kHalf, 0, 0, 0, kSlot419Path0);          // 0x56faf5..0x56fb39
    store_record(o.records[1], kF5, kHalf, kThird, 0, 0, kSlot419Path1);      // 0x56fb43..0x56fb80
    store_record(o.records[2], kF2, kHalf, kThird, 0, 1, kSlot419Path2);      // 0x56fb8a..0x56fbc0
    store_record(o.records[3], kF2, kHalf, kThird, 0, 1, kSlot419Path3);      // 0x56fbd5..0x56fc15
    store_record(o.records[4], kF2, 0, kThird, 0, 1, kSlot419Path4);          // 0x56fc1c..0x56fc63
    store_tsc_323d0(&o.tsc, tsc_hooks);                                       // 0x56fc6d: jmp 0x323d0, rdi = 0x2f11878
    return out;
}

}  // namespace sotc
