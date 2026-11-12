#include "ctor_slot_504_23f4d0.hpp"

namespace sotc {

Slot504Result init_slot504_23f4d0(Slot504Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot504Result out{};
    const SharedStr fname = lane_m4::borrowed_view_of(kSlot504FloatName, kSlot504FloatNameLength);
    const SharedStr fdesc = lane_m4::borrowed_view_of(kSlot504FloatDescription, kSlot504FloatDescriptionLength);
    RangedVarArgs args{};                              // four zero qwords, payload 0, aux 0
    args.fn_vptr = kSlot504FloatFnTag;                 // the caller's REAL functor tag

    // 0x23f581: [obj] = 0x15942a0 + 0x10 ; 0x23f59e: call 0x6aa00 (family wrapper)
    ConCmdRangedVar& fv = objects.float_var;
    fv.base.vptr = kRangedFloatFinalVptr;
    const HelperCtorResult ctor = helper_6aa00_unbounded_float(fv, fname, &fv.value, 1, args, fdesc);
    out.refusal = ctor.refusal;
    out.float_ctor_ok = ctor.ok;
    if (!out.float_ctor_ok) return out;

    // 0x23f5a3..0x23f5f1: call [argfn+0x18] ; vptr ; value/default = 0.0f ; 0x20d60 ; f590(desc) ; f590(name) ; atexit 0x20f0
    out.float_destroy_ok = ctor.arg_destroy_ok;
    fv.base.vptr = kRangedFloatFinalVptr;
    fv.value = 0;
    fv.default_value = 0;
    out.float_registration = register_concmd_20d60(fv.base, concmd);
    const bool d_inert = release_inert_view_f590(fdesc);
    const bool n_inert = release_inert_view_f590(fname);
    out.float_views_inert = d_inert && n_inert;
    out.float_atexit_recorded = concmd.atexit_log.record(kRangedFloatDtorRva, &fv);

    // 0x23f5f6..0x23f6d4: 0x20af0 ; stores ; vptr ; value/default = 0 ; 0x20d60 ; f590(desc) ; f590(name) ; atexit 0x20b0
    const SharedStr bname = lane_m4::borrowed_view_of(kSlot504BoolName, kSlot504BoolNameLength);
    const SharedStr bdesc = lane_m4::borrowed_view_of(kSlot504BoolDescription, kSlot504BoolDescriptionLength);
    out.bool_var = lane_m4::init_bool_var_late_release(objects.bool_var, bname, bdesc, kSlot504BoolFnTag, concmd, 0, 0);

    out.ok = out.float_ctor_ok && out.float_destroy_ok && out.float_registration.ok && out.float_views_inert &&
             out.float_atexit_recorded && out.bool_var.ok;
    return out;
}

}  // namespace sotc
