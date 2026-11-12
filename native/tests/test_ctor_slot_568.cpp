// CPU-01-STARTUP-SLOT-568: fixture for slot 568 / 0x207e0.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ctor_slot_568_207e0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

std::size_t g_noop_calls = 0;
std::uint64_t g_scratch_tag = 0;

bool noop_hook(void*, Slot568Scratch& scratch) noexcept {
    ++g_noop_calls;
    g_scratch_tag = scratch.tag;
    scratch.gap00[0] = 0xfeedfacecafebeefull;
    return true;
}

bool view_is(const SharedStr& v, const char* text) {
    const std::size_t length = std::strlen(text);
    return v.data != nullptr && v.size == static_cast<std::int64_t>(length) && v.aux == 0 &&
           v.ctl == nullptr && std::memcmp(v.data, text, length) == 0;
}

void section_alone() {
    Slot568Objects objects{};
    objects.tokens[0].flag48 = 0xa5;   // prove the body does not write it
    ConCmdRegistry registry{};
    Slot568Hooks hooks{};
    hooks.noop_vcall = &noop_hook;
    const Slot568Result r = init_slot568_207e0(objects, registry, hooks);

    CHECK(r.ok && r.canary_matched);
    CHECK(r.reserved && r.inline_push_first && r.inline_push_second && !r.grow_path);
    CHECK(r.empty_normalized && !r.first_flag_written && r.second_flag_written);
    CHECK(r.registered && r.noop_vcall && r.reserve_count == 2 && r.tokens == 2);
    CHECK(r.registrations == 1 && r.atexit_records == 1);
    CHECK(g_noop_calls == 1 && g_scratch_tag == kSlot568ScratchTag);

    const Slot568Command& command = objects.command;
    CHECK(command.base.vptr == kSlot568CommandVptr);
    CHECK(command.base.registered == 1 && command.base.flags == kSlot568CommandFlags);
    CHECK(view_is(command.base.name, kSlot568Name));
    CHECK(view_is(command.base.description, kSlot568Description));
    CHECK(command.tail0 == reinterpret_cast<std::uint64_t>(&command.tail0));
    CHECK(command.tail8 == kSlot568CommandTail8);
    CHECK(command.tail10 == kSlot568CommandTail10);
    CHECK(command.tail18 == kSlot568CommandHandler);
    CHECK(command.vec_begin == &objects.tokens[0]);
    CHECK(command.vec_end == &objects.tokens[2]);
    CHECK(command.vec_cap == &objects.tokens[2]);
    CHECK(command.gap98 == 0);

    const Slot568Token& first = objects.tokens[0];
    CHECK(first.kind == 4);
    CHECK(view_is(first.text, "file"));
    CHECK(first.empty.data == reinterpret_cast<const std::uint8_t*>(kSlot568EmptyData));
    CHECK(first.empty.size == 0 && first.empty.aux == 0 && first.empty.ctl == nullptr);
    CHECK(first.flag48 == 0xa5);   // no store at +0x48

    const Slot568Token& second = objects.tokens[1];
    CHECK(second.kind == 0);
    CHECK(view_is(second.text, "inline"));
    CHECK(second.empty.data == reinterpret_cast<const std::uint8_t*>(kSlot568EmptyData));
    CHECK(second.empty.size == 0 && second.empty.aux == 0 && second.empty.ctl == nullptr);
    CHECK(second.flag48 == 0);

    CHECK(objects.scratch.vtable == kSlot568ScratchVtable);
    CHECK(objects.scratch.tag == kSlot568ScratchTag);
    CHECK(objects.scratch.gap00[0] == 0xfeedfacecafebeefull);

    CHECK(registry.pending.size() == 1);
    CHECK(registry.atexit_log.size() == 1);
    CHECK(registry.atexit_log.at(0).dtor_rva == kSlot568CommandDtor);
    CHECK(registry.atexit_log.at(0).object == &command);
}

void section_hook_failure() {
    Slot568Objects objects{};
    ConCmdRegistry registry{};
    Slot568Hooks hooks{};
    hooks.noop_vcall = [](void*, Slot568Scratch&) noexcept { return false; };
    const Slot568Result r = init_slot568_207e0(objects, registry, hooks);
    CHECK(!r.ok && r.registered && r.registrations == 1 && r.atexit_records == 0);
    CHECK(registry.pending.size() == 1);
    CHECK(registry.atexit_log.size() == 0);
}

}  // namespace

int main() {
    CHECK(kSlot568Rva == 0x207e0 && kSlot568Size == 0x30e);
    CHECK(sizeof(Slot568Token) == 0x50 && sizeof(Slot568Command) == 0xa0);
    CHECK(sizeof(Slot568Scratch) == 0x30);
    section_alone();
    section_hook_failure();
    std::puts("OK test_ctor_slot_568");
    return 0;
}
