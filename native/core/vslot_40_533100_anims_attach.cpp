#include "vslot_40_533100_anims_attach.hpp"

#include "singleton_f08_531d84.hpp"
#include "vslot_40_533100_shadowmodels.hpp"

#include <cstring>

namespace sotc {
namespace {

alignas(8) std::uint8_t g_path_dtor_standin[8]{};

}  // namespace

Vslot40_533100AnimsAttachResult vslot_40_533100_anims_attach(
    void* loaded, bool success_flag,
    const Vslot40_533100AnimsAttachHooks& hooks) noexcept {
    Vslot40_533100AnimsAttachResult out{};

    void* f08 = singleton_f08_cell_2efc9e0();
    if (f08 == nullptr) {
        return out;
    }

    // OBS: cmp [rbp-0x98], 0; je fail @ 0x5337d4
    bool success = success_flag;
    if (!success) {
        out.took_fail_path = true;
    }

    // OBS join @ 0x5337d7: rbx = *[f08+0x20]; sentinel = 0x14d880(); …
    void* prev = nullptr;
    std::memcpy(&prev, static_cast<std::uint8_t*>(f08) + kVslot40AnimsAttachSlotOff,
                sizeof(void*));
    out.prev_slot = prev;

    void* sentinel = nullptr;
    if (hooks.sentinel_14d880 != nullptr) {
        sentinel = hooks.sentinel_14d880();
    }
    (void)sentinel;

    if (prev != loaded) {
        std::uint8_t flag = 0;
        std::memcpy(&flag, static_cast<std::uint8_t*>(f08) + kVslot40AnimsAttachFlagOff, 1);
        if (flag != 0 && prev != nullptr) {
            if (hooks.release_14d2d0 != nullptr) {
                hooks.release_14d2d0(prev);
            }
            flag = 0;
            std::memcpy(static_cast<std::uint8_t*>(f08) + kVslot40AnimsAttachFlagOff, &flag, 1);
        }
        std::memcpy(static_cast<std::uint8_t*>(f08) + kVslot40AnimsAttachSlotOff, &loaded,
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

    std::memcpy(&out.slot_after, static_cast<std::uint8_t*>(f08) + kVslot40AnimsAttachSlotOff,
                sizeof(void*));

    if (success) {
        void* slot_obj = static_cast<std::uint8_t*>(f08) + kVslot40AnimsAttachSlotOff;
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

    if (hooks.dtor_f590 != nullptr) {
        hooks.dtor_f590(g_path_dtor_standin);
    }
    ++out.dtor_f590_count;

    // next path @ 0x533870 — NOT modelled
    out.ok = (out.dtor_f590_count == 1) && (out.installed || prev == loaded);
    return out;
}

}  // namespace sotc
