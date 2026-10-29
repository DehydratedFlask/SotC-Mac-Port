// CPU-01-STARTUP-SLOT-386: fixture for startup constructor index 386 (0x6b12f0). Lane M1.
#include "l4_test_util.hpp"

#include "core/ctor_slot_386_6b12f0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot386Rva == 0x6b12f0 && kSlot386Size == 296 && kSlot386HandlerTag == 0x6aee10 && kSlot386ObjectRva == 0x3017de0);
    CHECK(std::strlen(kSlot386CommandName) == 36 && std::strcmp(kSlot386CommandName, "shadow_printCurrentlyLoadedSheetData") == 0);
    CHECK(std::strlen(kSlot386CommandDescription) == 0 && kSlot386CommandDescriptionLength == 0x0);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot386Objects o;
    poison(o);
    const Slot386Result r = init_slot386_6b12f0(o, creg);
    CHECK(r.ok && r.command.ok && r.command.views_inert && r.command.atexit_recorded && !r.command.handler_invoked);
    check_command(o.command, "shadow_printCurrentlyLoadedSheetData", "", 0x0, 0x6aee10);
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.command.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x4b0, &o.command);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot386Objects o;
    const Slot386Result r = init_slot386_6b12f0(o, creg);
    CHECK(!r.ok && !r.command.ok && !r.command.registration.ok && creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_386");
    return 0;
}
