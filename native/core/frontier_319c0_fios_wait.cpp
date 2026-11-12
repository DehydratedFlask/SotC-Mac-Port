#include "frontier_319c0_fios_wait.hpp"

#include <cstring>

namespace sotc {

Frontier319c0Result frontier_319c0_fios_wait(std::uint8_t* object,
                                             const Frontier319c0Hooks& hooks) noexcept {
    Frontier319c0Result out{};
    if (object == nullptr) {
        return out;
    }
    if (hooks.poll == nullptr) {
        out.missing_hook = "poll";
        return out;
    }
    if (hooks.wait == nullptr) {
        out.missing_hook = "wait";
        return out;
    }
    if (hooks.tail == nullptr) {
        out.missing_hook = "tail";
        return out;
    }
    while (!hooks.poll(object, hooks.user)) {
        if (out.waits >= kFrontier319c0MaxWaits) {
            out.wait_overflow = true;
            return out;
        }
        std::uint32_t handle = 0;
        std::memcpy(&handle, object + kFrontier319c0OpHandleOffset, sizeof(handle));
        hooks.wait(handle, hooks.user);
        ++out.waits;
    }
    out.value = hooks.tail(object, hooks.user);
    out.ok = true;
    return out;
}

}  // namespace sotc
