// Fixture for slot 554 batch 2b: the 0xd0 0x6abb0 occluder-debug wrapper plus the seven 0xc0
// bools at 0x2eaa4a0..0x2eaa9e0, exact byte masks, wrapper tags, fail-closed hooks and replay.
#include "core/ctor_slot_554_2b_6fc29.hpp"

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

struct Counts {
    unsigned transfer = 0;
    unsigned destroy = 0;
};
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
    for (std::uint16_t off = 0x18; off <= 0x22; ++off) page[off] = 1;
    return page;
}

std::uint8_t* object(Slot554Batch2BObjects& o, std::uint32_t i) noexcept {
    if (i == 0) return o.objects.data();
    return o.objects.data() + kSlot554B2BFunctorSize + (i - 1) * kSlot554B2BBoolSize;
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
    CHECK(kSlot554B2BRva == 0x6fc29 && kSlot554B2BBodySize == 0x744);
    CHECK(kSlot554B2BObjectBaseRva == 0x2eaa3d0 && kSlot554B2BObjectCount == 8);
    CHECK(kSlot554B2BFunctorSize == 0xd0 && kSlot554B2BBoolSize == 0xc0);
    CHECK(kSlot554B2BBlockSize == 0x610 && kSlot554B2BWritten == 1329 && kSlot554B2BHoles == 223);
    CHECK(kSlot554B2BInstructionCount == 328 && kSlot554B2BDirectCallCount == 39 &&
          kSlot554B2BIndirectCallCount == 1 && kSlot554B2BObjectStoreCount == 63 &&
          kSlot554B2BStackStoreCount == 54);
    CHECK(kSlot554B2BTableIndex == 554 && kSlot554B2BTableWordRva == 0x2dd5f90);
    CHECK(kSlot554B2BWrapperRva == 0x6abb0 && kSlot554B2BWrapperSize == 0x18d);
    CHECK(kSlot554B2BInnerRva == 0x6ad40 && kSlot554B2BInnerSize == 0x160);
    CHECK(kSlot554B2BInnerMidVptr == 0x1595e90 && kSlot554B2BFinalVptr == 0x15968e8);
    CHECK(kSlot554B2BTagVptr == 0x15968b9 && kSlot554B2BViewVptr == 0x15968b9);
    CHECK(kSlot554B2BFunctor.object_rva == 0x2eaa3d0);
    CHECK(std::strcmp(kSlot554B2BFunctor.name, "XPR Occluder Debug Mode") == 0);
    CHECK(kSlot554B2BBools[0].object_rva == 0x2eaa4a0);
    CHECK(kSlot554B2BBools[6].object_rva + kSlot554B2BBoolSize == 0x2eaa9e0);
    std::size_t written = 0;
    for (std::size_t i = 0; i < kSlot554B2BBlockSize; ++i) if (slot554_batch2b_written(i)) ++written;
    CHECK(written == kSlot554B2BWritten);
    std::size_t functor_written = 0;
    for (std::size_t i = 0; i < kSlot554B2BFunctorSize; ++i) {
        if (slot554_batch2b_object_written(0, i)) ++functor_written;
    }
    CHECK(functor_written == kSlot554B2BFunctorWritten);
}

