// CPU-01-STARTUP-SLOT-316-CHARACTERIZATION-01 / -NATIVE-LIFT-01: fixture for startup constructor
// index 316 (0xa33280, the Localization block: four language console variables, one bool variable,
// the `loc_reload` command, six "Localization" toggles, the "!!!Unlocalized!!!" string and 14
// __cxa_atexit records).
//
// Covers: the pinned constants (table word, object strides, atexit rows, variable/toggle rows, the
// three target classes and their gate answers), the language-variable model field by field with its
// never-written holes, the bool variable, the command, the six registrations, the complete atexit
// order, and the fail-visible stages: a missing language hook, a spec that is not the pinned row, a
// failing hook, a missing allocator for the unaligned string, a failed menu registration, the
// parent-contract path and a changed __stack_chk_guard.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_316_a33280.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;

template <class T>
void poison(T& obj) { std::memset(static_cast<void*>(&obj), kPoison, sizeof obj); }

template <class T>
std::uint32_t dword_at(const T& obj, std::size_t off) {
    std::uint32_t v;
    std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(&obj) + off, 4);
    return v;
}

template <class T>
bool region_poison(const T& obj, std::size_t off, std::size_t len) {
    const auto* b = reinterpret_cast<const std::uint8_t*>(&obj);
    for (std::size_t i = 0; i < len; ++i) if (b[off + i] != kPoison) return false;
    return true;
}

bool same_view(const SharedStr& v, const char* text, std::size_t len) {
    return v.data != nullptr && v.size == static_cast<std::int64_t>(len) && v.aux == 0 && v.ctl == nullptr &&
           std::memcmp(v.data, text, len) == 0;
}

// Recording wrapper around the native language-variable model.
struct HookLog {
    std::uint32_t language_calls = 0;
    std::uint32_t fail_at = 0xffffffffu;   // fail on this call index
    bool refuse = false;
    std::uint64_t* guard = nullptr;
};
HookLog g_log{};

Slot316LanguageVarResult recording_language_ctor(ConCmdRangedVar& var, const Slot316LanguageVarSpec& spec,
                                                 ConCmdRegistry& registry) noexcept {
    const std::uint32_t i = g_log.language_calls++;
    if (g_log.guard != nullptr && i == 0) *g_log.guard ^= 1;   // guard corruption inside the body (once)
    if (i == g_log.fail_at) return Slot316LanguageVarResult{};
    if (g_log.refuse) return init_slot316_language_var(var, Slot316LanguageVarSpec{}, registry);
    return init_slot316_language_var(var, spec, registry);
}

// The body's own 14 atexit records, skipping the one 0x268b0 records for the menu root.
std::size_t slot_records(const StartupAtexitLog& log, StartupAtexitEntry* out, std::size_t cap) {
    std::size_t n = 0;
    for (std::size_t i = 0; i < log.size(); ++i) {
        const StartupAtexitEntry& e = log.at(i);
        if (e.dtor_rva == kDebugMenuRootDtorRva) continue;   // recorded by 0x268b0, not by this body
        if (n < cap) out[n] = e;
        ++n;
    }
    return n;
}

