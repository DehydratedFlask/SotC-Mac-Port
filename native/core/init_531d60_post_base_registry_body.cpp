#include "init_531d60_post_base_registry_body.hpp"

#include "input_table_551140.hpp"

namespace sotc {

Init531d60PostBaseRegistryBodyResult init_531d60_post_base_registry_body(
    const Init531d60PostBaseRegistryBodyHooks& hooks) noexcept {
    Init531d60PostBaseRegistryBodyResult out{};
    if (!hooks.merge_call_54dc10_532b19_u1_ran) {
        Init531d60PostBaseCall54dc10_532b19_U1Cells cells = hooks.call_54dc10_cells;
        if (cells.table_in == nullptr) {
            cells.table_in = hooks.table_in;
        }
        const Init531d60PostBaseCall54dc10_532b19_U1Result call =
            init_531d60_post_base_call_54dc10_532b19_u1(cells);
        if (!call.ok) {
            return out;
        }
        out.call_54dc10_count = call.call_54dc10_count;
        out.rax_from_54dc10 = call.rax;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::call_54dc10_532b19_u1)] = 1;
        ++out.unit_executed;
    }
    if (!hooks.merge_mov_rbx_rax_532b1e_u1_ran) {
        Init531d60PostBaseMovRbxRax_532b1e_U1Cells cells = hooks.mov_rbx_rax_cells;
        if (cells.rax_in == nullptr) {
            cells.rax_in = out.rax_from_54dc10;
        }
        const Init531d60PostBaseMovRbxRax_532b1e_U1Result mov_rbx =
            init_531d60_post_base_mov_rbx_rax_532b1e_u1(cells);
        if (!mov_rbx.ok) {
            return out;
        }
        out.mov_rbx_rax_count = mov_rbx.mov_rbx_rax_count;
        out.rbx_from_54dc10 = mov_rbx.rbx;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::mov_rbx_rax_532b1e_u1)] = 1;
        ++out.unit_executed;
    }
    if (!hooks.merge_call_54dc20_532b21_u1_ran) {
        const Init531d60PostBaseCall54dc20_532b21_U1Result count_call =
            init_531d60_post_base_call_54dc20_532b21_u1(hooks.call_54dc20_cells);
        if (!count_call.ok) {
            return out;
        }
        out.call_54dc20_count = count_call.call_54dc20_count;
        out.count_from_54dc20 = count_call.count;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::call_54dc20_532b21_u1)] = 1;
        ++out.unit_executed;
    }
    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;
    const int expected_call = hooks.merge_call_54dc10_532b19_u1_ran ? 0 : 1;
    const int expected_mov_rbx = hooks.merge_mov_rbx_rax_532b1e_u1_ran ? 0 : 1;
    const int expected_count = hooks.merge_call_54dc20_532b21_u1_ran ? 0 : 1;
    const int expected_units = expected_call + expected_mov_rbx + expected_count;
    out.ok = (out.unit_executed == expected_units) && (out.call_54dc10_count == expected_call) &&
             (out.mov_rbx_rax_count == expected_mov_rbx) &&
             (out.call_54dc20_count == expected_count);
    return out;
}

}  // namespace sotc
