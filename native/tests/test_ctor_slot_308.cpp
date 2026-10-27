// CPU-01-STARTUP-SLOT-308-01: fixture for startup constructor index 308 (0xa58f10): the Save System
// block (sSaveManagerShowInfoLevel int 0..3 built by the native 0xc580, sSaveManagerSaveEnabled bool,
// the "Save System" group and two toggles). Checks every stored field, untouched bytes, list/atexit
// order and fail-closed stages. Also exercises the 0xc580 model on its own.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>

#include "core/ctor_slot_308_a58f10.hpp"

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

void section_constants() {
    CHECK(kSlot308Rva == 0xa58f10 && kSlot308BodySize == 1119);
    CHECK(kSlot308CellRva == 0x32f84f8 && kSlot308LevelRva == 0x32f8508 && kSlot308EnabledRva == 0x32f85e0);
    CHECK(kSlot308GroupRva == 0x32f86a8 && kSlot308Toggle1Rva == 0x32f8750 && kSlot308Toggle2Rva == 0x32f87f8);
    CHECK(kSlot308TailRva == 0x32f88a8);
    // The native object sizes reproduce the original .bss strides.
    CHECK(kSlot308LevelRva + sizeof(ConCmdRangedVar) == kSlot308EnabledRva);
    CHECK(kSlot308EnabledRva + sizeof(ConCmdBoolVar) == kSlot308GroupRva);
    CHECK(kSlot308GroupRva + 0xa8 == kSlot308Toggle1Rva && kSlot308Toggle1Rva + 0xa8 == kSlot308Toggle2Rva);
    CHECK(kSlot308ArgFnVtbl == 0x15d1d80 && kSlot308BoolFnVtbl == 0x15d1da0);
    CHECK(kSlot308LevelMin == 0 && kSlot308LevelMax == 3 && kSlot308EnabledValue == 1 && kSlot308EnabledDefault == 1);
    CHECK(kRangedIntCtorRva == 0xc580 && kRangedIntCtorSize == 0x15f && kRangedIntFinalVptr == 0x1594c08);
    CHECK(kRangedIntMidVptr == 0x15941a0 && kRangedIntFnVtbl == 0x15944a0 && kRangedIntDtorRva == 0x18a60);
}

void section_c580() {
    // 0xc580 on its own: layout, bounds, args copy, functor install, refusal of an unlifted tag.
    const SharedStr name = l2::literal_view(kSlot308LevelName);
    const SharedStr desc = l2::literal_view(kSlot308LevelDesc);
    ConCmdRangedVar v;
    poison(v);
    RangedVarArgs args{};
    args.q[0] = 0x1111; args.q[1] = 0x2222; args.q[2] = 0x3333; args.q[3] = 0;
    args.fn_payload = 0x77; args.fn_vptr = kRangedIntArgFnVtbl; args.fn_aux = 0x99;
    CHECK(init_ranged_int_var_c580(v, name, &v.value, -5, 9, 0x21, args, desc));
    CHECK(v.base.vptr == kRangedIntMidVptr && v.value_ref == &v.value && v.base.flags == 0x21);
    CHECK(d(v, 0x6c) == 0xfffffffbu && v.min_engaged == 1 && d(v, 0x74) == 9u && v.max_engaged == 1);
    CHECK(v.args[0] == 0x1111 && v.args[1] == 0x2222 && v.args[2] == 0x3333 && v.args[3] == 0);
    CHECK(v.fn_vptr == kRangedIntFnVtbl && v.fn_payload == 0x77 && v.fn_aux == 0x99);
    CHECK(v.default_value == 0xa5a5a5a5u && v.value == 0xa5a5a5a5u);   // the caller writes these, not 0xc580
    // q[3] != 0 makes q[0] self-referential
    ConCmdRangedVar w;
    poison(w);
    args.q[3] = 1;
    CHECK(init_ranged_int_var_c580(w, name, &w.value, 0, 3, 1, args, desc));
    CHECK(w.args[0] == reinterpret_cast<std::uint64_t>(&w.args[0]) && w.args[3] == 1);
    // class tag itself is also lifted; any other tag is refused and writes nothing
    ConCmdRangedVar x;
    poison(x);
    args.fn_vptr = kRangedIntFnVtbl;
    CHECK(init_ranged_int_var_c580(x, name, &x.value, 0, 3, 1, args, desc));
    ConCmdRangedVar y;
    poison(y);
    args.fn_vptr = 0x15d57e0;   // the float tag: not a lifted int site
    CHECK(!init_ranged_int_var_c580(y, name, &y.value, 0, 3, 1, args, desc));
    CHECK(all_poison(y));
    // 0x26ea0 virtual-slot results
    v.base.vptr = kRangedIntFinalVptr;
    const DebugMenuTarget t = ranged_int_c580_menu_target(v);
    CHECK(t.known && t.object == &v && t.slot3 == 1 && t.slot6 == 2);
    v.base.vptr = kRangedIntMidVptr;
    CHECK(!ranged_int_c580_menu_target(v).known);
}

