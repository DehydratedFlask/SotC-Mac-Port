// CPU-01-STARTUP-SLOT-510: fixture for startup constructor index 510 (0x211340).
//
// Proves the whole 0x278-byte block: the string property cell, the FollowGameCamera bool variable,
// the Debug Game toggle, the pose constant block, the SyncCameraAcrossTools command with its seven
// tokens, every documented hole, both fail-closed registration paths and a byte-identical re-run.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_510_211340.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kSentinel = 0x5a;

void fill(Slot510Objects& objects) {
    std::memset(static_cast<void*>(&objects), kSentinel, sizeof objects);
}

bool stub_register(ConCmdBase&, ConCmdRegistry&) noexcept { return true; }
bool stub_atexit(std::uint64_t, const void*, ConCmdRegistry&) noexcept { return true; }
bool stub_release(const SharedStr&) noexcept { return true; }
bool stub_scratch() noexcept { return true; }

bool stub_menu(DebugMenuItem& item, const SharedStr& label, const SharedStr& text,
               const DebugMenuTarget& target, std::uint32_t flags, DebugMenuRegistry&,
               ConCmdRegistry&) noexcept {
    // Construct only (no root registration), so the sentinel sweep sees the body/ctor stores only.
    return init_debug_menu_item_26ea0(item, label, text, target, flags);
}

Slot510Hooks stub_hooks() {
    Slot510Hooks hooks{};
    hooks.register_guarded = &stub_register;
    hooks.menu_toggle = &stub_menu;
    hooks.connection_token = &slot510_append_connection_token;
    hooks.axis_token = &slot510_append_axis_token;
    hooks.atexit = &stub_atexit;
    hooks.release_view = &stub_release;
    hooks.scratch_noop = &stub_scratch;
    return hooks;
}

bool refusing_register(ConCmdBase&, ConCmdRegistry&) noexcept { return false; }
bool refusing_menu(DebugMenuItem&, const SharedStr&, const SharedStr&, const DebugMenuTarget&,
                   std::uint32_t, DebugMenuRegistry&, ConCmdRegistry&) noexcept { return false; }
bool refusing_connection(Slot510TokenStore&, const SharedStr&, std::uint32_t) noexcept { return false; }

// Refuses the n-th register call (1-based). Used to fail the command registration after the var.
int g_register_call = 0;
bool nth_refusing_register(ConCmdBase&, ConCmdRegistry&) noexcept {
    return ++g_register_call != 2;
}
int g_axis_call = 0;
bool nth_refusing_axis(Slot510TokenStore& store, const SharedStr& name) noexcept {
    if (++g_axis_call == 3) return false;
    return slot510_append_axis_token(store, name);
}

void section_constants() {
    CHECK(kSlot510Rva == 0x211340 && kSlot510Size == 0x8b2 && kSlot510EndRva == 0x211bf2);
    CHECK(kSlot510Rva + kSlot510Size == kSlot510EndRva);
    CHECK(kSlot510TableWordRva == 0x2dd60f0 && kSlot510TableIndex == 510);
    CHECK(kSlot510InstructionCount == 377 && kSlot510DirectCallSites == 38 &&
          kSlot510IndirectCallSites == 2 && kSlot510BranchSites == 14);
    CHECK(kSlot510BlockRva == 0x2ee8438 && kSlot510BlockSize == 0x278);
    CHECK(kSlot510BlockRva + kSlot510BlockSize == 0x2ee86b0);
    CHECK(kSlot510BlockWritten == 0x211 && kSlot510BlockHoles == 0x67);
    CHECK(kSlot510CellVptr == 0x15976b0 && kSlot510VarVptr == 0x1594218);
    CHECK(kSlot510VarMidVptr == 0x15940b0 && kSlot510CommandVptr == 0x1594dd0);
    CHECK(kSlot510CellDtorRva == 0x8ef00 && kSlot510VarDtorRva == 0x20b0);
    CHECK(kSlot510ItemDtorRva == 0x272d0 && kSlot510CommandDtorRva == 0x4b0);
    CHECK(kSlot510VarNameLength == 16 && kSlot510GroupLength == 10 && kSlot510ToggleLength == 18);
    CHECK(kSlot510CommandNameLength == 21 && kSlot510CommandDescLength == 46);
    CHECK(kSlot510TokenCount == 7 && kSlot510TokenSize == 0x50);
    CHECK(kSlot510HandlerRva == 0x20e900 && kSlot510DelegateWord68 == 0x1990 &&
          kSlot510DelegateWord70 == 0x2512a0);
    CHECK(std::strcmp(kSlot510TokenName, "SendersConnectionType") == 0);
    CHECK(std::strcmp(kSlot510CommandName, "SyncCameraAcrossTools") == 0);
}

