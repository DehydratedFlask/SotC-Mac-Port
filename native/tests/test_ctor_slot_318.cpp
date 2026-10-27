// CPU-01-STARTUP-SLOT-318-01: fixture for startup constructor index 318 (0xa20e80): ten small global
// input-state objects built by plain stores, ten __cxa_atexit registrations and two process counters.
// Every stored byte is compared against an independently written sparse expectation; every other byte
// must still hold the poison value.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>

#include "core/ctor_slot_318_a20e80.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;

struct Store {
    std::size_t off;
    std::size_t len;
    std::uint64_t value;   // little endian, len <= 8; len 16 / 32 means that many zero bytes
};

template <std::size_t N>
void expect_image(const Slot318Raw<N>& obj, std::initializer_list<Store> stores) {
    std::uint8_t want[N];
    std::memset(want, kPoison, N);
    for (const Store& s : stores) {
        CHECK(s.off + s.len <= N);
        if (s.len > 8) {
            std::memset(want + s.off, 0, s.len);
        } else {
            for (std::size_t i = 0; i < s.len; ++i) want[s.off + i] = static_cast<std::uint8_t>(s.value >> (8 * i));
        }
    }
    for (std::size_t i = 0; i < N; ++i) {
        if (obj.bytes[i] != want[i]) {
            std::fprintf(stderr, "FAIL byte +%#zx is %#x, want %#x\n", i, obj.bytes[i], want[i]);
            std::exit(1);
        }
    }
}

template <std::size_t N>
void poison(Slot318Raw<N>& obj) { std::memset(obj.bytes.data(), kPoison, N); }

void poison_all(Slot318Objects& o) {
    poison(o.a); poison(o.b); poison(o.c); poison(o.d); poison(o.e);
    poison(o.f); poison(o.g1); poison(o.g2); poison(o.g3); poison(o.g4);
}

void section_constants() {
    CHECK(kSlot318Rva == 0xa20e80 && kSlot318BodySize == 458 && kSlot318DsoHandle == 0x1604218);
    CHECK(kSlot318BaseAVptr == 0x15d5790 && kSlot318BaseBVptr == 0x15d57c0);
    CHECK(kSlot318ClassAVptr == 0x15d5390 && kSlot318ClassEVptr == 0x15d5480);
    CHECK(kSlot318ObjA == 0x32f3418 && kSlot318ObjG4 == 0x32f3948);
    CHECK(kSlot318ObjB - kSlot318ObjA == sizeof(Slot318Raw<0x70>) && kSlot318ObjC - kSlot318ObjB == sizeof(Slot318Raw<0x58>));
    CHECK(kSlot318ObjD - kSlot318ObjC == sizeof(Slot318Raw<0x148>) && kSlot318ObjE - kSlot318ObjD == sizeof(Slot318Raw<0xa8>));
    CHECK(kSlot318ObjF - kSlot318ObjE == sizeof(Slot318Raw<0x138>) && kSlot318ObjG1 - kSlot318ObjF == sizeof(Slot318Raw<0x68>));
    CHECK(kSlot318ObjG2 - kSlot318ObjG1 == sizeof(Slot318Raw<0x38>) && kSlot318ObjG3 - kSlot318ObjG2 == sizeof(Slot318Raw<0x60>));
    CHECK(kSlot318ObjG4 - kSlot318ObjG3 == sizeof(Slot318Raw<0x40>));
    CHECK(kSlot318OneAndTenth == 0x3f8ccccd && kSlot318One == 0x3f800000 && kSlot318Vec4Tail == 0x40000000);
}

