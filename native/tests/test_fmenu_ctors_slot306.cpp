// CPU-01-SLOT306-MENU-CTORS-01: fixture for the debug-menu group (0x26c10), separator (0x26b60) and
// command-item (0x26f90) constructors and the complete registration helper 0x268b0 (kind-1 branch,
// gated assert sites), plus a slot-306-shaped menu composition.
//
// Sections:
//   1. layout and constants
//   2. group ctor: exact stores, untouched bytes, two control-counter acquires on the caption
//   3. separator ctor: allocation protocol, fail-closed on allocation failure
//   4. command ctor: kind, views, target, flags
//   5. registration: kinds 0..3, kind-1 branch, sites A/B/C with and without hooks, parent path
//   6. differential: new register == existing register for kinds 2/3
//   7. slot-306 shaped menu (the 20 items whose targets are native), list order and counts
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <initializer_list>
#include <utility>
#include <vector>

#include "core/concmd_base.hpp"
#include "core/concmd_vars.hpp"
#include "core/debug_menu_item.hpp"
#include "core/fmenu_ctors_slot306.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;
int g_checks = 0;

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
    return v.data != nullptr && v.size == static_cast<std::int64_t>(len) &&
           std::memcmp(v.data, text, len) == 0;
}

// Test allocator for the separator's 0xf1a0 call.
struct AllocLog {
    std::vector<void*> live;
    int allocs = 0;
    int frees = 0;
    int fail_on = 0;   // 1-based allocation index to fail; 0 = never
    std::size_t sizes[4]{};
    std::uint32_t tags[4]{};
    std::uint32_t aligns[4]{};
    std::uint32_t flags[4]{};
} g_alloc;

void* test_allocate(std::size_t size, std::uint32_t tag, std::uint32_t align, std::uint32_t flags) {
    const int n = ++g_alloc.allocs;
    if (n <= 4) {
        g_alloc.sizes[n - 1] = size;
        g_alloc.tags[n - 1] = tag;
        g_alloc.aligns[n - 1] = align;
        g_alloc.flags[n - 1] = flags;
    }
    if (g_alloc.fail_on == n) return nullptr;
    void* p = std::calloc(1, size);
    g_alloc.live.push_back(p);
    return p;
}

bool test_deallocate(void* p, std::uint32_t) {
    ++g_alloc.frees;
    for (auto& q : g_alloc.live) {
        if (q == p) { q = nullptr; std::free(p); return true; }
    }
    return false;
}

void release_all() {
    for (void*& q : g_alloc.live) { if (q != nullptr) { std::free(q); q = nullptr; } }
    g_alloc.live.clear();
}

SharedStrCtorHooks hooks() {
    SharedStrCtorHooks h{};
    h.alloc.allocate = &test_allocate;
    h.alloc.deallocate = &test_deallocate;
    return h;
}

// ------------------------------------------------------------------ 1. layout
void section_layout() {
    static_assert(sizeof(DebugMenuItem) >= 0xa4);
    CHECK(kMenuGroupCtorRva == 0x26c10 && kMenuGroupCtorSize == 0xc2);
    CHECK(kMenuSeparatorCtorRva == 0x26b60 && kMenuSeparatorCtorSize == 0xa6);
    CHECK(kMenuCommandCtorRva == 0x26f90 && kMenuCommandCtorSize == 0xc0);
    CHECK(kMenuKindGroup == 1 && kMenuKindCommand == 2 && kMenuKindSeparator == 3);
    CHECK(sizeof kMenuSeparatorText - 1 == 13 && kMenuSeparatorTextRva == 0xf96530);
    CHECK(kMenuSlot306Flags == 0x30);
    // Every slot-306 item flag value avoids all three gated sites (the reason no hook is needed).
    CHECK((kMenuSlot306Flags & kMenuAssertFlagsMaskA) == 0);
    CHECK((kMenuSlot306Flags & kMenuAssertFlagsMaskB) == 0);
    CHECK((kMenuSlot306Flags & kMenuKind1FlagsMask) == kMenuKind1FlagsOk);
}

