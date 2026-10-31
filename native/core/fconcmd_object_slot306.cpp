#include "fconcmd_object_slot306.hpp"

#include <cstring>
#include <new>
#include <strings.h>

namespace sotc {

namespace {

// 0xf2f0(out, 0, cstr): {ptr = cstr, size = strlen(cstr), aux = 0, ctl = 0}. Same behaviour as
// owner_row3_string_view_f2f0 (owner_ctor_14db00); kept local to avoid coupling to that module.
SharedStr name_view_f2f0(const char* cstr) noexcept {
    SharedStr v{};
    v.data = reinterpret_cast<const std::uint8_t*>(cstr);
    v.size = static_cast<std::int64_t>(std::strlen(cstr));
    v.aux = 0;
    v.ctl = nullptr;
    return v;
}

// `call 0xf590` on a SharedStr (holder {object = data, control = ctl at +0x18}).
bool release_view(const SharedStr& view, const SharedReleaseF590Hooks& hooks) noexcept {
    SharedHolderF590 holder{};
    holder.object = const_cast<std::uint8_t*>(view.data);
    holder.control = reinterpret_cast<std::atomic<std::uint64_t>*>(view.ctl);
    return shared_release_f590(holder, hooks).ok;
}

}  // namespace

// ---- construction -------------------------------------------------------------------------------------

FConCmdBuildResult init_fconcmd_object(ConCmdCommand& obj, const FConCmdCommandSpec& spec,
                                       ConCmdRegistry& registry, FConCmdTrace* trace) noexcept {
    FConCmdBuildResult out{};
    // Fail closed before touching the object: a null string cannot be measured, and a zero handler would
    // leave the original's stack staging value (an address) in the delegate.
    if (spec.name == nullptr || spec.description == nullptr || spec.handler_tag == 0) return out;
    auto note = [trace](FConCmdEvent e) noexcept {
        if (trace != nullptr) trace->push(e);
    };

    // 0xa64660: 0xf2f0(&name, 0, cstr).
    SharedStr name = name_view_f2f0(spec.name);
    note(FConCmdEvent::name_view_f2f0);

    // 0xa64669..0xa646b7: description view {cstr, imm length, 0, 0} and the delegate staging block
    // {r15 = stack address of the name view, 0x1990, 0, handler}. The first word is overwritten below.
    SharedStr description{};
    description.data = reinterpret_cast<const std::uint8_t*>(spec.description);
    description.size = static_cast<std::int64_t>(spec.description_length);
    description.aux = 0;
    description.ctl = nullptr;
    FConCmdDelegate staging{};
    staging.object = reinterpret_cast<std::uint64_t>(&name);
    staging.fn = kFConCmdInvokeThunkRva;   // vmovq xmm1, [0x15fe5e0] => 0x1990, upper half zero
    staging.adj = 0;
    staging.handler = spec.handler_tag;

    // 0xa646bc: call 0x20af0(obj, &name, &description, 2).
    init_concmd_base_20af0(obj.base, name, description, kFConCmdFlags);
    note(FConCmdEvent::base_ctor_20af0);

    // 0xa646c1..0xa646f9: [obj] = 0x1594dd0; ymm store of the staging block at +0x60; when the handler
    // word is non-zero [obj+0x60] = obj+0x60.
    obj.base.vptr = kFConCmdVptr;
    obj.delegate = staging;
    if (obj.delegate.handler != 0) obj.delegate.object = reinterpret_cast<std::uint64_t>(&obj.delegate);
    note(FConCmdEvent::vptr_and_delegate_store);

    // 0xa6470b..0xa64722: xmm0 = 0 stored at +0x80, qword 0 at +0x90.
    obj.args_begin = nullptr;
    obj.args_end = nullptr;
    obj.args_cap = nullptr;
    note(FConCmdEvent::vector_zero);

    // 0xa64700 / 0xa64725: r14 = [0x1594dc0 + 0x20] (= 0x20d60); call r14 with rdi = obj.
    out.vtable_register_slot_ok =
        kFConCmdVtableSlots[(kFConCmdVtableRegisterOffset - 0x10) / 8] == kConCmdRegisterRva;
    out.registration = register_concmd_20d60(obj.base, registry);
    note(FConCmdEvent::register_via_vtable_slot2);

    // 0xa6472d / 0xa64737: 0xf590(&description); 0xf590(&name). Neither carries a control counter.
    const bool desc_inert = release_inert_view_f590(description);
    note(FConCmdEvent::release_description_f590);
    const bool name_inert = release_inert_view_f590(name);
    note(FConCmdEvent::release_name_f590);
    out.views_inert = desc_inert && name_inert;

    // 0xa6474d: __cxa_atexit(0x4b0, obj, &dso).
    out.atexit_recorded = registry.atexit_log.record(kFConCmdDtorRva, &obj);
    note(FConCmdEvent::atexit_4b0);

    out.handler_invoked = false;
    out.ok = out.vtable_register_slot_ok && out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

FConCmdBuildResult init_slot306_command(std::size_t index, ConCmdCommand& obj, ConCmdRegistry& registry,
                                        FConCmdTrace* trace) noexcept {
    if (index >= kSlot306CommandSites.size()) return FConCmdBuildResult{};
    const Slot306CommandSite& site = kSlot306CommandSites[index];
    FConCmdCommandSpec spec{};
    spec.name = site.name;
    spec.description = site.description;
    spec.description_length = site.description_length;
    spec.handler_tag = site.handler_rva;
    return init_fconcmd_object(obj, spec, registry, trace);
}

// ---- invoker thunk and bound call ---------------------------------------------------------------------

FConCmdInvokeResult invoke_thunk_1990(ConCmdCommand& cmd, const FConCmdDelegate* delegate_object,
                                      void* args, const FConCmdInvokeHooks& hooks) noexcept {
    FConCmdInvokeResult out{};
    // 0x1990 reads [rdi + 0x18]; that is the handler word only when rdi is this command's delegate.
    if (delegate_object != &cmd.delegate) return out;
    const std::uint64_t handler = delegate_object->handler;   // mov rax,[rdi+0x18]
    if (handler == 0 || hooks.handler == nullptr) return out; // `jmp rax` to null / an unprovided body
    out.status = hooks.handler(handler, cmd, args);           // mov rdi,rsi; mov rsi,rdx; jmp rax
    out.ok = true;
    return out;
}

FConCmdInvokeResult invoke_delegate_head_21dc6(ConCmdCommand& cmd, void* args,
                                               const FConCmdInvokeHooks& hooks) noexcept {
    FConCmdInvokeResult out{};
    const std::uint64_t object = cmd.delegate.object;   // rdi = [this+0x60]
    const std::uint64_t fn = cmd.delegate.fn;           // rax = [this+0x68]
    if (object == 0 && fn == 0) {                       // 0x21dd1..0x21ddc: status 1, no call
        out.ok = true;
        out.no_delegate = true;
        out.status = 1;
        return out;
    }
    const std::uint64_t adjusted = object + cmd.delegate.adj;   // add rdi,[this+0x70]
    if ((fn & 1) != 0) return out;                              // virtual member pointer: not modelled
    if (fn != kFConCmdInvokeThunkRva) return out;               // only the 0x1990 identity is known
    if (adjusted != reinterpret_cast<std::uint64_t>(&cmd.delegate)) return out;
    return invoke_thunk_1990(cmd, &cmd.delegate, args, hooks);
}

// ---- destructor chain -------------------------------------------------------------------------------------

ConCmdBaseDtorResult destroy_concmd_base_20b50(ConCmdBase& cmd, ConCmdRegistry& registry,
                                               const FConCmdDtorHooks& hooks) noexcept {
    ConCmdBaseDtorResult out{};

    // 0x20b83: [this] = 0x1594d80 + 0x10.
    cmd.vptr = kConCmdBaseVptr;
    out.final_vptr = cmd.vptr;

    // 0x20b86..0x20bd5: manager = [0x2dfcbc8]; non-null => lock(manager+0x58), 0x213b0(manager, &name),
    // unlock. The manager side is a hook; a missing hook with a non-null manager fails closed.
    if (registry.manager.manager != nullptr) {
        out.manager_path = true;
        if (hooks.manager_erase == nullptr) return out;
        out.manager_erased = hooks.manager_erase(registry.manager.manager, cmd.name);
    }

    // 0x20bda..0x20c14: the same guarded lazy initialisation of the pending list as the register body.
    ConCmdPendingList& list = registry.pending;
    if (!list.guard_done) {
        list.head.next = &list.head;
        list.head.prev = &list.head;
        list.guard_done = true;
        out.list_initialised = true;
    }

    // 0x20c19..0x20c6f: walk the circular list from head.next; if this+8 is a member unlink it
    // (prev->next = next; next->prev = prev). The walk is bounded: a list that never returns to the head
    // is a corrupt contract, reported as ok == false instead of looping forever.
    ConCmdListNode* const node = &cmd.pending;
    ConCmdListNode* cursor = list.head.next;
    constexpr std::size_t kMaxWalk = std::size_t{1} << 24;
    std::size_t steps = 0;
    while (cursor != &list.head) {
        if (cursor == nullptr || ++steps > kMaxWalk) return out;
        if (cursor == node) {
            ConCmdListNode* const next = node->next;   // rax = [this+8]
            ConCmdListNode* const prev = node->prev;   // rcx = [this+0x10]
            if (next == nullptr || prev == nullptr) return out;
            prev->next = next;                         // mov [rcx], rax
            next->prev = prev;                         // mov [rax+8], rcx
            out.unlinked = true;
            break;
        }
        cursor = cursor->next;
    }

    // 0x20c77 / 0x20c83: 0xf590(this+0x40); 0xf590(this+0x20).
    out.description_released = release_view(cmd.description, hooks.shared);
    if (!out.description_released) return out;
    out.name_released = release_view(cmd.name, hooks.shared);
    out.ok = out.name_released;
    return out;
}

FConCmdDtorResult destroy_fconcmd_4b0(ConCmdCommand& cmd, ConCmdRegistry& registry,
                                      const FConCmdDtorHooks& hooks) noexcept {
    FConCmdDtorResult out{};

    // The original loops `rbx += 0x50` until rbx == end; an end before begin or a ragged length would
    // never terminate (or run off the storage), so it is a contract failure here, before any release.
    const auto begin = reinterpret_cast<std::uintptr_t>(cmd.args_begin);
    const auto end = reinterpret_cast<std::uintptr_t>(cmd.args_end);
    if (end < begin || ((end - begin) % kFConCmdArgRecordSize) != 0) return out;

    // 0x4c8: [this] = 0x1594dc0 + 0x10.
    cmd.base.vptr = kFConCmdVptr;

    // 0x4e0..0x4f9: for each record release +0x28 then +0x08.
    for (FConCmdArgRecord* rec = cmd.args_begin; rec != cmd.args_end; ++rec) {
        if (!release_view(rec->second, hooks.shared)) return out;
        if (!release_view(rec->first, hooks.shared)) return out;
        ++out.records_released;
    }

    // 0x4fb..0x50a: rbx = [this+0x80]; if non-null call 0x170c0(rbx). The pointers stay as they are.
    if (cmd.args_begin != nullptr) {
        if (hooks.free_storage == nullptr) return out;
        hooks.free_storage(cmd.args_begin);
        out.storage_freed = true;
    }

    // 0x51c: jmp 0x20b50 (rdi = this).
    out.base = destroy_concmd_base_20b50(cmd.base, registry, hooks);
    out.ok = out.base.ok;
    return out;
}

// ---- manager model ---------------------------------------------------------------------------------------

bool FConCmdManagerModel::CaseInsensitiveLess::operator()(const char* a, const char* b) const noexcept {
    return ::strcasecmp(a, b) < 0;
}

bool FConCmdManagerModel::insert_hook(void* manager, ConCmdBase& cmd) noexcept {
    if (manager == nullptr || cmd.name.data == nullptr) return false;
    auto& self = *static_cast<FConCmdManagerModel*>(manager);
    try {
        std::lock_guard<std::mutex> guard(self.lock_);
        const char* key = reinterpret_cast<const char*>(cmd.name.data);
        if (self.map_.find(key) != self.map_.end()) return true;   // present: nothing overwritten
        Entry entry{cmd.name, &cmd};
        concmd_acquire_view(entry.key);                            // key copy holds a reference
        self.map_.emplace(key, entry);
    } catch (...) {
        self.failed_ = true;
        return false;
    }
    return true;
}

bool FConCmdManagerModel::erase_hook(void* manager, const SharedStr& name) noexcept {
    if (manager == nullptr || name.data == nullptr) return false;
    auto& self = *static_cast<FConCmdManagerModel*>(manager);
    try {
        std::lock_guard<std::mutex> guard(self.lock_);
        const auto it = self.map_.find(reinterpret_cast<const char*>(name.data));
        if (it == self.map_.end()) return false;                   // 0x2142a: eax = 0
        const SharedStr key = it->second.key;
        self.map_.erase(it);                                       // size -= 1, unlink, free node
        if (!release_view(key, self.key_release_hooks_)) {         // 0xf590(node + 0x20)
            self.failed_ = true;
            return false;
        }
    } catch (...) {
        self.failed_ = true;
        return false;
    }
    return true;
}

std::size_t FConCmdManagerModel::size() const noexcept {
    try {
        std::lock_guard<std::mutex> guard(lock_);
        return map_.size();
    } catch (...) {
        return 0;
    }
}

ConCmdBase* FConCmdManagerModel::find(const char* name) const noexcept {
    if (name == nullptr) return nullptr;
    try {
        std::lock_guard<std::mutex> guard(lock_);
        const auto it = map_.find(name);
        return it == map_.end() ? nullptr : it->second.cmd;
    } catch (...) {
        return nullptr;
    }
}

}  // namespace sotc
