// Fixture for startup constructor slot 554 batch 1: page image, eight variables, byte map,
// fail-closed hook paths and byte-identical replay on a fresh copy.
#include "core/ctor_slot_554_6efe0.hpp"

#include <cstdio>
#include <cstring>

namespace {
using namespace sotc;
int failures = 0;
constexpr std::uint8_t kSentinel = 0x5a;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++failures; } } while (0)

bool refuse_page(std::uint8_t*, std::size_t) noexcept { return false; }
bool refuse_register(ConCmdBase&, ConCmdRegistry&) noexcept { return false; }
bool no_link_register(ConCmdBase&, ConCmdRegistry&) noexcept { return true; }
bool refuse_atexit(std::uint64_t, const void*, ConCmdRegistry&) noexcept { return false; }
bool refuse_release(const SharedStr&) noexcept { return false; }
bool refuse_destroy(std::uint64_t) noexcept { return false; }

Slot554Hooks hooks() noexcept {
    return default_slot554_hooks();
}

std::uint8_t* object(Slot554Objects& o, std::uint32_t i) noexcept {
    std::size_t off = 0;
    for (std::uint32_t j = 0; j < i; ++j) off += kSlot554Vars[j].size;
    return o.objects.data() + off;
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
    CHECK(kSlot554PageStoreCount == 187);
    CHECK(kSlot554PageWritten == 2113 && kSlot554PageHoles == 647);
    CHECK(kSlot554RangedWritten == 165 && kSlot554RangedHoles == 43);
    CHECK(kSlot554BoolWritten == 162 && kSlot554BoolHoles == 30);
    CHECK(kSlot554BlockWritten == 3427 && kSlot554BlockHoles == 965);
    CHECK(kSlot554Vars[0].object_rva == 0x2ea99b0);
    CHECK(kSlot554Vars[7].object_rva == 0x2ea9f50);
    CHECK(kSlot554Vars[7].object_rva + kSlot554Vars[7].size == 0x2eaa010);
    std::size_t written = 0;
    for (std::size_t i = 0; i < kSlot554PageSize; ++i) if (slot554_page_written(i)) ++written;
    CHECK(written == kSlot554PageWritten);
    written = 0;
    for (std::size_t i = 0; i < kSlot554BlockSize; ++i) if (slot554_written(i)) ++written;
    CHECK(written == kSlot554BlockWritten);
}

void check_page() {
    Slot554Objects o{};
    std::memset(&o, kSentinel, sizeof o);
    ConCmdRegistry reg{};
    const Slot554Result r = init_slot554_batch1(o, reg, hooks());
    CHECK(r.ok && r.blocked == Slot554Stage::none);
    CHECK(r.page_ok && r.built == 8 && r.registered == 8 && r.atexit_records == 8);
    CHECK(r.views_inert == 16 && r.functors_destroyed == 6);
    CHECK(u32(o.page.data(), 0x24) == 0 && u32(o.page.data(), 0x28) == 1);
    CHECK(u32(o.page.data(), 0x2c) == 0 && u32(o.page.data(), 0x38) == 0);
    CHECK(o.page[0x10] == 0 && o.page[0x11] == 1);
    for (std::size_t i = 0; i < kSlot554PageSize; ++i) {
        if (!slot554_page_written(i)) CHECK(o.page[i] == kSentinel);
    }
    for (std::uint32_t i = 0; i < kSlot554VariableCount; ++i) {
        const std::uint8_t* p = object(o, i);
        for (std::size_t j = 0; j < kSlot554Vars[i].size; ++j) {
            if (!slot554_object_written(i, j)) CHECK(p[j] == kSentinel);
        }
        CHECK(u64(p, 0) == kSlot554Vars[i].final_vptr);
        CHECK(u64(p, 0x60) == reinterpret_cast<std::uint64_t>(o.page.data() + kSlot554Vars[i].page_cell_offset));
        if (kSlot554Vars[i].kind == Slot554Kind::bool_var) CHECK(p[0x68] == o.page[kSlot554Vars[i].page_cell_offset]);
        else CHECK(u32(p, 0x68) == u32(o.page.data(), kSlot554Vars[i].page_cell_offset));
        CHECK(p[0x18] == 1);
        const SharedStr& name = *reinterpret_cast<const SharedStr*>(p + 0x20);
        CHECK(name.size == kSlot554Vars[i].name_length);
        CHECK(std::memcmp(name.data, kSlot554Vars[i].name, name.size) == 0);
    }
    CHECK(reg.atexit_log.size() == 8);
    for (std::size_t i = 0; i < reg.atexit_log.size(); ++i) {
        CHECK(reg.atexit_log.at(i).dtor_rva == kSlot554Vars[i].dtor_rva);
        CHECK(reg.atexit_log.at(i).object == object(o, static_cast<std::uint32_t>(i)));
    }
}

void check_replay() {
    Slot554Objects a{};
    std::memset(&a, kSentinel, sizeof a);
    ConCmdRegistry reg{};
    Slot554Hooks h = hooks();
    h.register_var = &no_link_register;
    const Slot554Result x = init_slot554_batch1(a, reg, h);
    CHECK(x.ok);
    Slot554Objects snapshot = a;
    const Slot554Result y = init_slot554_batch1(a, reg, h);
    CHECK(y.ok);
    CHECK(std::memcmp(&a, &snapshot, sizeof a) == 0);
}

void check_fail_closed() {
    Slot554Objects o{};
    ConCmdRegistry reg{};
    Slot554Hooks h = hooks();
    h.page_init = &refuse_page;
    CHECK(!init_slot554_batch1(o, reg, h).ok);
    h = hooks(); h.register_var = &refuse_register;
    Slot554Result r = init_slot554_batch1(o, reg, h);
    CHECK(!r.ok && r.blocked == Slot554Stage::registration && r.built == 0);
    h = hooks(); h.atexit = &refuse_atexit;
    r = init_slot554_batch1(o, reg, h);
    CHECK(!r.ok && r.blocked == Slot554Stage::atexit && r.built == 0);
    h = hooks(); h.release_view = &refuse_release;
    r = init_slot554_batch1(o, reg, h);
    CHECK(!r.ok && r.blocked == Slot554Stage::view_release);
    h = hooks(); h.destroy_arg = &refuse_destroy;
    r = init_slot554_batch1(o, reg, h);
    CHECK(!r.ok && r.blocked == Slot554Stage::arg_destroy && r.built == 0);
    h = hooks(); h.page_init = nullptr;
    CHECK(!init_slot554_batch1(o, reg, h).ok);
}
}  // namespace

int main() {
    check_constants();
    check_page();
    check_replay();
    check_fail_closed();
    if (failures) { std::fprintf(stderr, "%d failures\n", failures); return 1; }
    std::puts("ctor_slot_554_batch1: all checks passed");
    return 0;
}
