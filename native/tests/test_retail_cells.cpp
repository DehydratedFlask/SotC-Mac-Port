#include "../core/retail_cells.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string_view>
#include <vector>

namespace {

using sotc::GuestStatus;
using sotc::GuestWindow;
using sotc::marshal_13d20;
using sotc::marshal_187c0;
using sotc::marshal_20d60;
using sotc::marshal_358c0;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

void hook_tail(void*, void*) {}

void test_20d60_marshals_flag_and_global() {
    std::vector<std::uint8_t> storage(0x80, 0);
    GuestWindow window(storage.data(), 0x10000, storage.size());
    const std::uint64_t self = 0x10000;
    const std::uint64_t global = 0x10040;
    auto marshalled = marshal_20d60(window, self, global);
    check("20d60 ok", marshalled.ok && marshalled.status == GuestStatus::ok);
    check("20d60 flag", marshalled.cells.once_flag == storage.data() + 0x18);
    check("20d60 global", marshalled.cells.global_cell ==
                              reinterpret_cast<const void* const*>(storage.data() + 0x40));

    sotc::Vtable1594dc0Slot20_20d60Hooks hooks{};
    hooks.tail_call = hook_tail;
    const auto result = sotc::vtable_1594dc0_slot20_20d60(nullptr, marshalled.cells, hooks);
    check("20d60 model ok", result.ok && result.flag_written);
    check("20d60 flag written", storage[0x18] == 1);
}

void test_20d60_out_of_range() {
    std::vector<std::uint8_t> storage(0x10, 0);
    GuestWindow window(storage.data(), 0x20000, storage.size());
    auto marshalled = marshal_20d60(window, 0x20000, 0);
    check("20d60 range fails", !marshalled.ok && marshalled.status == GuestStatus::out_of_range);
    GuestWindow empty_window;
    auto empty = marshal_20d60(empty_window, 0x20000, 0);
    check("20d60 no window", !empty.ok && empty.status == GuestStatus::no_window);
}

void test_358c0_marshals_three_fields() {
    std::vector<std::uint8_t> storage(0x80, 0);
    GuestWindow window(storage.data(), 0x30000, storage.size());
    const std::uint64_t self = 0x30000;
    const std::uint64_t source = 0x30040;
    auto marshalled = marshal_358c0(window, self, source);
    check("358c0 ok", marshalled.ok);
    check("358c0 table", marshalled.cells.table_field ==
                             reinterpret_cast<void**>(storage.data() + 0x20));
    check("358c0 copy", marshalled.cells.copy_field ==
                            reinterpret_cast<void**>(storage.data() + 0x10));
    check("358c0 source", marshalled.cells.source_field ==
                              reinterpret_cast<const void* const*>(storage.data() + 0x50));

    std::uint64_t value = 0xABCDEF;
    std::memcpy(storage.data() + 0x50, &value, sizeof(value));
    const auto result = sotc::vtable_15955c0_slot08_358c0(marshalled.cells);
    check("358c0 model ok", result.ok);
    check("358c0 table value", std::memcmp(storage.data() + 0x20, &result.table_written,
                                           sizeof(void*)) == 0);
    check("358c0 copy value", std::memcmp(storage.data() + 0x10, &value, sizeof(value)) == 0);
}

void test_13d20_and_187c0() {
    std::vector<std::uint8_t> storage(0x80, 0);
    GuestWindow window(storage.data(), 0x40000, storage.size());
    auto setter = marshal_13d20(window, 0x40000);
    check("13d20 ok", setter.ok);
    const auto written = sotc::vtable_1594ae8_slot28_13d20(0x12345678u, setter.cells);
    check("13d20 model", written.ok && written.written == 0x12345678u);

    auto teardown = marshal_187c0(window, 0x40000);
    check("187c0 ok", teardown.ok);
    auto bad = marshal_187c0(window, 0x50000);
    check("187c0 range fails", !bad.ok && bad.status == GuestStatus::out_of_range);
}

}  // namespace

int main() {
    test_20d60_marshals_flag_and_global();
    test_20d60_out_of_range();
    test_358c0_marshals_three_fields();
    test_13d20_and_187c0();

    if (failures == 0) {
        std::printf("OK test_retail_cells\n");
        return 0;
    }
    std::fprintf(stderr, "test_retail_cells: %d failure(s)\n", failures);
    return 1;
}
