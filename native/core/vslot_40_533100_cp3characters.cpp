#include "vslot_40_533100_cp3characters.hpp"

#include "singleton_f08_531d84.hpp"
#include "vslot_40_533100_shadowmodels.hpp"

#include <cstring>

namespace sotc {
namespace {

alignas(8) std::uint8_t g_path_buf[0x40]{};
alignas(8) std::uint8_t g_out_slot[0x10]{};

}  // namespace

Vslot40_533100Cp3charactersResult vslot_40_533100_cp3characters(
    const Vslot40_533100Cp3charactersHooks& hooks) noexcept {
    Vslot40_533100Cp3charactersResult out{};

    // OBS: lea f08; call 0x95580(*f08 + 0x70)
    out.f08 = singleton_f08_cell_2efc9e0();
    out.loader = vslot40_shadow_loader_cell_2ee2cf8();
    if (out.f08 != nullptr && hooks.notify_95580 != nullptr) {
        hooks.notify_95580(
            static_cast<std::uint8_t*>(out.f08) + kVslot40Cp3charactersNotifyOff);
    }
    ++out.notify_95580_count;

    std::memset(g_path_buf, 0, sizeof(g_path_buf));
    std::memset(g_out_slot, 0, sizeof(g_out_slot));

    if (hooks.path_14c2f0 != nullptr) {
        hooks.path_14c2f0(g_path_buf, kVslot40Cp3charactersPath);
    }
    ++out.path_14c2f0_count;

    void* loaded = nullptr;
    if (hooks.load_vslot18 != nullptr) {
        loaded = hooks.load_vslot18(g_out_slot, out.loader, g_path_buf);
    }
    ++out.load_vslot18_count;
    if (loaded == nullptr) {
        std::memcpy(&loaded, g_out_slot, sizeof(void*));
    }
    out.loaded = loaded;

    // OBS: no 0x14ccb0 in this fragment — attach @ 0x533eac does refcount+install.
    // Success atexit / attach — NOT modelled
    out.ok = (out.notify_95580_count == 1) && (out.path_14c2f0_count == 1) &&
             (out.load_vslot18_count == 1);
    return out;
}

}  // namespace sotc
