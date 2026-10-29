// CPU-01-STARTUP-SLOT-540: fixture for startup constructor index 540 (0x10e390): ten particle-group records, a
// rand()-seeded cell, the `showParticle_Errors` name object and a list sentinel. Lane M5.
#include "l4_test_util.hpp"

#include <cstdlib>

#include "core/ctor_slot_540_10e390.hpp"

namespace {
using namespace l4;

int fixed_rand() noexcept { return 0x12345678; }
int high_rand() noexcept { return 0x7fffffff; }

void section_constants() {
    CHECK(kSlot540Rva == 0x10e390 && kSlot540Size == 488 && kSlot540EntryCount == 10 && kSlot540EntryId == 0x24);
    CHECK(kSlot540TableRva == 0x2edef70 && kSlot540RandCellRva == 0x2edf010 && kSlot540NameObjectRva == 0x2edf018 &&
          kSlot540SentinelRva == 0x2edf048);
    CHECK(kSlot540NameVptr == 0x15976a0 + 0x10 && kSlot540NameDtorRva == 0x8ef00 && kSlot540SentinelDtorRva == 0x101ea0);
    CHECK(kSlot540EntryWriterRva == 0xc1c70 && kSlot540StoreHelperRva == 0x158340);
    const struct { const char* text; std::size_t len; } names[] = {
        {kSlot540Name0, 20}, {kSlot540Name1, 25}, {kSlot540Name2, 24}, {kSlot540Name3, 24}, {kSlot540Name4, 17},
        {kSlot540Name5, 27}, {kSlot540Name6, 33}, {kSlot540Name7, 28}, {kSlot540Name8, 26}, {kSlot540Name9, 27}};
    for (const auto& n : names) CHECK(std::strlen(n.text) == n.len && std::strncmp(n.text, "ParticleGroup_", 14) == 0);
    CHECK(sizeof kSlot540ErrorsName - 1 == 19 && std::strcmp(kSlot540ErrorsName, "showParticle_Errors") == 0);
}

void section_alone() {
    StartupAtexitLog log;
    Slot540Objects o;
    poison(o);
    const Slot540Result r = init_slot540_10e390(o, log, &fixed_rand);
    CHECK(r.ok && r.blocked == Slot540Stage::none && r.rand_value == 0x12345678u);
    const char* const names[10] = {kSlot540Name0, kSlot540Name1, kSlot540Name2, kSlot540Name3, kSlot540Name4,
                                   kSlot540Name5, kSlot540Name6, kSlot540Name7, kSlot540Name8, kSlot540Name9};
    for (std::size_t k = 0; k < 10; ++k) {
        CHECK(o.entries[k].id == 0x24 && o.entries[k].name == names[k]);
        for (std::uint8_t b : o.entries[k].gap04) CHECK(b == kPoison);       // +4..+7 never written
    }
    CHECK(dword(o.rand_cell, 0) == 0x12345678u && o.rand_cell[4] == 0);
    for (std::size_t i = 5; i < 8; ++i) CHECK(o.rand_cell[i] == kPoison);
    CHECK(o.name_object.vptr == 0x15976b0 && o.name_object.tail == 0);
    CHECK(same_view(o.name_object.name, "showParticle_Errors", 19));
    CHECK(o.sentinel.next == &o.sentinel && o.sentinel.prev == &o.sentinel && o.sentinel.count == 0);
    CHECK(log.size() == 2);
    CHECK(log.at(0).dtor_rva == 0x8ef00 && log.at(0).object == &o.name_object);
    CHECK(log.at(1).dtor_rva == 0x101ea0 && log.at(1).object == &o.sentinel);
}

void section_rand_sources() {
    StartupAtexitLog log;
    Slot540Objects o;
    poison(o);
    CHECK(init_slot540_10e390(o, log, &high_rand).rand_value == 0x7fffffffu);
    // the default source is the host rand(): any value in [0, RAND_MAX] is stored verbatim
    StartupAtexitLog log2;
    Slot540Objects p;
    poison(p);
    const Slot540Result r = init_slot540_10e390(p, log2);
    CHECK(r.ok && r.rand_value <= static_cast<std::uint32_t>(RAND_MAX) && dword(p.rand_cell, 0) == r.rand_value);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_rand_sources();
    std::puts("OK test_ctor_slot_540");
    return 0;
}
