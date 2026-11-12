// CPU-01-STARTUP-SLOT-430: fixture for startup constructor index 430 (0x529df0): a zeroed static cell with an
// atexit destructor 0x523ba0 (recorded, never run), the ranged int variable `wanderForceCorruptionLevel` (-1..5 via
// 0xc580 through the family wrapper, functor tag 0x15c8450) and its debug-menu toggle. Batch 6.
#include "l4_test_util.hpp"

#include "core/ctor_slot_430_529df0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot430Rva == 0x529df0 && kSlot430Size == 539 && kSlot430IntFnTag == 0x15c8450);
    CHECK(std::strlen(kSlot430VarName) == kSlot430VarNameLength && kSlot430VarNameLength == 0x1a);
    CHECK(std::strlen(kSlot430VarDescription) == kSlot430VarDescriptionLength && kSlot430VarDescriptionLength == 0x13);
    CHECK(std::strlen(kSlot430MenuLabel) == 12 && std::strlen(kSlot430MenuText) == 20);
    CHECK(kSlot430Min == -1 && kSlot430Max == 5 && kSlot430Value == -1);
    CHECK(kSlot430VarRva - kSlot430CellRva == offsetof(Slot430Objects, var));
    CHECK(kSlot430ItemRva - kSlot430CellRva == offsetof(Slot430Objects, item));
    CHECK(is_int_functor_tag(kSlot430IntFnTag) && !is_float_functor_tag(kSlot430IntFnTag));
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot430Objects o;
    poison(o);
    const Slot430Result r = init_slot430_529df0(o, creg, menu);
    CHECK(r.ok && r.cell_stored && r.cell_atexit_recorded);
    CHECK(r.var.ok && r.var.refusal == HelperRefusal::none && r.var.ctor_ok && r.var.functor_destroy_ok &&
          r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(o.cell == 0);

    const ConCmdRangedVar& v = o.var;
    CHECK(v.base.vptr == kRangedIntFinalVptr && v.base.flags == 1 && v.base.registered == 1);
    CHECK(same_view(v.base.name, "wanderForceCorruptionLevel", 0x1a));
    CHECK(same_view(v.base.description, "Set Corrupt Unlock.", 0x13));
    CHECK(v.value_ref == &o.var.value && dword(v, 0xd0) == 0xffffffffu && dword(v, 0x68) == 0xffffffffu);
    CHECK(dword(v, 0x6c) == 0xffffffffu && v.min_engaged == 1 && dword(v, 0x74) == 5 && v.max_engaged == 1);
    for (std::uint64_t q : v.args) CHECK(q == 0);
    CHECK(v.fn_vptr == kRangedIntFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);   // class tag from the clone/copy bodies

    CHECK(r.toggle.ok && r.toggle.ctor_ok && r.toggle.views_inert && r.toggle.atexit_recorded);
    const DebugMenuItem& it = o.item;
    CHECK(it.kind == 2 && it.flags == 0x30 && it.target == &o.var && it.registered == 0);
    CHECK(same_view(it.label, "Shadow Cheat", 12) && same_view(it.text, "Wander Corrupt Level", 20));
    CHECK(empty_view(it.empty));
    CHECK(dword(it, 0x98) == 0x3f800000u && dword(it, 0x9c) == 0x41200000u);   // slots {1, 2}: default range kept
    CHECK(menu.root.count == 1 && menu.root.link.next == &it.link);

    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 4);
    check_atexit(creg, 0, 0x523ba0, &o.cell);
    check_atexit(creg, 1, 0x18a60, &o.var);
    check_atexit(creg, 2, 0x27280, &menu.root);
    check_atexit(creg, 3, 0x272d0, &o.item);
}

void section_tag_refusal() {
    // A tag that is not one of the 37 int-family tables is refused by the wrapper; nothing is written.
    ConCmdRegistry creg;
    ConCmdRangedVar v;
    poison(v);
    const HelperIntSiteSpec bad{kSlot430VarName, kSlot430VarNameLength, "", 0, -1, 5, 1, 0x1234567, -1, -1};
    const HelperIntSiteResult r = helper_init_ranged_int_var_site(v, bad, creg);
    CHECK(!r.ok && !r.ctor_ok && r.refusal == HelperRefusal::unadmitted_arg_functor);
    CHECK(creg.pending.size() == 0 && creg.atexit_log.size() == 0);
    const auto* b = reinterpret_cast<const std::uint8_t*>(&v);
    // The pre-stored vptr is the only write (the original site stores it before the call).
    for (std::size_t i = 8; i < sizeof v; ++i) CHECK(b[i] == kPoison);
    // A float-family tag is not an int functor: refused as well.
    const HelperIntSiteSpec fl{kSlot430VarName, kSlot430VarNameLength, "", 0, -1, 5, 1, 0x159ba68, -1, -1};
    CHECK(helper_init_ranged_int_var_site(v, fl, creg).refusal == HelperRefusal::unadmitted_arg_functor);
}

void section_failures() {
    {
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot430Objects o;
        const Slot430Result r = init_slot430_529df0(o, creg, menu);
        CHECK(!r.ok && r.cell_stored && !r.var.ok && !r.var.registration.ok && menu.root.count == 0);
        CHECK(creg.pending.size() == 0);
    }
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;   // parent path without an insert contract
        Slot430Objects o;
        const Slot430Result r = init_slot430_529df0(o, creg, menu);
        CHECK(!r.ok && r.var.ok && !r.toggle.ok && r.toggle.registration.path == DebugMenuRegisterPath::parent);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_tag_refusal();
    section_failures();
    std::puts("OK test_ctor_slot_430");
    return 0;
}
