// CPU-01-CONCMD-298-299-301-01: fixture for the shared ConCmd base / registry, the bool and ranged
// console-variable shapes, the debug-menu item, and startup constructor indices 298 (0xb0e250),
// 299 (0xb0c7a0) and 301 (0xac9550).
//
// Sections:
//   1. layout constants and static offsets
//   2. ConCmd base constructor (0x20af0): exact stores, untouched bytes, control-counter acquire
//   3. register (0x20d60/0x20d80): pending-list order, lazy head, idempotence, manager contract
//   4. bool variable shape (0xac9550 body): every written/untouched byte, failure visibility
//   5. slot 301 (rumbleEnabled)
//   6. ranged variable constructors: enum 0xb0e060 and float 0xc300, args/functor install
//   7. slot 298 (shaderTypeToLoad), including the resolved indirect call
//   8. debug-menu items (0x27050 / 0x26ea0) and registration (0x268b0)
//   9. slot 299 (debug text scale): four objects, atexit order, lists
//  10. composition: 298 + 299 + 301 in one registry, then the production StartupState rows
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <utility>

#include "core/concmd_base.hpp"
#include "core/concmd_vars.hpp"
#include "core/ctor_original_table.hpp"
#include "core/ctor_slot_298_b0e250.hpp"
#include "core/ctor_slot_299_b0c7a0.hpp"
#include "core/ctor_slot_301_ac9550.hpp"
#include "core/debug_menu_item.hpp"
#include "core/startup_ctors.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;

template <class T>
void poison(T& obj) { std::memset(static_cast<void*>(&obj), kPoison, sizeof obj); }

// Every byte of `obj` outside the listed [offset, offset+size) ranges must still be poison.
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
std::uint64_t q(const T& obj, std::size_t off) {
    std::uint64_t v;
    std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(&obj) + off, 8);
    return v;
}
template <class T>
std::uint32_t d(const T& obj, std::size_t off) {
    std::uint32_t v;
    std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(&obj) + off, 4);
    return v;
}
template <class T>
std::uint8_t b(const T& obj, std::size_t off) { return reinterpret_cast<const std::uint8_t*>(&obj)[off]; }

SharedStr view(const char* text, std::size_t len, std::int64_t* ctl = nullptr) {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(len), 0, ctl};
}

bool same_view(const SharedStr& v, const char* text, std::size_t len) {
    return v.data != nullptr && v.size == static_cast<std::int64_t>(len) && v.aux == 0 &&
           std::memcmp(v.data, text, len) == 0 && v.ctl == nullptr;
}

// ------------------------------------------------------------------ 1. layout
void section_layout() {
    static_assert(sizeof(ConCmdBase) == 0x60);
    static_assert(sizeof(ConCmdListNode) == 0x10);
    static_assert(offsetof(ConCmdBoolVar, value) == 0xc0);
    static_assert(offsetof(ConCmdRangedVar, value) == 0xd0);
    static_assert(offsetof(DebugMenuItem, flags) == 0xa0);
    CHECK(kConCmdBaseCtorRva == 0x20af0 && kConCmdRegisterRva == 0x20d60 && kConCmdRegisterBodyRva == 0x20d80);
    CHECK(kConCmdManagerCellRva == 0x2dfcbc8 && kConCmdPendingHeadRva == 0x2dfce50);
    CHECK(kConCmdManagerZeroIndex == 567 && kConCmdManagerCtorRva == 0x210b0 && kConCmdManagerInstallRva == 0x243f5);
    CHECK(kSlot298Rva == 0xb0e250 && kSlot298BodySize == 331);
    CHECK(kSlot299Rva == 0xb0c7a0 && kSlot299BodySize == 926);
    CHECK(kSlot301Rva == 0xac9550 && kSlot301BodySize == 323);
    CHECK(kRangedEnumFnDestroyRva == 0xb0e240 && kRangedFloatFnDestroyRva == 0xc4f0);
    CHECK(sizeof kSlot298Name - 1 == 16 && sizeof kSlot301Name - 1 == 13);
    CHECK(sizeof kSlot299ScaleName - 1 == 0x14 && sizeof kSlot299AdjustName - 1 == 0x1a);
    CHECK(sizeof kSlot299Label - 1 == 5 && sizeof kSlot299ScaleText - 1 == 0x17 &&
          sizeof kSlot299AdjustText - 1 == 0x25);
}

