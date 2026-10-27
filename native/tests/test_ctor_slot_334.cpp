// CPU-01-STARTUP-SLOT-334-01 (lane L3): fixture for startup constructor index 334 (0x7fc530, 481 B): the
// unbounded int variable `gMaxSimaltaneousAreaLoadRequests` (0x32882f8, value/default 0x10, worker 0x3a980,
// class vptr 0x1594c08, atexit 0x18a60) and its "Shadow Misc" toggle (0x26ea0, item 0x32883d0, flags 0x30).
// Sections: 1 constants, 2 every stored/untouched byte and atexit order, 3 the worker alone (bounds not
// engaged, functor install, fail-closed on an unlifted functor table), 4 visible failures.
#include "ctor_slot_family_test_util.hpp"

#include "core/ctor_slots_322_334.hpp"

namespace {

using namespace famtest;

void section_constants() {
    CHECK(kSlot334Rva == 0x7fc530 && kSlot334BodySize == 481);
    CHECK(kSlot334VarRva == 0x32882f8 && kSlot334ItemRva == 0x32883d0);
    CHECK(kSlot334ItemRva - kSlot334VarRva == 0xd8);   // item directly after the 0xd4-byte variable, 8-aligned
    CHECK(kSlot334ArgFnTable == 0x15ce8c0 && kSlot334CtorFlags == 1 && kSlot334Value == 0x10 &&
          kSlot334Default == 0x10 && kSlot334DtorRva == 0x18a60 && kSlot334ItemFlags == 0x30);
    CHECK(kUnboundedIntWorkerRva == 0x3a980 && kUnboundedIntWorkerSize == 0x142);
    CHECK(kUnboundedIntFinalVptr == 0x1594c08 && kUnboundedIntWorkerVptr == 0x15941a0);
    CHECK(kUnboundedIntSeedFnTable == 0x1595660 && kUnboundedIntFnTable == 0x15944a0);
    CHECK(kUnboundedIntSlot3Result == 1 && kUnboundedIntSlot6Result == 2);
    CHECK(sizeof kSlot334Name - 1 == 32 && std::strcmp(kSlot334Name, "gMaxSimaltaneousAreaLoadRequests") == 0);
    CHECK(sizeof kSlot334Label - 1 == 11 && std::strcmp(kSlot334Label, "Shadow Misc") == 0);
    CHECK(sizeof kSlot334Text - 1 == 31 && std::strcmp(kSlot334Text, "MaxSimaltaneousAreaLoadRequests") == 0);
}

void check_var(const ConCmdRangedVar& v) {
    CHECK(v.base.vptr == 0x1594c08 && v.base.registered == 1 && v.base.flags == 1);
    CHECK(same_view(v.base.name, kSlot334Name, 32) && empty_view(v.base.description));
    CHECK(v.value_ref == &v.value && v.default_value == 0x10 && v.value == 0x10);
    CHECK(v.min_value[0] == 0 && v.min_engaged == 0 && v.max_value[0] == 0 && v.max_engaged == 0);
    for (std::uint64_t q : v.args) CHECK(q == 0);
    CHECK(v.fn_payload == 0 && v.fn_vptr == 0x15944a0 && v.fn_aux == 0);
    // Stores: base + registration link, +0x60, +0x68, the four bound bytes (+0x6c/+0x70/+0x74/+0x78),
    // the 0x20-byte argument copy, functor payload/tag/aux and the value. The dword-aligned padding and
    // +0xa0..+0xaf / +0xb8 are never written.
    check_untouched(v, {{0x00, 8}, {0x08, 0x10}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 8}, {0x68, 4},
                        {0x6c, 1}, {0x70, 1}, {0x74, 1}, {0x78, 1}, {0x80, 0x20}, {0xb0, 8}, {0xc0, 8},
                        {0xc8, 8}, {0xd0, 4}});
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot334Objects o;
    poison(o);
    const Slot334Result r = init_slot334_7fc530(o, creg, menu);
    CHECK(r.ok && r.worker_ok && r.description_inert && r.arg_functor_destroyed && r.name_inert &&
          r.var_atexit_recorded);
    CHECK(r.registration.ok && r.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.item.ok() && r.item.registration.path == DebugMenuRegisterPath::root_list);
    check_var(o.var);
    // The int class answers 1 / 2 for the two virtual calls, so the {1.0, 10.0} default range stays.
    check_leaf_item(o.item, &o.var, 0x30, kSlot334Label, 11, kSlot334Text, 31);
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(menu.root_guard_done && menu.root.count == 1 && menu.root.link.next == &o.item.link);
    CHECK(creg.atexit_log.size() == 3);
    CHECK(creg.atexit_log.at(0).dtor_rva == 0x18a60 && creg.atexit_log.at(0).object == &o.var);
    CHECK(creg.atexit_log.at(1).dtor_rva == 0x27280 && creg.atexit_log.at(1).object == &menu.root);
    CHECK(creg.atexit_log.at(2).dtor_rva == 0x272d0 && creg.atexit_log.at(2).object == &o.item);
}

