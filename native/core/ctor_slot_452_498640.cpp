// CPU-01-AOT-CFG-SCALE-01-BATCH24 (slot 452): see ctor_slot_452_498640.hpp.
#include "ctor_slot_452_498640.hpp"

#include <cstring>

namespace sotc {

DebugMenuTarget slot452_toggle_target(const ConCmdCommand& command) noexcept {
    DebugMenuTarget out{};
    out.object = &command;
    // [0x1594dc0 + 0x18] = 0x22ae0 `xor eax,eax; ret`: the `slot3 == 1` narrowing never holds, so the
    // original never reads slot 6 and its value is unobserved.
    out.slot3 = kSlot452ToggleGate3;
    out.slot6 = 0;
    out.known = true;
    return out;
}

Slot452Result init_slot452_498640(Slot452Objects& objects, ConCmdRegistry& registry,
                                  DebugMenuRegistry& menu, const Slot452Hooks& hooks) noexcept {
    Slot452Result out{};

    // ---- 1. node 0x2ef8bf0 (0x498658..0x4986b5) -----------------------------------------------
    objects.node_head[1] = 0;
    objects.node_head[2] = 0;
    void* storage = hooks.allocate != nullptr ? hooks.allocate(kSlot452NodeSize) : &objects.node;
    if (storage == nullptr) return out;
    auto* node = static_cast<Slot452Node*>(storage);
    node->self0 = reinterpret_cast<std::uint64_t>(node);
    node->self8 = reinterpret_cast<std::uint64_t>(node);
    node->self10 = reinterpret_cast<std::uint64_t>(node);
    node->flag18 = 1;
    node->flag19 = 1;
    std::memset(node->pad, 0, sizeof node->pad);
    objects.node_head[1] = reinterpret_cast<std::uint64_t>(node);
    out.node_linked = true;
    if (!registry.atexit_log.record(kSlot452NodeDtor, &objects.node_head)) return out;
    ++out.atexit_records;

    // ---- 2. cell 0x2ef8c08 (0x4986ba..0x4986d2) -----------------------------------------------
    objects.cell = 0;
    if (!registry.atexit_log.record(kSlot452CellDtor, &objects.cell)) return out;
    ++out.atexit_records;

    // ---- 3. four command objects (0x4986d7..0x498a61) ------------------------------------------
    for (std::size_t i = 0; i < kSlot452CommandCount; ++i) {
        const Slot452CommandSite& site = kSlot452CommandSites[i];
        const FConCmdCommandSpec spec{site.name, site.description, site.description_length, site.handler_rva};
        const FConCmdBuildResult built = init_fconcmd_object(objects.commands[i], spec, registry);
        if (!built.ok) return out;
        ++out.commands_built;
        if (built.registration.path != ConCmdRegisterPath::already_registered) ++out.registrations;
        ++out.atexit_records;   // the helper records atexit(0x4b0, object)
    }

    // ---- 4. toggle 0x2ef8e90 (0x498a66..0x498ae5) ----------------------------------------------
    const FamilyItemResult item =
        family_toggle_item_26ea0(objects.item, kSlot452ToggleParent, kSlot452ToggleParentLength,
                                 kSlot452ToggleText, kSlot452ToggleTextLength,
                                 slot452_toggle_target(objects.commands[kSlot452ToggleTarget]),
                                 kSlot452ToggleFlags, registry, menu);
    if (!item.ok()) return out;
    ++out.menu_items;
    ++out.atexit_records;

    // The original's canary compare has no native counterpart: a native function has no cookie, so
    // the two guard words are the same local and the compare is satisfied by construction.
    out.canary_matched = true;
    out.ok = out.commands_built == kSlot452CommandCount && out.atexit_records == 7 && out.menu_items == 1;
    return out;
}

}  // namespace sotc
