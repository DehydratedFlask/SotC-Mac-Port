#include "ctor_slot_306_a633b0.hpp"

#include <bit>
#include <cstdlib>
#include <cstring>

namespace sotc {

namespace {

constexpr char kEmpty[] = "";

// Borrowed view over a NUL-terminated literal: ctl == nullptr, so no counter is ever touched. The
// original builds these on the stack with the literal's address and its length.
SharedStr borrowed_view(const char* text) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text),
                     static_cast<std::int64_t>(std::strlen(text)), 0, nullptr};
}

float f32(std::uint32_t bits) noexcept { return std::bit_cast<float>(bits); }

// Default host allocator hooks (documented stand-in for 0x12820 / 0x12af0).
void* host_allocate(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) {
    return std::malloc(size);
}
bool host_deallocate(void* ptr, std::uint32_t) {
    std::free(ptr);
    return true;
}

struct Fail {
    Slot306Result& out;
    bool at(std::size_t index, Slot306Stage stage) noexcept {
        out.blocked.index = index;
        out.blocked.stage = stage;
        return false;
    }
};

// ---- per-shape builders; each returns false (after naming the stage) when a helper fails ------------

bool build_root_cell(Slot306Objects& o, ConCmdRegistry& concmd, std::size_t index, Fail& fail) noexcept {
    o.root_cell = 0;                                                         // movq $0,[0x32f8ff0]
    if (!concmd.atexit_log.record(kSlot306RootCellDtorRva, &o.root_cell)) {  // __cxa_atexit(0xa5be70)
        return fail.at(index, Slot306Stage::root_cell_atexit);
    }
    return true;
}

bool build_int_var(const Slot306Entry& e, Slot306Objects& o, ConCmdRegistry& concmd, std::size_t index,
                   Fail& fail) noexcept {
    if (e.slot >= kSlot306IntVarCount) return fail.at(index, Slot306Stage::bad_table);
    // The site table of the int-var module is the authority for name/default/range; the entry must agree.
    if (std::strcmp(kSlot306IntVarSites[e.slot].name, e.name) != 0 ||
        kSlot306IntVarSites[e.slot].global_rva != e.global_rva) {
        return fail.at(index, Slot306Stage::bad_table);
    }
    if (!init_slot306_int_var(e.slot, o.int_vars[e.slot], concmd).ok) {
        return fail.at(index, Slot306Stage::int_var_site);
    }
    return true;
}

bool build_float_var(const Slot306Entry& e, Slot306Objects& o, ConCmdRegistry& concmd, std::size_t index,
                     Fail& fail) noexcept {
    if (e.slot >= kSlot306FloatVarCount) return fail.at(index, Slot306Stage::bad_table);
    ConCmdRangedVar& a = o.floats[e.slot];
    const SharedStr name = borrowed_view(e.name);
    const SharedStr description = borrowed_view(kEmpty);   // stack view {0x12c4d1a, 0}
    RangedVarArgs args{};                                   // four zero qwords
    args.fn_payload = 0;
    args.fn_vptr = kRangedFloatArgFnVtblSndx;               // caller functor {0, 0x15d2040, 0}
    args.fn_aux = 0;

    a.base.vptr = kRangedFloatFinalVptr;                    // overwritten by the constructor
    if (!init_ranged_float_var_c300(a, name, &a.value, f32(e.lo_bits), f32(e.hi_bits), kSlot306VarFlags,
                                    args, description)) {
        return fail.at(index, Slot306Stage::float_ctor);
    }
    if (!release_inert_view_f590(description)) return fail.at(index, Slot306Stage::float_view_release);
    // `call [0x15d2040 + 0x18]`: the bare-ret destroy slot of the caller's functor.
    if (args.fn_vptr != kRangedFloatArgFnVtblSndx) return fail.at(index, Slot306Stage::float_functor_destroy);
    a.base.vptr = kRangedFloatFinalVptr;                    // [this] = 0x15942b0
    a.value = e.default_bits;                               // dword [this+0xd0]
    a.default_value = e.default_bits;                       // dword [this+0x68]
    if (!register_concmd_20d60(a.base, concmd).ok) return fail.at(index, Slot306Stage::float_registration);
    if (!release_inert_view_f590(name)) return fail.at(index, Slot306Stage::float_view_release);
    if (!concmd.atexit_log.record(kRangedFloatDtorRva, &a)) return fail.at(index, Slot306Stage::float_atexit);
    return true;
}

bool build_bool_var(const Slot306Entry& e, Slot306Objects& o, ConCmdRegistry& concmd, std::size_t index,
                    Fail& fail) noexcept {
    if (e.slot >= kSlot306BoolVarCount) return fail.at(index, Slot306Stage::bad_table);
    const SharedStr name = borrowed_view(e.name);
    const SharedStr description = borrowed_view(kEmpty);
    // Inline shape: ctor 0x20af0, stores (value 0 at +0xc0, default 0 at +0x68, tag 0x15d2060), register,
    // inert releases, atexit 0x20b0.
    if (!init_bool_var_static(o.bools[e.slot], name, description, kSlot306BoolFnVtbl, concmd, 0, 0).ok) {
        return fail.at(index, Slot306Stage::bool_var);
    }
    return true;
}

