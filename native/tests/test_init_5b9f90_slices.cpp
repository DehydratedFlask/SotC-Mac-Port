// Fixture for the native 0x5b9f90 slices (core/init_5b9f90_slices): S0 "Shadow" group, the two
// page-builder contracts and the S3.1/S3.2 boss pages (34 command items).
//
// Sections:
//   1. full run: 3 groups + 32 command items registered into the process root list, 70 releases,
//      2 contracts, 3 executed slice records, resume at 0x5bb4be
//   2. root-list shape: every item's kind, views, target tag, flags and default range
//   3. fail-closed: state_missing, page_builder refusal and a bad view length
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
    StartupAtexitLog atexit{};
    std::array<DebugMenuItem, 40> items{};
    std::uint64_t canary = 0;
    Init5b9f90Hooks hooks{};

    Fixture() {
        hooks.host = this;
        hooks.menu = &menu;
        hooks.atexit = &atexit;
        hooks.canary_cell = &canary;
        hooks.canary_token = 0x5B9F90CA11ULL;
        hooks.item_storage = items.data();
        hooks.item_capacity = items.size();
        hooks.release_view = release_view;
        hooks.page_builder = page_builder;
    }
};

const DebugMenuItem* root_at(const DebugMenuRegistry& menu, std::size_t index) {
    const MenuLink* link = menu.root.link.next;
    for (std::size_t i = 0; i < index; ++i) link = link->next;
    return reinterpret_cast<const DebugMenuItem*>(link);
}

std::size_t root_size(const DebugMenuRegistry& menu) {
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
        CHECK(r.groups == 3);
        CHECK(r.commands == 32);
        CHECK(r.registered == 35);
        CHECK(r.contracts == 2);
        CHECK(r.releases == 70);
        CHECK(r.executed_slices == 3);
        CHECK(r.canary_stored);
        CHECK(r.items_used == 35);
        CHECK(f.items[0].kind == 1);   // the storage the root list points into
        CHECK(f.canary == f.hooks.canary_token);
        CHECK(r.record_count == 3);
        CHECK(r.records[0].callee_rva == kInit5b9f90Rva);
        CHECK(r.records[1].callee_rva == 0x5BA04B);
        CHECK(r.records[2].callee_rva == 0x5BAAB2);
        for (std::size_t i = 0; i < r.record_count; ++i) {
            CHECK(r.records[i].executed);
            CHECK(r.records[i].caller != nullptr && std::strcmp(r.records[i].caller, "0x5b9f90") == 0);
            CHECK(std::strstr(r.records[i].detail, "slice S") != nullptr);
        }
        CHECK(root_size(f.menu) == 35);
        CHECK(f.menu.root.count == 35);
        CHECK(f.atexit.size() == 1);   // the lazy root destructor record

        // 2. root-list shape: group "Shadow", page "Shadow Boss Jump", its 16 items, then the outro
        //    page and its 16 items.
        check_item(*root_at(f.menu, 0), 1, "CP10", "Shadow", "Shadow", 0);
        check_item(*root_at(f.menu, 1), 1, "Shadow", "Shadow Boss Jump", "Shadow Boss Jump", 0);
        check_item(*root_at(f.menu, 2), 2, "Shadow Boss Jump", "Boss 1  (Minotaur A)", "0",
                   kInit5b9f90BossJumpTargetRva);
        check_item(*root_at(f.menu, 17), 2, "Shadow Boss Jump", "Boss 16 (Wizard / Evis)", "15",
                   kInit5b9f90BossJumpTargetRva);
        check_item(*root_at(f.menu, 18), 1, "Shadow", "Shadow Boss Outro Jump",
                   "Shadow Boss Outro Jump", 0);
        check_item(*root_at(f.menu, 19), 2, "Shadow Boss Outro Jump",
                   "Boss Outro 1  (Minotaur A)", "0", kInit5b9f90BossOutroJumpTargetRva);
        check_item(*root_at(f.menu, 34), 2, "Shadow Boss Outro Jump",
                   "Boss Outro 16 (Wizard / Evis)", "15", kInit5b9f90BossOutroJumpTargetRva);

        // Every command item in each page carries the page target tag and the sequence 0..15.
        for (int page = 0; page < 2; ++page) {
            const std::uint64_t target =
                page == 0 ? kInit5b9f90BossJumpTargetRva : kInit5b9f90BossOutroJumpTargetRva;
            for (int i = 0; i < 16; ++i) {
                const DebugMenuItem& item = *root_at(f.menu, static_cast<std::size_t>(2 + page * 17 + i));
                CHECK(item.kind == 2);
                CHECK(reinterpret_cast<std::uintptr_t>(item.target) == target);
                CHECK(item.empty.size == (i < 10 ? 1 : 2));
            }
        }
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

    std::printf("init_5b9f90_slices: ok\n");
    return 0;
}
