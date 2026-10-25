// CPU-01-CONCMD-303-01: fixture for startup constructor index 303 (0xa82170): the `ProfileCPU` bool
// console variable (value 0 / default 0, functor tag 0x15d2478) and its "Profile CPU" debug-menu
// toggle (0x26ea0, flags 0x30), plus the new value/default parameters of init_bool_var_static.
//
// Sections:
//   1. constants
//   2. init_bool_var_static parameters: the defaults keep value 1 / default 1; value 0 / default 0
//      writes exactly the same bytes except +0x68 and +0xc0; the two bytes are independent
//   3. slot 303 alone: every written/untouched byte of both objects, lists, atexit order
//   4. slot 303 after slot 299 (production order): the root list exists already, so the root
//      destructor is not recorded again
//   5. visible failures (manager without contract, parent without contract, assert branch)
//   6. production row: registered at index 303, runs clean, composes with 298/299/301
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
#include "core/ctor_slot_303_a82170.hpp"
#include "core/debug_menu_item.hpp"
#include "core/startup_ctors.hpp"

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

bool same_view(const SharedStr& v, const char* text, std::size_t len) {
    return v.data != nullptr && v.size == static_cast<std::int64_t>(len) && v.aux == 0 &&
           std::memcmp(v.data, text, len) == 0 && v.ctl == nullptr;
}

// The written ranges of a bool variable (see concmd_vars.hpp): everything the shape stores.
void check_bool_untouched(const ConCmdBoolVar& v) {
    check_untouched(v, {{0x00, 8}, {0x08, 0x10}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 8},
                        {0x68, 1}, {0x69, 4}, {0x70, 0x20}, {0xa0, 8}, {0xb0, 8}, {0xb8, 8}, {0xc0, 1}});
}

void check_bool_common(const ConCmdBoolVar& v, std::uint64_t fn_tag) {
    CHECK(v.base.vptr == kBoolVarFinalVptr);
    CHECK(v.value_ref == &v.value);
    for (std::uint8_t byte : v.zero69) CHECK(byte == 0);
    for (std::uint8_t byte : v.zero70) CHECK(byte == 0);
    CHECK(v.fn_payload == 0 && v.fn_aux == 0 && v.fn_vptr == fn_tag);
    CHECK(v.base.registered == 1 && v.base.flags == 1);
    check_bool_untouched(v);
}

// ------------------------------------------------------------------ 1. constants
void section_constants() {
    CHECK(kSlot303Rva == 0xa82170 && kSlot303BodySize == 455);
    CHECK(kSlot303VarRva == 0x32fd228 && kSlot303ItemRva == 0x32fd2f0);
    CHECK(kSlot303ItemRva == kSlot303VarRva + offsetof(ConCmdBoolVar, value) + 8);   // 0xc0 + 8
    CHECK(kSlot303FnVtbl == 0x15d2478 && kSlot303Value == 0 && kSlot303Default == 0);
    CHECK(kSlot303ItemFlags == 0x30);
    CHECK(sizeof kSlot303Name - 1 == 10 && sizeof kSlot303Label - 1 == 7 && sizeof kSlot303Text - 1 == 11);
    CHECK(std::strcmp(kSlot303Name, "ProfileCPU") == 0 && std::strcmp(kSlot303Label, "Profile") == 0 &&
          std::strcmp(kSlot303Text, "Profile CPU") == 0);
}

// ------------------------------------------------------------------ 2. value/default parameters
void section_params() {
    const SharedStr desc = view("", 0);

    // Defaults keep the 301 / 299-B behaviour: value 1, default 1.
    ConCmdRegistry r1;
    ConCmdBoolVar a;
    poison(a);
    CHECK(init_bool_var_static(a, view("one", 3), desc, 0x15d5070, r1).ok);
    CHECK(a.value == 1 && a.default_value == 1);
    check_bool_common(a, 0x15d5070);

    // Explicit 1/1 is byte-identical to the defaulted call.
    ConCmdRegistry r2;
    ConCmdBoolVar b;
    poison(b);
    CHECK(init_bool_var_static(b, view("one", 3), desc, 0x15d5070, r2, 1, 1).ok);
    CHECK(b.value == 1 && b.default_value == 1);

    // 0/0 (slot 303): the stores are the only difference.
    ConCmdRegistry r3;
    ConCmdBoolVar z;
    poison(z);
    const BoolVarResult res = init_bool_var_static(z, view("zero", 4), desc, 0x15d2478, r3, 0, 0);
    CHECK(res.ok && res.views_inert && res.atexit_recorded && res.registration.ok);
    CHECK(z.value == 0 && z.default_value == 0);
    check_bool_common(z, 0x15d2478);

    // The two bytes are independent (value is +0xc0, default is +0x68), each stored once.
    ConCmdRegistry r4;
    ConCmdBoolVar m;
    poison(m);
    CHECK(init_bool_var_static(m, view("mixed", 5), desc, 0x15d2478, r4, 0, 1).ok);
    CHECK(m.value == 0 && m.default_value == 1);
    ConCmdBoolVar n;
    poison(n);
    ConCmdRegistry r5;
    CHECK(init_bool_var_static(n, view("mixed", 5), desc, 0x15d2478, r5, 1, 0).ok);
    CHECK(n.value == 1 && n.default_value == 0);
}

