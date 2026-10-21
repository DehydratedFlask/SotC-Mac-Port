#include "descriptor_registration.hpp"

#include <cerrno>

namespace sotc {

DescriptorPublishError DescriptorRegistration::publish(const void* descriptor) noexcept {
    // Order follows the original call sequence: the descriptor store is guarded only against a
    // null value, and the mutex is created/initialized before the registration is complete. Here
    // the descriptor is recorded last so a failed mutex init cannot leave a half-published state.
    if (descriptor == nullptr) return DescriptorPublishError::null_descriptor;
    if (published_) return DescriptorPublishError::already_published;

    const int rc = mutex_.initialize();
    if (rc != 0) return DescriptorPublishError::mutex_init_failed;

    descriptor_ = descriptor;
    published_ = true;
    return DescriptorPublishError::none;
}

}  // namespace sotc
