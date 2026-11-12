// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 390 (0x691c50): 2 bool and 0 bounded float variables and 0 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include <cstdlib>

#include "core/ctor_slot_390_691c50.hpp"

namespace {
using namespace l4;

std::size_t g_tree_requested = 0;       // size the model passed to the allocator
std::uint8_t* g_tree_block = nullptr;     // the block the test allocator returned (poison-filled; freed at exit)
void* test_tree_alloc(std::size_t size) noexcept {
    g_tree_requested = size;
    g_tree_block = static_cast<std::uint8_t*>(std::malloc(size));
    std::memset(g_tree_block, 0xa5, size);
    return g_tree_block;
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

Row rows_of(Slot390Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'T', &o.r00, -1, -1, -1, -1, -1, nullptr};
    out[1] = Row{'k', &o.r01, -1, -1, -1, -1, -1, nullptr};
    out[2] = Row{'h', &o.r02, 0, -1, 0, -1, -1, nullptr};
    out[3] = Row{'h', &o.r03, 0, -1, 1, -1, -1, nullptr};
    out[4] = Row{'h', &o.r04, 0, -1, 2, -1, -1, nullptr};
    out[5] = Row{'b', &o.r05, 0, -1, -1, -1, -1, nullptr};
    out[6] = Row{'b', &o.r06, 1, -1, -1, -1, -1, nullptr};
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
    CHECK(kSlot390Rva == 0x691c50 && kSlot390Size == 1035 && kSlot390RowCount == 7);
    CHECK(offsetof(Slot390Objects, r00) == 0x0);
    CHECK(offsetof(Slot390Objects, r01) == 0x18);
    CHECK(offsetof(Slot390Objects, r02) == 0x50);
    CHECK(offsetof(Slot390Objects, r03) == 0x80);
    CHECK(offsetof(Slot390Objects, r04) == 0xb0);
    CHECK(offsetof(Slot390Objects, r05) == 0xe0);
    CHECK(offsetof(Slot390Objects, r06) == 0x1a8);
    CHECK(sizeof(Slot390Objects) == 0x278);

    for (std::size_t n = 0; n < kSlot390BoolCount; ++n) CHECK(std::strlen(kSlot390BoolNames[n]) == kSlot390BoolNameLengths[n]);
    for (std::size_t n = 0; n < kSlot390StringCount; ++n) CHECK(std::strlen(kSlot390MenuStrings[n]) == kSlot390MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'b': {
            const auto& v = *static_cast<const ConCmdBoolVar*>(row.obj);
            if (kSlot390BoolDesc[row.var] < 0) check_bool_with_description(v, kSlot390BoolFnTag, kSlot390BoolValues[row.var], "", 0);
            else check_bool_with_description(v, kSlot390BoolFnTag, kSlot390BoolValues[row.var], kSlot390MenuStrings[kSlot390BoolDesc[row.var]], kSlot390MenuStringLengths[kSlot390BoolDesc[row.var]]);
            CHECK(same_view(v.base.name, kSlot390BoolNames[row.var], kSlot390BoolNameLengths[row.var]));
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot390MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot390MenuStrings[row.s0]));
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
            CHECK(node != nullptr && g_tree_requested == kSlot390TreeNodeSize && node == g_tree_block);
            std::uint64_t left = 0;
            std::uint64_t parent = 0;
            std::uint64_t right = 0;
            std::memcpy(&left, node + 0x00, 8);
            std::memcpy(&parent, node + 0x08, 8);
            std::memcpy(&right, node + 0x10, 8);
            const std::uint64_t self = reinterpret_cast<std::uintptr_t>(node);
            CHECK(left == self && parent == self && right == self && node[0x18] == 1 && node[0x19] == 1);
            for (std::size_t b = 0x1a; b < kSlot390TreeNodeSize; ++b) CHECK(node[b] == 0xa5);   // never written by the original
            break;
        }
        case 'k': {
            const auto& n = *static_cast<const NodeRef56b4b0*>(row.obj);
            const HashedStr& empty = static_empty_hashed_2ed3640();
            CHECK(n.node != nullptr && n.flag == 0 && n.id.hash == empty.hash && n.id.str.ctl == empty.str.ctl);   // 0x14c3a0 copy of the static empty string
            const auto* bytes = reinterpret_cast<const std::uint8_t*>(&n);
            for (int b = 4; b < 8; ++b) CHECK(bytes[b] == 0xa5);                  // HashedStr padding is never written
            for (std::size_t b = 0x31; b < 0x38; ++b) CHECK(bytes[b] == 0xa5);    // past the flag byte: never written
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            CHECK(h.vptr == kSlot390HolderVptr && h.tail == 0 && same_view(h.name, kSlot390MenuStrings[row.s0], kSlot390MenuStringLengths[row.s0]));
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot390ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot390MenuStrings[row.s0], kSlot390MenuStringLengths[row.s0]));
            if (row.kind == 'g') {
                CHECK(it.kind == kMenuKindGroup && it.target == nullptr);
                CHECK(same_view(it.text, kSlot390MenuStrings[row.s1], kSlot390MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = tg.kind == 'b' ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot390MenuStrings[row.s1], kSlot390MenuStringLengths[row.s1]));
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
    Slot390Objects o;
    poison(o);
    o.tree_alloc.allocate_17070 = &test_tree_alloc;
    const Slot390Result r = init_slot390_691c50(o, creg);
    CHECK(r.ok && r.rows_done == kSlot390RowCount);
    Row rows[kSlot390RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot390RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'i' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u') ++vars;
        else if (kd == 'c') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'T' && kd != 'k') ++items;
    }
    CHECK(vars == kSlot390BoolCount + kSlot390IntCount + kSlot390LocalIntCount + kSlot390UIntCount + kSlot390CmdCount + kSlot390StrVarCount + kSlot390FloatCount + kSlot390UFloatCount);
    CHECK(items == 0);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r05.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot390CellCount && creg.atexit_log.size() == kSlot390RowCount - cells);   // name cells record nothing; no menu root
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot390RowCount; ++i) {
        if (rows[i].kind == 'c') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = rows[i].kind == 'b' ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? kSlot390HolderDtorRva : rows[i].kind == 'q' ? 0 : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : rows[i].kind == 'T' ? kSlot390TreeDtorRva : rows[i].kind == 'k' ? kSlot390NodeRefDtorRva : 0x272d0;

        check_atexit(creg, at, dtor, rows[i].obj);
    }
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot390Objects o;
        o.tree_alloc.allocate_17070 = &test_tree_alloc;
        const Slot390Result r = init_slot390_691c50(o, creg);
        CHECK(!r.ok && r.rows_done == 5);
        CHECK(creg.pending.size() == 0);
    }
    {
        // No host allocator for 0x17070: the empty-tree row is refused visibly (header zeroed, no atexit record for it); every row before it was built.
        ConCmdRegistry creg;
        Slot390Objects o;
        poison(o);
        o.tree_alloc = EmptyTreeAlloc{};
        const Slot390Result r = init_slot390_691c50(o, creg);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(o.r00.head == nullptr && o.r00.size == 0);
    }
    {
        // The allocator answers null: same visible refusal.
        ConCmdRegistry creg;
        Slot390Objects o;
        poison(o);
        o.tree_alloc.allocate_17070 = [](std::size_t) noexcept -> void* { return nullptr; };
        const Slot390Result r = init_slot390_691c50(o, creg);
        CHECK(!r.ok && r.rows_done == 0);
        CHECK(o.r00.head == nullptr && o.r00.size == 0);
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_390");
    return 0;
}
