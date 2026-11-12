// CPU-01-STARTUP-SLOT-554-BATCH-07: see ctor_slot_554_batch7_7a68c.hpp.
#include "ctor_slot_554_batch7_7a68c.hpp"

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

std::uint32_t u32_at(const std::uint8_t* page, std::uint16_t offset) noexcept {
    std::uint32_t value = 0;
    std::memcpy(&value, page + offset, sizeof value);
    return value;
}

std::uint8_t* object_at(Slot554Batch7Objects& objects, std::uint32_t index) noexcept {
    std::size_t offset = 0;
    for (std::uint32_t i = 0; i < index; ++i) offset += kSlot554B7Specs[i].size;
    return objects.objects.data() + offset;
}

void init_bool(const Slot554B7Spec& spec, std::uint8_t* raw, const std::uint8_t* page,
               ConCmdRegistry& registry, const Slot554B7Hooks& hooks,
               Slot554Batch7Result& out) noexcept {
    Slot554B2ABool& var = *reinterpret_cast<Slot554B2ABool*>(raw);
    const SharedStr name = borrowed(spec.name, spec.name_length);
    const SharedStr description = empty_description();

    init_concmd_base_20af0(var.base, name, description, 1);
    var.base.vptr = kSlot554B7BoolMidVptr;
    var.value_ref = const_cast<std::uint8_t*>(page) + spec.page_cell_offset;
    std::memset(var.zero69, 0, sizeof var.zero69);
    std::memset(var.zero70, 0, sizeof var.zero70);
    var.fn_payload = 0;
    var.fn_vptr = kSlot554B7BoolFnVtbl;
    var.fn_aux = 0;
    if (!hooks.release_view(description)) {
        out.blocked = Slot554B7Stage::view_release;
        return;
    }
    ++out.views_inert;
    var.base.vptr = kSlot554B7BoolFinalVptr;
    var.default_value = page[spec.page_cell_offset];
    if (!hooks.register_var(var.base, registry)) {
        out.blocked = Slot554B7Stage::registration;
        return;
    }
    ++out.registered;
    if (!hooks.release_view(name)) {
        out.blocked = Slot554B7Stage::view_release;
        return;
    }
    ++out.views_inert;
    if (!hooks.atexit(kSlot554B7BoolDtorRva, raw, registry)) {
        out.blocked = Slot554B7Stage::atexit;
        return;
    }
    ++out.atexit_records;
    ++out.built;
}

void init_float(const Slot554B7Spec& spec, std::uint8_t* raw, const std::uint8_t* page,
                ConCmdRegistry& registry, const Slot554B7Hooks& hooks,
                Slot554Batch7Result& out) noexcept {
    ConCmdRangedVar& var = *reinterpret_cast<ConCmdRangedVar*>(raw);
    const SharedStr name = borrowed(spec.name, spec.name_length);
    const SharedStr description = empty_description();
    RangedVarArgs args{};
    args.fn_vptr = kSlot554B7FloatArgFnVtbl;
    float low = 0.0f;
    float high = 0.0f;
    std::memcpy(&low, &spec.low, sizeof low);
    std::memcpy(&high, &spec.high, sizeof high);
    if (!init_ranged_float_var_c300(var, name,
                                    const_cast<std::uint8_t*>(page) + spec.page_cell_offset,
                                    low, high, 1, args, description)) {
        out.blocked = Slot554B7Stage::ctor;
        return;
    }
    var.value_ref = const_cast<std::uint8_t*>(page) + spec.page_cell_offset;
    var.base.vptr = kSlot554B7FloatFinalVptr;
    var.default_value = u32_at(page, spec.page_cell_offset);
    if (!hooks.release_view(description)) {
        out.blocked = Slot554B7Stage::view_release;
        return;
    }
    ++out.views_inert;
    if (!hooks.register_var(var.base, registry)) {
        out.blocked = Slot554B7Stage::registration;
        return;
    }
    ++out.registered;
    if (!hooks.release_view(name)) {
        out.blocked = Slot554B7Stage::view_release;
        return;
    }
    ++out.views_inert;
    if (!hooks.atexit(kSlot554B7FloatDtorRva, raw, registry)) {
        out.blocked = Slot554B7Stage::atexit;
        return;
    }
    ++out.atexit_records;
    ++out.built;
}

