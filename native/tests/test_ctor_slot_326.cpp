// CPU-01-STARTUP-SLOT-326-CHARACTERIZATION-01: fixture for startup constructor index 326
// (0x837f30, the Colossus / Shadow eye-colour debug page: two console bools, thirteen console float
// variables, five menu groups, three toggles, twelve sliders, one kind-0 "MenuSeparator" row and 36
// __cxa_atexit records).
//
// Covers: the pinned constants (table word, object strides, atexit rows, variable/item rows, the two
// target classes and their gate answers), both float object shapes field by field with their
// never-written holes, the 0x26800 label ctor, the complete atexit order, and the fail-visible
// stages: a missing allocator for the "MenuSeparator" text, an unpinned argument tag, a failed menu
// registration, the parent-contract path and a changed __stack_chk_guard.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_326_837f30.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;

// The guard cell the canary section's parent hook corrupts.
std::uint64_t* guard_cell = nullptr;

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

// The body's own 36 atexit records, skipping the one 0x268b0 records for the menu root.
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
    CHECK(kSlot326Rva == 0x837f30 && kSlot326BodySize == 7055 && kSlot326EndRva == 0x839abf);
    CHECK(kSlot326ReachableBytes == 7054 && kSlot326InstructionCount == 1306);
    CHECK(kSlot326DirectCallSites == 159 && kSlot326DirectCallees == 10);
    CHECK(kSlot326IndirectCallSites == 13);
    CHECK(kSlot326RipOperands == 263 && kSlot326RipTargets == 113);
    CHECK(kSlot326StoreSites == 50 && kSlot326StoreTargets == 45);
    CHECK(kSlot326BranchCount == 1 && kSlot326BasicBlocks == 2);
    // 0x2dd70e0 - 326*8 == 0x2dd66b0, the one word that references the body. (0x2dd65d0, quoted in the
    // task brief, is entry 354 = 0x79e180.)
    CHECK(kSlot326TableTopRva - 8 * kSlot326TableIndex == kSlot326TableWordRva);
    CHECK(kSlot326TableWordRva == 0x2dd66b0 && kSlot326TableIndex == 326);
    CHECK(kSlot326TableTopRva - 0x2dd65d0 == 354 * 8);
    CHECK(kSlot326NextUnrecoveredIndex == 329 && kSlot326NextUnrecoveredRva == 0x829e90);
    CHECK(kSlot326CanaryCellRva == 0x15fe518);
    CHECK(kSlot326StackChkFailRva == 0xf8f3a0 && kSlot326CanaryFrameOffset == 0xc0);
    CHECK(kSlot326BranchRva == 0x839aa8 && kSlot326BranchTargetRva == 0x839ab9);
    CHECK(kSlot326EpilogueRva == 0x839aaa && kSlot326FrameBytes == 0xe0);
    CHECK(kSlot326AtexitRva == 0xf8f390 && kSlot326DsoRva == 0x1604218 && kSlot326ReleaseRva == 0xf590);
    CHECK(kSlot326ConCmdBaseCtorRva == 0x20af0 && kSlot326ConCmdRegisterRva == 0x20d60);
    CHECK(kSlot326FloatVarCtorRva == 0xc300 && kSlot326FloatVarCtorSize == 0x160);
    CHECK(kSlot326MenuGroupCtorRva == 0x26c10 && kSlot326MenuToggleCtorRva == 0x26ea0);
    CHECK(kSlot326MenuSliderCtorRva == 0x27050 && kSlot326MenuRegisterRva == 0x268b0);
    CHECK(kSlot326LabelItemCtorRva == 0x26800 && kSlot326LabelItemCtorSize == 173);
    CHECK(kSlot326LabelItemCtorEndRva == 0x268ad);
    CHECK(kSlot326SeparatorTextRva == 0xf96530 && kSlot326EmptyTextRva == 0x128e348);
    CHECK(kSlot326EmptyViewCtorRva == 0xf180 && kSlot326SharedStrCtorRva == 0xf1a0);

    // Identity tags.
    CHECK(kSlot326BoolMidVptr == 0x15940b0 && kSlot326BoolFinalVptr == 0x1594218);
    CHECK(kSlot326FloatMidVptr == 0x1594128 && kSlot326FloatFinalVptr == 0x15942b0);
    CHECK(kSlot326EyeFinalVptr == 0x15943c0);
    CHECK(kSlot326BoolArgFnVtbl == 0x15cebe0 && kSlot326FloatArgFnVtbl == 0x15cec00);
    CHECK(kSlot326FloatFnCopyRva == 0xc4a0 && kSlot326FloatFnCloneRva == 0xc4c0);
    CHECK(kSlot326FloatFnMoveRva == 0xc4e0 && kSlot326FloatFnDestroyRva == 0xc4f0);
    CHECK(kSlot326BoolGate3Rva == 0xb890 && kSlot326BoolGate6Rva == 0xb930);
    CHECK(kSlot326FloatGate3Rva == 0xbba0 && kSlot326FloatGate6Rva == 0xbbf0);
    CHECK(kSlot326BoolGate3 == 1 && kSlot326BoolGate6 == 0);
    CHECK(kSlot326FloatGate3 == 1 && kSlot326FloatGate6 == 3);

    // Per-site constants.
    CHECK(kSlot326VarFlags == 1 && kSlot326ItemFlags == 0x30 && kSlot326LabelItemFlags == 0);
    CHECK(kSlot326ThresholdMinBits == 0 && kSlot326ThresholdMaxBits == 0x7f7fffffu);
    CHECK(kSlot326EyeMinBits == 0 && kSlot326EyeMaxBits == 0x40000000u);
    CHECK(kSlot326ThresholdValueBits == 0x40400000u);
    CHECK(kSlot326SliderMinBits == 0x3c23d70au && kSlot326SliderMaxBits == 0x3dcccccd);
    CHECK(kSlot326LabelRangeMin == 1.0f && kSlot326LabelRangeMax == 10.0f);
    CHECK(kSlot326GroupKind == 1 && kSlot326ItemKind == 2 && kSlot326LabelKind == 0);
    CHECK(kSlot326LabelTransientKind == 3);

    // The mirrored 0x2dce400 colour table.
    CHECK(kSlot326EyeValueTableRva == 0x2dce400);
    CHECK(kSlot326EyeValueBits.inactive[0] == 0x3f800000u && kSlot326EyeValueBits.inactive[3] == 0);
    CHECK(kSlot326EyeValueBits.blue[0] == 0x3e99999au && kSlot326EyeValueBits.blue[1] == 0x3f333333u);
    CHECK(kSlot326EyeValueBits.red[0] == 0x3f800000u && kSlot326EyeValueBits.red[1] == 0x3f000000u &&
          kSlot326EyeValueBits.red[2] == 0x3e99999au && kSlot326EyeValueBits.red[3] == 0x3f800000u);

    // The two bool variables.
    CHECK(kSlot326Bools.size() == 2);
    CHECK(std::strlen(kSlot326Bools[0].name) == kSlot326Bools[0].name_len && kSlot326Bools[0].name_len == 0x14);
    CHECK(std::strlen(kSlot326Bools[1].name) == kSlot326Bools[1].name_len && kSlot326Bools[1].name_len == 0x12);
    CHECK(kSlot326Bools[0].object_rva == 0x3291890 && kSlot326Bools[1].object_rva == 0x3291958);
    for (const auto& spec : kSlot326Bools) CHECK(spec.value == 0 && spec.default_value == 0);

    // The thirteen float variables: everything at the same 0xd8 / 0xd0 strides the body uses.
    CHECK(kSlot326Floats.size() == kSlot326FloatCount && kSlot326FloatCount == 13);
    CHECK(kSlot326Floats[0].object_rva == 0x3291a20 && !kSlot326Floats[0].eye_colour);
    CHECK(kSlot326Floats[0].value_ref_rva == 0x3291af0);   // its own +0xd0
    CHECK(kSlot326Floats[0].writes_value_cell);
    for (std::size_t i = 0; i < kSlot326Floats.size(); ++i) {
        const Slot326FloatSpec& spec = kSlot326Floats[i];
        CHECK(spec.index == i);
        CHECK(std::strlen(spec.name) == spec.name_len);
        CHECK(spec.min_bits == (i == 0 ? kSlot326ThresholdMinBits : kSlot326EyeMinBits));
        CHECK(spec.max_bits == (i == 0 ? kSlot326ThresholdMaxBits : kSlot326EyeMaxBits));
        CHECK(spec.final_vptr == (i == 0 ? kSlot326FloatFinalVptr : kSlot326EyeFinalVptr));
        if (i == 0) continue;
        CHECK(spec.eye_colour && !spec.writes_value_cell);
        // The eye colours are 0xd0 apart and their value cells walk the 0x2dce400 table by 4.
        CHECK(spec.object_rva == 0x3291d98 + 0xd0 * (i - 1));
        CHECK(spec.value_ref_rva == 0x2dce400 + 4 * (i - 1));
        CHECK(spec.eye_row == (i - 1) / 4 && spec.eye_channel == (i - 1) % 4);
        CHECK(spec.default_bits == (spec.eye_row == 0   ? kSlot326EyeValueBits.inactive
                                    : spec.eye_row == 1 ? kSlot326EyeValueBits.blue
                                                        : kSlot326EyeValueBits.red)[spec.eye_channel]);
    }
    CHECK(kSlot326Floats[12].object_rva == 0x3292688);

    // The twenty-one items, in body order.
    CHECK(kSlot326Items.size() == kSlot326ItemCount && kSlot326ItemCount == 21);
    CHECK(std::strlen(kSlot326GroupCrown) == 4 && std::strlen(kSlot326GroupColossus) == 8);
    CHECK(std::strlen(kSlot326GroupShadow) == 6 && std::strlen(kSlot326GroupEyeColor) == 0x10);
    CHECK(std::strlen(kSlot326GroupInactive) == 0x19 && std::strlen(kSlot326GroupBlue) == 0x15);
    CHECK(std::strlen(kSlot326GroupRed) == 0x14 && std::strlen(kSlot326SeparatorText) == 0xd);
    for (const auto& item : kSlot326Items) {
        CHECK(std::strlen(item.label) == item.label_len);
        if (item.text != nullptr) CHECK(std::strlen(item.text) == item.text_len);
        CHECK(item.flags == (item.kind == Slot326ItemKind::label ? kSlot326LabelItemFlags : kSlot326ItemFlags));
    }
    CHECK(kSlot326Items[0].object_rva == 0x3291af8 && kSlot326Items[4].object_rva == 0x3292758);
    CHECK(kSlot326Items[5].object_rva == 0x3292800 && kSlot326Items[5].text == nullptr);
    CHECK(kSlot326Items[20].object_rva == 0x32931d8);
    // 5 groups, 3 toggles, 12 sliders, 1 label row.
    std::uint32_t groups = 0, toggles = 0, sliders = 0, labels = 0;
    for (const auto& item : kSlot326Items) {
        switch (item.kind) {
            case Slot326ItemKind::group: ++groups; break;
            case Slot326ItemKind::toggle: ++toggles; break;
            case Slot326ItemKind::slider: ++sliders; break;
            case Slot326ItemKind::label: ++labels; break;
        }
    }
    CHECK(groups == 5 && toggles == 3 && sliders == 12 && labels == 1);
    // The one toggle on a FLOAT variable is the threshold's.
    CHECK(kSlot326Items[3].target_kind == Slot326TargetKind::float_var && kSlot326Items[3].target_index == 0);
    CHECK(kSlot326Items[1].target_kind == Slot326TargetKind::bool_var && kSlot326Items[1].target_index == 1);
    CHECK(kSlot326Items[2].target_kind == Slot326TargetKind::bool_var && kSlot326Items[2].target_index == 0);
    // The twelve sliders point at the twelve eye colours in body order.
    for (std::uint32_t i = 7; i <= 20; ++i) {
        if (kSlot326Items[i].kind != Slot326ItemKind::slider) continue;
        CHECK(kSlot326Items[i].target_kind == Slot326TargetKind::float_var);
        const std::uint32_t eye = kSlot326Items[i].target_index - 1;   // float index 1..12
        CHECK(eye < kSlot326EyeCount);
        CHECK(kSlot326Items[i].object_rva == 0x3292950 + 0xa8 * (i - 7));
    }

    // The 36 atexit rows, in body order, and their relation to the object table.
    CHECK(kSlot326AtexitRows.size() == kSlot326AtexitRowCount && kSlot326AtexitRowCount == 36);
    for (std::size_t i = 0; i < kSlot326AtexitRows.size(); ++i) {
        CHECK(kSlot326AtexitRows[i].step == i + 1);
    }
    CHECK(kSlot326AtexitRows[0].dtor_rva == kSlot326BoolVarDtorRva &&
          kSlot326AtexitRows[0].object_rva == 0x3291890);
    CHECK(kSlot326AtexitRows[1].object_rva == 0x3291958);
    CHECK(kSlot326AtexitRows[2].dtor_rva == kSlot326FloatVarDtorRva &&
          kSlot326AtexitRows[2].object_rva == 0x3291a20);
    for (std::size_t i = 3; i < 7; ++i) CHECK(kSlot326AtexitRows[i].dtor_rva == kSlot326MenuItemDtorRva);
    for (std::size_t i = 7; i < 19; ++i) CHECK(kSlot326AtexitRows[i].dtor_rva == kSlot326EyeVarDtorRva);
    for (std::size_t i = 19; i < 36; ++i) CHECK(kSlot326AtexitRows[i].dtor_rva == kSlot326MenuItemDtorRva);
    const std::uint64_t want_objects[36] = {
        0x3291890, 0x3291958, 0x3291a20, 0x3291af8, 0x3291ba0, 0x3291c48, 0x3291cf0, 0x3291d98, 0x3291e68,
        0x3291f38, 0x3292008, 0x32920d8, 0x32921a8, 0x3292278, 0x3292348, 0x3292418, 0x32924e8, 0x32925b8,
        0x3292688, 0x3292758, 0x3292800, 0x32928a8, 0x3292950, 0x32929f8, 0x3292aa0, 0x3292b48, 0x3292bf0,
        0x3292c98, 0x3292d40, 0x3292de8, 0x3292e90, 0x3292f38, 0x3292fe0, 0x3293088, 0x3293130, 0x32931d8};
    for (std::size_t i = 0; i < 36; ++i) CHECK(kSlot326AtexitRows[i].object_rva == want_objects[i]);
    // The per-object rows carry the same site the variable / item row does. Body order is
    // bool, bool, threshold, items 0..3, eyes 1..12, items 4..20 -- so the threshold is row 2 and the
    // twelve eye colours are rows 7..18.
    CHECK(kSlot326Floats[0].atexit_site_rva == kSlot326AtexitRows[2].site_rva);
    CHECK(kSlot326Floats[0].dtor_rva == kSlot326AtexitRows[2].dtor_rva);
    for (std::size_t i = 1; i < kSlot326FloatCount; ++i) {
        CHECK(kSlot326Floats[i].atexit_site_rva == kSlot326AtexitRows[6 + i].site_rva);
        CHECK(kSlot326Floats[i].dtor_rva == kSlot326AtexitRows[6 + i].dtor_rva);
    }
    CHECK(kSlot326Bools[0].atexit_site_rva == kSlot326AtexitRows[0].site_rva);
    CHECK(kSlot326Bools[1].atexit_site_rva == kSlot326AtexitRows[1].site_rva);
    CHECK(kSlot326Items[0].atexit_site_rva == kSlot326AtexitRows[3].site_rva);
    CHECK(kSlot326Items[4].atexit_site_rva == kSlot326AtexitRows[19].site_rva);
    CHECK(kSlot326Items[20].atexit_site_rva == kSlot326AtexitRows[35].site_rva);

    // Object strides and the aggregate.
    CHECK(sizeof(Slot326EyeVar) == 0xd0 && sizeof(ConCmdRangedVar) == 0xd8 && sizeof(ConCmdBoolVar) == 0xc8);
    CHECK(sizeof(DebugMenuItem) == 0xa8);
    CHECK(offsetof(Slot326Objects, bool_eye_state) == 0x000);
    CHECK(offsetof(Slot326Objects, bool_force_angry) == 0x0c8);
    CHECK(offsetof(Slot326Objects, threshold) == 0x190);
    CHECK(offsetof(Slot326Objects, group_colossus) == 0x268);
    CHECK(offsetof(Slot326Objects, toggle_force_angry) == 0x310);
    CHECK(offsetof(Slot326Objects, toggle_eye_state) == 0x3b8);
    CHECK(offsetof(Slot326Objects, toggle_threshold) == 0x460);
    CHECK(offsetof(Slot326Objects, eye) == 0x508);
    CHECK(offsetof(Slot326Objects, group_eye_color) == 0xec8);
    CHECK(offsetof(Slot326Objects, item_label) == 0xf70);
    CHECK(offsetof(Slot326Objects, group_inactive) == 0x1018);
    CHECK(offsetof(Slot326Objects, slider_inactive) == 0x10c0);
    CHECK(offsetof(Slot326Objects, group_blue) == 0x1360);
    CHECK(offsetof(Slot326Objects, slider_blue) == 0x1408);
    CHECK(offsetof(Slot326Objects, group_red) == 0x16a8);
    CHECK(offsetof(Slot326Objects, slider_red) == 0x1750);
    CHECK(kSlot326BlockRva + kSlot326BlockBytes == 0x3293280);
    CHECK(kSlot326AtexitRows[35].object_rva + sizeof(DebugMenuItem) == 0x3293280);

    // Stage names.
    CHECK(std::strcmp(slot326_stage_name(Slot326Stage::none), "none") == 0);
    CHECK(std::strcmp(slot326_stage_name(Slot326Stage::bool_var_build), "bool_var_build") == 0);
    CHECK(std::strcmp(slot326_stage_name(Slot326Stage::float_var_registration),
                      "float_var_registration") == 0);
    CHECK(std::strcmp(slot326_stage_name(Slot326Stage::item_ctor), "item_ctor") == 0);
    CHECK(std::strcmp(slot326_stage_name(Slot326Stage::canary), "canary") == 0);
}

