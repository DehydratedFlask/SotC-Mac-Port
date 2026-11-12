// Fixture for slot 554 batch 3: the 36 objects in [0x718bb,0x739e8).
#include "core/ctor_slot_554_batch3_718bb.hpp"

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
bool refuse_wrapper(void*, ConCmdFunctor&) noexcept { return false; }
bool refuse_destroy(void*, const ConCmdFunctorView&) noexcept { return false; }

struct Counts { unsigned wrapper = 0; unsigned destroy = 0; };
bool count_wrapper(void* ctx, ConCmdFunctor&) noexcept {
    ++static_cast<Counts*>(ctx)->wrapper;
    return true;
}
bool count_destroy(void* ctx, const ConCmdFunctorView&) noexcept {
    ++static_cast<Counts*>(ctx)->destroy;
    return true;
}

std::array<std::uint8_t, 0xac8> make_page() {
    std::array<std::uint8_t, 0xac8> page{};
    page.fill(kSentinel);
    for (std::uint32_t i = 0; i < kSlot554B3ObjectCount; ++i) {
        const auto& s = kSlot554B3Specs[i];
        if (s.kind == Slot554B3Kind::bool_var) page[s.page_cell_offset] = 1;
        if (s.kind == Slot554B3Kind::float_var)
            std::memcpy(page.data() + s.page_cell_offset, &s.high, sizeof s.high);
        if (s.kind == Slot554B3Kind::wrapper) {
            const std::uint32_t v = 7;
            std::memcpy(page.data() + s.page_cell_offset, &v, sizeof v);
        }
    }
    return page;
}

std::uint8_t* object(Slot554Batch3Objects& o, std::uint32_t i) noexcept {
    std::size_t offset = 0;
    for (std::uint32_t n = 0; n < i; ++n) offset += kSlot554B3Specs[n].size;
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
    CHECK(kSlot554B3Rva == 0x718bb && kSlot554B3BodySize == 0x1b30);
    CHECK(kSlot554B3ObjectBaseRva == 0x2eabb80 && kSlot554B3ObjectEndRva == 0x2ead6b0);
    CHECK(kSlot554B3ObjectCount == 36 && kSlot554B3BlockSize == 0x1b30);
    CHECK(kSlot554B3Written == 5883 && kSlot554B3Holes == 1077);
    CHECK(kSlot554B3InstructionCount == 1494 && kSlot554B3DirectCallCount == 179 &&
          kSlot554B3IndirectCallCount == 3 && kSlot554B3StoreCount == 472 &&
          kSlot554B3ObjectStoreCount == 303 && kSlot554B3StackStoreCount == 169);
    CHECK(kSlot554B3BoolCount == 33 && kSlot554B3FloatCount == 2 && kSlot554B3WrapperCount == 1);
    CHECK(kSlot554B3TableIndex == 554 && kSlot554B3TableWordRva == 0x2dd5f90);
    std::size_t written = 0;
    for (std::size_t i = 0; i < kSlot554B3BlockSize; ++i) if (slot554_batch3_written(i)) ++written;
    CHECK(written == kSlot554B3Written);
    std::size_t bool_written = 0;
    for (std::size_t i = 0; i < 0xc0; ++i) if (slot554_batch3_object_written(0, i)) ++bool_written;
    CHECK(bool_written == kSlot554B3BoolWritten);
    std::size_t wrapper_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch3_object_written(27, i)) ++wrapper_written;
    CHECK(wrapper_written == kSlot554B3WrapperWritten);
    std::size_t float_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch3_object_written(33, i)) ++float_written;
    CHECK(float_written == kSlot554B3FloatWritten);
}

