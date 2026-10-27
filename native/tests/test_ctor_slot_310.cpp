// CPU-01-STARTUP-SLOT-310-01: fixture for startup constructor index 310 (0xa479d0): the `titleid` string
// variable (0x257a0 / 0x258f0), the `useCircleAsEnter` bool variable, two "TitleInfo" toggles and the null
// tail pointer. Checks every stored field, untouched bytes, list/atexit order, the string-var edge branches
// (self pointer, refcounted views, refused functor tag) and fail-closed stages.
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>

#include "core/ctor_slot_310_a479d0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;

template <class T>
void poison(T& obj) { std::memset(static_cast<void*>(&obj), kPoison, sizeof obj); }

template <class T>
bool all_poison_range(const T& obj, std::size_t from, std::size_t to) {
    const auto* b = reinterpret_cast<const std::uint8_t*>(&obj);
    for (std::size_t i = from; i < to; ++i) if (b[i] != kPoison) return false;
    return true;
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
    CHECK(kSlot310Rva == 0xa479d0 && kSlot310BodySize == 723);
    CHECK(kSlot310TitleIdRva + sizeof(ConCmdStringVar) == kSlot310CircleRva);
    CHECK(kSlot310CircleRva + sizeof(ConCmdBoolVar) == kSlot310ToggleTitleRva);
    CHECK(kSlot310ToggleTitleRva + 0xa8 == kSlot310ToggleCircleRva && kSlot310ToggleCircleRva + 0xa8 == kSlot310TailRva);
    CHECK(kStringVarFinalVptr == 0x1594ef0 && kStringVarInnerVptr == 0x1594f68 && kStringVarFnVtbl == 0x1595068);
    CHECK(kStringVarSlot3Result == 1 && kStringVarSlot6Result == 4 && kStringVarDtorRva == 0x23c90);
    CHECK(kSlot310CircleFnVtbl == 0x15d19b0 && kSlot310TailDtorRva == 0xa46ba0);
}

void section_string_var() {
    ConCmdRegistry creg;
    ConCmdStringVar v;
    poison(v);
    const SharedStr name = l2::literal_view("titleid");
    const SharedStr init = l2::literal_view("");
    const StringVarResult r = init_string_var_257a0(v, name, init, creg);
    CHECK(r.ok && r.views_inert && r.registration.ok);
    CHECK(v.base.vptr == 0x1594ef0 && v.base.flags == 1 && v.base.registered == 1);
    CHECK(same_view(v.base.name, "titleid", 7));
    CHECK(v.base.description.size == 0 && v.base.description.ctl == nullptr && v.base.description.data[0] == 0);
    CHECK(v.value_ref == &v.value);
    CHECK(same_view(v.default_value, "", 0) && same_view(v.value, "", 0));
    CHECK(v.byte88 == 0 && v.bytea8 == 0 && v.byteb0 == 0 && v.byted0 == 0);
    CHECK(v.args[0] == 0 && v.args[1] == 0 && v.args[2] == 0 && v.args[3] == 0);
    CHECK(v.fn_vptr == 0x1595068 && v.fn_payload == 0 && v.fn_aux == 0);
    // never written
    CHECK(all_poison_range(v, 0x89, 0xa8) && all_poison_range(v, 0xa9, 0xb0) && all_poison_range(v, 0xb1, 0xd0));
    CHECK(all_poison_range(v, 0xd1, 0xd8) && all_poison_range(v, 0xf8, 0x108) && all_poison_range(v, 0x110, 0x118));
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &v.base.pending);

    // class slot results for the toggle gate
    const DebugMenuTarget t = string_var_menu_target(v);
    CHECK(t.known && t.object == &v && t.slot3 == 1 && t.slot6 == 4);
    ConCmdStringVar w;
    poison(w);
    w.base.vptr = kStringVarInnerVptr;   // the transient inner class is not the lifted final class
    CHECK(!string_var_menu_target(w).known);
}

void section_string_var_refcount() {
    // A reference-counted initial value: both copies take a reference; 0xf590 on the (empty) default stays inert.
    ConCmdRegistry creg;
    ConCmdStringVar v;
    poison(v);
    std::int64_t counter = 1;
    static const char text[] = "ABCDEFG";
    const SharedStr name = l2::literal_view("titleid");
    SharedStr init{reinterpret_cast<const std::uint8_t*>(text), 7, 0, &counter};
    const StringVarResult r = init_string_var_257a0(v, name, init, creg);
    // 0xf590 on the replaced default (the f180 empty) is inert; the registration path is unaffected
    CHECK(r.ok && counter == 3);
    CHECK(v.value.ctl == &counter && v.default_value.ctl == &counter && v.value.size == 7 && v.default_value.size == 7);
    CHECK(v.value.data == init.data && v.default_value.data == init.data);
}

void section_string_var_inner() {
    // inner constructor, direct: the self-pointer rule and the refused functor tag
    ConCmdStringVar v;
    poison(v);
    RangedVarArgs args{};
    args.fn_vptr = kStringVarFnVtbl;
    args.q[0] = 11; args.q[1] = 22; args.q[2] = 33; args.q[3] = 44;
    args.fn_payload = 0x1234; args.fn_aux = 0x5678;
    const SharedStr name = l2::literal_view("n");
    const SharedStr desc = l2::literal_view("");
    int cell = 0;
    CHECK(init_string_var_inner_258f0(v, name, &cell, 1, args, desc));
    CHECK(v.base.vptr == kStringVarInnerVptr && v.value_ref == &cell);
    CHECK(v.args[0] == reinterpret_cast<std::uint64_t>(&v.args[0]) && v.args[1] == 22 && v.args[2] == 33 && v.args[3] == 44);
    CHECK(v.fn_payload == 0x1234 && v.fn_aux == 0x5678 && v.fn_vptr == 0x1595068);
    args.q[3] = 0;
    CHECK(init_string_var_inner_258f0(v, name, &cell, 1, args, desc) && v.args[0] == 11);
    // an unlifted tag runs an unread clone slot: refuse, write nothing
    ConCmdStringVar u;
    poison(u);
    args.fn_vptr = 0x15d1da0;
    CHECK(!init_string_var_inner_258f0(u, name, &cell, 1, args, desc));
    CHECK(all_poison_range(u, 0, sizeof u));
}