void section_functor_and_target() {
    // The thirteen row-local `callq *0x18(%rax)`: inert only when the pinned body is a bare ret.
    CHECK(kSlot326FunctorSites.size() == kSlot326FunctorSiteCount && kSlot326FunctorSiteCount == 13);
    for (const auto& site : kSlot326FunctorSites) {
        CHECK(site.tag_rva == kSlot326FloatArgFnVtbl && site.slot3_rva == kSlot326FloatFnDestroyRva);
        CHECK(slot326_inert_functor_destroy(site));
    }
    Slot326FunctorSite unlifted = kSlot326FunctorSites[0];
    unlifted.slot3_is_bare_ret = false;
    CHECK(!slot326_inert_functor_destroy(unlifted));
    // The thirteen sites sit in body order, one per float variable.
    for (std::size_t i = 1; i < kSlot326FunctorSites.size(); ++i) {
        CHECK(kSlot326FunctorSites[i - 1].site_rva < kSlot326FunctorSites[i].site_rva);
    }

    // The 0x26ea0 gate: {1, 0} for the bool class (range stays {1.0, 10.0}), {1, 3} for the float
    // class (range narrows to {0.1, 0.5}), unknown for anything else.
    ConCmdBoolVar b{};
    b.base.vptr = kSlot326BoolFinalVptr;
    DebugMenuTarget t = slot326_menu_target(b.base);
    CHECK(t.known && t.object == &b.base && t.slot3 == 1 && t.slot6 == 0);
    ConCmdRangedVar f{};
    f.base.vptr = kSlot326FloatFinalVptr;
    t = slot326_menu_target(f.base);
    CHECK(t.known && t.object == &f.base && t.slot3 == 1 && t.slot6 == 3);
    f.base.vptr = kSlot326FloatMidVptr;               // the transient tag must not answer
    CHECK(!slot326_menu_target(f.base).known);
    b.base.vptr = kSlot326BoolMidVptr;
    CHECK(!slot326_menu_target(b.base).known);
    f.base.vptr = kSlot326EyeFinalVptr;               // no toggle targets an eye colour either
    CHECK(!slot326_menu_target(f.base).known);
}

