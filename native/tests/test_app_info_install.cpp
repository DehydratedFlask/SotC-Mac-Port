// Fixture for the native 0x25a4d0 / 0xb10 entry writers: insertion order, platform/debug/camera
// selection, borrowed-string ownership and the visible failure stop.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string_view>
#include <vector>

#include "core/app_info_install.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
int g_allocs = 0;
int g_frees = 0;
int g_fail_after = -1;  // fail the allocation with this zero-based index

void* test_allocate(std::size_t size, std::uint32_t tag, std::uint32_t align, std::uint32_t flags) {
    const int index = g_allocs++;
    (void)tag;
    (void)align;
    (void)flags;
    if (g_fail_after >= 0 && index >= g_fail_after) {
        return nullptr;
    }
    void* p = std::calloc(1, size);
    CHECK(p != nullptr);
    return p;
}

bool test_deallocate(void* ptr, std::uint32_t tag) {
    ++g_frees;
    CHECK(tag == 2);
    std::free(ptr);
    return true;
}

void dtor_free_tag2(void* ptr) {
    CHECK(test_deallocate(ptr, 2));
}

std::string_view holder_text(const Callable1bdeb0& holder) {
    if (holder.value_00 == 0) {
        return {};
    }
    return {reinterpret_cast<const char*>(holder.value_00),
            static_cast<std::size_t>(holder.value_08)};
}

struct EntryText {
    std::string_view name;
    std::string_view value;
    std::uint32_t flag;
};

std::vector<EntryText> read_entries(const AppInfoRegistry& registry) {
    std::vector<EntryText> out;
    const List1bdeb0& list = registry.info_list();
    const auto* const head = reinterpret_cast<const Node1bdeb0*>(&list);
    const Node1bdeb0* node = list.next;
    while (node != head) {
        out.push_back({holder_text(node->callable_10), holder_text(node->callable_30),
                       node->flag_50});
        node = node->next;
    }
    return out;
}

void free_nodes(AppInfoRegistry& registry) {
    List1bdeb0& list = registry.info_list();
    auto* const head = reinterpret_cast<Node1bdeb0*>(&list);
    Node1bdeb0* node = list.next;
    while (node != head) {
        Node1bdeb0* const next = node->next;
        std::free(node);
        node = next;
    }
    list.prev = head;
    list.next = head;
    list.count = 0;
}

}  // namespace

