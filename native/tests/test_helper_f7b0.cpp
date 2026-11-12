// CPU-01-SLOT408-PREREQS: fixture for the 0xf7b0 twin. ELF pins: tools/lift/check_helper_f7b0.py. FIXTURE_VERIFIED.
#include <atomic>

#include "core/helper_f7b0_shared_release_twin.hpp"
#include "helper_test_util.hpp"

using namespace sotc;

namespace {
int g_destroy = 0, g_free = 0;
void on_destroy(void*, int flags) { ++g_destroy; CHECK(flags == 0); }
void on_free(void*) { ++g_free; }
}  // namespace

int main() {
    CHECK(kSharedReleaseTwinF7b0Rva == 0xf7b0 && kSharedReleaseTwinF7b0Size == 73);
    const SharedReleaseF590Hooks hooks{on_destroy, on_free};
    int pointee = 0;
    SharedHolderF590 holder{&pointee, nullptr, nullptr, nullptr};
    SharedReleaseF590Result r = shared_release_f7b0(holder, hooks);
    CHECK(r.ok && !r.had_control && g_destroy == 0);                       // no control block: nothing happens
    std::atomic<std::uint64_t> count{2};
    holder.control = &count;
    r = shared_release_f7b0(holder, hooks);
    CHECK(r.ok && r.decremented && !r.was_last && r.previous == 2 && count.load() == 1 && g_destroy == 0);
    r = shared_release_f7b0(holder, hooks);
    CHECK(r.ok && r.was_last && r.destroyed && r.freed && g_destroy == 1 && g_free == 1);
    // identical outcome to the 0xf590 model on the same input
    std::atomic<std::uint64_t> a{1}, b{1};
    SharedHolderF590 ha{&pointee, nullptr, nullptr, &a}, hb{&pointee, nullptr, nullptr, &b};
    const SharedReleaseF590Result ra = shared_release_f7b0(ha, hooks), rb = shared_release_f590(hb, hooks);
    CHECK(ra.ok == rb.ok && ra.was_last == rb.was_last && ra.destroyed == rb.destroyed && ra.freed == rb.freed && ra.previous == rb.previous);
    CHECK(ra.destroyed && ra.freed && ra.previous == 1);
    std::puts("OK test_helper_f7b0");
    return 0;
}