void section_float_shapes() {
    // Both shapes carry the same net state; the 0xd8 one puts its value cell at its own +0xd0.
    ConCmdRegistry creg;
    Slot326EyeValues values = kSlot326EyeValueBits;
    ConCmdRangedVar threshold;
    poison(threshold);
    const RangedVarArgs args = [] { RangedVarArgs a{}; a.fn_vptr = kSlot326FloatArgFnVtbl; return a; }();
    CHECK(slot326_float_c300_state(threshold, kSlot326Floats[0], args, values));
    // 0xc300 ends with the mid vptr; the *site* re-stores the class vptr after the call.
    CHECK(threshold.base.vptr == kRangedFloatMidVptr);
    CHECK(threshold.base.flags == kSlot326VarFlags && threshold.base.registered == 0);
    CHECK(same_view(threshold.base.name, kSlot326Floats[0].name, kSlot326Floats[0].name_len));
    CHECK(threshold.base.description.size == 0 && threshold.base.description.ctl == nullptr);
    CHECK(threshold.value_ref == &threshold.value);
    CHECK(dword_at(threshold, 0x6c) == kSlot326ThresholdMinBits && threshold.min_engaged == 1);
    CHECK(dword_at(threshold, 0x74) == kSlot326ThresholdMaxBits && threshold.max_engaged == 1);
    CHECK(threshold.fn_vptr == kRangedFloatFnVtbl && threshold.fn_payload == 0 && threshold.fn_aux == 0);
    CHECK(threshold.args[0] == 0 && threshold.args[1] == 0 && threshold.args[2] == 0 && threshold.args[3] == 0);
    // 0xc300 writes the mid vptr, not the class one: the model re-applies the site's final store.
    CHECK(region_poison(threshold, 0x71, 3) && region_poison(threshold, 0x79, 7));
    CHECK(region_poison(threshold, 0xa0, 0x10) && region_poison(threshold, 0xb8, 8));

    Slot326EyeVar eye;
    poison(eye);
    CHECK(slot326_float_c300_state(eye, kSlot326Floats[1], args, values));
    CHECK(eye.base.vptr == kRangedFloatMidVptr);
    CHECK(same_view(eye.base.name, kSlot326Floats[1].name, kSlot326Floats[1].name_len));
    CHECK(eye.value_ref == &values.inactive[0]);
    CHECK(dword_at(eye, 0x6c) == kSlot326EyeMinBits && eye.min_engaged == 1);
    CHECK(dword_at(eye, 0x74) == kSlot326EyeMaxBits && eye.max_engaged == 1);
    CHECK(eye.fn_vptr == kRangedFloatFnVtbl && eye.fn_payload == 0 && eye.fn_aux == 0);
    CHECK(region_poison(eye, 0x71, 3) && region_poison(eye, 0x79, 7));
    CHECK(region_poison(eye, 0xa0, 0x10) && region_poison(eye, 0xb8, 8));
    // Every channel resolves to its own mirrored float.
    for (std::uint32_t i = 1; i < kSlot326FloatCount; ++i) {
        Slot326EyeVar cell;
        poison(cell);
        CHECK(slot326_float_c300_state(cell, kSlot326Floats[i], args, values));
        const Slot326FloatSpec& spec = kSlot326Floats[i];
        std::uint32_t* const row = spec.eye_row == 0   ? values.inactive
                                   : spec.eye_row == 1 ? values.blue
                                                       : values.red;
        CHECK(cell.value_ref == row + spec.eye_channel);
    }

    // A foreign argument tag is refused and writes nothing (the original would dispatch unlifted
    // slot bodies of that table).
    Slot326EyeVar refused;
    poison(refused);
    RangedVarArgs foreign{};
    foreign.fn_vptr = 0x1594318;                     // a table the c300 body knows, but not this site's
    CHECK(!slot326_float_c300_state(refused, kSlot326Floats[1], foreign, values));
    CHECK(region_poison(refused, 0x00, sizeof refused));
    ConCmdRangedVar refused2;
    poison(refused2);
    CHECK(!slot326_float_c300_state(refused2, kSlot326Floats[0], foreign, values));
    CHECK(region_poison(refused2, 0x00, sizeof refused2));
    // ... and the two shapes do not accept each other's specs.
    CHECK(!slot326_float_c300_state(refused, kSlot326Floats[0], args, values));
    CHECK(!slot326_float_c300_state(refused2, kSlot326Floats[1], args, values));
    CHECK(creg.pending.size() == 0);
}

