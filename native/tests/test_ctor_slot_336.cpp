// CPU-01-STARTUP-BATCH-7: fixture for startup constructor index 336 (0x7ec4c0): enum variable `attract_mode_type` (0x7ec160, 0..3), bool
// `cp10_dontAllowAttractModeToFinish`, ranged int `cp10_ForceAttractModeSpawnPlaceId` (-1..0x1a), rand + 0x158340, a 0x26c10 group, three
// 0x26ea0 toggles, a name holder, unbounded float `shadowDemoMode_minLoadTime` (5.0f) and a getter/retain cell with atexit 0x7ea1a0.
#include "l4_test_util.hpp"

#include "core/ctor_slot_336_7ec4c0.hpp"

namespace {
using namespace l4;

int fixed_rand() noexcept { return 0x2468ace; }

int g_retain_count = 0;
void* g_retained = nullptr;
void counting_retain(void* handle) {
    ++g_retain_count;
    g_retained = handle;
}
Object14c9a0* null_acquire() { return nullptr; }

Slot336Hooks test_hooks() {
    Slot336Hooks hooks = default_slot336_hooks();
    hooks.rand_source = &fixed_rand;
    return hooks;
}

void poison_all(Slot336Objects& o) {
    poison(o.var0);
    poison(o.var1);
    poison(o.var2);
    poison(o.rand_cell);
    poison(o.group);
    poison(o.item0);
    poison(o.item1);
    poison(o.item2);
    poison(o.var3);
}

void section_constants() {
    CHECK(kSlot336Rva == 0x7ec4c0 && kSlot336Size == 1880);
    CHECK(std::strlen(kSlot336Var0Name) == kSlot336Var0NameLength && kSlot336Var0NameLength == 0x11);
    CHECK(std::strlen(kSlot336Var0Description) == kSlot336Var0DescriptionLength && kSlot336Var0DescriptionLength == 0x46);
    CHECK(std::strlen(kSlot336Var1Name) == kSlot336Var1NameLength && kSlot336Var1NameLength == 0x21);
    CHECK(std::strlen(kSlot336Var2Name) == kSlot336Var2NameLength && kSlot336Var2NameLength == 0x21);
    CHECK(std::strlen(kSlot336GroupParent) == kSlot336GroupParentLength && std::strlen(kSlot336GroupCaption) == kSlot336GroupCaptionLength);
    CHECK(std::strlen(kSlot336Item0Text) == kSlot336Item0TextLength && std::strlen(kSlot336Item1Text) == kSlot336Item1TextLength &&
          std::strlen(kSlot336Item2Text) == kSlot336Item2TextLength);
    CHECK(std::strlen(kSlot336HolderName) == kSlot336HolderNameLength && std::strlen(kSlot336Var3Name) == kSlot336Var3NameLength);
    CHECK(is_int_functor_tag(kSlot336IntFnTag) && is_float_functor_tag(kSlot336FloatFnTag) && !is_int_functor_tag(kSlot336EnumFnTag));
    CHECK(sizeof(ConCmdBoolVar) == 0xc8 && sizeof(ConCmdRangedVar) == 0xd8 && sizeof(DebugMenuItem) == 0xa8 &&
          sizeof(ReleaseFamilyNameObject) == 0x30);
    CHECK(offsetof(Slot336Objects, var1) == kSlot336Var1Rva - kSlot336Var0Rva);
    CHECK(offsetof(Slot336Objects, var2) == kSlot336Var2Rva - kSlot336Var0Rva);
    CHECK(offsetof(Slot336Objects, rand_cell) == kSlot336RandCellRva - kSlot336Var0Rva);
    CHECK(offsetof(Slot336Objects, group) == kSlot336GroupRva - kSlot336Var0Rva);
    CHECK(offsetof(Slot336Objects, item0) == kSlot336Item0Rva - kSlot336Var0Rva);
    CHECK(offsetof(Slot336Objects, item1) == kSlot336Item1Rva - kSlot336Var0Rva);
    CHECK(offsetof(Slot336Objects, item2) == kSlot336Item2Rva - kSlot336Var0Rva);
    CHECK(offsetof(Slot336Objects, holder) == kSlot336HolderRva - kSlot336Var0Rva);
    CHECK(offsetof(Slot336Objects, var3) == kSlot336Var3Rva - kSlot336Var0Rva);
    CHECK(offsetof(Slot336Objects, cell) == kSlot336CellRva - kSlot336Var0Rva);
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot336Objects o;
    poison_all(o);
    g_retain_count = 0;
    g_retained = nullptr;
    Slot336Hooks hooks = test_hooks();
    hooks.retain_14ccb0 = &counting_retain;
    const Slot336Result r = init_slot336_7ec4c0(o, creg, menu, hooks);
    CHECK(r.ok && r.refusal == HelperRefusal::none);
    CHECK(r.var0.ok && r.var0.ctor_ok && r.var0.functor_destroy_ok && r.var0.views_inert && r.var0.atexit_recorded && r.var0.registration.ok);
    CHECK(r.var1.ok && r.var1.registration.ok);
    CHECK(r.var2.ok && r.var2.refusal == HelperRefusal::none && r.var2.atexit_recorded);
    CHECK(r.group.ok() && r.item0.ok && r.item1.ok && r.item2.ok && r.holder_ok);
    CHECK(r.var3.ok && r.var3.refusal == HelperRefusal::none && r.var3.atexit_recorded);
    CHECK(r.cell_acquired && r.cell_retained && r.cell_atexit_recorded);

    // (1) enum variable
    const ConCmdRangedVar& e = o.var0;
    CHECK(e.base.vptr == kEnumVarFinalVptr && e.base.flags == 1 && e.base.registered == 1);
    CHECK(same_view(e.base.name, kSlot336Var0Name, kSlot336Var0NameLength));
    CHECK(same_view(e.base.description, kSlot336Var0Description, kSlot336Var0DescriptionLength));
    CHECK(e.value_ref == &o.var0.value && dword(e, 0xd0) == 0 && dword(e, 0x68) == 0);
    CHECK(dword(e, 0x6c) == 0 && e.min_engaged == 1 && dword(e, 0x74) == 3 && e.max_engaged == 1);
    CHECK(e.fn_vptr == kEnumVarFnVtbl && e.fn_payload == 0 && e.fn_aux == 0);
    // (2) bool variable
    check_bool_with_description(o.var1, kSlot336BoolFnTag, 1, "", 0);
    CHECK(same_view(o.var1.base.name, kSlot336Var1Name, kSlot336Var1NameLength));
    // (3) ranged int variable
    const ConCmdRangedVar& v = o.var2;
    CHECK(v.base.vptr == kRangedIntFinalVptr && v.base.flags == 1 && v.base.registered == 1);
    CHECK(same_view(v.base.name, kSlot336Var2Name, kSlot336Var2NameLength) && empty_view(v.base.description));
    CHECK(v.value_ref == &o.var2.value && dword(v, 0xd0) == 0 && dword(v, 0x68) == 0);
    CHECK(dword(v, 0x6c) == 0xffffffffu && v.min_engaged == 1 && dword(v, 0x74) == 0x1a && v.max_engaged == 1);
    CHECK(v.fn_vptr == kRangedIntFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
    // (4) rand cell: u32 value, byte +4 cleared, the rest never written
    CHECK(r.rand_value == 0x2468ace && dword(o.rand_cell, 0) == 0x2468ace && o.rand_cell[4] == 0);
    for (std::size_t i = 5; i < 8; ++i) CHECK(o.rand_cell[i] == kPoison);
    // (5) group + toggles (all keep the default {1.0, 10.0} range: no target answers slot3 == 1 && slot6 == 3)
    CHECK(o.group.kind == kMenuKindGroup && o.group.target == nullptr && o.group.flags == 0x30 && o.group.registered == 0);
    CHECK(same_view(o.group.label, kSlot336GroupParent, kSlot336GroupParentLength));   // the parent name
    CHECK(same_view(o.group.text, kSlot336GroupCaption, kSlot336GroupCaptionLength));
    CHECK(o.item0.target == &o.var0 && o.item0.flags == 0x30 && o.item0.kind == 2);
    CHECK(o.item1.target == &o.var1 && o.item1.flags == 0x30 && o.item1.kind == 2);
    CHECK(o.item2.target == &o.var2 && o.item2.flags == 0x30 && o.item2.kind == 2);
    CHECK(same_view(o.item0.label, kSlot336GroupCaption, kSlot336GroupCaptionLength) && same_view(o.item0.text, kSlot336Item0Text, kSlot336Item0TextLength));
    CHECK(same_view(o.item1.label, kSlot336GroupCaption, kSlot336GroupCaptionLength) && same_view(o.item1.text, kSlot336Item1Text, kSlot336Item1TextLength));
    CHECK(same_view(o.item2.label, kSlot336GroupCaption, kSlot336GroupCaptionLength) && same_view(o.item2.text, kSlot336Item2Text, kSlot336Item2TextLength));
    CHECK(o.item0.range_min == 1.0f && o.item0.range_max == 10.0f && o.item1.range_min == 1.0f && o.item1.range_max == 10.0f &&
          o.item2.range_min == 1.0f && o.item2.range_max == 10.0f);
    CHECK(menu.root.count == 4);
    // (6) holder
    CHECK(o.holder.vptr == kSlot336HolderVptr && o.holder.tail == 0 && same_view(o.holder.name, kSlot336HolderName, kSlot336HolderNameLength));
    // (7) unbounded float
    check_float_common(o.var3, false, 0, 0, kSlot336Var3ValueBits);
    CHECK(same_view(o.var3.base.name, kSlot336Var3Name, kSlot336Var3NameLength));
    // (8) cell
    CHECK(o.cell.handle != nullptr && o.cell.flag == 0 && g_retain_count == 1 && g_retained == o.cell.handle);
    for (std::uint8_t b : o.cell.gap09) CHECK(b == 0);   // never written (zero-initialised member)

    CHECK(creg.pending.size() == 4);
    CHECK(creg.pending.first() == &o.var0.base.pending && o.var0.base.pending.next == &o.var1.base.pending &&
          o.var1.base.pending.next == &o.var2.base.pending && o.var2.base.pending.next == &o.var3.base.pending);
    // var0, var1, var2, [root-list destructor on the first item], group, three toggles, holder, var3, cell.
    CHECK(creg.atexit_log.size() == 11);
    check_atexit(creg, 0, 0x7ea160, &o.var0);
    check_atexit(creg, 1, 0x20b0, &o.var1);
    check_atexit(creg, 2, 0x18a60, &o.var2);
    check_atexit(creg, 3, 0x27280, &menu.root);
    check_atexit(creg, 4, 0x272d0, &o.group);
    check_atexit(creg, 5, 0x272d0, &o.item0);
    check_atexit(creg, 6, 0x272d0, &o.item1);
    check_atexit(creg, 7, 0x272d0, &o.item2);
    check_atexit(creg, 8, 0x8ef00, &o.holder);
    check_atexit(creg, 9, 0x20f0, &o.var3);
    check_atexit(creg, 10, 0x7ea1a0, &o.cell);
}

void section_default_rand() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot336Objects o;
    poison_all(o);
    const Slot336Result r = init_slot336_7ec4c0(o, creg, menu, default_slot336_hooks());
    CHECK(r.ok && r.rand_value <= static_cast<std::uint32_t>(RAND_MAX) && dword(o.rand_cell, 0) == r.rand_value);
    // a null rand source falls back to the host rand as well
    ConCmdRegistry creg2;
    DebugMenuRegistry menu2;
    Slot336Objects p;
    poison_all(p);
    Slot336Hooks hooks = default_slot336_hooks();
    hooks.rand_source = nullptr;
    CHECK(init_slot336_7ec4c0(p, creg2, menu2, hooks).ok);
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot336Objects o;
        const Slot336Result r = init_slot336_7ec4c0(o, creg, menu, test_hooks());
        CHECK(!r.ok && !r.var0.ok && !r.var1.ok && !r.var2.ok && !r.group.ok() && !r.item0.ok && !r.holder_ok && !r.var3.ok);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0 && !r.cell_acquired);
    }
    {
        // Menu parent without an insert contract: the group registration fails visibly; the toggles are not reached.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot336Objects o;
        const Slot336Result r = init_slot336_7ec4c0(o, creg, menu, test_hooks());
        CHECK(!r.ok && r.var2.ok && !r.group.ok() && !r.item0.ok && !r.holder_ok && !r.var3.ok && !r.cell_atexit_recorded);
    }
    {
        // Missing acquire / retain hook: the whole page up to the cell is built, the cell stays untouched and ok is false.
        for (int which = 0; which < 3; ++which) {
            ConCmdRegistry creg;
            DebugMenuRegistry menu;
            Slot336Objects o;
            std::memset(static_cast<void*>(&o.cell), 0xA5, sizeof o.cell);
            Slot336Hooks hooks = test_hooks();
            if (which == 0) hooks.acquire_14d880 = nullptr;
            if (which == 1) hooks.retain_14ccb0 = nullptr;
            if (which == 2) hooks.acquire_14d880 = &null_acquire;
            const Slot336Result r = init_slot336_7ec4c0(o, creg, menu, hooks);
            CHECK(!r.ok && r.var3.ok && !r.cell_acquired && !r.cell_retained && !r.cell_atexit_recorded);
            CHECK(creg.atexit_log.size() == 10);
            const auto* raw = reinterpret_cast<const std::uint8_t*>(&o.cell);
            for (std::size_t i = 0; i < sizeof o.cell; ++i) CHECK(raw[i] == 0xA5);
        }
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_default_rand();
    section_failures();
    std::puts("OK test_ctor_slot_336");
    return 0;
}
