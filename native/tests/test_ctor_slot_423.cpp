// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 3): fixture for startup constructor index 423 (0x55f080): 4 bool console variable(s) (tag 0x15ca5a8), 2 hashed-name record(s).
#include "l4_test_util.hpp"

#include "core/ctor_slot_423_55f080.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot423Rva == 0x55f080 && kSlot423Size == 1000 && kSlot423BoolFnTag == 0x15ca5a8);
    CHECK(std::strlen(kSlot423Var0Name) == kSlot423Var0NameLength && kSlot423Var0NameLength == 0x15);
    CHECK(std::strlen(kSlot423Var1Name) == kSlot423Var1NameLength && kSlot423Var1NameLength == 0x1f);
    CHECK(std::strlen(kSlot423Var2Name) == kSlot423Var2NameLength && kSlot423Var2NameLength == 0x24);
    CHECK(std::strlen(kSlot423Var3Name) == kSlot423Var3NameLength && kSlot423Var3NameLength == 0x1c);
    CHECK(std::strlen(kSlot423Record0Name) > 0 && fnv_cstr_52a990(kSlot423Record0Name) == kSlot423Record0Hash && kSlot423Record0Hash == 0x030d5c4f);
    CHECK(std::strlen(kSlot423Record1Name) > 0 && fnv_cstr_52a990(kSlot423Record1Name) == kSlot423Record1Hash && kSlot423Record1Hash == 0xf17781ad);
    CHECK(sizeof(Slot423Objects) >= 4 * sizeof(ConCmdBoolVar) && sizeof(ConCmdBoolVar) == 0xc8);
}

void section_alone() {
    ConCmdRegistry creg;

    Slot423Objects o;
    poison(o);
    const Slot423Result r = init_slot423_55f080(o, creg);
    CHECK(r.ok && r.var0.ok && r.var0.views_inert && r.var1.ok && r.var1.views_inert && r.var2.ok && r.var2.views_inert && r.var3.ok && r.var3.views_inert);
    CHECK(r.var0.registration.ok && r.var0.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var1.registration.ok && r.var1.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var2.registration.ok && r.var2.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var3.registration.ok && r.var3.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_with_description(o.var0, 0x15ca5a8, 0, "", 0);
    CHECK(same_view(o.var0.base.name, kSlot423Var0Name, 0x15));
    check_bool_with_description(o.var1, 0x15ca5a8, 0, "", 0);
    CHECK(same_view(o.var1.base.name, kSlot423Var1Name, 0x1f));
    check_bool_with_description(o.var2, 0x15ca5a8, 0, "", 0);
    CHECK(same_view(o.var2.base.name, kSlot423Var2Name, 0x24));
    check_bool_with_description(o.var3, 0x15ca5a8, 0, "", 0);
    CHECK(same_view(o.var3.base.name, kSlot423Var3Name, 0x1c));
    CHECK(o.record0.name == kSlot423Record0Name && o.record0.hash == kSlot423Record0Hash);
    CHECK(o.record0.unwritten == 0xA5A5A5A5u);   // +0x0c is never written
    CHECK(o.record1.name == kSlot423Record1Name && o.record1.hash == kSlot423Record1Hash);
    CHECK(o.record1.unwritten == 0xA5A5A5A5u);   // +0x0c is never written
    CHECK(r.records_written == 2);
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
    Slot423Objects o;
    const Slot423Result r = init_slot423_55f080(o, creg);
    CHECK(!r.ok && !r.var0.ok && !r.var0.registration.ok && !r.var1.ok && !r.var1.registration.ok && !r.var2.ok && !r.var2.registration.ok && !r.var3.ok && !r.var3.registration.ok && creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_423");
    return 0;
}
