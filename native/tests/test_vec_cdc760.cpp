#include <cstdio>
#include <cstdlib>

#include "core/vec_cdc760.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main(int argc, char**) {
    using namespace sotc;

    VecCdc760 g{1, 2, 3, 4};
    init_vec_cdc760(g);
    CHECK(g == kVecCdc760Bits);

    g = {9, 9, 9, 9};
    init_vec_cdc760(g);
    CHECK(g[0] == 0xffffffffu && g[1] == 0xffffffffu &&
          g[2] == 0xffffffffu && g[3] == 0);

    if (argc > 1) {
        std::printf(
            "{\"bits\":[%u,%u,%u,%u],\"source\":%llu,\"destination\":%llu,"
            "\"rva\":%llu,\"survey_index\":%u,\"copy_manifest_index\":%u}\n",
            kVecCdc760Bits[0], kVecCdc760Bits[1], kVecCdc760Bits[2], kVecCdc760Bits[3],
            0x12f7440ull, 0x3351c70ull, 0xcdc760ull, 157u, 99u);
    } else {
        std::puts("vec_cdc760: all checks passed");
    }

    return 0;
}
