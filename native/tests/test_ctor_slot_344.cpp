// CPU-01-STARTUP-SLOT-344: fixture for startup constructor index 344 (0x7d3810): two flag/atomic cells, the
// bool variable `ShowCinematicTime`, a menu toggle built by the new 0x26db0 under the parent path `Cutscene`,
// and the command `cutscene_nextCut`. Lane L4.
#include "l4_test_util.hpp"

#include "core/ctor_slot_344_7d3810.hpp"
#include "core/ctor_slot_345_7cec60.hpp"

namespace {
using namespace l4;

void section_constants() {
    CHECK(kSlot344Rva == 0x7d3810 && kSlot344Size == 706 && kSlot344FnTag == 0x15ce3b0 && kSlot344HandlerTag == 0x7d2c20);
    CHECK(std::strlen(kSlot344VarName) == 0x11 && std::strlen(kSlot344Parent) == 8);
    CHECK(std::strlen(kSlot344CommandName) == 16 && std::strlen(kSlot344CommandDescription) == 0x2c);
    CHECK(kSlot344CommandDescriptionLength == 0x2c && kFlagScaleQword == 0x3f80000000000001ull);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot344Objects o;
    poison(o);
    const Slot344Result r = init_slot344_7d3810(o, creg, menu);
    CHECK(r.ok && r.var.ok && r.item.ok && r.command.ok);

    for (const FlagScaleCell* c : {&o.cell_a, &o.cell_b}) {
        CHECK(c->flag_scale == 0x3f80000000000001ull && c->atomic_word == 0);
        check_untouched(*c, {{0x00, 8}, {0x10, 4}});
    }

    check_bool_common(o.var, 0x15ce3b0, 0);
    CHECK(same_view(o.var.base.name, "ShowCinematicTime", 0x11));

    // The 0x26db0 item: parent path in +0x28, caption = the variable's name view (copied) in +0x50.
    const DebugMenuItem& it = o.item;
    CHECK(it.target == &o.var && it.flags == 0x30 && it.kind == 2 && it.registered == 0);
    CHECK(same_view(it.label, "Cutscene", 8));
    CHECK(it.text.data == o.var.base.name.data && it.text.size == 0x11 && it.text.ctl == nullptr);
    CHECK(dword(it, 0x98) == 0x3f800000u && dword(it, 0x9c) == 0x41200000u);
    CHECK(empty_view(it.empty));
    check_item_untouched(it);
    CHECK(menu.root.count == 1 && menu.root.link.next == &it.link);

    check_command(o.command, "cutscene_nextCut", "Skip to the next cut in the current cutscene", 0x2c, 0x7d2c20);

    CHECK(creg.pending.size() == 2 && creg.pending.first() == &o.var.base.pending &&
          o.var.base.pending.next == &o.command.base.pending);
    CHECK(creg.atexit_log.size() == 4);
    check_atexit(creg, 0, 0x20b0, &o.var);
    check_atexit(creg, 1, 0x27280, &menu.root);
    check_atexit(creg, 2, 0x272d0, &o.item);
    check_atexit(creg, 3, 0x4b0, &o.command);
}

void section_caption_counters() {
    // 0x26db0 acquires BOTH views when they carry a control counter ("lock inc qword [ctl]").
    ConCmdRegistry creg;
    std::int64_t name_ctl = 1;
    std::int64_t parent_ctl = 1;
    ConCmdBoolVar var{};
    const SharedStr name{reinterpret_cast<const std::uint8_t*>("n"), 1, 0, nullptr};
    const SharedStr desc{reinterpret_cast<const std::uint8_t*>(""), 0, 0, nullptr};
    CHECK(init_bool_var_static(var, name, desc, 0x15ce3b0, creg, 0, 0).ok);
    var.base.name.ctl = &name_ctl;   // give the stored name view a counter (the stack view that built it was inert)
    const std::int64_t name_before = name_ctl;
    DebugMenuItem item{};
    const SharedStr parent{reinterpret_cast<const std::uint8_t*>("P"), 1, 0, &parent_ctl};
    CHECK(init_menu_toggle_from_var_26db0(item, parent, var, 0x30));
    CHECK(parent_ctl == 2 && name_ctl == name_before + 1);
    CHECK(item.label.ctl == &parent_ctl && item.text.ctl == &name_ctl);
}

void section_after_neighbour() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot345Objects n;
    Slot344Objects o;
    CHECK(init_slot344_7d3810(o, creg, menu).ok);
    CHECK(init_slot345_7cec60(n, creg, menu).ok);
    CHECK(menu.root.count == 2 && o.item.link.next == &n.item.link && n.item.link.next == &menu.root.link);
    CHECK(creg.pending.size() == 3);
    CHECK(creg.atexit_log.size() == 4 + 2);   // the second slot adds the variable and its item, no second root dtor
}

void section_failures() {
    {
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot344Objects o;
        const Slot344Result r = init_slot344_7d3810(o, creg, menu);
        CHECK(!r.ok && !r.var.ok && !r.item.ctor_ok && !r.command.ok);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;   // parent path without an insert contract
        Slot344Objects o;
        const Slot344Result r = init_slot344_7d3810(o, creg, menu);
        CHECK(!r.ok && r.var.ok && !r.item.ok && r.item.registration.path == DebugMenuRegisterPath::parent);
        CHECK(!r.command.ok);   // the command stage is not reached
    }
    {
        // A target of an unlifted class is refused by 0x26db0 and nothing is written.
        ConCmdBoolVar fake{};
        fake.base.vptr = 0x1234;
        DebugMenuItem item;
        poison(item);
        const SharedStr p{reinterpret_cast<const std::uint8_t*>("P"), 1, 0, nullptr};
        CHECK(!init_menu_toggle_from_var_26db0(item, p, fake, 0x30));
        const auto* b = reinterpret_cast<const std::uint8_t*>(&item);
        for (std::size_t i = 0; i < sizeof item; ++i) CHECK(b[i] == kPoison);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_caption_counters();
    section_after_neighbour();
    section_failures();
    std::puts("OK test_ctor_slot_344");
    return 0;
}
