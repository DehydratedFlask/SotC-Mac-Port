#include "fmenu_ctors_slot306.hpp"

#include "init_sub_3be930.hpp"

namespace sotc {

namespace {

// Stores common to the first 0x30 bytes of all three ctors (link, registered byte). Returns nothing;
// the view copies differ per shape and follow in the callers in the original order.
void link_header(DebugMenuItem& item) noexcept {
    item.link.self = nullptr;            // movq $0,(this)
    item.link.prev = &item.link;         // movq this,8(this)
    item.link.next = &item.link;         // movq this,0x10(this)
    item.link.owner = nullptr;           // movq $0,0x18(this)
    item.registered = 0;                 // movb $0,0x20(this)
}

// Tail common to the three ctors: target, the shared {1.0, 10.0} range, flags. The original performs
// `leaq [0x2dfd4b8]` then stores target/range/flags and finally `mov rsi,[parent cell]; jmp 0x268b0`;
// registration itself is a separate native call here.
void tail_stores(DebugMenuItem& item, const void* target, std::uint32_t flags) noexcept {
    item.target = target;                // qword [this+0x90]
    item.range_min = 1.0f;               // dword [this+0x98] = 0x3f800000
    item.range_max = 10.0f;              // dword [this+0x9c] = 0x41200000
    item.flags = flags;                  // dword [this+0xa0]
}

}  // namespace

bool init_menu_group_26c10(DebugMenuItem& item, const SharedStr& parent, const SharedStr& caption,
                           std::uint32_t flags) noexcept {
    link_header(item);
    item.label = parent;                 // vmovups/qword copies to +0x28..+0x47
    concmd_acquire_view(parent);
    item.kind = kMenuKindGroup;          // movl $1,0x48(this)
    item.text = caption;                 // +0x50..+0x6f
    concmd_acquire_view(caption);
    item.empty = caption;                // +0x70..+0x8f: the caption is copied a second time
    concmd_acquire_view(caption);
    tail_stores(item, nullptr, flags);   // movq $0,0x90(this)
    return true;
}

bool init_menu_separator_26b60(DebugMenuItem& item, const SharedStr& parent, std::uint32_t flags,
                               const SharedStrCtorHooks& hooks) noexcept {
    // 0xf1a0(this+0x50, "MenuSeparator"): the only allocating step; done first so that a failure
    // leaves the item untouched.
    SharedStr text{};
    if (!init_shared_str_f1a0(text, kMenuSeparatorText, hooks)) return false;

    link_header(item);
    item.label = parent;
    concmd_acquire_view(parent);
    item.kind = kMenuKindSeparator;      // movl $3,0x48(this) (stored before the 0xf1a0 call)
    item.text = text;                    // written by 0xf1a0
    init_slot_f180(&item.empty);         // call 0xf180(this+0x70)
    tail_stores(item, nullptr, flags);
    return true;
}

bool init_menu_command_26f90(DebugMenuItem& item, const SharedStr& parent, const SharedStr& caption,
                             const void* target, const SharedStr& command, std::uint32_t flags) noexcept {
    link_header(item);
    item.label = parent;
    concmd_acquire_view(parent);
    item.kind = kMenuKindCommand;        // movl $2,0x48(this)
    item.text = caption;
    concmd_acquire_view(caption);
    item.empty = command;                // +0x70: the console command name (r8)
    concmd_acquire_view(command);
    tail_stores(item, target, flags);    // movq rcx,0x90(this)
    return true;
}

namespace {

// One gated-log site. Returns false when the call must stop (fail closed or hook failure).
bool gate(MenuRegisterResult& out, const DebugMenuAssertHooks& hooks, const DebugMenuItem& item,
          MenuAssertSite site, std::uint32_t line, std::uint64_t message_rva) noexcept {
    if (hooks.log_active == nullptr) {
        out.blocked_site = site;
        out.path = DebugMenuRegisterPath::unmodelled_assert;
        return false;
    }
    if (!hooks.log_active()) return true;      // `je` to the next check: no log
    if (hooks.emit == nullptr) {
        out.blocked_site = site;
        out.path = DebugMenuRegisterPath::unmodelled_assert;
        return false;
    }
    MenuAssertEvent ev;
    ev.site = site;
    ev.line = line;
    ev.message_rva = message_rva;
    ev.item = &item;
    if (!hooks.emit(ev)) {
        out.blocked_site = site;
        out.path = DebugMenuRegisterPath::unmodelled_assert;
        return false;
    }
    ++out.asserts_emitted;
    return true;
}

}  // namespace

MenuRegisterResult register_menu_item_268b0(DebugMenuItem& item, DebugMenuRegistry& menu,
                                            StartupAtexitLog& atexit,
                                            const DebugMenuAssertHooks& hooks) noexcept {
    MenuRegisterResult out{};

    // 0x268d4: cmp byte [rbx+0x20],0; jne end
    if (item.registered != 0) {
        out.ok = true;
        out.path = DebugMenuRegisterPath::already_registered;
        return out;
    }

    // 0x268de: site A.
    if ((item.flags & kMenuAssertFlagsMaskA) != 0 &&
        !gate(out, hooks, item, MenuAssertSite::a_flags_c0, kMenuAssertLineA, kMenuAssertTextRvaA)) {
        return out;
    }
    // 0x26962: kind == 0 skips straight to the parent test (0x26a7e).
    if (item.kind != 0) {
        // 0x2696d: site B.
        if ((item.flags & kMenuAssertFlagsMaskB) != 0 &&
            !gate(out, hooks, item, MenuAssertSite::b_flags_f00, kMenuAssertLineB, kMenuAssertTextRvaB)) {
            return out;
        }
        // 0x269f0: kind-1 branch (kind 2 and 3 fall through to 0x26a7e).
        if (item.kind == kMenuKindGroup && (item.flags & kMenuKind1FlagsMask) != kMenuKind1FlagsOk &&
            !gate(out, hooks, item, MenuAssertSite::c_kind1_flags, kMenuAssertLineC, kMenuAssertTextRvaC)) {
            return out;
        }
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
        root.link.self = nullptr;
        root.link.prev = &root.link;
        root.link.next = &root.link;
        root.link.owner = nullptr;
        root.count = 0;
        out.atexit_recorded = atexit.record(kDebugMenuRootDtorRva, &root);
        menu.root_guard_done = true;
        out.root_initialised = true;
        if (!out.atexit_recorded) return out;
    }

    // 0x26b0d..0x26b3b: append at the tail.
    MenuLink* const rootp = &root.link;
    MenuLink* const node = &item.link;
    node->self = node;
    node->next = rootp;
    MenuLink* const tail = rootp->prev;
    node->prev = tail;
    rootp->prev = node;
    tail->next = node;
    ++root.count;
    node->owner = rootp;
    out.ok = true;
    return out;
}

bool record_menu_item_dtor_272d0(StartupAtexitLog& atexit, const DebugMenuItem& item) noexcept {
    return atexit.record(kDebugMenuItemDtorRva, &item);
}

}  // namespace sotc
