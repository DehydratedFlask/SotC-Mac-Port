#include "ctor_slot_450_4af4b0.hpp"

namespace sotc {

Slot450Result init_slot450_4af4b0(Slot450Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot450Result out{};
    // v0: `uiLogoFadeInFromWhiteTime` (float_early)
    const HelperFloatSiteSpec spec0{kSlot450V0Name, kSlot450V0NameLength, "", kSlot450V0DescriptionLength, 1,
                                       kSlot450V0FnTag, kSlot450V0Value, true};
    out.v0 = helper_init_unbounded_float_var_site(objects.v0, spec0, concmd);
    if (out.refusal == HelperRefusal::none) out.refusal = out.v0.refusal;
    if (!out.v0.ctor_ok) return out;
    // v1: `uiLogoFadeOutTime` (float_early)
    const HelperFloatSiteSpec spec1{kSlot450V1Name, kSlot450V1NameLength, "", kSlot450V1DescriptionLength, 1,
                                       kSlot450V1FnTag, kSlot450V1Value, true};
    out.v1 = helper_init_unbounded_float_var_site(objects.v1, spec1, concmd);
    if (out.refusal == HelperRefusal::none) out.refusal = out.v1.refusal;
    if (!out.v1.ctor_ok) return out;
    // v2: `uiMainMenuStartDelay` (float_early)
    const HelperFloatSiteSpec spec2{kSlot450V2Name, kSlot450V2NameLength, "", kSlot450V2DescriptionLength, 1,
                                       kSlot450V2FnTag, kSlot450V2Value, true};
    out.v2 = helper_init_unbounded_float_var_site(objects.v2, spec2, concmd);
    if (out.refusal == HelperRefusal::none) out.refusal = out.v2.refusal;
    if (!out.v2.ctor_ok) return out;
    out.ok = out.v0.ok && out.v1.ok && out.v2.ok;
    return out;
}

}  // namespace sotc
