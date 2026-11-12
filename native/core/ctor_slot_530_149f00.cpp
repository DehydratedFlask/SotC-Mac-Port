// CPU-01-STARTUP-SLOT-530: see ctor_slot_530_149f00.hpp.
#include "ctor_slot_530_149f00.hpp"

#include <cstring>

namespace sotc {
namespace {

SharedStr empty_view() noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(kSlot530EmptyText), 0, 0, nullptr};
}

bool record_atexit(ConCmdRegistry& registry, std::uint64_t dtor, const void* object) noexcept {
    return registry.atexit_log.record(dtor, object);
}

}  // namespace

const char* slot530_stage_name(Slot530Stage stage) noexcept {
    switch (stage) {
        case Slot530Stage::none: return "none";
        case Slot530Stage::cell_atexit: return "cell_atexit";
        case Slot530Stage::zero_cells: return "zero_cells";
        case Slot530Stage::bool_build: return "bool_build";
        case Slot530Stage::bool_atexit: return "bool_atexit";
        case Slot530Stage::ranged_build: return "ranged_build";
        case Slot530Stage::ranged_registration: return "ranged_registration";
        case Slot530Stage::ranged_view_release: return "ranged_view_release";
        case Slot530Stage::ranged_atexit: return "ranged_atexit";
        case Slot530Stage::command_build: return "command_build";
        case Slot530Stage::command_atexit: return "command_atexit";
        case Slot530Stage::group_ctor: return "group_ctor";
        case Slot530Stage::group_registration: return "group_registration";
        case Slot530Stage::group_view_release: return "group_view_release";
        case Slot530Stage::group_atexit: return "group_atexit";
        case Slot530Stage::item_ctor: return "item_ctor";
        case Slot530Stage::item_registration: return "item_registration";
        case Slot530Stage::item_view_release: return "item_view_release";
        case Slot530Stage::item_atexit: return "item_atexit";
        case Slot530Stage::canary: return "canary";
    }
    return "";
}

DebugMenuTarget slot530_menu_target(const ConCmdBase& base, Slot530Target kind,
                                    std::uint32_t target_index, const Slot530Objects& objects) noexcept {
    switch (kind) {
        case Slot530Target::bool_var:
            if (target_index >= kSlot530BoolCount) return {};
            if (&objects.bools[target_index].base != &base) return {};
            // Bool class 0x1594218: slot 3 = 0xb890 (1), slot 6 = 0xb930 (0), so the
            // `slot3 == 1 && slot6 == 3` narrowing never applies.
            return debug_menu_target_for(objects.bools[target_index]);
        case Slot530Target::ranged_int:
            if (&objects.ranged.base != &base) return {};
            // Int class 0x1594c08: slot 3 = 0xbf70 (1), slot 6 = 0xbfc0 (2).
            return ranged_int_c580_menu_target(objects.ranged);
        case Slot530Target::command:
            if (&objects.command.base != &base) return {};
            // 0x1594dc0 slot 3 = 0x22ae0 `xor eax,eax; ret`; slot 6 is never read because slot 3 != 1.
            if (objects.command.base.vptr != kFConCmdVptr) return {};
            return l2::command_menu_target(objects.command);
    }
    return {};
}

