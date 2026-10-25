#include "vslot_40_533100_shadowmodels.hpp"

#include "singleton_f08_531d84.hpp"

#include <cstring>

namespace sotc {
namespace {

void* g_loader_cell = nullptr;
alignas(8) std::uint8_t g_path_buf[0x40]{};
alignas(8) std::uint8_t g_out_slot[0x10]{};  // [0]=ptr, [8]=flag byte area

}  // namespace

void*& vslot40_shadow_loader_cell_2ee2cf8() noexcept { return g_loader_cell; }

void refcount_inc_30_14ccb0(void* obj) noexcept {
    if (obj == nullptr) {
        return;
    }
    std::uint32_t v = 0;
    std::memcpy(&v, static_cast<std::uint8_t*>(obj) + kVslot40ShadowRefcountOff, sizeof(v));
    ++v;
    std::memcpy(static_cast<std::uint8_t*>(obj) + kVslot40ShadowRefcountOff, &v, sizeof(v));
}

Vslot40_533100ShadowResult vslot_40_533100_shadowmodels(
    const Vslot40_533100ShadowHooks& hooks) noexcept {
    Vslot40_533100ShadowResult out{};

    // OBS: rbx = *0x2ee2cf8; r15 = *0x2efc9e0
    out.loader = g_loader_cell;
    out.f08 = singleton_f08_cell_2efc9e0();

    std::memset(g_path_buf, 0, sizeof(g_path_buf));
    std::memset(g_out_slot, 0, sizeof(g_out_slot));

    // OBS: call 0x14c2f0(r12=path_buf, rsi=path)
    if (hooks.path_14c2f0 != nullptr) {
        hooks.path_14c2f0(g_path_buf, kVslot40ShadowPath);
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

    // OBS: call 0x14ccb0(result) — always issued even when result may be null in
    // some failure modes; measured hooks supply a stand-in so refcount is defined.
    if (loaded != nullptr) {
        refcount_inc_30_14ccb0(loaded);
        out.refcount_bumped = true;
        std::memcpy(&out.refcount_after,
                    static_cast<std::uint8_t*>(loaded) + kVslot40ShadowRefcountOff,
                    sizeof(out.refcount_after));
    }

    // Success-flag branch / attach / shadowskels — NOT modelled
    out.ok = (out.path_14c2f0_count == 1) && (out.load_vslot18_count == 1);
    return out;
}

}  // namespace sotc
