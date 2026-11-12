// Fixture for slot 554 batch 10: the 36 objects in [0x8160c,0x83ad1).
#include "core/ctor_slot_554_batch10_8160c.hpp"

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
bool refuse_wrapper(void*, std::uint64_t, ConCmdFunctor&) noexcept { return false; }
bool refuse_destroy(void*, const ConCmdFunctorView&) noexcept { return false; }

struct Counts {
    unsigned wrapper = 0;
    unsigned destroy = 0;
    std::uint64_t wrapper_rva = 0;
};
bool count_wrapper(void* ctx, std::uint64_t wrapper_rva, ConCmdFunctor&) noexcept {
    auto* c = static_cast<Counts*>(ctx);
    c->wrapper_rva = wrapper_rva;
    ++c->wrapper;
    return true;
}
bool count_destroy(void* ctx, const ConCmdFunctorView&) noexcept {
    ++static_cast<Counts*>(ctx)->destroy;
    return true;
}

std::array<std::uint8_t, 0xac8> make_page() {
    std::array<std::uint8_t, 0xac8> page{};
    page.fill(kSentinel);
    for (std::uint32_t i = 0; i < kSlot554B10ObjectCount; ++i) {
        const auto& s = kSlot554B10Specs[i];
        if (s.kind == Slot554B10Kind::bool_var) page[s.page_cell_offset] = 1;
        if (s.kind == Slot554B10Kind::float_var ||
            s.kind == Slot554B10Kind::unbounded_float_var) {
            const std::uint32_t v = s.high != 0 ? s.high : 0x3f800000u;
            std::memcpy(page.data() + s.page_cell_offset, &v, sizeof v);
        }
        if (s.kind == Slot554B10Kind::int_var) {
            const std::uint32_t v = 5;
            std::memcpy(page.data() + s.page_cell_offset, &v, sizeof v);
        }
        if (s.kind == Slot554B10Kind::wrapper) {
            const std::uint32_t v = 7;
            std::memcpy(page.data() + s.page_cell_offset, &v, sizeof v);
        }
    }
    return page;
}

std::uint8_t* object(Slot554Batch10Objects& o, std::uint32_t i) noexcept {
    std::size_t offset = 0;
    for (std::uint32_t n = 0; n < i; ++n) offset += kSlot554B10Specs[n].size;
    return o.objects.data() + offset;
}

void check_constants() {
    CHECK(kSlot554B10Rva == 0x8160c && kSlot554B10BodySize == 0x24c5);
    CHECK(kSlot554B10ObjectBaseRva == 0x2eb8218 && kSlot554B10ObjectEndRva == 0x2eb9ee8);
    CHECK(kSlot554B10ObjectCount == 36 && kSlot554B10BlockSize == 0x1cd0);
    CHECK(kSlot554B10Written == 6081 && kSlot554B10Holes == 1295);
    CHECK(kSlot554B10InstructionCount == 1641 && kSlot554B10DirectCallCount == 179 &&
          kSlot554B10IndirectCallCount == 29 && kSlot554B10StoreCount == 497 &&
          kSlot554B10ObjectStoreCount == 119 && kSlot554B10StackStoreCount == 378);
    CHECK(kSlot554B10BoolCount == 7 && kSlot554B10FloatCount == 22 &&
          kSlot554B10UnboundedFloatCount == 4 && kSlot554B10IntCount == 2 &&
          kSlot554B10WrapperCount == 1);
    CHECK(kSlot554B10TableIndex == 554 && kSlot554B10TableWordRva == 0x2dd5f90);
    CHECK(kSlot554B10WrapperFinalVptr == kSlot554B10WrapperTagVptr + 0x30);
    std::size_t written = 0;
    for (std::size_t i = 0; i < kSlot554B10BlockSize; ++i) if (slot554_batch10_written(i)) ++written;
    CHECK(written == kSlot554B10Written);
    std::size_t bool_written = 0;
    for (std::size_t i = 0; i < 0xc0; ++i) if (slot554_batch10_object_written(0, i)) ++bool_written;
    CHECK(bool_written == kSlot554B10BoolWritten);
    std::size_t float_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch10_object_written(1, i)) ++float_written;
    CHECK(float_written == kSlot554B10FloatWritten);
    std::size_t ubfloat_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch10_object_written(19, i)) ++ubfloat_written;
    CHECK(ubfloat_written == kSlot554B10UnboundedFloatWritten);
    std::size_t int_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch10_object_written(27, i)) ++int_written;
    CHECK(int_written == kSlot554B10IntWritten);
    std::size_t wrapper_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch10_object_written(24, i)) ++wrapper_written;
    CHECK(wrapper_written == kSlot554B10WrapperWritten);
}

