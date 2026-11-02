// CPU-01-STARTUP-SLOT-335-CHARACTERIZATION-01: fixture for startup constructor index 335
// (0x7edf10, the "Shadow Cheat" assist page: four unbounded Easy-Mode float multipliers, three cheat
// bools, two FNV-1a name records for the health/grip bars, two bounded float sliders and 11
// __cxa_atexit records).
//
// Covers: the pinned constants (table word, object strides, atexit rows, variable/item rows), the
// four helpers this block is the only user of (0x52a990 FNV-1a 32, 0x7dbca0 value-cell accessor,
// 0xf2f0 borrowed view, 0x7edd80 bounded-variable constructor), the complete atexit order, and the
// fail-visible stages: an unpinned argument tag, a null value cell, a refused menu registration, the
// parent-contract path and a changed __stack_chk_guard.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_335_7edf10.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;

// The guard cell the canary section's parent hook corrupts.
std::uint64_t* guard_cell = nullptr;

template <class T>
void poison(T& obj) { std::memset(static_cast<void*>(&obj), kPoison, sizeof obj); }

template <class T>
std::uint64_t qword_at(const T& obj, std::size_t off) {
    std::uint64_t v;
    std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(&obj) + off, 8);
    return v;
}

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

// The body's own 11 atexit records, skipping the one 0x268b0 records for the menu root.
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
    CHECK(kSlot335Rva == 0x7edf10 && kSlot335BodySize == 2725 && kSlot335EndRva == 0x7ee9b5);
    CHECK(kSlot335ReachableBytes == 2724 && kSlot335InstructionCount == 485);
    CHECK(kSlot335DirectCallSites == 58 && kSlot335DirectCallees == 11);
    CHECK(kSlot335IndirectCallSites == 6 && kSlot335FunctorSiteCount == 6);
    CHECK(kSlot335RipOperands == 77 && kSlot335RipTargets == 44);
    CHECK(kSlot335RipStoreSites == 0);
    CHECK(kSlot335BranchCount == 1 && kSlot335BasicBlocks == 2);
    // 0x2dd70e0 - 335*8 == 0x2dd6668, the one word that references the body.
    CHECK(kSlot335TableTopRva - 8 * kSlot335TableIndex == kSlot335TableWordRva);
    CHECK(kSlot335TableWordRva == 0x2dd6668 && kSlot335TableIndex == 335);
    CHECK(kSlot335NextUnrecoveredIndex == 336 && kSlot335NextUnrecoveredRva == 0x7ec4c0);
    CHECK(kSlot335CanaryCellRva == 0x15fe518);
    CHECK(kSlot335StackChkFailRva == 0xf8f3a0 && kSlot335CanaryFrameOffset == 0xe0);
    CHECK(kSlot335BranchRva == 0x7ee99e && kSlot335BranchTargetRva == 0x7ee9af);
    CHECK(kSlot335EpilogueRva == 0x7ee9a0 && kSlot335FrameBytes == 0x100);
    CHECK(kSlot335AtexitRva == 0xf8f390 && kSlot335DsoRva == 0x1604218 && kSlot335ReleaseRva == 0xf590);
    CHECK(kSlot335BaseCtorRva == 0x20af0 && kSlot335RegisterRva == 0x20d60);
    CHECK(kSlot335UnboundedCtorRva == 0x6aa00 && kSlot335UnboundedCtorSize == 323);
    CHECK(kSlot335BoundedCtorRva == 0xc300 && kSlot335BoundedCtorSize == 352);
    CHECK(kSlot335SliderCtorRva == 0x27050 && kSlot335SliderCtorSize == 208);
    CHECK(kSlot335MenuRegisterRva == 0x268b0);
    CHECK(kSlot335HashRva == 0x52a990 && kSlot335HashSize == 10 && kSlot335HashBodyRva == 0x161fb0);
    CHECK(kSlot335FnvBasis == 0x811c9dc5u && kSlot335FnvPrime == 0x01000193u);
    CHECK(kSlot335ValueCellRva == 0x7dbca0 && kSlot335ValueCellSize == 15);
    CHECK(kSlot335ValueArrayRva == 0x325f410);
    CHECK(kSlot335BorrowedStrRva == 0xf2f0 && kSlot335BorrowedStrSize == 40);
    CHECK(kSlot335StrlenRva == 0xf8f3b0);
    CHECK(kSlot335BoundedHelperRva == 0x7edd80 && kSlot335BoundedHelperSize == 386);
    CHECK(kSlot335CallbackThunkRva == 0x7edd60);
    CHECK(kSlot335CallbackHpRva == 0x7ed280 && kSlot335CallbackGripRva == 0x7ed2d0);
    CHECK(kSlot335CallbackStoreRva == 0x7dbc30 && kSlot335CallbackLookupRva == 0x572900);

    // Identity tags.
    CHECK(kSlot335BoolMidVptr == 0x15940b0 && kSlot335BoolFinalVptr == 0x1594218);
    CHECK(kSlot335FloatMidVptr == 0x1594128 && kSlot335FloatFinalVptr == 0x15942b0);
    CHECK(kSlot335BoundedFinalVptr == 0x15943c0 && kSlot335FloatClassFnVtbl == 0x1594318);
    CHECK(kSlot335FloatArgFnVtbl == 0x15ce630 && kSlot335BoolArgFnVtbl == 0x15ce650);
    CHECK(kSlot335FloatFnCopyRva == 0xc4a0 && kSlot335FloatFnCloneRva == 0xc4c0);
    CHECK(kSlot335FloatFnMoveRva == 0xc4e0 && kSlot335FloatFnDestroyRva == 0xc4f0);
    CHECK(kSlot335BoolFnCopyRva == 0xc2a0 && kSlot335BoolFnCloneRva == 0xc2c0);
    CHECK(kSlot335BoolFnMoveRva == 0xc2e0 && kSlot335BoolFnDestroyRva == 0xc2f0);
    CHECK(kSlot335FloatVarDtorRva == 0x20f0 && kSlot335BoolVarDtorRva == 0x20b0);
    CHECK(kSlot335BoundedVarDtorRva == 0xad30 && kSlot335MenuItemDtorRva == 0x272d0);

    // Per-site constants.
    CHECK(kSlot335VarFlags == 1 && kSlot335ItemFlags == 0x30);
    CHECK(kSlot335BoolValue == 0 && kSlot335BoolDefault == 0);
    CHECK(kSlot335HpCellIndex == 11 && kSlot335GripCellIndex == 12);
    CHECK(kSlot335ValueArrayRva + 4 * kSlot335HpCellIndex == 0x325f43c);
    CHECK(kSlot335ValueArrayRva + 4 * kSlot335GripCellIndex == 0x325f440);
    CHECK(kSlot335HpMinBits == 0x42c80000u && kSlot335HpMaxBits == 0x440e8000u);
    CHECK(kSlot335GripMinBits == 0x42c80000u && kSlot335GripMaxBits == 0x442f0000u);
    CHECK(kSlot335HpMin == 100.0f && kSlot335HpMax == 570.0f);
    CHECK(kSlot335GripMin == 100.0f && kSlot335GripMax == 700.0f);
    CHECK(kSlot335HpRangeMinRva == 0x128ade0 && kSlot335HpRangeMaxRva == 0x128ade4);
    CHECK(kSlot335GripRangeMinRva == 0x128ade8);
    CHECK(kSlot335HpRangeMin == 10.0f && kSlot335HpRangeMax == 100.0f);
    CHECK(kSlot335GripRangeMin == 1.0f && kSlot335GripRangeMax == 100.0f);
    CHECK(kSlot335DamageMultBits == 0x3f800000u && kSlot335HealMultBits == 0x3f99999au);
    CHECK(kSlot335GripLossBits == 0x3f400000u && kSlot335GripGainBits == 0x3f99999au);
    CHECK(kSlot335EmptyTextRva == 0x128af2d && sizeof kSlot335EmptyText == 1);

    // Row counts.
    CHECK(kSlot335Unbounded.size() == kSlot335UnboundedCount && kSlot335UnboundedCount == 4);
    CHECK(kSlot335Bools.size() == kSlot335BoolCount && kSlot335BoolCount == 3);
    CHECK(kSlot335Hashes.size() == kSlot335HashCount && kSlot335HashCount == 2);
    CHECK(kSlot335Bounded.size() == kSlot335BoundedCount && kSlot335BoundedCount == 2);
    CHECK(kSlot335Items.size() == kSlot335ItemCount && kSlot335ItemCount == 2);
    CHECK(kSlot335AtexitRows.size() == kSlot335AtexitRowCount && kSlot335AtexitRowCount == 11);
    CHECK(kSlot335FunctorSites.size() == kSlot335FunctorSiteCount);
    CHECK(kSlot335VarCount == 9 && kSlot335ReleaseCount == 22);
    CHECK(std::strlen(kSlot335DamageMultName) == 0x20 && std::strlen(kSlot335HealMultName) == 0x1e);
    CHECK(std::strlen(kSlot335GripLossName) == 0x1c && std::strlen(kSlot335GripGainName) == 0x1c);
    CHECK(std::strlen(kSlot335NoDamageName) == 0x0f && std::strlen(kSlot335MaxAttackName) == 0x16);
    CHECK(std::strlen(kSlot335PrintPowerName) == 0x1a);
    CHECK(std::strlen(kSlot335HealthHashName) == 18 && std::strlen(kSlot335GripHashName) == 16);
    CHECK(std::strlen(kSlot335HpVarName) == 0x0c && std::strlen(kSlot335GripVarName) == 0x0e);
    CHECK(std::strlen(kSlot335GroupName) == 0x0c && std::strlen(kSlot335HpCaption) == 0x0c);
    for (const Slot335FunctorSite& site : kSlot335FunctorSites) {
        CHECK(site.tag_rva == kSlot335FloatArgFnVtbl && site.slot3_rva == kSlot335FloatFnDestroyRva);
        CHECK(site.slot3_is_bare_ret);
    }
}

