#include "ctor_slot_521_17bf50.hpp"

namespace sotc {

Slot521Result init_slot521_17bf50(Slot521Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot521Result out{};
    ConCmdBoolVar* const bvars[kSlot521BoolCount] = {&objects.bvar0, &objects.bvar1, &objects.bvar2, &objects.bvar3, &objects.bvar4, &objects.bvar5};
    ConCmdRangedVar* const fvars[kSlot521FloatCount] = {&objects.fvar0};
    for (std::size_t n = 0; n < 1; ++n) {
        const HelperFloatSiteSpec spec{kSlot521FloatNames[n], kSlot521FloatNameLengths[n], kSlot521FloatDescriptions[n], kSlot521FloatDescriptionLengths[n], 1,
                                       kSlot521FloatFnTag, kSlot521FloatValueBits[n], false};
        out.fvar[n] = helper_init_unbounded_float_var_site(*fvars[n], spec, concmd);
        if (out.refusal == HelperRefusal::none) out.refusal = out.fvar[n].refusal;
        if (!out.fvar[n].ctor_ok) return out;
    }
    for (std::size_t n = 0; n < 6; ++n) {
        out.bvar[n] = lane_m6::init_bool_var_literal(*bvars[n], kSlot521BoolNames[n], kSlot521BoolNameLengths[n], kSlot521BoolDescriptions[n],
                                                     kSlot521BoolDescriptionLengths[n], kSlot521BoolFnTag, concmd, kSlot521BoolValues[n], kSlot521BoolValues[n]);
        if (!out.bvar[n].ok) return out;
    }
    out.ok = true;
    for (std::size_t n = 0; n < kSlot521BoolCount; ++n) out.ok = out.ok && out.bvar[n].ok;
    for (std::size_t n = 0; n < kSlot521FloatCount; ++n) out.ok = out.ok && out.fvar[n].ok;
    return out;
}

}  // namespace sotc
