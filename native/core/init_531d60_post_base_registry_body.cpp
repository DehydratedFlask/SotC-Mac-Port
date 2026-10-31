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
    if (!hooks.merge_mov_r15_rax_532b26_u1_ran) {
        Init531d60PostBaseMovR15Rax_532b26_U1Cells cells = hooks.mov_r15_rax_cells;
        cells.rax_in = out.count_from_54dc20;
        const Init531d60PostBaseMovR15Rax_532b26_U1Result mov_r15 =
            init_531d60_post_base_mov_r15_rax_532b26_u1(cells);
        if (!mov_r15.ok) {
            return out;
        }
        out.mov_r15_rax_count = mov_r15.mov_r15_rax_count;
        out.r15_from_54dc20 = mov_r15.r15;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::mov_r15_rax_532b26_u1)] = 1;
        ++out.unit_executed;
    }
    if (!hooks.merge_mov_rsi_rbx_532b29_u1_ran) {
        Init531d60PostBaseMovRsiRbx_532b29_U1Cells cells = hooks.mov_rsi_rbx_cells;
        if (cells.rbx_in == nullptr) {
            cells.rbx_in = out.rbx_from_54dc10;
        }
        const Init531d60PostBaseMovRsiRbx_532b29_U1Result mov_rsi =
            init_531d60_post_base_mov_rsi_rbx_532b29_u1(cells);
        if (!mov_rsi.ok) {
            return out;
        }
        out.mov_rsi_rbx_count = mov_rsi.mov_rsi_rbx_count;
        out.rsi_from_rbx = mov_rsi.rsi;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::mov_rsi_rbx_532b29_u1)] = 1;
        ++out.unit_executed;
    }
    if (!hooks.merge_mov_edx_eax_532b2c_u1_ran) {
        Init531d60PostBaseMovEdxEax_532b2c_U1Cells cells = hooks.mov_edx_eax_cells;
        cells.eax_in = out.count_from_54dc20;
        const Init531d60PostBaseMovEdxEax_532b2c_U1Result mov_edx =
            init_531d60_post_base_mov_edx_eax_532b2c_u1(cells);
        if (!mov_edx.ok) {
            return out;
        }
        out.mov_edx_eax_count = mov_edx.mov_edx_eax_count;
        out.edx_from_eax = mov_edx.edx;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::mov_edx_eax_532b2c_u1)] = 1;
        ++out.unit_executed;
    }
    if (!hooks.merge_call_52a220_532b2e_u1_ran) {
        Init531d60PostBaseCall52a220_532b2e_U1Cells cells = hooks.call_52a220_cells;
        if (cells.table_in == nullptr) {
            cells.table_in = hooks.table_in;
        }
        if (cells.obj_in == nullptr) {
            cells.obj_in = hooks.obj_in;
        }
        if (cells.alloc == nullptr) {
            cells.alloc = hooks.alloc;
        }
        cells.edx_in = out.edx_from_eax;
        const Init531d60PostBaseCall52a220_532b2e_U1Result fill_call =
            init_531d60_post_base_call_52a220_532b2e_u1(cells);
        if (!fill_call.ok) {
            return out;
        }
        out.call_52a220_count = fill_call.call_52a220_count;
        out.fill_invoked = fill_call.fill_invoked;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::call_52a220_532b2e_u1)] = 1;
        ++out.unit_executed;
    }
    if (!hooks.merge_lea_rbx_2efd160_532b33_u1_ran) {
        Init531d60PostBaseLeaRbx2efd160_532b33_U1Cells cells = hooks.lea_rbx_2efd160_cells;
        const Init531d60PostBaseLeaRbx2efd160_532b33_U1Result lea_rbx =
            init_531d60_post_base_lea_rbx_2efd160_532b33_u1(cells);
        if (!lea_rbx.ok) {
            return out;
        }
        out.lea_rbx_2efd160_count = lea_rbx.lea_rip_count;
        out.rbx_global_cell_addr = lea_rbx.rbx;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::lea_rbx_2efd160_532b33_u1)] = 1;
        ++out.unit_executed;
    }
    if (!hooks.merge_mov_r13_15fe518_532b3a_u1_ran) {
        Init531d60PostBaseMovR13_15fe518_532b3a_U1Cells cells = hooks.mov_r13_15fe518_cells;
        const Init531d60PostBaseMovR13_15fe518_532b3a_U1Result mov_r13 =
            init_531d60_post_base_mov_r13_15fe518_532b3a_u1(cells);
        if (!mov_r13.ok) {
            return out;
        }
        out.mov_r13_15fe518_count = mov_r13.mov_r13_rip_count;
        out.r13_from_guard_cell = mov_r13.r13;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::mov_r13_15fe518_532b3a_u1)] = 1;
        ++out.unit_executed;
    }
    if (!hooks.merge_mov_r12_rbx_532b41_u1_ran) {
        Init531d60PostBaseMovR12Rbx_532b41_U1Cells cells = hooks.mov_r12_rbx_cells;
        const Init531d60PostBaseMovR12Rbx_532b41_U1Result mov_r12 =
            init_531d60_post_base_mov_r12_rbx_532b41_u1(cells);
        if (!mov_r12.ok) {
            return out;
        }
        out.mov_r12_rbx_count = mov_r12.mov_r12_rbx_count;
        out.r12_from_cell = mov_r12.r12;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::mov_r12_rbx_532b41_u1)] = 1;
        ++out.unit_executed;
    }
    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;
    const int expected_call = hooks.merge_call_54dc10_532b19_u1_ran ? 0 : 1;
    const int expected_mov_rbx = hooks.merge_mov_rbx_rax_532b1e_u1_ran ? 0 : 1;
    const int expected_count = hooks.merge_call_54dc20_532b21_u1_ran ? 0 : 1;
    const int expected_mov_r15 = hooks.merge_mov_r15_rax_532b26_u1_ran ? 0 : 1;
    const int expected_mov_rsi = hooks.merge_mov_rsi_rbx_532b29_u1_ran ? 0 : 1;
    const int expected_mov_edx = hooks.merge_mov_edx_eax_532b2c_u1_ran ? 0 : 1;
    const int expected_fill = hooks.merge_call_52a220_532b2e_u1_ran ? 0 : 1;
    const int expected_lea_rbx = hooks.merge_lea_rbx_2efd160_532b33_u1_ran ? 0 : 1;
    const int expected_mov_r13 = hooks.merge_mov_r13_15fe518_532b3a_u1_ran ? 0 : 1;
    const int expected_mov_r12 = hooks.merge_mov_r12_rbx_532b41_u1_ran ? 0 : 1;
    const int expected_units = expected_call + expected_mov_rbx + expected_count + expected_mov_r15 +
                               expected_mov_rsi + expected_mov_edx + expected_fill + expected_lea_rbx +
                               expected_mov_r13 + expected_mov_r12;
    out.ok = (out.unit_executed == expected_units) && (out.call_54dc10_count == expected_call) &&
             (out.mov_rbx_rax_count == expected_mov_rbx) &&
             (out.call_54dc20_count == expected_count) &&
             (out.mov_r15_rax_count == expected_mov_r15) &&
             (out.mov_rsi_rbx_count == expected_mov_rsi) &&
             (out.mov_edx_eax_count == expected_mov_edx) &&
             (out.call_52a220_count == expected_fill) &&
             (out.lea_rbx_2efd160_count == expected_lea_rbx) &&
             (out.mov_r13_15fe518_count == expected_mov_r13) &&
             (out.mov_r12_rbx_count == expected_mov_r12);
    return out;
}

}  // namespace sotc