void section_layout() {
    CHECK(sizeof(Slot510Cell) == 0x30 && sizeof(Slot510Pose) == 0x30);
    CHECK(sizeof(Slot510Token) == 0x50 && sizeof(Slot510Command) == 0xa8);
    CHECK(sizeof(Slot510Objects) == 0x278);
    CHECK(offsetof(Slot510Objects, cell) == 0x000 && offsetof(Slot510Objects, var) == 0x030);
    CHECK(offsetof(Slot510Objects, item) == 0x0f8 && offsetof(Slot510Objects, pose) == 0x1a0);
    CHECK(offsetof(Slot510Objects, command) == 0x1d0);
    CHECK(offsetof(Slot510Command, delegate) == 0x60 && offsetof(Slot510Command, vector_begin) == 0x80);
    CHECK(offsetof(Slot510Command, vector_cap) == 0x90 && offsetof(Slot510Command, session_flag) == 0xa0);
    CHECK(offsetof(Slot510Token, name) == 0x08 && offsetof(Slot510Token, value) == 0x28);
}

void section_unwritten_bytes() {
    CHECK(slot510_byte_written(0x00) && slot510_byte_written(0x2f));          // cell
    CHECK(!slot510_byte_written(0x30 + 0x08) && !slot510_byte_written(0x30 + 0x17));
    CHECK(slot510_byte_written(0x30 + 0x18) && slot510_byte_written(0x30 + 0x1c));
    CHECK(!slot510_byte_written(0x30 + 0x19));
    CHECK(slot510_byte_written(0x30 + 0x68));
    CHECK(!slot510_byte_written(0x30 + 0x6d) && !slot510_byte_written(0x30 + 0x6f));
    CHECK(!slot510_byte_written(0x30 + 0x90) && !slot510_byte_written(0x30 + 0x9f));
    CHECK(!slot510_byte_written(0x30 + 0xa8) && !slot510_byte_written(0x30 + 0xaf));
    CHECK(!slot510_byte_written(0x30 + 0xc1) && !slot510_byte_written(0x30 + 0xc7));
    CHECK(!slot510_byte_written(0x0f8 + 0x21) && !slot510_byte_written(0x0f8 + 0x27));
    CHECK(!slot510_byte_written(0x0f8 + 0x4c) && !slot510_byte_written(0x0f8 + 0x4f));
    CHECK(!slot510_byte_written(0x19c) && !slot510_byte_written(0x19f));
    CHECK(slot510_byte_written(0x1a0) && slot510_byte_written(0x1cf));
    CHECK(!slot510_byte_written(0x1d0 + 0x98) && !slot510_byte_written(0x1d0 + 0x9f));
    CHECK(!slot510_byte_written(0x1d0 + 0xa0) && !slot510_byte_written(0x1d0 + 0xa7));
    CHECK(!slot510_byte_written(kSlot510BlockSize));
    std::size_t written = 0;
    for (std::size_t i = 0; i < kSlot510BlockSize; ++i) if (slot510_byte_written(i)) ++written;
    CHECK(written == kSlot510BlockWritten);
    CHECK(kSlot510BlockSize - written == kSlot510BlockHoles);
}

