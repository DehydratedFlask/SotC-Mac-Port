// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 446 (0x4c92a0): 4 bool and 0 bounded float variables and 2 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include <cstdlib>
#include <vector>

#include "core/ctor_slot_446_4c92a0.hpp"
#include "core/helper_f8f630_strncpy.hpp"

namespace {
using namespace l4;


struct Row {
    char kind;            // b bool, i/l/n ranged / local / unbounded int, f/u bounded / unbounded float, m console command, t 0x26db0 toggle, e 0x26ea0 toggle, s 0x27120 slider, S 0x27050 slider, g group
    const void* obj;
    int var;              // index into the bool / float tables (variables)
    int target;           // row index of the target variable (items)
    int s0;               // menu string: parent / label
    int s1;               // menu string: explicit text / caption
    int slider;           // slider table index
    const void* ext;      // identity pointer of an external slider target (target == -1), otherwise nullptr
};

Row rows_of(Slot446Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'b', &o.r00, 0, -1, -1, -1, -1, nullptr};
    out[1] = Row{'b', &o.r01, 1, -1, -1, -1, -1, nullptr};
    out[2] = Row{'h', &o.r02, 0, -1, 0, -1, -1, nullptr};
    out[3] = Row{'b', &o.r03, 2, -1, -1, -1, -1, nullptr};
    out[4] = Row{'b', &o.r04, 3, -1, -1, -1, -1, nullptr};
    out[5] = Row{'c', &o.r05, -1, -1, 3, -1, -1, nullptr};
    out[6] = Row{'l', &o.r06, 0, -1, -1, -1, -1, nullptr};
    out[7] = Row{'g', &o.r07, 48, -1, 4, 5, -1, nullptr};
    out[8] = Row{'e', &o.r08, -1, 6, 5, 6, -1, nullptr};
    out[9] = Row{'Q', &o.r09, 0, -1, -1, -1, -1, nullptr};
    out[10] = Row{'Q', &o.r10, 1, -1, -1, -1, -1, nullptr};
    return r;
}

[[maybe_unused]] float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

[[maybe_unused]] std::uint64_t lint_dtor([[maybe_unused]] std::size_t var) {
    std::uint64_t dtor = 0;
    dtor = find_local_int_var_class(kSlot446LocalIntSites[var].wrapper_rva)->dtor_rva;
    return dtor;
}

