// Fixture for the row-9 owner destructor `0xa5cb30` (the `kDtorThenFree` half of row 10).
//
// The byte characterization is in `local/re/evidence/owner-dtor-a5cb30-native/report.md`; this
// fixture is the falsifiable side of it. It builds the object with the real `owner_ctor_a5c850`
// lift, records the dtor's step order and every pointer it hands to a callee, and checks the
// partial-initialization contract (holes, ctor prefixes, armed vectors, the `+0x20` gate).
//
// Byte-exact ELF pins live in `tools/lift/check_fowner_dtor_a5cb30.py`.
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include "core/owner_ctor_a5c850.hpp"
#include "core/owner_dtor_a5cb30.hpp"
#include "core/shared_release_f590.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

constexpr std::size_t kObjBytes = 0xA530;
constexpr std::size_t kGuardBytes = 0x40;
constexpr std::uint8_t kFill = 0xCC;
constexpr std::uint8_t kGuardFill = 0xA5;
constexpr std::size_t kEvent0 = 0xA500;
constexpr std::size_t kEvent1 = 0xA510;

std::uint64_t load_u64(const void* address) {
    std::uint64_t value = 0;
    std::memcpy(&value, address, sizeof value);
    return value;
}

void store_ptr(void* address, const void* value) {
    std::memcpy(address, &value, sizeof value);
}

