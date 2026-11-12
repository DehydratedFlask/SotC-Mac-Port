// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 3): fixture for startup constructor index 410 (0x5ac580): 3 bool console variable(s) (tag 0x15cb188).
#include "l4_test_util.hpp"

#include "core/ctor_slot_410_5ac580.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot410Rva == 0x5ac580 && kSlot410Size == 735 && kSlot410BoolFnTag == 0x15cb188);
    CHECK(std::strlen(kSlot410Var0Name) == kSlot410Var0NameLength && kSlot410Var0NameLength == 0x10);
    CHECK(std::strlen(kSlot410Var1Name) == kSlot410Var1NameLength && kSlot410Var1NameLength == 0x14);
    CHECK(std::strlen(kSlot410Var2Name) == kSlot410Var2NameLength && kSlot410Var2NameLength == 0x15);
    CHECK(sizeof(Slot410Objects) >= 3 * sizeof(ConCmdBoolVar) && sizeof(ConCmdBoolVar) == 0xc8);
}

void section_alone() {
    ConCmdRegistry creg;

    Slot410Objects o;
    poison(o);
    const Slot410Result r = init_slot410_5ac580(o, creg);
    CHECK(r.ok && r.var0.ok && r.var0.views_inert && r.var1.ok && r.var1.views_inert && r.var2.ok && r.var2.views_inert);
    CHECK(r.var0.registration.ok && r.var0.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var1.registration.ok && r.var1.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var2.registration.ok && r.var2.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_with_description(o.var0, 0x15cb188, 1, "", 0);
    CHECK(same_view(o.var0.base.name, kSlot410Var0Name, 0x10));
    check_bool_with_description(o.var1, 0x15cb188, 0, "", 0);
    CHECK(same_view(o.var1.base.name, kSlot410Var1Name, 0x14));
    check_bool_with_description(o.var2, 0x15cb188, 0, "", 0);
    CHECK(same_view(o.var2.base.name, kSlot410Var2Name, 0x15));
    CHECK(creg.pending.size() == 3);
    CHECK(creg.atexit_log.size() == 3);
    check_atexit(creg, 0, 0x20b0, &o.var0);
    check_atexit(creg, 1, 0x20b0, &o.var1);
    check_atexit(creg, 2, 0x20b0, &o.var2);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot410Objects o;
    const Slot410Result r = init_slot410_5ac580(o, creg);
    CHECK(!r.ok && !r.var0.ok && !r.var0.registration.ok && !r.var1.ok && !r.var1.registration.ok && !r.var2.ok && !r.var2.registration.ok && creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_410");
    return 0;
}
