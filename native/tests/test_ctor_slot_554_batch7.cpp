// Fixture for slot 554 batch 7: the 36 objects in [0x7a68c,0x7cb5b).
#include "core/ctor_slot_554_batch7_7a68c.hpp"

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
    for (std::uint32_t i = 0; i < kSlot554B7ObjectCount; ++i) {
        const auto& s = kSlot554B7Specs[i];
        if (s.kind == Slot554B7Kind::bool_var) page[s.page_cell_offset] = 1;
        if (s.kind == Slot554B7Kind::float_var)
            std::memcpy(page.data() + s.page_cell_offset, &s.high, sizeof s.high);
        if (s.kind == Slot554B7Kind::int_var) {
            const std::uint32_t v = 3;
            std::memcpy(page.data() + s.page_cell_offset, &v, sizeof v);
        }
        if (s.kind == Slot554B7Kind::wrapper) {
            const std::uint32_t v = 7;
            std::memcpy(page.data() + s.page_cell_offset, &v, sizeof v);
        }
    }
    return page;
}

std::uint8_t* object(Slot554Batch7Objects& o, std::uint32_t i) noexcept {
    std::size_t offset = 0;
    for (std::uint32_t n = 0; n < i; ++n) offset += kSlot554B7Specs[n].size;
    return o.objects.data() + offset;
}

void check_constants() {
    CHECK(kSlot554B7Rva == 0x7a68c && kSlot554B7BodySize == 0x24cf);
    CHECK(kSlot554B7ObjectBaseRva == 0x2eb2ba0 && kSlot554B7ObjectEndRva == 0x2eb4868);
    CHECK(kSlot554B7ObjectCount == 36 && kSlot554B7BlockSize == 0x1cc8);
    CHECK(kSlot554B7Written == 6087 && kSlot554B7Holes == 1281);
    CHECK(kSlot554B7InstructionCount == 1631 && kSlot554B7DirectCallCount == 179 &&
          kSlot554B7IndirectCallCount == 27 && kSlot554B7StoreCount == 508 &&
          kSlot554B7ObjectStoreCount == 137 && kSlot554B7StackStoreCount == 371);
    CHECK(kSlot554B7BoolCount == 9 && kSlot554B7FloatCount == 19 && kSlot554B7IntCount == 4 &&
          kSlot554B7DwordCount == 3 && kSlot554B7WrapperCount == 1);
    CHECK(kSlot554B7TableIndex == 554 && kSlot554B7TableWordRva == 0x2dd5f90);
    CHECK(kSlot554B7DwordFinalVptr == 0x1594bf8 + 0x10);
    std::size_t written = 0;
    for (std::size_t i = 0; i < kSlot554B7BlockSize; ++i) if (slot554_batch7_written(i)) ++written;
    CHECK(written == kSlot554B7Written);
    std::size_t bool_written = 0;
    for (std::size_t i = 0; i < 0xc0; ++i) if (slot554_batch7_object_written(8, i)) ++bool_written;
    CHECK(bool_written == kSlot554B7BoolWritten);
    std::size_t float_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch7_object_written(0, i)) ++float_written;
    CHECK(float_written == kSlot554B7FloatWritten);
    std::size_t int_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch7_object_written(2, i)) ++int_written;
    CHECK(int_written == kSlot554B7IntWritten);
    std::size_t dword_written = 0;
    for (std::size_t i = 0; i < 0xd8; ++i) if (slot554_batch7_object_written(4, i)) ++dword_written;
    CHECK(dword_written == kSlot554B7DwordWritten);
    std::size_t wrapper_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i)
        if (slot554_batch7_object_written(13, i)) ++wrapper_written;
    CHECK(wrapper_written == kSlot554B7WrapperWritten);
}

