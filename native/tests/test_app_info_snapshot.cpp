// Fixture for the composed 0x1bdf30 report path: list-A FieldEntry, list-B Attachment, drain, clear.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "core/app_info_registry.hpp"
#include "core/app_info_snapshot.hpp"
#include "core/insert_1bdc80.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
int g_frees = 0;

void* test_allocate(std::size_t size, std::uint32_t tag, std::uint32_t align, std::uint32_t flags) {
    (void)tag;
    (void)align;
    (void)flags;
    void* p = std::calloc(1, size);
    CHECK(p != nullptr);
    return p;
}

bool test_deallocate(void* ptr, std::uint32_t tag) {
    CHECK(tag == 2);
    std::free(ptr);
    return true;
}

void free_tag2(void* ptr) {
    ++g_frees;
    CHECK(test_deallocate(ptr, 2));
}

Callable1bdeb0 view_holder(const char* text) {
    Callable1bdeb0 h{};
    h.value_00 = reinterpret_cast<std::uintptr_t>(text);
    h.value_08 = std::strlen(text);
    return h;
}

const ReportItem* child_at(const ReportItem& root, std::size_t index) {
    const ReportItem* child = root.first_child();
    while (child != nullptr && index > 0) {
        child = child->next_sibling();
        --index;
    }
    return child;
}

std::string property_name(const ReportItem& item) {
    const std::string* const name = item.property(kAppInfoListBNameKey);
    return name == nullptr ? std::string("<missing>") : *name;
}
}  // namespace

