#include "vtable_15e2028_slot2a0_e21420.hpp"

#include <cstring>

namespace sotc {

const char* vtable_15e2028_slot2a0_e21420_path_name(
    Vtable15e2028Slot2A0_e21420Path path) noexcept {
    switch (path) {
        case Vtable15e2028Slot2A0_e21420Path::none:
            return "none";
        case Vtable15e2028Slot2A0_e21420Path::state_mismatch:
            return "state_mismatch";
        case Vtable15e2028Slot2A0_e21420Path::scan_exhausted:
            return "scan_exhausted";
        case Vtable15e2028Slot2A0_e21420Path::missing:
            return "missing";
        case Vtable15e2028Slot2A0_e21420Path::found:
            return "found";
    }
    return "unknown";
}

Vtable15e2028Slot2A0_e21420Result vtable_15e2028_slot2a0_e21420(
    const Vtable15e2028Slot2A0_e21420Cells& cells,
    const Vtable15e2028Slot2A0_e21420Hooks& hooks) noexcept {
    Vtable15e2028Slot2A0_e21420Result out{};
    if (cells.provider == nullptr || hooks.provider_lookup == nullptr ||
        hooks.object_state == nullptr || hooks.object_kind == nullptr ||
        hooks.provider_count == nullptr || hooks.provider_name == nullptr) {
        return out;  // every guard below needs its hook before it can be modelled
    }

    void* object = hooks.provider_lookup(cells.provider, cells.name);
    if (object == nullptr) {
        out.path = Vtable15e2028Slot2A0_e21420Path::missing;
        out.status = kVtable15e2028Slot2A0_e21420StatusMissing;
        if (hooks.on_missing != nullptr) {
            hooks.on_missing(cells.provider);
            out.missing_hook_called = true;
        }
        out.ok = true;
        return out;
    }

    if (hooks.object_state(object) !=
        static_cast<std::int32_t>(kVtable15e2028Slot2A0_e21420ExpectedState)) {
        out.path = Vtable15e2028Slot2A0_e21420Path::state_mismatch;
        out.status = kVtable15e2028Slot2A0_e21420StatusState;
        out.ok = true;
        return out;
    }

    if (hooks.object_kind(object) != kVtable15e2028Slot2A0_e21420ExpectedKind) {
        out.path = Vtable15e2028Slot2A0_e21420Path::missing;
        out.status = kVtable15e2028Slot2A0_e21420StatusMissing;
        if (hooks.on_missing != nullptr) {
            hooks.on_missing(cells.provider);
            out.missing_hook_called = true;
        }
        out.ok = true;
        return out;
    }

    const std::int32_t count = hooks.provider_count(cells.provider);
    if (cells.name == nullptr && count > 1) {
        out.path = Vtable15e2028Slot2A0_e21420Path::scan_exhausted;
        out.status = kVtable15e2028Slot2A0_e21420StatusScan;
        out.ok = true;
        return out;
    }

    for (std::int32_t index = 0; index < count; ++index) {
        ++out.scanned;
        const char* candidate = hooks.provider_name(cells.provider, index);
        if (candidate == nullptr || cells.name == nullptr) {
            continue;
        }
        if (std::strcmp(candidate, cells.name) == 0) {
            out.path = Vtable15e2028Slot2A0_e21420Path::found;
            out.found_index = index;
            out.status = 0;
            if (hooks.on_found != nullptr) {
                hooks.on_found(cells.provider, index);
                out.found_hook_called = true;
            }
            out.ok = true;
            return out;
        }
    }

    out.path = Vtable15e2028Slot2A0_e21420Path::scan_exhausted;
    out.status = kVtable15e2028Slot2A0_e21420StatusScan;
    out.ok = true;
    return out;
}

}  // namespace sotc
