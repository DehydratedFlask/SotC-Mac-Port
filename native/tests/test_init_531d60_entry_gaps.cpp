// Fixture for Init 0x531d60's four remaining spans (entry frame, base-initialize call site,
// ret-nop call site, je-not-taken jump).
#include <cstdio>
#include <cstdlib>

#include "core/init_531d60_entry_gaps.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
int g_base_calls = 0;
int g_retnop_calls = 0;
void* g_base_self = nullptr;
bool g_base_return = true;

bool base_hook(void* self, void*) {
    ++g_base_calls;
    g_base_self = self;
    return g_base_return;
}

void retnop_hook() { ++g_retnop_calls; }
}  // namespace

int main() {
    // Pinned constants.
    CHECK(kInitEntrySpanARva == 0x531d60 && kInitEntrySpanASize == 36);
    CHECK(kInitEntrySpanBRva == 0x531dcf && kInitEntrySpanBSize == 16);
    CHECK(kInitEntrySpanCRva == 0x531df9 && kInitEntrySpanCSize == 5);
    CHECK(kInitEntrySpanDRva == 0x5327b1 && kInitEntrySpanDSize == 5);
    CHECK(kInitEntryGuardCellRva == 0x15fe518);
    CHECK(kInitEntryBaseInitRva == 0x255130);
    CHECK(kInitEntryRetNopRva == 0x56dca0);
    CHECK(kInitEntryJumpTargetRva == 0x532904);
    CHECK(kInitEntrySpans.size() == 4);

    int self_obj = 0;
    {
        InitEntryGapsHooks h{};
        h.call_base_initialize = base_hook;
        h.call_ret_nop_56dca0 = retnop_hook;
        g_base_return = true;
        const auto r = init_531d60_entry_gaps(&self_obj, h);
        CHECK(r.ok);
        CHECK(r.units_executed == 17);
        CHECK(r.guard_loads == 1);
        CHECK(r.self_saves == 1);
        CHECK(r.self_reloads == 1);
        CHECK(r.base_call_count == 1);
        CHECK(r.ret_nop_count == 1);
        CHECK(r.jump_pins == 1);
        CHECK(r.saved_self == &self_obj);
        CHECK(g_base_self == &self_obj);  // the saved self round-trips into rdi
        CHECK(r.base_ok);
        CHECK(r.jump_target_rva == 0x532904);
        CHECK(g_base_calls == 1);
        CHECK(g_retnop_calls == 1);
    }

    // Base Initialize returning false must be visible, not treated as success.
    {
        InitEntryGapsHooks h{};
        h.call_base_initialize = base_hook;
        g_base_return = false;
        g_base_calls = 0;
        const auto r = init_531d60_entry_gaps(&self_obj, h);
        CHECK(r.ok);
        CHECK(!r.base_ok);
        CHECK(g_base_calls == 1);
    }

    // Missing callees are counted, never silently successful.
    {
        InitEntryGapsHooks h{};
        const auto r = init_531d60_entry_gaps(&self_obj, h);
        CHECK(r.ok);           // the span pin is still complete
        CHECK(!r.base_ok);     // but the callee result is unknown/false
        CHECK(r.base_call_count == 1);
        CHECK(r.ret_nop_count == 1);
    }

    std::printf("init_531d60_entry_gaps: all checks passed (4 spans, self round-trip, base_ok visible)\n");
    return 0;
}
