// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 3): fixture for startup constructor index 518 (0x1a57d0, the
// `vgAmbientSoundSpacing` float variable plus the bounded static-float tail).
//
// Two things are the point of this fixture:
//   * the variable is a data row of the shared debug-page model, with the lane-M6 tag 0x1598a50;
//   * the tail writes eleven dwords into the cells that directly follow the variable, derived from the
//     file-backed 8.5f cache at 0x16048a0 with single-precision multiply / add / divide, and it is NOT
//     reached by a block that stopped before the variable's atexit record.
// Byte-for-byte pinning: tools/lift/check_ctor_slot_518.py.
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_518_1a57d0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;

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

std::size_t body_records(const StartupAtexitLog& log, StartupAtexitEntry* out, std::size_t cap) {
    std::size_t n = 0;
    for (std::size_t i = 0; i < log.size(); ++i) {
        const StartupAtexitEntry& e = log.at(i);
        if (e.dtor_rva == kDebugMenuRootDtorRva) continue;
        if (n < cap) out[n] = e;
        ++n;
    }
    return n;
}

float f32(std::uint32_t bits) { return std::bit_cast<float>(bits); }

void section_constants() {
    CHECK(kSlot518Rva == 0x1a57d0 && kSlot518BodySize == 527 && kSlot518EndRva == 0x1a59df);
    CHECK(kSlot518InstructionCount == 103 && kSlot518DirectCallSites == 6 && kSlot518DirectCallees == 5);
    CHECK(kSlot518FunctorSites == 1 && kSlot518BranchCount == 1);
    CHECK(kSlot518TableTopRva - 8 * kSlot518TableIndex == kSlot518TableWordRva && kSlot518TableWordRva == 0x2dd60b0);
    CHECK(kSlot518BranchRva == 0x1a59c8 && kSlot518BranchTargetRva == 0x1a59d9);

    CHECK(kSlot518Vars.size() == 1 && kSlot518Script.items.empty() && kSlot518Script.hashed_names.empty());
    const debug_page::FloatVarRow& v = kSlot518Vars[0];
    CHECK(v.object_rva == 0x2ee65e8 && v.object_rva + debug_page::kVarStride == kSlot518TailBaseRva);
    CHECK(std::strcmp(v.name, "vgAmbientSoundSpacing") == 0 && v.name_len == 0x15);
    CHECK(std::strcmp(v.description, "") == 0 && v.description_len == 0x00);
    CHECK(v.lo_bits == 0x3f800000u && v.hi_bits == 0x41a00000u && v.value_bits == 0x40e00000u);
    CHECK(f32(v.lo_bits) == 1.0f && f32(v.hi_bits) == 20.0f && f32(v.value_bits) == 7.0f);
    CHECK(v.arg_fn_vtbl == 0x1598a50 && debug_page::is_batch_float_site_tag(v.arg_fn_vtbl));
    CHECK(debug_page::kArgFnVtblSlot518 == 0x1598a50);
    CHECK(v.ctor_site_rva == 0x1a58c1 && v.functor_site_rva == 0x1a58dd && v.register_site_rva == 0x1a58f7 &&
          v.atexit_site_rva == 0x1a5915);

    // The tail's pinned results are the single-precision values of the pinned arithmetic.
    CHECK(f32(kSlot518CacheDefaultBits) == 8.5f);
    CHECK(f32(kSlot518TailE0Bits) == 4.25f);
    const float e4 = (f32(kSlot518TailE0Bits) + f32(kSlot518TailE0Bits)) * f32(kSlot518TailMulScaleBits);
    CHECK(std::bit_cast<std::uint32_t>(e4) == kSlot518TailE4Bits);
    CHECK(std::bit_cast<std::uint32_t>(1.0f / e4) == kSlot518TailE8Bits);

    CHECK(sizeof(ConCmdRangedVar) == 0xd8 && sizeof(AmbientSpacingBasis) == 0x2c);
    CHECK(offsetof(Slot518Objects, var) == 0 && offsetof(Slot518Objects, basis) == 0xd8);
}

void section_native_build() {
    Slot518Objects objects;
    poison(objects.var);
    poison(objects.basis);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    const Slot518Result r = init_slot518_1a57d0(objects, concmd, menu);

    CHECK(r.ok && r.tail_written && r.page.ok && r.page.blocked == debug_page::Stage::none);
    CHECK(r.page.vars_built == 1 && r.page.items_built == 0 && r.page.hashed_names_built == 0);
    CHECK(r.page.atexit_records == 1 && r.page.functors_destroyed == 1);
    CHECK(r.page.vars[0].ok && !r.page.canary_checked);

    const ConCmdRangedVar& var = objects.var;
    CHECK(var.base.vptr == 0x15942b0 && var.base.registered == 1 && var.base.flags == 1);
    CHECK(var.value_ref == &var.value && var.min_engaged == 1 && var.max_engaged == 1);
    std::uint32_t lo = 0, hi = 0;
    std::memcpy(&lo, var.min_value, 4);
    std::memcpy(&hi, var.max_value, 4);
    CHECK(lo == 0x3f800000u && hi == 0x41a00000u);
    CHECK(var.value == 0x40e00000u && var.default_value == 0x40e00000u);
    CHECK(var.fn_vptr == kRangedFloatFnVtbl);
    for (std::uint64_t q : var.args) CHECK(q == 0);
    CHECK(same_view(var.base.name, "vgAmbientSoundSpacing", 0x15));
    CHECK(same_view(var.base.description, "", 0));

    // The tail: every dword at its original offset from 0x2ee66c0.
    const AmbientSpacingBasis& b = objects.basis;
    CHECK(b.pair0[0] == 0x3f800000u && b.pair0[1] == 0x3f000000u);
    CHECK(b.pair1[0] == 0x00000000u && b.pair1[1] == 0x3f5db22du);
    CHECK(b.pair2[0] == 0x3f800000u && b.pair2[1] == 0xbf13ce56u);
    CHECK(b.pair3[0] == 0x00000000u && b.pair3[1] == 0x3f93ce56u);
    CHECK(b.spacing == kSlot518TailE0Bits && b.pitch == kSlot518TailE4Bits && b.inverse == kSlot518TailE8Bits);
    CHECK(f32(b.spacing) == 4.25f);

    StartupAtexitEntry records[4];
    const std::size_t n = body_records(concmd.atexit_log, records, 4);
    CHECK(n == 1 && records[0].dtor_rva == 0x20f0 && records[0].object == &objects.var);
    CHECK(concmd.pending.size() == 1);
}

