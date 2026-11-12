// CPU-01-STARTUP-SLOT-567: see ctor_slot_567_235d0.hpp.
#include "ctor_slot_567_235d0.hpp"

#include <cstring>

namespace sotc {

namespace {

SharedStr borrowed_view(const char* text, std::size_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(length),
                     0, nullptr};
}

// 0x20d80 (direct entry): the body passes `[0x2dfcbc8]`, which is null on the whole startup path.
bool default_register_direct(ConCmdBase& cmd, void* manager, ConCmdRegistry& registry) noexcept {
    return register_concmd_body_20d80(cmd, manager, registry).ok;
}

// 0x20d60 (guarded entry) reached through command vtable slot +0x10.
bool default_register_guarded(ConCmdBase& cmd, ConCmdRegistry& registry) noexcept {
    return register_concmd_20d60(cmd, registry).ok;
}

// 0x23380 and the two scratch identity tags: pinned, not modelled.
bool default_pre_register(ConCmdBase&) noexcept { return true; }
bool default_scratch(std::uint32_t, const SharedStr&) noexcept { return true; }

bool default_atexit(std::uint64_t dtor_rva, const void* object, ConCmdRegistry& registry) noexcept {
    return registry.atexit_log.record(dtor_rva, object);
}

bool default_release_view(const SharedStr& view) noexcept { return release_inert_view_f590(view); }

}  // namespace

Slot567Hooks default_slot567_hooks() noexcept {
    Slot567Hooks hooks{};
    hooks.register_direct = &default_register_direct;
    hooks.register_guarded = &default_register_guarded;
    hooks.pre_register = &default_pre_register;
    hooks.scratch = &default_scratch;
    hooks.atexit = &default_atexit;
    hooks.release_view = &default_release_view;
    return hooks;
}

const char* slot567_stage_name(Slot567Stage stage) noexcept {
    switch (stage) {
        case Slot567Stage::none: return "none";
        case Slot567Stage::hooks: return "hooks";
        case Slot567Stage::cell: return "cell";
        case Slot567Stage::cell_atexit: return "cell_atexit";
        case Slot567Stage::build: return "build";
        case Slot567Stage::pre_register: return "pre_register";
        case Slot567Stage::register_command: return "register_command";
        case Slot567Stage::scratch: return "scratch";
        case Slot567Stage::release: return "release";
        case Slot567Stage::command_atexit: return "command_atexit";
        case Slot567Stage::canary: return "canary";
    }
    return "";
}

const Slot567CommandSpec& slot567_command(std::uint32_t index) noexcept {
    return kSlot567Commands[index < kSlot567CommandCount ? index : 0];
}

void init_slot567_command(Slot567Command& command, const Slot567CommandSpec& spec) noexcept {
    ConCmdBase& base = command.base;
    Slot567Tail& tail = command.tail;
    // 0x236ab / 0x23894 / 0x23985 / 0x23b2b: the base vptr write; 0x2372b / 0x23894 / 0x23a11 /
    // 0x23bbb overwrite it with the derived address point in the same body.
    base.vptr = kSlot567BaseVptr;
    base.registered = 0;                                   // 0x236ae / 0x23802 / 0x23988 / 0x23b2e
    base.flags = spec.flags;                               // 0x236b2 / 0x23806 / 0x2398c / 0x23b32
    base.name = borrowed_view(spec.name, spec.name_len);   // 0x236b9 + 0x236be/0x236c2
    base.description = borrowed_view(spec.description, spec.description_len);  // 0x236e5..0x236ee
    // 0x2371a..0x23748: delegate {self, 0x1990, 0x2512a0, handler} with the self-rewrite.
    tail.self = reinterpret_cast<std::uint64_t>(&tail);
    tail.word68 = kSlot567DelegateWord68;
    tail.word70 = kSlot567DelegateWord70;
    tail.handler = spec.handler_rva;
    tail.zero80 = {0, 0};                                  // 0x23750
    tail.word90 = 0;                                       // 0x23758
    base.vptr = kSlot567CmdVptr;                           // 0x2372b derived address point
    if (!spec.guarded_register) base.registered = 1;       // 0x2376c: executeCmd/getValue set it
}

