// Fixture for slot 554 batch 11: the 36 objects in [0x83ad1,0x86052).
#include "core/ctor_slot_554_batch11_83ad1.hpp"

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
    for (std::uint32_t i = 0; i < kSlot554B11ObjectCount; ++i) {
        const auto& s = kSlot554B11Specs[i];
        if (s.kind == Slot554B11Kind::bool_var) page[s.page_cell_offset] = 1;
        if (s.kind == Slot554B11Kind::float_var) {
            const std::uint32_t v = s.high != 0 ? s.high : 0x3f800000u;
            std::memcpy(page.data() + s.page_cell_offset, &v, sizeof v);
        }
        if (s.kind == Slot554B11Kind::dword_var) {
            const std::uint32_t v = 5;
            std::memcpy(page.data() + s.page_cell_offset, &v, sizeof v);
        }
        if (s.kind == Slot554B11Kind::wrapper) {
            const std::uint32_t v = 7;
            std::memcpy(page.data() + s.page_cell_offset, &v, sizeof v);
        }
    }
    return page;
}

std::uint8_t* object(Slot554Batch11Objects& o, std::uint32_t i) noexcept {
    std::size_t offset = 0;
    for (std::uint32_t n = 0; n < i; ++n) offset += kSlot554B11Specs[n].size;
    return o.objects.data() + offset;
}

void check_constants() {
    CHECK(kSlot554B11Rva == 0x83ad1 && kSlot554B11BodySize == 0x2581);
    CHECK(kSlot554B11ObjectBaseRva == 0x2eb9ee8 && kSlot554B11ObjectEndRva == 0x2ebbbc8);
    CHECK(kSlot554B11ObjectCount == 36 && kSlot554B11BlockSize == 0x1ce0);
    CHECK(kSlot554B11Written == 6126 && kSlot554B11Holes == 1266);
    CHECK(kSlot554B11InstructionCount == 1670 && kSlot554B11DirectCallCount == 179 &&
          kSlot554B11IndirectCallCount == 30 && kSlot554B11StoreCount == 508 &&
          kSlot554B11ObjectStoreCount == 112 && kSlot554B11StackStoreCount == 396);
    CHECK(kSlot554B11BoolCount == 6 && kSlot554B11FloatCount == 26 &&
          kSlot554B11DwordCount == 3 && kSlot554B11WrapperCount == 1);
    CHECK(kSlot554B11TableIndex == 554 && kSlot554B11TableWordRva == 0x2dd5f90);
    CHECK(kSlot554B11WrapperFinalVptr == kSlot554B11WrapperTagVptr + 0x20);
    std::size_t written = 0;
    for (std::size_t i = 0; i < kSlot554B11BlockSize; ++i) if (slot554_batch11_written(i)) ++written;
    CHECK(written == kSlot554B11Written);
    std::size_t bool_written = 0;
    for (std::size_t i = 0; i < 0xc0; ++i) if (slot554_batch11_object_written(0, i)) ++bool_written;
    CHECK(bool_written == kSlot554B11BoolWritten);
    std::size_t float_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch11_object_written(2, i)) ++float_written;
    CHECK(float_written == kSlot554B11FloatWritten);
    std::size_t dword_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch11_object_written(11, i)) ++dword_written;
    CHECK(dword_written == kSlot554B11DwordWritten);
    std::size_t wrapper_written = 0;
    for (std::size_t i = 0; i < 0xd0; ++i) if (slot554_batch11_object_written(7, i)) ++wrapper_written;
    CHECK(wrapper_written == kSlot554B11WrapperWritten);
}

