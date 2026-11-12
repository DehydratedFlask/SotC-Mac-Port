// CPU-01-STARTUP-SLOT-497: see ctor_slot_497_262a80.hpp.
#include "ctor_slot_497_262a80.hpp"

#include <cstdlib>
#include <cstring>

#include "ctor_slot_326_837f30.hpp"
#include "ctor_slot_family_items.hpp"
#include "fmenu_ctors_slot306.hpp"
#include "string_var_257a0.hpp"

namespace sotc {
namespace {

SharedStr borrowed(const char* text, std::size_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text),
                     static_cast<std::int64_t>(length), 0, nullptr};
}

SharedStr empty_view() noexcept { return SharedStr{nullptr, 0, 0, nullptr}; }

template <class T>
T* at(Slot497Objects& objects, std::size_t offset) noexcept {
    return reinterpret_cast<T*>(objects.bytes.data() + offset);
}

void put_u32(Slot497Objects& objects, std::size_t offset, std::uint32_t value) noexcept {
    std::memcpy(objects.bytes.data() + offset, &value, sizeof value);
}

void put_u64(Slot497Objects& objects, std::size_t offset, std::uint64_t value) noexcept {
    std::memcpy(objects.bytes.data() + offset, &value, sizeof value);
}

bool menu_target_for(Slot497Objects& objects, std::uint32_t offset, DebugMenuTarget& out) noexcept {
    for (const Slot497VarSpec& spec : kSlot497Vars) {
        if (spec.offset != offset) continue;
        out.object = objects.bytes.data() + offset;
        out.known = true;
        switch (spec.type) {
            case Slot497VarType::bool_var: out.slot3 = 1; out.slot6 = 0; return true;
            case Slot497VarType::ranged_int: out.slot3 = 1; out.slot6 = 2; return true;
            case Slot497VarType::local_int_low:
            case Slot497VarType::local_int_high: out.slot3 = 1; out.slot6 = 1; return true;
            default: out.known = false; return false;
        }
    }
    out.known = false;
    return false;
}

void write_base(Slot497Objects& objects, std::size_t offset, const Slot497VarSpec& spec,
                std::uint64_t vptr) noexcept {
    ConCmdBase& base = *at<ConCmdBase>(objects, offset);
    base.vptr = vptr;
    base.registered = 0;
    base.flags = kSlot497VarFlags;
    base.name = borrowed(spec.name, spec.name_len);
    base.description = empty_view();
}

void build_bool_var(Slot497Objects& objects, const Slot497VarSpec& spec) noexcept {
    ConCmdBoolVar& var = *at<ConCmdBoolVar>(objects, spec.offset);
    write_base(objects, spec.offset, spec, kSlot497BoolMidVptr);
    var.value_ref = &var.value;
    var.default_value = static_cast<std::uint8_t>(spec.value);
    std::memset(var.zero69, 0, sizeof var.zero69);
    std::memset(var.zero70, 0, sizeof var.zero70);
    var.fn_payload = 0;
    var.fn_vptr = spec.fn_tag;
    var.fn_aux = 0;
    var.base.vptr = spec.final_vptr;
    var.value = static_cast<std::uint8_t>(spec.value);
}

void build_ranged_var(Slot497Objects& objects, const Slot497VarSpec& spec) noexcept {
    ConCmdRangedVar& var = *at<ConCmdRangedVar>(objects, spec.offset);
    write_base(objects, spec.offset, spec, kSlot497RangedMidVptr);
    var.value_ref = &var.value;
    var.default_value = spec.value;
    const bool dword_bounds = spec.type == Slot497VarType::ranged_int ||
                              spec.type == Slot497VarType::bounded_float ||
                              spec.type == Slot497VarType::local_int_low ||
                              spec.type == Slot497VarType::local_int_high;
    if (dword_bounds) {
        std::memcpy(var.min_value, &spec.min_value, sizeof var.min_value);
        var.min_engaged = 1;
        std::memcpy(var.max_value, &spec.max_value, sizeof var.max_value);
        var.max_engaged = 1;
    } else {
        // 0x6aa00 / 0x3a980 store a single zero byte at each of the four bound fields.
        objects.bytes[spec.offset + 0x6c] = 0;
        objects.bytes[spec.offset + 0x70] = 0;
        objects.bytes[spec.offset + 0x74] = 0;
        objects.bytes[spec.offset + 0x78] = 0;
    }
    std::memset(var.args, 0, sizeof var.args);
    var.fn_payload = 0;
    var.fn_vptr = spec.fn_tag;
    var.fn_aux = 0;
    var.base.vptr = spec.final_vptr;
    var.value = spec.value;
}

