// CPU-01-STARTUP-SLOT-567: fixture for startup constructor index 567 (0x235d0).
//
// Proves the four-command block byte-for-byte: the manager-cell zero store, every field of the four
// 0xa0-byte commands (including the delegate self-rewrite), the documented unwritten holes, the
// register-once flag handling, the five atexit records, both fail-closed registration paths and the
// body's idempotent store program.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_567_235d0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kSentinel = 0x5a;

void fill(Slot567Objects& objects) {
    std::memset(static_cast<void*>(&objects), kSentinel, sizeof objects);
}

// Registration stubs: succeed without touching the command or the registry, so the sentinel sweep
// observes exactly the body's own stores.
bool stub_register_direct(ConCmdBase&, void*, ConCmdRegistry&) noexcept { return true; }
bool stub_register_guarded(ConCmdBase&, ConCmdRegistry&) noexcept { return true; }
bool stub_pre_register(ConCmdBase&) noexcept { return true; }
bool stub_scratch(std::uint32_t, const SharedStr&) noexcept { return true; }
bool stub_atexit(std::uint64_t, const void*, ConCmdRegistry&) noexcept { return true; }
bool stub_release(const SharedStr&) noexcept { return true; }

bool refusing_register_direct(ConCmdBase&, void*, ConCmdRegistry&) noexcept { return false; }
bool refusing_register_guarded(ConCmdBase&, ConCmdRegistry&) noexcept { return false; }

Slot567Hooks stub_hooks() {
    Slot567Hooks hooks{};
    hooks.register_direct = &stub_register_direct;
    hooks.register_guarded = &stub_register_guarded;
    hooks.pre_register = &stub_pre_register;
    hooks.scratch = &stub_scratch;
    hooks.atexit = &stub_atexit;
    hooks.release_view = &stub_release;
    return hooks;
}

void section_constants() {
    CHECK(kSlot567Rva == 0x235d0 && kSlot567Size == 0x6b4 && kSlot567EndRva == 0x23c84);
    CHECK(kSlot567Rva + kSlot567Size == kSlot567EndRva);
    CHECK(kSlot567TableWordRva == 0x2dd5f28 && kSlot567TableIndex == 567);
    CHECK(kSlot567InstructionCount == 347 && kSlot567DirectCallSites == 20 &&
          kSlot567IndirectCallSites == 6 && kSlot567BranchSites == 17);
    CHECK(kSlot567BlockRva == 0x2dfcbc8 && kSlot567BlockSize == 0x288);
    CHECK(kSlot567BlockRva + kSlot567BlockSize == 0x2dfce50);
    CHECK(kSlot567CommandCount == 4 && kSlot567CommandSize == 0xa0);
    CHECK(kSlot567CommandWritten == 0x85 && kSlot567BlockWritten == 0x21c &&
          kSlot567BlockHoles == 0x6c);
    CHECK(kSlot567BaseVptr == 0x1594d90 && kSlot567CmdVptr == 0x1594dd0);
    CHECK(kSlot567DelegateWord68 == 0x1990 && kSlot567DelegateWord70 == 0x2512a0);
    CHECK(kSlot567CellDtorRva == 0x21060 && kSlot567CmdDtorRva == 0x4b0);
    CHECK(kSlot567CanaryRva == 0x15fe518 && kSlot567StackChkFailRva == 0xf8f3a0);
    CHECK(kSlot567Commands[0].flags == 2 && kSlot567Commands[1].flags == 2 &&
          kSlot567Commands[2].flags == 2 && kSlot567Commands[3].flags == 0);
    CHECK(kSlot567Commands[0].handler_rva == 0x22580 && kSlot567Commands[1].handler_rva == 0x225f0 &&
          kSlot567Commands[2].handler_rva == 0x22750 && kSlot567Commands[3].handler_rva == 0x22a40);
    CHECK(!kSlot567Commands[0].guarded_register && !kSlot567Commands[1].guarded_register &&
          kSlot567Commands[2].guarded_register && kSlot567Commands[3].guarded_register);
    CHECK(kSlot567Commands[2].pre_callback_rva == 0x23380 && kSlot567Commands[3].pre_callback_rva == 0);
    CHECK(kSlot567Commands[2].scratch_rva == 0x235c0 && kSlot567Commands[3].scratch_rva == 0x23370);
    CHECK(kSlot567Commands[0].atexit_site_rva == 0x237a1 &&
          kSlot567Commands[3].atexit_site_rva == 0x23c56);
}

