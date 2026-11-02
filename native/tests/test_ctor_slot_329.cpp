// CPU-01-STARTUP-SLOT-329-CHARACTERIZATION-01: fixture for startup constructor index 329
// (0x829e90, the Shadow Path / path-finding debug page: seven console bools, two integer range
// variables built by the block's own leaf/wrapper constructors, one float range variable, a
// "Shadow"/"Shadow Path" group, ten toggles, one separator row and 22 __cxa_atexit records).
//
// Covers: the pinned constants (table word, object strides, atexit rows, variable/item rows, the
// four target classes and their gate answers), the two local ranged-variable constructors field by
// field, the complete atexit order, and the fail-visible stages: a missing allocator for the
// separator text, an unpinned argument tag, a null value cell, a failed menu registration, the
// parent-contract path and a changed __stack_chk_guard.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_329_829e90.hpp"

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

// The body's own 22 atexit records, skipping the one 0x268b0 records for the menu root.
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
    CHECK(kSlot329Rva == 0x829e90 && kSlot329BodySize == 4082 && kSlot329EndRva == 0x82ae82);
    CHECK(kSlot329ReachableBytes == 4081 && kSlot329InstructionCount == 774);
    CHECK(kSlot329DirectCallSites == 97 && kSlot329DirectCallees == 11);
    CHECK(kSlot329IndirectCallSites == 3);
    CHECK(kSlot329RipOperands == 124 && kSlot329RipTargets == 69);
    CHECK(kSlot329StoreSites == 11 && kSlot329StoreTargets == 9);
    CHECK(kSlot329BranchCount == 1 && kSlot329BasicBlocks == 2);
    // 0x2dd70e0 - 329*8 == 0x2dd6698, the one word that references the body. (0x2dd65b8, quoted in the
    // task brief, is entry 357 = 0x78c760.)
    CHECK(kSlot329TableTopRva - 8 * kSlot329TableIndex == kSlot329TableWordRva);
    CHECK(kSlot329TableWordRva == 0x2dd6698 && kSlot329TableIndex == 329);
    CHECK(kSlot329TableTopRva - 0x2dd65b8 == 357 * 8);
    CHECK(kSlot329NextUnrecoveredIndex == 336 && kSlot329NextUnrecoveredRva == 0x7ec4c0);
    CHECK(kSlot329CanaryCellRva == 0x15fe518);
    CHECK(kSlot329StackChkFailRva == 0xf8f3a0 && kSlot329CanaryFrameOffset == 0xc0);
    CHECK(kSlot329BranchRva == 0x82ae6b && kSlot329BranchTargetRva == 0x82ae7c);
    CHECK(kSlot329EpilogueRva == 0x82ae6d && kSlot329FrameBytes == 0xe0);
    CHECK(kSlot329AtexitRva == 0xf8f390 && kSlot329DsoRva == 0x1604218 && kSlot329ReleaseRva == 0xf590);
    CHECK(kSlot329BaseCtorRva == 0x20af0 && kSlot329RegisterRva == 0x20d60);
    CHECK(kSlot329ToggleCtorRva == 0x26ea0 && kSlot329GroupCtorRva == 0x26c10);
    CHECK(kSlot329SeparatorCtorRva == 0x26b60 && kSlot329FloatCtorRva == 0xc300);
    CHECK(kSlot329FloatCtorSize == 0x160);
    // The block's own ranged-variable constructors.
    CHECK(kSlot329RangedLeafARva == 0x8298d0 && kSlot329RangedLeafASize == 352);
    CHECK(kSlot329RangedWrapperRva == 0x829b30 && kSlot329RangedWrapperSize == 436);
    CHECK(kSlot329RangedLeafBRva == 0x829cf0 && kSlot329RangedLeafBSize == 352);

    // Identity tags.
    CHECK(kSlot329BoolMidVptr == 0x15940b0 && kSlot329BoolFinalVptr == 0x1594218);
    CHECK(kSlot329BoolArgFnVtbl == 0x15cea30);
    CHECK(kSlot329NodeFnCopyRva == 0xc2a0 && kSlot329NodeFnCloneRva == 0xc2c0);
    CHECK(kSlot329NodeFnMoveRva == 0xc2e0 && kSlot329NodeFnDestroyRva == 0xc2f0);
    CHECK(kSlot329FloatMidVptr == 0x1594128 && kSlot329FloatFinalVptr == 0x15942b0);
    CHECK(kSlot329FloatClassFnVtbl == 0x1594318 && kSlot329FloatArgFnVtbl == 0x15ceb80);
    CHECK(kSlot329FloatFnCopyRva == 0xc4a0 && kSlot329FloatFnCloneRva == 0xc4c0);
    CHECK(kSlot329FloatFnMoveRva == 0xc4e0 && kSlot329FloatFnDestroyRva == 0xc4f0);
    CHECK(kSlot329PathDataMidVptr == 0x15ce950 && kSlot329PathDataFinalVptr == 0x15cea60);
    CHECK(kSlot329PathDataArgFnVtbl == 0x15ceac8);
    CHECK(kSlot329PathDataFnCopyRva == 0x829a70 && kSlot329PathDataFnCloneRva == 0x829a90);
    CHECK(kSlot329PathDataFnMoveRva == 0x829ab0 && kSlot329PathDataFnDestroyRva == 0x829ac0);
    CHECK(kSlot329CharaMidVptr == 0x15ce9c8 && kSlot329CharaFinalVptr == 0x15ceb18);
    CHECK(kSlot329CharaArgFnVtbl == 0x15ceae8);
    CHECK(kSlot329CharaFnCopyRva == 0x829ad0 && kSlot329CharaFnCloneRva == 0x829af0);
    CHECK(kSlot329CharaFnMoveRva == 0x829b10 && kSlot329CharaFnDestroyRva == 0x829b20);
    CHECK(kSlot329PathDataDtorRva == 0x81be00 && kSlot329PathDataNonDeletingDtorRva == 0x829a30);
    CHECK(kSlot329CharaDtorRva == 0x81be40 && kSlot329CharaNonDeletingDtorRva == 0x829e50);
    CHECK(kSlot329BoolVarDtorRva == 0x20b0 && kSlot329FloatVarDtorRva == 0x20f0);
    CHECK(kSlot329MenuItemDtorRva == 0x272d0);
    // The four classes' 0x26ea0 gate answers.
    CHECK(kSlot329BoolGate3Rva == 0xb890 && kSlot329BoolGate6Rva == 0xb930);
    CHECK(kSlot329FloatGate3Rva == 0xbba0 && kSlot329FloatGate6Rva == 0xbbf0);
    CHECK(kSlot329PathDataGate3Rva == 0x8254c0 && kSlot329PathDataGate6Rva == 0x825510);
    CHECK(kSlot329CharaGate3Rva == 0x825f40 && kSlot329CharaGate6Rva == 0x8261e0);
    CHECK(kSlot329BoolGate3 == 1 && kSlot329BoolGate6 == 0);
    CHECK(kSlot329FloatGate3 == 1 && kSlot329FloatGate6 == 3);
    CHECK(kSlot329PathDataGate3 == 1 && kSlot329PathDataGate6 == 1);
    CHECK(kSlot329CharaGate3 == 1 && kSlot329CharaGate6 == 1);

    // Per-site constants.
    CHECK(kSlot329VarFlags == 1 && kSlot329ItemFlags == 0x30);
    CHECK(kSlot329PathDataMin == 0 && kSlot329PathDataMax == 2 && kSlot329PathDataValue == 2);
    CHECK(kSlot329CharaMin == 0 && kSlot329CharaMax == 3 && kSlot329CharaValue == 1);
    CHECK(kSlot329TranslationMinBits == 0 && kSlot329TranslationMaxBits == 0x447a0000u);
    CHECK(kSlot329TranslationValueBits == 0x3f800000u);
    CHECK(kSlot329FunctorDispatches == 24);
    CHECK(kSlot329PathDataDispatches == 6 && kSlot329CharaDispatches == 12 &&
          kSlot329TranslationDispatches == 6);
    CHECK(kSlot329EmptyTextRva == 0x128dc9a && sizeof kSlot329EmptyText == 1);

    // The two ranged classes.
    CHECK(kSlot329RangedClasses.size() == kSlot329RangedClassCount && kSlot329RangedClassCount == 2);
    CHECK(kSlot329RangedClasses[0].fn_vtbl == kSlot329PathDataArgFnVtbl && !kSlot329RangedClasses[0].wrapped);
    CHECK(kSlot329RangedClasses[0].functor_dispatches == 5);
    CHECK(kSlot329RangedClasses[1].fn_vtbl == kSlot329CharaArgFnVtbl && kSlot329RangedClasses[1].wrapped);
    CHECK(kSlot329RangedClasses[1].functor_dispatches == 11);
    CHECK(slot329_ranged_class_for(kSlot329PathDataArgFnVtbl) == &kSlot329RangedClasses[0]);
    CHECK(slot329_ranged_class_for(kSlot329CharaArgFnVtbl) == &kSlot329RangedClasses[1]);
    CHECK(slot329_ranged_class_for(kSlot329FloatArgFnVtbl) == nullptr);
    CHECK(slot329_ranged_class_for(0x1594318) == nullptr);

    // The ten variables, in body order.
    CHECK(kSlot329Vars.size() == kSlot329VarCount && kSlot329VarCount == 10);
    for (const Slot329VarSpec& spec : kSlot329Vars) {
        CHECK(std::strlen(spec.name) == spec.name_len);
        if (spec.description != nullptr) CHECK(std::strlen(spec.description) == spec.description_len);
    }
    std::uint32_t bools = 0, ranges = 0, floats = 0;
    for (const Slot329VarSpec& spec : kSlot329Vars) {
        switch (spec.kind) {
            case Slot329VarKind::bool_var: ++bools; break;
            case Slot329VarKind::ranged_int: ++ranges; break;
            case Slot329VarKind::ranged_float: ++floats; break;
        }
    }
    CHECK(bools == 7 && ranges == 2 && floats == 1);
    CHECK(kSlot329Vars[0].object_rva == 0x3288d98 && kSlot329Vars[0].value == 1 &&
          kSlot329Vars[0].default_value == 1);
    CHECK(kSlot329Vars[1].object_rva == 0x3288e60 && kSlot329Vars[1].value == 0);
    CHECK(kSlot329Vars[2].object_rva == 0x3288f28 &&
          kSlot329Vars[2].final_vptr == kSlot329PathDataFinalVptr);
    CHECK(kSlot329Vars[3].object_rva == 0x3289000);
    CHECK(kSlot329Vars[4].object_rva == 0x32890c8);
    CHECK(kSlot329Vars[5].object_rva == 0x3289190 &&
          kSlot329Vars[5].final_vptr == kSlot329CharaFinalVptr &&
          kSlot329Vars[5].description_rva == 0x128d73d && kSlot329Vars[5].description_len == 0x46);
    CHECK(kSlot329Vars[6].object_rva == 0x3289268 &&
          kSlot329Vars[6].final_vptr == kSlot329FloatFinalVptr);
    CHECK(kSlot329Vars[7].object_rva == 0x3289340);
    CHECK(kSlot329Vars[8].object_rva == 0x3289408 && kSlot329Vars[8].value == 1 &&
          kSlot329Vars[8].default_value == 1);
    CHECK(kSlot329Vars[9].object_rva == 0x32894d0 && kSlot329Vars[9].value == 0);
    // Only the three range rows carry a range functor tag; the seven bools share 0x15cea30.
    for (std::size_t i = 0; i < kSlot329VarCount; ++i) {
        const Slot329VarSpec& spec = kSlot329Vars[i];
        if (spec.kind == Slot329VarKind::bool_var) CHECK(spec.fn_vtbl == kSlot329BoolArgFnVtbl);
        if (i != 5) CHECK(spec.description == nullptr);
    }
    // The variable objects are contiguous at the strides the body uses (0xc8 bool / 0xd8 range).
    CHECK(kSlot329Vars[1].object_rva - kSlot329Vars[0].object_rva == 0xc8);
    CHECK(kSlot329Vars[2].object_rva - kSlot329Vars[1].object_rva == 0xc8);
    CHECK(kSlot329Vars[3].object_rva - kSlot329Vars[2].object_rva == 0xd8);
    CHECK(kSlot329Vars[4].object_rva - kSlot329Vars[3].object_rva == 0xc8);
    CHECK(kSlot329Vars[5].object_rva - kSlot329Vars[4].object_rva == 0xc8);
    CHECK(kSlot329Vars[6].object_rva - kSlot329Vars[5].object_rva == 0xd8);
    CHECK(kSlot329Vars[7].object_rva - kSlot329Vars[6].object_rva == 0xd8);
    CHECK(kSlot329Vars[9].object_rva - kSlot329Vars[8].object_rva == 0xc8);

    // The twelve items, in body order.
    CHECK(kSlot329Items.size() == kSlot329ItemCount && kSlot329ItemCount == 12);
    CHECK(kSlot329ToggleCount == 10);
    for (const Slot329ItemSpec& item : kSlot329Items) {
        CHECK(std::strlen(item.label) == item.label_len);
        if (item.text != nullptr) CHECK(std::strlen(item.text) == item.text_len);
        CHECK(item.flags == kSlot329ItemFlags);
    }
    std::uint32_t groups = 0, toggles = 0, separators = 0;
    for (const Slot329ItemSpec& item : kSlot329Items) {
        switch (item.kind) {
            case Slot329ItemKind::group: ++groups; break;
            case Slot329ItemKind::toggle: ++toggles; break;
            case Slot329ItemKind::separator: ++separators; break;
        }
    }
    CHECK(groups == 1 && toggles == kSlot329ToggleCount && separators == 1);
    CHECK(kSlot329Items[0].object_rva == 0x3289598 && kSlot329Items[0].kind == Slot329ItemKind::group);
    CHECK(kSlot329Items[1].object_rva == 0x3289640 && kSlot329Items[1].target_index == 1);
    CHECK(kSlot329Items[5].target_index == 0);       // Enable Path Finding -> the first variable
    CHECK(kSlot329Items[6].kind == Slot329ItemKind::separator && kSlot329Items[6].text == nullptr);
    CHECK(kSlot329Items[6].object_rva == 0x3289988);
    CHECK(kSlot329Items[11].object_rva == 0x3289cd0 && kSlot329Items[11].target_index == 9);
    for (std::size_t i = 1; i < kSlot329Items.size(); ++i) {
        CHECK(kSlot329Items[i].object_rva - kSlot329Items[i - 1].object_rva == 0xa8);
    }
    // Every toggle targets a distinct variable, and the two range variables own toggles too.
    bool seen[kSlot329VarCount]{};
    for (const Slot329ItemSpec& item : kSlot329Items) {
        if (item.kind != Slot329ItemKind::toggle) continue;
        CHECK(item.target_index < kSlot329VarCount && !seen[item.target_index]);
        seen[item.target_index] = true;
    }
    for (bool hit : seen) CHECK(hit);

    // The 22 atexit rows, in body order.
    CHECK(kSlot329AtexitRows.size() == kSlot329AtexitRowCount && kSlot329AtexitRowCount == 22);
    for (std::size_t i = 0; i < kSlot329AtexitRows.size(); ++i) {
        CHECK(kSlot329AtexitRows[i].step == i + 1);
        if (i > 0) CHECK(kSlot329AtexitRows[i - 1].site_rva < kSlot329AtexitRows[i].site_rva);
    }
    const std::uint64_t want_objects[22] = {
        0x3288d98, 0x3288e60, 0x3288f28, 0x3289000, 0x32890c8, 0x3289190, 0x3289268, 0x3289340,
        0x3289408, 0x32894d0, 0x3289598, 0x3289640, 0x32896e8, 0x3289790, 0x3289838, 0x32898e0,
        0x3289988, 0x3289a30, 0x3289ad8, 0x3289b80, 0x3289c28, 0x3289cd0};
    const std::uint64_t want_dtors[22] = {
        kSlot329BoolVarDtorRva, kSlot329BoolVarDtorRva, kSlot329PathDataDtorRva, kSlot329BoolVarDtorRva,
        kSlot329BoolVarDtorRva, kSlot329CharaDtorRva, kSlot329FloatVarDtorRva, kSlot329BoolVarDtorRva,
        kSlot329BoolVarDtorRva, kSlot329BoolVarDtorRva, kSlot329MenuItemDtorRva, kSlot329MenuItemDtorRva,
        kSlot329MenuItemDtorRva, kSlot329MenuItemDtorRva, kSlot329MenuItemDtorRva, kSlot329MenuItemDtorRva,
        kSlot329MenuItemDtorRva, kSlot329MenuItemDtorRva, kSlot329MenuItemDtorRva, kSlot329MenuItemDtorRva,
        kSlot329MenuItemDtorRva, kSlot329MenuItemDtorRva};
    for (std::size_t i = 0; i < 22; ++i) {
        CHECK(kSlot329AtexitRows[i].object_rva == want_objects[i]);
        CHECK(kSlot329AtexitRows[i].dtor_rva == want_dtors[i]);
    }
    // The per-object rows carry the same site the variable / item row does.
    for (std::size_t i = 0; i < kSlot329VarCount; ++i)
        CHECK(kSlot329Vars[i].atexit_site_rva == kSlot329AtexitRows[i].site_rva);
    for (std::size_t i = 0; i < kSlot329ItemCount; ++i)
        CHECK(kSlot329Items[i].atexit_site_rva == kSlot329AtexitRows[10 + i].site_rva);

    // Object strides and the aggregate.
    CHECK(sizeof(ConCmdBoolVar) == 0xc8 && sizeof(ConCmdRangedVar) == 0xd8 && sizeof(DebugMenuItem) == 0xa8);
    CHECK(offsetof(Slot329Objects, bool_path_finding) == 0x000);
    CHECK(offsetof(Slot329Objects, bool_draw_enable) == 0x0c8);
    CHECK(offsetof(Slot329Objects, ranged_path_data) == 0x190);
    CHECK(offsetof(Slot329Objects, bool_attain_range) == 0x268);
    CHECK(offsetof(Slot329Objects, bool_enable_editing) == 0x330);
    CHECK(offsetof(Slot329Objects, ranged_chara_type) == 0x3f8);
    CHECK(offsetof(Slot329Objects, float_translation) == 0x4d0);
    CHECK(offsetof(Slot329Objects, bool_all_names) == 0x5a8);
    CHECK(offsetof(Slot329Objects, bool_raycasts_enable) == 0x670);
    CHECK(offsetof(Slot329Objects, bool_raycasts_debug) == 0x738);
    CHECK(offsetof(Slot329Objects, group) == 0x800);
    CHECK(offsetof(Slot329Objects, items) == 0x8a8);
    CHECK(kSlot329BlockRva + kSlot329BlockBytes == 0x3289d78);
    CHECK(kSlot329AtexitRows[21].object_rva + sizeof(DebugMenuItem) == 0x3289d78);
    CHECK(kSlot329BlockRva + offsetof(Slot329Objects, items) == kSlot329Items[1].object_rva);
    CHECK(kSlot329BlockRva + offsetof(Slot329Objects, group) == kSlot329Items[0].object_rva);

    // Stage names.
    CHECK(std::strcmp(slot329_stage_name(Slot329Stage::none), "none") == 0);
    CHECK(std::strcmp(slot329_stage_name(Slot329Stage::bool_var_build), "bool_var_build") == 0);
    CHECK(std::strcmp(slot329_stage_name(Slot329Stage::ranged_var_registration),
                      "ranged_var_registration") == 0);
    CHECK(std::strcmp(slot329_stage_name(Slot329Stage::item_ctor), "item_ctor") == 0);
    CHECK(std::strcmp(slot329_stage_name(Slot329Stage::canary), "canary") == 0);
}

