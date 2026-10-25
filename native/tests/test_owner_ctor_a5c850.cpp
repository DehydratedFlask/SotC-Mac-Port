// Fixture for the row-9 owner constructor `0xa5c850` (0xa530 B object, pointer slot 0x32f8ff0).
//
// The characterization is `local/re/evidence/owner-ctor-a5c850/report.md`; this fixture is the
// falsifiable side of it. It asserts every written byte, record, vector header and constant, the
// call order and arguments, the 24 B global copy, every failure surface, a double call, the guard
// ring, and that the object is *partially* initialized (holes keep the fill byte).
//
// Byte-exact ELF pins live in `tools/lift/check_fowner_ctor_a5c850.py`.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

#include "core/init_sub_3be930.hpp"
#include "core/owner_ctor_a5c850.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

constexpr std::size_t kObjBytes = 0xA530;
constexpr std::size_t kGuardBytes = 0x40;
constexpr std::uint8_t kFill = 0xCC;
constexpr std::uint8_t kGuardFill = 0xA5;
constexpr std::uint32_t kOneF = 0x3F800000u;

constexpr std::size_t kRecBase = 0x1C50;
constexpr std::size_t kRecLimit = 0x5C50;
constexpr std::size_t kRecStride = 0x40;
constexpr std::size_t kRecCount = 0x100;

// 0xfab6b0: {1.0f, 0, 0, 0} x 3.
constexpr std::uint32_t kQuadTable[12] = {
    kOneF, 0u, 0u, 0u, kOneF, 0u, 0u, 0u, kOneF, 0u, 0u, 0u,
};
// 0x12c49a0: 8 x 1.0f.
constexpr std::uint32_t kOnesTable[8] = {kOneF, kOneF, kOneF, kOneF, kOneF, kOneF, kOneF, kOneF};

// ---------------------------------------------------------------------------------------------
// Expected store table, transcribed from report §5 (offsets/widths are instruction-encoded there).
// 63 entries = 56 `this`-relative sites + the 7 loop sites (repeated for all 256 records).
// Group 1 is everything up to and including the 0xf180 call site; group 2 follows 0xb09650;
// group 3 is the +0xa520 store; group 4 is the 32 x 1.0f tail. The order inside a group is the
// instruction order, which is what makes a failing-prefix comparison meaningful.
// ---------------------------------------------------------------------------------------------
enum class Kind {
    kZero,
    kMinus1x32,
    kMinus1x64,
    kImm,
    kPtr,
    kQuadLo,
    kQuadHi,
    kOnesBlock,
    kOnes32,
    kOnes64,
};

struct Store {
    std::uint32_t offset;
    std::uint8_t width;
    Kind kind;
    std::uint64_t value;
    std::uint8_t group;
    bool in_loop;
};

