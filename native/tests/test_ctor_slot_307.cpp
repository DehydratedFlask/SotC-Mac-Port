// CPU-01-STARTUP-SLOT-307-01: fixture for startup constructor index 307 (0xa5b090): the Trophy System
// block (trophyProcessingEnabled, unlockTrophy, unlockAllTrophies, the "Trophy System" group and two
// toggles). Checks every stored field, untouched bytes, list/atexit order and fail-closed stages.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <utility>

#include "core/ctor_slot_307_a5b090.hpp"

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

void check_command(const ConCmdCommand& c, const char* name, std::size_t nlen, const char* desc, std::size_t dlen,
                   std::uint64_t handler) {
    CHECK(c.base.vptr == kFConCmdVptr && c.base.flags == kFConCmdFlags && c.base.registered == 1);
    CHECK(same_view(c.base.name, name, nlen));
    CHECK(c.base.description.size == static_cast<std::int64_t>(dlen) && c.base.description.ctl == nullptr &&
          std::memcmp(c.base.description.data, desc, dlen) == 0);
    CHECK(c.delegate.object == reinterpret_cast<std::uint64_t>(&c.delegate) && c.delegate.fn == 0x1990 &&
          c.delegate.adj == 0 && c.delegate.handler == handler);
    CHECK(c.args_begin == nullptr && c.args_end == nullptr && c.args_cap == nullptr);
}

void section_constants() {
    CHECK(kSlot307Rva == 0xa5b090 && kSlot307BodySize == 1288);
    CHECK(kSlot307CellRva == 0x32f8b80 && kSlot307VarRva == 0x32f8b90 && kSlot307CaptionRva == 0x32f8c58);
    CHECK(kSlot307GroupRva == 0x32f8c78 && kSlot307Toggle1Rva == 0x32f8d20 && kSlot307Tail1Rva == 0x32f8dc8);
    CHECK(kSlot307Tail2Rva == 0x32f8de8 && kSlot307Command1Rva == 0x32f8e08 && kSlot307Command2Rva == 0x32f8ea8);
    CHECK(kSlot307Toggle2Rva == 0x32f8f48);
    // The native object sizes reproduce the original .bss strides.
    CHECK(kSlot307VarRva + sizeof(ConCmdBoolVar) == kSlot307CaptionRva);
    CHECK(kSlot307GroupRva + 0xa8 == kSlot307Toggle1Rva && kSlot307Toggle1Rva + 0xa8 == kSlot307Tail1Rva);
    CHECK(kSlot307Command1Rva + sizeof(ConCmdCommand) == kSlot307Command2Rva);
    CHECK(kSlot307Command2Rva + sizeof(ConCmdCommand) == kSlot307Toggle2Rva);
    CHECK(kSlot307FnVtbl == 0x15d1dc0 && kSlot307Value == 0 && kSlot307Default == 0);
    CHECK(kSlot307Handler1Rva == 0xa59410 && kSlot307Handler2Rva == 0xa595c0);
}

