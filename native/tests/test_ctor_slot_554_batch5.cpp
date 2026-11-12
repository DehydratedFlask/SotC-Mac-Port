// Fixture for slot 554 batch 5: the 36 objects in [0x75d22,0x781c5).
#include "core/ctor_slot_554_batch5_75d22.hpp"

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
    for (std::uint32_t i = 0; i < kSlot554B5ObjectCount; ++i) {
        const auto& s = kSlot554B5Specs[i];
        if (s.kind == Slot554B5Kind::bool_var) page[s.page_cell_offset] = 1;
        if (s.kind == Slot554B5Kind::float_var)
            std::memcpy(page.data() + s.page_cell_offset, &s.high, sizeof s.high);
        if (s.kind == Slot554B5Kind::wrapper) {
            const std::uint32_t v = 7;
            std::memcpy(page.data() + s.page_cell_offset, &v, sizeof v);
        }
    }
    return page;
}

std::uint8_t* object(Slot554Batch5Objects& o, std::uint32_t i) noexcept {
    std::size_t offset = 0;
    for (std::uint32_t n = 0; n < i; ++n) offset += kSlot554B5Specs[n].size;
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
    CHECK(kSlot554B5Rva == 0x75d22 && kSlot554B5BodySize == 0x24a3);
    CHECK(kSlot554B5ObjectBaseRva == 0x2eaf2a0 && kSlot554B5ObjectEndRva == 0x2eb0f10);
    CHECK(kSlot554B5ObjectCount == 36 && kSlot554B5BlockSize == 0x1c70);
    CHECK(kSlot554B5Written == 6063 && kSlot554B5Holes == 1217);
    CHECK(kSlot554B5InstructionCount == 1628 && kSlot554B5DirectCallCount == 179 &&
          kSlot554B5IndirectCallCount == 23 && kSlot554B5StoreCount == 516 &&
          kSlot554B5ObjectStoreCount == 161 && kSlot554B5StackStoreCount == 355);
    CHECK(kSlot554B5BoolCount == 13 && kSlot554B5FloatCount == 22 &&
          kSlot554B5WrapperCount == 1);
    CHECK(kSlot554B5TableIndex == 554 && kSlot554B5TableWordRva == 0x2dd5f90);
    std::size_t written = 0;
    for (std::size_t i = 0; i < kSlot554B5BlockSize; ++i) if (slot554_batch5_written(i)) ++written;
    CHECK(written == kSlot554B5Written);
    std::size_t bool_written = 0;
    for (std::size_t i = 0; i < 0xc0; ++i) if (slot554_batch5_object_written(0, i)) ++bool_written;
    CHECK(bool_written == kSlot554B5BoolWritten);
    std::size_t float_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch5_object_written(8, i)) ++float_written;
    CHECK(float_written == kSlot554B5FloatWritten);
    std::size_t wrapper_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch5_object_written(7, i)) ++wrapper_written;
    CHECK(wrapper_written == kSlot554B5WrapperWritten);
}