// ------------------------------------------------------------------ 2. base constructor
void section_base() {
    ConCmdBase cmd;
    poison(cmd);
    const SharedStr name = view("alpha", 5);
    const SharedStr desc = view("", 0);
    init_concmd_base_20af0(cmd, name, desc, 7);
    CHECK(cmd.vptr == kConCmdBaseVptr);
    CHECK(cmd.registered == 0 && cmd.flags == 7);
    CHECK(cmd.name.data == name.data && cmd.name.size == 5 && cmd.name.ctl == nullptr);
    CHECK(cmd.description.data == desc.data && cmd.description.size == 0);
    // +0x08..+0x17 (list link) and +0x19..+0x1b are not written by the constructor.
    check_untouched(cmd, {{0x00, 8}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}});

    // Control counters: both views are acquired with `lock inc`, exactly once each.
    std::int64_t ctl_name = 1, ctl_desc = 5;
    SharedStr n2 = view("beta", 4, &ctl_name), d2 = view("gamma", 5, &ctl_desc);
    ConCmdBase c2;
    poison(c2);
    init_concmd_base_20af0(c2, n2, d2, 1);
    CHECK(ctl_name == 2 && ctl_desc == 6);
    CHECK(c2.name.ctl == &ctl_name && c2.description.ctl == &ctl_desc);
    // A shared counter used for both views is incremented twice.
    std::int64_t shared = 10;
    ConCmdBase c3;
    init_concmd_base_20af0(c3, view("x", 1, &shared), view("y", 1, &shared), 1);
    CHECK(shared == 12);

    // The stack-view release is inert only without a control counter.
    CHECK(release_inert_view_f590(name));
    std::int64_t live = 3;
    CHECK(!release_inert_view_f590(view("z", 1, &live)));
    CHECK(live == 2);   // the lifted release really decremented: the model refuses to call it inert
}

// ------------------------------------------------------------------ 3. register
bool g_manager_insert_calls = false;
ConCmdBase* g_manager_last = nullptr;
bool manager_insert(void*, ConCmdBase& cmd) noexcept { g_manager_insert_calls = true; g_manager_last = &cmd; return true; }
bool manager_insert_fail(void*, ConCmdBase&) noexcept { return false; }

void section_register() {
    ConCmdRegistry reg;
    ConCmdBase a, b, c;
    poison(a); poison(b); poison(c);
    init_concmd_base_20af0(a, view("a", 1), view("", 0), 1);
    init_concmd_base_20af0(b, view("b", 1), view("", 0), 1);
    init_concmd_base_20af0(c, view("c", 1), view("", 0), 1);

    CHECK(reg.pending.size() == 0 && !reg.pending.guard_done && reg.pending.head.next == nullptr);

    ConCmdRegisterResult r = register_concmd_20d60(a, reg);
    CHECK(r.ok && r.path == ConCmdRegisterPath::pending_list && r.list_initialised);
    CHECK(a.registered == 1 && reg.pending.guard_done);
    CHECK(reg.pending.head.next == &a.pending && reg.pending.head.prev == &a.pending);
    CHECK(a.pending.next == &reg.pending.head && a.pending.prev == &reg.pending.head);
    CHECK(reg.pending.size() == 1);

    r = register_concmd_20d60(b, reg);
    CHECK(r.ok && r.path == ConCmdRegisterPath::pending_list && !r.list_initialised);
    r = register_concmd_20d60(c, reg);
    CHECK(r.ok && !r.list_initialised);
    CHECK(reg.pending.size() == 3);
    // FIFO: head -> a -> b -> c -> head, and the back links mirror it.
    CHECK(reg.pending.head.next == &a.pending && a.pending.next == &b.pending &&
          b.pending.next == &c.pending && c.pending.next == &reg.pending.head);
    CHECK(reg.pending.head.prev == &c.pending && c.pending.prev == &b.pending &&
          b.pending.prev == &a.pending && a.pending.prev == &reg.pending.head);

    // Second registration of the same object: the register-once flag short-circuits everything.
    r = register_concmd_20d60(b, reg);
    CHECK(r.ok && r.path == ConCmdRegisterPath::already_registered && !r.list_initialised);
    CHECK(reg.pending.size() == 3);

    // Non-null manager without an insert contract: visible failure, flag already set (as 0x20d60).
    ConCmdRegistry mgr;
    int manager_token = 0;
    mgr.manager.manager = &manager_token;
    ConCmdBase m;
    poison(m);
    init_concmd_base_20af0(m, view("m", 1), view("", 0), 1);
    r = register_concmd_20d60(m, mgr);
    CHECK(!r.ok && r.path == ConCmdRegisterPath::manager && m.registered == 1);
    CHECK(mgr.pending.size() == 0 && !mgr.pending.guard_done);   // the pending list is untouched

    // With a contract the hook runs and the pending list is still untouched.
    ConCmdBase m2;
    init_concmd_base_20af0(m2, view("m2", 2), view("", 0), 1);
    mgr.manager.insert = &manager_insert;
    r = register_concmd_20d60(m2, mgr);
    CHECK(r.ok && r.path == ConCmdRegisterPath::manager && g_manager_insert_calls && g_manager_last == &m2);
    CHECK(mgr.pending.size() == 0);

    ConCmdBase m3;
    init_concmd_base_20af0(m3, view("m3", 2), view("", 0), 1);
    mgr.manager.insert = &manager_insert_fail;
    r = register_concmd_20d60(m3, mgr);
    CHECK(!r.ok && r.path == ConCmdRegisterPath::manager);

    // The atexit log records in order and stays readable.
    StartupAtexitLog log;
    CHECK(log.size() == 0);
    CHECK(log.record(0x20b0, &a) && log.record(0xb0cb40, &b));
    CHECK(log.size() == 2 && log.at(0).dtor_rva == 0x20b0 && log.at(0).object == &a &&
          log.at(1).dtor_rva == 0xb0cb40 && log.at(1).object == &b);
}

