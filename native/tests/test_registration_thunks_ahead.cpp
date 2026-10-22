// Native registration for constructor-table slots 24-47 (CPU-01-NATIVE-SLOT24-47-01).
// These rows are on the production path (recovered_constructors) after Phase B; this test still
// exercises the focused ahead span and verifies production publishes all 24 when the slot-23
// private fixture is available.
#include <cstdio>
#include <cstdlib>

#include "core/startup_ctors.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

constexpr std::size_t kAhead = kAheadRegistrationCount;

bool ahead_only(const StartupState& st, std::size_t first, std::size_t last) {
    for (std::size_t i = 0; i < kAhead; ++i) {
        const bool want = i >= first && i <= last;
        if (st.ahead_registrations[i].published() != want) return false;
    }
    return true;
}

bool thunks_only(const StartupState& st, std::size_t first, std::size_t last) {
    for (std::size_t i = 0; i < kRegistrationThunkCount; ++i) {
        const bool want = i >= first && i <= last;
        if (st.thunk_registrations[i].published() != want) return false;
    }
    return true;
}

}  // namespace

int main() {
    const std::uint32_t rvas[kAhead] = {0xe56b70, 0xe55f10, 0xe55240, 0xe54a30, 0xe53a10, 0xe52a20, 0xe52150, 0xe518e0,
                                        0xe510e0, 0xe50980, 0xe500b0, 0xe4f840, 0xe4f040, 0xe4e8e0, 0xe4e010, 0xe4d7a0,
                                        0xe4cfa0, 0xe4c840, 0xe4bf10, 0xe4b7b0, 0xe4afb0, 0xe47670, 0xe467e0, 0xe459e0};
    CHECK(kAheadRegistrationDescriptors.size() == 24);
    CHECK(kFirstAheadRegistrationIndex == 24);
    std::size_t base_only = 0, callee_inline = 0, vslot0 = 0;
    for (std::size_t i = 0; i < kAhead; ++i) {
        const RegistrationThunkDescriptor& d = kAheadRegistrationDescriptors[i];
        CHECK(d.ctor_index == 24 + i);
        CHECK(d.original_thunk_rva == rvas[i]);
        CHECK(d.original_global_rva >= 0x2dd7100);
        CHECK(d.original_vtable_rva > 0 && d.original_vtable_rva < 0x2dd7100);
        switch (d.probe_shape) {
            case RegistrationProbeShape::base_only: ++base_only; break;
            case RegistrationProbeShape::callee_inline: ++callee_inline; break;
            case RegistrationProbeShape::vslot0_dtor: ++vslot0; break;
        }
    }
    CHECK(base_only == 19 && callee_inline == 3 && vslot0 == 2);
    for (std::size_t i : {0u, 1u, 3u}) CHECK(kAheadRegistrationDescriptors[i].probe_shape == RegistrationProbeShape::callee_inline);
    for (std::size_t i : {4u, 23u}) CHECK(kAheadRegistrationDescriptors[i].probe_shape == RegistrationProbeShape::vslot0_dtor);
    CHECK(kAheadRegistrationDescriptors[2].probe_shape == RegistrationProbeShape::base_only);
    CHECK(kAheadRegistrationDescriptors[5].probe_shape == RegistrationProbeShape::base_only);
    for (const RegistrationThunkDescriptor& a : kAheadRegistrationDescriptors)
        for (const RegistrationThunkDescriptor& b : kRegistrationThunkDescriptors)
            CHECK(a.original_thunk_rva != b.original_thunk_rva && a.original_global_rva != b.original_global_rva &&
                  a.original_vtable_rva != b.original_vtable_rva);

    const std::span<const CtorSlot> ahead = registered_ahead_constructors();
    CHECK(ahead.size() == kAhead);
    for (std::size_t i = 0; i < kAhead; ++i) {
        CHECK(ahead[i].original_index == 24 + i);
        CHECK(ahead[i].original_rva == rvas[i]);
        CHECK(ahead[i].run != nullptr);
    }

    for (std::uint32_t index = 24; index <= 47; ++index) {
        StartupState st;
        CHECK(ahead_only(st, 1, 0));
        CHECK(init_registration_thunk_ahead(st, index));
        CHECK(!st.init_failed);
        const std::size_t slot = index - 24;
        CHECK(ahead_only(st, slot, slot));
        CHECK(thunks_only(st, 1, 0));
        CHECK(st.ahead_registrations[slot].descriptor() == &kAheadRegistrationDescriptors[slot]);
        RecursiveMutex& m = st.ahead_registrations[slot].mutex();
        CHECK(m.lock() == 0);
        CHECK(m.lock() == 0);
        CHECK(m.unlock() == 0);
        CHECK(m.unlock() == 0);
        CHECK(init_registration_thunk_ahead(st, index));
        CHECK(!st.init_failed);
        CHECK(st.ahead_registrations[slot].descriptor() == &kAheadRegistrationDescriptors[slot]);
    }

    {
        StartupState st;
        const CtorRunResult r = run_constructors(ahead, st);
        CHECK(r.ok() && r.completed == 24 && r.blocked_on_rva == 0 && !r.callback_failed);
        CHECK(ahead_only(st, 0, kAhead - 1));
        CHECK(thunks_only(st, 1, 0));
        for (std::size_t i = 0; i < kAhead; ++i) {
            CHECK(st.ahead_registrations[i].descriptor() == &kAheadRegistrationDescriptors[i]);
            RecursiveMutex& m = st.ahead_registrations[i].mutex();
            CHECK(m.lock() == 0 && m.unlock() == 0);
        }
    }

    // PRODUCTION runner (requires private slot-23 fixture via env): contiguous through 47 publishes
    // all ahead registrations; table carries every 24-47 row with a non-null callback.
    {
        StartupState st;
        const CtorRunResult r = run_constructors(recovered_constructors(), st);
        CHECK(r.ok() && !r.callback_failed && r.blocked_on_rva == 0);
        CHECK(r.completed == recovered_constructors().size());
        CHECK(thunks_only(st, 0, kRegistrationThunkCount - 1));
        CHECK(ahead_only(st, 0, kAhead - 1));
        CHECK(st.slot23_names.loaded);
    }
    {
        std::size_t ahead_rows = 0;
        for (const CtorSlot& s : recovered_constructors()) {
            CHECK(s.run != nullptr);
            if (s.original_index >= 24 && s.original_index <= 47) {
                CHECK(s.original_rva == rvas[s.original_index - 24]);
                ++ahead_rows;
            }
        }
        CHECK(ahead_rows == 24);
    }

    for (std::uint32_t index : {0u, 11u, 12u, 19u, 20u, 23u, 48u, 4000u}) {
        StartupState st;
        CHECK(!init_registration_thunk_ahead(st, index));
        CHECK(st.init_failed);
        CHECK(ahead_only(st, 1, 0) && thunks_only(st, 1, 0));
    }
    for (std::uint32_t index : {24u, 30u, 47u}) {
        StartupState st;
        CHECK(!init_registration_thunk(st, index));
        CHECK(st.init_failed);
        CHECK(ahead_only(st, 1, 0) && thunks_only(st, 1, 0));
    }

    {
        StartupState st;
        static const int foreign = 0;
        CHECK(st.ahead_registrations[5].publish(&foreign) == DescriptorPublishError::none);
        CHECK(!init_registration_thunk_ahead(st, 29));
        CHECK(st.init_failed);
        CHECK(st.ahead_registrations[5].descriptor() == &foreign);
    }
    {
        StartupState st;
        static const int foreign = 0;
        CHECK(st.ahead_registrations[5].publish(&foreign) == DescriptorPublishError::none);
        const CtorRunResult r = run_constructors(ahead, st);
        CHECK(!r.ok() && r.callback_failed && r.blocked_on_rva == 0xe52a20);
        CHECK(r.completed == 5);
        CHECK(st.ahead_registrations[4].published());
        CHECK(!st.ahead_registrations[6].published());
    }

    std::puts("registration_thunks_ahead: all checks passed");
    return 0;
}