// ------------------------------------------------------------------ 2. group
void section_group() {
    DebugMenuItem g;
    poison(g);
    std::int64_t pc = 1, cc = 7;
    const SharedStr parent = view("Sound", 5, &pc);
    const SharedStr caption = view("Category Mute", 13, &cc);
    CHECK(init_menu_group_26c10(g, parent, caption, 0x30));
    CHECK(g.link.self == nullptr && g.link.prev == &g.link && g.link.next == &g.link && g.link.owner == nullptr);
    CHECK(g.registered == 0 && g.kind == 1);
    CHECK(same_view(g.label, "Sound", 5) && g.label.ctl == &pc && g.label.aux == 0);
    CHECK(same_view(g.text, "Category Mute", 13) && g.text.ctl == &cc);
    CHECK(same_view(g.empty, "Category Mute", 13) && g.empty.ctl == &cc);   // caption copied twice
    CHECK(g.target == nullptr && g.flags == 0x30);
    CHECK(d(g, 0x98) == 0x3f800000u && d(g, 0x9c) == 0x41200000u);
    CHECK(pc == 2);        // parent acquired once
    CHECK(cc == 9);        // caption acquired twice
    check_untouched(g, {{0x00, 0x20}, {0x20, 1}, {0x28, 0x20}, {0x48, 4}, {0x50, 0x20}, {0x70, 0x20},
                        {0x90, 8}, {0x98, 4}, {0x9c, 4}, {0xa0, 4}});

    // Top group: empty parent, caption "Sound", borrowed views (no counters).
    DebugMenuItem top;
    poison(top);
    CHECK(init_menu_group_26c10(top, view("", 0), view("Sound", 5), 0x30));
    CHECK(top.label.size == 0 && same_view(top.text, "Sound", 5) && same_view(top.empty, "Sound", 5));
    ++g_checks;
}

// ------------------------------------------------------------------ 3. separator
void section_separator() {
    g_alloc = AllocLog{};
    DebugMenuItem s;
    poison(s);
    std::int64_t pc = 3;
    CHECK(init_menu_separator_26b60(s, view("Sound", 5, &pc), 0x30, hooks()));
    CHECK(s.link.self == nullptr && s.link.prev == &s.link && s.link.next == &s.link && s.link.owner == nullptr);
    CHECK(s.registered == 0 && s.kind == 3);
    CHECK(same_view(s.label, "Sound", 5) && pc == 4);
    // "MenuSeparator": 0xf1a0 protocol: data alloc (13+4)&~3 = 16, aux = 15, control block 8.
    CHECK(same_view(s.text, "MenuSeparator", 13) && s.text.aux == 15 && s.text.ctl != nullptr && *s.text.ctl == 1);
    CHECK(s.text.data[13] == 0);
    CHECK(g_alloc.allocs == 2 && g_alloc.sizes[0] == 16 && g_alloc.sizes[1] == 8);
    CHECK(g_alloc.tags[0] == 0 && g_alloc.flags[0] == 0 && g_alloc.tags[1] == 1 && g_alloc.flags[1] == 1);
    CHECK(g_alloc.aligns[0] == 8 && g_alloc.aligns[1] == 8);
    // View C is the 0xf180 empty slot.
    CHECK(s.empty.size == 0 && s.empty.aux == 0 && s.empty.ctl == nullptr);
    CHECK(s.target == nullptr && s.flags == 0x30);
    CHECK(d(s, 0x98) == 0x3f800000u && d(s, 0x9c) == 0x41200000u);
    check_untouched(s, {{0x00, 0x20}, {0x20, 1}, {0x28, 0x20}, {0x48, 4}, {0x50, 0x20}, {0x70, 0x20},
                        {0x90, 8}, {0x98, 4}, {0x9c, 4}, {0xa0, 4}});
    release_all();

    // Fail closed: no allocator -> nothing written, no counter acquired.
    DebugMenuItem n;
    poison(n);
    std::int64_t pc2 = 5;
    CHECK(!init_menu_separator_26b60(n, view("Sound", 5, &pc2), 0x30, SharedStrCtorHooks{}));
    check_untouched(n, {});
    CHECK(pc2 == 5);

    // First allocation fails.
    g_alloc = AllocLog{};
    g_alloc.fail_on = 1;
    CHECK(!init_menu_separator_26b60(n, view("Sound", 5, &pc2), 0x30, hooks()));
    check_untouched(n, {});
    CHECK(pc2 == 5 && g_alloc.allocs == 1 && g_alloc.frees == 0);

    // Control-block allocation fails: the data buffer is returned, item untouched.
    g_alloc = AllocLog{};
    g_alloc.fail_on = 2;
    CHECK(!init_menu_separator_26b60(n, view("Sound", 5, &pc2), 0x30, hooks()));
    check_untouched(n, {});
    CHECK(pc2 == 5 && g_alloc.allocs == 2 && g_alloc.frees == 1);
    release_all();
    g_alloc = AllocLog{};
}

