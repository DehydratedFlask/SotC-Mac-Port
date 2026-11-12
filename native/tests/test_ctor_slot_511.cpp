// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 511 (0x1d90c0): 0 bool and 0 bounded float variables and 4 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include <cstdlib>

#include "core/ctor_slot_511_1d90c0.hpp"

namespace {
using namespace l4;

std::size_t g_tree_requested = 0;       // size the model passed to the allocator
std::uint8_t* g_tree_blocks[8] = {};    // the blocks the test allocator returned (poison-filled; freed at exit)
std::size_t g_tree_count = 0;
void* test_tree_alloc(std::size_t size) noexcept {
    g_tree_requested = size;
    if (g_tree_count >= 8) return nullptr;
    std::uint8_t* b = static_cast<std::uint8_t*>(std::malloc(size));
    std::memset(b, 0xa5, size);
    g_tree_blocks[g_tree_count++] = b;
    return b;
}
bool is_tree_block(const std::uint8_t* p) noexcept {
    for (std::size_t i = 0; i < g_tree_count; ++i) if (g_tree_blocks[i] == p) return true;
    return false;
}

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

Row rows_of(Slot511Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'n', &o.r00, 0, -1, -1, -1, -1, nullptr};
    out[1] = Row{'n', &o.r01, 1, -1, -1, -1, -1, nullptr};
    out[2] = Row{'n', &o.r02, 2, -1, -1, -1, -1, nullptr};
    out[3] = Row{'n', &o.r03, 3, -1, -1, -1, -1, nullptr};
    out[4] = Row{'t', &o.r04, -1, 0, 0, -1, -1, nullptr};
    out[5] = Row{'t', &o.r05, -1, 1, 0, -1, -1, nullptr};
    out[6] = Row{'t', &o.r06, -1, 2, 0, -1, -1, nullptr};
    out[7] = Row{'t', &o.r07, -1, 3, 0, -1, -1, nullptr};
    out[8] = Row{'T', &o.r08, -1, -1, -1, -1, -1, nullptr};
    out[9] = Row{'T', &o.r09, -1, -1, -1, -1, -1, nullptr};
    return r;
}

[[maybe_unused]] float bits_to_float(std::uint32_t b) {
    float f;
    std::memcpy(&f, &b, sizeof f);
    return f;
}

[[maybe_unused]] std::uint64_t lint_dtor([[maybe_unused]] std::size_t var) {
    std::uint64_t dtor = 0;
    return dtor;
}

