// Fixture for the native lift of ctor RVA 0x1bdb10 (0x60 B three-list container).
// Proves the three self-referential lists, zeroed counts, the untouched padding fields, and
// that the lifted insert (0x1beeb0) works on a container built by this ctor.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/container_1bdb10.hpp"
#include "core/insert_1beeb0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
alignas(16) unsigned char g_pool[0x60];

void* pool_alloc(std::size_t bytes) { return (bytes == 0x60) ? static_cast<void*>(g_pool) : nullptr; }
}  // namespace

int main() {
    CHECK(kContainer1bdb10Rva == 0x1bdb10);
    CHECK(kContainer1bdb10Size == 56);
    CHECK(kContainer1bdb10EndRva == 0x1bdb48);

    Container1beeb0 c;
    std::memset(&c, 0xAA, sizeof(c));  // every byte starts as garbage, like fresh storage

    init_container_1bdb10(c);

    // Each list is an empty ring whose sentinel is its own link pair.
    Node1beeb0* const s1 = reinterpret_cast<Node1beeb0*>(&c.list1_prev);
    Node1beeb0* const s2 = reinterpret_cast<Node1beeb0*>(&c.list2_prev);
    Node1beeb0* const s3 = reinterpret_cast<Node1beeb0*>(&c.list3_prev);
    CHECK(c.list1_prev == s1 && c.list1_next == s1);
    CHECK(c.list2_prev == s2 && c.list2_next == s2);
    CHECK(c.list3_prev == s3 && c.list3_next == s3);
    CHECK(c.count1 == 0 && c.count2 == 0 && c.count3 == 0);

    // Offsets are the recovered ones.
    CHECK(reinterpret_cast<unsigned char*>(&c.list3_prev) - reinterpret_cast<unsigned char*>(&c) == 0x40);
    CHECK(reinterpret_cast<unsigned char*>(&c.count3) - reinterpret_cast<unsigned char*>(&c) == 0x50);

    // The original performs no stores to +0x18/+0x38/+0x58.
    CHECK(c.pad1 == 0xAAAAAAAAAAAAAAAAull);
    CHECK(c.pad2 == 0xAAAAAAAAAAAAAAAAull);
    CHECK(c.pad3 == 0xAAAAAAAAAAAAAAAAull);

    // Integration: the lifted insert accepts a container built by this ctor.
    Descriptor1beeb0 d{};
    d.ctx = reinterpret_cast<std::uintptr_t>(&c);
    d.invoke = 0x1a00;
    d.offset = 0;
    d.fn = 0xb10;
    d.vtable = 0x1594060;
    d.slot_48 = 0;

    Insert1beeb0Alloc alloc{};
    alloc.alloc = pool_alloc;
    const Insert1beeb0Result r = insert_1beeb0(c, d, alloc);
    CHECK(r.ok);
    CHECK(c.count3 == 1);
    CHECK(c.list3_next == r.node);
    CHECK(r.node->prev == s3);
    CHECK(r.self_linked);
    CHECK(r.node->fn == 0xb10);

    std::printf("container_1bdb10: all checks passed (3 self loops, counts 0, pads untouched, insert works)\n");
    return 0;
}