// ------------------------------------------------------------------ 4. command item
void section_command() {
    DebugMenuItem c;
    poison(c);
    std::int64_t pc = 2, cc = 4, nc = 6;
    int target_token = 0;
    CHECK(init_menu_command_26f90(c, view("Sound", 5, &pc), view("Reload All Sound Data", 21, &cc),
                                  &target_token, view("sndx_reloadData", 15, &nc), 0x30));
    CHECK(c.link.self == nullptr && c.link.prev == &c.link && c.link.next == &c.link && c.link.owner == nullptr);
    CHECK(c.registered == 0 && c.kind == 2);
    CHECK(same_view(c.label, "Sound", 5) && same_view(c.text, "Reload All Sound Data", 21) &&
          same_view(c.empty, "sndx_reloadData", 15));
    CHECK(c.target == &target_token && c.flags == 0x30);
    CHECK(d(c, 0x98) == 0x3f800000u && d(c, 0x9c) == 0x41200000u);
    CHECK(pc == 3 && cc == 5 && nc == 7);
    check_untouched(c, {{0x00, 0x20}, {0x20, 1}, {0x28, 0x20}, {0x48, 4}, {0x50, 0x20}, {0x70, 0x20},
                        {0x90, 8}, {0x98, 4}, {0x9c, 4}, {0xa0, 4}});
    ++g_checks;
}

// ------------------------------------------------------------------ 5. registration
struct HookState {
    bool active = false;
    bool emit_ok = true;
    int emitted = 0;
    MenuAssertEvent last{};
};
HookState g_hook;
bool hook_active() noexcept { return g_hook.active; }
bool hook_emit(const MenuAssertEvent& e) noexcept {
    ++g_hook.emitted;
    g_hook.last = e;
    return g_hook.emit_ok;
}
bool parent_insert_ok(void*, DebugMenuItem&) noexcept { return true; }

// Items stay alive for the whole run: linked nodes must not dangle in a registry that outlives a scope.
std::deque<DebugMenuItem> g_pool;

DebugMenuItem& make(std::uint32_t kind, std::uint32_t flags) {
    DebugMenuItem& it = g_pool.emplace_back();
    poison(it);
    const SharedStr e = view("", 0);
    (void)init_menu_group_26c10(it, e, e, flags);
    it.kind = kind;
    return it;
}

