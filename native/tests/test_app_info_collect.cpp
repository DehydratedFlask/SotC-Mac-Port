// Fixture for the list-A half of 0x1bdf30: FIFO serialization, flag==1 drain, flag!=1 retention,
// release order and the missing-hook failure surface.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

#include "core/app_info_collect.hpp"
#include "core/app_info_registry.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
int g_allocs = 0;
int g_frees = 0;

void* test_allocate(std::size_t size, std::uint32_t tag, std::uint32_t align, std::uint32_t flags) {
    ++g_allocs;
    (void)tag;
    (void)align;
    (void)flags;
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

void free_tag2(void* ptr) {
    CHECK(test_deallocate(ptr, 2));
}

struct VisitLog {
    std::vector<std::string> names;
    std::vector<std::string> values;
};

void visit(void* ctx, std::string_view name, std::string_view value) {
    auto* const log = static_cast<VisitLog*>(ctx);
    log->names.emplace_back(name);
    log->values.emplace_back(value);
}
}  // namespace

int main() {
    CHECK(kAppInfoCollectReaderRva == 0x1be14d);
    CHECK(kAppInfoDrainRva == 0x1be3f4);
    CHECK(kAppInfoDrainFlag == 1);

    Insert1bdb50Hooks insert{};
    insert.alloc.allocate = test_allocate;
    insert.alloc.deallocate = test_deallocate;

    // ---- 1. FIFO visit, then drain only flag==1. ----
    {
        AppInfoRegistry registry;
        CHECK(registry.add_string_entry("a", "1", 1, insert).ok);
        CHECK(registry.add_string_entry("b", "2", 1, insert).ok);
        CHECK(registry.add_string_entry("c", "3", 0, insert).ok);
        CHECK(registry.add_string_entry("d", "4", 1, insert).ok);
        CHECK(registry.entry_count() == 4);
        g_frees = 0;
        VisitLog log;
        AppInfoCollectHooks hooks{};
        hooks.free_tag2 = free_tag2;
        const auto result = collect_app_info(registry, visit, &log, hooks);
        CHECK(result.ok);
        CHECK(result.visited == 4);
        CHECK(result.drained == 3);
        CHECK(result.releases == 6);
        CHECK(result.release_failures == 0);
        CHECK(result.count_after == 1);
        CHECK(log.names.size() == 4);
        CHECK(log.names[0] == "a" && log.values[0] == "1");
        CHECK(log.names[1] == "b" && log.values[1] == "2");
        CHECK(log.names[2] == "c" && log.values[2] == "3");
        CHECK(log.names[3] == "d" && log.values[3] == "4");
        CHECK(g_frees == 3);
        CHECK(registry.entry_count() == 1);

        // The survivor is the flag=0 entry; the 0x1bdeb0 purge removes it.
        ListDtor1bdeb0Hooks dtor{};
        dtor.free_tag2 = free_tag2;
        const auto purge = registry.clear(dtor);
        CHECK(purge.ok);
        CHECK(purge.erased == 1);
        CHECK(registry.entry_count() == 0);
        CHECK(g_frees == 4);
    }

    // ---- 2. A second collect sees an empty list; no duplicate entries. ----
    {
        AppInfoRegistry registry;
        CHECK(registry.add_string_entry("x", "y", 1, insert).ok);
        VisitLog first;
        VisitLog second;
        AppInfoCollectHooks hooks{};
        hooks.free_tag2 = free_tag2;
        CHECK(collect_app_info(registry, visit, &first, hooks).ok);
        CHECK(first.names.size() == 1);
        const auto again = collect_app_info(registry, visit, &second, hooks);
        CHECK(again.ok);
        CHECK(again.visited == 0);
        CHECK(again.drained == 0);
        CHECK(second.names.empty());
    }

    // ---- 3. Missing free hook or visitor: visible failure, nothing touched. ----
    {
        AppInfoRegistry registry;
        CHECK(registry.add_string_entry("k", "v", 1, insert).ok);
        VisitLog log;
        AppInfoCollectHooks no_free{};
        const auto failed = collect_app_info(registry, visit, &log, no_free);
        CHECK(!failed.ok);
        CHECK(failed.visited == 0);
        CHECK(registry.entry_count() == 1);
        AppInfoCollectHooks hooks{};
        hooks.free_tag2 = free_tag2;
        const auto no_visitor = collect_app_info(registry, nullptr, nullptr, hooks);
        CHECK(!no_visitor.ok);
        CHECK(registry.entry_count() == 1);
    }

    // ---- 4. A last-reference release failure is reported but the node is still freed. ----
    {
        AppInfoRegistry registry;
        auto* control = static_cast<std::atomic<std::uint64_t>*>(
            std::malloc(sizeof(std::atomic<std::uint64_t>)));
        CHECK(control != nullptr);
        new (control) std::atomic<std::uint64_t>(1);
        Callable1bdeb0 name{};
        name.value_00 = reinterpret_cast<std::uintptr_t>("n");
        name.value_08 = 1;
        Callable1bdeb0 value{};
        value.value_00 = reinterpret_cast<std::uintptr_t>("v");
        value.value_08 = 1;
        value.control_18 = control;
        CHECK(registry.add_callable_entry(name, value, 1, insert).ok);
        CHECK(control->load() == 2);
        control->fetch_sub(1);  // the caller hands its own reference to the node
        CHECK(control->load() == 1);
        VisitLog log;
        AppInfoCollectHooks hooks{};
        hooks.free_tag2 = free_tag2;
        // No destroy hook: the last reference cannot be completed.
        const auto result = collect_app_info(registry, visit, &log, hooks);
        CHECK(!result.ok);
        CHECK(result.drained == 1);
        CHECK(result.release_failures == 1);
        CHECK(registry.entry_count() == 0);
        std::free(control);
    }

    return 0;
}