void section_construction_stub() {
    Slot510Objects objects;
    fill(objects);
    Slot510TokenStore tokens{};
    ConCmdRegistry registry;
    DebugMenuRegistry menu;
    const Slot510Hooks hooks = stub_hooks();
    const Slot510Result result = init_slot510_211340(objects, tokens, registry, menu, hooks);
    CHECK(result.ok && result.blocked == Slot510Stage::none);
    CHECK(result.property_atexit && result.toggle_ok && result.toggle_atexit);
    CHECK(result.pose_copied && result.command_built && result.delegate_rewritten);
    CHECK(result.reserved && result.tokens == 7 && result.registered && result.scratch_called);
    CHECK(result.views_inert && result.atexit_records == 4 && result.canary_checked);
    CHECK(registry.atexit_log.size() == 0 && registry.pending.size() == 0);

    // Holes keep the sentinel.
    const auto* bytes = reinterpret_cast<const std::uint8_t*>(&objects);
    std::size_t holes = 0;
    for (std::size_t i = 0; i < sizeof objects; ++i) {
        if (slot510_byte_written(i)) continue;
        ++holes;
        CHECK(bytes[i] == kSentinel);
    }
    CHECK(holes == kSlot510BlockHoles);

    CHECK(objects.cell.vptr == kSlot510CellVptr);
    CHECK(objects.cell.text.data == reinterpret_cast<const std::uint8_t*>(kSlot510Property));
    CHECK(objects.cell.text.size == static_cast<std::int64_t>(kSlot510PropertyLength));
    CHECK(objects.cell.text.aux == 0 && objects.cell.text.ctl == nullptr);
    CHECK(objects.cell.tail == 0);

    const ConCmdBoolVar& var = objects.var;
    CHECK(var.base.vptr == kSlot510VarVptr && var.base.flags == 1 && var.base.registered == 0);
    CHECK(var.base.name.data == reinterpret_cast<const std::uint8_t*>(kSlot510VarName));
    CHECK(var.base.name.size == static_cast<std::int64_t>(kSlot510VarNameLength));
    CHECK(var.base.description.size == 0);
    CHECK(var.base.description.ctl == nullptr);
    CHECK(var.value_ref == &var.value && var.value == 0 && var.default_value == 0);
    CHECK(var.fn_vptr == kSlot510VarFnVtable && var.fn_payload == 0 && var.fn_aux == 0);

    CHECK(objects.item.label.data == reinterpret_cast<const std::uint8_t*>(kSlot510Group));
    CHECK(objects.item.label.size == static_cast<std::int64_t>(kSlot510GroupLength));
    CHECK(objects.item.text.data == reinterpret_cast<const std::uint8_t*>(kSlot510Toggle));
    CHECK(objects.item.text.size == static_cast<std::int64_t>(kSlot510ToggleLength));
    CHECK(objects.item.target == &var && objects.item.flags == kSlot510ItemFlags);
    CHECK(objects.item.kind == 2 && objects.item.range_min == 1.0f && objects.item.range_max == 10.0f);
    CHECK(objects.item.link.self == nullptr);
    CHECK(objects.item.link.prev == reinterpret_cast<MenuLink*>(&objects.item));
    CHECK(objects.item.link.next == reinterpret_cast<MenuLink*>(&objects.item));

    for (int i = 0; i < 4; ++i) {
        CHECK(objects.pose.vec0[i] == (i == 0 ? 1.0f : 0.0f));
        CHECK(objects.pose.vec1[i] == (i == 0 ? 1.0f : 0.0f));
        CHECK(objects.pose.ids[i] == kSlot510PoseIds[i]);
    }

    const Slot510Command& command = objects.command;
    CHECK(command.base.vptr == kSlot510CommandVptr && command.base.flags == 2);
    CHECK(command.base.name.data == reinterpret_cast<const std::uint8_t*>(kSlot510CommandName));
    CHECK(command.base.name.size == static_cast<std::int64_t>(kSlot510CommandNameLength));
    CHECK(command.base.description.data == reinterpret_cast<const std::uint8_t*>(kSlot510CommandDesc));
    CHECK(command.base.description.size == static_cast<std::int64_t>(kSlot510CommandDescLength));
    CHECK(command.delegate.self == reinterpret_cast<std::uint64_t>(&command.delegate));
    CHECK(command.delegate.word68 == kSlot510DelegateWord68);
    CHECK(command.delegate.word70 == kSlot510DelegateWord70);
    CHECK(command.delegate.handler == kSlot510HandlerRva);
    CHECK(command.vector_begin == reinterpret_cast<std::uint64_t>(tokens.items.data()));
    CHECK(command.vector_end == command.vector_begin + kSlot510TokenCount * kSlot510TokenSize);
    CHECK(command.vector_cap == command.vector_end);

    CHECK(tokens.count == 7 && tokens.capacity == 7);
    CHECK(tokens.items[0].type == 1 && tokens.items[0].hint == 1);
    CHECK(tokens.items[0].name.data == reinterpret_cast<const std::uint8_t*>(kSlot510TokenName));
    CHECK(tokens.items[0].name.size == 21);
    CHECK(tokens.items[0].value.size == 4 && tokens.items[0].value.ctl == nullptr);
    CHECK(std::memcmp(tokens.items[0].value.data, "Tool", 4) == 0);
    for (std::size_t i = 1; i < 7; ++i) {
        CHECK(tokens.items[i].type == 3);
        CHECK(tokens.items[i].name.data ==
              reinterpret_cast<const std::uint8_t*>(kSlot510AxisNames[i - 1]));
        CHECK(tokens.items[i].name.size == 6);
        CHECK(tokens.items[i].value.data == nullptr && tokens.items[i].value.size == 0);
    }

    // Byte-identical re-run of the body's store program under the same stub hooks.
    const Slot510Objects first = objects;
    const std::array<Slot510Token, 7> first_tokens = tokens.items;
    const Slot510Result again = init_slot510_211340(objects, tokens, registry, menu, hooks);
    CHECK(again.ok && again.tokens == 7);
    CHECK(std::memcmp(&first, &objects, sizeof objects) == 0);
    CHECK(std::memcmp(first_tokens.data(), tokens.items.data(), sizeof first_tokens) == 0);
}

