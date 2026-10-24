// Fixture for the native lift of stat-query wrapper 0x54b250.
#include <cstdio>
#include <cstdlib>

#include "core/stat_query_54b250.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
int g_calls = 0;
void* g_this = nullptr;
std::uint32_t g_a1 = 0;
std::uint32_t g_a2 = 0;
void* g_desc = nullptr;
bool g_result = true;

bool engine_query(void* this_obj, std::uint32_t a1, std::uint32_t a2, void* desc,
                  std::uint64_t r8, std::uint64_t r9) {
    ++g_calls;
    g_this = this_obj;
    g_a1 = a1;
    g_a2 = a2;
    g_desc = desc;
    CHECK(r8 == 0 && r9 == 0);
    return g_result;
}
}  // namespace

int main() {
    CHECK(kStatQuery54b250Rva == 0x54b250);
    CHECK(kStatQuery54b250Size == 119);
    CHECK(kStatQuery54b250EndRva == 0x54b2c7);
    CHECK(kStatQuery54b250EngineRva == 0xa56560);
    CHECK(kStatQuery54b250TraitsRva == 0x15c9920);
    CHECK(kStatQuery54b250NoopSlotRva == 0x538200);
    CHECK(kStatQuery54b250GuardCellRva == 0x15fe518);
    CHECK(kStatQuery54b250DescriptorSize == 0x60);

    int self = 0;
    {
        StatsEngine54b250 e{};
        e.query = engine_query;
        g_result = true;
        const auto r = stat_query_54b250(&self, 0, 2, e);
        CHECK(r.ok);
        CHECK(r.engine_calls == 1);
        CHECK(r.noop_dispatches == 1);
        CHECK(r.engine_known && r.engine_ok);
        CHECK(r.traits_40 == 0x15c9920);
        CHECK(r.zero_30 == 0 && r.zero_48 == 0);
        CHECK(g_calls == 1);
        CHECK(g_this == &self);
        CHECK(g_a1 == 0 && g_a2 == 2);   // Init's call shape
        CHECK(g_desc != nullptr);
    }

    // Engine failure is visible.
    {
        StatsEngine54b250 e{};
        e.query = engine_query;
        g_result = false;
        const auto r = stat_query_54b250(&self, 0, 2, e);
        CHECK(!r.ok);
        CHECK(r.engine_known && !r.engine_ok);
    }

    // Missing engine contract must not look like success.
    {
        StatsEngine54b250 e{};
        const auto r = stat_query_54b250(&self, 0, 2, e);
        CHECK(!r.ok);
        CHECK(!r.engine_known);
        CHECK(r.engine_calls == 1);
        CHECK(r.noop_dispatches == 1);
    }

    std::printf("stat_query_54b250: all checks passed (engine call shape 0,2; no-op dispatch; visible failures)\n");
    return 0;
}