void section_build() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot308Objects o;
    poison(o.level);
    o.tail.untouched = 0xfeedface;
    const Slot308Result r = init_slot308_a58f10(o, creg, menu);
    CHECK(r.ok && r.blocked == Slot308Stage::none);
    CHECK(r.level_registration.ok && r.enabled.ok && r.enabled.views_inert && r.enabled.atexit_recorded);
    CHECK(r.group_registration.root_initialised && r.group_registration.atexit_recorded);
    CHECK(!r.toggle1_registration.root_initialised && !r.toggle2_registration.root_initialised);

    CHECK(o.cell == 0);
    CHECK(o.tail.untouched == 0xfeedface);
    for (auto w : o.tail.zero) CHECK(w == 0);

    // level: int 0..3, value 0 / default 0, final class 0x1594c08, functor class 0x15944a0, flags 1
    const ConCmdRangedVar& L = o.level;
    CHECK(L.base.vptr == 0x1594c08 && L.base.flags == 1 && L.base.registered == 1);
    CHECK(same_view(L.base.name, "sSaveManagerShowInfoLevel", 0x19));
    CHECK(L.base.description.size == 0x28 && L.base.description.ctl == nullptr &&
          std::memcmp(L.base.description.data, "Save System Debug Info Level from 0 to 3", 0x28) == 0);
    CHECK(L.value == 0 && L.default_value == 0 && L.value_ref == &o.level.value);
    CHECK(d(L, 0x6c) == 0 && L.min_engaged == 1 && d(L, 0x74) == 3 && L.max_engaged == 1);
    CHECK(L.args[0] == 0 && L.args[1] == 0 && L.args[2] == 0 && L.args[3] == 0);
    CHECK(L.fn_vptr == 0x15944a0 && L.fn_payload == 0 && L.fn_aux == 0);

    // enabled: bool value 1 / default 1, functor tag 0x15d1da0, flags 1, empty description
    const ConCmdBoolVar& v = o.enabled;
    CHECK(v.base.vptr == kBoolVarFinalVptr && v.base.flags == 1 && v.base.registered == 1);
    CHECK(v.value == 1 && v.default_value == 1 && v.fn_vptr == 0x15d1da0 && v.fn_payload == 0 && v.fn_aux == 0);
    CHECK(v.value_ref == &v.value);
    CHECK(same_view(v.base.name, "sSaveManagerSaveEnabled", 0x17));
    CHECK(v.base.description.size == 0 && v.base.description.ctl == nullptr && v.base.description.data[0] == 0);

    // group: parent as label, caption as text and empty view, kind 1
    const DebugMenuItem& g = o.group;
    CHECK(g.kind == 1 && g.flags == 0x30 && g.target == nullptr);
    CHECK(same_view(g.label, "Platform Services", 0x11));
    CHECK(same_view(g.text, "Save System", 0xb) && same_view(g.empty, "Save System", 0xb));

    // toggle 1 -> the int variable; slots {1, 2} keep the default range {1.0, 10.0}
    const DebugMenuItem& t1 = o.toggle1;
    CHECK(t1.kind == 2 && t1.flags == 0x30 && t1.target == &o.level);
    CHECK(same_view(t1.label, "Save System", 0xb) && same_view(t1.text, "Show Info Level", 0xf));
    CHECK(d(t1, 0x98) == 0x3f800000u && d(t1, 0x9c) == 0x41200000u);

    // toggle 2 -> the bool variable; slots {1, 0} keep it too
    const DebugMenuItem& t2 = o.toggle2;
    CHECK(t2.kind == 2 && t2.flags == 0x30 && t2.target == &o.enabled);
    CHECK(same_view(t2.label, "Save System", 0xb) && same_view(t2.text, "Enable Save System", 0x12));
    CHECK(d(t2, 0x98) == 0x3f800000u && d(t2, 0x9c) == 0x41200000u);

    // pending list: level first, then enabled (registration order)
    CHECK(creg.pending.size() == 2 && creg.pending.first() == &L.base.pending);

    // root list: group, toggle1, toggle2
    CHECK(menu.root_guard_done && menu.root.count == 3);
    CHECK(menu.root.link.next == &g.link && g.link.next == &t1.link && t1.link.next == &t2.link &&
          t2.link.next == &menu.root.link && menu.root.link.prev == &t2.link);

    // atexit order
    const std::pair<std::uint64_t, const void*> want[] = {
        {0xa50d50, &o.cell}, {0x18a60, &o.level}, {0x20b0, &o.enabled}, {0x27280, &menu.root}, {0x272d0, &o.group},
        {0x272d0, &o.toggle1}, {0x272d0, &o.toggle2}, {0xa55f30, &o.tail}};
    CHECK(creg.atexit_log.size() == 8);
    for (std::size_t i = 0; i < 8; ++i) {
        CHECK(creg.atexit_log.at(i).dtor_rva == want[i].first && creg.atexit_log.at(i).object == want[i].second);
    }
}

