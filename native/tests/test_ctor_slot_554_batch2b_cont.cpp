// Fixture for slot 554 batch 2b continuation: the remaining 23 objects in [0x7036d,0x718bb).
// Checks the exact per-object masks, names/pages, wrapper pins, fail-closed hooks and replay.
#include "core/ctor_slot_554_2b_cont_7036d.hpp"

#include <array>
#include <cstdio>
#include <cstring>

namespace {
using namespace sotc;
int failures = 0;
constexpr std::uint8_t kSentinel = 0x5a;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++failures; } } while (0)

bool refuse_register(ConCmdBase&, ConCmdRegistry&) noexcept { return false; }
bool no_link_register(ConCmdBase&, ConCmdRegistry&) noexcept { return true; }
bool refuse_atexit(std::uint64_t, const void*, ConCmdRegistry&) noexcept { return false; }
bool refuse_release(const SharedStr&) noexcept { return false; }
bool refuse_transfer(void*, ConCmdFunctor&, unsigned) noexcept { return false; }
bool refuse_destroy(void*, const ConCmdFunctorView&) noexcept { return false; }

struct Counts { unsigned transfer = 0; unsigned destroy = 0; };
bool count_transfer(void* ctx, ConCmdFunctor&, unsigned) noexcept {
    ++static_cast<Counts*>(ctx)->transfer;
    return true;
}
bool count_destroy(void* ctx, const ConCmdFunctorView&) noexcept {
    ++static_cast<Counts*>(ctx)->destroy;
    return true;
}

std::array<std::uint8_t, 0xac8> make_page() {
    std::array<std::uint8_t, 0xac8> page{};
    page.fill(kSentinel);
    for (std::uint32_t i = 0; i < kSlot554B2BContObjectCount; ++i) {
        const auto& s = kSlot554B2BContSpecs[i];
        if (s.kind == Slot554B2BContKind::bool_var) page[s.page_cell_offset] = 1;
    }
    for (std::uint16_t off : {std::uint16_t{0x3c}, std::uint16_t{0x40}, std::uint16_t{0x44},
                              std::uint16_t{0x48}, std::uint16_t{0x0c}}) {
        const std::uint32_t v = 0x11223344u + off;
        std::memcpy(page.data() + off, &v, sizeof v);
    }
    return page;
}

std::uint8_t* object(Slot554B2BContObjects& o, std::uint32_t i) noexcept {
    std::size_t offset = 0;
    for (std::uint32_t n = 0; n < i; ++n) offset += kSlot554B2BContSpecs[n].size;
    return o.objects.data() + offset;
}

std::uint32_t u32(const std::uint8_t* p, std::size_t off) noexcept {
    std::uint32_t v = 0;
    std::memcpy(&v, p + off, sizeof v);
    return v;
}
std::uint64_t u64(const std::uint8_t* p, std::size_t off) noexcept {
    std::uint64_t v = 0;
    std::memcpy(&v, p + off, sizeof v);
    return v;
}