void build_string_var(Slot497Objects& objects, const Slot497VarSpec& spec) noexcept {
    ConCmdStringVar& var = *at<ConCmdStringVar>(objects, spec.offset);
    write_base(objects, spec.offset, spec, kSlot497StringFinalVptr);
    var.value_ref = &var.value;
    var.default_value = empty_view();
    var.byte88 = 0;
    var.bytea8 = 0;
    var.byteb0 = 0;
    var.byted0 = 0;
    std::memset(var.args, 0, sizeof var.args);
    var.fn_payload = 0;
    var.fn_vptr = kSlot497StringFnVtbl;
    var.fn_aux = 0;
    var.value = empty_view();
}

void build_var(Slot497Objects& objects, const Slot497VarSpec& spec) noexcept {
    switch (spec.type) {
        case Slot497VarType::bool_var: build_bool_var(objects, spec); return;
        case Slot497VarType::string_var: build_string_var(objects, spec); return;
        case Slot497VarType::ranged_int:
        case Slot497VarType::unbounded_float:
        case Slot497VarType::bounded_float:
        case Slot497VarType::unbounded_int:
        case Slot497VarType::local_int_low:
        case Slot497VarType::local_int_high: build_ranged_var(objects, spec); return;
    }
}

// 0x26800 / 0x26b60 / 0x26c10 / 0x26ea0 / 0x27050 all leave the same two holes in the 0xa4-byte
// payload (+0x21..+0x27 and +0x4c..+0x4f). They are built by the native models; this function only
// dispatches and returns the views the body releases afterwards.
bool build_menu_item(Slot497Objects& objects, DebugMenuItem& item, const Slot497MenuSpec& spec,
                     const SharedStrCtorHooks& alloc) noexcept {
    const SharedStr parent = borrowed(spec.parent, spec.parent_len);
    const SharedStr text = borrowed(spec.text, spec.text_len);
    const void* target_ptr = spec.target_offset == kSlot497NoTarget
                                 ? nullptr
                                 : objects.bytes.data() + spec.target_offset;
    switch (spec.type) {
        case Slot497MenuType::label_26800:
            return init_slot326_label_item_26800(item, parent, spec.flags, alloc);
        case Slot497MenuType::separator_26b60:
            return init_menu_separator_26b60(item, parent, spec.flags, alloc);
        case Slot497MenuType::group_26c10:
            return init_menu_group_26c10(item, parent, text, spec.flags);
        case Slot497MenuType::toggle_26ea0: {
            DebugMenuTarget target{};
            if (target_ptr == nullptr || !menu_target_for(objects, spec.target_offset, target))
                return false;
            return init_debug_menu_item_26ea0(item, parent, text, target, spec.flags);
        }
        case Slot497MenuType::slider_27050:
            if (target_ptr == nullptr) return false;
            return init_debug_menu_item_27050(item, parent, text, target_ptr, spec.flags,
                                              spec.range_min, spec.range_max);
    }
    return false;
}

bool default_register_var(ConCmdBase& cmd, ConCmdRegistry& registry) noexcept {
    return register_concmd_20d60(cmd, registry).ok;
}

bool default_register_menu(DebugMenuItem& item, DebugMenuRegistry& menu,
                           StartupAtexitLog& atexit) noexcept {
    return register_menu_item_268b0(item, menu, atexit).ok;
}

bool default_register_record(RecordRegistryCell& cell, NamedRecord& record, const SharedStr& name,
                             std::uint32_t kind, std::uint8_t flag,
                             const AllocDispatchContract& alloc) noexcept {
    return register_named_record_a81700(cell, record, name, kind, flag, alloc).ok;
}

bool default_register_entry(Slot497Entry& entry) noexcept {
    if (entry.registered != 0) return true;   // 0x4b560: `cmpb $0,(%r12); jne ret`
    entry.registered = 1;                     // 0x4b586
    return true;                              // the 0x4b3f0 manager-list insert is NOT lifted
}