void section_constants() {
    CHECK(kSlot316Rva == 0xa33280 && kSlot316BodySize == 2561 && kSlot316EndRva == 0xa33c81);
    CHECK(kSlot316ReachableBytes == 2560 && kSlot316InstructionCount == 473);
    CHECK(kSlot316DirectCallSites == 54 && kSlot316IndirectCallSites == 5);
    CHECK(kSlot316RipOperands == 88 && kSlot316RipTargets == 62);
    CHECK(kSlot316StoreSites == 11 && kSlot316StoreTargets == 9);
    CHECK(kSlot316BranchCount == 2);
    // 0x2dd70e0 - 316*8 == 0x2dd6700, the one word that references the body.
    CHECK(kSlot316TableTopRva - 8 * kSlot316TableIndex == kSlot316TableWordRva);
    CHECK(kSlot316TableWordRva == 0x2dd6700 && kSlot316TableIndex == 316);
    CHECK(kSlot316NextUnrecoveredIndex == 326 && kSlot316NextUnrecoveredRva == 0x837f30);
    CHECK(kSlot316CanaryCellRva == 0x15fe518);
    CHECK(kSlot316StackChkFailRva == 0xf8f3a0 && kSlot316CanaryFrameOffset == 0xc0);
    CHECK(kSlot316BranchRva == 0xa33c6a && kSlot316BranchTargetRva == 0xa33c7b);
    CHECK(kSlot316InnerBranchRva == 0xa338a7 && kSlot316InnerBranchTargetRva == 0xa338b1);
    CHECK(kSlot316FrameBytes == 0xe0);
    CHECK(kSlot316AtexitRva == 0xf8f390 && kSlot316DsoRva == 0x1604218 && kSlot316ReleaseRva == 0xf590);
    CHECK(kSlot316ConCmdBaseCtorRva == 0x20af0 && kSlot316ConCmdRegisterRva == 0x20d60);
    CHECK(kSlot316MenuToggleCtorRva == 0x26ea0 && kSlot316MenuRegisterRva == 0x268b0);
    CHECK(kSlot316NameViewRva == 0xf2f0 && kSlot316SharedStrCtorRva == 0xf1a0);
    CHECK(kSlot316LanguageVarWrapperRva == 0xa324c0 && kSlot316LanguageVarWrapperSize == 436);
    CHECK(kSlot316LanguageVarWrapperInstructions == 98);
    CHECK(kSlot316LanguageVarLeafRva == 0xa32680 && kSlot316LanguageVarLeafSize == 352);
    CHECK(kSlot316LanguageVarLeafInstructions == 89);
    CHECK(kSlot316FunctorDispatches == 48 && kSlot316LanguageDispatches == 11);

    // Identity tags.
    CHECK(kSlot316LanguageVarMidVptr == 0x15d1120 && kSlot316LanguageVarFinalVptr == 0x15d11b8);
    CHECK(kSlot316LanguageFnVtbl == 0x15d1188 && kSlot316LanguageThunkRva == 0xa324a0);
    CHECK(kSlot316LanguageFnCopyRva == 0xa32440 && kSlot316LanguageFnCloneRva == 0xa32460);
    CHECK(kSlot316LanguageFnMoveRva == 0xa32480 && kSlot316LanguageFnDestroyRva == 0xa32490);
    CHECK(kSlot316LanguageGate3Rva == 0xa31d90 && kSlot316LanguageGate6Rva == 0xa31f10);
    CHECK(kSlot316LanguageGate3 == 1 && kSlot316LanguageGate6 == 1);
    CHECK(kSlot316BoolFinalVptr == 0x1594218 && kSlot316BoolFnTag == 0x15d1220);
    CHECK(kSlot316BoolGate3 == 1 && kSlot316BoolGate6 == 0);
    CHECK(kSlot316CommandVptr == 0x1594dd0 && kSlot316CommandGate3 == 0);
    CHECK(kSlot316CommandTableRva == 0x1594dc0 && kSlot316CommandRegisterSlot == 0x20);
    CHECK(kSlot316CommandRegisterSiteRva == 0xa338cb);

    // The four language variables, in body order.
    CHECK(kSlot316LanguageVars.size() == 4);
    for (std::size_t i = 0; i < kSlot316LanguageVars.size(); ++i) {
        CHECK(kSlot316LanguageVars[i].index == i);
        CHECK(std::strlen(kSlot316LanguageVars[i].name) == kSlot316LanguageVars[i].name_len);
        CHECK(std::strlen(kSlot316LanguageVars[i].description) == kSlot316LanguageVars[i].description_len);
        CHECK(kSlot316LanguageVars[i].object_rva + 0xd8 ==
              (i + 1 < 4 ? kSlot316LanguageVars[i + 1].object_rva : kSlot316BoolVar.object_rva));
    }
    const std::uint64_t want_callback[4] = {0xa2c790, 0xa2c890, 0xa2c950, 0xa2ca10};
    for (int i = 0; i < 4; ++i) CHECK(kSlot316LanguageVars[i].callback_rva == want_callback[i]);
    CHECK(kSlot316LanguageMin == 1 && kSlot316LanguageMax == 26);
    CHECK(kSlot316LanguageUnset == 0xffffffffu);

    // The bool variable and the command.
    CHECK(std::strlen(kSlot316BoolVar.name) == kSlot316BoolVar.name_len && kSlot316BoolVar.name_len == 0x1d);
    CHECK(kSlot316BoolVar.object_rva == 0x32f5348 && kSlot316BoolVar.fn_tag == 0x15d1220);
    CHECK(kSlot316BoolVar.value == 0 && kSlot316BoolVar.default_value == 0);
    CHECK(std::strlen(kSlot316Command.name) == 0xa);
    CHECK(std::strlen(kSlot316Command.description) == kSlot316Command.description_len);
    CHECK(kSlot316Command.object_rva == 0x32f5410 && kSlot316Command.handler_tag == 0xa2cad0);

    // The six toggles, all under the shared "Localization" parent.
    CHECK(kSlot316Items.size() == 6);
    for (std::size_t i = 0; i < kSlot316Items.size(); ++i) {
        CHECK(kSlot316Items[i].spec.index == i);
        CHECK(std::strlen(kSlot316Items[i].spec.caption) == kSlot316Items[i].spec.caption_len);
    }
    CHECK(std::strlen(kSlot316MenuParent) == 0xc && kSlot316MenuParentRva == 0x12c191f);
    CHECK(kSlot316Items[0].spec.object_rva == 0x32f54b0 && kSlot316Items[5].spec.object_rva == 0x32f57f8);
    CHECK(kSlot316Items[4].kind == Slot316Target::bool_var && kSlot316Items[5].kind == Slot316Target::command);
    CHECK(std::strlen(kSlot316UnlocalizedText) == 17);

    // The 14 atexit rows, in body order.
    CHECK(kSlot316AtexitRows.size() == 14);
    for (std::size_t i = 0; i < kSlot316AtexitRows.size(); ++i) CHECK(kSlot316AtexitRows[i].step == i + 1);
    CHECK(kSlot316AtexitRows[0].dtor_rva == kSlot316CellDtorRva && kSlot316AtexitRows[0].object_rva == 0x32f4fe0);
    CHECK(kSlot316AtexitRows[1].dtor_rva == kSlot316LanguageVarDtorRva);
    CHECK(kSlot316AtexitRows[5].dtor_rva == kSlot316BoolVarDtorRva && kSlot316AtexitRows[5].object_rva == 0x32f5348);
    CHECK(kSlot316AtexitRows[6].dtor_rva == kSlot316CommandDtorRva && kSlot316AtexitRows[6].object_rva == 0x32f5410);
    CHECK(kSlot316AtexitRows[7].dtor_rva == kSlot316MenuItemDtorRva && kSlot316AtexitRows[7].object_rva == 0x32f54b0);
    CHECK(kSlot316AtexitRows[13].dtor_rva == kSlot316UnlocalizedDtorRva &&
          kSlot316AtexitRows[13].object_rva == 0x32f58f8);
    for (std::size_t i = 0; i < 4; ++i) {
        CHECK(kSlot316LanguageVars[i].atexit_site_rva == kSlot316AtexitRows[1 + i].site_rva);
    }

    // Object strides and the aggregate.
    CHECK(sizeof(Slot316Objects) == 0x938 && sizeof(Slot316Cell) == 8);
    CHECK(offsetof(Slot316Objects, cell) == 0x000 && offsetof(Slot316Objects, var_text) == 0x008);
    CHECK(offsetof(Slot316Objects, var_subtitles) == 0x0e0 && offsetof(Slot316Objects, var_speech) == 0x1b8);
    CHECK(offsetof(Slot316Objects, var_system) == 0x290 && offsetof(Slot316Objects, var_english) == 0x368);
    CHECK(offsetof(Slot316Objects, command) == 0x430 && offsetof(Slot316Objects, item_text) == 0x4d0);
    CHECK(offsetof(Slot316Objects, item_reload) == 0x818 && offsetof(Slot316Objects, foreign_58a0) == 0x8c0);
    CHECK(offsetof(Slot316Objects, unlocalized) == 0x918);
    CHECK(kSlot316ForeignGapRva == 0x32f58a0 && kSlot316ForeignGapSize == 0x58);
    CHECK(sizeof(ConCmdRangedVar) == 0xd8 && sizeof(ConCmdBoolVar) == 0xc8 && sizeof(ConCmdCommand) == 0xa0);

    // Stage names.
    CHECK(std::strcmp(slot316_stage_name(Slot316Stage::none), "none") == 0);
    CHECK(std::strcmp(slot316_stage_name(Slot316Stage::cell_atexit), "cell_atexit") == 0);
    CHECK(std::strcmp(slot316_stage_name(Slot316Stage::language_var_view_release),
                      "language_var_view_release") == 0);
    CHECK(std::strcmp(slot316_stage_name(Slot316Stage::unlocalized_atexit), "unlocalized_atexit") == 0);
    CHECK(std::strcmp(slot316_stage_name(Slot316Stage::canary), "canary") == 0);
}