void section_default_hooks() {
    Slot510Objects objects;
    fill(objects);
    Slot510TokenStore tokens{};
    ConCmdRegistry registry;
    DebugMenuRegistry menu;
    const Slot510Result result =
        init_slot510_211340(objects, tokens, registry, menu, default_slot510_hooks());
    CHECK(result.ok && result.blocked == Slot510Stage::none);
    CHECK(result.atexit_records == 4 && result.registered && result.var.registration.ok);
    CHECK(registry.pending.size() == 2);           // FollowGameCamera + SyncCameraAcrossTools
    CHECK(menu.root.count == 1 && objects.item.registered == 0);   // root path never sets +0x20
    CHECK(objects.var.base.registered == 1 && objects.command.base.registered == 1);
    CHECK(registry.atexit_log.size() == 5);        // 4 body records + the menu root's 0x27280
    CHECK(registry.atexit_log.at(0).dtor_rva == kSlot510CellDtorRva);
    CHECK(registry.atexit_log.at(1).dtor_rva == kSlot510VarDtorRva);
    CHECK(registry.atexit_log.at(2).dtor_rva == 0x27280);
    CHECK(registry.atexit_log.at(3).dtor_rva == kSlot510ItemDtorRva);
    CHECK(registry.atexit_log.at(4).dtor_rva == kSlot510CommandDtorRva);
    CHECK(registry.atexit_log.at(0).object == &objects.cell);
    CHECK(registry.atexit_log.at(1).object == &objects.var);
    CHECK(registry.atexit_log.at(3).object == &objects.item);
    CHECK(registry.atexit_log.at(4).object == &objects.command);
}

