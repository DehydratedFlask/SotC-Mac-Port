// CPU-01-STARTUP-BATCH-10: fixture for startup constructor index 480 (0x30daf0): three bounded float variables, two bool variables and the console commands
// `clearPersistentMessages` / `clearErrorMessages`. The survey's "register indirect calls" are the console-command register-once slot
// [0x1594dc0 + 0x20] = 0x20d60 and its "slot-3 sites" the float caller-functor destroy of tag 0x15a66b8 (both checker-pinned).
#include "l4_test_util.hpp"

#include "core/ctor_slot_480_30daf0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot480Rva == 0x30daf0 && kSlot480Size == 1907);
    CHECK(is_known_float_arg_functor(kSlot480FloatFnTag) && is_float_functor_tag(kSlot480FloatFnTag));
    for (std::size_t n = 0; n < kSlot480FloatCount; ++n) CHECK(std::strlen(kSlot480FloatNames[n]) == kSlot480FloatNameLengths[n]);
    for (std::size_t n = 0; n < kSlot480BoolCount; ++n) CHECK(std::strlen(kSlot480BoolNames[n]) == kSlot480BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot480CommandCount; ++n) {
        CHECK(std::strlen(kSlot480CommandDescriptions[n]) == kSlot480CommandDescriptionLengths[n]);
    }
    CHECK(sizeof(ConCmdBoolVar) == 0xc8 && sizeof(ConCmdRangedVar) == 0xd8 && sizeof(ConCmdCommand) == 0xa0);
    CHECK(kSlot480Bool0Rva - kSlot480Float0Rva == offsetof(Slot480Objects, bvar));
    CHECK(kSlot480Command0Rva - kSlot480Float0Rva == offsetof(Slot480Objects, command));
    CHECK(sizeof(Slot480Objects) == 0x2ef03d8 - kSlot480Float0Rva);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot480Objects o;
    poison(o);
    const Slot480Result r = init_slot480_30daf0(o, creg);
    CHECK(r.ok && r.rows_done == 7);
    for (std::size_t n = 0; n < kSlot480FloatCount; ++n) {
        check_float_common(o.fvar[n], true, kSlot480FloatLoBits, kSlot480FloatHiBits, kSlot480FloatValueBits[n]);
        CHECK(same_view(o.fvar[n].base.name, kSlot480FloatNames[n], kSlot480FloatNameLengths[n]));
    }
    for (std::size_t n = 0; n < kSlot480BoolCount; ++n) {
        check_bool_common(o.bvar[n], kSlot480BoolFnTag, kSlot480BoolValue);
        CHECK(same_view(o.bvar[n].base.name, kSlot480BoolNames[n], kSlot480BoolNameLengths[n]));
    }
    for (std::size_t n = 0; n < kSlot480CommandCount; ++n) {
        CHECK(r.command[n].ok && r.command[n].vtable_register_slot_ok && r.command[n].views_inert && r.command[n].atexit_recorded && !r.command[n].handler_invoked);
        check_command(o.command[n], kSlot480CommandNames[n], kSlot480CommandDescriptions[n], kSlot480CommandDescriptionLengths[n], kSlot480HandlerTags[n]);
    }
    // registration order is the construction order
    CHECK(creg.pending.size() == 7 && creg.pending.first() == &o.fvar[0].base.pending);
    CHECK(o.fvar[0].base.pending.next == &o.fvar[1].base.pending && o.fvar[1].base.pending.next == &o.fvar[2].base.pending);
    CHECK(o.fvar[2].base.pending.next == &o.bvar[0].base.pending && o.bvar[0].base.pending.next == &o.bvar[1].base.pending);
    CHECK(o.bvar[1].base.pending.next == &o.command[0].base.pending && o.command[0].base.pending.next == &o.command[1].base.pending);
    CHECK(o.command[1].base.pending.next == &creg.pending.head);
    // atexit: floats 0x20f0 x3, bools 0x20b0 x2, commands 0x4b0 x2
    CHECK(creg.atexit_log.size() == 7);
    for (std::size_t n = 0; n < kSlot480FloatCount; ++n) check_atexit(creg, n, 0x20f0, &o.fvar[n]);
    for (std::size_t n = 0; n < kSlot480BoolCount; ++n) check_atexit(creg, 3 + n, 0x20b0, &o.bvar[n]);
    for (std::size_t n = 0; n < kSlot480CommandCount; ++n) check_atexit(creg, 5 + n, 0x4b0, &o.command[n]);
}

void section_failures() {
    {   // manager without an insert contract: the first float registration fails and nothing after it is built
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot480Objects o;
        poison(o);
        const Slot480Result r = init_slot480_30daf0(o, creg);
        CHECK(!r.ok && r.rows_done == 0 && creg.pending.size() == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_480");
    return 0;
}
