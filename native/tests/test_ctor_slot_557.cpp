// CPU-01-STARTUP-SLOT-557: fixture for startup constructor index 557 (0x47490): the console command `mem`
// (description `Dump memory statistics`, handler tag 0x465a0, flags 2, atexit 0x4b0). Lane M6.
#include "l4_test_util.hpp"

#include "core/ctor_slot_557_47490.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot557Rva == 0x47490 && kSlot557Size == 296 && kSlot557HandlerTag == 0x465a0);
    CHECK(std::strlen(kSlot557CommandName) == 3 && std::strlen(kSlot557CommandDescription) == 0x16);
    CHECK(kSlot557CommandDescriptionLength == 0x16);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot557Objects o;
    poison(o);
    const Slot557Result r = init_slot557_47490(o, creg);
    CHECK(r.ok && r.command.ok && r.command.views_inert && r.command.vtable_register_slot_ok && !r.command.handler_invoked);
    check_command(o.command, "mem", "Dump memory statistics", 0x16, 0x465a0);
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.command.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x4b0, &o.command);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;   // manager without an insert contract: the registration fails visibly
    Slot557Objects o;
    const Slot557Result r = init_slot557_47490(o, creg);
    CHECK(!r.ok && !r.command.ok && !r.command.registration.ok && creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_557");
    return 0;
}
