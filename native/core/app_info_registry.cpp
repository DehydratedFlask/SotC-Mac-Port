#include "app_info_registry.hpp"

#include <new>

namespace sotc {
namespace {

// Both views describe the same 0x60 B storage the original keeps as one object. The cast is kept in
// one place and is guarded by the size/offset asserts in the header; only one view is dereferenced
// per operation, and neither type has a non-trivial destructor.
List1bdeb0& as_info_list(Container1beeb0& container) noexcept {
    auto* const list = reinterpret_cast<List1bdeb0*>(&container);
    return *std::launder(list);
}

const List1bdeb0& as_info_list(const Container1beeb0& container) noexcept {
    auto const* const list = reinterpret_cast<const List1bdeb0*>(&container);
    return *std::launder(list);
}

Callable1bdeb0 make_holder(std::string_view view) noexcept {
    Callable1bdeb0 holder{};
    holder.value_00 = reinterpret_cast<std::uintptr_t>(view.data());
    holder.value_08 = static_cast<std::uintptr_t>(view.size());
    holder.value_10 = 0;
    holder.control_18 = nullptr;
    return holder;
}

}  // namespace

AppInfoRegistry::AppInfoRegistry() noexcept {
    init_container_1bdb10(container_);
}

List1bdeb0& AppInfoRegistry::info_list() noexcept {
    return as_info_list(container_);
}

const List1bdeb0& AppInfoRegistry::info_list() const noexcept {
    return as_info_list(container_);
}

std::uint32_t AppInfoRegistry::entry_count() const noexcept {
    return info_list().count;
}

Insert1bdb50Result AppInfoRegistry::add_string_entry(std::string_view name,
                                                     std::string_view value,
                                                     std::uint32_t flag,
                                                     const Insert1bdb50Hooks& hooks) noexcept {
    return add_callable_entry(make_holder(name), make_holder(value), flag, hooks);
}

Insert1bdb50Result AppInfoRegistry::add_callable_entry(const Callable1bdeb0& name,
                                                       const Callable1bdeb0& value,
                                                       std::uint32_t flag,
                                                       const Insert1bdb50Hooks& hooks) noexcept {
    return insert_1bdb50(info_list(), name, value, flag, hooks);
}

ListDtor1bdeb0Result AppInfoRegistry::clear(const ListDtor1bdeb0Hooks& hooks) noexcept {
    return list_dtor_1bdeb0(info_list(), hooks);
}

Callable1bdeb0 make_string_view_holder(std::string_view view) noexcept {
    return make_holder(view);
}

}  // namespace sotc
