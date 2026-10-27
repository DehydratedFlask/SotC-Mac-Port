// CPU-01-STARTUP-SLOT-317-01: fixture for startup constructor index 317 (0xa2c390): the Controller block
// (vibration_large_scalar / vibration_small_scalar unbounded floats, the "Controller" group and two toggles
// whose range is narrowed to {0.1, 0.5}). Checks every stored field, untouched bytes, list/atexit order and
// fail-closed stages.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>

#include "core/ctor_slot_317_a2c390.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;

template <class T>
void poison(T& obj) { std::memset(static_cast<void*>(&obj), kPoison, sizeof obj); }

template <class T>
bool all_poison(const T& obj) {
    const auto* b = reinterpret_cast<const std::uint8_t*>(&obj);
    for (std::size_t i = 0; i < sizeof obj; ++i) if (b[i] != kPoison) return false;
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

void check_float_var(const ConCmdRangedVar& v, const char* name) {
    CHECK(v.base.vptr == 0x15942b0 && v.base.flags == 1 && v.base.registered == 1);
    CHECK(same_view(v.base.name, name, 0x16));
    CHECK(v.base.description.size == 0 && v.base.description.ctl == nullptr && v.base.description.data[0] == 0);
    CHECK(v.value == 0x3f000000u && v.default_value == 0x3f000000u && v.value_ref == &v.value);
    // unbounded: neither bound is engaged
    CHECK(v.min_engaged == 0 && v.max_engaged == 0);
    CHECK(v.args[0] == 0 && v.args[1] == 0 && v.args[2] == 0 && v.args[3] == 0);
    CHECK(v.fn_vptr == 0x1594318 && v.fn_payload == 0 && v.fn_aux == 0);
}

void section_constants() {
    CHECK(kSlot317Rva == 0xa2c390 && kSlot317BodySize == 969);
    CHECK(kSlot317LargeRva == 0x32f4c38 && kSlot317SmallRva == 0x32f4d10 && kSlot317GroupRva == 0x32f4de8);
    CHECK(kSlot317Toggle1Rva == 0x32f4e90 && kSlot317Toggle2Rva == 0x32f4f38);
    // The native object sizes reproduce the original .bss strides.
    CHECK(kSlot317LargeRva + sizeof(ConCmdRangedVar) == kSlot317SmallRva);
    CHECK(kSlot317SmallRva + sizeof(ConCmdRangedVar) == kSlot317GroupRva);
    CHECK(kSlot317GroupRva + 0xa8 == kSlot317Toggle1Rva && kSlot317Toggle1Rva + 0xa8 == kSlot317Toggle2Rva);
    CHECK(kSlot317ArgFnVtbl == 0x15d10f0 && kSlot317ArgFnEquivalent == 0x15d57e0 && kSlot317DefaultBits == 0x3f000000);
    CHECK(is_known_float_arg_functor(kSlot317ArgFnEquivalent));   // the helper accepts the substituted tag
    CHECK(kFloatClassSlot3Result == 1 && kFloatClassSlot6Result == 3);
}

void section_menu_target() {
    ConCmdRangedVar v;
    poison(v);
    v.base.vptr = kRangedFloatFinalVptr;
    const DebugMenuTarget t = float_var_menu_target(v);
    CHECK(t.known && t.object == &v && t.slot3 == 1 && t.slot6 == 3);
    v.base.vptr = kRangedFloatMidVptr;   // not the final class: unlifted, so toggles refuse
    CHECK(!float_var_menu_target(v).known);
}

void section_build() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot317Objects o;
    poison(o.large);
    const Slot317Result r = init_slot317_a2c390(o, creg, menu);
    CHECK(r.ok && r.blocked == Slot317Stage::none);
    CHECK(r.large.ok && r.large.ctor_ok && r.large.views_inert && r.large.functor_destroy_ok && r.large.atexit_recorded);
    CHECK(r.small.ok && r.small.ctor_ok && r.small.views_inert && r.small.functor_destroy_ok && r.small.atexit_recorded);
    CHECK(r.group_registration.root_initialised && r.group_registration.atexit_recorded);
    CHECK(!r.toggle1_registration.root_initialised && !r.toggle2_registration.root_initialised);

    check_float_var(o.large, "vibration_large_scalar");
    check_float_var(o.small, "vibration_small_scalar");

    const DebugMenuItem& g = o.group;
    CHECK(g.kind == 1 && g.flags == 0x30 && g.target == nullptr);
    CHECK(same_view(g.label, "Platform Services", 0x11));
    CHECK(same_view(g.text, "Controller", 0xa) && same_view(g.empty, "Controller", 0xa));

    // toggles: float class slots {1, 3} NARROW the range to {0.1f, 0.5f}
    const DebugMenuItem& t1 = o.toggle1;
    CHECK(t1.kind == 2 && t1.flags == 0x30 && t1.target == &o.large);
    CHECK(same_view(t1.label, "Controller", 0xa) && same_view(t1.text, "Large Vibration Scalar", 0x16));
    CHECK(d(t1, 0x98) == 0x3dcccccdu && d(t1, 0x9c) == 0x3f000000u);
    const DebugMenuItem& t2 = o.toggle2;
    CHECK(t2.kind == 2 && t2.flags == 0x30 && t2.target == &o.small);
    CHECK(same_view(t2.label, "Controller", 0xa) && same_view(t2.text, "Small Vibration Scalar", 0x16));
    CHECK(d(t2, 0x98) == 0x3dcccccdu && d(t2, 0x9c) == 0x3f000000u);

    // pending list: large first, then small (registration order)
    CHECK(creg.pending.size() == 2 && creg.pending.first() == &o.large.base.pending);

    // root list: group, toggle1, toggle2
    CHECK(menu.root_guard_done && menu.root.count == 3);
    CHECK(menu.root.link.next == &g.link && g.link.next == &t1.link && t1.link.next == &t2.link &&
          t2.link.next == &menu.root.link && menu.root.link.prev == &t2.link);

    // atexit order
    const std::pair<std::uint64_t, const void*> want[] = {
        {0x20f0, &o.large}, {0x20f0, &o.small}, {0x27280, &menu.root}, {0x272d0, &o.group},
        {0x272d0, &o.toggle1}, {0x272d0, &o.toggle2}};
    CHECK(creg.atexit_log.size() == 6);
    for (std::size_t i = 0; i < 6; ++i) {
        CHECK(creg.atexit_log.at(i).dtor_rva == want[i].first && creg.atexit_log.at(i).object == want[i].second);
    }
}