void check_constants() {
    CHECK(kSlot554B2BContRva == 0x7036d && kSlot554B2BContBodySize == 0x154e);
    CHECK(kSlot554B2BContObjectBaseRva == 0x2eaa9e0 && kSlot554B2BContObjectCount == 23);
    CHECK(kSlot554B2BContBlockSize == 0x11a0 && kSlot554B2BContWritten == 3870 &&
          kSlot554B2BContHoles == 642);
    CHECK(kSlot554B2BContInstructionCount == 955 && kSlot554B2BContDirectCallCount == 111 &&
          kSlot554B2BContIndirectCallCount == 6 && kSlot554B2BContStoreCount == 332 &&
          kSlot554B2BContObjectStoreCount == 157 && kSlot554B2BContStackStoreCount == 175);
    CHECK(kSlot554B2BContWrapperCount == 4 && kSlot554B2BContRangedCount == 2 &&
          kSlot554B2BContBoolCount == 17);
    CHECK(kSlot554B2BContTableIndex == 554 && kSlot554B2BContTableWordRva == 0x2dd5f90);
    CHECK(std::strcmp(kSlot554B2BContSpecs[22].name, "drawJustParticleBlockers") == 0);
    CHECK(kSlot554B2BContBoolFinalVptr == 0x1594348);
    CHECK(kSlot554B2BContFloatWritten == 171 && kSlot554B2BContFloatHoles == 37);
    CHECK(kSlot554B2BContSpecs[2].low == 0xf && kSlot554B2BContSpecs[2].high == 0xf);
    CHECK(kSlot554B2BContSpecs[3].final_vptr == 0x1596a18);
    CHECK(kSlot554B2BContSpecs[6].final_vptr == 0x1596ab0);
    CHECK(kSlot554B2BContSpecs[6].low == 0 && kSlot554B2BContSpecs[6].high == 0x1c);
    std::size_t written = 0;
    for (std::size_t i = 0; i < kSlot554B2BContBlockSize; ++i) if (slot554_b2b_cont_written(i)) ++written;
    CHECK(written == kSlot554B2BContWritten);
    std::size_t bool_written = 0;
    for (std::size_t i = 0; i < 0xc0; ++i) if (slot554_b2b_cont_object_written(4, i)) ++bool_written;
    CHECK(bool_written == kSlot554B2BContBoolWritten);
    std::size_t wrapper_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_b2b_cont_object_written(0, i)) ++wrapper_written;
    CHECK(wrapper_written == kSlot554B2BContWrapperWritten);
    std::size_t int_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_b2b_cont_object_written(1, i)) ++int_written;
    CHECK(int_written == kSlot554B2BContRangedWritten);
    std::size_t float_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_b2b_cont_object_written(11, i)) ++float_written;
    CHECK(float_written == kSlot554B2BContFloatWritten);
}

void check_objects() {
    const auto page = make_page();
    const auto page_before = page;
    Slot554B2BContObjects o{};
    std::memset(&o, kSentinel, sizeof o);
    ConCmdRegistry reg{};
    Counts counts{};
    Slot554B2BContHooks hooks = default_slot554_b2b_cont_hooks();
    hooks.transfer = &count_transfer;
    hooks.destroy_view = &count_destroy;
    hooks.context = &counts;
    const Slot554B2BContResult r = init_slot554_b2b_cont(o, page.data(), reg, hooks);
    CHECK(r.ok && r.blocked == Slot554B2BContStage::none);
    CHECK(r.built == 23 && r.registered == 23 && r.atexit_records == 23 &&
          r.views_inert == 46 && r.transfers == 20 && r.view_destroys == 4);
    CHECK(counts.transfer == 20 && counts.destroy == 4);
    CHECK(page == page_before);

    for (std::uint32_t i = 0; i < kSlot554B2BContObjectCount; ++i) {
        const auto& spec = kSlot554B2BContSpecs[i];
        const std::uint8_t* p = object(o, i);
        for (std::size_t j = 0; j < spec.size; ++j) {
            if (!slot554_b2b_cont_object_written(i, j)) CHECK(p[j] == kSentinel);
        }
        CHECK(u64(p, 0) == spec.final_vptr);
        const SharedStr& name = *reinterpret_cast<const SharedStr*>(p + 0x20);
        CHECK(name.size == spec.name_length);
        CHECK(std::memcmp(name.data, spec.name, name.size) == 0);
        if (spec.kind == Slot554B2BContKind::bool_var) {
            CHECK(u64(p, 0x60) == reinterpret_cast<std::uint64_t>(page.data() + spec.page_cell_offset));
            CHECK(p[0x68] == 1 && u32(p, 0x69) == 0);
            CHECK(u64(p, 0xa0) == 0 && u64(p, 0xb0) == kSlot554B2BContBoolFnVtbl && u64(p, 0xb8) == 0);
        } else if (spec.kind == Slot554B2BContKind::wrapper) {
            CHECK(u64(p, 0x60) == reinterpret_cast<std::uint64_t>(page.data() + spec.page_cell_offset));
            CHECK(u32(p, 0x68) == u32(page.data(), spec.page_cell_offset));
            CHECK(u32(p, 0x6c) == spec.low && p[0x70] == 1 && u32(p, 0x74) == spec.high && p[0x78] == 1);
            CHECK(u64(p, 0x80) == 0 && u64(p, 0x88) == 0 && u64(p, 0x90) == 0 && u64(p, 0x98) == 0);
            CHECK(u64(p, 0xb0) == 0 && u64(p, 0xc0) == spec.tag_vptr && u64(p, 0xc8) == 0);
        } else {
            CHECK(u64(p, 0x60) == reinterpret_cast<std::uint64_t>(page.data() + spec.page_cell_offset));
            CHECK(u32(p, 0x68) == u32(page.data(), spec.page_cell_offset));
        }
    }
    CHECK(reg.atexit_log.size() == 23);
    for (std::size_t i = 0; i < reg.atexit_log.size(); ++i) {
        CHECK(reg.atexit_log.at(i).dtor_rva == kSlot554B2BContSpecs[i].dtor_rva);
        CHECK(reg.atexit_log.at(i).object == object(o, static_cast<std::uint32_t>(i)));
    }
}

