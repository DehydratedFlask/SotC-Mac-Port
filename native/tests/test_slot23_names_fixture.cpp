#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/leaf_e71990.hpp"
#include "core/name_hash.hpp"
#include "core/slot23_names_fixture.hpp"
#include "core/startup_ctors.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

int main() {
    using namespace sotc;

    // Missing path fails visibly (no throw).
    {
        Slot23NamesFixture f;
        CHECK(!load_slot23_names_fixture("/no/such/slot23_fixture.bin", f));
        CHECK(!f.loaded);
    }

    // Default resolve + load (requires private fixture on disk or SOTC_SLOT23_NAMES_FIXTURE).
    {
        Slot23NamesFixture f;
        CHECK(load_slot23_names_fixture_default(f));
        CHECK(f.loaded);
        for (std::size_t i = 0; i < 13; ++i) {
            const char* p = reinterpret_cast<const char*>(static_cast<std::uintptr_t>(f.inputs.name_address[i]));
            CHECK(p != nullptr);
            CHECK(name_hash(p) == kLeafE71990Hash[i]);
        }
        CHECK(kLeafE71990Hash[13] == 0);
        LeafE71990 g{};
        g.cells.fill(0xAAu);
        init_leaf_e71990(g, f.inputs);
        for (std::size_t k = 0; k < kLeafE71990Records; ++k) {
            std::uint64_t addr = 0;
            std::memcpy(&addr, g.cells.data() + 0x10 * k, 8);
            CHECK(addr == f.inputs.name_address[k]);
            std::uint32_t hash = 0;
            std::memcpy(&hash, g.cells.data() + 0x10 * k + 8, 4);
            CHECK(hash == kLeafE71990Hash[k]);
        }
    }

    // StartupState helper: success path.
    {
        StartupState st;
        CHECK(init_slot23_leaf_e71990(st));
        CHECK(!st.init_failed);
        CHECK(st.slot23_names.loaded);
    }

    // Fail-visible when env points at a missing file.
    {
        CHECK(setenv(kSlot23NamesFixtureEnv, "/no/such/slot23_fixture.bin", 1) == 0);
        StartupState st;
        CHECK(!init_slot23_leaf_e71990(st));
        CHECK(st.init_failed);
        unsetenv(kSlot23NamesFixtureEnv);
    }

    std::puts("slot23_names_fixture: all checks passed");
    return 0;
}