void section_functor_and_target() {
    // The four row-local `callq *0x18(%rax)`: inert only when the pinned body is a bare ret.
    CHECK(kSlot316FunctorSites.size() == 4);
    for (const auto& site : kSlot316FunctorSites) {
        CHECK(site.tag_rva == kSlot316LanguageFnVtbl && site.slot3_rva == kSlot316LanguageFnDestroyRva);
        CHECK(slot316_inert_functor_destroy(site));
    }
    Slot316FunctorSite unlifted = kSlot316FunctorSites[0];
    unlifted.slot3_is_bare_ret = false;
    CHECK(!slot316_inert_functor_destroy(unlifted));

    // The 0x26ea0 gate: {1, 1} for the language class, {1, 0} for the bool class, {0, 0} for the
    // command class (whose slot 6 the original never reads), unknown for anything else.
    ConCmdRangedVar lang{};
    lang.base.vptr = kSlot316LanguageVarFinalVptr;
    DebugMenuTarget t = slot316_menu_target(lang.base);
    CHECK(t.known && t.object == &lang.base && t.slot3 == 1 && t.slot6 == 1);
    ConCmdBoolVar b{};
    b.base.vptr = kSlot316BoolFinalVptr;
    t = slot316_menu_target(b.base);
    CHECK(t.known && t.object == &b.base && t.slot3 == 1 && t.slot6 == 0);
    ConCmdCommand c{};
    c.base.vptr = kSlot316CommandVptr;
    t = slot316_menu_target(c.base);
    CHECK(t.known && t.slot3 == 0 && t.slot6 == 0);
    lang.base.vptr = kSlot316LanguageVarMidVptr;      // the transient tag must not answer
    CHECK(!slot316_menu_target(lang.base).known);
}

