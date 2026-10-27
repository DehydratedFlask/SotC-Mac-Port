#include "ctor_slot_343_7d7970.hpp"

namespace sotc {

Slot343Result init_slot343_7d7970(Slot343Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot343Result out{};
    out.cell_a_atexit = init_zero_vector24(objects.cell_a, concmd);
    if (!out.cell_a_atexit) return out;
    out.cell_b_atexit = init_zero_vector24(objects.cell_b, concmd);
    if (!out.cell_b_atexit) return out;
    const FConCmdCommandSpec spec{kSlot343CommandName, kSlot343CommandDescription, kSlot343CommandDescriptionLength,
                                  kSlot343HandlerTag};
    out.command = init_fconcmd_object(objects.command, spec, concmd);
    out.ok = out.command.ok;
    return out;
}

}  // namespace sotc