void section_tail_follows_the_cache() {
    // The tail reads the cache field, so a different cache gives the independently computed results.
    AmbientSpacingBasis b;
    poison(b);
    apply_ambient_spacing_tail(b, 0x40000000u);   // 2.0f
    CHECK(f32(b.spacing) == 1.0f);
    const float pitch = 2.0f * 0.866f;
    CHECK(b.pitch == std::bit_cast<std::uint32_t>(pitch));
    CHECK(b.inverse == std::bit_cast<std::uint32_t>(1.0f / pitch));
    // The literal pairs do not depend on the cache.
    CHECK(b.pair0[1] == 0x3f000000u && b.pair3[1] == 0x3f93ce56u);

    Slot518Objects objects;
    objects.cache_bits = 0x40000000u;
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    const Slot518Result r = init_slot518_1a57d0(objects, concmd, menu);
    CHECK(r.ok && f32(objects.basis.spacing) == 1.0f);
    CHECK(Slot518Objects{}.cache_bits == 0x41080000u);   // the ELF default
}

void section_shape_refused() {
    // A script whose stand-in span is too small is refused before ANY write, and the tail stays untouched.
    Slot518Objects objects;
    poison(objects.var);
    poison(objects.basis);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    const debug_page::Objects view{std::span<ConCmdRangedVar>(), std::span<DebugMenuItem>(),
                                   std::span<NameHashCell>()};
    const debug_page::Result r = debug_page::init_debug_page(kSlot518Script, view, concmd, menu, {});
    CHECK(!r.ok && r.blocked == debug_page::Stage::shape && r.atexit_records == 0);
    CHECK(region_poison(objects.var, 0, sizeof objects.var) && region_poison(objects.basis, 0, sizeof objects.basis));
}

void section_register_refused() {
    // 0x20d60 refused: the original stops there, so neither the atexit record nor the tail exists.
    Slot518Objects objects;
    poison(objects.basis);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    int manager_token = 0;
    concmd.manager.manager = &manager_token;
    const Slot518Result r = init_slot518_1a57d0(objects, concmd, menu);
    CHECK(!r.ok && !r.tail_written && r.page.blocked == debug_page::Stage::var_register);
    CHECK(r.page.vars_built == 0 && r.page.atexit_records == 0);
    CHECK(region_poison(objects.basis, 0, sizeof objects.basis));
}

void section_unpinned_tag_refused() {
    // An unpinned caller tag refuses the 0xc300 block; nothing after it, including the tail, is claimed.
    Slot518Objects objects;
    poison(objects.var);
    poison(objects.basis);
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    debug_page::FloatVarRow bad = kSlot518Vars[0];
    bad.arg_fn_vtbl = 0x15dead0;
    const debug_page::FloatVarRow rows[1] = {bad};
    debug_page::Script script = kSlot518Script;
    script.vars = std::span<const debug_page::FloatVarRow>(rows, 1);
    const debug_page::Objects view{std::span<ConCmdRangedVar>(&objects.var, 1), std::span<DebugMenuItem>(),
                                   std::span<NameHashCell>()};
    const debug_page::Result r = debug_page::init_debug_page(script, view, concmd, menu, {});
    CHECK(!r.ok && r.blocked == debug_page::Stage::var_ctor && r.vars_built == 0);
    CHECK(region_poison(objects.var, 0, sizeof objects.var) && region_poison(objects.basis, 0, sizeof objects.basis));
}

void section_canary() {
    std::uint64_t guard = 0x13579bdf2468ace0ull;
    Slot518Objects objects;
    ConCmdRegistry concmd;
    DebugMenuRegistry menu;
    const Slot518Result r = init_slot518_1a57d0(objects, concmd, menu, {&guard});
    CHECK(r.ok && r.page.canary_checked && r.page.canary_matched && r.tail_written);
}

}  // namespace

int main() {
    section_constants();
    section_native_build();
    section_tail_follows_the_cache();
    section_shape_refused();
    section_register_refused();
    section_unpinned_tag_refused();
    section_canary();
    std::printf("OK test_ctor_slot_518\n");
    return 0;
}
