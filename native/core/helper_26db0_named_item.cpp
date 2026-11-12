#include "helper_26db0_named_item.hpp"

namespace sotc {

namespace {

HelperRefusal build(DebugMenuItem& item, const SharedStr& parent, const ConCmdBase& base,
                    const DebugMenuTarget& target, std::uint32_t flags) noexcept {
    if (!target.known) return HelperRefusal::unknown_target_class;   // item untouched
    return init_debug_menu_item_26db0(item, parent, base, target, flags) ? HelperRefusal::none
                                                                         : HelperRefusal::unknown_target_class;
}

}  // namespace

HelperRefusal helper_26db0_named_item_bool(DebugMenuItem& item, const SharedStr& parent, const ConCmdBoolVar& target,
                                           std::uint32_t flags) noexcept {
    return build(item, parent, target.base, debug_menu_target_for(target), flags);
}

HelperRefusal helper_26db0_named_item_int(DebugMenuItem& item, const SharedStr& parent,
                                          const ConCmdRangedVar& target, std::uint32_t flags) noexcept {
    return build(item, parent, target.base, ranged_int_c580_menu_target(target), flags);
}

HelperRefusal helper_26db0_named_item_float(DebugMenuItem& item, const SharedStr& parent,
                                            const ConCmdRangedVar& target, std::uint32_t flags) noexcept {
    return build(item, parent, target.base, lane_m6::ranged_float_menu_target(target), flags);
}

HelperRefusal helper_26db0_named_item_string(DebugMenuItem& item, const SharedStr& parent,
                                             const ConCmdStringVar& target, std::uint32_t flags) noexcept {
    return build(item, parent, target.base, string_var_menu_target(target), flags);
}

}  // namespace sotc