void check_objects() {
    const auto page = make_page();
    const auto page_before = page;
    Slot554Batch3Objects o{};
    std::memset(&o, kSentinel, sizeof o);
    ConCmdRegistry reg{};
    Counts counts{};
    Slot554B3Hooks hooks = default_slot554_batch3_hooks();
    hooks.wrapper_call = &count_wrapper;
    hooks.destroy_view = &count_destroy;
    hooks.context = &counts;
    const Slot554Batch3Result r = init_slot554_batch3(o, page.data(), reg, hooks);
    CHECK(r.ok && r.blocked == Slot554B3Stage::none);
    CHECK(r.built == 36 && r.registered == 36 && r.atexit_records == 36 && r.views_inert == 72);
    CHECK(r.wrapper_calls == 1 && r.view_destroys == 1);
    CHECK(counts.wrapper == 1 && counts.destroy == 1);
    CHECK(page == page_before);

    for (std::uint32_t i = 0; i < kSlot554B3ObjectCount; ++i) {
        const auto& spec = kSlot554B3Specs[i];
        const std::uint8_t* p = object(o, i);
        for (std::size_t j = 0; j < spec.size; ++j) {
            if (!slot554_batch3_object_written(i, j)) CHECK(p[j] == kSentinel);
        }
        const SharedStr& name = *reinterpret_cast<const SharedStr*>(p + 0x20);
        CHECK(name.size == spec.name_length);
        CHECK(std::memcmp(name.data, spec.name, name.size) == 0);
        if (spec.kind == Slot554B3Kind::bool_var) {
            CHECK(u64(p, 0) == kSlot554B3BoolFinalVptr);
            CHECK(u64(p, 0x60) == reinterpret_cast<std::uint64_t>(page.data() + spec.page_cell_offset));
            CHECK(p[0x68] == 1 && u32(p, 0x69) == 0);
            CHECK(u64(p, 0xa0) == 0 && u64(p, 0xb0) == kSlot554B3BoolFnVtbl && u64(p, 0xb8) == 0);
        } else if (spec.kind == Slot554B3Kind::float_var) {
            CHECK(u64(p, 0) == kSlot554B3FloatFinalVptr);
            CHECK(u64(p, 0x60) == reinterpret_cast<std::uint64_t>(page.data() + spec.page_cell_offset));
            CHECK(u32(p, 0x68) == u32(page.data(), spec.page_cell_offset));
            CHECK(u32(p, 0x6c) == 0);
        } else {
            CHECK(u64(p, 0) == kSlot554B3WrapperFinalVptr);
            CHECK(u64(p, 0x60) == reinterpret_cast<std::uint64_t>(page.data() + spec.page_cell_offset));
            CHECK(u32(p, 0x68) == 7);
            CHECK(u32(p, 0x6c) == 0 && p[0x70] == 1 && u32(p, 0x74) == 3 && p[0x78] == 1);
            CHECK(u64(p, 0xb0) == 0 && u64(p, 0xc0) == kSlot554B3WrapperTagVptr && u64(p, 0xc8) == 0);
        }
    }
    CHECK(reg.atexit_log.size() == 36);
    for (std::size_t i = 0; i < reg.atexit_log.size(); ++i) {
        const auto& s = kSlot554B3Specs[i];
        const std::uint64_t want = s.kind == Slot554B3Kind::bool_var   ? kSlot554B3BoolDtorRva
                                   : s.kind == Slot554B3Kind::float_var ? kSlot554B3FloatDtorRva
                                                                       : kSlot554B3WrapperDtorRva;
        CHECK(reg.atexit_log.at(i).dtor_rva == want);
        CHECK(reg.atexit_log.at(i).object == object(o, static_cast<std::uint32_t>(i)));
    }
}

void check_replay() {
    const auto page = make_page();
    Slot554Batch3Objects a{};
    std::memset(&a, kSentinel, sizeof a);
    ConCmdRegistry reg{};
    Slot554B3Hooks h = default_slot554_batch3_hooks();
    h.register_var = &no_link_register;
    CHECK(init_slot554_batch3(a, page.data(), reg, h).ok);
    Slot554Batch3Objects snapshot = a;
    CHECK(init_slot554_batch3(a, page.data(), reg, h).ok);
    CHECK(std::memcmp(&a, &snapshot, sizeof a) == 0);
}

void check_fail_closed() {
    const auto page = make_page();
    Slot554Batch3Objects o{};
    ConCmdRegistry reg{};
    Slot554B3Hooks h = default_slot554_batch3_hooks();
    CHECK(!init_slot554_batch3(o, nullptr, reg, h).ok);
    h = default_slot554_batch3_hooks(); h.register_var = nullptr;
    CHECK(init_slot554_batch3(o, page.data(), reg, h).blocked == Slot554B3Stage::page);
    h = default_slot554_batch3_hooks(); h.atexit = nullptr;
    CHECK(init_slot554_batch3(o, page.data(), reg, h).blocked == Slot554B3Stage::page);
    h = default_slot554_batch3_hooks(); h.release_view = nullptr;
    CHECK(init_slot554_batch3(o, page.data(), reg, h).blocked == Slot554B3Stage::page);
    h = default_slot554_batch3_hooks(); h.register_var = &refuse_register;
    auto r = init_slot554_batch3(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B3Stage::registration && r.built == 0);
    h = default_slot554_batch3_hooks(); h.atexit = &refuse_atexit;
    r = init_slot554_batch3(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B3Stage::atexit && r.built == 0 && r.registered == 1);
    h = default_slot554_batch3_hooks(); h.release_view = &refuse_release;
    r = init_slot554_batch3(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B3Stage::view_release && r.built == 0);
    h = default_slot554_batch3_hooks(); h.wrapper_call = &refuse_wrapper;
    r = init_slot554_batch3(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B3Stage::ctor && r.built == 27);
    h = default_slot554_batch3_hooks(); h.destroy_view = &refuse_destroy;
    r = init_slot554_batch3(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B3Stage::ctor && r.built == 27);
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
