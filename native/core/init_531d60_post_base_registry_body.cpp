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
    out.end_rva = kInit531d60PostBaseRegistryBodyEndRva;
    const int expected_call = hooks.merge_call_54dc10_532b19_u1_ran ? 0 : 1;
    out.ok = (out.unit_executed == expected_call) && (out.call_54dc10_count == expected_call);
    return out;
}

}  // namespace sotc