// The body's own store program: 8 bytes of manager cell + four 0xa0 commands of 0x85 written bytes.
bool slot567_byte_written(std::size_t offset) noexcept {
    if (offset >= kSlot567BlockSize) return false;
    if (offset < 0x08) return true;                        // manager cell zero store
    const std::size_t within = (offset - 0x08) % kSlot567CommandSize;
    if (within < 0x08) return true;                        // vptr
    if (within < 0x18) return false;                       // pending link: the callee writes it
    if (within < 0x19) return true;                        // registered
    if (within < 0x1c) return false;                       // +0x19..+0x1b
    if (within < 0x98) return true;                        // flags, views, tail, zero80, word90
    return false;                                          // +0x98..+0x9f
}

Slot567Result init_slot567_235d0(Slot567Objects& objects, ConCmdRegistry& registry,
                                 const Slot567Hooks& hooks) noexcept {
    Slot567Result out{};
    if (hooks.register_direct == nullptr || hooks.register_guarded == nullptr ||
        hooks.pre_register == nullptr || hooks.scratch == nullptr || hooks.atexit == nullptr ||
        hooks.release_view == nullptr) {
        out.blocked = Slot567Stage::hooks;
        return out;
    }

    // 0x23612: the manager cell is zeroed before the first atexit record of the block.
    out.blocked = Slot567Stage::cell;
    objects.manager = 0;

    // 0x23619: __cxa_atexit(0x21060, &[0x2dfcbc8], &__dso_handle), recorded first.
    out.blocked = Slot567Stage::cell_atexit;
    if (!hooks.atexit(kSlot567CellDtorRva, &objects.manager, registry)) return out;
    out.atexit_records = 1;

    for (std::uint32_t i = 0; i < kSlot567CommandCount; ++i) {
        const Slot567CommandSpec& spec = kSlot567Commands[i];
        Slot567Command& command = objects.commands[i];
        out.blocked = Slot567Stage::build;
        out.blocked_command = i;
        init_slot567_command(command, spec);
        out.commands_built = i + 1;

        // 0x23a32: only `help` carries a pre-register callback (0x23380).
        if (spec.pre_callback_rva != 0) {
            out.blocked = Slot567Stage::pre_register;
            if (!hooks.pre_register(command.base)) return out;
        }

        out.blocked = Slot567Stage::register_command;
        const bool registered =
            spec.guarded_register
                ? hooks.register_guarded(command.base, registry)
                : hooks.register_direct(command.base, reinterpret_cast<void*>(objects.manager),
                                        registry);
        if (!registered) return out;
        ++out.registered;

        // 0x23a84 / 0x23c2e: the scratch vtable slot +0x18 receives the command's name view.
        if (spec.scratch_rva != 0) {
            out.blocked = Slot567Stage::scratch;
            if (!hooks.scratch(i, command.base.name)) return out;
        }

        // 0x2377a/0x23787 and the matching pairs: both stack views are released inertly.
        out.blocked = Slot567Stage::release;
        if (!hooks.release_view(command.base.name) || !hooks.release_view(command.base.description))
            return out;

        // 0x237a1 / 0x2390a / 0x23aac / 0x23c56: __cxa_atexit(0x4b0, &command, &__dso_handle).
        out.blocked = Slot567Stage::command_atexit;
        if (!hooks.atexit(kSlot567CmdDtorRva, &command, registry)) return out;
        ++out.atexit_records;
    }

    // 0x23c5b..0x23c7d: the TLS canary comparison; 0x23c7e is the fail path, never taken here.
    out.blocked = Slot567Stage::canary;
    out.canary_checked = true;
    out.blocked = Slot567Stage::none;
    out.ok = true;
    return out;
}

}  // namespace sotc
