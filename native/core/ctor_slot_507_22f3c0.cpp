#include "ctor_slot_507_22f3c0.hpp"

namespace sotc {

Slot507Result init_slot507_22f3c0(Slot507Objects& objects, ConCmdRegistry& concmd) noexcept {
    Slot507Result out{};
    // 0x22f421..: ExhaustiveDegenerateSplineSegmentLookahead, empty description, value 1 / default 1
    out.first = lane_m6::init_bool_var_literal(objects.lookahead, kSlot507Var1Name, kSlot507Var1NameLength, "", 0,
                                               kSlot507BoolFnTag, concmd, 1, 1);
    // 0x22f522..: BruteForcePathSmoothing, empty description, value 1 / default 1 (runs even if the first failed)
    out.second = lane_m6::init_bool_var_literal(objects.smoothing, kSlot507Var2Name, kSlot507Var2NameLength, "", 0,
                                                kSlot507BoolFnTag, concmd, 1, 1);
    out.ok = out.first.ok && out.second.ok;
    return out;
}

}  // namespace sotc
