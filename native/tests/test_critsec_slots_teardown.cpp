// RE-02-CRITSEC-3B310-3B380-NATIVE-01: fixture for the native CritSec handle and the slot-414/535
// registered-destroy teardown. Fail-visible cases only: every rejection must name its stage, leave the
// 8-byte handle byte-identical and stay retryable. No retail contention or teardown-ordering claim.
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>

#include "core/critsec_3b310.hpp"
#include "core/ctor_tiny_misc_slots.hpp"

#define CHECK(c)                                                                                   \
    do {                                                                                           \
        if (!(c)) {                                                                                \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);                       \
            std::exit(1);                                                                          \
        }                                                                                          \
    } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kFill = 0xA5;

bool stage_is(const TinyMiscResult& r, const char* stage) {
    return !r.ok && r.blocked != nullptr && std::strcmp(r.blocked, stage) == 0;
}

std::uint64_t load_u64(const std::uint8_t* p) {
    std::uint64_t v = 0;
    std::memcpy(&v, p, sizeof(v));
    return v;
}

// Destroy hooks with observable results.
int g_destroy_calls = 0;
void* g_destroy_arg = nullptr;

bool destroy_ok(void* obj) noexcept {
    ++g_destroy_calls;
    g_destroy_arg = obj;
    return true;
}

bool destroy_fail(void* obj) noexcept {
    ++g_destroy_calls;
    g_destroy_arg = obj;
    return false;
}

}  // namespace

