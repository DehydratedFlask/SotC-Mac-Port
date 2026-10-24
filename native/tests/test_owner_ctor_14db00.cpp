// Fixture for the row-3 owner constructor `0x14db00` (the ResourcePool ctor, 0x138 B object,
// pointer slot 0x2ee2cf8).
//
// The characterization is in the header and `local/re/evidence/owner-row3-14db00/`; this fixture is the
// falsifiable side of it. It asserts every written byte against a byte-exact expected image, the
// allocation sizes and order, the three heap objects (anchor, helper, control block), the lifted
// callees in isolation (0xf2f0, 0x20af0 incl. the lock-inc of non-null controls, 0x1ab270), every
// hook argument, every failure prefix, missing hooks and vptrs, preconditions, a double call, the
// guard ring, that the object is *partially* initialized (holes keep the fill byte), and the
// adapter inside the real 21-row `init_owner_members` driver.
//
// Byte-exact ELF pins live in `tools/lift/check_fowner_ctor_14db00.py`.
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <type_traits>
#include <vector>

#include "core/owner_ctor_14db00.hpp"
#include "core/owner_members.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

static_assert(std::is_same_v<decltype(OwnerMemberHooks::ctor),
                             bool (*)(std::size_t, void*, void*)>,
              "the adapter must fit owner_members' context-free ctor hook");

namespace {

constexpr std::size_t kObj = kOwnerCtor14DB00ObjSize;
constexpr std::size_t kGuard = 0x40;
constexpr std::uint8_t kFill = 0xCC;
constexpr std::uint8_t kGuardFill = 0xA5;
constexpr std::uint8_t kPoolFill = 0xDD;
constexpr std::uint64_t kMutexHandle = 0x1122334455667788ull;

alignas(8) std::uint8_t g_anchor[kOwnerRow3AnchorSize];
alignas(8) std::uint8_t g_helper[kOwnerRow3HelperSize];
alignas(8) std::uint8_t g_control[kOwnerRow3ControlSize];
alignas(8) std::uint8_t g_engine_marker[16];  // stands in for the [0x2ee2d00] object

// Distinct, never-callable stand-ins for the five guest vtables.
alignas(8) std::uint8_t g_vt_pool[8];
alignas(8) std::uint8_t g_vt_base[8];
alignas(8) std::uint8_t g_vt_command[8];
alignas(8) std::uint8_t g_vt_control[8];
alignas(8) std::uint8_t g_vt_helper[8];

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
    bool untouched() const noexcept {
        for (std::size_t i = 0; i < kObj; ++i) {
            if (object()[i] != kFill) {
                return false;
            }
        }
        return true;
    }
};

void reset_pools() {
    std::memset(g_anchor, kPoolFill, sizeof g_anchor);
    std::memset(g_helper, kPoolFill, sizeof g_helper);
    std::memset(g_control, kPoolFill, sizeof g_control);
}

enum class Event { kAlloc, kMutex, kConsole, kAlloc17090 };

struct Recorder {
    std::vector<std::size_t> allocations;       // 0x17070
    std::vector<std::size_t> allocations_17090; // 0x17090
    std::vector<Event> order;

    int fail_allocate = -1;   // 0-based 0x17070 index
    bool fail_control = false;
    bool fail_mutex = false;
    bool fail_console = false;

    int mutex_calls = 0;
    void* mutex_at = nullptr;

    int console_calls = 0;
    void* console_command = nullptr;
    std::uint8_t console_registered_byte = 0xFF;
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
    if (index % 2 == 0) {
        return size == kOwnerRow3AnchorSize ? g_anchor : nullptr;
    }
    return size == kOwnerRow3HelperSize ? g_helper : nullptr;
}

void* hook_allocate_17090(std::size_t size) noexcept {
    if (g_rec == nullptr) {
        return nullptr;
    }
    g_rec->order.push_back(Event::kAlloc17090);
    g_rec->allocations_17090.push_back(size);
    if (g_rec->fail_control) {
        return nullptr;
    }
    return size == kOwnerRow3ControlSize ? g_control : nullptr;
}

bool hook_mutex(void* at) noexcept {
    if (g_rec == nullptr) {
        return false;
    }
    g_rec->order.push_back(Event::kMutex);
    ++g_rec->mutex_calls;
    g_rec->mutex_at = at;
    if (g_rec->fail_mutex) {
        return false;
    }
    std::memcpy(at, &kMutexHandle, sizeof kMutexHandle);  // fixture stand-in for 0x3b310
    return true;
}

