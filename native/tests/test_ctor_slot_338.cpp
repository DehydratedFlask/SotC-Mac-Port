// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 4): fixture for startup constructor index 338 (0x7e6c90): 9 BOOL console variable(s).
#include "l4_test_util.hpp"

#include "core/ctor_slot_338_7e6c90.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot338Rva == 0x7e6c90 && kSlot338Size == 2025);
    CHECK(kSlot338BoolFnTag == 0x15ce460);
    CHECK(std::strlen(kSlot338Var0Name) == kSlot338Var0NameLength && kSlot338Var0NameLength == 0x18);
    CHECK(std::strlen(kSlot338Var1Name) == kSlot338Var1NameLength && kSlot338Var1NameLength == 0x1f);
    CHECK(std::strlen(kSlot338Var2Name) == kSlot338Var2NameLength && kSlot338Var2NameLength == 0x21);
    CHECK(std::strlen(kSlot338Var3Name) == kSlot338Var3NameLength && kSlot338Var3NameLength == 0x1a);
    CHECK(std::strlen(kSlot338Var4Name) == kSlot338Var4NameLength && kSlot338Var4NameLength == 0x1c);
    CHECK(std::strlen(kSlot338Var5Name) == kSlot338Var5NameLength && kSlot338Var5NameLength == 0x20);
    CHECK(std::strlen(kSlot338Var6Name) == kSlot338Var6NameLength && kSlot338Var6NameLength == 0x1e);
    CHECK(std::strlen(kSlot338Var7Name) == kSlot338Var7NameLength && kSlot338Var7NameLength == 0x1d);
    CHECK(std::strlen(kSlot338Var8Name) == kSlot338Var8NameLength && kSlot338Var8NameLength == 0x1d);
    CHECK(sizeof(Slot338Objects) >= 9 * sizeof(ConCmdBoolVar) && sizeof(ConCmdBoolVar) == 0xc8);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot338Objects o;
    poison(o);
    const Slot338Result r = init_slot338_7e6c90(o, creg);
    CHECK(r.ok && r.var0.ok && r.var0.views_inert && r.var1.ok && r.var1.views_inert && r.var2.ok && r.var2.views_inert && r.var3.ok && r.var3.views_inert && r.var4.ok && r.var4.views_inert && r.var5.ok && r.var5.views_inert && r.var6.ok && r.var6.views_inert && r.var7.ok && r.var7.views_inert && r.var8.ok && r.var8.views_inert);
    CHECK(r.var0.registration.ok && r.var0.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var1.registration.ok && r.var1.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var2.registration.ok && r.var2.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var3.registration.ok && r.var3.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var4.registration.ok && r.var4.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var5.registration.ok && r.var5.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var6.registration.ok && r.var6.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var7.registration.ok && r.var7.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var8.registration.ok && r.var8.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_with_description(o.var0, kSlot338BoolFnTag, 0, "", 0);
    CHECK(same_view(o.var0.base.name, kSlot338Var0Name, kSlot338Var0NameLength));
    check_bool_with_description(o.var1, kSlot338BoolFnTag, 0, "", 0);
    CHECK(same_view(o.var1.base.name, kSlot338Var1Name, kSlot338Var1NameLength));
    check_bool_with_description(o.var2, kSlot338BoolFnTag, 0, "", 0);
    CHECK(same_view(o.var2.base.name, kSlot338Var2Name, kSlot338Var2NameLength));
    check_bool_with_description(o.var3, kSlot338BoolFnTag, 0, "", 0);
    CHECK(same_view(o.var3.base.name, kSlot338Var3Name, kSlot338Var3NameLength));
    check_bool_with_description(o.var4, kSlot338BoolFnTag, 0, "", 0);
    CHECK(same_view(o.var4.base.name, kSlot338Var4Name, kSlot338Var4NameLength));
    check_bool_with_description(o.var5, kSlot338BoolFnTag, 0, "", 0);
    CHECK(same_view(o.var5.base.name, kSlot338Var5Name, kSlot338Var5NameLength));
    check_bool_with_description(o.var6, kSlot338BoolFnTag, 0, "", 0);
    CHECK(same_view(o.var6.base.name, kSlot338Var6Name, kSlot338Var6NameLength));
    check_bool_with_description(o.var7, kSlot338BoolFnTag, 0, "", 0);
    CHECK(same_view(o.var7.base.name, kSlot338Var7Name, kSlot338Var7NameLength));
    check_bool_with_description(o.var8, kSlot338BoolFnTag, 0, "", 0);
    CHECK(same_view(o.var8.base.name, kSlot338Var8Name, kSlot338Var8NameLength));
    CHECK(creg.pending.size() == 9);
    CHECK(creg.atexit_log.size() == 9);
    check_atexit(creg, 0, 0x20b0, &o.var0);
    check_atexit(creg, 1, 0x20b0, &o.var1);
    check_atexit(creg, 2, 0x20b0, &o.var2);
    check_atexit(creg, 3, 0x20b0, &o.var3);
    check_atexit(creg, 4, 0x20b0, &o.var4);
    check_atexit(creg, 5, 0x20b0, &o.var5);
    check_atexit(creg, 6, 0x20b0, &o.var6);
    check_atexit(creg, 7, 0x20b0, &o.var7);
    check_atexit(creg, 8, 0x20b0, &o.var8);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot338Objects o;
    const Slot338Result r = init_slot338_7e6c90(o, creg);
    CHECK(!r.ok && !r.var0.ok && !r.var0.registration.ok && !r.var1.ok && !r.var1.registration.ok && !r.var2.ok && !r.var2.registration.ok && !r.var3.ok && !r.var3.registration.ok && !r.var4.ok && !r.var4.registration.ok && !r.var5.ok && !r.var5.registration.ok && !r.var6.ok && !r.var6.registration.ok && !r.var7.ok && !r.var7.registration.ok && !r.var8.ok && !r.var8.registration.ok && creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_338");
    return 0;
}
