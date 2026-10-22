#include "vslot_40_533100_gfx_2ee2d00.hpp"

#include <cstring>

namespace sotc {
namespace {

alignas(8) std::uint8_t g_gfx_obj[kVslot40GfxObjBytes]{};
void* g_cell_2ee2d00 = g_gfx_obj;
bool g_inited = false;

void ensure() noexcept {
    if (g_inited) {
        return;
    }
    std::memset(g_gfx_obj, 0, sizeof(g_gfx_obj));
    g_cell_2ee2d00 = g_gfx_obj;
    g_inited = true;
}

}  // namespace

void*& vslot40_gfx_cell_2ee2d00() noexcept {
    ensure();
    return g_cell_2ee2d00;
}

void store_u32_at_40_147e10(void* obj, std::uint32_t value) noexcept {
    if (obj == nullptr) {
        return;
    }
    std::memcpy(static_cast<std::uint8_t*>(obj) + kVslot40GfxMarkerOff, &value, sizeof(value));
}

Vslot40_533100GfxResult vslot_40_533100_gfx_2ee2d00(
    const Vslot40_533100GfxHooks& hooks) noexcept {
    Vslot40_533100GfxResult out{};
    ensure();

    void* obj = g_cell_2ee2d00;
    out.obj = obj;
    if (obj == nullptr) {
        return out;
    }

    // OBS: call 0x6a5370(*0x2ee2d00)
    if (hooks.call_6a5370 != nullptr) {
        hooks.call_6a5370(obj);
    }
    ++out.call_6a5370_count;

    // OBS: call 0x6abcc0(*0x2ee2d00)
    if (hooks.call_6abcc0 != nullptr) {
        hooks.call_6abcc0(obj);
    }
    ++out.call_6abcc0_count;

    // OBS: mov esi, 0x2a2a2a2a; call 0x147e10
    store_u32_at_40_147e10(obj, kVslot40GfxMarker);
    out.marker_stored = true;
    std::memcpy(&out.marker_value, static_cast<std::uint8_t*>(obj) + kVslot40GfxMarkerOff,
                sizeof(out.marker_value));

    out.ok = (out.call_6a5370_count == 1) && (out.call_6abcc0_count == 1) && out.marker_stored &&
             (out.marker_value == kVslot40GfxMarker);
    return out;
}

}  // namespace sotc
