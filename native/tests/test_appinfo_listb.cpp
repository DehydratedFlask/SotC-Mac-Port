// Fixture for the list-B reader/clear (0x1be2f7..0x1be4b6) and the report tree.
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "core/app_info_registry.hpp"
#include "core/appinfo_listb.hpp"
#include "core/insert_1bdc80.hpp"
#include "core/report_tree.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

int g_allocs = 0;
int g_frees = 0;
std::uint32_t g_last_flags = 0;
int g_destroy = 0;
int g_free_control = 0;

void* test_allocate(std::size_t size, std::uint32_t /*tag*/, std::uint32_t /*align*/,
                    std::uint32_t flags) {
    ++g_allocs;
    g_last_flags = flags;
    return std::malloc(size);
}

bool test_deallocate(void* ptr, std::uint32_t /*tag*/) {
    std::free(ptr);
    return true;
}

void count_destroy(void*, int) { ++g_destroy; }
void free_control(void* control) {
    ++g_free_control;
    delete static_cast<std::atomic<std::uint64_t>*>(control);
}

void count_free_tag2(void* node) {
    ++g_frees;
    std::free(node);
}

Callable1bdeb0 view_holder(const char* text) {
    Callable1bdeb0 h{};
    h.value_00 = reinterpret_cast<std::uintptr_t>(text);
    h.value_08 = std::strlen(text);
    return h;
}

std::string child_name(const ReportItem& node) {
    const std::string* const name = node.property(kAppInfoListBNameKey);
    return name == nullptr ? std::string("<missing>") : *name;
}

const ReportItem* child_at(const ReportItem& root, std::size_t index) {
    const ReportItem* child = root.first_child();
    while (child != nullptr && index > 0) {
        child = child->next_sibling();
        --index;
    }
    return child;
}

}  // namespace