void section_functor_and_target() {
    // The three row-local `callq *0x18(%rax)`: inert only when the pinned body is a bare ret.
    CHECK(kSlot329FunctorSites.size() == kSlot329FunctorSiteCount && kSlot329FunctorSiteCount == 3);
    for (const Slot329FunctorSite& site : kSlot329FunctorSites) CHECK(slot329_inert_functor_destroy(site));
    CHECK(kSlot329FunctorSites[0].tag_rva == kSlot329PathDataArgFnVtbl &&
          kSlot329FunctorSites[0].slot3_rva == kSlot329PathDataFnDestroyRva);
    CHECK(kSlot329FunctorSites[1].tag_rva == kSlot329CharaArgFnVtbl &&
          kSlot329FunctorSites[1].slot3_rva == kSlot329CharaFnDestroyRva);
    CHECK(kSlot329FunctorSites[2].tag_rva == kSlot329FloatArgFnVtbl &&
          kSlot329FunctorSites[2].slot3_rva == kSlot329FloatFnDestroyRva);
    for (std::size_t i = 1; i < kSlot329FunctorSites.size(); ++i)
        CHECK(kSlot329FunctorSites[i - 1].site_rva < kSlot329FunctorSites[i].site_rva);
    Slot329FunctorSite unlifted = kSlot329FunctorSites[0];
    unlifted.slot3_is_bare_ret = false;
    CHECK(!slot329_inert_functor_destroy(unlifted));

    // The 0x26ea0 gate: {1, 0} bool (range stays {1.0, 10.0}), {1, 3} float (narrows to {0.1, 0.5}),
    // {1, 1} for both integer ranges (range stays {1.0, 10.0}), unknown for anything else.
    ConCmdBoolVar b{};
    b.base.vptr = kSlot329BoolFinalVptr;
    DebugMenuTarget t = slot329_menu_target(b.base);
    CHECK(t.known && t.object == &b.base && t.slot3 == 1 && t.slot6 == 0);
    ConCmdRangedVar f{};
    f.base.vptr = kSlot329FloatFinalVptr;
    t = slot329_menu_target(f.base);
    CHECK(t.known && t.slot3 == 1 && t.slot6 == 3);
    f.base.vptr = kSlot329PathDataFinalVptr;
    t = slot329_menu_target(f.base);
    CHECK(t.known && t.slot3 == 1 && t.slot6 == 1);
    f.base.vptr = kSlot329CharaFinalVptr;
    t = slot329_menu_target(f.base);
    CHECK(t.known && t.slot3 == 1 && t.slot6 == 1);
    // A transient / foreign tag must not answer.
    f.base.vptr = kSlot329PathDataMidVptr;
    CHECK(!slot329_menu_target(f.base).known);
    f.base.vptr = kSlot329CharaMidVptr;
    CHECK(!slot329_menu_target(f.base).known);
    f.base.vptr = kSlot329FloatMidVptr;
    CHECK(!slot329_menu_target(f.base).known);
    b.base.vptr = kSlot329BoolMidVptr;
    CHECK(!slot329_menu_target(b.base).known);
    f.base.vptr = kSlot329FloatClassFnVtbl;
    CHECK(!slot329_menu_target(f.base).known);
}

