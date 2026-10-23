#include "init_531d60_post_base_continue_d.hpp"

namespace sotc {
namespace {

int& slot(Init531d60PostBaseContinueDResult& out, ContinueDUnit u) noexcept {
    return out.unit_counts[static_cast<int>(u)];
}

void pin(Init531d60PostBaseContinueDResult& out, ContinueDUnit u) noexcept {
    ++slot(out, u);
    ++out.unit_executed;
}

}  // namespace

Init531d60PostBaseContinueDResult init_531d60_post_base_continue_d(
    const Init531d60PostBaseContinueDHooks& hooks) noexcept {
    Init531d60PostBaseContinueDResult out{};

    // 0x53299d lea rax,[0x32f79a8] — address pin only.
    pin(out, ContinueDUnit::lea_rax_32f79a8);

    // 0x5329a4 xor esi,esi — second argument for the call.
    pin(out, ContinueDUnit::xor_esi_esi);

    // 0x5329a6 mov rdi,[rax] — cell load (value provenance NOT modelled).
    void* rdi = nullptr;
    if (hooks.cell_32f79a8 != nullptr) {
        rdi = const_cast<void*>(*hooks.cell_32f79a8);
        ++out.load_count;
    }
    pin(out, ContinueDUnit::mov_rdi_cell_load);

    // 0x5329a9 call 0xa473b0 (body lifted when capture_services is supplied).
    if (hooks.capture_services != nullptr) {
        out.capture = capture_a473b0_set_enabled(false, *hooks.capture_services);
        out.capture_native = true;
    } else if (hooks.call_a473b0 != nullptr) {
        hooks.call_a473b0(rdi);
    }
    pin(out, ContinueDUnit::call_a473b0);
    ++out.call_count;

    out.ok = (out.unit_executed == kInit531d60PostBaseContinueDUnitCount) &&
             (out.call_count == 1) && (out.load_count <= 1);
    return out;
}

}  // namespace sotc