int main() {
    CHECK(kAppInfoListBReaderRva == 0x1be2f7);
    CHECK(kAppInfoListBReaderSize == 239);
    CHECK(kAppInfoListBReaderEndRva == 0x1be3e6);
    CHECK(kAppInfoListBClearRva == 0x1be458);
    CHECK(kAppInfoListBClearSize == 69);
    CHECK(kAppInfoListBClearEndRva == 0x1be49d);
    CHECK(kAppInfoListBResetRva == 0x1be4a4);
    CHECK(kAppInfoListBResetSize == 19);
    CHECK(kAppInfoListBResetEndRva == 0x1be4b7);
    CHECK(kAppInfoListBValueKind == 4);

    Insert1bdc80Hooks hooks{};
    hooks.alloc.allocate = test_allocate;
    hooks.alloc.deallocate = test_deallocate;
    hooks.release.destroy = count_destroy;
    hooks.release.free_control = free_control;

    AppInfoRegistry registry;
    Container1beeb0& container = registry.container();

    // Three entries, inserted in a known order. The allocator must see the tag-2 family flags.
    char mutable_value[] = "first-value";
    CHECK(insert_1bdc80(container, view_holder("first"), view_holder(mutable_value), hooks).ok);
    CHECK(g_last_flags == 2);  // 0x17090: flags 2, pairing with the 0x170c0 free
    CHECK(insert_1bdc80(container, view_holder("second"), view_holder("second-value"), hooks).ok);
    CHECK(insert_1bdc80(container, view_holder("third"), view_holder("third-value"), hooks).ok);
    CHECK(container.count2 == 3);

    // ---- 1. Serialize: FIFO order, "Attachment", property "name", value child. ----
    {
        std::unique_ptr<ReportItem> root = report_node_new("BugData");
        const auto sr = appinfo_listb_serialize(container, *root);
        CHECK(sr.ok);
        CHECK(sr.entries == 3);
        CHECK(sr.nodes == 3);
        CHECK(sr.values == 3);
        CHECK(root->child_count() == 3);

        const char* const names[] = {"first", "second", "third"};
        const char* const values[] = {"first-value", "second-value", "third-value"};
        for (std::size_t i = 0; i < 3; ++i) {
            const ReportItem* const attachment = child_at(*root, i);
            CHECK(attachment != nullptr);
            CHECK(attachment->kind() == ReportKind::kNode);
            CHECK(attachment->text() == kAppInfoListBAttachmentType);
            CHECK(attachment->child_count() == 1);
            CHECK(child_name(*attachment) == names[i]);
            const ReportItem* const value = attachment->first_child();
            CHECK(value != nullptr);
            CHECK(value->kind() == ReportKind::kStringValue);
            CHECK(value->text() == values[i]);
            CHECK(value->parent() == attachment);
        }

        // The list itself is untouched by the read.
        CHECK(container.count2 == 3);
        CHECK(registry.container().list2_prev !=
              reinterpret_cast<Node1beeb0*>(&registry.container().list2_prev));

        // Deep copy: mutating the source after the read must not change the report.
        std::strcpy(mutable_value, "MUTATED");
        CHECK(child_at(*root, 0)->first_child()->text() == "first-value");

        // Property semantics: last write wins, one entry.
        ReportItem probe("probe", ReportKind::kNode);
        probe.set_property("name", "a");
        probe.set_property("name", "b");
        CHECK(probe.property_count() == 1);
        CHECK(probe.property("name") != nullptr && *probe.property("name") == "b");
    }

    // ---- 2. Clear: value then name, free, reset both links and the 8-byte count. ----
    {
        g_frees = 0;
        AppInfoListBClearHooks ch{};
        ch.free_tag2 = count_free_tag2;
        const auto cr = appinfo_listb_clear(container, ch);
        CHECK(cr.ok);
        CHECK(cr.freed == 3);
        CHECK(cr.releases == 6);
        CHECK(cr.release_failures == 0);
        CHECK(cr.count_after == 0);
        CHECK(container.count2 == 0);
        CHECK(g_frees == 3);
        CHECK(container.list2_prev == reinterpret_cast<Node1beeb0*>(&container.list2_prev));
        CHECK(container.list2_next == container.list2_prev);

        // Idempotent on the empty list.
        const auto again = appinfo_listb_clear(container, ch);
        CHECK(again.ok);
        CHECK(again.freed == 0);
        CHECK(again.count_after == 0);
    }

    // ---- 3. A null free hook is a visible failure and leaves the list untouched. ----
    {
        CHECK(insert_1bdc80(container, view_holder("keep"), view_holder("keep-value"), hooks).ok);
        AppInfoListBClearHooks ch{};
        const auto cr = appinfo_listb_clear(container, ch);
        CHECK(!cr.ok);
        CHECK(cr.freed == 0);
        CHECK(container.count2 == 1);
        ch.free_tag2 = count_free_tag2;
        CHECK(appinfo_listb_clear(container, ch).ok);
        CHECK(container.count2 == 0);
    }

    // ---- 4. A real refcount: the clear consumes the node's reference. ----
    {
        auto* const ca = new std::atomic<std::uint64_t>(1);
        auto* const cb = new std::atomic<std::uint64_t>(1);
        Callable1bdeb0 a{};
        a.value_00 = reinterpret_cast<std::uintptr_t>("owned-a");
        a.control_18 = ca;
        Callable1bdeb0 b{};
        b.value_00 = reinterpret_cast<std::uintptr_t>("owned-b");
        b.control_18 = cb;
        CHECK(insert_1bdc80(container, a, b, hooks).ok);
        CHECK(ca->load() == 2);
        CHECK(cb->load() == 2);

        g_destroy = 0;
        g_free_control = 0;
        g_frees = 0;
        AppInfoListBClearHooks ch{};
        ch.free_tag2 = count_free_tag2;
        ch.release.destroy = count_destroy;
        ch.release.free_control = free_control;
        CHECK(appinfo_listb_clear(container, ch).ok);
        CHECK(ca->load() == 1);  // the caller's reference remains
        CHECK(cb->load() == 1);
        CHECK(g_destroy == 0);
        CHECK(g_free_control == 0);

        // Drop the caller's references; the last release must destroy and free.
        SharedHolderF590 ha{reinterpret_cast<void*>(a.value_00), nullptr, nullptr, ca};
        SharedHolderF590 hb{reinterpret_cast<void*>(b.value_00), nullptr, nullptr, cb};
        CHECK(shared_release_f590(ha, ch.release).ok);
        CHECK(shared_release_f590(hb, ch.release).ok);
        CHECK(g_destroy == 2);
        CHECK(g_free_control == 2);
    }

    // ---- 5. A value root is rejected without mutating the list. ----
    {
        CHECK(insert_1bdc80(container, view_holder("x"), view_holder("y"), hooks).ok);
        std::unique_ptr<ReportItem> value_root = report_value_new("not-a-node");
        const auto sr = appinfo_listb_serialize(container, *value_root);
        CHECK(!sr.ok);
        CHECK(sr.entries == 0);
        CHECK(container.count2 == 1);
        AppInfoListBClearHooks ch{};
        ch.free_tag2 = count_free_tag2;
        CHECK(appinfo_listb_clear(container, ch).ok);
    }

    return 0;
}