void section_range_shapes() {
    RangedVarArgs args{};
    args.fn_vptr = kSlot329PathDataArgFnVtbl;
    ConCmdRangedVar range;
    poison(range);
    // Leaf A (class P): the mid vptr, the caller's value cell, both bounds engaged, the class functor
    // tag, and the argument copy under the self-pointer rule.
    CHECK(slot329_ranged_var_state(range, kSlot329RangedClasses[0],
                                   l2::measured_view(kSlot329Vars[2].name, kSlot329Vars[2].name_len),
                                   &range.value, kSlot329PathDataMin, kSlot329PathDataMax,
                                   kSlot329VarFlags, args, SharedStr{}));
    CHECK(range.base.vptr == kSlot329PathDataMidVptr);
    CHECK(range.base.flags == kSlot329VarFlags && range.base.registered == 0);
    CHECK(range.value_ref == &range.value);
    CHECK(dword_at(range, 0x6c) == kSlot329PathDataMin && range.min_engaged == 1);
    CHECK(dword_at(range, 0x74) == kSlot329PathDataMax && range.max_engaged == 1);
    CHECK(range.fn_vptr == kSlot329PathDataArgFnVtbl && range.fn_payload == 0 && range.fn_aux == 0);
    CHECK(range.args[0] == 0 && range.args[1] == 0 && range.args[2] == 0 && range.args[3] == 0);
    CHECK(region_poison(range, 0x71, 3) && region_poison(range, 0x79, 7));
    CHECK(region_poison(range, 0xa0, 0x10) && region_poison(range, 0xb8, 8));

    // Class C: same stores, its own tags.
    RangedVarArgs cargs{};
    cargs.fn_vptr = kSlot329CharaArgFnVtbl;
    ConCmdRangedVar chara;
    poison(chara);
    CHECK(slot329_ranged_var_state(chara, kSlot329RangedClasses[1],
                                   l2::measured_view(kSlot329Vars[5].name, kSlot329Vars[5].name_len),
                                   &chara.value, kSlot329CharaMin, kSlot329CharaMax,
                                   kSlot329VarFlags, cargs,
                                   l2::measured_view(kSlot329Vars[5].description,
                                                     kSlot329Vars[5].description_len)));
    CHECK(chara.base.vptr == kSlot329CharaMidVptr);
    CHECK(chara.value_ref == &chara.value);
    CHECK(dword_at(chara, 0x6c) == kSlot329CharaMin && dword_at(chara, 0x74) == kSlot329CharaMax);
    CHECK(chara.fn_vptr == kSlot329CharaArgFnVtbl);
    CHECK(same_view(chara.base.description, kSlot329Vars[5].description,
                    kSlot329Vars[5].description_len));

    // A foreign argument tag is refused and writes nothing (the original would dispatch that table's
    // unlifted slot bodies).
    ConCmdRangedVar refused;
    poison(refused);
    RangedVarArgs foreign{};
    foreign.fn_vptr = kSlot329CharaArgFnVtbl;   // a class this body knows, but not this row's
    CHECK(!slot329_ranged_var_state(refused, kSlot329RangedClasses[0],
                                    l2::measured_view(kSlot329Vars[2].name, kSlot329Vars[2].name_len),
                                    &refused.value, 0, 2, kSlot329VarFlags, foreign, SharedStr{}));
    CHECK(region_poison(refused, 0x00, sizeof refused));
    // A null value cell is refused the same way (the wrapper would fault on `*value_ref`).
    poison(refused);
    CHECK(!slot329_ranged_var_state(refused, kSlot329RangedClasses[0],
                                    l2::measured_view(kSlot329Vars[2].name, kSlot329Vars[2].name_len),
                                    nullptr, 0, 2, kSlot329VarFlags, args, SharedStr{}));
    CHECK(region_poison(refused, 0x00, sizeof refused));

    // 0xc300 for the one float row (site tag 0x15ceb80).
    RangedVarArgs fargs{};
    fargs.fn_vptr = kSlot329FloatArgFnVtbl;
    ConCmdRangedVar speed;
    poison(speed);
    CHECK(slot329_float_var_state(speed, l2::measured_view(kSlot329Vars[6].name, kSlot329Vars[6].name_len),
                                  &speed.value, kSlot329TranslationMinBits,
                                  kSlot329TranslationMaxBits, kSlot329VarFlags, fargs, SharedStr{}));
    // 0xc300 ends with the mid vptr; the *site* re-stores the class vptr after the call.
    CHECK(speed.base.vptr == kSlot329FloatMidVptr);
    CHECK(speed.value_ref == &speed.value);
    CHECK(dword_at(speed, 0x6c) == kSlot329TranslationMinBits && speed.min_engaged == 1);
    CHECK(dword_at(speed, 0x74) == kSlot329TranslationMaxBits && speed.max_engaged == 1);
    // The object's own functor tag is the class tag, not the caller's tag.
    CHECK(speed.fn_vptr == kSlot329FloatClassFnVtbl && speed.fn_payload == 0 && speed.fn_aux == 0);
    CHECK(region_poison(speed, 0x71, 3) && region_poison(speed, 0x79, 7));
    CHECK(region_poison(speed, 0xa0, 0x10) && region_poison(speed, 0xb8, 8));

    // The float row refuses another tag and writes nothing.
    RangedVarArgs fforeign{};
    fforeign.fn_vptr = kSlot329FloatClassFnVtbl;
    ConCmdRangedVar refused2;
    poison(refused2);
    CHECK(!slot329_float_var_state(refused2, l2::measured_view(kSlot329Vars[6].name, kSlot329Vars[6].name_len),
                                   &refused2.value, 0, kSlot329TranslationMaxBits, kSlot329VarFlags,
                                   fforeign, SharedStr{}));
    CHECK(region_poison(refused2, 0x00, sizeof refused2));
    poison(refused2);
    CHECK(!slot329_float_var_state(refused2, l2::measured_view(kSlot329Vars[6].name, kSlot329Vars[6].name_len),
                                   nullptr, 0, kSlot329TranslationMaxBits, kSlot329VarFlags, fargs,
                                   SharedStr{}));
    CHECK(region_poison(refused2, 0x00, sizeof refused2));
}