void section_build() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot310Objects o;
    poison(o.circle);
    const Slot310Result r = init_slot310_a479d0(o, creg, menu);
    CHECK(r.ok && r.blocked == Slot310Stage::none);
    CHECK(r.titleid.ok && r.circle.ok && r.circle.views_inert && r.circle.atexit_recorded);
    CHECK(r.title_registration.root_initialised && r.title_registration.atexit_recorded);
    CHECK(!r.circle_registration.root_initialised);

    const ConCmdBoolVar& c = o.circle;
    CHECK(c.base.vptr == kBoolVarFinalVptr && c.base.flags == 1 && c.base.registered == 1);
    CHECK(c.value == 0 && c.default_value == 0 && c.value_ref == &c.value);
    CHECK(c.fn_vptr == 0x15d19b0 && c.fn_payload == 0 && c.fn_aux == 0);
    CHECK(same_view(c.base.name, "useCircleAsEnter", 0x10));
    CHECK(c.base.description.size == 0 && c.base.description.ctl == nullptr && c.base.description.data[0] == 0);

    // toggle A -> titleid: slots {1, 4} keep the default range {1.0, 10.0}
    const DebugMenuItem& ta = o.toggle_title;
    CHECK(ta.kind == 2 && ta.flags == 0x30 && ta.target == &o.titleid);
    CHECK(same_view(ta.label, "TitleInfo", 9) && same_view(ta.text, "Title ID", 8));
    CHECK(d(ta, 0x98) == 0x3f800000u && d(ta, 0x9c) == 0x41200000u);
    // toggle B -> circle: bool slots {1, 0} keep the default range
    const DebugMenuItem& tb = o.toggle_circle;
    CHECK(tb.kind == 2 && tb.flags == 0x30 && tb.target == &o.circle);
    CHECK(same_view(tb.label, "TitleInfo", 9) && same_view(tb.text, "Circle as Enter", 0xf));
    CHECK(d(tb, 0x98) == 0x3f800000u && d(tb, 0x9c) == 0x41200000u);

    CHECK(o.tail == 0);

    // pending list: titleid first, then circle (registration order)
    CHECK(creg.pending.size() == 2 && creg.pending.first() == &o.titleid.base.pending);

    // root list: toggle A, toggle B (no group)
    CHECK(menu.root_guard_done && menu.root.count == 2);
    CHECK(menu.root.link.next == &ta.link && ta.link.next == &tb.link && tb.link.next == &menu.root.link &&
          menu.root.link.prev == &tb.link);

    // atexit order
    const std::pair<std::uint64_t, const void*> want[] = {
        {0x23c90, &o.titleid}, {0x20b0, &o.circle}, {0x27280, &menu.root}, {0x272d0, &o.toggle_title},
        {0x272d0, &o.toggle_circle}, {0xa46ba0, &o.tail}};
    CHECK(creg.atexit_log.size() == 6);
    for (std::size_t i = 0; i < 6; ++i) {
        CHECK(creg.atexit_log.at(i).dtor_rva == want[i].first && creg.atexit_log.at(i).object == want[i].second);
    }
}

void section_failures() {
    // Console manager without an insert contract: the string variable's registration fails first.
    {
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot310Objects o;
        poison(o.circle);
        poison(o.toggle_title);
        const Slot310Result r = init_slot310_a479d0(o, creg, menu);
        CHECK(!r.ok && r.blocked == Slot310Stage::titleid_build && !r.titleid.ok);
        CHECK(all_poison_range(o.circle, 0, sizeof o.circle) && all_poison_range(o.toggle_title, 0, sizeof o.toggle_title));
        CHECK(menu.root.count == 0 && creg.atexit_log.size() == 0);
    }
    // Debug-menu parent without contract: toggle A fails, B is never built.
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot310Objects o;
        poison(o.toggle_circle);
        o.tail = 0xdead;
        const Slot310Result r = init_slot310_a479d0(o, creg, menu);
        CHECK(!r.ok && r.blocked == Slot310Stage::title_registration && r.titleid.ok && r.circle.ok);
        CHECK(all_poison_range(o.toggle_circle, 0, sizeof o.toggle_circle) && o.tail == 0xdead);
    }
    // With a parent contract the items go to the parent and the fragment completes.
    {
        static int inserted = 0;
        struct P { static bool insert(void*, DebugMenuItem&) noexcept { ++inserted; return true; } };
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        menu.parent.insert = &P::insert;
        Slot310Objects o;
        const Slot310Result r = init_slot310_a479d0(o, creg, menu);
        CHECK(r.ok && inserted == 2 && menu.root.count == 0 && creg.atexit_log.size() == 5);
    }
    CHECK(std::strcmp(slot310_stage_name(Slot310Stage::tail_atexit), "tail_atexit") == 0);
    CHECK(std::strcmp(slot310_stage_name(Slot310Stage::titleid_build), "titleid_build") == 0);
}

}  // namespace

int main() {
    section_constants();
    section_string_var();
    section_string_var_refcount();
    section_string_var_inner();
    section_build();
    section_failures();
    std::puts("OK test_ctor_slot_310");
    return 0;
}
