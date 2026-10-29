#include "ctor_slot_440_4f0c60.hpp"

namespace sotc {

Slot440Result init_slot440_4f0c60(Slot440Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot440Result out{};
    const FConCmdCommandSpec spec{kSlot440CommandName, kSlot440CommandDescription, kSlot440CommandDescriptionLength,
                                  kSlot440HandlerTag};
    out.command = init_fconcmd_object(objects.command, spec, concmd);
    if (!out.command.ok) return out;
    objects.hashes[0] = fnv_cstr_a2ddc0(kSlot440Hash0);
    objects.hashes[1] = fnv_cstr_a2ddc0(kSlot440Hash1);
    objects.hashes[2] = fnv_cstr_a2ddc0(kSlot440Hash2);
    objects.hashes[3] = fnv_cstr_a2ddc0(kSlot440Hash3);
    out.hashes_stored = true;
    out.ok = out.command.ok && out.hashes_stored;
    return out;
}

}  // namespace sotc