void section_layout() {
    CHECK(sizeof(Slot567Tail) == 0x40 && sizeof(Slot567Command) == 0xa0);
    CHECK(sizeof(Slot567Objects) == 0x288);
    CHECK(offsetof(Slot567Objects, manager) == 0x000);
    CHECK(offsetof(Slot567Objects, commands) == 0x008);
    CHECK(offsetof(Slot567Command, base) == 0x00 && offsetof(Slot567Command, tail) == 0x60);
    CHECK(offsetof(Slot567Tail, self) == 0x00 && offsetof(Slot567Tail, handler) == 0x18 &&
          offsetof(Slot567Tail, zero80) == 0x20 && offsetof(Slot567Tail, word90) == 0x30 &&
          offsetof(Slot567Tail, gap98) == 0x38);
    CHECK(std::strcmp(slot567_command(0).name, "executeCmd") == 0);
    CHECK(std::strcmp(slot567_command(1).name, "getValue") == 0);
    CHECK(std::strcmp(slot567_command(2).name, "help") == 0);
    CHECK(std::strcmp(slot567_command(3).name, "core_unittests") == 0);
    CHECK(slot567_command(0).name_len == 10 && slot567_command(1).name_len == 8 &&
          slot567_command(2).name_len == 4 && slot567_command(3).name_len == 14);
    CHECK(slot567_command(0).description_len == 62 && slot567_command(1).description_len == 40);
    CHECK(slot567_command(2).description_len == 19 && slot567_command(3).description_len == 22);
    for (std::uint32_t i = 0; i < kSlot567CommandCount; ++i) {
        CHECK(slot567_command(i).name_len == std::strlen(slot567_command(i).name));
        CHECK(slot567_command(i).description_len == std::strlen(slot567_command(i).description));
    }
}

void section_unwritten_bytes() {
    // Manager cell is written; each command's pending link, +0x19..+0x1b and +0x98..+0x9f are not.
    CHECK(slot567_byte_written(0x00) && slot567_byte_written(0x07));
    CHECK(slot567_byte_written(0x08) && slot567_byte_written(0x0f));   // command 0 vptr
    CHECK(!slot567_byte_written(0x10) && !slot567_byte_written(0x1f)); // pending link
    CHECK(slot567_byte_written(0x20));                                // registered
    CHECK(!slot567_byte_written(0x21) && !slot567_byte_written(0x23));
    CHECK(slot567_byte_written(0x24) && slot567_byte_written(0x9f));
    CHECK(!slot567_byte_written(0xa0) && !slot567_byte_written(0xa7));
    CHECK(slot567_byte_written(0xa8) && slot567_byte_written(0xa8 + 0x07));
    CHECK(!slot567_byte_written(kSlot567BlockSize));
    std::size_t written = 0;
    for (std::size_t i = 0; i < kSlot567BlockSize; ++i) if (slot567_byte_written(i)) ++written;
    CHECK(written == kSlot567BlockWritten);
    CHECK(kSlot567BlockSize - written == kSlot567BlockHoles);
}

void section_construction_stub() {
    Slot567Objects objects;
    fill(objects);
    ConCmdRegistry registry;
    const Slot567Result result = init_slot567_235d0(objects, registry, stub_hooks());
    CHECK(result.ok && result.blocked == Slot567Stage::none);
    CHECK(result.commands_built == 4 && result.registered == 4 && result.atexit_records == 5);
    CHECK(result.canary_checked && registry.atexit_log.size() == 0);

    // The sentinel survives exactly on the documented holes.
    const auto* bytes = reinterpret_cast<const std::uint8_t*>(&objects);
    std::size_t holes = 0;
    for (std::size_t i = 0; i < sizeof objects; ++i) {
        if (slot567_byte_written(i)) continue;
        ++holes;
        CHECK(bytes[i] == kSentinel);
    }
    CHECK(holes == kSlot567BlockHoles);

    CHECK(objects.manager == 0);
    for (std::uint32_t i = 0; i < kSlot567CommandCount; ++i) {
        const Slot567Command& command = objects.commands[i];
        const Slot567CommandSpec& spec = kSlot567Commands[i];
        const auto self = reinterpret_cast<std::uint64_t>(&command);
        CHECK(command.base.vptr == kSlot567CmdVptr);
        CHECK(command.base.registered == (spec.guarded_register ? 0 : 1));
        CHECK(command.base.flags == spec.flags);
        CHECK(command.base.name.data == reinterpret_cast<const std::uint8_t*>(spec.name));
        CHECK(command.base.name.size == static_cast<std::int64_t>(spec.name_len));
        CHECK(command.base.name.aux == 0 && command.base.name.ctl == nullptr);
        CHECK(command.base.description.data ==
              reinterpret_cast<const std::uint8_t*>(spec.description));
        CHECK(command.base.description.size == static_cast<std::int64_t>(spec.description_len));
        CHECK(command.base.description.aux == 0 && command.base.description.ctl == nullptr);
        CHECK(command.tail.self == self + 0x60);
        CHECK(command.tail.word68 == 0x1990 && command.tail.word70 == 0x2512a0);
        CHECK(command.tail.handler == spec.handler_rva);
        CHECK(command.tail.zero80[0] == 0 && command.tail.zero80[1] == 0);
        CHECK(command.tail.word90 == 0);
    }

    // The body's own store program is idempotent (the retail ctor runs once; the register callee,
    // not modelled by the stub, is what would link the pending list again).
    const Slot567Objects first = objects;
    const Slot567Result again = init_slot567_235d0(objects, registry, stub_hooks());
    CHECK(again.ok && again.registered == 4);
    CHECK(std::memcmp(&first, &objects, sizeof objects) == 0);
}

