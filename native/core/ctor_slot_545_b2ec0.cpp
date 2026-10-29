#include "ctor_slot_545_b2ec0.hpp"

#include <algorithm>
#include <cstring>

namespace sotc {
namespace {

static_assert(sizeof(std::uintptr_t) == sizeof(std::uint64_t));

constexpr std::array<std::uint32_t, 16> kObjectPrefixWords{
    0x3F800000, 0, 0, 0, 0x3F800000, 0, 0, 0,
    0x3F800000, 0, 0, 0, 0xFF1B6468, 0xFF1B6416, 0xFF1B640C, 0xFF1B6402,
};

constexpr std::array<std::uint32_t, 8> kBasisWords{
    0x3F800000, 0, 0, 0, 0x3F800000, 0, 0, 0,
};

constexpr std::size_t kPairOneFirst = 0x158;
constexpr std::size_t kPairOneSecond = 0x178;
constexpr std::size_t kPairTwoFirst = 0x30;
constexpr std::size_t kPairTwoSecond = 0x50;

void store_u32(std::array<std::byte, kSlot545StateSize>& bytes, std::size_t offset,
               std::uint32_t value) noexcept {
    std::memcpy(bytes.data() + offset, &value, sizeof(value));
}

void store_pointer(std::byte* object, std::size_t offset, const void* pointer) noexcept {
    const auto value = reinterpret_cast<std::uintptr_t>(pointer);
    std::memcpy(object + offset, &value, sizeof(value));
}

void* load_pointer(const std::byte* object, std::size_t offset) noexcept {
    std::uintptr_t value = 0;
    std::memcpy(&value, object + offset, sizeof(value));
    return reinterpret_cast<void*>(value);
}

Slot545Matrix transform_a(float z_translation) noexcept {
    return Slot545Matrix{
        1.0F, 0.0F, 0.0F, 0.0F,
        0.0F, 1.0F, 0.0F, 0.0F,
        0.0F, 0.0F, 1.0F, 0.0F,
        0.5F, 0.5F, z_translation, 1.0F,
    };
}

Slot545Matrix transform_b(float y_scale) noexcept {
    return Slot545Matrix{
        1.0F, 0.0F, 0.0F, 0.0F,
        0.0F, y_scale, 0.0F, 0.0F,
        0.0F, 0.0F, 1.0F, 0.0F,
        0.0F, 0.0F, 0.0F, 1.0F,
    };
}

}  // namespace

Slot545ObjectView slot545_object_view(Slot545State& state) noexcept {
    return Slot545ObjectView{state.bytes.data() + kSlot545ObjectOffset};
}

void slot545_matrix_multiply(Slot545Matrix& out, const Slot545Matrix& left,
                             const Slot545Matrix& right) noexcept {
    const Slot545Matrix lhs = left;
    const Slot545Matrix rhs = right;
    Slot545Matrix result{};

    for (std::size_t column = 0; column != 4; ++column) {
        for (std::size_t row = 0; row != 4; ++row) {
            const float p0 = lhs[row] * rhs[column * 4];
            const float p1 = lhs[4 + row] * rhs[column * 4 + 1];
            const float s1 = p0 + p1;
            const float p2 = lhs[8 + row] * rhs[column * 4 + 2];
            const float s2 = s1 + p2;
            const float p3 = lhs[12 + row] * rhs[column * 4 + 3];
            result[column * 4 + row] = s2 + p3;
        }
    }
    out = result;
}

Slot545Result init_slot545_b2ec0(Slot545State& state, StartupAtexitLog* log,
                                 std::uint64_t entry_guard,
                                 std::uint64_t exit_guard) noexcept {
    const Slot545Matrix first_a = transform_a(0.0F);
    const Slot545Matrix first_b{
        0.5F, 0.0F, 0.0F, 0.0F,
        0.0F, -0.5F, 0.0F, 0.0F,
        0.0F, 0.0F, 1.0F, 0.0F,
        0.0F, 0.0F, 0.0F, 1.0F,
    };
    const Slot545Matrix second_a = transform_a(0.0F);
    const Slot545Matrix second_b = transform_b(-1.0F);
    const Slot545Matrix third_a = transform_a(0.5F);

    const auto store_matrix = [&state](std::size_t offset, const Slot545Matrix& matrix) {
        std::memcpy(state.bytes.data() + offset, matrix.data(), sizeof(matrix));
    };
    Slot545Matrix output{};
    slot545_matrix_multiply(output, first_a, first_b);
    store_matrix(0x00, output);
    slot545_matrix_multiply(output, second_a, second_b);
    store_matrix(0x40, output);
    slot545_matrix_multiply(output, third_a, second_b);
    store_matrix(0x80, output);

    // The 32-byte constant copy begins at global offset +0xc8; the following scalar store
    // overwrites the first word at +0xe8 with 1.0f, matching 0xb30ac and 0xb30c8.
    std::memcpy(state.bytes.data() + kSlot545BasisOffset, kBasisWords.data(), sizeof(kBasisWords));
    store_u32(state.bytes, kSlot545ScalarOffset, 0x3F800000);

    auto* const object = state.bytes.data() + kSlot545ObjectOffset;
    std::memcpy(object, kObjectPrefixWords.data(), sizeof(kObjectPrefixWords));
    store_pointer(object, kPairTwoSecond, object + 0x58);
    store_pointer(object, kPairTwoFirst, object + 0x58);
    store_pointer(object, kPairTwoFirst + sizeof(std::uint64_t), object + 0x58);
    store_pointer(object, 0x40, object + kPairOneFirst);

    const auto* const object_list = object + 0x180;
    const auto* const object_end = object + kSlot545ObjectSize;
    store_pointer(object, kPairOneSecond, object_list);
    store_pointer(object, kPairOneFirst, object_list);
    store_pointer(object, 0x168, object_end);
    std::fill(object + 0x180, object + kSlot545ObjectSize, std::byte{0});
    store_pointer(object, 0x160, object_end);

    Slot545Result result;
    if (log == nullptr) {
        result.error = Slot545Error::missing_atexit_log;
        return result;
    }
    result.atexit_recorded = log->record(kSlot545DtorRva, object);
    if (!result.atexit_recorded) {
        result.error = Slot545Error::atexit_record_failed;
        return result;
    }
    if (entry_guard != exit_guard) {
        result.error = Slot545Error::stack_guard_mismatch;
        return result;
    }
    result.ok = true;
    return result;
}

Slot545DtorResult destroy_slot545_object(
    Slot545ObjectView object, const AllocDispatchContract& allocator) noexcept {
    Slot545DtorResult result;
    if (object.bytes == nullptr) {
        result.ok = false;
        result.failure = Slot545DtorFailure::missing_object;
        return result;
    }

    const auto release = [&](std::size_t first, std::size_t peer, Slot545DtorFailure failure) {
        void* const pointer = load_pointer(object.bytes, first);
        if (pointer == nullptr || load_pointer(object.bytes, peer) == pointer) return true;
        ++result.release_attempts;
        if (!free_170c0(pointer, allocator).ok) {
            result.ok = false;
            result.failure = failure;
            return false;
        }
        return true;
    };

    // Preserve the callback order: pair +0x158/+0x178, then pair +0x30/+0x50.
    if (!release(kPairOneFirst, kPairOneSecond, Slot545DtorFailure::first_release)) return result;
    if (!release(kPairTwoFirst, kPairTwoSecond, Slot545DtorFailure::second_release)) return result;
    return result;
}

}  // namespace sotc
