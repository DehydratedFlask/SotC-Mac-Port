// CPU-01-STARTUP-SLOT-448: fixture for startup constructor index 448 (0x4b8680): the two bool console variables
// shadow_DisableWorldZoomOut (1/1) and shadow_mapShowBossCloudIndices (0/0), functor tag 0x15c0c30. Lane M6.
#include "l4_test_util.hpp"

#include "core/ctor_slot_448_4b8680.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot448Rva == 0x4b8680 && kSlot448Size == 501 && kSlot448BoolFnTag == 0x15c0c30);
    CHECK(std::strlen(kSlot448Var1Name) == kSlot448Var1NameLength && kSlot448Var1NameLength == 0x1a);
    CHECK(std::strlen(kSlot448Var1Description) == kSlot448Var1DescriptionLength && kSlot448Var1DescriptionLength == 0x4b);
    CHECK(std::strlen(kSlot448Var2Name) == kSlot448Var2NameLength && kSlot448Var2NameLength == 0x1e);
    CHECK(std::strlen(kSlot448Var2Description) == kSlot448Var2DescriptionLength && kSlot448Var2DescriptionLength == 0x4a);
    CHECK(sizeof(Slot448Objects) == 2 * sizeof(ConCmdBoolVar));   // second variable directly after the first (+0xc8)
    CHECK(sizeof(ConCmdBoolVar) == 0xc8);
}

// check_bool_common for a variable with a non-empty borrowed description (the shared helper expects an empty one).
void check_bool_common_desc(const ConCmdBoolVar& v, std::uint64_t fn_tag, std::uint8_t value, const char* desc,
                            std::size_t desc_len) {
    CHECK(v.base.vptr == kBoolVarFinalVptr);
    CHECK(v.value_ref == &v.value);
    for (std::uint8_t byte : v.zero69) CHECK(byte == 0);
    for (std::uint8_t byte : v.zero70) CHECK(byte == 0);
    CHECK(v.fn_payload == 0 && v.fn_aux == 0 && v.fn_vptr == fn_tag);
    CHECK(v.base.registered == 1 && v.base.flags == 1);
    CHECK(v.value == value && v.default_value == value);
    check_untouched(v, {{0x00, 8}, {0x08, 0x10}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 8}, {0x68, 1},
                        {0x69, 4}, {0x70, 0x20}, {0xa0, 8}, {0xb0, 8}, {0xb8, 8}, {0xc0, 1}});
    CHECK(same_view(v.base.description, desc, desc_len));
}

void section_alone() {
    ConCmdRegistry creg;
    Slot448Objects o;
    poison(o);
    const Slot448Result r = init_slot448_4b8680(o, creg);
    CHECK(r.ok && r.first.ok && r.second.ok && r.first.views_inert && r.second.views_inert);
    CHECK(r.first.registration.ok && r.first.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.second.registration.ok && r.second.registration.path == ConCmdRegisterPath::pending_list);

    check_bool_common_desc(o.disable_world_zoom_out, 0x15c0c30, 1, kSlot448Var1Description, 0x4b);
    CHECK(same_view(o.disable_world_zoom_out.base.name, "shadow_DisableWorldZoomOut", 0x1a));
    check_bool_common_desc(o.map_show_boss_cloud_idx, 0x15c0c30, 0, kSlot448Var2Description, 0x4a);
    CHECK(same_view(o.map_show_boss_cloud_idx.base.name, "shadow_mapShowBossCloudIndices", 0x1e));

    CHECK(creg.pending.size() == 2);
    CHECK(creg.atexit_log.size() == 2);
    check_atexit(creg, 0, 0x20b0, &o.disable_world_zoom_out);
    check_atexit(creg, 1, 0x20b0, &o.map_show_boss_cloud_idx);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;   // registration through a manager without an insert contract
    Slot448Objects o;
    const Slot448Result r = init_slot448_4b8680(o, creg);
    CHECK(!r.ok && !r.first.ok && !r.second.ok && !r.first.registration.ok && !r.second.registration.ok &&
          creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_448");
    return 0;
}
