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
    if (!hooks.merge_cmp_r12_r14_532b44_u1_ran) {
        Init531d60PostBaseCmpR12R14_532b44_U1Cells cells = hooks.cmp_r12_r14_cells;
        if (cells.r12_in == nullptr) {
            cells.r12_in = out.r12_from_cell;
        }
        if (cells.r14_in == nullptr) {
            cells.r14_in = hooks.r14_in;
        }
        const Init531d60PostBaseCmpR12R14_532b44_U1Result cmp =
            init_531d60_post_base_cmp_r12_r14_532b44_u1(cells);
        if (!cmp.ok) {
            return out;
        }
        out.cmp_r12_r14_count = cmp.cmp_r12_r14_count;
        out.r12_r14_equal = cmp.pointers_equal;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::cmp_r12_r14_532b44_u1)] = 1;
        ++out.unit_executed;
    }
    if (!hooks.merge_je_532bfb_532b47_u1_ran) {
        Init531d60PostBaseJe532bfb_532b47_U1Cells cells = hooks.je_532bfb_cells;
        if (!cells.pointers_equal_known) {
            cells.pointers_equal = out.r12_r14_equal;
            cells.pointers_equal_known = (out.cmp_r12_r14_count == 1);
        }
        const Init531d60PostBaseJe532bfb_532b47_U1Result je =
            init_531d60_post_base_je_532bfb_532b47_u1(cells);
        if (!je.ok) {
            return out;
        }
        out.je_532bfb_count = je.je_532bfb_count;
        out.je_branch_taken = je.branch_taken;
        out.je_branch_outcome_modelled = je.branch_outcome_modelled;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::je_532bfb_532b47_u1)] = 1;
        ++out.unit_executed;
    }
    if (!hooks.merge_test_r12_r12_532b4d_u1_ran) {
        Init531d60PostBaseTestR12R12_532b4d_U1Cells cells = hooks.test_r12_r12_cells;
        if (cells.r12_in == nullptr && out.mov_r12_rbx_count == 1) {
            cells.r12_in = out.r12_from_cell;
        }
        const Init531d60PostBaseTestR12R12_532b4d_U1Result test =
            init_531d60_post_base_test_r12_r12_532b4d_u1(cells);
        if (!test.ok) {
            return out;
        }
        out.test_r12_r12_count = test.test_r12_r12_count;
        out.r12_is_zero = test.r12_is_zero;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::test_r12_r12_532b4d_u1)] = 1;
        ++out.unit_executed;
    }
    if (!hooks.merge_je_532bf8_532b50_u1_ran) {
        Init531d60PostBaseJe532bf8_532b50_U1Cells cells = hooks.je_532bf8_cells;
        if (!cells.r12_is_zero_known) {
            cells.r12_is_zero = out.r12_is_zero;
            cells.r12_is_zero_known = (out.test_r12_r12_count == 1);
        }
        const Init531d60PostBaseJe532bf8_532b50_U1Result je =
            init_531d60_post_base_je_532bf8_532b50_u1(cells);
        if (!je.ok) {
            return out;
        }
        out.je_532bf8_count = je.je_532bf8_count;
        out.je_532bf8_branch_taken = je.branch_taken;
        out.je_532bf8_branch_outcome_modelled = je.branch_outcome_modelled;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::je_532bf8_532b50_u1)] = 1;
        ++out.unit_executed;
    }
    if (!hooks.merge_mov_r14_mem_rsp10_532b56_u1_ran) {
        Init531d60PostBaseMovR14MemRsp10_532b56_U1Cells cells = hooks.mov_r14_mem_rsp10_cells;
        if (cells.r14_in == nullptr) {
            cells.r14_in = hooks.r14_in;
        }
        const Init531d60PostBaseMovR14MemRsp10_532b56_U1Result mov_rsp =
            init_531d60_post_base_mov_r14_mem_rsp10_532b56_u1(cells);
        if (!mov_rsp.ok) {
            return out;
        }
        out.mov_r14_mem_rsp10_count = mov_rsp.mov_r14_mem_rsp10_count;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::mov_r14_mem_rsp10_532b56_u1)] = 1;
        ++out.unit_executed;
    }
    void* r13_from_rsp40 = nullptr;
    if (!hooks.merge_mov_r13_mem_rsp40_532b5b_u1_ran) {
        const Init531d60PostBaseMovR13MemRsp40_532b5b_U1Result mov_r13 =
            init_531d60_post_base_mov_r13_mem_rsp40_532b5b_u1(hooks.mov_r13_mem_rsp40_cells);
        if (!mov_r13.ok) {
            return out;
        }
        out.mov_r13_mem_rsp40_count = mov_r13.mov_r13_mem_rsp40_count;
        r13_from_rsp40 = mov_r13.r13_loaded;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::mov_r13_mem_rsp40_532b5b_u1)] = 1;
        ++out.unit_executed;
    }
    void* r14_from_rsp48 = nullptr;
    if (!hooks.merge_mov_r14_mem_rsp48_532b60_u1_ran) {
        const Init531d60PostBaseMovR14MemRsp48_532b60_U1Result mov_r14 =
            init_531d60_post_base_mov_r14_mem_rsp48_532b60_u1(hooks.mov_r14_mem_rsp48_cells);
        if (!mov_r14.ok) {
            return out;
        }
        out.mov_r14_mem_rsp48_count = mov_r14.mov_r14_mem_rsp48_count;
        r14_from_rsp48 = mov_r14.r14_loaded;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::mov_r14_mem_rsp48_532b60_u1)] = 1;
        ++out.unit_executed;
    }
    if (!hooks.merge_cmp_r13_r14_532b65_u1_ran) {
        Init531d60PostBaseCmpR13R14_532b65_U1Cells cells = hooks.cmp_r13_r14_cells;
        if (cells.r13_in == nullptr) {
            cells.r13_in = r13_from_rsp40;
        }
        if (cells.r14_in == nullptr) {
            cells.r14_in = r14_from_rsp48;
        }
        const Init531d60PostBaseCmpR13R14_532b65_U1Result cmp =
            init_531d60_post_base_cmp_r13_r14_532b65_u1(cells);
        if (!cmp.ok) {
            return out;
        }
        out.cmp_r13_r14_count = cmp.cmp_r13_r14_count;
        out.r13_r14_equal = cmp.pointers_equal;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::cmp_r13_r14_532b65_u1)] = 1;
        ++out.unit_executed;
    }
    if (!hooks.merge_je_532bba_532b68_u1_ran) {
        Init531d60PostBaseJe532bba_532b68_U1Cells cells = hooks.je_532bba_cells;
        if (!cells.pointers_equal_known) {
            cells.pointers_equal = out.r13_r14_equal;
            cells.pointers_equal_known = (out.cmp_r13_r14_count == 1);
        }
        const Init531d60PostBaseJe532bba_532b68_U1Result je =
            init_531d60_post_base_je_532bba_532b68_u1(cells);
        if (!je.ok) {
            return out;
        }
        out.je_532bba_count = je.je_532bba_count;
        out.je_532bba_branch_taken = je.branch_taken;
        out.je_532bba_branch_outcome_modelled = je.branch_outcome_modelled;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::je_532bba_532b68_u1)] = 1;
        ++out.unit_executed;
    }
    if (!hooks.merge_lea_rsi_rsp10_532b6a_u1_ran) {
        Init531d60PostBaseLeaRsiRsp10_532b6a_U1Cells cells = hooks.lea_rsi_rsp10_cells;
        if (cells.stack_rsp == nullptr) {
            cells.stack_rsp = hooks.stack_rsp;
        }
        const Init531d60PostBaseLeaRsiRsp10_532b6a_U1Result lea_rsi =
            init_531d60_post_base_lea_rsi_rsp10_532b6a_u1(cells);
        if (!lea_rsi.ok) {
            return out;
        }
        out.lea_rsi_rsp10_count = lea_rsi.lea_rsi_rsp10_count;
        out.rsi_from_rsp10_lea = lea_rsi.rsi;
        out.unit_counts[static_cast<std::size_t>(RegistryBodyUnit::lea_rsi_rsp10_532b6a_u1)] = 1;
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
    const int expected_cmp = hooks.merge_cmp_r12_r14_532b44_u1_ran ? 0 : 1;
    const int expected_je = hooks.merge_je_532bfb_532b47_u1_ran ? 0 : 1;
    const int expected_test = hooks.merge_test_r12_r12_532b4d_u1_ran ? 0 : 1;
    const int expected_je_null = hooks.merge_je_532bf8_532b50_u1_ran ? 0 : 1;
    const int expected_mov_rsp10 = hooks.merge_mov_r14_mem_rsp10_532b56_u1_ran ? 0 : 1;
    const int expected_mov_rsp40 = hooks.merge_mov_r13_mem_rsp40_532b5b_u1_ran ? 0 : 1;
    const int expected_mov_rsp48 = hooks.merge_mov_r14_mem_rsp48_532b60_u1_ran ? 0 : 1;
    const int expected_cmp_r13 = hooks.merge_cmp_r13_r14_532b65_u1_ran ? 0 : 1;
    const int expected_je_532bba = hooks.merge_je_532bba_532b68_u1_ran ? 0 : 1;
    const int expected_lea_rsi = hooks.merge_lea_rsi_rsp10_532b6a_u1_ran ? 0 : 1;
    const int expected_units = expected_call + expected_mov_rbx + expected_count + expected_mov_r15 +
                               expected_mov_rsi + expected_mov_edx + expected_fill + expected_lea_rbx +
                               expected_mov_r13 + expected_mov_r12 + expected_cmp + expected_je +
                               expected_test + expected_je_null + expected_mov_rsp10 + expected_mov_rsp40 +
                               expected_mov_rsp48 + expected_cmp_r13 + expected_je_532bba +
                               expected_lea_rsi;
    out.ok = (out.unit_executed == expected_units) && (out.call_54dc10_count == expected_call) &&
             (out.mov_rbx_rax_count == expected_mov_rbx) &&
             (out.call_54dc20_count == expected_count) &&
             (out.mov_r15_rax_count == expected_mov_r15) &&
             (out.mov_rsi_rbx_count == expected_mov_rsi) &&
             (out.mov_edx_eax_count == expected_mov_edx) &&
             (out.call_52a220_count == expected_fill) &&
             (out.lea_rbx_2efd160_count == expected_lea_rbx) &&
             (out.mov_r13_15fe518_count == expected_mov_r13) &&
             (out.mov_r12_rbx_count == expected_mov_r12) &&
             (out.cmp_r12_r14_count == expected_cmp) && (out.je_532bfb_count == expected_je) &&
             (out.test_r12_r12_count == expected_test) &&
             (out.je_532bf8_count == expected_je_null) &&
             (out.mov_r14_mem_rsp10_count == expected_mov_rsp10) &&
             (out.mov_r13_mem_rsp40_count == expected_mov_rsp40) &&
             (out.mov_r14_mem_rsp48_count == expected_mov_rsp48) &&
             (out.cmp_r13_r14_count == expected_cmp_r13) &&
             (out.je_532bba_count == expected_je_532bba) &&
             (out.lea_rsi_rsp10_count == expected_lea_rsi);
    return out;
}

}  // namespace sotc
