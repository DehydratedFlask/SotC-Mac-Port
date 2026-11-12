// CPU-01-STARTUP-SLOT-552: fixture for startup constructor index 552 (0x8c880).
//
// Proves the five-part global block: seven flag dwords, seventeen zeroed 0x18-byte records, six
// class-A pages with their twelve self-referential nodes, the class-B page's {page+0x80} head and the
// class-C page. It also proves the documented unwritten bytes (a 0x5a fill survives only where the
// original body never stores) and the fail-closed stage when the 0x475c0 hook refuses.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_552_8c880.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kSentinel = 0x5a;

void fill(Slot552Objects& objects) {
    std::memset(static_cast<void*>(&objects), kSentinel, sizeof objects);
}

Slot552PageResult refusing_ctor(void*, const SharedStr&) noexcept {
    return Slot552PageResult{};
}

Slot552PageResult accepting_ctor(void* page, const SharedStr& name) noexcept {
    return init_slot552_page_475c0(page, name);
}

void section_constants() {
    CHECK(kSlot552Rva == 0x8c880 && kSlot552Size == 3479 && kSlot552EndRva == 0x8d617);
    CHECK(kSlot552Rva + kSlot552Size == kSlot552EndRva);
    CHECK(kSlot552TableWordRva == 0x2dd5ec0 && kSlot552TableIndex == 552);
    CHECK(kSlot552PageCtorRva == 0x475c0 && kSlot552PageCtorSize == 87);
    CHECK(kSlot552AtexitRva == 0xf8f390 && kSlot552DsoRva == 0x1604218);
    CHECK(kSlot552BlockRva == 0x2ed1f48 && kSlot552BlockSize == 0x14a0);
    CHECK(kSlot552FlagsRva == 0x2ed1f48 && kSlot552FlagCount == 7);
    CHECK(kSlot552RecordRva == 0x2ed1f68 && kSlot552RecordCount == 17 &&
          kSlot552RecordStride == 0x18);
    CHECK(kSlot552GapAfterFlagsRva == 0x2ed1f64);
    CHECK(kSlot552GapBeforePagesRva == 0x2ed2100);
    CHECK(kSlot552PageARva == 0x2ed2120 && kSlot552PageACount == 6 &&
          kSlot552PageASize == 0x2e0);
    CHECK(kSlot552PageBRva == 0x2ed3260 && kSlot552PageBSize == 0xf0);
    CHECK(kSlot552PageCRva == 0x2ed3350 && kSlot552PageCSize == 0x98);
    CHECK(kSlot552PageCRva + kSlot552PageCSize == 0x2ed33e8);
    CHECK(kSlot552NodeCount == 12 && kSlot552NodeStride == 0x30 && kSlot552NodesOffset == 0xb0);
    CHECK(kSlot552PageCtorVptr == 0x1595900 && kSlot552PageAVptr == 0x1594ca0);
    CHECK(kSlot552PageBVptr == 0x1595b70 && kSlot552PageCVptr == 0x1597560);
    CHECK(kSlot552PageADtorRva == 0x1ce70 && kSlot552PageBDtorRva == 0x48790 &&
          kSlot552PageCDtorRva == 0x8c360);
    CHECK(kSlot552AtexitRowCount == 8 && kSlot552AtexitRows[7].site_rva == 0x8d612);
}

void section_layout() {
    CHECK(sizeof(Slot552PageBase) == 0x80);
    CHECK(sizeof(Slot552PageA) == 0x2e0);
    CHECK(sizeof(Slot552PageB) == 0xf0);
    CHECK(sizeof(Slot552PageC) == 0x98);
    CHECK(sizeof(Slot552Node) == 0x30 && sizeof(Slot552NodeTail) == 0x20);
    CHECK(sizeof(Slot552Record) == 0x18 && sizeof(Slot552Flags) == 0x20);
    CHECK(sizeof(Slot552Objects) == 0x14a0);
    CHECK(offsetof(Slot552Objects, flags) == 0x000);
    CHECK(offsetof(Slot552Objects, records) == 0x020);
    CHECK(offsetof(Slot552Objects, gap1b8) == 0x1b8);
    CHECK(offsetof(Slot552Objects, pages_a) == 0x1d8);
    CHECK(offsetof(Slot552Objects, page_b) == 0x1318);
    CHECK(offsetof(Slot552Objects, page_c) == 0x1408);
    CHECK(offsetof(Slot552PageA, nodes) == 0xb0 && offsetof(Slot552PageA, last_node) == 0x2c0);
    CHECK(offsetof(Slot552PageB, head) == 0x80 && offsetof(Slot552PageB, zero_b8) == 0xb8);
    CHECK(offsetof(Slot552PageC, zero80) == 0x80 && offsetof(Slot552PageC, word90) == 0x90);
    // The eight names are the body's own order.
    CHECK(std::strcmp(slot552_page_name(0), "GameToRenderThread") == 0);
    CHECK(std::strcmp(slot552_page_name(1), "GameToGPU") == 0);
    CHECK(std::strcmp(slot552_page_name(2), "CPU_RenderToGPUToGame") == 0);
    CHECK(std::strcmp(slot552_page_name(3), "RenderSingleFrameGPU") == 0);
    CHECK(std::strcmp(slot552_page_name(4), "RenderSingleFrameCPU") == 0);
    CHECK(std::strcmp(slot552_page_name(5), "XPRTempData") == 0);
    CHECK(std::strcmp(slot552_page_name(6), "MaterialParamHeap") == 0);
    CHECK(std::strcmp(slot552_page_name(7), "VirtualMemoryGPU") == 0);
    for (std::uint32_t page = 0; page < 8; ++page) {
        CHECK(slot552_page_name_len(page) == std::strlen(slot552_page_name(page)));
    }
}

