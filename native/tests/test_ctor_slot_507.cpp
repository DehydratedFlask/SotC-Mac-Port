// CPU-01-STARTUP-SLOT-507: fixture for startup constructor index 507 (0x22f3c0): the two bool console variables
// ExhaustiveDegenerateSplineSegmentLookahead and BruteForcePathSmoothing (both 1/1, tag 0x159b500). Lane M6.
#include "l4_test_util.hpp"

#include "core/ctor_slot_507_22f3c0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot507Rva == 0x22f3c0 && kSlot507Size == 567 && kSlot507BoolFnTag == 0x159b500);
    CHECK(std::strlen(kSlot507Var1Name) == kSlot507Var1NameLength && kSlot507Var1NameLength == 0x2a);
    CHECK(std::strlen(kSlot507Var2Name) == kSlot507Var2NameLength && kSlot507Var2NameLength == 0x17);
    CHECK(sizeof(Slot507Objects) == 2 * sizeof(ConCmdBoolVar) && sizeof(ConCmdBoolVar) == 0xc8);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot507Objects o;
    poison(o);
    const Slot507Result r = init_slot507_22f3c0(o, creg);
    CHECK(r.ok && r.first.ok && r.second.ok && r.first.views_inert && r.second.views_inert);
    CHECK(r.first.registration.ok && r.first.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.second.registration.ok && r.second.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_common(o.lookahead, 0x159b500, 1);
    CHECK(same_view(o.lookahead.base.name, "ExhaustiveDegenerateSplineSegmentLookahead", 0x2a));
    check_bool_common(o.smoothing, 0x159b500, 1);
    CHECK(same_view(o.smoothing.base.name, "BruteForcePathSmoothing", 0x17));
    CHECK(creg.pending.size() == 2);
    CHECK(creg.atexit_log.size() == 2);
    check_atexit(creg, 0, 0x20b0, &o.lookahead);
    check_atexit(creg, 1, 0x20b0, &o.smoothing);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot507Objects o;
    const Slot507Result r = init_slot507_22f3c0(o, creg);
    CHECK(!r.ok && !r.first.ok && !r.second.ok && !r.first.registration.ok && !r.second.registration.ok &&
          creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_507");
    return 0;
}
