#include "init_531d60_post_base_sboj.hpp"

namespace sotc {

Init531d60PostBaseSbojResult init_531d60_post_base_sboj(
    const Init531d60PostBaseSbojHooks& hooks) noexcept {
    Init531d60PostBaseSbojResult out{};

    // OBS: "Shadow Boss Outro Jump" stack/string prep — NOT modelled.

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

    // Next: call 0x52eb50 @ 0x532031 — NOT modelled here.
    out.ok = (out.call_26800_count == 1) && (out.call_f590_count == 1);
    return out;
}

}  // namespace sotc
