// CPU-01-STARTUP-SLOT-339: fixture for startup constructor index 339 (0x7e2fc0): seven {name, fnv1a(name)} cells, two 0x30 byte name holders, the
// unbounded float variable `shadowDemoMode_gameplayTimer` and the console command `killBoss`. Batch 9 (the startup-table cliff of batch 8).
#include "l4_test_util.hpp"

#include "core/ctor_slot_339_7e2fc0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot339Rva == 0x7e2fc0 && kSlot339Size == 1004);
    CHECK(kSlot339FloatFnTag == 0x15ce440 && is_float_functor_tag(kSlot339FloatFnTag) && !is_int_functor_tag(kSlot339FloatFnTag));
    CHECK(kSlot339HandlerTag == 0x7def00 && kSlot339HolderDtorRva == 0x8ef00 && kSlot339HolderVptr == 0x15976b0);
    CHECK(std::strlen(kSlot339Holder0Name) == kSlot339Holder0NameLength && kSlot339Holder0NameLength == 0xe);
    CHECK(std::strlen(kSlot339Holder1Name) == kSlot339Holder1NameLength && kSlot339Holder1NameLength == 0x1c);
    CHECK(std::strlen(kSlot339FloatName) == kSlot339FloatNameLength && kSlot339FloatNameLength == 0x1c);
    CHECK(std::strlen(kSlot339CommandDescription) == kSlot339CommandDescriptionLength && kSlot339CommandDescriptionLength == 0x2f);
    CHECK(kSlot339Holder0Rva - kSlot339Cell0Rva == offsetof(Slot339Objects, holder0) && kSlot339Holder1Rva - kSlot339Cell0Rva == offsetof(Slot339Objects, holder1));
    CHECK(kSlot339FloatRva - kSlot339Cell0Rva == offsetof(Slot339Objects, fvar) && kSlot339CommandRva - kSlot339Cell0Rva == offsetof(Slot339Objects, command));
}

void section_alone() {
    ConCmdRegistry creg;
    Slot339Objects o;
    poison(o);
    const Slot339Result r = init_slot339_7e2fc0(o, creg);
    CHECK(r.ok && r.cells_ok && r.holders_ok && r.refusal == HelperRefusal::none);
    for (std::size_t n = 0; n < kSlot339CellCount; ++n) {
        CHECK(o.cells[n].name == kSlot339CellNames[n] && o.cells[n].hash == fnv_cstr_52a990(kSlot339CellNames[n]));
        CHECK(o.cells[n].unwritten == 0xa5a5a5a5u);   // +0x0c is never written
    }
    CHECK(o.cells[0].hash == fnv_cstr_52a990("CurrentPlayTimeEASY") && o.cells[0].hash != o.cells[1].hash);
    CHECK(o.holder0.vptr == kSlot339HolderVptr && o.holder0.tail == 0 && same_view(o.holder0.name, kSlot339Holder0Name, kSlot339Holder0NameLength));
    CHECK(o.holder1.vptr == kSlot339HolderVptr && o.holder1.tail == 0 && same_view(o.holder1.name, kSlot339Holder1Name, kSlot339Holder1NameLength));
    CHECK(r.fvar.ok && r.fvar.ctor_ok && r.fvar.functor_destroy_ok && r.fvar.views_inert && r.fvar.atexit_recorded && r.fvar.registration.ok);
    check_float_common(o.fvar, false, 0, 0, kSlot339FloatValueBits);
    CHECK(same_view(o.fvar.base.name, kSlot339FloatName, kSlot339FloatNameLength));
    CHECK(r.command.ok && r.command.views_inert && r.command.atexit_recorded && !r.command.handler_invoked);
    check_command(o.command, kSlot339CommandName, kSlot339CommandDescription, kSlot339CommandDescriptionLength, kSlot339HandlerTag);
    CHECK(creg.pending.size() == 2 && creg.pending.first() == &o.fvar.base.pending);
    CHECK(o.fvar.base.pending.next == &o.command.base.pending && o.command.base.pending.next == &creg.pending.head);
    CHECK(creg.atexit_log.size() == 4);
    check_atexit(creg, 0, 0x8ef00, &o.holder0);
    check_atexit(creg, 1, 0x8ef00, &o.holder1);
    check_atexit(creg, 2, 0x20f0, &o.fvar);
    check_atexit(creg, 3, 0x4b0, &o.command);
}

void section_failures() {
    {   // manager without an insert contract: the float registration fails (the original has no early exit) and the command is built regardless
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot339Objects o;
        const Slot339Result r = init_slot339_7e2fc0(o, creg);
        CHECK(!r.ok && creg.pending.size() == 0);
    }
    {   // a tag outside the float family is refused by the wrapper and nothing is registered
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        HelperFloatSiteSpec spec{kSlot339FloatName, kSlot339FloatNameLength, "", 0, 1, 0x1234567, 0, true};
        const HelperFloatSiteResult bad = helper_init_unbounded_float_var_site(v, spec, creg);
        CHECK(!bad.ok && !bad.ctor_ok && bad.refusal == HelperRefusal::unadmitted_arg_functor && creg.pending.size() == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_339");
    return 0;
}
