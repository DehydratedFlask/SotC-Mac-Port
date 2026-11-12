// CPU-01-AOT-CFG-SCALE-01-BATCH18: fixture for the generic store-table replay (core/store_table). It checks the lane kinds, ordering, the untouched bytes and the
// fail-closed validation with a hand-written table; the slot-424 table itself is tested (and ELF-tied) by test_ctor_slot_424 / check_ctor_slot_424.py.
#include "core/store_table.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

constexpr char kBlob[] = "alpha\0beta";   // + the implicit terminator
constexpr std::uint32_t kOffsets[] = {0, 6};

StoreTableRecord rec(std::uint32_t off, std::uint8_t w, std::uint8_t k0, std::uint64_t v0, std::uint8_t k1 = 0, std::uint64_t v1 = 0) {
    return StoreTableRecord{0x1000 + off, off, w, k0, k1, 0, v0, v1};
}

StoreTableSpec spec_of(const StoreTableRecord* r, std::size_t n) {
    return StoreTableSpec{r, n, kBlob, sizeof kBlob, kOffsets, 2};
}

constexpr std::uint8_t C = static_cast<std::uint8_t>(StoreLaneKind::constant);
constexpr std::uint8_t S = static_cast<std::uint8_t>(StoreLaneKind::self);
constexpr std::uint8_t T = static_cast<std::uint8_t>(StoreLaneKind::string);
constexpr std::uint8_t G = static_cast<std::uint8_t>(StoreLaneKind::tag);

void test_lanes_and_order() {
    alignas(8) std::uint8_t img[0x40];
    std::memset(img, 0xA5, sizeof img);
    const StoreTableRecord t[] = {
        rec(0x00, 16, T, 1, T, 0),         // two string lanes
        rec(0x10, 4, C, 0x11223344),
        rec(0x18, 8, S, 0x20),             // self pointer
        rec(0x20, 8, G, 0x54bcf0),         // identity tag: the integer, never followed
        rec(0x28, 4, C, 7),
        rec(0x28, 4, C, 9),                // later record overwrites
        rec(0x30, 16, C, 0, G, 0x15ca230),
    };
    CHECK(replay_store_table(img, sizeof img, spec_of(t, sizeof t / sizeof t[0])));
    const char* p0 = nullptr;
    const char* p1 = nullptr;
    std::memcpy(&p0, img + 0, 8);
    std::memcpy(&p1, img + 8, 8);
    CHECK(p0 == kBlob + 6 && std::strcmp(p0, "beta") == 0 && p1 == kBlob && std::strcmp(p1, "alpha") == 0);
    std::uint32_t d = 0;
    std::memcpy(&d, img + 0x10, 4);
    CHECK(d == 0x11223344);
    CHECK(img[0x14] == 0xA5 && img[0x17] == 0xA5);   // never written
    std::uint64_t q = 0;
    std::memcpy(&q, img + 0x18, 8);
    CHECK(q == reinterpret_cast<std::uintptr_t>(img + 0x20));
    std::memcpy(&q, img + 0x20, 8);
    CHECK(q == 0x54bcf0);
    std::memcpy(&d, img + 0x28, 4);
    CHECK(d == 9 && img[0x2c] == 0xA5);
    std::memcpy(&q, img + 0x30, 8);
    CHECK(q == 0);
    std::memcpy(&q, img + 0x38, 8);
    CHECK(q == 0x15ca230);
}

bool refused(const StoreTableRecord& bad) {
    std::uint8_t img[0x40];
    std::memset(img, 0xA5, sizeof img);
    const StoreTableRecord t[] = {rec(0x00, 8, C, 1), bad};   // the first record is valid: it must NOT be applied either
    const bool ok = replay_store_table(img, sizeof img, spec_of(t, 2));
    for (std::uint8_t b : img) CHECK(b == 0xA5);   // validation happens before the first write
    return !ok;
}

void test_fail_closed() {
    CHECK(refused(rec(0x3d, 4, C, 1)));                          // runs past the end of the image
    CHECK(refused(rec(0x41, 4, C, 1)));                          // starts outside
    CHECK(refused(rec(0x00, 2, C, 1)));                          // width not 4 / 8 / 16
    CHECK(refused(rec(0x00, 4, C, 0x100000000ull)));             // does not fit 32 bits
    CHECK(refused(rec(0x00, 4, T, 0)));                          // a pointer lane in a dword store
    CHECK(refused(rec(0x00, 8, T, 2)));                          // string index out of range
    CHECK(refused(rec(0x00, 8, S, 0x39)));                       // self pointer outside the image
    CHECK(refused(rec(0x00, 8, 9, 0)));                          // unknown lane kind
    CHECK(refused(rec(0x00, 8, C, 0, C, 1)));                    // a second lane on an 8 byte store
    CHECK(refused(rec(0x00, 16, C, 0, 9, 0)));                   // unknown second lane kind
    StoreTableRecord reserved = rec(0x00, 8, C, 0);
    reserved.reserved = 1;
    CHECK(refused(reserved));
    std::uint8_t img[8] = {};
    CHECK(!replay_store_table(img, sizeof img, StoreTableSpec{}));                 // empty
    CHECK(!replay_store_table(nullptr, 8, spec_of(nullptr, 0)));
    const StoreTableRecord one[] = {rec(0, 8, T, 0)};
    const char unterminated[] = {'a', 'b'};
    CHECK(!replay_store_table(img, sizeof img, StoreTableSpec{one, 1, unterminated, 2, kOffsets, 1}));   // blob does not end in NUL
}

}  // namespace

int main() {
    test_lanes_and_order();
    test_fail_closed();
    std::puts("OK test_store_table");
    return 0;
}
