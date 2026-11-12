// CPU-01-STARTUP-SLOT-554-BATCH-02A: see ctor_slot_554_2a_6f793.hpp.
#include "ctor_slot_554_2a_6f793.hpp"

#include <cstring>

namespace sotc {
namespace {

SharedStr borrowed(const char* text, std::uint32_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(length), 0, nullptr};
}

SharedStr empty_description() noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(""), 0, 0, nullptr};
}

bool default_register(ConCmdBase& base, ConCmdRegistry& registry) noexcept {
    return register_concmd_20d60(base, registry).ok;
}

bool default_atexit(std::uint64_t dtor, const void* object, ConCmdRegistry& registry) noexcept {
    return registry.atexit_log.record(dtor, object);
}

bool default_release(const SharedStr& view) noexcept {
    return release_inert_view_f590(view);
}

Slot554B2ABool* object_at(Slot554Batch2AObjects& objects, std::uint32_t index) noexcept {
    return reinterpret_cast<Slot554B2ABool*>(objects.objects.data() + index * kSlot554B2AObjectSize);
}

}  // namespace

bool slot554_batch2a_object_written(std::uint32_t index, std::size_t offset) noexcept {
    if (index >= kSlot554B2AObjectCount || offset >= kSlot554B2AObjectSize) return false;
    if (offset < 0x60) return offset < 0x18 || offset == 0x18 || (offset >= 0x1c && offset < 0x60);
    return (offset >= 0x60 && offset < 0x69) || (offset >= 0x69 && offset < 0x6d) ||
           (offset >= 0x70 && offset < 0x90) || (offset >= 0xa0 && offset < 0xa8) ||
           (offset >= 0xb0 && offset < 0xc0);
}

bool slot554_batch2a_written(std::size_t offset) noexcept {
    if (offset >= kSlot554B2ABlockSize) return false;
    return slot554_batch2a_object_written(static_cast<std::uint32_t>(offset / kSlot554B2AObjectSize),
                                          offset % kSlot554B2AObjectSize);
}

Slot554B2AHooks default_slot554_batch2a_hooks() noexcept {
    Slot554B2AHooks hooks{};
    hooks.register_var = &default_register;
    hooks.atexit = &default_atexit;
    hooks.release_view = &default_release;
    return hooks;
}

Slot554Batch2AResult init_slot554_batch2a(Slot554Batch2AObjects& objects, const std::uint8_t* page,
                                          ConCmdRegistry& registry, const Slot554B2AHooks& hooks) noexcept {
    Slot554Batch2AResult out{};
    if (page == nullptr || hooks.register_var == nullptr || hooks.atexit == nullptr ||
        hooks.release_view == nullptr) {
        out.blocked = Slot554B2AStage::page;
        return out;
    }

    for (std::uint32_t i = 0; i < kSlot554B2AObjectCount; ++i) {
        const Slot554B2ASpec& spec = kSlot554B2AVars[i];
        out.blocked_index = i;
        Slot554B2ABool& var = *object_at(objects, i);
        const SharedStr name = borrowed(spec.name, spec.name_length);
        const SharedStr description = empty_description();

        init_concmd_base_20af0(var.base, name, description, 1);
        var.base.vptr = kSlot554B2AMidVptr;
        var.value_ref = const_cast<std::uint8_t*>(page) + spec.page_cell_offset;
        std::memset(var.zero69, 0, sizeof var.zero69);
        std::memset(var.zero70, 0, sizeof var.zero70);
        var.fn_payload = 0;
        var.fn_vptr = kSlot554B2AFnVtbl;
        var.fn_aux = 0;
        if (!hooks.release_view(description)) return (void)(out.blocked = Slot554B2AStage::view_release), out;
        ++out.views_inert;

        var.base.vptr = kSlot554B2AFinalVptr;
        var.default_value = page[spec.page_cell_offset];
        if (!hooks.register_var(var.base, registry)) return (void)(out.blocked = Slot554B2AStage::registration), out;
        ++out.registered;
        if (!hooks.release_view(name)) return (void)(out.blocked = Slot554B2AStage::view_release), out;
        ++out.views_inert;
        if (!hooks.atexit(kSlot554B2ADtorRva, &var, registry)) return (void)(out.blocked = Slot554B2AStage::atexit), out;
        ++out.atexit_records;
        ++out.built;
    }
    out.blocked = Slot554B2AStage::none;
    out.ok = true;
    return out;
}

}  // namespace sotc
