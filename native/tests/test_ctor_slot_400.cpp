// CPU-01-STARTUP-SLOT-400: fixture for startup constructor index 400 (0x62b100): 5 bool console variables with 0x26db0 toggles. Batch 8.
#include "l4_test_util.hpp"

#include "core/ctor_slot_400_62b100.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot400Rva == 0x62b100 && kSlot400Size == 1624 && kSlot400BoolFnTag == 0x15cc448);
    for (std::size_t n = 0; n < kSlot400BoolCount; ++n) {
        CHECK(std::strlen(kSlot400BoolNames[n]) == kSlot400BoolNameLengths[n]);
        CHECK(std::strlen(kSlot400Parents[n]) == kSlot400ParentLengths[n]);
    }
    CHECK(offsetof(Slot400Objects, bvar0) == 0x300e048 - kSlot400Obj0Rva);
    CHECK(offsetof(Slot400Objects, bvar1) == 0x300e110 - kSlot400Obj0Rva);
    CHECK(offsetof(Slot400Objects, bvar2) == 0x300e1d8 - kSlot400Obj0Rva);
    CHECK(offsetof(Slot400Objects, bitem0) == 0x300e2a0 - kSlot400Obj0Rva);
    CHECK(offsetof(Slot400Objects, bitem1) == 0x300e348 - kSlot400Obj0Rva);
    CHECK(offsetof(Slot400Objects, bitem2) == 0x300e3f0 - kSlot400Obj0Rva);
    CHECK(offsetof(Slot400Objects, bvar3) == 0x300e498 - kSlot400Obj0Rva);
    CHECK(offsetof(Slot400Objects, bvar4) == 0x300e560 - kSlot400Obj0Rva);
    CHECK(offsetof(Slot400Objects, bitem3) == 0x300e628 - kSlot400Obj0Rva);
    CHECK(offsetof(Slot400Objects, bitem4) == 0x300e6d0 - kSlot400Obj0Rva);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot400Objects o;
    poison(o);
    const Slot400Result r = init_slot400_62b100(o, creg, menu);
    CHECK(r.ok);
    const ConCmdBoolVar* const bvars[kSlot400BoolCount] = {&o.bvar0, &o.bvar1, &o.bvar2, &o.bvar3, &o.bvar4};
    const DebugMenuItem* const bitems[kSlot400BoolCount] = {&o.bitem0, &o.bitem1, &o.bitem2, &o.bitem3, &o.bitem4};
    for (std::size_t n = 0; n < kSlot400BoolCount; ++n) {
        CHECK(r.bvar[n].ok && r.bvar[n].registration.ok && r.bitem[n].ok && r.bitem[n].ctor_ok && r.bitem[n].views_inert && r.bitem[n].atexit_recorded);
        check_bool_with_description(*bvars[n], kSlot400BoolFnTag, kSlot400BoolValues[n], "", 0);
        CHECK(same_view(bvars[n]->base.name, kSlot400BoolNames[n], kSlot400BoolNameLengths[n]));
        const DebugMenuItem& it = *bitems[n];
        CHECK(it.target == bvars[n] && it.flags == 0x30 && it.kind == 2 && it.registered == 0);
        CHECK(same_view(it.label, kSlot400Parents[n], kSlot400ParentLengths[n]));
        CHECK(it.range_min == 1.0f && it.range_max == 10.0f);   // bool class: the gate never narrows
        CHECK(same_view(it.text, kSlot400BoolNames[n], kSlot400BoolNameLengths[n]));   // 0x26db0: the variable's own name
    }
    CHECK(menu.root.count == 5 && menu.root.link.next == &o.bitem0.link);
    CHECK(creg.pending.size() == 5 && creg.pending.first() == &o.bvar0.base.pending);
    CHECK(creg.atexit_log.size() == 11);
    check_atexit(creg, 0, 0x20b0, &o.bvar0);
    check_atexit(creg, 1, 0x20b0, &o.bvar1);
    check_atexit(creg, 2, 0x20b0, &o.bvar2);
    check_atexit(creg, 3, 0x27280, &menu.root);
    check_atexit(creg, 4, 0x272d0, &o.bitem0);
    check_atexit(creg, 5, 0x272d0, &o.bitem1);
    check_atexit(creg, 6, 0x272d0, &o.bitem2);
    check_atexit(creg, 7, 0x20b0, &o.bvar3);
    check_atexit(creg, 8, 0x20b0, &o.bvar4);
    check_atexit(creg, 9, 0x272d0, &o.bitem3);
    check_atexit(creg, 10, 0x272d0, &o.bitem4);
}

void section_failures() {
    {   // First variable registration fails: nothing after it is reached.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot400Objects o;
        const Slot400Result r = init_slot400_62b100(o, creg, menu);
        CHECK(!r.ok && !r.bvar[0].ok && !r.bitem[0].ctor_ok);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {   // Menu parent without an insert contract: the first toggle registration fails visibly; the later sites are not reached.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot400Objects o;
        const Slot400Result r = init_slot400_62b100(o, creg, menu);
        CHECK(!r.ok && r.bvar[0].ok && r.bvar[2].ok && !r.bitem[0].ok && !r.bvar[3].ok);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_400");
    return 0;
}
