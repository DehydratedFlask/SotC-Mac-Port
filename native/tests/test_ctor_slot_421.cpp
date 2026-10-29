// CPU-01-STARTUP-SLOT-421: fixture for startup constructor index 421 (0x56da20): the bool variable
// `filesystem_allowAsync` (constructor flags 8, value/default 0, tag 0x15cade0) and a 128-byte zero block. Lane M2.
#include "core/lane_m2_testkit.hpp"

#include "core/ctor_slot_421_56da20.hpp"

namespace {
using namespace sotc::lane_m2;

void section_constants() {
    CHECK(kSlot421Rva == 0x56da20 && kSlot421Size == 329 && kSlot421FnTag == 0x15cade0 && kSlot421CtorFlags == 8 &&
          kSlot421BlockSize == 128);
    CHECK(std::strlen(kSlot421VarName) == 0x15);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot421Objects o;
    poison(o);
    const Slot421Result r = init_slot421_56da20(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    check_bool_var(o.var, "filesystem_allowAsync", 0x15cade0, 0, 8);   // flags 8, not the usual 1
    for (std::uint8_t byte : o.block) CHECK(byte == 0);
    check_var_atexit_and_pending(creg, o.var);
    CHECK(creg.atexit_log.size() == 1);
}

void section_flags_helper() {
    // The flagged initialiser honours the flags argument and otherwise matches init_bool_var_site.
    ConCmdRegistry creg1, creg2;
    ConCmdBoolVar a, b;
    poison(a);
    poison(b);
    CHECK(init_bool_var_flagged(a, "x", 0x15cade0, 1, creg1, 1, 1).ok);
    CHECK(init_bool_var_site(b, "x", 0x15cade0, creg2, 1, 1).ok);
    CHECK(a.base.flags == 1 && b.base.flags == 1);
    CHECK(a.value == b.value && a.default_value == b.default_value && a.fn_vptr == b.fn_vptr);
    ConCmdBoolVar c;
    ConCmdRegistry creg3;
    CHECK(init_bool_var_flagged(c, "x", 0x15cade0, 8, creg3, 1, 1).ok && c.base.flags == 8);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot421Objects o;
    std::memset(o.block.data(), kPoison, o.block.size());
    const Slot421Result r = init_slot421_56da20(o, creg);
    CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && creg.pending.size() == 0);
    for (std::uint8_t byte : o.block) CHECK(byte == kPoison);   // the block stage is not reached
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_flags_helper();
    section_failures();
    std::puts("OK test_ctor_slot_421");
    return 0;
}