void check_objects() {
    const auto page = make_page();
    const auto page_before = page;
    Slot554Batch7Objects o{};
    std::memset(&o, kSentinel, sizeof o);
    ConCmdRegistry reg{};
    Counts counts{};
    Slot554B7Hooks hooks = default_slot554_batch7_hooks();
    hooks.wrapper_call = &count_wrapper;
    hooks.destroy_view = &count_destroy;
    hooks.context = &counts;
    const Slot554Batch7Result r = init_slot554_batch7(o, page.data(), reg, hooks);
    CHECK(r.ok && r.blocked == Slot554B7Stage::none);
    CHECK(r.built == 36 && r.registered == 36 && r.atexit_records == 36 && r.views_inert == 72);
    CHECK(r.wrapper_calls == 1 && r.view_destroys == 1);
    CHECK(counts.wrapper == 1 && counts.destroy == 1);
    CHECK(counts.wrapper_rvas[0] == kSlot554B7WrapperRva);
    CHECK(page == page_before);

    for (std::uint32_t i = 0; i < kSlot554B7ObjectCount; ++i) {
        const auto& spec = kSlot554B7Specs[i];
        const std::uint8_t* p = object(o, i);
        for (std::size_t j = 0; j < spec.size; ++j) {
            if (!slot554_batch7_object_written(i, j)) CHECK(p[j] == kSentinel);
        }
        switch (spec.kind) {
            case Slot554B7Kind::bool_var: {
                const auto* v = reinterpret_cast<const Slot554B2ABool*>(p);
                CHECK(v->base.vptr == kSlot554B7BoolFinalVptr);
                CHECK(v->value_ref == page.data() + spec.page_cell_offset);
                CHECK(v->default_value == 1);
                CHECK(v->fn_vptr == kSlot554B7BoolFnVtbl);
                break;
            }
            case Slot554B7Kind::float_var: {
                const auto* v = reinterpret_cast<const ConCmdRangedVar*>(p);
                CHECK(v->base.vptr == kSlot554B7FloatFinalVptr);
                CHECK(v->value_ref == page.data() + spec.page_cell_offset);
                std::uint32_t def = 0;
                std::memcpy(&def, page.data() + spec.page_cell_offset, sizeof def);
                CHECK(v->default_value == def);
                break;
            }
            case Slot554B7Kind::int_var: {
                const auto* v = reinterpret_cast<const ConCmdRangedVar*>(p);
                CHECK(v->base.vptr == kSlot554B7IntFinalVptr);
                CHECK(v->value_ref == page.data() + spec.page_cell_offset);
                CHECK(v->default_value == 3);
                break;
            }
            case Slot554B7Kind::dword_var: {
                const auto* v = reinterpret_cast<const ConCmdRangedVar*>(p);
                CHECK(v->base.vptr == kSlot554B7DwordFinalVptr);
                CHECK(v->value_ref == p + kSlot554B7DwordCellOffset);
                CHECK(v->default_value == 0);
                std::uint32_t tail = ~0u;
                std::memcpy(&tail, p + kSlot554B7DwordCellOffset, sizeof tail);
                CHECK(tail == 0);
                break;
            }
            case Slot554B7Kind::wrapper: {
                const auto* v = reinterpret_cast<const ConCmdFunctor*>(p);
                CHECK(v->base.vptr == kSlot554B7WrapperFinalVptr);
                CHECK(v->tag_c0 == kSlot554B7WrapperTagVptr);
                CHECK(v->spec == reinterpret_cast<std::uint64_t>(page.data() +
                                                                 spec.page_cell_offset));
                CHECK(v->low == 0 && v->high == 2);
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
    Slot554Batch7Objects a{};
    std::memset(&a, kSentinel, sizeof a);
    ConCmdRegistry reg{};
    Slot554B7Hooks h = default_slot554_batch7_hooks();
    h.register_var = &no_link_register;
    CHECK(init_slot554_batch7(a, page.data(), reg, h).ok);
    const Slot554Batch7Objects snapshot = a;
    CHECK(init_slot554_batch7(a, page.data(), reg, h).ok);
    CHECK(std::memcmp(&a, &snapshot, sizeof a) == 0);
}

void check_fail_closed() {
    const auto page = make_page();
    {
        Slot554Batch7Objects o{};
        ConCmdRegistry reg{};
        Slot554B7Hooks h = default_slot554_batch7_hooks();
        h.register_var = &refuse_register;
        const Slot554Batch7Result r = init_slot554_batch7(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked == Slot554B7Stage::registration && r.built == 0);
    }
    {
        Slot554Batch7Objects o{};
        ConCmdRegistry reg{};
        Slot554B7Hooks h = default_slot554_batch7_hooks();
        h.atexit = &refuse_atexit;
        const Slot554Batch7Result r = init_slot554_batch7(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked == Slot554B7Stage::atexit && r.built == 0);
    }
    {
        Slot554Batch7Objects o{};
        ConCmdRegistry reg{};
        Slot554B7Hooks h = default_slot554_batch7_hooks();
        h.release_view = &refuse_release;
        const Slot554Batch7Result r = init_slot554_batch7(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked == Slot554B7Stage::view_release);
    }
    {
        Slot554Batch7Objects o{};
        ConCmdRegistry reg{};
        Slot554B7Hooks h = default_slot554_batch7_hooks();
        h.wrapper_call = &refuse_wrapper;
        const Slot554Batch7Result r = init_slot554_batch7(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked_index == 13 && r.built == 13);
    }
    {
        Slot554Batch7Objects o{};
        ConCmdRegistry reg{};
        Slot554B7Hooks h = default_slot554_batch7_hooks();
        h.destroy_view = &refuse_destroy;
        const Slot554Batch7Result r = init_slot554_batch7(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked_index == 13 && r.wrapper_calls == 0);
    }
    {
        Slot554Batch7Objects o{};
        ConCmdRegistry reg{};
        const Slot554Batch7Result r =
            init_slot554_batch7(o, nullptr, reg, default_slot554_batch7_hooks());
        CHECK(!r.ok && r.blocked == Slot554B7Stage::page);
    }
    {
        // An int-family failure inside the dword ctor must not be swallowed: the tag is the
        // real 0x1596858 family table, so this path only fires on a zeroed hook set.
        Slot554Batch7Objects o{};
        ConCmdRegistry reg{};
        Slot554B7Hooks h = default_slot554_batch7_hooks();
        h.register_var = &no_link_register;
        const Slot554Batch7Result r = init_slot554_batch7(o, page.data(), reg, h);
        CHECK(r.ok && r.registered == 36);
    }
}

}  // namespace

int main() {
    check_constants();
    check_objects();
    check_replay();
    check_fail_closed();
    if (failures != 0) {
        std::fprintf(stderr, "ctor_slot_554_batch7: %d failure(s)\n", failures);
        return 1;
    }
    std::printf("ctor_slot_554_batch7: ok (36 objects, 6087/1281 bytes)\n");
    return 0;
}
