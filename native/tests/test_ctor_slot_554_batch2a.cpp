// Fixture for slot 554 batch 2a: five bool variables at 0x2eaa010..0x2eaa3d0, exact byte mask,
// field values, registration/atexit counts, fail-closed hooks and byte-identical replay.
#include "core/ctor_slot_554_2a_6f793.hpp"

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

std::array<std::uint8_t, 0xac8> make_page() {
    std::array<std::uint8_t, 0xac8> page{};
    page.fill(kSentinel);
    for (std::uint16_t off = 0x12; off <= 0x16; ++off) page[off] = 1;
    return page;
}

std::uint8_t* object(Slot554Batch2AObjects& o, std::uint32_t i) noexcept {
    return o.objects.data() + i * kSlot554B2AObjectSize;
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
    CHECK(kSlot554B2ARva == 0x6f793 && kSlot554B2ABodySize == 0x496);
    CHECK(kSlot554B2AObjectBaseRva == 0x2eaa010 && kSlot554B2AObjectCount == 5);
    CHECK(kSlot554B2AObjectSize == 0xc0 && kSlot554B2ABlockSize == 0x3c0);
    CHECK(kSlot554B2AWritten == 810 && kSlot554B2AHoles == 150);
    CHECK(kSlot554B2ATableIndex == 554 && kSlot554B2ATableWordRva == 0x2dd5f90);
    CHECK(kSlot554B2AVars[0].object_rva == 0x2eaa010);
    CHECK(std::strcmp(kSlot554B2AVars[0].name, "threadedCulling") == 0);
    CHECK(kSlot554B2AVars[4].object_rva == 0x2eaa310);
    CHECK(kSlot554B2AVars[4].object_rva + kSlot554B2AObjectSize == 0x2eaa3d0);
    std::size_t written = 0;
    for (std::size_t i = 0; i < kSlot554B2ABlockSize; ++i) if (slot554_batch2a_written(i)) ++written;
    CHECK(written == kSlot554B2AWritten);
}

void check_objects() {
    const auto page = make_page();
    const auto page_before = page;
    Slot554Batch2AObjects o{};
    std::memset(&o, kSentinel, sizeof o);
    ConCmdRegistry reg{};
    const Slot554Batch2AResult r = init_slot554_batch2a(o, page.data(), reg, default_slot554_batch2a_hooks());
    CHECK(r.ok && r.blocked == Slot554B2AStage::none);
    CHECK(r.built == 5 && r.registered == 5 && r.atexit_records == 5 && r.views_inert == 10);
    CHECK(page == page_before);   // this batch reads the page, it never writes it

    for (std::uint32_t i = 0; i < kSlot554B2AObjectCount; ++i) {
        const std::uint8_t* p = object(o, i);
        const Slot554B2ASpec& spec = kSlot554B2AVars[i];
        for (std::size_t j = 0; j < kSlot554B2AObjectSize; ++j) {
            if (!slot554_batch2a_object_written(i, j)) CHECK(p[j] == kSentinel);
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
    CHECK(reg.atexit_log.size() == 5);
    for (std::size_t i = 0; i < reg.atexit_log.size(); ++i) {
        CHECK(reg.atexit_log.at(i).dtor_rva == kSlot554B2ADtorRva);
        CHECK(reg.atexit_log.at(i).object == object(o, static_cast<std::uint32_t>(i)));
    }
}

void check_replay() {
    const auto page = make_page();
    Slot554Batch2AObjects a{};
    std::memset(&a, kSentinel, sizeof a);
    ConCmdRegistry reg{};
    Slot554B2AHooks h = default_slot554_batch2a_hooks();
    h.register_var = &no_link_register;
    CHECK(init_slot554_batch2a(a, page.data(), reg, h).ok);
    Slot554Batch2AObjects snapshot = a;
    CHECK(init_slot554_batch2a(a, page.data(), reg, h).ok);
    CHECK(std::memcmp(&a, &snapshot, sizeof a) == 0);
}

void check_fail_closed() {
    const auto page = make_page();
    Slot554Batch2AObjects o{};
    ConCmdRegistry reg{};
    Slot554B2AHooks h = default_slot554_batch2a_hooks();

    CHECK(!init_slot554_batch2a(o, nullptr, reg, h).ok);
    h = default_slot554_batch2a_hooks(); h.register_var = nullptr;
    CHECK(init_slot554_batch2a(o, page.data(), reg, h).blocked == Slot554B2AStage::page);
    h = default_slot554_batch2a_hooks(); h.atexit = nullptr;
    CHECK(init_slot554_batch2a(o, page.data(), reg, h).blocked == Slot554B2AStage::page);
    h = default_slot554_batch2a_hooks(); h.release_view = nullptr;
    CHECK(init_slot554_batch2a(o, page.data(), reg, h).blocked == Slot554B2AStage::page);

    h = default_slot554_batch2a_hooks(); h.register_var = &refuse_register;
    Slot554Batch2AResult r = init_slot554_batch2a(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B2AStage::registration && r.built == 0 && r.registered == 0);
    h = default_slot554_batch2a_hooks(); h.atexit = &refuse_atexit;
    r = init_slot554_batch2a(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B2AStage::atexit && r.built == 0 && r.registered == 1);
    h = default_slot554_batch2a_hooks(); h.release_view = &refuse_release;
    r = init_slot554_batch2a(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B2AStage::view_release && r.built == 0);
}
}  // namespace

int main() {
    check_constants();
    check_objects();
    check_replay();
    check_fail_closed();
    if (failures) { std::fprintf(stderr, "%d failures\n", failures); return 1; }
    std::puts("ctor_slot_554_batch2a: all checks passed");
    return 0;
}