void section_register() {
    StartupAtexitLog log;
    DebugMenuRegistry menu;

    // kind 1 (group) with the slot-306 flags: no site reachable, no hook needed, linked on the root list.
    DebugMenuItem& g = make(1, 0x30);
    MenuRegisterResult r = register_menu_item_268b0(g, menu, log);
    CHECK(r.ok && r.path == DebugMenuRegisterPath::root_list && r.root_initialised && r.atexit_recorded);
    CHECK(r.blocked_site == MenuAssertSite::none && r.asserts_emitted == 0);
    CHECK(menu.root.count == 1 && g.link.self == &g.link && g.link.owner == &menu.root.link);
    CHECK(log.size() == 1 && log.at(0).dtor_rva == 0x27280 && log.at(0).object == &menu.root);
    CHECK(g.registered == 0);

    // kind 3 (separator) and kind 2 (command) take the same path; order is FIFO.
    DebugMenuItem& sp = make(3, 0x30);
    r = register_menu_item_268b0(sp, menu, log);
    CHECK(r.ok && r.path == DebugMenuRegisterPath::root_list && !r.root_initialised && !r.atexit_recorded);
    DebugMenuItem& cm = make(2, 0x30);
    r = register_menu_item_268b0(cm, menu, log);
    CHECK(r.ok && menu.root.count == 3 && log.size() == 1);
    CHECK(menu.root.link.next == &g.link && g.link.next == &sp.link && sp.link.next == &cm.link &&
          cm.link.next == &menu.root.link && menu.root.link.prev == &cm.link && cm.link.prev == &sp.link &&
          sp.link.prev == &g.link && g.link.prev == &menu.root.link);

    // kind-1 branch: flags & 0x2c != 0x20 reaches site C. No hook -> fail closed, nothing linked.
    DebugMenuItem& k1 = make(1, 0x00);
    r = register_menu_item_268b0(k1, menu, log);
    CHECK(!r.ok && r.path == DebugMenuRegisterPath::unmodelled_assert && r.blocked_site == MenuAssertSite::c_kind1_flags);
    CHECK(menu.root.count == 3 && k1.link.self == nullptr);
    // 0x20 and 0x30 and 0x32 (0x32 & 0x2c == 0x20) and 0x10|0x20 all pass; 0x28/0x24/0x2c/0x0 do not.
    for (std::uint32_t f : {0x20u, 0x30u, 0x32u, 0x11u | 0x20u, 0x70u & ~0x40u}) {
        DebugMenuItem& k = make(1, f);
        MenuRegisterResult rr = register_menu_item_268b0(k, menu, log);
        CHECK(rr.ok && rr.blocked_site == MenuAssertSite::none);
    }
    CHECK(menu.root.count == 8);
    for (std::uint32_t f : {0x00u, 0x04u, 0x08u, 0x0cu, 0x24u, 0x28u, 0x2cu}) {
        DebugMenuItem& k = make(1, f);
        MenuRegisterResult rr = register_menu_item_268b0(k, menu, log);
        CHECK(!rr.ok && rr.blocked_site == MenuAssertSite::c_kind1_flags);
    }
    CHECK(menu.root.count == 8);
    // kind 2 / 3 / 0 never take site C.
    for (std::uint32_t kind : {0u, 2u, 3u}) {
        DebugMenuItem& k = make(kind, 0x00);
        CHECK(register_menu_item_268b0(k, menu, log).ok);
    }
    CHECK(menu.root.count == 11);

    // Sites A/B need a hook. Gate inactive -> skipped without emit; active -> emit with the right data.
    DebugMenuAssertHooks h{&hook_active, &hook_emit};
    g_hook = HookState{};
    DebugMenuItem& a = make(2, 0x40);   // flags & 0xc0
    r = register_menu_item_268b0(a, menu, log, DebugMenuAssertHooks{});
    CHECK(!r.ok && r.blocked_site == MenuAssertSite::a_flags_c0 && menu.root.count == 11);
    r = register_menu_item_268b0(a, menu, log, h);
    CHECK(r.ok && r.asserts_emitted == 0 && g_hook.emitted == 0 && menu.root.count == 12);
    g_hook.active = true;
    DebugMenuItem& a2 = make(2, 0x80);
    r = register_menu_item_268b0(a2, menu, log, h);
    CHECK(r.ok && r.asserts_emitted == 1 && g_hook.last.site == MenuAssertSite::a_flags_c0 &&
          g_hook.last.line == 0xf3 && g_hook.last.message_rva == 0xf9665d && g_hook.last.item == &a2);
    // Site A and B both reachable: two events, order A then B.
    DebugMenuItem& ab = make(2, 0xc0 | 0x100);
    r = register_menu_item_268b0(ab, menu, log, h);
    CHECK(r.ok && r.asserts_emitted == 2 && g_hook.last.site == MenuAssertSite::b_flags_f00 &&
          g_hook.last.line == 0xf8 && g_hook.last.message_rva == 0xf9668d);
    // Kind 0 skips B (and C), still takes A.
    DebugMenuItem& z = make(0, 0xc0 | 0x100);
    g_hook.emitted = 0;
    r = register_menu_item_268b0(z, menu, log, h);
    CHECK(r.ok && r.asserts_emitted == 1 && g_hook.emitted == 1 && g_hook.last.site == MenuAssertSite::a_flags_c0);
    // Kind 1 flags 0x108 => B (0x100) and C (0x08 & 0x2c != 0x20): emit, then C.
    DebugMenuItem& k3 = make(1, 0x108);
    g_hook.emitted = 0;
    r = register_menu_item_268b0(k3, menu, log, h);
    CHECK(r.ok && r.asserts_emitted == 2 && g_hook.last.site == MenuAssertSite::c_kind1_flags &&
          g_hook.last.line == 0xfe && g_hook.last.message_rva == 0xf966dc);
    // A hook that reports failure stops before linking.
    const std::uint32_t count_before = menu.root.count;
    g_hook.emit_ok = false;
    DebugMenuItem& fail = make(1, 0x00);
    r = register_menu_item_268b0(fail, menu, log, h);
    CHECK(!r.ok && r.blocked_site == MenuAssertSite::c_kind1_flags && menu.root.count == count_before &&
          fail.link.self == nullptr);
    // A gate with active==true but no emit hook fails closed.
    DebugMenuAssertHooks only_gate{&hook_active, nullptr};
    r = register_menu_item_268b0(fail, menu, log, only_gate);
    CHECK(!r.ok && r.blocked_site == MenuAssertSite::c_kind1_flags);

    // already registered returns first, even with failing flags.
    DebugMenuItem& done = make(1, 0x00);
    done.registered = 1;
    r = register_menu_item_268b0(done, menu, log);
    CHECK(r.ok && r.path == DebugMenuRegisterPath::already_registered && done.link.self == nullptr);

    // Parent path (kind 1 and 3): contract hook, registered = 1.
    DebugMenuRegistry pmenu;
    StartupAtexitLog plog;
    int token = 0;
    pmenu.parent.parent = &token;
    DebugMenuItem& pg = make(1, 0x30);
    r = register_menu_item_268b0(pg, pmenu, plog);
    CHECK(!r.ok && r.path == DebugMenuRegisterPath::parent && pg.registered == 0 && !pmenu.root_guard_done);
    pmenu.parent.insert = &parent_insert_ok;
    r = register_menu_item_268b0(pg, pmenu, plog);
    CHECK(r.ok && r.path == DebugMenuRegisterPath::parent && pg.registered == 1);
    DebugMenuItem& ps = make(3, 0x30);
    r = register_menu_item_268b0(ps, pmenu, plog);
    CHECK(r.ok && ps.registered == 1 && plog.size() == 0);
    // A gated failure precedes the parent insert.
    DebugMenuItem& pbad = make(1, 0x00);
    r = register_menu_item_268b0(pbad, pmenu, plog);
    CHECK(!r.ok && r.blocked_site == MenuAssertSite::c_kind1_flags && pbad.registered == 0);

    // atexit helper.
    StartupAtexitLog dl;
    CHECK(record_menu_item_dtor_272d0(dl, g) && dl.size() == 1 && dl.at(0).dtor_rva == 0x272d0 &&
          dl.at(0).object == &g);
}

