#include "registry_init_531d60.hpp"

#include "registry_sorted_insert.hpp"

#include <cstring>

namespace sotc {
namespace {

void destroy_registry_object(RegistryObject2efd160* obj, AllocatorFront& alloc) noexcept {
    if (obj == nullptr) {
        return;
    }
    // OBS destroy path frees +0x40 begin, then +0x20, then +0x00, then delete_ object.
    // Listener slot vcall-dtors are NOT modelled; fill leaves listeners null.
    if (obj->listeners_begin != nullptr) {
        alloc.delete_array(obj->listeners_begin);
        obj->listeners_begin = obj->listeners_end = obj->listeners_cap = nullptr;
    }
    clear_registry_object_tables_52a220(*obj, alloc);
    alloc.delete_(obj);
}

}  // namespace

std::atomic<RegistryObject2efd160*>& registry_cell_2efd160() noexcept {
    static std::atomic<RegistryObject2efd160*> cell{nullptr};
    return cell;
}

void destroy_registry_at_2efd160(AllocatorFront& alloc) noexcept {
    RegistryObject2efd160* prev = registry_cell_2efd160().exchange(nullptr);
    destroy_registry_object(prev, alloc);
}

RegistryInit531d60Status init_registry_fragment_531d60(InputTable551140& table, AllocatorFront& alloc, const RegistryInit531d60Hooks& hooks) {
    if (!hooks.merge_mov_edi_98_532b0c_u1_ran) { const auto prep = init_531d60_post_base_mov_edi_98_532b0c_u1(); if (!prep.ok) return RegistryInit531d60Status::alloc_failed; }
    void* raw = alloc.new_(kRegistryInitObjectBytes);
    if (raw == nullptr) {
        return RegistryInit531d60Status::alloc_failed;
    }
    // Native zeros before fill so a missing-names early return can delete_ safely without
    // interpreting poison as vector heads. Original relies on 0x52a220 writing the prologue.
    std::memset(raw, 0, kRegistryInitObjectBytes);
    auto* obj = static_cast<RegistryObject2efd160*>(raw);

    const RegistryFill52a220Status fill =
        fill_registry_object_52a220(*obj, table, alloc, input_table_551140_count());
    if (fill != RegistryFill52a220Status::ok) {
        destroy_registry_object(obj, alloc);
        if (fill == RegistryFill52a220Status::missing_names) {
            return RegistryInit531d60Status::missing_names;
        }
        return RegistryInit531d60Status::fill_failed;
    }

    // OBS 0x532b33..0x532bf8: compare with previous; destroy previous if different & non-null;
    // then movq new, (0x2efd160). Same-pointer fast path skips destroy+store.
    RegistryObject2efd160* prev = registry_cell_2efd160().load();
    if (prev == obj) {
        return RegistryInit531d60Status::ok;
    }
    if (prev != nullptr) {
        registry_cell_2efd160().store(nullptr);
        destroy_registry_object(prev, alloc);
    }
    registry_cell_2efd160().store(obj);
    return RegistryInit531d60Status::ok;
}

}  // namespace sotc