void section_build() {
    ConCmdRegistry creg;
    Slot318Objects o;
    poison_all(o);
    o.counters.count = 7;
    o.counters.bytes = 0x100;
    const Slot318Result r = init_slot318_a20e80(o, creg);
    CHECK(r.ok && r.blocked == Slot318Stage::none);

    const std::uint64_t base_a = 0x15d5780 + 0x10, base_b = 0x15d57b0 + 0x10;
    (void)base_a;
    (void)base_b;
    // A: base 0xb0c2a0 then class stores
    expect_image(o.a, {{0, 8, 0x15d5380 + 0x10}, {8, 4, 0}, {0x10, 16, 0}, {0x20, 8, 0}, {0x40, 8, 0},
                       {0x48, 4, 0}, {0x50, 8, 0}});
    expect_image(o.b, {{0, 8, 0x15d5410 + 0x10}, {8, 4, 0}, {0x10, 16, 0}, {0x20, 8, 0}, {0x40, 8, 0},
                       {0x48, 16, 0}});
    expect_image(o.c, {{0, 8, 0x15d5320 + 0x10}, {8, 4, 0}, {0x10, 16, 0}, {0x20, 8, 0}, {0x40, 8, 0},
                       {0x144, 1, 1}});
    // D: base 0xb0c260 (no +0x20/+0x40 stores)
    expect_image(o.d, {{0, 8, 0x15d53e0 + 0x10}, {8, 4, 0}, {0x10, 16, 0}, {0x70, 16, 0}, {0x80, 4, 0},
                       {0x9c, 4, 0x7f7fffff}, {0xa0, 2, 0x100}});
    // E: base 0xb0c260 + embedded vec4 at +0x20 (zero 0x20..0x30, 2.0f at 0x30)
    expect_image(o.e, {{0, 8, 0x15d5470 + 0x10}, {8, 4, 0}, {0x10, 16, 0}, {0x20, 16, 0}, {0x30, 4, 0x40000000},
                       {0x44, 16, 0}, {0x38, 16, 0}, {0x130, 2, 0x101}});
    // The later stores overwrite the 2.0f tail of the embedded vec4 at +0x30 (zero 0x38.. starts after it)
    CHECK(o.e.bytes[0x30] == 0x00 && o.e.bytes[0x33] == 0x40);
    expect_image(o.f, {{0, 8, 0x15d52c0 + 0x10}, {8, 4, 0}, {0x10, 16, 0}, {0x20, 8, 0}, {0x40, 8, 0}});
    // G1..G4: vec4 then post stores
    expect_image(o.g1, {{0, 16, 0}, {0x10, 4, 0x40000000}, {0x18, 16, 0}, {0x28, 8, 0}, {0x30, 4, 0x3f8ccccd}});
    // G2 stores overlap: 0x48 xmm zero is later overwritten by 1.0f at 0x48 and 0x58 is outside it
    expect_image(o.g2, {{0, 16, 0}, {0x10, 4, 0x40000000}, {0x18, 8, 0}, {0x20, 16, 0}, {0x30, 8, 0},
                        {0x38, 4, 0x3f800000}, {0x3c, 12, 0}, {0x48, 4, 0x3f800000}, {0x4c, 4, 0}, {0x50, 8, 0},
                        {0x58, 4, 0x3f800000}, {0x5c, 4, 0x3f8ccccd}});
    expect_image(o.g3, {{0, 16, 0}, {0x10, 4, 0x40000000}, {0x18, 32, 0}, {0x38, 4, 0}, {0x3c, 4, 0x3f8ccccd}});
    expect_image(o.g4, {{0, 16, 0}, {0x10, 4, 0x40000000}});

    // counters: five 0xb06980 calls (E's sub-object and G1..G4)
    CHECK(o.counters.count == 7 + 5 && o.counters.bytes == 0x100 + 5 * 0x18);

    // atexit order
    const std::pair<std::uint64_t, const void*> want[] = {
        {0xaeb9d0, &o.a}, {0xaf3b20, &o.b}, {0xae09f0, &o.c}, {0xaee360, &o.d}, {0xaf6dc0, &o.e},
        {0xace8c0, &o.f}, {0xa20460, &o.g1}, {0xa20470, &o.g2}, {0xa20480, &o.g3}, {0xa20490, &o.g4}};
    CHECK(creg.atexit_log.size() == 10);
    for (std::size_t i = 0; i < 10; ++i) {
        CHECK(creg.atexit_log.at(i).dtor_rva == want[i].first && creg.atexit_log.at(i).object == want[i].second);
    }
    // the slot touches no ConCmd list
    CHECK(creg.pending.size() == 0);
}

void section_idempotent_inputs() {
    // Re-running on zeroed objects gives the same images (all stores are absolute, nothing reads old bytes).
    ConCmdRegistry c1, c2;
    Slot318Objects a, b;
    poison_all(b);
    CHECK(init_slot318_a20e80(a, c1).ok && init_slot318_a20e80(b, c2).ok);
    CHECK(std::memcmp(a.c.bytes.data(), b.c.bytes.data(), 12) == 0);   // vptr and dword at +8 match
    CHECK(a.c.bytes[0xc] == 0 && b.c.bytes[0xc] == kPoison);           // bytes the constructor never writes
    CHECK(a.c.bytes[0x144] == 1 && b.c.bytes[0x144] == 1);
    CHECK(a.counters.count == 5 && a.counters.bytes == 5 * 0x18);
    CHECK(std::strcmp(slot318_stage_name(Slot318Stage::atexit_g4), "atexit_g4") == 0);
    CHECK(std::strcmp(slot318_stage_name(Slot318Stage::atexit_a), "atexit_a") == 0);
}

}  // namespace

int main() {
    section_constants();
    section_build();
    section_idempotent_inputs();
    std::puts("OK test_ctor_slot_318");
    return 0;
}
