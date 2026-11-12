// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 3): fixture for startup constructor index 477 (0x3270d0): 4 bool console variable(s) (tag 0x15a77b0).
#include "l4_test_util.hpp"

#include "core/ctor_slot_477_3270d0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot477Rva == 0x3270d0 && kSlot477Size == 918 && kSlot477BoolFnTag == 0x15a77b0);
    CHECK(std::strlen(kSlot477Var0Name) == kSlot477Var0NameLength && kSlot477Var0NameLength == 0x16);
    CHECK(std::strlen(kSlot477Var0Description) == kSlot477Var0DescriptionLength && kSlot477Var0DescriptionLength == 0x3b);
    CHECK(std::strlen(kSlot477Var1Name) == kSlot477Var1NameLength && kSlot477Var1NameLength == 0x15);
    CHECK(std::strlen(kSlot477Var1Description) == kSlot477Var1DescriptionLength && kSlot477Var1DescriptionLength == 0x55);
    CHECK(std::strlen(kSlot477Var2Name) == kSlot477Var2NameLength && kSlot477Var2NameLength == 0x13);
    CHECK(std::strlen(kSlot477Var2Description) == kSlot477Var2DescriptionLength && kSlot477Var2DescriptionLength == 0x3f);
    CHECK(std::strlen(kSlot477Var3Name) == kSlot477Var3NameLength && kSlot477Var3NameLength == 0x15);
    CHECK(std::strlen(kSlot477Var3Description) == kSlot477Var3DescriptionLength && kSlot477Var3DescriptionLength == 0x1c);
    CHECK(sizeof(Slot477Objects) >= 4 * sizeof(ConCmdBoolVar) && sizeof(ConCmdBoolVar) == 0xc8);
}

void section_alone() {
    ConCmdRegistry creg;

    Slot477Objects o;
    poison(o);
    const Slot477Result r = init_slot477_3270d0(o, creg);
    CHECK(r.ok && r.var0.ok && r.var0.views_inert && r.var1.ok && r.var1.views_inert && r.var2.ok && r.var2.views_inert && r.var3.ok && r.var3.views_inert);
    CHECK(r.var0.registration.ok && r.var0.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var1.registration.ok && r.var1.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var2.registration.ok && r.var2.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var3.registration.ok && r.var3.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_with_description(o.var0, 0x15a77b0, 0, kSlot477Var0Description, 59);
    CHECK(same_view(o.var0.base.name, kSlot477Var0Name, 0x16));
    check_bool_with_description(o.var1, 0x15a77b0, 0, kSlot477Var1Description, 85);
    CHECK(same_view(o.var1.base.name, kSlot477Var1Name, 0x15));
    check_bool_with_description(o.var2, 0x15a77b0, 0, kSlot477Var2Description, 63);
    CHECK(same_view(o.var2.base.name, kSlot477Var2Name, 0x13));
    check_bool_with_description(o.var3, 0x15a77b0, 0, kSlot477Var3Description, 28);
    CHECK(same_view(o.var3.base.name, kSlot477Var3Name, 0x15));
    CHECK(creg.pending.size() == 4);
    CHECK(creg.atexit_log.size() == 4);
    check_atexit(creg, 0, 0x20b0, &o.var0);
    check_atexit(creg, 1, 0x20b0, &o.var1);
    check_atexit(creg, 2, 0x20b0, &o.var2);
    check_atexit(creg, 3, 0x20b0, &o.var3);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot477Objects o;
    const Slot477Result r = init_slot477_3270d0(o, creg);
    CHECK(!r.ok && !r.var0.ok && !r.var0.registration.ok && !r.var1.ok && !r.var1.registration.ok && !r.var2.ok && !r.var2.registration.ok && !r.var3.ok && !r.var3.registration.ok && creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_477");
    return 0;
}
