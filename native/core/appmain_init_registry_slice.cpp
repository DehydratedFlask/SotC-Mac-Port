#include "appmain_init_registry_slice.hpp"

#include "registry_sorted_insert.hpp"

namespace sotc {

AppmainInitRegistrySliceResult run_appmain_init_registry_slice(InputTable551140& table,
                                                               AllocatorFront& alloc) {
    AppmainInitRegistrySliceResult out{};

    // Measured edge starts from a clean registry cell (app-main Init installs once per boot
    // in the original; re-run destroys previous via the fragment).
    destroy_registry_at_2efd160(alloc);

    out.status = init_registry_fragment_531d60(table, alloc);
    out.cell = registry_cell_2efd160().load();
    out.cell_installed = (out.cell != nullptr);

    if (out.status != RegistryInit531d60Status::ok || out.cell == nullptr) {
        return out;
    }

    out.object_bytes = static_cast<std::uint32_t>(kRegistryInitObjectBytes);
    auto* a_begin = static_cast<RegistryEntryA*>(out.cell->table_a_begin);
    auto* a_end = static_cast<RegistryEntryA*>(out.cell->table_a_end);
    if (a_begin != nullptr && a_end != nullptr && a_end >= a_begin) {
        out.entry_count = static_cast<std::uint32_t>(a_end - a_begin);
    }
    return out;
}

}  // namespace sotc