// ------------------------------------------------------------------ 3. slot 303 alone
void section_slot_303() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot303Objects o;
    poison(o);
    const Slot303Result r = init_slot303_a82170(o, creg, menu);
    CHECK(r.ok && r.var.ok && r.item_ctor_ok && r.item_views_inert && r.item_atexit_recorded);
    CHECK(r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::pending_list);
    CHECK(r.item_registration.ok && r.item_registration.path == DebugMenuRegisterPath::root_list &&
          r.item_registration.root_initialised && r.item_registration.atexit_recorded);

    // A: ProfileCPU.
    const ConCmdBoolVar& v = o.var;
    check_bool_common(v, kSlot303FnVtbl);
    CHECK(v.value == 0 && v.default_value == 0);
    CHECK(same_view(v.base.name, "ProfileCPU", 10));
    CHECK(v.base.description.data != nullptr && v.base.description.size == 0 && v.base.description.data[0] == 0 &&
          v.base.description.ctl == nullptr);
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &v.base.pending);

    // B: the toggle. Bool-class slots {1, 0} keep the default range {1.0, 10.0}.
    const DebugMenuItem& it = o.item;
    CHECK(it.target == &v && it.flags == 0x30 && it.kind == 2 && it.registered == 0);
    CHECK(d(it, 0x98) == 0x3f800000u && d(it, 0x9c) == 0x41200000u);
    CHECK(same_view(it.label, "Profile", 7) && same_view(it.text, "Profile CPU", 11));
    CHECK(it.empty.data != nullptr && it.empty.size == 0 && it.empty.ctl == nullptr);
    CHECK(it.link.self == &it.link && it.link.next == &menu.root.link && it.link.prev == &menu.root.link &&
          it.link.owner == &menu.root.link);
    CHECK(menu.root_guard_done && menu.root.count == 1 && menu.root.link.next == &it.link);
    check_untouched(it, {{0x00, 0x20}, {0x20, 1}, {0x28, 0x20}, {0x48, 4}, {0x50, 0x20}, {0x70, 0x20},
                         {0x90, 8}, {0x98, 4}, {0x9c, 4}, {0xa0, 4}});

    // atexit order when the root does not exist yet: 0x20b0(var), 0x27280(root), 0x272d0(item).
    CHECK(creg.atexit_log.size() == 3);
    CHECK(creg.atexit_log.at(0).dtor_rva == 0x20b0 && creg.atexit_log.at(0).object == &v);
    CHECK(creg.atexit_log.at(1).dtor_rva == 0x27280 && creg.atexit_log.at(1).object == &menu.root);
    CHECK(creg.atexit_log.at(2).dtor_rva == 0x272d0 && creg.atexit_log.at(2).object == &o.item);
}

// ------------------------------------------------------------------ 4. after slot 299
void section_after_299() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot299Objects o299;
    Slot303Objects o303;
    CHECK(init_slot299_b0c7a0(o299, creg, menu).ok);
    CHECK(creg.atexit_log.size() == 5 && menu.root.count == 2);
    const Slot303Result r = init_slot303_a82170(o303, creg, menu);
    CHECK(r.ok);
    CHECK(r.item_registration.path == DebugMenuRegisterPath::root_list && !r.item_registration.root_initialised &&
          !r.item_registration.atexit_recorded);
    CHECK(menu.root.count == 3);
    CHECK(o299.adjust_item.link.next == &o303.item.link && o303.item.link.next == &menu.root.link &&
          menu.root.link.prev == &o303.item.link);
    // Two entries are appended: the variable (0x20b0) and the item (0x272d0); no second root dtor.
    CHECK(creg.atexit_log.size() == 7);
    CHECK(creg.atexit_log.at(5).dtor_rva == 0x20b0 && creg.atexit_log.at(5).object == &o303.var);
    CHECK(creg.atexit_log.at(6).dtor_rva == 0x272d0 && creg.atexit_log.at(6).object == &o303.item);
    // The variable queues after 299-B in the pending list.
    CHECK(creg.pending.size() == 3);
    CHECK(o299.adjust.base.pending.next == &o303.var.base.pending &&
          o303.var.base.pending.next == &creg.pending.head);
}