// ------------------------------------------------------------------ 4. bool variable shape
void check_bool_var_written(const ConCmdBoolVar& v, std::uint64_t fn_tag) {
    CHECK(v.base.vptr == kBoolVarFinalVptr);
    CHECK(v.value_ref == &v.value);
    CHECK(v.default_value == 1 && v.value == 1);
    for (std::uint8_t byte : v.zero69) CHECK(byte == 0);
    for (std::uint8_t byte : v.zero70) CHECK(byte == 0);
    CHECK(v.fn_payload == 0 && v.fn_aux == 0 && v.fn_vptr == fn_tag);
    CHECK(v.base.registered == 1 && v.base.flags == 1);
    check_untouched(v, {{0x00, 8}, {0x08, 0x10}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 8},
                        {0x68, 1}, {0x69, 4}, {0x70, 0x20}, {0xa0, 8}, {0xb0, 8}, {0xb8, 8}, {0xc0, 1}});
}

void section_bool_var() {
    ConCmdRegistry reg;
    ConCmdBoolVar v;
    poison(v);
    const SharedStr name = view("sample", 6);
    const SharedStr desc = view("", 0);
    BoolVarResult r = init_bool_var_static(v, name, desc, 0x15d5070, reg);
    CHECK(r.ok && r.views_inert && r.atexit_recorded);
    CHECK(r.registration.ok && r.registration.path == ConCmdRegisterPath::pending_list);
    check_bool_var_written(v, 0x15d5070);
    CHECK(same_view(v.base.name, "sample", 6));
    CHECK(reg.pending.size() == 1 && reg.pending.first() == &v.base.pending);
    CHECK(reg.atexit_log.size() == 1 && reg.atexit_log.at(0).dtor_rva == 0x20b0 &&
          reg.atexit_log.at(0).object == &v);

    // The second observed site only changes the functor tag.
    ConCmdBoolVar w;
    poison(w);
    r = init_bool_var_static(w, view("other", 5), desc, 0x15d5800, reg);
    CHECK(r.ok);
    check_bool_var_written(w, 0x15d5800);
    CHECK(reg.pending.size() == 2 && reg.atexit_log.size() == 2);

    // A control counter on a stack view is a visible failure of the inert-release claim.
    ConCmdRegistry reg2;
    ConCmdBoolVar x;
    poison(x);
    std::int64_t ctl = 2;
    r = init_bool_var_static(x, view("counted", 7, &ctl), desc, 0x15d5070, reg2);
    CHECK(!r.ok && !r.views_inert);
    CHECK(ctl == 2);      // acquired by the base ctor (+1) and released by the model (-1)

    // A manager without a contract makes the whole shape fail visibly, after the stores.
    ConCmdRegistry reg3;
    int token = 0;
    reg3.manager.manager = &token;
    ConCmdBoolVar y;
    poison(y);
    r = init_bool_var_static(y, view("mgr", 3), desc, 0x15d5070, reg3);
    CHECK(!r.ok && !r.registration.ok && r.registration.path == ConCmdRegisterPath::manager);
    CHECK(r.atexit_recorded);   // the later stages still ran, as in the original
}

