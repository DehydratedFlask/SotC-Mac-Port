// CPU-01-SLOT306-INTVAR-CTORS-01: fixture for the ranged INT console-variable constructors
// 0xa624a0 / 0xa62860 (+ workers 0xa62660 / 0xa62a20) used three times by startup slot 306.
//
// Sections:
//   1. constants and layout
//   2. functor-table gate (lifted vs unlifted)
//   3. worker alone (0xa62660 / 0xa62a20): vptr, bounds, argument copy, functor install, untouched bytes
//   4. wrapper (0xa624a0 / 0xa62860): final object bytes, registration, argument-functor tag transfer
//   5. fail-closed paths: unlifted functor, manager without insert contract, bad site index
//   6. the three slot-306 sites: names, ranges, atexit records, pending order, borrowed views
//   7. 0x26ea0 virtual-slot results and the debug-menu item range
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <utility>

#include "core/concmd_base.hpp"
#include "core/concmd_vars.hpp"
#include "core/debug_menu_item.hpp"
#include "core/hashed_string_14c3a0.hpp"
#include "core/intvar_a624a0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;

template <class T>
void poison(T& obj) { std::memset(static_cast<void*>(&obj), kPoison, sizeof obj); }

template <class T>
void check_untouched(const T& obj, std::initializer_list<std::pair<std::size_t, std::size_t>> written) {
    const auto* bytes = reinterpret_cast<const std::uint8_t*>(&obj);
    for (std::size_t off = 0; off < sizeof obj; ++off) {
        bool is_written = false;
        for (const auto& [begin, size] : written) {
            if (off >= begin && off < begin + size) { is_written = true; break; }
        }
        if (!is_written && bytes[off] != kPoison) {
            std::fprintf(stderr, "untouched byte +0x%zx changed to 0x%02x\n", off, bytes[off]);
            std::exit(1);
        }
    }
}

template <class T>
std::uint32_t d(const T& obj, std::size_t off) {
    std::uint32_t v;
    std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(&obj) + off, 4);
    return v;
}

SharedStr view(const char* text, std::size_t len, std::int64_t* ctl = nullptr) {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(len), 0, ctl};
}

// Byte ranges the worker alone writes (base ctor included: vptr, flags, name, description; the base
// ctor does not write the pending link, the registered-flag gap or the +0x19 gap).
#define WORKER_WRITTEN                                                                                   \
    {0x00, 8}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 8}, {0x6c, 4}, {0x70, 1}, {0x74, 4}, {0x78, 1}, \
    {0x80, 0x20}, {0xb0, 8}, {0xc0, 8}, {0xc8, 8}
// The wrapper additionally writes default (+0x68), value (+0xd0) and the pending link (+0x08..0x17).
#define WRAPPER_WRITTEN WORKER_WRITTEN, {0x08, 0x10}, {0x68, 4}, {0xd0, 4}

RangedVarArgs zero_args(std::uint64_t table) {
    RangedVarArgs a{};
    a.fn_vptr = table;
    return a;
}

