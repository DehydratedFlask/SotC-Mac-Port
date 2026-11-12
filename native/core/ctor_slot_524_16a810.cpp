// CPU-01-STARTUP-SLOT-524: see ctor_slot_524_16a810.hpp.
#include "ctor_slot_524_16a810.hpp"

#include <cstring>

namespace sotc {
namespace {

SharedStr literal_view(const char* text, std::size_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text),
                     static_cast<std::int64_t>(length), 0, nullptr};
}

bool record_atexit(ConCmdRegistry& registry, std::uint64_t dtor, const void* object) noexcept {
    return registry.atexit_log.record(dtor, object);
}

}  // namespace

const char* slot524_stage_name(Slot524Stage stage) noexcept {
    switch (stage) {
        case Slot524Stage::none: return "none";
        case Slot524Stage::cell_atexit: return "cell_atexit";
        case Slot524Stage::name_view: return "name_view";
        case Slot524Stage::command_build: return "command_build";
        case Slot524Stage::delegate_store: return "delegate_store";
        case Slot524Stage::reserve: return "reserve";
        case Slot524Stage::token0_append: return "token0_append";
        case Slot524Stage::token0_release: return "token0_release";
        case Slot524Stage::token1_push: return "token1_push";
        case Slot524Stage::token1_normalize: return "token1_normalize";
        case Slot524Stage::registration: return "registration";
        case Slot524Stage::scratch_call: return "scratch_call";
        case Slot524Stage::description_release: return "description_release";
        case Slot524Stage::name_release: return "name_release";
        case Slot524Stage::command_atexit: return "command_atexit";
        case Slot524Stage::canary: return "canary";
    }
    return "";
}