// ------------------------------------------------------------------ 5. slot 301
void section_slot_301() {
    ConCmdRegistry reg;
    Slot301Object obj;
    poison(obj);
    const Slot301Result r = init_slot301_ac9550(obj, reg);
    CHECK(r.ok);
    check_bool_var_written(obj, kSlot301FnVtbl);
    CHECK(kSlot301FnVtbl == 0x15d5070);
    CHECK(same_view(obj.base.name, "rumbleEnabled", 13));
    CHECK(obj.base.description.size == 0 && obj.base.description.data != nullptr &&
          obj.base.description.data[0] == 0);
    CHECK(reg.pending.size() == 1);
    CHECK(reg.atexit_log.size() == 1 && reg.atexit_log.at(0).dtor_rva == kBoolVarDtorRva);
}

// ------------------------------------------------------------------ 6. ranged variables
RangedVarArgs zero_args(std::uint64_t vtbl) {
    RangedVarArgs a{};
    a.fn_vptr = vtbl;
    return a;
}

void section_ranged_vars() {
    const SharedStr name = view("enum", 4);
    const SharedStr desc = view("", 0);
    std::uint32_t cell = 0xdeadbeef;

    // ---- enum flavour (0xb0e060)
    ConCmdRangedVar e;
    poison(e);
    RangedVarArgs args = zero_args(kRangedEnumFnVtbl);
    CHECK(init_ranged_var_b0e060(e, name, &cell, 1, args, desc));
    CHECK(e.base.vptr == kRangedEnumMidVptr);
    CHECK(e.value_ref == &cell);
    CHECK(e.min_value[0] == 0 && e.min_engaged == 0 && e.max_value[0] == 0 && e.max_engaged == 0);
    for (std::uint64_t arg : e.args) CHECK(arg == 0);
    CHECK(e.fn_payload == 0 && e.fn_aux == 0 && e.fn_vptr == kRangedEnumFnVtbl);
    check_untouched(e, {{0x00, 8}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 8}, {0x6c, 1}, {0x70, 1},
                        {0x74, 1}, {0x78, 1}, {0x80, 0x20}, {0xb0, 8}, {0xc0, 8}, {0xc8, 8}});

    // Payload and aux propagate from the argument functor; a non-zero q[3] makes q[0] self-referential.
    ConCmdRangedVar f;
    poison(f);
    args = zero_args(kRangedEnumFnVtbl);
    args.q[0] = 0x11; args.q[1] = 0x22; args.q[2] = 0x33; args.q[3] = 0x44;
    args.fn_payload = 0xabcdef; args.fn_aux = 0x1234;
    CHECK(init_ranged_var_b0e060(f, name, &cell, 5, args, desc));
    CHECK(f.base.flags == 5);
    CHECK(f.args[0] == reinterpret_cast<std::uint64_t>(&f.args[0]));
    CHECK(f.args[1] == 0x22 && f.args[2] == 0x33 && f.args[3] == 0x44);
    CHECK(f.fn_payload == 0xabcdef && f.fn_aux == 0x1234 && f.fn_vptr == kRangedEnumFnVtbl);
    // q[3] == 0 keeps q[0] as given.
    ConCmdRangedVar g;
    args = zero_args(kRangedEnumFnVtbl);
    args.q[0] = 0x77;
    CHECK(init_ranged_var_b0e060(g, name, &cell, 1, args, desc));
    CHECK(g.args[0] == 0x77);

    // An unlifted functor vtable writes nothing.
    ConCmdRangedVar h;
    poison(h);
    CHECK(!init_ranged_var_b0e060(h, name, &cell, 1, zero_args(0x1234560), desc));
    check_untouched(h, {});

    // ---- float flavour (0xc300)
    std::uint32_t fcell = 0;
    ConCmdRangedVar fl;
    poison(fl);
    CHECK(init_ranged_float_var_c300(fl, view("float", 5), &fcell, 0.1f, 10.0f, 1,
                                     zero_args(kRangedFloatArgFnVtbl), desc));
    CHECK(fl.base.vptr == kRangedFloatMidVptr);
    CHECK(d(fl, 0x6c) == 0x3dcccccdu && d(fl, 0x74) == 0x41200000u);
    CHECK(fl.min_engaged == 1 && fl.max_engaged == 1);
    CHECK(fl.fn_vptr == kRangedFloatFnVtbl && fl.fn_payload == 0 && fl.fn_aux == 0);
    check_untouched(fl, {{0x00, 8}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 8}, {0x6c, 4}, {0x70, 1},
                         {0x74, 4}, {0x78, 1}, {0x80, 0x20}, {0xb0, 8}, {0xc0, 8}, {0xc8, 8}});
    // The class vtable is also accepted as the source vtable (identical slot bodies).
    ConCmdRangedVar fl2;
    CHECK(init_ranged_float_var_c300(fl2, view("float", 5), &fcell, 1.0f, 2.0f, 1,
                                     zero_args(kRangedFloatFnVtbl), desc));
    // An unlifted functor writes nothing.
    ConCmdRangedVar fl3;
    poison(fl3);
    CHECK(!init_ranged_float_var_c300(fl3, view("float", 5), &fcell, 1.0f, 2.0f, 1, zero_args(0x42), desc));
    check_untouched(fl3, {});
}