bool default_atexit(std::uint64_t dtor_rva, const void* object, ConCmdRegistry& registry) noexcept {
    return registry.atexit_log.record(dtor_rva, object);
}

bool default_release_view(const SharedStr& view) noexcept { return release_inert_view_f590(view); }

bool default_destroy_arg_functor() noexcept { return true; }

void default_store_tsc(void* dest) noexcept { store_tsc_323d0(dest); }

void* record_host_allocate(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) noexcept {
    return std::malloc(size);
}

bool record_host_deallocate(void* ptr, std::uint32_t) noexcept {
    std::free(ptr);
    return true;
}

}  // namespace

AllocDispatchContract default_slot497_record_alloc() noexcept {
    AllocDispatchContract contract;
    contract.allocate = &record_host_allocate;
    contract.deallocate = &record_host_deallocate;
    return contract;
}

Slot497Hooks default_slot497_hooks() noexcept {
    Slot497Hooks hooks{};
    hooks.register_var = &default_register_var;
    hooks.register_menu = &default_register_menu;
    hooks.register_record = &default_register_record;
    hooks.register_entry = &default_register_entry;
    hooks.atexit = &default_atexit;
    hooks.release_view = &default_release_view;
    hooks.destroy_arg_functor = &default_destroy_arg_functor;
    hooks.store_tsc = &default_store_tsc;
    hooks.menu_alloc = default_family_separator_alloc();
    return hooks;
}

ConCmdBase* slot497_var_base(Slot497Objects& objects, std::size_t index) noexcept {
    if (index >= kSlot497VarCount) return nullptr;
    return at<ConCmdBase>(objects, kSlot497Vars[index].offset);
}

void* slot497_var_object(Slot497Objects& objects, std::size_t index) noexcept {
    if (index >= kSlot497VarCount) return nullptr;
    return objects.bytes.data() + kSlot497Vars[index].offset;
}

DebugMenuItem* slot497_menu_item(Slot497Objects& objects, std::size_t index) noexcept {
    if (index >= kSlot497MenuCount) return nullptr;
    return at<DebugMenuItem>(objects, kSlot497Menus[index].offset);
}

NamedRecord* slot497_record(Slot497Objects& objects, std::size_t index) noexcept {
    if (index >= kSlot497RecordCount) return nullptr;
    return at<NamedRecord>(objects, kSlot497RecordOffset + index * kSlot497RecordSize);
}

Slot497Entry* slot497_entry(Slot497Objects& objects, std::size_t index) noexcept {
    if (index >= kSlot497EntryCount) return nullptr;
    return at<Slot497Entry>(objects, kSlot497EntryOffset + index * kSlot497EntrySize);
}