void section_default_hooks() {
    Slot567Objects objects;
    fill(objects);
    ConCmdRegistry registry;
    const Slot567Result result = init_slot567_235d0(objects, registry, default_slot567_hooks());
    CHECK(result.ok && result.blocked == Slot567Stage::none);
    CHECK(result.registered == 4 && result.atexit_records == 5);
    // All four queue into the pending list while the manager cell is null (the startup path).
    CHECK(registry.pending.size() == 4);
    CHECK(registry.atexit_log.size() == 5);
    CHECK(registry.atexit_log.at(0).dtor_rva == 0x21060);
    CHECK(registry.atexit_log.at(0).object == &objects.manager);
    for (std::uint32_t i = 0; i < kSlot567CommandCount; ++i) {
        CHECK(registry.atexit_log.at(i + 1).dtor_rva == 0x4b0);
        CHECK(registry.atexit_log.at(i + 1).object == &objects.commands[i]);
        CHECK(objects.commands[i].base.registered == 1);
        CHECK(objects.commands[i].base.pending.next != nullptr);
        CHECK(objects.commands[i].base.pending.prev != nullptr);
    }
}

void section_fail_closed() {
    // Missing hooks: nothing is written at all.
    {
        Slot567Objects objects;
        fill(objects);
        ConCmdRegistry registry;
        const Slot567Objects first = objects;
        const Slot567Hooks none{};
        const Slot567Result result = init_slot567_235d0(objects, registry, none);
        CHECK(!result.ok && result.blocked == Slot567Stage::hooks);
        CHECK(std::memcmp(&first, &objects, sizeof objects) == 0);
        CHECK(registry.atexit_log.size() == 0);
    }
    // A refusing direct register stops at command 0 after building it and setting +0x18 = 1.
    {
        Slot567Objects objects;
        fill(objects);
        ConCmdRegistry registry;
        Slot567Hooks hooks = stub_hooks();
        hooks.register_direct = &refusing_register_direct;
        const Slot567Result result = init_slot567_235d0(objects, registry, hooks);
        CHECK(!result.ok && result.blocked == Slot567Stage::register_command);
        CHECK(result.blocked_command == 0 && result.commands_built == 1 && result.registered == 0);
        CHECK(result.atexit_records == 1);
        CHECK(objects.commands[0].base.registered == 1);
        CHECK(objects.commands[1].base.vptr == 0x5a5a5a5a5a5a5a5aULL);
        CHECK(objects.commands[2].base.vptr == 0x5a5a5a5a5a5a5a5aULL);
    }
    // A refusing guarded register stops at `help` with its flag still 0.
    {
        Slot567Objects objects;
        fill(objects);
        ConCmdRegistry registry;
        Slot567Hooks hooks = stub_hooks();
        hooks.register_guarded = &refusing_register_guarded;
        const Slot567Result result = init_slot567_235d0(objects, registry, hooks);
        CHECK(!result.ok && result.blocked == Slot567Stage::register_command);
        CHECK(result.blocked_command == 2 && result.commands_built == 3 && result.registered == 2);
        CHECK(objects.commands[2].base.registered == 0);
        CHECK(result.atexit_records == 3);
    }
}

void section_stage_names() {
    CHECK(std::strcmp(slot567_stage_name(Slot567Stage::none), "none") == 0);
    CHECK(std::strcmp(slot567_stage_name(Slot567Stage::hooks), "hooks") == 0);
    CHECK(std::strcmp(slot567_stage_name(Slot567Stage::cell), "cell") == 0);
    CHECK(std::strcmp(slot567_stage_name(Slot567Stage::cell_atexit), "cell_atexit") == 0);
    CHECK(std::strcmp(slot567_stage_name(Slot567Stage::build), "build") == 0);
    CHECK(std::strcmp(slot567_stage_name(Slot567Stage::pre_register), "pre_register") == 0);
    CHECK(std::strcmp(slot567_stage_name(Slot567Stage::register_command), "register_command") == 0);
    CHECK(std::strcmp(slot567_stage_name(Slot567Stage::scratch), "scratch") == 0);
    CHECK(std::strcmp(slot567_stage_name(Slot567Stage::release), "release") == 0);
    CHECK(std::strcmp(slot567_stage_name(Slot567Stage::command_atexit), "command_atexit") == 0);
    CHECK(std::strcmp(slot567_stage_name(Slot567Stage::canary), "canary") == 0);
}

}  // namespace

int main() {
    section_constants();
    section_layout();
    section_unwritten_bytes();
    section_construction_stub();
    section_default_hooks();
    section_fail_closed();
    section_stage_names();
    std::puts("ctor_slot_567: all checks passed");
    return 0;
}