void section_language_var() {
    ConCmdRegistry creg;
    ConCmdRangedVar var;
    poison(var);
    const Slot316LanguageVarResult built = init_slot316_language_var(var, kSlot316LanguageVars[0], creg);
    CHECK(built.ok && built.registration.ok && built.functor_dispatches == kSlot316LanguageDispatches);
    const Slot316LanguageVarSpec& spec = kSlot316LanguageVars[0];
    CHECK(var.base.vptr == kSlot316LanguageVarFinalVptr && var.base.flags == kSlot316VarFlags);
    CHECK(var.base.registered == 1);
    CHECK(same_view(var.base.name, spec.name, spec.name_len));
    CHECK(same_view(var.base.description, spec.description, spec.description_len));
    CHECK(var.value_ref == &var.value);
    CHECK(dword_at(var, 0x6c) == kSlot316LanguageMin && var.min_engaged == 1);
    CHECK(dword_at(var, 0x74) == kSlot316LanguageMax && var.max_engaged == 1);
    CHECK(var.value == kSlot316LanguageUnset && var.default_value == kSlot316LanguageUnset);
    CHECK(var.args[0] == reinterpret_cast<std::uint64_t>(&var.args[0]));   // q[3] != 0 -> self pointer
    CHECK(var.args[1] == kSlot316LanguageThunkRva && var.args[2] == 0 && var.args[3] == spec.callback_rva);
    CHECK(var.fn_vptr == kSlot316LanguageFnVtbl && var.fn_payload == 0 && var.fn_aux == 0);
    // Never written by 0xa32680 / 0xa324c0.
    CHECK(region_poison(var, 0x71, 3) && region_poison(var, 0x79, 7) && region_poison(var, 0xa0, 0x10));
    CHECK(region_poison(var, 0xb8, 8) && region_poison(var, 0xd4, 4));
    CHECK(creg.pending.size() == 1);

    // A spec that is not the pinned row for its index is refused and writes nothing.
    ConCmdRangedVar other;
    poison(other);
    Slot316LanguageVarSpec broken = kSlot316LanguageVars[1];
    broken.callback_rva = kSlot316LanguageVars[0].callback_rva;   // another row's callback
    CHECK(!init_slot316_language_var(other, broken, creg).ok);
    broken = kSlot316LanguageVars[1];
    broken.index = 9;                                            // out of range
    CHECK(!init_slot316_language_var(other, broken, creg).ok);
    broken = Slot316LanguageVarSpec{};                            // default-constructed
    CHECK(!init_slot316_language_var(other, broken, creg).ok);
    CHECK(region_poison(other, 0x00, sizeof other));
    CHECK(creg.pending.size() == 1);                              // nothing was queued either
}