// ------------------------------------------------------------------ 7. slot 298
void section_slot_298() {
    ConCmdRegistry reg;
    Slot298Object obj;
    poison(obj);
    const Slot298Result r = init_slot298_b0e250(obj, reg);
    CHECK(r.ok && r.ctor_ok && r.functor_destroy_noop && r.views_inert && r.atexit_recorded);
    CHECK(r.registration.ok && r.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(obj.base.vptr == kSlot298FinalVptr && kSlot298FinalVptr == 0x15d58f8);
    CHECK(same_view(obj.base.name, "shaderTypeToLoad", 16));
    CHECK(obj.base.description.size == 0 && obj.base.flags == 1 && obj.base.registered == 1);
    CHECK(obj.value_ref == &obj.value);
    CHECK(obj.value == 0 && obj.default_value == 0);
    CHECK(obj.min_engaged == 0 && obj.max_engaged == 0 && obj.min_value[0] == 0 && obj.max_value[0] == 0);
    for (std::uint64_t arg : obj.args) CHECK(arg == 0);
    CHECK(obj.fn_payload == 0 && obj.fn_aux == 0 && obj.fn_vptr == 0x15d5960);
    check_untouched(obj, {{0x00, 8}, {0x08, 0x10}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 8},
                          {0x68, 4}, {0x6c, 1}, {0x70, 1}, {0x74, 1}, {0x78, 1}, {0x80, 0x20},
                          {0xb0, 8}, {0xc0, 8}, {0xc8, 8}, {0xd0, 4}});
    CHECK(reg.pending.size() == 1 && reg.pending.first() == &obj.base.pending);
    CHECK(reg.atexit_log.size() == 1 && reg.atexit_log.at(0).dtor_rva == 0xb0cb40 &&
          reg.atexit_log.at(0).object == &obj);

    // A manager without an insert contract is a visible failure, not a silent success.
    ConCmdRegistry bad;
    int token = 0;
    bad.manager.manager = &token;
    Slot298Object o2;
    const Slot298Result r2 = init_slot298_b0e250(o2, bad);
    CHECK(!r2.ok && !r2.registration.ok && r2.atexit_recorded);
}

// ------------------------------------------------------------------ 8. debug-menu items
bool parent_insert_called = false;
bool parent_insert(void*, DebugMenuItem&) noexcept { parent_insert_called = true; return true; }

