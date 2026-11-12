// CPU-01-SLOT408-PREREQS: fixture for the holder dtor family. ELF pins: tools/lift/check_helper_8ef00.py. FIXTURE_VERIFIED.
#include <atomic>
#include <vector>

#include "core/helper_8ef00_holder_dtor.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

namespace {
int g_destroy = 0, g_free = 0;
void* g_destroyed = nullptr;
void on_destroy(void* object, int flags) { ++g_destroy; g_destroyed = object; CHECK(flags == 0); }
void on_free(void*) { ++g_free; }
}  // namespace

int main() {
    CHECK(kHolderDtorSize == 23 && kHolderDtorVptr == kHolderDtorClass + 0x10 && kHolderDtorHolderOffset == 8);
    CHECK(kHolderDtorTwins.size() == 7 && sizeof(HolderObject28) == 0x28);
    CHECK(offsetof(HolderObject28, holder) == 8);
    for (std::uint64_t rva : {0x34e820ull, 0x5b5470ull, 0x5b5490ull, 0x2c84f0ull, 0x8ef00ull, 0x2fced0ull, 0x2b5050ull}) CHECK(is_holder_dtor_twin(rva));
    CHECK(!is_holder_dtor_twin(0x8ef01) && !is_holder_dtor_twin(0x534580));

    const SharedReleaseF590Hooks hooks{on_destroy, on_free};
    int pointee = 0;

    // no control block: only the vptr is written, no hook runs, the holder is untouched
    for (std::uint64_t rva : kHolderDtorTwins) {
        HolderObject28 obj{};
        obj.vptr = 0xdead;
        obj.holder.object = &pointee;
        const SharedReleaseF590Result r = holder_dtor_run(rva, obj, hooks);
        CHECK(r.ok && !r.had_control && !r.destroyed && !r.freed);
        CHECK(obj.vptr == kHolderDtorVptr && obj.holder.object == &pointee);
    }
    CHECK(g_destroy == 0 && g_free == 0);

    // shared (count 2): decremented, nothing destroyed
    std::atomic<std::uint64_t> shared{2};
    HolderObject28 obj{};
    obj.holder.object = &pointee;
    obj.holder.control = &shared;
    SharedReleaseF590Result r = holder_dtor_run(0x5b5470, obj, hooks);
    CHECK(r.ok && r.decremented && !r.was_last && r.previous == 2 && shared.load() == 1);
    CHECK(g_destroy == 0 && g_free == 0);

    // last reference: destroy(pointee, 0) then free(control)
    r = holder_dtor_run(0x34e820, obj, hooks);
    CHECK(r.ok && r.was_last && r.destroyed && r.freed && r.previous == 1);
    CHECK(g_destroy == 1 && g_free == 1 && g_destroyed == &pointee);

    // last reference with no hooks: honest failure, not a fake destroy
    std::atomic<std::uint64_t> last{1};
    HolderObject28 bare{};
    bare.holder.control = &last;
    r = holder_dtor_run(0x2c84f0, bare, SharedReleaseF590Hooks{});
    CHECK(!r.ok && !r.destroyed && !r.freed);

    // an RVA outside the family writes nothing
    HolderObject28 other{};
    poison(other);
    const HolderObject28 before = other;
    r = holder_dtor_run(0x534580, other, hooks);
    CHECK(!r.ok && same_bytes(before, other));
    std::puts("OK test_helper_8ef00");
    return 0;
}
