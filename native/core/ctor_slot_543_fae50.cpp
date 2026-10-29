#include "ctor_slot_543_fae50.hpp"

#include "l2_slot_common.hpp"

namespace sotc {

Slot543Result init_slot543_fae50(Slot543Objects& objects, lane_m5::AppInfoQueue& queue,
                                  ConCmdRegistry& concmd) noexcept {
    Slot543Result out{};
    const lane_m5::NamedDelegateSpec spec{l2::literal_view(kSlot543Name), kSlot543HandlerTag};
    out.entry = lane_m5::init_named_delegate_entry(objects.entry, spec, queue, concmd.atexit_log);
    out.ok = out.entry.ok;
    return out;
}

}  // namespace sotc
