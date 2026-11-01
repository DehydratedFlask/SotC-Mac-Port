#include "init_531d60_post_base_mov_r13_15fe518_532bdf_u1.hpp"

#include "vslot_40_533100_stackchk.hpp"

namespace sotc {
namespace {

void* default_guard_cell_qword() noexcept {
    static void* slot = reinterpret_cast<void*>(&stack_guard_value_cell());
    return slot;
}

}  // namespace

Init531d60PostBaseMovR13_15fe518_532bdf_U1Result init_531d60_post_base_mov_r13_15fe518_532bdf_u1(
    const Init531d60PostBaseMovR13_15fe518_532bdf_U1Cells& cells) noexcept {
    Init531d60PostBaseMovR13_15fe518_532bdf_U1Result out{};

    void* loaded =
        (cells.guard_cell_slot != nullptr) ? *cells.guard_cell_slot : default_guard_cell_qword();
    out.r13 = loaded;
    if (cells.r13_out != nullptr) {
        *cells.r13_out = loaded;
    }
    ++out.mov_r13_rip_532bdf_count;

    // Next: E604 test rdi,rdi; je +5 @ 0x532be6 — separate unit.
    out.ok = (out.mov_r13_rip_532bdf_count == 1);
    return out;
}

}  // namespace sotc
