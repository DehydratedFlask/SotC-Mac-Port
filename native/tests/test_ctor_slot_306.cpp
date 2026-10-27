// CPU-01-STARTUP-SLOT-306-ASSEMBLE-01: fixture for startup constructor index 306 (0xa633b0), the 46
// `sndx_` objects. The expected table below is an INDEPENDENT copy of
// docs/checkpoints/2026-10-25-slot306-semantics.md (construction order #0..#45); it is deliberately not
// derived from kSlot306Table, so a missing, extra, swapped or re-parameterised object fails here.
//
// Sections:
//   1. the independent table versus kSlot306Table and the per-module site tables
//   2. a full run on fresh registries: every object's bytes, pending-list order, atexit log, root list
//   3. a run on a registry whose root list already exists (production state after slot 299)
//   4. allocator accounting for the four MenuSeparator strings and their release
//   5. registry variants: manager with the behavioural model, parent contract, assert hooks
//   6. fail-closed cases with named blocked sites (allocator, manager, parent, bad functor tag)
//   7. production row: position between 305 and 319, composition with 298/299/301/303, init_failed
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <vector>

#include "core/concmd_base.hpp"
#include "core/concmd_vars.hpp"
#include "core/ctor_original_table.hpp"
#include "core/ctor_slot_298_b0e250.hpp"
#include "core/ctor_slot_299_b0c7a0.hpp"
#include "core/ctor_slot_301_ac9550.hpp"
#include "core/ctor_slot_303_a82170.hpp"
#include "core/ctor_slot_306_a633b0.hpp"
#include "core/debug_menu_item.hpp"
#include "core/startup_ctors.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

enum class Sh { Root, Int, Float, Bool, Cmd, Group, Slider, Sep, Toggle, CmdItem };

// {index, shape, original global, atexit destructor, name/caption, parent path, command name,
//  a, b, c, target index}. a/b/c per shape: Int = default/min/max; Float = min bits/max bits/default
// bits; Cmd = a is the handler tag; Slider = range min/max bits; CmdItem = c is the opaque target.
struct Exp {
    int index;
    Sh shape;
    std::uint64_t rva;
    std::uint64_t dtor;
    const char* name;
    const char* parent;
    const char* command;
    std::uint64_t a, b, c;
    int target;
};

