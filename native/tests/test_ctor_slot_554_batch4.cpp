// Fixture for slot 554 batch 4: the 36 objects in [0x739e8,0x75d22).
#include "core/ctor_slot_554_batch4_739e8.hpp"

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
    for (std::uint32_t i = 0; i < kSlot554B4ObjectCount; ++i) {
        const auto& s = kSlot554B4Specs[i];
        if (s.kind == Slot554B4Kind::bool_var) page[s.page_cell_offset] = 1;
        if (s.kind == Slot554B4Kind::float_var || s.kind == Slot554B4Kind::int_var)
            std::memcpy(page.data() + s.page_cell_offset, &s.high, sizeof s.high);
        if (s.kind == Slot554B4Kind::wrapper) {
            const std::uint32_t v = 7;
            std::memcpy(page.data() + s.page_cell_offset, &v, sizeof v);
        }
    }
    return page;
}

std::uint8_t* object(Slot554Batch4Objects& o, std::uint32_t i) noexcept {
    std::size_t offset = 0;
    for (std::uint32_t n = 0; n < i; ++n) offset += kSlot554B4Specs[n].size;
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
    CHECK(kSlot554B4Rva == 0x739e8 && kSlot554B4BodySize == 0x233a);
    CHECK(kSlot554B4ObjectBaseRva == 0x2ead6b0 && kSlot554B4ObjectEndRva == 0x2eaf2a0);
    CHECK(kSlot554B4ObjectCount == 36 && kSlot554B4BlockSize == 0x1bf0);
    CHECK(kSlot554B4Written == 6015 && kSlot554B4Holes == 1137);
    CHECK(kSlot554B4InstructionCount == 1568 && kSlot554B4DirectCallCount == 178 &&
          kSlot554B4IndirectCallCount == 15 && kSlot554B4StoreCount == 522 &&
          kSlot554B4ObjectStoreCount == 215 && kSlot554B4StackStoreCount == 307);
    CHECK(kSlot554B4BoolCount == 21 && kSlot554B4FloatCount == 12 && kSlot554B4IntCount == 1 &&
          kSlot554B4WrapperCount == 2);
    CHECK(kSlot554B4TableIndex == 554 && kSlot554B4TableWordRva == 0x2dd5f90);
    std::size_t written = 0;
    for (std::size_t i = 0; i < kSlot554B4BlockSize; ++i) if (slot554_batch4_written(i)) ++written;
    CHECK(written == kSlot554B4Written);
    std::size_t bool_written = 0;
    for (std::size_t i = 0; i < 0xc0; ++i) if (slot554_batch4_object_written(0, i)) ++bool_written;
    CHECK(bool_written == kSlot554B4BoolWritten);
    std::size_t int_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch4_object_written(2, i)) ++int_written;
    CHECK(int_written == kSlot554B4IntWritten);
    std::size_t float_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch4_object_written(4, i)) ++float_written;
    CHECK(float_written == kSlot554B4FloatWritten);
    std::size_t wrapper_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch4_object_written(3, i)) ++wrapper_written;
    CHECK(wrapper_written == kSlot554B4WrapperWritten);
}

