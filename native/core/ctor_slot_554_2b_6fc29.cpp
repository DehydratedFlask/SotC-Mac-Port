// CPU-01-STARTUP-SLOT-554-BATCH-02B: see ctor_slot_554_2b_6fc29.hpp.
#include "ctor_slot_554_2b_6fc29.hpp"

#include <cstring>

namespace sotc {
namespace {

SharedStr borrowed(const char* text, std::uint32_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text),
                     static_cast<std::int64_t>(length), 0, nullptr};
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

bool run_transfer(const Slot554B2BHooks& hooks, ConCmdFunctor& object, unsigned stage,
                  Slot554Batch2BResult& out) noexcept {
    if (hooks.transfer == nullptr) return true;
    if (!hooks.transfer(hooks.context, object, stage)) return false;
    ++out.transfers;
    return true;
}

std::uint32_t u32_at(const std::uint8_t* page, std::uint16_t offset) noexcept {
    std::uint32_t value = 0;
    std::memcpy(&value, page + offset, sizeof value);
    return value;
}

ConCmdFunctor* functor_at(Slot554Batch2BObjects& objects) noexcept {
    return reinterpret_cast<ConCmdFunctor*>(objects.objects.data());
}

Slot554B2ABool* bool_at(Slot554Batch2BObjects& objects, std::uint32_t index) noexcept {
    return reinterpret_cast<Slot554B2ABool*>(objects.objects.data() + kSlot554B2BFunctorSize +
                                             (index - 1) * kSlot554B2BBoolSize);
}

Slot554Batch2BResult init_functor(Slot554Batch2BObjects& objects, const std::uint8_t* page,
                                  ConCmdRegistry& registry, const Slot554B2BHooks& hooks,
                                  Slot554Batch2BResult& out) noexcept {
    const Slot554B2BSpec& spec = kSlot554B2BFunctor;
    out.blocked_index = 0;
    ConCmdFunctor& object = *functor_at(objects);
    const SharedStr name = borrowed(spec.name, spec.name_length);
    const SharedStr description = empty_description();
    const ConCmdFunctorView view{};   // caller view: all qwords and tag48 are zero
    ConCmdFunctorView installed = view;
    installed.vtable = kSlot554B2BViewVptr;   // identity tag used by the wrapper's temp view

    init_concmd_base_20af0(object.base, name, description, 1);
    object.base.vptr = kSlot554B2BInnerMidVptr;
    object.spec = reinterpret_cast<std::uint64_t>(page + spec.page_cell_offset);
    object.low = kSlot554B2BOccluderLow;
    object.one70 = 1;
    object.high = kSlot554B2BOccluderHigh;
    object.one78 = 1;
    std::memset(&object.view0, 0, 0x20);
    object.tag_c0 = kSlot554B2BTagVptr;
    object.view0 = installed.q0;
    object.view8 = installed.q8;
    object.view10 = installed.q10;
    object.view18 = installed.q18;
    if (object.view18 != 0) {
        object.view0 = reinterpret_cast<std::uint64_t>(&object.view0);
    }
    std::memset(object.region, 0, sizeof object.region);
    object.tag_c8 = 0;

    for (unsigned stage = 0; stage < 5; ++stage) {
        if (!run_transfer(hooks, object, stage, out)) return (void)(out.blocked = Slot554B2BStage::ctor), out;
    }
    object.tag_c8 = installed.tag48;

    object.base.vptr = kSlot554B2BFinalVptr;
    object.spec_word = u32_at(page, spec.page_cell_offset);
    if (!hooks.register_var(object.base, registry)) {
        out.blocked = Slot554B2BStage::registration;
        return out;
    }
    ++out.registered;

    if (!hooks.release_view(description)) {
        out.blocked = Slot554B2BStage::view_release;
        return out;
    }
    ++out.views_inert;
    if (hooks.destroy_view != nullptr) {
        if (!hooks.destroy_view(hooks.context, installed)) {
            out.blocked = Slot554B2BStage::ctor;
            return out;
        }
        ++out.view_destroys;
    }
    if (!hooks.release_view(name)) {
        out.blocked = Slot554B2BStage::view_release;
        return out;
    }
    ++out.views_inert;
    if (!hooks.atexit(kSlot554B2BDtorRva, &object, registry)) {
        out.blocked = Slot554B2BStage::atexit;
        return out;
    }
    ++out.atexit_records;
    ++out.built;
    out.ok = true;
    return out;
}

Slot554Batch2BResult init_bool(Slot554Batch2BObjects& objects, const std::uint8_t* page,
                               ConCmdRegistry& registry, const Slot554B2BHooks& hooks,
                               std::uint32_t index, Slot554Batch2BResult& out) noexcept {
    const Slot554B2BSpec& spec = kSlot554B2BBools[index - 1];
    out.blocked_index = index;
    Slot554B2ABool& var = *bool_at(objects, index);
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
    if (!hooks.release_view(description)) {
        out.blocked = Slot554B2BStage::view_release;
        return out;
    }
    ++out.views_inert;

    var.base.vptr = kSlot554B2AFinalVptr;
    var.default_value = page[spec.page_cell_offset];
    if (!hooks.register_var(var.base, registry)) {
        out.blocked = Slot554B2BStage::registration;
        return out;
    }
    ++out.registered;
    if (!hooks.release_view(name)) {
        out.blocked = Slot554B2BStage::view_release;
        return out;
    }
    ++out.views_inert;
    if (!hooks.atexit(kSlot554B2ADtorRva, &var, registry)) {
        out.blocked = Slot554B2BStage::atexit;
        return out;
    }
    ++out.atexit_records;
    ++out.built;
    out.ok = true;
    return out;
}

}  // namespace