constexpr Exp kExp[46] = {
    {0, Sh::Root, 0x32f8ff0, 0xa5be70, "", "", "", 0x0, 0x0, 0x0, -1},
    {1, Sh::Int, 0x32f8ff8, 0xa5bea0, "sndx_listenerOrientation", "", "", 0x0, 0x0, 0x1, -1},
    {2, Sh::Int, 0x32f90d0, 0xa5bea0, "sndx_listenerPosition", "", "", 0x0, 0x0, 0x1, -1},
    {3, Sh::Float, 0x32f91a8, 0x20f0, "sndx_listenerPlayerOffset", "", "", 0x0, 0x42c80000, 0x0, -1},
    {4, Sh::Bool, 0x32f9280, 0x20b0, "sndx_drawListenerPosition", "", "", 0x0, 0x0, 0x0, -1},
    {5, Sh::Bool, 0x32f9348, 0x20b0, "sndx_showEnvironmentInfo", "", "", 0x0, 0x0, 0x0, -1},
    {6, Sh::Bool, 0x32f9410, 0x20b0, "sndx_showWorldAmbientInfo", "", "", 0x0, 0x0, 0x0, -1},
    {7, Sh::Bool, 0x32f94d8, 0x20b0, "sndx_showTouchVolumes", "", "", 0x0, 0x0, 0x0, -1},
    {8, Sh::Bool, 0x32f95a0, 0x20b0, "sndx_showDuckingInfo", "", "", 0x0, 0x0, 0x0, -1},
    {9, Sh::Bool, 0x32f9668, 0x20b0, "sndx_showGlobalInstVars", "", "", 0x0, 0x0, 0x0, -1},
    {10, Sh::Bool, 0x32f9730, 0x20b0, "sndx_onlyUseLastCore", "", "", 0x0, 0x0, 0x0, -1},
    {11, Sh::Float, 0x32f9810, 0x20f0, "sndx_masterVolume", "", "", 0x0, 0x3f800000, 0x3f800000, -1},
    {12, Sh::Bool, 0x32f98e8, 0x20b0, "sndx_attachSndxDebugger", "", "", 0x0, 0x0, 0x0, -1},
    {13, Sh::Bool, 0x32f99b0, 0x20b0, "sndx_showBanks", "", "", 0x0, 0x0, 0x0, -1},
    {14, Sh::Int, 0x32f9a78, 0xa5bee0, "sndx_showActive", "", "", 0x0, 0x0, 0x2, -1},
    {15, Sh::Bool, 0x32f9b50, 0x20b0, "sndx_showActive3d", "", "", 0x0, 0x0, 0x0, -1},
    {16, Sh::Bool, 0x32f9c18, 0x20b0, "sndx_showSystemInfo", "", "", 0x0, 0x0, 0x0, -1},
    {17, Sh::Float, 0x32f9ce0, 0x20f0, "sndx_debugGlobalAlpha", "", "", 0x0, 0x3f800000, 0x3f800000, -1},
    {18, Sh::Float, 0x32f9db8, 0x20f0, "sndx_debugLingerTime", "", "", 0x0, 0x41200000, 0x0, -1},
    {19, Sh::Float, 0x32f9e90, 0x20f0, "sndx_debugLingerFadeTime", "", "", 0x0, 0x41200000, 0x3f000000, -1},
    {20, Sh::Cmd, 0x32f9f68, 0x4b0, "sndx_reloadData", "", "", 0xa5bf20, 0x0, 0x0, -1},
    {21, Sh::Cmd, 0x32fa008, 0x4b0, "sndx_categoryMuteAll", "", "", 0xa5bf50, 0x0, 0x0, -1},
    {22, Sh::Cmd, 0x32fa0a8, 0x4b0, "sndx_categoryFilterAll", "", "", 0xa5c020, 0x0, 0x0, -1},
    {23, Sh::Group, 0x32fa148, 0x272d0, "Sound", "", "", 0x0, 0x0, 0x0, -1},
    {24, Sh::Slider, 0x32fa1f0, 0x272d0, "Master Volume", "Sound", "", 0x3c23d70a, 0x3dcccccd, 0x0, 11},
    {25, Sh::Group, 0x32fa298, 0x272d0, "Category Mute", "Sound", "", 0x0, 0x0, 0x0, -1},
    {26, Sh::Sep, 0x32fa340, 0x272d0, "", "Sound", "", 0x0, 0x0, 0x0, -1},
    {27, Sh::Toggle, 0x32fa3e8, 0x272d0, "Show System Info", "Sound", "", 0x0, 0x0, 0x0, 16},
    {28, Sh::Toggle, 0x32fa490, 0x272d0, "Show Banks", "Sound", "", 0x0, 0x0, 0x0, 13},
    {29, Sh::Toggle, 0x32fa538, 0x272d0, "Show Environment Info", "Sound", "", 0x0, 0x0, 0x0, 5},
    {30, Sh::Toggle, 0x32fa5e0, 0x272d0, "Show World Ambient Info", "Sound", "", 0x0, 0x0, 0x0, 6},
    {31, Sh::Toggle, 0x32fa688, 0x272d0, "Draw Touch Volumes", "Sound", "", 0x0, 0x0, 0x0, 7},
    {32, Sh::Toggle, 0x32fa730, 0x272d0, "Show Ducking Info", "Sound", "", 0x0, 0x0, 0x0, 8},
    {33, Sh::Toggle, 0x32fa7d8, 0x272d0, "Show Global Inst Vars", "Sound", "", 0x0, 0x0, 0x0, 9},
    {34, Sh::Sep, 0x32fa880, 0x272d0, "", "Sound", "", 0x0, 0x0, 0x0, -1},
    {35, Sh::Toggle, 0x32fa928, 0x272d0, "Show Active", "Sound", "", 0x0, 0x0, 0x0, 14},
    {36, Sh::Toggle, 0x32fa9d0, 0x272d0, "Show Active 3d", "Sound", "", 0x0, 0x0, 0x0, 15},
    {37, Sh::Group, 0x32faa78, 0x272d0, "Category Filters", "Sound", "", 0x0, 0x0, 0x0, -1},
    {38, Sh::Slider, 0x32fab20, 0x272d0, "Inactive Linger Time", "Sound", "", 0x3dcccccd, 0x3f000000, 0x0, 18},
    {39, Sh::Sep, 0x32fabc8, 0x272d0, "", "Sound", "", 0x0, 0x0, 0x0, -1},
    {40, Sh::Toggle, 0x32fac70, 0x272d0, "Draw Listener Position", "Sound", "", 0x0, 0x0, 0x0, 4},
    {41, Sh::Toggle, 0x32fad18, 0x272d0, "Listener Orientation", "Sound", "", 0x0, 0x0, 0x0, 1},
    {42, Sh::Toggle, 0x32fadc0, 0x272d0, "Listener Position", "Sound", "", 0x0, 0x0, 0x0, 2},
    {43, Sh::Slider, 0x32fae68, 0x272d0, "Player Position Offset", "Sound", "", 0x3dcccccd, 0x3f800000, 0x0, 3},
    {44, Sh::Sep, 0x32faf10, 0x272d0, "", "Sound", "", 0x0, 0x0, 0x0, -1},
    {45, Sh::CmdItem, 0x32fafb8, 0x272d0, "Reload All Sound Data", "Sound", "sndx_reloadData", 0x0, 0x0, 0x2dfcbd0, -1},
};


SharedStr view(const char* text) {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(std::strlen(text)), 0,
                     nullptr};
}