// ------------------------------------------------------------------ 1. constants
void section_constants() {
    static_assert(sizeof(ConCmdRangedVar) >= 0xd4);
    static_assert(offsetof(ConCmdRangedVar, value) == 0xd0 && offsetof(ConCmdRangedVar, default_value) == 0x68);
    CHECK(kIntVarWrapperListenerRva == 0xa624a0 && kIntVarWorkerListenerRva == 0xa62660);
    CHECK(kIntVarWrapperShowActiveRva == 0xa62860 && kIntVarWorkerShowActiveRva == 0xa62a20);
    CHECK(kIntVarWrapperSize == 0x1b3 && kIntVarWorkerSize == 0x15f && kIntVarDtorSize == 0x36);
    CHECK(kIntVarFinalVptrListener == 0x15d1fd8 && kIntVarWorkerVptrListener == 0x15d1ea0);
    CHECK(kIntVarFinalVptrShowActive == 0x15d20b0 && kIntVarWorkerVptrShowActive == 0x15d1f18);
    CHECK(kIntVarFnTableListener == 0x15d1fa8 && kIntVarFnTableShowActive == 0x15d2080);
    CHECK(kIntVarDtorListenerRva == 0xa5bea0 && kIntVarDtorShowActiveRva == 0xa5bee0);
    CHECK(kIntVarCtorFlags == 1);
    const IntVarShapeInfo& l = int_var_shape_info(IntVarShape::listener);
    const IntVarShapeInfo& s = int_var_shape_info(IntVarShape::show_active);
    CHECK(l.wrapper_rva == 0xa624a0 && l.worker_rva == 0xa62660 && l.final_vptr == 0x15d1fd8 &&
          l.worker_vptr == 0x15d1ea0 && l.fn_table == 0x15d1fa8 && l.dtor_rva == 0xa5bea0);
    CHECK(s.wrapper_rva == 0xa62860 && s.worker_rva == 0xa62a20 && s.final_vptr == 0x15d20b0 &&
          s.worker_vptr == 0x15d1f18 && s.fn_table == 0x15d2080 && s.dtor_rva == 0xa5bee0);
    CHECK(kSlot306IntVarSites.size() == 3);
    CHECK(kSlot306IntVarSites[0].global_rva == 0x32f8ff8 && kSlot306IntVarSites[1].global_rva == 0x32f90d0 &&
          kSlot306IntVarSites[2].global_rva == 0x32f9a78);
    CHECK(kSlot306IntVarSites[0].call_site_rva == 0xa63497 && kSlot306IntVarSites[1].call_site_rva == 0xa63567 &&
          kSlot306IntVarSites[2].call_site_rva == 0xa640c7);
    for (const auto& site : kSlot306IntVarSites) {
        CHECK(std::strlen(site.name) == site.name_len);
    }
}

// ------------------------------------------------------------------ 2. functor-table gate
void section_gate() {
    CHECK(int_var_fn_table_lifted(0x15d1fa8) && int_var_fn_table_lifted(0x15d2080));
    CHECK(!int_var_fn_table_lifted(0) && !int_var_fn_table_lifted(0x15d2040) &&
          !int_var_fn_table_lifted(0x1594318) && !int_var_fn_table_lifted(0x15d2060) &&
          !int_var_fn_table_lifted(0x15d1fa9));
}

