// CPU-01-STARTUP-SLOT-355: fixture for startup constructor index 355 (0x7946d0): bool variables
// `shadow_yamoriBossDead` and `shadow_yamoriBossInfiniteStun` (tag 0x15ce050, value 0 / default 0) and the
// 0x26ea0 toggle that targets the SECOND one. Lane L4.
#include "l4_test_util.hpp"

#include "core/ctor_slot_355_7946d0.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot355Rva == 0x7946d0 && kSlot355Size == 646 && kSlot355FnTag == 0x15ce050);
    CHECK(sizeof kSlot355NameA - 1 == 0x15 && sizeof kSlot355NameB - 1 == 0x1d);
    CHECK(sizeof kSlot355Label - 1 == 9 && sizeof kSlot355Text - 1 == 0x19);
    CHECK(std::strcmp(kSlot355NameA, "shadow_yamoriBossDead") == 0);
    CHECK(std::strcmp(kSlot355NameB, "shadow_yamoriBossInfiniteStun") == 0);
    CHECK(std::strcmp(kSlot355Label, "Shadow AI") == 0 && std::strcmp(kSlot355Text, "Yamori Boss Infinite Stun") == 0);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot355Objects o;
    poison(o);
    const Slot355Result r = init_slot355_7946d0(o, creg, menu);
    CHECK(r.ok && r.var_a.ok && r.var_b.ok && r.item.ok);
    check_bool_common(o.var_a, 0x15ce050, 0);
    check_bool_common(o.var_b, 0x15ce050, 0);
    CHECK(same_view(o.var_a.base.name, "shadow_yamoriBossDead", 0x15));
    CHECK(same_view(o.var_b.base.name, "shadow_yamoriBossInfiniteStun", 0x1d));
    CHECK(creg.pending.size() == 2);
    CHECK(creg.pending.first() == &o.var_a.base.pending && o.var_a.base.pending.next == &o.var_b.base.pending &&
          o.var_b.base.pending.next == &creg.pending.head);

    const DebugMenuItem& it = o.item;
    CHECK(it.target == &o.var_b && it.target != &o.var_a);     // the toggle targets the SECOND variable
    CHECK(it.flags == 0x30 && it.kind == 2 && dword(it, 0x98) == 0x3f800000u && dword(it, 0x9c) == 0x41200000u);
    CHECK(same_view(it.label, "Shadow AI", 9) && same_view(it.text, "Yamori Boss Infinite Stun", 0x19));
    check_item_untouched(it);
    CHECK(menu.root.count == 1 && menu.root.link.next == &it.link);

    // 0x20b0(A), 0x20b0(B), 0x27280(root), 0x272d0(item).
    CHECK(creg.atexit_log.size() == 4);
    check_atexit(creg, 0, 0x20b0, &o.var_a);
    check_atexit(creg, 1, 0x20b0, &o.var_b);
    check_atexit(creg, 2, 0x27280, &menu.root);
    check_atexit(creg, 3, 0x272d0, &o.item);
}

void section_failures() {
    ConCmdRegistry creg;
    int token = 0;
    creg.manager.manager = &token;
    DebugMenuRegistry menu;
    Slot355Objects o;
    const Slot355Result r = init_slot355_7946d0(o, creg, menu);
    CHECK(!r.ok && !r.var_a.ok && !r.var_b.ok && !r.item.ctor_ok);   // stops after the first failing stage
    CHECK(creg.pending.size() == 0 && menu.root.count == 0);
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_355");
    return 0;
}