bool same_view(const SharedStr& v, const char* text) {
    const std::size_t len = std::strlen(text);
    return v.data != nullptr && v.size == static_cast<std::int64_t>(len) && v.aux == 0 && v.ctl == nullptr &&
           std::memcmp(v.data, text, len) == 0;
}

std::uint32_t dword(const void* obj, std::size_t off) {
    std::uint32_t v;
    std::memcpy(&v, static_cast<const std::uint8_t*>(obj) + off, 4);
    return v;
}

// ---- counting allocator hooks -----------------------------------------------------------------------
struct AllocRecord {
    std::size_t size;
    std::uint32_t tag, align, flags;
    void* ptr;
};
std::vector<AllocRecord> g_allocs;
std::vector<std::pair<void*, std::uint32_t>> g_frees;
bool g_alloc_fail = false;

void* counting_allocate(std::size_t size, std::uint32_t tag, std::uint32_t align, std::uint32_t flags) {
    if (g_alloc_fail) return nullptr;
    void* p = std::malloc(size);
    g_allocs.push_back({size, tag, align, flags, p});
    return p;
}
bool counting_deallocate(void* ptr, std::uint32_t tag) {
    g_frees.emplace_back(ptr, tag);
    std::free(ptr);
    return true;
}
void reset_counters() {
    g_allocs.clear();
    g_frees.clear();
    g_alloc_fail = false;
}
Slot306Hooks counting_hooks() {
    Slot306Hooks h;
    h.separator_alloc.alloc.allocate = &counting_allocate;
    h.separator_alloc.alloc.deallocate = &counting_deallocate;
    return h;
}

const ConCmdBase* base_of_pending(const ConCmdListNode* node) {
    return reinterpret_cast<const ConCmdBase*>(reinterpret_cast<const std::uint8_t*>(node) -
                                               offsetof(ConCmdBase, pending));
}

const DebugMenuItem* item_of_link(const MenuLink* link) {
    return reinterpret_cast<const DebugMenuItem*>(reinterpret_cast<const std::uint8_t*>(link) -
                                                  offsetof(DebugMenuItem, link));
}

const void* addr(const Slot306Objects& o, std::size_t i) { return slot306_object_address(o, i); }

// ------------------------------------------------------------------ 1. tables
void section_tables() {
    CHECK(kSlot306Rva == 0xa633b0 && kSlot306EndRva == 0xa654bd && kSlot306BodySize == 0x210d);
    CHECK(kSlot306EndRva - kSlot306Rva == kSlot306BodySize);
    CHECK(kSlot306ObjectCount == 46 && kSlot306Table.size() == 46);
    CHECK(kSlot306Gaps.size() == 0);   // no object is a gap: the fragment must be complete
    CHECK(kSlot306IntVarCount + kSlot306FloatVarCount + kSlot306BoolVarCount + kSlot306CommandCount +
              kSlot306MenuItemCount + 1 == 46);
    CHECK(kSlot306FirstMenuIndex == 23 && kSlot306ItemFlags == 0x30 && kSlot306VarFlags == 1);

    std::size_t n_int = 0, n_float = 0, n_bool = 0, n_cmd = 0, n_item = 0, n_root = 0;
    for (std::size_t i = 0; i < 46; ++i) {
        const Exp& x = kExp[i];
        const Slot306Entry& e = kSlot306Table[i];
        CHECK(static_cast<std::size_t>(x.index) == i);
        CHECK(e.global_rva == x.rva);
        CHECK(e.atexit_dtor_rva == x.dtor);
        CHECK(std::strcmp(e.name, x.name) == 0);
        CHECK(std::strcmp(e.parent, x.parent) == 0);
        CHECK(std::strcmp(e.command, x.command) == 0);
        CHECK(e.target_index == x.target);
        switch (x.shape) {
            case Sh::Root: CHECK(e.kind == Slot306Kind::root_cell); ++n_root; break;
            case Sh::Int: {
                CHECK(e.kind == Slot306Kind::int_var);
                const Slot306IntVarSite& s = kSlot306IntVarSites[e.slot];
                CHECK(std::strcmp(s.name, x.name) == 0 && s.global_rva == x.rva);
                CHECK(s.default_value == static_cast<std::int32_t>(x.a) && s.min_value == static_cast<std::int32_t>(x.b) &&
                      s.max_value == static_cast<std::int32_t>(x.c));
                ++n_int;
                break;
            }
            case Sh::Float:
                CHECK(e.kind == Slot306Kind::float_var);
                CHECK(e.lo_bits == x.a && e.hi_bits == x.b && e.default_bits == x.c);
                ++n_float;
                break;
            case Sh::Bool: CHECK(e.kind == Slot306Kind::bool_var); ++n_bool; break;
            case Sh::Cmd: {
                CHECK(e.kind == Slot306Kind::command && e.handler_rva == x.a);
                const Slot306CommandSite& s = kSlot306CommandSites[e.slot];
                CHECK(std::strcmp(s.name, x.name) == 0 && s.object_rva == x.rva && s.handler_rva == x.a);
                ++n_cmd;
                break;
            }
            case Sh::Group: CHECK(e.kind == Slot306Kind::menu_group); ++n_item; break;
            case Sh::Slider:
                CHECK(e.kind == Slot306Kind::menu_slider && e.lo_bits == x.a && e.hi_bits == x.b);
                ++n_item;
                break;
            case Sh::Sep: CHECK(e.kind == Slot306Kind::menu_separator); ++n_item; break;
            case Sh::Toggle: CHECK(e.kind == Slot306Kind::menu_toggle); ++n_item; break;
            case Sh::CmdItem:
                CHECK(e.kind == Slot306Kind::menu_command && e.target_rva == x.c && x.c == 0x2dfcbd0);
                ++n_item;
                break;
        }
        if (i >= 23) CHECK(e.slot == i - 23);
    }
    CHECK(n_root == 1 && n_int == 3 && n_float == 5 && n_bool == 11 && n_cmd == 3 && n_item == 23);
    // The 46 object addresses are distinct and ascend within construction order inside the two .bss
    // ranges, which is how the original lays them out (0x32f8ff0 .. 0x32fafb8 + 0xc0).
    for (std::size_t i = 1; i < 46; ++i) CHECK(kExp[i].rva > kExp[i - 1].rva);
    // Menu targets point at the variables of the same names (spot checks of the semantics table).
    CHECK(kExp[24].target == 11 && kExp[27].target == 16 && kExp[28].target == 13);
    CHECK(kExp[35].target == 14 && kExp[41].target == 1 && kExp[42].target == 2 && kExp[43].target == 3);
    CHECK(kExp[35].shape == Sh::Toggle && kExp[41].shape == Sh::Toggle && kExp[42].shape == Sh::Toggle);
    CHECK(kExp[kExp[35].target].shape == Sh::Int && kExp[kExp[41].target].shape == Sh::Int &&
          kExp[kExp[42].target].shape == Sh::Int);   // toggles on int variables
}

