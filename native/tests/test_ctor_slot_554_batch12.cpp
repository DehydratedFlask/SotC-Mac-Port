// Fixture for slot 554 batch 12: the 36 objects in [0x86052,0x8849d).
#include "core/ctor_slot_554_batch12_86052.hpp"

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
    std::uint64_t rvas[2] = {};
};
bool count_wrapper(void* ctx, std::uint64_t wrapper_rva, ConCmdFunctor&) noexcept {
    auto* c = static_cast<Counts*>(ctx);
    if (c->wrapper < 2) c->rvas[c->wrapper] = wrapper_rva;
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
    for (std::uint32_t i = 0; i < kSlot554B12ObjectCount; ++i) {
        const auto& s = kSlot554B12Specs[i];
        if (s.kind == Slot554B12Kind::bool_var) page[s.page_cell_offset] = 1;
        if (s.kind == Slot554B12Kind::float_var) {
            const std::uint32_t v = s.high != 0 ? s.high : 0x3f800000u;
            std::memcpy(page.data() + s.page_cell_offset, &v, sizeof v);
        }
        if (s.kind == Slot554B12Kind::dword_var) {
            const std::uint32_t v = 5;
            std::memcpy(page.data() + s.page_cell_offset, &v, sizeof v);
        }
        if (s.kind == Slot554B12Kind::wrapper) {
            const std::uint32_t v = 7;
            std::memcpy(page.data() + s.page_cell_offset, &v, sizeof v);
        }
    }
    return page;
}

std::uint8_t* object(Slot554Batch12Objects& o, std::uint32_t i) noexcept {
    std::size_t offset = 0;
    for (std::uint32_t n = 0; n < i; ++n) offset += kSlot554B12Specs[n].size;
    return o.objects.data() + offset;
}

void check_constants() {
    CHECK(kSlot554B12Rva == 0x86052 && kSlot554B12BodySize == 0x244b);
    CHECK(kSlot554B12ObjectBaseRva == 0x2ebbbc8 && kSlot554B12ObjectEndRva == 0x2ebd7f8);
    CHECK(kSlot554B12ObjectCount == 36 && kSlot554B12BlockSize == 0x1c30);
    CHECK(kSlot554B12Written == 6051 && kSlot554B12Holes == 1165);
    CHECK(kSlot554B12InstructionCount == 1613 && kSlot554B12DirectCallCount == 180 &&
          kSlot554B12IndirectCallCount == 19 && kSlot554B12StoreCount == 518 &&
          kSlot554B12ObjectStoreCount == 191 && kSlot554B12StackStoreCount == 327);
    CHECK(kSlot554B12BoolCount == 17 && kSlot554B12FloatCount == 17 &&
          kSlot554B12DwordCount == 0 && kSlot554B12WrapperCount == 2);
    CHECK(kSlot554B12TableIndex == 554 && kSlot554B12TableWordRva == 0x2dd5f90);
    CHECK(kSlot554B12FurStatsGetRva == 0x6e990 && kSlot554B12FurStatsSetRva == 0x6e9b0);
    CHECK(kSlot554B12FurDebugGetRva == 0x6eb80 && kSlot554B12FurDebugSetRva == 0x6eba0);
    CHECK(kSlot554B12FurStatsFinalVptr == 0x15972d0 && kSlot554B12FurDebugFinalVptr == 0x1597368);
    std::size_t written = 0;
    for (std::size_t i = 0; i < kSlot554B12BlockSize; ++i) if (slot554_batch12_written(i)) ++written;
    CHECK(written == kSlot554B12Written);
    std::size_t bool_written = 0;
    for (std::size_t i = 0; i < 0xc0; ++i) if (slot554_batch12_object_written(0, i)) ++bool_written;
    CHECK(bool_written == kSlot554B12BoolWritten);
    std::size_t float_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch12_object_written(1, i)) ++float_written;
    CHECK(float_written == kSlot554B12FloatWritten);
    CHECK(kSlot554B12DwordCount == 0 && kSlot554B12DwordWritten == 0);
    std::size_t wrapper_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch12_object_written(30, i)) ++wrapper_written;
    CHECK(wrapper_written == kSlot554B12WrapperWritten);
}

