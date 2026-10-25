#include "init_1290.hpp"

namespace sotc {

Init1290Result init_1290(void* self, const Init1290Hooks& hooks) noexcept {
    Init1290Result out{};
    const std::uintptr_t ctx = reinterpret_cast<std::uintptr_t>(self);

    // A = {ctx=&self, invoke=0x1a00, 0, fn=0xb10}; A+0x40 = table, A+0x48 = 0.
    out.reg_a.entry.ctx = ctx;
    out.reg_a.entry.invoke = kFn1290RegAInvoke;
    out.reg_a.entry.reserved = 0;
    out.reg_a.entry.fn = kFn1290RegAFn;
    for (void*& slot : out.reg_a.self_arg) slot = nullptr;
    out.reg_a.table = kFn1290Table;
    out.reg_a.zero_48 = 0;

    // call 0x1bde30 (owner = *(0x2ee68c8) — owner identity not modelled). The remarshal
    // tail-calls 0x1beeb0, so when a real container and allocator are supplied the lifted
    // insert body executes the 0x20 B copy and the list link.
    if (hooks.container_a != nullptr && hooks.alloc_a.alloc != nullptr) {
        Descriptor1beeb0 d{};
        d.ctx = out.reg_a.entry.ctx;
        d.invoke = out.reg_a.entry.invoke;
        d.offset = out.reg_a.entry.reserved;
        d.fn = out.reg_a.entry.fn;
        for (std::size_t i = 0; i < 4; ++i) {
            d.context[i] = reinterpret_cast<std::uintptr_t>(out.reg_a.self_arg[i]);
        }
        d.vtable = out.reg_a.table;
        d.slot_48 = out.reg_a.zero_48;
        out.inserted_a = insert_1bde30(*hooks.container_a, d, hooks.alloc_a);
        if (!out.inserted_a.ok) return out;  // allocation failure is visible
        out.insert_a_executed = true;
    } else {
        if (hooks.insert_1bde30 == nullptr) return out;
        hooks.insert_1bde30(nullptr, out.reg_a);
    }
    ++out.insert_1bde30_count;

    // call *[A+0x20][0x18] — rdi = &A + 0x20. The slot-3 target (0x19f0) is a bare ret.
    if (hooks.vcall_1594060_slot3 == nullptr) return out;
    out.vcall_self_arg = static_cast<void*>(out.reg_a.self_arg);
    hooks.vcall_1594060_slot3(out.vcall_self_arg);
    ++out.vcall_count;

    // B = {ctx=&self, invoke=0x1a10, 0, fn=0xbb0}.
    out.reg_b.entry.ctx = ctx;
    out.reg_b.entry.invoke = kFn1290RegBInvoke;
    out.reg_b.entry.reserved = 0;
    out.reg_b.entry.fn = kFn1290RegBFn;

    // call 0x21cd0 (owner = *(0x2dfcbc8) — owner identity not modelled). When a real
    // container is supplied, the lifted bump-allocator insert body executes the 0x20 B
    // copy and self-link instead of the hook.
    if (hooks.container_b != nullptr) {
        Descriptor21cd0 db{};
        db.ctx = out.reg_b.entry.ctx;
        db.invoke = out.reg_b.entry.invoke;
        db.reserved = out.reg_b.entry.reserved;
        db.fn = out.reg_b.entry.fn;
        const Insert21cd0Result ir = insert_21cd0(*hooks.container_b, db, hooks.insert_b_hooks);
        if (!ir.ok) return out;  // allocation/grow failure is visible
        out.inserted_b = ir;
        out.insert_b_executed = true;
    } else {
        if (hooks.insert_21cd0 == nullptr) return out;
        hooks.insert_21cd0(nullptr, out.reg_b);
    }
    ++out.insert_21cd0_count;

    // Stack-check fail branch (0x134e -> 0xf8f3a0) is not modelled: read-only guard.
    out.ok = (out.insert_1bde30_count == 1 && out.vcall_count == 1 &&
              (out.insert_21cd0_count == 1 || out.insert_b_executed));
    return out;
}

}  // namespace sotc
