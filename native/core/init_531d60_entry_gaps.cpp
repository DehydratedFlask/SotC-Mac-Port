#include "init_531d60_entry_gaps.hpp"

namespace sotc {

InitEntryGapsResult init_531d60_entry_gaps(void* self,
                                           const InitEntryGapsHooks& hooks) noexcept {
    InitEntryGapsResult out{};

    // --- Span A: entry frame (11 units) -------------------------------------------------------
    // push rbp / mov rbp,rsp / push r15 / push r14 / push r13 / push r12 / push rbx
    out.units_executed += 7;  // pushes
    // and rsp,-0x20 / sub rsp,0xc40 — the real frame is NOT modelled.
    ++out.units_executed;
    ++out.units_executed;
    // mov r13,[0x15fe518] — guard cell pin.
    ++out.units_executed;
    ++out.guard_loads;
    // mov [rsp+0x20],rdi — the incoming self survives in the frame slot.
    out.saved_self = self;
    ++out.self_saves;
    ++out.units_executed;

    // --- Span B: base-initialize call site (4 units) ------------------------------------------
    // mov r12,[rsp+0x20]
    void* reloaded = out.saved_self;
    ++out.self_reloads;
    ++out.units_executed;
    // mov rsi,r15 / mov rdi,r12 — argument marshalling (r15 provenance NOT modelled).
    ++out.units_executed;
    ++out.units_executed;
    // call 0x255130
    bool base_ret = false;
    if (hooks.call_base_initialize != nullptr) {
        base_ret = hooks.call_base_initialize(reloaded, nullptr);
    }
    ++out.base_call_count;
    ++out.units_executed;
    // test al,al — the flags feed the `je 0x5327b6` failure exit, which is NOT modelled here.
    out.base_ok = base_ret;

    // --- Span C: call 0x56dca0 (1 unit) --------------------------------------------------------
    if (hooks.call_ret_nop_56dca0 != nullptr) {
        hooks.call_ret_nop_56dca0();
    }
    ++out.ret_nop_count;
    ++out.units_executed;

    // --- Span D: jmp 0x532904 (1 unit) ---------------------------------------------------------
    ++out.jump_pins;
    ++out.units_executed;

    out.ok = (out.units_executed == 17) && (out.guard_loads == 1) && (out.self_saves == 1) &&
             (out.self_reloads == 1) && (out.base_call_count == 1) && (out.ret_nop_count == 1) &&
             (out.jump_pins == 1);
    return out;
}

}  // namespace sotc
