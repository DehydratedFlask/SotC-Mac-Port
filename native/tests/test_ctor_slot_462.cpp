// CPU-01-AOT-CFG-SCALE-01-BATCH27: fixture for slot 462 / 0x3e2730. Verifies every static object,
// the six commands plus the bool variable, the 0xd0-byte functor command, the four menu items and
// the fifteen __cxa_atexit records in body order.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_462_3e2730.hpp"
#include "core/fmenu_ctors_slot306.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

std::size_t g_stages = 0;
bool count_stage(void*, ConCmdFunctor& object, unsigned stage) noexcept {
    ++g_stages;
    object.region[0] = stage + 1;
    return true;
}

std::size_t g_view_destroys = 0;
bool count_view_destroy(void*, ConCmdFunctor&) noexcept {
    ++g_view_destroys;
    return true;
}

bool view_is(const SharedStr& v, const char* text) {
    return v.data != nullptr && v.size == static_cast<std::int64_t>(std::strlen(text)) && v.aux == 0 &&
           v.ctl == nullptr && std::memcmp(v.data, text, std::strlen(text)) == 0 && v.data[v.size] == 0;
}

void init_root(DebugMenuRegistry& menu) {
    menu.root_guard_done = true;
    menu.root.link.self = nullptr;
    menu.root.link.prev = &menu.root.link;
    menu.root.link.next = &menu.root.link;
    menu.root.link.owner = nullptr;
    menu.root.count = 0;
}

}  // namespace

