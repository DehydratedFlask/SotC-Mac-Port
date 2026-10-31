#include "init_531d60_post_base_a54920_532ae2_u1.hpp"

namespace sotc {

Init531d60PostBaseA54920_532ae2_U1Result init_531d60_post_base_a54920_532ae2_u1(
    const Init531d60PostBaseA54920_532ae2_U1Hooks& hooks) noexcept {
    Init531d60PostBaseA54920_532ae2_U1Result out{};

    if (hooks.call_a54920 != nullptr) {
        hooks.call_a54920(
            hooks.parent,
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(kInit531d60PostBaseA54920_532ae2_U1Name3Rva)),
            hooks.descriptor);
    }
    ++out.call_a54920_count;

    out.a54920_hook_supplied = (hooks.call_a54920 != nullptr);
    out.ok = (out.call_a54920_count == 1);
    return out;
}

}  // namespace sotc
