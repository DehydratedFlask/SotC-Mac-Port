#include "ctor_slot_359_785f60.hpp"

namespace sotc {

Slot359Result init_slot359_785f60(Slot359Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot359Result out{};
    const FConCmdCommandSpec spec{kSlot359CommandName, kSlot359CommandDescription, kSlot359CommandDescriptionLength,
                                  kSlot359HandlerTag};
    out.command = init_fconcmd_object(objects.command, spec, concmd);
    out.ok = out.command.ok;
    return out;
}

}  // namespace sotc
