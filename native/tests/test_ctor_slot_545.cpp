#include "core/ctor_slot_545_b2ec0.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace {

using sotc::Slot545Matrix;

constexpr std::byte kPoison{0xA5};
int failures = 0;
std::array<void*, 4> released{};
std::array<std::uint32_t, 4> release_tags{};
std::size_t release_count = 0;
void* fail_pointer = nullptr;

void check(bool condition, const char* message) {
    if (condition) return;
    std::fprintf(stderr, "FAIL: %s\n", message);
    ++failures;
}

void poison(sotc::Slot545State& state) {
    state.bytes.fill(kPoison);
}

std::uint32_t bits(float value) {
    std::uint32_t out = 0;
    std::memcpy(&out, &value, sizeof(out));
    return out;
}

std::uint32_t word_at(const sotc::Slot545State& state, std::size_t offset) {
    std::uint32_t out = 0;
    std::memcpy(&out, state.bytes.data() + offset, sizeof(out));
    return out;
}

std::uintptr_t pointer_word(const sotc::Slot545State& state, std::size_t offset) {
    std::uintptr_t out = 0;
    std::memcpy(&out, state.bytes.data() + offset, sizeof(out));
    return out;
}

void store_pointer(sotc::Slot545State& state, std::size_t offset, const void* pointer) {
    const auto value = reinterpret_cast<std::uintptr_t>(pointer);
    std::memcpy(state.bytes.data() + offset, &value, sizeof(value));
}

bool deallocate(void* pointer, std::uint32_t tag) {
    if (release_count < released.size()) {
        released[release_count] = pointer;
        release_tags[release_count] = tag;
    }
    ++release_count;
    return pointer != fail_pointer;
}

void reset_releases() {
    released = {};
    release_tags = {};
    release_count = 0;
    fail_pointer = nullptr;
}

bool same_matrix(const Slot545Matrix& actual, const std::array<std::uint32_t, 16>& expected) {
    for (std::size_t i = 0; i != actual.size(); ++i) {
        if (bits(actual[i]) != expected[i]) return false;
    }
    return true;
}

Slot545Matrix read_matrix(const sotc::Slot545State& state, std::size_t offset) {
    Slot545Matrix matrix{};
    std::memcpy(matrix.data(), state.bytes.data() + offset, sizeof(matrix));
    return matrix;
}

bool all_bytes_are(const std::byte* bytes, std::size_t count, std::byte value) {
    for (std::size_t i = 0; i != count; ++i) {
        if (bytes[i] != value) return false;
    }
    return true;
}

