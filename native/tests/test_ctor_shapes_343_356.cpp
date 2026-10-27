// CPU-01-STARTUP-SLOTS-343-356 (lane L4): fixture for the shared shapes (0x6aa00, extended 0xc300 functor set,
// 0x26db0, the zero-vector and flag cells) and for running all nine slots in table order against ONE registry.
#include "l4_test_util.hpp"

#include "core/ctor_slot_343_7d7970.hpp"
#include "core/ctor_slot_344_7d3810.hpp"
#include "core/ctor_slot_345_7cec60.hpp"
#include "core/ctor_slot_347_7c93a0.hpp"
#include "core/ctor_slot_348_7c9150.hpp"
#include "core/ctor_slot_350_7af0b0.hpp"
#include "core/ctor_slot_353_7a14d0.hpp"
#include "core/ctor_slot_355_7946d0.hpp"
#include "core/ctor_slot_356_78e2f0.hpp"

namespace {
using namespace l4;

SharedStr borrowed(const char* t) {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(t), static_cast<std::int64_t>(std::strlen(t)), 0, nullptr};
}

void section_functors() {
    for (std::uint64_t v : {0x15d57e0ull, 0x1594318ull, 0x15d2040ull, 0x15ce318ull, 0x15ce260ull, 0x15ce1c0ull,
                            0x1596878ull}) {
        CHECK(is_known_float_arg_functor(v));
    }
    CHECK(!is_known_float_arg_functor(0) && !is_known_float_arg_functor(0x15ce1a0) /* a BOOL functor */ &&
          !is_known_float_arg_functor(0x15968b8));
}

void section_unbounded_vs_bounded() {
    RangedVarArgs args{};
    args.fn_vptr = kFloatArgFnVtbl_350;
    const SharedStr n = borrowed("v");
    const SharedStr d = borrowed("");
    ConCmdRangedVar b, u;
    poison(b);
    poison(u);
    float lo = 0.0f, hi = 4.0f;
    CHECK(init_bounded_float_var_c300(b, n, &b.value, lo, hi, 1, args, d));
    CHECK(init_unbounded_float_var_6aa00(u, n, &u.value, 1, args, d));
    // Same stores except the engaged bytes and the bound values.
    CHECK(b.min_engaged == 1 && b.max_engaged == 1 && u.min_engaged == 0 && u.max_engaged == 0);
    CHECK(u.min_value[0] == 0 && u.max_value[0] == 0);
    CHECK(b.base.vptr == 0x1594128 && u.base.vptr == 0x1594128);
    CHECK(b.fn_vptr == 0x1594318 && u.fn_vptr == 0x1594318 && u.fn_payload == 0 && u.fn_aux == 0);
    CHECK(u.value_ref == &u.value && b.value_ref == &b.value);
    check_untouched(u, {{0x00, 8}, {0x08, 0x10}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 8}, {0x6c, 1}, {0x70, 1},
                        {0x74, 1}, {0x78, 1}, {0x80, 0x20}, {0xb0, 8}, {0xc0, 8}, {0xc8, 8}});

    // Non-zero args block: q[3] != 0 makes q[0] self-referential, payload and aux are copied.
    RangedVarArgs a2{};
    a2.q[0] = 7; a2.q[1] = 8; a2.q[2] = 9; a2.q[3] = 10;
    a2.fn_payload = 0x55; a2.fn_aux = 0x66; a2.fn_vptr = kFloatArgFnVtbl_353B;
    ConCmdRangedVar w;
    poison(w);
    CHECK(init_unbounded_float_var_6aa00(w, n, &w.value, 1, a2, d));
    CHECK(w.args[0] == reinterpret_cast<std::uint64_t>(&w.args[0]) && w.args[1] == 8 && w.args[2] == 9 && w.args[3] == 10);
    CHECK(w.fn_payload == 0x55 && w.fn_aux == 0x66 && w.fn_vptr == 0x1594318);

    // Unknown functor class: nothing written.
    RangedVarArgs bad{};
    bad.fn_vptr = 0x1234;
    ConCmdRangedVar x;
    poison(x);
    CHECK(!init_unbounded_float_var_6aa00(x, n, &x.value, 1, bad, d));
    CHECK(!init_bounded_float_var_c300(x, n, &x.value, 0.0f, 1.0f, 1, bad, d));
    check_untouched(x, {});
}

void section_cells() {
    ConCmdRegistry creg;
    ZeroVector24 z;
    poison(z);
    CHECK(init_zero_vector24(z, creg) && z.begin == 0 && z.end == 0 && z.cap == 0);
    check_atexit(creg, 0, 0x7d3af0, &z);
    FlagScaleCell c;
    poison(c);
    init_flag_scale_cell(c);
    CHECK(c.flag_scale == 0x3f80000000000001ull && c.atomic_word == 0);
    check_untouched(c, {{0, 8}, {0x10, 4}});
}

void section_all_nine() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot343Objects o343; Slot344Objects o344; Slot345Objects o345; Slot347Objects o347; Slot348Objects o348;
    Slot350Objects o350; Slot353Objects o353; Slot355Objects o355; Slot356Objects o356;
    CHECK(init_slot343_7d7970(o343, creg).ok);
    CHECK(init_slot344_7d3810(o344, creg, menu).ok);
    CHECK(init_slot345_7cec60(o345, creg, menu).ok);
    CHECK(init_slot347_7c93a0(o347, creg).ok);
    CHECK(init_slot348_7c9150(o348, creg).ok);
    CHECK(init_slot350_7af0b0(o350, creg).ok);
    CHECK(init_slot353_7a14d0(o353, creg).ok);
    CHECK(init_slot355_7946d0(o355, creg, menu).ok);
    CHECK(init_slot356_78e2f0(o356, creg).ok);
    // pending: 343 cmd, 344 var + cmd, 345 var, 347, 348, 350 x2, 353 x2, 355 x2, 356 = 13
    CHECK(creg.pending.size() == 13);
    // menu items: 344, 345, 355 in this order, one root list, one root destructor.
    CHECK(menu.root.count == 3 && menu.root.link.next == &o344.item.link && o344.item.link.next == &o345.item.link &&
          o345.item.link.next == &o355.item.link && o355.item.link.next == &menu.root.link);
    std::size_t root_dtors = 0;
    for (std::size_t i = 0; i < creg.atexit_log.size(); ++i) {
        if (creg.atexit_log.at(i).dtor_rva == 0x27280) ++root_dtors;
    }
    CHECK(root_dtors == 1);
    // atexit: 343: 3, 344: 4, 345: 2, 347: 1, 348: 1, 350: 2, 353: 2, 355: 3, 356: 1 = 19
    CHECK(creg.atexit_log.size() == 19);
}

}  // namespace

int main() {
    section_functors();
    section_unbounded_vs_bounded();
    section_cells();
    section_all_nine();
    std::puts("OK test_ctor_shapes_343_356");
    return 0;
}
