// CPU-01-AOT-CFG-SCALE-01-BATCH25 (slot 495): fixture for startup constructor index 495
// (0x268ee0). Asserts the command object, the parsed blob record, the twelve item records and the
// fourteen __cxa_atexit registrations in body order.
#include "l4_test_util.hpp"

#include <cstring>

#include "core/ctor_slot_495_268ee0.hpp"

namespace {
using namespace l4;
using namespace sotc;

void constants() {
    CHECK(kSlot495Rva == 0x268ee0 && kSlot495Size == 1698);
    CHECK(kSlot495TableWord == 0x2dd70e0 - 8 * 495 && kSlot495TableWord == 0x2dd6168);
    CHECK(kSlot495Command == 0x2eeebb0 && kSlot495Region == 0x2eeec70);
    CHECK(kSlot495BlobRecord == 0x2eeeca8 && kSlot495Item0 == 0x2eeecd8);
    CHECK(kSlot495ItemCount == 12 && kSlot495ItemStride == 0x48);
    CHECK(kSlot495CommandSize == 0xc0 && sizeof(Slot495Command) == 0xc0);
    CHECK(kSlot495Item0 + kSlot495ItemCount * kSlot495ItemStride == 0x2eef038);
    CHECK(kSlot495Vtable == 0x15940a0 && kSlot495Vptr == 0x15940b0);
    CHECK(kSlot495Vptr == kSlot495Vtable + 0x10);
    CHECK(kSlot495PreVptr == 0x1594218 && kSlot495TagB0Value == 0xc2a0);
    CHECK(kSlot495Flags == 1);
    CHECK(kSlot495CommandDtor == 0x20b0 && kSlot495BlobDtor == 0x209d80 && kSlot495ItemDtor == 0x268e20);
    CHECK(kSlot495NameLength == 30 && kSlot495DescriptionLength == 0);
    CHECK(kSlot495BlobDwords[0] == 0x10000020 && kSlot495BlobDwords[1] == 0xbeefbeef);
    CHECK(kSlot495BlobA == 0x20 && kSlot495BlobSize == 0x20 && kSlot495BlobBits == 2);
    CHECK(kSlot495BlobBytes == 1 && kSlot495BlobTotal == 0x21 && kSlot495BlobDwordCount == 2);
    CHECK(kSlot495CanaryCell == 0x15fe518 && kSlot495StackChkFailRva == 0xf8f3a0);

    for (std::size_t i = 0; i < kSlot495ItemCount; ++i) {
        const Slot495ItemSpec& s = kSlot495Items[i];
        CHECK(std::strlen(s.text) == s.length);
        CHECK(s.object_rva == kSlot495Item0 + i * kSlot495ItemStride);
        if (i > 0) CHECK(s.text_rva > kSlot495Items[i - 1].text_rva);
    }
    CHECK(std::strlen("network_AllowCrossPlatformPlay") == kSlot495NameLength);
}

void run_full() {
    ConCmdRegistry registry;
    Slot495Objects o;
    poison(o);
    const Slot495Result res = init_slot495_268ee0(o, registry);

    CHECK(res.ok && res.canary_matched && res.blob_parsed);
    CHECK(res.items == kSlot495ItemCount && res.atexit_records == 14);
    CHECK(registry.atexit_log.size() == 14);
    CHECK(registry.pending.size() == 1);

    // 1. command object 0x2eeebb0.
    const ConCmdBase& cmd = o.command.base;
    CHECK(cmd.vptr == kSlot495Vptr);
    CHECK(cmd.flags == 1 && cmd.registered == 1);
    CHECK(cmd.name.size == static_cast<std::int64_t>(kSlot495NameLength));
    CHECK(std::memcmp(cmd.name.data, "network_AllowCrossPlatformPlay", kSlot495NameLength) == 0);
    CHECK(cmd.description.size == 0);
    CHECK(o.command.tail.region == reinterpret_cast<std::uint64_t>(o.region60));
    CHECK(o.command.tail.flag69 == 0);
    for (std::uint8_t b : o.command.tail.zero70) CHECK(b == 0);
    CHECK(o.command.tail.zeroA0 == 0 && o.command.tail.tagB0 == 0xc2a0 && o.command.tail.zeroB8 == 0);

    // 2. parsed blob record.
    CHECK(o.blob.valid == 1);
    CHECK(o.blob.a == kSlot495BlobA && o.blob.size == kSlot495BlobSize);
    CHECK(o.blob.bits == kSlot495BlobBits && o.blob.bytes == kSlot495BlobBytes);
    CHECK(o.blob.total == kSlot495BlobTotal);
    CHECK(o.blob.data != nullptr && o.blob.data[0] == 0x10000020 && o.blob.data[1] == 0xbeefbeef);
    CHECK(o.arena.control[0] == 0 && o.arena.control[1] == 1);

    // 3. items and the atexit order.
    CHECK(registry.atexit_log.at(0).dtor_rva == kSlot495CommandDtor);
    CHECK(registry.atexit_log.at(0).object == &o.command);
    CHECK(registry.atexit_log.at(1).dtor_rva == kSlot495BlobDtor);
    CHECK(registry.atexit_log.at(1).object == &o.blob);
    for (std::size_t i = 0; i < kSlot495ItemCount; ++i) {
        const Slot495Item& item = o.items[i];
        const Slot495ItemSpec& spec = kSlot495Items[i];
        CHECK(item.text.size == static_cast<std::int64_t>(spec.length));
        CHECK(std::memcmp(item.text.data, spec.text, spec.length) == 0);
        CHECK(item.zero20 == 0 && item.zero30 == 0 && item.zero38 == 0 && item.zero40 == 0);
        CHECK(registry.atexit_log.at(2 + i).dtor_rva == kSlot495ItemDtor);
        CHECK(registry.atexit_log.at(2 + i).object == &item);
    }
}

void run_hook_refusal() {
    ConCmdRegistry registry;
    Slot495Objects o;
    poison(o);
    o.blob = BlobHeader{};   // the parse refuses before it writes the record
    BlobHooks half{};
    half.allocate = [](std::size_t) noexcept -> void* { return nullptr; };
    const Slot495Result res = init_slot495_268ee0(o, registry, half);
    // The command and its atexit record exist before the parse; the parse refuses and stops.
    CHECK(!res.ok && !res.blob_parsed);
    CHECK(res.atexit_records == 1 && registry.atexit_log.size() == 1);
    CHECK(res.items == 0 && o.blob.control == nullptr);
}

}  // namespace

int main() {
    constants();
    run_full();
    run_hook_refusal();
    std::printf("OK test_ctor_slot_495\n");
    return 0;
}
