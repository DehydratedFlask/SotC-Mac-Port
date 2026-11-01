#include "vtable_15e0b48_slot28_ddf4b0.hpp"

namespace sotc {

Vtable15e0b48Slot28_ddf4b0Result vtable_15e0b48_slot28_ddf4b0(
    const Vtable15e0b48Slot28_ddf4b0Cells& cells,
    const Vtable15e0b48Slot28_ddf4b0Hooks& hooks) noexcept {
    Vtable15e0b48Slot28_ddf4b0Result out{};

    if (hooks.pre_call_ddf310 != nullptr) {
        hooks.pre_call_ddf310(cells.self);
    }
    out.pre_call_modelled = true;  // unconditional in the retail body

    if (cells.self == nullptr) {
        out.early_return = true;  // `test rbx,rbx; je` takes the plain epilogue
        out.ok = true;
        return out;
    }

    if (hooks.fetch_manager == nullptr) {
        return out;  // the manager value is a real argument; cannot fabricate it
    }
    out.manager = hooks.fetch_manager();
    if (out.manager == nullptr) {
        return out;  // `mov (%rax),%rcx` would fault on a null manager
    }

    if (hooks.tail_vcall18 != nullptr) {
        hooks.tail_vcall18(out.manager, cells.self);
    }
    out.tail_modelled = true;
    out.ok = true;
    return out;
}

}  // namespace sotc
