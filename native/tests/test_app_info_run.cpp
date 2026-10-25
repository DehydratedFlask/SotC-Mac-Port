// Fixture for the composed 0x1bdf30 path: list-C install, then FieldEntry/Attachment/drain/clear.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string_view>

#include "core/app_info_install.hpp"
#include "core/app_info_run.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
int g_delegate_frees = 0;
int g_entry_frees = 0;

void* delegate_allocate(std::size_t size) {
    void* p = std::calloc(1, size);
    CHECK(p != nullptr);
    return p;
}

void delegate_deallocate(void* ptr) {
    ++g_delegate_frees;
    std::free(ptr);
}

void* entry_allocate(std::size_t size, std::uint32_t tag, std::uint32_t align, std::uint32_t flags) {
    (void)tag;
    (void)align;
    (void)flags;
    void* p = std::calloc(1, size);
    CHECK(p != nullptr);
    return p;
}

bool entry_deallocate(void* ptr, std::uint32_t tag) {
    (void)tag;
    std::free(ptr);
    return true;
}

void entry_free_tag2(void* ptr) {
    ++g_entry_frees;
    CHECK(entry_deallocate(ptr, 2));
}
}  // namespace

int main() {
    CHECK(kAppInfoRunDispatchStartRva == 0x1bdf66);
    CHECK(kAppInfoRunDispatchNextRva == 0x1bdf79);

    Insert1beeb0Alloc delegate_alloc{};
    delegate_alloc.alloc = delegate_allocate;
    delegate_alloc.dealloc = delegate_deallocate;
    Insert1bdb50Hooks entry_hooks{};
    entry_hooks.alloc.allocate = entry_allocate;
    entry_hooks.alloc.deallocate = entry_deallocate;

    // ---- 1. Dispatch installs five entries, then the snapshot serializes and drains them. ----
    {
        RegistryOwner owner(delegate_alloc);
        AppInfoProductDelegate product{};
        product.values.build_version = "b";
        product.values.camera_available = true;
        product.values.camera_location = "setCameraPos 0 0 0 0 0 0";
        product.insert = entry_hooks;
        CHECK(owner.add_delegate({&product, install_app_info_product_entry}).ok);
        AppInfoGameDelegate game{};
        game.game_name = "Game";
        game.insert = entry_hooks;
        CHECK(owner.add_delegate({&game, install_app_info_game_entry_delegate}).ok);

        std::unique_ptr<ReportItem> root = report_node_new("BugData");
        AppInfoSnapshotHooks snap{};
        snap.list_a.free_tag2 = entry_free_tag2;
        snap.list_b.free_tag2 = entry_free_tag2;
        const auto result = run_app_info_1bdf30(owner, *root, snap);
        CHECK(result.ok);
        CHECK(result.dispatch.ok);
        CHECK(result.dispatch.called == 2);
        CHECK(result.snapshot.list_a_nodes == 5);
        CHECK(result.snapshot.list_b_nodes == 0);
        CHECK(result.snapshot.drained == 5);
        CHECK(root->child_count() == 5);
        for (const ReportItem* child = root->first_child(); child != nullptr;
             child = child->next_sibling()) {
            CHECK(child->text() == "FieldEntry");
            CHECK(child->child_count() == 1);
        }
        CHECK(owner.registry().entry_count() == 0);
        owner.clear_delegates();
        CHECK(g_delegate_frees == 2);
    }

    // ---- 2. A dispatch failure stops before serialization. ----
    {
        RegistryOwner owner(delegate_alloc);
        CHECK(owner.add_delegate({nullptr, nullptr}).ok);
        std::unique_ptr<ReportItem> root = report_node_new("BugData");
        AppInfoSnapshotHooks snap{};
        snap.list_a.free_tag2 = entry_free_tag2;
        snap.list_b.free_tag2 = entry_free_tag2;
        const auto result = run_app_info_1bdf30(owner, *root, snap);
        CHECK(!result.ok);
        CHECK(!result.dispatch.ok);
        CHECK(result.snapshot.list_a_nodes == 0);
        CHECK(root->child_count() == 0);
        owner.clear_delegates();
    }

    return 0;
}