void section_label_item() {
    // 0x26800: link, the caller's label, kind 3 -> 0, "MenuSeparator", the empty view, target 0,
    // range {1.0, 10.0}, the caller's flags.
    const Slot326Hooks hooks = default_slot326_hooks();
    DebugMenuItem item;
    poison(item);
    CHECK(init_slot326_label_item_26800(item, l2::measured_view(kSlot326GroupEyeColor, 0x10), 0,
                                       hooks.string_alloc));
    CHECK(item.kind == kSlot326LabelKind && item.flags == kSlot326LabelItemFlags);
    CHECK(item.link.self == nullptr && item.link.prev == &item.link && item.link.next == &item.link);
    CHECK(item.link.owner == nullptr && item.registered == 0);
    CHECK(same_view(item.label, kSlot326GroupEyeColor, 0x10));
    CHECK(item.text.data != nullptr && item.text.size == 0xd && item.text.ctl != nullptr);
    CHECK(std::memcmp(item.text.data, kSlot326SeparatorText, 0xd) == 0 && item.text.data[0xd] == 0);
    // 0xf180 stores the process-wide empty-string address point in the data word and zeroes the rest.
    CHECK(item.empty.data != nullptr && item.empty.size == 0 && item.empty.ctl == nullptr);
    CHECK(item.target == nullptr);
    CHECK(dword_at(item, 0x98) == 0x3f800000u && dword_at(item, 0x9c) == 0x41200000u);
    CHECK(region_poison(item, 0x21, 7) && region_poison(item, 0x4c, 4));

    // Without the allocator the ctor refuses BEFORE the first store.
    DebugMenuItem refused;
    poison(refused);
    CHECK(!init_slot326_label_item_26800(refused, l2::measured_view(kSlot326GroupEyeColor, 0x10), 0,
                                        SharedStrCtorHooks{}));
    CHECK(region_poison(refused, 0x00, sizeof refused));

    // A failing allocator is the same refusal, not a partial object.
    struct BadAlloc {
        static void* allocate(std::size_t, std::uint32_t, std::uint32_t, std::uint32_t) noexcept {
            return nullptr;
        }
        static bool deallocate(void*, std::uint32_t) noexcept { return true; }
    };
    SharedStrCtorHooks bad{};
    bad.alloc.allocate = &BadAlloc::allocate;
    bad.alloc.deallocate = &BadAlloc::deallocate;
    poison(refused);
    CHECK(!init_slot326_label_item_26800(refused, l2::measured_view(kSlot326GroupEyeColor, 0x10), 0, bad));
    CHECK(region_poison(refused, 0x00, sizeof refused));
}

