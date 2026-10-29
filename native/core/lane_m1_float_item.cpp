#include "lane_m1_float_item.hpp"

#include <cstring>

namespace sotc::lane_m1 {

namespace {

SharedStr borrowed(const char* text) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(std::strlen(text)), 0,
                     nullptr};
}

}  // namespace

MenuSiteResult init_item_27050_site(DebugMenuItem& item, const void* target, const char* label, const char* text,
                                    std::uint32_t flags, float range_min, float range_max,
                                    ConCmdRegistry& registry, DebugMenuRegistry& menu) noexcept {
    MenuSiteResult out{};
    const SharedStr l = borrowed(label);
    const SharedStr t = borrowed(text);
    out.ctor_ok = init_debug_menu_item_27050(item, l, t, target, flags, range_min, range_max);
    if (!out.ctor_ok) return out;
    out.registration = register_debug_menu_item_268b0(item, menu, registry.atexit_log);
    out.views_inert = release_inert_view_f590(t) && release_inert_view_f590(l);   // text first, then label
    out.atexit_recorded = registry.atexit_log.record(kDebugMenuItemDtorRva, &item);
    out.ok = out.ctor_ok && out.registration.ok && out.views_inert && out.atexit_recorded;
    return out;
}

}  // namespace sotc::lane_m1