bool build_command(const Slot306Entry& e, Slot306Objects& o, ConCmdRegistry& concmd, std::size_t index,
                   Fail& fail) noexcept {
    if (e.slot >= kSlot306CommandCount) return fail.at(index, Slot306Stage::bad_table);
    const Slot306CommandSite& site = kSlot306CommandSites[e.slot];
    if (std::strcmp(site.name, e.name) != 0 || site.object_rva != e.global_rva ||
        site.handler_rva != e.handler_rva) {
        return fail.at(index, Slot306Stage::bad_table);
    }
    if (!init_slot306_command(e.slot, o.commands[e.slot], concmd).ok) {
        return fail.at(index, Slot306Stage::command_build);
    }
    return true;
}

// Target of a toggle: the virtual-slot results of the variable the item points at.
bool toggle_target(const Slot306Entry& item, const Slot306Objects& o, DebugMenuTarget& out) noexcept {
    if (item.target_index < 0 || static_cast<std::size_t>(item.target_index) >= kSlot306ObjectCount) return false;
    const Slot306Entry& t = kSlot306Table[static_cast<std::size_t>(item.target_index)];
    if (t.global_rva == 0 || t.target_index != -1) return false;
    if (t.kind == Slot306Kind::bool_var) {
        out = debug_menu_target_for(o.bools[t.slot]);
    } else if (t.kind == Slot306Kind::int_var) {
        out = int_var_menu_target(o.int_vars[t.slot]);
    } else {
        return false;
    }
    return out.known && out.object == slot306_object_address(o, static_cast<std::size_t>(item.target_index));
}

bool build_menu_item(const Slot306Entry& e, Slot306Objects& o, ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                     const Slot306Hooks& hooks, std::size_t index, Slot306Result& out, Fail& fail) noexcept {
    if (index < kSlot306FirstMenuIndex || e.slot != index - kSlot306FirstMenuIndex) {
        return fail.at(index, Slot306Stage::bad_table);
    }
    DebugMenuItem& item = o.items[e.slot];
    const SharedStr parent = borrowed_view(e.parent);
    const SharedStr caption = borrowed_view(e.name);
    SharedStr command{};   // item #45 only
    bool has_command = false;
    bool has_caption_release = true;
    bool ctor_ok = false;

    switch (e.kind) {
        case Slot306Kind::menu_group:
            ctor_ok = init_menu_group_26c10(item, parent, caption, kSlot306ItemFlags);
            break;
        case Slot306Kind::menu_slider: {
            if (e.target_index < 0 || static_cast<std::size_t>(e.target_index) >= kSlot306ObjectCount ||
                kSlot306Table[static_cast<std::size_t>(e.target_index)].kind != Slot306Kind::float_var) {
                return fail.at(index, Slot306Stage::menu_target_unknown);
            }
            ctor_ok = init_debug_menu_item_27050(item, parent, caption,
                                                 slot306_object_address(o, static_cast<std::size_t>(e.target_index)),
                                                 kSlot306ItemFlags, f32(e.lo_bits), f32(e.hi_bits));
            break;
        }
        case Slot306Kind::menu_toggle: {
            DebugMenuTarget target{};
            if (!toggle_target(e, o, target)) return fail.at(index, Slot306Stage::menu_target_unknown);
            ctor_ok = init_debug_menu_item_26ea0(item, parent, caption, target, kSlot306ItemFlags);
            break;
        }
        case Slot306Kind::menu_separator: {
            has_caption_release = false;   // 0xf590 only on the path view
            ctor_ok = init_menu_separator_26b60(item, parent, kSlot306ItemFlags, hooks.separator_alloc);
            if (!ctor_ok) return fail.at(index, Slot306Stage::menu_separator_alloc);
            o.owned_alloc = hooks.separator_alloc.alloc;
            break;
        }
        case Slot306Kind::menu_command: {
            command = borrowed_view(e.command);
            has_command = true;
            o.command_target_cell = 0;
            if (e.target_rva != kSlot306CommandTargetRva) return fail.at(index, Slot306Stage::bad_table);
            ctor_ok = init_menu_command_26f90(item, parent, caption, &o.command_target_cell, command,
                                              kSlot306ItemFlags);
            break;
        }
        default:
            return fail.at(index, Slot306Stage::bad_table);
    }
    if (!ctor_ok) return fail.at(index, Slot306Stage::menu_ctor);

    // The constructor tail-calls 0x268b0 (null parent -> root list, or the parent contract).
    const MenuRegisterResult reg = register_menu_item_268b0(item, menu, concmd.atexit_log, hooks.menu_assert);
    out.menu_asserts_emitted += reg.asserts_emitted;
    if (!reg.ok) return fail.at(index, Slot306Stage::menu_registration);

    if (has_command && !release_inert_view_f590(command)) return fail.at(index, Slot306Stage::menu_view_release);  // 0xa6546e
    if (has_caption_release && !release_inert_view_f590(caption)) return fail.at(index, Slot306Stage::menu_view_release);
    if (!release_inert_view_f590(parent)) return fail.at(index, Slot306Stage::menu_view_release);
    if (!record_menu_item_dtor_272d0(concmd.atexit_log, item)) return fail.at(index, Slot306Stage::menu_atexit);
    return true;
}

}  // namespace

