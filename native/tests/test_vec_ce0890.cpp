#include <cstdio>
#include <cstdlib>

#include "core/vec_ce0890.hpp"

#define CHECK(condition) \
    do { \
        if (!(condition)) { \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #condition); \
            std::exit(1); \
        } \
    } while (false)

int main(int argc, char**) {
    using namespace sotc;

    VecCe0890 value{1u, 2u, 3u, 4u};
    init_vec_ce0890(value);
    CHECK(value == kVecCe0890Bits);

    // The source constructor has no guard and rewrites all four lanes each time.
    value = {9u, 8u, 7u, 6u};
    init_vec_ce0890(value);
    CHECK(value == kVecCe0890Bits);

    if (argc > 1) {
        std::printf(
            "{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"destination\":%llu,"
            "\"rva\":%llu,\"index\":%u}\n",
            value[0], value[1], value[2], value[3],
            static_cast<unsigned long long>(0x12f7c00u),
            static_cast<unsigned long long>(0x3351c90u),
            static_cast<unsigned long long>(0xce0890u), 155u);
    } else {
        std::puts("vec_ce0890: all checks passed");
    }
    return 0;
}
