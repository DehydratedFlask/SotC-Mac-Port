#include "ctor_slot_397_64d760.hpp"

namespace sotc {

Slot397Result init_slot397_64d760(Slot397Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot397Result out{};
    // v0: `playerAccelerationScalar30Hz` (float_early)
    const HelperFloatSiteSpec spec0{kSlot397V0Name, kSlot397V0NameLength, "", kSlot397V0DescriptionLength, 1,
                                       kSlot397V0FnTag, kSlot397V0Value, true};
    out.v0 = helper_init_unbounded_float_var_site(objects.v0, spec0, concmd);
    if (out.refusal == HelperRefusal::none) out.refusal = out.v0.refusal;
    if (!out.v0.ctor_ok) return out;
    // v1: `playerAccelerationScalarEasyMode30Hz` (float_early)
    const HelperFloatSiteSpec spec1{kSlot397V1Name, kSlot397V1NameLength, "", kSlot397V1DescriptionLength, 1,
                                       kSlot397V1FnTag, kSlot397V1Value, true};
    out.v1 = helper_init_unbounded_float_var_site(objects.v1, spec1, concmd);
    if (out.refusal == HelperRefusal::none) out.refusal = out.v1.refusal;
    if (!out.v1.ctor_ok) return out;
    // v2: `playerAccelerationScalar60Hz` (float_early)
    const HelperFloatSiteSpec spec2{kSlot397V2Name, kSlot397V2NameLength, "", kSlot397V2DescriptionLength, 1,
                                       kSlot397V2FnTag, kSlot397V2Value, true};
    out.v2 = helper_init_unbounded_float_var_site(objects.v2, spec2, concmd);
    if (out.refusal == HelperRefusal::none) out.refusal = out.v2.refusal;
    if (!out.v2.ctor_ok) return out;
    // v3: `playerAccelerationScalarEasyMode60Hz` (float_early)
    const HelperFloatSiteSpec spec3{kSlot397V3Name, kSlot397V3NameLength, "", kSlot397V3DescriptionLength, 1,
                                       kSlot397V3FnTag, kSlot397V3Value, true};
    out.v3 = helper_init_unbounded_float_var_site(objects.v3, spec3, concmd);
    if (out.refusal == HelperRefusal::none) out.refusal = out.v3.refusal;
    if (!out.v3.ctor_ok) return out;
    out.ok = out.v0.ok && out.v1.ok && out.v2.ok && out.v3.ok;
    return out;
}

}  // namespace sotc
