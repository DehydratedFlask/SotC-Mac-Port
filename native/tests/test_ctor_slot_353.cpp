// CPU-01-STARTUP-SLOT-353: fixture for startup constructor index 353 (0x7a14d0): bool variable
// `shadow_camera_normal_overrideDefaultPitchOffset` (tag 0x15ce1a0, 1/1) then the UNBOUNDED float variable
// `shadow_camera_normal_defaultPitchOffset` (0x6aa00, functor 0x15ce1c0, default 0). Lane L4.
#include "l4_test_util.hpp"

#include "core/ctor_slot_353_7a14d0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot353Rva == 0x7a14d0 && kSlot353Size == 569 && kSlot353BoolFnTag == 0x15ce1a0);
    CHECK(std::strlen(kSlot353BoolName) == 0x2f && std::strlen(kSlot353FloatSpec.name) == 0x27);
    CHECK(std::strcmp(kSlot353BoolName, "shadow_camera_normal_overrideDefaultPitchOffset") == 0);
    CHECK(std::strcmp(kSlot353FloatSpec.name, "shadow_camera_normal_defaultPitchOffset") == 0);
    CHECK(!kSlot353FloatSpec.bounded && kSlot353FloatSpec.default_bits == 0 && kSlot353FloatSpec.arg_fn_vtbl == 0x15ce1c0);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot353Objects o;
    poison(o);
    const Slot353Result r = init_slot353_7a14d0(o, creg);
    CHECK(r.ok && r.flag.ok && r.offset.ok);
    check_bool_common(o.flag, 0x15ce1a0, 1);
    check_float_common(o.offset, false, 0u, 0u, 0u);
    CHECK(same_view(o.flag.base.name, "shadow_camera_normal_overrideDefaultPitchOffset", 0x2f));
    CHECK(same_view(o.offset.base.name, "shadow_camera_normal_defaultPitchOffset", 0x27));
    CHECK(creg.pending.size() == 2 && creg.pending.first() == &o.flag.base.pending &&
          o.flag.base.pending.next == &o.offset.base.pending);
    CHECK(creg.atexit_log.size() == 2);
    check_atexit(creg, 0, 0x20b0, &o.flag);
    check_atexit(creg, 1, 0x20f0, &o.offset);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    Slot353Objects o;
    const Slot353Result r = init_slot353_7a14d0(o, creg);
    CHECK(!r.ok && !r.flag.ok && !r.offset.ctor_ok);
    CHECK(creg.pending.size() == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_353");
    return 0;
}