void section_native_build() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot316Objects o;
    poison(o);
    const Slot316Hooks hooks = default_slot316_hooks();
    CHECK(hooks.language_var_ctor == &init_slot316_language_var);
    CHECK(hooks.string_alloc.alloc.allocate != nullptr && hooks.string_alloc.alloc.deallocate != nullptr);
    CHECK(hooks.stack_guard == nullptr);
    const Slot316Result r = init_slot316_a33280(o, creg, menu, hooks);

    CHECK(r.ok && r.blocked == Slot316Stage::none && r.blocked_index == 0);
    CHECK(r.atexit_records == 14 && r.functors_destroyed == 4);
    CHECK(r.functor_dispatches == kSlot316FunctorDispatches);      // 4 * 11 + 4 * 1
    CHECK(r.vars_built == 6 && r.menu_items == 6 && r.views_inert);
    CHECK(r.unlocalized_built && !r.canary_checked && !r.canary_matched);
    CHECK(r.bool_var.ok && r.command.ok && r.command.vtable_register_slot_ok);
    CHECK(o.cell.word == 0);

    // The four language variables (class 0x15d11b8, tag 0x15d1188, bounds [1, 26], value -1).
    const ConCmdRangedVar* const langs[4] = {&o.var_text, &o.var_subtitles, &o.var_speech, &o.var_system};
    for (int i = 0; i < 4; ++i) {
        const ConCmdRangedVar& v = *langs[i];
        const Slot316LanguageVarSpec& spec = kSlot316LanguageVars[i];
        CHECK(v.base.vptr == kSlot316LanguageVarFinalVptr && v.base.flags == 1 && v.base.registered == 1);
        CHECK(same_view(v.base.name, spec.name, spec.name_len));
        CHECK(same_view(v.base.description, spec.description, spec.description_len));
        CHECK(v.value_ref == &v.value);
        CHECK(dword_at(v, 0x6c) == 1 && v.min_engaged == 1);
        CHECK(dword_at(v, 0x74) == 26 && v.max_engaged == 1);
        CHECK(v.value == 0xffffffffu && v.default_value == 0xffffffffu);
        CHECK(v.args[0] == reinterpret_cast<std::uint64_t>(&v.args[0]));
        CHECK(v.args[1] == kSlot316LanguageThunkRva && v.args[2] == 0 && v.args[3] == spec.callback_rva);
        CHECK(v.fn_vptr == kSlot316LanguageFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
        CHECK(r.language_vars[i].ok && r.language_vars[i].registration.ok);
        CHECK(r.language_vars[i].description_inert && r.language_vars[i].name_inert);
        CHECK(r.language_vars[i].local_functor_inert);
        CHECK(r.language_vars[i].functor_dispatches == kSlot316LanguageDispatches);
        CHECK(region_poison(v, 0xa0, 0x10) && region_poison(v, 0xb8, 8) && region_poison(v, 0xd4, 4));
    }

    // The bool variable: class 0x1594218, functor tag 0x15d1220, value and default 0.
    CHECK(o.var_english.base.vptr == kSlot316BoolFinalVptr && o.var_english.base.flags == 1);
    CHECK(o.var_english.base.registered == 1);
    CHECK(same_view(o.var_english.base.name, kSlot316BoolVar.name, kSlot316BoolVar.name_len));
    CHECK(o.var_english.base.description.size == 0 && o.var_english.base.description.ctl == nullptr);
    CHECK(o.var_english.value_ref == &o.var_english.value);
    CHECK(o.var_english.value == 0 && o.var_english.default_value == 0);
    CHECK(o.var_english.fn_vptr == kSlot316BoolFnTag && o.var_english.fn_payload == 0 &&
          o.var_english.fn_aux == 0);
    CHECK(region_poison(o.var_english, 0x6d, 3) && region_poison(o.var_english, 0x90, 0x10));
    CHECK(region_poison(o.var_english, 0xa8, 8) && region_poison(o.var_english, 0xc1, 7));

    // The command: class 0x1594dd0, flags 2, the delegate bound to its own +0x60.
    CHECK(o.command.base.vptr == kSlot316CommandVptr && o.command.base.flags == 2);
    CHECK(o.command.base.registered == 1);
    CHECK(same_view(o.command.base.name, kSlot316Command.name, 10));
    CHECK(same_view(o.command.base.description, kSlot316Command.description,
                    kSlot316Command.description_len));
    CHECK(o.command.delegate.object == reinterpret_cast<std::uint64_t>(&o.command.delegate));
    CHECK(o.command.delegate.fn == 0x1990 && o.command.delegate.adj == 0);
    CHECK(o.command.delegate.handler == 0xa2cad0);
    CHECK(o.command.args_begin == nullptr && o.command.args_end == nullptr && o.command.args_cap == nullptr);
    CHECK(region_poison(o.command, 0x98, 8));

    // The six toggles: kind 2, flags 0x30, the shared "Localization" caption, each pointing at its object.
    const DebugMenuItem* const items[6] = {&o.item_text,     &o.item_subtitles, &o.item_speech,
                                          &o.item_system,   &o.item_english,   &o.item_reload};
    for (int i = 0; i < 6; ++i) {
        const DebugMenuItem& it = *items[i];
        CHECK(it.kind == kDebugMenuItemKind && it.flags == kSlot316ItemFlags);
        CHECK(same_view(it.label, kSlot316MenuParent, 0xc));
        CHECK(same_view(it.text, kSlot316Items[i].spec.caption, kSlot316Items[i].spec.caption_len));
        CHECK(it.target == &slot316_target_base(o, kSlot316Items[i].spec.target));
        CHECK(it.registered == 0 && r.item_registrations[i].path == DebugMenuRegisterPath::root_list);
        CHECK(r.item_registrations[i].ok);
        // All three classes answer slot3 != 1 or slot6 != 3, so no toggle is narrowed.
        CHECK(dword_at(it, 0x98) == 0x3f800000u && dword_at(it, 0x9c) == 0x41200000u);
    }

    // Registration side effects: six pending ConCmds in body order, six root-list toggles.
    CHECK(creg.pending.size() == 6);
    CHECK(creg.pending.first() == &o.var_text.base.pending);
    CHECK(menu.root_guard_done && menu.root.count == 6);

    // The 14 atexit records in body order, with the menu root's record between the command and item 0.
    CHECK(creg.atexit_log.size() == 15);
    CHECK(creg.atexit_log.at(7).dtor_rva == kDebugMenuRootDtorRva);
    CHECK(creg.atexit_log.at(7).object == &menu.root);
    StartupAtexitEntry recs[14]{};
    CHECK(slot_records(creg.atexit_log, recs, 14) == 14);
    const void* const want_object[14] = {
        &o.cell,        &o.var_text,    &o.var_subtitles, &o.var_speech,   &o.var_system,
        &o.var_english, &o.command,     &o.item_text,     &o.item_subtitles, &o.item_speech,
        &o.item_system, &o.item_english, &o.item_reload,  &o.unlocalized};
    for (std::size_t i = 0; i < 14; ++i) {
        CHECK(recs[i].dtor_rva == kSlot316AtexitRows[i].dtor_rva);
        CHECK(recs[i].object == want_object[i]);
    }

    // The process-wide "!!!Unlocalized!!!" holder built by 0xf1a0.
    CHECK(o.unlocalized.data != nullptr && o.unlocalized.size == 17 && o.unlocalized.ctl != nullptr);
    CHECK(std::memcmp(o.unlocalized.data, kSlot316UnlocalizedText, 17) == 0);
    CHECK(o.unlocalized.data[17] == 0);
}