bool slot497_byte_written(std::size_t offset) noexcept {
    if (offset >= kSlot497BlockSize) return false;
    // Cell: four bytes, then four holes.
    if (offset < kSlot497CellOffset + kSlot497CellSize) return offset < 4;
    // Page header: [8,0x14) and [0x18,0x24).
    if (offset < kSlot497ViewOffset) {
        return (offset >= 0x08 && offset < 0x14) || (offset >= 0x18 && offset < 0x24);
    }
    // Borrowed page-name view: all 0x20 bytes.
    if (offset < kSlot497VarOffset) return true;
    // Variables.
    if (offset < kSlot497MenuOffset) {
        for (const Slot497VarSpec& spec : kSlot497Vars) {
            if (offset < spec.offset || offset >= spec.offset + spec.size) continue;
            const std::size_t local = offset - spec.offset;
            if (local < 0x08) return true;               // vptr
            if (local < 0x18) return false;              // pending link
            if (local < 0x19) return true;               // registered
            if (local < 0x1c) return false;              // +0x19..0x1b
            if (local < 0x60) return true;               // flags, name, description
            if (spec.type == Slot497VarType::bool_var) {
                if (local < 0x6d) return true;           // value_ref, default, zero dword
                if (local < 0x70) return false;
                if (local < 0x90) return true;
                if (local < 0xa0) return false;
                if (local < 0xa8) return true;
                if (local < 0xb0) return false;
                return local < 0xc1;
            }
            if (spec.type == Slot497VarType::string_var) {
                if (local < 0x88) return true;           // value_ref + default view
                if (local < 0x89) return true;           // byte88
                if (local < 0xa8) return false;
                if (local < 0xa9) return true;           // bytea8
                if (local < 0xb0) return false;
                if (local < 0xb1) return true;           // byteb0
                if (local < 0xd0) return false;
                if (local < 0xd1) return true;           // byted0
                if (local < 0xd8) return false;
                if (local < 0xf8) return true;           // args
                if (local < 0x108) return false;         // fn inline buffer
                if (local < 0x110) return true;          // fn payload
                if (local < 0x118) return false;         // +0x110..0x117
                if (local < 0x128) return true;          // fn tag + aux
                return local < 0x148;                    // value view
            }
            const bool unbounded = spec.type == Slot497VarType::unbounded_float ||
                                   spec.type == Slot497VarType::unbounded_int;
            if (unbounded) {
                if (local < 0x6d) return true;           // value_ref + default dword + bound byte
                if (local == 0x70 || local == 0x74 || local == 0x78) return true;
                if (local >= 0x80 && local < 0xa0) return true;
                if (local >= 0xb0 && local < 0xb8) return true;
                return local >= 0xc0 && local < 0xd4;
            }
            if (local < 0x71) return true;               // value_ref .. max engaged
            if (local < 0x74) return false;
            if (local < 0x79) return true;
            if (local < 0x80) return false;
            if (local < 0xa0) return true;               // args
            if (local < 0xb0) return false;              // fn inline buffer
            if (local < 0xb8) return true;               // fn payload
            if (local < 0xc0) return false;              // +0xb8..0xbf
            return local < 0xd4;                         // fn tag, aux, value
        }
        return false;
    }
    // Menu items.
    if (offset < kSlot497ZeroOffset) {
        for (std::size_t i = 0; i < kSlot497MenuCount; ++i) {
            const std::size_t start = kSlot497Menus[i].offset;
            if (offset < start || offset >= start + kSlot497MenuItemSize) continue;
            const std::size_t local = offset - start;
            return local < 0x21 || (local >= 0x28 && local < 0x4c) ||
                   (local >= 0x50 && local < kSlot497MenuItemPayload);
        }
        return false;
    }
    // Zero container, TSC stamp.
    if (offset < kSlot497TscOffset) return true;
    if (offset < kSlot497RecordOffset) return true;
    // Records.
    if (offset < kSlot497TailQwordOffset) {
        const std::size_t local = (offset - kSlot497RecordOffset) % kSlot497RecordSize;
        return local < 0x24 || (local >= 0x28 && local < 0x32);
    }
    // Tail qword.
    if (offset < kSlot497EntryOffset) return offset < kSlot497TailQwordOffset + 8;
    // Entries: registered byte and +0x08..+0x50.
    {
        const std::size_t local = (offset - kSlot497EntryOffset) % kSlot497EntrySize;
        return local < 1 || (local >= 0x08 && local < kSlot497EntrySize);
    }
}

const char* slot497_stage_name(Slot497Stage stage) noexcept {
    switch (stage) {
        case Slot497Stage::none: return "none";
        case Slot497Stage::hooks: return "hooks";
        case Slot497Stage::cell: return "cell";
        case Slot497Stage::page_header: return "page_header";
        case Slot497Stage::view: return "view";
        case Slot497Stage::page_atexit: return "page_atexit";
        case Slot497Stage::var_build: return "var_build";
        case Slot497Stage::var_register: return "var_register";
        case Slot497Stage::var_atexit: return "var_atexit";
        case Slot497Stage::menu_build: return "menu_build";
        case Slot497Stage::menu_register: return "menu_register";
        case Slot497Stage::menu_atexit: return "menu_atexit";
        case Slot497Stage::zero_container: return "zero_container";
        case Slot497Stage::tsc: return "tsc";
        case Slot497Stage::record_build: return "record_build";
        case Slot497Stage::record_register: return "record_register";
        case Slot497Stage::record_atexit: return "record_atexit";
        case Slot497Stage::tail_qword: return "tail_qword";
        case Slot497Stage::entry_build: return "entry_build";
        case Slot497Stage::entry_register: return "entry_register";
        case Slot497Stage::entry_atexit: return "entry_atexit";
        case Slot497Stage::canary: return "canary";
    }
    return "?";
}