void check_objects() {
    const auto page = make_page();
    const auto page_before = page;
    Slot554Batch5Objects o{};
    std::memset(&o, kSentinel, sizeof o);
    ConCmdRegistry reg{};
    Counts counts{};
    Slot554B5Hooks hooks = default_slot554_batch5_hooks();
    hooks.wrapper_call = &count_wrapper;
    hooks.destroy_view = &count_destroy;
    hooks.context = &counts;
    const Slot554Batch5Result r = init_slot554_batch5(o, page.data(), reg, hooks);
    CHECK(r.ok && r.blocked == Slot554B5Stage::none);
    CHECK(r.built == 36 && r.registered == 36 && r.atexit_records == 36 && r.views_inert == 72);
    CHECK(r.wrapper_calls == 1 && r.view_destroys == 1);
    CHECK(counts.wrapper == 1 && counts.destroy == 1);
    CHECK(page == page_before);

    for (std::uint32_t i = 0; i < kSlot554B5ObjectCount; ++i) {
        const auto& spec = kSlot554B5Specs[i];
        const std::uint8_t* p = object(o, i);
        for (std::size_t j = 0; j < spec.size; ++j) {
            if (!slot554_batch5_object_written(i, j)) CHECK(p[j] == kSentinel);
        }
        const SharedStr& name = *reinterpret_cast<const SharedStr*>(p + 0x20);
        CHECK(name.size == spec.name_length);
        CHECK(std::memcmp(name.data, spec.name, name.size) == 0);
        if (spec.kind == Slot554B5Kind::bool_var) {
            CHECK(u64(p, 0) == kSlot554B5BoolFinalVptr);
            CHECK(u64(p, 0x60) == reinterpret_cast<std::uint64_t>(page.data() + spec.page_cell_offset));
            CHECK(p[0x68] == 1 && u32(p, 0x69) == 0);
            CHECK(u64(p, 0xa0) == 0 && u64(p, 0xb0) == kSlot554B5BoolFnVtbl && u64(p, 0xb8) == 0);
        } else if (spec.kind == Slot554B5Kind::float_var) {
            CHECK(u64(p, 0) == kSlot554B5FloatFinalVptr);
            CHECK(u64(p, 0x60) == reinterpret_cast<std::uint64_t>(page.data() + spec.page_cell_offset));
            CHECK(u32(p, 0x68) == spec.high);
            CHECK(u32(p, 0x6c) == spec.low && p[0x70] == 1);
            CHECK(u32(p, 0x74) == spec.high && p[0x78] == 1);
            CHECK(u64(p, 0xc0) == kSlot554B5FloatClassVtbl);
        } else {
            CHECK(u64(p, 0) == kSlot554B5WrapperFinalVptr);
            CHECK(u64(p, 0x60) == reinterpret_cast<std::uint64_t>(page.data() + spec.page_cell_offset));
            CHECK(u32(p, 0x68) == 7);
            CHECK(u32(p, 0x6c) == spec.low && p[0x70] == 1 && u32(p, 0x74) == spec.high &&
                  p[0x78] == 1);
            CHECK(u64(p, 0xb0) == 0 && u64(p, 0xc0) == kSlot554B5WrapperTagVptr &&
                  u64(p, 0xc8) == 0);
        }
    }
    CHECK(reg.atexit_log.size() == 36);
    for (std::size_t i = 0; i < reg.atexit_log.size(); ++i) {
        const auto& s = kSlot554B5Specs[i];
        std::uint64_t want = 0;
        if (s.kind == Slot554B5Kind::bool_var) want = kSlot554B5BoolDtorRva;
        if (s.kind == Slot554B5Kind::float_var) want = kSlot554B5FloatDtorRva;
        if (s.kind == Slot554B5Kind::wrapper) want = kSlot554B5WrapperDtorRva;
        CHECK(reg.atexit_log.at(i).dtor_rva == want);
        CHECK(reg.atexit_log.at(i).object == object(o, static_cast<std::uint32_t>(i)));
    }
}

void check_replay() {
    const auto page = make_page();
    Slot554Batch5Objects a{};
    std::memset(&a, kSentinel, sizeof a);
    ConCmdRegistry reg{};
    Slot554B5Hooks h = default_slot554_batch5_hooks();
    h.register_var = &no_link_register;
    CHECK(init_slot554_batch5(a, page.data(), reg, h).ok);
    Slot554Batch5Objects snapshot = a;
    CHECK(init_slot554_batch5(a, page.data(), reg, h).ok);
    CHECK(std::memcmp(&a, &snapshot, sizeof a) == 0);
}

void check_fail_closed() {
    const auto page = make_page();
    Slot554Batch5Objects o{};
    ConCmdRegistry reg{};
    Slot554B5Hooks h = default_slot554_batch5_hooks();
    CHECK(!init_slot554_batch5(o, nullptr, reg, h).ok);
    h = default_slot554_batch5_hooks(); h.register_var = nullptr;
    CHECK(init_slot554_batch5(o, page.data(), reg, h).blocked == Slot554B5Stage::page);
    h = default_slot554_batch5_hooks(); h.atexit = nullptr;
    CHECK(init_slot554_batch5(o, page.data(), reg, h).blocked == Slot554B5Stage::page);
    h = default_slot554_batch5_hooks(); h.release_view = nullptr;
    CHECK(init_slot554_batch5(o, page.data(), reg, h).blocked == Slot554B5Stage::page);
    h = default_slot554_batch5_hooks(); h.register_var = &refuse_register;
    auto r = init_slot554_batch5(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B5Stage::registration && r.built == 0);
    h = default_slot554_batch5_hooks(); h.atexit = &refuse_atexit;
    r = init_slot554_batch5(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B5Stage::atexit && r.built == 0 && r.registered == 1);
    h = default_slot554_batch5_hooks(); h.release_view = &refuse_release;
    r = init_slot554_batch5(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B5Stage::view_release && r.built == 0);
    h = default_slot554_batch5_hooks(); h.wrapper_call = &refuse_wrapper;
    r = init_slot554_batch5(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B5Stage::ctor && r.built == 7);
    h = default_slot554_batch5_hooks(); h.destroy_view = &refuse_destroy;
    r = init_slot554_batch5(o, page.data(), reg, h);
    CHECK(!r.ok && r.blocked == Slot554B5Stage::ctor && r.built == 7);
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
