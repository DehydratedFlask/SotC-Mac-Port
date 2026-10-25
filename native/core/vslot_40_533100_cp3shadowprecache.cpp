#include "vslot_40_533100_cp3shadowprecache.hpp"

#include "vslot_40_533100_shadowmodels.hpp"

#include <cstring>

namespace sotc {
namespace {

alignas(8) std::uint8_t g_path_buf[0x40]{};
alignas(8) std::uint8_t g_out_slot[0x10]{};

}  // namespace

Vslot40_533100Cp3shadowprecacheResult vslot_40_533100_cp3shadowprecache(
    void* self, const Vslot40_533100Cp3shadowprecacheHooks& hooks) noexcept {
    Vslot40_533100Cp3shadowprecacheResult out{};
    out.self = self;

    // OBS: mov rdi, r15 (= self+0x3cb8); call 0x95580
    out.loader = vslot40_shadow_loader_cell_2ee2cf8();
    if (self != nullptr && hooks.notify_95580 != nullptr) {
        hooks.notify_95580(
            static_cast<std::uint8_t*>(self) + kVslot40Cp3shadowprecacheNotifyOff);
    }
    ++out.notify_95580_count;

    std::memset(g_path_buf, 0, sizeof(g_path_buf));
    std::memset(g_out_slot, 0, sizeof(g_out_slot));

    if (hooks.path_14c2f0 != nullptr) {
        hooks.path_14c2f0(g_path_buf, kVslot40Cp3shadowprecachePath);
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

    // OBS: no 0x14ccb0 in this fragment — attach @ 0x533f97 does refcount+install.
    // Success atexit / attach — NOT modelled
    out.ok = (out.notify_95580_count == 1) && (out.path_14c2f0_count == 1) &&
             (out.load_vslot18_count == 1);
    return out;
}

}  // namespace sotc
