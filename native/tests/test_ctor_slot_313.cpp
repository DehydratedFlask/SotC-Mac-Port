// CPU-01-STARTUP-SLOT-313-01: fixture for startup constructor index 313 (0xa447a0): the
// `update_entitlement_status` console command, its "Entitlement System" group and "Update Entitlement
// State" item. Checks every field the original stores, the untouched bytes, the root-list order, the
// atexit order and the fail-closed stages. The production row is wired by the integrator.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <utility>

#include "core/ctor_slot_313_a447a0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;

template <class T>
void poison(T& obj) { std::memset(static_cast<void*>(&obj), kPoison, sizeof obj); }

template <class T>
void check_untouched(const T& obj, std::initializer_list<std::pair<std::size_t, std::size_t>> written) {
    const auto* bytes = reinterpret_cast<const std::uint8_t*>(&obj);
    for (std::size_t off = 0; off < sizeof obj; ++off) {
        bool is_written = false;
        for (const auto& [begin, size] : written) {
            if (off >= begin && off < begin + size) { is_written = true; break; }
        }
        if (!is_written && bytes[off] != kPoison) {
            std::fprintf(stderr, "untouched byte +0x%zx changed to 0x%02x\n", off, bytes[off]);
            std::exit(1);
        }
    }
}

template <class T>
std::uint32_t d(const T& obj, std::size_t off) {
    std::uint32_t v;
    std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(&obj) + off, 4);
    return v;
}

bool same_view(const SharedStr& v, const char* text, std::size_t len) {
    return v.data != nullptr && v.size == static_cast<std::int64_t>(len) && v.aux == 0 &&
           std::memcmp(v.data, text, len) == 0 && v.ctl == nullptr;
}

void section_constants() {
    CHECK(kSlot313Rva == 0xa447a0 && kSlot313BodySize == 661);
    CHECK(kSlot313CellRva == 0x32f6d18 && kSlot313CaptionRva == 0x32f6d28 && kSlot313CommandRva == 0x32f6d48);
    CHECK(kSlot313GroupRva == 0x32f6de8 && kSlot313ToggleRva == 0x32f6e90 && kSlot313TailRva == 0x32f6f38);
    // The original .bss placement is contiguous with the native object sizes.
    CHECK(kSlot313CommandRva + sizeof(ConCmdCommand) == kSlot313GroupRva);
    CHECK(kSlot313GroupRva + 0xa8 == kSlot313ToggleRva && kSlot313ToggleRva + 0xa8 == kSlot313TailRva);
    CHECK(kSlot313CellDtorRva == 0xa42900 && kSlot313CaptionDtorRva == 0xf7b0 && kSlot313TailDtorRva == 0xa42950);
    CHECK(kSlot313HandlerRva == 0xa42930 && kSlot313ItemFlags == 0x30);
    CHECK(std::strcmp(kSlot313CommandName, "update_entitlement_status") == 0);
}

