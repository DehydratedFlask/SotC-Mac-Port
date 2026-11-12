#include "ctor_slot_442_4e3760.hpp"

namespace sotc {

namespace {

SharedStr view_of(const char* text, std::size_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(length), 0, nullptr};
}

}  // namespace

Slot442Result init_slot442_4e3760(Slot442Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot442Result out{};
    // v0: `cp10_testUI_testGripFillPercent` (bool_late)
    out.v0 = lane_m4::init_bool_var_late_release(objects.v0, view_of(kSlot442V0Name, kSlot442V0NameLength), view_of(kSlot442V0Description, kSlot442V0DescriptionLength), kSlot442V0FnTag, concmd, static_cast<std::uint8_t>(kSlot442V0Value), static_cast<std::uint8_t>(kSlot442V0Value));
    // v1: `cp10_testUI_testGripFillPercentSpeed` (float_late)
    const HelperFloatSiteSpec spec1{kSlot442V1Name, kSlot442V1NameLength, kSlot442V1Description, kSlot442V1DescriptionLength, 1,
                                       kSlot442V1FnTag, kSlot442V1Value, false};
    out.v1 = helper_init_unbounded_float_var_site(objects.v1, spec1, concmd);
    if (out.refusal == HelperRefusal::none) out.refusal = out.v1.refusal;
    if (!out.v1.ctor_ok) return out;
    // v2: `cp10_testUI_testGripAttackCircle` (bool_late)
    out.v2 = lane_m4::init_bool_var_late_release(objects.v2, view_of(kSlot442V2Name, kSlot442V2NameLength), view_of(kSlot442V2Description, kSlot442V2DescriptionLength), kSlot442V2FnTag, concmd, static_cast<std::uint8_t>(kSlot442V2Value), static_cast<std::uint8_t>(kSlot442V2Value));
    out.ok = out.v0.ok && out.v1.ok && out.v2.ok;
    return out;
}

}  // namespace sotc
