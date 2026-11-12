// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 3): fixture for startup constructor index 362 (0x76b8d0): 3 bool console variable(s) (tag 0x15cd9f8), 1 hashed-name record(s).
#include "l4_test_util.hpp"

#include "core/ctor_slot_362_76b8d0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot362Rva == 0x76b8d0 && kSlot362Size == 760 && kSlot362BoolFnTag == 0x15cd9f8);
    CHECK(std::strlen(kSlot362Var0Name) == kSlot362Var0NameLength && kSlot362Var0NameLength == 0x15);
    CHECK(std::strlen(kSlot362Var1Name) == kSlot362Var1NameLength && kSlot362Var1NameLength == 0x18);
    CHECK(std::strlen(kSlot362Var2Name) == kSlot362Var2NameLength && kSlot362Var2NameLength == 0x20);
    CHECK(std::strlen(kSlot362Record0Name) > 0 && fnv_cstr_52a990(kSlot362Record0Name) == kSlot362Record0Hash && kSlot362Record0Hash == 0xcfb05cf0);
    CHECK(sizeof(Slot362Objects) >= 3 * sizeof(ConCmdBoolVar) && sizeof(ConCmdBoolVar) == 0xc8);
}

void section_alone() {
    ConCmdRegistry creg;

    Slot362Objects o;
    poison(o);
    const Slot362Result r = init_slot362_76b8d0(o, creg);
    CHECK(r.ok && r.var0.ok && r.var0.views_inert && r.var1.ok && r.var1.views_inert && r.var2.ok && r.var2.views_inert);
    CHECK(r.var0.registration.ok && r.var0.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var1.registration.ok && r.var1.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var2.registration.ok && r.var2.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_with_description(o.var0, 0x15cd9f8, 0, "", 0);
    CHECK(same_view(o.var0.base.name, kSlot362Var0Name, 0x15));
    check_bool_with_description(o.var1, 0x15cd9f8, 0, "", 0);
    CHECK(same_view(o.var1.base.name, kSlot362Var1Name, 0x18));
    check_bool_with_description(o.var2, 0x15cd9f8, 0, "", 0);
    CHECK(same_view(o.var2.base.name, kSlot362Var2Name, 0x20));
    CHECK(o.record0.name == kSlot362Record0Name && o.record0.hash == kSlot362Record0Hash);
    CHECK(o.record0.unwritten == 0xA5A5A5A5u);   // +0x0c is never written
    CHECK(r.records_written == 1);
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
    Slot362Objects o;
    const Slot362Result r = init_slot362_76b8d0(o, creg);
    CHECK(!r.ok && !r.var0.ok && !r.var0.registration.ok && !r.var1.ok && !r.var1.registration.ok && !r.var2.ok && !r.var2.registration.ok && creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_362");
    return 0;
}
