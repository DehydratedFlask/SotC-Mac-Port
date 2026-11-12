// CPU-01-STARTUP-SLOT-530: fixture for startup constructor index 530 (0x149f00): the "Resources"
// debug page (five bool variables, one ranged int, one command, one group, seven toggles, two
// partially-written zero cells). Checks every stored field, the bytes the body does NOT write
// (pre-filled with 0x5a), list/atexit order, and fail-closed stages.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>

#include "core/ctor_slot_530_149f00.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kSentinel = 0x5a;

template <class T>
void fill(T& obj) { std::memset(static_cast<void*>(&obj), kSentinel, sizeof obj); }

template <class T>
bool all_sentinel(const T& obj) {
    const auto* b = reinterpret_cast<const std::uint8_t*>(&obj);
    for (std::size_t i = 0; i < sizeof obj; ++i) if (b[i] != kSentinel) return false;
    return true;
}

std::uint32_t d(const void* base, std::size_t off) {
    std::uint32_t v;
    std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(base) + off, 4);
    return v;
}

bool same_view(const SharedStr& v, const char* text, std::size_t len) {
    return v.data != nullptr && v.size == static_cast<std::int64_t>(len) && v.aux == 0 &&
           std::memcmp(v.data, text, len) == 0 && v.ctl == nullptr;
}

void section_constants() {
    CHECK(kSlot530Rva == 0x149f00 && kSlot530BodySize == 3009);
    CHECK(kSlot530TableWordRva == 0x2dd6050 && kSlot530CanaryCellRva == 0x15fe518);
    CHECK(kSlot530CanaryOffset == 0xc0 && kSlot530StackChkFailRva == 0xf8f3a0);
    CHECK(kSlot530CanaryBranchRva == 0x14aaaa && kSlot530CanaryTargetRva == 0x14aabb);
    CHECK(kSlot530CellRva == 0x2ee2d00 && kSlot530ZeroCell0Rva == 0x2ee2d08 && kSlot530ZeroCell1Rva == 0x2ee2d20);
    CHECK(kSlot530Bool0Rva == 0x2ee2d38 && kSlot530RangedRva == 0x2ee3120 && kSlot530CommandRva == 0x2ee31f8);
    CHECK(kSlot530GroupRva == 0x2ee3298 && kSlot530Toggle0Rva == 0x2ee3340 && kSlot530EndRva == 0x2ee37d8);
    CHECK(kSlot530Bool0Rva + kSlot530BoolCount * kSlot530BoolStride == kSlot530RangedRva);
    CHECK(kSlot530RangedRva + sizeof(ConCmdRangedVar) == kSlot530CommandRva);
    CHECK(kSlot530CommandRva + sizeof(ConCmdCommand) == kSlot530GroupRva);
    CHECK(kSlot530GroupRva + sizeof(DebugMenuItem) == kSlot530Toggle0Rva);
    CHECK(kSlot530Toggle0Rva + kSlot530ItemCount * kSlot530ItemStride == kSlot530EndRva);
    CHECK(sizeof(Slot530Objects) == 0xad8);
    CHECK(kSlot530AtexitRows.size() == 16 && kSlot530Items.size() == 7 && kSlot530Bools.size() == 5);
    CHECK(kSlot530AtexitRva == 0xf8f390 && kSlot530ReleaseRva == 0xf590 && kSlot530DsoRva == 0x1604218);
    CHECK(kSlot530ConCmdBaseCtorRva == 0x20af0 && kSlot530ConCmdRegisterRva == 0x20d60);
    CHECK(kSlot530MenuToggleCtorRva == 0x26ea0 && kSlot530MenuGroupCtorRva == 0x26c10);
    CHECK(kSlot530MenuRegisterRva == 0x268b0 && kSlot530MenuItemDtorRva == 0x272d0);
    CHECK(kSlot530BoolFnVtbl == 0x15985b8 && kSlot530RangedArgFnVtbl == 0x15985d8);
    CHECK(kRangedIntArgFnVtblResources == kSlot530RangedArgFnVtbl && kRangedIntFnVtbl == 0x15944a0);
    CHECK(kRangedIntFinalVptr == 0x1594c08 && kFConCmdVptr == 0x1594dd0 && kBoolVarFinalVptr == 0x1594218);
    CHECK(kSlot530RangedMin == 0 && kSlot530RangedMax == 0x7fffffff);
    CHECK(kSlot530RangedValue == 0x1e && kSlot530RangedDefault == 0x1e);
    CHECK(kSlot530BoolValue == 0 && kSlot530BoolDefault == 0);
    CHECK(kSlot530CommandHandler == 0x144320 && kSlot530CommandFlags == 2 && kSlot530ItemFlags == 0x30);
    CHECK(std::strlen(kSlot530Bools[0].name) == kSlot530Bools[0].name_length);
    for (const Slot530BoolSpec& b : kSlot530Bools) CHECK(std::strlen(b.name) == b.name_length);
    for (const Slot530ItemSpec& i : kSlot530Items) CHECK(std::strlen(i.text) == i.text_length);
    CHECK(std::strlen(kSlot530Command.description) == kSlot530Command.description_length);
    CHECK(std::strlen(kSlot530Parent) == kSlot530Group.parent_length);
    CHECK(std::strlen(kSlot530Label) == kSlot530Group.caption_length);
    CHECK(std::strcmp(slot530_stage_name(Slot530Stage::item_atexit), "item_atexit") == 0);
}

