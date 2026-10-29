// CPU-01-STARTUP-SLOT-516: fixture for startup constructor index 516 (0x1b97b0): the string console variable
// `network_PlayerName` (empty initial value). Lane M5.
#include "l4_test_util.hpp"

#include "core/ctor_slot_516_1b97b0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot516Rva == 0x1b97b0 && kSlot516Size == 177 && kSlot516ObjectRva == 0x2ee6780);
    CHECK(sizeof kSlot516Name - 1 == 18 && std::strcmp(kSlot516Name, "network_PlayerName") == 0);
}

bool all_poison(const ConCmdStringVar& v, std::size_t lo, std::size_t hi) {
    const auto* b = reinterpret_cast<const std::uint8_t*>(&v);
    for (std::size_t i = lo; i < hi; ++i) if (b[i] != kPoison) return false;
    return true;
}

void section_alone() {
    ConCmdRegistry creg;
    Slot516Objects o;
    poison(o);
    const Slot516Result r = init_slot516_1b97b0(o, creg);
    const ConCmdStringVar& v = o.var;
    CHECK(r.ok && r.blocked == Slot516Stage::none && r.var.ok && r.var.views_inert && r.var.registration.ok);
    CHECK(r.var.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(v.base.vptr == kStringVarFinalVptr && v.base.flags == 1 && v.base.registered == 1);
    CHECK(same_view(v.base.name, "network_PlayerName", 18));
    CHECK(v.value_ref == &v.value);
    CHECK(same_view(v.default_value, "", 0) && same_view(v.value, "", 0));
    CHECK(v.byte88 == 0 && v.bytea8 == 0 && v.byteb0 == 0 && v.byted0 == 0);
    CHECK(v.fn_vptr == kStringVarFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
    CHECK(all_poison(v, 0x89, 0xa8) && all_poison(v, 0xa9, 0xb0) && all_poison(v, 0xb1, 0xd0));
    CHECK(all_poison(v, 0xf8, 0x108) && all_poison(v, 0x110, 0x118));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &v.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x23c90, &o.var);
}

void section_twice_is_two_objects() {
    // Two independent objects (as two sites would be): pending order is registration order.
    ConCmdRegistry creg;
    Slot516Objects a, b;
    CHECK(init_slot516_1b97b0(a, creg).ok && init_slot516_1b97b0(b, creg).ok);
    CHECK(creg.pending.size() == 2 && a.var.base.pending.next == &b.var.base.pending);
    CHECK(creg.atexit_log.size() == 2);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;   // a manager without an insert contract: visible failure, nothing queued
    Slot516Objects o;
    const Slot516Result r = init_slot516_1b97b0(o, creg);
    CHECK(!r.ok && r.blocked == Slot516Stage::string_var && !r.var.ok);
    CHECK(creg.pending.size() == 0 && creg.atexit_log.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_twice_is_two_objects();
    section_failures();
    std::puts("OK test_ctor_slot_516");
    return 0;
}
