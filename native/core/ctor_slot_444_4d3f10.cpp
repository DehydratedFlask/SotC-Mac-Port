#include "ctor_slot_444_4d3f10.hpp"

namespace sotc {

Slot444Result init_slot444_4d3f10(Slot444Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot444Result out{};
    ConCmdBoolVar* const bvars[kSlot444BoolCount] = {&objects.bvar0};
    ConCmdRangedVar* const fvars[kSlot444FloatCount] = {&objects.fvar0, &objects.fvar1, &objects.fvar2, &objects.fvar3};
    for (std::size_t n = 0; n < 4; ++n) {
        const HelperFloatSiteSpec spec{kSlot444FloatNames[n], kSlot444FloatNameLengths[n], kSlot444FloatDescriptions[n], kSlot444FloatDescriptionLengths[n], 1,
                                       kSlot444FloatFnTag, kSlot444FloatValueBits[n], true};
        out.fvar[n] = helper_init_unbounded_float_var_site(*fvars[n], spec, concmd);
        if (out.refusal == HelperRefusal::none) out.refusal = out.fvar[n].refusal;
        if (!out.fvar[n].ctor_ok) return out;
    }
    for (std::size_t n = 0; n < 1; ++n) {
        out.bvar[n] = lane_m6::init_bool_var_literal(*bvars[n], kSlot444BoolNames[n], kSlot444BoolNameLengths[n], kSlot444BoolDescriptions[n],
                                                     kSlot444BoolDescriptionLengths[n], kSlot444BoolFnTag, concmd, kSlot444BoolValues[n], kSlot444BoolValues[n]);
        if (!out.bvar[n].ok) return out;
    }
    out.ok = true;
    for (std::size_t n = 0; n < kSlot444BoolCount; ++n) out.ok = out.ok && out.bvar[n].ok;
    for (std::size_t n = 0; n < kSlot444FloatCount; ++n) out.ok = out.ok && out.fvar[n].ok;
    return out;
}

}  // namespace sotc
