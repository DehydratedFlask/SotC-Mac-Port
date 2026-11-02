#include "../core/vtable_15949a0_slot58_c2c0.hpp"
#include "../core/vtable_1594ae8_slot40_13d50.hpp"
#include "../core/vtable_1594ae8_slot50_13d60.hpp"
#include "../core/vtable_1595320_slot10_30e50.hpp"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string_view>

namespace {

using sotc::Vtable15949a0Slot58_c2c0Cells;
using sotc::Vtable15949a0Slot58_c2c0Result;
using sotc::Vtable1594ae8Slot40_13d50Result;
using sotc::Vtable1594ae8Slot50_13d60Cells;
using sotc::Vtable1594ae8Slot50_13d60Result;
using sotc::Vtable1595320Slot10_30e50Hooks;
using sotc::Vtable1595320Slot10_30e50Result;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

void test_constants() {
    check("13d50 rva", sotc::kVtable1594ae8Slot40_13d50Rva == 0x13d50);
    check("13d50 size", sotc::kVtable1594ae8Slot40_13d50Size == 0x3);
    check("13d50 reloc", sotc::kVtable1594ae8Slot40_13d50RelocAddress == 0x1594b28);
    check("13d50 slot", sotc::kVtable1594ae8Slot40_13d50SlotOffset == 0x40);

    check("13d60 rva", sotc::kVtable1594ae8Slot50_13d60Rva == 0x13d60);
    check("13d60 size", sotc::kVtable1594ae8Slot50_13d60Size == 0x9);
    check("13d60 reloc", sotc::kVtable1594ae8Slot50_13d60RelocAddress == 0x1594b38);
    check("13d60 base", sotc::kVtable1594ae8Slot50_13d60BaseOffset == 0x60);
    check("13d60 field", sotc::kVtable1594ae8Slot50_13d60FieldOffset == 0x70);

    check("c2c0 rva", sotc::kVtable15949a0Slot58_c2c0Rva == 0xc2c0);
    check("c2c0 size", sotc::kVtable15949a0Slot58_c2c0Size == 0x14);
    check("c2c0 reloc", sotc::kVtable15949a0Slot58_c2c0RelocAddress == 0x15949f8);
    check("c2c0 stamp", sotc::kVtable15949a0Slot58_c2c0StampAddress == 0x1594280);

    check("30e50 rva", sotc::kVtable1595320Slot10_30e50Rva == 0x30e50);
    check("30e50 size", sotc::kVtable1595320Slot10_30e50Size == 0x5);
    check("30e50 reloc", sotc::kVtable1595320Slot10_30e50RelocAddress == 0x1595330);
    check("30e50 tail", sotc::kVtable1595320Slot10_30e50TailTargetRva == 0x312a0);
}

void test_zero_return() {
    const Vtable1594ae8Slot40_13d50Result result = sotc::vtable_1594ae8_slot40_13d50();
    check("13d50 ok", result.ok);
    check("13d50 zero", result.code == 0);
}

void test_self_pointer_store() {
    const Vtable1594ae8Slot50_13d60Result empty = sotc::vtable_1594ae8_slot50_13d60();
    check("13d60 null fails", !empty.ok);

    alignas(8) std::uint8_t object[0x78] = {};
    const std::uint64_t marker = 0x1234;
    std::memcpy(object + 0x70, &marker, sizeof(marker));

    Vtable1594ae8Slot50_13d60Cells cells{};
    cells.object = object;
    const Vtable1594ae8Slot50_13d60Result result = sotc::vtable_1594ae8_slot50_13d60(cells);
    check("13d60 ok", result.ok);
    check("13d60 stored", result.stored == object + 0x60);
    check("13d60 previous", result.previous == reinterpret_cast<void*>(marker));

    void* stored = nullptr;
    std::memcpy(&stored, object + 0x70, sizeof(stored));
    check("13d60 memory", stored == object + 0x60);
}

void test_stamp_and_copy() {
    const Vtable15949a0Slot58_c2c0Result empty = sotc::vtable_15949a0_slot58_c2c0();
    check("c2c0 null fails", !empty.ok);

    alignas(8) std::uint8_t object[0x28] = {};
    alignas(8) std::uint8_t source[0x18] = {};
    const std::uint64_t copied = 0xfeedfacecafebeefull;
    std::memcpy(source + 0x10, &copied, sizeof(copied));
    const std::uint64_t previous = 0x55;
    std::memcpy(object + 0x10, &previous, sizeof(previous));

    Vtable15949a0Slot58_c2c0Cells cells{};
    cells.object = object;
    cells.source = source;
    const Vtable15949a0Slot58_c2c0Result result = sotc::vtable_15949a0_slot58_c2c0(cells);
    check("c2c0 ok", result.ok);
    check("c2c0 stamp", result.stamp == 0x1594280);
    check("c2c0 copied", result.copied == copied);
    check("c2c0 previous", result.previous == previous);

    std::uint64_t stamp = 0;
    std::memcpy(&stamp, object + 0x20, sizeof(stamp));
    check("c2c0 stamp memory", stamp == 0x1594280);
    std::uint64_t at16 = 0;
    std::memcpy(&at16, object + 0x10, sizeof(at16));
    check("c2c0 copy memory", at16 == copied);
}

std::int32_t tail_stub(void* self) {
    return self == reinterpret_cast<void*>(0x42) ? 7 : -1;
}

void test_tail_forward() {
    const Vtable1595320Slot10_30e50Result missing = sotc::vtable_1595320_slot10_30e50();
    check("30e50 missing hook", !missing.ok);
    check("30e50 missing not forwarded", !missing.forwarded);

    Vtable1595320Slot10_30e50Hooks hooks{};
    hooks.tail_312a0 = tail_stub;
    const Vtable1595320Slot10_30e50Result result =
        sotc::vtable_1595320_slot10_30e50(reinterpret_cast<void*>(0x42), hooks);
    check("30e50 ok", result.ok);
    check("30e50 forwarded", result.forwarded);
    check("30e50 code", result.code == 7);
}

}  // namespace

int main() {
    test_constants();
    test_zero_return();
    test_self_pointer_store();
    test_stamp_and_copy();
    test_tail_forward();

    if (failures == 0) {
        std::printf("OK test_vtable_siblings_low\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_siblings_low: %d failure(s)\n", failures);
    return 1;
}
