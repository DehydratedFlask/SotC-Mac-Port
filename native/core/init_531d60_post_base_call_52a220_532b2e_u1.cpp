#include "init_531d60_post_base_call_52a220_532b2e_u1.hpp"

#include "input_table_551140.hpp"

namespace sotc {

Init531d60PostBaseCall52a220_532b2e_U1Result init_531d60_post_base_call_52a220_532b2e_u1(
    const Init531d60PostBaseCall52a220_532b2e_U1Cells& cells) noexcept {
    Init531d60PostBaseCall52a220_532b2e_U1Result out{};
    ++out.call_52a220_count;

    if (cells.obj_in != nullptr && cells.table_in != nullptr && cells.alloc != nullptr) {
        const std::uint32_t count =
            cells.edx_in != 0 ? cells.edx_in : input_table_551140_count();
        out.fill_status =
            fill_registry_object_52a220(*cells.obj_in, *cells.table_in, *cells.alloc, count);
        out.fill_invoked = true;
    }

    // Next: lea rbx @ 0x532b33 — not modelled in this unit.
    out.ok = (out.call_52a220_count == 1);
    return out;
}

}  // namespace sotc