void section_build() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot307Objects o;
    poison(o.var);
    const Slot307Result r = init_slot307_a5b090(o, creg, menu);
    CHECK(r.ok && r.blocked == Slot307Stage::none);
    CHECK(r.var.ok && r.var.views_inert && r.var.atexit_recorded);
    CHECK(r.command1.ok && r.command2.ok && !r.command1.handler_invoked && !r.command2.handler_invoked);
    CHECK(r.group_registration.root_initialised && r.group_registration.atexit_recorded);
    CHECK(!r.toggle1_registration.root_initialised && !r.toggle2_registration.root_initialised);

    CHECK(o.cell == 0);
    for (auto w : o.tail1) CHECK(w == 0);
    for (auto w : o.tail2) CHECK(w == 0);

    // bool variable: value 0 / default 0, functor tag 0x15d1dc0, flags 1, empty description
    const ConCmdBoolVar& v = o.var;
    CHECK(v.base.vptr == kBoolVarFinalVptr && v.base.flags == 1 && v.base.registered == 1);
    CHECK(v.value == 0 && v.default_value == 0 && v.fn_vptr == 0x15d1dc0 && v.fn_payload == 0 && v.fn_aux == 0);
    CHECK(v.value_ref == &v.value);
    CHECK(same_view(v.base.name, "trophyProcessingEnabled", 0x17));
    CHECK(v.base.description.size == 0 && v.base.description.ctl == nullptr && v.base.description.data[0] == 0);

    CHECK(same_view(o.caption, "Trophy System", 0xd));

    // group
    const DebugMenuItem& g = o.group;
    CHECK(g.kind == 1 && g.flags == 0x30 && g.target == nullptr);
    CHECK(same_view(g.label, "Platform Services", 0x11));
    CHECK(same_view(g.text, "Trophy System", 0xd) && same_view(g.empty, "Trophy System", 0xd));

    // toggle 1 -> the variable; bool slots {1, 0} keep {1.0, 10.0}
    const DebugMenuItem& t1 = o.toggle1;
    CHECK(t1.kind == 2 && t1.flags == 0x30 && t1.target == &o.var);
    CHECK(same_view(t1.label, "Trophy System", 0xd) && same_view(t1.text, "Processing Enabled", 0x12));
    CHECK(d(t1, 0x98) == 0x3f800000u && d(t1, 0x9c) == 0x41200000u);

    // commands
    check_command(o.command1, "unlockTrophy", 12, "Unlocks a trophy. +unlockTrophy <trophy_name>", 0x2d, 0xa59410);
    check_command(o.command2, "unlockAllTrophies", 17, "Unlocks all trophies.", 0x15, 0xa595c0);

    // toggle 2 -> command 2
    const DebugMenuItem& t2 = o.toggle2;
    CHECK(t2.kind == 2 && t2.flags == 0x30 && t2.target == &o.command2);
    CHECK(same_view(t2.label, "Trophy System", 0xd) && same_view(t2.text, "Unlock All Trophies", 0x13));
    CHECK(d(t2, 0x98) == 0x3f800000u && d(t2, 0x9c) == 0x41200000u);

    // pending list: variable first, then the two commands (registration order)
    CHECK(creg.pending.size() == 3 && creg.pending.first() == &v.base.pending);

    // root list: group, toggle1, toggle2
    CHECK(menu.root_guard_done && menu.root.count == 3);
    CHECK(menu.root.link.next == &g.link && g.link.next == &t1.link && t1.link.next == &t2.link &&
          t2.link.next == &menu.root.link && menu.root.link.prev == &t2.link);

    // atexit order
    const std::pair<std::uint64_t, const void*> want[] = {
        {0xa59370, &o.cell}, {0x20b0, &o.var}, {0xf7b0, &o.caption}, {0x27280, &menu.root}, {0x272d0, &o.group},
        {0x272d0, &o.toggle1}, {0xa593a0, &o.tail1}, {0xa593b0, &o.tail2}, {0x4b0, &o.command1},
        {0x4b0, &o.command2}, {0x272d0, &o.toggle2}};
    CHECK(creg.atexit_log.size() == 11);
    for (std::size_t i = 0; i < 11; ++i) {
        CHECK(creg.atexit_log.at(i).dtor_rva == want[i].first && creg.atexit_log.at(i).object == want[i].second);
    }
}

void section_failures() {
    // Console manager without an insert contract: the variable registration fails first.
    {
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot307Objects o;
        poison(o.group);
        const Slot307Result r = init_slot307_a5b090(o, creg, menu);
        CHECK(!r.ok && r.blocked == Slot307Stage::var_build && !r.var.ok);
        CHECK(all_poison(o.group) && menu.root.count == 0);
    }
    // Debug-menu parent without contract: the group fails, toggle 1 and later are never built.
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot307Objects o;
        poison(o.toggle1);
        poison(o.command1);
        const Slot307Result r = init_slot307_a5b090(o, creg, menu);
        CHECK(!r.ok && r.blocked == Slot307Stage::group_registration && r.var.ok);
        CHECK(all_poison(o.toggle1) && all_poison(o.command1));
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
        Slot307Objects o;
        const Slot307Result r = init_slot307_a5b090(o, creg, menu);
        CHECK(r.ok && inserted == 3 && menu.root.count == 0 && creg.atexit_log.size() == 10);
    }
    CHECK(std::strcmp(slot307_stage_name(Slot307Stage::toggle2_atexit), "toggle2_atexit") == 0);
}

}  // namespace

int main() {
    section_constants();
    section_build();
    section_failures();
    std::puts("OK test_ctor_slot_307");
    return 0;
}
