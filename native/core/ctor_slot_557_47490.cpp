#include "ctor_slot_557_47490.hpp"

namespace sotc {

Slot557Result init_slot557_47490(Slot557Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot557Result out{};
    const FConCmdCommandSpec spec{kSlot557CommandName, kSlot557CommandDescription, kSlot557CommandDescriptionLength,
                                  kSlot557HandlerTag};
    out.command = init_fconcmd_object(objects.command, spec, concmd);
    out.ok = out.command.ok;
    return out;
}

}  // namespace sotc