void section_native_build() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot326Objects o;
    Slot326EyeValues values;
    poison(o);
    poison(values);
    const Slot326Hooks hooks = default_slot326_hooks();
    CHECK(hooks.string_alloc.alloc.allocate != nullptr && hooks.string_alloc.alloc.deallocate != nullptr);
    CHECK(hooks.stack_guard == nullptr);
    const Slot326Result r = init_slot326_837f30(o, values, creg, menu, hooks);

    CHECK(r.ok && r.blocked == Slot326Stage::none && r.blocked_index == 0);
    CHECK(r.atexit_records == 36 && r.functors_destroyed == 13);
    CHECK(r.vars_built == 15 && r.menu_items == 21 && r.views_inert);
    CHECK(r.eye_values_mirrored && !r.canary_checked && !r.canary_matched);

    // The mirrored colour table was seeded and then only read (this body never writes it).
    CHECK(std::memcmp(&values, &kSlot326EyeValueBits, sizeof(Slot326EyeValues)) == 0);

    // The two bools: class 0x1594218, argument tag 0x15cebe0, value_ref = their own +0xc0.
    CHECK(o.bool_eye_state.base.vptr == kSlot326BoolFinalVptr && o.bool_eye_state.base.registered == 1);
    CHECK(same_view(o.bool_eye_state.base.name, kSlot326Bools[0].name, kSlot326Bools[0].name_len));
    CHECK(o.bool_eye_state.value_ref == &o.bool_eye_state.value);
    CHECK(o.bool_eye_state.value == 0 && o.bool_eye_state.default_value == 0);
    CHECK(o.bool_eye_state.fn_vptr == kSlot326BoolArgFnVtbl);
    CHECK(o.bool_force_angry.base.vptr == kSlot326BoolFinalVptr && o.bool_force_angry.base.registered == 1);
    CHECK(same_view(o.bool_force_angry.base.name, kSlot326Bools[1].name, kSlot326Bools[1].name_len));
    CHECK(o.bool_force_angry.value_ref == &o.bool_force_angry.value);
    CHECK(region_poison(o.bool_eye_state, 0x6d, 3) && region_poison(o.bool_eye_state, 0x90, 0x10));

    // The threshold: its own value cell, both 3.0f stores, class 0x15942b0.
    CHECK(o.threshold.base.vptr == kSlot326FloatFinalVptr && o.threshold.base.registered == 1);
    CHECK(o.threshold.value_ref == &o.threshold.value);
    CHECK(o.threshold.value == kSlot326ThresholdValueBits);
    CHECK(o.threshold.default_value == kSlot326ThresholdValueBits);
    CHECK(dword_at(o.threshold, 0x6c) == kSlot326ThresholdMinBits && o.threshold.min_engaged == 1);
    CHECK(dword_at(o.threshold, 0x74) == kSlot326ThresholdMaxBits && o.threshold.max_engaged == 1);

    // The twelve eye colours: class 0x15943c0, value cells inside the mirror, default = the cell.
    for (std::size_t i = 0; i < kSlot326EyeCount; ++i) {
        const Slot326EyeVar& v = o.eye[i];
        const Slot326FloatSpec& spec = kSlot326Floats[i + 1];
        CHECK(v.base.vptr == kSlot326EyeFinalVptr && v.base.registered == 1 && v.base.flags == 1);
        CHECK(same_view(v.base.name, spec.name, spec.name_len));
        CHECK(v.value_ref == slot326_value_cell(values, spec));
        CHECK(v.default_value == spec.default_bits);
        CHECK(dword_at(v, 0x6c) == kSlot326EyeMinBits && dword_at(v, 0x74) == kSlot326EyeMaxBits);
        CHECK(region_poison(v, 0x71, 3) && region_poison(v, 0x79, 7) && region_poison(v, 0xa0, 0x10));
        CHECK(region_poison(v, 0xb8, 8));
        CHECK(r.float_vars[i + 1].ok && r.float_vars[i + 1].registration.ok);
        CHECK(r.float_vars[i + 1].name_inert && r.float_vars[i + 1].description_inert &&
              r.float_vars[i + 1].functor_inert);
    }
    CHECK(r.bool_vars[0].ok && r.bool_vars[1].ok && r.float_vars[0].ok);

    // The twenty-one items.
    const DebugMenuItem* const items[21] = {
        &o.group_colossus,   &o.toggle_force_angry, &o.toggle_eye_state, &o.toggle_threshold,
        &o.group_eye_color,  &o.item_label,         &o.group_inactive,   &o.slider_inactive[0],
        &o.slider_inactive[1], &o.slider_inactive[2], &o.slider_inactive[3], &o.group_blue,
        &o.slider_blue[0],   &o.slider_blue[1],     &o.slider_blue[2],   &o.slider_blue[3],
        &o.group_red,        &o.slider_red[0],      &o.slider_red[1],    &o.slider_red[2],
        &o.slider_red[3]};
    for (int i = 0; i < 21; ++i) {
        const Slot326ItemSpec& spec = kSlot326Items[i];
        const DebugMenuItem& it = *items[i];
        CHECK(&it == &slot326_item(o, static_cast<std::uint32_t>(i)));
        CHECK(same_view(it.label, spec.label, spec.label_len));
        if (spec.text != nullptr) CHECK(same_view(it.text, spec.text, spec.text_len));
        CHECK(it.flags == spec.flags && it.registered == 0);
        CHECK(r.item_registrations[i].ok);
        switch (spec.kind) {
            case Slot326ItemKind::group: CHECK(it.kind == kSlot326GroupKind); break;
            case Slot326ItemKind::toggle: CHECK(it.kind == kSlot326ItemKind); break;
            case Slot326ItemKind::slider: CHECK(it.kind == kSlot326ItemKind); break;
            case Slot326ItemKind::label: CHECK(it.kind == kSlot326LabelKind); break;
        }
        if (spec.target_kind == Slot326TargetKind::none) {
            CHECK(it.target == nullptr);
        } else if (spec.target_kind == Slot326TargetKind::bool_var) {
            CHECK(it.target == &slot326_bool_base(o, spec.target_index));
        } else {
            CHECK(it.target == &slot326_float_base(o, spec.target_index));
        }
        // Ranges: groups and label rows {1.0, 10.0}; the bool toggles too, because slot3/slot6 is
        // {1, 0}; the float toggle narrows to {0.1, 0.5}; every slider is {0.01, 0.1}.
        std::uint32_t lo = 0x3f800000u, hi = 0x41200000u;
        if (spec.kind == Slot326ItemKind::slider) {
            lo = kSlot326SliderMinBits;
            hi = kSlot326SliderMaxBits;
        } else if (spec.target_kind == Slot326TargetKind::float_var) {
            lo = 0x3dcccccd;
            hi = 0x3f000000u;
        }
        CHECK(dword_at(it, 0x98) == lo && dword_at(it, 0x9c) == hi);
    }
    // The label row's own built text.
    CHECK(o.item_label.text.size == 0xd && o.item_label.text.ctl != nullptr);
    CHECK(std::memcmp(o.item_label.text.data, kSlot326SeparatorText, 0xd) == 0);

    // Registration side effects: fifteen pending ConCmds and twenty-one root-list items.
    CHECK(creg.pending.size() == 15);
    CHECK(creg.pending.first() == &o.bool_eye_state.base.pending);
    CHECK(menu.root_guard_done && menu.root.count == 21);

    // The 36 atexit records in body order, with the menu root's record between bool 2 and item 4.
    CHECK(creg.atexit_log.size() == 37);
    CHECK(creg.atexit_log.at(3).dtor_rva == kDebugMenuRootDtorRva);
    CHECK(creg.atexit_log.at(3).object == &menu.root);
    StartupAtexitEntry recs[36]{};
    CHECK(slot_records(creg.atexit_log, recs, 36) == 36);
    const void* const want_object[36] = {
        &o.bool_eye_state, &o.bool_force_angry, &o.threshold, &o.group_colossus, &o.toggle_force_angry,
        &o.toggle_eye_state, &o.toggle_threshold, &o.eye[0], &o.eye[1], &o.eye[2], &o.eye[3], &o.eye[4],
        &o.eye[5], &o.eye[6], &o.eye[7], &o.eye[8], &o.eye[9], &o.eye[10], &o.eye[11], &o.group_eye_color,
        &o.item_label, &o.group_inactive, &o.slider_inactive[0], &o.slider_inactive[1],
        &o.slider_inactive[2], &o.slider_inactive[3], &o.group_blue, &o.slider_blue[0], &o.slider_blue[1],
        &o.slider_blue[2], &o.slider_blue[3], &o.group_red, &o.slider_red[0], &o.slider_red[1],
        &o.slider_red[2], &o.slider_red[3]};
    for (std::size_t i = 0; i < 36; ++i) {
        CHECK(recs[i].dtor_rva == kSlot326AtexitRows[i].dtor_rva);
        CHECK(recs[i].object == want_object[i]);
    }
}

