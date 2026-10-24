// Fixture for the list destructor 0x1bdeb0 (0x58-byte flag nodes).
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "core/list_dtor_1bdeb0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
int g_releases = 0;
int g_frees = 0;
int g_controls = 0;
std::vector<void*> g_freed;
std::vector<void*> g_released;

void destroy_hook(void* obj, int) { ++g_releases; g_released.push_back(obj); }
void control_hook(void* p) { ++g_controls; (void)p; }
void free_hook(void* p) { ++g_frees; g_freed.push_back(p); }
}  // namespace

int main() {
    CHECK(kListDtor1bdeb0Rva == 0x1bdeb0);
    CHECK(kListDtor1bdeb0Size == 120);
    CHECK(kListDtor1bdeb0EndRva == 0x1bdf28);
    CHECK(kListDtor1bdeb0ReleaseRva == 0xf590);
    CHECK(kListDtor1bdeb0FreeRva == 0x170c0);
    CHECK(sizeof(Node1bdeb0) == 0x58);
    CHECK(offsetof(Node1bdeb0, flag_50) == 0x50);
    CHECK(offsetof(List1bdeb0, count) == 0x10);

    ListDtor1bdeb0Hooks h{};
    h.release.destroy = destroy_hook;
    h.release.free_control = control_hook;
    h.free_tag2 = free_hook;

    // Build a list of three nodes: flags 0, 5, 0 (the middle one must be skipped).
    Node1bdeb0 n1{}, n2{}, n3{};
    n1.flag_50 = 0;
    n2.flag_50 = 5;
    n3.flag_50 = 0;
    // One live control block per callable, so each release really reaches "last reference".
    std::atomic<std::uint64_t> c10_1{1}, c30_1{1}, c10_2{1}, c30_2{1}, c10_3{1}, c30_3{1};
    n1.callable_10.control_18 = &c10_1; n1.callable_30.control_18 = &c30_1;
    n2.callable_10.control_18 = &c10_2; n2.callable_30.control_18 = &c30_2;
    n3.callable_10.control_18 = &c10_3; n3.callable_30.control_18 = &c30_3;
    for (Node1bdeb0* n : {&n1, &n2, &n3}) {
        n->callable_10.value_00 = reinterpret_cast<std::uintptr_t>(n);
        n->callable_30.value_00 = reinterpret_cast<std::uintptr_t>(n) + 1;
    }

    List1bdeb0 list{};
    Node1bdeb0* const sentinel = reinterpret_cast<Node1bdeb0*>(&list.prev);
    // Link n1 -> n2 -> n3 (prev/next), sentinel head/tail.
    list.prev = &n3;
    list.next = &n1;
    n1.prev = sentinel; n1.next = &n2;
    n2.prev = &n1;      n2.next = &n3;
    n3.prev = &n2;      n3.next = sentinel;
    list.count = 3;

    g_releases = g_frees = g_controls = 0;
    const auto r = list_dtor_1bdeb0(list, h);
    CHECK(r.ok);
    CHECK(r.erased == 2);
    CHECK(r.skipped == 1);
    CHECK(r.releases == 4);
    CHECK(r.frees == 2);
    CHECK(r.count_after == 1);
    CHECK(list.count == 1);
    CHECK(g_releases == 4);
    CHECK(g_frees == 2);
    CHECK(g_controls == 4);  // each release freed its control block
    // The skipped node's callables were never touched.
    CHECK(c10_2.load() == 1 && c30_2.load() == 1);
    // The skipped node survives and stays linked.
    CHECK(list.prev == &n2 && n2.next == sentinel && n2.prev == sentinel);
    CHECK(sentinel->next == &n2);
    // Both releases happen before the free for the same node.
    CHECK(g_released.size() == 4);
    // The walk starts at sentinel.prev, so n3 is erased first; its +0x30 callable is released first.
    CHECK(g_released[0] == reinterpret_cast<void*>(reinterpret_cast<char*>(&n3) + 1));
    CHECK(g_freed.back() == static_cast<void*>(&n1));
    CHECK(g_freed[0] == static_cast<void*>(&n3));

    // Empty list: nothing to do, and that is success.
    {
        List1bdeb0 empty{};
        empty.prev = reinterpret_cast<Node1bdeb0*>(&empty.prev);
        empty.next = empty.prev;
        const auto e = list_dtor_1bdeb0(empty, h);
        CHECK(e.ok);
        CHECK(e.erased == 0 && e.skipped == 0);
    }

    // Missing free hook: visible failure, list untouched.
    {
        ListDtor1bdeb0Hooks bad{};
        bad.release = h.release;
        List1bdeb0 l2{};
        const auto f = list_dtor_1bdeb0(l2, bad);
        CHECK(!f.ok);
        CHECK(f.erased == 0);
    }

    std::printf("list_dtor_1bdeb0: all checks passed (skip flagged, unlink, release x2, tag-2 free, count--)\n");
    return 0;
}