// ------------------------------------------------------------------ object checks
void check_bool(const ConCmdBoolVar& v, const Exp& x) {
    CHECK(v.base.vptr == kBoolVarFinalVptr);
    CHECK(v.base.flags == 1 && v.base.registered == 1);
    CHECK(same_view(v.base.name, x.name));
    CHECK(v.base.description.size == 0 && v.base.description.ctl == nullptr);
    CHECK(v.value_ref == &v.value && v.value == 0 && v.default_value == 0);
    CHECK(v.fn_vptr == 0x15d2060 && v.fn_payload == 0 && v.fn_aux == 0);
}

void check_float(const ConCmdRangedVar& v, const Exp& x) {
    CHECK(v.base.vptr == 0x15942b0);
    CHECK(v.base.flags == 1 && v.base.registered == 1);
    CHECK(same_view(v.base.name, x.name));
    CHECK(v.base.description.size == 0 && v.base.description.ctl == nullptr);
    CHECK(v.value_ref == &v.value);
    CHECK(dword(&v, 0x6c) == x.a && dword(&v, 0x74) == x.b);
    CHECK(v.min_engaged == 1 && v.max_engaged == 1);
    CHECK(v.value == x.c && v.default_value == x.c);
    CHECK(v.fn_vptr == 0x1594318 && v.fn_payload == 0 && v.fn_aux == 0);
}

void check_int(const ConCmdRangedVar& v, const Exp& x) {
    CHECK(v.base.flags == 1 && v.base.registered == 1);
    CHECK(same_view(v.base.name, x.name));
    CHECK(v.value_ref == &v.value);
    CHECK(v.default_value == x.a && v.value == x.a);
    CHECK(dword(&v, 0x6c) == x.b && dword(&v, 0x74) == x.c);
    CHECK(v.min_engaged == 1 && v.max_engaged == 1);
    const bool show_active = std::strcmp(x.name, "sndx_showActive") == 0;
    CHECK(v.base.vptr == (show_active ? 0x15d20b0u : 0x15d1fd8u));
    CHECK(v.fn_vptr == (show_active ? 0x15d2080u : 0x15d1fa8u));
}

void check_command(const ConCmdCommand& c, const Exp& x) {
    CHECK(c.base.vptr == 0x1594dd0 && c.base.flags == 2 && c.base.registered == 1);
    CHECK(same_view(c.base.name, x.name));
    CHECK(c.base.description.size > 0 && c.base.description.ctl == nullptr);
    CHECK(c.delegate.object == reinterpret_cast<std::uint64_t>(&c.delegate));
    CHECK(c.delegate.fn == 0x1990 && c.delegate.adj == 0 && c.delegate.handler == x.a);
    CHECK(c.args_begin == nullptr && c.args_end == nullptr && c.args_cap == nullptr);
}

