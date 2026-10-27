// CPU-01-STARTUP-SLOT-343: fixture for startup constructor index 343 (0x7d7970): two zeroed 24-byte statics with
// atexit(0x7d3af0) and the console command `fireCaption`. Lane L4.
#include "l4_test_util.hpp"

#include "core/ctor_slot_343_7d7970.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot343Rva == 0x7d7970 && kSlot343Size == 378 && kSlot343HandlerTag == 0x7d6420);
    CHECK(std::strlen(kSlot343CommandName) == 11 && std::strlen(kSlot343CommandDescription) == 0x31);
    CHECK(kSlot343CommandDescriptionLength == 0x31 && sizeof(ZeroVector24) == 0x18 && kZeroVectorDtorRva == 0x7d3af0);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot343Objects o;
    poison(o);
    const Slot343Result r = init_slot343_7d7970(o, creg);
    CHECK(r.ok && r.cell_a_atexit && r.cell_b_atexit && r.command.ok && r.command.views_inert);
    CHECK(o.cell_a.begin == 0 && o.cell_a.end == 0 && o.cell_a.cap == 0);
    CHECK(o.cell_b.begin == 0 && o.cell_b.end == 0 && o.cell_b.cap == 0);
    check_command(o.command, "fireCaption", "Fires any caption by name. Params: <caption name>", 0x31, 0x7d6420);
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.command.base.pending);
    CHECK(creg.atexit_log.size() == 3);
    check_atexit(creg, 0, 0x7d3af0, &o.cell_a);
    check_atexit(creg, 1, 0x7d3af0, &o.cell_b);
    check_atexit(creg, 2, 0x4b0, &o.command);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot343Objects o;
    const Slot343Result r = init_slot343_7d7970(o, creg);
    CHECK(!r.ok && r.cell_a_atexit && r.cell_b_atexit && !r.command.ok);   // cells are plain stores; the command fails
    CHECK(!r.command.registration.ok && creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_343");
    return 0;
}