void section_failures() {
    // Console manager without an insert contract: the level registration fails first.
    {
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot308Objects o;
        poison(o.enabled);
        poison(o.group);
        const Slot308Result r = init_slot308_a58f10(o, creg, menu);
        CHECK(!r.ok && r.blocked == Slot308Stage::level_registration && !r.level_registration.ok);
        CHECK(all_poison(o.enabled) && all_poison(o.group) && menu.root.count == 0);
        CHECK(creg.atexit_log.size() == 1);   // only the cell dtor was recorded before the failure
    }
    // Debug-menu parent without contract: the group fails, the toggles and tail are never built.
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot308Objects o;
        poison(o.toggle1);
        poison(o.toggle2);
        o.tail.zero = {7, 7, 7};
        const Slot308Result r = init_slot308_a58f10(o, creg, menu);
        CHECK(!r.ok && r.blocked == Slot308Stage::group_registration && r.level_registration.ok && r.enabled.ok);
        CHECK(all_poison(o.toggle1) && all_poison(o.toggle2) && o.tail.zero[0] == 7);
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
        Slot308Objects o;
        const Slot308Result r = init_slot308_a58f10(o, creg, menu);
        CHECK(r.ok && inserted == 3 && menu.root.count == 0 && creg.atexit_log.size() == 7);
    }
    CHECK(std::strcmp(slot308_stage_name(Slot308Stage::tail_atexit), "tail_atexit") == 0);
}

}  // namespace

int main() {
    section_constants();
    section_c580();
    section_build();
    section_failures();
    std::puts("OK test_ctor_slot_308");
    return 0;
}