void section_unwritten_bytes() {
    // The exact gaps the body never stores: the four bytes after the flags, each record's +4..+7,
    // the 0x20 before the first page, and the fourteen bytes no page store reaches.
    CHECK(!slot552_byte_written(0x1c) && slot552_byte_written(0x1b) && slot552_byte_written(0x20));
    CHECK(!slot552_byte_written(0x24) && slot552_byte_written(0x28));
    CHECK(!slot552_byte_written(0x1b8) && slot552_byte_written(0x1b7) && slot552_byte_written(0x1d8));
    CHECK(!slot552_byte_written(0x1d8 + 0x0c) && slot552_byte_written(0x1d8 + 0x10));
    CHECK(!slot552_byte_written(0x1d8 + 0x5e) && slot552_byte_written(0x1d8 + 0x5c) == false);
    CHECK(!slot552_byte_written(0x1d8 + 0x6c) && slot552_byte_written(0x1d8 + 0x70));
    CHECK(!slot552_byte_written(0x1d8 + 0x7c) && slot552_byte_written(0x1d8 + 0x80));
    CHECK(!slot552_byte_written(0x1d8 + 0x8d) && slot552_byte_written(0x1d8 + 0x90));
    CHECK(!slot552_byte_written(0x1d8 + 0xc0) && slot552_byte_written(0x1d8 + 0xd0));
    CHECK(!slot552_byte_written(0x1d8 + 0x2d8) && slot552_byte_written(0x1d8 + 0x2cf));
    CHECK(!slot552_byte_written(0x1318 + 0xb4) && slot552_byte_written(0x1318 + 0xb8));
    CHECK(!slot552_byte_written(0x1408 + 0x0c) && slot552_byte_written(0x1408 + 0x10));
    CHECK(!slot552_byte_written(kSlot552BlockSize));
    // 0x14a0 - (7*4 + 17*0x10 + 6*(0xa0 - 0x18)) bytes are written; the count is asserted through
    // the sentinel sweep below, this is only a sanity bound.
    std::size_t written = 0;
    for (std::size_t i = 0; i < kSlot552BlockSize; ++i) if (slot552_byte_written(i)) ++written;
    CHECK(written == 0xec8);   // 0x14a0 - 0x5d8 unwritten bytes
}

