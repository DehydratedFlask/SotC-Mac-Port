// CPU-01-STARTUP-SLOTS-296-297-300-01: fixture for startup constructor indices 296, 297 and 300.
//
// 296: the 12-byte thunk into the already-lifted 0x323d0 TSC store. Covers fresh guard, preset
//      guard (hook must not run), a null hook (sentinel 1 + partial flag, stamp still written), a
//      zero-returning hook (same visible partial), and a second call.
// 297: the 400 x 0x118 record table. Covers both missing hooks (table untouched), a null handle on
//      the first and on a middle record (exact prefix), the full run (exact offsets, every other
//      byte untouched), and the production wiring through getter_14d880/add_ref_14ccb0.
// 300: always true, one inert cleanup entry per call.
//
// Guard rings surround the 8-byte stamp and the 112 KB table.
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>

#include "core/ctor_slot_296_b2b6b0.hpp"
#include "core/ctor_slot_297_b25470.hpp"
#include "core/ctor_slot_300_acc880.hpp"
#include "core/registry_object_14c9a0.hpp"
#include "core/store_tsc_323d0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using sotc::Object14c9a0;
using sotc::Slot296Stamp;
using sotc::Slot297Hooks;
using sotc::Slot297Result;
using sotc::Slot297Stop;
using sotc::Slot297Table;

// ---------------------------------------------------------------- 296 hook state
std::uint64_t g_frequency = 3'000'000'000ull;
bool g_frequency_called = false;
bool g_abort_called = false;

std::uint64_t hook_frequency() { g_frequency_called = true; return g_frequency; }
std::uint64_t hook_abort() { g_abort_called = true; std::abort(); }
std::uint64_t hook_zero_frequency() { return 0; }

void reset_tsc_cells() {
    sotc::store_tsc_guard_cell_2ea4700() = 0;
    sotc::store_tsc_global_cell_2ea4708() = 0;
}

struct StampRing {
    std::uint64_t before;
    Slot296Stamp stamp;
    std::uint64_t after;
};

// ---------------------------------------------------------------- 297 hook state
int g_acquire_calls = 0;
int g_retain_calls = 0;
Object14c9a0* g_acquire_result = nullptr;
std::size_t g_null_at = std::numeric_limits<std::size_t>::max();

Object14c9a0* hook_acquire() {
    const std::size_t call = static_cast<std::size_t>(g_acquire_calls++);
    if (call == g_null_at) return nullptr;
    return g_acquire_result;
}

void hook_retain(void* handle) {
    CHECK(handle == static_cast<void*>(g_acquire_result));
    ++g_retain_calls;
}

constexpr std::size_t kRingBytes = 64;
constexpr std::uint8_t kSentinel = 0xA5;

struct TableRing {
    std::array<std::uint8_t, kRingBytes> before;
    Slot297Table table;
    std::array<std::uint8_t, kRingBytes> after;
};

TableRing g_ring;

void fill_ring() {
    g_ring.before.fill(kSentinel);
    g_ring.after.fill(kSentinel);
    std::memset(static_cast<void*>(&g_ring.table), kSentinel, sizeof g_ring.table);
}

void check_ring() {
    for (std::uint8_t b : g_ring.before) CHECK(b == kSentinel);
    for (std::uint8_t b : g_ring.after) CHECK(b == kSentinel);
}

const unsigned char* record_bytes(std::size_t i) {
    return reinterpret_cast<const unsigned char*>(&g_ring.table[i]);
}

void check_written_record(std::size_t i, const Object14c9a0* handle) {
    const sotc::Slot297Record& rec = g_ring.table[i];
    CHECK(rec.node == handle);
    CHECK(rec.flag110 == 0);
    CHECK(rec.flag00 == 0);
    const unsigned char* bytes = record_bytes(i);
    for (std::size_t off = 0; off < sotc::kSlot297Stride; ++off) {
        const bool written = off == 0 || (off >= sotc::kSlot297HandleOffset &&
                                          off < sotc::kSlot297HandleOffset + sizeof(void*)) ||
                             off == sotc::kSlot297FlagOffset;
        if (!written) CHECK(bytes[off] == kSentinel);
    }
}

void check_untouched_record(std::size_t i) {
    const unsigned char* bytes = record_bytes(i);
    for (std::size_t off = 0; off < sotc::kSlot297Stride; ++off) CHECK(bytes[off] == kSentinel);
}

void check_all_untouched() {
    for (std::size_t i = 0; i < sotc::kSlot297Count; ++i) check_untouched_record(i);
    check_ring();
}

Slot297Hooks counted_hooks() {
    Slot297Hooks hooks{};
    hooks.acquire_shared_14d880 = &hook_acquire;
    hooks.retain_14ccb0 = &hook_retain;
    return hooks;
}

void reset_297_counters(Object14c9a0* handle, std::size_t null_at) {
    g_acquire_calls = 0;
    g_retain_calls = 0;
    g_acquire_result = handle;
    g_null_at = null_at;
}

