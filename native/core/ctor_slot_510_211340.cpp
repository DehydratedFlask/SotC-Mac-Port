#include "ctor_slot_510_211340.hpp"

#include <cstring>

namespace sotc {
namespace {

bool default_register_guarded(ConCmdBase& cmd, ConCmdRegistry& registry) noexcept {
    return register_concmd_20d60(cmd, registry).ok;
}

bool default_menu_toggle(DebugMenuItem& item, const SharedStr& label, const SharedStr& text,
                         const DebugMenuTarget& target, std::uint32_t flags, DebugMenuRegistry& menu,
                         ConCmdRegistry& registry) noexcept {
    if (!init_debug_menu_item_26ea0(item, label, text, target, flags)) return false;
    return register_debug_menu_item_268b0(item, menu, registry.atexit_log).ok;
}

bool default_atexit(std::uint64_t dtor_rva, const void* object, ConCmdRegistry& registry) noexcept {
    return registry.atexit_log.record(dtor_rva, object);
}

bool default_release_view(const SharedStr& view) noexcept { return release_inert_view_f590(view); }

bool default_scratch_noop() noexcept { return true; }

}  // namespace

bool slot510_append_connection_token(Slot510TokenStore& store, const SharedStr& name,
                                     std::uint32_t value) noexcept {
    if (store.count >= store.capacity || store.capacity > store.items.size()) return false;
    const Slot510ConnectionRow* row = nullptr;
    for (const Slot510ConnectionRow& candidate : kSlot510ConnectionRows) {
        if (candidate.value == value) { row = &candidate; break; }
    }
    if (row == nullptr) return false;   // only the pinned rows are lifted; fail closed otherwise
    Slot510Token& token = store.items[store.count];
    token.type = 1;
    std::memset(token.gap04, 0, sizeof token.gap04);
    token.name = name;
    token.value = SharedStr{reinterpret_cast<const std::uint8_t*>(row->label),
                            static_cast<std::int64_t>(std::strlen(row->label)), 0, nullptr};
    token.hint = value;
    std::memset(token.gap4c, 0, sizeof token.gap4c);
    ++store.count;
    return true;
}

bool slot510_append_axis_token(Slot510TokenStore& store, const SharedStr& name) noexcept {
    if (store.count >= store.capacity || store.capacity > store.items.size()) return false;
    Slot510Token& token = store.items[store.count];
    token.type = 3;
    std::memset(token.gap04, 0, sizeof token.gap04);
    token.name = name;
    token.value = SharedStr{nullptr, 0, 0, nullptr};   // 0xf180 empty view
    token.hint = 0;                                    // +0x48 is NOT written by the inline push
    std::memset(token.gap4c, 0, sizeof token.gap4c);
    ++store.count;
    return true;
}

Slot510Hooks default_slot510_hooks() noexcept {
    Slot510Hooks hooks{};
    hooks.register_guarded = &default_register_guarded;
    hooks.menu_toggle = &default_menu_toggle;
    hooks.connection_token = &slot510_append_connection_token;
    hooks.axis_token = &slot510_append_axis_token;
    hooks.atexit = &default_atexit;
    hooks.release_view = &default_release_view;
    hooks.scratch_noop = &default_scratch_noop;
    return hooks;
}

bool slot510_byte_written(std::size_t offset) noexcept {
    if (offset >= kSlot510BlockSize) return false;
    // Cell: every byte of the 0x30-byte property cell.
    if (offset < kSlot510CellOffset + kSlot510CellSize) return true;
    // Bool variable: base ctor stores (vptr, registered, flags, name, description) plus the derived
    // tail, minus the pending link / pad19..1b / +0x6d..6f / +0x90..9f / +0xa8..af / +0xc1..c7.
    if (offset < kSlot510VarOffset + kSlot510VarSize) {
        const std::size_t local = offset - kSlot510VarOffset;
        if (local < 0x08) return true;                  // vptr
        if (local < 0x18) return false;                 // pending link
        if (local == 0x18) return true;                 // registered
        if (local < 0x1c) return false;                 // +0x19..0x1b pad
        if (local < 0x60) return true;                  // flags, name, description
        if (local < 0x68) return true;                  // value_ref
        if (local == 0x68) return true;                 // default byte
        if (local < 0x6d) return true;                  // dword zero at +0x69
        if (local < 0x70) return false;                 // +0x6d..0x6f
        if (local < 0x90) return true;                  // 32 zero bytes
        if (local < 0xa0) return false;                 // functor inline buffer
        if (local < 0xa8) return true;                  // payload
        if (local < 0xb0) return false;
        if (local < 0xc1) return true;                  // functor tag, aux, value
        return false;                                   // +0xc1..0xc7
    }
    // Toggle item: 0x26ea0 stores everything except +0x21..0x27 and +0x4c..0x4f.
    if (offset < kSlot510ItemOffset + kSlot510ItemSize) {
        const std::size_t local = offset - kSlot510ItemOffset;
        if (local < 0x21) return true;
        if (local < 0x28) return false;
        if (local < 0x4c) return true;
        if (local < 0x50) return false;
        return true;
    }
    // 4-byte alignment pad.
    if (offset < kSlot510PoseOffset) return false;
    // Pose: all 0x30 bytes.
    if (offset < kSlot510CommandOffset) return true;
    // Command: base stores, delegate, vector header; +0x98..0x9f and +0xa0..0xa7 unwritten.
    const std::size_t local = offset - kSlot510CommandOffset;
    if (local < 0x08) return true;
    if (local < 0x18) return false;
    if (local == 0x18) return true;
    if (local < 0x1c) return false;
    if (local < 0x98) return true;   // flags, name, description, delegate, vector header
    return false;
}

const char* slot510_stage_name(Slot510Stage stage) noexcept {
    switch (stage) {
        case Slot510Stage::none: return "none";
        case Slot510Stage::hooks: return "hooks";
        case Slot510Stage::property_cell: return "property_cell";
        case Slot510Stage::property_atexit: return "property_atexit";
        case Slot510Stage::var_build: return "var_build";
        case Slot510Stage::var_register: return "var_register";
        case Slot510Stage::var_atexit: return "var_atexit";
        case Slot510Stage::toggle: return "toggle";
        case Slot510Stage::toggle_atexit: return "toggle_atexit";
        case Slot510Stage::pose: return "pose";
        case Slot510Stage::command_build: return "command_build";
        case Slot510Stage::command_reserve: return "command_reserve";
        case Slot510Stage::token_connection: return "token_connection";
        case Slot510Stage::token_axes: return "token_axes";
        case Slot510Stage::register_command: return "register_command";
        case Slot510Stage::scratch_call: return "scratch_call";
        case Slot510Stage::release_views: return "release_views";
        case Slot510Stage::command_atexit: return "command_atexit";
        case Slot510Stage::canary: return "canary";
    }
    return "?";
}

Slot510Result init_slot510_211340(Slot510Objects& objects, Slot510TokenStore& tokens,
                                  ConCmdRegistry& registry, DebugMenuRegistry& menu,
                                  const Slot510Hooks& hooks) noexcept {
    Slot510Result out{};
    if (hooks.register_guarded == nullptr || hooks.menu_toggle == nullptr ||
        hooks.connection_token == nullptr || hooks.axis_token == nullptr ||
        hooks.atexit == nullptr || hooks.release_view == nullptr || hooks.scratch_noop == nullptr) {
        out.blocked = Slot510Stage::hooks;
        return out;
    }

    // 0x21135f..0x2113fa: string property cell, release of its stack view, atexit 0x8ef00.
    {
        const SharedStr view{reinterpret_cast<const std::uint8_t*>(kSlot510Property),
                             static_cast<std::int64_t>(kSlot510PropertyLength), 0, nullptr};
        objects.cell.text = view;
        objects.cell.tail = 0;
        objects.cell.vptr = kSlot510CellVptr;
        if (!hooks.release_view(view)) { out.blocked = Slot510Stage::property_cell; return out; }
    }
    out.property_atexit = hooks.atexit(kSlot510CellDtorRva, &objects.cell, registry);
    if (!out.property_atexit) { out.blocked = Slot510Stage::property_atexit; return out; }
    ++out.atexit_records;
    out.blocked = Slot510Stage::var_build;

    // 0x2113ff..0x211509: bool variable `FollowGameCamera`, pre/mid/final vptr sequence.
    ConCmdBoolVar& var = objects.var;
    const SharedStr var_name{reinterpret_cast<const std::uint8_t*>(kSlot510VarName),
                             static_cast<std::int64_t>(kSlot510VarNameLength), 0, nullptr};
    const SharedStr var_desc{reinterpret_cast<const std::uint8_t*>(""),
                             static_cast<std::int64_t>(0), 0, nullptr};
    var.base.vptr = kSlot510VarVptr;                    // 0x211477 pre-store
    init_concmd_base_20af0(var.base, var_name, var_desc, kSlot510VarFlags);
    var.base.vptr = kSlot510VarMidVptr;                 // 0x2114a7
    var.value_ref = &var.value;
    std::memset(var.zero69, 0, sizeof var.zero69);
    std::memset(var.zero70, 0, sizeof var.zero70);
    var.fn_payload = 0;
    var.fn_vptr = kSlot510VarFnVtable;
    var.fn_aux = 0;
    if (!hooks.release_view(var_desc)) { out.blocked = Slot510Stage::var_build; return out; }
    var.base.vptr = kSlot510VarVptr;                    // 0x2114e9
    var.value = 0;                                      // 0x2114f3
    var.default_value = 0;                              // 0x2114fa
    if (!hooks.register_guarded(var.base, registry)) {
        out.blocked = Slot510Stage::var_register;
        return out;
    }
    out.var.registration.ok = true;
    out.var.registration.path = ConCmdRegisterPath::pending_list;
    out.var.ok = true;
    if (!hooks.release_view(var_name)) { out.blocked = Slot510Stage::var_build; return out; }
    out.var.atexit_recorded = hooks.atexit(kSlot510VarDtorRva, &var, registry);
    if (!out.var.atexit_recorded) { out.blocked = Slot510Stage::var_atexit; return out; }
    ++out.atexit_records;
    out.var.views_inert = true;

    // 0x21150e..0x2115b7: toggle `Debug Game` / `Follow Game Camera`, flags 0x21.
    const SharedStr group{reinterpret_cast<const std::uint8_t*>(kSlot510Group),
                          static_cast<std::int64_t>(kSlot510GroupLength), 0, nullptr};
    const SharedStr text{reinterpret_cast<const std::uint8_t*>(kSlot510Toggle),
                         static_cast<std::int64_t>(kSlot510ToggleLength), 0, nullptr};
    const DebugMenuTarget target = debug_menu_target_for(var);
    if (!target.known) { out.blocked = Slot510Stage::toggle; return out; }
    out.toggle_ok = hooks.menu_toggle(objects.item, group, text, target, kSlot510ItemFlags, menu,
                                      registry);
    if (!out.toggle_ok) { out.blocked = Slot510Stage::toggle; return out; }
    if (!hooks.release_view(text) || !hooks.release_view(group)) {
        out.blocked = Slot510Stage::toggle; return out;
    }
    out.toggle_atexit = hooks.atexit(kSlot510ItemDtorRva, &objects.item, registry);
    if (!out.toggle_atexit) { out.blocked = Slot510Stage::toggle_atexit; return out; }
    ++out.atexit_records;

    // 0x2115bc..0x2115ed: 0x30-byte pose constant block (2 float4 + 4 id words).
    objects.pose = Slot510Pose{{1.0f, 0.0f, 0.0f, 0.0f},
                               {1.0f, 0.0f, 0.0f, 0.0f},
                               {kSlot510PoseIds[0], kSlot510PoseIds[1], kSlot510PoseIds[2],
                                kSlot510PoseIds[3]}};
    out.pose_copied = true;

    // 0x2115f2..0x211738: command `SyncCameraAcrossTools`, name view through 0xf2f0.
    const SharedStr command_name{reinterpret_cast<const std::uint8_t*>(kSlot510CommandName),
                                 static_cast<std::int64_t>(kSlot510CommandNameLength), 0, nullptr};
    const SharedStr command_desc{reinterpret_cast<const std::uint8_t*>(kSlot510CommandDesc),
                                 static_cast<std::int64_t>(kSlot510CommandDescLength), 0, nullptr};
    Slot510Command& command = objects.command;
    init_concmd_base_20af0(command.base, command_name, command_desc, kSlot510CommandFlags);
    command.base.vptr = kSlot510CommandVptr;            // 0x21167a
    command.delegate.self = reinterpret_cast<std::uint64_t>(&command.delegate);   // 0x211690
    command.delegate.word68 = kSlot510DelegateWord68;
    command.delegate.word70 = kSlot510DelegateWord70;
    command.delegate.handler = kSlot510HandlerRva;
    if (command.delegate.handler != 0) {                // 0x211695..0x2116a1 self-rewrite
        command.delegate.self = reinterpret_cast<std::uint64_t>(&command.delegate);
    }
    out.delegate_rewritten = command.delegate.self == reinterpret_cast<std::uint64_t>(&command.delegate);

    // 0x2116a8..0x2116d2: zero the vector header, then reserve seven tokens.
    command.vector_begin = 0;
    command.vector_end = 0;
    command.vector_cap = 0;
    tokens.count = 0;
    tokens.capacity = kSlot510TokenCount;
    command.vector_begin = reinterpret_cast<std::uint64_t>(tokens.items.data());
    command.vector_end = command.vector_begin;
    command.vector_cap = command.vector_begin + kSlot510TokenCount * kSlot510TokenSize;
    out.reserved = true;
    out.command_built = true;

    // 0x2116d7..0x211728: token 0 through 0x210e00 (SendersConnectionType = 1 -> `Tool`).
    const SharedStr token_name{reinterpret_cast<const std::uint8_t*>(kSlot510TokenName),
                               static_cast<std::int64_t>(kSlot510TokenNameLength), 0, nullptr};
    if (!hooks.connection_token(tokens, token_name, kSlot510ConnectionTypeValue)) {
        out.blocked = Slot510Stage::token_connection; return out;
    }
    if (!hooks.release_view(token_name)) { out.blocked = Slot510Stage::release_views; return out; }
    command.vector_end = command.vector_begin + tokens.count * kSlot510TokenSize;
    out.tokens = tokens.count;

    // 0x21172d..0x211b73: six inline kind-3 axis tokens, each followed by its own view release.
    for (std::size_t i = 0; i < kSlot510AxisNames.size(); ++i) {
        const SharedStr axis{reinterpret_cast<const std::uint8_t*>(kSlot510AxisNames[i]),
                             static_cast<std::int64_t>(kSlot510AxisNameLength), 0, nullptr};
        if (!hooks.axis_token(tokens, axis)) { out.blocked = Slot510Stage::token_axes; return out; }
        if (!hooks.release_view(axis)) { out.blocked = Slot510Stage::release_views; return out; }
        command.vector_end = command.vector_begin + tokens.count * kSlot510TokenSize;
        out.tokens = tokens.count;
    }

    // 0x21178f..0x211b91: register through the command vtable slot +0x10.
    if (!hooks.register_guarded(command.base, registry)) {
        out.blocked = Slot510Stage::register_command; return out;
    }
    out.command_registration.ok = true;
    out.command_registration.path = ConCmdRegisterPath::pending_list;
    out.registered = true;

    // 0x211b9c: scratch slot 0x159abc0 + 0x18 -> 0x2111d0 (bare `ret`).
    if (!hooks.scratch_noop()) { out.blocked = Slot510Stage::scratch_call; return out; }
    out.scratch_called = true;

    // 0x211ba4..0x211bae: description release first, then the name release.
    if (!hooks.release_view(command_desc) || !hooks.release_view(command_name)) {
        out.blocked = Slot510Stage::release_views; return out;
    }
    out.views_inert = true;

    if (!hooks.atexit(kSlot510CommandDtorRva, &command, registry)) {
        out.blocked = Slot510Stage::command_atexit; return out;
    }
    ++out.atexit_records;

    out.canary_checked = true;
    out.ok = true;
    out.blocked = Slot510Stage::none;
    return out;
}

}  // namespace sotc
