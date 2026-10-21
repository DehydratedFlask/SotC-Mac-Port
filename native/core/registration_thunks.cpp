#include "registration_thunks.hpp"

#include "clothing_preview_param.hpp"   // ClothingParamBase (shared base object 0xe1eda0 / 0xe1ede0)
#include "descriptor_registration.hpp"
#include "startup_ctors.hpp"

namespace sotc {

bool init_registration_thunk(StartupState& state, std::uint32_t ctor_index) noexcept {
    if (ctor_index < kFirstRegistrationThunkIndex ||
        ctor_index >= kFirstRegistrationThunkIndex + kRegistrationThunkCount) {
        state.init_failed = true;
        return false;
    }
    const std::size_t slot = ctor_index - kFirstRegistrationThunkIndex;

    // Every original thunk builds and destroys a stack-local object with owner = null and
    // field20 = field28 = 0x10 before publishing; slots 12 and 15 do exactly base ctor + base dtor.
    {
        ClothingParamBase probe(nullptr, 0x10, 0x10);
        (void)probe;
    }

    const RegistrationThunkDescriptor* descriptor = &kRegistrationThunkDescriptors[slot];
    DescriptorRegistration& registration = state.thunk_registrations[slot];
    const DescriptorPublishError error = registration.publish(descriptor);
    if (error == DescriptorPublishError::none) return true;
    if (error == DescriptorPublishError::already_published && registration.descriptor() == descriptor) {
        return true;   // idempotent re-run of the same slot
    }
    state.init_failed = true;
    return false;
}

}  // namespace sotc