void check_objects() {
    const auto page = make_page();
    const auto page_before = page;
    Slot554Batch2BObjects o{};
    std::memset(&o, kSentinel, sizeof o);
    ConCmdRegistry reg{};
    Counts counts{};
    Slot554B2BHooks hooks = default_slot554_batch2b_hooks();
    hooks.transfer = &count_transfer;
    hooks.destroy_view = &count_destroy;
    hooks.context = &counts;
    const Slot554Batch2BResult r = init_slot554_batch2b(o, page.data(), reg, hooks);
    CHECK(r.ok && r.blocked == Slot554B2BStage::none);
    CHECK(r.built == 8 && r.registered == 8 && r.atexit_records == 8 && r.views_inert == 16);
    CHECK(r.transfers == 5 && r.view_destroys == 1);
    CHECK(counts.transfer == 5 && counts.destroy == 1);
    CHECK(page == page_before);   // the batch reads the page, it never writes it

    const std::uint8_t* f = object(o, 0);
    for (std::size_t j = 0; j < kSlot554B2BFunctorSize; ++j) {
        if (!slot554_batch2b_object_written(0, j)) CHECK(f[j] == kSentinel);
    }
    CHECK(u64(f, 0) == kSlot554B2BFinalVptr);
    CHECK(u64(f, 0x60) == reinterpret_cast<std::uint64_t>(page.data() + 0x18));
    CHECK(u32(f, 0x68) == u32(page.data(), 0x18));
    CHECK(u32(f, 0x6c) == 0 && f[0x70] == 1 && u32(f, 0x74) == 2 && f[0x78] == 1);
    CHECK(u64(f, 0x80) == 0 && u64(f, 0x88) == 0 && u64(f, 0x90) == 0 && u64(f, 0x98) == 0);
    CHECK(u64(f, 0xb0) == 0 && u64(f, 0xc0) == kSlot554B2BTagVptr && u64(f, 0xc8) == 0);
    const SharedStr& fname = *reinterpret_cast<const SharedStr*>(f + 0x20);
    CHECK(fname.size == 23 && std::memcmp(fname.data, kSlot554B2BFunctor.name, 23) == 0);

    for (std::uint32_t i = 1; i < kSlot554B2BObjectCount; ++i) {
        const std::uint8_t* p = object(o, i);
        const Slot554B2BSpec& spec = kSlot554B2BBools[i - 1];
        for (std::size_t j = 0; j < kSlot554B2BBoolSize; ++j) {
            if (!slot554_batch2b_object_written(i, j)) CHECK(p[j] == kSentinel);
        }
        CHECK(u64(p, 0) == kSlot554B2AFinalVptr);
        CHECK(u64(p, 0x60) == reinterpret_cast<std::uint64_t>(page.data() + spec.page_cell_offset));
        CHECK(p[0x68] == 1 && u32(p, 0x69) == 0);
        CHECK(u64(p, 0xa0) == 0 && u64(p, 0xb0) == kSlot554B2AFnVtbl && u64(p, 0xb8) == 0);
        CHECK(p[0x18] == 1);
        const SharedStr& name = *reinterpret_cast<const SharedStr*>(p + 0x20);
        CHECK(name.size == spec.name_length);
        CHECK(std::memcmp(name.data, spec.name, name.size) == 0);
    }
    CHECK(reg.atexit_log.size() == 8);
    for (std::size_t i = 0; i < reg.atexit_log.size(); ++i) {
        const std::uint64_t expected = i == 0 ? kSlot554B2BDtorRva : kSlot554B2ADtorRva;
        CHECK(reg.atexit_log.at(i).dtor_rva == expected);
        CHECK(reg.atexit_log.at(i).object == object(o, static_cast<std::uint32_t>(i)));
    }
}

void check_replay() {
    const auto page = make_page();
    Slot554Batch2BObjects a{};
    std::memset(&a, kSentinel, sizeof a);
    ConCmdRegistry reg{};
    Slot554B2BHooks h = default_slot554_batch2b_hooks();
    h.register_var = &no_link_register;
    CHECK(init_slot554_batch2b(a, page.data(), reg, h).ok);
    Slot554Batch2BObjects snapshot = a;
    CHECK(init_slot554_batch2b(a, page.data(), reg, h).ok);
    CHECK(std::memcmp(&a, &snapshot, sizeof a) == 0);
}

void check_fail_closed() {
    const auto page = make_page();
    Slot554Batch2BObjects o{};
    ConCmdRegistry reg{};
    Slot554B2BHooks h = default_slot554_batch2b_hooks();

    CHECK(!init_slot554_batch2b(o, nullptr, reg, h).ok);
    h = default_slot554_batch2b_hooks(); h.register_var = nullptr;
    CHECK(init_slot554_batch2b(o, page.data(), reg, h).blocked == Slot554B2BStage::page);
    h = default_slot554_batch2b_hooks(); h.atexit = nullptr;
    CHECK(init_slot554_batch2b(o, page.data(), reg, h).blocked == Slot554B2BStage::page);
    h = default_slot554_batch2b_hooks(); h.release_view = nullptr;
    CHECK(init_slot554_batch2b(o, page.data(), reg, h).blocked == Slot554B2BStage::page);

    h = default_slot554_batch2b_hooks(); h.register_var = &refuse_register;
    Slot554Batch2BResult r = init_slot554_batch2b(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B2BStage::registration && r.built == 0 &&
          r.registered == 0);
    h = default_slot554_batch2b_hooks(); h.atexit = &refuse_atexit;
    r = init_slot554_batch2b(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B2BStage::atexit && r.built == 0 && r.registered == 1);
    h = default_slot554_batch2b_hooks(); h.release_view = &refuse_release;
    r = init_slot554_batch2b(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B2BStage::view_release && r.built == 0);
    h = default_slot554_batch2b_hooks(); h.transfer = &refuse_transfer;
    r = init_slot554_batch2b(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B2BStage::ctor && r.built == 0);
    h = default_slot554_batch2b_hooks(); h.destroy_view = &refuse_destroy;
    r = init_slot554_batch2b(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B2BStage::ctor && r.built == 0 && r.registered == 1);
    h = default_slot554_batch2b_hooks(); h.release_view = &refuse_release;
    r = init_slot554_batch2b(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B2BStage::view_release && r.built == 0);
}
}  // namespace

int main() {
    check_constants();
    check_objects();
    check_replay();
    check_fail_closed();
    if (failures) { std::fprintf(stderr, "%d failures\n", failures); return 1; }
    std::puts("ctor_slot_554_batch2b: all checks passed");
    return 0;
}