bool slot554_batch2b_object_written(std::uint32_t index, std::size_t offset) noexcept {
    if (index >= kSlot554B2BObjectCount) return false;
    if (index == 0) {
        if (offset >= kSlot554B2BFunctorSize) return false;
        if (offset < 0x60) {
            return offset < 0x18 || offset == 0x18 || (offset >= 0x1c && offset < 0x60);
        }
        return !(offset >= 0x71 && offset < 0x74) && !(offset >= 0x79 && offset < 0x80);
    }
    return slot554_batch2a_object_written(0, offset);
}

bool slot554_batch2b_written(std::size_t offset) noexcept {
    if (offset >= kSlot554B2BBlockSize) return false;
    if (offset < kSlot554B2BFunctorSize) {
        return slot554_batch2b_object_written(0, offset);
    }
    const std::size_t bool_offset = offset - kSlot554B2BFunctorSize;
    return slot554_batch2b_object_written(
        1, bool_offset % kSlot554B2BBoolSize);
}

Slot554B2BHooks default_slot554_batch2b_hooks() noexcept {
    Slot554B2BHooks hooks{};
    hooks.register_var = &default_register;
    hooks.atexit = &default_atexit;
    hooks.release_view = &default_release;
    hooks.transfer = nullptr;
    hooks.destroy_view = nullptr;
    hooks.context = nullptr;
    return hooks;
}

Slot554Batch2BResult init_slot554_batch2b(Slot554Batch2BObjects& objects,
                                           const std::uint8_t* page,
                                           ConCmdRegistry& registry,
                                           const Slot554B2BHooks& hooks) noexcept {
    Slot554Batch2BResult out{};
    if (page == nullptr || hooks.register_var == nullptr || hooks.atexit == nullptr ||
        hooks.release_view == nullptr) {
        out.blocked = Slot554B2BStage::page;
        return out;
    }

    out = init_functor(objects, page, registry, hooks, out);
    if (!out.ok) {
        out.blocked = out.blocked == Slot554B2BStage::none ? Slot554B2BStage::ctor : out.blocked;
        return out;
    }
    for (std::uint32_t i = 1; i < kSlot554B2BObjectCount; ++i) {
        out = init_bool(objects, page, registry, hooks, i, out);
        if (!out.ok) {
            out.blocked = out.blocked == Slot554B2BStage::none ? Slot554B2BStage::ctor : out.blocked;
            return out;
        }
    }
    out.blocked = Slot554B2BStage::none;
    out.ok = true;
    return out;
}

}  // namespace sotc