void section_constants() {
    CHECK(kSlot511Rva == 0x1d90c0 && kSlot511Size == 1550 && kSlot511RowCount == 10);
    CHECK(is_int_functor_tag(kSlot511UIntFnTag));
    CHECK(offsetof(Slot511Objects, r00) == 0x0);
    CHECK(offsetof(Slot511Objects, r01) == 0xd8);
    CHECK(offsetof(Slot511Objects, r02) == 0x1b0);
    CHECK(offsetof(Slot511Objects, r03) == 0x288);
    CHECK(offsetof(Slot511Objects, r04) == 0x360);
    CHECK(offsetof(Slot511Objects, r05) == 0x408);
    CHECK(offsetof(Slot511Objects, r06) == 0x4b0);
    CHECK(offsetof(Slot511Objects, r07) == 0x558);
    CHECK(offsetof(Slot511Objects, r08) == 0x600);
    CHECK(offsetof(Slot511Objects, r09) == 0x618);
    CHECK(sizeof(Slot511Objects) == 0x638);

    for (std::size_t n = 0; n < kSlot511UIntCount; ++n) CHECK(std::strlen(kSlot511UIntNames[n]) == kSlot511UIntNameLengths[n]);
    for (std::size_t n = 0; n < kSlot511StringCount; ++n) CHECK(std::strlen(kSlot511MenuStrings[n]) == kSlot511MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'n': {
            const auto& v = *static_cast<const ConCmdRangedVar*>(row.obj);
            CHECK(v.base.vptr == kUnboundedIntFinalVptr && v.base.flags == kSlot511VarFlags && v.base.registered == 1 && v.value_ref == &v.value);
            CHECK(same_view(v.base.name, kSlot511UIntNames[row.var], kSlot511UIntNameLengths[row.var]) && empty_view(v.base.description));
            CHECK(dword(v, 0xd0) == static_cast<std::uint32_t>(kSlot511UIntValue[row.var]) && dword(v, 0x68) == static_cast<std::uint32_t>(kSlot511UIntValue[row.var]));
            CHECK(v.min_engaged == 0 && v.max_engaged == 0);
            CHECK(v.fn_vptr == kIntFunctorClassTag && v.fn_payload == 0 && v.fn_aux == 0);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot511MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot511MenuStrings[row.s0]));
            CHECK(c.unwritten == 0xa5a5a5a5u);   // +0x0c is never written
            break;
        }
        case 'q': {
            break;
        }
        case 'T': {
            const auto& t = *static_cast<const EmptyTreeHeader*>(row.obj);
            CHECK(t.q00 == 0xa5a5a5a5a5a5a5a5ull && t.size == 0);   // +0x00 is never written; the size is zeroed
            const auto* node = static_cast<const std::uint8_t*>(t.head);
            CHECK(node != nullptr && g_tree_requested == kSlot511TreeNodeSize && is_tree_block(node));
            std::uint64_t left = 0;
            std::uint64_t parent = 0;
            std::uint64_t right = 0;
            std::memcpy(&left, node + 0x00, 8);
            std::memcpy(&parent, node + 0x08, 8);
            std::memcpy(&right, node + 0x10, 8);
            const std::uint64_t self = reinterpret_cast<std::uintptr_t>(node);
            CHECK(left == self && parent == self && right == self && node[0x18] == 1 && node[0x19] == 1);
            for (std::size_t b = 0x1a; b < kSlot511TreeNodeSize; ++b) CHECK(node[b] == 0xa5);   // never written by the original
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            (void)h;
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot511ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot511MenuStrings[row.s0], kSlot511MenuStringLengths[row.s0]));
            if (row.kind == 'x') {
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, kSlot511MenuStrings[row.s1], kSlot511MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, kSlot511MenuStrings[row.var], kSlot511MenuStringLengths[row.var]));   // +0x70: the console command name
                CHECK(it.range_min == 1.0f && it.range_max == 10.0f);
                break;
            }
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot511MenuStrings[row.s1], kSlot511MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = (tg.kind == 'b' || tg.kind == 'y') ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot511MenuStrings[row.s1], kSlot511MenuStringLengths[row.s1]));
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
    Slot511Objects o;
    poison(o);
    o.tree_alloc.allocate_17070 = &test_tree_alloc;
    const Slot511Result r = init_slot511_1d90c0(o, creg, menu);
    CHECK(r.ok && r.rows_done == kSlot511RowCount);
    Row rows[kSlot511RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot511RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'y' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'p' && kd != 'T' && kd != 'k') ++items;
    }
    CHECK(vars == kSlot511BoolCount + kSlot511IntCount + kSlot511LocalIntCount + kSlot511UIntCount + kSlot511CmdCount + kSlot511StrVarCount + kSlot511FloatCount + kSlot511UFloatCount);
    CHECK(menu.root.count == items && items == 4 && menu.root.link.next == &o.r04.link);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r00.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot511CellCount && creg.atexit_log.size() == kSlot511RowCount - cells + 1);   // name cells record nothing
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot511RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = (rows[i].kind == 'b' || rows[i].kind == 'y') ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? 0 : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : rows[i].kind == 'p' ? 0 : rows[i].kind == 'j' ? 0 : rows[i].kind == 'T' ? kSlot511TreeDtorRva : rows[i].kind == 'k' ? 0 : 0x272d0;

        if (dtor == 0x272d0 && !root_seen) {
            check_atexit(creg, at, 0x27280, &menu.root);
            root_seen = true;
            check_atexit(creg, at + 1, dtor, rows[i].obj);
        } else {
            check_atexit(creg, at, dtor, rows[i].obj);
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
        Slot511Objects o;
        o.tree_alloc.allocate_17070 = &test_tree_alloc;
        const Slot511Result r = init_slot511_1d90c0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(menu.root.count == 0 && creg.pending.size() == 0);
    }
    {
        // Menu parent without an insert contract: the first item's registration fails visibly; every variable before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot511Objects o;
        const Slot511Result r = init_slot511_1d90c0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 4);
    }
    {
        // No host allocator for 0x17070: the empty-tree row is refused visibly (header zeroed, no atexit record for it); every row before it was built.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        Slot511Objects o;
        poison(o);
        o.tree_alloc = EmptyTreeAlloc{};
        const Slot511Result r = init_slot511_1d90c0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 8);
        CHECK(o.r08.head == nullptr && o.r08.size == 0);
    }
    {
        // The allocator answers null: same visible refusal.
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        Slot511Objects o;
        poison(o);
        o.tree_alloc.allocate_17070 = [](std::size_t) noexcept -> void* { return nullptr; };
        const Slot511Result r = init_slot511_1d90c0(o, creg, menu);
        CHECK(!r.ok && r.rows_done == 8);
        CHECK(o.r08.head == nullptr && o.r08.size == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_511");
    return 0;
}