int main() {
    Slot462Objects objects{};
    ConCmdRegistry registry{};
    DebugMenuRegistry menu{};
    init_root(menu);
    Slot462Hooks hooks{};
    hooks.functor.transfer = count_stage;
    hooks.destroy_view = count_view_destroy;

    const Slot462Result result = init_slot462_3e2730(objects, registry, menu, hooks);
    CHECK(result.ok);
    CHECK(result.canary_matched);
    CHECK(result.registered_commands == 7);
    CHECK(result.menu_items == 4);
    CHECK(result.static_views == 2);
    CHECK(result.atexit_records == 15);
    CHECK(result.functor_transfers == 5 && g_stages == 5);
    CHECK(result.view_destructions == 1 && g_view_destroys == 1);
    CHECK(registry.pending.size() == 7);
    CHECK(registry.atexit_log.size() == 15);
    CHECK(menu.root.count == 4);

    CHECK(objects.zero_cell == 0);
    CHECK(view_is(objects.presentation, "Presentation"));
    CHECK(view_is(objects.clear_active, "Clear Active Point of Interest"));

    const struct {
        const Slot462Command* command;
        const char* name;
        const char* description;
        std::uint64_t callback;
    } commands[] = {
        {&objects.activate, "activatePOI",
         "Sets the poi as active. +activatePOI <group> <name>", kSlot462ActivateCallback},
        {&objects.clear, "clearPOI", "Sets no poi as active. +clearPOI", kSlot462ClearCallback},
        {&objects.clear_errors, "poiClearErrorMessages", "Clear Error Messages",
         kSlot462ClearErrorsCallback},
        {&objects.poi_group, "poiGroup",
         "Add a point of interest group. +poiGroup <name> <description>", kSlot462PoiGroupCallback},
        {&objects.poi, "poi",
         "Add a point of interest the debug camera can use to jump to. +poi <group> <name> "
         "<posx> <posy> <posz> <opt:size> <opt:description>",
         kSlot462PoiCallback},
    };
    for (const auto& row : commands) {
        const Slot462Command& cmd = *row.command;
        CHECK(cmd.base.vptr == kSlot462CommandVptr);
        CHECK(cmd.base.registered == 1 && cmd.base.flags == kSlot462CommandFlags);
        CHECK(view_is(cmd.base.name, row.name));
        CHECK(view_is(cmd.base.description, row.description));
        CHECK(cmd.tail0 == reinterpret_cast<std::uint64_t>(&cmd.tail0));
        CHECK(cmd.tail8 == kSlot462CommandTail8 && cmd.tail10 == kSlot462CommandTail10);
        CHECK(cmd.tail18 == row.callback);
        for (std::uint64_t word : cmd.zero80) CHECK(word == 0);
    }

    const ConCmdBoolVar& var = objects.show_description;
    CHECK(var.base.vptr == kSlot462BoolFinalVptr);
    CHECK(var.base.registered == 1 && var.base.flags == kSlot462VarFlags);
    CHECK(view_is(var.base.name, "showPoiDescription"));
    CHECK(var.base.description.size == 0 && var.base.description.data[0] == 0);
    CHECK(var.value_ref == &var.value);
    CHECK(var.default_value == 0 && var.value == 0);
    CHECK(var.zero69[0] == 0 && var.zero69[1] == 0 && var.zero69[2] == 0 && var.zero69[3] == 0);
    CHECK(var.fn_payload == 0 && var.fn_vptr == kSlot462BoolFnTag && var.fn_aux == 0);

    CHECK(objects.render_errors.vptr == kSlot462StringVptr);
    CHECK(view_is(objects.render_errors.view, "renderErrorMsgs"));
    CHECK(objects.render_errors.zero28 == 0);

    const ConCmdFunctor& functor = objects.show_render_target;
    CHECK(functor.base.vptr == kSlot462FunctorVptr);
    CHECK(functor.base.registered == 1 && functor.base.flags == kSlot462VarFlags);
    CHECK(view_is(functor.base.name, "presentationShowRenderTarget"));
    CHECK(functor.base.description.size == 0);
    CHECK(functor.spec == kSlot462FunctorSpec && functor.spec_word == 0);
    CHECK(functor.low == kSlot462FunctorLow && functor.one70 == 1);
    CHECK(functor.high == kSlot462FunctorHigh && functor.one78 == 1);
    CHECK(functor.view0 == 0 && functor.view8 == 0 && functor.view10 == 0 && functor.view18 == 0);
    CHECK(functor.region[0] == 5);
    CHECK(functor.tag_c0 == kSlot462FunctorTagVptr && functor.tag_c8 == 0);

    CHECK(objects.item_show_description.kind == kDebugMenuItemKind);
    CHECK(objects.item_show_description.flags == kSlot462MenuFlags);
    CHECK(view_is(objects.item_show_description.label, "Presentation"));
    CHECK(view_is(objects.item_show_description.text, "Show Description"));
    CHECK(objects.item_show_description.target == &var);

    CHECK(objects.item_clear_errors.kind == kDebugMenuItemKind);
    CHECK(objects.item_clear_errors.flags == kSlot462MenuFlags);
    CHECK(view_is(objects.item_clear_errors.label, "Presentation"));
    CHECK(view_is(objects.item_clear_errors.text, "Clear Error Messages"));
    CHECK(view_is(objects.item_clear_errors.empty, "poiClearErrorMessages"));
    CHECK(objects.item_clear_errors.target == reinterpret_cast<const void*>(kSlot462TargetGlobal));

    CHECK(objects.item_show_target.kind == kDebugMenuItemKind);
    CHECK(view_is(objects.item_show_target.text, "Show Render Target"));
    CHECK(objects.item_show_target.target == &functor);

    CHECK(objects.item_shadow_jump.kind == kMenuKindGroup);
    CHECK(view_is(objects.item_shadow_jump.text, "Shadow Boss Jump"));
    CHECK(view_is(objects.item_shadow_jump.empty, "Shadow Boss Jump"));

    const std::uint64_t expected_dtors[15] = {
        kSlot462ZeroCellDtor, kSlot462ViewDtor, kSlot462ViewDtor, kSlot462CommandDtor,
        kSlot462CommandDtor, kSlot462BoolDtor, kSlot462StringDtor, kSlot462FunctorDtor,
        kSlot462CommandDtor, kSlot462MenuItemDtor, kSlot462MenuItemDtor, kSlot462MenuItemDtor,
        kSlot462MenuItemDtor, kSlot462CommandDtor, kSlot462CommandDtor,
    };
    const void* const expected_objects[15] = {
        &objects.zero_cell, &objects.presentation, &objects.clear_active, &objects.activate,
        &objects.clear, &objects.show_description, &objects.render_errors,
        &objects.show_render_target, &objects.clear_errors, &objects.item_show_description,
        &objects.item_clear_errors, &objects.item_show_target, &objects.item_shadow_jump,
        &objects.poi_group, &objects.poi,
    };
    for (std::size_t i = 0; i < 15; ++i) {
        CHECK(registry.atexit_log.at(i).dtor_rva == expected_dtors[i]);
        CHECK(registry.atexit_log.at(i).object == expected_objects[i]);
    }

    std::printf("ctor_slot_462: 7 registrations, 4 menu items, 2 views, 15 atexit rows\n");
    return 0;
}