Slot306Objects::~Slot306Objects() {
    // Only the four MenuSeparator texts own storage (0xf1a0: data tag 0, control tag 1).
    if (owned_alloc.deallocate == nullptr) return;
    for (DebugMenuItem& item : items) {
        if (item.kind != kMenuKindSeparator || item.text.ctl == nullptr) continue;
        (void)owned_alloc.deallocate(const_cast<std::uint8_t*>(item.text.data), 0);
        (void)owned_alloc.deallocate(item.text.ctl, 1);
        item.text.ctl = nullptr;
    }
}

Slot306Hooks default_slot306_hooks() noexcept {
    Slot306Hooks hooks;
    hooks.separator_alloc.alloc.allocate = &host_allocate;
    hooks.separator_alloc.alloc.deallocate = &host_deallocate;
    return hooks;
}

const char* slot306_stage_name(Slot306Stage stage) noexcept {
    switch (stage) {
        case Slot306Stage::none: return "none";
        case Slot306Stage::root_cell_atexit: return "root_cell_atexit";
        case Slot306Stage::int_var_site: return "int_var_site";
        case Slot306Stage::float_ctor: return "float_ctor";
        case Slot306Stage::float_view_release: return "float_view_release";
        case Slot306Stage::float_functor_destroy: return "float_functor_destroy";
        case Slot306Stage::float_registration: return "float_registration";
        case Slot306Stage::float_atexit: return "float_atexit";
        case Slot306Stage::bool_var: return "bool_var";
        case Slot306Stage::command_build: return "command_build";
        case Slot306Stage::menu_target_unknown: return "menu_target_unknown";
        case Slot306Stage::menu_ctor: return "menu_ctor";
        case Slot306Stage::menu_separator_alloc: return "menu_separator_alloc";
        case Slot306Stage::menu_registration: return "menu_registration";
        case Slot306Stage::menu_view_release: return "menu_view_release";
        case Slot306Stage::menu_atexit: return "menu_atexit";
        case Slot306Stage::bad_table: return "bad_table";
    }
    return "";
}

const void* slot306_object_address(const Slot306Objects& o, std::size_t index) noexcept {
    if (index >= kSlot306ObjectCount) return nullptr;
    const Slot306Entry& e = kSlot306Table[index];
    switch (e.kind) {
        case Slot306Kind::root_cell: return &o.root_cell;
        case Slot306Kind::int_var: return e.slot < o.int_vars.size() ? &o.int_vars[e.slot] : nullptr;
        case Slot306Kind::float_var: return e.slot < o.floats.size() ? &o.floats[e.slot] : nullptr;
        case Slot306Kind::bool_var: return e.slot < o.bools.size() ? &o.bools[e.slot] : nullptr;
        case Slot306Kind::command: return e.slot < o.commands.size() ? &o.commands[e.slot] : nullptr;
        case Slot306Kind::menu_group:
        case Slot306Kind::menu_slider:
        case Slot306Kind::menu_toggle:
        case Slot306Kind::menu_separator:
        case Slot306Kind::menu_command: return e.slot < o.items.size() ? &o.items[e.slot] : nullptr;
    }
    return nullptr;
}

Slot306Result init_slot306_a633b0(Slot306Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                  const Slot306Hooks& hooks) noexcept {
    Slot306Result out{};
    out.gap_count = kSlot306Gaps.size();
    Fail fail{out};

    for (std::size_t i = 0; i < kSlot306ObjectCount; ++i) {
        const Slot306Entry& e = kSlot306Table[i];
        bool ok = false;
        switch (e.kind) {
            case Slot306Kind::root_cell: ok = build_root_cell(objects, concmd, i, fail); break;
            case Slot306Kind::int_var: ok = build_int_var(e, objects, concmd, i, fail); break;
            case Slot306Kind::float_var: ok = build_float_var(e, objects, concmd, i, fail); break;
            case Slot306Kind::bool_var: ok = build_bool_var(e, objects, concmd, i, fail); break;
            case Slot306Kind::command: ok = build_command(e, objects, concmd, i, fail); break;
            case Slot306Kind::menu_group:
            case Slot306Kind::menu_slider:
            case Slot306Kind::menu_toggle:
            case Slot306Kind::menu_separator:
            case Slot306Kind::menu_command:
                ok = build_menu_item(e, objects, concmd, menu, hooks, i, out, fail);
                break;
        }
        if (!ok) return out;
        out.built[i] = true;
        ++out.built_count;
    }
    out.ok = out.built_count == kSlot306ObjectCount && out.gap_count == 0;
    return out;
}

}  // namespace sotc