void section_build() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot313Objects o;
    poison(o);
    const Slot313Result r = init_slot313_a447a0(o, creg, menu);
    CHECK(r.ok && r.blocked == Slot313Stage::none);
    CHECK(r.command.ok && r.command.views_inert && r.command.atexit_recorded && !r.command.handler_invoked);
    CHECK(r.group_registration.ok && r.group_registration.path == DebugMenuRegisterPath::root_list &&
          r.group_registration.root_initialised && r.group_registration.atexit_recorded);
    CHECK(r.toggle_registration.ok && r.toggle_registration.path == DebugMenuRegisterPath::root_list &&
          !r.toggle_registration.root_initialised && !r.toggle_registration.atexit_recorded);

    // cell and tail are plain zeros
    CHECK(o.cell == 0);
    CHECK(o.tail[0] == 0 && o.tail[1] == 0 && o.tail[2] == 0);

    // global caption view {data, 0x12, 0, 0}
    CHECK(same_view(o.caption, "Entitlement System", 0x12));

    // command: name/description views, vptr, delegate, empty vector, pending list
    const ConCmdCommand& c = o.command;
    CHECK(c.base.vptr == kFConCmdVptr && c.base.flags == kFConCmdFlags && c.base.registered == 1);
    CHECK(same_view(c.base.name, "update_entitlement_status", 25));
    CHECK(c.base.description.data != nullptr && c.base.description.size == 30 && c.base.description.ctl == nullptr &&
          std::memcmp(c.base.description.data, "Updates the entitlement status", 30) == 0);
    CHECK(c.delegate.object == reinterpret_cast<std::uint64_t>(&c.delegate) && c.delegate.fn == 0x1990 &&
          c.delegate.adj == 0 && c.delegate.handler == 0xa42930);
    CHECK(c.args_begin == nullptr && c.args_end == nullptr && c.args_cap == nullptr);
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &c.base.pending);

    // group: kind 1, parent view A, caption in B and C, no target, default range, flags 0x30
    const DebugMenuItem& g = o.group;
    CHECK(g.kind == 1 && g.flags == 0x30 && g.target == nullptr && g.registered == 0);
    CHECK(same_view(g.label, "Platform Services", 0x11));
    CHECK(same_view(g.text, "Entitlement System", 0x12) && same_view(g.empty, "Entitlement System", 0x12));
    CHECK(d(g, 0x98) == 0x3f800000u && d(g, 0x9c) == 0x41200000u);

    // toggle: kind 2, label = global caption, text, target = the command, default range (slot3 == 0)
    const DebugMenuItem& t = o.toggle;
    CHECK(t.kind == 2 && t.flags == 0x30 && t.target == &c && t.registered == 0);
    CHECK(same_view(t.label, "Entitlement System", 0x12));
    CHECK(same_view(t.text, "Update Entitlement State", 0x18));
    CHECK(t.empty.data != nullptr && t.empty.size == 0 && t.empty.ctl == nullptr);
    CHECK(d(t, 0x98) == 0x3f800000u && d(t, 0x9c) == 0x41200000u);

    // root list order: group then toggle
    CHECK(menu.root_guard_done && menu.root.count == 2);
    CHECK(menu.root.link.next == &g.link && g.link.next == &t.link && t.link.next == &menu.root.link);
    CHECK(menu.root.link.prev == &t.link && t.link.prev == &g.link && g.link.prev == &menu.root.link);
    CHECK(g.link.owner == &menu.root.link && t.link.owner == &menu.root.link);

    // atexit order: cell, caption, command, root (first menu registration), group, toggle, tail
    CHECK(creg.atexit_log.size() == 7);
    const std::pair<std::uint64_t, const void*> want[] = {
        {0xa42900, &o.cell}, {0xf7b0, &o.caption}, {0x4b0, &o.command}, {0x27280, &menu.root},
        {0x272d0, &o.group}, {0x272d0, &o.toggle}, {0xa42950, &o.tail}};
    for (std::size_t i = 0; i < 7; ++i) {
        CHECK(creg.atexit_log.at(i).dtor_rva == want[i].first && creg.atexit_log.at(i).object == want[i].second);
    }
}

void section_after_other_root_items() {
    // A second run into a registry whose root already exists: no second root destructor, order kept.
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot313Objects a, b;
    CHECK(init_slot313_a447a0(a, creg, menu).ok);
    CHECK(init_slot313_a447a0(b, creg, menu).ok);
    CHECK(menu.root.count == 4 && creg.atexit_log.size() == 7 + 6);
    CHECK(a.toggle.link.next == &b.group.link && b.toggle.link.next == &menu.root.link);
}

void section_failures() {
    // Console manager without an insert contract: the command registration fails, nothing after it runs.
    {
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot313Objects o;
        poison(o.group);
        const Slot313Result r = init_slot313_a447a0(o, creg, menu);
        CHECK(!r.ok && r.blocked == Slot313Stage::command_build && !r.command.ok);
        CHECK(menu.root.count == 0 && !menu.root_guard_done);

        const auto* gb = reinterpret_cast<const std::uint8_t*>(&o.group);
        for (std::size_t i = 0; i < sizeof o.group; ++i) CHECK(gb[i] == kPoison);
    }
    // Debug-menu parent without a contract: the group registration fails, the toggle is never built.
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot313Objects o;
        poison(o.toggle);
        const Slot313Result r = init_slot313_a447a0(o, creg, menu);
        CHECK(!r.ok && r.blocked == Slot313Stage::group_registration && !r.group_registration.ok);
        CHECK(r.command.ok);   // everything before the group did run
        const auto* tb = reinterpret_cast<const std::uint8_t*>(&o.toggle);
        for (std::size_t i = 0; i < sizeof o.toggle; ++i) CHECK(tb[i] == kPoison);
    }
    CHECK(std::strcmp(slot313_stage_name(Slot313Stage::tail_atexit), "tail_atexit") == 0);
}

}  // namespace

int main() {
    section_constants();
    section_build();
    section_after_other_root_items();
    section_failures();
    std::puts("OK test_ctor_slot_313");
    return 0;
}