void test_matrix_helper() {
    Slot545Matrix a{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    Slot545Matrix b{2, -1, 3, 1, 4, 2, -2, 5, 1, 3, 2, -4, 6, 1, 0, 2};
    constexpr std::array<std::uint32_t, 16> expected{
        0x42140000, 0x42280000, 0x423C0000, 0x42500000,
        0x42740000, 0x428C0000, 0x429E0000, 0x42B00000,
        0xC1900000, 0xC1800000, 0xC1600000, 0xC1400000,
        0x42140000, 0x42380000, 0x425C0000, 0x42800000,
    };
    Slot545Matrix out{};
    sotc::slot545_matrix_multiply(out, a, b);
    check(same_matrix(out, expected), "matrix helper computes the pinned element-wise product");

    Slot545Matrix alias_left = a;
    sotc::slot545_matrix_multiply(alias_left, alias_left, b);
    check(same_matrix(alias_left, expected), "matrix helper supports output aliasing the left input");

    Slot545Matrix alias_right = b;
    sotc::slot545_matrix_multiply(alias_right, a, alias_right);
    check(same_matrix(alias_right, expected), "matrix helper supports output aliasing the right input");
}

void test_constructor_and_default_cleanup() {
    sotc::Slot545State state;
    poison(state);
    sotc::StartupAtexitLog log;
    const sotc::Slot545Result result = sotc::init_slot545_b2ec0(state, &log, 0x12345678, 0x12345678);
    check(result.ok && result.atexit_recorded && result.error == sotc::Slot545Error::none,
          "constructor succeeds and records the atexit identity");
    check(log.size() == 1, "constructor records exactly one atexit entry");
    const auto object = sotc::slot545_object_view(state);
    if (log.size() == 1) {
        check(log.at(0).dtor_rva == 0xB0A10, "atexit entry pins the callback RVA");
        check(log.at(0).object == object.bytes, "atexit entry uses the host object address at offset 0x120");
    }

    constexpr std::array<std::uint32_t, 16> first{
        0x3F000000, 0, 0, 0, 0, 0xBF000000, 0, 0,
        0, 0, 0x3F800000, 0, 0x3F000000, 0x3F000000, 0, 0x3F800000,
    };
    constexpr std::array<std::uint32_t, 16> second{
        0x3F800000, 0, 0, 0, 0, 0xBF800000, 0, 0,
        0, 0, 0x3F800000, 0, 0x3F000000, 0x3F000000, 0, 0x3F800000,
    };
    constexpr std::array<std::uint32_t, 16> third{
        0x3F800000, 0, 0, 0, 0, 0xBF800000, 0, 0,
        0, 0, 0x3F800000, 0, 0x3F000000, 0x3F000000, 0x3F000000, 0x3F800000,
    };
    check(same_matrix(read_matrix(state, 0x00), first), "first output matrix matches pinned transform operands");
    check(same_matrix(read_matrix(state, 0x40), second), "second output matrix matches pinned transform operands");
    check(same_matrix(read_matrix(state, 0x80), third), "third output matrix matches pinned transform operands");

    constexpr std::array<std::uint32_t, 8> basis{
        0x3F800000, 0, 0, 0, 0x3F800000, 0, 0, 0,
    };
    std::array<std::uint32_t, 8> actual_basis{};
    std::memcpy(actual_basis.data(), state.bytes.data() + sotc::kSlot545BasisOffset, sizeof(actual_basis));
    check(actual_basis == basis, "32-byte constant copy starts at static-image offset 0xc8");
    check(word_at(state, sotc::kSlot545ScalarOffset) == 0x3F800000,
          "scalar store at static-image offset 0xe8 overwrites the copied word with 1.0f");
    check(all_bytes_are(state.bytes.data() + 0xC0, 8, kPoison), "unwritten global gap +0xc0..+0xc7 stays untouched");
    check(all_bytes_are(state.bytes.data() + 0xEC, 0x34, kPoison), "unwritten global gap +0xec..+0x11f stays untouched");

    constexpr std::array<std::uint32_t, 16> object_prefix{
        0x3F800000, 0, 0, 0, 0x3F800000, 0, 0, 0,
        0x3F800000, 0, 0, 0, 0xFF1B6468, 0xFF1B6416, 0xFF1B640C, 0xFF1B6402,
    };
    std::array<std::uint32_t, 16> actual_prefix{};
    std::memcpy(actual_prefix.data(), object.bytes, sizeof(actual_prefix));
    bool prefix_matches = true;
    for (std::size_t i = 0; i != 12; ++i) prefix_matches = prefix_matches && actual_prefix[i] == object_prefix[i];
    check(prefix_matches, "unoverwritten object prefix bytes match the copied table");

    const auto base = reinterpret_cast<std::uintptr_t>(object.bytes);
    check(pointer_word(state, sotc::kSlot545ObjectOffset + 0x30) == base + 0x58 &&
              pointer_word(state, sotc::kSlot545ObjectOffset + 0x38) == base + 0x58 &&
              pointer_word(state, sotc::kSlot545ObjectOffset + 0x50) == base + 0x58,
          "second cleanup pair uses the equal +0x58 sentinel pointers");
    check(pointer_word(state, sotc::kSlot545ObjectOffset + 0x40) == base + 0x158 &&
              pointer_word(state, sotc::kSlot545ObjectOffset + 0x158) == base + 0x180 &&
              pointer_word(state, sotc::kSlot545ObjectOffset + 0x178) == base + 0x180,
          "first cleanup pair and list links use object-relative host pointers");
    check(pointer_word(state, sotc::kSlot545ObjectOffset + 0x160) == base + 0x1D0 &&
              pointer_word(state, sotc::kSlot545ObjectOffset + 0x168) == base + 0x1D0,
          "empty-list tail pointers target the host storage at object+0x1d0");
    check(all_bytes_are(object.bytes + 0x180, 0x50, std::byte{0}), "callback list tail bytes are zeroed");
    check(all_bytes_are(object.bytes + 0x100, 0x10, kPoison), "untouched object bytes remain opaque");
    check(base + 0x1D0 >= reinterpret_cast<std::uintptr_t>(state.bytes.data()) &&
              base + 0x1D0 < reinterpret_cast<std::uintptr_t>(state.bytes.data() + state.bytes.size()),
          "one-past object link points into the host-owned state extent");

    reset_releases();
    const auto cleanup = sotc::destroy_slot545_object(object, sotc::AllocDispatchContract{nullptr, deallocate});
    check(cleanup.ok && cleanup.release_attempts == 0 && release_count == 0,
          "the constructor's equal sentinels make the registered cleanup a no-op");
}

void test_cleanup_contract() {
    sotc::Slot545State state;
    poison(state);
    auto object = sotc::slot545_object_view(state);
    std::array<std::uint64_t, 4> tokens{};
    void* const first = &tokens[0];
    void* const peer_first = &tokens[1];
    void* const second = &tokens[2];
    void* const peer_second = &tokens[3];
    store_pointer(state, sotc::kSlot545ObjectOffset + 0x158, first);
    store_pointer(state, sotc::kSlot545ObjectOffset + 0x178, peer_first);
    store_pointer(state, sotc::kSlot545ObjectOffset + 0x30, second);
    store_pointer(state, sotc::kSlot545ObjectOffset + 0x50, peer_second);

    reset_releases();
    const auto result = sotc::destroy_slot545_object(object, sotc::AllocDispatchContract{nullptr, deallocate});
    check(result.ok && result.release_attempts == 2 && release_count == 2,
          "distinct non-null pointer pairs dispatch two releases");
    check(released[0] == first && released[1] == second && release_tags[0] == 2 && release_tags[1] == 2,
          "cleanup preserves pair order and the tag-2 release contract");
    check(pointer_word(state, sotc::kSlot545ObjectOffset + 0x158) == reinterpret_cast<std::uintptr_t>(first) &&
              pointer_word(state, sotc::kSlot545ObjectOffset + 0x30) == reinterpret_cast<std::uintptr_t>(second),
          "cleanup leaves object fields unchanged");

    store_pointer(state, sotc::kSlot545ObjectOffset + 0x178, first);
    reset_releases();
    const auto one_pair = sotc::destroy_slot545_object(object, sotc::AllocDispatchContract{nullptr, deallocate});
    check(one_pair.ok && one_pair.release_attempts == 1 && release_count == 1 && released[0] == second,
          "equal first pair is skipped while the second pair is released");

    store_pointer(state, sotc::kSlot545ObjectOffset + 0x158, nullptr);
    store_pointer(state, sotc::kSlot545ObjectOffset + 0x178, peer_first);
    store_pointer(state, sotc::kSlot545ObjectOffset + 0x30, second);
    store_pointer(state, sotc::kSlot545ObjectOffset + 0x50, second);
    reset_releases();
    const auto skipped = sotc::destroy_slot545_object(object, sotc::AllocDispatchContract{});
    check(skipped.ok && skipped.release_attempts == 0 && release_count == 0,
          "null and equal pairs skip the allocator hook");

    store_pointer(state, sotc::kSlot545ObjectOffset + 0x158, first);
    store_pointer(state, sotc::kSlot545ObjectOffset + 0x178, peer_first);
    store_pointer(state, sotc::kSlot545ObjectOffset + 0x30, second);
    store_pointer(state, sotc::kSlot545ObjectOffset + 0x50, peer_second);
    reset_releases();
    const auto no_hook = sotc::destroy_slot545_object(object, sotc::AllocDispatchContract{});
    check(!no_hook.ok && no_hook.release_attempts == 1 && release_count == 0 &&
              no_hook.failure == sotc::Slot545DtorFailure::first_release,
          "missing allocator hook fails at the first required release");

    reset_releases();
    fail_pointer = first;
    const auto first_failure = sotc::destroy_slot545_object(object, sotc::AllocDispatchContract{nullptr, deallocate});
    check(!first_failure.ok && first_failure.release_attempts == 1 && release_count == 1 &&
              first_failure.failure == sotc::Slot545DtorFailure::first_release,
          "failed first allocator result stops before the second release");

    reset_releases();
    fail_pointer = second;
    const auto second_failure = sotc::destroy_slot545_object(object, sotc::AllocDispatchContract{nullptr, deallocate});
    check(!second_failure.ok && second_failure.release_attempts == 2 && release_count == 2 &&
              released[0] == first && released[1] == second &&
              second_failure.failure == sotc::Slot545DtorFailure::second_release,
          "second-release failure is reported after the first release completes");
}

void test_fail_visible_lifecycle() {
    sotc::Slot545State missing_log_state;
    poison(missing_log_state);
    const auto missing_log = sotc::init_slot545_b2ec0(missing_log_state, nullptr, 1, 1);
    check(!missing_log.ok && missing_log.error == sotc::Slot545Error::missing_atexit_log,
          "missing atexit log fails visibly after data writes");
    check(word_at(missing_log_state, sotc::kSlot545ScalarOffset) == 0x3F800000,
          "missing-log failure retains the earlier observed static stores");

    sotc::Slot545State guard_state;
    poison(guard_state);
    sotc::StartupAtexitLog guard_log;
    const auto guard = sotc::init_slot545_b2ec0(guard_state, &guard_log, 1, 2);
    check(!guard.ok && guard.atexit_recorded && guard.error == sotc::Slot545Error::stack_guard_mismatch,
          "guard mismatch fails after preserving the earlier atexit registration effect");
    check(guard_log.size() == 1 &&
              guard_log.at(0).object == guard_state.bytes.data() + sotc::kSlot545ObjectOffset,
          "guard-failure fixture retains registration with host object identity");
}

}  // namespace

int main() {
    test_matrix_helper();
    test_constructor_and_default_cleanup();
    test_cleanup_contract();
    test_fail_visible_lifecycle();
    if (failures != 0) {
        std::fprintf(stderr, "%d slot-545 checks failed\n", failures);
        return 1;
    }
    std::puts("slot-545 native fixture PASS");
    return 0;
}