void section_no_allocator() {
    // The 0x26800 row is the only site that allocates. It is the SIXTH object of the body (step 21 of
    // 36), so without the allocator contract the fragment stops there after the two bools, the
    // threshold, the "Colossus" group with its three toggles, the twelve eye colours and the
    // "Shadow Eye Color" group -- 15 variables and 5 items.
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot326Objects o;
    Slot326EyeValues values;
    poison(o);
    const Slot326Result r =
        init_slot326_837f30(o, values, creg, menu, [] {
            Slot326Hooks h = default_slot326_hooks();
            h.string_alloc = {};
            return h;
        }());
    CHECK(!r.ok && r.blocked == Slot326Stage::item_ctor && r.blocked_index == 5);
    CHECK(r.vars_built == 15 && r.menu_items == 5 && r.atexit_records == 20);
    CHECK(region_poison(o.item_label, 0x00, sizeof(DebugMenuItem)));
    CHECK(creg.atexit_log.size() == 21);   // 20 + the menu root's record
}

void section_menu_registration_blocked() {
    // A non-null parent cell with no insert contract fails the first group visibly.
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    int token = 0;
    menu.parent.parent = &token;
    Slot326Objects o;
    Slot326EyeValues values;
    const Slot326Result r = init_slot326_837f30(o, values, creg, menu, default_slot326_hooks());
    CHECK(!r.ok && r.blocked == Slot326Stage::item_registration && r.blocked_index == 0);
    CHECK(r.vars_built == 3 && r.atexit_records == 3 && r.menu_items == 0);
    CHECK(!r.item_registrations[0].ok && r.item_registrations[0].path == DebugMenuRegisterPath::parent);
    CHECK(menu.root.count == 0 && !menu.root_guard_done);
}

