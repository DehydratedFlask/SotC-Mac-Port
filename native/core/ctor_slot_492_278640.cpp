#include "ctor_slot_492_278640.hpp"

#include "l2_slot_common.hpp"

namespace sotc {

Slot492Result init_slot492_278640(Slot492Objects& objects, lane_m5::AppInfoQueue& queue,
                                  ConCmdRegistry& concmd) noexcept {
    Slot492Result out{};
    const lane_m5::NamedDelegateSpec spec{l2::literal_view(kSlot492Name), kSlot492HandlerTag};
    out.entry = lane_m5::init_named_delegate_entry(objects.entry, spec, queue, concmd.atexit_log);
    out.ok = out.entry.ok;
    return out;
}

}  // namespace sotc