void section_no_language_hook() {
    // Without the 0xa324c0 contract only the pointer cell is built and recorded.
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot316Objects o;
    poison(o);
    const Slot316Result r = init_slot316_a33280(o, creg, menu);
    CHECK(!r.ok && r.blocked == Slot316Stage::language_var_build && r.blocked_index == 0);
    CHECK(r.atexit_records == 1 && r.vars_built == 0 && r.menu_items == 0);
    CHECK(creg.atexit_log.size() == 1 && o.cell.word == 0);
    CHECK(region_poison(o.var_text, 0x00, 0xd4));
}

void section_hook_fails() {
    // A hook that fails on its third call stops the fragment at that row, after two complete objects.
    g_log = HookLog{};
    g_log.fail_at = 2;
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot316Objects o;
    poison(o);
    Slot316Hooks hooks = default_slot316_hooks();
    hooks.language_var_ctor = &recording_language_ctor;
    const Slot316Result r = init_slot316_a33280(o, creg, menu, hooks);
    CHECK(!r.ok && r.blocked == Slot316Stage::language_var_build && r.blocked_index == 2);
    CHECK(g_log.language_calls == 3);
    CHECK(r.atexit_records == 3 && r.vars_built == 2 && r.menu_items == 0);
    CHECK(region_poison(o.var_speech, 0x00, 0xd4));
}