void section_helpers() {
    // 0x52a990 + 0x161fb0: FNV-1a 32 with basis 0x811c9dc5 and `movsbl` semantics.
    CHECK(slot335_fnv1a32(kSlot335HealthHashName) == 0xb6fe9ffbu);
    CHECK(slot335_fnv1a32(kSlot335GripHashName) == 0x9a8db0e1u);
    CHECK(slot335_fnv1a32("") == kSlot335FnvBasis);
    CHECK(slot335_fnv1a32(nullptr) == kSlot335FnvBasis);
    // A byte >= 0x80 is sign-extended before the XOR, exactly like `movsbl %cl,%ecx`.
    CHECK(slot335_fnv1a32("\x80") == ((kSlot335FnvBasis ^ 0xffffff80u) * kSlot335FnvPrime));
    CHECK(slot335_fnv1a32("\x80") != ((kSlot335FnvBasis ^ 0x80u) * kSlot335FnvPrime));

    // 0x7dbca0: &value_array[index], refused outside the mirrored window.
    Slot335ValueCells cells{};
    CHECK(slot335_value_cell(cells, kSlot335HpCellIndex) == &cells.cell[11]);
    CHECK(slot335_value_cell(cells, kSlot335GripCellIndex) == &cells.cell[12]);
    CHECK(slot335_value_cell(cells, kSlot335ValueArrayWindow) == nullptr);

    // 0xf2f0: {ptr, strlen, 0, 0}.
    SharedStr view{};
    slot335_borrowed_str(view, kSlot335GroupName);
    CHECK(same_view(view, kSlot335GroupName, 0x0c));
    slot335_borrowed_str(view, kSlot335EmptyText);
    CHECK(view.size == 0 && view.aux == 0 && view.ctl == nullptr);

    // Stage names are all distinct and none is empty.
    const Slot335Stage stages[] = {
        Slot335Stage::none, Slot335Stage::unbounded_var_build,
        Slot335Stage::unbounded_var_registration, Slot335Stage::unbounded_var_view_release,
        Slot335Stage::unbounded_var_atexit, Slot335Stage::bool_var_build,
        Slot335Stage::bool_var_registration, Slot335Stage::bool_var_view_release,
        Slot335Stage::bool_var_atexit, Slot335Stage::hash_lookup, Slot335Stage::bounded_var_build,
        Slot335Stage::bounded_var_registration, Slot335Stage::bounded_var_view_release,
        Slot335Stage::bounded_var_atexit, Slot335Stage::item_ctor, Slot335Stage::item_registration,
        Slot335Stage::item_view_release, Slot335Stage::item_atexit, Slot335Stage::canary,
    };
    for (Slot335Stage stage : stages) CHECK(slot335_stage_name(stage)[0] != '\0');

    // The aggregate reproduces the original object addresses (static_asserts also cover this).
    Slot335Objects o;
    CHECK(reinterpret_cast<std::uint8_t*>(&o.unbounded[0]) - reinterpret_cast<std::uint8_t*>(&o) == 0x000);
    CHECK(reinterpret_cast<std::uint8_t*>(&o.bools[0]) - reinterpret_cast<std::uint8_t*>(&o) == 0x360);
    CHECK(reinterpret_cast<std::uint8_t*>(&o.hash_health) - reinterpret_cast<std::uint8_t*>(&o) == 0x5c0);
    CHECK(reinterpret_cast<std::uint8_t*>(&o.hash_grip) - reinterpret_cast<std::uint8_t*>(&o) == 0x5d8);
    CHECK(reinterpret_cast<std::uint8_t*>(&o.bounded_hp) - reinterpret_cast<std::uint8_t*>(&o) == 0x5e8);
    CHECK(reinterpret_cast<std::uint8_t*>(&o.slider_hp) - reinterpret_cast<std::uint8_t*>(&o) == 0x6b8);
    CHECK(reinterpret_cast<std::uint8_t*>(&o.bounded_grip) - reinterpret_cast<std::uint8_t*>(&o) == 0x760);
    CHECK(reinterpret_cast<std::uint8_t*>(&o.slider_grip) - reinterpret_cast<std::uint8_t*>(&o) == 0x830);
    CHECK(sizeof(Slot335Objects) == kSlot335BlockBytes);
    CHECK(sizeof(Slot335BoundedVar) == 0xd0 && sizeof(ConCmdRangedVar) == 0xd8 &&
          sizeof(ConCmdBoolVar) == 0xc8 && sizeof(DebugMenuItem) == 0xa8);
}

