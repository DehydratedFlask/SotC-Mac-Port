// Fixture for the row-18 owner constructor `0x3c5cb0` (0xb8 B object, field slot this+0x3b70).
//
// The characterization is in the header and `local/re/evidence/owner-row18-3c5cb0/`; this fixture is the
// falsifiable side of it. It asserts every written byte against a byte-exact expected image, the
// allocation sizes and order, the three self-linked sentinels, the two registered callable frames,
// every hook argument, every failure prefix, missing hooks, preconditions, a double call, the
// guard ring, that the object is *partially* initialized (holes keep the fill byte), and the
// adapter inside the real 21-row `init_owner_members` driver.
//
// Byte-exact ELF pins live in `tools/lift/check_fowner_ctor_3c5cb0.py`.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <type_traits>
#include <vector>

#include "core/init_sub_3be930.hpp"
#include "core/owner_ctor_3c5cb0.hpp"
#include "core/owner_members.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

static_assert(std::is_same_v<decltype(OwnerMemberHooks::ctor),
                             bool (*)(std::size_t, void*, void*)>,
              "the adapter must fit owner_members' context-free ctor hook");

namespace {

constexpr std::size_t kObj = kOwnerCtor3C5CB0ObjSize;
constexpr std::size_t kGuard = 0x40;
constexpr std::uint8_t kFill = 0xCC;
constexpr std::uint8_t kGuardFill = 0xA5;
constexpr std::uint8_t kPoolFill = 0xDD;
constexpr std::uint64_t kMutexHandle = 0x0102030405060708ull;

alignas(8) std::uint8_t g_node_a[kOwnerRow18NodeASize];
alignas(8) std::uint8_t g_node_b[kOwnerRow18NodeBSize];
alignas(8) std::uint8_t g_node_c[kOwnerRow18NodeCSize];
alignas(8) std::uint8_t g_pool_marker[16];  // stands in for the [0x2edf140] object

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
    std::memset(g_node_a, kPoolFill, sizeof g_node_a);
    std::memset(g_node_b, kPoolFill, sizeof g_node_b);
    std::memset(g_node_c, kPoolFill, sizeof g_node_c);
}

enum class Event { kAlloc, kMutex, kRegisterA, kRegisterB };

struct Recorder {
    std::vector<std::size_t> allocations;
    std::vector<Event> order;

    int fail_allocate = -1;  // 0-based allocation index
    bool fail_mutex = false;
    bool fail_register_a = false;
    bool fail_register_b = false;

    int mutex_calls = 0;
    void* mutex_at = nullptr;

    int a_calls = 0;
    void* a_pool = nullptr;
    OwnerRow18Callable a_callable{};
    std::uint32_t a_flag = 0xFFFFFFFFu;

