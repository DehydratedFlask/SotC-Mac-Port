#include "init_531d60_post_base_26800_f590.hpp"

namespace sotc {

Init531d60PostBase26800F590Result init_531d60_post_base_26800_f590(
    const Init531d60PostBase26800F590Hooks& hooks) noexcept {
    Init531d60PostBase26800F590Result out{};

    // OBS: stack prep (vxorps/lea/vmovups) — NOT modelled.

    // OBS: call 0x26800 (body NOT modelled)
    if (hooks.call_26800 != nullptr) {
        hooks.call_26800();
    }
    ++out.call_26800_count;

    // OBS: mov rdi, rbx; call 0xf590 on the measured 32 B stack temp (rsp+0x388,
    // `+0x10..0x1f` zeroed): no control block, so the lifted release is a no-op unless the
    // host overrides it.
    SharedHolderF590 holder{};
    if (hooks.call_f590 != nullptr) {
        out.release = hooks.call_f590(holder);
    } else {
        out.release = shared_release_f590(holder, hooks.release_hooks);
    }
    ++out.call_f590_count;

    // Next: "Shadow Boss Jump" string pair @ 0x531f93 — E462; not this fragment.
    out.ok = (out.call_26800_count == 1) && (out.call_f590_count == 1);
    return out;
}

}  // namespace sotc
