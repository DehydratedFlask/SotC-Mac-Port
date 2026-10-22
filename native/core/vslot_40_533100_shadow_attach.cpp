#include "vslot_40_533100_shadow_attach.hpp"

#include "singleton_f08_531d84.hpp"
#include "vslot_40_533100_shadowmodels.hpp"

#include <cstring>

namespace sotc {
namespace {

alignas(8) std::uint8_t g_path_dtor_standin[8]{};

}  // namespace

Vslot40_533100ShadowAttachResult vslot_40_533100_shadow_attach(
    void* loaded, bool success_flag,
    const Vslot40_533100ShadowAttachHooks& hooks) noexcept {
    Vslot40_533100ShadowAttachResult out{};

    void* f08 = singleton_f08_cell_2efc9e0();
    if (f08 == nullptr) {
        return out;
    }

    // OBS: cmp [rbp-0x78], 0; je fail
    bool success = success_flag;
    if (!success) {
        out.took_fail_path = true;
        // success atexit/tag cluster NOT modelled
    }

    // OBS join @ 0x5334f9: rbx = *[f08]; sentinel = 0x14d880(); …
    void* prev = nullptr;
    std::memcpy(&prev, static_cast<std::uint8_t*>(f08) + kVslot40ShadowAttachSlotOff,
                sizeof(void*));
    out.prev_slot = prev;

    void* sentinel = nullptr;
    if (hooks.sentinel_14d880 != nullptr) {
        sentinel = hooks.sentinel_14d880();
    }
    (void)sentinel;  // compared in OBS; equal still falls into install path

    // OBS: if prev == loaded → skip install; else flag-cleanup + store
    if (prev != loaded) {
        std::uint8_t flag = 0;
        std::memcpy(&flag, static_cast<std::uint8_t*>(f08) + kVslot40ShadowAttachFlagOff, 1);
        if (flag != 0 && prev != nullptr) {
            if (hooks.release_14d2d0 != nullptr) {
                hooks.release_14d2d0(prev);
            }
            flag = 0;
            std::memcpy(static_cast<std::uint8_t*>(f08) + kVslot40ShadowAttachFlagOff, &flag, 1);
        }
        // OBS: store loaded; refcount_inc new; release_14ccc0(old) when old non-null
        std::memcpy(static_cast<std::uint8_t*>(f08) + kVslot40ShadowAttachSlotOff, &loaded,
                    sizeof(void*));
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

    std::memcpy(&out.slot_after, static_cast<std::uint8_t*>(f08) + kVslot40ShadowAttachSlotOff,
                sizeof(void*));

    // OBS: if success → notify_95580(f08); release_14d2d0(loaded)
    if (success) {
        if (hooks.notify_95580 != nullptr) {
            hooks.notify_95580(f08);
        }
        if (loaded != nullptr && hooks.release_14d2d0 != nullptr) {
            hooks.release_14d2d0(loaded);
        }
    }

    // OBS: always release_14ccc0(loaded) after notify branch
    if (loaded != nullptr) {
        if (hooks.release_14ccc0 != nullptr) {
            hooks.release_14ccc0(loaded);
        }
        ++out.release_14ccc0_count;
    }

    // OBS: cleanup out-slot releases (measured: out empty after our E426 model)
    // then 0xf590 on path stand-in
    if (hooks.dtor_f590 != nullptr) {
        hooks.dtor_f590(g_path_dtor_standin);
    }
    ++out.dtor_f590_count;

    // shadowskels @ 0x533582 — see vslot_40_533100_shadowskels.*
    out.ok = (out.dtor_f590_count == 1) && (out.installed || prev == loaded);
    return out;
}

}  // namespace sotc
