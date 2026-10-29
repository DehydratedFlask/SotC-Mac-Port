#include "ctor_slot_386_6b12f0.hpp"

namespace sotc {

Slot386Result init_slot386_6b12f0(Slot386Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot386Result out{};
    const FConCmdCommandSpec spec{kSlot386CommandName, kSlot386CommandDescription, kSlot386CommandDescriptionLength,
                                  kSlot386HandlerTag};
    out.command = init_fconcmd_object(objects.command, spec, concmd);
    out.ok = out.command.ok;
    return out;
}

}  // namespace sotc