// ------------------------------------------------------------------ 6. differential
void section_differential() {
    for (std::uint32_t kind : {2u, 3u}) {
        for (std::uint32_t flags : {0x30u, 0x00u, 0x40u, 0x100u, 0x2cu}) {
            StartupAtexitLog l1, l2;
            DebugMenuRegistry m1, m2;
            DebugMenuItem& i1 = make(kind, flags);
            DebugMenuItem& i2 = make(kind, flags);
            DebugMenuAssertHooks none{};
            for (int n = 0; n < 3; ++n) {
                DebugMenuRegisterResult r1 = register_debug_menu_item_268b0(i1, m1, l1);
                MenuRegisterResult r2 = register_menu_item_268b0(i2, m2, l2, none);
                CHECK(r1.ok == r2.ok && r1.path == r2.path && r1.root_initialised == r2.root_initialised &&
                      r1.atexit_recorded == r2.atexit_recorded);
                CHECK(m1.root.count == m2.root.count && l1.size() == l2.size() &&
                      m1.root_guard_done == m2.root_guard_done);
            }
        }
    }
    ++g_checks;
}

// ------------------------------------------------------------------ 7. slot-306 shaped menu
void section_slot306_menu() {
    StartupAtexitLog log;
    DebugMenuRegistry menu;
    ConCmdRegistry creg;
    ConCmdBoolVar bool_target;
    poison(bool_target);
    CHECK(init_bool_var_static(bool_target, view("t", 1), view("", 0), 0x15d5800, creg).ok);
    const DebugMenuTarget bt = debug_menu_target_for(bool_target);
    CHECK(bt.known);
    int float_token = 0, cmd_token = 0;

    std::vector<DebugMenuItem> items;
    items.reserve(32);   // pointers stay valid
    int n_group = 0, n_sep = 0, n_cmd = 0, n_toggle = 0, n_slider = 0;
    auto reg = [&](DebugMenuItem& it) {
        MenuRegisterResult r = register_menu_item_268b0(it, menu, log);
        CHECK(r.ok && r.blocked_site == MenuAssertSite::none && r.asserts_emitted == 0);
        CHECK(record_menu_item_dtor_272d0(log, it));
    };
    const SharedStr root_path = view("", 0), sound = view("Sound", 5);
    auto group = [&](const SharedStr& parent, const char* cap, std::size_t len) {
        items.emplace_back();
        CHECK(init_menu_group_26c10(items.back(), parent, view(cap, len), 0x30));
        reg(items.back());
        ++n_group;
    };
    g_alloc = AllocLog{};
    auto sep = [&]() {
        items.emplace_back();
        CHECK(init_menu_separator_26b60(items.back(), sound, 0x30, hooks()));
        reg(items.back());
        ++n_sep;
    };
    auto toggle = [&](const char* cap, std::size_t len) {
        items.emplace_back();
        CHECK(init_debug_menu_item_26ea0(items.back(), sound, view(cap, len), bt, 0x30));
        reg(items.back());
        ++n_toggle;
    };
    auto slider = [&](const char* cap, std::size_t len, float lo, float hi) {
        items.emplace_back();
        CHECK(init_debug_menu_item_27050(items.back(), sound, view(cap, len), &float_token, 0x30, lo, hi));
        reg(items.back());
        ++n_slider;
    };

    group(root_path, "Sound", 5);                              // #23
    slider("Master Volume", 13, 0.01f, 0.1f);                  // #24
    group(sound, "Category Mute", 13);                         // #25
    sep();                                                     // #26
    toggle("Show System Info", 16);                            // #27
    toggle("Show Banks", 10);                                  // #28
    toggle("Show Environment Info", 21);                       // #29
    toggle("Show World Ambient Info", 23);                     // #30
    toggle("Draw Touch Volumes", 18);                          // #31
    toggle("Show Ducking Info", 17);                           // #32
    toggle("Show Global Inst Vars", 21);                       // #33
    sep();                                                     // #34
    // #35 "Show Active" targets an int variable: virtual slots 3/6 of that class are not lifted.
    toggle("Show Active 3d", 14);                              // #36
    group(sound, "Category Filters", 16);                      // #37
    slider("Inactive Linger Time", 20, 0.1f, 0.5f);            // #38
    sep();                                                     // #39
    toggle("Draw Listener Position", 22);                      // #40
    // #41 "Listener Orientation", #42 "Listener Position": int-variable targets, same gap as #35.
    slider("Player Position Offset", 22, 0.1f, 1.0f);          // #43
    sep();                                                     // #44
    items.emplace_back();                                      // #45
    CHECK(init_menu_command_26f90(items.back(), sound, view("Reload All Sound Data", 21), &cmd_token,
                                  view("sndx_reloadData", 15), 0x30));
    reg(items.back());
    ++n_cmd;

    CHECK(n_group == 3 && n_sep == 4 && n_cmd == 1 && n_toggle == 9 && n_slider == 3);
    CHECK(items.size() == 20 && menu.root.count == 20);
    // List order == construction order; all link invariants hold.
    const MenuLink* cur = menu.root.link.next;
    for (const DebugMenuItem& it : items) {
        CHECK(cur == &it.link && it.link.self == &it.link && it.link.owner == &menu.root.link && it.registered == 0);
        CHECK(it.link.next->prev == &it.link && it.link.prev->next == &it.link);
        cur = cur->next;
    }
    CHECK(cur == &menu.root.link && menu.root.link.prev == &items.back().link);
    // Kinds in construction order (#23.. skipping the three int-variable toggles #35/#41/#42).
    const std::uint32_t expect[20] = {1, 2, 1, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2, 1, 2, 3, 2, 2, 3, 2};
    for (std::size_t i = 0; i < items.size(); ++i) CHECK(items[i].kind == expect[i]);
    // Every item carries the slot-306 flags and a target/view layout from its own ctor.
    for (const DebugMenuItem& it : items) CHECK(it.flags == 0x30);
    CHECK(items[0].label.size == 0 && same_view(items[0].text, "Sound", 5));
    CHECK(items[19].target == &cmd_token && same_view(items[19].empty, "sndx_reloadData", 15));
    // One root atexit (0x27280) plus one 0x272d0 record per item.
    CHECK(log.size() == 1 + 20 && log.at(0).dtor_rva == 0x27280);
    for (std::size_t i = 0; i < items.size(); ++i) {
        CHECK(log.at(1 + i).dtor_rva == 0x272d0 && log.at(1 + i).object == &items[i]);
    }
    release_all();
}

}  // namespace

int main() {
    section_layout();
    section_group();
    section_separator();
    section_command();
    section_register();
    section_differential();
    section_slot306_menu();
    std::puts("test_fmenu_ctors_slot306 OK");
    return 0;
}
