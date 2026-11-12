// Fixture for slot 554 batch 8: the 36 objects in [0x7cb5b,0x7f098).
#include "core/ctor_slot_554_batch8_7cb5b.hpp"

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
    std::uint64_t wrapper_rvas[3]{};
};
bool count_wrapper(void* ctx, std::uint64_t wrapper_rva, ConCmdFunctor&) noexcept {
    auto* c = static_cast<Counts*>(ctx);
    if (c->wrapper < 3) c->wrapper_rvas[c->wrapper] = wrapper_rva;
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
    for (std::uint32_t i = 0; i < kSlot554B8ObjectCount; ++i) {
        const auto& s = kSlot554B8Specs[i];
        if (s.kind == Slot554B8Kind::bool_var) page[s.page_cell_offset] = 1;
        if (s.kind == Slot554B8Kind::float_var)
            std::memcpy(page.data() + s.page_cell_offset, &s.high, sizeof s.high);
        if (s.kind == Slot554B8Kind::wrapper) {
            const std::uint32_t v = 7;
            std::memcpy(page.data() + s.page_cell_offset, &v, sizeof v);
        }
    }
    return page;
}

std::uint8_t* object(Slot554Batch8Objects& o, std::uint32_t i) noexcept {
    std::size_t offset = 0;
    for (std::uint32_t n = 0; n < i; ++n) offset += kSlot554B8Specs[n].size;
    return o.objects.data() + offset;
}

void check_constants() {
    CHECK(kSlot554B8Rva == 0x7cb5b && kSlot554B8BodySize == 0x253d);
    CHECK(kSlot554B8ObjectBaseRva == 0x2eb4868 && kSlot554B8ObjectEndRva == 0x2eb6538);
    CHECK(kSlot554B8ObjectCount == 36 && kSlot554B8BlockSize == 0x1cd0);
    CHECK(kSlot554B8Written == 6165 && kSlot554B8Holes == 1211);
    CHECK(kSlot554B8InstructionCount == 1659 && kSlot554B8DirectCallCount == 177 &&
          kSlot554B8IndirectCallCount == 29 && kSlot554B8StoreCount == 505 &&
          kSlot554B8ObjectStoreCount == 115 && kSlot554B8StackStoreCount == 390);
    CHECK(kSlot554B8BoolCount == 7 && kSlot554B8FloatCount == 26 &&
          kSlot554B8WrapperCount == 3);
    CHECK(kSlot554B8TableIndex == 554 && kSlot554B8TableWordRva == 0x2dd5f90);
    CHECK(kSlot554B8WaterWrapperFinalVptr == 0x1596d78 + 0x30);
    CHECK(kSlot554B8TonemapWrapperFinalVptr == 0x1596fd8 + 0x30);
    std::size_t written = 0;
    for (std::size_t i = 0; i < kSlot554B8BlockSize; ++i) if (slot554_batch8_written(i)) ++written;
    CHECK(written == kSlot554B8Written);
    std::size_t bool_written = 0;
    for (std::size_t i = 0; i < 0xc0; ++i) if (slot554_batch8_object_written(0, i)) ++bool_written;
    CHECK(bool_written == kSlot554B8BoolWritten);
    std::size_t float_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch8_object_written(3, i)) ++float_written;
    CHECK(float_written == kSlot554B8FloatWritten);
    std::size_t wrapper_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch8_object_written(2, i)) ++wrapper_written;
    CHECK(wrapper_written == kSlot554B8WrapperWritten);
}

