#include "init_531d60_post_base_a473b0_u1.hpp"

namespace sotc {

Init531d60PostBaseA473b0U1Result init_531d60_post_base_a473b0_u1(
    const Init531d60PostBaseA473b0U1Hooks& hooks) noexcept {
    Init531d60PostBaseA473b0U1Result out{};

    void* rdi = nullptr;
    if (hooks.cell_32f79a8 != nullptr) {
        rdi = const_cast<void*>(*hooks.cell_32f79a8);
        ++out.load_count;
    }
    out.rdi_arg = rdi;

    if (hooks.capture_services != nullptr) {
        out.capture = capture_a473b0_set_enabled(false, *hooks.capture_services);
        out.capture_native = true;
    } else if (hooks.call_a473b0 != nullptr) {
        hooks.call_a473b0(rdi);
    }
    ++out.call_a473b0_count;

    out.ok = (out.call_a473b0_count == 1) && (out.load_count <= 1);
    return out;
}

}  // namespace sotc