void section_foreign_spec_refused() {
    // A refusing hook stops the slot at the first row and records nothing after the cell.
    g_log = HookLog{};
    g_log.refuse = true;
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot316Objects o;
    poison(o);
    Slot316Hooks hooks = default_slot316_hooks();
    hooks.language_var_ctor = &recording_language_ctor;
    const Slot316Result r = init_slot316_a33280(o, creg, menu, hooks);
    CHECK(!r.ok && r.blocked == Slot316Stage::language_var_build && r.blocked_index == 0);
    CHECK(g_log.language_calls == 1 && r.atexit_records == 1);
    CHECK(region_poison(o.var_text, 0x00, 0xd4));
    CHECK(creg.pending.size() == 0);
}

void section_no_allocator() {
    // The last object is a non-empty 0xf1a0 string; without the allocator contract the fragment fails
    // there, after every console object and toggle was built.
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot316Objects o;
    poison(o);
    Slot316Hooks hooks = default_slot316_hooks();
    hooks.string_alloc = {};
    const Slot316Result r = init_slot316_a33280(o, creg, menu, hooks);
    CHECK(!r.ok && r.blocked == Slot316Stage::unlocalized_build && r.blocked_index == 0);
    CHECK(!r.unlocalized_built && r.atexit_records == 13);
    CHECK(r.vars_built == 6 && r.menu_items == 6);
    CHECK(region_poison(o.unlocalized, 0x00, sizeof(SharedStr)));
    CHECK(creg.atexit_log.size() == 14);   // 13 + the menu root's record
}

