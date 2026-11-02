#include "vtable_2dcb170_slot08_6e92c0.hpp"

#include <cstring>

namespace sotc {
namespace {

// `0x1276ef0`, staged on the stack by `vmovups` before the format call.
constexpr float kVector[4] = {0.01F, 0.0F, -0.05F, 1.0F};
// `0x1606140`: the block the first destination receives.
constexpr float kCopyBlockA[4] = {0.0F, 0.0F, 0.0F, 1.0F};
// `0x2f4cfd0` is zero-initialised mutable storage in the pinned image.
constexpr float kCopyBlockB[4] = {0.0F, 0.0F, 0.0F, 0.0F};

void copy_block(const std::uint8_t* source, const float (&fallback)[4], void* destination,
                float (&observed)[4]) {
    const void* from = source;
    if (from == nullptr) {
        from = fallback;
    }
    std::memcpy(observed, from, sizeof(observed));
    std::memcpy(destination, observed, sizeof(observed));
}

}  // namespace

Vtable2dcb170Slot08_6e92c0Result vtable_2dcb170_slot08_6e92c0(
    const Vtable2dcb170Slot08_6e92c0Cells& cells,
    const Vtable2dcb170Slot08_6e92c0Hooks& hooks) noexcept {
    Vtable2dcb170Slot08_6e92c0Result out{};

    // `leaq -0x58(%rbp),%rdi` and the format call are unconditional.
    if (cells.format_out == nullptr || hooks.format_call == nullptr) {
        return out;
    }
    out.id = kVtable2dcb170Slot08_6e92c0IdValue;
    std::memcpy(out.vector, kVector, sizeof(out.vector));
    hooks.format_call(cells.format_out, cells.argument, out.id,
                      reinterpret_cast<const void*>(kVtable2dcb170Slot08_6e92c0CopySourceAAddress),
                      out.vector);
    out.formatted = true;  // the body ignores the return value

    // `movq 0x18(%rax),%rbx; testq %rbx,%rbx; je` — a null owner skips the whole tail.
    if (cells.owner == nullptr) {
        out.ok = true;
        return out;
    }
    out.tail_ran = true;

    if (hooks.access_targets == nullptr) {
        return out;
    }
    const Vtable2dcb170Slot08_6e92c0Targets* targets = hooks.access_targets(cells.owner);
    if (targets == nullptr || targets->block_a == nullptr || targets->block_b == nullptr) {
        return out;  // `movq 0x160(%rax)` / `movq 0x150(%rax)` then a store through each
    }

    copy_block(cells.copy_source_a, kCopyBlockA, targets->block_a, out.copied_a_values);
    out.copied_a = true;
    copy_block(cells.copy_source_b, kCopyBlockB, targets->block_b, out.copied_b_values);
    out.copied_b = true;

    if (hooks.commit == nullptr) {
        return out;  // `callq 0x6610d0` is unconditional on this path
    }
    hooks.commit(cells.owner);
    out.committed = true;
    out.ok = true;
    return out;
}

}  // namespace sotc