Slot530Result init_slot530_149f00(Slot530Objects& o, ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                  const Slot530Hooks& hooks) noexcept {
    Slot530Result out{};
    l2::StageFail<Slot530Result, Slot530Stage> fail{out};

    // 0x149f18..0x149f37: guard <- *[*[0x15fe518]], stored at [rsp+0xc0].
    const std::uint64_t guard_entry = hooks.stack_guard != nullptr ? *hooks.stack_guard : 0;

    // 0x149f3f: qword [0x2ee2d00] = 0; 0x149f46: __cxa_atexit(0x1442f0, 0x2ee2d00).
    o.cell.word = 0;
    if (!record_atexit(concmd, kSlot530CellDtorRva, &o.cell)) {
        return (void)fail.at(Slot530Stage::cell_atexit), out;
    }
    ++out.atexit_records;

    // 0x149f4b..0x149fbc: dword 0 at +0x00 and 16 bytes 0 at +0x08 of both cells; +0x04..+0x07 is
    // NOT written by the original (the host's bytes stay).
    o.zero0.word = 0;
    o.zero0.zero8 = 0;
    o.zero0.zero10 = 0;
    o.zero1.word = 0;
    o.zero1.zero8 = 0;
    o.zero1.zero10 = 0;
    out.views_inert = true;

    // 0x149fe3..0x14a45a: five bool variables through the shared static-initialiser shape.
    for (std::uint32_t index = 0; index < kSlot530BoolCount; ++index) {
        const Slot530BoolSpec& spec = kSlot530Bools[index];
        out.blocked_index = index;
        const SharedStr name = l2::measured_view(spec.name, spec.name_length);
        out.bools[index] = init_bool_var_static(o.bools[index], name, empty_view(), kSlot530BoolFnVtbl,
                                                concmd, kSlot530BoolValue, kSlot530BoolDefault);
        out.views_inert = out.views_inert && out.bools[index].views_inert;
        if (!out.bools[index].ok) return (void)fail.at(Slot530Stage::bool_build), out;
        ++out.bools_built;
        ++out.atexit_records;
    }

    // 0x14a45f..0x14a573: res_maxLoadingMemoryMiB through 0xc580, min 0 / max INT32_MAX, flags 1,
    // caller functor tag 0x15985d8, empty description, value/default 0x1e.
    out.blocked_index = 0;
    o.ranged.base.vptr = kRangedIntFinalVptr;   // 0x14a508: pre-store before the constructor call
    const SharedStr ranged_name = l2::measured_view(kSlot530Ranged.name, kSlot530Ranged.name_length);
    RangedVarArgs args{};
    args.fn_vptr = kSlot530RangedArgFnVtbl;
    if (!init_ranged_int_var_c580(o.ranged, ranged_name, &o.ranged.value, kSlot530RangedMin,
                                  kSlot530RangedMax, kSlot530VarFlags, args, empty_view())) {
        return (void)fail.at(Slot530Stage::ranged_build), out;
    }
    // 0x14a51c: f590(empty description). 0x14a534: `call [0x15985d8 + 0x18]` = 0xc770, a bare ret.
    if (!release_inert_view_f590(empty_view())) {
        return (void)fail.at(Slot530Stage::ranged_view_release), out;
    }
    out.args_functor_inert = true;
    o.ranged.base.vptr = kRangedIntFinalVptr;   // 0x14a53a
    o.ranged.value = kSlot530RangedValue;       // 0x14a541
    o.ranged.default_value = kSlot530RangedDefault;   // 0x14a54b
    out.ranged_registration = register_concmd_20d60(o.ranged.base, concmd);   // 0x14a555
    if (!out.ranged_registration.ok) return (void)fail.at(Slot530Stage::ranged_registration), out;
    if (!release_inert_view_f590(ranged_name)) {   // 0x14a55a
        return (void)fail.at(Slot530Stage::ranged_view_release), out;
    }
    if (!record_atexit(concmd, kSlot530RangedDtorRva, &o.ranged)) {   // 0x14a573
        return (void)fail.at(Slot530Stage::ranged_atexit), out;
    }
    out.ranged_built = true;
    ++out.atexit_records;

    // 0x14a578..0x14a670: res_resetLoadingStats command.
    {
        FConCmdCommandSpec spec{};
        spec.name = kSlot530Command.name;
        spec.description = kSlot530Command.description;
        spec.description_length = kSlot530Command.description_length;
        spec.handler_tag = kSlot530CommandHandler;
        out.command = init_fconcmd_object(o.command, spec, concmd);
        out.views_inert = out.views_inert && out.command.views_inert;
        if (!out.command.ok) return (void)fail.at(Slot530Stage::command_build), out;
        out.command_built = true;
        ++out.atexit_records;
    }

    // 0x14a675..0x14a6f6: the "Resources" group under "Systems".
    {
        const SharedStr parent = l2::literal_view(kSlot530Parent);
        const SharedStr caption = l2::literal_view(kSlot530Label);
        if (!init_menu_group_26c10(o.group, parent, caption, kSlot530ItemFlags)) {
            return (void)fail.at(Slot530Stage::group_ctor), out;
        }
        out.group_registration = register_menu_item_268b0(o.group, menu, concmd.atexit_log);
        if (!out.group_registration.ok) return (void)fail.at(Slot530Stage::group_registration), out;
        const bool caption_inert = release_inert_view_f590(caption);
        const bool parent_inert = release_inert_view_f590(parent);
        out.views_inert = out.views_inert && caption_inert && parent_inert;
        if (!caption_inert || !parent_inert) {
            return (void)fail.at(Slot530Stage::group_view_release), out;
        }
        if (!record_menu_item_dtor_272d0(concmd.atexit_log, o.group)) {
            return (void)fail.at(Slot530Stage::group_atexit), out;
        }
        ++out.menu_items;
        ++out.atexit_records;
    }

    // 0x14a6fb..0x14aa93: seven toggles under the "Resources" label.
    for (std::uint32_t index = 0; index < kSlot530ItemCount; ++index) {
        const Slot530ItemSpec& spec = kSlot530Items[index];
        out.blocked_index = index;
        const SharedStr label = l2::literal_view(kSlot530Label);
        const SharedStr text = l2::measured_view(spec.text, spec.text_length);
        const ConCmdBase& target_base =
            spec.target == Slot530Target::bool_var     ? o.bools[spec.target_index].base
            : spec.target == Slot530Target::ranged_int ? o.ranged.base
                                                       : o.command.base;
        const DebugMenuTarget target =
            slot530_menu_target(target_base, spec.target, spec.target_index, o);
        if (!target.known) return (void)fail.at(Slot530Stage::item_ctor), out;
        if (!init_debug_menu_item_26ea0(o.items[index], label, text, target, kSlot530ItemFlags)) {
            return (void)fail.at(Slot530Stage::item_ctor), out;
        }
        out.item_registrations[index] = register_menu_item_268b0(o.items[index], menu, concmd.atexit_log);
        if (!out.item_registrations[index].ok) {
            return (void)fail.at(Slot530Stage::item_registration), out;
        }
        const bool text_inert = release_inert_view_f590(text);
        const bool label_inert = release_inert_view_f590(label);
        out.views_inert = out.views_inert && text_inert && label_inert;
        if (!text_inert || !label_inert) return (void)fail.at(Slot530Stage::item_view_release), out;
        if (!record_menu_item_dtor_272d0(concmd.atexit_log, o.items[index])) {
            return (void)fail.at(Slot530Stage::item_atexit), out;
        }
        ++out.menu_items;
        ++out.atexit_records;
    }

    // 0x14aa98..0x14aaba: canary compare and epilogue.
    if (hooks.stack_guard != nullptr) {
        out.canary_checked = true;
        out.canary_matched = *hooks.stack_guard == guard_entry;
        if (!out.canary_matched) return (void)fail.at(Slot530Stage::canary), out;
    }

    out.blocked_index = 0;
    out.ok = out.atexit_records == kSlot530AtexitCount && out.bools_built == kSlot530BoolCount &&
             out.ranged_built && out.command_built && out.menu_items == kSlot530MenuCount &&
             out.views_inert;
    return out;
}

}  // namespace sotc