void section_worker() {
    const SharedStr name = view("n", 1);
    const SharedStr desc = view("", 0);
    // every lifted table ends with the 0x15944a0 tag; payload/aux come from the block
    for (std::uint64_t table : {0x15ce8c0ull, 0x1595660ull, 0x15944a0ull}) {
        ConCmdRangedVar v;
        poison(v);
        RangedVarArgs args{};
        args.fn_payload = 0x1111;
        args.fn_aux = 0x2222;
        args.fn_vptr = table;
        CHECK(unbounded_int_fn_table_lifted(table));
        CHECK(init_unbounded_int_var_worker_3a980(v, name, 1, args, desc));
        CHECK(v.base.vptr == 0x15941a0 && v.fn_vptr == 0x15944a0 && v.fn_payload == 0x1111 && v.fn_aux == 0x2222);
        CHECK(v.min_engaged == 0 && v.max_engaged == 0);
    }
    {   // block word +0x18 non-zero: the copy of the first qword becomes a self pointer
        ConCmdRangedVar v;
        RangedVarArgs args{};
        args.q[0] = 7; args.q[1] = 8; args.q[2] = 9; args.q[3] = 10;
        args.fn_vptr = 0x15ce8c0;
        CHECK(init_unbounded_int_var_worker_3a980(v, name, 1, args, desc));
        CHECK(v.args[0] == reinterpret_cast<std::uint64_t>(&v.args[0]) && v.args[1] == 8 && v.args[2] == 9 &&
              v.args[3] == 10);
    }
    {   // an unlifted functor table is refused before any write
        ConCmdRangedVar v;
        poison(v);
        RangedVarArgs args{};
        args.fn_vptr = 0x15d1fa8;   // a different (slot-306) table: not this worker's lift
        CHECK(!unbounded_int_fn_table_lifted(args.fn_vptr));
        CHECK(!init_unbounded_int_var_worker_3a980(v, name, 1, args, desc));
        check_untouched(v, {});
    }
    {   // the menu target is only known for the final class
        ConCmdRangedVar v;
        poison(v);
        CHECK(!unbounded_int_menu_target(v).known);
        v.base.vptr = 0x1594c08;
        const DebugMenuTarget t = unbounded_int_menu_target(v);
        CHECK(t.known && t.slot3 == 1 && t.slot6 == 2 && t.object == &v);
    }
}

void section_failures() {
    {   // manager without a contract: the variable fails, the toggle is still built
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot334Objects o;
        const Slot334Result r = init_slot334_7fc530(o, creg, menu);
        CHECK(!r.ok && !r.registration.ok && r.registration.path == ConCmdRegisterPath::manager);
        CHECK(r.item.ok() && creg.pending.size() == 0);
    }
    {   // parent without a contract: the toggle registration fails visibly
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot334Objects o;
        const Slot334Result r = init_slot334_7fc530(o, creg, menu);
        CHECK(!r.ok && r.registration.ok && !r.item.registration.ok && menu.root.count == 0);
    }
    {   // second run on the same registries
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        Slot334Objects a, b;
        CHECK(init_slot334_7fc530(a, creg, menu).ok && init_slot334_7fc530(b, creg, menu).ok);
        CHECK(creg.pending.size() == 2 && menu.root.count == 2 && creg.atexit_log.size() == 5);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_worker();
    section_failures();
    std::puts("OK test_ctor_slot_334");
    return 0;
}