void check_objects() {
    const auto page = make_page();
    const auto page_before = page;
    Slot554Batch4Objects o{};
    std::memset(&o, kSentinel, sizeof o);
    ConCmdRegistry reg{};
    Counts counts{};
    Slot554B4Hooks hooks = default_slot554_batch4_hooks();
    hooks.wrapper_call = &count_wrapper;
    hooks.destroy_view = &count_destroy;
    hooks.context = &counts;
    const Slot554Batch4Result r = init_slot554_batch4(o, page.data(), reg, hooks);
    CHECK(r.ok && r.blocked == Slot554B4Stage::none);
    CHECK(r.built == 36 && r.registered == 36 && r.atexit_records == 36 && r.views_inert == 72);
    CHECK(r.wrapper_calls == 2 && r.view_destroys == 2);
    CHECK(counts.wrapper == 2 && counts.destroy == 2);
    CHECK(page == page_before);

    for (std::uint32_t i = 0; i < kSlot554B4ObjectCount; ++i) {
        const auto& spec = kSlot554B4Specs[i];
        const std::uint8_t* p = object(o, i);
        for (std::size_t j = 0; j < spec.size; ++j) {
            if (!slot554_batch4_object_written(i, j)) CHECK(p[j] == kSentinel);
        }
        const SharedStr& name = *reinterpret_cast<const SharedStr*>(p + 0x20);
        CHECK(name.size == spec.name_length);
        CHECK(std::memcmp(name.data, spec.name, name.size) == 0);
        if (spec.kind == Slot554B4Kind::bool_var) {
            CHECK(u64(p, 0) == kSlot554B4BoolFinalVptr);
            CHECK(u64(p, 0x60) == reinterpret_cast<std::uint64_t>(page.data() + spec.page_cell_offset));
            CHECK(p[0x68] == 1 && u32(p, 0x69) == 0);
            CHECK(u64(p, 0xa0) == 0 && u64(p, 0xb0) == kSlot554B4BoolFnVtbl && u64(p, 0xb8) == 0);
        } else if (spec.kind == Slot554B4Kind::float_var) {
            CHECK(u64(p, 0) == kSlot554B4FloatFinalVptr);
            CHECK(u64(p, 0x60) == reinterpret_cast<std::uint64_t>(page.data() + spec.page_cell_offset));
            CHECK(u32(p, 0x68) == spec.high);
            CHECK(u32(p, 0x6c) == spec.low && p[0x70] == 1);
            CHECK(u32(p, 0x74) == spec.high && p[0x78] == 1);
            CHECK(u64(p, 0xc0) == 0x1594318);
        } else if (spec.kind == Slot554B4Kind::int_var) {
            CHECK(u64(p, 0) == kSlot554B4IntFinalVptr);
            CHECK(u64(p, 0x60) == reinterpret_cast<std::uint64_t>(page.data() + spec.page_cell_offset));
            CHECK(u32(p, 0x68) == spec.high);
            CHECK(u32(p, 0x6c) == spec.low && p[0x70] == 1);
            CHECK(u32(p, 0x74) == spec.high && p[0x78] == 1);
            CHECK(u64(p, 0xc0) == kSlot554B4IntClassVtbl);
        } else {
            const bool a = spec.wrapper_rva == kSlot554B4WrapperARva;
            CHECK(u64(p, 0) == (a ? kSlot554B4WrapperAFinalVptr : kSlot554B4WrapperBFinalVptr));
            CHECK(u64(p, 0x60) == reinterpret_cast<std::uint64_t>(page.data() + spec.page_cell_offset));
            CHECK(u32(p, 0x68) == 7);
            CHECK(u32(p, 0x6c) == spec.low && p[0x70] == 1 && u32(p, 0x74) == spec.high &&
                  p[0x78] == 1);
            CHECK(u64(p, 0xb0) == 0 &&
                  u64(p, 0xc0) == (a ? kSlot554B4WrapperATagVptr : kSlot554B4WrapperBTagVptr) &&
                  u64(p, 0xc8) == 0);
        }
    }
    CHECK(reg.atexit_log.size() == 36);
    for (std::size_t i = 0; i < reg.atexit_log.size(); ++i) {
        const auto& s = kSlot554B4Specs[i];
        std::uint64_t want = 0;
        if (s.kind == Slot554B4Kind::bool_var) want = kSlot554B4BoolDtorRva;
        if (s.kind == Slot554B4Kind::float_var) want = kSlot554B4FloatDtorRva;
        if (s.kind == Slot554B4Kind::int_var) want = kSlot554B4IntDtorRva;
        if (s.kind == Slot554B4Kind::wrapper)
            want = s.wrapper_rva == kSlot554B4WrapperARva ? kSlot554B4WrapperADtorRva
                                                          : kSlot554B4WrapperBDtorRva;
        CHECK(reg.atexit_log.at(i).dtor_rva == want);
        CHECK(reg.atexit_log.at(i).object == object(o, static_cast<std::uint32_t>(i)));
    }
}

void check_replay() {
    const auto page = make_page();
    Slot554Batch4Objects a{};
    std::memset(&a, kSentinel, sizeof a);
    ConCmdRegistry reg{};
    Slot554B4Hooks h = default_slot554_batch4_hooks();
    h.register_var = &no_link_register;
    CHECK(init_slot554_batch4(a, page.data(), reg, h).ok);
    Slot554Batch4Objects snapshot = a;
    CHECK(init_slot554_batch4(a, page.data(), reg, h).ok);
    CHECK(std::memcmp(&a, &snapshot, sizeof a) == 0);
}

void check_fail_closed() {
    const auto page = make_page();
    Slot554Batch4Objects o{};
    ConCmdRegistry reg{};
    Slot554B4Hooks h = default_slot554_batch4_hooks();
    CHECK(!init_slot554_batch4(o, nullptr, reg, h).ok);
    h = default_slot554_batch4_hooks(); h.register_var = nullptr;
    CHECK(init_slot554_batch4(o, page.data(), reg, h).blocked == Slot554B4Stage::page);
    h = default_slot554_batch4_hooks(); h.atexit = nullptr;
    CHECK(init_slot554_batch4(o, page.data(), reg, h).blocked == Slot554B4Stage::page);
    h = default_slot554_batch4_hooks(); h.release_view = nullptr;
    CHECK(init_slot554_batch4(o, page.data(), reg, h).blocked == Slot554B4Stage::page);
    h = default_slot554_batch4_hooks(); h.register_var = &refuse_register;
    auto r = init_slot554_batch4(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B4Stage::registration && r.built == 0);
    h = default_slot554_batch4_hooks(); h.atexit = &refuse_atexit;
    r = init_slot554_batch4(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B4Stage::atexit && r.built == 0 && r.registered == 1);
    h = default_slot554_batch4_hooks(); h.release_view = &refuse_release;
    r = init_slot554_batch4(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B4Stage::view_release && r.built == 0);
    h = default_slot554_batch4_hooks(); h.wrapper_call = &refuse_wrapper;
    r = init_slot554_batch4(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B4Stage::ctor && r.built == 3);
    h = default_slot554_batch4_hooks(); h.destroy_view = &refuse_destroy;
    r = init_slot554_batch4(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B4Stage::ctor && r.built == 3);
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
