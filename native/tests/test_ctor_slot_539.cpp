// CPU-01-STARTUP-SLOT-539: fixture for startup constructor index 539 (0x121940): a zeroed cell, two 0x30-byte name
// holders and the bool variable r_showObjectCount (tag 0x1598020, 0/0). Lane M6.
#include "l4_test_util.hpp"

#include "core/ctor_slot_539_121940.hpp"

namespace {
using namespace l4;

void check_holder(const NameHolder30& h, const char* text, std::size_t len) {
    CHECK(h.vptr == kSlot539HolderVptr && h.vptr == 0x15976b0 && h.tail == 0);
    CHECK(same_view(h.name, text, len));
}

void section_constants() {
    CHECK(kSlot539Rva == 0x121940 && kSlot539Size == 571 && kSlot539CellDtorRva == 0x112d80 &&
          kSlot539HolderDtorRva == 0x8ef00 && kSlot539BoolFnTag == 0x1598020);
    CHECK(std::strlen(kSlot539Holder1Name) == kSlot539Holder1NameLength && kSlot539Holder1NameLength == 0xc);
    CHECK(std::strlen(kSlot539VarName) == kSlot539VarNameLength && kSlot539VarNameLength == 0x11);
    CHECK(std::strlen(kSlot539Holder2Name) == kSlot539Holder2NameLength && kSlot539Holder2NameLength == 0x19);
    CHECK(sizeof(NameHolder30) == 0x30 && sizeof(ConCmdBoolVar) == 0xc8 && sizeof(Slot539Objects) == 0x130);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot539Objects o;
    poison(o);
    const Slot539Result r = init_slot539_121940(o, creg);
    CHECK(r.ok && r.cell_atexit && r.holder1_ok && r.holder2_ok && r.var.ok && r.var.views_inert);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(o.cell == 0);
    check_holder(o.holder1, "r_showBounds", 0xc);
    check_holder(o.holder2, "r_threadRObjectNewAndFree", 0x19);
    check_bool_common(o.var, 0x1598020, 0);
    CHECK(same_view(o.var.base.name, "r_showObjectCount", 0x11));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 4);
    check_atexit(creg, 0, 0x112d80, &o.cell);
    check_atexit(creg, 1, 0x8ef00, &o.holder1);
    check_atexit(creg, 2, 0x20b0, &o.var);
    check_atexit(creg, 3, 0x8ef00, &o.holder2);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot539Objects o;
    poison(o);
    const Slot539Result r = init_slot539_121940(o, creg);
    CHECK(!r.ok && r.cell_atexit && r.holder1_ok && !r.var.ok && !r.var.registration.ok && r.holder2_ok);
    CHECK(creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_539");
    return 0;
}
