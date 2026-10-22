#include "vslot_40_533100_stackchk.hpp"

namespace sotc {
namespace {

std::uint64_t g_stack_guard_value = 0;

}  // namespace

std::uint64_t& stack_guard_value_cell() noexcept { return g_stack_guard_value; }

Vslot40_533100StackchkResult vslot_40_533100_stackchk(
    std::uint64_t saved_canary, const Vslot40_533100StackchkHooks& hooks) noexcept {
    Vslot40_533100StackchkResult out{};
    out.saved = saved_canary;

    // OBS: rax = *0x15fe518; rax = *rax
    if (hooks.load_guard != nullptr) {
        out.live = hooks.load_guard();
    } else {
        out.live = stack_guard_value_cell();
    }

    // OBS: cmp [rbp-0x30], rax; jne fail @ 0x534074
    if (out.live != out.saved) {
        out.took_fail_path = true;
        if (hooks.stack_chk_fail_f8f3a0 != nullptr) {
            hooks.stack_chk_fail_f8f3a0();
        }
        ++out.fail_count;
        out.ok = false;
        return out;
    }

    // OBS success: add rsp,0x2d8; pops; ret — modelled as returned
    out.returned = true;
    out.ok = true;
    return out;
}

}  // namespace sotc
