// Fixture for the shared-ownership release 0xf590.
#include <atomic>
#include <cstdio>
#include <cstdlib>

#include "core/shared_release_f590.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
int g_destroyed = 0;
int g_freed = 0;
void* g_destroy_obj = nullptr;
void* g_freed_ptr = nullptr;

void destroy_hook(void* obj, int flags) {
    ++g_destroyed;
    g_destroy_obj = obj;
    CHECK(flags == 0);
}
void free_hook(void* p) {
    ++g_freed;
    g_freed_ptr = p;
}
}  // namespace

int main() {
    CHECK(kSharedReleaseF590Rva == 0xf590);
    CHECK(kSharedReleaseF590Size == 73);
    CHECK(kSharedReleaseF590EndRva == 0xf5d9);
    CHECK(kSharedReleaseF590DestroyRva == 0x12af0);
    CHECK(kSharedReleaseF590FreeRva == 0x170b0);
    CHECK(kSharedReleaseF590ControlOffset == 0x18);

    SharedReleaseF590Hooks h{};
    h.destroy = destroy_hook;
    h.free_control = free_hook;

    // Two references: decrement only, nothing destroyed.
    {
        std::atomic<std::uint64_t> count{2};
        SharedHolderF590 holder{};
        int obj = 0;
        holder.object = &obj;
        holder.control = &count;
        g_destroyed = g_freed = 0;
        const auto r = shared_release_f590(holder, h);
        CHECK(r.ok);
        CHECK(r.had_control && r.decremented);
        CHECK(!r.was_last);
        CHECK(r.previous == 2);
        CHECK(count.load() == 1);
        CHECK(g_destroyed == 0 && g_freed == 0);
    }

    // Last reference: destroy then free the control block.
    {
        std::atomic<std::uint64_t> count{1};
        SharedHolderF590 holder{};
        int obj = 0;
        holder.object = &obj;
        holder.control = &count;
        g_destroyed = g_freed = 0;
        const auto r = shared_release_f590(holder, h);
        CHECK(r.ok);
        CHECK(r.was_last);
        CHECK(r.previous == 1);
        CHECK(r.destroyed && r.freed);
        CHECK(count.load() == 0);
        CHECK(g_destroyed == 1 && g_destroy_obj == &obj);
        CHECK(g_freed == 1 && g_freed_ptr == &count);
    }

    // No control block: nothing to do, and that is success (the original's `je`).
    {
        SharedHolderF590 holder{};
        holder.control = nullptr;
        const auto r = shared_release_f590(holder, h);
        CHECK(r.ok);
        CHECK(!r.had_control && !r.decremented);
    }

    // Last reference with a missing destroy hook: visible failure, not a fake release.
    {
        std::atomic<std::uint64_t> count{1};
        SharedHolderF590 holder{};
        holder.control = &count;
        SharedReleaseF590Hooks partial{};
        const auto r = shared_release_f590(holder, partial);
        CHECK(!r.ok);
        CHECK(r.was_last && r.decremented);
        CHECK(!r.destroyed && !r.freed);
        CHECK(count.load() == 0);  // the atomic decrement really happened
    }

    std::printf("shared_release_f590: all checks passed (atomic decrement, last-ref destroy+free, visible failures)\n");
    return 0;
}
