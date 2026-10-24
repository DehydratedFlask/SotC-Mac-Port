// Fixture for the row-14 owner constructor `0xa597d0` (0x958 B object, pointer slot 0x32f8b80).
//
// The characterization is `local/re/evidence/owner-row14-a597d0/report.md`; this fixture is the
// falsifiable side of it. It asserts every written byte against a byte-exact expected image, the
// allocation sizes and order, the deque header, the 7-argument `0x3b5c0` frame, every hook
// argument, every failure prefix, a double call, the guard ring, and that the object is
// *partially* initialized (holes keep the fill byte).
//
// Byte-exact ELF pins live in `tools/lift/check_fowner_ctor_a597d0.py`.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <type_traits>
#include <vector>

#include "core/owner_ctor_a597d0.hpp"
#include "core/owner_members.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

static_assert(std::is_same_v<decltype(OwnerMemberHooks::ctor),
                             bool (*)(std::size_t, void*, void*)>,
              "the adapter must fit owner_members' context-free ctor hook");

namespace {

constexpr std::size_t kObj = kOwnerCtorA597D0ObjSize;
constexpr std::size_t kGuard = 0x40;
constexpr std::uint8_t kFill = 0xCC;
constexpr std::uint8_t kGuardFill = 0xA5;
constexpr std::uint8_t kPoolFill = 0xDD;

constexpr std::uint64_t kEventHandle = 0x1111222233334444ull;
constexpr std::uint8_t kEventFlag = 0x01;
constexpr std::uint64_t kThreadVptr = 0x5151515151515151ull;
constexpr std::uint64_t kThreadEvent = 0x2222333344445555ull;

alignas(8) std::uint8_t g_map[kOwnerA597D0MapBytes];
alignas(8) std::uint8_t g_chunk[kOwnerA597D0ChunkBytes];

struct Guarded {
    alignas(16) std::uint8_t storage[kGuard + kObj + kGuard];

    Guarded() {
        std::memset(storage, kGuardFill, sizeof storage);
        std::memset(object(), kFill, kObj);
    }
    std::uint8_t* object() noexcept { return storage + kGuard; }
    const std::uint8_t* object() const noexcept { return storage + kGuard; }
    bool guards_ok() const noexcept {
        for (std::size_t i = 0; i < kGuard; ++i) {
            if (storage[i] != kGuardFill || storage[kGuard + kObj + i] != kGuardFill) {
                return false;
            }
        }
        return true;
    }
};

void reset_pools() {
    std::memset(g_map, kPoolFill, sizeof g_map);
    std::memset(g_chunk, kPoolFill, sizeof g_chunk);
}

enum class Event { kAlloc, kEventFlag, kThread, kMode };

struct Recorder {
    std::vector<std::size_t> allocations;
    std::vector<Event> order;

    int fail_allocate = -1;  // 0-based allocation index
    bool fail_event = false;
    bool fail_thread = false;
    bool fail_mode = false;

    std::size_t alloc_calls() const noexcept { return allocations.size(); }

    int event_calls = 0;
    void* event_at = nullptr;
    std::uint32_t event_byte = 0xFFFFFFFFu;
    std::uint32_t event_pattern = 0xFFFFFFFFu;

    int thread_calls = 0;
    OwnerA597D0ThreadRequest request{};

