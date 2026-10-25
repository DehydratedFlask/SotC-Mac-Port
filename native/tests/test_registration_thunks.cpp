// Native registration for constructor-table slots 12-19. Fails if any thunk is unregistered or stubbed:
// each slot must publish its own descriptor and leave a usable recursive mutex, and nothing else.
#include <cstdio>
#include <cstdlib>

#include "core/startup_ctors.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

bool only_registered(const StartupState& st, std::size_t first, std::size_t last) {
    for (std::size_t i = 0; i < kRegistrationThunkCount; ++i) {
        const bool want = i >= first && i <= last;
        if (st.thunk_registrations[i].published() != want) return false;
    }
    return true;
}

}  // namespace

int main() {
    // Table identity: eight consecutive indices with the original thunk RVAs of slots 12..19.
    const std::uint32_t rvas[kRegistrationThunkCount] = {0xf06340, 0xf058f0, 0xf02ff0, 0xf01100,
                                                          0xf00090, 0xefe320, 0xefd020, 0xefacd0};
    CHECK(kRegistrationThunkDescriptors.size() == 8);
    for (std::size_t i = 0; i < kRegistrationThunkCount; ++i) {
        CHECK(kRegistrationThunkDescriptors[i].ctor_index == 12 + i);
        CHECK(kRegistrationThunkDescriptors[i].original_thunk_rva == rvas[i]);
        CHECK(kRegistrationThunkDescriptors[i].original_global_rva >= 0x2dd7100);   // first non-file-backed byte
    }
    CHECK(kRegistrationThunkDescriptors[0].probe_shape == RegistrationProbeShape::base_only);
    CHECK(kRegistrationThunkDescriptors[1].probe_shape == RegistrationProbeShape::callee_inline);
    CHECK(kRegistrationThunkDescriptors[2].probe_shape == RegistrationProbeShape::vslot0_dtor);
    CHECK(kRegistrationThunkDescriptors[7].probe_shape == RegistrationProbeShape::callee_inline);

    // Each slot publishes exactly its own descriptor, in isolation.
    for (std::uint32_t index = 12; index <= 19; ++index) {
        StartupState st;
        CHECK(only_registered(st, 1, 0));   // nothing published yet
        CHECK(init_registration_thunk(st, index));
        CHECK(!st.init_failed);
        const std::size_t slot = index - 12;
        CHECK(only_registered(st, slot, slot));
        CHECK(st.thunk_registrations[slot].descriptor() == &kRegistrationThunkDescriptors[slot]);
        // The owned mutex is the recursive one: a second lock on the same thread succeeds.
        RecursiveMutex& m = st.thunk_registrations[slot].mutex();
        CHECK(m.lock() == 0);
        CHECK(m.lock() == 0);
        CHECK(m.unlock() == 0);
        CHECK(m.unlock() == 0);
        // Re-running the same slot is idempotent and keeps the descriptor.
        CHECK(init_registration_thunk(st, index));
        CHECK(!st.init_failed);
        CHECK(st.thunk_registrations[slot].descriptor() == &kRegistrationThunkDescriptors[slot]);
    }

    // The runner publishes all eight, in order, and nothing else of this family. The stop point is
    // derived from the table itself: the runner must report the first row with a null callback, and
    // succeed when every row is recovered (the production table has no explicit blocker today).
    {
        StartupState st;
        const std::span<const CtorSlot> slots = recovered_constructors();
        std::size_t first_unresolved = 0;
        while (first_unresolved < slots.size() && slots[first_unresolved].run != nullptr) ++first_unresolved;
        const auto r = run_constructors(slots, st);
        CHECK(!r.callback_failed);
        CHECK(r.completed == first_unresolved);
        if (first_unresolved == slots.size()) {
            CHECK(r.ok() && r.blocked_on_rva == 0);
        } else {
            CHECK(!r.ok() && r.blocked_on_rva == slots[first_unresolved].original_rva);
        }
        CHECK(only_registered(st, 0, 7));
        for (std::size_t i = 0; i < kRegistrationThunkCount; ++i)
            CHECK(st.thunk_registrations[i].descriptor() == &kRegistrationThunkDescriptors[i]);
    }

    // Out-of-range indices fail visibly and publish nothing.
    for (std::uint32_t index : {0u, 11u, 20u, 21u, 4000u}) {
        StartupState st;
        CHECK(!init_registration_thunk(st, index));
        CHECK(st.init_failed);
        CHECK(only_registered(st, 1, 0));
    }

    // A different descriptor already occupying the registration is a visible failure, not an overwrite.
    {
        StartupState st;
        static const int foreign = 0;
        CHECK(st.thunk_registrations[3].publish(&foreign) == DescriptorPublishError::none);
        CHECK(!init_registration_thunk(st, 15));
        CHECK(st.init_failed);
        CHECK(st.thunk_registrations[3].descriptor() == &foreign);
    }
    {
        // ... and the runner stops with callback_failed at that slot (0xf01100, index 15).
        StartupState st;
        static const int foreign = 0;
        CHECK(st.thunk_registrations[3].publish(&foreign) == DescriptorPublishError::none);
        auto r = run_constructors(recovered_constructors(), st);
        CHECK(!r.ok() && r.callback_failed && r.blocked_on_rva == 0xf01100);
        CHECK(r.completed == 15);
        CHECK(st.thunk_registrations[4].published() == false);   // slot 16 never ran
    }

    std::puts("registration_thunks: all checks passed");
    return 0;
}
