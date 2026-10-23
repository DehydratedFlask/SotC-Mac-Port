#include "vslot_40_533100_join.hpp"

#include <cstring>

namespace sotc {
namespace {

// The six fields the measured stack-name `0xf590` no-op must report.
[[nodiscard]] bool is_inert_release(const SharedReleaseF590Result& r) noexcept {
    return r.ok && !r.had_control && !r.decremented && !r.was_last && !r.destroyed && !r.freed &&
           r.previous == 0;
}

// OBS 0x533399: 0x20 B stack string holding the literal `0x1014e5c "Position"`, length 8,
// capacity 0, control 0. Only `+0x00`/`+0x18` are read by `0xf590`.
[[nodiscard]] SharedHolderF590 make_position_holder() noexcept {
    SharedHolderF590 h{};
    h.object = const_cast<char*>(static_cast<const char*>(kVslot40JoinNamePosition));
    h.reserved_08 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(8));
    h.reserved_10 = nullptr;
    h.control = nullptr;  // OBS: `+0x18` is zero (vpxor @ 0x533341)
    return h;
}

void destroy_prev_holder(void* prev, AllocatorFront& alloc,
                         const Vslot40_533100JoinHooks& hooks,
                         Vslot40_533100JoinResult& out) noexcept {
    if (prev == nullptr) {
        return;
    }
    // OBS: begin/end at [prev+0]/[prev+8]; stride 0x50; f590 each; delete[] begin; delete prev.
    void* begin = nullptr;
    void* end = nullptr;
    std::memcpy(&begin, prev, sizeof(void*));
    std::memcpy(&end, static_cast<std::uint8_t*>(prev) + 8, sizeof(void*));
    if (begin != nullptr && end != nullptr && begin != end) {
        for (auto* p = static_cast<std::uint8_t*>(begin); p != end; p += 0x50) {
            // 0x5333e3 is the unresolved site: the element's `+0x18` is heap state, so it stays
            // a plain per-element host hook rather than an inert zero-holder release.
            if (hooks.dtor_f590_element != nullptr) {
                hooks.dtor_f590_element(p);
            }
            ++out.dtor_f590_element_count;
            ++out.dtor_f590_count;
        }
    }
    if (begin != nullptr) {
        if (hooks.delete_array_170c0 != nullptr) {
            hooks.delete_array_170c0(begin);
        } else {
            alloc.delete_array(begin);
        }
    }
    if (hooks.delete_170b0 != nullptr) {
        hooks.delete_170b0(prev);
    } else {
        alloc.delete_(prev);
    }
    out.destroyed_prev = true;
}

}  // namespace

Vslot40_533100JoinResult vslot_40_533100_join(AllocatorFront& alloc, void* self,
                                              const Vslot40_533100JoinHooks& hooks) noexcept {
    Vslot40_533100JoinResult out{};
    if (self == nullptr) {
        return out;
    }

    // OBS: stack descriptor → 0x42390; "Position" name pin (layout NOT fully modelled).
    alignas(8) std::uint8_t stack_desc[0x20]{};
    const char* name = kVslot40JoinNamePosition;
    std::memcpy(stack_desc, &name, sizeof(void*));
    if (hooks.call_42390 != nullptr) {
        hooks.call_42390(stack_desc);
    }
    ++out.call_42390_count;

    // OBS 0x533399: one 0xf590 on the temporary hashed-string `{"Position", 8, 0, 0}`.
    SharedHolderF590 position_holder = make_position_holder();
    if (hooks.call_f590 != nullptr) {
        out.release_f590 = hooks.call_f590(position_holder);
    } else {
        out.release_f590 = shared_release_f590(position_holder, hooks.release_f590);
    }
    ++out.release_f590_count;
    ++out.dtor_f590_count;

    // The lifted default path must be the exact inert release proven by the static audit.
    // A host override supplies its own result and is not held to the measured shape.
    const bool release_ok = (hooks.call_f590 != nullptr) || is_inert_release(out.release_f590);

    // OBS: mov edi, 0x20; call 0x17070
    void* neu = alloc.new_(kVslot40JoinPosObjectBytes);
    if (neu == nullptr) {
        out.alloc_failed = true;
        return out;
    }
    out.pos_obj = neu;

    // OBS: call 0x2779a0(neu, &desc, 1)
    if (hooks.ctor_2779a0 != nullptr) {
        hooks.ctor_2779a0(neu, stack_desc, 1);
    } else {
        std::memset(neu, 0, kVslot40JoinPosObjectBytes);
    }
    ++out.ctor_2779a0_count;

    void* slot_addr = static_cast<std::uint8_t*>(self) + kVslot40JoinSlotOff;
    void* prev = nullptr;
    std::memcpy(&prev, slot_addr, sizeof(void*));
    out.prev_slot = prev;

    if (prev == neu) {
        out.replaced = false;
        out.ok = release_ok;
        return out;
    }
    if (prev != nullptr) {
        destroy_prev_holder(prev, alloc, hooks, out);
    }
    std::memcpy(slot_addr, &neu, sizeof(void*));
    out.replaced = true;
    out.ok = (out.call_42390_count == 1) && (out.ctor_2779a0_count == 1) && (neu != nullptr) &&
             release_ok;
    return out;
}

}  // namespace sotc
