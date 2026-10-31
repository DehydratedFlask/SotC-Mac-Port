#include "init_531d60_post_base_call_54dc10_532b19_u1.hpp"

#include "input_table_551140.hpp"

namespace sotc {

Init531d60PostBaseCall54dc10_532b19_U1Result init_531d60_post_base_call_54dc10_532b19_u1(
    const Init531d60PostBaseCall54dc10_532b19_U1Cells& cells) noexcept {
    Init531d60PostBaseCall54dc10_532b19_U1Result out{};
    void* rax = nullptr;
    if (cells.table_in != nullptr) {
        rax = input_table_551140_base(*cells.table_in);
    }
    out.rax = rax;
    if (cells.rax_out != nullptr) {
        *cells.rax_out = rax;
    }
    ++out.call_54dc10_count;

    // Next: mov rbx,rax @ 0x532b1e — not modelled in this unit.
    out.ok = (out.call_54dc10_count == 1);
    return out;
}

}  // namespace sotc
