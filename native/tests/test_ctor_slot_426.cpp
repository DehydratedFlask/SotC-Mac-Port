// CPU-01-STARTUP-SLOT-426: fixture for startup constructor index 426 (0x53fb50): the bool console variable
// `showGameStateValues` (value 0, default 0, functor tag 0x15c8e98) and a zeroed 16-byte static with
// atexit(0x539a00). Lane m3.
#include "l4_test_util.hpp"

#include "core/ctor_slot_426_53fb50.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot426Rva == 0x53fb50 && kSlot426Size == 336 && kSlot426FnTag == 0x15c8e98);
    CHECK(kSlot426ObjectRva == 0x2eff370 && kSlot426CellRva == 0x2eff438 && kSlot426PairDtorRva == 0x539a00);
    CHECK(std::strlen(kSlot426Name) == 19 && std::strcmp(kSlot426Name, "showGameStateValues") == 0);
    CHECK(sizeof(Slot426Pair16) == 0x10 && kSlot426PairDtorSize == 83);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot426Objects o;
    poison(o);
    const Slot426Result r = init_slot426_53fb50(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.cell_atexit);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_common(o.var, 0x15c8e98, 0);
    CHECK(same_view(o.var.base.name, "showGameStateValues", 19));
    CHECK(o.cell.ptr == 0 && o.cell.ctl == 0);
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 2);
    check_atexit(creg, 0, 0x20b0, &o.var);
    check_atexit(creg, 1, 0x539a00, &o.cell);
}

void section_failures() {
    // A manager without an insert contract: the variable fails visibly; the cell stage is not reached.
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot426Objects o;
    poison(o);
    const Slot426Result r = init_slot426_53fb50(o, creg);
    CHECK(!r.ok && !r.var.ok && !r.cell_atexit && creg.pending.size() == 0);
    CHECK(o.cell.ptr == 0xa5a5a5a5a5a5a5a5ull && o.cell.ctl == 0xa5a5a5a5a5a5a5a5ull);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_426");
    return 0;
}