// ---------------------------------------------------------------- 296 cases
void case_296_fresh_process() {
    reset_tsc_cells();
    g_frequency_called = false;
    g_abort_called = false;
    StampRing ring{0x1111222233334444ull, {}, 0x5555666677778888ull};
    ring.stamp.tsc = 0xDEADBEEFCAFEBABEull;   // must be overwritten

    const auto result = sotc::init_slot296_b2b6b0(ring.stamp, {&hook_frequency});

    CHECK(!result.guard_was_set);
    CHECK(result.frequency_from_hook);
    CHECK(g_frequency_called);
    CHECK(result.frequency == g_frequency);
    CHECK(result.base != 0);
    CHECK(result.stamp != 0);
    CHECK(ring.stamp.tsc == result.stamp);
    CHECK(sotc::store_tsc_guard_cell_2ea4700() == g_frequency);
    CHECK(sotc::store_tsc_global_cell_2ea4708() == result.base);
    CHECK(ring.before == 0x1111222233334444ull);
    CHECK(ring.after == 0x5555666677778888ull);
}

void case_296_null_hook() {
    reset_tsc_cells();
    StampRing ring{0x0102030405060708ull, {}, 0x090a0b0c0d0e0f10ull};
    ring.stamp.tsc = 0xDEADBEEFCAFEBABEull;

    const auto result = sotc::init_slot296_b2b6b0(ring.stamp);   // default hooks: none supplied

    CHECK(!result.guard_was_set);
    CHECK(!result.frequency_from_hook);
    CHECK(result.frequency == 1);   // store_tsc_323d0's documented sentinel
    CHECK(sotc::store_tsc_guard_cell_2ea4700() == 1);
    CHECK(result.base != 0);
    CHECK(result.stamp != 0);
    CHECK(ring.stamp.tsc == result.stamp);
    CHECK(ring.before == 0x0102030405060708ull);
    CHECK(ring.after == 0x090a0b0c0d0e0f10ull);
}

void case_296_zero_hook() {
    reset_tsc_cells();
    Slot296Stamp stamp{};

    const auto result = sotc::init_slot296_b2b6b0(stamp, {&hook_zero_frequency});

    CHECK(!result.guard_was_set);
    CHECK(!result.frequency_from_hook);
    CHECK(result.frequency == 1);
    CHECK(sotc::store_tsc_guard_cell_2ea4700() == 1);
    CHECK(result.stamp != 0);
}

void case_296_preset_guard_and_second_call() {
    // First call publishes the guard; the second call must skip 0xf8f860 and keep both cells.
    reset_tsc_cells();
    Slot296Stamp first{};
    const auto fresh = sotc::init_slot296_b2b6b0(first, {&hook_frequency});
    CHECK(!fresh.guard_was_set && fresh.frequency_from_hook);

    g_abort_called = false;
    Slot296Stamp second{};
    second.tsc = 0xDEADBEEFCAFEBABEull;
    const auto again = sotc::init_slot296_b2b6b0(second, {&hook_abort});

    CHECK(again.guard_was_set);
    CHECK(!again.frequency_from_hook);
    CHECK(!g_abort_called);   // the hook must not run on the guarded path
    CHECK(again.frequency == fresh.frequency);
    CHECK(again.base == fresh.base);
    CHECK(again.stamp != 0);
    CHECK(second.tsc == again.stamp);
}

// ---------------------------------------------------------------- 297 cases
void case_297_missing_hooks() {
    fill_ring();
    reset_297_counters(reinterpret_cast<Object14c9a0*>(0x1000), 0);
    const Slot297Result none = sotc::init_slot297_b25470(g_ring.table, {});
    CHECK(!none.ok);
    CHECK(none.initialised == 0);
    CHECK(none.stopped_at == Slot297Stop::acquire);
    CHECK(std::strcmp(sotc::slot297_stop_name(none.stopped_at), "acquire") == 0);
    CHECK(g_acquire_calls == 0 && g_retain_calls == 0);
    check_all_untouched();

    Slot297Hooks only_acquire{};
    only_acquire.acquire_shared_14d880 = &hook_acquire;
    const Slot297Result no_retain = sotc::init_slot297_b25470(g_ring.table, only_acquire);
    CHECK(!no_retain.ok);
    CHECK(no_retain.initialised == 0);
    CHECK(no_retain.stopped_at == Slot297Stop::retain);
    CHECK(std::strcmp(sotc::slot297_stop_name(no_retain.stopped_at), "retain") == 0);
    CHECK(g_acquire_calls == 0 && g_retain_calls == 0);
    check_all_untouched();
}

void case_297_null_handle_first() {
    fill_ring();
    reset_297_counters(nullptr, 0);
    const Slot297Result result = sotc::init_slot297_b25470(g_ring.table, counted_hooks());
    CHECK(!result.ok);
    CHECK(result.initialised == 0);
    CHECK(result.stopped_at == Slot297Stop::acquire_null);
    CHECK(std::strcmp(sotc::slot297_stop_name(result.stopped_at), "acquire(null)") == 0);
    CHECK(result.shared == nullptr);
    CHECK(g_acquire_calls == 1);
    CHECK(g_retain_calls == 0);
    check_all_untouched();
}