void check_objects() {
    const auto page = make_page();
    const auto page_before = page;
    Slot554Batch10Objects o{};
    std::memset(&o, kSentinel, sizeof o);
    ConCmdRegistry reg{};
    Counts counts{};
    Slot554B10Hooks hooks = default_slot554_batch10_hooks();
    hooks.wrapper_call = &count_wrapper;
    hooks.destroy_view = &count_destroy;
    hooks.context = &counts;
    const Slot554Batch10Result r = init_slot554_batch10(o, page.data(), reg, hooks);
    CHECK(r.ok && r.blocked == Slot554B10Stage::none);
    CHECK(r.built == 36 && r.registered == 36 && r.atexit_records == 36 && r.views_inert == 72);
    CHECK(r.wrapper_calls == 1 && r.view_destroys == 1);
    CHECK(counts.wrapper == 1 && counts.destroy == 1);
    CHECK(counts.wrapper_rva == kSlot554B10WrapperRva);
    CHECK(page == page_before);

    for (std::uint32_t i = 0; i < kSlot554B10ObjectCount; ++i) {
        const auto& spec = kSlot554B10Specs[i];
        const std::uint8_t* p = object(o, i);
        for (std::size_t j = 0; j < spec.size; ++j) {
            if (!slot554_batch10_object_written(i, j)) CHECK(p[j] == kSentinel);
        }
        switch (spec.kind) {
            case Slot554B10Kind::bool_var: {
                const auto* v = reinterpret_cast<const Slot554B2ABool*>(p);
                CHECK(v->base.vptr == kSlot554B10BoolFinalVptr);
                CHECK(v->value_ref == page.data() + spec.page_cell_offset);
                CHECK(v->default_value == 1);
                CHECK(v->fn_vptr == kSlot554B10BoolFnVtbl);
                break;
            }
            case Slot554B10Kind::float_var:
            case Slot554B10Kind::unbounded_float_var:
            case Slot554B10Kind::int_var: {
                const auto* v = reinterpret_cast<const ConCmdRangedVar*>(p);
                const std::uint64_t want = spec.kind == Slot554B10Kind::int_var
                                               ? kSlot554B10IntFinalVptr
                                               : kSlot554B10FloatFinalVptr;
                CHECK(v->base.vptr == want);
                CHECK(v->value_ref == page.data() + spec.page_cell_offset);
                std::uint32_t def = 0;
                std::memcpy(&def, page.data() + spec.page_cell_offset, sizeof def);
                CHECK(v->default_value == def);
                CHECK(v->fn_vptr == (spec.kind == Slot554B10Kind::int_var
                                         ? kSlot554B10IntClassVtbl
                                         : kSlot554B10FloatClassVtbl));
                if (spec.kind == Slot554B10Kind::float_var) {
                    CHECK(v->min_engaged == 1 && v->max_engaged == 1);
                    std::uint32_t lo = 0, hi = 0;
                    std::memcpy(&lo, v->min_value, 4);
                    std::memcpy(&hi, v->max_value, 4);
                    CHECK(lo == spec.low && hi == spec.high);
                }
                break;
            }
            case Slot554B10Kind::wrapper: {
                const auto* v = reinterpret_cast<const ConCmdFunctor*>(p);
                CHECK(v->base.vptr == kSlot554B10WrapperFinalVptr);
                CHECK(v->tag_c0 == kSlot554B10WrapperTagVptr);
                CHECK(v->spec == reinterpret_cast<std::uint64_t>(page.data() +
                                                                 spec.page_cell_offset));
                CHECK(v->low == spec.low && v->high == spec.high);
                CHECK(v->one70 == 1 && v->one78 == 1);
                CHECK(v->spec_word == 7);
                CHECK(v->tag_c8 == 0);
                break;
            }
        }
    }
}