// ------------------------------------------------------------------ 5. visible failures
bool parent_insert_called = false;
bool parent_insert(void*, DebugMenuItem&) noexcept { parent_insert_called = true; return true; }

void section_failures() {
    // A ConCmd manager without an insert contract: the variable fails, the later stages still run.
    {
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        Slot303Objects o;
        const Slot303Result r = init_slot303_a82170(o, creg, menu);
        CHECK(!r.ok && !r.var.ok && !r.var.registration.ok && r.var.registration.path == ConCmdRegisterPath::manager);
        CHECK(r.item_ctor_ok && r.item_registration.ok && r.item_atexit_recorded);
        CHECK(creg.pending.size() == 0);
    }
    // A non-null debug-menu parent without a contract: the item registration fails visibly.
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        Slot303Objects o;
        const Slot303Result r = init_slot303_a82170(o, creg, menu);
        CHECK(!r.ok && r.var.ok && !r.item_registration.ok &&
              r.item_registration.path == DebugMenuRegisterPath::parent);
        CHECK(menu.root.count == 0 && !menu.root_guard_done);
        // With a contract the parent path takes over: the item is inserted there, not in the root.
        DebugMenuRegistry menu2;
        menu2.parent.parent = &token;
        menu2.parent.insert = &parent_insert;
        ConCmdRegistry creg2;
        Slot303Objects o2;
        const Slot303Result r2 = init_slot303_a82170(o2, creg2, menu2);
        CHECK(r2.ok && parent_insert_called && r2.item_registration.path == DebugMenuRegisterPath::parent);
        CHECK(o2.item.registered == 1 && menu2.root.count == 0);
        CHECK(creg2.atexit_log.size() == 2);   // var + item, no root dtor on the parent path
    }
    // An item whose flags would take the 0x268b0 assert branch is not silently skipped.
    {
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        ConCmdBoolVar target;
        CHECK(init_bool_var_static(target, view("t", 1), view("", 0), 0x15d2478, creg, 0, 0).ok);
        DebugMenuItem bad;
        CHECK(init_debug_menu_item_26ea0(bad, view("Profile", 7), view("Profile CPU", 11),
                                         debug_menu_target_for(target), 0x40));
        const DebugMenuRegisterResult rr = register_debug_menu_item_268b0(bad, menu, creg.atexit_log);
        CHECK(!rr.ok && rr.path == DebugMenuRegisterPath::unmodelled_assert && menu.root.count == 0);
    }
}

// ------------------------------------------------------------------ 6. production row
void section_production() {
    const auto slots = recovered_constructors();
    bool have = false;
    std::size_t pos303 = 0, pos301 = 0, pos304 = 0;
    for (std::size_t i = 0; i < slots.size(); ++i) {
        if (slots[i].original_index == 301) pos301 = i;
        if (slots[i].original_index == 304) pos304 = i;
        if (slots[i].original_index == 303) {
            have = true;
            pos303 = i;
            CHECK(slots[i].original_rva == 0xa82170);
            CHECK(slots[i].original_rva == original_constructors()[303].rva);
        }
    }
    CHECK(have && pos301 < pos303 && pos303 + 1 == pos304);
    auto* st = new StartupState();
    for (const CtorSlot& s : slots) {
        if (s.original_index == 298 || s.original_index == 299 || s.original_index == 301 ||
            s.original_index == 303) s.run(*st);
    }
    CHECK(!st->init_failed);
    CHECK(st->concmd.pending.size() == 5);
    CHECK(st->slot303.var.base.registered == 1 && st->slot303.var.value == 0 && st->slot303.var.default_value == 0);
    CHECK(st->slot303.var.fn_vptr == 0x15d2478);
    CHECK(st->slot303.item.target == &st->slot303.var && st->slot303.item.flags == 0x30);
    CHECK(st->debug_menu.root.count == 3);
    CHECK(st->concmd.atexit_log.size() == 1 + 5 + 1 + 2);
    CHECK(st->concmd.manager.manager == nullptr && st->debug_menu.parent.parent == nullptr);
    delete st;
}

}  // namespace

int main() {
    section_constants();
    section_params();
    section_slot_303();
    section_after_299();
    section_failures();
    section_production();
    std::puts("OK test_ctor_slot_303");
    return 0;
}
