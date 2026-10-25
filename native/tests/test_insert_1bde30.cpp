// Fixture for the 18-byte remarshal 0x1bde30: it must produce exactly the insert's effect.
#include <cstdio>
#include <cstdlib>

#include "core/container_1bdb10.hpp"
#include "core/insert_1bde30.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
alignas(16) unsigned char g_pool[0x60];
void* pool_alloc(std::size_t bytes) { return (bytes == 0x60) ? static_cast<void*>(g_pool) : nullptr; }
}  // namespace

int main() {
    CHECK(kInsert1bde30Rva == 0x1bde30);
    CHECK(kInsert1bde30Size == 18);
    CHECK(kInsert1bde30EndRva == 0x1bde42);
    CHECK(kInsert1bde30TailTargetRva == 0x1beeb0);

    Container1beeb0 c;
    init_container_1bdb10(c);

    Descriptor1beeb0 d{};
    d.ctx = 0x11;
    d.invoke = 0x1a00;
    d.offset = 0;
    d.fn = 0xb10;
    d.vtable = 0x1594060;
    d.slot_48 = 0x77;
    d.context[2] = 0xCAFEF00Dull;

    Insert1beeb0Alloc alloc{};
    alloc.alloc = pool_alloc;
    const auto r = insert_1bde30(c, d, alloc);
    CHECK(r.ok);
    CHECK(r.node != nullptr);
    CHECK(r.self_linked);
    CHECK(r.node->invoke == 0x1a00);
    CHECK(r.node->fn == 0xb10);
    CHECK(r.node->callable_value == 0xCAFEF00Dull);
    CHECK(r.node->slot_58 == 0x77);
    CHECK(c.count3 == 1);

    // The remarshal's only job is the argument adjustment: the sentinel is container+0x40.
    Node1beeb0* const sentinel = reinterpret_cast<Node1beeb0*>(&c.list3_prev);
    CHECK(r.node->prev == sentinel);

    std::printf("insert_1bde30: all checks passed (remarshal -> insert, sentinel = &container+0x40)\n");
    return 0;
}