void check_item(const Slot306Objects& o, const DebugMenuItem& it, const Exp& x) {
    CHECK(it.flags == 0x30);
    CHECK(same_view(it.label, x.parent));
    CHECK(it.registered == 0);   // root-list items keep registered == 0
    switch (x.shape) {
        case Sh::Group:
            CHECK(it.kind == 1 && it.target == nullptr);
            CHECK(same_view(it.text, x.name) && same_view(it.empty, x.name));
            CHECK(dword(&it, 0x98) == 0x3f800000u && dword(&it, 0x9c) == 0x41200000u);
            break;
        case Sh::Slider:
            CHECK(it.kind == 2 && it.target == addr(o, static_cast<std::size_t>(x.target)));
            CHECK(same_view(it.text, x.name));
            CHECK(dword(&it, 0x98) == x.a && dword(&it, 0x9c) == x.b);
            break;
        case Sh::Toggle:
            CHECK(it.kind == 2 && it.target == addr(o, static_cast<std::size_t>(x.target)));
            CHECK(same_view(it.text, x.name));
            // bool slots {1, 0} and int slots {1, 1} both keep the default range {1.0, 10.0}
            CHECK(dword(&it, 0x98) == 0x3f800000u && dword(&it, 0x9c) == 0x41200000u);
            CHECK(it.empty.size == 0 && it.empty.ctl == nullptr);
            break;
        case Sh::Sep:
            CHECK(it.kind == 3 && it.target == nullptr);
            CHECK(it.text.size == 13 && it.text.aux == 15 && it.text.ctl != nullptr && *it.text.ctl == 1);
            CHECK(std::memcmp(it.text.data, "MenuSeparator", 14) == 0);
            CHECK(it.empty.size == 0 && it.empty.ctl == nullptr);
            CHECK(dword(&it, 0x98) == 0x3f800000u && dword(&it, 0x9c) == 0x41200000u);
            break;
        case Sh::CmdItem:
            CHECK(it.kind == 2 && it.target == &o.command_target_cell);
            CHECK(same_view(it.text, "Reload All Sound Data") && same_view(it.empty, "sndx_reloadData"));
            CHECK(dword(&it, 0x98) == 0x3f800000u && dword(&it, 0x9c) == 0x41200000u);
            break;
        default: CHECK(false);
    }
}

// Verifies every one of the 46 objects against the independent table.
void check_all_objects(const Slot306Objects& o) {
    CHECK(o.root_cell == 0);
    for (std::size_t i = 0; i < 46; ++i) {
        const Exp& x = kExp[i];
        const Slot306Entry& e = kSlot306Table[i];
        CHECK(addr(o, i) != nullptr);
        switch (x.shape) {
            case Sh::Root: CHECK(addr(o, i) == &o.root_cell); break;
            case Sh::Int: CHECK(addr(o, i) == &o.int_vars[e.slot]); check_int(o.int_vars[e.slot], x); break;
            case Sh::Float: CHECK(addr(o, i) == &o.floats[e.slot]); check_float(o.floats[e.slot], x); break;
            case Sh::Bool: CHECK(addr(o, i) == &o.bools[e.slot]); check_bool(o.bools[e.slot], x); break;
            case Sh::Cmd: CHECK(addr(o, i) == &o.commands[e.slot]); check_command(o.commands[e.slot], x); break;
            default: CHECK(addr(o, i) == &o.items[i - 23]); check_item(o, o.items[i - 23], x); break;
        }
    }
}

// Pending list: the 22 console objects #1..#22 in construction order (FIFO).
void check_pending(const Slot306Objects& o, const ConCmdRegistry& creg, std::size_t first_extra) {
    const ConCmdListNode* node = creg.pending.first();
    for (std::size_t skip = 0; skip < first_extra; ++skip) node = node->next;
    for (std::size_t i = 1; i <= 22; ++i) {
        CHECK(node != nullptr && node != &creg.pending.head);
        CHECK(base_of_pending(node) == addr(o, i));
        node = node->next;
    }
    CHECK(node == &creg.pending.head);
}

// atexit log entries for table objects, starting at `pos`; `root_after` is the table index after which the
// root-list destructor (0x27280) is recorded (23: first item registration) or -1 when it already exists.
void check_atexit(const Slot306Objects& o, const StartupAtexitLog& log, std::size_t pos, bool root_recorded_here) {
    for (std::size_t i = 0; i < 46; ++i) {
        if (i == 23 && root_recorded_here) {
            CHECK(log.at(pos).dtor_rva == 0x27280);
            ++pos;
        }
        CHECK(log.at(pos).dtor_rva == kExp[i].dtor);
        CHECK(log.at(pos).object == addr(o, i));
        ++pos;
    }
    CHECK(pos == log.size());
}

void check_root_list(const Slot306Objects& o, const DebugMenuRegistry& menu, std::size_t first_extra) {
    const MenuLink* link = menu.root.link.next;
    for (std::size_t skip = 0; skip < first_extra; ++skip) link = link->next;
    for (std::size_t i = 23; i < 46; ++i) {
        CHECK(link != &menu.root.link);
        CHECK(item_of_link(link) == &o.items[i - 23]);
        CHECK(link->self == link && link->owner == &menu.root.link);
        link = link->next;
    }
    CHECK(link == &menu.root.link && menu.root.link.prev == &o.items[22].link);
}