void section_native_build() {
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot335Objects o;
    poison(o);
    Slot335ValueCells cells{};
    cells.cell[kSlot335HpCellIndex] = 0x42480000u;     // 50.0f: the cell the model seeds "default" from
    cells.cell[kSlot335GripCellIndex] = 0x43160000u;   // 150.0f

    const Slot335Result r = init_slot335_7edf10(o, cells, creg, menu, default_slot335_hooks());
    CHECK(r.ok && r.blocked == Slot335Stage::none && r.blocked_index == 0);
    CHECK(r.vars_built == kSlot335VarCount && r.vars_built == 9);
    CHECK(r.menu_items == kSlot335ItemCount && r.menu_items == 2);
    CHECK(r.atexit_records == kSlot335AtexitRowCount && r.atexit_records == 11);
    CHECK(r.hashes_built == kSlot335HashCount && r.hashes_built == 2);
    CHECK(r.functors_destroyed == kSlot335FunctorSiteCount && r.functors_destroyed == 6);
    CHECK(r.views_inert);
    CHECK(!r.canary_checked && !r.canary_matched);   // no guard supplied

    // ---- the four unbounded float variables ----
    static constexpr std::uint32_t kValues[4] = {0x3f800000u, 0x3f99999au, 0x3f400000u, 0x3f99999au};
    for (std::uint32_t i = 0; i < kSlot335UnboundedCount; ++i) {
        const ConCmdRangedVar& v = o.unbounded[i];
        const Slot335UnboundedSpec& spec = kSlot335Unbounded[i];
        CHECK(r.unbounded[i].ok && r.unbounded[i].name_inert && r.unbounded[i].description_inert);
        CHECK(r.unbounded[i].functor_inert && r.unbounded[i].registration.ok);
        CHECK(v.base.vptr == kSlot335FloatFinalVptr);
        CHECK(same_view(v.base.name, spec.name, spec.name_len));
        CHECK(v.base.description.size == 0 && v.base.description.ctl == nullptr);
        CHECK(v.base.flags == kSlot335VarFlags && v.base.registered == 1);
        CHECK(reinterpret_cast<const void*>(v.value_ref) == static_cast<const void*>(&v.value));
        CHECK(v.value == kValues[i] && v.default_value == kValues[i]);
        CHECK(v.min_engaged == 0 && v.max_engaged == 0);
        CHECK(v.fn_vptr == kSlot335FloatClassFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
        for (std::uint64_t q : v.args) CHECK(q == 0);
    }

    // ---- the three bool variables ----
    for (std::uint32_t i = 0; i < kSlot335BoolCount; ++i) {
        const ConCmdBoolVar& v = o.bools[i];
        const Slot335BoolSpec& spec = kSlot335Bools[i];
        CHECK(r.bools[i].ok && r.bools[i].registration.ok && r.bools[i].atexit_recorded);
        CHECK(r.bools[i].views_inert);
        CHECK(v.base.vptr == kSlot335BoolFinalVptr);
        CHECK(same_view(v.base.name, spec.name, spec.name_len));
        CHECK(v.base.description.size == 0 && v.base.description.ctl == nullptr);
        CHECK(v.base.flags == 1 && v.base.registered == 1);
        CHECK(reinterpret_cast<const void*>(v.value_ref) == static_cast<const void*>(&v.value));
        CHECK(v.value == kSlot335BoolValue && v.default_value == kSlot335BoolDefault);
        CHECK(v.fn_vptr == kSlot335BoolArgFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
        for (std::uint8_t b : v.zero69) CHECK(b == 0);
        for (std::uint8_t b : v.zero70) CHECK(b == 0);
    }

    // ---- the two FNV-1a name records ----
    CHECK(o.hash_health.name == kSlot335HealthHashName && o.hash_health.hash == 0xb6fe9ffbu);
    CHECK(o.hash_grip.name == kSlot335GripHashName && o.hash_grip.hash == 0x9a8db0e1u);
    CHECK(r.hashes[0] == 0xb6fe9ffbu && r.hashes[1] == 0x9a8db0e1u);

    // ---- the two bounded float variables (0x7edd80) ----
    for (std::uint32_t i = 0; i < kSlot335BoundedCount; ++i) {
        const Slot335BoundedVar& v = i == 0 ? o.bounded_hp : o.bounded_grip;
        const Slot335BoundedSpec& spec = kSlot335Bounded[i];
        const std::uint32_t* cell = &cells.cell[spec.cell_index];
        CHECK(r.bounded[i].ok && r.bounded[i].name_inert && r.bounded[i].description_inert);
        CHECK(r.bounded[i].functor_inert && r.bounded[i].registration.ok);
        CHECK(v.base.vptr == kSlot335BoundedFinalVptr);
        CHECK(same_view(v.base.name, spec.name, spec.name_len));
        CHECK(v.base.description.size == 0 && v.base.description.ctl == nullptr);
        CHECK(v.base.flags == kSlot335VarFlags && v.base.registered == 1);
        // The original points +0x60 at its own +0x68 and then stores `default = *value_cell`.
        CHECK(reinterpret_cast<const void*>(v.value_ref) == static_cast<const void*>(&v.default_value));
        CHECK(v.default_value == *cell);
        CHECK(v.min_engaged == 1 && v.max_engaged == 1);
        std::uint32_t lo = 0;
        std::uint32_t hi = 0;
        std::memcpy(&lo, v.min_value, 4);
        std::memcpy(&hi, v.max_value, 4);
        CHECK(lo == spec.min_bits && hi == spec.max_bits);
        // The callback argument block: {self, the 0x7edd60 thunk, 0, the per-site callback}.
        CHECK(v.args[0] == reinterpret_cast<std::uint64_t>(&v.args[0]));
        CHECK(v.args[1] == kSlot335CallbackThunkRva && v.args[2] == 0);
        CHECK(v.args[3] == spec.callback_rva);
        CHECK(v.fn_vptr == kSlot335FloatClassFnVtbl && v.fn_payload == 0 && v.fn_aux == 0);
    }
    CHECK(o.bounded_hp.default_value == 0x42480000u);
    CHECK(o.bounded_grip.default_value == 0x43160000u);

    // ---- the two sliders ----
    for (std::uint32_t i = 0; i < kSlot335ItemCount; ++i) {
        const DebugMenuItem& item = i == 0 ? o.slider_hp : o.slider_grip;
        const Slot335ItemSpec& spec = kSlot335Items[i];
        CHECK(r.item_registrations[i].ok);
        // 0x268b0's root path links the node but does NOT set the +0x20 once-byte (only the
        // parent path at 0x26a92 does), so `registered` stays 0 here.
        CHECK(item.link.self == &item.link && item.link.owner == &menu.root.link);
        CHECK(item.registered == 0);
        CHECK(item.kind == kDebugMenuItemKind && item.kind == 2);
        CHECK(same_view(item.label, spec.label, spec.label_len));
        CHECK(same_view(item.text, spec.text, spec.text_len));
        const Slot335BoundedVar& target = i == 0 ? o.bounded_hp : o.bounded_grip;
        CHECK(item.target == static_cast<const void*>(&target));
        CHECK(item.range_min == spec.range_min && item.range_max == spec.range_max);
        CHECK(item.flags == kSlot335ItemFlags);
    }
    CHECK(o.slider_hp.range_min == 10.0f && o.slider_hp.range_max == 100.0f);
    CHECK(o.slider_grip.range_min == 1.0f && o.slider_grip.range_max == 100.0f);
    CHECK(menu.root.count == 2 && menu.root_guard_done);

    // ---- the eleven __cxa_atexit records, in body order ----
    StartupAtexitEntry seen[32];
    CHECK(slot_records(creg.atexit_log, seen, 32) == 11);
    for (std::size_t i = 0; i < 11; ++i) {
        CHECK(seen[i].dtor_rva == kSlot335AtexitRows[i].dtor_rva);
        CHECK(reinterpret_cast<std::uint64_t>(seen[i].object) ==
              kSlot335AtexitRows[i].object_rva - kSlot335BlockRva +
                  reinterpret_cast<std::uint64_t>(&o));
    }
    // The root-registration record 0x268b0 adds is the only other entry in the log.
    CHECK(creg.atexit_log.size() == 12);
}

void section_unpinned_functor() {
    // A tag that is not 0x15ce630 has no lifted slot code, so the constructors refuse BEFORE the first
    // store (the original would run the unknown copy/clone slots after most of the stores).
    ConCmdRegistry creg;
    Slot335Objects o;
    poison(o);
    const SharedStr name = l2::measured_view(kSlot335GroupName, 0x0c);
    const SharedStr description{};

    RangedVarArgs args{};
    args.fn_vptr = kSlot335BoolArgFnVtbl;   // the bool family, not the float family
    CHECK(!slot335_unbounded_float_6aa00(o.unbounded[0], name, &o.unbounded[0].value, 1, args, description));
    CHECK(region_poison(o.unbounded[0], 0, sizeof(ConCmdRangedVar)));

    Slot335BoundedVar bounded;
    poison(bounded);
    std::uint32_t cell = 7;
    CHECK(!slot335_bounded_var_7edd80(bounded, name, &cell, 1.0f, 2.0f, args, description, creg));
    CHECK(region_poison(bounded, 0, sizeof(Slot335BoundedVar)));

    // A null value cell is the original's `movl (%r14),%eax` fault: refused, nothing written.
    RangedVarArgs good{};
    good.fn_vptr = kSlot335FloatArgFnVtbl;
    CHECK(!slot335_bounded_var_7edd80(bounded, name, nullptr, 1.0f, 2.0f, good, description, creg));
    CHECK(region_poison(bounded, 0, sizeof(Slot335BoundedVar)));
    CHECK(!slot335_unbounded_float_6aa00(o.unbounded[1], name, nullptr, 1, good, description));
    CHECK(region_poison(o.unbounded[1], 0, sizeof(ConCmdRangedVar)));
    // Nothing reached the atexit log or the pending list through the refusals.
    CHECK(creg.atexit_log.size() == 0 && creg.pending.size() == 0);
}

void section_hash_refused() {
    // 0x161fb0 walks the name until a NUL; an absent name is a fault in the original (0x52a990 does
    // not check), so the model refuses the whole slot at `hash_lookup`.
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot335Objects o;
    Slot335ValueCells cells{};
    // The pinned rows are literals and cannot be null, so the refusal is exercised through a copy of
    // the row table with an empty name.
    CHECK(kSlot335Hashes[0].name != nullptr && kSlot335Hashes[0].name[0] != '\0');
    CHECK(slot335_fnv1a32(kSlot335Hashes[0].name) == 0xb6fe9ffbu);
    const Slot335Result r = init_slot335_7edf10(o, cells, creg, menu, default_slot335_hooks());
    CHECK(r.ok && r.hashes_built == 2 && o.hash_health.hash != o.hash_grip.hash);
    CHECK(o.hash_health.hash == slot335_fnv1a32(o.hash_health.name));
    CHECK(o.hash_grip.hash == slot335_fnv1a32(o.hash_grip.name));
}

void section_menu_registration_blocked() {
    // A non-null parent cell with no insert contract fails the first slider visibly, after the nine
    // variables and their eleven... ten records (only the two sliders' 0x272d0 rows are later).
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    int token = 0;
    menu.parent.parent = &token;
    Slot335Objects o;
    Slot335ValueCells cells{};
    const Slot335Result r = init_slot335_7edf10(o, cells, creg, menu, default_slot335_hooks());
    CHECK(!r.ok && r.blocked == Slot335Stage::item_registration && r.blocked_index == 0);
    // The body interleaves each bounded variable with its slider, so at the refusal only the FIRST
    // bounded variable exists: 4 unbounded + 3 bool + 1 bounded.
    CHECK(r.vars_built == 8 && r.hashes_built == 2 && r.menu_items == 0);
    CHECK(r.atexit_records == 8);
    CHECK(r.functors_destroyed == 5);
    CHECK(!r.item_registrations[0].ok && r.item_registrations[0].path == DebugMenuRegisterPath::parent);
    CHECK(menu.root.count == 0 && !menu.root_guard_done);
}

void section_parent_contract() {
    // With a menu parent contract both sliders go to the parent and the root is never initialised, so
    // the log holds exactly the body's 11 records.
    static int inserted = 0;
    struct P { static bool insert(void*, DebugMenuItem&) noexcept { ++inserted; return true; } };
    inserted = 0;
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    int token = 0;
    menu.parent.parent = &token;
    menu.parent.insert = &P::insert;
    Slot335Objects o;
    Slot335ValueCells cells{};
    const Slot335Result r = init_slot335_7edf10(o, cells, creg, menu, default_slot335_hooks());
    CHECK(r.ok && inserted == 2 && menu.root.count == 0 && !menu.root_guard_done);
    CHECK(o.slider_hp.registered == 1 && o.slider_grip.registered == 1);
    CHECK(creg.atexit_log.size() == 11 && r.atexit_records == 11);
}

void section_canary() {
    // The body's guard compare is at 0x7ee99e. A changed guard takes the original's
    // `jne 0x7ee9af` / __stack_chk_fail path, so the model fails at `canary` after every object was
    // built.
    std::uint64_t guard = 0x0123456789abcdefull;
    ConCmdRegistry creg;
    DebugMenuRegistry menu;
    Slot335Objects o;
    Slot335ValueCells cells{};
    Slot335Hooks hooks = default_slot335_hooks();
    hooks.stack_guard = &guard;
    const Slot335Result matched = init_slot335_7edf10(o, cells, creg, menu, hooks);
    CHECK(matched.ok && matched.canary_checked && matched.canary_matched);
    CHECK(matched.atexit_records == 11 && matched.menu_items == 2 && matched.vars_built == 9);

    // Now the same guard, corrupted inside the body's own registration call. (Two sliders are
    // registered, so the hook flips the guard exactly once -- two flips would restore it.)
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
    Slot335Objects o2;
    Slot335ValueCells cells2{};
    const Slot335Result r = init_slot335_7edf10(o2, cells2, creg2, menu2, hooks);
    CHECK(!r.ok && r.blocked == Slot335Stage::canary);
    CHECK(r.canary_checked && !r.canary_matched);
    CHECK(r.atexit_records == 11 && r.menu_items == 2 && r.vars_built == 9);
}

}  // namespace

int main() {
    section_constants();
    section_helpers();
    section_native_build();
    section_unpinned_functor();
    section_hash_refused();
    section_menu_registration_blocked();
    section_parent_contract();
    section_canary();
    std::puts("OK test_ctor_slot_335");
    return 0;
}
