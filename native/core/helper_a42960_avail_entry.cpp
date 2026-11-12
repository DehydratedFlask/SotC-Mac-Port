#include "helper_a42960_avail_entry.hpp"

#include <cstdio>
#include <cstring>
#include <new>

#include "init_sub_3be930.hpp"

namespace sotc {

bool format_entitlement_name(char* out, std::size_t capacity, const char* text) noexcept {
    if (out == nullptr || capacity == 0) return false;
    const int written = std::snprintf(out, capacity, "entitlement_available_%s", text != nullptr ? text : "(null)");
    return written >= 0 && static_cast<std::size_t>(written) < capacity;
}

AvailEntryResult register_avail_entry_a42960(PtrVec24& vec, AvailEntry& entry, const SharedStr& a, const SharedStr& b,
                                             ConCmdRegistry& registry, const AvailEntryHooks& hooks) noexcept {
    AvailEntryResult out{};
    if (hooks.alloc.allocate == nullptr || hooks.name_dealloc == nullptr) {
        out.stage = AvailEntryStage::hooks_missing;
        return out;
    }
    init_slot_f180(&entry.a);                  // call 0xf180 (rdi = entry)
    init_slot_f180(&entry.b);                  // call 0xf180 (rdi = entry + 0x20)
    entry.var = nullptr;                       // mov qword [entry+0x40], 0
    if (&entry.a == &a) {                      // cmp r14, rbx ; je
        out.a_aliased = true;
    } else {
        if (!release_inert_view_f590(entry.a)) return out;   // 0xf590 on the freshly emptied string (cannot fail)
        copy_shared_str_inc(entry.a, a);
    }
    if (&entry.b == &b) {                      // cmp r12, r15 ; je
        out.b_aliased = true;
    } else {
        if (!release_inert_view_f590(entry.b)) return out;
        copy_shared_str_inc(entry.b, b);
    }
    const AllocWrapperResult block = alloc_17070(kAvailVarAllocSize, hooks.alloc);   // mov edi,0xc8 ; call 0x17070
    if (!block.ok || block.pointer == nullptr) {
        out.stage = AvailEntryStage::var_alloc_failed;
        return out;
    }
    auto* const var = new (block.pointer) ConCmdBoolVar{};
    char text[256];
    if (!format_entitlement_name(text, sizeof text, reinterpret_cast<const char*>(entry.a.data))) {   // call 0x10a90 (stand-in)
        out.stage = AvailEntryStage::name_failed;
        return out;
    }
    SharedStr name{};
    if (!init_shared_str_f1a0(name, text, hooks.name_alloc)) {
        out.stage = AvailEntryStage::name_failed;
        return out;
    }
    const SharedStr description{reinterpret_cast<const std::uint8_t*>(""), 0, 0, nullptr};   // lea 0x12c2e23 ; the rest zeroed
    out.var_result = init_bool_var_vector(*var, name, description, kAvailVarFnVtbl, registry, hooks.name_dealloc, 0, 0);
    // The original releases the name inside that call's tail (the real 0xf590); the previous entry->var is 0, so no virtual delete.
    entry.var = var;                           // mov [r14+0x40], rbx
    if (!out.var_result.ok || !out.var_result.registration.ok) {
        out.stage = AvailEntryStage::construct_failed;
        return out;
    }
    const PtrVecPush pushed = push_ptr_doubling(vec, &entry, hooks.alloc);
    out.push_refusal = pushed.refusal;
    out.grew = pushed.grew;
    out.new_capacity = pushed.new_capacity;
    out.ok = pushed.ok;
    out.stage = pushed.ok ? AvailEntryStage::ok : AvailEntryStage::push_failed;
    return out;
}

}  // namespace sotc