void check_objects() {
    const auto page = make_page();
    const auto page_before = page;
    Slot554Batch12Objects o{};
    std::memset(&o, kSentinel, sizeof o);
    ConCmdRegistry reg{};
    Counts counts{};
    Slot554B12Hooks hooks = default_slot554_batch12_hooks();
    hooks.wrapper_call = &count_wrapper;
    hooks.destroy_view = &count_destroy;
    hooks.context = &counts;
    const Slot554Batch12Result r = init_slot554_batch12(o, page.data(), reg, hooks);
    CHECK(r.ok && r.blocked == Slot554B12Stage::none);
    CHECK(r.built == 36 && r.registered == 36 && r.atexit_records == 36 && r.views_inert == 72);
    CHECK(r.wrapper_calls == 2 && r.view_destroys == 2);
    CHECK(counts.wrapper == 2 && counts.destroy == 2);
    CHECK(counts.rvas[0] == kSlot554B12FurStatsOuterRva);
    CHECK(counts.rvas[1] == kSlot554B12FurDebugOuterRva);
    CHECK(page == page_before);

    for (std::uint32_t i = 0; i < kSlot554B12ObjectCount; ++i) {
        const auto& spec = kSlot554B12Specs[i];
        const std::uint8_t* p = object(o, i);
        for (std::size_t j = 0; j < spec.size; ++j) {
            if (!slot554_batch12_object_written(i, j)) CHECK(p[j] == kSentinel);
        }
        switch (spec.kind) {
            case Slot554B12Kind::bool_var: {
                const auto* v = reinterpret_cast<const Slot554B2ABool*>(p);
                CHECK(v->base.vptr == kSlot554B12BoolFinalVptr);
                CHECK(v->value_ref == page.data() + spec.page_cell_offset);
                CHECK(v->default_value == 1);
                CHECK(v->fn_vptr == kSlot554B12BoolFnVtbl);
                break;
            }
            case Slot554B12Kind::float_var:
            case Slot554B12Kind::dword_var: {
                const auto* v = reinterpret_cast<const ConCmdRangedVar*>(p);
                const bool dword = spec.kind == Slot554B12Kind::dword_var;
                CHECK(v->base.vptr == (dword ? kSlot554B12DwordFinalVptr
                                             : kSlot554B12FloatFinalVptr));
                CHECK(v->value_ref == page.data() + spec.page_cell_offset);
                std::uint32_t def = 0;
                std::memcpy(&def, page.data() + spec.page_cell_offset, sizeof def);
                CHECK(v->default_value == def);
                CHECK(v->fn_vptr == (dword ? kSlot554B12DwordClassVtbl
                                           : kSlot554B12FloatClassVtbl));
                CHECK(v->min_engaged == 1 && v->max_engaged == 1);
                std::uint32_t lo = 0, hi = 0;
                std::memcpy(&lo, v->min_value, 4);
                std::memcpy(&hi, v->max_value, 4);
                CHECK(lo == spec.low && hi == spec.high);
                break;
            }
            case Slot554B12Kind::wrapper: {
                const auto* v = reinterpret_cast<const ConCmdFunctor*>(p);
                CHECK(v->base.vptr == spec.final_vptr);
                CHECK(v->tag_c0 == spec.tag_vptr);
                CHECK(v->spec == reinterpret_cast<std::uint64_t>(page.data() +
                                                                 spec.page_cell_offset));
                CHECK(v->low == spec.low && v->high == spec.high);
                CHECK(v->one70 == 0 && v->one78 == 0);
                CHECK(v->spec_word == 7);
                CHECK(v->tag_c8 == 0);
                break;
            }
        }
    }
}

void check_replay() {
    const auto page = make_page();
    Slot554Batch12Objects a{};
    std::memset(&a, kSentinel, sizeof a);
    ConCmdRegistry reg{};
    Slot554B12Hooks h = default_slot554_batch12_hooks();
    h.register_var = &no_link_register;
    CHECK(init_slot554_batch12(a, page.data(), reg, h).ok);
    const Slot554Batch12Objects snapshot = a;
    CHECK(init_slot554_batch12(a, page.data(), reg, h).ok);
    CHECK(std::memcmp(&a, &snapshot, sizeof a) == 0);
}

void check_fail_closed() {
    const auto page = make_page();
    {
        Slot554Batch12Objects o{};
        ConCmdRegistry reg{};
        Slot554B12Hooks h = default_slot554_batch12_hooks();
        h.register_var = &refuse_register;
        const Slot554Batch12Result r = init_slot554_batch12(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked == Slot554B12Stage::registration && r.built == 0);
    }
    {
        Slot554Batch12Objects o{};
        ConCmdRegistry reg{};
        Slot554B12Hooks h = default_slot554_batch12_hooks();
        h.atexit = &refuse_atexit;
        const Slot554Batch12Result r = init_slot554_batch12(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked == Slot554B12Stage::atexit && r.built == 0);
    }
    {
        Slot554Batch12Objects o{};
        ConCmdRegistry reg{};
        Slot554B12Hooks h = default_slot554_batch12_hooks();
        h.release_view = &refuse_release;
        const Slot554Batch12Result r = init_slot554_batch12(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked == Slot554B12Stage::view_release);
    }
    {
        Slot554Batch12Objects o{};
        ConCmdRegistry reg{};
        Counts c{};
        Slot554B12Hooks h = default_slot554_batch12_hooks();
        h.wrapper_call = &refuse_wrapper;
        h.context = &c;
        const Slot554Batch12Result r = init_slot554_batch12(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked_index == 30 && r.built == 30 && r.wrapper_calls == 0);
    }
    {
        Slot554Batch12Objects o{};
        ConCmdRegistry reg{};
        Counts c{};
        Slot554B12Hooks h = default_slot554_batch12_hooks();
        h.wrapper_call = &count_wrapper;
        h.destroy_view = &refuse_destroy;
        h.context = &c;
        const Slot554Batch12Result r = init_slot554_batch12(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked_index == 30 && r.built == 30 && r.wrapper_calls == 1);
        CHECK(c.wrapper == 1);
    }
    {
        Slot554Batch12Objects o{};
        ConCmdRegistry reg{};
        const Slot554Batch12Result r =
            init_slot554_batch12(o, nullptr, reg, default_slot554_batch12_hooks());
        CHECK(!r.ok && r.blocked == Slot554B12Stage::page);
    }
}

}  // namespace

int main() {
    check_constants();
    check_objects();
    check_replay();
    check_fail_closed();
    if (failures != 0) {
        std::fprintf(stderr, "ctor_slot_554_batch12: %d failure(s)\n", failures);
        return 1;
    }
    std::printf("ctor_slot_554_batch12: ok (36 objects, 6051/1165 bytes)\n");
    return 0;
}
