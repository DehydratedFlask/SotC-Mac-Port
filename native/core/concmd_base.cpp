#include "concmd_base.hpp"

#include <new>

namespace sotc {

void concmd_acquire_view(const SharedStr& view) noexcept {
    // 0x20b1e / 0x20b40: `test rax,rax; je skip; lock inc qword [rax]`.
    if (view.ctl != nullptr) {
        std::atomic_ref<std::int64_t>(*view.ctl).fetch_add(1);
    }
}

void init_concmd_base_20af0(ConCmdBase& cmd, const SharedStr& name, const SharedStr& description,
                            std::uint32_t flags) noexcept {
    cmd.vptr = kConCmdBaseVptr;      // lea rax,[0x1594d80]; add rax,0x10; mov [rdi],rax
    cmd.registered = 0;              // mov byte [rdi+0x18],0
    cmd.flags = flags;               // mov [rdi+0x1c],ecx
    cmd.name = name;                 // vmovups [rdi+0x20] / +0x30 / +0x38 (ctl)
    concmd_acquire_view(name);
    cmd.description = description;   // vmovups [rdi+0x40] / +0x50 / +0x58 (ctl)
    concmd_acquire_view(description);
}

bool StartupAtexitLog::record(std::uint64_t dtor_rva, const void* object) noexcept {
    try {
        entries_.push_back(StartupAtexitEntry{dtor_rva, object});
    } catch (const std::bad_alloc&) {
        return false;
    }
    return true;
}

std::size_t ConCmdPendingList::size() const noexcept {
    if (head.next == nullptr) return 0;
    std::size_t n = 0;
    for (const ConCmdListNode* node = head.next; node != &head; node = node->next) ++n;
    return n;
}

ConCmdRegisterResult register_concmd_20d60(ConCmdBase& cmd, ConCmdRegistry& registry) noexcept {
    ConCmdRegisterResult out{};

    // 0x20d60: cmp byte [rdi+0x18],0; je continue; ret
    if (cmd.registered != 0) {
        out.ok = true;
        out.path = ConCmdRegisterPath::already_registered;
        return out;
    }
    cmd.registered = 1;   // mov byte [rdi+0x18],1

    // 0x20d72: rsi = [0x2dfcbc8]; jmp 0x20d80
    if (registry.manager.manager != nullptr) {
        // 0x20dae..0x20e76: lock manager+0x58, std::map insert keyed by name.data under strcmp, unlock.
        out.path = ConCmdRegisterPath::manager;
        if (registry.manager.insert == nullptr) return out;   // contract missing: ok stays false
        out.ok = registry.manager.insert(registry.manager.manager, cmd);
        return out;
    }

    // 0x20e7f: null manager -> tail-link into the global pending list.
    out.path = ConCmdRegisterPath::pending_list;
    ConCmdPendingList& list = registry.pending;
    if (!list.guard_done) {
        // 0x20e87..0x20ebb: __cxa_guard_acquire(0x2dfce60) then head = {&head, &head}; release.
        list.head.next = &list.head;
        list.head.prev = &list.head;
        list.guard_done = true;
        out.list_initialised = true;
    }
    // 0x20ebe..0x20edf: rcx = head.prev (tail); node = this+8;
    //   node.prev = tail; node.next = &head; head.prev = node; tail->next = node.
    ConCmdListNode* const tail = list.head.prev;
    ConCmdListNode* const node = &cmd.pending;
    node->prev = tail;
    node->next = &list.head;
    list.head.prev = node;
    tail->next = node;

    out.ok = true;
    return out;
}

bool release_inert_view_f590(const SharedStr& view) noexcept {
    SharedHolderF590 holder{};
    holder.object = const_cast<std::uint8_t*>(view.data);
    holder.control = reinterpret_cast<std::atomic<std::uint64_t>*>(view.ctl);
    const SharedReleaseF590Result r = shared_release_f590(holder, SharedReleaseF590Hooks{});
    return r.ok && !r.had_control;
}

}  // namespace sotc
