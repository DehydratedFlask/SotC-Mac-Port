// Fixture for the native 0x5b9f90 slices (core/init_5b9f90_slices): S0 "Shadow" group, the S2
// 0x5cf140 "Shadow Credits" body (5,526 B) and all six S3 pages (34 command items, 8 toggles, 4
// float ranges and 4 guarded bool variables).
//
// Sections:
//   1. full run: 8 groups + 81 registered items, 134 releases, 44 destroys, 1 contract, 8 executed
//      slice records, resume at 0x5bc3d4
//   2. root-list shape: every item's kind, views, target identity and range
//   3. fail-closed: state_missing, page_builder refusal, missing toggle storage
//
// The model never executes an original RVA. The target tags are stored, never dereferenced.
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/debug_menu_item.hpp"
#include "core/init_5b9f90_slices.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

int g_releases = 0;

bool release_view(void*, const SharedStr& view) noexcept {
    ++g_releases;
    return view.data != nullptr;
}

void* test_alloc(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) noexcept {
    return std::malloc(size);
}

bool test_dealloc(void* p, std::uint32_t) noexcept {
    std::free(p);
    return true;
}

bool g_refuse_builder = false;

InitPostCallOutcome page_builder(void*, std::uint64_t rva, const char*) noexcept {
    InitPostCallOutcome o;
    if (g_refuse_builder && rva == kInit5b9f90PageBuilderShadowMiscRva) {
        o.blocker = "contract_refused";
        std::snprintf(o.detail, sizeof o.detail, "0x%llx refused", static_cast<unsigned long long>(rva));
        return o;
    }
    o.executed = true;
    std::snprintf(o.detail, sizeof o.detail, "0x%llx observed", static_cast<unsigned long long>(rva));
    return o;
}

struct Fixture {
    DebugMenuRegistry menu{};
    ConCmdRegistry registry{};
    StartupAtexitLog atexit{};
    std::array<DebugMenuItem, 192> items{};
    std::array<bool, 8> toggle_values{};
    std::array<ConCmdBoolVar, 32> vars{};
    std::array<bool, 32> var_values{};
    std::array<bool, 32> var_guards{};
    std::array<ConCmdRangedVar, 64> ranged{};
    std::array<float, 64> ranged_values{};
    std::array<std::int32_t, 64> ranged_int_values{};
    std::array<bool, 64> ranged_guards{};
    std::uint64_t canary = 0;
    Init5b9f90Hooks hooks{};

    Fixture() {
        hooks.host = this;
        hooks.menu = &menu;
        hooks.atexit = &atexit;
        hooks.registry = &registry;
        hooks.canary_cell = &canary;
        hooks.canary_token = 0x5B9F90CA11ULL;
        hooks.item_storage = items.data();
        hooks.item_capacity = items.size();
        hooks.var_storage = vars.data();
        hooks.var_capacity = vars.size();
        hooks.var_value_storage = var_values.data();
        hooks.var_guard_storage = var_guards.data();
        hooks.var_guard_capacity = var_guards.size();
        hooks.ranged_storage = ranged.data();
        hooks.ranged_capacity = ranged.size();
        hooks.ranged_value_storage = ranged_values.data();
        hooks.ranged_int_value_storage = ranged_int_values.data();
        hooks.ranged_guard_storage = ranged_guards.data();
        hooks.separator_alloc.alloc.allocate = test_alloc;
        hooks.separator_alloc.alloc.deallocate = test_dealloc;
        hooks.release_view = release_view;
        hooks.page_builder = page_builder;
        hooks.toggle_value_storage = toggle_values.data();
        hooks.toggle_value_capacity = toggle_values.size();
        hooks.var_storage = vars.data();
        hooks.var_capacity = vars.size();
        hooks.var_value_storage = var_values.data();
        hooks.var_guard_storage = var_guards.data();
        hooks.var_guard_capacity = var_guards.size();
    }
};

const DebugMenuItem* root_at(const DebugMenuRegistry& menu, std::size_t index) {
    const MenuLink* link = menu.root.link.next;
    for (std::size_t i = 0; i < index; ++i) link = link->next;
    return reinterpret_cast<const DebugMenuItem*>(link);
}

[[maybe_unused]] std::size_t root_size(const DebugMenuRegistry& menu) {
    std::size_t n = 0;
    for (const MenuLink* link = menu.root.link.next; link != &menu.root.link; link = link->next) {
        ++n;
        if (n > 128) break;
    }
    return n;
}

