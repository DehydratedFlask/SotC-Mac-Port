#include "vtable_resolved_entries.hpp"

namespace sotc {

const ResolvedVtableCallSite* find_resolved_vtable_call_site(std::uint64_t site) noexcept {
    for (std::size_t index = 0; index < kResolvedVtableCallSiteCount; ++index) {
        if (kResolvedVtableCallSites[index].site == site) {
            return &kResolvedVtableCallSites[index];
        }
    }
    return nullptr;
}

std::size_t resolved_vtable_target_count() noexcept {
    std::uint64_t sorted[kResolvedVtableCallSiteCount];
    std::size_t count = 0;
    for (std::size_t index = 0; index < kResolvedVtableCallSiteCount; ++index) {
        const std::uint64_t target = kResolvedVtableCallSites[index].target;
        bool seen = false;
        for (std::size_t probe = 0; probe < count; ++probe) {
            if (sorted[probe] == target) {
                seen = true;
                break;
            }
        }
        if (!seen) {
            sorted[count++] = target;
        }
    }
    return count;
}

}  // namespace sotc
