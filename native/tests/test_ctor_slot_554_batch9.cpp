// Fixture for slot 554 batch 9: the 36 objects in [0x7f098,0x8160c).
#include "core/ctor_slot_554_batch9_7f098.hpp"

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
    std::uint64_t wrapper_rvas[2]{};
};
bool count_wrapper(void* ctx, std::uint64_t wrapper_rva, ConCmdFunctor&) noexcept {
    auto* c = static_cast<Counts*>(ctx);
    if (c->wrapper < 2) c->wrapper_rvas[c->wrapper] = wrapper_rva;
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
    for (std::uint32_t i = 0; i < kSlot554B9ObjectCount; ++i) {
        const auto& s = kSlot554B9Specs[i];
        if (s.kind == Slot554B9Kind::bool_var) page[s.page_cell_offset] = 1;
        if (s.kind == Slot554B9Kind::float_var)
            std::memcpy(page.data() + s.page_cell_offset, &s.high, sizeof s.high);
        if (s.kind == Slot554B9Kind::wrapper) {
            const std::uint32_t v = 7;
            std::memcpy(page.data() + s.page_cell_offset, &v, sizeof v);
        }
    }
    return page;
}

std::uint8_t* object(Slot554Batch9Objects& o, std::uint32_t i) noexcept {
    std::size_t offset = 0;
    for (std::uint32_t n = 0; n < i; ++n) offset += kSlot554B9Specs[n].size;
    return o.objects.data() + offset;
}

void check_constants() {
    CHECK(kSlot554B9Rva == 0x7f098 && kSlot554B9BodySize == 0x2574);
    CHECK(kSlot554B9ObjectBaseRva == 0x2eb6538 && kSlot554B9ObjectEndRva == 0x2eb8218);
    CHECK(kSlot554B9ObjectCount == 36 && kSlot554B9BlockSize == 0x1ce0);
    CHECK(kSlot554B9Written == 6150 && kSlot554B9Holes == 1242);
    CHECK(kSlot554B9InstructionCount == 1660 && kSlot554B9DirectCallCount == 178 &&
          kSlot554B9IndirectCallCount == 30 && kSlot554B9StoreCount == 506 &&
          kSlot554B9ObjectStoreCount == 110 && kSlot554B9StackStoreCount == 396);
    CHECK(kSlot554B9BoolCount == 6 && kSlot554B9FloatCount == 28 &&
          kSlot554B9WrapperCount == 2);
    CHECK(kSlot554B9TableIndex == 554 && kSlot554B9TableWordRva == 0x2dd5f90);
    CHECK(kSlot554B9ShowSettingsWrapperFinalVptr == kSlot554B9ShowSettingsWrapperTagVptr + 0x30);
    CHECK(kSlot554B9ShowSettingsValuesWrapperFinalVptr ==
          kSlot554B9ShowSettingsValuesWrapperTagVptr + 0x30);
    std::size_t written = 0;
    for (std::size_t i = 0; i < kSlot554B9BlockSize; ++i) if (slot554_batch9_written(i)) ++written;
    CHECK(written == kSlot554B9Written);
    std::size_t bool_written = 0;
    for (std::size_t i = 0; i < 0xc0; ++i) if (slot554_batch9_object_written(8, i)) ++bool_written;
    CHECK(bool_written == kSlot554B9BoolWritten);
    std::size_t float_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch9_object_written(3, i)) ++float_written;
    CHECK(float_written == kSlot554B9FloatWritten);
    std::size_t wrapper_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch9_object_written(11, i)) ++wrapper_written;
    CHECK(wrapper_written == kSlot554B9WrapperWritten);
}