void init_int(const Slot554B7Spec& spec, std::uint8_t* raw, const std::uint8_t* page,
              ConCmdRegistry& registry, const Slot554B7Hooks& hooks,
              Slot554Batch7Result& out) noexcept {
    ConCmdRangedVar& var = *reinterpret_cast<ConCmdRangedVar*>(raw);
    const SharedStr name = borrowed(spec.name, spec.name_length);
    const SharedStr description = empty_description();
    RangedVarArgs args{};
    args.fn_vptr = kSlot554B7IntArgFnVtbl;
    const HelperCtorResult ctor = helper_3a980_unbounded_int(var, name, 1, args, description);
    if (!ctor.ok) {
        out.blocked = Slot554B7Stage::ctor;
        return;
    }
    var.value_ref = const_cast<std::uint8_t*>(page) + spec.page_cell_offset;
    var.base.vptr = kSlot554B7IntFinalVptr;
    var.default_value = u32_at(page, spec.page_cell_offset);
    if (!hooks.release_view(description)) {
        out.blocked = Slot554B7Stage::view_release;
        return;
    }
    ++out.views_inert;
    if (!hooks.register_var(var.base, registry)) {
        out.blocked = Slot554B7Stage::registration;
        return;
    }
    ++out.registered;
    if (!hooks.release_view(name)) {
        out.blocked = Slot554B7Stage::view_release;
        return;
    }
    ++out.views_inert;
    if (!hooks.atexit(kSlot554B7IntDtorRva, raw, registry)) {
        out.blocked = Slot554B7Stage::atexit;
        return;
    }
    ++out.atexit_records;
    ++out.built;
}

// 0xc580 receives `object + 0xd0` as its value cell: there is no page read and the object is
// 0xd8 B. The body then stores a zero dword into that same cell (`movl $0x0, 0xd0(%r12)`).
void init_dword(const Slot554B7Spec& spec, std::uint8_t* raw, const std::uint8_t* page,
                ConCmdRegistry& registry, const Slot554B7Hooks& hooks,
                Slot554Batch7Result& out) noexcept {
    (void)page;
    ConCmdRangedVar& var = *reinterpret_cast<ConCmdRangedVar*>(raw);
    const SharedStr name = borrowed(spec.name, spec.name_length);
    const SharedStr description = empty_description();
    RangedVarArgs args{};
    args.fn_vptr = kSlot554B7IntArgFnVtbl;
    std::uint8_t* cell = raw + kSlot554B7DwordCellOffset;
    const HelperCtorResult ctor =
        helper_c580_ranged_int(var, name, cell, 0, 0x10, 1, args, description);
    if (!ctor.ok) {
        out.blocked = Slot554B7Stage::ctor;
        return;
    }
    var.value_ref = cell;
    var.base.vptr = kSlot554B7DwordFinalVptr;
    var.default_value = 0;
    std::uint32_t zero = 0;
    std::memcpy(cell, &zero, sizeof zero);
    if (!hooks.release_view(description)) {
        out.blocked = Slot554B7Stage::view_release;
        return;
    }
    ++out.views_inert;
    if (!hooks.register_var(var.base, registry)) {
        out.blocked = Slot554B7Stage::registration;
        return;
    }
    ++out.registered;
    if (!hooks.release_view(name)) {
        out.blocked = Slot554B7Stage::view_release;
        return;
    }
    ++out.views_inert;
    if (!hooks.atexit(kSlot554B7DwordDtorRva, raw, registry)) {
        out.blocked = Slot554B7Stage::atexit;
        return;
    }
    ++out.atexit_records;
    ++out.built;
}

void init_wrapper(const Slot554B7Spec& spec, std::uint8_t* raw, const std::uint8_t* page,
                  ConCmdRegistry& registry, const Slot554B7Hooks& hooks,
                  Slot554Batch7Result& out) noexcept {
    ConCmdFunctor& object = *reinterpret_cast<ConCmdFunctor*>(raw);
    const SharedStr name = borrowed(spec.name, spec.name_length);
    const SharedStr description = empty_description();
    ConCmdFunctorView installed{};
    installed.vtable = spec.tag_vptr;

    init_concmd_base_20af0(object.base, name, description, 1);
    object.base.vptr = spec.inner_vptr;
    object.spec = reinterpret_cast<std::uint64_t>(page + spec.page_cell_offset);
    object.low = spec.low;
    object.one70 = 1;
    object.high = spec.high;
    object.one78 = 1;
    std::memset(&object.view0, 0, 0x20);
    object.tag_c0 = spec.tag_vptr;
    object.view0 = installed.q0;
    object.view8 = installed.q8;
    object.view10 = installed.q10;
    object.view18 = installed.q18;
    if (object.view18 != 0) object.view0 = reinterpret_cast<std::uint64_t>(&object.view0);
    std::memset(object.region, 0, sizeof object.region);
    object.tag_c8 = 0;

    if (hooks.wrapper_call != nullptr) {
        if (!hooks.wrapper_call(hooks.context, spec.wrapper_rva, object)) {
            out.blocked = Slot554B7Stage::ctor;
            return;
        }
        ++out.wrapper_calls;
    }
    object.tag_c8 = installed.tag48;
    object.base.vptr = spec.final_vptr;
    object.spec_word = u32_at(page, spec.page_cell_offset);
    if (!hooks.register_var(object.base, registry)) {
        out.blocked = Slot554B7Stage::registration;
        return;
    }
    ++out.registered;
    if (!hooks.release_view(description)) {
        out.blocked = Slot554B7Stage::view_release;
        return;
    }
    ++out.views_inert;
    if (hooks.destroy_view != nullptr) {
        if (!hooks.destroy_view(hooks.context, installed)) {
            out.blocked = Slot554B7Stage::ctor;
            return;
        }
        ++out.view_destroys;
    }
    if (!hooks.release_view(name)) {
        out.blocked = Slot554B7Stage::view_release;
        return;
    }
    ++out.views_inert;
    if (!hooks.atexit(spec.dtor_rva, raw, registry)) {
        out.blocked = Slot554B7Stage::atexit;
        return;
    }
    ++out.atexit_records;
    ++out.built;
}

}  // namespace

