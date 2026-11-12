// CPU-01-STARTUP-SLOT-554-BATCH-13: see ctor_slot_554_batch13_8849d.hpp.
#include "ctor_slot_554_batch13_8849d.hpp"

#include <cstring>

#include "helper_c580_ranged_int.hpp"
#include "ctor_slot_554_6efe0.hpp"

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
bool default_release(const SharedStr& view) noexcept { return release_inert_view_f590(view); }
std::uint32_t u32_at(const std::uint8_t* page, std::uint16_t offset) noexcept {
    std::uint32_t value = 0;
    std::memcpy(&value, page + offset, sizeof value);
    return value;
}
std::uint8_t* object_at(Slot554Batch13Objects& objects, std::uint32_t index) noexcept {
    if (index >= kSlot554B13BlockCount) return objects.dummy.data();
    std::size_t offset = 0;
    for (std::uint32_t i = 0; i < index; ++i) offset += kSlot554B13Specs[i].size;
    return objects.block.data() + offset;
}

void init_bool(const Slot554B13Spec& spec, std::uint8_t* raw, const std::uint8_t* page,
               ConCmdRegistry& registry, const Slot554B13Hooks& hooks,
               Slot554Batch13Result& out) noexcept {
    Slot554B2ABool& var = *reinterpret_cast<Slot554B2ABool*>(raw);
    const SharedStr name = borrowed(spec.name, spec.name_length);
    const SharedStr description = empty_description();
    init_concmd_base_20af0(var.base, name, description, 1);
    var.base.vptr = kSlot554B13BoolMidVptr;
    var.value_ref = const_cast<std::uint8_t*>(page) + spec.page_cell_offset;
    std::memset(var.zero69, 0, sizeof var.zero69);
    std::memset(var.zero70, 0, sizeof var.zero70);
    var.fn_payload = 0;
    var.fn_vptr = kSlot554B13BoolFnVtbl;
    var.fn_aux = 0;
    if (!hooks.release_view(description)) { out.blocked = Slot554B13Stage::view_release; return; }
    ++out.views_inert;
    var.base.vptr = kSlot554B13BoolFinalVptr;
    var.default_value = page[spec.page_cell_offset];
    if (!hooks.register_var(var.base, registry)) { out.blocked = Slot554B13Stage::registration; return; }
    ++out.registered;
    if (!hooks.release_view(name)) { out.blocked = Slot554B13Stage::view_release; return; }
    ++out.views_inert;
    if (!hooks.atexit(kSlot554B13BoolDtorRva, raw, registry)) { out.blocked = Slot554B13Stage::atexit; return; }
    ++out.atexit_records;
    ++out.built;
}

void init_float(const Slot554B13Spec& spec, std::uint8_t* raw, const std::uint8_t* page,
                ConCmdRegistry& registry, const Slot554B13Hooks& hooks,
                Slot554Batch13Result& out) noexcept {
    ConCmdRangedVar& var = *reinterpret_cast<ConCmdRangedVar*>(raw);
    const SharedStr name = borrowed(spec.name, spec.name_length);
    const SharedStr description = empty_description();
    RangedVarArgs args{};
    args.fn_vptr = kSlot554B13FloatArgFnVtbl;
    float low = 0.0f, high = 0.0f;
    std::memcpy(&low, &spec.low, sizeof low);
    std::memcpy(&high, &spec.high, sizeof high);
    if (!init_ranged_float_var_c300(var, name, const_cast<std::uint8_t*>(page) + spec.page_cell_offset,
                                    low, high, 1, args, description)) {
        out.blocked = Slot554B13Stage::ctor; return;
    }
    var.value_ref = const_cast<std::uint8_t*>(page) + spec.page_cell_offset;
    var.base.vptr = kSlot554B13FloatFinalVptr;
    var.default_value = u32_at(page, spec.page_cell_offset);
    if (!hooks.release_view(description)) { out.blocked = Slot554B13Stage::view_release; return; }
    ++out.views_inert;
    if (!hooks.register_var(var.base, registry)) { out.blocked = Slot554B13Stage::registration; return; }
    ++out.registered;
    if (!hooks.release_view(name)) { out.blocked = Slot554B13Stage::view_release; return; }
    ++out.views_inert;
    if (!hooks.atexit(spec.dtor_rva, raw, registry)) {
        out.blocked = Slot554B13Stage::atexit; return;
    }
    ++out.atexit_records;
    ++out.built;
}