// ------------------------------------------------------------------ 3. worker alone
void section_worker() {
    for (IntVarShape shape : {IntVarShape::listener, IntVarShape::show_active}) {
        const IntVarShapeInfo& info = int_var_shape_info(shape);
        ConCmdRangedVar var;
        poison(var);
        const SharedStr name = view("w", 1), desc = view("", 0);
        RangedVarArgs block = zero_args(info.fn_table);
        CHECK(init_int_var_worker(shape, var, name, -5, 77, 3, block, desc));
        CHECK(var.base.vptr == info.worker_vptr);            // NOT the final vptr: the wrapper restores it
        CHECK(var.base.registered == 0 && var.base.flags == 3);
        CHECK(var.base.name.data == name.data && var.base.description.data == desc.data);
        CHECK(var.value_ref == &var.value);
        CHECK(d(var, 0x6c) == static_cast<std::uint32_t>(-5) && var.min_engaged == 1);
        CHECK(d(var, 0x74) == 77u && var.max_engaged == 1);
        CHECK(var.args[0] == 0 && var.args[1] == 0 && var.args[2] == 0 && var.args[3] == 0);
        CHECK(var.fn_payload == 0 && var.fn_aux == 0 && var.fn_vptr == info.fn_table);
        // The wrapper-owned words +0x68 / +0xd0 and the pending link are not touched by the worker.
        check_untouched(var, {WORKER_WRITTEN});
        CHECK(d(var, 0x68) == 0xA5A5A5A5u && d(var, 0xd0) == 0xA5A5A5A5u);
    }

    // Argument block with q[3] != 0: the object word +0x80 becomes the object's own address +0x80.
    ConCmdRangedVar var;
    poison(var);
    RangedVarArgs block = zero_args(kIntVarFnTableListener);
    block.q[0] = 0x1111; block.q[1] = 0x2222; block.q[2] = 0x3333; block.q[3] = 0x4444;
    block.fn_payload = 0xAAAA; block.fn_aux = 0xBBBB;
    CHECK(init_int_var_worker(IntVarShape::listener, var, view("w", 1), 0, 1, 1, block, view("", 0)));
    CHECK(var.args[0] == reinterpret_cast<std::uint64_t>(&var.args[0]));
    CHECK(var.args[1] == 0x2222 && var.args[2] == 0x3333 && var.args[3] == 0x4444);
    CHECK(var.fn_payload == 0xAAAA && var.fn_aux == 0xBBBB && var.fn_vptr == kIntVarFnTableListener);
    check_untouched(var, {WORKER_WRITTEN});

    // q[3] == 0 keeps q[0] verbatim.
    poison(var);
    block.q[3] = 0;
    CHECK(init_int_var_worker(IntVarShape::show_active, var, view("w", 1), 0, 1, 1, block, view("", 0)));
    CHECK(var.args[0] == 0x1111 && var.args[3] == 0);

    // Unlifted table: nothing is written.
    poison(var);
    block = zero_args(0x15d2040);
    CHECK(!init_int_var_worker(IntVarShape::listener, var, view("w", 1), 0, 1, 1, block, view("", 0)));
    check_untouched(var, {});
}

