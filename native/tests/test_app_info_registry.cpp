// Fixture for the native app-info registry owner: the 0x1bdb10 container construction, the
// 0x1bdb50 0x58-node entry insert (string views and callable holders), and the 0x1bdeb0 teardown
// interaction including the observed flag=1 skip.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string_view>

#include "core/app_info_registry.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
struct AllocLog {
    int allocs = 0;
    int frees = 0;
    std::size_t last_size = 0;
    std::uint32_t last_tag = 0;
    std::uint32_t last_align = 0;
    std::uint32_t last_flags = 0;
    bool fail_next = false;
} g_log;

void* test_allocate(std::size_t size, std::uint32_t tag, std::uint32_t align, std::uint32_t flags) {
    ++g_log.allocs;
    g_log.last_size = size;
    g_log.last_tag = tag;
    g_log.last_align = align;
    g_log.last_flags = flags;
    if (g_log.fail_next) {
        return nullptr;
    }
    void* p = std::calloc(1, size);
    CHECK(p != nullptr);
    return p;
}

bool test_deallocate(void* ptr, std::uint32_t tag) {
    ++g_log.frees;
    CHECK(tag == 2);
    std::free(ptr);
    return true;
}

void dtor_free_tag2(void* ptr) {
    CHECK(test_deallocate(ptr, 2));
}

int g_destroyed = 0;
void count_destroy(void* obj, int) {
    ++g_destroyed;
    (void)obj;
}
}  // namespace