void section_native_build() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot329Objects o;
    poison(o);
    const Slot329Hooks hooks = default_slot329_hooks();
    CHECK(hooks.separator_alloc.alloc.allocate != nullptr && hooks.separator_alloc.alloc.deallocate != nullptr);
    CHECK(hooks.stack_guard == nullptr);
    const Slot329Result r = init_slot329_829e90(o, creg, menu, hooks);

    CHECK(r.ok && r.blocked == Slot329Stage::none && r.blocked_index == 0);
    CHECK(r.atexit_records == 22 && r.functors_destroyed == 3 && r.functor_dispatches == 24);
    CHECK(r.vars_built == 10 && r.menu_items == 12 && r.views_inert);
    CHECK(!r.canary_checked && !r.canary_matched);

    // The seven bools: class 0x1594218, argument tag 0x15cea30, value_ref = their own +0xc0.
    const ConCmdBoolVar* const bools[7] = {
        &o.bool_path_finding, &o.bool_draw_enable, &o.bool_attain_range, &o.bool_enable_editing,
        &o.bool_all_names, &o.bool_raycasts_enable, &o.bool_raycasts_debug};
    const std::uint32_t bool_index[7] = {0, 1, 3, 4, 7, 8, 9};
    for (int i = 0; i < 7; ++i) {
        const ConCmdBoolVar& v = *bools[i];
        const Slot329VarSpec& spec = kSlot329Vars[bool_index[i]];
        CHECK(v.base.vptr == kSlot329BoolFinalVptr && v.base.registered == 1 && v.base.flags == 1);
        CHECK(same_view(v.base.name, spec.name, spec.name_len));
        CHECK(v.base.description.size == 0 && v.base.description.ctl == nullptr);
        CHECK(v.value_ref == &v.value);
        CHECK(v.value == spec.value && v.default_value == spec.default_value);
        CHECK(v.fn_vptr == kSlot329BoolArgFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
        CHECK(region_poison(v, 0x6d, 3) && region_poison(v, 0x90, 0x10));
        CHECK(r.vars[bool_index[i]].ok && r.vars[bool_index[i]].registration.ok);
        CHECK(r.vars[bool_index[i]].atexit_recorded);
    }
    CHECK(o.bool_path_finding.value == 1 && o.bool_draw_enable.value == 0);
    CHECK(o.bool_raycasts_enable.value == 1 && o.bool_raycasts_debug.value == 0);

    // The path-data range: class P, its own value cell, bounds [0, 2], value/default 2.
    CHECK(o.ranged_path_data.base.vptr == kSlot329PathDataFinalVptr);
    CHECK(o.ranged_path_data.base.registered == 1 && o.ranged_path_data.base.flags == 1);
    CHECK(o.ranged_path_data.value_ref == &o.ranged_path_data.value);
    CHECK(o.ranged_path_data.value == kSlot329PathDataValue);
    CHECK(o.ranged_path_data.default_value == kSlot329PathDataValue);
    CHECK(dword_at(o.ranged_path_data, 0x6c) == kSlot329PathDataMin && o.ranged_path_data.min_engaged == 1);
    CHECK(dword_at(o.ranged_path_data, 0x74) == kSlot329PathDataMax && o.ranged_path_data.max_engaged == 1);
    CHECK(o.ranged_path_data.fn_vptr == kSlot329PathDataArgFnVtbl);
    CHECK(region_poison(o.ranged_path_data, 0x71, 3) && region_poison(o.ranged_path_data, 0xa0, 0x10));
    CHECK(r.vars[2].ok && r.vars[2].functor_dispatches == kSlot329PathDataDispatches);

    // The chara-type range: class C, bounds [0, 3], value/default 1, the one non-empty description.
    CHECK(o.ranged_chara_type.base.vptr == kSlot329CharaFinalVptr);
    CHECK(o.ranged_chara_type.base.registered == 1);
    CHECK(o.ranged_chara_type.value_ref == &o.ranged_chara_type.value);
    CHECK(o.ranged_chara_type.value == kSlot329CharaValue);
    CHECK(o.ranged_chara_type.default_value == kSlot329CharaValue);
    CHECK(dword_at(o.ranged_chara_type, 0x6c) == kSlot329CharaMin);
    CHECK(dword_at(o.ranged_chara_type, 0x74) == kSlot329CharaMax);
    CHECK(o.ranged_chara_type.fn_vptr == kSlot329CharaArgFnVtbl);
    CHECK(same_view(o.ranged_chara_type.base.description, kSlot329Vars[5].description,
                    kSlot329Vars[5].description_len));
    CHECK(r.vars[5].ok && r.vars[5].functor_dispatches == kSlot329CharaDispatches);

    // The float range: class 0x15942b0, bounds [0, 1000], value/default 1.0f.
    CHECK(o.float_translation.base.vptr == kSlot329FloatFinalVptr);
    CHECK(o.float_translation.base.registered == 1);
    CHECK(o.float_translation.value_ref == &o.float_translation.value);
    CHECK(o.float_translation.value == kSlot329TranslationValueBits);
    CHECK(o.float_translation.default_value == kSlot329TranslationValueBits);
    CHECK(dword_at(o.float_translation, 0x6c) == kSlot329TranslationMinBits);
    CHECK(dword_at(o.float_translation, 0x74) == kSlot329TranslationMaxBits);
    CHECK(o.float_translation.fn_vptr == kSlot329FloatClassFnVtbl);
    CHECK(r.vars[6].ok && r.vars[6].functor_dispatches == kSlot329TranslationDispatches);

    // The twelve items.
    for (std::size_t i = 0; i < kSlot329ItemCount; ++i) {
        const Slot329ItemSpec& spec = kSlot329Items[i];
        const DebugMenuItem& it = slot329_item(o, static_cast<std::uint32_t>(i));
        CHECK(&it == &slot329_item(o, static_cast<std::uint32_t>(i)));
        CHECK(same_view(it.label, spec.label, spec.label_len));
        if (spec.text != nullptr) CHECK(same_view(it.text, spec.text, spec.text_len));
        CHECK(it.flags == spec.flags && it.registered == 0);
        CHECK(r.item_registrations[i].ok);
        switch (spec.kind) {
            case Slot329ItemKind::group: CHECK(it.kind == kMenuKindGroup); break;
            case Slot329ItemKind::toggle: CHECK(it.kind == kMenuKindCommand); break;
            case Slot329ItemKind::separator: CHECK(it.kind == kMenuKindSeparator); break;
        }
        if (spec.kind == Slot329ItemKind::toggle) {
            CHECK(it.target == &slot329_var_base(o, spec.target_index));
        } else {
            CHECK(it.target == nullptr);
        }
        // Ranges: groups and the separator {1.0, 10.0}; every integer/bool toggle too (slot3/slot6 is
        // {1, 0} or {1, 1}); the float toggle narrows to {0.1, 0.5}.
        std::uint32_t lo = 0x3f800000u, hi = 0x41200000u;
        if (spec.kind == Slot329ItemKind::toggle && spec.target_index == 6) {
            lo = 0x3dcccccd;
            hi = 0x3f000000u;
        }
        CHECK(dword_at(it, 0x98) == lo && dword_at(it, 0x9c) == hi);
    }
    // The separator's own built text.
    CHECK(o.items[5].text.size == 0xd && o.items[5].text.ctl != nullptr);
    CHECK(std::memcmp(o.items[5].text.data, kMenuSeparatorText, 0xd) == 0);

    // Registration side effects: ten pending ConCmds and twelve root-list items.
    CHECK(creg.pending.size() == 10);
    CHECK(creg.pending.first() == &o.bool_path_finding.base.pending);
    CHECK(menu.root_guard_done && menu.root.count == 12);

    // The 22 atexit records in body order, with the menu root's record after the tenth variable.
    CHECK(creg.atexit_log.size() == 23);
    CHECK(creg.atexit_log.at(10).dtor_rva == kDebugMenuRootDtorRva);
    CHECK(creg.atexit_log.at(10).object == &menu.root);
    StartupAtexitEntry recs[22]{};
    CHECK(slot_records(creg.atexit_log, recs, 22) == 22);
    const void* const want_object[22] = {
        &o.bool_path_finding, &o.bool_draw_enable, &o.ranged_path_data, &o.bool_attain_range,
        &o.bool_enable_editing, &o.ranged_chara_type, &o.float_translation, &o.bool_all_names,
        &o.bool_raycasts_enable, &o.bool_raycasts_debug, &o.group, &o.items[0], &o.items[1],
        &o.items[2], &o.items[3], &o.items[4], &o.items[5], &o.items[6], &o.items[7], &o.items[8],
        &o.items[9], &o.items[10]};
    for (std::size_t i = 0; i < 22; ++i) {
        CHECK(recs[i].dtor_rva == kSlot329AtexitRows[i].dtor_rva);
        CHECK(recs[i].object == want_object[i]);
    }
}

