// CPU-01-STARTUP-SLOT-489: fixture for startup constructor index 489 (0x2acc10): the single unbounded int
// console variable `disableInput` (value 0, default 0, worker 0x3a980 with argument-functor tag 0x159fae0,
// class vptr 0x1594c08, atexit 0x18a60). Lane M4.
#include "l4_test_util.hpp"

#include "core/ctor_slot_489_2acc10.hpp"
#include "core/ctor_slot_490_296d90.hpp"

namespace {
using namespace l4;

void check_int_var(const ConCmdRangedVar& v, const char* name, std::size_t name_length, std::uint32_t value) {
    CHECK(v.base.vptr == 0x1594c08 && v.base.registered == 1 && v.base.flags == 1);
    CHECK(same_view(v.base.name, name, name_length) && empty_view(v.base.description));
    CHECK(v.value_ref == &v.value && v.default_value == value && v.value == value);
    CHECK(v.min_value[0] == 0 && v.min_engaged == 0 && v.max_value[0] == 0 && v.max_engaged == 0);
    for (std::uint64_t q : v.args) CHECK(q == 0);
    CHECK(v.fn_payload == 0 && v.fn_vptr == 0x15944a0 && v.fn_aux == 0);   // the worker ends with the class tag
    check_untouched(v, {{0x00, 8}, {0x08, 0x10}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 8}, {0x68, 4},
                        {0x6c, 1}, {0x70, 1}, {0x74, 1}, {0x78, 1}, {0x80, 0x20}, {0xb0, 8}, {0xc0, 8},
                        {0xc8, 8}, {0xd0, 4}});
}

void section_constants() {
    CHECK(kSlot489Rva == 0x2acc10 && kSlot489Size == 342 && kSlot489ArgFnTag == 0x159fae0);
    CHECK(sizeof kSlot489Name - 1 == 12 && std::strcmp(kSlot489Name, "disableInput") == 0);
    CHECK(kSlot489ObjectRva == 0x2eef6b0 && kSlot489Value == 0 && kSlot489Default == 0);
    CHECK(lane_m4::kIntVarDtorRva == 0x18a60 && lane_m4::kIntVarCtorFlags == 1);
    CHECK(lane_m4::kIntArgTagSlot489 == 0x159fae0 && lane_m4::kIntArgTagSlot486 == 0x15a2248);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot489Objects o;
    poison(o);
    const Slot489Result r = init_slot489_2acc10(o, creg);
    CHECK(r.ok && r.var.ok && r.var.worker_ok && r.var.description_inert && r.var.arg_functor_destroyed);
    CHECK(r.var.name_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_int_var(o.var, "disableInput", 12, 0);
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x18a60, &o.var);
    // The int class answers 1 / 2 to the toggle's two virtual calls (no item follows at this slot).
    const DebugMenuTarget t = unbounded_int_menu_target(o.var);
    CHECK(t.known && t.slot3 == 1 && t.slot6 == 2);
}

void section_arg_tags() {
    // Only tags whose four slot bodies are pinned are accepted; anything else is refused before any write.
    CHECK(unbounded_int_fn_table_lifted(0x159fae0) && unbounded_int_fn_table_lifted(0x15a2248));
    CHECK(unbounded_int_fn_table_lifted(0x15ce8c0) && unbounded_int_fn_table_lifted(0x1595660));
    CHECK(!unbounded_int_fn_table_lifted(0x15d1fa8));
    ConCmdRegistry creg;
    ConCmdRangedVar v;
    poison(v);
    const SharedStr name = lane_m4::borrowed_view_of("n", 1);
    const SharedStr desc = lane_m4::borrowed_view_of("", 0);
    const lane_m4::IntVarSiteResult r = lane_m4::init_int_var_site(v, name, desc, 0x15d1fa8, creg, 0, 0);
    CHECK(!r.ok && !r.worker_ok && creg.pending.size() == 0 && creg.atexit_log.size() == 0);
    check_untouched(v, {});
    // An M4 caller table is passed through unchanged.
    ConCmdRangedVar w;
    const lane_m4::IntVarSiteResult s = lane_m4::init_int_var_site(w, name, desc, 0x15a2248, creg, 5, 6);
    CHECK(s.ok && w.value == 5 && w.default_value == 6);
}

void section_with_neighbour() {
    ConCmdRegistry creg;
    Slot489Objects a;
    Slot490Objects b;
    CHECK(init_slot489_2acc10(a, creg).ok);
    CHECK(init_slot490_296d90(b, creg).ok);
    CHECK(creg.pending.size() == 2);
    CHECK(a.var.base.pending.next == &b.var.base.pending && b.var.base.pending.next == &creg.pending.head);
    CHECK(creg.atexit_log.size() == 2);
    check_atexit(creg, 0, 0x18a60, &a.var);
    check_atexit(creg, 1, 0x20b0, &b.var);
}

void section_failures() {
    {   // manager without an insert contract: the registration fails visibly and nothing is queued
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot489Objects o;
        const Slot489Result r = init_slot489_2acc10(o, creg);
        CHECK(!r.ok && !r.var.ok && r.var.worker_ok && !r.var.registration.ok &&
              r.var.registration.path == ConCmdRegisterPath::manager);
        CHECK(creg.pending.size() == 0);
    }
    {   // an owned (counted) description view is not an inert release
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        std::int64_t control = 1;
        const SharedStr name = lane_m4::borrowed_view_of("n", 1);
        const SharedStr desc{reinterpret_cast<const std::uint8_t*>("d"), 1, 0, &control};
        const lane_m4::IntVarSiteResult r = lane_m4::init_int_var_site(v, name, desc, 0x159fae0, creg, 0, 0);
        CHECK(!r.ok && r.worker_ok && !r.description_inert && r.registration.ok);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_arg_tags();
    section_with_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_489");
    return 0;
}
