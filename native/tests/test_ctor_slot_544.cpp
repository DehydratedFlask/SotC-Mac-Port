// CPU-01-STARTUP-SLOT-544: fixture for startup constructor index 544 (0xd1b20): memset of a 0xae40-byte block and
// the 13-entry shadow-pass name table. Lane M6.
#include "l4_test_util.hpp"

#include "core/ctor_slot_544_d1b20.hpp"

namespace {
using namespace l4;

struct Expect {
    std::size_t index;
    std::uint32_t id;
    const char* name;
};

constexpr Expect kExpected[] = {
    {0, 7, "SpotShadow0"},      {1, 8, "SpotShadow1"},      {2, 9, "SpotShadow2"},
    {3, 10, "SpotShadow3"},     {7, 0xc, "ShadowCascade0"}, {8, 0xd, "ShadowCascade1"},
    {9, 0xe, "ShadowCascade2"}, {10, 0xf, "ShadowCascade3"}, {11, 0x10, "ShadowCascade4(SuperClose)"},
    {12, 0, "EntireFrame"},
};

void section_constants() {
    CHECK(kSlot544Rva == 0xd1b20 && kSlot544Size == 273 && kSlot544BlockSize == 0xae40);
    CHECK(kSlot544TableRva == 0x2eded90 && kSlot544TerminatorRva == kSlot544TableRva + 0xd0 && kSlot544TableEntries == 13);
    CHECK(sizeof(Slot544Entry) == 0x10 && offsetof(Slot544Entry, name) == 8);
    CHECK(kSlot544EntryWriterRva == 0xc1c70);
}

void section_alone() {
    auto* o = new Slot544Objects;
    poison(*o);
    const Slot544Result r = init_slot544_d1b20(*o);
    CHECK(r.ok);
    for (std::uint8_t byte : o->block) CHECK(byte == 0);   // the whole 0xae40 block is zeroed
    for (const Expect& x : kExpected) {
        const Slot544Entry& e = o->entries[x.index];
        CHECK(e.id == x.id);
        CHECK(e.name != nullptr && std::strcmp(e.name, x.name) == 0);
        for (std::uint8_t byte : e.gap04) CHECK(byte == kPoison);   // the dword at +4 of every record is never written
    }
    // Records 4, 5 and 6 are not written by this constructor at all.
    for (std::size_t i = 4; i <= 6; ++i) {
        const auto* bytes = reinterpret_cast<const std::uint8_t*>(&o->entries[i]);
        for (std::size_t b = 0; b < sizeof(Slot544Entry); ++b) CHECK(bytes[b] == kPoison);
    }
    CHECK(o->terminator == 0);
    delete o;
}

void section_idempotent_and_distinct_names() {
    auto* a = new Slot544Objects;
    auto* b = new Slot544Objects;
    poison(*a);
    poison(*b);
    CHECK(init_slot544_d1b20(*a).ok && init_slot544_d1b20(*b).ok);
    CHECK(std::memcmp(a, b, sizeof(Slot544Objects)) == 0);   // deterministic: name pointers are the same literals
    CHECK(init_slot544_d1b20(*a).ok && std::memcmp(a, b, sizeof(Slot544Objects)) == 0);
    for (std::size_t i = 0; i < 10; ++i)
        for (std::size_t j = i + 1; j < 10; ++j)
            CHECK(std::strcmp(kExpected[i].name, kExpected[j].name) != 0);   // the ten written names are all distinct
    delete a;
    delete b;
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_idempotent_and_distinct_names();
    std::puts("OK test_ctor_slot_544");
    return 0;
}