void section_no_allocator() {
    // The separator row is the only site that allocates. It is the SEVENTH item (step 17 of 22), so
    // without the allocator contract the fragment stops there after all ten variables and six items.
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot329Objects o;
    poison(o);
    const Slot329Result r = init_slot329_829e90(o, creg, menu, [] {
        Slot329Hooks h = default_slot329_hooks();
        h.separator_alloc = {};
        return h;
    }());
    CHECK(!r.ok && r.blocked == Slot329Stage::item_ctor && r.blocked_index == 6);
    CHECK(r.vars_built == 10 && r.menu_items == 6 && r.atexit_records == 16);
    CHECK(region_poison(o.items[5], 0x00, sizeof(DebugMenuItem)));
    CHECK(creg.atexit_log.size() == 17);   // 16 + the menu root's record

    // A failing allocator is the same refusal, not a partial object.
    struct BadAlloc {
        static void* allocate(std::size_t, std::uint32_t, std::uint32_t, std::uint32_t) noexcept {
            return nullptr;
        }
        static bool deallocate(void*, std::uint32_t) noexcept { return true; }
    };
    ConCmdRegistry creg2;
    DebugMenuRegistry menu2;
    Slot329Objects o2;
    poison(o2);
    Slot329Hooks bad = default_slot329_hooks();
    bad.separator_alloc.alloc.allocate = &BadAlloc::allocate;
    bad.separator_alloc.alloc.deallocate = &BadAlloc::deallocate;
    const Slot329Result r2 = init_slot329_829e90(o2, creg2, menu2, bad);
    CHECK(!r2.ok && r2.blocked == Slot329Stage::item_ctor && r2.blocked_index == 6);
    CHECK(region_poison(o2.items[5], 0x00, sizeof(DebugMenuItem)));
}

