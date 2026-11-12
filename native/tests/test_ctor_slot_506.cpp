// CPU-01-STARTUP-BATCH-7: fixture for startup constructor index 506 (0x2340b0): unbounded int `drawTouch` (twin constructor 0x233ec0), unbounded int
// `checkTouch` (0x3a980, tag 0x159b6a0), a 0x26c10 group `Touch Manager` and one 0x26ea0 toggle `Draw`.
#include "l4_test_util.hpp"

#include "core/ctor_slot_506_2340b0.hpp"

namespace {
using namespace l4;

void poison_all(Slot506Objects& o) {
    poison(o.var0);
    poison(o.var1);
    poison(o.group);
    poison(o.item);
}

void check_unbounded(const ConCmdRangedVar& v, std::uint64_t vptr, std::uint64_t fn_class, const char* name, std::size_t length) {
    CHECK(v.base.vptr == vptr && v.base.flags == 1 && v.base.registered == 1);
    CHECK(same_view(v.base.name, name, length) && empty_view(v.base.description));
    CHECK(v.value_ref == &v.value && dword(v, 0xd0) == 0 && dword(v, 0x68) == 0);
    CHECK(v.min_engaged == 0 && v.max_engaged == 0);
    CHECK(v.fn_vptr == fn_class && v.fn_payload == 0 && v.fn_aux == 0);
}

void section_constants() {
    CHECK(kSlot506Rva == 0x2340b0 && kSlot506Size == 860);
    CHECK(std::strlen(kSlot506Var0Name) == kSlot506Var0NameLength && std::strlen(kSlot506Var1Name) == kSlot506Var1NameLength);
    CHECK(std::strlen(kSlot506GroupCaption) == kSlot506GroupCaptionLength && std::strlen(kSlot506ItemText) == kSlot506ItemTextLength);
    CHECK(!is_int_functor_tag(kSlot506Var0FnTag) && is_int_functor_tag(kSlot506Var1FnTag));
    CHECK(sizeof(ConCmdRangedVar) == 0xd8 && sizeof(DebugMenuItem) == 0xa8);
    CHECK(offsetof(Slot506Objects, var1) == kSlot506Var1Rva - kSlot506Var0Rva);
    CHECK(offsetof(Slot506Objects, group) == kSlot506GroupRva - kSlot506Var0Rva);
    CHECK(offsetof(Slot506Objects, item) == kSlot506ItemRva - kSlot506Var0Rva);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot506Objects o;
    poison_all(o);
    const Slot506Result r = init_slot506_2340b0(o, creg, menu);
    CHECK(r.ok && r.refusal == HelperRefusal::none);
    CHECK(r.var0.ok && r.var0.ctor_ok && r.var0.functor_destroy_ok && r.var0.views_inert && r.var0.atexit_recorded && r.var0.registration.ok);
    CHECK(r.var1.ok && r.var1.ctor_ok && r.var1.functor_destroy_ok && r.var1.views_inert && r.var1.atexit_recorded && r.var1.registration.ok);
    CHECK(r.group.ok() && r.item.ok);

    check_unbounded(o.var0, kUnboundedIntTwinFinalVptr, kSlot506Var0FnTag, kSlot506Var0Name, kSlot506Var0NameLength);
    check_unbounded(o.var1, kUnboundedIntFinalVptr, kIntFunctorClassTag, kSlot506Var1Name, kSlot506Var1NameLength);

    CHECK(o.group.kind == kMenuKindGroup && o.group.target == nullptr && o.group.flags == 0x30 && o.group.registered == 0);
    CHECK(empty_view(o.group.label));
    CHECK(same_view(o.group.text, kSlot506GroupCaption, kSlot506GroupCaptionLength));
    CHECK(o.item.target == &o.var0 && o.item.flags == 0x30 && o.item.kind == 2);
    CHECK(same_view(o.item.label, kSlot506GroupCaption, kSlot506GroupCaptionLength) && same_view(o.item.text, kSlot506ItemText, kSlot506ItemTextLength));
    CHECK(o.item.range_min == 1.0f && o.item.range_max == 10.0f);   // slot 3 = slot 6 = 1: the {1.0, 10.0} default stays
    CHECK(menu.root.count == 2);

    CHECK(creg.pending.size() == 2);
    CHECK(creg.pending.first() == &o.var0.base.pending && o.var0.base.pending.next == &o.var1.base.pending);
    // var0, var1, [root-list destructor on the first item], group, toggle.
    CHECK(creg.atexit_log.size() == 5);
    check_atexit(creg, 0, 0x230d60, &o.var0);
    check_atexit(creg, 1, 0x18a60, &o.var1);
    check_atexit(creg, 2, 0x27280, &menu.root);
    check_atexit(creg, 3, 0x272d0, &o.group);
    check_atexit(creg, 4, 0x272d0, &o.item);
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot506Objects o;
        const Slot506Result r = init_slot506_2340b0(o, creg, menu);
        CHECK(!r.ok && !r.var0.ok && !r.var1.ok && !r.group.ok() && !r.item.ok);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the group registration fails visibly; the toggle is not reached.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot506Objects o;
        const Slot506Result r = init_slot506_2340b0(o, creg, menu);
        CHECK(!r.ok && r.var0.ok && r.var1.ok && !r.group.ok() && !r.item.ok);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_506");
    return 0;
}
