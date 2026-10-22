#include "init_531d60_post_base_sbj.hpp"

namespace sotc {

Init531d60PostBaseSbjResult init_531d60_post_base_sbj(
    const Init531d60PostBaseSbjHooks& hooks) noexcept {
    Init531d60PostBaseSbjResult out{};

    // OBS: "Shadow Boss Jump" stack/string prep — NOT modelled.

    // OBS: call 0x26800 (body NOT modelled)
    if (hooks.call_26800 != nullptr) {
        hooks.call_26800();
    }
    ++out.call_26800_count;

    // OBS: mov rdi, rbx; call 0xf590 (body NOT modelled)
    if (hooks.call_f590 != nullptr) {
        hooks.call_f590();
    }
    ++out.call_f590_count;

    // Next: "Shadow Boss Outro Jump" string pair @ 0x531fe2 — E463; not this fragment.
    out.ok = (out.call_26800_count == 1) && (out.call_f590_count == 1);
    return out;
}

}  // namespace sotc