// ------------------------------------------------------------------ 4. wrapper
void section_wrapper() {
    for (IntVarShape shape : {IntVarShape::listener, IntVarShape::show_active}) {
        const IntVarShapeInfo& info = int_var_shape_info(shape);
        ConCmdRegistry reg;
        ConCmdRangedVar var;
        poison(var);
        const SharedStr name = view("sndx_x", 6), desc = view("", 0);
        const IntVarCtorResult r = init_int_var(shape, var, name, 7, -3, 9, 1, zero_args(info.fn_table), desc, reg);
        CHECK(r.ok && !r.unlifted_functor && r.registration.ok);
        CHECK(r.registration.path == ConCmdRegisterPath::pending_list && r.registration.list_initialised);
        CHECK(r.installed_fn_table == info.fn_table);
        CHECK(var.base.vptr == info.final_vptr);
        CHECK(var.base.registered == 1 && var.base.flags == 1);
        CHECK(var.base.name.data == name.data && var.base.name.size == 6 && var.base.name.ctl == nullptr);
        CHECK(var.value_ref == &var.value);
        CHECK(d(var, 0x68) == 7u && d(var, 0xd0) == 7u);                     // default and value
        CHECK(d(var, 0x6c) == static_cast<std::uint32_t>(-3) && var.min_engaged == 1);
        CHECK(d(var, 0x74) == 9u && var.max_engaged == 1);
        CHECK(var.args[0] == 0 && var.args[1] == 0 && var.args[2] == 0 && var.args[3] == 0);
        CHECK(var.fn_payload == 0 && var.fn_aux == 0 && var.fn_vptr == info.fn_table);
        CHECK(reg.pending.size() == 1 && reg.pending.head.next == &var.base.pending);
        check_untouched(var, {WRAPPER_WRITTEN});
        CHECK(reg.atexit_log.size() == 0);    // the wrapper itself records no atexit; the call site does

        // Negative default: the value is the raw int32 bit pattern.
        ConCmdRangedVar neg;
        poison(neg);
        ConCmdRegistry reg2;
        CHECK(init_int_var(shape, neg, name, -1, -10, 10, 1, zero_args(info.fn_table), desc, reg2).ok);
        CHECK(d(neg, 0x68) == 0xFFFFFFFFu && d(neg, 0xd0) == 0xFFFFFFFFu);
    }

    // Arguments with live functor payload/aux and q[3] != 0 go through BOTH the wrapper's local copy and
    // the worker: payload and aux arrive intact, q[0] ends as the object's own +0x80.
    ConCmdRegistry reg;
    ConCmdRangedVar var;
    poison(var);
    RangedVarArgs args = zero_args(kIntVarFnTableListener);
    args.q[0] = 0x10; args.q[1] = 0x20; args.q[2] = 0x30; args.q[3] = 0x40;
    args.fn_payload = 0xDEAD0001; args.fn_aux = 0xDEAD0002;
    CHECK(init_int_var(IntVarShape::listener, var, view("p", 1), 0, 0, 1, 1, args, view("", 0), reg).ok);
    CHECK(var.args[0] == reinterpret_cast<std::uint64_t>(&var.args[0]));
    CHECK(var.args[1] == 0x20 && var.args[2] == 0x30 && var.args[3] == 0x40);
    CHECK(var.fn_payload == 0xDEAD0001 && var.fn_aux == 0xDEAD0002);
    // The caller's block is const: unchanged by construction.
    CHECK(args.q[0] == 0x10 && args.fn_payload == 0xDEAD0001);

    // q[3] == 0: q[0] is copied verbatim through both stages.
    poison(var);
    ConCmdRegistry reg3;
    args.q[3] = 0;
    CHECK(init_int_var(IntVarShape::show_active, var, view("p", 1), 0, 0, 2, 1, args,
                       view("", 0), reg3).ok);
    CHECK(var.args[0] == 0x10 && var.args[3] == 0);

    // The object's tag is the tag of the caller's argument functor (A), not the wrapper's seed (W):
    // the listener wrapper fed a show_active-table functor ends with 0x15d2080 at +0xc0.
    poison(var);
    ConCmdRegistry reg4;
    const IntVarCtorResult cross = init_int_var(IntVarShape::listener, var, view("x", 1), 0, 0, 1, 1,
                                                zero_args(kIntVarFnTableShowActive), view("", 0), reg4);
    CHECK(cross.ok && cross.installed_fn_table == kIntVarFnTableShowActive);
    CHECK(var.fn_vptr == kIntVarFnTableShowActive && var.base.vptr == kIntVarFinalVptrListener);

    // Control counters: the base constructor acquires each view once; nothing releases them here.
    std::int64_t ctl_name = 1, ctl_desc = 4;
    poison(var);
    ConCmdRegistry reg5;
    CHECK(init_int_var(IntVarShape::listener, var, view("c", 1, &ctl_name), 0, 0, 1, 1,
                       zero_args(kIntVarFnTableListener), view("", 0, &ctl_desc), reg5).ok);
    CHECK(ctl_name == 2 && ctl_desc == 5);
    CHECK(var.base.name.ctl == &ctl_name && var.base.description.ctl == &ctl_desc);

    // Registration order: three variables queue FIFO.
    ConCmdRegistry fifo;
    ConCmdRangedVar a, b, c;
    CHECK(init_int_var(IntVarShape::listener, a, view("a", 1), 0, 0, 1, 1, zero_args(kIntVarFnTableListener), view("", 0), fifo).ok);
    CHECK(init_int_var(IntVarShape::show_active, b, view("b", 1), 0, 0, 2, 1, zero_args(kIntVarFnTableShowActive), view("", 0), fifo).ok);
    CHECK(init_int_var(IntVarShape::listener, c, view("c", 1), 0, 0, 1, 1, zero_args(kIntVarFnTableListener), view("", 0), fifo).ok);
    CHECK(fifo.pending.size() == 3 && fifo.pending.head.next == &a.base.pending &&
          a.base.pending.next == &b.base.pending && b.base.pending.next == &c.base.pending &&
          c.base.pending.next == &fifo.pending.head);
}

