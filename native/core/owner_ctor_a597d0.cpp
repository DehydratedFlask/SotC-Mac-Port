#include "owner_ctor_a597d0.hpp"

#include <cstring>
#include <type_traits>

namespace sotc {
namespace {

constexpr std::uint8_t kZero16[16] = {};
constexpr std::uint8_t kZero32[32] = {};

template <typename T>
void put(std::uint8_t* object, std::size_t offset, T value) noexcept {
    static_assert(std::is_trivially_copyable_v<T>);
    std::memcpy(object + offset, &value, sizeof value);
}

void put_bytes(std::uint8_t* object, std::size_t offset, const void* source,
               std::size_t size) noexcept {
    std::memcpy(object + offset, source, size);
}

void put_pointer(std::uint8_t* object, std::size_t offset, const void* value) noexcept {
    static_assert(sizeof(void*) == sizeof(std::uint64_t));
    std::memcpy(object + offset, &value, sizeof value);
}

// Report §4: one clear-loop record is three partial stores; +0x01..+0x07 and +0x1c..+0x1f stay at
// the caller's fill byte.
void clear_record(std::uint8_t* object, std::size_t offset) noexcept {
    object[offset + 0x00] = 0;
    put_bytes(object, offset + kOwnerA597D0RecordBytesOffset, kZero16, 16);
    put<std::uint32_t>(object, offset + kOwnerA597D0RecordDwordOffset, 0u);
}

OwnerRow14Binding* g_row14_binding = nullptr;

}  // namespace

static_assert(kOwnerA597D0RecordBase + kOwnerA597D0RecordCount * kOwnerA597D0RecordStride ==
              kOwnerA597D0RecordLimit);
static_assert(kOwnerA597D0RecordLimit == kOwnerA597D0DequeOffset);
static_assert(kOwnerA597D0MapBytes == kOwnerA597D0MapSlots * sizeof(void*));
static_assert(kOwnerA597D0ChunkTrips == 1);
static_assert(kOwnerA597D0DequeMapOffset + 0x08 == kOwnerA597D0DequeMapSizeOffset);
static_assert(kOwnerA597D0DequeMapSizeOffset + 0x08 == kOwnerA597D0DequeBeginOffset);
static_assert(kOwnerA597D0DequeBeginOffset + 0x20 == kOwnerA597D0DequeEndOffset);
static_assert(kOwnerA597D0DequeEndOffset + 0x20 == kOwnerA597D0DequeBytes);
static_assert(kOwnerA597D0DequeOffset + kOwnerA597D0DequeBytes + 0x08 ==
              kOwnerA597D0Qword888Offset);  // +0x880..+0x887 is a hole (never written)
static_assert(kOwnerA597D0EventOffset + 0x10 == kOwnerA597D0ThreadOffset);
static_assert(kOwnerA597D0OwnedPtr938Offset + 0x10 == kOwnerA597D0Qword948Offset);
static_assert(kOwnerA597D0Qword948Offset + 0x08 <= kOwnerCtorA597D0ObjSize);
static_assert(kOwnerDtorA59A10Rva > kOwnerCtorA597D0Rva + kOwnerCtorA597D0Size);
static_assert(kOwnerA597D0CallableWords == 4);
static_assert(kOwnerA597D0ThreadEntryRva != 0);

OwnerA597D0Result owner_ctor_a597d0(void* object, const OwnerA597D0Hooks& hooks) noexcept {
    if (object == nullptr) {
        return {false, "null-object"};
    }
    if (reinterpret_cast<std::uintptr_t>(object) % alignof(std::uint64_t) != 0) {
        return {false, "object-align"};
    }

    auto* const o = static_cast<std::uint8_t*>(object);

    // ---- head: 7 stores (0xa597ff..0xa5982d, report §1). -------------------------------------
    put<std::uint32_t>(o, kOwnerA597D0RefCountOffset, 1u);
    put_bytes(o, kOwnerA597D0VectorOffset, kZero16, 16);  // begin = end
    put<std::uint64_t>(o, kOwnerA597D0VectorOffset + 0x10, 0u);  // cap_end
    put<std::uint32_t>(o, kOwnerA597D0FlagsOffset, kOwnerA597D0FlagsValue);
    // Emitted as two 32 B zero stores, +0x860 first. The dword at +0x28 already left +0x2a == 1.
    put_bytes(o, kOwnerA597D0DequeOffset + kOwnerA597D0DequeEndOffset, kZero32, 32);
    put_bytes(o, kOwnerA597D0DequeOffset + kOwnerA597D0DequeBeginOffset, kZero32, 32);
    put<std::uint64_t>(o, kOwnerA597D0DequeOffset + kOwnerA597D0DequeMapSizeOffset,
                       kOwnerA597D0MapSlots);

    // ---- 0x17090(0x40): the deque map block (0xa59838). -------------------------------------
    if (hooks.allocate == nullptr) {
        return {false, "alloc_block"};
    }
    void* const map = hooks.allocate(kOwnerA597D0MapBytes);
    if (map == nullptr) {
        return {false, "alloc_block"};
    }
    auto* const map_bytes = static_cast<std::uint8_t*>(map);
    // The original stores the map pointer before allocating the chunk (0xa59844 < 0xa59865), so a
    // chunk failure keeps this prefix and the map slot +0x18 stays at the host's fill.
    put_pointer(o, kOwnerA597D0DequeOffset + kOwnerA597D0DequeMapOffset, map);

    // ---- 0x17090(0x100) inside a single-trip loop (0xa5984f..0xa59875, report §3). -----------
    if (hooks.allocate == nullptr) {
        return {false, "alloc_chunk"};
    }
    void* const chunk = hooks.allocate(kOwnerA597D0ChunkBytes);
    if (chunk == nullptr) {
        return {false, "alloc_chunk"};
    }
    put_pointer(map_bytes, 0x18, chunk);  // the loop wrote [r13] for r13 = map + 0x18

    // ---- deque begin/end iterators, in instruction order (0xa59877..0xa598be). --------------
    void* const map_slot = map_bytes + 0x18;
    auto* const chunk_bytes = static_cast<std::uint8_t*>(chunk);
    put_pointer(o, kOwnerA597D0DequeOffset + kOwnerA597D0DequeBeginOffset + 0x18, map_slot);
    put_pointer(o, kOwnerA597D0DequeOffset + kOwnerA597D0DequeBeginOffset + 0x08, chunk);
    put_pointer(o, kOwnerA597D0DequeOffset + kOwnerA597D0DequeBeginOffset + 0x10,
                chunk_bytes + kOwnerA597D0ChunkBytes);
    put_pointer(o, kOwnerA597D0DequeOffset + kOwnerA597D0DequeBeginOffset + 0x00, chunk);
    put_pointer(o, kOwnerA597D0DequeOffset + kOwnerA597D0DequeEndOffset + 0x18, map_slot);
    put_pointer(o, kOwnerA597D0DequeOffset + kOwnerA597D0DequeEndOffset + 0x08, chunk);
    put_pointer(o, kOwnerA597D0DequeOffset + kOwnerA597D0DequeEndOffset + 0x10,
                chunk_bytes + kOwnerA597D0ChunkBytes);
    put_pointer(o, kOwnerA597D0DequeOffset + kOwnerA597D0DequeEndOffset + 0x00, chunk);

    // ---- 0x888 / 0x8b0 (0xa598c5, 0xa598d0). ------------------------------------------------
    put<std::uint64_t>(o, kOwnerA597D0Qword888Offset, 0u);
    put<std::uint64_t>(o, kOwnerA597D0Qword8B0Offset, 0u);

    // ---- 0x3b3d0(this+0x8b8, 0, 0): the event flag object (0xa598db). -----------------------
    if (hooks.event_flag_create == nullptr ||
        !hooks.event_flag_create(o + kOwnerA597D0EventOffset, kOwnerA597D0EventFlagByte,
                                 kOwnerA597D0EventPattern)) {
        return {false, "event_flag"};
    }

    // ---- 0x3b5c0: 7 arguments (0xa598e0..0xa59924, report §2). ------------------------------
    // rbp-0x50 is a 0x20 B callable: qword 0 = this, qword 1 = the relocated slot 0x15fff08
    // (addend 0xb09590), qwords 2/3 = 0.
    OwnerA597D0ThreadRequest request{};
    request.thread_object = o + kOwnerA597D0ThreadOffset;
    request.callable[0] = reinterpret_cast<std::uint64_t>(o);
    request.callable[1] = kOwnerA597D0ThreadEntryRva;
    request.callable[2] = 0;
    request.callable[3] = 0;
    request.name = kOwnerA597D0ThreadName;
    request.arg3 = kOwnerA597D0ThreadArg3;
    request.arg4 = kOwnerA597D0ThreadArg4;
    request.arg5 = kOwnerA597D0ThreadArg5;
    request.stack_arg = kOwnerA597D0ThreadArg6;
    if (hooks.thread_create == nullptr || !hooks.thread_create(request)) {
        return {false, "thread_create"};
    }

    // ---- +0x938..+0x94f zero (0xa59934, 0xa5993c). -------------------------------------------
    put_bytes(o, kOwnerA597D0OwnedPtr938Offset, kZero16, 16);
    put<std::uint64_t>(o, kOwnerA597D0Qword948Offset, 0u);

    // ---- the 64-trip clear loop (0xa5992d..0xa59970, report §4). -----------------------------
    // `mov rax,-0x800` .. `add rax,0x20` .. `jne`: 0x800 / 0x20 == 64 records from +0x30.
    for (std::size_t index = 0; index < kOwnerA597D0RecordCount; ++index) {
        clear_record(o, kOwnerA597D0RecordBase + index * kOwnerA597D0RecordStride);
    }

    // ---- +0x29 = 0, +0x2b = 1, then 0xba40 (0xa59972..0xa5998b). -----------------------------
    put<std::uint8_t>(o, kOwnerA597D0Byte29Offset, 0u);
    put<std::uint8_t>(o, kOwnerA597D0ModeByteOffset, 1u);
    if (hooks.startup_mode_notify == nullptr ||
        !hooks.startup_mode_notify(o + kOwnerA597D0ModeByteOffset)) {
        return {false, "startup_mode"};
    }

    // ---- tail: +0x890, +0x898..+0x8a7, +0x8a8 (0xa5999b..0xa599aa). --------------------------
    put<std::uint8_t>(o, kOwnerA597D0Byte890Offset, 0u);
    put_bytes(o, kOwnerA597D0Block898Offset, kZero16, 16);
    put<std::uint32_t>(o, kOwnerA597D0Dword8A8Offset, 0u);

    return {true, nullptr};
}

void owner_row14_bind(OwnerRow14Binding* binding) noexcept {
    g_row14_binding = binding;
}

bool owner_row14_ctor_hook(std::size_t row, void* object, void* arg) noexcept {
    static_cast<void>(arg);
    if (row != 14 || g_row14_binding == nullptr) {
        return false;
    }
    g_row14_binding->last = owner_ctor_a597d0(object, g_row14_binding->hooks);
    ++g_row14_binding->calls;
    return g_row14_binding->last.ok;
}

}  // namespace sotc