void section_construction() {
    Slot552Objects objects;
    fill(objects);
    ConCmdRegistry concmd;
    const std::size_t before = concmd.atexit_log.size();
    const Slot552Result result = init_slot552_8c880(objects, concmd, default_slot552_hooks());
    CHECK(result.ok && result.blocked == Slot552Stage::none);
    CHECK(result.flags_written == 7 && result.records_written == 17);
    CHECK(result.pages_built == 8 && result.atexit_records == 8 && result.names_borrowed);
    CHECK(concmd.atexit_log.size() == before + 8);

    // The sentinel survives exactly on the documented unwritten bytes and nowhere else.
    // Only the documented holes may keep the fill: a written byte can legitimately equal 0x5a
    // (the name pointers carry ASLR addresses), so written bytes are checked by value above.
    const auto* bytes = reinterpret_cast<const std::uint8_t*>(&objects);
    std::size_t holes = 0;
    for (std::size_t i = 0; i < sizeof objects; ++i) {
        if (slot552_byte_written(i)) continue;
        ++holes;
        CHECK(bytes[i] == kSentinel);
    }
    CHECK(holes == 0x5d8);

    for (std::uint32_t flag : objects.flags.value) CHECK(flag == 1);
    for (const Slot552Record& record : objects.records) {
        CHECK(record.word == 0);
        for (std::uint8_t byte : record.zero8) CHECK(byte == 0);
    }

    for (std::size_t i = 0; i < objects.pages_a.size(); ++i) {
        const Slot552PageA& page = objects.pages_a[i];
        const auto self = reinterpret_cast<std::uint64_t>(&page);
        CHECK(page.base.vptr == kSlot552PageAVptr);
        CHECK(page.base.byte08 == 0 && page.base.byte09 == 1);
        CHECK(page.base.name == reinterpret_cast<std::uint64_t>(slot552_page_name(i)));
        CHECK(page.base.word18 == 0 && page.base.word3c == 0);
        for (std::uint8_t byte : page.base.zero1c) CHECK(byte == 0);
        CHECK(page.base.link40 == self + 0x38 && page.base.link48 == self + 0x38);
        CHECK(page.base.word50 == 0 && page.base.word58 == 0 && page.base.word60 == 0);
        CHECK(page.base.word70 == 0);
        CHECK(page.word80 == 0 && page.word88 == 0);
        for (std::uint8_t byte : page.zero90) CHECK(byte == 0);
        for (const Slot552Node& node : page.nodes) {
            CHECK(node.next == &node && node.prev == &node);
            for (std::uint8_t byte : node.zero20) CHECK(byte == 0);
        }
        CHECK(page.last_node.next == &page.last_node && page.last_node.prev == &page.last_node);
    }

    {
        const Slot552PageB& page = objects.page_b;
        const auto self = reinterpret_cast<std::uint64_t>(&page);
        CHECK(page.base.vptr == kSlot552PageBVptr);
        CHECK(page.base.name == reinterpret_cast<std::uint64_t>(slot552_page_name(6)));
        CHECK(page.base.link40 == self + 0x38 && page.base.link48 == self + 0x38);
        for (std::uint64_t slot : page.head) CHECK(slot == self + 0x80);
        CHECK(page.word_a8 == 0);
        for (std::uint8_t byte : page.zero_b8) CHECK(byte == 0);
    }
    {
        const Slot552PageC& page = objects.page_c;
        CHECK(page.base.vptr == kSlot552PageCVptr);
        CHECK(page.base.name == reinterpret_cast<std::uint64_t>(slot552_page_name(7)));
        for (std::uint8_t byte : page.zero80) CHECK(byte == 0);
        CHECK(page.word90 == 0);
    }

    // A second run over the same object reproduces the same state (the body is idempotent).
    const Slot552Objects first = objects;
    const Slot552Result again = init_slot552_8c880(objects, concmd, default_slot552_hooks());
    CHECK(again.ok && again.pages_built == 8);
    CHECK(std::memcmp(&first, &objects, sizeof objects) == 0);

    // An explicit native hook is accepted too.
    Slot552Objects other;
    fill(other);
    Slot552Hooks hooks{};
    hooks.page_ctor = &accepting_ctor;
    CHECK(init_slot552_8c880(other, concmd, hooks).ok);
}

void section_fail_closed() {
    // A missing hook is the fail-closed stage: the prefix is written (it precedes the first page
    // ctor in the body) and the pages keep the caller's bytes.
    Slot552Objects objects;
    fill(objects);
    ConCmdRegistry concmd;
    const std::size_t before = concmd.atexit_log.size();
    const Slot552Hooks no_hook{};
    const Slot552Result result = init_slot552_8c880(objects, concmd, no_hook);
    CHECK(!result.ok && result.blocked == Slot552Stage::page_ctor);
    CHECK(result.blocked_page == 0 && result.pages_built == 0 && result.atexit_records == 0);
    CHECK(concmd.atexit_log.size() == before);
    CHECK(result.flags_written == 7 && result.records_written == 17);
    for (std::uint32_t flag : objects.flags.value) CHECK(flag == 1);

    // A hook that refuses stops at the same stage and writes no page bytes either.
    Slot552Objects refused;
    fill(refused);
    Slot552Hooks refusing{};
    refusing.page_ctor = &refusing_ctor;
    const Slot552Result stopped = init_slot552_8c880(refused, concmd, refusing);
    CHECK(!stopped.ok && stopped.blocked == Slot552Stage::page_ctor);
    for (std::uint32_t flag : refused.flags.value) CHECK(flag == 1);
    const auto* bytes = reinterpret_cast<const std::uint8_t*>(&refused);
    for (std::size_t i = offsetof(Slot552Objects, pages_a); i < sizeof refused; ++i) {
        CHECK(bytes[i] == kSentinel);
    }
    CHECK(concmd.atexit_log.size() == before);
}

void section_stage_names() {
    CHECK(std::strcmp(slot552_stage_name(Slot552Stage::none), "none") == 0);
    CHECK(std::strcmp(slot552_stage_name(Slot552Stage::flags), "flags") == 0);
    CHECK(std::strcmp(slot552_stage_name(Slot552Stage::records), "records") == 0);
    CHECK(std::strcmp(slot552_stage_name(Slot552Stage::page_ctor), "page_ctor") == 0);
    CHECK(std::strcmp(slot552_stage_name(Slot552Stage::page_stores), "page_stores") == 0);
    CHECK(std::strcmp(slot552_stage_name(Slot552Stage::page_atexit), "page_atexit") == 0);
}

}  // namespace

int main() {
    section_constants();
    section_layout();
    section_unwritten_bytes();
    section_construction();
    section_fail_closed();
    section_stage_names();
    std::puts("ctor_slot_552: all checks passed");
    return 0;
}