void check_replay() {
    const auto page = make_page();
    Slot554Batch10Objects a{};
    std::memset(&a, kSentinel, sizeof a);
    ConCmdRegistry reg{};
    Slot554B10Hooks h = default_slot554_batch10_hooks();
    h.register_var = &no_link_register;
    CHECK(init_slot554_batch10(a, page.data(), reg, h).ok);
    const Slot554Batch10Objects snapshot = a;
    CHECK(init_slot554_batch10(a, page.data(), reg, h).ok);
    CHECK(std::memcmp(&a, &snapshot, sizeof a) == 0);
}

void check_fail_closed() {
    const auto page = make_page();
    {
        Slot554Batch10Objects o{};
        ConCmdRegistry reg{};
        Slot554B10Hooks h = default_slot554_batch10_hooks();
        h.register_var = &refuse_register;
        const Slot554Batch10Result r = init_slot554_batch10(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked == Slot554B10Stage::registration && r.built == 0);
    }
    {
        Slot554Batch10Objects o{};
        ConCmdRegistry reg{};
        Slot554B10Hooks h = default_slot554_batch10_hooks();
        h.atexit = &refuse_atexit;
        const Slot554Batch10Result r = init_slot554_batch10(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked == Slot554B10Stage::atexit && r.built == 0);
    }
    {
        Slot554Batch10Objects o{};
        ConCmdRegistry reg{};
        Slot554B10Hooks h = default_slot554_batch10_hooks();
        h.release_view = &refuse_release;
        const Slot554Batch10Result r = init_slot554_batch10(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked == Slot554B10Stage::view_release);
    }
    {
        Slot554Batch10Objects o{};
        ConCmdRegistry reg{};
        Counts c{};
        Slot554B10Hooks h = default_slot554_batch10_hooks();
        h.wrapper_call = &refuse_wrapper;
        h.context = &c;
        const Slot554Batch10Result r = init_slot554_batch10(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked_index == 24 && r.built == 24 && r.wrapper_calls == 0);
    }
    {
        Slot554Batch10Objects o{};
        ConCmdRegistry reg{};
        Counts c{};
        Slot554B10Hooks h = default_slot554_batch10_hooks();
        h.wrapper_call = &count_wrapper;
        h.destroy_view = &refuse_destroy;
        h.context = &c;
        const Slot554Batch10Result r = init_slot554_batch10(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked_index == 24 && r.built == 24 && r.wrapper_calls == 1);
        CHECK(c.wrapper == 1);
    }
    {
        Slot554Batch10Objects o{};
        ConCmdRegistry reg{};
        const Slot554Batch10Result r =
            init_slot554_batch10(o, nullptr, reg, default_slot554_batch10_hooks());
        CHECK(!r.ok && r.blocked == Slot554B10Stage::page);
    }
}

}  // namespace

int main() {
    check_constants();
    check_objects();
    check_replay();
    check_fail_closed();
    if (failures != 0) {
        std::fprintf(stderr, "ctor_slot_554_batch10: %d failure(s)\n", failures);
        return 1;
    }
    std::printf("ctor_slot_554_batch10: ok (36 objects, 6081/1295 bytes)\n");
    return 0;
}
