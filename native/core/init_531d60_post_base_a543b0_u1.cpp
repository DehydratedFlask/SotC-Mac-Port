#include "init_531d60_post_base_a543b0_u1.hpp"

#include <cstring>

namespace sotc {

Init531d60PostBaseA543b0U1Result init_531d60_post_base_a543b0_u1(
    const Init531d60PostBaseA543b0U1Hooks& hooks) noexcept {
    Init531d60PostBaseA543b0U1Result out{};

    // OBS: prep 0x532740..0x532760 (32 B) NOT modelled as units.

    void* rdi = const_cast<void*>(hooks.incoming_rdi);
    if (rdi == nullptr && hooks.cell_32f84f8 != nullptr) {
        rdi = const_cast<void*>(*hooks.cell_32f84f8);
    }

    std::uintptr_t rsi = hooks.incoming_rsi;
    bool rsi_known = hooks.incoming_rsi_known;
    if (!rsi_known && hooks.cell_32f79a8 != nullptr) {
        const void* obj = *hooks.cell_32f79a8;
        if (obj != nullptr) {
            const auto* bytes = static_cast<const std::uint8_t*>(obj);
            std::memcpy(&rsi, bytes + kInit531d60PostBaseA543b0U1ObjOff, sizeof(rsi));
            rsi += kInit531d60PostBaseA543b0U1RsiAddend;
            rsi_known = true;
        }
    }

    if (hooks.call_a543b0 != nullptr) {
        hooks.call_a543b0(rdi, rsi);
    }
    ++out.call_a543b0_count;
    out.rsi_at_0x532760 = rsi;
    out.rsi_known = rsi_known;

    // Next: mov rax,[rsp+0x20] @ 0x532765 — NOT modelled here.
    out.ok = (out.call_a543b0_count == 1);
    return out;
}

}  // namespace sotc
