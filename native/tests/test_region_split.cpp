#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/region_split.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main() {
    using namespace sotc;
    CHECK(RegionNode::kBytes == 0x70);
    CHECK(RegionNode::kSizeOffset == 0x58 && RegionNode::kStartOffset == 0x60);

    {
        RegionNode free{};
        free.set_start(0x20001000ull);
        free.set_size(0x30000ull);
        free.set_flag_a(0xAB);
        free.set_capacity(0x30000ull);
        RegionNode neu{};
        std::memset(neu.raw, 0xCC, sizeof(neu.raw));
        CHECK(split_free_at_lead(free, neu, 0xf000ull));
        CHECK(free.start() == 0x20001000ull);
        CHECK(free.size() == 0xf000ull && free.capacity() == 0xf000ull);
        CHECK(neu.start() == 0x20010000ull);
        CHECK(neu.size() == 0x21000ull && neu.capacity() == 0x21000ull);
        CHECK(neu.flag_a() == 0xAB && neu.flag_b() == 0);
        CHECK(neu.ptr_field() == 0 && neu.sentinel() == 0xffffffffu);
    }
    {
        RegionNode a{}, b{};
        CHECK(!split_free_at_lead(a, b, 0));
    }
    {
        RegionNode free{};
        free.set_start(0x20001000ull);
        free.set_size(0x30000ull);
        free.set_flag_a(1);
        RegionNode neu{};
        auto fit = split_using_carve_fit(free, neu, 0x10000ull, 0x10000u);
        CHECK(fit && fit->lead == 0xf000ull);
        CHECK(free.size() == fit->lead);
        CHECK(neu.start() == fit->allocated_start);
        CHECK(neu.size() == fit->allocated_size);
        CHECK(neu.size() >= 0x10000ull);
    }
    {
        RegionNode free{};
        free.set_start(0x20000000ull);
        free.set_size(0x400000ull);
        RegionNode neu{};
        CHECK(!split_using_carve_fit(free, neu, 0x100000ull, 0x100000u).has_value());
        CHECK(free.size() == 0x400000ull);
    }
    std::puts("region_split: all checks passed");
    return 0;
}