void init_dword(const Slot554B13Spec& spec, std::uint8_t* raw, const std::uint8_t* page,
                ConCmdRegistry& registry, const Slot554B13Hooks& hooks,
                Slot554Batch13Result& out) noexcept {
    ConCmdRangedVar& var = *reinterpret_cast<ConCmdRangedVar*>(raw);
    const SharedStr name = borrowed(spec.name, spec.name_length);
    const SharedStr description = empty_description();
    RangedVarArgs args{};
    args.fn_vptr = kSlot554B13DwordArgFnVtbl;
    const bool owned = spec.kind == Slot554B13Kind::dword_owned;
    std::uint8_t* cell = owned ? raw + 0xd0 : const_cast<std::uint8_t*>(page) + spec.page_cell_offset;
    const std::uint32_t lo = owned ? 0u : spec.low;
    const std::uint32_t hi = owned ? 4u : spec.high;
    if (!helper_c580_ranged_int(var, name, cell, static_cast<std::int32_t>(lo),
                                static_cast<std::int32_t>(hi), 1, args, description).ok) {
        out.blocked = Slot554B13Stage::ctor; return;
    }
    var.value_ref = cell;
    var.base.vptr = kSlot554B13DwordFinalVptr;
    var.default_value = owned ? spec.owned_value : u32_at(page, spec.page_cell_offset);
    if (owned) {
        std::memcpy(cell, &spec.owned_value, sizeof spec.owned_value);
    }
    if (!hooks.release_view(description)) { out.blocked = Slot554B13Stage::view_release; return; }
    ++out.views_inert;
    if (!hooks.register_var(var.base, registry)) { out.blocked = Slot554B13Stage::registration; return; }
    ++out.registered;
    if (!hooks.release_view(name)) { out.blocked = Slot554B13Stage::view_release; return; }
    ++out.views_inert;
    if (!hooks.atexit(kSlot554B13DwordDtorRva, raw, registry)) { out.blocked = Slot554B13Stage::atexit; return; }
    ++out.atexit_records;
    ++out.built;
}

void init_wrapper(const Slot554B13Spec& spec, std::uint8_t* raw, const std::uint8_t* page,
                  ConCmdRegistry& registry, const Slot554B13Hooks& hooks,
                  Slot554Batch13Result& out) noexcept {
    ConCmdFunctor& object = *reinterpret_cast<ConCmdFunctor*>(raw);
    const SharedStr name = borrowed(spec.name, spec.name_length);
    const SharedStr description = empty_description();
    ConCmdFunctorView installed{};
    installed.vtable = spec.tag_vptr;
    init_concmd_base_20af0(object.base, name, description, 1);
    object.base.vptr = spec.tag_vptr;
    object.spec = reinterpret_cast<std::uint64_t>(const_cast<std::uint8_t*>(page) + spec.page_cell_offset);
    object.low = spec.low;
    object.one70 = 0;
    object.high = spec.high;
    object.one78 = 0;
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
            out.blocked = Slot554B13Stage::ctor; return;
        }
        ++out.wrapper_calls;
    }
    object.tag_c8 = installed.tag48;
    object.base.vptr = spec.final_vptr;
    object.spec_word = u32_at(page, spec.page_cell_offset);
    if (!hooks.register_var(object.base, registry)) { out.blocked = Slot554B13Stage::registration; return; }
    ++out.registered;
    if (!hooks.release_view(description)) { out.blocked = Slot554B13Stage::view_release; return; }
    ++out.views_inert;
    if (hooks.destroy_view != nullptr) {
        if (!hooks.destroy_view(hooks.context, installed)) { out.blocked = Slot554B13Stage::ctor; return; }
        ++out.view_destroys;
    }
    if (!hooks.release_view(name)) { out.blocked = Slot554B13Stage::view_release; return; }
    ++out.views_inert;
    if (!hooks.atexit(spec.dtor_rva, raw, registry)) { out.blocked = Slot554B13Stage::atexit; return; }
    ++out.atexit_records;
    ++out.built;
}

}  // namespace

