#include "ctor_slot_554_6efe0.hpp"

#include <cstring>

#include "ctor_slot_554_page_init.inc"
#include "ctor_shapes_343_356.hpp"
#include "ctor_slots_322_334.hpp"
#include "helper_3a980_unbounded_int.hpp"
#include "helper_6aa00_unbounded_float.hpp"
#include "helper_functor_tags.hpp"

namespace sotc {

namespace {

SharedStr borrowed(const char* text, std::uint32_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(length), 0, nullptr};
}

SharedStr empty_description() noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(""), 0, 0, nullptr};
}

std::uint32_t page_dword(const std::uint8_t* page, std::size_t offset) noexcept {
    std::uint32_t value = 0;
    std::memcpy(&value, page + offset, sizeof value);
    return value;
}

bool default_page_init(std::uint8_t* page, std::size_t size) noexcept {
    if (page == nullptr || size != kSlot554PageSize) return false;
    slot554_init_page(page, size);
    return true;
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

bool default_destroy(std::uint64_t arg_fn_vtbl) noexcept {
    return is_int_functor_tag(arg_fn_vtbl) || is_float_functor_tag(arg_fn_vtbl);
}

struct RangedFields {
    ConCmdBase base;
    void* value_ref;
    std::uint32_t default_value;
    std::uint8_t min_value[4];
    std::uint8_t min_engaged;
    std::uint8_t gap71[3];
    std::uint8_t max_value[4];
    std::uint8_t max_engaged;
    std::uint8_t gap79[7];
    std::uint64_t args[4];
    std::uint8_t fn_inline[0x10];
    std::uint64_t fn_payload;
    std::uint64_t gapb8;
    std::uint64_t fn_vptr;
    std::uint64_t fn_aux;
};
static_assert(sizeof(RangedFields) == kSlot554RangedSize);
static_assert(offsetof(RangedFields, value_ref) == 0x60 && offsetof(RangedFields, default_value) == 0x68);

struct BoolFields {
    ConCmdBase base;
    std::uint8_t* value_ref;
    std::uint8_t default_value;
    std::uint8_t zero69[4];
    std::uint8_t gap6d[3];
    std::uint8_t zero70[0x20];
    std::uint8_t gap90[0x10];
    std::uint64_t fn_payload;
    std::uint8_t gapa8[8];
    std::uint64_t fn_vptr;
    std::uint64_t fn_aux;
};
static_assert(sizeof(BoolFields) == kSlot554BoolSize);
static_assert(offsetof(BoolFields, value_ref) == 0x60 && offsetof(BoolFields, default_value) == 0x68);

std::uint8_t* object_at(Slot554Objects& objects, std::uint32_t index) noexcept {
    std::size_t offset = 0;
    for (std::uint32_t i = 0; i < index; ++i)
        offset += kSlot554Vars[i].size;
    return objects.objects.data() + offset;
}


}  // namespace

bool slot554_page_written(std::size_t offset) noexcept {
    if (offset >= kSlot554PageSize) return false;
    for (const Slot554PageStore& store : kSlot554PageStores) {
        if (offset >= store.offset && offset < store.offset + store.size) return true;
    }
    return false;
}

bool slot554_object_written(std::uint32_t index, std::size_t offset) noexcept {
    if (index >= kSlot554VariableCount) return false;
    const Slot554VarSpec& spec = kSlot554Vars[index];
    if (offset >= spec.size) return false;
    if (spec.kind == Slot554Kind::bool_var) {
        if (offset < 0x60) return offset < 0x18 || offset == 0x18 || (offset >= 0x1c && offset < 0x60);
        return (offset >= 0x60 && offset < 0x69) || (offset >= 0x69 && offset < 0x6d) ||
               (offset >= 0x70 && offset < 0x90) || (offset >= 0xa0 && offset < 0xa8) ||
               (offset >= 0xb0 && offset < 0xc0);
    }
    if (offset < 0x60) return offset < 0x18 || offset == 0x18 || (offset >= 0x1c && offset < 0x60);
    return (offset >= 0x60 && offset < 0x6c) || (offset >= 0x6c && offset < 0x6d) ||
           (offset >= 0x70 && offset < 0x71) || (offset >= 0x74 && offset < 0x75) ||
           (offset >= 0x78 && offset < 0x79) || (offset >= 0x80 && offset < 0xa0) ||
           (offset >= 0xb0 && offset < 0xb8) || (offset >= 0xc0 && offset < 0xd0);
}

bool slot554_written(std::size_t offset) noexcept {
    if (offset < kSlot554PageSize) return slot554_page_written(offset);
    if (offset >= kSlot554BlockSize) return false;
    std::size_t rel = offset - kSlot554ObjectOffset;
    for (std::uint32_t i = 0, base = 0; i < kSlot554VariableCount; base += kSlot554Vars[i].size, ++i) {
        if (rel < base + kSlot554Vars[i].size) return slot554_object_written(i, rel - base);
    }
    return false;
}