void section_menu_registration_blocked() {
    // A non-null parent cell with no insert contract fails the first group visibly.
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    int token = 0;
    menu.parent.parent = &token;
    Slot329Objects o;
    const Slot329Result r = init_slot329_829e90(o, creg, menu, default_slot329_hooks());
    CHECK(!r.ok && r.blocked == Slot329Stage::item_registration && r.blocked_index == 0);
    CHECK(r.vars_built == 10 && r.atexit_records == 10 && r.menu_items == 0);
    CHECK(!r.item_registrations[0].ok && r.item_registrations[0].path == DebugMenuRegisterPath::parent);
    CHECK(menu.root.count == 0 && !menu.root_guard_done);
}

void section_parent_contract() {
    // With a menu parent contract every item goes to the parent and the root is never initialised, so
    // the log holds exactly the body's 22 records.
    static int inserted = 0;
    struct P { static bool insert(void*, DebugMenuItem&) noexcept { ++inserted; return true; } };
    inserted = 0;
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    int token = 0;
    menu.parent.parent = &token;
    menu.parent.insert = &P::insert;
    Slot329Objects o;
    const Slot329Result r = init_slot329_829e90(o, creg, menu, default_slot329_hooks());
    CHECK(r.ok && inserted == 12 && menu.root.count == 0 && !menu.root_guard_done);
    CHECK(o.group.registered == 1 && o.items[10].registered == 1);
    CHECK(creg.atexit_log.size() == 22 && r.atexit_records == 22);
}