void section_menu_registration_blocked() {
    // A non-null parent cell with no insert contract fails the first toggle visibly, after the cell,
    // the four language variables, the bool variable and the command were built.
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    int token = 0;
    menu.parent.parent = &token;
    Slot316Objects o;
    const Slot316Result r = init_slot316_a33280(o, creg, menu, default_slot316_hooks());
    CHECK(!r.ok && r.blocked == Slot316Stage::item_registration && r.blocked_index == 0);
    CHECK(r.vars_built == 6 && r.atexit_records == 7 && r.menu_items == 0);
    CHECK(!r.item_registrations[0].ok && r.item_registrations[0].path == DebugMenuRegisterPath::parent);
    CHECK(menu.root.count == 0 && !menu.root_guard_done);
}

void section_parent_contract() {
    // With a menu parent contract every toggle goes to the parent and the root is never initialised,
    // so the log holds exactly the body's 14 records.
    static int inserted = 0;
    struct P { static bool insert(void*, DebugMenuItem&) noexcept { ++inserted; return true; } };
    inserted = 0;
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    int token = 0;
    menu.parent.parent = &token;
    menu.parent.insert = &P::insert;
    Slot316Objects o;
    const Slot316Result r = init_slot316_a33280(o, creg, menu, default_slot316_hooks());
    CHECK(r.ok && inserted == 6 && menu.root.count == 0 && !menu.root_guard_done);
    CHECK(o.item_text.registered == 1 && o.item_reload.registered == 1);   // the parent path sets +0x20
    CHECK(creg.atexit_log.size() == 14 && r.atexit_records == 14);
    for (std::size_t i = 0; i < creg.atexit_log.size(); ++i) {
        CHECK(creg.atexit_log.at(i).dtor_rva == kSlot316AtexitRows[i].dtor_rva);
    }
}

void section_canary() {
    // The body's guard compare is at 0xa33c6a. A guard that changed inside the body takes the
    // original's `jne 0xa33c7b` / __stack_chk_fail path, so the model fails at `canary` after every
    // object was built.
    g_log = HookLog{};
    std::uint64_t guard = 0x0123456789abcdefull;
    g_log.guard = &guard;
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot316Objects o;
    Slot316Hooks hooks = default_slot316_hooks();
    hooks.language_var_ctor = &recording_language_ctor;
    hooks.stack_guard = &guard;
    const Slot316Result r = init_slot316_a33280(o, creg, menu, hooks);
    CHECK(!r.ok && r.blocked == Slot316Stage::canary);
    CHECK(r.canary_checked && !r.canary_matched);
    CHECK(r.atexit_records == 14 && r.menu_items == 6 && r.vars_built == 6);
    CHECK(creg.atexit_log.size() == 15);
}

void section_recording_hooks() {
    // The recorded language hook runs exactly four times.
    g_log = HookLog{};
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot316Objects o;
    Slot316Hooks hooks = default_slot316_hooks();
    hooks.language_var_ctor = &recording_language_ctor;
    const Slot316Result r = init_slot316_a33280(o, creg, menu, hooks);
    CHECK(r.ok && g_log.language_calls == 4);
    CHECK(o.var_system.args[3] == 0xa2ca10);
}

}  // namespace

int main() {
    section_constants();
    section_functor_and_target();
    section_language_var();
    section_native_build();
    section_no_language_hook();
    section_hook_fails();
    section_foreign_spec_refused();
    section_no_allocator();
    section_menu_registration_blocked();
    section_parent_contract();
    section_canary();
    section_recording_hooks();
    std::puts("OK test_ctor_slot_316");
    return 0;
}
