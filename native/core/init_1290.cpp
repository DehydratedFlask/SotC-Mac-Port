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

    // call 0x1bde30 (owner = *(0x2ee68c8) — owner identity not modelled).
    if (hooks.insert_1bde30 == nullptr) return out;
    hooks.insert_1bde30(nullptr, out.reg_a);
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

    // call 0x21cd0 (owner = *(0x2dfcbc8) — owner identity not modelled).
    if (hooks.insert_21cd0 == nullptr) return out;
    hooks.insert_21cd0(nullptr, out.reg_b);
    ++out.insert_21cd0_count;

    // Stack-check fail branch (0x134e -> 0xf8f3a0) is not modelled: read-only guard.
    out.ok = (out.insert_1bde30_count == 1 && out.vcall_count == 1 &&
              out.insert_21cd0_count == 1);
    return out;
}

}  // namespace sotc