constexpr Store kStores[] = {
    // ---- group 1: report §5.1 head scalars -------------------------------------------------
    {0x0000, 1, Kind::kZero, 0, 1, false},
    {0x0001, 1, Kind::kZero, 0, 1, false},
    {0x0002, 1, Kind::kZero, 0, 1, false},
    {0x0004, 4, Kind::kZero, 0, 1, false},
    {0x0008, 4, Kind::kMinus1x32, 0, 1, false},
    {0x0010, 1, Kind::kZero, 0, 1, false},  // one byte only (report R1)
    {0x0020, 1, Kind::kZero, 0, 1, false},
    // ---- group 1: report §5.2 vector header 1 (+0x28) --------------------------------------
    {0x0048, 8, Kind::kPtr, 0x0050, 1, false},  // inline marker
    {0x0030, 8, Kind::kPtr, 0x0050, 1, false},  // end
    {0x0028, 8, Kind::kPtr, 0x0050, 1, false},  // begin
    {0x0038, 8, Kind::kPtr, 0x1C50, 1, false},  // cap-end
    // ---- group 1: report §5.3 record fields, record-relative -------------------------------
    {0x0000, 4, Kind::kMinus1x32, 0, 1, true},
    {0x0004, 1, Kind::kZero, 0, 1, true},
    {0x0005, 1, Kind::kZero, 0, 1, true},
    {0x0006, 2, Kind::kZero, 0, 1, true},
    {0x002C, 4, Kind::kZero, 0, 1, true},
    {0x0034, 4, Kind::kZero, 0, 1, true},
    {0x0038, 8, Kind::kZero, 0, 1, true},
    // ---- group 1: report §5.4 lane group ---------------------------------------------------
    {0x5C50, 4, Kind::kZero, 0, 1, false},
    {0x5C60, 8, Kind::kZero, 0, 1, false},  // the atomic xchg
    {0x5C70, 8, Kind::kZero, 0, 1, false},
    {0x5C90, 8, Kind::kMinus1x64, 0, 1, false},
    {0x5C98, 4, Kind::kZero, 0, 1, false},
    {0x5C80, 8, Kind::kZero, 0, 1, false},
    {0x5C88, 8, Kind::kPtr, 0x5CA0, 1, false},
    {0x5C9C, 4, Kind::kImm, 0x20, 1, false},
    // ---- group 1: report §5.2 vector header 2 (+0x68a0) ------------------------------------
    {0x68C0, 8, Kind::kPtr, 0x68C8, 1, false},
    {0x68A8, 8, Kind::kPtr, 0x68C8, 1, false},
    {0x68A0, 8, Kind::kPtr, 0x68C8, 1, false},
    {0x68B0, 8, Kind::kPtr, 0x69C8, 1, false},
    // ---- group 1: report §5.5 constants ----------------------------------------------------
    {0x69C8, 4, Kind::kMinus1x32, 0, 1, false},
    {0x69DC, 32, Kind::kQuadHi, 0, 1, false},  // 48 B block 0, src+0x10
    {0x69CC, 32, Kind::kQuadLo, 0, 1, false},  // 48 B block 0, src+0x00
    {0x6A0C, 32, Kind::kQuadHi, 0, 1, false},  // 48 B block 1, src+0x10
    {0x69FC, 32, Kind::kQuadLo, 0, 1, false},  // 48 B block 1, src+0x00
    {0x6A2C, 16, Kind::kZero, 0, 1, false},
    // ---- group 1: report §5.2 vector headers 3 (+0x6ac0) and 4 (+0x6fe8) -------------------
    {0x6AE0, 8, Kind::kPtr, 0x6AE8, 1, false},
    {0x6AC8, 8, Kind::kPtr, 0x6AE8, 1, false},
    {0x6AC0, 8, Kind::kPtr, 0x6AE8, 1, false},
    {0x6AD0, 8, Kind::kPtr, 0x6FE8, 1, false},
    {0x7008, 8, Kind::kPtr, 0x7010, 1, false},
    {0x6FF0, 8, Kind::kPtr, 0x7010, 1, false},
    {0x6FE8, 8, Kind::kPtr, 0x7010, 1, false},
    {0x6FF8, 8, Kind::kPtr, 0x7250, 1, false},
    // ---- group 2: after 0xb09650 -----------------------------------------------------------
    {0x7288, 32, Kind::kZero, 0, 2, false},
    {0x72C8, 8, Kind::kPtr, 0x72D0, 2, false},
    {0x72B0, 8, Kind::kPtr, 0x72D0, 2, false},
    {0x72A8, 8, Kind::kPtr, 0x72D0, 2, false},
    {0x72B8, 8, Kind::kPtr, 0x8BD0, 2, false},
    {0x8BF0, 8, Kind::kPtr, 0x8BF8, 2, false},
    {0x8BD8, 8, Kind::kPtr, 0x8BF8, 2, false},
    {0x8BD0, 8, Kind::kPtr, 0x8BF8, 2, false},
    {0x8BE0, 8, Kind::kPtr, 0xA4F8, 2, false},
    {0xA4F8, 8, Kind::kZero, 0, 2, false},
    // ---- group 3: +0xa520, written just before the 0x11bc0 call ----------------------------
    {0xA520, 4, Kind::kZero, 0, 3, false},
    // ---- group 4: 32 x 1.0f at +0x6a3c, widths as emitted ----------------------------------
    {0x6A3C, 4, Kind::kOnes32, 0, 4, false},
    {0x6A40, 4, Kind::kOnes32, 0, 4, false},
    {0x6A44, 32, Kind::kOnesBlock, 0, 4, false},
    {0x6A64, 32, Kind::kOnesBlock, 0, 4, false},
    {0x6A84, 32, Kind::kOnesBlock, 0, 4, false},
    {0x6AA4, 8, Kind::kOnes64, 0, 4, false},
    {0x6AAC, 8, Kind::kOnes64, 0, 4, false},
    {0x6AB4, 8, Kind::kOnes64, 0, 4, false},
};

std::uint8_t g_shadow[kObjBytes];

void write_expected(std::uint8_t* shadow, const std::uint8_t* object, const Store& store,
                    std::size_t at) {
    std::uint8_t* dst = shadow + at;
    switch (store.kind) {
        case Kind::kZero:
            std::memset(dst, 0, store.width);
            break;
        case Kind::kMinus1x32: {
            const std::uint32_t value = 0xFFFFFFFFu;
            std::memcpy(dst, &value, sizeof value);
            break;
        }
        case Kind::kMinus1x64: {
            const std::uint64_t value = ~std::uint64_t{0};
            std::memcpy(dst, &value, sizeof value);
            break;
        }
        case Kind::kImm: {
            const std::uint64_t value = store.value;
            std::memcpy(dst, &value, store.width);
            break;
        }
        case Kind::kPtr: {
            const void* value = object + store.value;
            std::memcpy(dst, &value, sizeof value);
            break;
        }
        case Kind::kQuadLo:
            std::memcpy(dst, kQuadTable, 32);
            break;
        case Kind::kQuadHi:
            std::memcpy(dst, kQuadTable + 4, 32);
            break;
        case Kind::kOnesBlock:
            std::memcpy(dst, kOnesTable, 32);
            break;
        case Kind::kOnes32: {
            const std::uint32_t value = kOneF;
            std::memcpy(dst, &value, sizeof value);
            break;
        }
        case Kind::kOnes64: {
            const std::uint64_t value = 0x3F8000003F800000ull;
            std::memcpy(dst, &value, sizeof value);
            break;
        }
    }
}