bool slot554_batch7_object_written(std::uint32_t index, std::size_t offset) noexcept {
    if (index >= kSlot554B7ObjectCount) return false;
    const Slot554B7Spec& spec = kSlot554B7Specs[index];
    if (offset >= spec.size) return false;
    switch (spec.kind) {
        case Slot554B7Kind::bool_var:
            return slot554_batch2a_object_written(0, offset);
        case Slot554B7Kind::float_var:
            // 0xc300 writes both bounds as full dwords (+0x6c..+0x70, +0x74..+0x78).
            if ((offset >= 0x6d && offset < 0x70) || (offset >= 0x75 && offset < 0x78)) return true;
            return slot554_object_written(2, offset);
        case Slot554B7Kind::int_var:
            return slot554_object_written(0, offset);
        case Slot554B7Kind::dword_var:
            // 0xc580 fills 0xd0 and the body stores a zero dword into the object-hosted cell.
            if (offset >= kSlot554B7DwordCellOffset && offset < kSlot554B7DwordCellOffset + 4)
                return true;
            if ((offset >= 0x6d && offset < 0x70) || (offset >= 0x75 && offset < 0x78)) return true;
            return slot554_object_written(2, offset);
        case Slot554B7Kind::wrapper:
            return slot554_batch2b_object_written(0, offset);
    }
    return false;
}

bool slot554_batch7_written(std::size_t offset) noexcept {
    if (offset >= kSlot554B7BlockSize) return false;
    std::size_t base = 0;
    for (std::uint32_t i = 0; i < kSlot554B7ObjectCount; ++i) {
        const Slot554B7Spec& spec = kSlot554B7Specs[i];
        if (offset < base + spec.size) return slot554_batch7_object_written(i, offset - base);
        base += spec.size;
    }
    return false;
}

Slot554B7Hooks default_slot554_batch7_hooks() noexcept {
    Slot554B7Hooks hooks{};
    hooks.register_var = &default_register;
    hooks.atexit = &default_atexit;
    hooks.release_view = &default_release;
    hooks.wrapper_call = nullptr;
    hooks.destroy_view = nullptr;
    hooks.context = nullptr;
    return hooks;
}

Slot554Batch7Result init_slot554_batch7(Slot554Batch7Objects& objects, const std::uint8_t* page,
                                        ConCmdRegistry& registry,
                                        const Slot554B7Hooks& hooks) noexcept {
    Slot554Batch7Result out{};
    if (page == nullptr || hooks.register_var == nullptr || hooks.atexit == nullptr ||
        hooks.release_view == nullptr) {
        out.blocked = Slot554B7Stage::page;
        return out;
    }

    for (std::uint32_t i = 0; i < kSlot554B7ObjectCount; ++i) {
        const Slot554B7Spec& spec = kSlot554B7Specs[i];
        out.blocked_index = i;
        std::uint8_t* raw = object_at(objects, i);
        switch (spec.kind) {
            case Slot554B7Kind::bool_var:
                init_bool(spec, raw, page, registry, hooks, out);
                break;
            case Slot554B7Kind::float_var:
                init_float(spec, raw, page, registry, hooks, out);
                break;
            case Slot554B7Kind::int_var:
                init_int(spec, raw, page, registry, hooks, out);
                break;
            case Slot554B7Kind::dword_var:
                init_dword(spec, raw, page, registry, hooks, out);
                break;
            case Slot554B7Kind::wrapper:
                init_wrapper(spec, raw, page, registry, hooks, out);
                break;
        }
        if (out.built != i + 1) {
            out.ok = false;
            out.blocked = out.blocked == Slot554B7Stage::none ? Slot554B7Stage::ctor : out.blocked;
            return out;
        }
    }
    out.blocked = Slot554B7Stage::none;
    out.ok = true;
    return out;
}

}  // namespace sotc
