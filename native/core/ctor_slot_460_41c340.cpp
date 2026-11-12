#include "ctor_slot_460_41c340.hpp"

namespace sotc {

namespace {

SharedStr view_of(const char* text, std::size_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(length), 0, nullptr};
}

}  // namespace

Slot460Result init_slot460_41c340(Slot460Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot460Result out{};
    // v0: `drawWaypointNode` (bool_static)
    out.v0 = init_bool_var_static(objects.v0, view_of(kSlot460V0Name, kSlot460V0NameLength), view_of("", kSlot460V0DescriptionLength), kSlot460V0FnTag, concmd, static_cast<std::uint8_t>(kSlot460V0Value), static_cast<std::uint8_t>(kSlot460V0Value));
    // v1: `drawWaypointSegment` (float_early)
    const HelperFloatSiteSpec spec1{kSlot460V1Name, kSlot460V1NameLength, "", kSlot460V1DescriptionLength, 1,
                                       kSlot460V1FnTag, kSlot460V1Value, true};
    out.v1 = helper_init_unbounded_float_var_site(objects.v1, spec1, concmd);
    if (out.refusal == HelperRefusal::none) out.refusal = out.v1.refusal;
    if (!out.v1.ctor_ok) return out;
    // v2: `drawWaypointSample` (float_early)
    const HelperFloatSiteSpec spec2{kSlot460V2Name, kSlot460V2NameLength, "", kSlot460V2DescriptionLength, 1,
                                       kSlot460V2FnTag, kSlot460V2Value, true};
    out.v2 = helper_init_unbounded_float_var_site(objects.v2, spec2, concmd);
    if (out.refusal == HelperRefusal::none) out.refusal = out.v2.refusal;
    if (!out.v2.ctor_ok) return out;
    out.ok = out.v0.ok && out.v1.ok && out.v2.ok;
    return out;
}

}  // namespace sotc