void section_c580_alias() {
    // 0x15985d8 is the slot-530 caller tag: its four slots are the same bodies as 0x15d1d80, so the
    // constructor accepts it and installs the class tag 0x15944a0.
    ConCmdRangedVar v;
    fill(v);
    RangedVarArgs args{};
    args.fn_vptr = kSlot530RangedArgFnVtbl;
    CHECK(init_ranged_int_var_c580(v, l2::literal_view("x"), &v.value, 0, 9, 1, args,
                                   SharedStr{nullptr, 0, 0, nullptr}));
    CHECK(v.base.vptr == kRangedIntMidVptr && v.fn_vptr == kRangedIntFnVtbl);
    CHECK(v.args[0] == 0 && v.args[3] == 0 && v.fn_payload == 0 && v.fn_aux == 0);
    ConCmdRangedVar bad;
    fill(bad);
    args.fn_vptr = 0x15d1d80;   // a lifted tag, but accepted too; use an unlifted one below
    args.fn_vptr = 0x15985b8;   // bool tag: not a ranged-int argument functor
    CHECK(!init_ranged_int_var_c580(bad, l2::literal_view("x"), &bad.value, 0, 9, 1, args,
                                    SharedStr{nullptr, 0, 0, nullptr}));
    CHECK(all_sentinel(bad));
}

