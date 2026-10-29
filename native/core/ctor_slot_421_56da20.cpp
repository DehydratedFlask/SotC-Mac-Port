#include "ctor_slot_421_56da20.hpp"

#include <cstring>

namespace sotc {

Slot421Result init_slot421_56da20(Slot421Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot421Result out{};
    out.var = lane_m2::init_bool_var_flagged(objects.var, kSlot421VarName, kSlot421FnTag, kSlot421CtorFlags, concmd,
                                             0, 0);
    if (!out.var.ok) return out;
    std::uint8_t* const block = objects.block.data();    // rax = 0x2f090d0
    std::memset(block + 0x60, 0, 0x20);                  // vmovups ymmword [rax+0x60], ymm0 (zero)
    std::memset(block + 0x40, 0, 0x20);                  // [rax+0x40]
    std::memset(block + 0x20, 0, 0x20);                  // [rax+0x20]
    std::memset(block + 0x00, 0, 0x20);                  // [rax]
    out.ok = out.var.ok;
    return out;
}

}  // namespace sotc
