#include "debug_menu_item.hpp"

#include <cstring>

#include "init_sub_3be930.hpp"

namespace sotc {

DebugMenuTarget debug_menu_target_for(const ConCmdBoolVar& var) noexcept {
    DebugMenuTarget out{};
    out.object = &var;
    if (var.base.vptr == kBoolVarFinalVptr) {
        out.slot3 = kBoolVarSlot3Result;   // 0xb890: mov eax,1; ret
        out.slot6 = kBoolVarSlot6Result;   // 0xb930: xor eax,eax; ret
        out.known = true;
    }
    return out;
}

namespace {

// Stores shared by 0x27050 and 0x26ea0 up to (not including) the range words.
void construct_common(DebugMenuItem& item, const SharedStr& label, const SharedStr& text,
                      const void* target, std::uint32_t flags) noexcept {
    // [0]=0; [8]=this; [0x10]=this; [0x18]=0; byte [0x20]=0.
    item.link.self = nullptr;
    item.link.prev = &item.link;
    item.link.next = &item.link;
    item.link.owner = nullptr;
    item.registered = 0;
    // [0x28..0x48) = *label (+ lock inc ctl); dword [0x48] = 2; [0x50..0x70) = *text (+ lock inc ctl).
    item.label = label;
    concmd_acquire_view(label);
    item.kind = kDebugMenuItemKind;
    item.text = text;
    concmd_acquire_view(text);
    // call 0xf180(this+0x70).
    init_slot_f180(&item.empty);
    item.target = target;     // [0x90]
    item.flags = flags;       // [0xa0] (the original stores it after the range words; independent)
}

}  // namespace

bool init_debug_menu_item_27050(DebugMenuItem& item, const SharedStr& label, const SharedStr& text,
                                const void* target, std::uint32_t flags, float range_min,
                                float range_max) noexcept {
    construct_common(item, label, text, target, flags);
    item.range_min = range_min;   // [0x98] = xmm0
    item.range_max = range_max;   // [0x9c] = xmm1
    return true;
}

bool init_debug_menu_item_26ea0(DebugMenuItem& item, const SharedStr& label, const SharedStr& text,
                                const DebugMenuTarget& target, std::uint32_t flags) noexcept {
    // The two `call [vptr + 0x18/0x30]` run unlifted code for any class but the known one.
    if (!target.known) return false;
    construct_common(item, label, text, target.object, flags);
    item.range_min = 1.0f;        // dword [0x98] = 0x3f800000
    item.range_max = 10.0f;       // dword [0x9c] = 0x41200000
    // 0x26f44..0x26f6a: if (slot3 == 1 && slot6 == 3) { [0x98] = 0x3f0000003dcccccd }.
    if (target.slot3 == 1 && target.slot6 == 3) {
        item.range_min = 0.1f;    // 0x3dcccccd
        item.range_max = 0.5f;    // 0x3f000000
    }
    return true;
}

DebugMenuRegisterResult register_debug_menu_item_268b0(DebugMenuItem& item, DebugMenuRegistry& menu,
                                                       StartupAtexitLog& atexit) noexcept {
    DebugMenuRegisterResult out{};

    // 0x268d4: cmp byte [rbx+0x20],0; jne end
    if (item.registered != 0) {
        out.ok = true;
        out.path = DebugMenuRegisterPath::already_registered;
        return out;
    }

    // 0x268de: test byte [rbx+0xa0],0xc0 -> assert path A (engine-state compare + 0x3e7e0 log).
    // 0x26962/0x2696d: kind == 0 skips; test byte [rbx+0xa1],0xf -> assert path B.
    // 0x269f0: kind == 1 block. None of these is lifted.
    if ((item.flags & 0xc0u) != 0 || (item.kind != 0 && (item.flags & 0xf00u) != 0) ||
        item.kind == 1) {
        out.path = DebugMenuRegisterPath::unmodelled_assert;
        return out;
    }

    // 0x26a7e: test r14,r14 (parent).
    if (menu.parent.parent != nullptr) {
        out.path = DebugMenuRegisterPath::parent;
        if (menu.parent.insert == nullptr) return out;          // contract missing
        if (!menu.parent.insert(menu.parent.parent, item)) return out;
        item.registered = 1;                                      // 0x26a92
        out.ok = true;
        return out;
    }

    // 0x26a9b..0x26b08: lazy guarded root initialisation + __cxa_atexit(0x27280, root).
    out.path = DebugMenuRegisterPath::root_list;
    DebugMenuRoot& root = menu.root;
    if (!menu.root_guard_done) {
        root.link.self = nullptr;            // [0x2dfd4c0] = 0
        root.link.prev = &root.link;         // [0x2dfd4c8]
        root.link.next = &root.link;         // [0x2dfd4d0]
        root.link.owner = nullptr;           // [0x2dfd4d8] = 0
        root.count = 0;                      // dword [0x2dfd4e0] = 0
        out.atexit_recorded = atexit.record(kDebugMenuRootDtorRva, &root);
        menu.root_guard_done = true;
        out.root_initialised = true;
        if (!out.atexit_recorded) return out;
    }

    // 0x26b0d..0x26b3b: append at the tail.
    MenuLink* const rootp = &root.link;
    MenuLink* const node = &item.link;
    node->self = node;                       // [rbx] = rbx
    node->next = rootp;                      // [rbx+0x10] = &root
    MenuLink* const tail = rootp->prev;      // rcx = [0x2dfd4c8]
    node->prev = tail;                       // [rbx+8] = rcx
    rootp->prev = node;                      // [0x2dfd4c8] = rbx
    tail->next = node;                       // [[rbx+8]+0x10] = rbx
    ++root.count;                            // inc dword [0x2dfd4e0]
    node->owner = rootp;                     // [rbx+0x18] = &root
    out.ok = true;
    return out;
}

}  // namespace sotc