void slot554_init_page(std::uint8_t* page, std::size_t size) noexcept {
    if (page == nullptr || size != kSlot554PageSize) return;
    for (const Slot554PageStore& store : kSlot554PageStores) {
        if (store.offset + store.size > size) continue;
        std::memcpy(page + store.offset, store.bytes.data(), store.size);
    }
}

Slot554Hooks default_slot554_hooks() noexcept {
    Slot554Hooks hooks{};
    hooks.page_init = &default_page_init;
    hooks.register_var = &default_register;
    hooks.atexit = &default_atexit;
    hooks.release_view = &default_release;
    hooks.destroy_arg = &default_destroy;
    return hooks;
}

Slot554Result init_slot554_batch1(Slot554Objects& objects, ConCmdRegistry& registry,
                                  const Slot554Hooks& hooks) noexcept {
    Slot554Result out{};
    if (hooks.page_init == nullptr || hooks.register_var == nullptr || hooks.atexit == nullptr ||
        hooks.release_view == nullptr || hooks.destroy_arg == nullptr) {
        out.blocked = Slot554Stage::page_init;
        return out;
    }
    if (!hooks.page_init(objects.page.data(), objects.page.size())) {
        out.blocked = Slot554Stage::page_init;
        return out;
    }
    out.page_ok = true;

    for (std::uint32_t i = 0; i < kSlot554VariableCount; ++i) {
        const Slot554VarSpec& spec = kSlot554Vars[i];
        out.blocked_index = i;
        std::uint8_t* raw = object_at(objects, i);

        if (spec.kind == Slot554Kind::bool_var) {
            auto& var = *reinterpret_cast<BoolFields*>(raw);
            const SharedStr name = borrowed(spec.name, spec.name_length);
            const SharedStr description = empty_description();
            init_concmd_base_20af0(var.base, name, description, 1);
            var.base.vptr = kSlot554BoolMidVptr;
            var.value_ref = objects.page.data() + spec.page_cell_offset;
            std::memset(var.zero69, 0, sizeof var.zero69);
            std::memset(var.zero70, 0, sizeof var.zero70);
            var.fn_payload = 0;
            var.fn_vptr = kSlot554BoolFnVtbl;
            var.fn_aux = 0;
                    if (!hooks.release_view(description)) return (void)(out.blocked = Slot554Stage::view_release), out;
            ++out.views_inert;
            var.base.vptr = spec.final_vptr;
            var.default_value = objects.page[spec.page_cell_offset];
            if (!hooks.register_var(var.base, registry)) return (void)(out.blocked = Slot554Stage::registration), out;
            ++out.registered;
            if (!hooks.release_view(name)) return (void)(out.blocked = Slot554Stage::view_release), out;
            ++out.views_inert;
            if (!hooks.atexit(spec.dtor_rva, raw, registry)) return (void)(out.blocked = Slot554Stage::atexit), out;
            ++out.atexit_records;
            ++out.built;
            continue;
        }

        auto& var = *reinterpret_cast<ConCmdRangedVar*>(raw);
        const SharedStr name = borrowed(spec.name, spec.name_length);
        const SharedStr description = empty_description();
        RangedVarArgs args{};
        args.fn_vptr = spec.kind == Slot554Kind::int_var ? kSlot554IntArgFnVtbl : kSlot554FloatArgFnVtbl;
        bool built = false;
        if (spec.kind == Slot554Kind::int_var) {
            const HelperCtorResult r = helper_3a980_unbounded_int(var, name, 1, args, description);
            built = r.ok;
            var.value_ref = objects.page.data() + spec.page_cell_offset;
        } else {
            const HelperCtorResult r = helper_6aa00_unbounded_float(var, name, objects.page.data() + spec.page_cell_offset,
                                                                    1, args, description);
            built = r.ok;
        }
        if (!built) return (void)(out.blocked = Slot554Stage::ctor), out;
        if (!hooks.release_view(description)) return (void)(out.blocked = Slot554Stage::view_release), out;
        ++out.views_inert;
        if (!hooks.destroy_arg(args.fn_vptr)) return (void)(out.blocked = Slot554Stage::arg_destroy), out;
        ++out.functors_destroyed;
        var.base.vptr = spec.final_vptr;
        var.default_value = page_dword(objects.page.data(), spec.page_cell_offset);
        if (!hooks.register_var(var.base, registry)) return (void)(out.blocked = Slot554Stage::registration), out;
        ++out.registered;
        if (!hooks.release_view(name)) return (void)(out.blocked = Slot554Stage::view_release), out;
        ++out.views_inert;
        if (!hooks.atexit(spec.dtor_rva, raw, registry)) return (void)(out.blocked = Slot554Stage::atexit), out;
        ++out.atexit_records;
        ++out.built;
    }
    out.blocked = Slot554Stage::none;
    out.ok = true;
    return out;
}

}  // namespace sotc
