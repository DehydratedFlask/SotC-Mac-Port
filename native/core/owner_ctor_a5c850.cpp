#include "owner_ctor_a5c850.hpp"

#include <atomic>
#include <cstring>
#include <type_traits>

#include "init_sub_3be930.hpp"

namespace sotc {
namespace {

// 0xfab6b0: three rows of {1.0f, 0.0f, 0.0f, 0.0f} (48 B). The original copies it as two
// overlapping 32 B vectors per 48 B block, which is why four 32 B stores cover two blocks.
constexpr std::uint32_t kQuadConstant[12] = {
    0x3F800000u, 0u, 0u, 0u,
    0x3F800000u, 0u, 0u, 0u,
    0x3F800000u, 0u, 0u, 0u,
};

// 0x12c49a0: eight 1.0f (32 B), the source of the three 32 B stores and the two 1.0f in between.
constexpr std::uint32_t kOnesConstant[8] = {
    0x3F800000u, 0x3F800000u, 0x3F800000u, 0x3F800000u,
    0x3F800000u, 0x3F800000u, 0x3F800000u, 0x3F800000u,
};

constexpr std::uint64_t kTwoOnes = 0x3F8000003F800000ull;
constexpr std::uint8_t kZero32[32] = {};

void put_bytes(std::uint8_t* object, std::size_t offset, const void* source,
               std::size_t size) noexcept {
    std::memcpy(object + offset, source, size);
}

template <typename T>
void put(std::uint8_t* object, std::size_t offset, T value) noexcept {
    static_assert(std::is_trivially_copyable_v<T>);
    std::memcpy(object + offset, &value, sizeof value);
}

void put_pointer(std::uint8_t* object, std::size_t offset, const void* value) noexcept {
    static_assert(sizeof(void*) == sizeof(std::uint64_t));
    std::memcpy(object + offset, &value, sizeof value);
}

// Report §5.2: begin == end == inline marker == buffer, cap-end == buffer + buffer bytes, and the
// fourth slot (header + 0x18) is never written by this constructor.
void write_vector_header(std::uint8_t* object, const OwnerA5C850Vector& vector) noexcept {
    void* const buffer = object + vector.buffer;
    void* const cap_end = object + vector.cap_end;
    put_pointer(object, vector.header + 0x00, buffer);
    put_pointer(object, vector.header + 0x08, buffer);
    put_pointer(object, vector.header + 0x10, cap_end);
    // vector.header + 0x18: hole, kept from report §8.
    put_pointer(object, vector.header + 0x20, buffer);
}

}  // namespace

// Report §5.2 table. Element strides are the destructor's (0x38/0x28/0x48/0xc8/0xc8), not guesses;
// the +0x68a0 buffer has no element destructor, so its element size stays unproven (report R3).
constexpr OwnerA5C850Vector kOwnerA5C850Vectors[kOwnerA5C850VecCount] = {
    {0x0028, 0x0050, 0x1C50, 0x38},  // 128 x 0x38, buffer 0x1C00
    {0x68A0, 0x68C8, 0x69C8, 0x00},  // buffer 0x100, POD, 8 or 32 elements unproven
    {0x6AC0, 0x6AE8, 0x6FE8, 0x28},  // 32 x 0x28, buffer 0x500
    {0x6FE8, 0x7010, 0x7250, 0x48},  // 8 x 0x48, buffer 0x240
    {0x72A8, 0x72D0, 0x8BD0, 0xC8},  // 32 x 0xC8, buffer 0x1900
    {0x8BD0, 0x8BF8, 0xA4F8, 0xC8},  // 32 x 0xC8, buffer 0x1900
};

static_assert(kOwnerA5C850RecBase + kOwnerA5C850RecCount * kOwnerA5C850RecStride ==
              kOwnerA5C850RecLimit);
static_assert(kOwnerA5C850LaneArrayOffset + kOwnerA5C850LaneCount * kOwnerA5C850LaneStride == 0x68A0);
static_assert(kOwnerA5C850OnesOffset + kOwnerA5C850OnesCount * 4 == 0x6ABC);
static_assert(kOwnerA5C850TreeOffset + 0x18 == kOwnerA5C850TreeBodyOffset);
static_assert(kOwnerA5C850TreeBodyOffset + 0x20 == kOwnerA5C850Vectors[4].header);
static_assert(kOwnerA5C850SoundRecordBytes == 24);
static_assert(kOwnerDtorA5CB30Rva == kOwnerCtorA5C850Rva + kOwnerCtorA5C850Size + 15);

namespace {

constexpr bool vectors_are_well_formed() {
    for (const OwnerA5C850Vector& vector : kOwnerA5C850Vectors) {
        if (vector.header + 0x28 != vector.buffer) {
            return false;
        }
        if (vector.cap_end <= vector.buffer) {
            return false;
        }
        if (vector.elem != 0 && (vector.cap_end - vector.buffer) % vector.elem != 0) {
            return false;
        }
    }
    return true;
}

static_assert(vectors_are_well_formed());

}  // namespace

OwnerA5C850Result owner_ctor_a5c850(void* object, std::uint8_t* sound_record,
                                    const OwnerA5C850Hooks& hooks) noexcept {
    if (object == nullptr) {
        return {false, "null-object"};
    }
    if (sound_record == nullptr) {
        return {false, "sound-record"};
    }
    if (reinterpret_cast<std::uintptr_t>(object) % alignof(std::uint64_t) != 0) {
        return {false, "object-align"};
    }

    auto* const o = static_cast<std::uint8_t*>(object);

    // ---- B0: scalar head, 7 stores (0xa5c85d..0xa5c87a, report §5.1). ---------------------
    o[0x00] = 0;
    o[0x01] = 0;
    o[0x02] = 0;
    put<std::uint32_t>(o, 0x04, 0u);
    put<std::uint32_t>(o, 0x08, 0xFFFFFFFFu);
    o[0x10] = 0;  // single byte; +0x11..+0x17 stay untouched (report R1)
    o[0x20] = 0;  // dtor gate; the ctor guarantees it is 0

    // ---- B0: first inline vector header at +0x28 (4 stores). ------------------------------
    write_vector_header(o, kOwnerA5C850Vectors[0]);

    // ---- B1: 256 records at +0x1c50, stride 0x40, 7 partial stores each (report §5.3). ----
    // Holes per record: +0x08..+0x2b, +0x30..+0x33. (+0x38 is a qword store, so +0x3c..+0x3f
    // is written too.) Do not clear them.
    for (std::size_t i = 0; i < kOwnerA5C850RecCount; ++i) {
        const std::size_t record = kOwnerA5C850RecBase + i * kOwnerA5C850RecStride;
        put<std::uint32_t>(o, record + 0x00, 0xFFFFFFFFu);
        o[record + 0x04] = 0;
        o[record + 0x05] = 0;
        put<std::uint16_t>(o, record + 0x06, 0u);
        put<std::uint32_t>(o, record + 0x2C, 0u);
        put<std::uint32_t>(o, record + 0x34, 0u);
        put<std::uint64_t>(o, record + 0x38, 0u);
    }

    // ---- B2: lane/atomic group, 8 stores (0xa5c8d3..0xa5c934, report §5.4). ----------------
    put<std::uint32_t>(o, kOwnerA5C850LaneGroupOffset, 0u);
    {
        // 0xa5c8f4 `xchg qword [rbx+0x5c60], rax`: a sequentially consistent store whose loaded
        // value is discarded. The original writes no lane (+0x5ca0..+0x68a0).
        auto* const slot = reinterpret_cast<std::uint64_t*>(o + kOwnerA5C850LaneAtomicOffset);
        const std::uint64_t previous = std::atomic_ref<std::uint64_t>(*slot).exchange(
            0u, std::memory_order_seq_cst);
        static_cast<void>(previous);
    }
    put<std::uint64_t>(o, 0x5C70, 0u);  // drain counter pointer: 0 => dtor drain is a no-op
    put<std::uint64_t>(o, 0x5C90, ~std::uint64_t{0});
    put<std::uint32_t>(o, 0x5C98, 0u);
    put<std::uint64_t>(o, 0x5C80, 0u);
    put_pointer(o, 0x5C88, o + kOwnerA5C850LaneArrayOffset);
    put<std::uint32_t>(o, 0x5C9C, static_cast<std::uint32_t>(kOwnerA5C850LaneCount));

    // ---- B2: second vector header (+0x68a0), then the 0x69c8 constants (report §5.5). -----
    write_vector_header(o, kOwnerA5C850Vectors[1]);
    put<std::uint32_t>(o, 0x69C8, 0xFFFFFFFFu);
    // Two 48 B copies, each emitted as src+0x00 -> dst and src+0x10 -> dst+0x10 (32 B vectors).
    put_bytes(o, 0x69DC, &kQuadConstant[4], 32);
    put_bytes(o, 0x69CC, &kQuadConstant[0], 32);
    put_bytes(o, 0x6A0C, &kQuadConstant[4], 32);
    put_bytes(o, 0x69FC, &kQuadConstant[0], 32);
    put_bytes(o, kOwnerA5C850ConstZeroOffset, kZero32, 16);

    // ---- B2: third and fourth vector headers (+0x6ac0, +0x6fe8). --------------------------
    write_vector_header(o, kOwnerA5C850Vectors[2]);
    write_vector_header(o, kOwnerA5C850Vectors[3]);

    // ---- 0xf180(this+0x7250): 0x20 B empty shared handle, already native. -----------------
    // `null_payload` only replaces the stored payload; everything else is the reused lift.
    void* payload = nullptr;
    if (hooks.null_payload != nullptr) {
        payload = hooks.null_payload();
        if (payload == nullptr) {
            return {false, "null_payload"};
        }
    }
    init_slot_f180(o + kOwnerA5C850SharedOffset);
    if (payload != nullptr) {
        put_pointer(o, kOwnerA5C850SharedOffset, payload);
    }

    // ---- 0xb09650(this+0x7270): tree header. ----------------------------------------------
    if (hooks.tree_init == nullptr || !hooks.tree_init(o + kOwnerA5C850TreeOffset)) {
        return {false, "tree_init"};
    }

    // ---- B2 continued: +0x7288 zero, vectors 5/6, polymorphic pointer (10 stores). ---------
    put_bytes(o, kOwnerA5C850TreeBodyOffset, kZero32, 32);
    write_vector_header(o, kOwnerA5C850Vectors[4]);
    write_vector_header(o, kOwnerA5C850Vectors[5]);
    put<std::uint64_t>(o, kOwnerA5C850PolyOffset, 0u);  // dtor calls [vptr+8] on this

    // ---- 0x3b3d0(this+0xa500, pattern 0) and (this+0xa510, pattern 1). -------------------
    if (hooks.event_flag_create == nullptr ||
        !hooks.event_flag_create(o + kOwnerA5C850Event0Offset, kOwnerA5C850EventPattern0)) {
        return {false, "event_flag_0"};
    }
    if (hooks.event_flag_create == nullptr ||
        !hooks.event_flag_create(o + kOwnerA5C850Event1Offset, kOwnerA5C850EventPattern1)) {
        return {false, "event_flag_1"};
    }

    // ---- +0xa520 u32 0, then 0x11bc0("Sound", "Sound", create=false). --------------------
    put<std::uint32_t>(o, kOwnerA5C850TailOffset, 0u);
    if (hooks.name_record == nullptr) {
        return {false, "name_record"};
    }
    const void* const record =
        hooks.name_record(kOwnerA5C850SoundName, kOwnerA5C850SoundName, false);
    if (record == nullptr) {
        // Report R4: with create == false an absent name has no record; the original would fault
        // on the load at 0xa5cab0. Never fabricate one.
        return {false, "name_record"};
    }

    // ---- 24 B copy into the global at 0x32f97f8: 8 B at +0x10, then 16 B at +0x00. --------
    const auto* const entry = static_cast<const std::uint8_t*>(record);
    std::memcpy(sound_record + 0x10, entry + 0x10, 8);
    std::memcpy(sound_record + 0x00, entry + 0x00, 16);

    // ---- 0x6a3c..0x6abc: 32 x 1.0f, store widths as emitted (4,4,32,32,32,8,8,8). --------
    put<std::uint32_t>(o, kOwnerA5C850OnesOffset + 0x00, kOnesConstant[0]);
    put<std::uint32_t>(o, kOwnerA5C850OnesOffset + 0x04, kOnesConstant[0]);
    put_bytes(o, kOwnerA5C850OnesOffset + 0x08, kOnesConstant, 32);
    put_bytes(o, kOwnerA5C850OnesOffset + 0x28, kOnesConstant, 32);
    put_bytes(o, kOwnerA5C850OnesOffset + 0x48, kOnesConstant, 32);
    put<std::uint64_t>(o, kOwnerA5C850OnesOffset + 0x68, kTwoOnes);
    put<std::uint64_t>(o, kOwnerA5C850OnesOffset + 0x70, kTwoOnes);
    put<std::uint64_t>(o, kOwnerA5C850OnesOffset + 0x78, kTwoOnes);

    return {true, nullptr};
}

}  // namespace sotc