bool view_is(const SharedStr& view, const char* text) {
    const std::size_t len = std::strlen(text);
    return view.size == static_cast<std::int64_t>(len) &&
           (len == 0 || std::memcmp(view.data, text, len) == 0) && view.ctl == nullptr;
}

void check_item(const DebugMenuItem& item, std::uint32_t kind, const char* label, const char* text,
                const char* command, std::uint64_t target) {
    CHECK(item.kind == kind);
    CHECK(item.flags == kInit5b9f90ItemFlags);
    CHECK(item.link.self == &item.link);   // linked by the root path
    CHECK(item.link.owner != nullptr);
    if (!view_is(item.label, label)) {
        std::fprintf(stderr, "label mismatch: got \"%.*s\" want \"%s\"\n",
                     static_cast<int>(item.label.size),
                     reinterpret_cast<const char*>(item.label.data), label);
    }
    CHECK(view_is(item.label, label));
    CHECK(view_is(item.text, text));
    CHECK(view_is(item.empty, command));
    CHECK(reinterpret_cast<std::uintptr_t>(item.target) == target);
    CHECK(item.range_min == 1.0f && item.range_max == 10.0f);
    CHECK(item.registered == 0);   // the root path leaves +0x20 at zero
}

}  // namespace

int main() {
    // 1. full run
    {
        Fixture f;
        g_releases = 0;
        const Init5b9f90Result r = run_init_5b9f90_slices(f.hooks);

        CHECK(r.ok);
        CHECK(!r.complete);
        CHECK(r.blocked == nullptr);
        CHECK(r.resume_rva == kInit5b9f90ResumeRva);
        CHECK(kInit5b9f90ResumeRva == 0x5C1196);
        CHECK(r.groups == 12);   // + S5.2 Dynamic 3D + wormhole group
        CHECK(r.commands == 41);   // + 3 S5.2 keyframe commands   // 37 S3 + the S2 credits-level command
        CHECK(r.toggles == 8);
        CHECK(r.float_items == 4);
        CHECK(r.named_vars == 28);   // + 9 S5.2 bool variables   // 7 (S3.4/S2) + 12 S4.1 bool variables
        CHECK(r.separators == 13);   // + 7 S5.2 separators
        CHECK(r.sliders == 45);   // + 28 S5.2 floats      // 10 S2 + 5 S4.1 + 2 S5.1
        CHECK(r.int_sliders == 9);   // + 7 S5.2 0xc580 ints   // S4.1 0xc580
        CHECK(r.s2_executed);
        CHECK(r.s2_items == 21);
        CHECK(r.s4_executed);
        CHECK(r.s4_items == 20);   // group + 7 sliders + 12 bool items
        CHECK(r.s4_bytes == kInit5dbf40Size);
        CHECK(r.registered == 160);   // 81 + 20 S4.1 + 3 S5.1 items
        CHECK(r.contracts == 2);     // S1 0x2170 + S4.2 0x7f8930
        CHECK(r.releases == 279);
        CHECK(r.destroys == 120);
        CHECK(r.executed_slices == 11);
        CHECK(r.s5_executed);
        CHECK(r.s5_items == 59);
        CHECK(r.s5_bytes == kInit5b9f90S5_1Size + kInit5b9f90S5_2Size);
        CHECK(r.canary_stored);
        CHECK(r.items_used == 160);
        CHECK(f.items[0].kind == 1);   // the storage the root list points into
        CHECK(f.canary == f.hooks.canary_token);
        CHECK(r.record_count == 11);
        CHECK(r.records[0].callee_rva == kInit5b9f90Rva);
        CHECK(r.records[1].callee_rva == kInit5cf140Rva);
        CHECK(r.records[2].callee_rva == 0x5BA04B);
        CHECK(r.records[3].callee_rva == 0x5BAAB2);
        CHECK(r.records[4].callee_rva == 0x5BB4BE);
        CHECK(r.records[5].callee_rva == 0x5BBD2E);
        CHECK(r.records[6].callee_rva == 0x5BC377);
        CHECK(r.records[7].callee_rva == 0x5BC3C7);
        CHECK(r.records[8].callee_rva == kInit5dbf40Rva);
        CHECK(r.records[9].callee_rva == 0x5BC413);
        CHECK(r.records[10].callee_rva == kInit5b9f90S5_2Rva);
        for (std::size_t i = 0; i < r.record_count; ++i) {
            CHECK(r.records[i].executed);
            CHECK(r.records[i].caller != nullptr && std::strcmp(r.records[i].caller, "0x5b9f90") == 0);
            CHECK(std::strstr(r.records[i].detail, "slice S") != nullptr);
        }
        CHECK(f.menu.root.count == 160);
        // root + 17 + S4.1 (19) + S5.1 (2)
        CHECK(f.atexit.size() == 82);

        // 2. root-list shape: S0, S3.1, S3.2.
        check_item(*root_at(f.menu, 0), 1, "CP10", "Shadow", "Shadow", 0);
        check_item(*root_at(f.menu, 22), 1, "Shadow", "Shadow Boss Jump", "Shadow Boss Jump", 0);
        check_item(*root_at(f.menu, 23), 2, "Shadow", "Boss 1  (Minotaur A)", "0",
                   kInit5b9f90BossJumpTargetRva);
        check_item(*root_at(f.menu, 39), 1, "Shadow", "Shadow Boss Outro Jump",
                   "Shadow Boss Outro Jump", 0);
        check_item(*root_at(f.menu, 40), 2, "Shadow",
                   "Boss Outro 1  (Minotaur A)", "0", kInit5b9f90BossOutroJumpTargetRva);

        // S3.3 "Shadow Cheat": group, 8 toggles, 5 commands, 4 float ranges (root 35..52).
        // S2 "Shadow Credits" (root 20..40): group, command, separator, external named item and
        // the two bool variables before the first bounded slider.
        check_item(*root_at(f.menu, 1), 1, "Shadow", "Shadow Credits", "Shadow Credits", 0);
        check_item(*root_at(f.menu, 2), 2, "Shadow Credits", "Jump to credits level", "BP_JumpToCredits",
                   0x2DFCBD0);
        CHECK(root_at(f.menu, 3)->kind == kMenuKindSeparator);
        CHECK(view_is(root_at(f.menu, 4)->text, "cp10_creditsUseNewList"));
        CHECK(view_is(root_at(f.menu, 5)->text, "Apply adjustments"));
        CHECK(reinterpret_cast<std::uintptr_t>(root_at(f.menu, 5)->target) ==
              reinterpret_cast<std::uintptr_t>(&f.vars[5]));
        CHECK(view_is(root_at(f.menu, 6)->text, "Credit Pause"));
        CHECK(view_is(root_at(f.menu, 9)->text, "Font Scale"));
        CHECK(root_at(f.menu, 9)->range_min == 0.01f && root_at(f.menu, 9)->range_max == 0.1f);
        CHECK(f.ranged_guards[0]);
        CHECK(f.ranged[0].base.vptr == kRangedFloatMidVptr);
        CHECK(f.atexit.at(3).dtor_rva == kInit5cf140BoolVarDtorRva);

        check_item(*root_at(f.menu, 56), 1, "CP10", "Shadow Cheat", "Shadow Cheat", 0);
        for (int i = 0; i < 8; ++i) {
            const std::size_t slot = static_cast<std::size_t>(57 + (i < 4 ? i : i + 5));
            const DebugMenuItem& item = *root_at(f.menu, slot);
            CHECK(item.kind == 2);
            CHECK(view_is(item.label, "CP10"));   // the items hang off the group's parent view
            CHECK(item.target == &f.toggle_values[static_cast<std::size_t>(i)]);
        }
        CHECK(view_is(root_at(f.menu, 57)->text, "No_Damage"));
        CHECK(view_is(root_at(f.menu, 61)->empty, "1"));   // Set Game Completed
        CHECK(view_is(root_at(f.menu, 64)->empty, "0"));   // Set Game Normal Mode
        CHECK(reinterpret_cast<std::uintptr_t>(root_at(f.menu, 61)->target) == 0x2FED3D8);
        CHECK(reinterpret_cast<std::uintptr_t>(root_at(f.menu, 64)->target) == 0x2FED478);
        for (int i = 0; i < 4; ++i) {
            const DebugMenuItem& item = *root_at(f.menu, static_cast<std::size_t>(70 + i));
            CHECK(item.kind == 2);
            CHECK(item.range_min == 0.01f && item.range_max == 0.1f);
        }
        CHECK(view_is(root_at(f.menu, 70)->text, "30Hz Acceleration Scalar"));
        CHECK(view_is(root_at(f.menu, 73)->text, "60Hz Acceleration Scalar Easy Mode"));

        // S3.4 "Shadow Trophy": group + four named bool variables (root 53..57).
        check_item(*root_at(f.menu, 74), 1, "Shadow", "Shadow Trophy", "Shadow Trophy", 0);
        for (int i = 0; i < 4; ++i) {
            const DebugMenuItem& item = *root_at(f.menu, static_cast<std::size_t>(75 + i));
            CHECK(item.kind == 2);
            CHECK(view_is(item.label, "Shadow"));
            CHECK(item.target == &f.vars[static_cast<std::size_t>(i)]);
            CHECK(view_is(item.text, kInit5b9f90TrophyItems[i].caption));
            CHECK(f.vars[static_cast<std::size_t>(i)].base.vptr == kInit5b9f90BoolVarVptr);
            CHECK(f.var_guards[static_cast<std::size_t>(i)]);
            CHECK(f.vars[static_cast<std::size_t>(i)].value_ref ==
                  reinterpret_cast<std::uint8_t*>(&f.var_values[static_cast<std::size_t>(i)]));
        }

        // S3.5 "Shadow Render" and S3.6 "MLAA" close the six pages (root 58, 59).
        check_item(*root_at(f.menu, 79), 1, "Shadow", "Shadow Render", "Shadow Render", 0);
        check_item(*root_at(f.menu, 80), 1, "Shadow Render", "MLAA", "MLAA", 0);

        // S4.1 "Particle" (root 81..100): group, 7 variable sliders and 12 bool items.
        check_item(*root_at(f.menu, 81), 1, "Shadow Render", "Particle", "Particle", 0);
        CHECK(root_at(f.menu, 82)->range_min == 1.0f && root_at(f.menu, 82)->range_max == 1.0f);
        CHECK(view_is(root_at(f.menu, 82)->text, "Soft Particle Mode"));
        CHECK(root_at(f.menu, 82)->target == &f.ranged[10]);
        CHECK(view_is(root_at(f.menu, 83)->text, "Soft Particle Fixed Fade"));
        CHECK(root_at(f.menu, 83)->target == &f.ranged[11]);
        CHECK(view_is(root_at(f.menu, 84)->text, "Refract Min Fade"));
        CHECK(view_is(root_at(f.menu, 85)->text, "Refract Max Fade"));
        CHECK(view_is(root_at(f.menu, 86)->text, "Camera Min Fade"));
        CHECK(view_is(root_at(f.menu, 87)->text, "Camera Max Fade"));
        CHECK(view_is(root_at(f.menu, 88)->text, "Draw Extend"));
        CHECK(root_at(f.menu, 88)->target == &f.vars[7]);
        CHECK(view_is(root_at(f.menu, 99)->text, "Optimize Null"));
        CHECK(view_is(root_at(f.menu, 100)->text, "Particle Offscreen Render"));
        CHECK(root_at(f.menu, 100)->target == &f.vars[18]);
        CHECK(f.ranged_guards[10] && f.ranged_guards[16]);
        CHECK(f.var_guards[7] && f.var_guards[18]);

        // S5.1 "Stereo 3D" (root 101..103): group and two bounded sliders.
        check_item(*root_at(f.menu, 101), 1, "CP10", "Stereo 3D", "Stereo 3D", 0);
        CHECK(view_is(root_at(f.menu, 102)->text, "Auto Focal Distance"));
        CHECK(root_at(f.menu, 102)->target == &f.ranged[17]);
        CHECK(view_is(root_at(f.menu, 103)->text, "Auto Distortion"));
        CHECK(root_at(f.menu, 103)->target == &f.ranged[18]);
        CHECK(f.ranged_guards[17] && f.ranged_guards[18]);
    }

    // 3. fail-closed
    {
        Fixture f;
        Init5b9f90Hooks hooks = f.hooks;
        hooks.canary_cell = nullptr;
        const Init5b9f90Result r = run_init_5b9f90_slices(hooks);
        CHECK(!r.ok);
        CHECK(std::strcmp(r.blocked, "state_missing") == 0);
    }
    {
        Fixture f;
        g_refuse_builder = true;
        const Init5b9f90Result r = run_init_5b9f90_slices(f.hooks);
        g_refuse_builder = false;
        CHECK(!r.ok);
        CHECK(std::strcmp(r.blocked, "contract_refused") == 0);
        CHECK(r.groups == 1);      // S0 ran before the refusal
        CHECK(r.commands == 0);
        CHECK(r.record_count == 1);
    }
    {
        Fixture f;
        Init5b9f90Hooks hooks = f.hooks;
        hooks.toggle_value_storage = nullptr;
        const Init5b9f90Result r = run_init_5b9f90_slices(hooks);
        CHECK(!r.ok);
        CHECK(std::strcmp(r.blocked, "toggle_storage_missing") == 0);
        CHECK(r.toggles == 0);
    }

    std::printf("init_5b9f90_slices: ok\n");
    return 0;
}
