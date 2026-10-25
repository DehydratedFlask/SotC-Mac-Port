#include "registry_slot.hpp"

#include <new>

namespace sotc {

RegistrySlotInstallResult RegistrySlot::install(const RegistrySlotHooks& hooks) noexcept {
    RegistrySlotInstallResult out{};
    const bool has_previous = owner_ != nullptr;
    if (has_previous && hooks.dtor.free_tag2 == nullptr) {
        return out;  // no way to tear the previous owner down: touch nothing
    }

    // 0x256f98: allocate + construct the replacement before touching the previous owner.
    auto replacement = std::unique_ptr<RegistryOwner>(
        new (std::nothrow) RegistryOwner(hooks.delegates));
    if (replacement == nullptr) {
        return out;
    }
    out.created = true;

    if (has_previous) {
        // 0x256fc5/0x256fcd: destroy the previous container, then free its storage. The value
        // owner runs the same teardown; its destructor then finds the lists already reset.
        const ContainerDtor1bede0Result dtor =
            container_dtor_1bede0(owner_->registry().container(), hooks.dtor);
        out.replaced_previous = true;
        out.freed_previous_nodes = dtor.freed_a + dtor.freed_b + dtor.freed_c;
        out.previous_release_failures = dtor.release_failures;
        owner_.reset();
        out.ok = dtor.ok;
    } else {
        out.ok = true;
    }

    // 0x256fd2: store the new pointer in the slot.
    owner_ = std::move(replacement);
    ++replace_count_;
    return out;
}

}  // namespace sotc