    int mode_calls = 0;
    std::uint8_t* mode_ptr = nullptr;
    std::uint8_t mode_value = 0;
};

Recorder* g_rec = nullptr;

void* hook_allocate(std::size_t size) noexcept {
    if (g_rec == nullptr) {
        return nullptr;
    }
    const std::size_t index = g_rec->allocations.size();
    g_rec->order.push_back(Event::kAlloc);
    g_rec->allocations.push_back(size);
    if (g_rec->fail_allocate >= 0 && index == static_cast<std::size_t>(g_rec->fail_allocate)) {
        return nullptr;
    }
    if (index == 0) {
        return size == kOwnerA597D0MapBytes ? g_map : nullptr;
    }
    if (index == 1) {
        return size == kOwnerA597D0ChunkBytes ? g_chunk : nullptr;
    }
    // Later constructor calls reuse the same two test pools in the same order.
    if (index % 2 == 0) {
        return size == kOwnerA597D0MapBytes ? g_map : nullptr;
    }
    return size == kOwnerA597D0ChunkBytes ? g_chunk : nullptr;
}

bool hook_event(void* at, std::uint32_t flag_byte, std::uint32_t initial_pattern) noexcept {
    if (g_rec == nullptr) {
        return false;
    }
    g_rec->order.push_back(Event::kEventFlag);
    ++g_rec->event_calls;
    g_rec->event_at = at;
    g_rec->event_byte = flag_byte;
    g_rec->event_pattern = initial_pattern;
    if (g_rec->fail_event) {
        return false;
    }
    // Fixture stand-in for 0x3b3d0: qword handle at +0, u8 flag at +8, +9..+0xf untouched.
    auto* const p = static_cast<std::uint8_t*>(at);
    std::memcpy(p, &kEventHandle, sizeof kEventHandle);
    p[8] = kEventFlag;
    return true;
}

bool hook_thread(const OwnerA597D0ThreadRequest& request) noexcept {
    if (g_rec == nullptr) {
        return false;
    }
    g_rec->order.push_back(Event::kThread);
    ++g_rec->thread_calls;
    g_rec->request = request;
    if (g_rec->fail_thread) {
        return false;
    }
    // Fixture stand-in for 0x3b5c0: vptr at +0, its own event flag at +0x18.
    auto* const p = static_cast<std::uint8_t*>(request.thread_object);
    std::memcpy(p, &kThreadVptr, sizeof kThreadVptr);
    std::memcpy(p + 0x18, &kThreadEvent, sizeof kThreadEvent);
    p[0x20] = 1;
    return true;
}

bool hook_mode(std::uint8_t* value) noexcept {
    if (g_rec == nullptr) {
        return false;
    }
    g_rec->order.push_back(Event::kMode);
    ++g_rec->mode_calls;
    g_rec->mode_ptr = value;
    g_rec->mode_value = *value;
    return !g_rec->fail_mode;
}

OwnerA597D0Hooks make_hooks() {
    OwnerA597D0Hooks hooks;
    hooks.allocate = &hook_allocate;
    hooks.event_flag_create = &hook_event;
    hooks.thread_create = &hook_thread;
    hooks.startup_mode_notify = &hook_mode;
    return hooks;
}

// Prefix of the original store sequence, used to check every failure path byte-for-byte.
enum class Prefix {
    kHead,        // before the first allocation
    kMapPointer,  // map pointer stored, chunk missing
    kIterators,   // chunk + deque header + 0x888 + 0x8b0
    kEvent,       // event flag object written
    kThread,      // 0x938/0x948 + clear loop + 0x29/+0x2b
    kFull,
};

struct Image {
    std::uint8_t bytes[kObj];

    Image() { std::memset(bytes, kFill, sizeof bytes); }

