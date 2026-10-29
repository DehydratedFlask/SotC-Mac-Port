// CPU-01-STARTUP-SLOT-486: fixture for startup constructor index 486 (0x2d5e00): the unbounded int console
// variable `SM_debugging` (argument-functor tag 0x15a2248, atexit 0x18a60) followed by the string variable
// `SM_debugIdentifier` (0x257a0, empty initial value, atexit 0x23c90). Lane M4.
#include "l4_test_util.hpp"

#include "core/ctor_slot_486_2d5e00.hpp"
#include "core/ctor_slot_489_2acc10.hpp"

namespace {
using namespace l4;

bool all_poison(const void* p, std::size_t from, std::size_t to) {
    const auto* b = static_cast<const std::uint8_t*>(p);
    for (std::size_t i = from; i < to; ++i) {
        if (b[i] != kPoison) return false;
    }
    return true;
}

void section_constants() {
    CHECK(kSlot486Rva == 0x2d5e00 && kSlot486Size == 462 && kSlot486ArgFnTag == 0x15a2248);
    CHECK(sizeof kSlot486IntName - 1 == 12 && std::strcmp(kSlot486IntName, "SM_debugging") == 0);
    CHECK(sizeof kSlot486StringName - 1 == 18 && std::strcmp(kSlot486StringName, "SM_debugIdentifier") == 0);
    CHECK(kSlot486IntRva == 0x2eefa70 && kSlot486StringRva == 0x2eefb48);
    CHECK(kSlot486StringRva - kSlot486IntRva == sizeof(ConCmdRangedVar));   // string variable directly after
    CHECK(kSlot486Value == 0 && kSlot486Default == 0);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot486Objects o;
    poison(o);
    const Slot486Result r = init_slot486_2d5e00(o, creg);
    CHECK(r.ok && r.int_var.ok && r.string_var.ok && r.string_var.views_inert);
    CHECK(r.string_views_inert && r.string_atexit_recorded);
    CHECK(r.int_var.registration.ok && r.string_var.registration.ok);

    const ConCmdRangedVar& iv = o.int_var;
    CHECK(iv.base.vptr == 0x1594c08 && iv.base.registered == 1 && iv.base.flags == 1);
    CHECK(same_view(iv.base.name, "SM_debugging", 12) && empty_view(iv.base.description));
    CHECK(iv.value_ref == &iv.value && iv.default_value == 0 && iv.value == 0);
    CHECK(iv.min_engaged == 0 && iv.max_engaged == 0 && iv.fn_vptr == 0x15944a0 && iv.fn_payload == 0 &&
          iv.fn_aux == 0);
    for (std::uint64_t q : iv.args) CHECK(q == 0);

    const ConCmdStringVar& sv = o.string_var;
    CHECK(sv.base.vptr == 0x1594ef0 && sv.base.registered == 1 && sv.base.flags == 1);
    CHECK(same_view(sv.base.name, "SM_debugIdentifier", 18));
    CHECK(sv.base.description.size == 0 && sv.base.description.ctl == nullptr && sv.base.description.data[0] == 0);
    CHECK(sv.value_ref == &sv.value);
    CHECK(same_view(sv.default_value, "", 0) && same_view(sv.value, "", 0));
    CHECK(sv.byte88 == 0 && sv.bytea8 == 0 && sv.byteb0 == 0 && sv.byted0 == 0);
    for (std::uint64_t q : sv.args) CHECK(q == 0);
    CHECK(sv.fn_vptr == 0x1595068 && sv.fn_payload == 0 && sv.fn_aux == 0);
    CHECK(all_poison(&sv, 0x89, 0xa8) && all_poison(&sv, 0xa9, 0xb0) && all_poison(&sv, 0xb1, 0xd0));
    CHECK(all_poison(&sv, 0xd1, 0xd8) && all_poison(&sv, 0xf8, 0x108) && all_poison(&sv, 0x110, 0x118));
    const DebugMenuTarget t = string_var_menu_target(sv);
    CHECK(t.known && t.slot3 == 1 && t.slot6 == 4);

    // Registration order: the int variable first, then the string variable.
    CHECK(creg.pending.size() == 2 && creg.pending.first() == &iv.base.pending);
    CHECK(iv.base.pending.next == &sv.base.pending && sv.base.pending.next == &creg.pending.head);
    CHECK(creg.atexit_log.size() == 2);
    check_atexit(creg, 0, 0x18a60, &o.int_var);
    check_atexit(creg, 1, 0x23c90, &o.string_var);
}

void section_after_neighbour() {
    ConCmdRegistry creg;
    Slot486Objects a;
    Slot489Objects b;
    CHECK(init_slot486_2d5e00(a, creg).ok && init_slot489_2acc10(b, creg).ok);
    CHECK(creg.pending.size() == 3 && a.string_var.base.pending.next == &b.var.base.pending);
    CHECK(creg.atexit_log.size() == 3);
    check_atexit(creg, 2, 0x18a60, &b.var);
}

void section_failures() {
    {   // manager without an insert contract: the int variable fails and the string variable is not reached
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot486Objects o;
        poison(o.string_var);
        const Slot486Result r = init_slot486_2d5e00(o, creg);
        CHECK(!r.ok && !r.int_var.ok && !r.string_var.ok && creg.pending.size() == 0);
        CHECK(all_poison(&o.string_var, 0, sizeof o.string_var));
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_after_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_486");
    return 0;
}
