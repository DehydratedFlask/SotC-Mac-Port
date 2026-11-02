#include "../core/vtable_15cf248_slot68_9b7b80.hpp"
#include "../core/vtable_15de228_slot20_d7c4b0.hpp"
#include "../core/vtable_2dcb170_slot08_6e92c0.hpp"
#include "../core/vtable_2dd09b0_slot08_9804c0.hpp"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string_view>

namespace {

using sotc::Vtable15cf248Slot68_9b7b80Cells;
using sotc::Vtable15cf248Slot68_9b7b80Result;
using sotc::Vtable15de228Slot20_d7c4b0Result;
using sotc::Vtable2dcb170Slot08_6e92c0Cells;
using sotc::Vtable2dcb170Slot08_6e92c0Hooks;
using sotc::Vtable2dcb170Slot08_6e92c0Result;
using sotc::Vtable2dcb170Slot08_6e92c0Targets;
using sotc::Vtable2dd09b0Slot08_9804c0Cells;
using sotc::Vtable2dd09b0Slot08_9804c0Hooks;
using sotc::Vtable2dd09b0Slot08_9804c0Result;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

void test_constants() {
    check("9b7b80 rva", sotc::kVtable15cf248Slot68_9b7b80Rva == 0x9b7b80);
    check("9b7b80 size", sotc::kVtable15cf248Slot68_9b7b80Size == 0x6);
    check("9b7b80 end", sotc::kVtable15cf248Slot68_9b7b80EndAddress == 0x9b7b86);
    check("9b7b80 table", sotc::kVtable15cf248Slot68_9b7b80VtableAddress == 0x15cf248);
    check("9b7b80 slot", sotc::kVtable15cf248Slot68_9b7b80SlotOffset == 0x68);
    check("9b7b80 reloc", sotc::kVtable15cf248Slot68_9b7b80RelocAddress == 0x15cf2b0);
    check("9b7b80 field", sotc::kVtable15cf248Slot68_9b7b80ValueOffset == 0x38);

    check("d7c4b0 rva", sotc::kVtable15de228Slot20_d7c4b0Rva == 0xd7c4b0);
    check("d7c4b0 size", sotc::kVtable15de228Slot20_d7c4b0Size == 0x3);
    check("d7c4b0 table", sotc::kVtable15de228Slot20_d7c4b0VtableAddress == 0x15de228);
    check("d7c4b0 slot", sotc::kVtable15de228Slot20_d7c4b0SlotOffset == 0x20);
    check("d7c4b0 reloc", sotc::kVtable15de228Slot20_d7c4b0RelocAddress == 0x15de248);

    check("6e92c0 rva", sotc::kVtable2dcb170Slot08_6e92c0Rva == 0x6e92c0);
    check("6e92c0 size", sotc::kVtable2dcb170Slot08_6e92c0Size == 0xd8);
    check("6e92c0 table", sotc::kVtable2dcb170Slot08_6e92c0TableAddress == 0x2dcb170);
    check("6e92c0 slot", sotc::kVtable2dcb170Slot08_6e92c0SlotOffset == 0x08);
    check("6e92c0 reloc", sotc::kVtable2dcb170Slot08_6e92c0RelocAddress == 0x2dcb178);
    check("6e92c0 id", sotc::kVtable2dcb170Slot08_6e92c0IdValue == -256);
    check("6e92c0 block a", sotc::kVtable2dcb170Slot08_6e92c0TargetsBlockAOffset == 0x160);
    check("6e92c0 block b", sotc::kVtable2dcb170Slot08_6e92c0TargetsBlockBOffset == 0x150);

    check("9804c0 rva", sotc::kVtable2dd09b0Slot08_9804c0Rva == 0x9804c0);
    check("9804c0 size", sotc::kVtable2dd09b0Slot08_9804c0Size == 0x155);
    check("9804c0 table", sotc::kVtable2dd09b0Slot08_9804c0TableAddress == 0x2dd09b0);
    check("9804c0 slot", sotc::kVtable2dd09b0Slot08_9804c0SlotOffset == 0x08);
    check("9804c0 reloc", sotc::kVtable2dd09b0Slot08_9804c0RelocAddress == 0x2dd09b8);
    check("9804c0 record", sotc::kVtable2dd09b0Slot08_9804c0RecordSize == 0x670);
    check("9804c0 side", sotc::kVtable2dd09b0Slot08_9804c0SideStride == 0x80);
}

void test_float_getter() {
    const Vtable15cf248Slot68_9b7b80Result empty = sotc::vtable_15cf248_slot68_9b7b80();
    check("9b7b80 null fails", !empty.ok);

    alignas(4) std::uint8_t object[0x40] = {};
    const float stored = 12.5F;
    std::memcpy(object + 0x38, &stored, sizeof(stored));

    Vtable15cf248Slot68_9b7b80Cells cells{};
    cells.self = object;
    const Vtable15cf248Slot68_9b7b80Result result = sotc::vtable_15cf248_slot68_9b7b80(cells);
    check("9b7b80 ok", result.ok);
    check("9b7b80 value", result.value == stored);
}

void test_zero_return() {
    const Vtable15de228Slot20_d7c4b0Result result = sotc::vtable_15de228_slot20_d7c4b0();
    check("d7c4b0 ok", result.ok);
    check("d7c4b0 zero", result.code == 0);
}

// --- 0x6e92c0 ---------------------------------------------------------------------------------

struct FormatCapture {
    bool called = false;
    void* out = nullptr;
    const void* argument = nullptr;
    std::int32_t id = 0;
    const void* block = nullptr;
    float vector[4] = {};
};

FormatCapture g_format{};

void format_stub(void* out, const void* argument, std::int32_t id, const void* block,
                 const float (&vector)[4]) {
    g_format.called = true;
    g_format.out = out;
    g_format.argument = argument;
    g_format.id = id;
    g_format.block = block;
    std::memcpy(g_format.vector, vector, sizeof(g_format.vector));
}

void* g_owner = reinterpret_cast<void*>(0x1000);
Vtable2dcb170Slot08_6e92c0Targets g_targets{};
bool g_committed = false;

const Vtable2dcb170Slot08_6e92c0Targets* access_stub(void* owner) {
    return owner == g_owner ? &g_targets : nullptr;
}

void commit_stub(void* owner) {
    g_committed = owner == g_owner;
}

void test_preset_apply() {
    const Vtable2dcb170Slot08_6e92c0Result missing =
        sotc::vtable_2dcb170_slot08_6e92c0();
    check("6e92c0 missing hook", !missing.ok);

    alignas(4) std::uint8_t format_out[0x40] = {};
    Vtable2dcb170Slot08_6e92c0Cells cells{};
    cells.format_out = format_out;
    cells.argument = reinterpret_cast<const void*>(0xb0b0);

    Vtable2dcb170Slot08_6e92c0Hooks hooks{};
    hooks.format_call = format_stub;
    g_format = {};

    // No owner: the format call still runs, the tail is skipped.
    const Vtable2dcb170Slot08_6e92c0Result skipped =
        sotc::vtable_2dcb170_slot08_6e92c0(cells, hooks);
    check("6e92c0 skipped ok", skipped.ok);
    check("6e92c0 skipped formatted", skipped.formatted && g_format.called);
    check("6e92c0 skipped id", g_format.id == -256);
    check("6e92c0 skipped block",
          g_format.block == reinterpret_cast<const void*>(std::uintptr_t{0x1606140}));
    check("6e92c0 skipped argument", g_format.argument == cells.argument);
    check("6e92c0 skipped vector", g_format.vector[0] == 0.01F && g_format.vector[1] == 0.0F &&
                                       g_format.vector[2] == -0.05F && g_format.vector[3] == 1.0F);
    check("6e92c0 skipped tail", !skipped.tail_ran && !skipped.committed);

    // Full path.
    alignas(4) std::uint8_t block_a[0x10] = {};
    alignas(4) std::uint8_t block_b[0x10] = {};
    g_targets.block_a = block_a;
    g_targets.block_b = block_b;
    g_committed = false;
    cells.owner = g_owner;
    hooks.access_targets = access_stub;
    hooks.commit = commit_stub;

    const Vtable2dcb170Slot08_6e92c0Result full =
        sotc::vtable_2dcb170_slot08_6e92c0(cells, hooks);
    check("6e92c0 full ok", full.ok);
    check("6e92c0 full tail", full.tail_ran);
    check("6e92c0 full copies", full.copied_a && full.copied_b);
    check("6e92c0 full commit", full.committed && g_committed);
    check("6e92c0 full vector a", full.copied_a_values[0] == 0.0F &&
                                      full.copied_a_values[3] == 1.0F);
    check("6e92c0 full vector b", full.copied_b_values[0] == 0.0F &&
                                      full.copied_b_values[3] == 0.0F);
    float observed[4] = {};
    std::memcpy(observed, block_a, sizeof(observed));
    check("6e92c0 full memory a", observed[3] == 1.0F);

    // A null destination is a visible failure, not a silent skip.
    g_targets.block_b = nullptr;
    const Vtable2dcb170Slot08_6e92c0Result bad =
        sotc::vtable_2dcb170_slot08_6e92c0(cells, hooks);
    check("6e92c0 null block fails", !bad.ok && !bad.committed);
}

// --- 0x9804c0 ---------------------------------------------------------------------------------

struct BuilderCapture {
    std::int32_t parse_result = 0;
    int parse_calls = 0;
    std::uint64_t array_size = 0;
    std::uint64_t side_size = 0;
    int alloc_calls = 0;
    bool fail_array = false;
    int ctor_calls = 0;
    std::uint64_t ctor_callback = 0;
    void* ctor_records[8] = {};
    void* ctor_destinations[8] = {};
};

BuilderCapture g_builder{};

alignas(8) std::uint8_t g_array[0x400 * 0x670]{};
alignas(8) std::uint8_t g_side[0x400 * 0x80]{};

void parse_stub(const char* text, std::int32_t* out) {
    ++g_builder.parse_calls;
    check("9804c0 parse name", text != nullptr && std::strcmp(text, "waves") == 0);
    *out = g_builder.parse_result;
}

void* alloc_stub(void* /*context*/, std::uint64_t size, std::uint32_t align) {
    ++g_builder.alloc_calls;
    check("9804c0 alloc align", align == 1);
    if (g_builder.alloc_calls == 1) {
        g_builder.array_size = size;
        return g_builder.fail_array ? nullptr : g_array;
    }
    g_builder.side_size = size;
    return g_side;
}

void ctor_stub(void* destination, std::uint32_t mode, std::uint64_t callback, void* record) {
    check("9804c0 ctor mode", mode == 0);
    if (g_builder.ctor_calls < 8) {
        g_builder.ctor_records[g_builder.ctor_calls] = record;
        g_builder.ctor_destinations[g_builder.ctor_calls] = destination;
    }
    g_builder.ctor_callback = callback;
    ++g_builder.ctor_calls;
}

void reset_builder(std::int32_t parsed) {
    g_builder = {};
    g_builder.parse_result = parsed;
}

void test_record_builder() {
    const Vtable2dd09b0Slot08_9804c0Result missing = sotc::vtable_2dd09b0_slot08_9804c0();
    check("9804c0 missing hook", !missing.ok);

    Vtable2dd09b0Slot08_9804c0Cells cells{};
    cells.text = "waves";
    Vtable2dd09b0Slot08_9804c0Hooks hooks{};
    hooks.parse_max_waves = parse_stub;
    hooks.allocate = alloc_stub;
    hooks.construct_record = ctor_stub;

    void* array_out = nullptr;
    std::uint64_t stride_out = 0;
    std::uint32_t count_out = 0;
    cells.out_array = &array_out;
    cells.out_stride = &stride_out;
    cells.out_count = &count_out;

    reset_builder(3);
    const Vtable2dd09b0Slot08_9804c0Result built =
        sotc::vtable_2dd09b0_slot08_9804c0(cells, hooks);
    check("9804c0 built ok", built.ok);
    check("9804c0 parsed", built.parsed == 3 && built.count == 3);
    check("9804c0 array size", g_builder.array_size == 3 * 0x670);
    check("9804c0 side size", g_builder.side_size == 3 * 0x80);
    check("9804c0 two allocs", g_builder.alloc_calls == 2);
    check("9804c0 ctor count", g_builder.ctor_calls == 3 && built.constructed == 3);
    check("9804c0 callback",
          g_builder.ctor_callback == std::uint64_t{0x9838a0});
    check("9804c0 record 0", g_builder.ctor_records[0] == g_array);
    check("9804c0 record 1", g_builder.ctor_records[1] == g_array + 0x670);
    check("9804c0 destination 0", g_builder.ctor_destinations[0] == g_array + 0x538);
    check("9804c0 out array", array_out == g_array && built.array_written);
    check("9804c0 out stride", stride_out == 0x670 && built.stride_written);
    check("9804c0 out count", count_out == 3 && built.count_written);

    void* self_slot = nullptr;
    std::memcpy(&self_slot, g_array + 0x520, sizeof(self_slot));
    check("9804c0 record self", self_slot == g_array);
    void* side_slot = nullptr;
    std::memcpy(&side_slot, g_array + 0x618, sizeof(side_slot));
    check("9804c0 side slot", side_slot == g_side);

    // A parsed zero becomes one record.
    reset_builder(0);
    const Vtable2dd09b0Slot08_9804c0Result one =
        sotc::vtable_2dd09b0_slot08_9804c0(cells, hooks);
    check("9804c0 zero clamp", one.ok && one.count == 1 && one.clamped_to_one);
    check("9804c0 zero alloc", g_builder.array_size == 0x670);

    // Above the cap clamps to 0x400.
    reset_builder(0x500);
    const Vtable2dd09b0Slot08_9804c0Result capped =
        sotc::vtable_2dd09b0_slot08_9804c0(cells, hooks);
    check("9804c0 max clamp", capped.ok && capped.count == 0x400 && capped.clamped_to_max);
    check("9804c0 max alloc", g_builder.array_size == std::uint64_t{0x400} * 0x670);

    // A null array allocation still writes the out-parameters and builds nothing.
    reset_builder(4);
    g_builder.fail_array = true;
    const Vtable2dd09b0Slot08_9804c0Result failed =
        sotc::vtable_2dd09b0_slot08_9804c0(cells, hooks);
    check("9804c0 null array ok", failed.ok);
    check("9804c0 null array ctor", g_builder.ctor_calls == 0 && failed.constructed == 0);
    check("9804c0 null array out", array_out == nullptr && count_out == 4);
}

}  // namespace

int main() {
    test_constants();
    test_float_getter();
    test_zero_return();
    test_preset_apply();
    test_record_builder();

    if (failures == 0) {
        std::printf("OK test_vtable_frontier2\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_frontier2: %d failure(s)\n", failures);
    return 1;
}
