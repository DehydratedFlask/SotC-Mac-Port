// CPU-01-STARTUP-SLOT-468: see ctor_slot_468_3c3a90.hpp.
#include "ctor_slot_468_3c3a90.hpp"

#include <array>
#include <cstring>

#include "ctor_98a90_3be050.hpp"

namespace sotc {
namespace {

SharedStr literal_view(const char* text, std::size_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(length), 0,
                     nullptr};
}

bool record_atexit(Slot468Result& out, ConCmdRegistry& registry, std::uint64_t dtor,
                   const void* object) noexcept {
    if (!registry.atexit_log.record(dtor, object)) return false;
    ++out.atexit_records;
    return true;
}

bool init_app(Slot468Objects& objects, std::size_t index, ConCmdRegistry& registry,
              lane_m5::AppInfoQueue& queue, Slot468Result& out) noexcept {
    const Slot468AppSpec& spec = kSlot468Apps[index];
    const SharedStr name = literal_view(spec.name, spec.name_length);
    const lane_m5::NamedDelegateSpec named{name, spec.handler};
    const lane_m5::NamedDelegateResult result = lane_m5::init_named_delegate_entry(
        objects.apps[index], named, queue, registry.atexit_log);
    if (!result.ok) return false;
    ++out.apps;
    ++out.app_registrations;
    ++out.atexit_records;
    return true;
}

bool init_entry(Slot468Objects& objects, std::size_t index, ConCmdRegistry& registry,
                const RbHooks& hooks, Slot468Result& out) noexcept {
    const Slot468EntrySpec& spec = kSlot468Entries[index];
    ScriptedEntry& entry = objects.entries[index];
    const SharedStr name = literal_view(spec.name, spec.name_length);
    const SharedStr description = literal_view(spec.description, spec.description_length);
    const std::uint64_t self = reinterpret_cast<std::uint64_t>(&entry) + 0x88;
    const std::array<ScriptedEntryDescriptor, 3> descriptors{{
        {self, spec.fns[0], 0, 0},
        {self, spec.fns[1], 0, 0},
        {self, spec.fns[2], 0, 0},
    }};
    ScriptedEntry* head = static_cast<ScriptedEntry*>(ctor_98a90_list_head_cell_2ef6400());
    RbMap* map = static_cast<RbMap*>(ctor_98a90_self_cell_2ef63f8());
    const ScriptedEntryResult result =
        init_scripted_entry_3c2a00(entry, descriptors, name, head, map, hooks);
    ctor_98a90_list_head_cell_2ef6400() = head;
    if (!result.ok) return false;
    ++out.entries;
    if (result.inserted) ++out.entry_inserts;
    if (!release_inert_view_f590(description)) return false;
    if (!release_inert_view_f590(name)) return false;
    return record_atexit(out, registry, kSlot468EntryDtor, &entry);
}

void init_command_common(Slot468Command& command, const Slot468CommandSpec& spec) noexcept {
    const SharedStr name = literal_view(spec.name, spec.name_length);
    const SharedStr description = literal_view(spec.description, spec.description_length);
    init_concmd_base_20af0(command.base, name, description, kSlot468CommandFlags);
    command.base.vptr = kSlot468CommandVptr;
    command.tail0 = reinterpret_cast<std::uint64_t>(&command.tail0);
    command.tail8 = kSlot468CommandTail8;
    command.tail10 = 0;
    command.tail18 = spec.callback;
    command.vec_begin = nullptr;
    command.vec_end = nullptr;
    command.vec_cap = nullptr;
}

bool init_standard_command(Slot468Command& command, const Slot468CommandSpec& spec,
                           ConCmdRegistry& registry, Slot468Result& out) noexcept {
    const SharedStr name = literal_view(spec.name, spec.name_length);
    const SharedStr description = literal_view(spec.description, spec.description_length);
    init_command_common(command, spec);
    if (!register_concmd_20d60(command.base, registry).ok) return false;
    ++out.commands;
    ++out.command_registrations;
    if (!release_inert_view_f590(description)) return false;
    if (!release_inert_view_f590(name)) return false;
    return record_atexit(out, registry, kSlot468CommandDtor, &command);
}

bool init_rewind_command(Slot468Objects& objects, Slot468Command& command, const Slot468CommandSpec& spec,
                         ConCmdRegistry& registry, const Slot468Hooks& hooks, Slot468Result& out) noexcept {
    const SharedStr name = literal_view(spec.name, spec.name_length);
    const SharedStr description = literal_view(spec.description, spec.description_length);
    init_command_common(command, spec);

    // 0x471c: 0x20540(vector, 1) reserves one 0x50-byte token. The model uses the embedded
    // token as the reserved storage; the checker pins the body and the call. After a successful
    // reserve, 0x4758 takes the inline branch (end < cap); the 0x20640 grow branch is not taken.
    Slot468Token& token = objects.rewind_token;
    token.kind = 0;
    token.text = literal_view("pause", 5);
    token.empty = SharedStr{reinterpret_cast<const std::uint8_t*>(kSlot468TokenEmptyData), 0, 0, nullptr};
    token.flag48 = 1;
    command.vec_begin = &token;
    command.vec_end = &token + 1;
    command.vec_cap = &token + 1;
    out.rewind_reserved = true;
    out.rewind_inline_push = true;
    out.rewind_grow_path = false;

    // 0x47dc: the command's own vtable +0x10 is 0x20d60; 0x47e7 calls the identity cell
    // 0x15b0e10 + 0x18. Neither original call target is executed here.
    if (!register_concmd_20d60(command.base, registry).ok) return false;
    out.rewind_registered = true;
    ++out.commands;
    ++out.command_registrations;
    if (hooks.rewind_secondary != nullptr &&
        !hooks.rewind_secondary(hooks.rewind_secondary_context, command)) {
        return false;
    }
    out.rewind_secondary_called = true;
    if (!release_inert_view_f590(description)) return false;
    if (!release_inert_view_f590(name)) return false;
    return record_atexit(out, registry, kSlot468CommandDtor, &command);
}

}  // namespace

Slot468Result init_slot468_3c3a90(Slot468Objects& objects, ConCmdRegistry& registry,
                                  lane_m5::AppInfoQueue& app_queue,
                                  const Slot468Hooks& hooks) noexcept {
    Slot468Result out{};

    // 1. three named app-info delegate entries.
    for (std::size_t i = 0; i < kSlot468AppCount; ++i) {
        if (!init_app(objects, i, registry, app_queue, out)) return out;
    }

    // 2. five scripted-command entries (shared 0x3c2a00 helper).
    for (std::size_t i = 0; i < kSlot468EntryCount; ++i) {
        if (!init_entry(objects, i, registry, hooks.entry_hooks, out)) return out;
    }

    // 3. ten console commands; automate_rewind is the special token-vector form.
    for (std::size_t i = 0; i < kSlot468CommandCount; ++i) {
        const Slot468CommandSpec& spec = kSlot468Commands[i];
        if (spec.rewind) {
            if (!init_rewind_command(objects, objects.commands[i], spec, registry, hooks, out)) return out;
        } else {
            if (!init_standard_command(objects.commands[i], spec, registry, out)) return out;
        }
    }

    out.canary_matched = true;
    out.ok = true;
    return out;
}

}  // namespace sotc
