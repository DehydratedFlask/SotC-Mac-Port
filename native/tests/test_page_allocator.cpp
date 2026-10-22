// Tests for the page-allocator layer and the ctx-aware host_map wrapper.
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

#include "core/host_map.hpp"
#include "core/page_allocator.hpp"

#define CHECK(c)                                                              \
    do {                                                                      \
        if (!(c)) {                                                           \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);  \
            std::exit(1);                                                     \
        }                                                                     \
    } while (0)

int main() {
    using namespace sotc;

    // The gap: host_map rejects a nonzero base (ctx), the wrapper resolves it.
    {
        AllocatorRegionRequest r{};
        r.preferred_base = 0x1234;  // a ctx pointer in the original
        r.size = 0x10000;
        r.alignment = 8;
        CHECK(!map_region(r).has_value());

        CtxRegionRequest c;
        c.ctx = 0x1234;
        c.size = 1;
        c.alignment = 8;
        c.flags = 1;
        auto m = map_region_with_ctx(c);
        CHECK(m.has_value());
        CHECK(m->ctx == 0x1234);
        CHECK(m->mapped.region.size == 0x10000);  // 64 KiB rounding (0x479a0)
        CHECK(m->mapped.region.start % 0x100000 == 0);  // >= 1 MiB reserve alignment
        auto* b = reinterpret_cast<unsigned char*>(m->mapped.region.start);
        b[0] = 1;
        b[0xffff] = 2;  // the whole rounded range is usable
        unmap(m->mapped.mapping);

        c.size = 0x10001;
        m = map_region_with_ctx(c);
        CHECK(m && m->mapped.region.size == 0x20000);
        unmap(m->mapped.mapping);
        c.size = 0x10000;
        m = map_region_with_ctx(c);
        CHECK(m && m->mapped.region.size == 0x10000);  // exact multiple unchanged
        unmap(m->mapped.mapping);

        c.size = 0;
        CHECK(!map_region_with_ctx(c));
        c.size = ~std::uint64_t{0};  // rounding wraps to 0
        CHECK(!map_region_with_ctx(c));
        c.size = ~std::uint64_t{0} - 0x20000;  // rounds fine but cannot be mapped
        CHECK(!map_region_with_ctx(c));
        c.size = 0x10000;
        c.alignment = 0x300000;  // not a power of two after host rounding
        CHECK(!map_region_with_ctx(c));
    }

    // PageAllocator basics.
    {
        PageOwnerMap owners;
        PageAllocator pa(owners, 7, 3);
        const AllocDesc d{kAllocFlagNew, 0};
        void* p = pa.allocate(100, 8, d);
        CHECK(p != nullptr);
        CHECK(reinterpret_cast<std::uintptr_t>(p) % 0x10000 == 0);
        CHECK(pa.owns_block(p));
        CHECK(owners.lookup(p) == 7u);
        // whole rounded size is registered, byte after is not
        CHECK(owners.lookup(static_cast<char*>(p) + 0xffff) == 7u);
        CHECK(owners.lookup(static_cast<char*>(p) + 0x10000) == std::nullopt);
        CHECK(pa.stats().live_blocks == 1 && pa.stats().bytes_in_use == 0x10000);
        std::memset(p, 0xcd, 0x10000);

        // OS-zero pages are not a contract; only "block start" matters for free.
        pa.deallocate(static_cast<char*>(p) + 16, d);  // interior pointer ignored
        CHECK(pa.stats().ignored_frees == 1 && pa.owns_block(p));
        pa.deallocate(nullptr, d);
        CHECK(pa.stats().ignored_frees == 1);

        void* big = pa.allocate(3u * 1024 * 1024 + 1, 0x200000, d);
        CHECK(big != nullptr);
        CHECK(reinterpret_cast<std::uintptr_t>(big) % 0x200000 == 0);
        CHECK(pa.stats().bytes_in_use == 0x10000u + 0x310000u);
        CHECK(pa.stats().peak_bytes == 0x320000u);
        pa.deallocate(big, d);
        CHECK(!pa.owns_block(big));
        CHECK(owners.lookup(big) == std::nullopt);
        CHECK(pa.stats().bytes_in_use == 0x10000u && pa.stats().peak_bytes == 0x320000u);
        pa.deallocate(big, d);  // double free ignored
        CHECK(pa.stats().ignored_frees == 2);

        CHECK(pa.allocate(0, 8, d) != nullptr);  // size 0 tolerated
        CHECK(pa.allocate(64, std::uint64_t{1} << 33, d) == nullptr);  // align > u32
        CHECK(pa.allocate(~std::size_t{0}, 8, d) == nullptr);
        CHECK(pa.stats().live_blocks == 2);
        // destructor unmaps + unregisters the two live blocks
        CHECK(owners.range_count() == 2);
    }

    // Destructor cleanup observed through the shared map.
    {
        PageOwnerMap owners;
        {
            PageAllocator pa(owners, 2);
            CHECK(pa.allocate(1, 8, {}) != nullptr);
            CHECK(owners.range_count() == 1);
        }
        CHECK(owners.range_count() == 0);
    }

    // Thread safety: concurrent alloc/free on one allocator; blocks never overlap/lose owner.
    {
        PageOwnerMap owners;
        PageAllocator pa(owners, 9);
        std::atomic<int> bad{0};
        std::vector<std::thread> ts;
        for (int t = 0; t < 4; ++t)
            ts.emplace_back([&, t] {
                for (int i = 0; i < 40; ++i) {
                    auto* p = static_cast<unsigned char*>(pa.allocate(70000 + t, 8, {}));
                    if (!p) { ++bad; continue; }
                    std::memset(p, t + 1, 70000);
                    if (owners.lookup(p) != 9u) ++bad;
                    if (p[0] != t + 1 || p[69999] != t + 1) ++bad;
                    pa.deallocate(p, {});
                }
            });
        for (auto& th : ts) th.join();
        CHECK(bad == 0);
        CHECK(pa.stats().live_blocks == 0 && pa.stats().bytes_in_use == 0);
        CHECK(owners.range_count() == 0);
    }
    std::puts("page_allocator OK");
    return 0;
}