int main() {
    CHECK(kAppInfoListASerializeRva == 0x1be14d);
    CHECK(kAppInfoListASerializeEndRva == 0x1be222);
    CHECK(kAppInfoListAEntryLiteralRva == 0xfb3e29);
    CHECK(std::strcmp(kAppInfoListAEntryType, "FieldEntry") == 0);

    Insert1bdb50Hooks insert_a{};
    insert_a.alloc.allocate = test_allocate;
    insert_a.alloc.deallocate = test_deallocate;
    Insert1bdc80Hooks insert_b{};
    insert_b.alloc.allocate = test_allocate;
    insert_b.alloc.deallocate = test_deallocate;

    // ---- 1. Phases 2, 4, 5, 6 in order. ----
    {
        AppInfoRegistry registry;
        char a_name[16] = "a";
        char a_value[16] = "1";
        CHECK(registry.add_string_entry(a_name, a_value, 1, insert_a).ok);
        CHECK(registry.add_string_entry("b", "2", 1, insert_a).ok);
        CHECK(registry.add_string_entry("c", "3", 0, insert_a).ok);
        CHECK(registry.add_string_entry("d", "4", 1, insert_a).ok);
        CHECK(insert_1bdc80(registry.container(), view_holder("e"), view_holder("e1"), insert_b).ok);
        CHECK(insert_1bdc80(registry.container(), view_holder("f"), view_holder("f2"), insert_b).ok);
        CHECK(registry.entry_count() == 4);
        CHECK(registry.container().count2 == 2);

        std::unique_ptr<ReportItem> root = report_node_new("BugData");
        AppInfoSnapshotHooks hooks{};
        hooks.list_a.free_tag2 = free_tag2;
        hooks.list_b.free_tag2 = free_tag2;
        g_frees = 0;
        const auto result = app_info_snapshot(registry, *root, hooks);
        CHECK(result.ok);
        CHECK(result.list_a_nodes == 4);
        CHECK(result.list_b_nodes == 2);
        CHECK(result.drained == 3);
        CHECK(result.list_b_freed == 2);
        CHECK(result.releases == 10);  // 2 per drained list-A node + 2 per list-B node
        CHECK(result.release_failures == 0);
        CHECK(result.list_a_count_after == 1);
        CHECK(registry.entry_count() == 1);
        CHECK(registry.container().count2 == 0);
        CHECK(g_frees == 5);

        // Phase 2 first, phase 4 second: four FieldEntry, then two Attachment.
        CHECK(root->child_count() == 6);
        const char* const a_names[] = {"a", "b", "c", "d"};
        const char* const a_values[] = {"1", "2", "3", "4"};
        for (std::size_t i = 0; i < 4; ++i) {
            const ReportItem* const entry = child_at(*root, i);
            CHECK(entry != nullptr);
            CHECK(entry->kind() == ReportKind::kNode);
            CHECK(entry->text() == "FieldEntry");
            CHECK(entry->child_count() == 1);
            CHECK(property_name(*entry) == a_names[i]);
            CHECK(entry->first_child()->text() == a_values[i]);
        }
        const char* const b_names[] = {"e", "f"};
        const char* const b_values[] = {"e1", "f2"};
        for (std::size_t i = 0; i < 2; ++i) {
            const ReportItem* const attachment = child_at(*root, 4 + i);
            CHECK(attachment != nullptr);
            CHECK(attachment->text() == "Attachment");
            CHECK(property_name(*attachment) == b_names[i]);
            CHECK(attachment->first_child()->text() == b_values[i]);
        }

        // The report copied the text before the drain released the source.
        std::strcpy(a_name, "MUTATED");
        std::strcpy(a_value, "MUTATED");
        CHECK(property_name(*child_at(*root, 0)) == "a");
        CHECK(child_at(*root, 0)->first_child()->text() == "1");

        ListDtor1bdeb0Hooks dtor{};
        dtor.free_tag2 = free_tag2;
        CHECK(registry.clear(dtor).ok);
        CHECK(registry.entry_count() == 0);
    }

    // ---- 2. Second snapshot: only the flag==0 survivor, no list-B entries. ----
    {
        AppInfoRegistry registry;
        CHECK(registry.add_string_entry("x", "1", 1, insert_a).ok);
        CHECK(registry.add_string_entry("y", "2", 0, insert_a).ok);
        CHECK(insert_1bdc80(registry.container(), view_holder("z"), view_holder("3"), insert_b).ok);

        AppInfoSnapshotHooks hooks{};
        hooks.list_a.free_tag2 = free_tag2;
        hooks.list_b.free_tag2 = free_tag2;
        std::unique_ptr<ReportItem> first = report_node_new("BugData");
        CHECK(app_info_snapshot(registry, *first, hooks).ok);
        CHECK(first->child_count() == 3);  // x, y, z
        CHECK(registry.entry_count() == 1);
        CHECK(registry.container().count2 == 0);

        std::unique_ptr<ReportItem> second = report_node_new("BugData");
        const auto again = app_info_snapshot(registry, *second, hooks);
        CHECK(again.ok);
        CHECK(again.list_a_nodes == 1);
        CHECK(again.list_b_nodes == 0);
        CHECK(again.drained == 0);
        CHECK(again.list_b_freed == 0);
        CHECK(second->child_count() == 1);
        CHECK(property_name(*second->first_child()) == "y");

        ListDtor1bdeb0Hooks dtor{};
        dtor.free_tag2 = free_tag2;
        CHECK(registry.clear(dtor).ok);
        CHECK(registry.entry_count() == 0);
    }

    // ---- 3. A missing hook fails before any mutation. ----
    {
        AppInfoRegistry registry;
        CHECK(registry.add_string_entry("k", "v", 1, insert_a).ok);
        CHECK(insert_1bdc80(registry.container(), view_holder("m"), view_holder("n"), insert_b).ok);
        std::unique_ptr<ReportItem> root = report_node_new("BugData");
        AppInfoSnapshotHooks hooks{};
        hooks.list_a.free_tag2 = free_tag2;
        const auto result = app_info_snapshot(registry, *root, hooks);
        CHECK(!result.ok);
        CHECK(result.list_a_nodes == 0);
        CHECK(result.list_b_nodes == 0);
        CHECK(root->child_count() == 0);
        CHECK(registry.entry_count() == 1);
        CHECK(registry.container().count2 == 1);

        hooks.list_b.free_tag2 = free_tag2;
        CHECK(app_info_snapshot(registry, *root, hooks).ok);
        CHECK(registry.entry_count() == 0);
        CHECK(registry.container().count2 == 0);
    }

    // ---- 4. A value root is rejected without touching either list. ----
    {
        AppInfoRegistry registry;
        CHECK(registry.add_string_entry("p", "q", 0, insert_a).ok);
        std::unique_ptr<ReportItem> value_root = report_value_new("not-a-node");
        AppInfoSnapshotHooks hooks{};
        hooks.list_a.free_tag2 = free_tag2;
        hooks.list_b.free_tag2 = free_tag2;
        const auto result = app_info_snapshot(registry, *value_root, hooks);
        CHECK(!result.ok);
        CHECK(registry.entry_count() == 1);
        ListDtor1bdeb0Hooks dtor{};
        dtor.free_tag2 = free_tag2;
        CHECK(registry.clear(dtor).ok);
    }

    return 0;
}
