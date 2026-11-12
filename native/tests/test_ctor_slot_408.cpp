// CPU-01-STARTUP-BATCH-9: generated fixture for startup constructor index 408 (0x5c7f50): 0 bool and 0 bounded float variables and 0 menu rows
// (0x26db0 / 0x26ea0 toggles, 0x27120 sliders, 0x26c10 groups). Expectations are the ELF-pinned tables of the header; the layout, atexit order and
// the 0x26db0 range rule (bool {1, 10}, float {0.1, 0.5}) are checked against the model.
#include "l4_test_util.hpp"

#include <cmath>

#include "core/ctor_slot_408_5c7f50.hpp"
#include "core/helper_f8f630_strncpy.hpp"

namespace {
using namespace l4;

float test_tanf(float x) noexcept { return std::tan(x); }   // the host tanf hook (libc)

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

Row rows_of(Slot408Objects& o, Row* out) {
    Row r{};
    out[0] = Row{'a', &o.r00, -1, -1, -1, -1, -1, nullptr};
    out[1] = Row{'h', &o.r01, 0, -1, 0, -1, -1, nullptr};
    out[2] = Row{'h', &o.r02, 0, -1, 1, -1, -1, nullptr};
    out[3] = Row{'h', &o.r03, 0, -1, 2, -1, -1, nullptr};
    out[4] = Row{'h', &o.r04, 0, -1, 3, -1, -1, nullptr};
    out[5] = Row{'h', &o.r05, 1, -1, 4, -1, -1, nullptr};
    out[6] = Row{'h', &o.r06, 2, -1, 5, -1, -1, nullptr};
    out[7] = Row{'h', &o.r07, 3, -1, 6, -1, -1, nullptr};
    out[8] = Row{'v', &o.r08, 0, -1, -1, -1, -1, nullptr};
    out[9] = Row{'q', &o.r09, -1, -1, -1, -1, -1, nullptr};
    out[10] = Row{'m', &o.r10, 0, -1, -1, -1, -1, nullptr};
    out[11] = Row{'m', &o.r11, 1, -1, -1, -1, -1, nullptr};
    out[12] = Row{'m', &o.r12, 2, -1, -1, -1, -1, nullptr};
    out[13] = Row{'m', &o.r13, 3, -1, -1, -1, -1, nullptr};
    out[14] = Row{'m', &o.r14, 4, -1, -1, -1, -1, nullptr};
    out[15] = Row{'m', &o.r15, 5, -1, -1, -1, -1, nullptr};
    out[16] = Row{'m', &o.r16, 6, -1, -1, -1, -1, nullptr};
    out[17] = Row{'m', &o.r17, 7, -1, -1, -1, -1, nullptr};
    out[18] = Row{'m', &o.r18, 8, -1, -1, -1, -1, nullptr};
    out[19] = Row{'m', &o.r19, 9, -1, -1, -1, -1, nullptr};
    out[20] = Row{'m', &o.r20, 10, -1, -1, -1, -1, nullptr};
    out[21] = Row{'m', &o.r21, 11, -1, -1, -1, -1, nullptr};
    out[22] = Row{'m', &o.r22, 12, -1, -1, -1, -1, nullptr};
    out[23] = Row{'m', &o.r23, 13, -1, -1, -1, -1, nullptr};
    out[24] = Row{'v', &o.r24, 1, -1, -1, -1, -1, nullptr};
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
    CHECK(kSlot408Rva == 0x5c7f50 && kSlot408Size == 4750 && kSlot408RowCount == 25);
    CHECK(sizeof(ConCmdCommand) == 0xa0);
    CHECK(offsetof(Slot408Objects, r00) == 0x4);
    CHECK(offsetof(Slot408Objects, r01) == 0x90);
    CHECK(offsetof(Slot408Objects, r02) == 0xc0);
    CHECK(offsetof(Slot408Objects, r03) == 0xf0);
    CHECK(offsetof(Slot408Objects, r04) == 0x120);
    CHECK(offsetof(Slot408Objects, r05) == 0x150);
    CHECK(offsetof(Slot408Objects, r06) == 0x180);
    CHECK(offsetof(Slot408Objects, r07) == 0x1b0);
    CHECK(offsetof(Slot408Objects, r08) == 0x568);
    CHECK(offsetof(Slot408Objects, r09) == 0x588);
    CHECK(offsetof(Slot408Objects, r10) == 0x12b20);
    CHECK(offsetof(Slot408Objects, r11) == 0x12bc0);
    CHECK(offsetof(Slot408Objects, r12) == 0x12ca0);
    CHECK(offsetof(Slot408Objects, r13) == 0x12d40);
    CHECK(offsetof(Slot408Objects, r14) == 0x12de0);
    CHECK(offsetof(Slot408Objects, r15) == 0x12e80);
    CHECK(offsetof(Slot408Objects, r16) == 0x12f20);
    CHECK(offsetof(Slot408Objects, r17) == 0x12fc0);
    CHECK(offsetof(Slot408Objects, r18) == 0x13060);
    CHECK(offsetof(Slot408Objects, r19) == 0x13100);
    CHECK(offsetof(Slot408Objects, r20) == 0x131a0);
    CHECK(offsetof(Slot408Objects, r21) == 0x13240);
    CHECK(offsetof(Slot408Objects, r22) == 0x132e0);
    CHECK(offsetof(Slot408Objects, r23) == 0x13380);
    CHECK(offsetof(Slot408Objects, r24) == 0x186d8);
    CHECK(sizeof(Slot408Objects) == 0x18700);

    for (std::size_t n = 0; n < kSlot408CmdCount; ++n) CHECK(std::strlen(kSlot408CmdNames[n]) == kSlot408CmdNameLengths[n] && kSlot408CmdHandler[n] != 0);
    for (std::size_t n = 0; n < kSlot408StringCount; ++n) CHECK(std::strlen(kSlot408MenuStrings[n]) == kSlot408MenuStringLengths[n]);
}

void check_row(const Row& row, const Row* all, const ConCmdRegistry&) {
    switch (row.kind) {
        case 'm': {
            const auto& c = *static_cast<const ConCmdCommand*>(row.obj);
            check_command(c, kSlot408CmdNames[row.var], kSlot408MenuStrings[kSlot408CmdDesc[row.var]], kSlot408MenuStringLengths[kSlot408CmdDesc[row.var]], kSlot408CmdHandler[row.var]);
            break;
        }
        case 'c': {
            const auto& c = *static_cast<const NameHashCell*>(row.obj);
            CHECK(c.name == kSlot408MenuStrings[row.s0] && c.hash == fnv_cstr_52a990(kSlot408MenuStrings[row.s0]));
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
           const auto& q = *static_cast<const Slot408Sentinel*>(row.obj);
           const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(&q);
           CHECK(q.q00 == base + 0x28 && q.q08 == base + 0x28 && q.q20 == base + 0x28 && q.q10 == base + 0xa8);
           CHECK(q.q18 == 0xa5a5a5a5a5a5a5a5ull);   // never written
           for (std::uint8_t b : q.area) CHECK(b == 0xa5);   // never written
            break;
        }
        case 'v': {
            const auto& z = *static_cast<const Slot408Zero*>(row.obj);
            CHECK(z.q00 == 0xa5a5a5a5a5a5a5a5ull && z.q08 == 0 && z.q10 == 0 && z.q18 == 0);   // begin / end / cap zeroed; +0x00 is never written
            break;
        }
        case 'a': {
            const auto& p = *static_cast<const Slot408Perspective*>(row.obj);
            // Independent evaluation of the documented 0x1580a0 arithmetic (single precision, instruction order): left-handed projection, m[11] = 1.
            float fov = 0.0f;
            float aspect = 0.0f;
            float near_plane = 0.0f;
            float far_plane = 0.0f;
            std::memcpy(&fov, &kSlot408PerspectiveFovBits, 4);
            std::memcpy(&aspect, &kSlot408PerspectiveAspectBits, 4);
            std::memcpy(&near_plane, &kSlot408PerspectiveNearBits, 4);
            std::memcpy(&far_plane, &kSlot408PerspectiveFarBits, 4);
            const float t = std::tan(fov * 0.5f);
            const float c = 1.0f / t;
            float want[16] = {};
            want[0] = c / aspect;
            want[5] = c;
            want[11] = 1.0f;
            const float d = kSlot408PerspectiveFlag != 0 ? near_plane - far_plane : far_plane - near_plane;
            want[10] = (kSlot408PerspectiveFlag != 0 ? near_plane : far_plane) / d;
            want[14] = -(near_plane * far_plane) / d;
            for (int q = 0; q < 16; ++q) CHECK(std::memcmp(&p.m[q], &want[q], 4) == 0);   // all sixteen elements written, bit-exact
            CHECK(p.m[11] == 1.0f && p.m[1] == 0.0f && p.m[15] == 0.0f);
            break;
        }
        case 'h': {
            const auto& h = *static_cast<const NameHolder30*>(row.obj);
            CHECK(h.vptr == (row.var == 0 ? kSlot408HolderVptr : (row.var == 1 ? kSlot408HolderVptrB : (row.var == 2 ? kSlot408HolderVptrC : kSlot408HolderVptrD))) && h.tail == 0 && same_view(h.name, kSlot408MenuStrings[row.s0], kSlot408MenuStringLengths[row.s0]));
            break;
        }
        default: {
            const auto& it = *static_cast<const DebugMenuItem*>(row.obj);
            CHECK(it.flags == kSlot408ItemFlags && it.registered == 0);
            CHECK(same_view(it.label, kSlot408MenuStrings[row.s0], kSlot408MenuStringLengths[row.s0]));
            if (row.kind == 'x') {
                CHECK(it.kind == 2 && row.ext != nullptr && it.target == row.ext);   // opaque identity pointer, stored verbatim
                CHECK(same_view(it.text, kSlot408MenuStrings[row.s1], kSlot408MenuStringLengths[row.s1]));
                CHECK(same_view(it.empty, kSlot408MenuStrings[row.var], kSlot408MenuStringLengths[row.var]));   // +0x70: the console command name
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
                CHECK(same_view(it.text, kSlot408MenuStrings[row.s1], kSlot408MenuStringLengths[row.s1]));
                break;
            }
            const Row& tg = all[row.target];
            CHECK(it.kind == 2 && it.target == tg.obj);
            const bool is_float = tg.kind == 'f' || tg.kind == 'u';
            const ConCmdBase& tb = (tg.kind == 'b' || tg.kind == 'y') ? static_cast<const ConCmdBoolVar*>(tg.obj)->base : tg.kind == 'm' ? static_cast<const ConCmdCommand*>(tg.obj)->base : tg.kind == 'z' ? static_cast<const ConCmdStringVar*>(tg.obj)->base : static_cast<const ConCmdRangedVar*>(tg.obj)->base;
            if (row.kind == 'e' || row.kind == 'S') {
                CHECK(same_view(it.text, kSlot408MenuStrings[row.s1], kSlot408MenuStringLengths[row.s1]));
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
    Slot408Objects o;
    poison(o);
    o.perspective_tanf = &test_tanf;
    const Slot408Result r = init_slot408_5c7f50(o, creg);
    CHECK(r.ok && r.rows_done == kSlot408RowCount);
    Row rows[kSlot408RowCount];
    (void)rows_of(o, rows);
    std::size_t vars = 0;
    std::size_t items = 0;
    std::size_t cells = 0;
    for (std::size_t i = 0; i < kSlot408RowCount; ++i) {
        check_row(rows[i], rows, creg);
        const char kd = rows[i].kind;
        if (kd == 'b' || kd == 'y' || kd == 'i' || kd == 'j' || kd == 'l' || kd == 'n' || kd == 'm' || kd == 'z' || kd == 'f' || kd == 'u' || kd == 'd') ++vars;
        else if (kd == 'c' || kd == 'o' || kd == 'a' || kd == 'G') ++cells;
        else if (kd != 'h' && kd != 'q' && kd != 'r' && kd != 'w' && kd != 'v' && kd != 'p' && kd != 'T' && kd != 'k' && kd != 'R' && kd != 'G' && kd != 'C') ++items;
    }
    CHECK(vars == kSlot408BoolCount + kSlot408IntCount + kSlot408LocalIntCount + kSlot408UIntCount + kSlot408CmdCount + kSlot408StrVarCount + kSlot408FloatCount + kSlot408UFloatCount);
    CHECK(items == 0);
    CHECK(creg.pending.size() == vars && creg.pending.first() == &o.r10.base.pending);
    // one atexit record per row, plus the menu-root destructor 0x27280 recorded once, just before the first item.
    CHECK(cells == kSlot408CellCount && creg.atexit_log.size() == kSlot408RowCount - cells);   // name cells record nothing; no menu root
    bool root_seen = false;
    std::size_t at_base = 0;
    std::size_t reg_shift = 0;   // the record registry's first-use atexit record (0xa81b80), when the slot has named records
    for (std::size_t i = 0; i < kSlot408RowCount; ++i) {
        if (rows[i].kind == 'c' || rows[i].kind == 'o' || rows[i].kind == 'a' || rows[i].kind == 'G') continue;
        const std::size_t at = at_base++ + (root_seen ? 1 : 0) + reg_shift;
        const std::uint64_t dtor = (rows[i].kind == 'b' || rows[i].kind == 'y') ? 0x20b0 : (rows[i].kind == 'i' || rows[i].kind == 'n') ? 0x18a60 : rows[i].kind == 'm' ? 0x4b0 : rows[i].kind == 'z' ? 0x23c90 : rows[i].kind == 'l' ? lint_dtor(rows[i].var) : (rows[i].kind == 'f' || rows[i].kind == 'u') ? 0x20f0 : rows[i].kind == 'h' ? (rows[i].var == 0 ? kSlot408HolderDtorRva : (rows[i].var == 1 ? kSlot408HolderDtorRvaB : (rows[i].var == 2 ? kSlot408HolderDtorRvaC : kSlot408HolderDtorRvaD))) : rows[i].kind == 'q' ? kSlot408SentinelDtorRva : rows[i].kind == 'r' ? 0 : rows[i].kind == 'w' ? 0 : rows[i].kind == 'v' ? kSlot408ZVecDtorRva[rows[i].var] : rows[i].kind == 'p' ? 0 : rows[i].kind == 'j' ? 0 : rows[i].kind == 'T' ? 0 : rows[i].kind == 'k' ? 0 : rows[i].kind == 'R' ? 0 : rows[i].kind == 'd' ? 0 : rows[i].kind == 'C' ? 0 : 0x272d0;

        check_atexit(creg, at, dtor, rows[i].obj);
    }
}

void section_failures() {
    {
        // First variable registration fails: nothing after it is reached and nothing is queued.
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        Slot408Objects o;
        o.perspective_tanf = &test_tanf;
        const Slot408Result r = init_slot408_5c7f50(o, creg);
        CHECK(!r.ok && r.rows_done == 10);
        CHECK(creg.pending.size() == 0);
    }
    {
        // No tanf hook: the perspective row is refused visibly (nothing written for it, no later row runs).
        ConCmdRegistry creg;
        Slot408Objects o;
        poison(o);
        o.perspective_tanf = nullptr;
        const Slot408Result r = init_slot408_5c7f50(o, creg);
        CHECK(!r.ok && r.rows_done == 0);
        const auto* pb = reinterpret_cast<const std::uint8_t*>(&o.r00);
        for (std::size_t b = 0; b < sizeof(o.r00); ++b) CHECK(pb[b] == 0xa5);   // untouched
    }
}

}  // namespace

int main() {
    section_constants();
    section_alone();
    section_failures();
    std::puts("OK test_ctor_slot_408");
    return 0;
}
