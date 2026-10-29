// CPU-01-STARTUP-SLOT-569: fixture for startup constructor index 569 (0x1cc80): an atomically zeroed cell, the
// ranged int variable `mem_platformMemVerify` (0..2 via 0xc580, functor tag 0x1594c70) and its debug-menu toggle. Lane M6.
#include "l4_test_util.hpp"

#include "core/ctor_slot_569_1cc80.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot569Rva == 0x1cc80 && kSlot569Size == 491 && kSlot569IntFnTag == 0x1594c70);
    CHECK(std::strlen(kSlot569VarName) == kSlot569VarNameLength && kSlot569VarNameLength == 21);
    CHECK(std::strlen(kSlot569MenuLabel) == 6 && std::strlen(kSlot569MenuText) == 30);
    CHECK(kSlot569Min == 0 && kSlot569Max == 2);
    CHECK(lane_m6::is_lane_m6_int_site_tag(0x1594c70) && !lane_m6::is_lane_m6_int_site_tag(0x15944a0));
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot569Objects o;
    poison(o);
    const Slot569Result r = init_slot569_1cc80(o, creg, menu);
    CHECK(r.ok && r.cell_stored && r.var.ok && r.var.ctor_ok && r.var.functor_destroy_ok && r.var.views_inert &&
          r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(o.cell == 0);

    const ConCmdRangedVar& v = o.var;
    CHECK(v.base.vptr == kRangedIntFinalVptr && v.base.flags == 1 && v.base.registered == 1);
    CHECK(same_view(v.base.name, "mem_platformMemVerify", 21));
    CHECK(empty_view(v.base.description));
    CHECK(v.value_ref == &o.var.value && v.value == 0 && v.default_value == 0);
    CHECK(dword(v, 0x6c) == 0 && v.min_engaged == 1 && dword(v, 0x74) == 2 && v.max_engaged == 1);
    for (std::uint64_t q : v.args) CHECK(q == 0);
    CHECK(v.fn_vptr == kRangedIntFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);   // class tag from the clone/copy bodies

    CHECK(r.toggle.ok && r.toggle.ctor_ok && r.toggle.views_inert && r.toggle.atexit_recorded);
    const DebugMenuItem& it = o.item;
    CHECK(it.kind == 2 && it.flags == 0x30 && it.target == &o.var && it.registered == 0);   // the root-list path leaves +0x20 clear (as the lifted 0x268b0 does)
    CHECK(same_view(it.label, "Memory", 6) && same_view(it.text, "PS4 Platform MemVerify (0=off)", 30));
    CHECK(empty_view(it.empty));
    CHECK(dword(it, 0x98) == 0x3f800000u && dword(it, 0x9c) == 0x41200000u);   // slots {1, 2}: default range kept
    CHECK(menu.root.count == 1 && menu.root.link.next == &it.link);

    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 3);
    check_atexit(creg, 0, 0x18a60, &o.var);
    check_atexit(creg, 1, 0x27280, &menu.root);
    check_atexit(creg, 2, 0x272d0, &o.item);
}

void section_tag_refusal() {
    ConCmdRegistry creg;
    ConCmdRangedVar v;
    poison(v);
    const lane_m6::IntVarSiteSpec bad{kSlot569VarName, kSlot569VarNameLength, "", 0, 0, 2, 1, 0x1234567, 0, 0};
    const lane_m6::IntVarResult r = lane_m6::init_int_var_site(v, bad, creg);
    CHECK(!r.ok && !r.ctor_ok && creg.pending.size() == 0 && creg.atexit_log.size() == 0);
    const auto* b = reinterpret_cast<const std::uint8_t*>(&v);
    for (std::size_t i = 0; i < sizeof v; ++i) CHECK(b[i] == kPoison);   // an unknown functor tag writes nothing
}

void section_failures() {
    {
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot569Objects o;
        const Slot569Result r = init_slot569_1cc80(o, creg, menu);
        CHECK(!r.ok && r.cell_stored && !r.var.ok && !r.var.registration.ok && menu.root.count == 0);
        CHECK(creg.pending.size() == 0);
    }
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;   // parent path without an insert contract
        Slot569Objects o;
        const Slot569Result r = init_slot569_1cc80(o, creg, menu);
        CHECK(!r.ok && r.var.ok && !r.toggle.ok && r.toggle.registration.path == DebugMenuRegisterPath::parent);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_tag_refusal();
    section_failures();
    std::puts("OK test_ctor_slot_569");
    return 0;
}