void section_debug_menu() {
    const SharedStr label = view("Debug", 5);
    const SharedStr text = view("Some Text", 9);
    ConCmdBoolVar target;
    poison(target);
    ConCmdRegistry creg;
    CHECK(init_bool_var_static(target, view("t", 1), view("", 0), 0x15d5800, creg).ok);

    // 0x27050: explicit range.
    DebugMenuItem a;
    poison(a);
    CHECK(init_debug_menu_item_27050(a, label, text, &target, 0, 0.1f, 1.0f));
    CHECK(a.link.self == nullptr && a.link.prev == &a.link && a.link.next == &a.link && a.link.owner == nullptr);
    CHECK(a.registered == 0 && a.kind == 2);
    CHECK(same_view(a.label, "Debug", 5) && same_view(a.text, "Some Text", 9));
    CHECK(a.empty.data != nullptr && a.empty.size == 0 && a.empty.aux == 0 && a.empty.ctl == nullptr);
    CHECK(a.target == &target && a.flags == 0);
    CHECK(d(a, 0x98) == 0x3dcccccdu && d(a, 0x9c) == 0x3f800000u);
    check_untouched(a, {{0x00, 0x20}, {0x20, 1}, {0x28, 0x20}, {0x48, 4}, {0x50, 0x20}, {0x70, 0x20},
                        {0x90, 8}, {0x98, 4}, {0x9c, 4}, {0xa0, 4}});

    // 0x26ea0: default range, known bool target -> slot results {1, 0}: no override.
    DebugMenuItem bi;
    poison(bi);
    const DebugMenuTarget t = debug_menu_target_for(target);
    CHECK(t.known && t.slot3 == 1 && t.slot6 == 0 && t.object == &target);
    CHECK(init_debug_menu_item_26ea0(bi, label, text, t, 0x30));
    CHECK(d(bi, 0x98) == 0x3f800000u && d(bi, 0x9c) == 0x41200000u && bi.flags == 0x30 && bi.kind == 2);
    // The override branch needs slot3 == 1 AND slot6 == 3 (synthetic target, not an observed class).
    DebugMenuTarget synth = t;
    synth.slot6 = 3;
    DebugMenuItem so;
    poison(so);
    CHECK(init_debug_menu_item_26ea0(so, label, text, synth, 0));
    CHECK(q(so, 0x98) == 0x3f0000003dcccccdull);
    synth.slot3 = 2;
    CHECK(init_debug_menu_item_26ea0(so, label, text, synth, 0));
    CHECK(d(so, 0x98) == 0x3f800000u);
    // An unknown class (foreign vptr) writes nothing.
    ConCmdBoolVar alien;
    poison(alien);
    alien.base.vptr = 0x1234;
    const DebugMenuTarget unknown = debug_menu_target_for(alien);
    CHECK(!unknown.known);
    DebugMenuItem nothing;
    poison(nothing);
    CHECK(!init_debug_menu_item_26ea0(nothing, label, text, unknown, 0));
    check_untouched(nothing, {});

    // Control counters on label/text are acquired.
    std::int64_t lc = 1, tc = 1;
    DebugMenuItem cnt;
    CHECK(init_debug_menu_item_27050(cnt, view("L", 1, &lc), view("T", 1, &tc), nullptr, 0, 0.0f, 1.0f));
    CHECK(lc == 2 && tc == 2);

    // ---- registration with a null parent
    StartupAtexitLog log;
    DebugMenuRegistry menu;
    DebugMenuRegisterResult r = register_debug_menu_item_268b0(a, menu, log);
    CHECK(r.ok && r.path == DebugMenuRegisterPath::root_list && r.root_initialised && r.atexit_recorded);
    CHECK(menu.root_guard_done && menu.root.count == 1);
    CHECK(a.link.self == &a.link && a.link.next == &menu.root.link && a.link.prev == &menu.root.link &&
          a.link.owner == &menu.root.link);
    CHECK(menu.root.link.prev == &a.link && menu.root.link.next == &a.link && menu.root.link.self == nullptr &&
          menu.root.link.owner == nullptr);
    CHECK(log.size() == 1 && log.at(0).dtor_rva == 0x27280 && log.at(0).object == &menu.root);
    CHECK(a.registered == 0);   // the null-parent path never sets +0x20 (only the parent path does)

    r = register_debug_menu_item_268b0(bi, menu, log);
    CHECK(r.ok && r.path == DebugMenuRegisterPath::root_list && !r.root_initialised && !r.atexit_recorded);
    CHECK(menu.root.count == 2 && log.size() == 1);
    CHECK(menu.root.link.next == &a.link && a.link.next == &bi.link && bi.link.next == &menu.root.link);
    CHECK(menu.root.link.prev == &bi.link && bi.link.prev == &a.link && a.link.prev == &menu.root.link);
    CHECK(bi.link.self == &bi.link && bi.link.owner == &menu.root.link);

    // ---- assert branches are visible failures, not silent skips
    DebugMenuItem as;
    CHECK(init_debug_menu_item_27050(as, label, text, &target, 0x40, 0.0f, 1.0f));
    r = register_debug_menu_item_268b0(as, menu, log);
    CHECK(!r.ok && r.path == DebugMenuRegisterPath::unmodelled_assert && menu.root.count == 2);
    as.flags = 0x100;
    r = register_debug_menu_item_268b0(as, menu, log);
    CHECK(!r.ok && r.path == DebugMenuRegisterPath::unmodelled_assert);
    as.flags = 0;
    as.kind = 1;
    r = register_debug_menu_item_268b0(as, menu, log);
    CHECK(!r.ok && r.path == DebugMenuRegisterPath::unmodelled_assert);
    as.kind = 0;          // kind 0 skips the flag test at 0x26962
    as.flags = 0x100;
    r = register_debug_menu_item_268b0(as, menu, log);
    CHECK(r.ok && r.path == DebugMenuRegisterPath::root_list && menu.root.count == 3);

    // ---- non-null parent: contract
    DebugMenuRegistry pmenu;
    int parent_token = 0;
    pmenu.parent.parent = &parent_token;
    StartupAtexitLog plog;
    DebugMenuItem pi;
    CHECK(init_debug_menu_item_27050(pi, label, text, &target, 0, 0.0f, 1.0f));
    r = register_debug_menu_item_268b0(pi, pmenu, plog);
    CHECK(!r.ok && r.path == DebugMenuRegisterPath::parent && pi.registered == 0);
    CHECK(!pmenu.root_guard_done && plog.size() == 0);
    pmenu.parent.insert = &parent_insert;
    r = register_debug_menu_item_268b0(pi, pmenu, plog);
    CHECK(r.ok && r.path == DebugMenuRegisterPath::parent && parent_insert_called && pi.registered == 1);
    r = register_debug_menu_item_268b0(pi, pmenu, plog);
    CHECK(r.ok && r.path == DebugMenuRegisterPath::already_registered);
}