void check_replay() {
    const auto page = make_page();
    Slot554B2BContObjects a{};
    std::memset(&a, kSentinel, sizeof a);
    ConCmdRegistry reg{};
    Slot554B2BContHooks h = default_slot554_b2b_cont_hooks();
    h.register_var = &no_link_register;
    CHECK(init_slot554_b2b_cont(a, page.data(), reg, h).ok);
    Slot554B2BContObjects snapshot = a;
    CHECK(init_slot554_b2b_cont(a, page.data(), reg, h).ok);
    CHECK(std::memcmp(&a, &snapshot, sizeof a) == 0);
}

void check_fail_closed() {
    const auto page = make_page();
    Slot554B2BContObjects o{};
    ConCmdRegistry reg{};
    Slot554B2BContHooks h = default_slot554_b2b_cont_hooks();
    CHECK(!init_slot554_b2b_cont(o, nullptr, reg, h).ok);
    h = default_slot554_b2b_cont_hooks(); h.register_var = nullptr;
    CHECK(init_slot554_b2b_cont(o, page.data(), reg, h).blocked == Slot554B2BContStage::page);
    h = default_slot554_b2b_cont_hooks(); h.atexit = nullptr;
    CHECK(init_slot554_b2b_cont(o, page.data(), reg, h).blocked == Slot554B2BContStage::page);
    h = default_slot554_b2b_cont_hooks(); h.release_view = nullptr;
    CHECK(init_slot554_b2b_cont(o, page.data(), reg, h).blocked == Slot554B2BContStage::page);
    h = default_slot554_b2b_cont_hooks(); h.register_var = &refuse_register;
    auto r = init_slot554_b2b_cont(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B2BContStage::registration && r.built == 0);
    h = default_slot554_b2b_cont_hooks(); h.atexit = &refuse_atexit;
    r = init_slot554_b2b_cont(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B2BContStage::atexit && r.built == 0 && r.registered == 1);
    h = default_slot554_b2b_cont_hooks(); h.release_view = &refuse_release;
    r = init_slot554_b2b_cont(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B2BContStage::view_release && r.built == 0);
    h = default_slot554_b2b_cont_hooks(); h.transfer = &refuse_transfer;
    r = init_slot554_b2b_cont(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B2BContStage::ctor && r.built == 0);
    h = default_slot554_b2b_cont_hooks(); h.destroy_view = &refuse_destroy;
    r = init_slot554_b2b_cont(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B2BContStage::ctor && r.built == 0);
}

}  // namespace

int main() {
    check_constants();
    check_objects();
    check_replay();
    check_fail_closed();
    if (failures != 0) std::fprintf(stderr, "%d failure(s)\n", failures);
    return failures == 0 ? 0 : 1;
}