void section_build() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot530Objects o;
    fill(o);
    std::uint64_t guard = 0x1234;
    const Slot530Result r = init_slot530_149f00(o, creg, menu, Slot530Hooks{&guard});
    CHECK(r.ok && r.blocked == Slot530Stage::none);
    CHECK(r.atexit_records == 16 && r.bools_built == 5 && r.ranged_built && r.command_built);
    CHECK(r.menu_items == 8 && r.views_inert && r.args_functor_inert);
    CHECK(r.canary_checked && r.canary_matched && r.blocked_index == 0);
    CHECK(creg.manager.manager == nullptr);

    // cell and the two partially-written zero cells
    CHECK(o.cell.word == 0);
    for (const Slot530ZeroCell* z : {&o.zero0, &o.zero1}) {
        CHECK(z->word == 0 && z->zero8 == 0 && z->zero10 == 0);
        for (std::uint8_t byte : z->gap04) CHECK(byte == kSentinel);   // +0x04..+0x07 never written
    }

    // five bool variables: 0x20af0 flags 1, final class, value/default 0, tag 0x15985b8
    for (std::size_t i = 0; i < kSlot530BoolCount; ++i) {
        const Slot530BoolSpec& spec = kSlot530Bools[i];
        const ConCmdBoolVar& v = o.bools[i];
        CHECK(v.base.vptr == kBoolVarFinalVptr && v.base.flags == 1 && v.base.registered == 1);
        CHECK(same_view(v.base.name, spec.name, spec.name_length));
        CHECK(v.base.description.size == 0 && v.base.description.ctl == nullptr && v.base.description.data[0] == 0);
        CHECK(v.value == 0 && v.default_value == 0 && v.value_ref == &v.value);
        CHECK(v.fn_vptr == kSlot530BoolFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
        CHECK(v.zero69[0] == 0 && v.zero69[1] == 0 && v.zero69[2] == 0 && v.zero69[3] == 0);
        for (std::uint8_t byte : v.zero70) CHECK(byte == 0);
        for (std::uint8_t byte : v.gap90) CHECK(byte == kSentinel);   // functor buffer never written
        for (std::uint8_t byte : v.gapa8) CHECK(byte == kSentinel);
        for (std::uint8_t byte : v.gap6d) CHECK(byte == kSentinel);
        CHECK(r.bools[i].ok && r.bools[i].views_inert && r.bools[i].atexit_recorded);
    }

    // ranged int: name 23 chars, 0..INT32_MAX, value/default 0x1e, final class, class functor tag
    const ConCmdRangedVar& L = o.ranged;
    CHECK(L.base.vptr == kRangedIntFinalVptr && L.base.flags == 1 && L.base.registered == 1);
    CHECK(same_view(L.base.name, "res_maxLoadingMemoryMiB", 0x17));
    CHECK(L.base.description.size == 0 && L.base.description.ctl == nullptr);
    CHECK(L.value == 0x1e && L.default_value == 0x1e && L.value_ref == &o.ranged.value);
    CHECK(d(&L, 0x6c) == 0 && L.min_engaged == 1 && d(&L, 0x74) == 0x7fffffffu && L.max_engaged == 1);
    CHECK(L.args[0] == 0 && L.args[1] == 0 && L.args[2] == 0 && L.args[3] == 0);
    CHECK(L.fn_vptr == kRangedIntFnVtbl && L.fn_payload == 0 && L.fn_aux == 0);
    for (std::uint8_t byte : L.gap71) CHECK(byte == kSentinel);
    for (std::uint8_t byte : L.gap79) CHECK(byte == kSentinel);
    for (std::uint8_t byte : L.fn_inline) CHECK(byte == kSentinel);
    CHECK(L.gapb8 == 0xa5a5a5a5a5a5a5a5ull || L.gapb8 == 0x5a5a5a5a5a5a5a5aull);
    CHECK(r.ranged_registration.ok && r.ranged_registration.path == ConCmdRegisterPath::pending_list);

    // command: 0x1594dd0, flags 2, delegate self/0x1990/0/0x144320, empty vector, +0x98 untouched
    const ConCmdCommand& c = o.command;
    CHECK(c.base.vptr == kFConCmdVptr && c.base.flags == 2 && c.base.registered == 1);
    CHECK(same_view(c.base.name, "res_resetLoadingStats", 0x15));
    CHECK(same_view(c.base.description, "Reset Loading Stats", 0x13));
    CHECK(c.delegate.object == reinterpret_cast<std::uint64_t>(&c.delegate));
    CHECK(c.delegate.fn == kFConCmdInvokeThunkRva && c.delegate.fn == 0x1990 && c.delegate.adj == 0);
    CHECK(c.delegate.handler == kSlot530CommandHandler);
    CHECK(c.args_begin == nullptr && c.args_end == nullptr && c.args_cap == nullptr);
    for (std::uint8_t byte : c.gap98) CHECK(byte == kSentinel);
    CHECK(r.command.ok && r.command.views_inert && r.command.atexit_recorded);
    CHECK(r.command.vtable_register_slot_ok);

    // group: kind 1, flags 0x30, label Systems, text Resources, no target
    const DebugMenuItem& g = o.group;
    // The 0x268b0 root-list path does not write item.registered (only the parent path does); the
    // byte stays as the host left it.
    CHECK(g.kind == 1 && g.flags == 0x30 && g.target == nullptr && g.registered == 0);
    CHECK(same_view(g.label, "Systems", 7) && same_view(g.text, "Resources", 9));
    CHECK(same_view(g.empty, "Resources", 9));
    CHECK(r.group_registration.ok && r.group_registration.root_initialised && r.group_registration.atexit_recorded);

    // seven toggles: kind 2, label Resources, target classes, default ranges 1.0 / 10.0
    const void* want_target[7] = {&o.bools[0], &o.bools[1], &o.command, &o.ranged,
                                  &o.bools[2], &o.bools[3], &o.bools[4]};
    for (std::size_t i = 0; i < kSlot530ItemCount; ++i) {
        const Slot530ItemSpec& spec = kSlot530Items[i];
        const DebugMenuItem& it = o.items[i];
        CHECK(it.kind == 2 && it.flags == 0x30 && it.registered == 0);
        CHECK(it.target == want_target[i]);
        CHECK(same_view(it.label, "Resources", 9));
        CHECK(same_view(it.text, spec.text, spec.text_length));
        // 0x26ea0 initialises +0x70 through 0xf180 (empty), unlike the group ctor which copies the
        // caption there.
        CHECK(it.empty.size == 0 && it.empty.ctl == nullptr && it.empty.data[0] == 0);
        CHECK(d(&it, 0x98) == 0x3f800000u && d(&it, 0x9c) == 0x41200000u);
        CHECK(r.item_registrations[i].ok && !r.item_registrations[i].root_initialised);
    }

    // pending list: five bools, the ranged int, the command
    CHECK(creg.pending.size() == 7);
    CHECK(creg.pending.first() == &o.bools[0].base.pending);
    const ConCmdBase* pending_order[7] = {&o.bools[0].base, &o.bools[1].base, &o.bools[2].base,
                                          &o.bools[3].base, &o.bools[4].base, &L.base, &c.base};
    {
        std::size_t i = 0;
        for (const ConCmdListNode* node = creg.pending.first();
             node != nullptr && node != &creg.pending.head && i < 7; node = node->next, ++i) {
            const auto* base = reinterpret_cast<const ConCmdBase*>(
                reinterpret_cast<const char*>(node) - offsetof(ConCmdBase, pending));
            CHECK(base == pending_order[i]);
        }
        CHECK(i == 7);
    }

    // menu root: group then the seven toggles
    CHECK(menu.root_guard_done && menu.root.count == 8);
    CHECK(menu.root.link.next == &g.link);
    for (std::size_t i = 0; i < kSlot530ItemCount; ++i) {
        const MenuLink* want_next = i + 1 < kSlot530ItemCount ? &o.items[i + 1].link : &menu.root.link;
        CHECK(o.items[i].link.next == want_next);
    }
    CHECK(g.link.next == &o.items[0].link && menu.root.link.prev == &o.items[6].link);

    // atexit: the sixteen body rows in body order around the one-time menu-root record, which the
    // first 0x268b0 (the group) emits between the command's row and the group's own row.
    CHECK(creg.atexit_log.size() == 17);
    const auto body_object = [&o](const Slot530AtexitRow& row) {
        return reinterpret_cast<const void*>(reinterpret_cast<const std::uint8_t*>(&o) +
                                             (row.object_rva - kSlot530CellRva));
    };
    for (std::size_t i = 0; i < 8; ++i) {
        CHECK(creg.atexit_log.at(i).dtor_rva == kSlot530AtexitRows[i].dtor_rva);
        CHECK(creg.atexit_log.at(i).object == body_object(kSlot530AtexitRows[i]));
    }
    CHECK(creg.atexit_log.at(8).dtor_rva == kDebugMenuRootDtorRva);
    CHECK(creg.atexit_log.at(8).object == &menu.root);
    for (std::size_t i = 8; i < kSlot530AtexitCount; ++i) {
        CHECK(creg.atexit_log.at(i + 1).dtor_rva == kSlot530AtexitRows[i].dtor_rva);
        CHECK(creg.atexit_log.at(i + 1).object == body_object(kSlot530AtexitRows[i]));
    }
}