// ------------------------------------------------------------------ 9. slot 299
void section_slot_299() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot299Objects o;
    poison(o);
    const Slot299Result r = init_slot299_b0c7a0(o, creg, menu);
    CHECK(r.ok);
    CHECK(r.scale_ctor_ok && r.scale_functor_destroy_noop && r.scale_registration.ok && r.scale_views_inert &&
          r.scale_atexit_recorded && r.adjust.ok);
    CHECK(r.scale_item_ctor_ok && r.scale_item_registration.ok && r.scale_item_views_inert &&
          r.scale_item_atexit_recorded && r.adjust_item_ctor_ok && r.adjust_item_registration.ok &&
          r.adjust_item_views_inert && r.adjust_item_atexit_recorded);

    // A: float variable.
    const ConCmdRangedVar& a = o.scale;
    CHECK(a.base.vptr == kRangedFloatFinalVptr && kRangedFloatFinalVptr == 0x15942b0);
    CHECK(same_view(a.base.name, "globalDebugTextScale", 0x14));
    CHECK(a.base.description.size == 0 && a.base.flags == 1 && a.base.registered == 1);
    CHECK(a.value_ref == &a.value && a.value == 0x3f800000u && a.default_value == 0x3f800000u);
    CHECK(d(a, 0x6c) == 0x3dcccccdu && d(a, 0x74) == 0x41200000u && a.min_engaged == 1 && a.max_engaged == 1);
    CHECK(a.fn_vptr == 0x1594318 && a.fn_payload == 0 && a.fn_aux == 0);
    check_untouched(a, {{0x00, 8}, {0x08, 0x10}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 8}, {0x68, 4},
                        {0x6c, 4}, {0x70, 1}, {0x74, 4}, {0x78, 1}, {0x80, 0x20}, {0xb0, 8}, {0xc0, 8},
                        {0xc8, 8}, {0xd0, 4}});

    // B: bool variable (second observed site of the shape).
    CHECK(same_view(o.adjust.base.name, "globalDebugTextAdjustFor4K", 0x1a));
    check_bool_var_written(o.adjust, 0x15d5800);

    // C: the slider item targets A with {0.1, 1.0}; D: the toggle item targets B with {1.0, 10.0}.
    const DebugMenuItem& c = o.scale_item;
    CHECK(c.target == &a && c.flags == 0 && c.kind == 2);
    CHECK(d(c, 0x98) == 0x3dcccccdu && d(c, 0x9c) == 0x3f800000u);
    CHECK(same_view(c.label, "Debug", 5) && same_view(c.text, "Global Debug Text Scale", 0x17));
    const DebugMenuItem& dd = o.adjust_item;
    CHECK(dd.target == &o.adjust && dd.flags == 0x30);
    CHECK(d(dd, 0x98) == 0x3f800000u && d(dd, 0x9c) == 0x41200000u);
    CHECK(same_view(dd.label, "Debug", 5) && same_view(dd.text, "Global Debug Text Scale Adjust for 4k", 0x25));

    // Lists: ConCmd pending = A then B; debug-menu root = C then D.
    CHECK(creg.pending.size() == 2);
    CHECK(creg.pending.head.next == &a.base.pending && a.base.pending.next == &o.adjust.base.pending &&
          o.adjust.base.pending.next == &creg.pending.head);
    CHECK(menu.root.count == 2 && menu.root.link.next == &o.scale_item.link &&
          o.scale_item.link.next == &o.adjust_item.link && o.adjust_item.link.next == &menu.root.link);

    // atexit order: A, B, root (first menu registration), C, D.
    CHECK(creg.atexit_log.size() == 5);
    const std::uint64_t expect_rva[5] = {0x20f0, 0x20b0, 0x27280, 0x272d0, 0x272d0};
    const void* expect_obj[5] = {&a, &o.adjust, &menu.root, &o.scale_item, &o.adjust_item};
    for (std::size_t i = 0; i < 5; ++i) {
        CHECK(creg.atexit_log.at(i).dtor_rva == expect_rva[i]);
        CHECK(creg.atexit_log.at(i).object == expect_obj[i]);
    }

    // A non-null debug-menu parent without a contract fails the slot visibly.
    ConCmdRegistry c2;
    DebugMenuRegistry m2;
    int token = 0;
    m2.parent.parent = &token;
    Slot299Objects o2;
    const Slot299Result r2 = init_slot299_b0c7a0(o2, c2, m2);
    CHECK(!r2.ok && !r2.scale_item_registration.ok);
}