void section_failures() {
    // Console manager without an insert contract: the first variable's registration fails.
    {
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot317Objects o;
        poison(o.small);
        poison(o.group);
        const Slot317Result r = init_slot317_a2c390(o, creg, menu);
        CHECK(!r.ok && r.blocked == Slot317Stage::large_build && !r.large.ok);
        CHECK(all_poison(o.small) && all_poison(o.group) && menu.root.count == 0);
    }
    // Debug-menu parent without contract: the group fails, the toggles are never built.
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot317Objects o;
        poison(o.toggle1);
        poison(o.toggle2);
        const Slot317Result r = init_slot317_a2c390(o, creg, menu);
        CHECK(!r.ok && r.blocked == Slot317Stage::group_registration && r.large.ok && r.small.ok);
        CHECK(all_poison(o.toggle1) && all_poison(o.toggle2));
    }
    // With a parent contract the items go to the parent, the fragment completes.
    {
        static int inserted = 0;
        struct P { static bool insert(void*, DebugMenuItem&) noexcept { ++inserted; return true; } };
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        menu.parent.insert = &P::insert;
        Slot317Objects o;
        const Slot317Result r = init_slot317_a2c390(o, creg, menu);
        CHECK(r.ok && inserted == 3 && menu.root.count == 0 && creg.atexit_log.size() == 5);
    }
    CHECK(std::strcmp(slot317_stage_name(Slot317Stage::toggle2_atexit), "toggle2_atexit") == 0);
}

}  // namespace

int main() {
    section_constants();
    section_menu_target();
    section_build();
    section_failures();
    std::puts("OK test_ctor_slot_317");
    return 0;
}