void section_failures() {
    // Missing manager insert contract: bool 0's registration fails before anything else is built.
    {
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot530Objects o;
        fill(o);
        const Slot530Result r = init_slot530_149f00(o, creg, menu);
        CHECK(!r.ok && r.blocked == Slot530Stage::bool_build && r.blocked_index == 0);
        CHECK(!r.bools[0].ok && r.atexit_records == 1 && !r.ranged_built && !r.command_built);
        // 0x20b0 is recorded unconditionally by the shared bool initialiser, even when the
        // register-once call failed (the original does the same); our body-row counter stays 1.
        CHECK(creg.atexit_log.size() == 2 && menu.root.count == 0);
        CHECK(all_sentinel(o.ranged) && all_sentinel(o.command) && all_sentinel(o.group));
        CHECK(all_sentinel(o.items[0]));
    }
    // Debug-menu parent without an insert contract: the group fails; variables/command exist, items not.
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot530Objects o;
        fill(o);
        const Slot530Result r = init_slot530_149f00(o, creg, menu);
        CHECK(!r.ok && r.blocked == Slot530Stage::group_registration);
        CHECK(r.bools_built == 5 && r.ranged_built && r.command_built && r.menu_items == 0);
        // The group is constructed before 0x268b0 rejects its registration; nothing links it and no
        // toggle is built.
        CHECK(o.group.kind == 1 && all_sentinel(o.items[0]) && all_sentinel(o.items[6]));
        CHECK(creg.atexit_log.size() == 8);   // cell + 5 bools + ranged + command
        CHECK(menu.root.count == 0);
    }
    // With a parent contract the group and seven toggles go to the parent; the fragment completes.
    {
        static int inserted = 0;
        struct P { static bool insert(void*, DebugMenuItem&) noexcept { ++inserted; return true; } };
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        menu.parent.insert = &P::insert;
        Slot530Objects o;
        const Slot530Result r = init_slot530_149f00(o, creg, menu);
        CHECK(r.ok && inserted == 8 && menu.root.count == 0 && creg.atexit_log.size() == 16);
    }
}

}  // namespace

int main() {
    section_constants();
    section_c580_alias();
    section_build();
    section_failures();
    std::puts("OK test_ctor_slot_530");
    return 0;
}