void check_objects() {
    const auto page = make_page();
    const auto page_before = page;
    Slot554Batch11Objects o{};
    std::memset(&o, kSentinel, sizeof o);
    ConCmdRegistry reg{};
    Counts counts{};
    Slot554B11Hooks hooks = default_slot554_batch11_hooks();
    hooks.wrapper_call = &count_wrapper;
    hooks.destroy_view = &count_destroy;
    hooks.context = &counts;
    const Slot554Batch11Result r = init_slot554_batch11(o, page.data(), reg, hooks);
    CHECK(r.ok && r.blocked == Slot554B11Stage::none);
    CHECK(r.built == 36 && r.registered == 36 && r.atexit_records == 36 && r.views_inert == 72);
    CHECK(r.wrapper_calls == 1 && r.view_destroys == 1);
    CHECK(counts.wrapper == 1 && counts.destroy == 1);
    CHECK(counts.wrapper_rva == kSlot554B11WrapperRva);
    CHECK(page == page_before);

    for (std::uint32_t i = 0; i < kSlot554B11ObjectCount; ++i) {
        const auto& spec = kSlot554B11Specs[i];
        const std::uint8_t* p = object(o, i);
        for (std::size_t j = 0; j < spec.size; ++j) {
            if (!slot554_batch11_object_written(i, j)) CHECK(p[j] == kSentinel);
        }
        switch (spec.kind) {
            case Slot554B11Kind::bool_var: {
                const auto* v = reinterpret_cast<const Slot554B2ABool*>(p);
                CHECK(v->base.vptr == kSlot554B11BoolFinalVptr);
                CHECK(v->value_ref == page.data() + spec.page_cell_offset);
                CHECK(v->default_value == 1);
                CHECK(v->fn_vptr == kSlot554B11BoolFnVtbl);
                break;
            }
            case Slot554B11Kind::float_var:
            case Slot554B11Kind::dword_var: {
                const auto* v = reinterpret_cast<const ConCmdRangedVar*>(p);
                const bool dword = spec.kind == Slot554B11Kind::dword_var;
                CHECK(v->base.vptr == (dword ? kSlot554B11DwordFinalVptr
                                             : kSlot554B11FloatFinalVptr));
                CHECK(v->value_ref == page.data() + spec.page_cell_offset);
                std::uint32_t def = 0;
                std::memcpy(&def, page.data() + spec.page_cell_offset, sizeof def);
                CHECK(v->default_value == def);
                CHECK(v->fn_vptr == (dword ? kSlot554B11DwordClassVtbl
                                           : kSlot554B11FloatClassVtbl));
                CHECK(v->min_engaged == 1 && v->max_engaged == 1);
                std::uint32_t lo = 0, hi = 0;
                std::memcpy(&lo, v->min_value, 4);
                std::memcpy(&hi, v->max_value, 4);
                CHECK(lo == spec.low && hi == spec.high);
                break;
            }
            case Slot554B11Kind::wrapper: {
                const auto* v = reinterpret_cast<const ConCmdFunctor*>(p);
                CHECK(v->base.vptr == kSlot554B11WrapperFinalVptr);
                CHECK(v->tag_c0 == kSlot554B11WrapperTagVptr);
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
    Slot554Batch11Objects a{};
    std::memset(&a, kSentinel, sizeof a);
    ConCmdRegistry reg{};
    Slot554B11Hooks h = default_slot554_batch11_hooks();
    h.register_var = &no_link_register;
    CHECK(init_slot554_batch11(a, page.data(), reg, h).ok);
    const Slot554Batch11Objects snapshot = a;
    CHECK(init_slot554_batch11(a, page.data(), reg, h).ok);
    CHECK(std::memcmp(&a, &snapshot, sizeof a) == 0);
}

void check_fail_closed() {
    const auto page = make_page();
    {
        Slot554Batch11Objects o{};
        ConCmdRegistry reg{};
        Slot554B11Hooks h = default_slot554_batch11_hooks();
        h.register_var = &refuse_register;
        const Slot554Batch11Result r = init_slot554_batch11(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked == Slot554B11Stage::registration && r.built == 0);
    }
    {
        Slot554Batch11Objects o{};
        ConCmdRegistry reg{};
        Slot554B11Hooks h = default_slot554_batch11_hooks();
        h.atexit = &refuse_atexit;
        const Slot554Batch11Result r = init_slot554_batch11(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked == Slot554B11Stage::atexit && r.built == 0);
    }
    {
        Slot554Batch11Objects o{};
        ConCmdRegistry reg{};
        Slot554B11Hooks h = default_slot554_batch11_hooks();
        h.release_view = &refuse_release;
        const Slot554Batch11Result r = init_slot554_batch11(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked == Slot554B11Stage::view_release);
    }
    {
        Slot554Batch11Objects o{};
        ConCmdRegistry reg{};
        Counts c{};
        Slot554B11Hooks h = default_slot554_batch11_hooks();
        h.wrapper_call = &refuse_wrapper;
        h.context = &c;
        const Slot554Batch11Result r = init_slot554_batch11(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked_index == 7 && r.built == 7 && r.wrapper_calls == 0);
    }
    {
        Slot554Batch11Objects o{};
        ConCmdRegistry reg{};
        Counts c{};
        Slot554B11Hooks h = default_slot554_batch11_hooks();
        h.wrapper_call = &count_wrapper;
        h.destroy_view = &refuse_destroy;
        h.context = &c;
        const Slot554Batch11Result r = init_slot554_batch11(o, page.data(), reg, h);
        CHECK(!r.ok && r.blocked_index == 7 && r.built == 7 && r.wrapper_calls == 1);
        CHECK(c.wrapper == 1);
    }
    {
        Slot554Batch11Objects o{};
        ConCmdRegistry reg{};
        const Slot554Batch11Result r =
            init_slot554_batch11(o, nullptr, reg, default_slot554_batch11_hooks());
        CHECK(!r.ok && r.blocked == Slot554B11Stage::page);
    }
}

}  // namespace

int main() {
    check_constants();
    check_objects();
    check_replay();
    check_fail_closed();
    if (failures != 0) {
        std::fprintf(stderr, "ctor_slot_554_batch11: %d failure(s)\n", failures);
        return 1;
    }
    std::printf("ctor_slot_554_batch11: ok (36 objects, 6126/1266 bytes)\n");
    return 0;
}
