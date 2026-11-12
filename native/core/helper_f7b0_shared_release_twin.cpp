#include "helper_f7b0_shared_release_twin.hpp"

namespace sotc {

SharedReleaseF590Result shared_release_f7b0(const SharedHolderF590& holder, const SharedReleaseF590Hooks& hooks) noexcept {
    return shared_release_f590(holder, hooks);   // byte-identical twin body (see the header)
}

}  // namespace sotc
