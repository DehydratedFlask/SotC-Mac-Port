// CPU-01-STARTUP-SLOT-359: fixture for startup constructor index 359 (0x785f60). Lane M1.
#include "l4_test_util.hpp"

#include "core/ctor_slot_359_785f60.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot359Rva == 0x785f60 && kSlot359Size == 296 && kSlot359HandlerTag == 0x7836d0 && kSlot359ObjectRva == 0x3255c30);
    CHECK(std::strlen(kSlot359CommandName) == 8 && std::strcmp(kSlot359CommandName, "giveItem") == 0);
    CHECK(std::strlen(kSlot359CommandDescription) == 73 && kSlot359CommandDescriptionLength == 0x49);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot359Objects o;
    poison(o);
    const Slot359Result r = init_slot359_785f60(o, creg);
    CHECK(r.ok && r.command.ok && r.command.views_inert && r.command.atexit_recorded && !r.command.handler_invoked);
    check_command(o.command, "giveItem", "Give the player the indicated item. Params: <item_index> <from_hard_list>", 0x49, 0x7836d0);
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.command.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x4b0, &o.command);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot359Objects o;
    const Slot359Result r = init_slot359_785f60(o, creg);
    CHECK(!r.ok && !r.command.ok && !r.command.registration.ok && creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_359");
    return 0;
}
