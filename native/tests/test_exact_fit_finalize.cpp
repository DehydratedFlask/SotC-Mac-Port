#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/exact_fit_finalize.hpp"
#include "core/free_list_carve.hpp"

#define CHECK(c)                                                                          \
    do {                                                                                  \
        if (!(c)) {                                                                       \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);               \
            std::exit(1);                                                                 \
        }                                                                                 \
    } while (0)

int main() {
    using namespace sotc;

    // Carve exact_node math alone does not set flag_b/ptr/sentinel — finalize does.
    {
        constexpr std::uint64_t exact_request = 0x20000;
        auto fit = try_carve_fit(/*free_start=*/0x100000, /*free_size=*/exact_request,
                                 /*req_size=*/exact_request, /*alignment=*/0x10000);
        CHECK(fit.has_value());
        CHECK(fit->exact_node);
        CHECK(fit->lead == 0);

        RegionNode exact{};
        exact.set_start(0x100000);
        exact.set_size(exact_request);
        exact.set_capacity(exact_request);
        exact_fit_finalize(exact, exact_request, /*arena_flag=*/0x3C,
                           /*ctx=*/0xDEADBEEFULL);
        CHECK(exact.start() == 0x100000);
        CHECK(exact.capacity() == exact_request);
        CHECK(exact.size() == exact_request);
        CHECK(exact.flag_a() == 0x3C);
        CHECK(exact.flag_b() == 1);
        CHECK(exact.ptr_field() == 0xDEADBEEFULL);
        CHECK(exact.sentinel() == 0xffffffffu);
    }

    RegionNode n{};
    n.set_start(0x100000);
    n.set_size(0x30000);
    n.set_capacity(0x30000);
    n.set_flag_a(0xAB);
    n.raw[RegionNode::kFlagBOffset] = 0;
    std::uint64_t z = 0;
    std::memcpy(n.raw + RegionNode::kPtrOffset, &z, 8);
    std::uint32_t s0 = 0;
    std::memcpy(n.raw + RegionNode::kSentinelOffset, &s0, 4);

    exact_fit_finalize(n, /*req_size=*/0x18000, /*arena_flag=*/0x3C, /*ctx=*/0xDEADBEEFULL);

    CHECK(n.start() == 0x100000);  // untouched
    CHECK(n.capacity() == 0x30000);
    CHECK(n.size() == 0x18000);
    CHECK(n.flag_a() == 0x3C);
    CHECK(n.flag_b() == 1);
    CHECK(n.ptr_field() == 0xDEADBEEFULL);
    CHECK(n.sentinel() == 0xffffffffu);

    // Null ctx path (OBS rsi==0 after test).
    exact_fit_finalize(n, 0x1000, 0x01, 0);
    CHECK(n.size() == 0x1000);
    CHECK(n.flag_a() == 0x01);
    CHECK(n.flag_b() == 1);
    CHECK(n.ptr_field() == 0);
    CHECK(n.sentinel() == 0xffffffffu);

    // Contrast: split remainder uses flag_b=0 and ptr=0 (already lifted).
    RegionNode free{}, neu{};
    free.set_start(0x200000);
    free.set_size(0x40000);
    free.set_capacity(0x40000);
    free.set_flag_a(0x55);
    CHECK(split_free_at_lead(free, neu, 0x10000));
    CHECK(neu.flag_b() == 0);
    CHECK(neu.ptr_field() == 0);
    CHECK(neu.sentinel() == 0xffffffffu);

    std::puts("exact_fit_finalize: all checks passed");
    return 0;
}
