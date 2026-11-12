// CPU-01-AOT-CFG-SCALE-01-BATCH26: fixture for slot 454 / 0x483e40. Verifies the observable
// stores, the three console registrations, the four records, the three tree headers (with their
// allocation sizes), the properties view and the eight __cxa_atexit records in body order.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/ctor_slot_454_483e40.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

std::size_t g_transfers = 0;
bool count_transfer(void*, Slot454LocalPlayer&) noexcept {
    ++g_transfers;
    return true;
}

std::vector<std::size_t> g_sizes;
std::size_t g_frees = 0;
void* recording_alloc(std::size_t bytes) noexcept {
    g_sizes.push_back(bytes);
    return std::malloc(bytes);
}
void recording_free(void* p) noexcept {
    ++g_frees;
    std::free(p);
}

bool view_is(const SharedStr& v, const char* text) {
    return v.data != nullptr && v.size == static_cast<std::int64_t>(std::strlen(text)) && v.aux == 0 &&
           v.ctl == nullptr && std::memcmp(v.data, text, std::strlen(text)) == 0 && v.data[v.size] == 0;
}

}  // namespace

int main() {
    Slot454Objects objects{};
    ConCmdRegistry registry{};
    Slot454Hooks hooks{};
    hooks.rb = RbEraseHooks{recording_alloc, recording_free};
    hooks.functor_transfer = count_transfer;

    const Slot454Result result = init_slot454_483e40(objects, registry, hooks);
    CHECK(result.ok);
    CHECK(result.canary_matched);
    CHECK(result.registered_commands == 3);
    CHECK(result.records == 4);
    CHECK(result.trees == 3);
    CHECK(result.atexit_records == 8);
    CHECK(result.functor_transfers == 5 && g_transfers == 5);
    CHECK(registry.pending.size() == 3);
    CHECK(registry.atexit_log.size() == 8);
    CHECK((g_sizes == std::vector<std::size_t>{0x38, 0x38, 0x1a8}));

    // ---- 1. local player ---------------------------------------------------------------------
    const Slot454LocalPlayer& lp = objects.local_player;
    CHECK(lp.base.vptr == kSlot454LocalPlayerVptr);
    CHECK(lp.base.registered == 1 && lp.base.flags == 1);
    CHECK(view_is(lp.base.name, "network_LocalPlayerId"));
    CHECK(lp.base.description.size == 0 && lp.base.description.data[0] == 0);
    CHECK(lp.value_ref == reinterpret_cast<std::uint64_t>(&lp.value));
    CHECK(lp.default_value == -1 && lp.value == -1);
    CHECK(lp.zero6c == 0 && lp.zero70 == 0 && lp.zero74 == 0 && lp.zero78 == 0);
    CHECK(lp.functor_vptr == kSlot454LocalPlayerFunctorVptr && lp.zeroB0 == 0 && lp.zeroC8 == 0);

    // ---- 2/3. bool variables -----------------------------------------------------------------
    const Slot454BoolVar& interp = objects.interpolation;
    CHECK(interp.base.vptr == kSlot454BoolVptr && interp.base.registered == 1 && interp.base.flags == 1);
    CHECK(view_is(interp.base.name, "network_Interpolation"));
    CHECK(interp.tail.value_ref == reinterpret_cast<std::uint64_t>(&interp.tail.value));
    CHECK(interp.tail.default_value == 1 && interp.tail.value == 1 && interp.tail.zero69 == 0);
    CHECK(interp.tail.tagB0 == kSlot454BoolFnTagValue && interp.tail.zeroA0 == 0 && interp.tail.zeroB8 == 0);
    const Slot454BoolVar& show = objects.show_bandwidth;
    CHECK(view_is(show.base.name, "network_ShowObjectBandwidth"));
    CHECK(show.tail.value_ref == reinterpret_cast<std::uint64_t>(&show.tail.value));
    CHECK(show.tail.default_value == 0 && show.tail.value == 0);
    CHECK(show.tail.tagB0 == kSlot454BoolFnTagValue);

    // ---- 4. records --------------------------------------------------------------------------
    for (std::size_t i = 0; i < kSlot454RecordCount; ++i) {
        CHECK(view_is(objects.records[i].text, kSlot454RecordText[i]));
        CHECK(objects.records[i].kind == kSlot454RecordKind[i]);
        CHECK(objects.records[i].zero28 == 0 && objects.records[i].zero30 == 0 &&
              objects.records[i].flag31 == 0);
    }

    // ---- 5/7. trees --------------------------------------------------------------------------
    for (const RbEraseTree* tree : {&objects.tree_a, &objects.tree_b, &objects.tree_c}) {
        CHECK(tree->header != nullptr);
        CHECK(tree->header->left == tree->header && tree->header->right == tree->header &&
              tree->header->parent == tree->header);
        CHECK(tree->header->color == kRbBlack && tree->header->nil == 1);
        CHECK(tree->size == 0);
    }

    // ---- 6. properties view ------------------------------------------------------------------
    CHECK(view_is(objects.properties, "SServerObjectProperties"));

    // ---- 8. __cxa_atexit order (body order: 0x483f75..0x4843a9) ------------------------------
    const std::uint64_t expected_dtors[8] = {
        kSlot454LocalPlayerDtor, kSlot454BoolDtor, kSlot454BoolDtor, kSlot454RecordsDtor,
        kSlot454TreeDtorA, kSlot454TreeDtorA, kSlot454ViewDtor, kSlot454TreeDtorB,
    };
    const void* expected_objects[8] = {
        &objects.local_player, &objects.interpolation, &objects.show_bandwidth, objects.records.data(),
        &objects.tree_a, &objects.tree_b, &objects.properties, &objects.tree_c,
    };
    for (std::size_t i = 0; i < 8; ++i) {
        CHECK(registry.atexit_log.at(i).dtor_rva == expected_dtors[i]);
        CHECK(registry.atexit_log.at(i).object == expected_objects[i]);
    }

    // ---- teardown through the lifted tree release (0x47e0f0 / 0x482f50 tags) -----------------
    rb_tree_release(objects.tree_a, hooks.rb);
    rb_tree_release(objects.tree_b, hooks.rb);
    rb_tree_release(objects.tree_c, hooks.rb);
    CHECK(g_frees == 3);
    CHECK(objects.tree_a.header == nullptr && objects.tree_b.header == nullptr &&
          objects.tree_c.header == nullptr);

    std::printf("ctor_slot_454: 3 commands, 4 records, 3 trees, 8 atexit rows; tree headers freed\n");
    return 0;
}