bool slot554_batch13_object_written(std::uint32_t index, std::size_t offset) noexcept {
    if (index > kSlot554B13BlockCount) return false;
    if (index == kSlot554B13BlockCount) {
        if (offset >= kSlot554B13DummySize) return false;
        if (offset < 0x60) return offset < 0x18 || offset == 0x18 || (offset >= 0x1c && offset < 0x60);
        return offset < 0x98;
    }
    const Slot554B13Spec& spec = kSlot554B13Specs[index];
    if (offset >= spec.size) return false;
    switch (spec.kind) {
        case Slot554B13Kind::bool_var: return slot554_batch2a_object_written(0, offset);
        case Slot554B13Kind::float_var: return slot554_object_written(2, offset) ||
            ((offset >= 0x6d && offset < 0x70) || (offset >= 0x75 && offset < 0x78));
        case Slot554B13Kind::dword_page: return slot554_object_written(2, offset) ||
            ((offset >= 0x6d && offset < 0x70) || (offset >= 0x75 && offset < 0x78));
        case Slot554B13Kind::dword_owned:
            if (offset >= 0xd0 && offset < 0xd4) return true;
            return slot554_object_written(2, offset) ||
                ((offset >= 0x6d && offset < 0x70) || (offset >= 0x75 && offset < 0x78));
        case Slot554B13Kind::wrapper: return slot554_batch2b_object_written(0, offset);
    }
    return false;
}

bool slot554_batch13_written(std::size_t offset) noexcept {
    if (offset < kSlot554B13BlockSize) {
        std::size_t base = 0;
        for (std::uint32_t i = 0; i < kSlot554B13BlockCount; ++i) {
            if (offset < base + kSlot554B13Specs[i].size) return slot554_batch13_object_written(i, offset - base);
            base += kSlot554B13Specs[i].size;
        }
        return false;
    }
    return slot554_batch13_object_written(kSlot554B13BlockCount, offset - kSlot554B13BlockSize);
}

Slot554B13Hooks default_slot554_batch13_hooks() noexcept {
    Slot554B13Hooks hooks{};
    hooks.register_var = &default_register;
    hooks.atexit = &default_atexit;
    hooks.release_view = &default_release;
    return hooks;
}

Slot554Batch13Result init_slot554_batch13(Slot554Batch13Objects& objects, const std::uint8_t* page,
                                          ConCmdRegistry& registry, const Slot554B13Hooks& hooks) noexcept {
    Slot554Batch13Result out{};
    if (page == nullptr || hooks.register_var == nullptr || hooks.atexit == nullptr ||
        hooks.release_view == nullptr) { out.blocked = Slot554B13Stage::page; return out; }
    for (std::uint32_t i = 0; i < kSlot554B13BlockCount; ++i) {
        out.blocked_index = i;
        std::uint8_t* raw = object_at(objects, i);
        switch (kSlot554B13Specs[i].kind) {
            case Slot554B13Kind::bool_var: init_bool(kSlot554B13Specs[i], raw, page, registry, hooks, out); break;
            case Slot554B13Kind::float_var: init_float(kSlot554B13Specs[i], raw, page, registry, hooks, out); break;
            case Slot554B13Kind::dword_page:
            case Slot554B13Kind::dword_owned: init_dword(kSlot554B13Specs[i], raw, page, registry, hooks, out); break;
            case Slot554B13Kind::wrapper: init_wrapper(kSlot554B13Specs[i], raw, page, registry, hooks, out); break;
        }
        if (out.built != i + 1) { out.ok = false; return out; }
    }
    out.blocked_index = kSlot554B13BlockCount;
    ConCmdCommand& dummy = *reinterpret_cast<ConCmdCommand*>(objects.dummy.data());
    const FConCmdCommandSpec spec{"option_unavailable",
        "Option unavailable in this build. Params: None", 0x39, kSlot554B13DummyHandlerRva};
    if (!init_fconcmd_object(dummy, spec, registry).ok) { out.blocked = Slot554B13Stage::ctor; return out; }
    ++out.built;
    ++out.registered;
    out.atexit_records += 1;
    out.views_inert += 2;
    out.blocked = Slot554B13Stage::none;
    out.ok = true;
    return out;
}

}  // namespace sotc