    int b_calls = 0;
    void* b_pool = nullptr;
    OwnerRow18Callable b_callable{};
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
    switch (index % 3) {
        case 0: return size == kOwnerRow18NodeASize ? g_node_a : nullptr;
        case 1: return size == kOwnerRow18NodeBSize ? g_node_b : nullptr;
        default: return size == kOwnerRow18NodeCSize ? g_node_c : nullptr;
    }
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

bool hook_register_a(void* pool, const OwnerRow18Callable& callable, std::uint32_t flag) noexcept {
    if (g_rec == nullptr) {
        return false;
    }
    g_rec->order.push_back(Event::kRegisterA);
    ++g_rec->a_calls;
    g_rec->a_pool = pool;
    g_rec->a_callable = callable;
    g_rec->a_flag = flag;
    return !g_rec->fail_register_a;
}

bool hook_register_b(void* pool, const OwnerRow18Callable& callable) noexcept {
    if (g_rec == nullptr) {
        return false;
    }
    g_rec->order.push_back(Event::kRegisterB);
    ++g_rec->b_calls;
    g_rec->b_pool = pool;
    g_rec->b_callable = callable;
    return !g_rec->fail_register_b;
}

OwnerRow18Hooks make_hooks() {
    OwnerRow18Hooks hooks;
    hooks.allocate = &hook_allocate;
    hooks.mutex_init = &hook_mutex;
    hooks.register_116040 = &hook_register_a;
    hooks.register_116540 = &hook_register_b;
    return hooks;
}

// Prefix of the original store sequence, used to check every failure path byte-for-byte.
enum class Prefix {
    kHead,    // +0x00, +0x04, slot 0, +0x30 zero: before the first allocation
    kNodeA,   // sentinel A linked, +0x48 zero: before the second allocation
    kNodeB,   // sentinel B linked, +0x58, +0x68 zero: before the third allocation
    kNodeC,   // sentinel C linked, scalars, slot 1: before the mutex
    kMutex,   // mutex handle written; registrations do not touch the object
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
    void slot(std::size_t offset) noexcept {  // 0xf180: stand-in ptr, then 0x18 zero bytes
        put_ptr(offset, slot_vtable_2dd9f78());
        zero(offset + 8, 0x18);
    }
};

Image expected_prefix(Prefix prefix) {
    Image img;
    img.put8(kOwnerRow18Byte00Offset, 0u);
    img.put32(kOwnerRow18Dword04Offset, 0u);
    img.slot(kOwnerRow18Slot0Offset);
    img.zero(kOwnerRow18HeadAOffset, 16);
    if (prefix == Prefix::kHead) {
        return img;
    }

    img.put_ptr(kOwnerRow18HeadAOffset, g_node_a);
    img.zero(kOwnerRow18HeadBOffset, 16);
    if (prefix == Prefix::kNodeA) {
        return img;
    }

    img.put_ptr(kOwnerRow18HeadBOffset, g_node_b);
    img.put32(kOwnerRow18Dword58Offset, 0u);
    img.zero(kOwnerRow18HeadCOffset, 16);
    if (prefix == Prefix::kNodeB) {
        return img;
    }

    img.put_ptr(kOwnerRow18HeadCOffset, g_node_c);
    img.put32(kOwnerRow18Dword78Offset, 0xFFFFFFFFu);
    img.put32(kOwnerRow18Dword7COffset, 0u);
    img.put8(kOwnerRow18Byte80Offset, 0u);
    img.put8(kOwnerRow18Byte81Offset, 0u);
    img.put64(kOwnerRow18Qword84Offset, 0u);
    img.slot(kOwnerRow18Slot1Offset);
    if (prefix == Prefix::kNodeC) {
        return img;
    }

    img.put64(kOwnerRow18MutexOffset, kMutexHandle);
    return img;
}

bool image_matches(const Guarded& guarded, Prefix prefix) {
    const Image expected = expected_prefix(prefix);
    return std::memcmp(guarded.object(), expected.bytes, kObj) == 0;
}

// A sentinel node is written in its first two qwords only; the rest keeps the pool fill.
bool node_matches(const std::uint8_t* node, std::size_t size, bool linked) {
    std::uint8_t expected[0x50];
    CHECK(size <= sizeof expected);
    std::memset(expected, kPoolFill, size);
    if (linked) {
        const void* self = node;
        std::memcpy(expected, &self, sizeof self);
        std::memcpy(expected + 8, &self, sizeof self);
    }
    return std::memcmp(node, expected, size) == 0;
}

bool pools_match(Prefix prefix) {
    const bool a = prefix != Prefix::kHead;
    const bool b = prefix != Prefix::kHead && prefix != Prefix::kNodeA;
    const bool c = prefix == Prefix::kNodeC || prefix == Prefix::kMutex || prefix == Prefix::kFull;
    return node_matches(g_node_a, sizeof g_node_a, a) &&
           node_matches(g_node_b, sizeof g_node_b, b) &&
           node_matches(g_node_c, sizeof g_node_c, c);
}

// ---------------------------------------------------------------------------------------------
// Sections
// ---------------------------------------------------------------------------------------------

void test_constants() {
    CHECK(kOwnerCtor3C5CB0Rva == 0x3C5CB0);
    CHECK(kOwnerCtor3C5CB0Size == 0x13D);
    CHECK(kOwnerCtor3C5CB0ObjSize == 0xB8);
    CHECK(kOwnerCtor3C5CB0SlotOffset == 0x3B70);
    CHECK(kOwnerCtor3C5CB0ArgRva == 0x2EDF140);
    CHECK(kOwnerMemberRow18 == 18);
    CHECK(kOwnerDtor3C74B0Rva == 0x3C74B0);
    CHECK(kOwnerRow18NodeASize == 0x30 && kOwnerRow18NodeBSize == 0x30 &&
          kOwnerRow18NodeCSize == 0x50);
    CHECK(kOwnerRow18Slot1Offset + 0x20 == kOwnerRow18MutexOffset);
    CHECK(kOwnerRow18MutexOffset + 8 == kObj);
    CHECK(kOwnerRow18Dword78Value == 0xFFFFFFFFu);
    CHECK(kOwnerRow18CallbackASlotRva == 0x15FF810 && kOwnerRow18CallbackARva == 0x3C5DF0);
    CHECK(kOwnerRow18CallbackBSlotRva == 0x15FF590 && kOwnerRow18CallbackBRva == 0x3C7380);
    CHECK(kOwnerRow18RegisterAFlag == 1);
    CHECK(kOwnerRow18RegisterARva == 0x116040 && kOwnerRow18RegisterBRva == 0x116540);
    CHECK(kOwnerRow18MutexInitRva == 0x3B310 && kOwnerRow18SlotInitRva == 0xF180);
    // Rows 18 and 14/21 agree on the driver table entry.
    CHECK(kOwnerMemberRows[17].size == kObj);
    CHECK(kOwnerMemberRows[17].ctor_rva == kOwnerCtor3C5CB0Rva);
    CHECK(kOwnerMemberRows[17].dest_kind == OwnerRowDest::kThisField);
    CHECK(kOwnerMemberRows[17].dest == kOwnerCtor3C5CB0SlotOffset);
    CHECK(kOwnerMemberRows[17].destroy == OwnerRowDestroy::kDtorThenFree);
    CHECK(kOwnerMemberRows[17].arg_kind == OwnerRowArg::kGlobal);
    CHECK(kOwnerMemberRows[17].arg_rva == kOwnerCtor3C5CB0ArgRva);
}

void test_success() {
    Guarded guarded;
    reset_pools();
    Recorder rec;
    g_rec = &rec;

    const OwnerRow18Result result =
        owner_ctor_3c5cb0(guarded.object(), g_pool_marker, make_hooks());
    g_rec = nullptr;

    CHECK(result.ok);
    CHECK(result.failed_at == nullptr);
    CHECK(rec.allocations == (std::vector<std::size_t>{0x30, 0x30, 0x50}));
    CHECK((rec.order == std::vector<Event>{Event::kAlloc, Event::kAlloc, Event::kAlloc,
                                           Event::kMutex, Event::kRegisterA,
                                           Event::kRegisterB}));
    CHECK(image_matches(guarded, Prefix::kFull));
    CHECK(pools_match(Prefix::kFull));
    CHECK(guarded.guards_ok());

    // Holes the original never writes (see the header layout).
    const std::uint8_t* const o = guarded.object();
    const struct { std::size_t lo, hi; } holes[] = {
        {0x01, 0x04}, {0x28, 0x30}, {0x40, 0x48}, {0x5C, 0x68},
        {0x82, 0x84}, {0x8C, 0x90},
    };
    for (const auto& hole : holes) {
        for (std::size_t offset = hole.lo; offset < hole.hi; ++offset) {
            CHECK(o[offset] == kFill);
        }
    }
    // The sentinels point at themselves; the counters next to the heads are zero.
    void* head = nullptr;
    std::memcpy(&head, o + kOwnerRow18HeadAOffset, sizeof head);
    CHECK(head == g_node_a);
    std::memcpy(&head, o + kOwnerRow18HeadBOffset, sizeof head);
    CHECK(head == g_node_b);
    std::memcpy(&head, o + kOwnerRow18HeadCOffset, sizeof head);
    CHECK(head == g_node_c);
    for (std::size_t i = 0; i < 8; ++i) {
        CHECK(o[kOwnerRow18CountAOffset + i] == 0);
        CHECK(o[kOwnerRow18CountBOffset + i] == 0);
        CHECK(o[kOwnerRow18CountCOffset + i] == 0);
    }
    // The two 0xf180 slots carry the host stand-in and are otherwise empty.
    for (const std::size_t base : {kOwnerRow18Slot0Offset, kOwnerRow18Slot1Offset}) {
        void* vt = nullptr;
        std::memcpy(&vt, o + base, sizeof vt);
        CHECK(vt == slot_vtable_2dd9f78());
        for (std::size_t i = 8; i < 0x20; ++i) {
            CHECK(o[base + i] == 0);
        }
    }
    // dword +0x78 is -1, +0x7c is 0, the unaligned qword +0x84 is 0.
    std::uint32_t d = 0;
    std::memcpy(&d, o + kOwnerRow18Dword78Offset, sizeof d);
    CHECK(d == 0xFFFFFFFFu);
    std::memcpy(&d, o + kOwnerRow18Dword7COffset, sizeof d);
    CHECK(d == 0);
    for (std::size_t i = 0; i < 8; ++i) {
        CHECK(o[kOwnerRow18Qword84Offset + i] == 0);
    }
}

void test_hook_arguments() {
    Guarded guarded;
    reset_pools();
    Recorder rec;
    g_rec = &rec;

    const OwnerRow18Result result =
        owner_ctor_3c5cb0(guarded.object(), g_pool_marker, make_hooks());
    g_rec = nullptr;
    CHECK(result.ok);

    CHECK(rec.mutex_calls == 1);
    CHECK(rec.mutex_at == guarded.object() + kOwnerRow18MutexOffset);

    CHECK(rec.a_calls == 1);
    CHECK(rec.a_pool == g_pool_marker);
    CHECK(rec.a_callable.self == guarded.object());
    CHECK(rec.a_callable.function == 0x3C5DF0);
    CHECK(rec.a_callable.word2 == 0 && rec.a_callable.word3 == 0);
    CHECK(rec.a_flag == 1);

    CHECK(rec.b_calls == 1);
    CHECK(rec.b_pool == g_pool_marker);
    CHECK(rec.b_callable.self == guarded.object());
    CHECK(rec.b_callable.function == 0x3C7380);
    CHECK(rec.b_callable.word2 == 0 && rec.b_callable.word3 == 0);
}

struct FailureCase {
    const char* failed_at;
    Prefix prefix;
    int fail_allocate;
    bool fail_mutex;
    bool fail_register_a;
    bool fail_register_b;
    int expect_mutex_calls;
    int expect_a_calls;
    int expect_b_calls;
};

void test_failure_prefixes() {
    const FailureCase cases[] = {
        {"alloc_list_a", Prefix::kHead, 0, false, false, false, 0, 0, 0},
        {"alloc_list_b", Prefix::kNodeA, 1, false, false, false, 0, 0, 0},
        {"alloc_list_c", Prefix::kNodeB, 2, false, false, false, 0, 0, 0},
        {"mutex_init", Prefix::kNodeC, -1, true, false, false, 1, 0, 0},
        {"register_116040", Prefix::kMutex, -1, false, true, false, 1, 1, 0},
        {"register_116540", Prefix::kMutex, -1, false, false, true, 1, 1, 1},
    };
    for (const FailureCase& item : cases) {
        Guarded guarded;
        reset_pools();
        Recorder rec;
        rec.fail_allocate = item.fail_allocate;
        rec.fail_mutex = item.fail_mutex;
        rec.fail_register_a = item.fail_register_a;
        rec.fail_register_b = item.fail_register_b;
        g_rec = &rec;

        const OwnerRow18Result result =
            owner_ctor_3c5cb0(guarded.object(), g_pool_marker, make_hooks());
        g_rec = nullptr;

        CHECK(!result.ok);
        CHECK(result.failed_at != nullptr);
        CHECK(std::string(result.failed_at) == item.failed_at);
        CHECK(image_matches(guarded, item.prefix));
        CHECK(pools_match(item.prefix));
        CHECK(guarded.guards_ok());
        CHECK(rec.mutex_calls == item.expect_mutex_calls);
        CHECK(rec.a_calls == item.expect_a_calls);
        CHECK(rec.b_calls == item.expect_b_calls);
    }
}

void test_missing_hooks() {
    struct Missing {
        const char* failed_at;
        Prefix prefix;
        bool with_alloc;
        bool with_mutex;
        bool with_a;
    };
    const Missing cases[] = {
        {"alloc_list_a", Prefix::kHead, false, false, false},
        {"mutex_init", Prefix::kNodeC, true, false, false},
        {"register_116040", Prefix::kMutex, true, true, false},
        {"register_116540", Prefix::kMutex, true, true, true},
    };
    for (const Missing& item : cases) {
        Guarded guarded;
        reset_pools();
        Recorder rec;
        OwnerRow18Hooks hooks;
        if (item.with_alloc) {
            hooks.allocate = &hook_allocate;
        }
        if (item.with_mutex) {
            hooks.mutex_init = &hook_mutex;
        }
        if (item.with_a) {
            hooks.register_116040 = &hook_register_a;
        }
        // register_116540 stays null in every case where it is reached.
        g_rec = &rec;
        const OwnerRow18Result result = owner_ctor_3c5cb0(guarded.object(), g_pool_marker, hooks);
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
    const OwnerRow18Result null_object = owner_ctor_3c5cb0(nullptr, g_pool_marker, make_hooks());
    CHECK(!null_object.ok);
    CHECK(null_object.failed_at != nullptr);
    CHECK(std::string(null_object.failed_at) == "null-object");

    Guarded guarded;
    const OwnerRow18Result misaligned =
        owner_ctor_3c5cb0(guarded.object() + 1, g_pool_marker, make_hooks());
    CHECK(!misaligned.ok);
    CHECK(misaligned.failed_at != nullptr);
    CHECK(std::string(misaligned.failed_at) == "object-align");
    CHECK(rec.allocations.empty());
    CHECK(rec.mutex_calls == 0 && rec.a_calls == 0 && rec.b_calls == 0);
    CHECK(guarded.object()[0] == kFill);
    CHECK(guarded.guards_ok());

    // The pool is passed through verbatim, including null (the original does not test it).
    reset_pools();
    rec.a_pool = g_pool_marker;
    rec.b_pool = g_pool_marker;
    Guarded with_null_pool;
    CHECK(owner_ctor_3c5cb0(with_null_pool.object(), nullptr, make_hooks()).ok);
    CHECK(rec.a_pool == nullptr && rec.b_pool == nullptr);
    CHECK(image_matches(with_null_pool, Prefix::kFull));
    g_rec = nullptr;
}

void test_second_call() {
    Guarded guarded;
    reset_pools();
    Recorder first;
    g_rec = &first;
    CHECK(owner_ctor_3c5cb0(guarded.object(), g_pool_marker, make_hooks()).ok);
    g_rec = nullptr;
    const Image after_first = expected_prefix(Prefix::kFull);
    CHECK(std::memcmp(guarded.object(), after_first.bytes, kObj) == 0);

    // The body never reads `this`: a second call writes the same offsets with the same values.
    reset_pools();
    Recorder second;
    g_rec = &second;
    CHECK(owner_ctor_3c5cb0(guarded.object(), g_pool_marker, make_hooks()).ok);
    g_rec = nullptr;
    CHECK(std::memcmp(guarded.object(), after_first.bytes, kObj) == 0);
    CHECK(image_matches(guarded, Prefix::kFull));
    CHECK(first.allocations == second.allocations);
    CHECK(first.order == second.order);
    CHECK(second.a_calls == 1 && second.b_calls == 1);
    CHECK(guarded.guards_ok());
}

// ---- driver integration -----------------------------------------------------------------------

struct DriverLog {
    std::vector<std::size_t> rows;
    std::vector<void*> live;
    void* row9_object = nullptr;
    void* row18_object = nullptr;
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

// Every row except 18 is a stand-in; row 18 goes through the native adapter.
bool composite_ctor(std::size_t row, void* object, void* arg) {
    g_driver->rows.push_back(row);
    if (row == 9) {
        g_driver->row9_object = object;
    }
    if (row == 18) {
        g_driver->row18_object = object;
        return owner_row18_ctor_hook(row, object, arg);
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
        OwnerRow18Binding binding;
        binding.hooks = make_hooks();
        owner_row18_bind(&binding);

        OwnerMemberHooks hooks;
        hooks.ctor = &owner_row18_ctor_hook;

        Guarded guarded;
        reset_pools();
        CHECK(hooks.ctor(18, guarded.object(), g_pool_marker));
        CHECK(binding.calls == 1);
        CHECK(binding.last.ok);
        CHECK(binding.last_pool == g_pool_marker);
        CHECK(rec.a_pool == g_pool_marker && rec.b_pool == g_pool_marker);
        CHECK(image_matches(guarded, Prefix::kFull));

        Guarded other;
        CHECK(!hooks.ctor(17, other.object(), g_pool_marker));
        CHECK(!hooks.ctor(14, other.object(), g_pool_marker));
        CHECK(binding.calls == 1);
        CHECK(other.object()[0] == kFill);

        rec.fail_register_b = true;
        Guarded failed;
        reset_pools();
        CHECK(!hooks.ctor(18, failed.object(), g_pool_marker));
        CHECK(binding.calls == 2);
        CHECK(!binding.last.ok);
        CHECK(std::string(binding.last.failed_at) == "register_116540");

        owner_row18_bind(nullptr);
        Guarded unbound;
        CHECK(!hooks.ctor(18, unbound.object(), g_pool_marker));
        CHECK(binding.calls == 2);
        CHECK(unbound.object()[0] == kFill);
        g_rec = nullptr;
    }

    // ---- b) inside the real 21-row driver: row 18 native, every other row a stand-in ----
    {
        DriverLog log;
        g_driver = &log;
        Recorder rec;
        rec.fail_allocate = -1;
        g_rec = &rec;
        reset_pools();

        OwnerRow18Binding binding;
        binding.hooks = make_hooks();
        owner_row18_bind(&binding);

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
        CHECK(log.row9_object != nullptr && log.row18_object != nullptr);

        // Row 18's argument is the row-9 object stored into the 0x2edf140 cell, and that is the
        // pool the registrations were made on.
        CHECK(state.globals[0x2EDF140] == log.row9_object);
        CHECK(binding.calls == 1);
        CHECK(binding.last.ok);
        CHECK(binding.last_pool == log.row9_object);
        CHECK(rec.a_pool == log.row9_object && rec.b_pool == log.row9_object);
        // The ctor ran on the 0xb8 B driver allocation and the result lives at this+0x3b70.
        CHECK(state.this_fields[0x3B70] == log.row18_object);
        CHECK(rec.a_callable.self == log.row18_object);
        CHECK(rec.b_callable.self == log.row18_object);
        const auto* const bytes = static_cast<const std::uint8_t*>(log.row18_object);
        void* head = nullptr;
        std::memcpy(&head, bytes + kOwnerRow18HeadAOffset, sizeof head);
        CHECK(head == g_node_a);

        // The hook path stays available: swapping the ctor back to a plain stand-in still works.
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

        owner_row18_bind(nullptr);
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
    test_hook_arguments();
    test_failure_prefixes();
    test_missing_hooks();
    test_preconditions();
    test_second_call();
    test_adapter();
    std::printf("OK test_owner_ctor_3c5cb0: 8 sections, %zu B object, prefix + double call + driver\n",
                kObj);
    return 0;
}
