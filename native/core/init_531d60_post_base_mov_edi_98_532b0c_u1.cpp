#include "init_531d60_post_base_mov_edi_98_532b0c_u1.hpp"

namespace sotc {

Init531d60PostBaseMovEdi98_532b0c_U1Result init_531d60_post_base_mov_edi_98_532b0c_u1() noexcept {
    Init531d60PostBaseMovEdi98_532b0c_U1Result out{};
    out.edi = kInit531d60PostBaseMovEdi98_532b0c_U1Immediate;
    ++out.mov_edi_count;
    out.ok = (out.mov_edi_count == 1 && out.edi == 0x98);
    return out;
}

}  // namespace sotc
