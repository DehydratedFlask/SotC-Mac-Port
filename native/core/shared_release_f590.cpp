#include "shared_release_f590.hpp"

namespace sotc {

SharedReleaseF590Result shared_release_f590(const SharedHolderF590& holder,
                                            const SharedReleaseF590Hooks& hooks) noexcept {
    SharedReleaseF590Result out{};

    // rax = [rbx+0x18]; test rax,rax; je ret
    std::atomic<std::uint64_t>* control = holder.control;
    if (control == nullptr) {
        out.ok = true;  // the original's `je` path returns without doing anything
        return out;
    }
    out.had_control = true;

    // rcx = -1; lock xadd rcx,[rax]; cmp rcx,1; jne ret
    const std::uint64_t previous = control->fetch_add(static_cast<std::uint64_t>(-1));
    out.decremented = true;
    out.previous = previous;
    if (previous != 1) {
        out.ok = true;  // still referenced
        return out;
    }
    out.was_last = true;

    // rdi = [rbx]; esi = 0; call 0x12af0
    if (hooks.destroy == nullptr) {
        return out;  // visible failure: last reference but the pointee was not destroyed
    }
    hooks.destroy(holder.object, 0);
    out.destroyed = true;

    // rdi = [rbx+0x18]; test; je ret; jmp 0x170b0
    if (hooks.free_control == nullptr) {
        return out;
    }
    hooks.free_control(static_cast<void*>(control));
    out.freed = true;

    out.ok = true;
    return out;
}

}  // namespace sotc
