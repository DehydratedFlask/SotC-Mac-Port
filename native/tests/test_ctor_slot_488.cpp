// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): fixture for startup constructor index 488 (0x2ba5d0): ranged int variable
// `SM_showActiveState` (0..2, 0xc580 through the family wrapper, tag 0x15a0a18), a 0x30 B static holder, and the unbounded int
// variables `SM_showActiveStateMinTime` / `SM_showActiveStateMaxTime` (0x3a980 through the family wrapper).
#include "l4_test_util.hpp"

#include "core/ctor_slot_488_2ba5d0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot488Rva == 0x2ba5d0 && kSlot488Size == 1028 && kSlot488IntFnTag == 0x15a0a18);
    CHECK(std::strlen(kSlot488Var0Name) == kSlot488Var0NameLength && kSlot488Var0NameLength == 0x12);
    CHECK(std::strlen(kSlot488HolderText) == kSlot488HolderTextLength && kSlot488HolderTextLength == 0x12);
    CHECK(std::strlen(kSlot488Var1Name) == kSlot488Var1NameLength && kSlot488Var1NameLength == 0x19);
    CHECK(std::strlen(kSlot488Var2Name) == kSlot488Var2NameLength && kSlot488Var2NameLength == 0x19);
    CHECK(sizeof(ConCmdRangedVar) == 0xd8 && sizeof(Slot488Holder) == 0x30);
    CHECK(offsetof(Slot488Objects, holder) == kSlot488HolderRva - kSlot488Var0Rva);
    CHECK(offsetof(Slot488Objects, var1) == kSlot488Var1Rva - kSlot488Var0Rva);
    CHECK(offsetof(Slot488Objects, var2) == kSlot488Var2Rva - kSlot488Var0Rva);
    CHECK(is_int_functor_tag(kSlot488IntFnTag));
}

void check_site(const HelperIntSiteResult& s) {
    CHECK(s.ok && s.refusal == HelperRefusal::none && s.ctor_ok && s.functor_destroy_ok && s.views_inert &&
          s.atexit_recorded && s.registration.ok && s.registration.path == ConCmdRegisterPath::pending_list);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot488Objects o;
    poison(o);
    const Slot488Result r = init_slot488_2ba5d0(o, creg);
    CHECK(r.ok && r.holder_stored && r.holder_views_inert && r.holder_atexit_recorded);
    check_site(r.var0);
    check_site(r.var1);
    check_site(r.var2);

    const ConCmdRangedVar& v0 = o.var0;
    CHECK(v0.base.vptr == kRangedIntFinalVptr && v0.base.flags == 1 && v0.base.registered == 1);
    CHECK(same_view(v0.base.name, kSlot488Var0Name, kSlot488Var0NameLength) && empty_view(v0.base.description));
    CHECK(v0.value_ref == &o.var0.value && dword(v0, 0xd0) == 0 && dword(v0, 0x68) == 0);
    CHECK(dword(v0, 0x6c) == 0 && v0.min_engaged == 1 && dword(v0, 0x74) == 2 && v0.max_engaged == 1);
    CHECK(v0.fn_vptr == kRangedIntFnVtbl && v0.fn_payload == 0 && v0.fn_aux == 0);

    const ConCmdRangedVar* ub[2] = {&o.var1, &o.var2};
    const char* names[2] = {kSlot488Var1Name, kSlot488Var2Name};
    for (int i = 0; i < 2; ++i) {
        const ConCmdRangedVar& p = *ub[i];
        CHECK(p.base.vptr == 0x1594c08 && p.base.registered == 1 && p.base.flags == 1);
        CHECK(same_view(p.base.name, names[i], 0x19) && empty_view(p.base.description));
        CHECK(p.value_ref == &p.value && p.value == 0 && p.default_value == 0);
        CHECK(p.min_engaged == 0 && p.max_engaged == 0 && p.fn_vptr == 0x15944a0 && p.fn_payload == 0 && p.fn_aux == 0);
        for (std::uint64_t q : p.args) CHECK(q == 0);
    }

    const Slot488Holder& h = o.holder;
    CHECK(h.vptr == kSlot488HolderVtbl && h.text == kSlot488HolderText && h.text_length == 0x12);
    for (std::uint8_t b : h.zero) CHECK(b == 0);

    CHECK(creg.pending.size() == 3 && creg.pending.first() == &o.var0.base.pending);
    CHECK(o.var0.base.pending.next == &o.var1.base.pending && o.var1.base.pending.next == &o.var2.base.pending &&
          o.var2.base.pending.next == &creg.pending.head);
    CHECK(creg.atexit_log.size() == 4);
    check_atexit(creg, 0, 0x18a60, &o.var0);
    check_atexit(creg, 1, 0x2b5050, &o.holder);
    check_atexit(creg, 2, 0x18a60, &o.var1);
    check_atexit(creg, 3, 0x18a60, &o.var2);
}

void section_tag_refusal() {
    // A tag that is not one of the 37 int-family tables is refused by both wrappers; nothing is written or queued.
    ConCmdRegistry creg;
    ConCmdRangedVar v;
    poison(v);
    const HelperIntSiteSpec bad_r{kSlot488Var0Name, 0x12, "", 0, 0, 2, 1, 0x1234567, 0, 0};
    const HelperIntSiteResult r = helper_init_ranged_int_var_site(v, bad_r, creg);
    CHECK(!r.ok && !r.ctor_ok && r.refusal == HelperRefusal::unadmitted_arg_functor);
    const HelperIntSiteSpec bad_u{kSlot488Var1Name, 0x19, "", 0, 0, 0, 1, 0x1234567, 0, 0};
    const HelperIntSiteResult u = helper_init_unbounded_int_var_site(v, bad_u, creg);
    CHECK(!u.ok && !u.ctor_ok && u.refusal == HelperRefusal::unadmitted_arg_functor);
    CHECK(creg.pending.size() == 0 && creg.atexit_log.size() == 0);
    const auto* b = reinterpret_cast<const std::uint8_t*>(&v);
    for (std::size_t i = 8; i < sizeof v; ++i) CHECK(b[i] == kPoison);
}

void section_failures() {
    // No registration contract: the first variable fails visibly and the holder / later variables are never reached.
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot488Objects o;
    poison(o);
    const Slot488Result r = init_slot488_2ba5d0(o, creg);
    CHECK(!r.ok && !r.var0.ok && !r.holder_stored && !r.var1.ctor_ok && !r.var2.ctor_ok);
    CHECK(creg.pending.size() == 0);
    CHECK(o.holder.vptr == 0xa5a5a5a5a5a5a5a5ull);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_tag_refusal();
    section_failures();
    std::puts("OK test_ctor_slot_488");
    return 0;
}