Slot524Result init_slot524_16a810(Slot524Objects& o, ConCmdRegistry& registry,
                                  const Slot524Hooks& hooks) noexcept {
    Slot524Result out{};
    ConCmdCommand& command = o.command;

    // 0x16a828: the cookie the body re-checks at 0x16aac4.
    const std::uint64_t guard_entry = hooks.stack_guard != nullptr ? *hooks.stack_guard : 0;

    // 0x16a82f..0x16a857: qword cell = 0, then the cell destructor is registered first.
    o.cell.word = 0;
    if (!record_atexit(registry, kSlot524CellDtorRva, &o.cell)) {
        out.blocked = Slot524Stage::cell_atexit;
        return out;
    }
    ++out.atexit_records;

    // 0x16a85c..0x16a86d: 0xf2f0 measures "screenshot" into the rsp+0x40 view.
    const SharedStr name = literal_view(kSlot524Name, kSlot524NameLength);
    // 0x16a872..0x16a8a2: the rsp+0x20 description view is {0xfaba63, 0x37, 0, 0}.
    const SharedStr description = literal_view(kSlot524Description, kSlot524DescriptionLength);

    // 0x16a8e8: flags 2, base constructor.
    init_concmd_base_20af0(command.base, name, description, kSlot524CommandFlags);

    // 0x16a8ed..0x16a91b: vptr then the 32-byte delegate. The fix-up tests the handler word; it is
    // nonzero, so the object word becomes &command->delegate, not the stack staging address.
    command.base.vptr = kSlot524CommandVptrRva;
    command.delegate.fn = kSlot524CommandTail8;
    command.delegate.adj = 0;
    command.delegate.handler = kSlot524CommandHandlerRva;
    command.delegate.object =
        reinterpret_cast<std::uint64_t>(&command.delegate);

    // 0x16a922..0x16a946: zero +0x80/+0x88/+0x90 (+0x98 untouched), then 0x20540(vector, 2).
    command.args_begin = nullptr;
    command.args_end = nullptr;
    command.args_cap = nullptr;
    command.args_begin = reinterpret_cast<FConCmdArgRecord*>(o.tokens.data());
    command.args_end = command.args_begin;
    command.args_cap = command.args_begin + kSlot524TokenCount;
    out.reserved = true;
    out.reserve_count = kSlot524TokenCount;

    // 0x16a94b..0x16a994: token 0 through 0x16a380, key 0 and name "resolution".
    VectorAppendHooks16a380 append_hooks{};
    append_hooks.stack_guard = hooks.stack_guard;
    append_hooks.grow = hooks.grow;
    append_hooks.grow_context = hooks.grow_context;
    const SharedStr resolution = literal_view(kSlot524ResolutionName, kSlot524ResolutionLength);
    out.first = append_arg_token_16a380(command, resolution, kSlot524FirstKey, append_hooks);
    if (!out.first.ok) {
        out.blocked = Slot524Stage::token0_append;
        return out;
    }
    out.tokens = 1;
    out.first_key_written = true;
    out.first_label_off = out.first.label_found && out.first.label != nullptr &&
                          std::strcmp(out.first.label, kVirtualResolutionOff) == 0 &&
                          out.first.label_length == 3;

    // 0x16a999: the stack view built by 0xf2f0 is released; it is borrowed, so this is inert.
    if (!release_inert_view_f590(resolution)) {
        out.blocked = Slot524Stage::token0_release;
        out.views_inert = false;
        return out;
    }

    // 0x16a9a1..0x16aa3c: token 1 is built inline (end < cap after the two-token reserve).
    if (command.args_end >= command.args_cap) {
        out.blocked = Slot524Stage::token1_push;
        return out;
    }
    ConCmdArgToken& second = *reinterpret_cast<ConCmdArgToken*>(command.args_end);
    const SharedStr filename = literal_view(kSlot524FilenameName, kSlot524FilenameLength);
    second.kind = kSlot524SecondKind;
    second.name = filename;
    init_slot_f180(&second.text);   // 0x16aa0e / 0x16aa13
    command.args_end = reinterpret_cast<FConCmdArgRecord*>(&second + 1);
    out.second_inline_push = true;

    // 0x16aa43..0x16aa74: compare +0x28 with the 0xf730 static; when different, release the current
    // view and copy the static's 32 bytes. The static has ctl == null, so no counter is acquired.
    const SharedStr& empty_static = empty_string_f730();
    if (empty_static.size != 0 || empty_static.aux != 0 || empty_static.ctl != nullptr) {
        out.blocked = Slot524Stage::token1_normalize;
        return out;
    }
    if (&second.text != &empty_static) {
        if (!release_inert_view_f590(second.text)) {
            out.blocked = Slot524Stage::token1_normalize;
            out.views_inert = false;
            return out;
        }
        second.text = empty_static;
    }
    out.empty_normalized = true;
    out.tokens = 2;
    out.second_flag_written = false;   // the body never stores +0x48 for token 1

    // 0x16aa7e..0x16aa8c: vptr +0x10 = 0x20d60. The manager cell is null at startup, so this queues
    // the command on the pending list.
    out.registration = register_concmd_20d60(command.base, registry);
    if (!out.registration.ok) {
        out.blocked = Slot524Stage::registration;
        return out;
    }
    out.registered = true;

    // 0x16aa8f..0x16aa97: call [0x1598780 + 0x18] on the rsp+0xb0 scratch. That slot is the
    // one-byte `ret` at 0x16a5d0; only the two scratch words are written by this body.
    o.scratch.vtable = kSlot524ScratchVtableRva;
    o.scratch.tag = kSlot524ScratchFnRva;
    if (hooks.noop_vcall != nullptr && !hooks.noop_vcall(hooks.noop_context, o.scratch)) {
        out.blocked = Slot524Stage::scratch_call;
        return out;
    }
    out.noop_vcall = true;

    // 0x16aa9a / 0x16aaa4: description then name, both borrowed and inert.
    if (!release_inert_view_f590(description)) {
        out.blocked = Slot524Stage::description_release;
        out.views_inert = false;
        return out;
    }
    if (!release_inert_view_f590(name)) {
        out.blocked = Slot524Stage::name_release;
        out.views_inert = false;
        return out;
    }

    // 0x16aaae..0x16aabf: __cxa_atexit(0x4b0, command, &__dso_handle).
    if (!record_atexit(registry, kSlot524CommandDtorRva, &command)) {
        out.blocked = Slot524Stage::command_atexit;
        return out;
    }
    ++out.atexit_records;

    // 0x16aac4..0x16aad0: cookie compare.
    if (hooks.stack_guard != nullptr) {
        out.canary_checked = true;
        out.canary_matched = *hooks.stack_guard == guard_entry;
        if (!out.canary_matched) {
            out.blocked = Slot524Stage::canary;
            return out;
        }
    }

    out.ok = out.atexit_records == kSlot524AtexitCount && out.tokens == kSlot524TokenCount &&
             out.reserved && out.first_key_written && out.first_label_off &&
             out.second_inline_push && !out.second_flag_written && out.empty_normalized &&
             out.registered && out.noop_vcall && out.views_inert;
    return out;
}

}  // namespace sotc