void apply_group(std::uint8_t* shadow, const std::uint8_t* object, std::uint8_t group) {
    for (const Store& store : kStores) {
        if (store.group != group) {
            continue;
        }
        if (store.in_loop) {
            for (std::size_t i = 0; i < kRecCount; ++i) {
                write_expected(shadow, object, store, kRecBase + i * kRecStride + store.offset);
            }
        } else {
            write_expected(shadow, object, store, store.offset);
        }
    }
}

// Written-byte predicate, independent of the values (so the hole refill in section 8 cannot be
// confused by a pointer byte that happens to be 0xCC).
bool is_written(std::size_t offset) {
    for (const Store& store : kStores) {
        if (store.in_loop) {
            if (offset >= kRecBase && offset < kRecLimit) {
                const std::size_t in_record = (offset - kRecBase) % kRecStride;
                if (in_record >= store.offset && in_record < store.offset + store.width) {
                    return true;
                }
            }
        } else if (offset >= store.offset && offset < store.offset + store.width) {
            return true;
        }
    }
    // Hook-owned sub-objects, at their characterized widths.
    if (offset >= 0x7250 && offset < 0x7270) {
        return true;  // 0xf180: payload qword + 24 zero bytes
    }
    if (offset >= 0x7278 && offset < 0x7288) {
        return true;  // 0xb09650: node pointer at +0x7278, 0 at +0x7280
    }
    if (offset >= 0xA500 && offset < 0xA509) {
        return true;  // 0x3b3d0: qword handle + the byte at +8
    }
    if (offset >= 0xA510 && offset < 0xA519) {
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------------------------
// Recording hooks. Each models only the bytes the characterization attributes to the callee
// (report §4.1-§4.4); the ctor must not write those bytes itself.
// ---------------------------------------------------------------------------------------------
enum Call { kTree = 0, kEvent0 = 1, kEvent1 = 2, kName = 3 };

struct Recorder {
    std::uint8_t* object = nullptr;
    std::vector<int> order;
    std::vector<std::pair<std::uint8_t*, std::uint32_t>> events;
    std::uint8_t* tree_at = nullptr;
    std::string name1;
    std::string name2;
    bool create = true;
    bool null_record = false;
    bool tree_ok = true;
    int event_ok_calls = 2;  // calls that succeed inside one run before the hook starts failing
    int event_attempts = 0;
    bool use_payload = false;
    bool hook_payload = false;
    void* payload = nullptr;
    std::uint8_t record[24] = {};
    std::uint8_t node[0x88] = {};
    std::uint64_t handle0 = 0;
    std::uint64_t handle1 = 0;
};

Recorder g_rec;
std::uint8_t g_custom_payload[8] = {};

void reset_recorder(std::uint8_t* object) {
    Recorder fresh;
    fresh.object = object;
    for (std::size_t i = 0; i < sizeof fresh.record; ++i) {
        fresh.record[i] = static_cast<std::uint8_t>(0x80 + i);
    }
    for (std::size_t i = 0; i < sizeof fresh.node; ++i) {
        fresh.node[i] = static_cast<std::uint8_t>(i);
    }
    g_rec = fresh;
}

bool tree_init(void* at) {
    g_rec.order.push_back(kTree);
    g_rec.tree_at = static_cast<std::uint8_t*>(at);
    if (!g_rec.tree_ok) {
        return false;
    }
    // 0xb09650: zero +8..+0x18, then store the 0x88 B header node at +8.
    auto* bytes = static_cast<std::uint8_t*>(at);
    std::memset(bytes + 8, 0, 16);
    void* node = g_rec.node;
    std::memcpy(bytes + 8, &node, sizeof node);
    return true;
}

bool event_flag_create(void* at, std::uint32_t initial_pattern) {
    auto* bytes = static_cast<std::uint8_t*>(at);
    const bool first = (bytes == g_rec.object + 0xA500);
    g_rec.order.push_back(first ? kEvent0 : kEvent1);
    g_rec.events.emplace_back(bytes, initial_pattern);
    ++g_rec.event_attempts;
    if (g_rec.event_attempts > g_rec.event_ok_calls) {
        return false;
    }
    // 0x3b3d0: store the kernel handle at +0, then the byte at +8 is 0 for both calls.
    std::uint64_t handle = 0x4000000000000000ull + initial_pattern;
    std::memcpy(bytes, &handle, sizeof handle);
    bytes[8] = 0;
    if (first) {
        g_rec.handle0 = handle;
    } else {
        g_rec.handle1 = handle;
    }
    return true;
}

const void* name_record(const char* name1, const char* name2, bool create) {
    g_rec.order.push_back(kName);
    g_rec.name1 = (name1 != nullptr) ? name1 : "<null>";
    g_rec.name2 = (name2 != nullptr) ? name2 : "<null>";
    g_rec.create = create;
    if (g_rec.null_record) {
        return nullptr;
    }
    return g_rec.record;
}

void* null_payload() { return g_rec.payload; }

OwnerA5C850Hooks make_hooks() {
    OwnerA5C850Hooks hooks{&tree_init, &event_flag_create, &name_record, nullptr};
    if (g_rec.hook_payload) {
        hooks.null_payload = &null_payload;
    }
    return hooks;
}

struct ObjectBuffer {
    alignas(8) std::uint8_t storage[kGuardBytes + kObjBytes + kGuardBytes];

    explicit ObjectBuffer(std::uint8_t fill = kFill) { std::memset(storage, fill, sizeof storage); }

    [[nodiscard]] std::uint8_t* obj() { return storage + kGuardBytes; }
    [[nodiscard]] const std::uint8_t* obj() const { return storage + kGuardBytes; }
};

void check_guards(const ObjectBuffer& buffer, std::uint8_t fill) {
    for (std::size_t i = 0; i < kGuardBytes; ++i) {
        CHECK(buffer.storage[i] == fill);
        CHECK(buffer.storage[kGuardBytes + kObjBytes + i] == fill);
    }
}

// `limit` = completed store groups (0..4); `stage` = hook-owned regions already produced
// (0 none, 1 f180, 2 tree, 3 event flag 0, 4 event flag 1). Everything else must still hold the
// fill byte, which is exactly the original's "write a prefix, then fault" behaviour.
void build_expected(const std::uint8_t* object, std::uint8_t limit, std::uint8_t stage) {
    std::memset(g_shadow, kFill, kObjBytes);
    apply_group(g_shadow, object, 1);
    if (stage >= 1) {
        const void* payload = g_rec.use_payload ? g_rec.payload : slot_vtable_2dd9f78();
        std::memcpy(g_shadow + 0x7250, &payload, sizeof payload);
        std::memset(g_shadow + 0x7258, 0, 24);
    }
    if (stage >= 2) {
        std::memset(g_shadow + 0x7278, 0, 16);
        const void* node = g_rec.node;
        std::memcpy(g_shadow + 0x7278, &node, sizeof node);
    }
    if (stage >= 3) {
        std::memcpy(g_shadow + 0xA500, &g_rec.handle0, sizeof g_rec.handle0);
        g_shadow[0xA508] = 0;
    }
    if (stage >= 4) {
        std::memcpy(g_shadow + 0xA510, &g_rec.handle1, sizeof g_rec.handle1);
        g_shadow[0xA518] = 0;
    }
    if (limit >= 2) {
        apply_group(g_shadow, object, 2);
    }
    if (limit >= 3) {
        apply_group(g_shadow, object, 3);
    }
    if (limit >= 4) {
        apply_group(g_shadow, object, 4);
    }
}

void check_object(const std::uint8_t* object, std::uint8_t limit, std::uint8_t stage,
                  const char* what) {
    build_expected(object, limit, stage);
    if (std::memcmp(g_shadow, object, kObjBytes) == 0) {
        return;
    }
    for (std::size_t i = 0; i < kObjBytes; ++i) {
        if (g_shadow[i] != object[i]) {
            std::fprintf(stderr, "FAIL %s: first mismatch at +0x%zx: got 0x%02x want 0x%02x\n",
                         what, i, object[i], g_shadow[i]);
            break;
        }
    }
    CHECK(false);
}

void run(ObjectBuffer& buffer, std::uint8_t* sound, const OwnerA5C850Hooks& hooks,
         OwnerA5C850Result* out = nullptr) {
    g_rec.event_attempts = 0;  // the failure budget is per run, not per recorder lifetime
    const OwnerA5C850Result result = owner_ctor_a5c850(buffer.obj(), sound, hooks);
    if (out != nullptr) {
        *out = result;
    }
}

// ---------------------------------------------------------------------------------------------
// 1. Exact-bytes skeleton: successful run, full-object comparison against the store table.
// ---------------------------------------------------------------------------------------------
void section_skeleton() {
    ObjectBuffer buffer;
    std::uint8_t sound[32];
    std::memset(sound, 0x5A, sizeof sound);
    reset_recorder(buffer.obj());

    OwnerA5C850Result result{};
    run(buffer, sound, make_hooks(), &result);
    CHECK(result.ok);
    CHECK(result.failed_at == nullptr);
    CHECK(g_rec.order.size() == 4);

    check_object(buffer.obj(), 4, 4, "skeleton");
    check_guards(buffer, kFill);

    // Report §5.6: nobody writes the tree header's own first qword.
    for (std::size_t i = 0x7270; i < 0x7278; ++i) {
        CHECK(buffer.obj()[i] == kFill);
    }
    // Report §8: +0xa524..+0xa530 is a 12 B hole.
    for (std::size_t i = 0xA524; i < kObjBytes; ++i) {
        CHECK(buffer.obj()[i] == kFill);
    }
    std::printf("PASS 1 exact-bytes skeleton (%zu store entries, %zu sites + 7 loop sites)\n",
                std::size(kStores), std::size(kStores) - 7);
}

// ---------------------------------------------------------------------------------------------
// 2. Record array: 256 records, 7 partial fields, holes preserved, last record at +0x5c10.
// ---------------------------------------------------------------------------------------------
void section_records() {
    ObjectBuffer buffer;
    std::uint8_t sound[32];
    reset_recorder(buffer.obj());
    run(buffer, sound, make_hooks());

    const std::uint8_t* obj = buffer.obj();
    const std::size_t before = kRecBase;
    // +0x50..+0x1c50 is the first vector's inline buffer and must stay untouched.
    for (std::size_t i = 0x50; i < before; ++i) {
        CHECK(obj[i] == kFill);
    }

    for (std::size_t i = 0; i < kRecCount; ++i) {
        const std::size_t r = kRecBase + i * kRecStride;
        std::uint32_t head = 0;
        std::memcpy(&head, obj + r, sizeof head);
        CHECK(head == 0xFFFFFFFFu);
        CHECK(obj[r + 0x04] == 0);
        CHECK(obj[r + 0x05] == 0);
        CHECK(obj[r + 0x06] == 0);
        CHECK(obj[r + 0x07] == 0);
        std::uint32_t field = 1;
        std::memcpy(&field, obj + r + 0x2C, sizeof field);
        CHECK(field == 0);
        std::memcpy(&field, obj + r + 0x34, sizeof field);
        CHECK(field == 0);
        std::uint64_t tail = 1;
        std::memcpy(&tail, obj + r + 0x38, sizeof tail);
        CHECK(tail == 0);
        // Holes: +0x08..+0x2c and +0x30..+0x34. (`+0x3c..+0x3f` is *written* by the qword at
        // +0x38; the report's hole census line for those bytes contradicts its own store table.)
        for (std::size_t h = 0x08; h < 0x2C; ++h) {
            CHECK(obj[r + h] == kFill);
        }
        for (std::size_t h = 0x30; h < 0x34; ++h) {
            CHECK(obj[r + h] == kFill);
        }
    }

    const std::size_t last = kRecBase + (kRecCount - 1) * kRecStride;
    CHECK(last == 0x5C10);
    CHECK(last + kRecStride == kRecLimit);
    std::uint32_t group = 1;
    std::memcpy(&group, obj + kRecLimit, sizeof group);
    CHECK(group == 0);
    std::printf("PASS 2 record array: 256 records at +0x1c50 stride 0x40, holes intact\n");
}

// ---------------------------------------------------------------------------------------------
// 3. Vector header table.
// ---------------------------------------------------------------------------------------------
void section_vectors() {
    CHECK(kOwnerA5C850VecCount == 6);
    const std::size_t expected_headers[6] = {0x28, 0x68A0, 0x6AC0, 0x6FE8, 0x72A8, 0x8BD0};
    const std::size_t expected_counts[6] = {128, 0, 32, 8, 32, 32};
    for (std::size_t i = 0; i < kOwnerA5C850VecCount; ++i) {
        CHECK(kOwnerA5C850Vectors[i].header == expected_headers[i]);
        CHECK(kOwnerA5C850Vectors[i].header + 0x28 == kOwnerA5C850Vectors[i].buffer);
        if (kOwnerA5C850Vectors[i].elem != 0) {
            CHECK((kOwnerA5C850Vectors[i].cap_end - kOwnerA5C850Vectors[i].buffer) /
                      kOwnerA5C850Vectors[i].elem ==
                  expected_counts[i]);
        } else {
            CHECK(i == 1);  // only +0x68a0 has an unproven element size (report R3)
        }
    }

    ObjectBuffer buffer;
    std::uint8_t sound[32];
    reset_recorder(buffer.obj());
    run(buffer, sound, make_hooks());
    const std::uint8_t* obj = buffer.obj();

    for (const OwnerA5C850Vector& vector : kOwnerA5C850Vectors) {
        const void* begin = nullptr;
        const void* end = nullptr;
        const void* cap_end = nullptr;
        const void* marker = nullptr;
        std::memcpy(&begin, obj + vector.header + 0x00, sizeof begin);
        std::memcpy(&end, obj + vector.header + 0x08, sizeof end);
        std::memcpy(&cap_end, obj + vector.header + 0x10, sizeof cap_end);
        std::memcpy(&marker, obj + vector.header + 0x20, sizeof marker);
        CHECK(begin == obj + vector.buffer);
        CHECK(end == obj + vector.buffer);
        CHECK(marker == obj + vector.buffer);
        CHECK(cap_end == obj + vector.cap_end);
        // The fourth slot is never written.
        for (std::size_t i = 0; i < 8; ++i) {
            CHECK(obj[vector.header + 0x18 + i] == kFill);
        }
        // Inline buffer: no element is constructed, no byte is touched.
        for (std::size_t i = 0; i < vector.cap_end - vector.buffer; ++i) {
            CHECK(obj[vector.buffer + i] == kFill);
        }
    }
    std::printf("PASS 3 six vector headers: begin==end==marker, cap-end, +0x18 slot and buffers "
                "untouched\n");
}

// ---------------------------------------------------------------------------------------------
// 4. Constants.
// ---------------------------------------------------------------------------------------------
void section_constants() {
    ObjectBuffer buffer;
    std::uint8_t sound[32];
    reset_recorder(buffer.obj());
    run(buffer, sound, make_hooks());
    const std::uint8_t* obj = buffer.obj();

    std::int32_t neg_one = 0;
    std::memcpy(&neg_one, obj + 0x69C8, sizeof neg_one);
    CHECK(neg_one == -1);

    std::uint32_t quad[12] = {};
    std::memcpy(quad, obj + 0x69CC, 48);
    CHECK(std::memcmp(quad, kQuadTable, 48) == 0);
    std::memcpy(quad, obj + 0x69FC, 48);
    CHECK(std::memcmp(quad, kQuadTable, 48) == 0);

    for (std::size_t i = 0; i < 16; ++i) {
        CHECK(obj[0x6A2C + i] == 0);
    }

    std::uint32_t ones[32] = {};
    std::memcpy(ones, obj + 0x6A3C, sizeof ones);
    for (std::uint32_t value : ones) {
        CHECK(value == kOneF);  // uint32 compare, not float
    }

    std::uint64_t all_ones = 0;
    std::memcpy(&all_ones, obj + 0x5C90, sizeof all_ones);
    CHECK(all_ones == ~std::uint64_t{0});
    std::uint32_t lane_count = 0;
    std::memcpy(&lane_count, obj + 0x5C9C, sizeof lane_count);
    CHECK(lane_count == 0x20);
    const void* lane_base = nullptr;
    std::memcpy(&lane_base, obj + 0x5C88, sizeof lane_base);
    CHECK(lane_base == obj + 0x5CA0);

    const std::uint64_t zeros[3] = {0, 0, 0};
    CHECK(std::memcmp(obj + 0x5C60, zeros, 8) == 0);
    CHECK(std::memcmp(obj + 0x5C70, zeros, 8) == 0);
    CHECK(std::memcmp(obj + 0x5C80, zeros, 8) == 0);
    CHECK(std::memcmp(obj + 0x5C50, zeros, 4) == 0);
    CHECK(std::memcmp(obj + 0x5C98, zeros, 4) == 0);
    CHECK(std::memcmp(obj + 0xA4F8, zeros, 8) == 0);
    CHECK(std::memcmp(obj + 0xA520, zeros, 4) == 0);

    std::printf("PASS 4 constants: -1 head/record/cap values, 2x48 B quad, 16 B zero, 32x1.0f, "
                "lane group\n");
}

// ---------------------------------------------------------------------------------------------
// 5. Call order, arguments and the 0xf180 handle.
// ---------------------------------------------------------------------------------------------
void section_calls() {
    ObjectBuffer buffer;
    std::uint8_t sound[32];
    reset_recorder(buffer.obj());
    run(buffer, sound, make_hooks());

    CHECK(g_rec.order.size() == 4);
    CHECK(g_rec.order[0] == kTree);
    CHECK(g_rec.order[1] == kEvent0);
    CHECK(g_rec.order[2] == kEvent1);
    CHECK(g_rec.order[3] == kName);
    CHECK(g_rec.tree_at == buffer.obj() + 0x7270);
    CHECK(g_rec.events.size() == 2);
    CHECK(g_rec.events[0].first == buffer.obj() + 0xA500);
    CHECK(g_rec.events[0].second == 0);
    CHECK(g_rec.events[1].first == buffer.obj() + 0xA510);
    CHECK(g_rec.events[1].second == 1);  // the only difference between the two flags
    CHECK(g_rec.name1 == "Sound");
    CHECK(g_rec.name2 == "Sound");
    CHECK(!g_rec.create);

    const void* payload = nullptr;
    std::memcpy(&payload, buffer.obj() + 0x7250, sizeof payload);
    CHECK(payload == slot_vtable_2dd9f78());
    const std::uint64_t zeros[3] = {0, 0, 0};
    CHECK(std::memcmp(buffer.obj() + 0x7258, zeros, 24) == 0);

    // A supplied stand-in replaces only the payload qword.
    ObjectBuffer second;
    reset_recorder(second.obj());
    g_rec.use_payload = true;
    g_rec.hook_payload = true;
    g_rec.payload = g_custom_payload;
    run(second, sound, make_hooks());
    std::memcpy(&payload, second.obj() + 0x7250, sizeof payload);
    CHECK(payload == g_custom_payload);
    CHECK(std::memcmp(second.obj() + 0x7258, zeros, 24) == 0);

    std::printf("PASS 5 call order/args: tree(+0x7270), event(+0xa500,0), event(+0xa510,1), "
                "name(\"Sound\",\"Sound\",false)\n");
}

// ---------------------------------------------------------------------------------------------
// 6. 24 B global record copy.
// ---------------------------------------------------------------------------------------------
void section_global_copy() {
    ObjectBuffer buffer;
    std::uint8_t sound[32];
    std::memset(sound, 0x5A, sizeof sound);
    reset_recorder(buffer.obj());
    run(buffer, sound, make_hooks());

    CHECK(std::memcmp(sound, g_rec.record, 24) == 0);
    for (std::size_t i = 24; i < sizeof sound; ++i) {
        CHECK(sound[i] == 0x5A);  // exactly 24 B, no more
    }
    // The id survives verbatim as the low dword (record[i] = 0x80 + i).
    std::uint32_t id = 0;
    std::memcpy(&id, sound, sizeof id);
    CHECK(id == 0x83828180u);
    std::printf("PASS 6 24 B global copy is byte-identical, destination guard intact\n");
}

// ---------------------------------------------------------------------------------------------
// 7. Failure surfaces: visible step, exact prefix, no fabricated record.
// ---------------------------------------------------------------------------------------------
void section_failures() {
    std::uint8_t sound[32];
    std::memset(sound, 0x5A, sizeof sound);

    // (a) null object
    {
        reset_recorder(nullptr);
        const OwnerA5C850Result result = owner_ctor_a5c850(nullptr, sound, make_hooks());
        CHECK(!result.ok);
        CHECK(std::strcmp(result.failed_at, "null-object") == 0);
        CHECK(g_rec.order.empty());
    }
    // (b) null sound record: nothing is written at all
    {
        ObjectBuffer buffer;
        reset_recorder(buffer.obj());
        const OwnerA5C850Result result = owner_ctor_a5c850(buffer.obj(), nullptr, make_hooks());
        CHECK(!result.ok);
        CHECK(std::strcmp(result.failed_at, "sound-record") == 0);
        CHECK(g_rec.order.empty());
        for (std::size_t i = 0; i < kObjBytes; ++i) {
            CHECK(buffer.obj()[i] == kFill);
        }
    }
    // (c) misaligned object: native precondition, nothing written
    {
        ObjectBuffer buffer;
        reset_recorder(buffer.obj());
        const OwnerA5C850Result result =
            owner_ctor_a5c850(buffer.storage + 1, sound, make_hooks());
        CHECK(!result.ok);
        CHECK(std::strcmp(result.failed_at, "object-align") == 0);
        CHECK(g_rec.order.empty());
        for (std::size_t i = 0; i < sizeof buffer.storage; ++i) {
            CHECK(buffer.storage[i] == kFill);
        }
    }
    // (d) tree hook missing / failing: group 1 + 0xf180 only
    for (int mode = 0; mode < 2; ++mode) {
        ObjectBuffer buffer;
        reset_recorder(buffer.obj());
        OwnerA5C850Hooks hooks = make_hooks();
        if (mode == 0) {
            hooks.tree_init = nullptr;
        } else {
            g_rec.tree_ok = false;
        }
        const OwnerA5C850Result result = owner_ctor_a5c850(buffer.obj(), sound, hooks);
        CHECK(!result.ok);
        CHECK(std::strcmp(result.failed_at, "tree_init") == 0);
        check_object(buffer.obj(), 1, 1, "tree_init");
        // The failed callee fabricated nothing: +0x7278..+0x7288 is still the fill.
        for (std::size_t i = 0x7278; i < 0x7288; ++i) {
            CHECK(buffer.obj()[i] == kFill);
        }
    }
    // (e) event flag 0 missing / failing: group 1 + tree, no event bytes
    for (int mode = 0; mode < 2; ++mode) {
        ObjectBuffer buffer;
        reset_recorder(buffer.obj());
        OwnerA5C850Hooks hooks = make_hooks();
        if (mode == 0) {
            hooks.event_flag_create = nullptr;
        } else {
            g_rec.event_ok_calls = 0;
        }
        const OwnerA5C850Result result = owner_ctor_a5c850(buffer.obj(), sound, hooks);
        CHECK(!result.ok);
        CHECK(std::strcmp(result.failed_at, "event_flag_0") == 0);
        check_object(buffer.obj(), 2, 2, "event_flag_0");
        for (std::size_t i = 0xA500; i < 0xA520; ++i) {
            CHECK(buffer.obj()[i] == kFill);
        }
    }
    // (f) the second event flag fails: flag 0 exists, flag 1 does not
    {
        ObjectBuffer buffer;
        reset_recorder(buffer.obj());
        g_rec.event_ok_calls = 1;
        const OwnerA5C850Result result = owner_ctor_a5c850(buffer.obj(), sound, make_hooks());
        CHECK(!result.ok);
        CHECK(std::strcmp(result.failed_at, "event_flag_1") == 0);
        check_object(buffer.obj(), 2, 3, "event_flag_1");
        for (std::size_t i = 0xA510; i < 0xA520; ++i) {
            CHECK(buffer.obj()[i] == kFill);
        }
    }
    // (g) name lookup missing / not found with create == false
    for (int mode = 0; mode < 2; ++mode) {
        ObjectBuffer buffer;
        reset_recorder(buffer.obj());
        OwnerA5C850Hooks hooks = make_hooks();
        if (mode == 0) {
            hooks.name_record = nullptr;
        } else {
            g_rec.null_record = true;
        }
        const OwnerA5C850Result result = owner_ctor_a5c850(buffer.obj(), sound, hooks);
        CHECK(!result.ok);
        CHECK(std::strcmp(result.failed_at, "name_record") == 0);
        check_object(buffer.obj(), 3, 4, "name_record");
        if (mode == 1) {
            CHECK(!g_rec.create);  // the hook still saw create == false
        }
        // Nothing of the tail 1.0f block was written and the global copy stayed at its fill.
        for (std::size_t i = 0x6A3C; i < 0x6ABC; ++i) {
            CHECK(buffer.obj()[i] == kFill);
        }
        for (std::size_t i = 0; i < sizeof sound; ++i) {
            CHECK(sound[i] == 0x5A);
        }
    }
    // (h) null payload stand-in: nothing of the 0xf180 block is written
    {
        ObjectBuffer buffer;
        reset_recorder(buffer.obj());
        g_rec.use_payload = true;
        g_rec.hook_payload = true;
        g_rec.payload = nullptr;
        const OwnerA5C850Result result = owner_ctor_a5c850(buffer.obj(), sound, make_hooks());
        CHECK(!result.ok);
        CHECK(std::strcmp(result.failed_at, "null_payload") == 0);
        check_object(buffer.obj(), 1, 0, "null_payload");
        for (std::size_t i = 0x7250; i < 0x7288; ++i) {
            CHECK(buffer.obj()[i] == kFill);
        }
    }

    std::printf("PASS 7 failure surfaces: 11 cases, exact prefix, no record fabricated\n");
}

// ---------------------------------------------------------------------------------------------
// 8. Double call: identical written bytes, hooks repeated, holes untouched, guards intact.
// ---------------------------------------------------------------------------------------------
void section_double_call() {
    ObjectBuffer buffer;
    std::uint8_t sound[32];
    reset_recorder(buffer.obj());
    run(buffer, sound, make_hooks());
    check_object(buffer.obj(), 4, 4, "double-call first");

    std::vector<std::uint8_t> first(buffer.obj(), buffer.obj() + kObjBytes);

    // Refill every hole with a distinct byte: the ctor must not read it back into a written field
    // and must not write it.
    for (std::size_t i = 0; i < kObjBytes; ++i) {
        if (!is_written(i)) {
            buffer.obj()[i] = 0x5A;
        }
    }

    run(buffer, sound, make_hooks());  // second call, same buffer, same hooks
    CHECK(g_rec.order.size() == 8);    // 4 + 4: the ctor is not re-entrant-safe (report R5)
    for (std::size_t i = 0; i < kObjBytes; ++i) {
        if (is_written(i)) {
            CHECK(buffer.obj()[i] == first[i]);  // content-independent: the body never reads `this`
        } else {
            CHECK(buffer.obj()[i] == 0x5A);
        }
    }
    check_guards(buffer, kFill);
    std::printf("PASS 8 double call: identical written bytes, 8 hook calls, holes stay 0x5a\n");
}

// ---------------------------------------------------------------------------------------------
// 9. Guard ring: 0x40 bytes of 0xa5 on both sides.
// ---------------------------------------------------------------------------------------------
void section_guard_ring() {
    ObjectBuffer buffer(kGuardFill);
    std::uint8_t sound[32];
    reset_recorder(buffer.obj());
    run(buffer, sound, make_hooks());
    check_guards(buffer, kGuardFill);
    // The object itself was written; the ring was not (an off-by-one at +0xa520 or a memset of the
    // object size would trip here).
    CHECK(buffer.obj()[0] == 0);
    CHECK(buffer.obj()[0xA523] == 0);
    CHECK(buffer.obj()[0xA524] == kGuardFill);
    std::printf("PASS 9 guard ring: 0x40 bytes 0xa5 before and after unchanged\n");
}

}  // namespace

int main() {
    CHECK(kOwnerCtorA5C850ObjSize == kObjBytes);
    CHECK(kOwnerA5C850RecCount == kRecCount);
    CHECK(kOwnerA5C850RecStride == kRecStride);
    CHECK(kOwnerA5C850RecBase == kRecBase);
    CHECK(kOwnerA5C850RecLimit == kRecLimit);
    CHECK(std::size(kStores) == 63);

    std::size_t in_loop = 0;
    for (const Store& store : kStores) {
        if (store.in_loop) {
            ++in_loop;
        }
    }
    CHECK(in_loop == 7);
    CHECK(std::size(kStores) - in_loop == 56);  // report §5: 56 this-relative store sites

    section_skeleton();
    section_records();
    section_vectors();
    section_constants();
    section_calls();
    section_global_copy();
    section_failures();
    section_double_call();
    section_guard_ring();
    std::printf("ALL 9 SECTIONS PASSED\n");
    return 0;
}