void check_objects() {
    const auto page = make_page();
    const auto page_before = page;
    Slot554Batch8Objects o{};
    std::memset(&o, kSentinel, sizeof o);
    ConCmdRegistry reg{};
    Counts counts{};
    Slot554B8Hooks hooks = default_slot554_batch8_hooks();
    hooks.wrapper_call = &count_wrapper;
    hooks.destroy_view = &count_destroy;
    hooks.context = &counts;
    const Slot554Batch8Result r = init_slot554_batch8(o, page.data(), reg, hooks);
    CHECK(r.ok && r.blocked == Slot554B8Stage::none);
    CHECK(r.built == 36 && r.registered == 36 && r.atexit_records == 36 && r.views_inert == 72);
    CHECK(r.wrapper_calls == 3 && r.view_destroys == 3);
    CHECK(counts.wrapper == 3 && counts.destroy == 3);
    CHECK(counts.wrapper_rvas[0] == kSlot554B8WaterWrapperRva);
    CHECK(counts.wrapper_rvas[1] == kSlot554B8TonemapWrapperRva);
    CHECK(counts.wrapper_rvas[2] == kSlot554B8TonemapWrapperRva);
    CHECK(page == page_before);

    for (std::uint32_t i = 0; i < kSlot554B8ObjectCount; ++i) {
        const auto& spec = kSlot554B8Specs[i];
        const std::uint8_t* p = object(o, i);
        for (std::size_t j = 0; j < spec.size; ++j) {
            if (!slot554_batch8_object_written(i, j)) CHECK(p[j] == kSentinel);
        }
        switch (spec.kind) {
            case Slot554B8Kind::bool_var: {
                const auto* v = reinterpret_cast<const Slot554B2ABool*>(p);
                CHECK(v->base.vptr == kSlot554B8BoolFinalVptr);
                CHECK(v->value_ref == page.data() + spec.page_cell_offset);
                CHECK(v->default_value == 1);
                CHECK(v->fn_vptr == kSlot554B8BoolFnVtbl);
                break;
            }
            case Slot554B8Kind::float_var: {
                const auto* v = reinterpret_cast<const ConCmdRangedVar*>(p);
                CHECK(v->base.vptr == kSlot554B8FloatFinalVptr);
                CHECK(v->value_ref == page.data() + spec.page_cell_offset);
                std::uint32_t def = 0;
                std::memcpy(&def, page.data() + spec.page_cell_offset, sizeof def);
                CHECK(v->default_value == def);
                break;
            }
            case Slot554B8Kind::wrapper: {
                const auto* v = reinterpret_cast<const ConCmdFunctor*>(p);
                CHECK(v->base.vptr == spec.final_vptr);
                CHECK(v->tag_c0 == spec.tag_vptr);
                CHECK(v->spec == reinterpret_cast<std::uint64_t>(page.data() +
                                                                 spec.page_cell_offset));
                CHECK(v->low == 0 && v->high == 5);
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
    Slot554Batch8Objects a{};
    std::memset(&a, kSentinel, sizeof a);
    ConCmdRegistry reg{};
    Slot554B8Hooks h = default_slot554_batch8_hooks();
    h.register_var = &no_link_register;
    CHECK(init_slot554_batch8(a, page.data(), reg, h).ok);
    const Slot554Batch8Objects snapshot = a;
    CHECK(init_slot554_batch8(a, page.data(), reg, h).ok);
    CHECK(std::memcmp(&a, &snapshot, sizeof a) == 0);
}

void check_fail_closed() {
    const auto page = make_page();
    {
        Slot554Batch8Objects o{};
        ConCmdRegistry reg{};
        Slot554B8Hooks h = default_slot554_batch8_hooks();
        h.register_var = &refuse_register;
        const Slot554Batch8Result r = init_slot554_batch8(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked == Slot554B8Stage::registration && r.built == 0);
    }
    {
        Slot554Batch8Objects o{};
        ConCmdRegistry reg{};
        Slot554B8Hooks h = default_slot554_batch8_hooks();
        h.atexit = &refuse_atexit;
        const Slot554Batch8Result r = init_slot554_batch8(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked == Slot554B8Stage::atexit && r.built == 0);
    }
    {
        Slot554Batch8Objects o{};
        ConCmdRegistry reg{};
        Slot554B8Hooks h = default_slot554_batch8_hooks();
        h.release_view = &refuse_release;
        const Slot554Batch8Result r = init_slot554_batch8(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked == Slot554B8Stage::view_release);
    }
    {
        Slot554Batch8Objects o{};
        ConCmdRegistry reg{};
        Slot554B8Hooks h = default_slot554_batch8_hooks();
        h.wrapper_call = &refuse_wrapper;
        const Slot554Batch8Result r = init_slot554_batch8(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked_index == 2 && r.built == 2);
    }
    {
        Slot554Batch8Objects o{};
        ConCmdRegistry reg{};
        Slot554B8Hooks h = default_slot554_batch8_hooks();
        h.destroy_view = &refuse_destroy;
        const Slot554Batch8Result r = init_slot554_batch8(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked_index == 2 && r.wrapper_calls == 0);
    }
    {
        Slot554Batch8Objects o{};
        ConCmdRegistry reg{};
        const Slot554Batch8Result r =
            init_slot554_batch8(o, nullptr, reg, default_slot554_batch8_hooks());
        CHECK(!r.ok && r.blocked == Slot554B8Stage::page);
    }
}

}  // namespace

int main() {
    check_constants();
    check_objects();
    check_replay();
    check_fail_closed();
    if (failures != 0) {
        std::fprintf(stderr, "ctor_slot_554_batch8: %d failure(s)\n", failures);
        return 1;
    }
    std::printf("ctor_slot_554_batch8: ok (36 objects, 6165/1211 bytes)\n");
    return 0;
}