// ------------------------------------------------------------------ 2. fresh registries
void section_fresh_run() {
    reset_counters();
    {
        auto o = std::make_unique<Slot306Objects>();
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        const Slot306Result r = init_slot306_a633b0(*o, creg, menu, counting_hooks());
        CHECK(r.ok && r.built_count == 46 && r.gap_count == 0);
        CHECK(r.blocked.index == kSlot306ObjectCount && r.blocked.stage == Slot306Stage::none);
        for (std::size_t i = 0; i < 46; ++i) CHECK(r.built[i]);
        CHECK(r.menu_asserts_emitted == 0);

        check_all_objects(*o);
        CHECK(creg.pending.size() == 22);
        check_pending(*o, creg, 0);
        // 46 object records plus the root-list destructor, which a fresh registry records on the first item.
        CHECK(creg.atexit_log.size() == 47);
        check_atexit(*o, creg.atexit_log, 0, true);
        CHECK(menu.root_guard_done && menu.root.count == 23);
        check_root_list(*o, menu, 0);
        CHECK(creg.atexit_log.at(23).object == &menu.root);
        CHECK(creg.manager.manager == nullptr && menu.parent.parent == nullptr);
        // Counters: four separators, two allocations each.
        CHECK(g_allocs.size() == 8 && g_frees.empty());
    }
    // The destructor returned all eight allocations (data tag 0, control tag 1).
    CHECK(g_frees.size() == 8);
    std::size_t tag0 = 0, tag1 = 0;
    for (const auto& f : g_frees) (f.second == 0 ? tag0 : tag1)++;
    CHECK(tag0 == 4 && tag1 == 4);
}

// ------------------------------------------------------------------ 3. production state
void section_after_299() {
    reset_counters();
    auto o299 = std::make_unique<Slot299Objects>();
    auto o306 = std::make_unique<Slot306Objects>();
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    CHECK(init_slot299_b0c7a0(*o299, creg, menu).ok);
    const std::size_t atexit_before = creg.atexit_log.size();
    const std::size_t pending_before = creg.pending.size();
    CHECK(atexit_before == 5 && pending_before == 2 && menu.root.count == 2);
    const Slot306Result r = init_slot306_a633b0(*o306, creg, menu, counting_hooks());
    CHECK(r.ok && r.built_count == 46);
    CHECK(creg.atexit_log.size() == atexit_before + 46);   // no second root destructor
    check_atexit(*o306, creg.atexit_log, atexit_before, false);
    CHECK(creg.pending.size() == pending_before + 22);
    check_pending(*o306, creg, pending_before);
    CHECK(menu.root.count == 2 + 23);
    check_root_list(*o306, menu, 2);
    CHECK(o299->adjust_item.link.next == &o306->items[0].link);
    check_all_objects(*o306);
}

// ------------------------------------------------------------------ 4. allocator accounting
void section_allocator() {
    reset_counters();
    auto o = std::make_unique<Slot306Objects>();
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    CHECK(init_slot306_a633b0(*o, creg, menu, counting_hooks()).ok);
    CHECK(g_allocs.size() == 8);
    for (std::size_t k = 0; k < 4; ++k) {
        const AllocRecord& data = g_allocs[2 * k];
        const AllocRecord& ctl = g_allocs[2 * k + 1];
        // 0xf1a0: (strlen + 4) & ~3 = 16 with tag 0 / align 8 / flags 0, then the control block (8, 1, 8, 1)
        CHECK(data.size == 16 && data.tag == 0 && data.align == 8 && data.flags == 0);
        CHECK(ctl.size == 8 && ctl.tag == 1 && ctl.align == 8 && ctl.flags == 1);
    }
    // The separators are #26, #34, #39, #44 in this order; their texts are the allocations in this order.
    const std::size_t seps[4] = {26, 34, 39, 44};
    for (std::size_t k = 0; k < 4; ++k) {
        CHECK(o->items[seps[k] - 23].text.data == g_allocs[2 * k].ptr);
        CHECK(o->items[seps[k] - 23].text.ctl == g_allocs[2 * k + 1].ptr);
    }
}

// ------------------------------------------------------------------ 5. registry variants
bool g_assert_probe_calls = false;
bool probe_log_active() noexcept { g_assert_probe_calls = true; return true; }
bool probe_emit(const MenuAssertEvent&) noexcept { g_assert_probe_calls = true; return true; }

std::size_t g_parent_inserts = 0;
bool parent_insert(void*, DebugMenuItem& item) noexcept { ++g_parent_inserts; item.registered = 0; return true; }

