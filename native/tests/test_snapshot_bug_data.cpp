// Fixture for the snapshotBugData block: four gate/flag cases, synthetic Command, list-B screenshot,
// adoption and failure surfaces.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <vector>

#include "core/snapshot_bug_data.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
int g_queries = 0;
int g_adopts = 0;
int g_listb_frees = 0;

void* listb_allocate(std::size_t size, std::uint32_t tag, std::uint32_t align, std::uint32_t flags) {
    (void)tag;
    (void)align;
    (void)flags;
    void* p = std::calloc(1, size);
    CHECK(p != nullptr);
    return p;
}

bool listb_deallocate(void* ptr, std::uint32_t tag) {
    CHECK(tag == 2);
    std::free(ptr);
    return true;
}

void entry_free_tag2(void* ptr) {
    std::free(ptr);
}

void listb_free_tag2(void* ptr) {
    ++g_listb_frees;
    CHECK(listb_deallocate(ptr, 2));
}

bool query_true(std::vector<std::uint8_t>& png) {
    ++g_queries;
    png = {0x89, 0x50, 0x4E, 0x47};
    return true;
}

bool query_false(std::vector<std::uint8_t>&) {
    ++g_queries;
    return false;
}

ReportItem* adopt(ReportItem* root) {
    ++g_adopts;
    return root;
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
    CHECK(kSnapshotBugDataBodyRva == 0x25d8b0);
    CHECK(kSnapshotBugDataBodySize == 1086);
    CHECK(kSnapshotBugDataCallRva == 0x25db9e);
    CHECK(kScreenshotQueryRva == 0x961a0);

    Insert1beeb0Alloc delegate_alloc{};
    delegate_alloc.alloc = [](std::size_t size) -> void* { return std::calloc(1, size); };
    delegate_alloc.dealloc = [](void* p) { std::free(p); };

    // Common setup: no list-C delegates, so the report is built from the registry contents only.
    auto make_owner = [&]() { return std::make_unique<RegistryOwner>(delegate_alloc); };
    auto make_hooks = [&]() {
        SnapshotBugDataHooks hooks{};
        hooks.list_b_insert.alloc.allocate = listb_allocate;
        hooks.list_b_insert.alloc.deallocate = listb_deallocate;
        hooks.snapshot.list_a.free_tag2 = entry_free_tag2;
        hooks.snapshot.list_b.free_tag2 = listb_free_tag2;
        hooks.adopt_root = adopt;
        return hooks;
    };

    // ---- 1. Not armed: nothing happens. ----
    {
        g_queries = g_adopts = 0;
        auto owner = make_owner();
        std::unique_ptr<ReportItem> root = report_node_new("BugData");
        SnapshotBugDataHooks hooks = make_hooks();
        hooks.armed = false;
        hooks.arg2_present = true;
        hooks.query_screenshot = query_true;
        const auto result = snapshot_bug_data(*owner, *root, hooks);
        CHECK(!result.ok);
        CHECK(!result.gate_armed);
        CHECK(!result.gate_cleared);
        CHECK(!result.built);
        CHECK(root->child_count() == 0);
        CHECK(g_queries == 0);
        CHECK(g_adopts == 0);
        owner->clear_delegates();
    }

    // ---- 2. Armed, no screenshot source: synthetic Command, adopted. ----
    {
        g_queries = g_adopts = 0;
        auto owner = make_owner();
        std::unique_ptr<ReportItem> root = report_node_new("BugData");
        SnapshotBugDataHooks hooks = make_hooks();
        hooks.armed = true;
        hooks.arg2_present = false;
        const auto result = snapshot_bug_data(*owner, *root, hooks);
        CHECK(result.ok);
        CHECK(result.gate_armed);
        CHECK(result.gate_cleared);
        CHECK(result.built);
        CHECK(!result.screenshot_attached);
        CHECK(result.synthetic_command);
        CHECK(result.adopted);
        CHECK(root->child_count() == 1);
        const ReportItem* const command = child_at(*root, 0);
        CHECK(command != nullptr);
        CHECK(command->text() == "Command");
        CHECK(command->property("name") != nullptr && *command->property("name") == "screenshot");
        CHECK(command->first_child()->text() == "bug_screenshot");
        CHECK(g_queries == 0);
        CHECK(g_adopts == 1);
        owner->clear_delegates();
    }

    // ---- 3. Armed, source present but query fails: synthetic Command. ----
    {
        g_queries = g_adopts = 0;
        auto owner = make_owner();
        std::unique_ptr<ReportItem> root = report_node_new("BugData");
        SnapshotBugDataHooks hooks = make_hooks();
        hooks.armed = true;
        hooks.arg2_present = true;
        hooks.query_screenshot = query_false;
        const auto result = snapshot_bug_data(*owner, *root, hooks);
        CHECK(result.ok);
        CHECK(result.synthetic_command);
        CHECK(!result.screenshot_attached);
        CHECK(g_queries == 1);
        CHECK(root->child_count() == 1);
        CHECK(child_at(*root, 0)->text() == "Command");
        owner->clear_delegates();
    }

    // ---- 4. Armed, source present and query succeeds: Attachment, no Command. ----
    {
        g_queries = g_adopts = 0;
        auto owner = make_owner();
        std::unique_ptr<ReportItem> root = report_node_new("BugData");
        SnapshotBugDataHooks hooks = make_hooks();
        hooks.armed = true;
        hooks.arg2_present = true;
        hooks.query_screenshot = query_true;
        const auto result = snapshot_bug_data(*owner, *root, hooks);
        CHECK(result.ok);
        CHECK(result.screenshot_attached);
        CHECK(!result.synthetic_command);
        CHECK(g_queries == 1);
        CHECK(g_adopts == 1);
        CHECK(owner->registry().container().count2 == 0);  // cleared by phase 6
        CHECK(root->child_count() == 1);
        const ReportItem* const attachment = child_at(*root, 0);
        CHECK(attachment->text() == "Attachment");
        CHECK(attachment->property("name") != nullptr &&
              *attachment->property("name") == "Screenshot");
        CHECK(attachment->first_child()->text() == "bug_screenshot.png");
        CHECK(g_listb_frees == 1);
        owner->clear_delegates();
    }

    // ---- 5. Missing adopt hook: the report is built but not adopted. ----
    {
        auto owner = make_owner();
        std::unique_ptr<ReportItem> root = report_node_new("BugData");
        SnapshotBugDataHooks hooks = make_hooks();
        hooks.armed = true;
        hooks.adopt_root = nullptr;
        const auto result = snapshot_bug_data(*owner, *root, hooks);
        CHECK(!result.ok);
        CHECK(result.built);
        CHECK(!result.adopted);
        CHECK(result.synthetic_command);
        owner->clear_delegates();
    }

    // ---- 6. Screenshot insert without an allocator stops before the build. ----
    {
        auto owner = make_owner();
        std::unique_ptr<ReportItem> root = report_node_new("BugData");
        SnapshotBugDataHooks hooks = make_hooks();
        hooks.armed = true;
        hooks.arg2_present = true;
        hooks.query_screenshot = query_true;
        hooks.list_b_insert.alloc.allocate = nullptr;
        const auto result = snapshot_bug_data(*owner, *root, hooks);
        CHECK(!result.ok);
        CHECK(result.gate_cleared);
        CHECK(!result.built);
        CHECK(!result.screenshot_attached);
        CHECK(root->child_count() == 0);
        owner->clear_delegates();
    }

    return 0;
}
