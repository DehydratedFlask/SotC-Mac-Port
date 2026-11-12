// CPU-01-AOT-CFG-SCALE-01-BATCH24 (slot 452): fixture for startup constructor index 452 (0x498640).
// Asserts the node/cell preamble, the four 0xa0-byte command objects (name/description lengths,
// flags, vptr, the +0x60 delegate with its self address, the handler tags), registration, the seven
// __cxa_atexit records in body order, and the one toggle whose target is command 3.
#include "l4_test_util.hpp"

#include <cstring>

#include "core/ctor_slot_452_498640.hpp"

namespace {
using namespace l4;
using namespace sotc;

void section_constants() {
    CHECK(kSlot452Rva == 0x498640 && kSlot452Size == 1235);
    CHECK(kSlot452TableWord == 0x2dd70e0 - 8 * 452);
    CHECK(kSlot452TableWord == 0x2dd62c0);
    CHECK(kSlot452NodeObject == 0x2ef8bf0 && kSlot452NodeCell == 0x2ef8bf8);
    CHECK(kSlot452Cell == 0x2ef8c08 && kSlot452Object0 == 0x2ef8c10);
    CHECK(kSlot452CommandCount == 4 && kSlot452CommandStride == 0xa0);
    CHECK(kSlot452Object0 + kSlot452CommandCount * kSlot452CommandStride == kSlot452Toggle);
    CHECK(kSlot452Toggle == 0x2ef8e90);
    CHECK(kSlot452NodeSize == 0x48 && sizeof(Slot452Node) == 0x48);
    CHECK(kSlot452CanaryCell == 0x15fe518 && kSlot452CanaryFrameOffset == 0x80 && kSlot452FrameBytes == 0xa0);
    CHECK(kSlot452StackChkFailRva == 0xf8f3a0 && kSlot452AllocRva == 0x17070);
    CHECK(kSlot452AtexitRva == 0xf8f390 && kSlot452DsoHandle == 0x1604218);
    CHECK(kSlot452NodeDtor == 0x497610 && kSlot452CellDtor == 0x493190);
    CHECK(kSlot452CommandDtor == 0x4b0 && kSlot452ItemDtor == 0x272d0);
    CHECK(kSlot452ToggleFlags == 0x30 && kSlot452ToggleTarget == 3);

    // The site table: distinct RVAs, ascending objects, the pinned lengths.
    for (std::size_t i = 0; i < kSlot452CommandCount; ++i) {
        const Slot452CommandSite& s = kSlot452CommandSites[i];
        CHECK(std::strlen(s.name) == s.name_length);
        CHECK(std::strlen(s.description) == s.description_length);
        CHECK(s.object_rva == kSlot452Object0 + i * kSlot452CommandStride);
        CHECK(s.handler_rva != 0);
        if (i > 0) CHECK(s.name_rva > kSlot452CommandSites[i - 1].name_rva);
    }
    CHECK(kSlot452CommandSites[0].name_length == 18 && kSlot452CommandSites[0].description_length == 127);
    CHECK(kSlot452CommandSites[1].name_length == 23 && kSlot452CommandSites[1].description_length == 60);
    CHECK(kSlot452CommandSites[2].name_length == 25 && kSlot452CommandSites[2].description_length == 71);
    CHECK(kSlot452CommandSites[3].name_length == 26 && kSlot452CommandSites[3].description_length == 30);
    CHECK(std::strlen(kSlot452ToggleParent) == kSlot452ToggleParentLength);
    CHECK(std::strlen(kSlot452ToggleText) == kSlot452ToggleTextLength);

    // The four handler tags are distinct.
    for (std::size_t i = 0; i < kSlot452CommandCount; ++i)
        for (std::size_t j = i + 1; j < kSlot452CommandCount; ++j)
            CHECK(kSlot452CommandSites[i].handler_rva != kSlot452CommandSites[j].handler_rva);
}

void run_full() {
    ConCmdRegistry registry;
    DebugMenuRegistry menu;
    Slot452Objects o;
    poison(o);
    const Slot452Result res = init_slot452_498640(o, registry, menu);

    CHECK(res.ok);
    CHECK(res.canary_matched && res.node_linked);
    CHECK(res.commands_built == kSlot452CommandCount && res.menu_items == 1);
    // Seven body records. The eighth is the menu root's lazy guard (0x27280), which a fresh registry
    // raises on the first registration; in the full startup it is already done by earlier slots.
    CHECK(res.atexit_records == 7 && registry.atexit_log.size() == 8);
    CHECK(registry.atexit_log.at(6).dtor_rva == kDebugMenuRootDtorRva);
    CHECK(registry.atexit_log.at(7).dtor_rva == kSlot452ItemDtor);

    // 1. node: self links, the two flags, the pointer stored at +0x08 of the object.
    const auto* node = reinterpret_cast<const Slot452Node*>(static_cast<std::uintptr_t>(o.node_head[1]));
    CHECK(node == &o.node);
    CHECK(node->self0 == o.node_head[1] && node->self8 == o.node_head[1] && node->self10 == o.node_head[1]);
    CHECK(node->flag18 == 1 && node->flag19 == 1);
    CHECK(registry.atexit_log.at(0).dtor_rva == kSlot452NodeDtor);
    CHECK(registry.atexit_log.at(0).object == &o.node_head);

    // 2. cell.
    CHECK(o.cell == 0);
    CHECK(registry.atexit_log.at(1).dtor_rva == kSlot452CellDtor);
    CHECK(registry.atexit_log.at(1).object == &o.cell);

    // 3. commands: base fields, delegate, registration, atexit rows 2..5.
    for (std::size_t i = 0; i < kSlot452CommandCount; ++i) {
        const Slot452CommandSite& s = kSlot452CommandSites[i];
        const ConCmdCommand& cmd = o.commands[i];
        CHECK(cmd.base.vptr == kFConCmdVptr);
        CHECK(cmd.base.flags == 2);
        CHECK(cmd.base.registered == 1);
        CHECK(cmd.base.name.size == static_cast<std::int64_t>(s.name_length));
        CHECK(std::memcmp(cmd.base.name.data, s.name, s.name_length) == 0);
        CHECK(cmd.base.description.size == static_cast<std::int64_t>(s.description_length));
        CHECK(std::memcmp(cmd.base.description.data, s.description, s.description_length) == 0);
        CHECK(cmd.delegate.fn == kFConCmdInvokeThunkRva);
        CHECK(cmd.delegate.adj == 0);
        CHECK(cmd.delegate.handler == s.handler_rva);
        CHECK(cmd.delegate.object == reinterpret_cast<std::uint64_t>(&cmd.delegate));   // self-fixed
        CHECK(cmd.args_begin == nullptr && cmd.args_end == nullptr && cmd.args_cap == nullptr);
        CHECK(registry.atexit_log.at(2 + i).dtor_rva == kSlot452CommandDtor);
        CHECK(registry.atexit_log.at(2 + i).object == &cmd);
    }

    // 4. toggle: parent and text views, flags, target = command 3, atexit row 6.
    CHECK(o.item.label.size == static_cast<std::int64_t>(kSlot452ToggleParentLength));
    CHECK(std::memcmp(o.item.label.data, kSlot452ToggleParent, kSlot452ToggleParentLength) == 0);
    CHECK(o.item.text.size == static_cast<std::int64_t>(kSlot452ToggleTextLength));
    CHECK(std::memcmp(o.item.text.data, kSlot452ToggleText, kSlot452ToggleTextLength) == 0);
    CHECK(registry.atexit_log.at(7).dtor_rva == kSlot452ItemDtor);
    CHECK(registry.atexit_log.at(7).object == &o.item);
}

void run_target() {
    ConCmdCommand cmd{};
    const DebugMenuTarget target = slot452_toggle_target(cmd);
    CHECK(target.object == &cmd);
    CHECK(target.slot3 == kSlot452ToggleGate3 && target.slot3 == 0x22ae0);
    CHECK(target.slot6 == 0);      // unobserved: slot 3 returns 0, so slot 6 is never read
    CHECK(target.known);
}

void run_hook_failure() {
    ConCmdRegistry registry;
    DebugMenuRegistry menu;
    Slot452Objects o;
    poison(o);
    Slot452Hooks hooks{};
    hooks.allocate = [](std::size_t) noexcept -> void* { return nullptr; };
    const Slot452Result res = init_slot452_498640(o, registry, menu, hooks);
    CHECK(!res.ok && !res.node_linked);
    CHECK(res.atexit_records == 0 && registry.atexit_log.size() == 0);
}

void run_custom_allocator() {
    ConCmdRegistry registry;
    DebugMenuRegistry menu;
    Slot452Objects o;
    poison(o);
    static Slot452Node external{};
    Slot452Hooks hooks{};
    hooks.allocate = [](std::size_t size) noexcept -> void* {
        return size == kSlot452NodeSize ? static_cast<void*>(&external) : nullptr;
    };
    const Slot452Result res = init_slot452_498640(o, registry, menu, hooks);
    CHECK(res.ok && res.node_linked);
    CHECK(external.self0 == reinterpret_cast<std::uint64_t>(&external));
    CHECK(o.node_head[1] == reinterpret_cast<std::uint64_t>(&external));
    // The slot's own storage is untouched when the hook supplies the node.
    CHECK(reinterpret_cast<void*>(o.node_head[1]) != static_cast<void*>(&o.node));
}

}  // namespace

int main() {
    section_constants();
    run_full();
    run_target();
    run_hook_failure();
    run_custom_allocator();
    std::printf("OK test_ctor_slot_452\n");
    return 0;
}
