// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01: fixture for startup constructor index 536 (0x1342d0, the
// `r_probePreviewRoughness` / `r_probePreviewMetalness` float variables and NO menu item).
//
// This is the "n variables, no item" member of the cluster: two complete 0xc300 blocks back to back, two
// __cxa_atexit records and no 0x272d0 record. Fail-visible: an unpinned caller tag in either row, a script
// that does not fit the stand-ins, a refused 0x20d60 on the first row (which must stop before the second)
// and a changed __stack_chk_guard (the guard-flipping contract is installed on a registry the body never
// reaches a menu through, so it is exercised through the direct model call instead).
// Byte-for-byte pinning: tools/lift/check_ctor_slot_536.py.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_536_1342d0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;

// The guard cell the canary section corrupts from inside the ConCmd manager contract.
std::uint64_t* guard_cell = nullptr;

bool manager_insert_flips_guard(void* manager, ConCmdBase& cmd) noexcept {
    (void)manager;
    (void)cmd;
    if (guard_cell != nullptr) {
        *guard_cell ^= 1u;
        guard_cell = nullptr;   // flip exactly once (a second registration must not undo it)
    }
    return true;
}

template <class T>
void poison(T& obj) { std::memset(static_cast<void*>(&obj), kPoison, sizeof obj); }

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

void section_constants() {
    CHECK(kSlot536Rva == 0x1342d0 && kSlot536BodySize == 659 && kSlot536EndRva == 0x134563);
    CHECK(kSlot536InstructionCount == 124 && kSlot536DirectCallSites == 11 && kSlot536DirectCallees == 5);
    CHECK(kSlot536FunctorSites == 2 && kSlot536BranchCount == 1);
    CHECK(kSlot536TableTopRva - 8 * kSlot536TableIndex == kSlot536TableWordRva && kSlot536TableWordRva == 0x2dd6020);
    CHECK(kSlot536BranchRva == 0x13454c && kSlot536BranchTargetRva == 0x13455d);

    CHECK(kSlot536Vars.size() == 2 && kSlot536Script.items.empty());
    const debug_page::FloatVarRow& v1 = kSlot536Vars[0];
    const debug_page::FloatVarRow& v2 = kSlot536Vars[1];
    CHECK(v1.object_rva == 0x2ee1340 && v1.object_rva + debug_page::kVarStride == v2.object_rva);
    CHECK(v2.object_rva == 0x2ee1418);
    CHECK(std::strcmp(v1.name, "r_probePreviewRoughness") == 0 && v1.name_len == 0x17);
    CHECK(std::strcmp(v2.name, "r_probePreviewMetalness") == 0 && v2.name_len == 0x17);
    CHECK(std::strcmp(v1.description, "") == 0 && v1.description_len == 0 && v2.description_len == 0);
    CHECK(v1.lo_bits == 0x00000000u && v1.hi_bits == 0x3f800000u && v1.value_bits == 0x3f000000u);
    CHECK(v2.lo_bits == 0x00000000u && v2.hi_bits == 0x3f800000u && v2.value_bits == 0x00000000u);
    CHECK(v1.arg_fn_vtbl == 0x15981a0 && v2.arg_fn_vtbl == 0x15981a0);
    CHECK(debug_page::is_batch_float_site_tag(v1.arg_fn_vtbl));
    CHECK(v1.ctor_site_rva == 0x1343c1 && v1.functor_site_rva == 0x1343e2 && v1.register_site_rva == 0x134403 &&
          v1.atexit_site_rva == 0x134421);
    CHECK(v2.ctor_site_rva == 0x1344da && v2.functor_site_rva == 0x1344f6 && v2.register_site_rva == 0x134517 &&
          v2.atexit_site_rva == 0x134535);
    CHECK(offsetof(Slot536Objects, var2) == 0xd8 && sizeof(Slot536Objects) == 0x1b0);
}

void section_native_build() {
    Slot536Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    const debug_page::Result r = init_slot536_1342d0(objects, concmd, menu);

    CHECK(r.ok && r.blocked == debug_page::Stage::none);
    CHECK(r.vars_built == 2 && r.items_built == 0 && r.atexit_records == 2 && r.functors_destroyed == 2);
    CHECK(r.vars[0].ok && r.vars[1].ok && !r.canary_checked);

    const ConCmdRangedVar& v1 = objects.var1;
    const ConCmdRangedVar& v2 = objects.var2;
    CHECK(v1.base.vptr == 0x15942b0 && v2.base.vptr == 0x15942b0);
    CHECK(v1.base.registered == 1 && v2.base.registered == 1 && v1.base.flags == 1 && v2.base.flags == 1);
    CHECK(v1.value_ref == &v1.value && v2.value_ref == &v2.value);
    CHECK(v1.min_engaged == 1 && v1.max_engaged == 1 && v2.min_engaged == 1 && v2.max_engaged == 1);
    CHECK(v1.value == 0x3f000000u && v1.default_value == 0x3f000000u);   // 0.5f
    CHECK(v2.value == 0x00000000u && v2.default_value == 0x00000000u);   // 0.0f
    CHECK(v1.fn_vptr == kRangedFloatFnVtbl && v2.fn_vptr == kRangedFloatFnVtbl);
    for (std::uint64_t q : v1.args) CHECK(q == 0);
    for (std::uint64_t q : v2.args) CHECK(q == 0);
    CHECK(same_view(v1.base.name, "r_probePreviewRoughness", 0x17));
    CHECK(same_view(v2.base.name, "r_probePreviewMetalness", 0x17));
    CHECK(same_view(v1.base.description, "", 0) && same_view(v2.base.description, "", 0));

    // Exactly two records, both 0x20f0, in body order; no menu item and no root record.
    CHECK(concmd.atexit_log.size() == 2);
    CHECK(concmd.atexit_log.at(0).dtor_rva == 0x20f0 && concmd.atexit_log.at(0).object == &objects.var1);
    CHECK(concmd.atexit_log.at(1).dtor_rva == 0x20f0 && concmd.atexit_log.at(1).object == &objects.var2);
    CHECK(concmd.pending.size() == 2);
    CHECK(menu.root.count == 0 && !menu.root_guard_done);
}

