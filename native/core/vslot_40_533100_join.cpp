#include "vslot_40_533100_join.hpp"

#include <cstring>

namespace sotc {
namespace {

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
            if (hooks.dtor_f590 != nullptr) {
                hooks.dtor_f590(p);
            }
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

    // OBS: one 0xf590 on the temporary hashed-string after Position setup.
    if (hooks.dtor_f590 != nullptr) {
        hooks.dtor_f590(stack_desc);
    }
    ++out.dtor_f590_count;

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
        out.ok = true;
        return out;
    }
    if (prev != nullptr) {
        destroy_prev_holder(prev, alloc, hooks, out);
    }
    std::memcpy(slot_addr, &neu, sizeof(void*));
    out.replaced = true;
    out.ok = (out.call_42390_count == 1) && (out.ctor_2779a0_count == 1) && (neu != nullptr);
    return out;
}

}  // namespace sotc
