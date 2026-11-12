// Fixture for startup constructor slot 497 (0x262a80). Exercises the page model, every fail-closed
// hook path, the documented hole map (sentinel sweep) and a byte-identical idempotent re-run.
#include "core/ctor_slot_497_262a80.hpp"
#include "core/ctor_slot_family_items.hpp"
#include "core/string_var_257a0.hpp"

#include <cstdio>
#include <cstring>

namespace {

using namespace sotc;

int failures = 0;
constexpr std::uint8_t kSentinel = 0x5a;

#define CHECK(condition)                                                          \
    do {                                                                          \
        if (!(condition)) {                                                       \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__,          \
                         #condition);                                             \
            ++failures;                                                           \
        }                                                                         \
    } while (0)

bool accept_var(ConCmdBase&, ConCmdRegistry&) noexcept { return true; }
bool accept_menu(DebugMenuItem&, DebugMenuRegistry&, StartupAtexitLog&) noexcept { return true; }
bool accept_record(RecordRegistryCell&, NamedRecord&, const SharedStr&, std::uint32_t,
                   std::uint8_t, const AllocDispatchContract&) noexcept {
    return true;
}
bool accept_entry(Slot497Entry&) noexcept { return true; }
bool accept_atexit(std::uint64_t, const void*, ConCmdRegistry&) noexcept { return true; }
bool accept_release(const SharedStr&) noexcept { return true; }
bool accept_destroy() noexcept { return true; }
void stub_tsc(void* dest) noexcept {
    const std::uint64_t stamp = 0x1122334455667788ull;
    std::memcpy(dest, &stamp, sizeof stamp);
}

alignas(8) std::uint8_t arena[8192];
std::size_t arena_used = 0;

void* arena_allocate(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) noexcept {
    if (arena_used + size > sizeof arena) return nullptr;
    void* pointer = arena + arena_used;
    arena_used += size;
    return pointer;
}

bool arena_deallocate(void*, std::uint32_t) noexcept { return true; }

bool refuse_var(ConCmdBase&, ConCmdRegistry&) noexcept { return false; }
bool refuse_menu(DebugMenuItem&, DebugMenuRegistry&, StartupAtexitLog&) noexcept { return false; }
bool refuse_record(RecordRegistryCell&, NamedRecord&, const SharedStr&, std::uint32_t,
                   std::uint8_t, const AllocDispatchContract&) noexcept {
    return false;
}
bool refuse_entry(Slot497Entry&) noexcept { return false; }
bool refuse_atexit(std::uint64_t, const void*, ConCmdRegistry&) noexcept { return false; }
bool refuse_release(const SharedStr&) noexcept { return false; }

Slot497Hooks stub_hooks() noexcept {
    Slot497Hooks hooks{};
    hooks.register_var = &accept_var;
    hooks.register_menu = &accept_menu;
    hooks.register_record = &accept_record;
    hooks.register_entry = &accept_entry;
    hooks.atexit = &accept_atexit;
    hooks.release_view = &accept_release;
    hooks.destroy_arg_functor = &accept_destroy;
    hooks.store_tsc = &stub_tsc;
    hooks.menu_alloc.alloc.allocate = &arena_allocate;
    hooks.menu_alloc.alloc.deallocate = &arena_deallocate;
    return hooks;
}

void fill(Slot497Objects& objects) noexcept {
    std::memset(&objects, kSentinel, sizeof objects);
}

std::uint32_t u32(const Slot497Objects& objects, std::size_t offset) noexcept {
    std::uint32_t value = 0;
    std::memcpy(&value, objects.bytes.data() + offset, sizeof value);
    return value;
}

std::uint64_t u64(const Slot497Objects& objects, std::size_t offset) noexcept {
    std::uint64_t value = 0;
    std::memcpy(&value, objects.bytes.data() + offset, sizeof value);
    return value;
}

void check_constants() {
    std::size_t end = 0;
    for (const Slot497VarSpec& spec : kSlot497Vars) {
        CHECK(spec.offset >= end);
        end = spec.offset + spec.size;
    }
    CHECK(end == kSlot497MenuOffset);
    CHECK(kSlot497MenuOffset + kSlot497MenuCount * kSlot497MenuItemSize == kSlot497ZeroOffset);
    CHECK(kSlot497RecordOffset + kSlot497RecordCount * kSlot497RecordSize ==
          kSlot497TailQwordOffset);
    CHECK(kSlot497EntryOffset + kSlot497EntryCount * kSlot497EntrySize == kSlot497BlockSize);
    CHECK(kSlot497BlockWritten + kSlot497BlockHoles == kSlot497BlockSize);

    std::size_t written = 0;
    std::size_t holes = 0;
    for (std::size_t offset = 0; offset < kSlot497BlockSize; ++offset) {
        if (slot497_byte_written(offset)) ++written;
        else ++holes;
    }
    CHECK(written == kSlot497BlockWritten);
    CHECK(holes == kSlot497BlockHoles);
    CHECK(!slot497_byte_written(kSlot497BlockSize));
    CHECK(slot497_byte_written(0x00));
    CHECK(!slot497_byte_written(0x04));
    CHECK(!slot497_byte_written(0x14));
    CHECK(slot497_byte_written(0x18));
    CHECK(!slot497_byte_written(0x24));
}

void check_stub_construction() {
    Slot497Objects objects{};
    fill(objects);
    arena_used = 0;
    ConCmdRegistry registry{};
    DebugMenuRegistry menu{};
    RecordRegistryCell records{};
    const Slot497Result result = init_slot497_262a80(objects, registry, menu, records, stub_hooks());
    CHECK(result.ok);
    CHECK(result.blocked == Slot497Stage::none);
    CHECK(result.vars_built == 24 && result.vars_registered == 24);
    CHECK(result.menus_built == 19 && result.menus_registered == 19);
    CHECK(result.records_built == 7 && result.records_registered == 7);
    CHECK(result.entries_built == 3 && result.entries_registered == 3);
    CHECK(result.atexit_records == 54);   // 24 vars + page + 19 menus + 7 records + 3 entries
    CHECK(result.cell_set && result.header_built && result.view_built && result.page_atexit);
    CHECK(result.zeroed && result.tsc_stamped && result.tail_qword_zeroed && result.canary_checked);

    // The sentinel survives in every documented hole.
    for (std::size_t offset = 0; offset < kSlot497BlockSize; ++offset) {
        if (!slot497_byte_written(offset)) {
            if (objects.bytes[offset] != kSentinel)
                std::fprintf(stderr, "hole %#zx: %02x\n", offset, objects.bytes[offset]);
            CHECK(objects.bytes[offset] == kSentinel);
        }
    }

    CHECK(u32(objects, 0x00) == 1);
    CHECK(u32(objects, 0x08) == 0x780);
    CHECK(u32(objects, 0x0c) == 0x438);
    CHECK(u32(objects, 0x10) == 0);
    CHECK(u64(objects, 0x18) == 0);
    CHECK(u32(objects, 0x20) == 0xbf800000u);
    const SharedStr* page = reinterpret_cast<const SharedStr*>(objects.bytes.data() + 0x28);
    CHECK(page->size == 0);
    CHECK(page->data == reinterpret_cast<const std::uint8_t*>(kSlot497PageName));
    CHECK(u64(objects, kSlot497TscOffset) == 0x1122334455667788ull);

    const ConCmdBoolVar* fps =
        reinterpret_cast<const ConCmdBoolVar*>(objects.bytes.data() + 0x0048);
    CHECK(fps->base.vptr == kSlot497BoolFinalVptr);
    CHECK(fps->base.registered == 0);
    CHECK(fps->base.flags == 1);
    CHECK(fps->base.name.size == 5 && std::memcmp(fps->base.name.data, "r_fps", 5) == 0);
    CHECK(fps->value_ref == &fps->value);
    CHECK(fps->default_value == 1 && fps->value == 1);
    CHECK(fps->fn_vptr == kSlot497BoolFnVtbl);

    const ConCmdRangedVar* profile =
        reinterpret_cast<const ConCmdRangedVar*>(objects.bytes.data() + 0x0430);
    CHECK(profile->base.vptr == kSlot497RangedFinalVptr);
    CHECK(profile->min_value[0] == 0 && profile->max_value[0] == 2);
    CHECK(profile->min_engaged == 1 && profile->max_engaged == 1);
    CHECK(profile->value_ref == &profile->value);

    const ConCmdRangedVar* speed =
        reinterpret_cast<const ConCmdRangedVar*>(objects.bytes.data() + 0x0698);
    CHECK(speed->base.vptr == kSlot497FloatFinalVptr);
    CHECK(speed->value == 0x3f800000u && speed->default_value == 0x3f800000u);
    CHECK(speed->fn_vptr == kSlot497FloatFnVtbl);
    CHECK(objects.bytes[0x0698 + 0x6c] == 0);   // unbounded: a single zero byte, not a dword
    CHECK(speed->max_engaged == 0);

    const ConCmdRangedVar* tolerance =
        reinterpret_cast<const ConCmdRangedVar*>(objects.bytes.data() + 0x0910);
    CHECK(tolerance->value == 0x3f000000u);
    std::uint32_t tolerance_max = 0;
    std::memcpy(&tolerance_max, tolerance->max_value, sizeof tolerance_max);
    CHECK(tolerance->min_value[0] == 0 && tolerance_max == 0x3f800000u);
    CHECK(tolerance->min_engaged == 1 && tolerance->max_engaged == 1);

    const ConCmdRangedVar* seed =
        reinterpret_cast<const ConCmdRangedVar*>(objects.bytes.data() + 0x09e8);
    CHECK(seed->value == 0xffffffffu && seed->default_value == 0xffffffffu);

    const ConCmdStringVar* scripting =
        reinterpret_cast<const ConCmdStringVar*>(objects.bytes.data() + 0x0ac0);
    CHECK(scripting->base.vptr == kSlot497StringFinalVptr);
    CHECK(scripting->value_ref == &scripting->value);
    CHECK(scripting->value.size == 0);
    CHECK(scripting->default_value.size == 0);
    CHECK(scripting->fn_vptr == kSlot497StringFnVtbl);

    const ConCmdRangedVar* target_fps =
        reinterpret_cast<const ConCmdRangedVar*>(objects.bytes.data() + 0x12c8);
    CHECK(target_fps->base.vptr == kSlot497LocalLowFinalVptr);
    CHECK(target_fps->min_value[0] == 0 && target_fps->max_value[0] == 3);
    const ConCmdRangedVar* vsync =
        reinterpret_cast<const ConCmdRangedVar*>(objects.bytes.data() + 0x13a0);
    CHECK(vsync->base.vptr == kSlot497LocalHighFinalVptr);
    CHECK(vsync->min_value[0] == 0 && vsync->max_value[0] == 5);

    const DebugMenuItem* top =
        reinterpret_cast<const DebugMenuItem*>(objects.bytes.data() + 0x1478);
    CHECK(top->kind == 0 && top->flags == 0x608);
    CHECK(top->target == nullptr);
    CHECK(top->text.size == 13 && std::memcmp(top->text.data, "MenuSeparator", 13) == 0);

    const DebugMenuItem* slider =
        reinterpret_cast<const DebugMenuItem*>(objects.bytes.data() + 0x1520);
    CHECK(slider->kind == 2 && slider->flags == 0x30);
    CHECK(slider->target == objects.bytes.data() + 0x0698);
    CHECK(slider->range_min == 0.01f && slider->range_max == 0.5f);
    CHECK(slider->text.size == 10 && std::memcmp(slider->text.data, "Game Speed", 10) == 0);

    const DebugMenuItem* group =
        reinterpret_cast<const DebugMenuItem*>(objects.bytes.data() + 0x1718);
    CHECK(group->kind == 1 && group->flags == 0x20);
    CHECK(group->label.size == 0);   // the group ctor copies the parent into the label
    CHECK(group->text.size == 5 && std::memcmp(group->text.data, "Debug", 5) == 0);

    const DebugMenuItem* toggle =
        reinterpret_cast<const DebugMenuItem*>(objects.bytes.data() + 0x1a60);
    CHECK(toggle->target == objects.bytes.data() + 0x0048);
    CHECK(toggle->text.size == 18);

    const DebugMenuItem* separator =
        reinterpret_cast<const DebugMenuItem*>(objects.bytes.data() + 0x1d00);
    CHECK(separator->kind == 3 && separator->flags == 0x30);

    const NamedRecord* wall = reinterpret_cast<const NamedRecord*>(
        objects.bytes.data() + kSlot497RecordOffset);
    CHECK(wall->kind == 0 && wall->flag == 1 && wall->q28 == 0 && wall->b30 == 0);
    CHECK(wall->name.size == 20 && std::memcmp(wall->name.data, "Main loop: Wall time", 20) == 0);
    const NamedRecord* render = reinterpret_cast<const NamedRecord*>(
        objects.bytes.data() + kSlot497RecordOffset + 6 * kSlot497RecordSize);
    CHECK(render->kind == 1 && render->flag == 1);

    CHECK(u64(objects, kSlot497TailQwordOffset) == 0);

    const Slot497Entry* first = reinterpret_cast<const Slot497Entry*>(
        objects.bytes.data() + kSlot497EntryOffset);
    CHECK(first->registered == 0);
    CHECK(first->name.size == 25);
    CHECK(first->callback == kSlot497EntryCallbackRva);
    CHECK(first->link_self == reinterpret_cast<std::uint64_t>(&first->link_self));
    CHECK(first->key == 0 && first->reserved == 0 && first->link_next == 0);
    const Slot497Entry* third = reinterpret_cast<const Slot497Entry*>(
        objects.bytes.data() + kSlot497EntryOffset + 2 * kSlot497EntrySize);
    CHECK(third->name.size == 22 &&
          std::memcmp(third->name.data, "SendMessageToComponent", 22) == 0);

    // Byte-identical idempotent re-run on the same page with the arena reset.
    Slot497Objects again = objects;
    arena_used = 0;
    ConCmdRegistry registry2{};
    DebugMenuRegistry menu2{};
    RecordRegistryCell records2{};
    const Slot497Result replay =
        init_slot497_262a80(objects, registry2, menu2, records2, stub_hooks());
    CHECK(replay.ok);
    CHECK(std::memcmp(again.bytes.data(), objects.bytes.data(), kSlot497BlockSize) == 0);
}

void check_default_registration() {
    Slot497Objects objects{};
    fill(objects);
    ConCmdRegistry registry{};
    DebugMenuRegistry menu{};
    RecordRegistryCell records{};
    const Slot497Result result =
        init_slot497_262a80(objects, registry, menu, records, default_slot497_hooks());
    CHECK(result.ok);
    CHECK(result.vars_registered == 24 && result.menus_registered == 19);
    CHECK(result.records_registered == 7 && result.entries_registered == 3);
    CHECK(menu.root.count == 19);
    const ConCmdBoolVar* fps =
        reinterpret_cast<const ConCmdBoolVar*>(objects.bytes.data() + 0x0048);
    CHECK(fps->base.registered == 1);
    const Slot497Entry* first = reinterpret_cast<const Slot497Entry*>(
        objects.bytes.data() + kSlot497EntryOffset);
    CHECK(first->registered == 1);
    const DebugMenuItem* top =
        reinterpret_cast<const DebugMenuItem*>(objects.bytes.data() + 0x1478);
    CHECK(top->registered == 0);   // 0x268b0's root path does not set the item byte
}

void check_fail_closed() {
    ConCmdRegistry registry{};
    DebugMenuRegistry menu{};
    RecordRegistryCell records{};

    {
        Slot497Objects objects{};
        fill(objects);
        const Slot497Result result =
            init_slot497_262a80(objects, registry, menu, records, Slot497Hooks{});
        CHECK(!result.ok && result.blocked == Slot497Stage::hooks);
        bool untouched = true;
        for (std::uint8_t byte : objects.bytes) untouched = untouched && byte == kSentinel;
        CHECK(untouched);
    }
    {
        Slot497Hooks hooks = stub_hooks();
        hooks.register_var = &refuse_var;
        Slot497Objects objects{};
        fill(objects);
        const Slot497Result result = init_slot497_262a80(objects, registry, menu, records, hooks);
        CHECK(!result.ok && result.blocked == Slot497Stage::var_register);
        CHECK(result.atexit_records == 1);
        CHECK(result.vars_built == 1 && result.vars_registered == 0);
        CHECK(objects.bytes[kSlot497MenuOffset] == kSentinel);
    }
    {
        Slot497Hooks hooks = stub_hooks();
        hooks.register_menu = &refuse_menu;
        Slot497Objects objects{};
        fill(objects);
        const Slot497Result result = init_slot497_262a80(objects, registry, menu, records, hooks);
        CHECK(!result.ok && result.blocked == Slot497Stage::menu_register);
        CHECK(result.vars_registered == 24 && result.atexit_records == 25);
        CHECK(result.menus_built == 1 && result.menus_registered == 0);
    }
    {
        Slot497Hooks hooks = stub_hooks();
        hooks.register_record = &refuse_record;
        Slot497Objects objects{};
        fill(objects);
        const Slot497Result result = init_slot497_262a80(objects, registry, menu, records, hooks);
        CHECK(!result.ok && result.blocked == Slot497Stage::record_register);
        CHECK(result.menus_registered == 19 && result.atexit_records == 44);
    }
    {
        Slot497Hooks hooks = stub_hooks();
        hooks.register_entry = &refuse_entry;
        Slot497Objects objects{};
        fill(objects);
        const Slot497Result result = init_slot497_262a80(objects, registry, menu, records, hooks);
        CHECK(!result.ok && result.blocked == Slot497Stage::entry_register);
        CHECK(result.records_registered == 7 && result.atexit_records == 51);
    }
    {
        Slot497Hooks hooks = stub_hooks();
        hooks.atexit = &refuse_atexit;
        Slot497Objects objects{};
        fill(objects);
        const Slot497Result result = init_slot497_262a80(objects, registry, menu, records, hooks);
        CHECK(!result.ok && result.blocked == Slot497Stage::page_atexit);
        CHECK(result.atexit_records == 0);
    }
    {
        Slot497Hooks hooks = stub_hooks();
        hooks.release_view = &refuse_release;
        Slot497Objects objects{};
        fill(objects);
        const Slot497Result result = init_slot497_262a80(objects, registry, menu, records, hooks);
        CHECK(!result.ok && result.blocked == Slot497Stage::var_build);
        CHECK(result.vars_registered == 0);
    }
    {
        Slot497Hooks hooks = stub_hooks();
        hooks.menu_alloc.alloc.allocate = nullptr;
        Slot497Objects objects{};
        fill(objects);
        const Slot497Result result = init_slot497_262a80(objects, registry, menu, records, hooks);
        CHECK(!result.ok && result.blocked == Slot497Stage::hooks);
    }
}

void check_stage_names() {
    CHECK(std::strcmp(slot497_stage_name(Slot497Stage::none), "none") == 0);
    CHECK(std::strcmp(slot497_stage_name(Slot497Stage::var_register), "var_register") == 0);
    CHECK(std::strcmp(slot497_stage_name(Slot497Stage::entry_atexit), "entry_atexit") == 0);
    CHECK(std::strcmp(slot497_stage_name(Slot497Stage::canary), "canary") == 0);
}

}  // namespace

int main() {
    check_constants();
    check_stub_construction();
    check_default_registration();
    check_fail_closed();
    check_stage_names();
    if (failures != 0) {
        std::fprintf(stderr, "%d failure(s)\n", failures);
        return 1;
    }
    std::printf("slot 497: ok\n");
    return 0;
}