// ------------------------------------------------------------------ 5. fail-closed
bool manager_insert_ok(void*, ConCmdBase&) noexcept { return true; }

void section_failclosed() {
    ConCmdRegistry reg;
    ConCmdRangedVar var;
    poison(var);
    const IntVarCtorResult bad = init_int_var(IntVarShape::listener, var, view("u", 1), 0, 0, 1, 1,
                                              zero_args(0x1594318), view("", 0), reg);
    CHECK(!bad.ok && bad.unlifted_functor && bad.installed_fn_table == 0);
    check_untouched(var, {});            // not a single byte written
    CHECK(reg.pending.size() == 0 && !reg.pending.guard_done);
    for (std::uint64_t table : {std::uint64_t{0}, std::uint64_t{0x15d2040}, std::uint64_t{0x15d2060},
                                std::uint64_t{0x15d5960}}) {
        poison(var);
        const IntVarCtorResult r = init_int_var(IntVarShape::show_active, var, view("u", 1), 0, 0, 1, 1,
                                                zero_args(table), view("", 0), reg);
        CHECK(!r.ok && r.unlifted_functor);
        check_untouched(var, {});
    }

    // A non-null manager without an insert contract is a visible failure; the object is fully built
    // (as in the original, the registration is the last step) and is NOT queued.
    ConCmdRegistry mgr;
    int token = 0;
    mgr.manager.manager = &token;
    poison(var);
    const IntVarCtorResult nm = init_int_var(IntVarShape::listener, var, view("m", 1), 0, 0, 1, 1,
                                             zero_args(kIntVarFnTableListener), view("", 0), mgr);
    CHECK(!nm.ok && !nm.registration.ok && nm.registration.path == ConCmdRegisterPath::manager);
    CHECK(var.base.registered == 1 && mgr.pending.size() == 0);
    // With the contract the manager path succeeds and the pending list stays empty.
    mgr.manager.insert = &manager_insert_ok;
    ConCmdRangedVar var2;
    poison(var2);
    const IntVarCtorResult ok = init_int_var(IntVarShape::listener, var2, view("m", 1), 0, 0, 1, 1,
                                             zero_args(kIntVarFnTableListener), view("", 0), mgr);
    CHECK(ok.ok && ok.registration.path == ConCmdRegisterPath::manager && mgr.pending.size() == 0);

    // Bad site index.
    ConCmdRegistry r2;
    poison(var);
    const IntVarSiteResult s = init_slot306_int_var(3, var, r2);
    CHECK(!s.ok && !s.atexit_recorded && r2.atexit_log.size() == 0);
    check_untouched(var, {});
}

