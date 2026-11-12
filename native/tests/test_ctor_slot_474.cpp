// CPU-01-STARTUP-SLOT-474: fixture for startup constructor index 474 (0x344850): four 0x30 byte name holders (two classes), three bool console
// variables and one string console variable (the "areaLoader" page). Batch 8.
#include "l4_test_util.hpp"

#include "core/ctor_slot_474_344850.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot474Rva == 0x344850 && kSlot474Size == 1300 && kSlot474BoolFnTag == 0x15a9850);
    CHECK(kSlot474HolderVptrA == 0x15976b0 && kSlot474HolderVptrB == 0x159e1a0 && kSlot474HolderDtorA == 0x8ef00 && kSlot474HolderDtorB == 0x2fced0);
    for (std::size_t n = 0; n < kSlot474HolderCount; ++n) CHECK(std::strlen(kSlot474HolderNames[n]) == kSlot474HolderNameLengths[n]);
    for (std::size_t n = 0; n < kSlot474BoolCount; ++n) CHECK(std::strlen(kSlot474BoolNames[n]) == kSlot474BoolNameLengths[n]);
    CHECK(std::strlen(kSlot474StringName) == kSlot474StringNameLength && std::strlen(kSlot474StringInit) == kSlot474StringInitLength);
    CHECK(offsetof(Slot474Objects, holder1) == 0x2ef0a48 - kSlot474Obj0Rva);
    CHECK(offsetof(Slot474Objects, holder2) == 0x2ef0a78 - kSlot474Obj0Rva);
    CHECK(offsetof(Slot474Objects, holder3) == 0x2ef0aa8 - kSlot474Obj0Rva);
    CHECK(offsetof(Slot474Objects, bvar0) == 0x2ef0ad8 - kSlot474Obj0Rva);
    CHECK(offsetof(Slot474Objects, bvar1) == 0x2ef0ba0 - kSlot474Obj0Rva);
    CHECK(offsetof(Slot474Objects, svar) == 0x2ef0c68 - kSlot474Obj0Rva);
    CHECK(offsetof(Slot474Objects, bvar2) == 0x2ef0db0 - kSlot474Obj0Rva);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot474Objects o;
    poison(o);
    const Slot474Result r = init_slot474_344850(o, creg);
    CHECK(r.ok && r.svar.ok && r.svar_views_inert && r.svar_atexit_recorded);
    const NameHolder30* const hs[kSlot474HolderCount] = {&o.holder0, &o.holder1, &o.holder2, &o.holder3};
    for (std::size_t n = 0; n < kSlot474HolderCount; ++n) {
        CHECK(r.holder_ok[n] && hs[n]->tail == 0);
        CHECK(hs[n]->vptr == (n < 2 ? kSlot474HolderVptrA : kSlot474HolderVptrB));
        CHECK(same_view(hs[n]->name, kSlot474HolderNames[n], kSlot474HolderNameLengths[n]));
    }
    const ConCmdBoolVar* const bv[kSlot474BoolCount] = {&o.bvar0, &o.bvar1, &o.bvar2};
    for (std::size_t n = 0; n < kSlot474BoolCount; ++n) {
        CHECK(r.bvar[n].ok && r.bvar[n].registration.ok && r.bvar[n].views_inert && r.bvar[n].atexit_recorded);
        check_bool_with_description(*bv[n], kSlot474BoolFnTag, kSlot474BoolValues[n], "", 0);
        CHECK(same_view(bv[n]->base.name, kSlot474BoolNames[n], kSlot474BoolNameLengths[n]));
    }
    CHECK(o.svar.base.vptr == kStringVarFinalVptr && o.svar.base.registered == 1 && o.svar.base.flags == 1);
    CHECK(same_view(o.svar.base.name, kSlot474StringName, kSlot474StringNameLength));
    CHECK(same_view(o.svar.value, kSlot474StringInit, kSlot474StringInitLength) && same_view(o.svar.default_value, kSlot474StringInit, kSlot474StringInitLength));
    CHECK(creg.pending.size() == 4 && creg.pending.first() == &o.bvar0.base.pending);
    CHECK(o.bvar0.base.pending.next == &o.bvar1.base.pending && o.bvar1.base.pending.next == &o.svar.base.pending);
    CHECK(o.svar.base.pending.next == &o.bvar2.base.pending && o.bvar2.base.pending.next == &creg.pending.head);
    CHECK(creg.atexit_log.size() == 8);
    check_atexit(creg, 0, 0x8ef00, &o.holder0);
    check_atexit(creg, 1, 0x8ef00, &o.holder1);
    check_atexit(creg, 2, 0x2fced0, &o.holder2);
    check_atexit(creg, 3, 0x2fced0, &o.holder3);
    check_atexit(creg, 4, 0x20b0, &o.bvar0);
    check_atexit(creg, 5, 0x20b0, &o.bvar1);
    check_atexit(creg, 6, 0x23c90, &o.svar);
    check_atexit(creg, 7, 0x20b0, &o.bvar2);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;   // no insert contract: the first bool variable fails visibly; the holders are not registered variables
    Slot474Objects o;
    const Slot474Result r = init_slot474_344850(o, creg);
    CHECK(!r.ok && r.holder_ok[3] && !r.bvar[0].ok && !r.svar.ok && creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_474");
    return 0;
}
