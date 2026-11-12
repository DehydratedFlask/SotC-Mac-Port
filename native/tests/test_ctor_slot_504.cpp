// CPU-01-STARTUP-SLOT-504: fixture for startup constructor index 504 (0x23f4d0): the unbounded float variable
// `uiOverrideSafeFrame` (0x6aa00 through the family wrapper, functor tag 0x159ba68, atexit 0x20f0) followed by the
// bool variable `uiShowSafeFrame` (0x20af0, tag 0x159ba88, atexit 0x20b0). Batch 6.
#include "l4_test_util.hpp"

#include "core/ctor_slot_504_23f4d0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot504Rva == 0x23f4d0 && kSlot504Size == 562);
    CHECK(kSlot504FloatFnTag == 0x159ba68 && kSlot504BoolFnTag == 0x159ba88);
    CHECK(std::strlen(kSlot504FloatName) == kSlot504FloatNameLength && kSlot504FloatNameLength == 0x13);
    CHECK(std::strlen(kSlot504FloatDescription) == kSlot504FloatDescriptionLength && kSlot504FloatDescriptionLength == 0x65);
    CHECK(std::strlen(kSlot504BoolName) == kSlot504BoolNameLength && kSlot504BoolNameLength == 0xf);
    CHECK(std::strlen(kSlot504BoolDescription) == kSlot504BoolDescriptionLength && kSlot504BoolDescriptionLength == 0x45);
    CHECK(kSlot504BoolRva - kSlot504FloatRva == offsetof(Slot504Objects, bool_var));
    CHECK(is_float_functor_tag(kSlot504FloatFnTag) && !is_int_functor_tag(kSlot504FloatFnTag));
}

void section_alone() {
    ConCmdRegistry creg;
    Slot504Objects o;
    poison(o);
    const Slot504Result r = init_slot504_23f4d0(o, creg);
    CHECK(r.ok && r.refusal == HelperRefusal::none && r.float_ctor_ok && r.float_destroy_ok && r.float_views_inert &&
          r.float_atexit_recorded && r.float_registration.ok && r.bool_var.ok);

    const ConCmdRangedVar& f = o.float_var;
    CHECK(f.base.vptr == kRangedFloatFinalVptr && f.base.flags == 1 && f.base.registered == 1);
    CHECK(same_view(f.base.name, "uiOverrideSafeFrame", 0x13));
    CHECK(same_view(f.base.description, kSlot504FloatDescription, kSlot504FloatDescriptionLength));
    CHECK(f.value_ref == &f.value && f.value == 0 && f.default_value == 0);
    CHECK(f.min_engaged == 0 && f.max_engaged == 0 && f.min_value[0] == 0 && f.max_value[0] == 0);
    for (std::uint64_t a : f.args) CHECK(a == 0);
    CHECK(f.fn_payload == 0 && f.fn_aux == 0 && f.fn_vptr == kRangedFloatFnVtbl);

    const ConCmdBoolVar& b = o.bool_var;
    check_bool_with_description(b, kSlot504BoolFnTag, 0, kSlot504BoolDescription, kSlot504BoolDescriptionLength);
    CHECK(same_view(b.base.name, "uiShowSafeFrame", 0xf));

    CHECK(creg.pending.size() == 2 && creg.pending.first() == &f.base.pending);
    CHECK(f.base.pending.next == &b.base.pending && b.base.pending.next == &creg.pending.head);
    CHECK(creg.atexit_log.size() == 2);
    check_atexit(creg, 0, 0x20f0, &o.float_var);
    check_atexit(creg, 1, 0x20b0, &o.bool_var);
}

void section_failures() {
    {   // manager without an insert contract: both registrations fail (the original has no early exit)
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot504Objects o;
        const Slot504Result r = init_slot504_23f4d0(o, creg);
        CHECK(!r.ok && r.float_ctor_ok && !r.float_registration.ok && !r.bool_var.ok && creg.pending.size() == 0);
    }
    {   // an unadmitted float functor tag cannot be reached through the model, but the wrapper refuses it
        ConCmdRegistry creg;
        ConCmdRangedVar v;
        poison(v);
        RangedVarArgs args{};
        args.fn_vptr = 0x1234567;
        const SharedStr n{reinterpret_cast<const std::uint8_t*>("x"), 1, 0, nullptr};
        const HelperCtorResult c = helper_6aa00_unbounded_float(v, n, &v.value, 1, args, n);
        CHECK(!c.ok && c.refusal == HelperRefusal::unadmitted_arg_functor);
        args.fn_vptr = 0x15944a0;   // an int-family tag is not a float functor
        CHECK(!helper_6aa00_unbounded_float(v, n, &v.value, 1, args, n).ok);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_504");
    return 0;
}
