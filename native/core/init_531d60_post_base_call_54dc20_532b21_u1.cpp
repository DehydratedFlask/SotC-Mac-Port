#include "init_531d60_post_base_call_54dc20_532b21_u1.hpp"

#include "input_table_551140.hpp"

namespace sotc {

Init531d60PostBaseCall54dc20_532b21_U1Result init_531d60_post_base_call_54dc20_532b21_u1(
    const Init531d60PostBaseCall54dc20_532b21_U1Cells& cells) noexcept {
    Init531d60PostBaseCall54dc20_532b21_U1Result out{};
    const std::uint32_t count = input_table_551140_count();
    out.count = count;
    if (cells.eax_out != nullptr) {
        *cells.eax_out = count;
    }
    ++out.call_54dc20_count;

    // Next: mov r15,rax @ 0x532b26 — peeled as registry_body unit E554.
    out.ok = (out.call_54dc20_count == 1);
    return out;
}

}  // namespace sotc