int main() {
    CHECK(kAppInfoRegistryOwnerSize == 0x60);
    CHECK(kAppInfoRegistryInfoListOffset == 0x00);
    CHECK(kAppInfoRegistryHandlerListOffset == 0x40);
    CHECK(sizeof(Container1beeb0) == 0x60);
    CHECK(sizeof(List1bdeb0) == 0x18);
    CHECK(offsetof(Container1beeb0, list3_prev) == 0x40);
    CHECK(offsetof(List1bdeb0, count) == 0x10);

    Insert1bdb50Hooks hooks{};
    hooks.alloc.allocate = test_allocate;
    hooks.alloc.deallocate = test_deallocate;
    hooks.release.destroy = count_destroy;

    // ---- 1. Construction: three self-referential lists, zero counts, list-3 count at +0x50. ----
    {
        AppInfoRegistry registry;
        const auto* const base = reinterpret_cast<const unsigned char*>(&registry.container());
        Node1beeb0* const sentinel1 = reinterpret_cast<Node1beeb0*>(&registry.container().list1_prev);
        Node1beeb0* const sentinel2 = reinterpret_cast<Node1beeb0*>(&registry.container().list2_prev);
        Node1beeb0* const sentinel3 = reinterpret_cast<Node1beeb0*>(&registry.container().list3_prev);
        CHECK(registry.container().list1_prev == sentinel1);
        CHECK(registry.container().list1_next == sentinel1);
        CHECK(registry.container().list2_prev == sentinel2);
        CHECK(registry.container().list2_next == sentinel2);
        CHECK(registry.container().list3_prev == sentinel3);
        CHECK(registry.container().list3_next == sentinel3);
        CHECK(registry.container().count1 == 0);
        CHECK(registry.container().count2 == 0);
        CHECK(registry.container().count3 == 0);
        CHECK(*(reinterpret_cast<const std::uint64_t*>(base + 0x50)) == 0);
        CHECK(registry.info_list().prev == reinterpret_cast<Node1bdeb0*>(&registry.container().list1_prev));
        CHECK(registry.info_list().next == registry.info_list().prev);
        CHECK(registry.entry_count() == 0);
    }

    // ---- 2. Insert two string entries with the observed flag=1. ----
    {
        AppInfoRegistry registry;
        static constexpr std::string_view dir_name = "Game";
        static constexpr std::string_view dir_value = "Shadow of the Colossus";
        g_log = AllocLog{};
        const auto first = registry.add_string_entry(dir_name, dir_value, /*flag*/ 1, hooks);
        CHECK(first.ok);
        CHECK(first.allocated);
        CHECK(first.acquires == 0);  // string-view holders have no control block
        CHECK(first.node != nullptr);
        CHECK(first.count_after == 1);
        CHECK(g_log.allocs == 1);
        CHECK(g_log.last_size == 0x58);
        CHECK(g_log.last_tag == 1);
        CHECK(g_log.last_align == 8);
        CHECK(g_log.last_flags == 1);
        CHECK(first.node->flag_50 == 1);
        CHECK(first.node->callable_10.value_00 == reinterpret_cast<std::uintptr_t>(dir_name.data()));
        CHECK(first.node->callable_10.value_08 == dir_name.size());
        CHECK(first.node->callable_10.value_10 == 0);
        CHECK(first.node->callable_10.control_18 == nullptr);
        CHECK(first.node->callable_30.value_00 == reinterpret_cast<std::uintptr_t>(dir_value.data()));
        CHECK(first.node->callable_30.value_08 == dir_value.size());
        CHECK(first.node->callable_30.control_18 == nullptr);
        CHECK(registry.entry_count() == 1);

        static constexpr std::string_view platform_name = "Platform Type";
        static constexpr std::string_view platform_value = "ps4 - NEO";
        const auto second = registry.add_string_entry(platform_name, platform_value, 1, hooks);
        CHECK(second.ok);
        CHECK(second.count_after == 2);
        CHECK(registry.entry_count() == 2);
        // Push-front: the second insert becomes the head and links to the first.
        CHECK(registry.info_list().next == reinterpret_cast<Node1bdeb0*>(second.node));
        CHECK(second.node->next == reinterpret_cast<Node1bdeb0*>(first.node));
        CHECK(second.node->prev == reinterpret_cast<Node1bdeb0*>(&registry.info_list()));

        // ---- 3. Callable value holder (the original "Build Version" value is not a string). ----
        Callable1bdeb0 functor_name{};
        functor_name.value_00 = 0x1234;
        functor_name.value_08 = 1;
        Callable1bdeb0 functor_value{};
        functor_value.value_00 = 0x5678;
        functor_value.value_08 = 0x9abc;
        functor_value.value_10 = 0xdef0;
        const auto third = registry.add_callable_entry(functor_name, functor_value, 1, hooks);
        CHECK(third.ok);
        CHECK(third.node->callable_10.value_00 == 0x1234);
        CHECK(third.node->callable_30.value_00 == 0x5678);
        CHECK(third.node->callable_30.value_10 == 0xdef0);

        // ---- 4. Teardown with the observed flag=1: every entry is skipped, nothing is freed. ----
        ListDtor1bdeb0Hooks dtor_hooks{};
        dtor_hooks.free_tag2 = dtor_free_tag2;
        const auto skipped = registry.clear(dtor_hooks);
        CHECK(skipped.ok);
        CHECK(skipped.skipped == 3);
        CHECK(skipped.erased == 0);
        CHECK(registry.entry_count() == 3);
        CHECK(g_log.frees == 0);

        // The fixture owns the manually allocated nodes because the original dtor skipped them.
        // Free them directly to keep the sanitizer run clean; this is test cleanup, not registry
        // behaviour.
        Node1bdeb0* node = registry.info_list().next;
        while (node != reinterpret_cast<Node1bdeb0*>(&registry.info_list())) {
            Node1bdeb0* const next = node->next;
            std::free(node);
            node = next;
        }
        g_log.frees = 0;
    }

    // ---- 5. Failure surface: a missing allocator touches nothing. ----
    {
        AppInfoRegistry registry;
        Insert1bdb50Hooks no_alloc{};
        const auto failed = registry.add_string_entry("x", "y", 1, no_alloc);
        CHECK(!failed.ok);
        CHECK(!failed.allocated);
        CHECK(failed.node == nullptr);
        CHECK(registry.entry_count() == 0);
        CHECK(registry.info_list().next == reinterpret_cast<Node1bdeb0*>(&registry.info_list()));
    }

    // ---- 6. Flag=0 path: the destructor erases, releases and frees with tag 2. ----
    {
        AppInfoRegistry registry;
        g_log = AllocLog{};
        const auto inserted = registry.add_string_entry("temp", "value", /*flag*/ 0, hooks);
        CHECK(inserted.ok);
        CHECK(registry.entry_count() == 1);
        ListDtor1bdeb0Hooks dtor_hooks{};
        dtor_hooks.free_tag2 = dtor_free_tag2;
        const auto erased = registry.clear(dtor_hooks);
        CHECK(erased.ok);
        CHECK(erased.erased == 1);
        CHECK(erased.skipped == 0);
        CHECK(erased.frees == 1);
        CHECK(registry.entry_count() == 0);
        CHECK(registry.info_list().next == reinterpret_cast<Node1bdeb0*>(&registry.info_list()));
        CHECK(g_log.frees == 1);
    }

    return 0;
}