bool hook_console(void* command) noexcept {
    if (g_rec == nullptr) {
        return false;
    }
    g_rec->order.push_back(Event::kConsole);
    ++g_rec->console_calls;
    g_rec->console_command = command;
    g_rec->console_registered_byte = static_cast<std::uint8_t*>(command)[kOwnerCommandRegisteredOffset];
    return !g_rec->fail_console;
}

OwnerRow3Hooks make_hooks() {
    OwnerRow3Hooks hooks;
    hooks.allocate = &hook_allocate;
    hooks.allocate_17090 = &hook_allocate_17090;
    hooks.mutex_init = &hook_mutex;
    hooks.console_register = &hook_console;
    hooks.pool_vptr = g_vt_pool;
    hooks.command_base_vptr = g_vt_base;
    hooks.command_vptr = g_vt_command;
    hooks.control_vptr = g_vt_control;
    hooks.helper_vptr = g_vt_helper;
    return hooks;
}

// Prefix of the original store sequence, used to check every failure path byte-for-byte.
enum class Prefix {
    kBlocks,   // vptr, engine, map header, zero blocks: before the anchor allocation
    kAnchor,   // anchor linked, +0x60: before the mutex
    kMutex,    // mutex handle + [+0x78]: before the holders / command base
    kCommand,  // command base, derived tail, registered byte = 1: console hook runs here
    kTail,     // +0x120 / +0x128: before the helper allocation (= state at alloc_helper/alloc_control)
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

Image expected_prefix(Prefix prefix, const void* engine, const void* name_data,
                      const void* description_data, const std::uint8_t* object) {
    Image img;
    img.put_ptr(kOwnerRow3VptrOffset, g_vt_pool);
    img.put_ptr(kOwnerRow3EngineOffset, engine);
    img.put_ptr(kOwnerRow3MapHeaderOffset, object + kOwnerRow3MapHeaderOffset);
    img.put_ptr(kOwnerRow3MapLeftOffset, object + kOwnerRow3MapHeaderOffset);
    img.put64(kOwnerRow3MapRootOffset, 0u);
    img.zero(kOwnerRow3Zero30Offset, 16);
    img.zero(kOwnerRow3HelperPtrOffset, 16);
    img.zero(kOwnerRow3AnchorOffset, 16);
    if (prefix == Prefix::kBlocks) {
        return img;
    }

    img.put_ptr(kOwnerRow3AnchorOffset, g_anchor);
    if (prefix == Prefix::kAnchor) {
        return img;
    }

    img.put64(kOwnerRow3MutexOffset, kMutexHandle);
    img.put64(kOwnerRow3Qword78Offset, 0u);
    if (prefix == Prefix::kMutex) {
        return img;
    }

    // Command base (0x20af0), then the derived tail.
    const std::size_t cmd = kOwnerRow3CommandOffset;
    img.put_ptr(cmd + kOwnerCommandVptrOffset, g_vt_command);
    img.put8(cmd + kOwnerCommandRegisteredOffset, 1u);  // 0x20d60 set it before the console hook
    img.put32(cmd + kOwnerCommandFlagsOffset, kOwnerRow3CommandFlags);
    img.put_ptr(cmd + kOwnerCommandNameOffset, name_data);
    img.put64(cmd + kOwnerCommandNameOffset + 8, kOwnerRow3CommandNameLength);
    img.put64(cmd + kOwnerCommandNameOffset + 0x10, 0u);
    img.put64(cmd + kOwnerCommandNameOffset + 0x18, 0u);
    img.put_ptr(cmd + kOwnerCommandDescriptionOffset, description_data);
    img.put64(cmd + kOwnerCommandDescriptionOffset + 8, kOwnerRow3CommandDescriptionLength);
    img.put64(cmd + kOwnerCommandDescriptionOffset + 0x10, 0u);
    img.put64(cmd + kOwnerCommandDescriptionOffset + 0x18, 0u);
    img.put64(kOwnerRow3HandlerOffset, kOwnerRow3HandlerRva);
    img.put64(kOwnerRow3HandlerOffset + 8, 0u);
    img.put_ptr(kOwnerRow3BackPointerOffset, object);
    img.zero(kOwnerRow3Zero0F8Offset, 32);
    if (prefix == Prefix::kCommand) {
        return img;
    }

    img.put8(kOwnerRow3Byte120Offset, 0u);
    img.zero(kOwnerRow3Zero128Offset, 16);
    if (prefix == Prefix::kTail) {
        return img;
    }

    img.put_ptr(kOwnerRow3HelperPtrOffset, g_helper);
    img.put_ptr(kOwnerRow3HelperCtrlOffset, g_control);
    return img;
}

// The module's string literals have their own addresses; compare content, then reuse the actual
// pointers in the expected image.
struct Strings {
    const void* name = nullptr;
    const void* description = nullptr;
};

Strings read_strings(const std::uint8_t* object) {
    Strings out;
    std::memcpy(&out.name, object + kOwnerRow3CommandOffset + kOwnerCommandNameOffset,
                sizeof out.name);
    std::memcpy(&out.description,
                object + kOwnerRow3CommandOffset + kOwnerCommandDescriptionOffset,
                sizeof out.description);
    return out;
}

bool image_matches(const Guarded& guarded, Prefix prefix, const void* engine = g_engine_marker) {
    Strings strings;
    if (prefix >= Prefix::kCommand) {
        strings = read_strings(guarded.object());
        if (strings.name == nullptr || strings.description == nullptr ||
            std::strcmp(static_cast<const char*>(strings.name), kOwnerRow3CommandName) != 0 ||
            std::strcmp(static_cast<const char*>(strings.description),
                        kOwnerRow3CommandDescription) != 0) {
            return false;
        }
    }
    const Image expected = expected_prefix(prefix, engine, strings.name, strings.description,
                                           guarded.object());
    return std::memcmp(guarded.object(), expected.bytes, kObj) == 0;
}

bool anchor_matches(bool written) {
    std::uint8_t expected[kOwnerRow3AnchorSize];
    std::memset(expected, kPoolFill, sizeof expected);
    if (written) {
        const void* self = g_anchor;
        std::memcpy(expected + 0x00, &self, sizeof self);
        std::memcpy(expected + 0x08, &self, sizeof self);
        std::memcpy(expected + 0x10, &self, sizeof self);
        expected[0x18] = 1;
        expected[0x19] = 1;
    }
    return std::memcmp(g_anchor, expected, sizeof expected) == 0;
}

bool helper_matches(bool written) {
    std::uint8_t expected[kOwnerRow3HelperSize];
    std::memset(expected, kPoolFill, sizeof expected);
    if (written) {
        const void* vt = g_vt_helper;
        std::memcpy(expected + 0x00, &vt, sizeof vt);
        std::memset(expected + 0x10, 0, 0x10);
        std::memset(expected + 0x20, 0, 0x08);
    }
    return std::memcmp(g_helper, expected, sizeof expected) == 0;
}

bool control_matches(bool written) {
    std::uint8_t expected[kOwnerRow3ControlSize];
    std::memset(expected, kPoolFill, sizeof expected);
    if (written) {
        const void* vt = g_vt_control;
        const void* helper = g_helper;
        const std::uint32_t one = 1;
        std::memcpy(expected + 0x00, &vt, sizeof vt);
        std::memcpy(expected + 0x08, &one, sizeof one);
        std::memcpy(expected + 0x0C, &one, sizeof one);
        std::memcpy(expected + 0x10, &helper, sizeof helper);
    }
    return std::memcmp(g_control, expected, sizeof expected) == 0;
}

// ---------------------------------------------------------------------------------------------
// Sections
// ---------------------------------------------------------------------------------------------

void test_constants() {
    CHECK(kOwnerCtor14DB00Rva == 0x14DB00);
    CHECK(kOwnerCtor14DB00Size == 0x183);
    CHECK(kOwnerCtor14DB00ObjSize == 0x138);
    CHECK(kOwnerCtor14DB00SlotRva == 0x2EE2CF8);
    CHECK(kOwnerCtor14DB00ArgRva == 0x2EE2D00);
    CHECK(kOwnerMemberRow3 == 3);
    CHECK(kOwnerDtor14DE00Rva == 0x14DE00);
    CHECK(kOwnerDtor14E170Rva == 0x14E170);
    CHECK(kOwnerRow3PoolVptrRva == 0x1598628);
    CHECK(kOwnerRow3CommandBaseVptrRva == 0x1594D90);
    CHECK(kOwnerRow3CommandVptrRva == 0x1594DD0);
    CHECK(kOwnerRow3ControlVptrRva == 0x15986F0);
    CHECK(kOwnerRow3HelperVptrRva == 0x1598AB0);
    CHECK(kOwnerRow3CommandRegisterRva == 0x20D60);
    CHECK(kOwnerRow3SetHelperRva == 0x14E910);
    CHECK(kOwnerRow3AnchorSize == 0x60 && kOwnerRow3HelperSize == 0x28 &&
          kOwnerRow3ControlSize == 0x20);
    CHECK(std::strlen(kOwnerRow3CommandName) == kOwnerRow3CommandNameLength);
    CHECK(kOwnerRow3CommandNameLength == 13);
    CHECK(std::strlen(kOwnerRow3CommandDescription) == kOwnerRow3CommandDescriptionLength);
    CHECK(kOwnerRow3CommandDescriptionLength == 0x3E);
    CHECK(kOwnerRow3CommandFlags == 2);
    CHECK(kOwnerRow3HandlerSlotRva == 0x15FEB58 && kOwnerRow3HandlerRva == 0x14DC90);
    CHECK(kOwnerRow3CommandOffset + kOwnerCommandBaseSize == kOwnerRow3BackPointerOffset);
    CHECK(kOwnerRow3Zero128Offset + 0x10 == kObj);
    // Rows 3 agrees with the driver table entry.
    CHECK(kOwnerMemberRows[2].size == kObj);
    CHECK(kOwnerMemberRows[2].ctor_rva == kOwnerCtor14DB00Rva);
    CHECK(kOwnerMemberRows[2].dest_kind == OwnerRowDest::kGlobal);
    CHECK(kOwnerMemberRows[2].dest == kOwnerCtor14DB00SlotRva);
    CHECK(kOwnerMemberRows[2].destroy == OwnerRowDestroy::kVirtualDtor);
    CHECK(kOwnerMemberRows[2].arg_kind == OwnerRowArg::kGlobal);
    CHECK(kOwnerMemberRows[2].arg_rva == kOwnerCtor14DB00ArgRva);
}

void test_success() {
    Guarded guarded;
    reset_pools();
    Recorder rec;
    g_rec = &rec;

    const OwnerRow3Result result =
        owner_ctor_14db00(guarded.object(), g_engine_marker, make_hooks());
    g_rec = nullptr;

    CHECK(result.ok);
    CHECK(result.failed_at == nullptr);
    CHECK(rec.allocations == (std::vector<std::size_t>{0x60, 0x28}));
    CHECK(rec.allocations_17090 == (std::vector<std::size_t>{0x20}));
    CHECK((rec.order == std::vector<Event>{Event::kAlloc, Event::kMutex, Event::kConsole,
                                           Event::kAlloc, Event::kAlloc17090}));
    CHECK(image_matches(guarded, Prefix::kFull));
    CHECK(anchor_matches(true));
    CHECK(helper_matches(true));
    CHECK(control_matches(true));
    CHECK(guarded.guards_ok());

    // Holes the original never writes (see the header layout).
    const std::uint8_t* const o = guarded.object();
    const struct { std::size_t lo, hi; } holes[] = {
        {0x10, 0x18},   // the std::map EBO slot
        {0x40, 0x48},
        {0x58, 0x60},
        {0x88, 0x98},   // command base +0x08..+0x17 (pending-list link)
        {0x99, 0x9C},   // command base +0x19..+0x1b
        {0x118, 0x120},
        {0x121, 0x128},
    };
    for (const auto& hole : holes) {
        for (std::size_t offset = hole.lo; offset < hole.hi; ++offset) {
            CHECK(o[offset] == kFill);
        }
    }
    // The registered byte is 1 (set by 0x20d60), the flags dword is 2, the pointers are native.
    CHECK(o[kOwnerRow3CommandOffset + kOwnerCommandRegisteredOffset] == 1);
    std::uint32_t flags = 0;
    std::memcpy(&flags, o + kOwnerRow3CommandOffset + kOwnerCommandFlagsOffset, sizeof flags);
    CHECK(flags == 2);
    void* back = nullptr;
    std::memcpy(&back, o + kOwnerRow3BackPointerOffset, sizeof back);
    CHECK(back == o);
    void* header = nullptr;
    std::memcpy(&header, o + kOwnerRow3MapHeaderOffset, sizeof header);
    CHECK(header == o + 0x18);
    std::memcpy(&header, o + kOwnerRow3MapLeftOffset, sizeof header);
    CHECK(header == o + 0x18);
}

void test_hook_arguments_and_callees() {
    // ---- hooks ----
    {
        Guarded guarded;
        reset_pools();
        Recorder rec;
        g_rec = &rec;
        const OwnerRow3Result result =
            owner_ctor_14db00(guarded.object(), g_engine_marker, make_hooks());
        g_rec = nullptr;
        CHECK(result.ok);
        CHECK(rec.mutex_calls == 1);
        CHECK(rec.mutex_at == guarded.object() + kOwnerRow3MutexOffset);
        CHECK(rec.console_calls == 1);
        CHECK(rec.console_command == guarded.object() + kOwnerRow3CommandOffset);
        CHECK(rec.console_registered_byte == 1);  // the guard byte is set before the hook runs
    }

    // ---- 0xf2f0: {ptr, strlen, 0, 0}, independent of the previous contents ----
    {
        SharedStr out;
        std::memset(&out, 0xEE, sizeof out);
        owner_row3_string_view_f2f0(out, "hello");
        CHECK(std::strcmp(reinterpret_cast<const char*>(out.data), "hello") == 0);
        CHECK(out.size == 5);
        CHECK(out.aux == 0 && out.ctl == nullptr);
        owner_row3_string_view_f2f0(out, "");
        CHECK(out.size == 0 && out.ctl == nullptr);
    }

    // ---- 0x20af0: layout, holes, lock-inc of non-null controls only ----
    {
        alignas(8) std::uint8_t buffer[0x60];
        std::memset(buffer, kFill, sizeof buffer);
        alignas(8) std::int64_t name_counter = 5;
        alignas(8) std::int64_t desc_counter = 9;
        const char name_text[] = "n";
        const char desc_text[] = "dd";
        SharedStr name{reinterpret_cast<const std::uint8_t*>(name_text), 1, 0x77, &name_counter};
        SharedStr desc{reinterpret_cast<const std::uint8_t*>(desc_text), 2, 0x88, &desc_counter};
        owner_row3_command_ctor_20af0(buffer, g_vt_base, name, desc, 7);
        void* vt = nullptr;
        std::memcpy(&vt, buffer, sizeof vt);
        CHECK(vt == g_vt_base);
        CHECK(buffer[0x18] == 0);
        std::uint32_t f = 0;
        std::memcpy(&f, buffer + 0x1C, sizeof f);
        CHECK(f == 7);
        SharedStr copy;
        std::memcpy(&copy, buffer + 0x20, sizeof copy);
        CHECK(copy.data == name.data && copy.size == 1 && copy.aux == 0x77 &&
              copy.ctl == &name_counter);
        std::memcpy(&copy, buffer + 0x40, sizeof copy);
        CHECK(copy.data == desc.data && copy.size == 2 && copy.aux == 0x88 &&
              copy.ctl == &desc_counter);
        CHECK(name_counter == 6 && desc_counter == 10);  // one lock inc each
        for (std::size_t i = 0x08; i < 0x18; ++i) CHECK(buffer[i] == kFill);
        for (std::size_t i = 0x19; i < 0x1C; ++i) CHECK(buffer[i] == kFill);

        // A null control is never touched.
        SharedStr plain{reinterpret_cast<const std::uint8_t*>(name_text), 1, 0, nullptr};
        owner_row3_command_ctor_20af0(buffer, g_vt_base, plain, plain, 1);
        CHECK(name_counter == 6 && desc_counter == 10);
    }

    // ---- 0x1ab270: vptr, zero blocks, hole at +8 ----
    {
        std::uint8_t buffer[kOwnerRow3HelperSize];
        std::memset(buffer, kFill, sizeof buffer);
        owner_row3_helper_ctor_1ab270(buffer, g_vt_helper);
        void* vt = nullptr;
        std::memcpy(&vt, buffer, sizeof vt);
        CHECK(vt == g_vt_helper);
        for (std::size_t i = 0x08; i < 0x10; ++i) CHECK(buffer[i] == kFill);
        for (std::size_t i = 0x10; i < 0x28; ++i) CHECK(buffer[i] == 0);
    }
}

struct FailureCase {
    const char* failed_at;
    Prefix prefix;
    int fail_allocate;
    bool fail_control;
    bool fail_mutex;
    bool fail_console;
    int expect_mutex_calls;
    int expect_console_calls;
    bool anchor_written;
    bool helper_written;
    bool control_written;
};

void test_failure_prefixes() {
    const FailureCase cases[] = {
        {"alloc_anchor", Prefix::kBlocks, 0, false, false, false, 0, 0, false, false, false},
        {"mutex_init", Prefix::kAnchor, -1, false, true, false, 1, 0, true, false, false},
        {"console_register", Prefix::kCommand, -1, false, false, true, 1, 1, true, false, false},
        {"alloc_helper", Prefix::kTail, 1, false, false, false, 1, 1, true, false, false},
        {"alloc_control", Prefix::kTail, -1, true, false, false, 1, 1, true, true, false},
    };
    for (const FailureCase& item : cases) {
        Guarded guarded;
        reset_pools();
        Recorder rec;
        rec.fail_allocate = item.fail_allocate;
        rec.fail_control = item.fail_control;
        rec.fail_mutex = item.fail_mutex;
        rec.fail_console = item.fail_console;
        g_rec = &rec;

        const OwnerRow3Result result =
            owner_ctor_14db00(guarded.object(), g_engine_marker, make_hooks());
        g_rec = nullptr;

        CHECK(!result.ok);
        CHECK(result.failed_at != nullptr);
        CHECK(std::string(result.failed_at) == item.failed_at);
        CHECK(image_matches(guarded, item.prefix));
        CHECK(anchor_matches(item.anchor_written));
        CHECK(helper_matches(item.helper_written));
        CHECK(control_matches(item.control_written));
        CHECK(guarded.guards_ok());
        CHECK(rec.mutex_calls == item.expect_mutex_calls);
        CHECK(rec.console_calls == item.expect_console_calls);
    }

    // Each of the five host vptrs is mandatory and checked before any write.
    for (int missing = 0; missing < 5; ++missing) {
        Guarded guarded;
        reset_pools();
        Recorder rec;
        g_rec = &rec;
        OwnerRow3Hooks hooks = make_hooks();
        switch (missing) {
            case 0: hooks.pool_vptr = nullptr; break;
            case 1: hooks.command_base_vptr = nullptr; break;
            case 2: hooks.command_vptr = nullptr; break;
            case 3: hooks.control_vptr = nullptr; break;
            default: hooks.helper_vptr = nullptr; break;
        }
        const OwnerRow3Result result = owner_ctor_14db00(guarded.object(), g_engine_marker, hooks);
        g_rec = nullptr;
        CHECK(!result.ok);
        CHECK(std::string(result.failed_at) == "vptr");
        CHECK(guarded.untouched());
        CHECK(rec.allocations.empty() && rec.allocations_17090.empty());
        CHECK(guarded.guards_ok());
    }
}

void test_missing_hooks() {
    struct Missing {
        const char* failed_at;
        Prefix prefix;
        bool with_alloc;
        bool with_mutex;
        bool with_console;
        bool anchor_written;
        bool helper_written;
    };
    const Missing cases[] = {
        {"alloc_anchor", Prefix::kBlocks, false, false, false, false, false},
        {"mutex_init", Prefix::kAnchor, true, false, false, true, false},
        {"console_register", Prefix::kCommand, true, true, false, true, false},
        {"alloc_control", Prefix::kTail, true, true, true, true, true},
    };
    for (const Missing& item : cases) {
        Guarded guarded;
        reset_pools();
        Recorder rec;
        OwnerRow3Hooks hooks = make_hooks();
        hooks.allocate_17090 = nullptr;  // the missing hook of the last case; unreached earlier
        if (!item.with_alloc) {
            hooks.allocate = nullptr;
        }
        if (!item.with_mutex) {
            hooks.mutex_init = nullptr;
        }
        if (!item.with_console) {
            hooks.console_register = nullptr;
        }
        g_rec = &rec;
        const OwnerRow3Result result = owner_ctor_14db00(guarded.object(), g_engine_marker, hooks);
        g_rec = nullptr;
        CHECK(!result.ok);
        CHECK(result.failed_at != nullptr);
        CHECK(std::string(result.failed_at) == item.failed_at);
        CHECK(image_matches(guarded, item.prefix));
        CHECK(anchor_matches(item.anchor_written));
        CHECK(helper_matches(item.helper_written));
        CHECK(guarded.guards_ok());
    }
}

void test_preconditions() {
    Recorder rec;
    g_rec = &rec;
    const OwnerRow3Result null_object = owner_ctor_14db00(nullptr, g_engine_marker, make_hooks());
    CHECK(!null_object.ok);
    CHECK(null_object.failed_at != nullptr);
    CHECK(std::string(null_object.failed_at) == "null-object");

    Guarded guarded;
    const OwnerRow3Result misaligned =
        owner_ctor_14db00(guarded.object() + 1, g_engine_marker, make_hooks());
    CHECK(!misaligned.ok);
    CHECK(misaligned.failed_at != nullptr);
    CHECK(std::string(misaligned.failed_at) == "object-align");
    CHECK(rec.allocations.empty() && rec.allocations_17090.empty());
    CHECK(rec.mutex_calls == 0 && rec.console_calls == 0);
    CHECK(guarded.untouched());
    CHECK(guarded.guards_ok());

    // The engine is stored verbatim, including null (the original does not test it).
    reset_pools();
    Guarded with_null_engine;
    CHECK(owner_ctor_14db00(with_null_engine.object(), nullptr, make_hooks()).ok);
    CHECK(image_matches(with_null_engine, Prefix::kFull, nullptr));
    g_rec = nullptr;
}

void test_second_call() {
    Guarded guarded;
    reset_pools();
    Recorder first;
    g_rec = &first;
    CHECK(owner_ctor_14db00(guarded.object(), g_engine_marker, make_hooks()).ok);
    g_rec = nullptr;
    CHECK(image_matches(guarded, Prefix::kFull));
    std::uint8_t snapshot[kObj];
    std::memcpy(snapshot, guarded.object(), kObj);

    // The body never reads `this`, except the registered byte, which it rewrites to 0 first:
    // a second call writes the same offsets with the same values and registers again.
    reset_pools();
    Recorder second;
    g_rec = &second;
    CHECK(owner_ctor_14db00(guarded.object(), g_engine_marker, make_hooks()).ok);
    g_rec = nullptr;
    CHECK(std::memcmp(snapshot, guarded.object(), kObj) == 0);
    CHECK(first.allocations == second.allocations);
    CHECK(first.allocations_17090 == second.allocations_17090);
    CHECK(first.order == second.order);
    CHECK(second.console_calls == 1);
    CHECK(guarded.guards_ok());
}

// ---- driver integration -----------------------------------------------------------------------

struct DriverLog {
    std::vector<std::size_t> rows;
    std::vector<void*> live;
    void* row1_object = nullptr;
    void* row3_object = nullptr;
};
DriverLog* g_driver = nullptr;

void* driver_alloc(std::size_t size) {
    void* p = std::calloc(1, size);
    CHECK(p != nullptr);
    g_driver->live.push_back(p);
    return p;
}

bool driver_free(void* ptr) {
    for (auto& live : g_driver->live) {
        if (live == ptr) {
            std::free(ptr);
            live = nullptr;
            return true;
        }
    }
    return false;
}

// Every row except 3 is a stand-in; row 3 goes through the native adapter.
bool composite_ctor(std::size_t row, void* object, void* arg) {
    g_driver->rows.push_back(row);
    if (row == 1) {
        g_driver->row1_object = object;
    }
    if (row == 3) {
        g_driver->row3_object = object;
        return owner_row3_ctor_hook(row, object, arg);
    }
    if (row == 4 || row == 8) {
        *static_cast<void**>(arg) = object;  // self-registered cells
    }
    return true;
}

void driver_virtual_dtor(void* object) { (void)driver_free(object); }
void driver_dtor(void*) {}

void test_adapter() {
    // ---- a) the adapter alone ----
    {
        Recorder rec;
        g_rec = &rec;
        OwnerRow3Binding binding;
        binding.hooks = make_hooks();
        owner_row3_bind(&binding);

        OwnerMemberHooks hooks;
        hooks.ctor = &owner_row3_ctor_hook;

        Guarded guarded;
        reset_pools();
        CHECK(hooks.ctor(3, guarded.object(), g_engine_marker));
        CHECK(binding.calls == 1);
        CHECK(binding.last.ok);
        CHECK(binding.last_engine == g_engine_marker);
        CHECK(image_matches(guarded, Prefix::kFull));

        Guarded other;
        CHECK(!hooks.ctor(2, other.object(), g_engine_marker));
        CHECK(!hooks.ctor(18, other.object(), g_engine_marker));
        CHECK(binding.calls == 1);
        CHECK(other.untouched());

        rec.fail_console = true;
        Guarded failed;
        reset_pools();
        CHECK(!hooks.ctor(3, failed.object(), g_engine_marker));
        CHECK(binding.calls == 2);
        CHECK(!binding.last.ok);
        CHECK(std::string(binding.last.failed_at) == "console_register");

        owner_row3_bind(nullptr);
        Guarded unbound;
        CHECK(!hooks.ctor(3, unbound.object(), g_engine_marker));
        CHECK(binding.calls == 2);
        CHECK(unbound.untouched());
        g_rec = nullptr;
    }

    // ---- b) inside the real 21-row driver: row 3 native, every other row a stand-in ----
    {
        DriverLog log;
        g_driver = &log;
        Recorder rec;
        g_rec = &rec;
        reset_pools();

        OwnerRow3Binding binding;
        binding.hooks = make_hooks();
        owner_row3_bind(&binding);

        OwnerMemberHooks hooks;
        hooks.allocate = &driver_alloc;
        hooks.deallocate = &driver_free;
        hooks.ctor = &composite_ctor;
        hooks.virtual_dtor = &driver_virtual_dtor;
        hooks.dtor = &driver_dtor;

        OwnerMemberState state;
        const OwnerMembersResult result = init_owner_members(state, hooks);
        CHECK(result.ok);
        CHECK(result.installed == 19);
        CHECK(log.rows.size() == 21);
        CHECK(log.row1_object != nullptr && log.row3_object != nullptr);

        // Row 3's argument is the row-1 object stored into the 0x2ee2d00 cell; the result lands in
        // the 0x2ee2cf8 cell.
        CHECK(state.globals[0x2EE2D00] == log.row1_object);
        CHECK(binding.calls == 1);
        CHECK(binding.last.ok);
        CHECK(binding.last_engine == log.row1_object);
        CHECK(state.globals[0x2EE2CF8] == log.row3_object);
        const auto* const bytes = static_cast<const std::uint8_t*>(log.row3_object);
        void* engine = nullptr;
        std::memcpy(&engine, bytes + kOwnerRow3EngineOffset, sizeof engine);
        CHECK(engine == log.row1_object);
        void* vptr = nullptr;
        std::memcpy(&vptr, bytes + kOwnerRow3VptrOffset, sizeof vptr);
        CHECK(vptr == g_vt_pool);
        CHECK(rec.console_command == bytes + kOwnerRow3CommandOffset);

        // The hook path stays available: a plain stand-in ctor still installs all 21 rows.
        OwnerMemberState hook_only;
        DriverLog log2;
        g_driver = &log2;
        hooks.ctor = [](std::size_t row, void* object, void* arg) -> bool {
            g_driver->rows.push_back(row);
            if (row == 4 || row == 8) {
                *static_cast<void**>(arg) = object;
            }
            return true;
        };
        const OwnerMembersResult again = init_owner_members(hook_only, hooks);
        CHECK(again.ok);
        CHECK(log2.rows.size() == 21);
        CHECK(binding.calls == 1);  // the native ctor was not run on the hook-only install

        owner_row3_bind(nullptr);
        g_rec = nullptr;
        for (void* p : log.live) {
            std::free(p);
        }
        for (void* p : log2.live) {
            std::free(p);
        }
        g_driver = nullptr;
    }
}

}  // namespace

int main() {
    test_constants();
    test_success();
    test_hook_arguments_and_callees();
    test_failure_prefixes();
    test_missing_hooks();
    test_preconditions();
    test_second_call();
    test_adapter();
    std::printf("OK test_owner_ctor_14db00: 8 sections, %zu B object, prefix + callees + double call + driver\n",
                kObj);
    return 0;
}
