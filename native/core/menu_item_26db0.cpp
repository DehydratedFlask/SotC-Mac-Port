#include "menu_item_26db0.hpp"

#include "init_sub_3be930.hpp"

namespace sotc {

bool init_debug_menu_item_26db0(DebugMenuItem& item, const SharedStr& parent, const ConCmdBase& target_base,
                                const DebugMenuTarget& target, std::uint32_t flags) noexcept {
    // The two `call [vptr + 0x18/0x30]` run unlifted code for any class but a known one.
    if (!target.known) return false;
    item.link.self = nullptr;              // 0x26dc3
    item.link.prev = &item.link;           // 0x26dca
    item.link.next = &item.link;           // 0x26dce
    item.link.owner = nullptr;             // 0x26dd2
    item.registered = 0;                   // 0x26dda
    item.label = parent;                   // 0x26dde..0x26def: vmovups + two qwords
    concmd_acquire_view(parent);           // 0x26df7: lock inc [ctl]
    item.kind = kDebugMenuItemKind;        // 0x26e00
    item.text = target_base.name;          // 0x26e07..0x26e1a: [rdx+0x20..0x40)
    concmd_acquire_view(target_base.name); // 0x26e22
    init_slot_f180(&item.empty);           // 0x26e2f
    item.target = &target_base;            // 0x26e34
    item.range_min = 1.0f;                 // 0x26e3b
    item.range_max = 10.0f;                // 0x26e45
    item.flags = flags;                    // 0x26e4f
    if (target.slot3 == 1 && target.slot6 == 3) {   // 0x26e5f / 0x26e6d
        item.range_min = 0.1f;             // 0x3dcccccd
        item.range_max = 0.5f;             // 0x3f000000
    }
    return true;
}

}  // namespace sotc