void section_constants() {
    CHECK(kSlot446Rva == 0x4c92a0 && kSlot446Size == 2334 && kSlot446RowCount == 11);
    for (const LocalIntSiteSpec& site : kSlot446LocalIntSites) CHECK(find_local_int_var_class(site.wrapper_rva) != nullptr && (find_local_int_var_class(site.wrapper_rva)->fn_vtbl == site.arg_fn_tag || is_local_int_caller_twin_tag(site.arg_fn_tag, find_local_int_var_class(site.wrapper_rva)->fn_vtbl)));   // per-site tag = the wrapper's own class or its proven alias

    CHECK(offsetof(Slot446Objects, r00) == 0x0);
    CHECK(offsetof(Slot446Objects, r01) == 0xc8);
    CHECK(offsetof(Slot446Objects, r02) == 0x190);
    CHECK(offsetof(Slot446Objects, r03) == 0x1c0);
    CHECK(offsetof(Slot446Objects, r04) == 0x288);
    CHECK(offsetof(Slot446Objects, r05) == 0x350);
    CHECK(offsetof(Slot446Objects, r06) == 0x360);
    CHECK(offsetof(Slot446Objects, r07) == 0x438);
    CHECK(offsetof(Slot446Objects, r08) == 0x4e0);
    CHECK(offsetof(Slot446Objects, r09) == 0x5a8);
    CHECK(offsetof(Slot446Objects, r10) == 0x638);
    CHECK(sizeof(Slot446Objects) == 0x6e0);

    for (std::size_t n = 0; n < kSlot446BoolCount; ++n) CHECK(std::strlen(kSlot446BoolNames[n]) == kSlot446BoolNameLengths[n]);
    for (const LocalIntSiteSpec& site : kSlot446LocalIntSites) CHECK(std::strlen(site.name) == site.name_length);
    for (std::size_t n = 0; n < kSlot446StringCount; ++n) CHECK(std::strlen(kSlot446MenuStrings[n]) == kSlot446MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            if (kSlot446BoolDesc[row.var] < 0) check_bool_with_description(v, kSlot446BoolFnTag, kSlot446BoolValues[row.var], "", 0);
            else check_bool_with_description(v, kSlot446BoolFnTag, kSlot446BoolValues[row.var], kSlot446MenuStrings[kSlot446BoolDesc[row.var]], kSlot446MenuStringLengths[kSlot446BoolDesc[row.var]]);
            CHECK(same_view(v.base.name, kSlot446BoolNames[row.var], kSlot446BoolNameLengths[row.var]));
            break;
        }
        case 'l': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            const LocalIntSiteSpec& site = kSlot446LocalIntSites[row.var];
            const LocalIntVarClass* cls = find_local_int_var_class(site.wrapper_rva);
            CHECK(cls != nullptr && (site.arg_fn_tag == cls->fn_vtbl || is_local_int_caller_twin_tag(site.arg_fn_tag, cls->fn_vtbl)));
            CHECK(v.base.vptr == cls->final_vptr && v.base.flags == site.flags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, site.name, site.name_length));
            if (kSlot446LintDesc[row.var] == nullptr) CHECK(empty_view(v.base.description));
            else CHECK(same_view(v.base.description, kSlot446LintDesc[row.var], kSlot446LintDescLength[row.var]));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(site.value) && dword(v, 0x68) == static_cast<std::uint32_t>(site.value));
            CHECK(dword(v, 0x6c) == static_cast<std::uint32_t>(site.min_value) && v.min_engaged == 1 && dword(v, 0x74) == static_cast<std::uint32_t>(site.max_value) && v.max_engaged == 1);
            for (std::uint64_t q : v.args) CHECK(q == 0);
            CHECK(v.fn_vptr == cls->fn_vtbl && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot446MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot446MenuStrings[row.s0]));
            CHECK(c.unwritten == 0xa5a5a5a5u);   // +0x0c is never written
            break;
        }
        case 'o': {
            const auto& c = *static_cast<const NullNameCell247f47*>(row.obj);
            CHECK(c.id == 0 && c.name[0] == 'N' && c.name[1] == 'U' && c.name[2] == 'L' && c.name[3] == 'L');
            for (int b = 4; b < 16; ++b) CHECK(c.name[b] == 0);   // strncpy pads with NULs up to 15 bytes; byte 0x13 is stored 0
            break;
        }
        case 'q': {
            break;
        }
        case 'Q': {
            const auto* b = static_cast<const std::uint8_t*>(row.obj);
            auto q = [&](std::size_t off) { std::uint64_t v = 0; std::memcpy(&v, b + off, sizeof v); return v; };
            const std::uint64_t self88 = static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(b)) + 0x88;
            const std::size_t v = static_cast<std::size_t>(row.var);
            CHECK(q(0x00) == self88 && q(0x20) == self88 && q(0x40) == self88);
            CHECK(q(0x08) == kSlot446EntryFns[3 * v + 0] && q(0x28) == kSlot446EntryFns[3 * v + 1] && q(0x48) == kSlot446EntryFns[3 * v + 2]);
            CHECK(q(0x10) == 0 && q(0x18) == 0 && q(0x30) == 0 && q(0x38) == 0 && q(0x50) == 0 && q(0x58) == 0);
            SharedStr key{};
            std::memcpy(&key, b + 0x60, sizeof key);
            CHECK(same_view(key, kSlot446EntryNames[v], kSlot446EntryNameLengths[v]) && key.ctl == nullptr);
            CHECK(q(0x88) == 0xa5a5a5a5a5a5a5a5ull);   // never written
            std::uint64_t expect_prev = 0;   // the list head cell is null before the first entry (test fixture)
            for (std::size_t j = 0; j < kSlot446RowCount; ++j) if (all[j].kind == 'Q' && static_cast<std::size_t>(all[j].var) + 1 == v) expect_prev = static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(all[j].obj));
            CHECK(q(0x80) == expect_prev);
            if (v + 1 == kSlot446EntryCount) CHECK(ctor_98a90_list_head_cell_2ef6400() == row.obj);   // the last entry is the list head
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            CHECK(h.vptr == kSlot446HolderVptr && h.tail == 0 && same_view(h.name, kSlot446MenuStrings[row.s0], kSlot446MenuStringLengths[row.s0]));
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == ((row.kind == 'g' || row.kind == 'J') ? static_cast<std::uint32_t>(row.var) : kSlot446ItemFlags) && it.registered == 0);
            CHECK(same_view(it.label, kSlot446MenuStrings[row.s0], kSlot446MenuStringLengths[row.s0]));
            if (row.kind == 'x') {
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, kSlot446MenuStrings[row.s1], kSlot446MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, kSlot446MenuStrings[row.var], kSlot446MenuStringLengths[row.var]));   // +0x70: the console command name
                CHECK(it.range_min == 1.0f && it.range_max == 10.0f);
                break;
            }
            if (row.kind == 'J') {
                CHECK(it.kind == 0 && it.target == nullptr && it.flags == static_cast<std::uint32_t>(row.var));   // 0x26800 stores kind 3, then overwrites it with 0
                CHECK(it.text.size == 13 && it.text.data != nullptr && std::memcmp(it.text.data, "MenuSeparator", 13) == 0);   // built by 0xf1a0 (heap copy)
                CHECK(same_view(it.label, kSlot446MenuStrings[row.s0], kSlot446MenuStringLengths[row.s0]));
                CHECK(it.range_min == 1.0f && it.range_max == 10.0f);
                break;
            }
            if (row.kind == 'K') {
                CHECK(it.kind == kMenuKindSeparator && it.target == nullptr);
                CHECK(it.text.size == 13 && it.text.data != nullptr && std::memcmp(it.text.data, "MenuSeparator", 13) == 0);   // built by 0xf1a0 (heap copy)
                break;
            }
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot446MenuStrings[row.s1], kSlot446MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = (tg.kind == 'b' || tg.kind == 'y') ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot446MenuStrings[row.s1], kSlot446MenuStringLengths[row.s1]));
            } else {
                CHECK(it.text.data == tb.name.data && it.text.size == tb.name.size && it.text.ctl == nullptr);   // the variable's own name view
            }
            if (row.kind == 's' || row.kind == 'S') {
                CHECK(false);   // no slider rows in this slot
            } else if (is_float) {
                CHECK(it.range_min == 0.1f && it.range_max == 0.5f);   // float class slot3 = 1, slot6 = 3: the one narrowing case
            } else {
                CHECK(it.range_min == 1.0f && it.range_max == 10.0f);
            }
            break;
        }
    }
}

