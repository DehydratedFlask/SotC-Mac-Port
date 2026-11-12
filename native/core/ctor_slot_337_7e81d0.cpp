#include "ctor_slot_337_7e81d0.hpp"

namespace sotc {

namespace {

// 0x7e886b..0x7e88db: the 0x30 byte holder is built from a stack view (view copy at +8, zero tail at +0x28, vptr = 0x15976a0 + 0x10), the
// temporary view is released by an exact no-op f590 and __cxa_atexit(0x8ef00, holder) is recorded.
bool build_holder(NameHolder30& holder, ConCmdRegistry& registry) noexcept {
    const SharedStr view = l2::measured_view(kSlot337HolderName, kSlot337HolderNameLength);
    holder.name = view;
    holder.tail = 0;
    holder.vptr = kSlot337HolderVptr;
    const bool inert = release_inert_view_f590(view);
    const bool recorded = registry.atexit_log.record(kSlot337HolderDtorRva, &holder);
    return inert && recorded;
}

}  // namespace

Slot337Result init_slot337_7e81d0(Slot337Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot337Result out{};
    ConCmdBoolVar* const bvars[kSlot337BoolCount] = {&objects.bvar0, &objects.bvar1};
    ConCmdRangedVar* const fvars[kSlot337FloatCount] = {&objects.fvar0, &objects.fvar1, &objects.fvar2, &objects.fvar3, &objects.fvar4};
    for (std::size_t n = 0; n < kSlot337BoolCount; ++n) {
        out.bvar[n] = lane_m6::init_bool_var_literal(*bvars[n], kSlot337BoolNames[n], kSlot337BoolNameLengths[n], "", 0,
                                                     kSlot337BoolFnTag, concmd, kSlot337BoolValues[n], kSlot337BoolValues[n]);
        if (!out.bvar[n].ok) return out;
    }
    for (std::size_t n = 0; n < kSlot337FloatCount; ++n) {
        const HelperFloatSiteSpec spec{kSlot337FloatNames[n], kSlot337FloatNameLengths[n], "", 0, 1,
                                       kSlot337FloatFnTag, kSlot337FloatValueBits[n], true};
        out.fvar[n] = helper_init_unbounded_float_var_site(*fvars[n], spec, concmd);
        if (out.refusal == HelperRefusal::none) out.refusal = out.fvar[n].refusal;
        if (!out.fvar[n].ctor_ok) return out;
    }
    out.holder_ok = build_holder(objects.holder, concmd);
    out.ok = out.holder_ok;
    for (std::size_t n = 0; n < kSlot337BoolCount; ++n) out.ok = out.ok && out.bvar[n].ok;
    for (std::size_t n = 0; n < kSlot337FloatCount; ++n) out.ok = out.ok && out.fvar[n].ok;
    return out;
}

}  // namespace sotc
