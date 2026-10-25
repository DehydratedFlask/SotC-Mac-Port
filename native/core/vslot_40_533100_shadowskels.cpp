#include "vslot_40_533100_shadowskels.hpp"

#include "singleton_f08_531d84.hpp"
#include "vslot_40_533100_shadowmodels.hpp"

#include <cstring>

namespace sotc {
namespace {

alignas(8) std::uint8_t g_path_buf[0x40]{};
alignas(8) std::uint8_t g_out_slot[0x10]{};  // [0]=ptr, [8]=flag byte area

}  // namespace

Vslot40_533100ShadowskelsResult vslot_40_533100_shadowskels(
    const Vslot40_533100ShadowskelsHooks& hooks) noexcept {
    Vslot40_533100ShadowskelsResult out{};

    // OBS: lea f08 cell; call 0x95580(*f08)
    out.f08 = singleton_f08_cell_2efc9e0();
    out.loader = vslot40_shadow_loader_cell_2ee2cf8();
    if (out.f08 != nullptr && hooks.notify_95580 != nullptr) {
        hooks.notify_95580(out.f08);
    }
    ++out.notify_95580_count;

    std::memset(g_path_buf, 0, sizeof(g_path_buf));
    std::memset(g_out_slot, 0, sizeof(g_out_slot));

    // OBS: call 0x14c2f0(r12=path_buf, rsi=path)
    if (hooks.path_14c2f0 != nullptr) {
        hooks.path_14c2f0(g_path_buf, kVslot40ShadowskelsPath);
    }
    ++out.path_14c2f0_count;

    // OBS: vcall [loader.vt+0x18](out, loader, path_buf)
    void* loaded = nullptr;
    if (hooks.load_vslot18 != nullptr) {
        loaded = hooks.load_vslot18(g_out_slot, out.loader, g_path_buf);
    }
    ++out.load_vslot18_count;
    if (loaded == nullptr) {
        std::memcpy(&loaded, g_out_slot, sizeof(void*));
    }
    out.loaded = loaded;

    // OBS: call 0x14ccb0(result)
    if (loaded != nullptr) {
        refcount_inc_30_14ccb0(loaded);
        out.refcount_bumped = true;
        std::memcpy(&out.refcount_after,
                    static_cast<std::uint8_t*>(loaded) + kVslot40ShadowRefcountOff,
                    sizeof(out.refcount_after));
    }

    // Success-flag / atexit / skels-attach — NOT modelled
    out.ok = (out.notify_95580_count == 1) && (out.path_14c2f0_count == 1) &&
             (out.load_vslot18_count == 1);
    return out;
}

}  // namespace sotc