void section_alone() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot446Objects o;
    poison(o);
    ctor_98a90_self_cell_2ef63f8() = nullptr;
    ctor_98a90_list_head_cell_2ef6400() = nullptr;
    const Slot446Result r = init_slot446_4c92a0(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot446RowCount);
    Row rows[kSlot446RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot446RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'y' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'N' || kd == 'P' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u' || kd == 'd') ++vars;
        else if (kd == 'c' || kd == 'o' || kd == 'a' || kd == 'G' || kd == 'H') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'v' && kd != 'p' && kd != 'T' && kd != 'k' && kd != 'R' && kd != 'G' && kd != 'C' && kd != 'E' && kd != 'L' && kd != 'Q' && kd != 'Z' && kd != 'H') ++items;
    }
    CHECK(vars == kSlot446BoolCount + kSlot446IntCount + kSlot446LocalIntCount + kSlot446UIntCount + kSlot446CmdCount + kSlot446StrVarCount + kSlot446FloatCount + kSlot446UFloatCount);
    CHECK(menu.root.count == items && items == 2 && menu.root.link.next == &o.r07.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot446CellCount && creg.atexit_log.size() == kSlot446RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot446RowCount; ++i) {
        if (rows[i].kind == 'c' || rows[i].kind == 'o' || rows[i].kind == 'a' || rows[i].kind == 'G' || rows[i].kind == 'H') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = (rows[i].kind == 'b' || rows[i].kind == 'y') ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : (rows[i].kind == 'N' || rows[i].kind == 'P') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? kSlot446HolderDtorRva : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : rows[i].kind == 'v' ? 0 : rows[i].kind == 'p' ? 0 : rows[i].kind == 'Z' ? 0 : rows[i].kind == 'j' ? 0 : rows[i].kind == 'T' ? 0 : rows[i].kind == 'k' ? 0 : rows[i].kind == 'R' ? 0 : rows[i].kind == 'd' ? 0 : rows[i].kind == 'C' ? 0 : rows[i].kind == 'E' ? 0 : rows[i].kind == 'L' ? 0 : rows[i].kind == 'Q' ? kSlot446EntryDtorRva : 0x272d0;

        if (dtor == 0x272d0 && !root_seen) {
            check_atexit(creg, at, 0x27280, &menu.root);
            root_seen = true;
            check_atexit(creg, at + 1, dtor, rows[i].obj);
        } else {
            check_atexit(creg, at, dtor, rows[i].kind == 'Z' ? nullptr : rows[i].obj);
        }
    }
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot446Objects o;
    ctor_98a90_self_cell_2ef63f8() = nullptr;
    ctor_98a90_list_head_cell_2ef6400() = nullptr;
        const Slot446Result r = init_slot446_4c92a0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot446Objects o;
    ctor_98a90_self_cell_2ef63f8() = nullptr;
    ctor_98a90_list_head_cell_2ef6400() = nullptr;
        const Slot446Result r = init_slot446_4c92a0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 7);
    }
}

