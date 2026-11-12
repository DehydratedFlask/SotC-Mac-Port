// CPU-01-STARTUP-SLOT-554-BATCH-13: model/fixture checks.
#include "core/ctor_slot_554_batch13_8849d.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>

using namespace sotc;

int main() {
    static_assert(kSlot554B13ObjectCount == 32);
    static_assert(kSlot554B13BlockCount == 31);
    static_assert(kSlot554B13TotalSize == 0x19a8);
    static_assert(kSlot554B13Written + kSlot554B13Holes == kSlot554B13TotalSize);
    static_assert(kSlot554B13ObjectEndRva - kSlot554B13ObjectBaseRva == kSlot554B13BlockSize);

    std::array<std::uint8_t, 0x600> page{};
    for (std::size_t i = 0; i < page.size(); ++i) page[i] = static_cast<std::uint8_t>(i * 7);
    Slot554Batch13Objects objects{};
    ConCmdRegistry registry{};
    const auto result = init_slot554_batch13(objects, page.data(), registry, default_slot554_batch13_hooks());
    assert(result.ok);
    assert(result.built == 32);
    assert(result.registered == 32);
    assert(result.atexit_records == 32);
    assert(result.views_inert == 62 + 2);  // block: 30 variable views; dummy: name+description
    assert(result.blocked_index == 31);

    std::size_t written = 0;
    for (std::size_t i = 0; i < kSlot554B13TotalSize; ++i) written += slot554_batch13_written(i) ? 1 : 0;
    assert(written == kSlot554B13Written);

    // Store-trace masks: dummy 149/59 and owned d8 rows 175/41.
    std::size_t dummy_written = 0, owned_written = 0;
    for (std::size_t i = 0; i < kSlot554B13DummySize; ++i)
        dummy_written += slot554_batch13_object_written(kSlot554B13BlockCount, i) ? 1 : 0;
    for (std::size_t i = 0; i < kSlot554B13Specs[15].size; ++i)
        owned_written += slot554_batch13_object_written(15, i) ? 1 : 0;
    assert(dummy_written == 149 && owned_written == 175);

    std::size_t off = 0;
    for (std::uint32_t i = 0; i < kSlot554B13BlockCount; ++i) {
        const auto& s = kSlot554B13Specs[i];
        assert(s.object_rva == kSlot554B13ObjectBaseRva + off);
        assert(s.size == (s.kind == Slot554B13Kind::bool_var ? 0xc0 : 0xd0) ||
               s.size == 0xd8);
        off += s.size;
    }
    assert(off == kSlot554B13BlockSize);

    // name lengths and exact object-hosted/default values
    assert(std::strlen(kSlot554B13Specs[0].name) == kSlot554B13Specs[0].name_length);
    const auto* first = reinterpret_cast<const ConCmdRangedVar*>(objects.block.data());
    assert(first->min_engaged == 1 && first->max_engaged == 1);
    const auto* owned = reinterpret_cast<const ConCmdRangedVar*>(objects.block.data() + (0x2ebe3e8 - kSlot554B13ObjectBaseRva));
    assert(owned->value == 3);
    const auto* owned2 = reinterpret_cast<const ConCmdRangedVar*>(objects.block.data() + (0x2ebe4c0 - kSlot554B13ObjectBaseRva));
    assert(owned2->value == 2);
    const auto* dummy = reinterpret_cast<const ConCmdCommand*>(objects.dummy.data());
    assert(dummy->base.flags == kSlot554B13DummyFlags);
    assert(dummy->base.name.size == kSlot554B13DummyNameLength);
    assert(dummy->delegate.handler == kSlot554B13DummyHandlerRva);
    return 0;
}