int main() {
    CHECK(kAppInfoInstallerRva == 0x25a4d0);
    CHECK(kAppInfoInstallerSize == 1014);
    CHECK(kAppInfoGameWriterRva == 0xb10);
    CHECK(kAppInfoEntryFlag == 1);

    Insert1bdb50Hooks hooks{};
    hooks.alloc.allocate = test_allocate;
    hooks.alloc.deallocate = test_deallocate;

    AppInfoInstallHooks install_hooks{};
    install_hooks.insert = hooks;

    // ---- 1. Full five-entry install: order is Game first, then newest-first list A. ----
    {
        AppInfoRegistry registry;
        g_allocs = 0;
        g_frees = 0;
        g_fail_after = -1;

        static constexpr std::string_view game = "Test Game";
        static constexpr std::string_view build = "build-1234";
        static constexpr std::string_view camera = "setCameraPos 1.00 2.00 3.00 4.00 5.00 6.00";
        AppInfoProductValues values{};
        values.build_version = build;
        values.neo_mode = true;
        values.debug_memory_available = false;
        values.camera_available = true;
        values.camera_location = camera;

        const auto game_result = install_app_info_game_entry(registry, game, hooks);
        CHECK(game_result.ok);
        const auto result = install_app_info_product_entries(registry, values, install_hooks);
        CHECK(result.ok);
        CHECK(result.inserted == 4);
        CHECK(result.failed_site == 0);
        CHECK(result.platform_value == "ps4 - NEO");
        CHECK(registry.entry_count() == 5);
        CHECK(g_allocs == 5);

        const std::vector<EntryText> entries = read_entries(registry);
        CHECK(entries.size() == 5);
        // Front-to-back is the reverse of insertion.
        CHECK(entries[0].name == kAppInfoCameraName);
        CHECK(entries[0].value == camera);
        CHECK(entries[1].name == kAppInfoDebugMemoryName);
        CHECK(entries[1].value == "False");
        CHECK(entries[2].name == kAppInfoPlatformName);
        CHECK(entries[2].value == "ps4 - NEO");
        CHECK(entries[3].name == kAppInfoBuildVersionName);
        CHECK(entries[3].value == build);
        CHECK(entries[4].name == kAppInfoGameName);
        CHECK(entries[4].value == game);
        for (const EntryText& entry : entries) {
            CHECK(entry.flag == 1);
        }

        // Borrowed views: the inserter did not take ownership.
        const List1bdeb0& list = registry.info_list();
        const auto* const head = reinterpret_cast<const Node1bdeb0*>(&list);
        for (const Node1bdeb0* node = list.next; node != head; node = node->next) {
            CHECK(node->callable_10.control_18 == nullptr);
            CHECK(node->callable_30.control_18 == nullptr);
            CHECK(node->callable_10.value_10 == 0);
            CHECK(node->callable_30.value_10 == 0);
        }

        // The observed flag=1 purge skips all five; nodes survive.
        ListDtor1bdeb0Hooks dtor_hooks{};
        dtor_hooks.free_tag2 = dtor_free_tag2;
        const auto skipped = registry.clear(dtor_hooks);
        CHECK(skipped.ok);
        CHECK(skipped.skipped == 5);
        CHECK(skipped.erased == 0);
        CHECK(registry.entry_count() == 5);
        CHECK(g_frees == 0);
        free_nodes(registry);
    }

    // ---- 2. STANDARD platform, debug true, no camera -> four entries. ----
    {
        AppInfoRegistry registry;
        g_allocs = 0;
        g_fail_after = -1;
        AppInfoProductValues values{};
        values.build_version = "b";
        values.neo_mode = false;
        values.debug_memory_available = true;
        values.camera_available = false;
        const auto result = install_app_info_product_entries(registry, values, install_hooks);
        CHECK(result.ok);
        CHECK(result.inserted == 3);
        CHECK(result.platform_value == "ps4 - STANDARD");
        const std::vector<EntryText> entries = read_entries(registry);
        CHECK(entries.size() == 3);
        CHECK(entries[0].name == kAppInfoDebugMemoryName);
        CHECK(entries[0].value == "True");
        CHECK(entries[1].name == kAppInfoPlatformName);
        CHECK(entries[1].value == "ps4 - STANDARD");
        CHECK(entries[2].name == kAppInfoBuildVersionName);
        free_nodes(registry);
    }

    // ---- 3. Insertion failure stops at the exact site and leaves earlier entries. ----
    {
        AppInfoRegistry registry;
        g_allocs = 0;
        g_fail_after = 1;  // first insert succeeds, second fails
        AppInfoProductValues values{};
        values.build_version = "b";
        values.neo_mode = false;
        values.camera_available = false;
        const auto result = install_app_info_product_entries(registry, values, install_hooks);
        CHECK(!result.ok);
        CHECK(result.inserted == 1);
        CHECK(result.failed_site == 2);
        CHECK(registry.entry_count() == 1);
        CHECK(read_entries(registry)[0].name == kAppInfoBuildVersionName);
        free_nodes(registry);
    }

    // ---- 4. Missing allocator is a visible failure, not a fake success. ----
    {
        AppInfoRegistry registry;
        AppInfoInstallHooks null_hooks{};
        AppInfoProductValues values{};
        values.build_version = "b";
        const auto result = install_app_info_product_entries(registry, values, null_hooks);
        CHECK(!result.ok);
        CHECK(result.inserted == 0);
        CHECK(result.failed_site == 1);
        CHECK(registry.entry_count() == 0);
    }

    return 0;
}
