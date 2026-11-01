#include "registry_init_531d60.hpp"

#include "registry_sorted_insert.hpp"

#include <cstring>

namespace sotc {
namespace {

void destroy_registry_object(RegistryObject2efd160* obj, AllocatorFront& alloc) noexcept {
    if (obj == nullptr) {
        return;
    }
    // OBS destroy path frees +0x40 begin, then +0x20, then +0x00, then delete_ object.
    // Listener slot vcall-dtors are NOT modelled; fill leaves listeners null.
    if (obj->listeners_begin != nullptr) {
        alloc.delete_array(obj->listeners_begin);
        obj->listeners_begin = obj->listeners_end = obj->listeners_cap = nullptr;
    }
    clear_registry_object_tables_52a220(*obj, alloc);
    alloc.delete_(obj);
}

}  // namespace

std::atomic<RegistryObject2efd160*>& registry_cell_2efd160() noexcept {
    static std::atomic<RegistryObject2efd160*> cell{nullptr};
    return cell;
}

void destroy_registry_at_2efd160(AllocatorFront& alloc) noexcept {
    RegistryObject2efd160* prev = registry_cell_2efd160().exchange(nullptr);
    destroy_registry_object(prev, alloc);
}

RegistryInit531d60Status init_registry_fragment_531d60(InputTable551140& table, AllocatorFront& alloc, const RegistryInit531d60Hooks& hooks) {
    if (!hooks.merge_mov_edi_98_532b0c_u1_ran) {
        const auto prep = init_531d60_post_base_mov_edi_98_532b0c_u1();
        if (!prep.ok) {
            return RegistryInit531d60Status::alloc_failed;
        }
    }
    if (!hooks.merge_call_17070_532b11_u1_ran) {
        const auto call = init_531d60_post_base_call_17070_532b11_u1();
        if (!call.ok) {
            return RegistryInit531d60Status::alloc_failed;
        }
    }
    if (!hooks.merge_mov_r14_rax_532b16_u1_ran) {
        const auto mov_r14 = init_531d60_post_base_mov_r14_rax_532b16_u1();
        if (!mov_r14.ok) {
            return RegistryInit531d60Status::alloc_failed;
        }
    }
    void* rax_from_54dc10 = nullptr;
    if (!hooks.merge_call_54dc10_532b19_u1_ran) {
        Init531d60PostBaseCall54dc10_532b19_U1Cells cells{};
        cells.table_in = &table;
        const auto table_base = init_531d60_post_base_call_54dc10_532b19_u1(cells);
        if (!table_base.ok) {
            return RegistryInit531d60Status::alloc_failed;
        }
        rax_from_54dc10 = table_base.rax;
    }
    if (!hooks.merge_mov_rbx_rax_532b1e_u1_ran) {
        Init531d60PostBaseMovRbxRax_532b1e_U1Cells cells{};
        cells.rax_in = rax_from_54dc10;
        const auto mov_rbx = init_531d60_post_base_mov_rbx_rax_532b1e_u1(cells);
        if (!mov_rbx.ok) {
            return RegistryInit531d60Status::alloc_failed;
        }
    }
    std::uint32_t count_from_54dc20 = 0;
    if (!hooks.merge_call_54dc20_532b21_u1_ran) {
        const auto count_call = init_531d60_post_base_call_54dc20_532b21_u1();
        if (!count_call.ok) {
            return RegistryInit531d60Status::alloc_failed;
        }
        count_from_54dc20 = count_call.count;
    }
    void* rbx_from_54dc10 = rax_from_54dc10;
    if (!hooks.merge_mov_r15_rax_532b26_u1_ran) {
        Init531d60PostBaseMovR15Rax_532b26_U1Cells cells{};
        cells.rax_in = count_from_54dc20;
        const auto mov_r15 = init_531d60_post_base_mov_r15_rax_532b26_u1(cells);
        if (!mov_r15.ok) {
            return RegistryInit531d60Status::alloc_failed;
        }
    }
    if (!hooks.merge_mov_rsi_rbx_532b29_u1_ran) {
        Init531d60PostBaseMovRsiRbx_532b29_U1Cells cells{};
        cells.rbx_in = rbx_from_54dc10;
        const auto mov_rsi = init_531d60_post_base_mov_rsi_rbx_532b29_u1(cells);
        if (!mov_rsi.ok) {
            return RegistryInit531d60Status::alloc_failed;
        }
    }
    if (!hooks.merge_mov_edx_eax_532b2c_u1_ran) {
        Init531d60PostBaseMovEdxEax_532b2c_U1Cells cells{};
        cells.eax_in = count_from_54dc20;
        const auto mov_edx = init_531d60_post_base_mov_edx_eax_532b2c_u1(cells);
        if (!mov_edx.ok) {
            return RegistryInit531d60Status::alloc_failed;
        }
    }
    void* raw = alloc.new_(kRegistryInitObjectBytes);
    if (raw == nullptr) {
        return RegistryInit531d60Status::alloc_failed;
    }
    // Native zeros before fill so a missing-names early return can delete_ safely without
    // interpreting poison as vector heads. Original relies on 0x52a220 writing the prologue.
    std::memset(raw, 0, kRegistryInitObjectBytes);
    auto* obj = static_cast<RegistryObject2efd160*>(raw);

    const RegistryFill52a220Status fill =
        fill_registry_object_52a220(*obj, table, alloc, input_table_551140_count());
    if (fill != RegistryFill52a220Status::ok) {
        destroy_registry_object(obj, alloc);
        if (fill == RegistryFill52a220Status::missing_names) {
            return RegistryInit531d60Status::missing_names;
        }
        return RegistryInit531d60Status::fill_failed;
    }

    if (!hooks.merge_lea_rbx_2efd160_532b33_u1_ran) {
        const auto lea_rbx = init_531d60_post_base_lea_rbx_2efd160_532b33_u1();
        if (!lea_rbx.ok) {
            destroy_registry_object(obj, alloc);
            return RegistryInit531d60Status::alloc_failed;
        }
    }

    if (!hooks.merge_mov_r13_15fe518_532b3a_u1_ran) {
        const auto mov_r13 = init_531d60_post_base_mov_r13_15fe518_532b3a_u1();
        if (!mov_r13.ok) {
            destroy_registry_object(obj, alloc);
            return RegistryInit531d60Status::alloc_failed;
        }
    }

    void* r12_from_cell = nullptr;
    if (!hooks.merge_mov_r12_rbx_532b41_u1_ran) {
        const auto mov_r12 = init_531d60_post_base_mov_r12_rbx_532b41_u1();
        if (!mov_r12.ok) {
            destroy_registry_object(obj, alloc);
            return RegistryInit531d60Status::alloc_failed;
        }
        r12_from_cell = mov_r12.r12;
    }

    bool pointers_equal = false;
    bool pointers_equal_known = false;
    if (!hooks.merge_cmp_r12_r14_532b44_u1_ran) {
        Init531d60PostBaseCmpR12R14_532b44_U1Cells cells{};
        cells.r12_in = r12_from_cell;
        cells.r14_in = obj;
        const auto cmp = init_531d60_post_base_cmp_r12_r14_532b44_u1(cells);
        if (!cmp.ok) {
            destroy_registry_object(obj, alloc);
            return RegistryInit531d60Status::alloc_failed;
        }
        pointers_equal = cmp.pointers_equal;
        pointers_equal_known = true;
    }

    if (!hooks.merge_je_532bfb_532b47_u1_ran) {
        Init531d60PostBaseJe532bfb_532b47_U1Cells je_cells{};
        je_cells.pointers_equal = pointers_equal;
        je_cells.pointers_equal_known = pointers_equal_known;
        const auto je = init_531d60_post_base_je_532bfb_532b47_u1(je_cells);
        if (!je.ok) {
            destroy_registry_object(obj, alloc);
            return RegistryInit531d60Status::alloc_failed;
        }
        if (je.branch_taken) {
            return RegistryInit531d60Status::ok;
        }
    }

    bool r12_is_zero = false;
    bool r12_is_zero_known = false;
    if (!hooks.merge_test_r12_r12_532b4d_u1_ran) {
        Init531d60PostBaseTestR12R12_532b4d_U1Cells test_cells{};
        test_cells.r12_in = r12_from_cell;
        const auto test = init_531d60_post_base_test_r12_r12_532b4d_u1(test_cells);
        if (!test.ok) {
            destroy_registry_object(obj, alloc);
            return RegistryInit531d60Status::alloc_failed;
        }
        r12_is_zero = test.r12_is_zero;
        r12_is_zero_known = true;
    }

    if (!hooks.merge_je_532bf8_532b50_u1_ran) {
        Init531d60PostBaseJe532bf8_532b50_U1Cells je_cells{};
        je_cells.r12_is_zero = r12_is_zero;
        je_cells.r12_is_zero_known = r12_is_zero_known;
        const auto je = init_531d60_post_base_je_532bf8_532b50_u1(je_cells);
        if (!je.ok) {
            destroy_registry_object(obj, alloc);
            return RegistryInit531d60Status::alloc_failed;
        }
        // NOT modelled: branch to 0x532bf8 vs destroy fall-through @ 0x532b56 (E562 merge rule).
    }

    if (!hooks.merge_mov_r14_mem_rsp10_532b56_u1_ran) {
        Init531d60PostBaseMovR14MemRsp10_532b56_U1Cells cells{};
        cells.r14_in = obj;
        const auto mov_rsp = init_531d60_post_base_mov_r14_mem_rsp10_532b56_u1(cells);
        if (!mov_rsp.ok) {
            destroy_registry_object(obj, alloc);
            return RegistryInit531d60Status::alloc_failed;
        }
    }

    void* r13_from_rsp40 = nullptr;
    if (!hooks.merge_mov_r13_mem_rsp40_532b5b_u1_ran) {
        const auto mov_r13 = init_531d60_post_base_mov_r13_mem_rsp40_532b5b_u1({});
        if (!mov_r13.ok) {
            destroy_registry_object(obj, alloc);
            return RegistryInit531d60Status::alloc_failed;
        }
        r13_from_rsp40 = mov_r13.r13_loaded;
    }

    void* r14_from_rsp48 = nullptr;
    if (!hooks.merge_mov_r14_mem_rsp48_532b60_u1_ran) {
        const auto mov_r14 = init_531d60_post_base_mov_r14_mem_rsp48_532b60_u1({});
        if (!mov_r14.ok) {
            destroy_registry_object(obj, alloc);
            return RegistryInit531d60Status::alloc_failed;
        }
        r14_from_rsp48 = mov_r14.r14_loaded;
    }

    bool r13_r14_equal = false;
    bool r13_r14_equal_known = false;
    if (!hooks.merge_cmp_r13_r14_532b65_u1_ran) {
        Init531d60PostBaseCmpR13R14_532b65_U1Cells cells{};
        cells.r13_in = r13_from_rsp40;
        cells.r14_in = r14_from_rsp48;
        const auto cmp = init_531d60_post_base_cmp_r13_r14_532b65_u1(cells);
        if (!cmp.ok) {
            destroy_registry_object(obj, alloc);
            return RegistryInit531d60Status::alloc_failed;
        }
        r13_r14_equal = cmp.pointers_equal;
        r13_r14_equal_known = true;
    } else {
        // cmp ran in registry_body merge; je still records the observational branch.
        r13_r14_equal = (r13_from_rsp40 == r14_from_rsp48);
        r13_r14_equal_known = true;
    }

    if (!hooks.merge_je_532bba_532b68_u1_ran) {
        Init531d60PostBaseJe532bba_532b68_U1Cells je_cells{};
        je_cells.pointers_equal = r13_r14_equal;
        je_cells.pointers_equal_known = r13_r14_equal_known;
        const auto je = init_531d60_post_base_je_532bba_532b68_u1(je_cells);
        if (!je.ok) {
            destroy_registry_object(obj, alloc);
            return RegistryInit531d60Status::alloc_failed;
        }
        // NOT modelled: branch to 0x532bba vs fall-through @ 0x532b6a (E569 merge rule).
    }

    // OBS 0x532b6a..0x532bf8: destroy previous if different & non-null; then movq new, (0x2efd160).
    RegistryObject2efd160* prev = registry_cell_2efd160().load();
    if (prev == obj) {
        return RegistryInit531d60Status::ok;
    }
    if (prev != nullptr) {
        registry_cell_2efd160().store(nullptr);
        destroy_registry_object(prev, alloc);
    }
    registry_cell_2efd160().store(obj);
    return RegistryInit531d60Status::ok;
}

}  // namespace sotc
