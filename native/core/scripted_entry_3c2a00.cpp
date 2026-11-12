// CPU-01-STARTUP-SLOT-468: see scripted_entry_3c2a00.hpp.
#include "scripted_entry_3c2a00.hpp"

namespace sotc {

ScriptedEntryResult init_scripted_entry_3c2a00(
    ScriptedEntry& entry, const std::array<ScriptedEntryDescriptor, 3>& descriptors,
    const SharedStr& name, ScriptedEntry*& list_head, RbMap* registry,
    const RbHooks& hooks) noexcept {
    ScriptedEntryResult out{};

    for (std::size_t i = 0; i < 3; ++i) {
        entry.groups[i] = descriptors[i];
        if (descriptors[i].flag18 != 0) {
            entry.groups[i].self = reinterpret_cast<std::uint64_t>(&entry.groups[i]);
        }
    }

    entry.name = name;
    concmd_acquire_view(entry.name);   // lock inc [ctl], only when ctl != 0

    entry.next = list_head;
    list_head = &entry;
    out.linked = true;

    if (registry != nullptr) {
        out.registry_present = true;
        const RbIndexResult index = rb_index_3c0840(*registry, entry.name, hooks);
        if (!index.ok || index.slot == nullptr) return out;
        if (index.created || *index.slot == nullptr) {
            *index.slot = &entry;
            out.inserted = true;
        } else {
            out.already_present = true;
        }
    }

    out.ok = true;
    return out;
}

}  // namespace sotc