void case_297_null_handle_mid_table() {
    constexpr std::size_t kStop = 7;
    auto* handle = reinterpret_cast<Object14c9a0*>(0x2000);
    fill_ring();
    reset_297_counters(handle, kStop);

    const Slot297Result result = sotc::init_slot297_b25470(g_ring.table, counted_hooks());

    CHECK(!result.ok);
    CHECK(result.initialised == kStop);
    CHECK(result.stopped_at == Slot297Stop::acquire_null);
    CHECK(result.shared == handle);
    CHECK(g_acquire_calls == static_cast<int>(kStop) + 1);
    CHECK(g_retain_calls == static_cast<int>(kStop));
    for (std::size_t i = 0; i < kStop; ++i) check_written_record(i, handle);
    for (std::size_t i = kStop; i < sotc::kSlot297Count; ++i) check_untouched_record(i);
    check_ring();
}

void case_297_full_table() {
    auto* handle = reinterpret_cast<Object14c9a0*>(0x3000);
    fill_ring();
    reset_297_counters(handle, std::numeric_limits<std::size_t>::max());

    const Slot297Result result = sotc::init_slot297_b25470(g_ring.table, counted_hooks());

    CHECK(result.ok);
    CHECK(result.initialised == sotc::kSlot297Count);
    CHECK(result.stopped_at == Slot297Stop::none);
    CHECK(std::strcmp(sotc::slot297_stop_name(result.stopped_at), "") == 0);
    CHECK(result.shared == handle);
    CHECK(g_acquire_calls == static_cast<int>(sotc::kSlot297Count));
    CHECK(g_retain_calls == static_cast<int>(sotc::kSlot297Count));
    for (std::size_t i = 0; i < sotc::kSlot297Count; ++i) check_written_record(i, handle);
    check_ring();
}

void case_297_default_hooks() {
    fill_ring();
    Object14c9a0& object = sotc::static_object_2ee3b10();
    const std::int32_t before = std::atomic_ref<std::int32_t>(object.a30).load();

    const Slot297Result result = sotc::init_slot297_b25470(g_ring.table,
                                                           sotc::default_slot297_hooks());

    CHECK(result.ok);
    CHECK(result.initialised == sotc::kSlot297Count);
    CHECK(result.shared == &object);
    for (std::size_t i = 0; i < sotc::kSlot297Count; ++i) check_written_record(i, &object);
    check_ring();
    // First 0x14d880 call publishes the object with one reference, then the ctor retains each of the
    // 400 records once: +401 exactly (the original does the same).
    const std::int32_t after = std::atomic_ref<std::int32_t>(object.a30).load();
    CHECK(after - before == static_cast<std::int32_t>(sotc::kSlot297Count) + 1);
}

// ---------------------------------------------------------------- 300 case
void case_300_inert_cleanup() {
    sotc::Slot300Cleanup cleanup{};
    CHECK(cleanup.registered == 0);
    CHECK(sotc::init_slot300_acc880(cleanup));
    CHECK(cleanup.registered == 1);
    CHECK(sotc::init_slot300_acc880(cleanup));
    CHECK(cleanup.registered == 2);
    CHECK(sotc::kSlot300BodySize == 21);
    CHECK(sotc::kSlot300DtorSize == 1);
}

void case_constants() {
    CHECK(sotc::kSlot296ThunkRva == 0xb2b6b0);
    CHECK(sotc::kSlot296ThunkSize == 12);
    CHECK(sotc::kSlot296ObjectRva == 0x334ac18);
    CHECK(sotc::kSlot296CalleeRva == 0x323d0);
    CHECK(sotc::kSlot297Rva == 0xb25470);
    CHECK(sotc::kSlot297BodySize == 105);
    CHECK(sotc::kSlot297Count == 400);
    CHECK(sotc::kSlot297Stride == 0x118);
    CHECK(sotc::kSlot297HandleOffset == 0x108);
    CHECK(sotc::kSlot297FlagOffset == 0x110);
    CHECK(sotc::kSlot297TableRva == 0x332f650);
    CHECK(sotc::kSlot297DtorRva == 0xb24be0);
    CHECK(sotc::kSlot297DtorSize == 0x59);
    CHECK(sizeof(sotc::Slot296Stamp) == 8);
    CHECK(sizeof(sotc::Slot297Record) == 0x118);
}

}  // namespace

int main() {
    case_constants();
    case_296_fresh_process();
    case_296_null_hook();
    case_296_zero_hook();
    case_296_preset_guard_and_second_call();
    case_297_missing_hooks();
    case_297_null_handle_first();
    case_297_null_handle_mid_table();
    case_297_full_table();
    case_297_default_hooks();
    case_300_inert_cleanup();
    std::puts("ctor_slots_296_297_300: all checks passed");
    return 0;
}