int main() {
    // ---- byte-pinned contracts ---------------------------------------------------------------
    CHECK(kCritSecCtorRva == 0x3b310 && kCritSecDtorRva == 0x3b380);
    CHECK(kCritSecHandleBytes == 8);
    CHECK(kCritSecCtorFdeLength == 107 && kCritSecDtorFdeLength == 5);
    CHECK(kCritSecSettypeValue == 2);
    CHECK(!kCritSecUsesPriorityInheritance);
    CHECK(kSlot414DtorRva == 0x3b380 && kSlot535MutexDtorRva == 0x3b380);
    CHECK(kSlot414ObjectSize == 0x20 && kSlot535MutexSize == 8);

    // ---- CritSec handle: rejections are visible and do not consume the handle ----------------
    {
        const std::size_t before = critsec_live_sidecars();
        CHECK(critsec_init_at(nullptr) == EINVAL);
        CHECK(critsec_destroy_at(nullptr) == EINVAL);

        CritSecHandle unknown{};
        CHECK(critsec_destroy_at(unknown.bytes) == EINVAL);        // zero token: unknown
        CHECK(critsec_lock_at(unknown.bytes) == EINVAL);
        CHECK(critsec_unlock_at(unknown.bytes) == EINVAL);
        CHECK(critsec_live_sidecars() == before);                  // no registry growth

        CritSecHandle bogus{};
        const std::uint64_t never_issued = 0xDEADBEEFCAFEF00Dull;
        std::memcpy(bogus.bytes, &never_issued, sizeof(never_issued));
        CHECK(critsec_destroy_at(bogus.bytes) == EINVAL);          // unknown token
        CHECK(load_u64(bogus.bytes) == never_issued);              // handle bytes untouched

        CritSecHandle h{};
        CHECK(h.initialize() == 0);
        CHECK(critsec_live_sidecars() == before + 1);
        const std::uint64_t token = load_u64(h.bytes);
        CHECK(token != 0);

        CHECK(h.initialize() == EALREADY);                         // re-init is rejected
        CHECK(critsec_live_sidecars() == before + 1);              // and does not add a sidecar
        CHECK(load_u64(h.bytes) == token);                         // token retained

        CHECK(h.lock() == 0);
        CHECK(h.lock() == 0);                                      // recursive
        CHECK(h.destroy() == EBUSY);                               // held: refused
        CHECK(load_u64(h.bytes) == token);                         // handle retained for retry
        CHECK(h.unlock() == 0);
        CHECK(h.unlock() == 0);
        CHECK(h.destroy() == 0);                                   // retry succeeds
        CHECK(load_u64(h.bytes) == 0);                             // handle cleared
        CHECK(critsec_live_sidecars() == before);                  // sidecar released
        CHECK(h.destroy() == EINVAL);                              // second destroy is visible

        // An unzeroed (garbage) cell is overwritten, not rejected: the retail shim never reads the field.
        CritSecHandle junk{};
        std::memset(junk.bytes, 0xA5, sizeof(junk.bytes));
        CHECK(junk.initialize() == 0);
        CHECK(critsec_live_sidecars() == before + 1);
        CHECK(junk.destroy() == 0);
        CHECK(critsec_live_sidecars() == before);
    }

    // ---- slot 414: the constructor never runs the registered destroy ------------------------
    {
        auto state = std::make_unique<Slot414State>();
        std::memset(state->obj.data(), kFill, state->obj.size());
        StartupAtexitLog log;
        const Slot414Hooks hooks = default_critsec_slot414_hooks();
        CHECK(hooks.mutex_init_3b310 != nullptr && hooks.mutex_destroy_3b380 != nullptr);

        CHECK(init_slot414_57b9b0(*state, hooks, log).ok);
        CHECK(log.size() == 1 && log.at(0).dtor_rva == 0x3b380 && log.at(0).object == state->obj.data());
        const std::uint64_t token = load_u64(state->obj.data());
        CHECK(token != 0);
        // The handle is still live after the constructor - proof the ctor did not destroy it.
        CHECK(critsec_lock_at(state->obj.data()) == 0 && critsec_unlock_at(state->obj.data()) == 0);

        // Missing destroy hook: visible, handle untouched, log untouched.
        CHECK(stage_is(teardown_slot414_57b9b0(*state, Slot414Hooks{}, log), "mutex_destroy_hook"));
        CHECK(load_u64(state->obj.data()) == token);

        // Hook present but the log no longer holds the identity: refused before touching the handle.
        StartupAtexitLog empty;
        g_destroy_calls = 0;
        Slot414Hooks counted = hooks;
        counted.mutex_destroy_3b380 = destroy_ok;
        CHECK(stage_is(teardown_slot414_57b9b0(*state, counted, empty), "atexit_identity"));
        CHECK(g_destroy_calls == 0);
        CHECK(load_u64(state->obj.data()) == token);

        // Log holds the identity for a different object: still refused.
        StartupAtexitLog wrong_object;
        CHECK(wrong_object.record(0x3b380, state->obj.data() + 1));
        CHECK(stage_is(teardown_slot414_57b9b0(*state, counted, wrong_object), "atexit_identity"));
        CHECK(g_destroy_calls == 0);

        // Hook failure is visible and retryable: the token survives.
        counted.mutex_destroy_3b380 = destroy_fail;
        CHECK(stage_is(teardown_slot414_57b9b0(*state, counted, log), "mutex_destroy"));
        CHECK(g_destroy_arg == state->obj.data());
        CHECK(load_u64(state->obj.data()) == token);

        // Retry with the real hook succeeds and clears the 8 bytes.
        CHECK(teardown_slot414_57b9b0(*state, hooks, log).ok);
        CHECK(load_u64(state->obj.data()) == 0);
        CHECK(teardown_slot414_57b9b0(*state, hooks, log).ok == false);
    }

    // ---- slot 535: same contract for the separate mutex cell --------------------------------
    {
        auto state = std::make_unique<Slot535State>();
        std::memset(state->mutex.data(), kFill, state->mutex.size());
        std::memset(state->obj.data(), kFill, state->obj.size());
        StartupAtexitLog log;
        const Slot414Hooks hooks = default_critsec_slot414_hooks();
        CHECK(init_slot535_138e20(*state, hooks, log).ok);
        CHECK(log.size() == 2);
        CHECK(log.at(0).dtor_rva == 0x3b380 && log.at(0).object == state->mutex.data());
        CHECK(log.at(1).dtor_rva == 0x138370 && log.at(1).object == state->obj.data());
        const std::uint64_t token = load_u64(state->mutex.data());
        CHECK(token != 0);
        const Slot535State before_teardown = *state;

        CHECK(stage_is(teardown_slot535_138e20(*state, Slot414Hooks{}, log), "mutex_destroy_hook"));
        CHECK(load_u64(state->mutex.data()) == token);

        // The 0x138370 record for the object is not this teardown's identity.
        StartupAtexitLog object_only;
        CHECK(object_only.record(0x138370, state->obj.data()));
        CHECK(stage_is(teardown_slot535_138e20(*state, hooks, object_only), "atexit_identity"));
        CHECK(load_u64(state->mutex.data()) == token);

        CHECK(teardown_slot535_138e20(*state, hooks, log).ok);
        CHECK(load_u64(state->mutex.data()) == 0);
        // The object bytes are untouched by a mutex teardown (compare against the post-ctor snapshot).
        CHECK(std::memcmp(state->obj.data(), before_teardown.obj.data(), state->obj.size()) == 0);
        CHECK(teardown_slot535_138e20(*state, hooks, log).ok == false);
    }

    std::puts("OK test_critsec_slots_teardown");
    return 0;
}