void* load_ptr(const void* address) {
    void* value = nullptr;
    std::memcpy(&value, address, sizeof value);
    return value;
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

// ---------------------------------------------------------------------------------------------
// Constructor recorder: the real ctor lift, with a selectable failure stage for prefix objects.
// ---------------------------------------------------------------------------------------------
struct CtorRec {
    std::vector<void*> event_at;
    std::vector<std::uint32_t> event_pattern;
    int fail_event = -1;  // index into the two event_flag_create calls
    bool fail_tree = false;
    bool fail_name = false;
    bool null_name = false;
    int event_calls = 0;
    int tree_calls = 0;
    int name_calls = 0;
    void* tree_at = nullptr;
    std::uint64_t handles[2] = {0xE000000000000010ull, 0xE000000000000011ull};
    std::uint8_t node[0x88] = {};
    std::uint8_t sound[24] = {};
};

CtorRec g_ctor;
std::uint8_t* g_object = nullptr;

void reset_ctor_rec() {
    CtorRec fresh;
    for (std::size_t i = 0; i < sizeof fresh.node; ++i) {
        fresh.node[i] = static_cast<std::uint8_t>(i);
    }
    for (std::size_t i = 0; i < sizeof fresh.sound; ++i) {
        fresh.sound[i] = static_cast<std::uint8_t>(0x80 + i);
    }
    g_ctor = fresh;
}

bool ctor_tree_init(void* at) {
    ++g_ctor.tree_calls;
    g_ctor.tree_at = at;
    if (g_ctor.fail_tree) {
        return false;
    }
    auto* const bytes = static_cast<std::uint8_t*>(at);
    std::memset(bytes + 8, 0, 16);
    void* const node = g_ctor.node;
    std::memcpy(bytes + 8, &node, sizeof node);
    return true;
}

bool ctor_event_create(void* at, std::uint32_t pattern) {
    const int index = g_ctor.event_calls++;
    g_ctor.event_at.push_back(at);
    g_ctor.event_pattern.push_back(pattern);
    if (index == g_ctor.fail_event) {
        return false;
    }
    const std::uint64_t handle = g_ctor.handles[index == 0 ? 0 : 1];
    std::memcpy(at, &handle, sizeof handle);
    static_cast<std::uint8_t*>(at)[8] = 0;
    return true;
}

const void* ctor_name_record(const char*, const char*, bool) {
    ++g_ctor.name_calls;
    if (g_ctor.fail_name || g_ctor.null_name) {
        return nullptr;
    }
    return g_ctor.sound;
}

OwnerA5C850Hooks ctor_hooks() {
    return {&ctor_tree_init, &ctor_event_create, &ctor_name_record, nullptr};
}

OwnerA5C850Result construct(ObjectBuffer& buffer) {
    std::uint8_t sound[32] = {};
    g_object = buffer.obj();
    reset_ctor_rec();
    return owner_ctor_a5c850(buffer.obj(), sound, ctor_hooks());
}

// ---------------------------------------------------------------------------------------------
// Destructor recorder. Step identity is what the order assertions compare; the payload records
// every pointer the body reads or hands to a callee.
// ---------------------------------------------------------------------------------------------
enum class Step : int {
    Event1,
    Event0,
    PolyRelease,
    Elem8BD0,
    Free8BD0,
    Elem72A8,
    Free72A8,
    TreeDestroy,
    Shared7250,
    Shared6FE8Hi,
    Shared6FE8Lo,
    Free6FE8,
    Shared6AC0,
    Free6AC0,
    Free68A0,
    Drain,
    Shared28,
    RefCond28,
    RefRelease28,
    Free28,
    HeadCond,
    HeadRelease,
};

const char* step_name(Step step) {
    switch (step) {
        case Step::Event1: return "Event1";
        case Step::Event0: return "Event0";
        case Step::PolyRelease: return "PolyRelease";
        case Step::Elem8BD0: return "Elem8BD0";
        case Step::Free8BD0: return "Free8BD0";
        case Step::Elem72A8: return "Elem72A8";
        case Step::Free72A8: return "Free72A8";
        case Step::TreeDestroy: return "TreeDestroy";
        case Step::Shared7250: return "Shared7250";
        case Step::Shared6FE8Hi: return "Shared6FE8Hi";
        case Step::Shared6FE8Lo: return "Shared6FE8Lo";
        case Step::Free6FE8: return "Free6FE8";
        case Step::Shared6AC0: return "Shared6AC0";
        case Step::Free6AC0: return "Free6AC0";
        case Step::Free68A0: return "Free68A0";
        case Step::Drain: return "Drain";
        case Step::Shared28: return "Shared28";
        case Step::RefCond28: return "RefCond28";
        case Step::RefRelease28: return "RefRelease28";
        case Step::Free28: return "Free28";
        case Step::HeadCond: return "HeadCond";
        case Step::HeadRelease: return "HeadRelease";
    }
    return "<bad>";
}

struct DtorCall {
    Step step;
    const void* ptr;
    const void* vptr;
};

struct DtorRec {
    std::vector<DtorCall> calls;
    std::vector<std::uint64_t> event_handles;
    std::vector<void*> tree_nodes;
    std::vector<std::pair<void*, void*>> shared_holders;
    std::vector<void*> shared_destroyed;
    std::vector<void*> shared_freed;
    bool fail_enabled = false;
    Step fail_step = Step::Event1;
};

DtorRec g_dtor;

// Armed fixture storage. No real allocation: the dtor never dereferences a vector buffer itself;
// it only forwards the pointers to the hooks, and the hooks model the guest callees.
struct alignas(8) ArmStore {
    std::uint8_t vec8bd0[2 * kOwnerA5C850StrideC8] = {};
    std::uint8_t vec72a8[1 * kOwnerA5C850StrideC8] = {};
    std::uint8_t vec6fe8[1 * kOwnerA5C850Stride48] = {};
    std::uint8_t vec6ac0[2 * kOwnerA5C850Stride28] = {};
    std::uint8_t vec68a0[0x100] = {};
    std::uint8_t vec28[1 * kOwnerA5C850Stride38] = {};
    std::uint8_t poly[0x20] = {};
    std::uint8_t vec28_target[0x20] = {};
    std::uint8_t head_target[0x20] = {};
    std::uint64_t drain_flag = 0;
    std::uint64_t fake_vtable[2] = {0, 0};
};

ArmStore g_arm;
std::atomic<std::uint64_t> g_controls[5];

void reset_dtor_rec() {
    DtorRec fresh;
    g_dtor = fresh;
}

bool fail_step(Step step) {
    return g_dtor.fail_enabled && g_dtor.fail_step == step;
}

bool in_range(const void* pointer, const void* base, std::size_t size) {
    const auto p = reinterpret_cast<std::uintptr_t>(pointer);
    const auto b = reinterpret_cast<std::uintptr_t>(base);
    return p >= b && p < b + size;
}

bool dtor_event_flag(void* flag) {
    auto* const bytes = static_cast<std::uint8_t*>(flag);
    const Step step = (bytes == g_object + kEvent1) ? Step::Event1 : Step::Event0;
    CHECK(bytes == g_object + kEvent0 || bytes == g_object + kEvent1);
    g_dtor.calls.push_back({step, flag, nullptr});
    g_dtor.event_handles.push_back(load_u64(flag));
    return !fail_step(step);
}

bool dtor_poly_release(void* object, void* const* vptr) {
    g_dtor.calls.push_back({Step::PolyRelease, object, vptr});
    return !fail_step(Step::PolyRelease);
}

bool dtor_element_release(void* element, void* const* vptr) {
    Step step = Step::Elem8BD0;
    if (in_range(element, g_arm.vec72a8, sizeof g_arm.vec72a8)) {
        step = Step::Elem72A8;
    } else {
        CHECK(in_range(element, g_arm.vec8bd0, sizeof g_arm.vec8bd0));
    }
    g_dtor.calls.push_back({step, element, vptr});
    return !fail_step(step);
}

bool dtor_tree_destroy(void* at) {
    auto* const bytes = static_cast<std::uint8_t*>(at);
    void* const node = load_ptr(bytes + 8);
    g_dtor.calls.push_back({Step::TreeDestroy, at, nullptr});
    g_dtor.tree_nodes.push_back(node);
    if (fail_step(Step::TreeDestroy)) {
        return false;
    }
    if (node == nullptr) {
        // A ctor prefix that faulted before 0xb09650: the guest 0xb09690 would dereference null.
        return false;
    }
    // 0xb0a080: self-link the header node, then [at+0x10] = 0 (0xb09690 frees [at+8]).
    std::memcpy(node, &node, sizeof node);
    auto* const node_bytes = static_cast<std::uint8_t*>(node);
    std::memcpy(node_bytes + 8, &node, sizeof node);
    std::memcpy(node_bytes + 0x10, &node, sizeof node);
    void* const zero = nullptr;
    std::memcpy(bytes + 0x10, &zero, sizeof zero);
    return true;
}

void shared_destroy(void* object, int) { g_dtor.shared_destroyed.push_back(object); }
void shared_free_control(void* control) { g_dtor.shared_freed.push_back(control); }

bool dtor_shared_release(void* holder_ptr) {
    Step step = Step::Shared7250;
    if (in_range(holder_ptr, g_arm.vec6fe8, sizeof g_arm.vec6fe8)) {
        step = (static_cast<std::uint8_t*>(holder_ptr) - g_arm.vec6fe8 == 0x28)
                   ? Step::Shared6FE8Hi
                   : Step::Shared6FE8Lo;
    } else if (in_range(holder_ptr, g_arm.vec6ac0, sizeof g_arm.vec6ac0)) {
        step = Step::Shared6AC0;
    } else if (in_range(holder_ptr, g_arm.vec28, sizeof g_arm.vec28)) {
        step = Step::Shared28;
    } else {
        CHECK(holder_ptr == g_object + kOwnerA5C850SharedOffset);
    }
    g_dtor.calls.push_back({step, holder_ptr, nullptr});
    if (fail_step(step)) {
        return false;
    }
    auto* const holder = static_cast<SharedHolderF590*>(holder_ptr);
    g_dtor.shared_holders.emplace_back(holder_ptr, holder->control);
    const SharedReleaseF590Hooks nested{&shared_destroy, &shared_free_control};
    const SharedReleaseF590Result result = shared_release_f590(*holder, nested);
    return result.ok;
}

bool dtor_buffer_free(void* buffer) {
    Step step = Step::Free8BD0;
    if (in_range(buffer, g_arm.vec8bd0, sizeof g_arm.vec8bd0)) {
        step = Step::Free8BD0;
    } else if (in_range(buffer, g_arm.vec72a8, sizeof g_arm.vec72a8)) {
        step = Step::Free72A8;
    } else if (in_range(buffer, g_arm.vec6fe8, sizeof g_arm.vec6fe8)) {
        step = Step::Free6FE8;
    } else if (in_range(buffer, g_arm.vec6ac0, sizeof g_arm.vec6ac0)) {
        step = Step::Free6AC0;
    } else if (in_range(buffer, g_arm.vec68a0, sizeof g_arm.vec68a0)) {
        step = Step::Free68A0;
    } else if (in_range(buffer, g_arm.vec28, sizeof g_arm.vec28)) {
        step = Step::Free28;
    } else {
        CHECK(false);
    }
    g_dtor.calls.push_back({step, buffer, nullptr});
    return !fail_step(step);
}

bool dtor_drain(void* base) {
    g_dtor.calls.push_back({Step::Drain, base, nullptr});
    return !fail_step(Step::Drain);
}

bool dtor_ref_cond(void* object) {
    const Step step = (object == g_arm.head_target) ? Step::HeadCond : Step::RefCond28;
    CHECK(object == g_arm.head_target || object == g_arm.vec28_target);
    g_dtor.calls.push_back({step, object, nullptr});
    return !fail_step(step);
}

bool dtor_ref_release(void* object) {
    const Step step = (object == g_arm.head_target) ? Step::HeadRelease : Step::RefRelease28;
    CHECK(object == g_arm.head_target || object == g_arm.vec28_target);
    g_dtor.calls.push_back({step, object, nullptr});
    return !fail_step(step);
}

OwnerDtorA5CB30Hooks full_hooks() {
    OwnerDtorA5CB30Hooks hooks{};
    hooks.event_flag_destroy = &dtor_event_flag;
    hooks.poly_release = &dtor_poly_release;
    hooks.element_release = &dtor_element_release;
    hooks.tree_destroy = &dtor_tree_destroy;
    hooks.shared_release = &dtor_shared_release;
    hooks.buffer_free = &dtor_buffer_free;
    hooks.drain_armed = &dtor_drain;
    hooks.ref_release_conditional = &dtor_ref_cond;
    hooks.ref_release = &dtor_ref_release;
    return hooks;
}

OwnerDtorA5CB30Result run_dtor(ObjectBuffer& buffer, const OwnerDtorA5CB30Hooks& hooks) {
    g_object = buffer.obj();
    g_dtor.calls.clear();
    g_dtor.event_handles.clear();
    g_dtor.tree_nodes.clear();
    g_dtor.shared_holders.clear();
    g_dtor.shared_destroyed.clear();
    g_dtor.shared_freed.clear();
    return owner_dtor_a5cb30(buffer.obj(), hooks);
}

void expect_steps(const std::vector<Step>& expected, const char* what) {
    if (g_dtor.calls.size() != expected.size()) {
        std::fprintf(stderr, "FAIL %s: %zu calls, want %zu:", what, g_dtor.calls.size(),
                     expected.size());
        for (const DtorCall& call : g_dtor.calls) {
            std::fprintf(stderr, " %s", step_name(call.step));
        }
        std::fprintf(stderr, "\n");
        CHECK(false);
    }
    for (std::size_t i = 0; i < expected.size(); ++i) {
        if (g_dtor.calls[i].step != expected[i]) {
            std::fprintf(stderr, "FAIL %s: step %zu is %s, want %s\n", what, i,
                         step_name(g_dtor.calls[i].step), step_name(expected[i]));
            CHECK(false);
        }
    }
}

// ---------------------------------------------------------------------------------------------
// Arming: put real-shaped pointers into a constructed object so every branch of the dtor runs.
// The buffers are fixture storage; the dtor only forwards them to the hooks.
// ---------------------------------------------------------------------------------------------
void arm_object(ObjectBuffer& buffer) {
    g_arm = ArmStore{};
    g_controls[0].store(2, std::memory_order_relaxed);  // +0x6fe8 element, holder +0x28
    g_controls[1].store(1, std::memory_order_relaxed);  // +0x6fe8 element, holder +0x00
    g_controls[2].store(1, std::memory_order_relaxed);  // +0x6ac0 element 0
    g_controls[3].store(2, std::memory_order_relaxed);  // +0x6ac0 element 1
    g_controls[4].store(1, std::memory_order_relaxed);  // +0x28 element

    auto* const o = buffer.obj();

    // +0xa4f8 polymorphic pointer: object -> fake vtable.
    store_ptr(o + kOwnerA5C850PolyOffset, g_arm.poly);
    store_ptr(g_arm.poly, g_arm.fake_vtable);

    // +0x8bd0: two elements, heap begin (marker field stays at object+0x8bf8).
    store_ptr(o + 0x8BD0 + 0x00, g_arm.vec8bd0);
    store_ptr(o + 0x8BD0 + 0x08, g_arm.vec8bd0 + sizeof g_arm.vec8bd0);
    for (std::size_t i = 0; i < 2; ++i) {
        store_ptr(g_arm.vec8bd0 + i * kOwnerA5C850StrideC8, g_arm.fake_vtable);
    }

    // +0x72a8: one element.
    store_ptr(o + 0x72A8 + 0x00, g_arm.vec72a8);
    store_ptr(o + 0x72A8 + 0x08, g_arm.vec72a8 + sizeof g_arm.vec72a8);
    store_ptr(g_arm.vec72a8, g_arm.fake_vtable);

    // +0x6fe8: one element with holders at +0x00 and +0x28.
    store_ptr(o + 0x6FE8 + 0x00, g_arm.vec6fe8);
    store_ptr(o + 0x6FE8 + 0x08, g_arm.vec6fe8 + sizeof g_arm.vec6fe8);
    for (std::size_t holder = 0; holder < 2; ++holder) {
        auto* const at = g_arm.vec6fe8 + holder * 0x28;
        store_ptr(at + 0x00, at);  // SharedHolderF590::object
        store_ptr(at + 0x18, &g_controls[holder]);
    }

    // +0x6ac0: two elements with one holder each.
    store_ptr(o + 0x6AC0 + 0x00, g_arm.vec6ac0);
    store_ptr(o + 0x6AC0 + 0x08, g_arm.vec6ac0 + sizeof g_arm.vec6ac0);
    for (std::size_t i = 0; i < 2; ++i) {
        auto* const at = g_arm.vec6ac0 + i * kOwnerA5C850Stride28;
        store_ptr(at + 0x00, at);
        store_ptr(at + 0x18, &g_controls[2 + i]);
    }

    // +0x68a0: heap POD buffer (no element loop).
    store_ptr(o + 0x68A0 + 0x00, g_arm.vec68a0);
    store_ptr(o + 0x68A0 + 0x08, g_arm.vec68a0 + sizeof g_arm.vec68a0);

    // +0x28: one element, shared holder at +0x18, ref pointer at +0x00, flag at +0x08.
    store_ptr(o + 0x28 + 0x00, g_arm.vec28);
    store_ptr(o + 0x28 + 0x08, g_arm.vec28 + sizeof g_arm.vec28);
    store_ptr(g_arm.vec28 + 0x00, g_arm.vec28_target);
    g_arm.vec28[0x08] = 1;
    store_ptr(g_arm.vec28 + 0x18, g_arm.vec28_target);
    store_ptr(g_arm.vec28 + 0x18 + 0x18, &g_controls[4]);

    // Head gate: +0x20 armed, +0x18 armed, +0x10 a full qword (the ctor only wrote one byte).
    store_ptr(o + kOwnerA5C850HeadPtrOffset, g_arm.head_target);
    o[kOwnerA5C850HeadFlagOffset] = 1;
    o[kOwnerA5C850HeadGateOffset] = 1;

    // Armed drain word.
    store_ptr(o + kOwnerA5C850LaneAtomicOffset + kOwnerA5C850DrainWordOffset,
              &g_arm.drain_flag);
}

// ---------------------------------------------------------------------------------------------
// 1. A fully constructed object: exact order, every pointer, byte-identical teardown, holes.
// ---------------------------------------------------------------------------------------------
void section_constructed() {
    ObjectBuffer buffer;
    const OwnerA5C850Result ctor = construct(buffer);
    CHECK(ctor.ok);

    const std::vector<std::uint8_t> before(buffer.obj(), buffer.obj() + kObjBytes);
    reset_dtor_rec();
    const OwnerDtorA5CB30Result dtor = run_dtor(buffer, full_hooks());
    CHECK(dtor.ok);
    CHECK(dtor.failed_at == nullptr);
    CHECK(dtor.event_flags == 2);
    CHECK(dtor.polymorphic_releases == 0);
    CHECK(dtor.element_releases == 0);
    CHECK(dtor.tree_destroys == 1);
    CHECK(dtor.shared_releases == 1);
    CHECK(dtor.buffers_freed == 0);
    CHECK(dtor.ref_conditionals == 0);
    CHECK(dtor.ref_releases == 0);
    CHECK(dtor.drain_ran);

    expect_steps({Step::Event1, Step::Event0, Step::TreeDestroy, Step::Shared7250},
                 "constructed order");
    CHECK(g_dtor.calls[0].ptr == buffer.obj() + kEvent1);
    CHECK(g_dtor.calls[1].ptr == buffer.obj() + kEvent0);
    CHECK(g_dtor.calls[2].ptr == buffer.obj() + kOwnerA5C850TreeOffset);
    CHECK(g_dtor.calls[3].ptr == buffer.obj() + kOwnerA5C850SharedOffset);
    CHECK(g_dtor.event_handles.size() == 2);
    CHECK(g_dtor.event_handles[0] == g_ctor.handles[1]);  // +0xa510 first
    CHECK(g_dtor.event_handles[1] == g_ctor.handles[0]);
    CHECK(g_dtor.tree_nodes.size() == 1);
    CHECK(g_dtor.tree_nodes[0] == g_ctor.node);
    CHECK(g_dtor.shared_holders.size() == 1);
    CHECK(g_dtor.shared_holders[0].first == buffer.obj() + kOwnerA5C850SharedOffset);
    CHECK(g_dtor.shared_holders[0].second == nullptr);  // 0xf180 left an empty holder

    // The dtor only rewrites values the ctor already set, and the pattern-1 tree clear rewrites
    // object+0x7280 to 0. Every byte of the constructed object is therefore unchanged, including
    // every hole.
    CHECK(std::memcmp(buffer.obj(), before.data(), kObjBytes) == 0);
    check_guards(buffer, kFill);

    // Report section 8 holes that this body must not touch.
    for (std::size_t i = 0x000C; i < 0x0010; ++i) CHECK(buffer.obj()[i] == kFill);
    for (std::size_t i = 0x0011; i < 0x0018; ++i) CHECK(buffer.obj()[i] == kFill);
    for (std::size_t i = 0x0019; i < 0x0020; ++i) CHECK(buffer.obj()[i] == kFill);
    for (std::size_t i = 0x0021; i < 0x0028; ++i) CHECK(buffer.obj()[i] == kFill);
    for (const OwnerA5C850Vector& vector : kOwnerA5C850Vectors) {
        for (std::size_t i = 0; i < 8; ++i) {
            CHECK(buffer.obj()[vector.header + 0x18 + i] == kFill);
        }
    }
    for (std::size_t i = 0x5CA0; i < 0x68A0; ++i) CHECK(buffer.obj()[i] == kFill);
    for (std::size_t i = 0x7270; i < 0x7278; ++i) CHECK(buffer.obj()[i] == kFill);
    for (std::size_t i = 0xA524; i < kObjBytes; ++i) CHECK(buffer.obj()[i] == kFill);
    // +0x10 was a one-byte store; its upper seven bytes were never written and are not read back
    // because +0x20 is 0.
    CHECK(buffer.obj()[0x10] == 0);
    std::printf("PASS 1 constructed teardown: event(+0xa510,+0xa500), tree, shared, drain; "
                "byte-identical, holes intact\n");
}

// ---------------------------------------------------------------------------------------------
// 2. Armed object: every vector/ref/free branch, exact order and every pointer.
// ---------------------------------------------------------------------------------------------
void section_armed() {
    ObjectBuffer buffer;
    CHECK(construct(buffer).ok);
    arm_object(buffer);

    reset_dtor_rec();
    const OwnerDtorA5CB30Result dtor = run_dtor(buffer, full_hooks());
    CHECK(dtor.ok);
    CHECK(dtor.failed_at == nullptr);
    CHECK(dtor.event_flags == 2);
    CHECK(dtor.polymorphic_releases == 1);
    CHECK(dtor.element_releases == 3);  // 2 at +0x8bd0, 1 at +0x72a8
    CHECK(dtor.tree_destroys == 1);
    CHECK(dtor.shared_releases == 6);  // +0x7250, 2 at +0x6fe8, 2 at +0x6ac0, 1 at +0x28
    CHECK(dtor.buffers_freed == 6);    // all six armed heaps
    CHECK(dtor.ref_conditionals == 2); // +0x28 element and head
    CHECK(dtor.ref_releases == 2);
    CHECK(dtor.drain_ran);

    expect_steps({Step::Event1, Step::Event0, Step::PolyRelease,
                  Step::Elem8BD0, Step::Elem8BD0, Step::Free8BD0,
                  Step::Elem72A8, Step::Free72A8,
                  Step::TreeDestroy, Step::Shared7250,
                  Step::Shared6FE8Hi, Step::Shared6FE8Lo, Step::Free6FE8,
                  Step::Shared6AC0, Step::Shared6AC0, Step::Free6AC0,
                  Step::Free68A0, Step::Drain,
                  Step::Shared28, Step::RefCond28, Step::RefRelease28, Step::Free28,
                  Step::HeadCond, Step::HeadRelease},
                 "armed order");

    const void* const expected_ptr[] = {
        buffer.obj() + kEvent1,
        buffer.obj() + kEvent0,
        g_arm.poly,
        g_arm.vec8bd0 + 0 * kOwnerA5C850StrideC8,
        g_arm.vec8bd0 + 1 * kOwnerA5C850StrideC8,
        g_arm.vec8bd0,
        g_arm.vec72a8,
        g_arm.vec72a8,
        buffer.obj() + kOwnerA5C850TreeOffset,
        buffer.obj() + kOwnerA5C850SharedOffset,
        g_arm.vec6fe8 + 0x28,
        g_arm.vec6fe8 + 0x00,
        g_arm.vec6fe8,
        g_arm.vec6ac0 + 0 * kOwnerA5C850Stride28,
        g_arm.vec6ac0 + 1 * kOwnerA5C850Stride28,
        g_arm.vec6ac0,
        g_arm.vec68a0,
        buffer.obj() + kOwnerA5C850LaneAtomicOffset,
        g_arm.vec28 + 0x18,
        g_arm.vec28_target,
        g_arm.vec28_target,
        g_arm.vec28,
        g_arm.head_target,
        g_arm.head_target,
    };
    static_assert(std::size(expected_ptr) == 24);
    for (std::size_t i = 0; i < std::size(expected_ptr); ++i) {
        CHECK(g_dtor.calls[i].ptr == expected_ptr[i]);
    }
    CHECK(g_dtor.calls[2].vptr == g_arm.fake_vtable);
    CHECK(g_dtor.calls[3].vptr == g_arm.fake_vtable);
    CHECK(g_dtor.calls[4].vptr == g_arm.fake_vtable);
    CHECK(g_dtor.calls[6].vptr == g_arm.fake_vtable);

    // +0xa4f8 cleared; +0x28 element pointer and flag cleared; head pointer and flag cleared.
    CHECK(load_ptr(buffer.obj() + kOwnerA5C850PolyOffset) == nullptr);
    CHECK(load_ptr(g_arm.vec28) == nullptr);
    CHECK(g_arm.vec28[0x08] == 0);
    CHECK(load_ptr(buffer.obj() + kOwnerA5C850HeadPtrOffset) == nullptr);
    CHECK(buffer.obj()[kOwnerA5C850HeadFlagOffset] == 0);

    // Shared release controls: those armed with a count of 1 destroy and free exactly once.
    CHECK(g_dtor.shared_destroyed.size() == 3);
    CHECK(g_dtor.shared_freed.size() == 3);
    CHECK(g_dtor.shared_destroyed[0] == g_arm.vec6fe8 + 0x28);
    CHECK(g_dtor.shared_destroyed[1] == g_arm.vec6ac0 + 0x00);
    CHECK(g_dtor.shared_destroyed[2] == g_arm.vec28_target);
    CHECK(g_dtor.shared_freed[0] == &g_controls[1]);
    CHECK(g_dtor.shared_freed[1] == &g_controls[2]);
    CHECK(g_dtor.shared_freed[2] == &g_controls[4]);
    CHECK(g_controls[0].load(std::memory_order_relaxed) == 1);
    CHECK(g_controls[3].load(std::memory_order_relaxed) == 1);

    check_guards(buffer, kFill);
    std::printf("PASS 2 armed teardown: 24 steps, 6 shared releases, 6 buffers, 3 last-ref "
                "destroys, every pointer exact\n");
}

// ---------------------------------------------------------------------------------------------
// 3. Span guard: a non-multiple begin/end is a visible failure, not an overrun.
// ---------------------------------------------------------------------------------------------
void section_span() {
    ObjectBuffer buffer;
    CHECK(construct(buffer).ok);
    store_ptr(buffer.obj() + 0x8BD0 + 0x00, g_arm.vec8bd0);
    store_ptr(buffer.obj() + 0x8BD0 + 0x08, g_arm.vec8bd0 + 0x65);  // not a 0xc8 multiple

    reset_dtor_rec();
    const OwnerDtorA5CB30Result dtor = run_dtor(buffer, full_hooks());
    CHECK(!dtor.ok);
    CHECK(std::strcmp(dtor.failed_at, "vector_8bd0_span") == 0);
    expect_steps({Step::Event1, Step::Event0}, "span prefix");
    CHECK(dtor.element_releases == 0);
    CHECK(dtor.buffers_freed == 0);

    // Null begin with a non-null end: same guard, never a free or a dereference.
    ObjectBuffer second;
    CHECK(construct(second).ok);
    store_ptr(second.obj() + 0x8BD0 + 0x00, nullptr);
    store_ptr(second.obj() + 0x8BD0 + 0x08, g_arm.vec8bd0 + 0xC8);
    reset_dtor_rec();
    const OwnerDtorA5CB30Result null_dtor = run_dtor(second, full_hooks());
    CHECK(!null_dtor.ok);
    CHECK(std::strcmp(null_dtor.failed_at, "vector_8bd0_span") == 0);

    // Native preconditions: null and misaligned objects are refused before any read.
    reset_dtor_rec();
    const OwnerDtorA5CB30Result null_object = owner_dtor_a5cb30(nullptr, full_hooks());
    CHECK(!null_object.ok);
    CHECK(std::strcmp(null_object.failed_at, "null-object") == 0);
    alignas(8) std::uint8_t misaligned[16] = {};
    const OwnerDtorA5CB30Result bad_align = owner_dtor_a5cb30(misaligned + 1, full_hooks());
    CHECK(!bad_align.ok);
    CHECK(std::strcmp(bad_align.failed_at, "object-align") == 0);
    std::printf("PASS 3 span/precondition guards: non-multiple span, null begin, null object, "
                "misaligned object\n");
}

// ---------------------------------------------------------------------------------------------
// 4. Missing hooks: every one of the nine callees reports its own failed_at.
// ---------------------------------------------------------------------------------------------
void section_missing_hooks() {
    auto missing = [](int which) {
        ObjectBuffer buffer;
        CHECK(construct(buffer).ok);
        OwnerDtorA5CB30Hooks hooks = full_hooks();
        switch (which) {
            case 0: hooks.event_flag_destroy = nullptr; break;
            case 1:
                arm_object(buffer);
                hooks.poly_release = nullptr;
                break;
            case 2:
                store_ptr(buffer.obj() + 0x8BD0 + 0x00, g_arm.vec8bd0);
                store_ptr(buffer.obj() + 0x8BD0 + 0x08, g_arm.vec8bd0 + 0xC8);
                store_ptr(g_arm.vec8bd0, g_arm.fake_vtable);
                hooks.element_release = nullptr;
                break;
            case 3:
                store_ptr(buffer.obj() + 0x8BD0 + 0x00, g_arm.vec8bd0);
                store_ptr(buffer.obj() + 0x8BD0 + 0x08, g_arm.vec8bd0 + 0xC8);
                store_ptr(g_arm.vec8bd0, g_arm.fake_vtable);
                hooks.buffer_free = nullptr;
                break;
            case 4: hooks.tree_destroy = nullptr; break;
            case 5: hooks.shared_release = nullptr; break;
            case 6:
                store_ptr(buffer.obj() + kOwnerA5C850LaneAtomicOffset +
                              kOwnerA5C850DrainWordOffset,
                          &g_arm.drain_flag);
                hooks.drain_armed = nullptr;
                break;
            case 7:
                store_ptr(buffer.obj() + kOwnerA5C850HeadPtrOffset, g_arm.head_target);
                buffer.obj()[kOwnerA5C850HeadFlagOffset] = 1;
                buffer.obj()[kOwnerA5C850HeadGateOffset] = 1;
                hooks.ref_release_conditional = nullptr;
                break;
            case 8:
                store_ptr(buffer.obj() + kOwnerA5C850HeadPtrOffset, g_arm.head_target);
                buffer.obj()[kOwnerA5C850HeadFlagOffset] = 0;
                buffer.obj()[kOwnerA5C850HeadGateOffset] = 1;
                hooks.ref_release = nullptr;
                break;
            default: CHECK(false);
        }
        reset_dtor_rec();
        const OwnerDtorA5CB30Result result = run_dtor(buffer, hooks);
        CHECK(!result.ok);
        return result;
    };

    struct Case {
        int which;
        const char* failed_at;
    };
    const Case cases[] = {
        {0, "event_flag_1"},        {1, "poly_release"},
        {2, "vector_8bd0_element"}, {3, "vector_8bd0_buffer"},
        {4, "tree_destroy"},        {5, "shared_7250"},
        {6, "drain"},               {7, "head_conditional"},
        {8, "head_release"},
    };
    for (const Case& item : cases) {
        const OwnerDtorA5CB30Result result = missing(item.which);
        if (std::strcmp(result.failed_at, item.failed_at) != 0) {
            std::fprintf(stderr, "FAIL missing hook %d: failed_at %s, want %s\n", item.which,
                         result.failed_at, item.failed_at);
            CHECK(false);
        }
    }
    std::printf("PASS 4 missing hooks: 9 callees each report a visible failed_at\n");
}

// ---------------------------------------------------------------------------------------------
// 5. Failing hooks: a hook that returns false stops at the same visible step.
// ---------------------------------------------------------------------------------------------
void section_failing_hooks() {
    struct Case {
        Step step;
        const char* failed_at;
        bool arm;
    };
    const Case cases[] = {
        {Step::Event1, "event_flag_1", false},
        {Step::PolyRelease, "poly_release", true},
        {Step::TreeDestroy, "tree_destroy", false},
        {Step::Shared7250, "shared_7250", false},
        {Step::Drain, "drain", true},
    };
    for (const Case& item : cases) {
        ObjectBuffer buffer;
        CHECK(construct(buffer).ok);
        OwnerDtorA5CB30Hooks hooks = full_hooks();
        if (item.arm) {
            arm_object(buffer);
            if (item.step == Step::Drain) {
                // arm_object already sets the drain word; keep it.
            }
        }
        reset_dtor_rec();
        g_dtor.fail_enabled = true;
        g_dtor.fail_step = item.step;
        const OwnerDtorA5CB30Result result = run_dtor(buffer, hooks);
        CHECK(!result.ok);
        if (std::strcmp(result.failed_at, item.failed_at) != 0) {
            std::fprintf(stderr, "FAIL hook failure %s: failed_at %s\n", item.failed_at,
                         result.failed_at);
            CHECK(false);
        }
        CHECK(g_dtor.calls.back().step == item.step);
    }
    reset_dtor_rec();
    std::printf("PASS 5 failing hooks: event/poly/tree/shared/drain failures are visible\n");
}

// ---------------------------------------------------------------------------------------------
// 6. Ctor prefixes: a faulted ctor leaves a partially initialized object. The dtor releases only
//    what exists; the pre-tree prefix is reported, because the guest tree dtor would deref null.
// ---------------------------------------------------------------------------------------------
void section_prefixes() {
    // (a) ctor fails at the first event flag: zero-filled allocation, no kernel handles.
    {
        ObjectBuffer buffer(0x00);
        g_object = buffer.obj();
        reset_ctor_rec();
        g_ctor.fail_event = 0;
        std::uint8_t sound[24] = {};
        const OwnerA5C850Result ctor = owner_ctor_a5c850(buffer.obj(), sound, ctor_hooks());
        CHECK(!ctor.ok);
        CHECK(std::strcmp(ctor.failed_at, "event_flag_0") == 0);

        reset_dtor_rec();
        const OwnerDtorA5CB30Result dtor = run_dtor(buffer, full_hooks());
        CHECK(dtor.ok);
        CHECK(dtor.event_flags == 2);
        CHECK(dtor.tree_destroys == 1);
        CHECK(dtor.shared_releases == 1);
        CHECK(dtor.buffers_freed == 0);
        CHECK(g_dtor.event_handles[0] == 0);  // +0xa510 never created
        CHECK(g_dtor.event_handles[1] == 0);  // +0xa500 never created
        expect_steps({Step::Event1, Step::Event0, Step::TreeDestroy, Step::Shared7250},
                     "prefix event0");
        for (std::size_t i = 0xA500; i < 0xA520; ++i) CHECK(buffer.obj()[i] == 0);
        for (std::size_t i = 0xA524; i < kObjBytes; ++i) CHECK(buffer.obj()[i] == 0);
    }
    // (b) ctor fails at the second event flag: +0xa500 exists, +0xa510 does not.
    {
        ObjectBuffer buffer(0x00);
        g_object = buffer.obj();
        reset_ctor_rec();
        g_ctor.fail_event = 1;
        std::uint8_t sound[24] = {};
        const OwnerA5C850Result ctor = owner_ctor_a5c850(buffer.obj(), sound, ctor_hooks());
        CHECK(!ctor.ok);
        CHECK(std::strcmp(ctor.failed_at, "event_flag_1") == 0);

        reset_dtor_rec();
        const OwnerDtorA5CB30Result dtor = run_dtor(buffer, full_hooks());
        CHECK(dtor.ok);
        CHECK(g_dtor.event_handles[0] == 0);
        CHECK(g_dtor.event_handles[1] == g_ctor.handles[0]);
        expect_steps({Step::Event1, Step::Event0, Step::TreeDestroy, Step::Shared7250},
                     "prefix event1");
    }
    // (c) ctor fails at the name lookup: both flags exist, the 32x1.0f tail does not.
    {
        ObjectBuffer buffer(0x00);
        g_object = buffer.obj();
        reset_ctor_rec();
        g_ctor.fail_name = true;
        std::uint8_t sound[24] = {};
        const OwnerA5C850Result ctor = owner_ctor_a5c850(buffer.obj(), sound, ctor_hooks());
        CHECK(!ctor.ok);
        CHECK(std::strcmp(ctor.failed_at, "name_record") == 0);

        reset_dtor_rec();
        const OwnerDtorA5CB30Result dtor = run_dtor(buffer, full_hooks());
        CHECK(dtor.ok);
        CHECK(g_dtor.event_handles[0] == g_ctor.handles[1]);
        CHECK(g_dtor.event_handles[1] == g_ctor.handles[0]);
        expect_steps({Step::Event1, Step::Event0, Step::TreeDestroy, Step::Shared7250},
                     "prefix name");
        for (std::size_t i = 0x6A3C; i < 0x6ABC; ++i) CHECK(buffer.obj()[i] == 0);
        for (std::size_t i = 0xA524; i < kObjBytes; ++i) CHECK(buffer.obj()[i] == 0);
    }
    // (d) ctor fails at the tree: no node exists. The dtor must not invent one or crash.
    {
        ObjectBuffer buffer(0x00);
        g_object = buffer.obj();
        reset_ctor_rec();
        g_ctor.fail_tree = true;
        std::uint8_t sound[24] = {};
        const OwnerA5C850Result ctor = owner_ctor_a5c850(buffer.obj(), sound, ctor_hooks());
        CHECK(!ctor.ok);
        CHECK(std::strcmp(ctor.failed_at, "tree_init") == 0);

        reset_dtor_rec();
        const OwnerDtorA5CB30Result dtor = run_dtor(buffer, full_hooks());
        CHECK(!dtor.ok);
        CHECK(std::strcmp(dtor.failed_at, "tree_destroy") == 0);
        expect_steps({Step::Event1, Step::Event0, Step::TreeDestroy}, "prefix tree");
        CHECK(g_dtor.tree_nodes.size() == 1);
        CHECK(g_dtor.tree_nodes[0] == nullptr);
    }
    std::printf("PASS 6 ctor prefixes: event0/event1/name prefixes tear down; pre-tree prefix is "
                "a visible tree_destroy failure\n");
}

// ---------------------------------------------------------------------------------------------
// 7. Double destroy: the original is not idempotent. Kernel deletes and the tree free repeat;
//    +0xa4f8 is cleared so its release does not.
// ---------------------------------------------------------------------------------------------
void section_double_destroy() {
    ObjectBuffer buffer;
    CHECK(construct(buffer).ok);
    reset_dtor_rec();
    CHECK(run_dtor(buffer, full_hooks()).ok);
    const std::vector<std::uint8_t> after_first(buffer.obj(), buffer.obj() + kObjBytes);
    const std::vector<std::uint64_t> first_handles = g_dtor.event_handles;
    const std::vector<void*> first_nodes = g_dtor.tree_nodes;

    reset_dtor_rec();
    const OwnerDtorA5CB30Result second = run_dtor(buffer, full_hooks());
    CHECK(second.ok);
    expect_steps({Step::Event1, Step::Event0, Step::TreeDestroy, Step::Shared7250},
                 "double destroy");
    CHECK(g_dtor.event_handles == first_handles);
    CHECK(g_dtor.tree_nodes == first_nodes);
    CHECK(std::memcmp(buffer.obj(), after_first.data(), kObjBytes) == 0);
    check_guards(buffer, kFill);
    std::printf("PASS 7 double destroy: event deletes and tree free repeat, +0xa4f8 stays clear, "
                "bytes unchanged\n");
}

// ---------------------------------------------------------------------------------------------
// 8. Guard ring on a constructed object.
// ---------------------------------------------------------------------------------------------
void section_guard() {
    ObjectBuffer buffer(kGuardFill);
    CHECK(construct(buffer).ok);
    reset_dtor_rec();
    CHECK(run_dtor(buffer, full_hooks()).ok);
    check_guards(buffer, kGuardFill);
    std::printf("PASS 8 guard ring: 0x40 bytes 0xa5 on both sides unchanged\n");
}

}  // namespace

int main() {
    CHECK(kOwnerCtorA5C850ObjSize == kObjBytes);
    CHECK(kOwnerDtorA5CB30Rva == kOwnerCtorA5C850Rva + kOwnerCtorA5C850Size + 15);
    CHECK(kOwnerDtorA5CB30Size == 0x252);
    CHECK(kOwnerDtorA5CB30InstrCount == 142);
    CHECK(kOwnerDtorA5CB30CallCount == 22);
    CHECK(kOwnerDtorA5CB30DirectCallCount == 19);

    section_constructed();
    section_armed();
    section_span();
    section_missing_hooks();
    section_failing_hooks();
    section_prefixes();
    section_double_destroy();
    section_guard();
    std::printf("ALL 8 SECTIONS PASSED\n");
    return 0;
}
