#include "ctor_slot_448_4b8680.hpp"

namespace sotc {

Slot448Result init_slot448_4b8680(Slot448Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot448Result out{};
    // 0x4b86e1..0x4b879f: shadow_DisableWorldZoomOut, value 1 / default 1
    out.first = lane_m6::init_bool_var_literal(objects.disable_world_zoom_out, kSlot448Var1Name, kSlot448Var1NameLength,
                                               kSlot448Var1Description, kSlot448Var1DescriptionLength,
                                               kSlot448BoolFnTag, concmd, 1, 1);
    // 0x4b87b1..0x4b8861: shadow_mapShowBossCloudIndices, value 0 / default 0 (runs even if the first failed)
    out.second = lane_m6::init_bool_var_literal(objects.map_show_boss_cloud_idx, kSlot448Var2Name,
                                                kSlot448Var2NameLength, kSlot448Var2Description,
                                                kSlot448Var2DescriptionLength, kSlot448BoolFnTag, concmd, 0, 0);
    out.ok = out.first.ok && out.second.ok;
    return out;
}

}  // namespace sotc
