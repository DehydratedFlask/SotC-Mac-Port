#include "init_531d60_post_base_499f80.hpp"

namespace sotc {

Init531d60PostBase499f80Result init_531d60_post_base_499f80(
    void* self, const Init531d60PostBase499f80Hooks& hooks) noexcept {
    Init531d60PostBase499f80Result out{};

    if (self == nullptr) {
        return out;
    }

    // OBS: test al,al already satisfied by caller (base_ok); jz fail not taken.
    // OBS: lea rdi, [r12+0x3c70]; call 0x499f80
    out.arg = static_cast<std::uint8_t*>(self) + kInit531d60PostBase499f80ArgOff;
    if (hooks.call_499f80 != nullptr) {
        hooks.call_499f80(out.arg);
    }
    ++out.call_499f80_count;

    // Next: call 0x56dcb0 @ 0x531df4 — NOT modelled here (E406 remainder continues).
    out.ok = (out.call_499f80_count == 1) && (out.arg != nullptr);
    return out;
}

}  // namespace sotc
