// CPU-01-STARTUP-SLOT-333-01 (lane L3): fixture for startup constructor index 333 (0x7fd6d0, 550 B): the
// two time-attack console COMMANDS `shadow_timeAttackSetCompletionCount` (0x3288640, handler tag
// 0x7fd4d0) and `shadow_timeAttackSetRetroactiveCompletionCount` (0x32886e0, handler tag 0x7fd590), built
// with the slot-306 command object shape (flags 2, vptr 0x1594dd0, delegate {self, 0x1990, 0, handler}).
// Sections: 1 constants, 2 every stored/untouched byte, list and atexit order, 3 the handler tag reaches
// the invoker without a handler being called by construction, 4 visible failures.
#include "ctor_slot_family_test_util.hpp"

#include "core/ctor_slots_322_334.hpp"

namespace {

using namespace famtest;

void section_constants() {
    CHECK(kSlot333Rva == 0x7fd6d0 && kSlot333BodySize == 550);
    CHECK(kSlot333CmdARva == 0x3288640 && kSlot333CmdBRva == 0x32886e0);
    CHECK(kSlot333CmdBRva - kSlot333CmdARva == kFConCmdObjectSize);   // consecutive 0xa0 B objects
    CHECK(kSlot333CmdAHandler == 0x7fd4d0 && kSlot333CmdBHandler == 0x7fd590);
    CHECK(sizeof kSlot333CmdAName - 1 == 35 && std::strcmp(kSlot333CmdAName, "shadow_timeAttackSetCompletionCount") == 0);
    CHECK(sizeof kSlot333CmdBName - 1 == 46 &&
          std::strcmp(kSlot333CmdBName, "shadow_timeAttackSetRetroactiveCompletionCount") == 0);
    CHECK(sizeof kSlot333CmdADescription - 1 == 0x75 && sizeof kSlot333CmdBDescription - 1 == 0xb2);
    CHECK(std::strcmp(kSlot333CmdADescription,
                      "Sets how many colossi have been defeated in time attack mode. "
                      "+shadow_timeAttackSetCompletionCount <number completed>") == 0);
}

void check_command(const ConCmdCommand& c, const char* name, std::size_t name_len, const char* description,
                   std::size_t description_len, std::uint64_t handler) {
    CHECK(c.base.vptr == kFConCmdVptr && c.base.registered == 1 && c.base.flags == kFConCmdFlags);
    CHECK(same_view(c.base.name, name, name_len));
    CHECK(same_view(c.base.description, description, description_len));
    CHECK(c.delegate.object == reinterpret_cast<std::uint64_t>(&c.delegate));   // self address (handler != 0)
    CHECK(c.delegate.fn == 0x1990 && c.delegate.adj == 0 && c.delegate.handler == handler);
    CHECK(c.args_begin == nullptr && c.args_end == nullptr && c.args_cap == nullptr);
    // Stores: base ctor + registration link, vptr, delegate block, zeroed vector. +0x19..+0x1b, +0x98.. untouched.
    check_untouched(c, {{0x00, 8}, {0x08, 0x10}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 0x38}});
}

int record_handler(std::uint64_t tag, ConCmdCommand&, void*) noexcept {
    return static_cast<int>(tag & 0xff);
}

void section_alone() {
    ConCmdRegistry creg;
    Slot333Objects o;
    poison(o);
    const Slot333Result r = init_slot333_7fd6d0(o, creg);
    CHECK(r.ok && r.cmd_a.ok && r.cmd_b.ok);
    for (const FConCmdBuildResult* b : {&r.cmd_a, &r.cmd_b}) {
        CHECK(b->vtable_register_slot_ok && b->registration.ok && b->views_inert && b->atexit_recorded);
        CHECK(!b->handler_invoked);
        CHECK(b->registration.path == ConCmdRegisterPath::pending_list);
    }
    check_command(o.cmd_a, kSlot333CmdAName, sizeof kSlot333CmdAName - 1, kSlot333CmdADescription,
                  sizeof kSlot333CmdADescription - 1, 0x7fd4d0);
    check_command(o.cmd_b, kSlot333CmdBName, sizeof kSlot333CmdBName - 1, kSlot333CmdBDescription,
                  sizeof kSlot333CmdBDescription - 1, 0x7fd590);
    CHECK(creg.pending.size() == 2 && creg.pending.first() == &o.cmd_a.base.pending);
    CHECK(o.cmd_a.base.pending.next == &o.cmd_b.base.pending && o.cmd_b.base.pending.next == &creg.pending.head);
    CHECK(creg.atexit_log.size() == 2);
    CHECK(creg.atexit_log.at(0).dtor_rva == 0x4b0 && creg.atexit_log.at(0).object == &o.cmd_a);
    CHECK(creg.atexit_log.at(1).dtor_rva == 0x4b0 && creg.atexit_log.at(1).object == &o.cmd_b);
}

// The handler words are identity tags: the bound-call head of the execute slot hands the tag of THIS
// command to the host hook (thunk 0x1990), and construction never called any handler.
void section_handler_tags() {
    ConCmdRegistry creg;
    Slot333Objects o;
    CHECK(init_slot333_7fd6d0(o, creg).ok);
    FConCmdInvokeHooks hooks;
    hooks.handler = &record_handler;
    const FConCmdInvokeResult a = invoke_delegate_head_21dc6(o.cmd_a, nullptr, hooks);
    const FConCmdInvokeResult b = invoke_delegate_head_21dc6(o.cmd_b, nullptr, hooks);
    CHECK(a.ok && a.status == (0x7fd4d0 & 0xff) && b.ok && b.status == (0x7fd590 & 0xff));
    // Without a host hook the call fails closed instead of pretending the command ran.
    CHECK(!invoke_delegate_head_21dc6(o.cmd_a, nullptr, FConCmdInvokeHooks{}).ok);
}

void section_failures() {
    {   // a manager without an insert contract: both commands fail visibly
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot333Objects o;
        const Slot333Result r = init_slot333_7fd6d0(o, creg);
        CHECK(!r.ok && !r.cmd_a.ok && !r.cmd_b.ok && !r.cmd_a.registration.ok);
        CHECK(creg.pending.size() == 0);
    }
    {   // two runs share the registry: four pending entries, four atexit records
        ConCmdRegistry creg;
        Slot333Objects first, second;
        CHECK(init_slot333_7fd6d0(first, creg).ok && init_slot333_7fd6d0(second, creg).ok);
        CHECK(creg.pending.size() == 4 && first.cmd_b.base.pending.next == &second.cmd_a.base.pending);
        CHECK(creg.atexit_log.size() == 4);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_handler_tags();
    section_failures();
    std::puts("OK test_ctor_slot_333");
    return 0;
}
