// CPU-01-STARTUP-SLOT-478: fixture for startup constructor index 478 (0x31eb00): the single bool console
// variable `uiTextShowExtents` WITH a description (value 0, default 0, functor tag 0x15a7260), late-release shape. Lane M4.
#include "l4_test_util.hpp"

#include "core/ctor_slot_478_31eb00.hpp"
#include "core/ctor_slot_475_33cc60.hpp"

namespace {
using namespace l4;

void check_bool_with_description(const ConCmdBoolVar& v, std::uint64_t fn_tag, std::uint8_t value,
                                 const char* description, std::size_t description_length) {
    CHECK(v.base.vptr == kBoolVarFinalVptr);
    CHECK(v.value_ref == &v.value);
    for (std::uint8_t byte : v.zero69) CHECK(byte == 0);
    for (std::uint8_t byte : v.zero70) CHECK(byte == 0);
    CHECK(v.fn_payload == 0 && v.fn_aux == 0 && v.fn_vptr == fn_tag);
    CHECK(v.base.registered == 1 && v.base.flags == 1);
    CHECK(v.value == value && v.default_value == value);
    check_untouched(v, {{0x00, 8}, {0x08, 0x10}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 8}, {0x68, 1},
                        {0x69, 4}, {0x70, 0x20}, {0xa0, 8}, {0xb0, 8}, {0xb8, 8}, {0xc0, 1}});
    CHECK(same_view(v.base.description, description, description_length));
}

void section_constants() {
    CHECK(kSlot478Rva == 0x31eb00 && kSlot478Size == 285);
    CHECK(sizeof kSlot478Name - 1 == 17 && std::strcmp(kSlot478Name, "uiTextShowExtents") == 0);
    CHECK(sizeof kSlot478Description - 1 == 69 && std::strlen(kSlot478Description) == 69);
    CHECK(kSlot478FnTag == 0x15a7260 && kSlot478ObjectRva == 0x2ef04a0);
    CHECK(kSlot478Value == 0 && kSlot478Default == 0);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot478Objects o;
    poison(o);
    const Slot478Result r = init_slot478_31eb00(o, creg);
    CHECK(r.ok && r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_with_description(o.var, 0x15a7260, 0, kSlot478Description, 69);
    CHECK(same_view(o.var.base.name, "uiTextShowExtents", 17));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &o.var.base.pending);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x20b0, &o.var);
}

void section_with_neighbour() {
    ConCmdRegistry creg;
    Slot478Objects a;
    Slot475Objects b;
    CHECK(init_slot478_31eb00(a, creg).ok);
    CHECK(init_slot475_33cc60(b, creg).ok);
    CHECK(creg.pending.size() == 2);
    CHECK(a.var.base.pending.next == &b.var.base.pending && b.var.base.pending.next == &creg.pending.head);
    CHECK(creg.atexit_log.size() == 2);
    check_atexit(creg, 0, 0x20b0, &a.var);
    check_atexit(creg, 1, 0x20b0, &b.var);
}

void section_failures() {
    {   // A manager without an insert contract: the registration fails visibly and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot478Objects o;
        const Slot478Result r = init_slot478_31eb00(o, creg);
        CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
        CHECK(creg.pending.size() == 0);
    }
    {   // The late release is only an exact no-op for borrowed views: an owned description is reported.
        ConCmdRegistry creg;
        ConCmdBoolVar var{};
        std::int64_t control = 1;
        const SharedStr name{reinterpret_cast<const std::uint8_t*>("n"), 1, 0, nullptr};
        const SharedStr desc{reinterpret_cast<const std::uint8_t*>("d"), 1, 0, &control};
        const BoolVarResult r = lane_m4::init_bool_var_late_release(var, name, desc, 0x15a7260, creg, 0, 0);
        CHECK(!r.ok && !r.views_inert && r.registration.ok && r.atexit_recorded);
        CHECK(control == 1);   // 0x20af0 acquired it (2); the real 0xf590 released it again (1)
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_with_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_478");
    return 0;
}
