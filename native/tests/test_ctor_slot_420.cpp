// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): fixture for startup constructor index 420 (0x56f6e0): bool `sndx_debugOcclusionWork`, ranged int
// `sndx_occlusionWorkIndex` (0..0x1f, 0xc580 family wrapper, tag 0x15caf38), a 0x26b60 separator (needs the allocator hook), a 0x26ea0 toggle,
// a 0x27050 slider targeting the int variable, and an 8 byte cell with atexit 0x56ef80.
#include "l4_test_util.hpp"

#include "core/ctor_slot_420_56f6e0.hpp"

namespace {
using namespace l4;

float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

void section_constants() {
    CHECK(kSlot420Rva == 0x56f6e0 && kSlot420Size == 1006);
    CHECK(std::strlen(kSlot420Var0Name) == kSlot420Var0NameLength && kSlot420Var0NameLength == 0x17);
    CHECK(std::strlen(kSlot420Var1Name) == kSlot420Var1NameLength && kSlot420Var1NameLength == 0x17);
    CHECK(std::strlen(kSlot420Parent) == kSlot420ParentLength && kSlot420ParentLength == 5);
    CHECK(std::strlen(kSlot420Item1Text) == kSlot420Item1TextLength && kSlot420Item1TextLength == 0x14);
    CHECK(std::strlen(kSlot420Item2Text) == kSlot420Item2TextLength && kSlot420Item2TextLength == 0xf);
    CHECK(is_int_functor_tag(kSlot420IntFnTag) && !is_float_functor_tag(kSlot420IntFnTag));
    CHECK(sizeof(ConCmdBoolVar) == 0xc8 && sizeof(ConCmdRangedVar) == 0xd8 && sizeof(DebugMenuItem) == 0xa8);
    CHECK(offsetof(Slot420Objects, var1) == kSlot420Var1Rva - kSlot420Var0Rva);
    CHECK(offsetof(Slot420Objects, sep) == kSlot420SepRva - kSlot420Var0Rva);
    CHECK(offsetof(Slot420Objects, item1) == kSlot420Item1Rva - kSlot420Var0Rva);
    CHECK(offsetof(Slot420Objects, item2) == kSlot420Item2Rva - kSlot420Var0Rva);
    CHECK(offsetof(Slot420Objects, holder) == kSlot420HolderRva - kSlot420Var0Rva);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot420Objects o;
    // Poison the members individually (the object owns the separator allocation, so it is not memset as a whole).
    poison(o.var0);
    poison(o.var1);
    poison(o.sep);
    poison(o.item1);
    poison(o.item2);
    const Slot420Result r = init_slot420_56f6e0(o, creg, menu, default_slot420_hooks());
    CHECK(r.ok && r.refusal == HelperRefusal::none);
    CHECK(r.var0.ok && r.var0.registration.ok && r.var0.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.var1.ok && r.var1.refusal == HelperRefusal::none && r.var1.ctor_ok && r.var1.functor_destroy_ok &&
          r.var1.views_inert && r.var1.atexit_recorded && r.var1.registration.ok);
    CHECK(r.sep.ok && r.sep.ctor_ok && r.sep.registration_ok && r.sep.views_inert && r.sep.atexit_recorded);
    CHECK(r.item1.ctor_ok && r.item1.views_inert && r.item1.atexit_recorded && r.item1.ok);
    CHECK(r.item2.ctor_ok && r.item2.views_inert && r.item2.atexit_recorded && r.item2.ok);
    CHECK(r.holder_stored && r.holder_atexit_recorded);

    check_bool_with_description(o.var0, kSlot420BoolFnTag, 0, "", 0);
    CHECK(same_view(o.var0.base.name, kSlot420Var0Name, kSlot420Var0NameLength));
    const ConCmdRangedVar& v = o.var1;
    CHECK(v.base.vptr == kRangedIntFinalVptr && v.base.flags == 1 && v.base.registered == 1);
    CHECK(same_view(v.base.name, kSlot420Var1Name, kSlot420Var1NameLength) && empty_view(v.base.description));
    CHECK(v.value_ref == &o.var1.value && dword(v, 0xd0) == 0 && dword(v, 0x68) == 0);
    CHECK(dword(v, 0x6c) == 0 && v.min_engaged == 1 && dword(v, 0x74) == 0x1f && v.max_engaged == 1);
    CHECK(v.fn_vptr == kRangedIntFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);

    // 0x26b60 separator: kind 3, parent `Sound`, text "MenuSeparator" (allocated), flags 0x30
    CHECK(o.sep.kind == kMenuKindSeparator && o.sep.flags == 0x30 && o.sep.registered == 0);
    CHECK(same_view(o.sep.label, kSlot420Parent, kSlot420ParentLength));
    CHECK(o.sep.text.ctl != nullptr && o.owned_alloc.deallocate != nullptr);
    CHECK(o.sep.text.size == 13 && std::memcmp(o.sep.text.data, kMenuSeparatorText, 14) == 0 && o.sep.text.aux == 15 &&
          *o.sep.text.ctl == 1);
    // 0x26ea0 toggle: bool class slot 6 is 0, so the {1.0, 10.0} default range stays
    CHECK(o.item1.target == &o.var0 && o.item1.flags == 0x30 && o.item1.kind == 2 && o.item1.registered == 0);
    CHECK(same_view(o.item1.label, kSlot420Parent, kSlot420ParentLength));
    CHECK(same_view(o.item1.text, kSlot420Item1Text, kSlot420Item1TextLength));
    CHECK(o.item1.range_min == 1.0f && o.item1.range_max == 10.0f);
    // 0x27050 slider: the explicit range, target = the int variable
    CHECK(o.item2.target == &o.var1 && o.item2.flags == 0x30 && o.item2.kind == 2 && o.item2.registered == 0);
    CHECK(same_view(o.item2.label, kSlot420Parent, kSlot420ParentLength));
    CHECK(same_view(o.item2.text, kSlot420Item2Text, kSlot420Item2TextLength));
    CHECK(o.item2.range_min == bits_to_float(kSlot420Item2MinBits) && o.item2.range_max == bits_to_float(kSlot420Item2MaxBits));
    CHECK(menu.root.count == 3 && menu.root.link.next == &o.sep.link && o.sep.link.next == &o.item1.link &&
          o.item1.link.next == &o.item2.link);
    CHECK(o.holder == 0);

    CHECK(creg.pending.size() == 2 && creg.pending.first() == &o.var0.base.pending);
    CHECK(o.var0.base.pending.next == &o.var1.base.pending && o.var1.base.pending.next == &creg.pending.head);
    // var0, var1, [root-list destructor on the first item], separator, toggle, slider, holder.
    CHECK(creg.atexit_log.size() == 7);
    check_atexit(creg, 0, 0x20b0, &o.var0);
    check_atexit(creg, 1, 0x18a60, &o.var1);
    check_atexit(creg, 2, 0x27280, &menu.root);
    check_atexit(creg, 3, 0x272d0, &o.sep);
    check_atexit(creg, 4, 0x272d0, &o.item1);
    check_atexit(creg, 5, 0x272d0, &o.item2);
    check_atexit(creg, 6, 0x56ef80, &o.holder);
}

void section_refusal() {
    // A tag outside the int family is refused by the 0xc580 wrapper; nothing is written and the menu is never reached.
    ConCmdRegistry creg;
    ConCmdRangedVar v;
    poison(v);
    const HelperIntSiteSpec bad{kSlot420Var1Name, kSlot420Var1NameLength, "", 0, 0, 0x1f, 1, 0x1234567, 0, 0};
    const HelperIntSiteResult r = helper_init_ranged_int_var_site(v, bad, creg);
    CHECK(!r.ok && !r.ctor_ok && r.refusal == HelperRefusal::unadmitted_arg_functor);
    CHECK(creg.pending.size() == 0 && creg.atexit_log.size() == 0);
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot420Objects o;
        const Slot420Result r = init_slot420_56f6e0(o, creg, menu, default_slot420_hooks());
        CHECK(!r.ok && !r.var0.ok && !r.var1.ctor_ok && !r.sep.ctor_ok && !r.item1.ctor_ok && !r.item2.ctor_ok);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0 && !r.holder_stored);
    }
    {
        // No allocator contract for 0x26b60: the separator fails visibly, nothing after it is built, and no storage leaks.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        Slot420Objects o;
        Slot420Hooks none{};
        const Slot420Result r = init_slot420_56f6e0(o, creg, menu, none);
        CHECK(!r.ok && r.var0.ok && r.var1.ok && !r.sep.ctor_ok && !r.sep.ok && !r.item1.ctor_ok && !r.item2.ctor_ok);
        CHECK(menu.root.count == 0 && !r.holder_stored && o.owned_alloc.deallocate == nullptr);
        CHECK(creg.pending.size() == 2 && creg.atexit_log.size() == 2);
    }
    {
        // Menu parent without an insert contract: the separator registration fails visibly; the toggle is not reached.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot420Objects o;
        const Slot420Result r = init_slot420_56f6e0(o, creg, menu, default_slot420_hooks());
        CHECK(!r.ok && r.var1.ok && r.sep.ctor_ok && !r.sep.registration_ok && !r.item1.ctor_ok);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_refusal();
    section_failures();
    std::puts("OK test_ctor_slot_420");
    return 0;
}
