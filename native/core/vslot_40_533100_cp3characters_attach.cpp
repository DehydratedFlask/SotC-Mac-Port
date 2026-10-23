#include "vslot_40_533100_cp3characters_attach.hpp"

#include "resource_id_14c2f0.hpp"
#include "vslot_40_533100_shadowmodels.hpp"

#include <atomic>
#include <cstring>

namespace sotc {

Vslot40_533100Cp3charactersAttachResult vslot_40_533100_cp3characters_attach(
    void* self, void* loaded, bool success_flag,
    const Vslot40_533100Cp3charactersAttachHooks& hooks) noexcept {
    Vslot40_533100Cp3charactersAttachResult out{};

    if (self == nullptr) {
        return out;
    }

    // OBS: success-flag @ [rbp-0xe8]; measured default false → skip notify
    bool success = success_flag;
    if (!success) {
        out.took_fail_path = true;
    }

    // OBS: rbx = *[self+0x3cb8]; sentinel = 0x14d880(); …
    void* prev = nullptr;
    std::memcpy(&prev, static_cast<std::uint8_t*>(self) + kVslot40Cp3charactersAttachSlotOff,
                sizeof(void*));
    out.prev_slot = prev;

    void* sentinel = nullptr;
    if (hooks.sentinel_14d880 != nullptr) {
        sentinel = hooks.sentinel_14d880();
    }
    (void)sentinel;

    if (prev != loaded) {
        std::uint8_t flag = 0;
        std::memcpy(&flag,
                    static_cast<std::uint8_t*>(self) + kVslot40Cp3charactersAttachFlagOff, 1);
        if (flag != 0 && prev != nullptr) {
            if (hooks.release_14d2d0 != nullptr) {
                hooks.release_14d2d0(prev);
            }
            flag = 0;
            std::memcpy(static_cast<std::uint8_t*>(self) + kVslot40Cp3charactersAttachFlagOff,
                        &flag, 1);
        }
        std::memcpy(static_cast<std::uint8_t*>(self) + kVslot40Cp3charactersAttachSlotOff,
                    &loaded, sizeof(void*));
        if (loaded != nullptr) {
            refcount_inc_30_14ccb0(loaded);
        }
        if (prev != nullptr) {
            if (hooks.release_14ccc0 != nullptr) {
                hooks.release_14ccc0(prev);
            }
            ++out.release_14ccc0_count;
        }
        out.installed = true;
    }

    std::memcpy(&out.slot_after,
                static_cast<std::uint8_t*>(self) + kVslot40Cp3charactersAttachSlotOff,
                sizeof(void*));

    if (success) {
        void* slot_obj =
            static_cast<std::uint8_t*>(self) + kVslot40Cp3charactersAttachSlotOff;
        if (hooks.notify_95580 != nullptr) {
            hooks.notify_95580(slot_obj);
        }
        if (loaded != nullptr && hooks.release_14d2d0 != nullptr) {
            hooks.release_14d2d0(loaded);
        }
    }

    if (loaded != nullptr) {
        if (hooks.release_14ccc0 != nullptr) {
            hooks.release_14ccc0(loaded);
        }
        ++out.release_14ccc0_count;
    }

    // 0xf1a0 + 0x14c2f0: build the ResourceId key, then release it with the shared 0xf590.
    HashedStr key{};
    ResourceIdCtorHooks kh{};
    kh.str = hooks.key_alloc;
    kh.platform_tag = hooks.platform_tag;
    if (!resource_id_ctor_14c2f0(key, kCp3charactersKeyLiteral, kh)) {
        out.ok = false;
        return out;
    }
    // ResourcePool::GetResource ([[0x2ee2cf8]]+0x18): a map miss stores its own key copy (+1).
    if (hooks.resource_pool_miss && key.str.ctl != nullptr) {
        std::atomic_ref<std::int64_t>(*key.str.ctl).fetch_add(1, std::memory_order_relaxed);
    }
    out.key_size = key.str.size;
    out.key_capacity = key.str.aux;
    out.key_hash = key.hash;
    const SharedHolderF590 holder{const_cast<std::uint8_t*>(key.str.data), nullptr, nullptr,
                                  reinterpret_cast<std::atomic<std::uint64_t>*>(key.str.ctl)};
    out.key_release = shared_release_f590(holder, hooks.release);
    ++out.dtor_f590_count;
    const bool release_ok =
        out.key_release.ok && out.key_release.had_control &&
        (out.key_release.previous == 1 || out.key_release.previous == 2) &&
        out.key_release.was_last == (out.key_release.previous == 1);

    // next path (cp3shadowprecache) @ 0x533f51 — NOT modelled here
    out.ok = release_ok && (out.dtor_f590_count == 1) && (out.installed || prev == loaded);
    return out;
}

}  // namespace sotc
