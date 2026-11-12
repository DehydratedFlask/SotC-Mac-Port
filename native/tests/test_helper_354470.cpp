// CPU-01-SLOT408-PREREQS: fixture for the ring dtor 0x354470. ELF pins: tools/lift/check_helper_354470.py.
#include <atomic>
#include <vector>

#include "core/helper_1f220_ring_header.hpp"
#include "core/helper_354470_ring_dtor.hpp"
#include "helper_test_util.hpp"

using namespace sotc;

namespace {
std::vector<void*> g_freed;
int g_destroy = 0, g_ctrl_free = 0;
void on_free2(void* p) { g_freed.push_back(p); }
void on_destroy(void*, int) { ++g_destroy; }
void on_ctrl(void*) { ++g_ctrl_free; }

void put(std::uint8_t* base, std::size_t off, void* value) { std::memcpy(base + off, &value, sizeof value); }
}  // namespace

int main() {
    CHECK(kRingDtor354470Rva == 0x354470 && kRingDtor354470Size == 116 && kRingDtorElementSize == 0x98);
    CHECK(kRingDtorElementHolderOffset == 8 && kRingDtorElementBufferOffset == 0x30 && kRingDtorElementInlineOffset == 0x50);
    CHECK(kRingDtorInlineStorageOffset == 0x20);
    const RingDtorHooks hooks{on_free2, {on_destroy, on_ctrl}};

    // the 0x1f220-initialised empty ring: nothing to free, begin == the inline storage
    alignas(16) static std::uint8_t ring[0x2640];
    ring_header_init_1f220(ring);
    RingDtorResult r = ring_dtor_354470(ring, hooks);
    CHECK(r.ok && r.elements == 0 && !r.ring_buffer_freed && g_freed.empty());
    CHECK(ring_dtor_354470(ring, RingDtorHooks{}).ok);          // nothing reachable, so no hook is needed

    // three inline elements: [0] heap string + shared(1); [1] string == its inline buffer, shared(2); [2] null, no control
    std::uint8_t* const e0 = ring + 0x28;
    std::uint8_t* const e1 = e0 + 0x98;
    std::uint8_t* const e2 = e1 + 0x98;
    int heap = 0, pointee = 0;
    std::atomic<std::uint64_t> c0{1}, c1{2};
    put(e0, 0x30, &heap); put(e0, 0x50, e0 + 0x60); put(e0, 0x08, &pointee); put(e0, 0x20, &c0);
    put(e1, 0x30, e1 + 0x60); put(e1, 0x50, e1 + 0x60); put(e1, 0x08, &pointee); put(e1, 0x20, &c1);
    put(ring, 0x08, e2 + 0x98);                                  // end = begin + 3 elements
    r = ring_dtor_354470(ring, hooks);
    CHECK(r.ok && r.elements == 3 && r.buffers_freed == 1 && r.releases_last == 1 && !r.ring_buffer_freed);
    CHECK(g_freed.size() == 1 && g_freed[0] == &heap);
    CHECK(g_destroy == 1 && g_ctrl_free == 1 && c1.load() == 1);

    // a heap ring buffer (begin != [obj+0x20]) is freed last
    alignas(8) std::uint8_t object[0x30] = {};
    alignas(8) std::uint8_t heap_ring[0x20];
    put(object, 0x00, heap_ring); put(object, 0x08, heap_ring); put(object, 0x20, ring + 0x28);
    g_freed.clear();
    r = ring_dtor_354470(object, hooks);
    CHECK(r.ok && r.elements == 0 && r.ring_buffer_freed && g_freed.size() == 1 && g_freed[0] == heap_ring);
    CHECK(!ring_dtor_354470(object, RingDtorHooks{}).ok);        // reachable free with no hook
    std::puts("OK test_helper_354470");
    return 0;
}
