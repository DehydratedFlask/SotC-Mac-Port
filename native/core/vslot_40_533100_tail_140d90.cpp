#include "vslot_40_533100_tail_140d90.hpp"

#include <cstddef>

namespace sotc {
namespace {

// OBS 0x533380..0x533395: 0x38 B stack temporary.
//   +0x00..+0x1f  `{"Position", 8, 0, 0}` (the 0x20 B holder `0xf590` reads)
//   +0x20         1.0f (0x3f800000)
//   +0x28/+0x30   unmodelled; `0xf590` never reads them
struct alignas(8) TailPositionTemp {
    SharedHolderF590 holder;
    float value_20;
    void* field_28;
    void* field_30;
};

static_assert(sizeof(TailPositionTemp) == 0x38);
static_assert(offsetof(TailPositionTemp, holder) == 0x00);
static_assert(offsetof(TailPositionTemp, value_20) == 0x20);

// The six fields the measured `0xf590` no-op must report.
[[nodiscard]] bool is_inert_release(const SharedReleaseF590Result& r) noexcept {
    return r.ok && !r.had_control && !r.decremented && !r.was_last && !r.destroyed && !r.freed &&
           r.previous == 0;
}

}  // namespace

Vslot40_533100Tail140d90Result vslot_40_533100_tail_140d90(
    void* self, const Vslot40_533100Tail140d90Hooks& hooks) noexcept {
    Vslot40_533100Tail140d90Result out{};

    if (self == nullptr) {
        return out;
    }

    // OBS: mov esi, 9; mov rdi, r15 (= self+0x3cc8); call 0x140d90
    out.slot_obj = static_cast<std::uint8_t*>(self) + kVslot40Tail140d90SlotOff;
    out.arg = kVslot40Tail140d90Arg;
    if (hooks.slot != nullptr) {
        // 0x140d90: TLockedResourceRef::Lock(slot, 9); idempotent when the slot is already locked.
        out.lock_ok = lock_resource_ref(hooks.slot->ref, kVslot40Tail140d90Arg,
                                        HashedStr{}, hooks.pool_hooks);
        out.state_after = hooks.slot->ref.ptr != nullptr ? hooks.slot->ref.ptr->state : 0;
        out.deps_after = hooks.slot->ref.ptr != nullptr ? hooks.slot->ref.ptr->deps.load() : 0;
    } else if (hooks.call_140d90 != nullptr) {
        hooks.call_140d90(out.slot_obj, out.arg);
    }
    ++out.call_140d90_count;

    // OBS 0x53404d: 0xf590 on the 0x38 B stack temp built at 0x533380..0x533395.
    TailPositionTemp temp{};
    temp.holder.object =
        const_cast<char*>(static_cast<const char*>(kVslot40Tail140d90NamePosition));
    temp.holder.reserved_08 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(8));
    temp.holder.reserved_10 = nullptr;
    temp.holder.control = nullptr;  // OBS: `+0x18` is zero (vpxor @ 0x533341)
    temp.value_20 = 1.0f;           // OBS 0x53338a: mov dword [rbp-0x50], 0x3f800000
    temp.field_28 = nullptr;        // unmodelled
    temp.field_30 = nullptr;        // unmodelled

    if (hooks.call_f590 != nullptr) {
        out.release_f590 = hooks.call_f590(temp.holder);
    } else {
        out.release_f590 = shared_release_f590(temp.holder, hooks.release_f590);
    }
    ++out.dtor_f590_count;

    // The lifted default path must be the exact inert release proven by the static audit.
    // A host override supplies its own result and is not held to the measured shape.
    const bool release_ok = (hooks.call_f590 != nullptr) || is_inert_release(out.release_f590);

    // stack-check / epilogue @ 0x534052 — see vslot_40_533100_stackchk.*
    const bool lock_ok = hooks.slot == nullptr || out.lock_ok;
    out.ok = (out.call_140d90_count == 1) && (out.dtor_f590_count == 1) &&
             (out.arg == kVslot40Tail140d90Arg) && release_ok && lock_ok;
    return out;
}

}  // namespace sotc