// ------------------------------------------------------------------ 6. the three sites
void section_sites() {
    ConCmdRegistry reg;
    ConCmdRangedVar vars[3];
    for (auto& v : vars) poison(v);
    for (std::size_t i = 0; i < 3; ++i) {
        const IntVarSiteResult r = init_slot306_int_var(i, vars[i], reg);
        CHECK(r.ok && r.ctor.ok && r.arg_functor_destroyed && r.name_release_inert && r.atexit_recorded);
        const Slot306IntVarSite& site = kSlot306IntVarSites[i];
        const IntVarShapeInfo& info = int_var_shape_info(site.shape);
        const ConCmdRangedVar& v = vars[i];
        CHECK(v.base.vptr == info.final_vptr);
        CHECK(v.base.flags == 1 && v.base.registered == 1);
        CHECK(v.base.name.size == static_cast<std::int64_t>(site.name_len) && v.base.name.aux == 0 &&
              v.base.name.ctl == nullptr && std::memcmp(v.base.name.data, site.name, site.name_len) == 0);
        // Description is the shared empty string 0xf730: same data pointer, length 0, no counter.
        CHECK(v.base.description.data == empty_string_f730().data && v.base.description.size == 0 &&
              v.base.description.ctl == nullptr);
        CHECK(static_cast<std::int32_t>(d(v, 0x68)) == site.default_value &&
              static_cast<std::int32_t>(d(v, 0xd0)) == site.default_value);
        CHECK(static_cast<std::int32_t>(d(v, 0x6c)) == site.min_value && v.min_engaged == 1);
        CHECK(static_cast<std::int32_t>(d(v, 0x74)) == site.max_value && v.max_engaged == 1);
        CHECK(v.fn_vptr == info.fn_table && v.fn_payload == 0 && v.fn_aux == 0);
        CHECK(v.args[0] == 0 && v.args[1] == 0 && v.args[2] == 0 && v.args[3] == 0);
        check_untouched(v, {WRAPPER_WRITTEN});
    }
    // Exact observed values.
    CHECK(std::strcmp(kSlot306IntVarSites[0].name, "sndx_listenerOrientation") == 0 &&
          kSlot306IntVarSites[0].default_value == 0 && kSlot306IntVarSites[0].min_value == 0 &&
          kSlot306IntVarSites[0].max_value == 1);
    CHECK(std::strcmp(kSlot306IntVarSites[1].name, "sndx_listenerPosition") == 0 &&
          kSlot306IntVarSites[1].max_value == 1);
    CHECK(std::strcmp(kSlot306IntVarSites[2].name, "sndx_showActive") == 0 &&
          kSlot306IntVarSites[2].max_value == 2 &&
          kSlot306IntVarSites[2].shape == IntVarShape::show_active);
    // Pending list FIFO and atexit records in construction order.
    CHECK(reg.pending.size() == 3 && reg.pending.head.next == &vars[0].base.pending &&
          vars[0].base.pending.next == &vars[1].base.pending && vars[1].base.pending.next == &vars[2].base.pending);
    CHECK(reg.atexit_log.size() == 3);
    CHECK(reg.atexit_log.at(0).dtor_rva == 0xa5bea0 && reg.atexit_log.at(0).object == &vars[0]);
    CHECK(reg.atexit_log.at(1).dtor_rva == 0xa5bea0 && reg.atexit_log.at(1).object == &vars[1]);
    CHECK(reg.atexit_log.at(2).dtor_rva == 0xa5bee0 && reg.atexit_log.at(2).object == &vars[2]);
}

// ------------------------------------------------------------------ 7. menu target (0x26ea0 slots 3 / 6)
void section_menu_target() {
    ConCmdRegistry reg;
    ConCmdRangedVar vars[3];
    for (std::size_t i = 0; i < 3; ++i) CHECK(init_slot306_int_var(i, vars[i], reg).ok);
    for (const auto& v : vars) {
        const DebugMenuTarget t = int_var_menu_target(v);
        CHECK(t.known && t.object == &v && t.slot3 == 1 && t.slot6 == 1);
        DebugMenuItem item;
        poison(item);
        CHECK(init_debug_menu_item_26ea0(item, view("Sound", 5), view("Listener Position", 17), t, 0x30));
        // slot 6 is 1, not 3: the {0.1, 0.5} narrowing is NOT taken, the default range is kept.
        CHECK(item.range_min == 1.0f && item.range_max == 10.0f);
        CHECK(item.target == &v && item.flags == 0x30);
    }
    // A vptr that is not one of the two final classes is unknown, and 0x26ea0 refuses it.
    ConCmdRangedVar other;
    poison(other);
    other.base.vptr = kIntVarWorkerVptrListener;
    const DebugMenuTarget unknown = int_var_menu_target(other);
    CHECK(!unknown.known);
    DebugMenuItem item;
    CHECK(!init_debug_menu_item_26ea0(item, view("a", 1), view("b", 1), unknown, 0x30));
}

}  // namespace

int main() {
    section_constants();
    section_gate();
    section_worker();
    section_wrapper();
    section_failclosed();
    section_sites();
    section_menu_target();
    std::puts("OK test_intvar_a624a0");
    return 0;
}