void check_objects() {
    const auto page = make_page();
    const auto page_before = page;
    Slot554Batch9Objects o{};
    std::memset(&o, kSentinel, sizeof o);
    ConCmdRegistry reg{};
    Counts counts{};
    Slot554B9Hooks hooks = default_slot554_batch9_hooks();
    hooks.wrapper_call = &count_wrapper;
    hooks.destroy_view = &count_destroy;
    hooks.context = &counts;
    const Slot554Batch9Result r = init_slot554_batch9(o, page.data(), reg, hooks);
    CHECK(r.ok && r.blocked == Slot554B9Stage::none);
    CHECK(r.built == 36 && r.registered == 36 && r.atexit_records == 36 && r.views_inert == 72);
    CHECK(r.wrapper_calls == 2 && r.view_destroys == 2);
    CHECK(counts.wrapper == 2 && counts.destroy == 2);
    CHECK(counts.wrapper_rvas[0] == kSlot554B9ShowSettingsWrapperRva);
    CHECK(counts.wrapper_rvas[1] == kSlot554B9ShowSettingsValuesWrapperRva);
    CHECK(page == page_before);

    for (std::uint32_t i = 0; i < kSlot554B9ObjectCount; ++i) {
        const auto& spec = kSlot554B9Specs[i];
        const std::uint8_t* p = object(o, i);
        for (std::size_t j = 0; j < spec.size; ++j) {
            if (!slot554_batch9_object_written(i, j)) CHECK(p[j] == kSentinel);
        }
        switch (spec.kind) {
            case Slot554B9Kind::bool_var: {
                const auto* v = reinterpret_cast<const Slot554B2ABool*>(p);
                CHECK(v->base.vptr == kSlot554B9BoolFinalVptr);
                CHECK(v->value_ref == page.data() + spec.page_cell_offset);
                CHECK(v->default_value == 1);
                CHECK(v->fn_vptr == kSlot554B9BoolFnVtbl);
                break;
            }
            case Slot554B9Kind::float_var: {
                const auto* v = reinterpret_cast<const ConCmdRangedVar*>(p);
                CHECK(v->base.vptr == kSlot554B9FloatFinalVptr);
                CHECK(v->value_ref == page.data() + spec.page_cell_offset);
                std::uint32_t def = 0;
                std::memcpy(&def, page.data() + spec.page_cell_offset, sizeof def);
                CHECK(v->default_value == def);
                break;
            }
            case Slot554B9Kind::wrapper: {
                const auto* v = reinterpret_cast<const ConCmdFunctor*>(p);
                CHECK(v->base.vptr == spec.final_vptr);
                CHECK(v->tag_c0 == spec.tag_vptr);
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
    Slot554Batch9Objects a{};
    std::memset(&a, kSentinel, sizeof a);
    ConCmdRegistry reg{};
    Slot554B9Hooks h = default_slot554_batch9_hooks();
    h.register_var = &no_link_register;
    CHECK(init_slot554_batch9(a, page.data(), reg, h).ok);
    const Slot554Batch9Objects snapshot = a;
    CHECK(init_slot554_batch9(a, page.data(), reg, h).ok);
    CHECK(std::memcmp(&a, &snapshot, sizeof a) == 0);
}

void check_fail_closed() {
    const auto page = make_page();
    {
        Slot554Batch9Objects o{};
        ConCmdRegistry reg{};
        Slot554B9Hooks h = default_slot554_batch9_hooks();
        h.register_var = &refuse_register;
        const Slot554Batch9Result r = init_slot554_batch9(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked == Slot554B9Stage::registration && r.built == 0);
    }
    {
        Slot554Batch9Objects o{};
        ConCmdRegistry reg{};
        Slot554B9Hooks h = default_slot554_batch9_hooks();
        h.atexit = &refuse_atexit;
        const Slot554Batch9Result r = init_slot554_batch9(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked == Slot554B9Stage::atexit && r.built == 0);
    }
    {
        Slot554Batch9Objects o{};
        ConCmdRegistry reg{};
        Slot554B9Hooks h = default_slot554_batch9_hooks();
        h.release_view = &refuse_release;
        const Slot554Batch9Result r = init_slot554_batch9(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked == Slot554B9Stage::view_release);
    }
    {
        Slot554Batch9Objects o{};
        ConCmdRegistry reg{};
        Slot554B9Hooks h = default_slot554_batch9_hooks();
        h.wrapper_call = &refuse_wrapper;
        const Slot554Batch9Result r = init_slot554_batch9(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked_index == 11 && r.built == 11);
    }
    {
        Slot554Batch9Objects o{};
        ConCmdRegistry reg{};
        Slot554B9Hooks h = default_slot554_batch9_hooks();
        h.destroy_view = &refuse_destroy;
        const Slot554Batch9Result r = init_slot554_batch9(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked_index == 11 && r.wrapper_calls == 0);
    }
    {
        Slot554Batch9Objects o{};
        ConCmdRegistry reg{};
        const Slot554Batch9Result r =
            init_slot554_batch9(o, nullptr, reg, default_slot554_batch9_hooks());
        CHECK(!r.ok && r.blocked == Slot554B9Stage::page);
    }
}

}  // namespace

int main() {
    check_constants();
    check_objects();
    check_replay();
    check_fail_closed();
    if (failures != 0) {
        std::fprintf(stderr, "ctor_slot_554_batch9: %d failure(s)\n", failures);
        return 1;
    }
    std::printf("ctor_slot_554_batch9: ok (36 objects, 6150/1242 bytes)\n");
    return 0;
}
