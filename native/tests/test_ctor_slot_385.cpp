// CPU-01-STARTUP-SLOT-385: fixture for startup constructor index 385 (0x6b4cf0). Lane M1.
#include "l4_test_util.hpp"

#include "core/ctor_slot_385_6b4cf0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot385Rva == 0x6b4cf0 && kSlot385Size == 296 && kSlot385HandlerTag == 0x6b4600 && kSlot385ObjectRva == 0x3238340);
    CHECK(std::strlen(kSlot385CommandName) == 29 && std::strcmp(kSlot385CommandName, "gCP10ExtractCP3SERandomTables") == 0);
    CHECK(std::strlen(kSlot385CommandDescription) == 37 && kSlot385CommandDescriptionLength == 0x25);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot385Objects o;
    poison(o);
    const Slot385Result r = init_slot385_6b4cf0(o, creg);
    CHECK(r.ok && r.command.ok && r.command.views_inert && r.command.atexit_recorded && !r.command.handler_invoked);
    check_command(o.command, "gCP10ExtractCP3SERandomTables", "Dumps se-random-def tables to output.", 0x25, 0x6b4600);
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.command.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x4b0, &o.command);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot385Objects o;
    const Slot385Result r = init_slot385_6b4cf0(o, creg);
    CHECK(!r.ok && !r.command.ok && !r.command.registration.ok && creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_385");
    return 0;
}