void section_unpinned_tag_in_second_row() {
    // The FIRST row is correct; the SECOND names a tag nobody proved. The first row's effects stand and
    // the second is refused before any of its stores, which is exactly the original's stop-at-failure shape.
    Slot536Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;

    debug_page::FloatVarRow bad = kSlot536Vars[1];
    bad.arg_fn_vtbl = 0x15dead0;
    const debug_page::FloatVarRow rows[2] = {kSlot536Vars[0], bad};
    debug_page::Script script = kSlot536Script;
    script.vars = std::span<const debug_page::FloatVarRow>(rows, 2);

    const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var1, 2), std::span<DebugMenuItem>()};
    const debug_page::Result r = debug_page::init_debug_page(script, view, concmd, menu, {});
    CHECK(!r.ok && r.blocked == debug_page::Stage::var_ctor && r.blocked_index == 1);
    CHECK(r.vars_built == 1 && r.atexit_records == 1 && concmd.atexit_log.size() == 1);
    CHECK(objects.var1.base.registered == 1);   // the first row completed
    CHECK(region_poison(objects.var2, 0, sizeof objects.var2));
}

void section_shape_refused() {
    Slot536Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;

    // Two rows against a one-object span.
    const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var1, 1), std::span<DebugMenuItem>()};
    const debug_page::Result r = debug_page::init_debug_page(kSlot536Script, view, concmd, menu, {});
    CHECK(!r.ok && r.blocked == debug_page::Stage::shape && r.atexit_records == 0);
    CHECK(region_poison(objects, 0, sizeof objects));

    // Three variable rows exceed the batch's declared maximum even when the span is large enough.
    Slot536Objects big;
    poison(big);
    const debug_page::FloatVarRow rows[3] = {kSlot536Vars[0], kSlot536Vars[1], kSlot536Vars[0]};
    debug_page::Script script = kSlot536Script;
    script.vars = std::span<const debug_page::FloatVarRow>(rows, 3);
    const debug_page::Objects view3{std::span<ConCmdRangedVar>(&big.var1, 2), std::span<DebugMenuItem>()};
    const debug_page::Result r3 = debug_page::init_debug_page(script, view3, concmd, menu, {});
    CHECK(!r3.ok && r3.blocked == debug_page::Stage::shape);
    CHECK(region_poison(big, 0, sizeof big));
}

void section_register_refused() {
    Slot536Objects objects;
    poison(objects);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    int manager_token = 0;
    concmd.manager.manager = &manager_token;

    const debug_page::Result r = init_slot536_1342d0(objects, concmd, menu);
    CHECK(!r.ok && r.blocked == debug_page::Stage::var_register && r.blocked_index == 0);
    CHECK(r.vars_built == 0 && r.atexit_records == 0 && concmd.pending.size() == 0);
    CHECK(region_poison(objects.var2, 0, sizeof objects.var2));   // the second row never ran
}

void section_canary() {
    std::uint64_t guard = 0x1111222233334444ull;
    {
        Slot536Objects objects;
        ConCmdRegistry concmd;
        DebugMenuRegistry menu;
        const debug_page::Result r = init_slot536_1342d0(objects, concmd, menu, {&guard});
        CHECK(r.ok && r.canary_checked && r.canary_matched);
    }
    {
        // This body has no menu item, so the only injectable mid-flight hook is the ConCmd manager insert.
        // It flips the guard cell while the first row registers; the epilogue compare must then fail.
        Slot536Objects objects;
        ConCmdRegistry concmd;
        DebugMenuRegistry menu;
        int manager_token = 0;
        guard_cell = &guard;
        concmd.manager.manager = &manager_token;
        concmd.manager.insert = &manager_insert_flips_guard;
        const debug_page::Result r = init_slot536_1342d0(objects, concmd, menu, {&guard});
        guard_cell = nullptr;
        CHECK(!r.ok && r.blocked == debug_page::Stage::canary && !r.canary_matched);
        // The manager path succeeded, so both rows and both records exist; only the guard stopped it.
        CHECK(r.vars_built == 2 && r.atexit_records == 2);
    }
}

}  // namespace

int main() {
    section_constants();
    section_native_build();
    section_unpinned_tag_in_second_row();
    section_shape_refused();
    section_register_refused();
    section_canary();
    std::printf("OK test_ctor_slot_536\n");
    return 0;
}