    void put8(std::size_t offset, std::uint8_t value) noexcept { bytes[offset] = value; }
    void put32(std::size_t offset, std::uint32_t value) noexcept {
        std::memcpy(bytes + offset, &value, sizeof value);
    }
    void put64(std::size_t offset, std::uint64_t value) noexcept {
        std::memcpy(bytes + offset, &value, sizeof value);
    }
    void put_ptr(std::size_t offset, const void* value) noexcept {
        std::memcpy(bytes + offset, &value, sizeof value);
    }
    void zero(std::size_t offset, std::size_t size) noexcept {
        std::memset(bytes + offset, 0, size);
    }
};

Image expected_prefix(Prefix prefix) {
    Image img;
    // head
    img.put32(kOwnerA597D0RefCountOffset, 1u);
    img.zero(kOwnerA597D0VectorOffset, 16);
    img.put64(kOwnerA597D0VectorOffset + 0x10, 0u);
    img.put32(kOwnerA597D0FlagsOffset, kOwnerA597D0FlagsValue);
    img.zero(kOwnerA597D0DequeOffset + kOwnerA597D0DequeEndOffset, 32);
    img.zero(kOwnerA597D0DequeOffset + kOwnerA597D0DequeBeginOffset, 32);
    img.put64(kOwnerA597D0DequeOffset + kOwnerA597D0DequeMapSizeOffset, kOwnerA597D0MapSlots);
    if (prefix == Prefix::kHead) {
        return img;
    }

    img.put_ptr(kOwnerA597D0DequeOffset + kOwnerA597D0DequeMapOffset, g_map);
    if (prefix == Prefix::kMapPointer) {
        return img;
    }

    void* const map_slot = g_map + 0x18;
    img.put_ptr(kOwnerA597D0DequeOffset + kOwnerA597D0DequeBeginOffset + 0x18, map_slot);
    img.put_ptr(kOwnerA597D0DequeOffset + kOwnerA597D0DequeBeginOffset + 0x08, g_chunk);
    img.put_ptr(kOwnerA597D0DequeOffset + kOwnerA597D0DequeBeginOffset + 0x10,
                g_chunk + kOwnerA597D0ChunkBytes);
    img.put_ptr(kOwnerA597D0DequeOffset + kOwnerA597D0DequeBeginOffset + 0x00, g_chunk);
    img.put_ptr(kOwnerA597D0DequeOffset + kOwnerA597D0DequeEndOffset + 0x18, map_slot);
    img.put_ptr(kOwnerA597D0DequeOffset + kOwnerA597D0DequeEndOffset + 0x08, g_chunk);
    img.put_ptr(kOwnerA597D0DequeOffset + kOwnerA597D0DequeEndOffset + 0x10,
                g_chunk + kOwnerA597D0ChunkBytes);
    img.put_ptr(kOwnerA597D0DequeOffset + kOwnerA597D0DequeEndOffset + 0x00, g_chunk);
    img.put64(kOwnerA597D0Qword888Offset, 0u);
    img.put64(kOwnerA597D0Qword8B0Offset, 0u);
    if (prefix == Prefix::kIterators) {
        return img;
    }

    img.put64(kOwnerA597D0EventOffset, kEventHandle);
    img.put8(kOwnerA597D0EventOffset + 0x08, kEventFlag);
    if (prefix == Prefix::kEvent) {
        return img;
    }

    img.put64(kOwnerA597D0ThreadOffset, kThreadVptr);
    img.put64(kOwnerA597D0ThreadOffset + 0x18, kThreadEvent);
    img.put8(kOwnerA597D0ThreadOffset + 0x20, 1);
    img.zero(kOwnerA597D0OwnedPtr938Offset, 16);
    img.put64(kOwnerA597D0Qword948Offset, 0u);
    for (std::size_t index = 0; index < kOwnerA597D0RecordCount; ++index) {
        const std::size_t record =
            kOwnerA597D0RecordBase + index * kOwnerA597D0RecordStride;
        img.put8(record + 0x00, 0u);
        img.zero(record + kOwnerA597D0RecordBytesOffset, 16);
        img.put32(record + kOwnerA597D0RecordDwordOffset, 0u);
    }
    img.put8(kOwnerA597D0Byte29Offset, 0u);
    img.put8(kOwnerA597D0ModeByteOffset, 1u);
    if (prefix == Prefix::kThread) {
        return img;
    }

    img.put8(kOwnerA597D0Byte890Offset, 0u);
    img.zero(kOwnerA597D0Block898Offset, 16);
    img.put32(kOwnerA597D0Dword8A8Offset, 0u);
    return img;
}

bool image_matches(const Guarded& guarded, Prefix prefix) {
    const Image expected = expected_prefix(prefix);
    return std::memcmp(guarded.object(), expected.bytes, kObj) == 0;
}

bool map_matches(Prefix prefix) {
    std::uint8_t expected[kOwnerA597D0MapBytes];
    std::memset(expected, kPoolFill, sizeof expected);
    if (prefix != Prefix::kHead && prefix != Prefix::kMapPointer) {
        void* const slot = g_chunk;
        std::memcpy(expected + 0x18, &slot, sizeof slot);
    }
    return std::memcmp(g_map, expected, sizeof expected) == 0;
}

bool chunk_untouched() {
    for (const std::uint8_t byte : g_chunk) {
        if (byte != kPoolFill) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------------------------
// Sections
// ---------------------------------------------------------------------------------------------

void test_constants() {
    CHECK(kOwnerCtorA597D0Rva == 0xA597D0);
    CHECK(kOwnerCtorA597D0Size == 0x202);
    CHECK(kOwnerCtorA597D0ObjSize == 0x958);
    CHECK(kOwnerCtorA597D0SlotRva == 0x32F8B80);
    CHECK(kOwnerDtorA59A10Rva == 0xA59A10);
    CHECK(kOwnerDtorA59A10Size == 0x1A7);
    CHECK(kOwnerA597D0MapBytes == 0x40);
    CHECK(kOwnerA597D0MapSlots == 8);
    CHECK(kOwnerA597D0ChunkBytes == 0x100);
    CHECK(kOwnerA597D0RecordBase == 0x30);
    CHECK(kOwnerA597D0RecordCount == 64);
    CHECK(kOwnerA597D0RecordStride == 0x20);
    CHECK(kOwnerA597D0RecordLimit == kOwnerA597D0RecordBase +
                                          kOwnerA597D0RecordCount * kOwnerA597D0RecordStride);
    CHECK(kOwnerA597D0RecordLimit == kOwnerA597D0DequeOffset);
    CHECK(kOwnerA597D0DequeBytes == 0x50);
    CHECK(kOwnerA597D0ThreadEntryRva == 0xB09590);
    CHECK(kOwnerA597D0ThreadNameRva == 0x12C4892);
    CHECK(kOwnerA597D0ThreadArg3 == 0x2BC);
    CHECK(kOwnerA597D0ThreadArg4 == 0x60);
    CHECK(kOwnerA597D0ThreadArg5 == 1);
    CHECK(kOwnerA597D0ThreadArg6 == 0x100000);
    CHECK(std::strcmp(kOwnerA597D0ThreadName, "Trophy Task Thread") == 0);
    CHECK(kOwnerA597D0ModeConfigRva == 0x32F8B90);
    CHECK(kOwnerA597D0ModeCellRva == 0x32F8BF0);
    CHECK(kOwnerA597D0FlagsValue == 0x10000);
    CHECK(kOwnerA597D0EventFlagByte == 0);
    CHECK(kOwnerA597D0EventPattern == 0);
}

void test_success() {
    Guarded guarded;
    reset_pools();
    Recorder rec;
    g_rec = &rec;

    const OwnerA597D0Result result = owner_ctor_a597d0(guarded.object(), make_hooks());
    g_rec = nullptr;

    CHECK(result.ok);
    CHECK(result.failed_at == nullptr);
    CHECK(rec.allocations.size() == 2);
    CHECK(rec.allocations[0] == 0x40);
    CHECK(rec.allocations[1] == 0x100);
    CHECK((rec.order == std::vector<Event>{Event::kAlloc, Event::kAlloc, Event::kEventFlag,
                                           Event::kThread, Event::kMode}));
    CHECK(image_matches(guarded, Prefix::kFull));
    CHECK(map_matches(Prefix::kFull));
    CHECK(guarded.guards_ok());

    // Holes the original never writes (report §1, §4).
    for (std::size_t offset = 0x20; offset < 0x28; ++offset) {
        CHECK(guarded.object()[offset] == kFill);
    }
    for (std::size_t offset = 0x2C; offset < 0x30; ++offset) {
        CHECK(guarded.object()[offset] == kFill);
    }
    CHECK(guarded.object()[0x2A] == 1);  // side effect of the dword 0x10000 at +0x28
    CHECK(guarded.object()[0x2B] == 1);
    for (std::size_t index = 0; index < kOwnerA597D0RecordCount; ++index) {
        const std::size_t record = kOwnerA597D0RecordBase + index * kOwnerA597D0RecordStride;
        CHECK(guarded.object()[record + 0x00] == 0);
        for (std::size_t hole = 0x01; hole < 0x08; ++hole) {
            CHECK(guarded.object()[record + hole] == kFill);
        }
        CHECK(guarded.object()[record + 0x18] == 0);
        for (std::size_t hole = 0x1C; hole < 0x20; ++hole) {
            CHECK(guarded.object()[record + hole] == kFill);
        }
    }
    for (std::size_t offset = 0x82C; offset < 0x830; ++offset) {
        CHECK(guarded.object()[offset] == kFill);
    }
    for (std::size_t offset = 0x880; offset < 0x888; ++offset) {
        CHECK(guarded.object()[offset] == kFill);
    }
    for (std::size_t offset = 0x891; offset < 0x898; ++offset) {
        CHECK(guarded.object()[offset] == kFill);
    }
    for (std::size_t offset = 0x8AC; offset < 0x8B0; ++offset) {
        CHECK(guarded.object()[offset] == kFill);
    }
    for (std::size_t offset = 0x8C1; offset < 0x8C8; ++offset) {
        CHECK(guarded.object()[offset] == kFill);
    }
    for (std::size_t offset = 0x8F0; offset < 0x938; ++offset) {
        CHECK(guarded.object()[offset] == kFill);
    }
    for (std::size_t offset = 0x950; offset < kObj; ++offset) {
        CHECK(guarded.object()[offset] == kFill);
    }
}

void test_hook_arguments() {
    Guarded guarded;
    reset_pools();
    Recorder rec;
    g_rec = &rec;

    const OwnerA597D0Result result = owner_ctor_a597d0(guarded.object(), make_hooks());
    g_rec = nullptr;

    CHECK(result.ok);
    CHECK(rec.event_calls == 1);
    CHECK(rec.event_at == guarded.object() + kOwnerA597D0EventOffset);
    CHECK(rec.event_byte == 0);
    CHECK(rec.event_pattern == 0);

    CHECK(rec.thread_calls == 1);
    const OwnerA597D0ThreadRequest& request = rec.request;
    CHECK(request.thread_object == guarded.object() + kOwnerA597D0ThreadOffset);
    CHECK(request.callable[0] == reinterpret_cast<std::uint64_t>(guarded.object()));
    CHECK(request.callable[1] == kOwnerA597D0ThreadEntryRva);
    CHECK(request.callable[2] == 0);
    CHECK(request.callable[3] == 0);
    // String literals have per-TU addresses, so compare content, not the folded constant pointer.
    CHECK(request.name != nullptr);
    CHECK(std::strcmp(request.name, "Trophy Task Thread") == 0);
    CHECK(std::strcmp(request.name, kOwnerA597D0ThreadName) == 0);
    CHECK(request.arg3 == 0x2BC);
    CHECK(request.arg4 == 0x60);
    CHECK(request.arg5 == 1);
    CHECK(request.stack_arg == 0x100000);

    CHECK(rec.mode_calls == 1);
    CHECK(rec.mode_ptr == guarded.object() + kOwnerA597D0ModeByteOffset);
    CHECK(rec.mode_value == 1);
}

struct FailureCase {
    const char* failed_at;
    Prefix prefix;
    int fail_allocate;
    bool fail_event;
    bool fail_thread;
    bool fail_mode;
};

void test_failure_prefixes() {
    const FailureCase cases[] = {
        {"alloc_block", Prefix::kHead, 0, false, false, false},
        {"alloc_chunk", Prefix::kMapPointer, 1, false, false, false},
        {"event_flag", Prefix::kIterators, -1, true, false, false},
        {"thread_create", Prefix::kEvent, -1, false, true, false},
        {"startup_mode", Prefix::kThread, -1, false, false, true},
    };
    for (const FailureCase& item : cases) {
        Guarded guarded;
        reset_pools();
        Recorder rec;
        rec.fail_allocate = item.fail_allocate;
        rec.fail_event = item.fail_event;
        rec.fail_thread = item.fail_thread;
        rec.fail_mode = item.fail_mode;
        g_rec = &rec;

        const OwnerA597D0Result result = owner_ctor_a597d0(guarded.object(), make_hooks());
        g_rec = nullptr;

        CHECK(!result.ok);
        CHECK(result.failed_at != nullptr);
        CHECK(std::string(result.failed_at) == item.failed_at);
        CHECK(image_matches(guarded, item.prefix));
        CHECK(map_matches(item.prefix));
        CHECK(guarded.guards_ok());
        if (item.prefix == Prefix::kHead || item.prefix == Prefix::kMapPointer) {
            CHECK(chunk_untouched());
        }
    }
}

void test_missing_hooks() {
    struct Missing {
        const char* failed_at;
        Prefix prefix;
        bool with_alloc;
        bool with_event;
        bool with_thread;
    };
    const Missing cases[] = {
        {"alloc_block", Prefix::kHead, false, false, false},
        {"event_flag", Prefix::kIterators, true, false, false},
        {"thread_create", Prefix::kEvent, true, true, false},
        {"startup_mode", Prefix::kThread, true, true, true},
    };
    for (const Missing& item : cases) {
        Guarded guarded;
        reset_pools();
        Recorder rec;
        OwnerA597D0Hooks hooks;
        if (item.with_alloc) {
            hooks.allocate = &hook_allocate;
        }
        if (item.with_event) {
            hooks.event_flag_create = &hook_event;
        }
        if (item.with_thread) {
            hooks.thread_create = &hook_thread;
        }
        // startup_mode_notify stays null in every case; that is the missing hook being tested.
        g_rec = &rec;
        const OwnerA597D0Result result = owner_ctor_a597d0(guarded.object(), hooks);
        g_rec = nullptr;
        CHECK(!result.ok);
        CHECK(result.failed_at != nullptr);
        CHECK(std::string(result.failed_at) == item.failed_at);
        CHECK(image_matches(guarded, item.prefix));
        CHECK(guarded.guards_ok());
    }
}

void test_preconditions() {
    Recorder rec;
    g_rec = &rec;
    const OwnerA597D0Result null_object = owner_ctor_a597d0(nullptr, make_hooks());
    CHECK(!null_object.ok);
    CHECK(null_object.failed_at != nullptr);
    CHECK(std::string(null_object.failed_at) == "null-object");
    CHECK(rec.alloc_calls() == 0);

    Guarded guarded;
    const OwnerA597D0Result misaligned =
        owner_ctor_a597d0(guarded.object() + 1, make_hooks());
    CHECK(!misaligned.ok);
    CHECK(misaligned.failed_at != nullptr);
    CHECK(std::string(misaligned.failed_at) == "object-align");
    CHECK(rec.allocations.empty());
    CHECK(rec.event_calls == 0 && rec.thread_calls == 0 && rec.mode_calls == 0);
    CHECK(guarded.object()[0] == kFill);
    CHECK(guarded.guards_ok());
    g_rec = nullptr;
}

void test_second_call() {
    Guarded guarded;
    reset_pools();
    Recorder first;
    g_rec = &first;
    CHECK(owner_ctor_a597d0(guarded.object(), make_hooks()).ok);
    g_rec = nullptr;
    const Image after_first = expected_prefix(Prefix::kFull);
    CHECK(std::memcmp(guarded.object(), after_first.bytes, kObj) == 0);

    reset_pools();
    Recorder second;
    g_rec = &second;
    CHECK(owner_ctor_a597d0(guarded.object(), make_hooks()).ok);
    g_rec = nullptr;
    CHECK(std::memcmp(guarded.object(), after_first.bytes, kObj) == 0);
    CHECK(image_matches(guarded, Prefix::kFull));
    CHECK(first.allocations == second.allocations);
    CHECK(first.order == second.order);
    CHECK(guarded.guards_ok());
}

void test_adapter() {
    Recorder rec;
    g_rec = &rec;
    OwnerRow14Binding binding;
    binding.hooks = make_hooks();
    owner_row14_bind(&binding);

    OwnerMemberHooks hooks;
    hooks.ctor = &owner_row14_ctor_hook;

    Guarded guarded;
    reset_pools();
    CHECK(hooks.ctor(14, guarded.object(), nullptr));
    CHECK(binding.calls == 1);
    CHECK(binding.last.ok);
    CHECK(image_matches(guarded, Prefix::kFull));

    Guarded other;
    CHECK(!hooks.ctor(13, other.object(), nullptr));
    CHECK(binding.calls == 1);
    CHECK(other.object()[0] == kFill);

    rec.fail_event = true;
    Guarded failed;
    reset_pools();
    CHECK(!hooks.ctor(14, failed.object(), nullptr));
    CHECK(binding.calls == 2);
    CHECK(!binding.last.ok);
    CHECK(std::string(binding.last.failed_at) == "event_flag");

    owner_row14_bind(nullptr);
    Guarded unbound;
    CHECK(!hooks.ctor(14, unbound.object(), nullptr));
    CHECK(binding.calls == 2);
    CHECK(unbound.object()[0] == kFill);
    g_rec = nullptr;
}

}  // namespace

int main() {
    test_constants();
    test_success();
    test_hook_arguments();
    test_failure_prefixes();
    test_missing_hooks();
    test_preconditions();
    test_second_call();
    test_adapter();
    std::printf("OK test_owner_ctor_a597d0: 8 sections, %zu B object, prefix + double call\n", kObj);
    return 0;
}