// ------------------------------------------------------------------ 10. composition
void section_composition() {
    // 298, 299 and 301 share one ConCmd registry; the pending list keeps index order.
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot298Object o298;
    Slot299Objects o299;
    Slot301Object o301;
    CHECK(init_slot298_b0e250(o298, creg).ok);
    CHECK(init_slot299_b0c7a0(o299, creg, menu).ok);
    CHECK(init_slot301_ac9550(o301, creg).ok);
    CHECK(creg.pending.size() == 4);
    const ConCmdListNode* n = creg.pending.first();
    CHECK(n == &o298.base.pending); n = n->next;
    CHECK(n == &o299.scale.base.pending); n = n->next;
    CHECK(n == &o299.adjust.base.pending); n = n->next;
    CHECK(n == &o301.base.pending); n = n->next;
    CHECK(n == &creg.pending.head);

    // Production rows: 298, 299 and 301 are registered, in index order, and run clean.
    const auto slots = recovered_constructors();
    bool have298 = false, have299 = false, have301 = false;
    for (const CtorSlot& s : slots) {
        if (s.original_index == 298) { have298 = true; CHECK(s.original_rva == 0xb0e250); }
        if (s.original_index == 299) { have299 = true; CHECK(s.original_rva == 0xb0c7a0); }
        if (s.original_index == 301) { have301 = true; CHECK(s.original_rva == 0xac9550); }
    }
    CHECK(have298 && have299 && have301);
    auto* st = new StartupState();
    for (const CtorSlot& s : slots) {
        if (s.original_index == 298 || s.original_index == 299 || s.original_index == 301) s.run(*st);
    }
    CHECK(!st->init_failed);
    CHECK(st->concmd.pending.size() == 4);
    CHECK(st->concmd.pending.first() == &st->slot298_shader_type.base.pending);
    CHECK(st->slot301_rumble.base.registered == 1 && st->slot298_shader_type.base.registered == 1);
    CHECK(st->debug_menu.root.count == 2);
    CHECK(st->concmd.atexit_log.size() == 1 + 5 + 1);
    delete st;
}

}  // namespace

int main() {
    section_layout();
    section_base();
    section_register();
    section_bool_var();
    section_slot_301();
    section_ranged_vars();
    section_slot_298();
    section_debug_menu();
    section_slot_299();
    section_composition();
    std::puts("OK test_concmd_slots_298_299_301");
    return 0;
}
