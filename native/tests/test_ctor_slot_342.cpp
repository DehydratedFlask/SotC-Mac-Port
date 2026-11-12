// CPU-01-STARTUP-BATCH-10: fixture for startup constructor index 342 (0x7d8e10): bool variable `enableCutsceneDebugCommands`, two unbounded float
// variables, two NULL-source 0x23fd00 name cells and the console command `cp10_skipCurCutsceneCut`. The survey's "indirect call" is the console-command
// register-once slot [0x1594dc0 + 0x20] = 0x20d60 (checker-pinned) and 0x23fd00 is core/name_cell_23fd00.
#include "l4_test_util.hpp"

#include "core/ctor_slot_342_7d8e10.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot342Rva == 0x7d8e10 && kSlot342Size == 1070);
    CHECK(is_float_functor_tag(kSlot342FloatFnTag) && !is_int_functor_tag(kSlot342FloatFnTag));
    CHECK(std::strlen(kSlot342BoolName) == kSlot342BoolNameLength && kSlot342BoolNameLength == 0x1b);
    for (std::size_t n = 0; n < kSlot342FloatCount; ++n) CHECK(std::strlen(kSlot342FloatNames[n]) == kSlot342FloatNameLengths[n]);
    CHECK(std::strlen(kSlot342CommandDescription) == kSlot342CommandDescriptionLength && kSlot342CommandDescriptionLength == 0x1f);
    CHECK(sizeof(ConCmdBoolVar) == 0xc8 && sizeof(ConCmdRangedVar) == 0xd8 && sizeof(ConCmdCommand) == 0xa0);
    CHECK(kSlot342Float0Rva - kSlot342BoolRva == offsetof(Slot342Objects, fvar));
    CHECK(kSlot342Float1Rva - kSlot342BoolRva == offsetof(Slot342Objects, fvar) + sizeof(ConCmdRangedVar));
    CHECK(kSlot342Cell0Rva - kSlot342BoolRva == offsetof(Slot342Objects, cells));
    CHECK(kSlot342Cell1Rva - kSlot342BoolRva == offsetof(Slot342Objects, cells) + sizeof(HashedName20));
    CHECK(kSlot342CommandRva - kSlot342BoolRva == offsetof(Slot342Objects, command));
}

void section_alone() {
    ConCmdRegistry creg;
    Slot342Objects o;
    poison(o);
    const Slot342Result r = init_slot342_7d8e10(o, creg);
    CHECK(r.ok && r.rows_done == 6 && r.refusal == HelperRefusal::none);
    check_bool_common(o.bvar, kSlot342BoolFnTag, kSlot342BoolValue);
    CHECK(same_view(o.bvar.base.name, kSlot342BoolName, kSlot342BoolNameLength));
    for (std::size_t n = 0; n < kSlot342FloatCount; ++n) {
        CHECK(r.fvar[n].ok && r.fvar[n].ctor_ok && r.fvar[n].functor_destroy_ok && r.fvar[n].views_inert && r.fvar[n].atexit_recorded && r.fvar[n].registration.ok);
        check_float_common(o.fvar[n], false, 0, 0, kSlot342FloatValueBits);
        CHECK(same_view(o.fvar[n].base.name, kSlot342FloatNames[n], kSlot342FloatNameLengths[n]));
    }
    for (std::size_t n = 0; n < kSlot342NameCellCount; ++n) {
        CHECK(o.cells[n].hash == 0 && std::memcmp(o.cells[n].text, "NULL", 5) == 0);
        for (std::size_t i = 4; i < sizeof o.cells[n].text; ++i) CHECK(o.cells[n].text[i] == 0);
    }
    for (std::uint8_t byte : o.pad_cmd) CHECK(byte == kPoison);   // the 0x28 bytes before the command are never written
    CHECK(r.command.ok && r.command.vtable_register_slot_ok && r.command.views_inert && r.command.atexit_recorded && !r.command.handler_invoked);
    check_command(o.command, kSlot342CommandName, kSlot342CommandDescription, kSlot342CommandDescriptionLength, kSlot342HandlerTag);
    // registration order b0, f0, f1, cmd
    CHECK(creg.pending.size() == 4 && creg.pending.first() == &o.bvar.base.pending);
    CHECK(o.bvar.base.pending.next == &o.fvar[0].base.pending && o.fvar[0].base.pending.next == &o.fvar[1].base.pending);
    CHECK(o.fvar[1].base.pending.next == &o.command.base.pending && o.command.base.pending.next == &creg.pending.head);
    // atexit: bool 0x20b0, floats 0x20f0 x2, command 0x4b0; the name cells record nothing
    CHECK(creg.atexit_log.size() == 4);
    check_atexit(creg, 0, 0x20b0, &o.bvar);
    check_atexit(creg, 1, 0x20f0, &o.fvar[0]);
    check_atexit(creg, 2, 0x20f0, &o.fvar[1]);
    check_atexit(creg, 3, 0x4b0, &o.command);
}

void section_failures() {
    {   // manager without an insert contract: the bool registration fails and nothing after it is built
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot342Objects o;
        poison(o);
        const Slot342Result r = init_slot342_7d8e10(o, creg);
        CHECK(!r.ok && r.rows_done == 0 && creg.pending.size() == 0);
        CHECK(o.cells[0].hash == 0xa5a5a5a5u);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_342");
    return 0;
}