// Entries with the registry object present (the later startup state): each entry is registered under its name (strcasecmp order) with the entry itself as the mapped value; without an allocator
// the first registration is refused visibly. The globals 0x2ef63f8 / 0x2ef6400 are process state and are reset afterwards.
struct EntryAllocLog {
    std::vector<void*> blocks;
};
EntryAllocLog g_entry_log;
void* entry_alloc(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) {
    void* p = std::malloc(size);
    g_entry_log.blocks.push_back(p);
    return p;
}
bool entry_free(void* p, std::uint32_t) {
    std::free(p);
    for (void*& b : g_entry_log.blocks) if (b == p) b = nullptr;
    return true;
}
void entry_log_release() {
    for (void* p : g_entry_log.blocks) std::free(p);
    g_entry_log.blocks.clear();
}

void section_entries() {
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        Slot446Objects o;
        poison(o);
        RbMap map{};
        o.entry_hooks = RbHooks{};
        o.entry_hooks.alloc.allocate = &entry_alloc;
        o.entry_hooks.alloc.deallocate = &entry_free;
        CHECK(rb_map_init_empty(map, o.entry_hooks));
        ctor_98a90_self_cell_2ef63f8() = &map;
        ctor_98a90_list_head_cell_2ef6400() = nullptr;
        const Slot446Result r = init_slot446_4c92a0(o, creg, menu);
        CHECK(r.ok && r.rows_done == kSlot446RowCount);
        CHECK(map.size == kSlot446EntryCount && rb_check_invariants(map).ok);
        Row rows[kSlot446RowCount];
        (void)rows_of(o, rows);
        for (std::size_t i = 0; i < kSlot446RowCount; ++i) {
            if (rows[i].kind != 'Q') continue;
            const std::size_t v = static_cast<std::size_t>(rows[i].var);
            const RbIndexResult found = rb_index_3c0840(map, family_view(kSlot446EntryNames[v], kSlot446EntryNameLengths[v]), o.entry_hooks);
            CHECK(found.ok && !found.created && found.slot != nullptr && *found.slot == rows[i].obj);
        }
        CHECK(map.size == kSlot446EntryCount);
        ctor_98a90_self_cell_2ef63f8() = nullptr;
        ctor_98a90_list_head_cell_2ef6400() = nullptr;
        entry_log_release();
    }
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        Slot446Objects o;
        poison(o);
        RbMap map{};
        RbHooks init_hooks{};
        init_hooks.alloc.allocate = &entry_alloc;
        init_hooks.alloc.deallocate = &entry_free;
        CHECK(rb_map_init_empty(map, init_hooks));
        o.entry_hooks = RbHooks{};   // no allocator: the first registration cannot create its node
        ctor_98a90_self_cell_2ef63f8() = &map;
        ctor_98a90_list_head_cell_2ef6400() = nullptr;
        const Slot446Result r = init_slot446_4c92a0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 9);
        CHECK(map.size == 0);
        ctor_98a90_self_cell_2ef63f8() = nullptr;
        ctor_98a90_list_head_cell_2ef6400() = nullptr;
        entry_log_release();
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    section_entries();
    std::puts("OK test_ctor_slot_446");
    return 0;
}