void section_canary() {
    // The body's guard compare is at 0x82ae6b. A changed guard takes the original's
    // `jne 0x82ae7c` / __stack_chk_fail path, so the model fails at `canary` after every object was
    // built.
    std::uint64_t guard = 0x0123456789abcdefull;
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot329Objects o;
    Slot329Hooks hooks = default_slot329_hooks();
    hooks.stack_guard = &guard;
    const Slot329Result matched = init_slot329_829e90(o, creg, menu, hooks);
    CHECK(matched.ok && matched.canary_checked && matched.canary_matched);
    CHECK(matched.atexit_records == 22 && matched.menu_items == 12 && matched.vars_built == 10);

    // Now the same guard, corrupted inside the body's own registration call. (The body registers
    // twelve items, so the hook flips the guard exactly once -- twelve flips would restore it.)
    struct Q {
        static bool insert(void*, DebugMenuItem&) noexcept {
            static bool flipped = false;
            if (!flipped) {
                *guard_cell ^= 1;
                flipped = true;
            }
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
    Slot329Objects o2;
    const Slot329Result r = init_slot329_829e90(o2, creg2, menu2, hooks);
    CHECK(!r.ok && r.blocked == Slot329Stage::canary);
    CHECK(r.canary_checked && !r.canary_matched);
    CHECK(r.atexit_records == 22 && r.menu_items == 12 && r.vars_built == 10);
}

}  // namespace

int main() {
    section_constants();
    section_functor_and_target();
    section_range_shapes();
    section_native_build();
    section_no_allocator();
    section_menu_registration_blocked();
    section_parent_contract();
    section_canary();
    std::puts("OK test_ctor_slot_329");
    return 0;
}
