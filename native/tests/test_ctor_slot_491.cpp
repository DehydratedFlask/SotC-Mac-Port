// CPU-01-STARTUP-SLOT-491: fixture for startup constructor index 491 (0x295f90): nine zeroed handle cells with
// cleanup registrations, an identity cell and the "<TEXT NOT FOUND>" caption. Lane M5.
#include "l4_test_util.hpp"

#include "core/ctor_slot_491_295f90.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot491Rva == 0x295f90 && kSlot491Size == 372 && kSlot491HandleCount == 9);
    CHECK(kSlot491FirstHandleRva == 0x2eef540 && kSlot491IdentityCellRva == 0x2eef588 && kSlot491CaptionRva == 0x2eef5a0);
    CHECK(kSlot491IdentityVptr == 0x159e0e8 + 0x10 && kSlot491HandleDtorRva == 0xaaf050 && kSlot491CaptionDtorRva == 0xf7b0);
    CHECK(sizeof kSlot491CaptionText - 1 == 16 && std::strcmp(kSlot491CaptionText, "<TEXT NOT FOUND>") == 0);
}

void section_alone() {
    StartupAtexitLog log;
    Slot491Objects o;
    poison(o);
    CHECK(init_slot491_295f90(o, log));
    for (std::size_t k = 0; k < 9; ++k) CHECK(o.handles[k] == nullptr);
    CHECK(o.identity_vptr == 0x159e0f8);
    CHECK(same_view(o.caption, "<TEXT NOT FOUND>", 16));
    check_untouched(o, {{0x00, 0x48}, {0x48, 8}, {0x60, 0x20}});
    CHECK(log.size() == 10);
    for (std::size_t k = 0; k < 9; ++k) {
        CHECK(log.at(k).dtor_rva == 0xaaf050 && log.at(k).object == &o.handles[k]);
    }
    CHECK(log.at(9).dtor_rva == 0xf7b0 && log.at(9).object == &o.caption);
}

void section_idempotent_state() {
    // Re-running on already-nulled cells leaves the same bytes (the stores are unconditional).
    StartupAtexitLog log;
    Slot491Objects o;
    poison(o);
    CHECK(init_slot491_295f90(o, log));
    Slot491Objects copy = o;
    CHECK(init_slot491_295f90(o, log));
    CHECK(std::memcmp(&copy, &o, sizeof o) == 0 && log.size() == 20);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_idempotent_state();
    std::puts("OK test_ctor_slot_491");
    return 0;
}