Slot497Result init_slot497_262a80(Slot497Objects& objects, ConCmdRegistry& registry,
                                  DebugMenuRegistry& menu, RecordRegistryCell& records,
                                  const Slot497Hooks& hooks) noexcept {
    Slot497Result out{};
    if (hooks.register_var == nullptr || hooks.register_menu == nullptr ||
        hooks.register_record == nullptr || hooks.register_entry == nullptr ||
        hooks.atexit == nullptr || hooks.release_view == nullptr ||
        hooks.destroy_arg_functor == nullptr || hooks.store_tsc == nullptr ||
        hooks.menu_alloc.alloc.allocate == nullptr) {
        out.blocked = Slot497Stage::hooks;
        return out;
    }
    const AllocDispatchContract record_alloc = default_slot497_record_alloc();

    // 0x262a9f..0x262ab1: the manager cell is set to 1.
    out.blocked = Slot497Stage::cell;
    put_u32(objects, kSlot497CellOffset, 1);
    out.cell_set = true;

    // 0x262ac6..0x262af6: {0x780, 0x438, 0, hole, 0, -1.0f, hole}.
    out.blocked = Slot497Stage::page_header;
    put_u32(objects, 0x08, 0x780);
    put_u32(objects, 0x0c, 0x438);
    put_u32(objects, 0x10, 0);
    put_u64(objects, 0x18, 0);
    const std::uint32_t minus_one = 0xbf800000u;
    std::memcpy(objects.bytes.data() + 0x20, &minus_one, sizeof minus_one);
    out.header_built = true;

    // 0x262ab6..0x262b00: `Bluepoint Engine`, length 0.
    out.blocked = Slot497Stage::view;
    *at<SharedStr>(objects, kSlot497ViewOffset) = borrowed(kSlot497PageName, kSlot497PageNameLength);
    out.view_built = true;

    // 0x262b16: __cxa_atexit(0x253430, page, dso).
    out.blocked = Slot497Stage::page_atexit;
    out.page_atexit = hooks.atexit(kSlot497PageDtorRva, objects.bytes.data() + kSlot497PageOffset,
                                   registry);
    if (!out.page_atexit) return out;
    ++out.atexit_records;

    // 24 console variables, in body order.
    for (std::size_t i = 0; i < kSlot497VarCount; ++i) {
        const Slot497VarSpec& spec = kSlot497Vars[i];
        out.blocked = Slot497Stage::var_build;
        out.blocked_index = i;
        build_var(objects, spec);
        ConCmdBase& base = *at<ConCmdBase>(objects, spec.offset);
        ++out.vars_built;

        const SharedStr description = empty_view();
        const SharedStr name = borrowed(spec.name, spec.name_len);
        if (!hooks.release_view(description)) return out;
        if (spec.type != Slot497VarType::string_var && spec.type != Slot497VarType::local_int_low &&
            spec.type != Slot497VarType::local_int_high) {
            if (!hooks.destroy_arg_functor()) return out;
        } else if (spec.type == Slot497VarType::local_int_low ||
                   spec.type == Slot497VarType::local_int_high) {
            if (!hooks.destroy_arg_functor()) return out;
        }

        out.blocked = Slot497Stage::var_register;
        if (!hooks.register_var(base, registry)) return out;
        ++out.vars_registered;

        if (spec.type == Slot497VarType::string_var || spec.type == Slot497VarType::local_int_low ||
            spec.type == Slot497VarType::local_int_high) {
            if (!hooks.release_view(empty_view())) return out;   // 0xf730 init view
        } else {
            if (!hooks.release_view(name)) return out;
        }

        out.blocked = Slot497Stage::var_atexit;
        if (!hooks.atexit(spec.dtor_rva, objects.bytes.data() + spec.offset, registry)) return out;
        ++out.atexit_records;
    }

    // 19 debug-menu items.
    for (std::size_t i = 0; i < kSlot497MenuCount; ++i) {
        const Slot497MenuSpec& spec = kSlot497Menus[i];
        DebugMenuItem& item = *at<DebugMenuItem>(objects, spec.offset);
        out.blocked = Slot497Stage::menu_build;
        out.blocked_index = i;
        if (!build_menu_item(objects, item, spec, hooks.menu_alloc)) return out;
        ++out.menus_built;
        out.blocked = Slot497Stage::menu_register;
        if (!hooks.register_menu(item, menu, registry.atexit_log)) return out;
        ++out.menus_registered;
        const SharedStr parent = borrowed(spec.parent, spec.parent_len);
        const SharedStr text = borrowed(spec.text, spec.text_len);
        if (spec.type == Slot497MenuType::label_26800 || spec.type == Slot497MenuType::separator_26b60) {
            if (!hooks.release_view(parent)) return out;
        } else if (spec.type == Slot497MenuType::group_26c10) {
            if (!hooks.release_view(text) || !hooks.release_view(parent)) return out;
        } else {
            if (!hooks.release_view(text) || !hooks.release_view(parent)) return out;
        }
        out.blocked = Slot497Stage::menu_atexit;
        if (!hooks.atexit(kSlot497MenuItemDtorRva, &item, registry)) return out;
        ++out.atexit_records;
    }

    // 0x2648d8..0x2648f3: three ymm zero stores cover the 0x50-byte container.
    out.blocked = Slot497Stage::zero_container;
    std::memset(objects.bytes.data() + kSlot497ZeroOffset, 0, kSlot497ZeroSize);
    out.zeroed = true;

    // 0x2648fb: 0x323d0 on the 8-byte stamp.
    out.blocked = Slot497Stage::tsc;
    hooks.store_tsc(objects.bytes.data() + kSlot497TscOffset);
    out.tsc_stamped = true;

    // Seven named records.
    for (std::size_t i = 0; i < kSlot497RecordCount; ++i) {
        const Slot497RecordSpec& spec = kSlot497Records[i];
        NamedRecord& record = *at<NamedRecord>(
            objects, kSlot497RecordOffset + i * kSlot497RecordSize);
        out.blocked = Slot497Stage::record_build;
        out.blocked_index = i;
        const SharedStr name = borrowed(spec.name, spec.name_len);
        record.name = name;
        record.kind = spec.kind;
        record.q28 = 0;
        record.b30 = 0;
        record.flag = spec.flag;
        ++out.records_built;
        out.blocked = Slot497Stage::record_register;
        if (!hooks.register_record(records, record, name, spec.kind, spec.flag, record_alloc))
            return out;
        ++out.records_registered;
        out.blocked = Slot497Stage::record_atexit;
        if (!hooks.atexit(kSlot497RecordDtorRva, &record, registry)) return out;
        ++out.atexit_records;
    }

    // 0x264b6c: the qword between the records and the entries.
    out.blocked = Slot497Stage::tail_qword;
    put_u64(objects, kSlot497TailQwordOffset, 0);
    out.tail_qword_zeroed = true;

    // Three 0x50-byte registration entries.
    for (std::size_t i = 0; i < kSlot497EntryCount; ++i) {
        const Slot497EntrySpec& spec = kSlot497Entries[i];
        Slot497Entry& entry = *at<Slot497Entry>(
            objects, kSlot497EntryOffset + i * kSlot497EntrySize);
        out.blocked = Slot497Stage::entry_build;
        out.blocked_index = i;
        const SharedStr name = borrowed(spec.name, spec.name_len);
        entry.registered = 0;
        entry.name = name;
        entry.link_next = 0;
        entry.link_self = reinterpret_cast<std::uint64_t>(&entry.link_self);
        entry.key = 0;
        entry.callback = kSlot497EntryCallbackRva;
        entry.reserved = 0;
        ++out.entries_built;
        out.blocked = Slot497Stage::entry_register;
        if (!hooks.register_entry(entry)) return out;
        ++out.entries_registered;
        if (!hooks.release_view(name)) return out;
        out.blocked = Slot497Stage::entry_atexit;
        if (!hooks.atexit(kSlot497EntryDtorRva, &entry, registry)) return out;
        ++out.atexit_records;
    }

    out.blocked = Slot497Stage::canary;
    out.canary_checked = true;
    out.blocked = Slot497Stage::none;
    out.ok = true;
    return out;
}

}  // namespace sotc
