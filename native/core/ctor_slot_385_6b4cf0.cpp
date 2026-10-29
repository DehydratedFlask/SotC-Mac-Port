#include "ctor_slot_385_6b4cf0.hpp"

namespace sotc {

Slot385Result init_slot385_6b4cf0(Slot385Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot385Result out{};
    const FConCmdCommandSpec spec{kSlot385CommandName, kSlot385CommandDescription, kSlot385CommandDescriptionLength,
                                  kSlot385HandlerTag};
    out.command = init_fconcmd_object(objects.command, spec, concmd);
    out.ok = out.command.ok;
    return out;
}

}  // namespace sotc
