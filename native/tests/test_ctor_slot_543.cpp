// CPU-01-STARTUP-SLOT-543: fixture for startup constructor index 543 (0xfae50): the named delegate entry `UpdateMaterialProperties`
// queued on the application-info queue. Lane M5.
#include "l4_test_util.hpp"

#include "core/l2_slot_common.hpp"

#include "core/ctor_slot_543_fae50.hpp"
#include "core/ctor_slot_492_278640.hpp"

namespace {
using namespace l4;
using lane_m5::AppInfoQueue;
using lane_m5::AppInfoQueueEntry;

void check_entry(const AppInfoQueueEntry& e, const char* name, std::size_t len, std::uint64_t handler, const AppInfoQueueEntry* next) {
    CHECK(e.registered == 1);
    CHECK(same_view(e.name, name, len));
    CHECK(e.next == next);
    CHECK(e.delegate.object == reinterpret_cast<std::uint64_t>(&e.delegate));
    CHECK(e.delegate.fn == 0x4cca0 && e.delegate.adj == 0 && e.delegate.handler == handler);
    check_untouched(e, {{0x00, 1}, {0x08, 0x20}, {0x28, 8}, {0x30, 0x20}});
}

void section_constants() {
    CHECK(kSlot543Rva == 0xfae50 && kSlot543Size == 289 && kSlot543ObjectRva == 0x2edef10 && kSlot543HandlerTag == 0xf9520);
    CHECK(sizeof kSlot543Name - 1 == 24 && std::strcmp(kSlot543Name, "UpdateMaterialProperties") == 0);
    CHECK(lane_m5::kEntrySize == 0x50 && lane_m5::kDelegateInvokerRva == 0x4cca0 && lane_m5::kEntryDtorRva == 0x4b660);
}

void section_alone() {
    ConCmdRegistry creg;
    AppInfoQueue q;
    Slot543Objects o;
    poison(o);
    const Slot543Result r = init_slot543_fae50(o, q, creg);
    CHECK(r.ok && r.entry.ok && r.entry.view_inert && r.entry.atexit_recorded);
    CHECK(r.entry.registration.ok && r.entry.registration.path == lane_m5::EntryRegisterPath::queued);
    check_entry(o.entry, "UpdateMaterialProperties", 24, 0xf9520, nullptr);
    CHECK(q.head == &o.entry && q.singleton == nullptr);
    CHECK(creg.atexit_log.size() == 1);
    check_atexit(creg, 0, 0x4b660, &o.entry);
    CHECK(creg.pending.size() == 0);   // not a console object
}

void section_with_neighbour() {
    ConCmdRegistry creg;
    AppInfoQueue q;
    Slot543Objects a;
    Slot492Objects b;
    poison(a);
    poison(b);
    CHECK(init_slot543_fae50(a, q, creg).ok);
    CHECK(init_slot492_278640(b, q, creg).ok);
    // push front: the later entry is the head and points at the earlier one
    CHECK(q.head == &b.entry && b.entry.next == &a.entry && a.entry.next == nullptr);
    CHECK(creg.atexit_log.size() == 2);
    check_atexit(creg, 0, 0x4b660, &a.entry);
    check_atexit(creg, 1, 0x4b660, &b.entry);
}

void section_failures() {
    {
        // A live singleton takes the unmodelled tree branch: registered is set (as in the original), then fail closed.
        ConCmdRegistry creg;
        AppInfoQueue q;
        int token = 0;
        q.singleton = &token;
        Slot543Objects o;
        poison(o);
        const Slot543Result r = init_slot543_fae50(o, q, creg);
        CHECK(!r.ok && !r.entry.registration.ok && r.entry.registration.path == lane_m5::EntryRegisterPath::singleton_present);
        CHECK(o.entry.registered == 1 && q.head == nullptr && creg.atexit_log.size() == 0 && !r.entry.atexit_recorded);
    }
    {
        // register-once: an entry whose byte is already non-zero is left alone by 0x4b560 (it is reset to 0 by the
        // constructor first, so this is the direct primitive).
        AppInfoQueue q;
        AppInfoQueueEntry e{};
        e.registered = 1;
        const lane_m5::EntryRegisterResult r = lane_m5::register_entry_4b560(e, q);
        CHECK(!r.ok && r.path == lane_m5::EntryRegisterPath::already_registered && q.head == nullptr);
    }
    {
        // a zero handler tag would skip the self-pointer rule: refused, nothing written
        ConCmdRegistry creg;
        AppInfoQueue q;
        AppInfoQueueEntry e;
        poison(e);
        const lane_m5::NamedDelegateSpec spec{l2::literal_view("x"), 0};
        CHECK(!lane_m5::init_named_delegate_entry(e, spec, q, creg.atexit_log).ok);
        check_untouched(e, {});
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_with_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_543");
    return 0;
}