void section_variants() {
    // (a) A non-null ConCmd manager with the behavioural model: nothing queues, 22 entries by name.
    {
        reset_counters();
        auto o = std::make_unique<Slot306Objects>();
        ConCmdRegistry creg;
        FConCmdManagerModel model;
        creg.manager.manager = &model;
        creg.manager.insert = &FConCmdManagerModel::insert_hook;
        DebugMenuRegistry menu;
        const Slot306Result r = init_slot306_a633b0(*o, creg, menu, counting_hooks());
        CHECK(r.ok && creg.pending.size() == 0);
        CHECK(model.size() == 22 && !model.failed());
        for (std::size_t i = 1; i <= 22; ++i) CHECK(model.find(kExp[i].name) == addr(*o, i));
    }
    // (b) The assert hooks are never consulted: every item uses flags 0x30.
    {
        reset_counters();
        auto o = std::make_unique<Slot306Objects>();
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        Slot306Hooks h = counting_hooks();
        h.menu_assert.log_active = &probe_log_active;
        h.menu_assert.emit = &probe_emit;
        g_assert_probe_calls = false;
        const Slot306Result r = init_slot306_a633b0(*o, creg, menu, h);
        CHECK(r.ok && !g_assert_probe_calls && r.menu_asserts_emitted == 0);
    }
    // (c) A debug-menu parent with its insert contract: all 23 items go there, the root list stays empty.
    {
        reset_counters();
        auto o = std::make_unique<Slot306Objects>();
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        menu.parent.insert = &parent_insert;
        g_parent_inserts = 0;
        const Slot306Result r = init_slot306_a633b0(*o, creg, menu, counting_hooks());
        CHECK(r.ok && g_parent_inserts == 23 && menu.root.count == 0 && !menu.root_guard_done);
        CHECK(creg.atexit_log.size() == 46);   // no root destructor on the parent path
    }
}

// ------------------------------------------------------------------ 6. fail-closed
void section_failures() {
    // (a) No allocator at all: the first separator (#26) is a named blocked site; nothing after it exists.
    {
        reset_counters();
        auto o = std::make_unique<Slot306Objects>();
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        const Slot306Hooks none{};   // separator_alloc.alloc.allocate == nullptr
        const Slot306Result r = init_slot306_a633b0(*o, creg, menu, none);
        CHECK(!r.ok && r.built_count == 26 && r.blocked.index == 26);
        CHECK(r.blocked.stage == Slot306Stage::menu_separator_alloc);
        CHECK(std::strcmp(slot306_stage_name(r.blocked.stage), "menu_separator_alloc") == 0);
        for (std::size_t i = 0; i < 46; ++i) CHECK(r.built[i] == (i < 26));
        CHECK(menu.root.count == 3);   // #23 #24 #25 only
        CHECK(o->items[26 - 23].link.self == nullptr && o->items[27 - 23].kind == 0);
        CHECK(creg.atexit_log.size() == 26 + 1);   // objects #0..#25 and the root destructor
        CHECK(creg.pending.size() == 22);
        CHECK(g_allocs.empty());
    }
    // (b) Allocator that fails: same site, no leak (nothing was allocated).
    {
        reset_counters();
        g_alloc_fail = true;
        auto o = std::make_unique<Slot306Objects>();
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        const Slot306Result r = init_slot306_a633b0(*o, creg, menu, counting_hooks());
        CHECK(!r.ok && r.blocked.index == 26 && r.blocked.stage == Slot306Stage::menu_separator_alloc);
        CHECK(g_allocs.empty());
        g_alloc_fail = false;
    }
    // (c) Allocator that fails on the second separator: the first one is released by the destructor.
    {
        reset_counters();
        {
            auto o = std::make_unique<Slot306Objects>();
            ConCmdRegistry creg;
            DebugMenuRegistry menu;
            Slot306Hooks h = counting_hooks();
            // Let exactly the first separator (two allocations) succeed.
            static std::size_t budget;
            budget = 2;
            h.separator_alloc.alloc.allocate = [](std::size_t size, std::uint32_t tag, std::uint32_t align,
                                                  std::uint32_t flags) -> void* {
                if (budget == 0) return nullptr;
                --budget;
                return counting_allocate(size, tag, align, flags);
            };
            const Slot306Result r = init_slot306_a633b0(*o, creg, menu, h);
            CHECK(!r.ok && r.blocked.index == 34 && r.built_count == 34);
            CHECK(g_allocs.size() == 2 && g_frees.empty());
        }
        CHECK(g_frees.size() == 2);
    }
    // (d) A manager without its insert contract: the first console object (#1) is the blocked site.
    {
        auto o = std::make_unique<Slot306Objects>();
        ConCmdRegistry creg;
        int token = 0;
        creg.manager.manager = &token;
        DebugMenuRegistry menu;
        const Slot306Result r = init_slot306_a633b0(*o, creg, menu, counting_hooks());
        CHECK(!r.ok && r.built_count == 1 && r.blocked.index == 1 && r.blocked.stage == Slot306Stage::int_var_site);
        CHECK(r.built[0] && !r.built[1] && menu.root.count == 0);
    }
    // (e) A debug-menu parent without a contract: #23 is the blocked site, #0..#22 are built.
    {
        reset_counters();
        auto o = std::make_unique<Slot306Objects>();
        ConCmdRegistry creg;
        DebugMenuRegistry menu;
        int token = 0;
        menu.parent.parent = &token;
        const Slot306Result r = init_slot306_a633b0(*o, creg, menu, counting_hooks());
        CHECK(!r.ok && r.built_count == 23 && r.blocked.index == 23 && r.blocked.stage == Slot306Stage::menu_registration);
        CHECK(menu.root.count == 0 && creg.pending.size() == 22);
    }
    // (f) The float constructor still refuses an unlifted functor tag (0x15d2040 is accepted since slot 306;
    //     the bool tag 0x15d2060 is not a float functor) and writes nothing then.
    {
        ConCmdRangedVar v;
        std::memset(static_cast<void*>(&v), 0xA5, sizeof v);
        RangedVarArgs args{};
        args.fn_vptr = 0x15d2060;
        std::uint32_t cell = 0;
        CHECK(!init_ranged_float_var_c300(v, view("x"), &cell, 0.0f, 1.0f, 1, args, view("")));
        const auto* bytes = reinterpret_cast<const std::uint8_t*>(&v);
        for (std::size_t i = 0; i < sizeof v; ++i) CHECK(bytes[i] == 0xA5);
        ConCmdRegistry creg;
        args.fn_vptr = kRangedFloatArgFnVtblSndx;
        CHECK(init_ranged_float_var_c300(v, view("x"), &cell, 0.0f, 1.0f, 1, args, view("")));
        CHECK(v.fn_vptr == kRangedFloatFnVtbl);
    }
    // (g) The stage names are distinct and non-empty.
    for (int a = 0; a <= static_cast<int>(Slot306Stage::bad_table); ++a) {
        const char* na = slot306_stage_name(static_cast<Slot306Stage>(a));
        CHECK(na != nullptr && na[0] != '\0');
        for (int b = a + 1; b <= static_cast<int>(Slot306Stage::bad_table); ++b) {
            CHECK(std::strcmp(na, slot306_stage_name(static_cast<Slot306Stage>(b))) != 0);
        }
    }
}