void section_parent_contract() {
    // With a menu parent contract every item goes to the parent and the root is never initialised, so
    // the log holds exactly the body's 36 records.
    static int inserted = 0;
    struct P { static bool insert(void*, DebugMenuItem&) noexcept { ++inserted; return true; } };
    inserted = 0;
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    int token = 0;
    menu.parent.parent = &token;
    menu.parent.insert = &P::insert;
    Slot326Objects o;
    Slot326EyeValues values;
    const Slot326Result r = init_slot326_837f30(o, values, creg, menu, default_slot326_hooks());
    CHECK(r.ok && inserted == 21 && menu.root.count == 0 && !menu.root_guard_done);
    CHECK(o.group_colossus.registered == 1 && o.slider_red[3].registered == 1);
    CHECK(creg.atexit_log.size() == 36 && r.atexit_records == 36);
}

void section_canary() {
    // The body's guard compare is at 0x839aa8. A changed guard takes the original's
    // `jne 0x839ab9` / __stack_chk_fail path, so the model fails at `canary` after every object
    // was built.
    std::uint64_t guard = 0x0123456789abcdefull;
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot326Objects o;
    Slot326EyeValues values;
    Slot326Hooks hooks = default_slot326_hooks();
    hooks.stack_guard = &guard;
    const Slot326Result matched = init_slot326_837f30(o, values, creg, menu, hooks);
    CHECK(matched.ok && matched.canary_checked && matched.canary_matched);
    CHECK(matched.atexit_records == 36 && matched.menu_items == 21 && matched.vars_built == 15);

    // Now the same guard, corrupted inside the body's own registration call.
    struct Q {
        static bool insert(void*, DebugMenuItem&) noexcept {
            *guard_cell ^= 1;
            return true;
        }
    };
    guard = 0x0123456789abcdefull;
    guard_cell = &guard;
    ConCmdRegistry creg2;
    DebugMenuRegistry menu2;
    int token = 0;
    menu2.parent.parent = &token;
    menu2.parent.insert = &Q::insert;
    Slot326Objects o2;
    Slot326EyeValues values2;
    const Slot326Result r = init_slot326_837f30(o2, values2, creg2, menu2, hooks);
    CHECK(!r.ok && r.blocked == Slot326Stage::canary);
    CHECK(r.canary_checked && !r.canary_matched);
    CHECK(r.atexit_records == 36 && r.menu_items == 21 && r.vars_built == 15);
}

}  // namespace

int main() {
    section_constants();
    section_functor_and_target();
    section_float_shapes();
    section_label_item();
    section_native_build();
    section_no_allocator();
    section_menu_registration_blocked();
    section_parent_contract();
    section_canary();
    std::puts("OK test_ctor_slot_326");
    return 0;
}