void section_fail_closed() {
    {   // Missing hooks: nothing at all.
        Slot510Objects objects;
        fill(objects);
        Slot510TokenStore tokens{};
        ConCmdRegistry registry;
        DebugMenuRegistry menu;
        const Slot510Objects first = objects;
        const Slot510Result result = init_slot510_211340(objects, tokens, registry, menu, Slot510Hooks{});
        CHECK(!result.ok && result.blocked == Slot510Stage::hooks);
        CHECK(std::memcmp(&first, &objects, sizeof objects) == 0);
        CHECK(registry.pending.size() == 0 && registry.atexit_log.size() == 0 && menu.root.count == 0);
    }
    {   // Refused var registration.
        Slot510Objects objects;
        fill(objects);
        Slot510TokenStore tokens{};
        ConCmdRegistry registry;
        DebugMenuRegistry menu;
        Slot510Hooks hooks = stub_hooks();
        hooks.register_guarded = &refusing_register;
        const Slot510Result result = init_slot510_211340(objects, tokens, registry, menu, hooks);
        CHECK(!result.ok && result.blocked == Slot510Stage::var_register);
        CHECK(result.atexit_records == 1 && objects.cell.vptr == kSlot510CellVptr);
        CHECK(objects.var.base.vptr == kSlot510VarVptr && objects.var.value == 0);
        CHECK(objects.item.flags == 0x5a5a5a5au);
    }
    {   // Refused command registration (the var registration succeeds).
        Slot510Objects objects;
        fill(objects);
        Slot510TokenStore tokens{};
        ConCmdRegistry registry;
        DebugMenuRegistry menu;
        g_register_call = 0;
        Slot510Hooks hooks = stub_hooks();
        hooks.register_guarded = &nth_refusing_register;
        const Slot510Result result = init_slot510_211340(objects, tokens, registry, menu, hooks);
        CHECK(!result.ok && result.blocked == Slot510Stage::register_command);
        CHECK(result.tokens == 7 && result.atexit_records == 3);
        CHECK(objects.command.base.name.data ==
              reinterpret_cast<const std::uint8_t*>(kSlot510CommandName));
    }
    {   // Refused toggle registration.
        Slot510Objects objects;
        fill(objects);
        Slot510TokenStore tokens{};
        ConCmdRegistry registry;
        DebugMenuRegistry menu;
        Slot510Hooks hooks = stub_hooks();
        hooks.menu_toggle = &refusing_menu;
        const Slot510Result result = init_slot510_211340(objects, tokens, registry, menu, hooks);
        CHECK(!result.ok && result.blocked == Slot510Stage::toggle);
        CHECK(objects.var.base.registered == 0 && result.atexit_records == 2);
    }
    {   // Refused connection token.
        Slot510Objects objects;
        fill(objects);
        Slot510TokenStore tokens{};
        ConCmdRegistry registry;
        DebugMenuRegistry menu;
        Slot510Hooks hooks = stub_hooks();
        hooks.connection_token = &refusing_connection;
        const Slot510Result result = init_slot510_211340(objects, tokens, registry, menu, hooks);
        CHECK(!result.ok && result.blocked == Slot510Stage::token_connection);
        CHECK(result.tokens == 0 && tokens.count == 0);
    }
    {   // Refused third axis token.
        Slot510Objects objects;
        fill(objects);
        Slot510TokenStore tokens{};
        ConCmdRegistry registry;
        DebugMenuRegistry menu;
        g_axis_call = 0;
        Slot510Hooks hooks = stub_hooks();
        hooks.axis_token = &nth_refusing_axis;
        const Slot510Result result = init_slot510_211340(objects, tokens, registry, menu, hooks);
        CHECK(!result.ok && result.blocked == Slot510Stage::token_axes);
        CHECK(result.tokens == 3 && tokens.count == 3);   // connection + transX + transY
    }
    {   // Refused 0xf590 release on the property view.
        Slot510Objects objects;
        fill(objects);
        Slot510TokenStore tokens{};
        ConCmdRegistry registry;
        DebugMenuRegistry menu;
        Slot510Hooks hooks = stub_hooks();
        hooks.release_view = [](const SharedStr&) noexcept { return false; };
        const Slot510Result result = init_slot510_211340(objects, tokens, registry, menu, hooks);
        CHECK(!result.ok && result.blocked == Slot510Stage::property_cell);
    }
}

void section_stage_names() {
    CHECK(std::strcmp(slot510_stage_name(Slot510Stage::hooks), "hooks") == 0);
    CHECK(std::strcmp(slot510_stage_name(Slot510Stage::var_register), "var_register") == 0);
    CHECK(std::strcmp(slot510_stage_name(Slot510Stage::toggle), "toggle") == 0);
    CHECK(std::strcmp(slot510_stage_name(Slot510Stage::token_connection), "token_connection") == 0);
    CHECK(std::strcmp(slot510_stage_name(Slot510Stage::register_command), "register_command") == 0);
    CHECK(std::strcmp(slot510_stage_name(Slot510Stage::canary), "canary") == 0);
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
    std::puts("ctor_slot_510: all checks passed");
    return 0;
}