// ------------------------------------------------------------------ 7. production row
void section_production() {
    const auto slots = recovered_constructors();
    std::size_t pos305 = 0, pos306 = 0, pos319 = 0;
    bool have = false;
    for (std::size_t i = 0; i < slots.size(); ++i) {
        if (slots[i].original_index == 305) pos305 = i;
        if (slots[i].original_index == 319) pos319 = i;
        if (slots[i].original_index == 306) {
            have = true;
            pos306 = i;
            CHECK(slots[i].original_rva == 0xa633b0);
            CHECK(slots[i].original_rva == original_constructors()[306].rva);
            CHECK(slots[i].original_rva == kSlot306Rva);
        }
    }
    CHECK(have && pos305 + 1 == pos306 && pos306 < pos319);

    auto run = [&](StartupState& st) {
        for (const CtorSlot& s : slots) {
            if (s.original_index == 298 || s.original_index == 299 || s.original_index == 301 ||
                s.original_index == 303 || s.original_index == 306) s.run(st);
        }
    };

    {
        auto st = std::make_unique<StartupState>();
        run(*st);
        CHECK(!st->init_failed && st->slot306_result.ok && st->slot306_result.built_count == 46);
        CHECK(st->concmd.pending.size() == 5 + 22);
        CHECK(st->debug_menu.root.count == 3 + 23);
        const std::size_t before = 1 + 5 + 1 + 2;   // 298, 299 (+ root), 301, 303
        CHECK(st->concmd.atexit_log.size() == before + 46);
        check_atexit(st->slot306, st->concmd.atexit_log, before, false);
        check_all_objects(st->slot306);
        CHECK(st->concmd.manager.manager == nullptr && st->debug_menu.parent.parent == nullptr);
    }
    // Clearing the allocator in the attach state makes the production row fail visibly, with a named site.
    {
        auto st = std::make_unique<StartupState>();
        st->slot306_hooks.separator_alloc.alloc.allocate = nullptr;
        run(*st);
        CHECK(st->init_failed && !st->slot306_result.ok);
        CHECK(st->slot306_result.blocked.index == 26 &&
              st->slot306_result.blocked.stage == Slot306Stage::menu_separator_alloc);
    }
    // Slot 306 sits between 305 and the later-lane rows (307..); the row order is strictly increasing.
    for (std::size_t i = 1; i < slots.size(); ++i) CHECK(slots[i - 1].original_index < slots[i].original_index);
}

}  // namespace

int main() {
    section_tables();
    section_fresh_run();
    section_after_299();
    section_allocator();
    section_variants();
    section_failures();
    section_production();
    std::puts("OK test_ctor_slot_306");
    return 0;
}
