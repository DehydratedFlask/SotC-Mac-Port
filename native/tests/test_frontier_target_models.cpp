// Fixture tests for the four frontier-audit target bodies (0x47860, 0x31900, 0x318f0, 0x31520) and
// the 0x319c0 wait they reach. FIXTURE_VERIFIED only; the ELF-backed facts live in
// tools/lift/check_frontier_target_models.py.
#include "../core/frontier_318f0_fios_succeeded.hpp"
#include "../core/frontier_31520_fios_close.hpp"
#include "../core/frontier_31900_fios_poll.hpp"
#include "../core/frontier_319c0_fios_wait.hpp"
#include "../core/frontier_47860_locked_store.hpp"
#include "../core/vtable_15953f8_slot18_31310.hpp"

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string_view>
#include <vector>

namespace {

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

struct alignas(16) Object {
    std::array<std::uint8_t, 0x100> bytes{};
    std::uint8_t* data() { return bytes.data(); }
    template <typename T> void put(std::uint32_t off, T v) { std::memcpy(bytes.data() + off, &v, sizeof(T)); }
    template <typename T> T get(std::uint32_t off) const {
        T v{};
        std::memcpy(&v, bytes.data() + off, sizeof(T));
        return v;
    }
};

// ---------------------------------------------------------------------------------------- 0x318f0
void test_318f0() {
    using namespace sotc;
    check("318f0 constants", kFrontier318f0Rva == 0x318f0 && kFrontier318f0Size == 8 &&
                                 kFrontier318f0SlotAddress == 0x15954a8 && kFrontier318f0ErrorOffset == 0x5c);
    Object o;
    Frontier318f0Cells cells{o.data()};
    auto r = frontier_318f0_fios_succeeded(cells);
    check("318f0 zero error is success", r.ok && r.value && r.error == 0);
    o.put<std::int32_t>(0x5c, 1);
    r = frontier_318f0_fios_succeeded(cells);
    check("318f0 error 1 fails", r.ok && !r.value && r.error == 1);
    o.put<std::int32_t>(0x5c, -5);
    r = frontier_318f0_fios_succeeded(cells);
    check("318f0 negative error fails", r.ok && !r.value && r.error == -5);
    // only the dword at +0x5c matters
    o.put<std::int32_t>(0x5c, 0);
    o.put<std::uint32_t>(0x58, 0xdeadbeef);
    o.put<std::uint32_t>(0x60, 0xdeadbeef);
    r = frontier_318f0_fios_succeeded(cells);
    check("318f0 neighbours ignored", r.ok && r.value);
    const auto before = o.bytes;
    (void)frontier_318f0_fios_succeeded(cells);
    check("318f0 no writes", before == o.bytes);
    check("318f0 null object fails", !frontier_318f0_fios_succeeded({}).ok);
}

// ---------------------------------------------------------------------------------------- 0x31520
struct CloseTrace {
    int waits = 0;
    int closes = 0;
    std::uint64_t attr = 99;
    std::uint32_t handle = 0;
    std::vector<int> order;
    std::uint32_t set_mode_on_wait = 0;
    bool set_mode = false;
};
CloseTrace g_close;
void close_wait(void* object, void*) {
    ++g_close.waits;
    g_close.order.push_back(1);
    if (g_close.set_mode) {
        std::memcpy(static_cast<std::uint8_t*>(object) + 8, &g_close.set_mode_on_wait, 4);
    }
}
void close_sync(std::uint64_t attr, std::uint32_t handle, void*) {
    ++g_close.closes;
    g_close.order.push_back(2);
    g_close.attr = attr;
    g_close.handle = handle;
}

void test_31520() {
    using namespace sotc;
    check("31520 constants", kFrontier31520Rva == 0x31520 && kFrontier31520Size == 0x38 &&
                                 kFrontier31520SlotAddress == 0x1595420 &&
                                 kFrontier31520WaitVtableOffset == 0xb0 &&
                                 kFrontier31520CloseSyncPlt == 0xf8f660);
    Frontier31520Hooks hooks{};
    hooks.wait = close_wait;
    hooks.close_sync = close_sync;

    // open: wait, then close handle, then zero handle dword and the 8-byte mode qword
    g_close = CloseTrace{};
    Object o;
    o.put<std::uint32_t>(8, 1);
    o.put<std::uint32_t>(0xc, 0xabcd1234);  // cleared by the qword store
    o.put<std::uint32_t>(0x10, 77);
    o.put<std::uint64_t>(0x18, 0x1122334455667788ull);  // untouched
    auto r = frontier_31520_fios_close({o.data()}, hooks);
    check("31520 open ok", r.ok && r.wait_called && r.closed && r.closed_handle == 77 && !r.missing_hook);
    check("31520 attr is null", g_close.attr == 0 && g_close.handle == 77);
    check("31520 wait before close", g_close.order == std::vector<int>({1, 2}));
    check("31520 handle zeroed", o.get<std::uint32_t>(0x10) == 0);
    check("31520 mode qword zeroed", o.get<std::uint64_t>(8) == 0);
    check("31520 cursor untouched", o.get<std::uint64_t>(0x18) == 0x1122334455667788ull);

    // the compare is a dword: a high dword in the qword does not matter
    g_close = CloseTrace{};
    Object hi;
    hi.put<std::uint64_t>(8, 0x0000000500000001ull);
    hi.put<std::uint32_t>(0x10, 9);
    r = frontier_31520_fios_close({hi.data()}, hooks);
    check("31520 dword compare", r.ok && r.closed && g_close.handle == 9 && hi.get<std::uint64_t>(8) == 0);

    // not open: wait only
    for (std::uint32_t mode : {0u, 2u, 0xffffffffu}) {
        g_close = CloseTrace{};
        Object n;
        n.put<std::uint32_t>(8, mode);
        n.put<std::uint32_t>(0x10, 5);
        const auto before = n.bytes;
        r = frontier_31520_fios_close({n.data()}, hooks);
        check("31520 not open waits only", r.ok && r.wait_called && !r.closed && g_close.closes == 0 &&
                                               before == n.bytes);
    }

    // the mode is re-read after the virtual wait
    g_close = CloseTrace{};
    g_close.set_mode = true;
    g_close.set_mode_on_wait = 1;
    Object late;
    late.put<std::uint32_t>(0x10, 3);
    r = frontier_31520_fios_close({late.data()}, hooks);
    check("31520 mode re-read after wait", r.ok && r.closed && g_close.handle == 3);
    g_close = CloseTrace{};
    g_close.set_mode = true;
    g_close.set_mode_on_wait = 0;
    Object gone;
    gone.put<std::uint32_t>(8, 1);
    r = frontier_31520_fios_close({gone.data()}, hooks);
    check("31520 wait can clear mode", r.ok && !r.closed && g_close.closes == 0);

    // fail-visible hooks
    g_close = CloseTrace{};
    Object m;
    m.put<std::uint32_t>(8, 1);
    m.put<std::uint32_t>(0x10, 4);
    const auto snapshot = m.bytes;
    Frontier31520Hooks no_wait{};
    no_wait.close_sync = close_sync;
    r = frontier_31520_fios_close({m.data()}, no_wait);
    check("31520 missing wait fails untouched", !r.ok && r.missing_hook != nullptr &&
                                                    std::string_view(r.missing_hook) == "wait" &&
                                                    !r.wait_called && snapshot == m.bytes && g_close.closes == 0);
    Frontier31520Hooks no_close{};
    no_close.wait = close_wait;
    r = frontier_31520_fios_close({m.data()}, no_close);
    check("31520 missing close fails untouched", !r.ok && std::string_view(r.missing_hook) == "close_sync" &&
                                                     r.wait_called && !r.closed && snapshot == m.bytes);
    check("31520 null object fails", !frontier_31520_fios_close({}, hooks).ok);
}

// ---------------------------------------------------------------------------------------- 0x319c0
struct WaitTrace {
    int polls = 0;
    int pending = 0;
    std::vector<std::uint32_t> handles;
    int tails = 0;
    bool tail_value = true;
    std::vector<int> order;
};
WaitTrace g_wait;
bool wait_poll(void*, void*) {
    ++g_wait.polls;
    g_wait.order.push_back(1);
    return g_wait.polls > g_wait.pending;
}
void wait_wait(std::uint32_t h, void*) {
    g_wait.handles.push_back(h);
    g_wait.order.push_back(2);
}
bool wait_tail(void*, void*) {
    ++g_wait.tails;
    g_wait.order.push_back(3);
    return g_wait.tail_value;
}

void test_319c0() {
    using namespace sotc;
    check("319c0 constants", kFrontier319c0Rva == 0x319c0 && kFrontier319c0Size == 0x3a &&
                                 kFrontier319c0SlotAddress == 0x15954b8 && kFrontier319c0WaitPlt == 0xf8f7d0);
    Frontier319c0Hooks hooks{wait_poll, wait_wait, wait_tail, nullptr};
    Object o;
    o.put<std::uint32_t>(0x50, 0x4321);
    g_wait = WaitTrace{};
    g_wait.pending = 3;
    auto r = frontier_319c0_fios_wait(o.data(), hooks);
    check("319c0 waits then tail", r.ok && r.waits == 3 && r.value && g_wait.tails == 1);
    check("319c0 handle passed", g_wait.handles == std::vector<std::uint32_t>({0x4321, 0x4321, 0x4321}));
    check("319c0 order", g_wait.order == std::vector<int>({1, 2, 1, 2, 1, 2, 1, 3}));
    g_wait = WaitTrace{};
    g_wait.tail_value = false;
    r = frontier_319c0_fios_wait(o.data(), hooks);
    check("319c0 tail result returned", r.ok && !r.value && r.waits == 0 && g_wait.handles.empty());
    g_wait = WaitTrace{};
    g_wait.pending = 1 << 30;
    r = frontier_319c0_fios_wait(o.data(), hooks);
    check("319c0 overflow refused", !r.ok && r.wait_overflow && r.waits == kFrontier319c0MaxWaits &&
                                        g_wait.tails == 0);
    Frontier319c0Hooks none{};
    check("319c0 missing hooks fail", !frontier_319c0_fios_wait(o.data(), none).ok);
    Frontier319c0Hooks no_tail{wait_poll, wait_wait, nullptr, nullptr};
    r = frontier_319c0_fios_wait(o.data(), no_tail);
    check("319c0 missing tail named", !r.ok && std::string_view(r.missing_hook) == "tail");
    check("319c0 null object fails", !frontier_319c0_fios_wait(nullptr, hooks).ok);
}

// ---------------------------------------------------------------------------------------- 0x31900
struct PollTrace {
    bool done = true;
    std::int32_t error = 0;
    std::int32_t count = 0;
    std::uint64_t size = 0;
    int is_done_calls = 0, error_calls = 0, count_calls = 0, delete_calls = 0, size_calls = 0;
    std::uint32_t is_done_op = 0, deleted_op = 0;
    std::vector<int> order;
    std::uint32_t op_after_size = 0;
    bool change_op_in_size = false;
};
PollTrace g_poll;
bool poll_is_done(std::uint32_t op, void*) {
    ++g_poll.is_done_calls; g_poll.is_done_op = op; g_poll.order.push_back(1); return g_poll.done;
}
std::int32_t poll_error(std::uint32_t, void*) { ++g_poll.error_calls; g_poll.order.push_back(2); return g_poll.error; }
std::int32_t poll_count(std::uint32_t, void*) { ++g_poll.count_calls; g_poll.order.push_back(3); return g_poll.count; }
void poll_delete(std::uint32_t op, void*) { ++g_poll.delete_calls; g_poll.deleted_op = op; g_poll.order.push_back(5); }
std::uint64_t poll_size(void* object, void*) {
    ++g_poll.size_calls;
    g_poll.order.push_back(4);
    if (g_poll.change_op_in_size) {
        std::memcpy(static_cast<std::uint8_t*>(object) + 0x50, &g_poll.op_after_size, 4);
    }
    return g_poll.size;
}

sotc::Frontier31900Hooks poll_hooks() {
    sotc::Frontier31900Hooks h{};
    h.op_is_done = poll_is_done;
    h.op_get_error = poll_error;
    h.op_get_actual_count = poll_count;
    h.op_delete = poll_delete;
    h.file_size = poll_size;
    return h;
}

Object poll_object(std::uint32_t state) {
    Object o;
    o.put<std::uint32_t>(0x50, 0x1234);
    o.put<std::uint32_t>(0x54, state);
    o.put<std::uint32_t>(0x58, 0x55555555);
    o.put<std::uint32_t>(0x5c, 0x66666666);
    o.put<std::uint64_t>(8, 0x7777777700000000ull | 0xaaaa);   // dword 0xaaaa, high dword 0x77777777
    o.put<std::uint32_t>(0x10, 0x4040);
    o.put<std::uint64_t>(0x18, 1000);
    return o;
}

void test_31900() {
    using namespace sotc;
    check("31900 constants", kFrontier31900Rva == 0x31900 && kFrontier31900Size == 0xb1 &&
                                 kFrontier31900SlotAddress == 0x15954b0 &&
                                 kFrontier31900IsDonePlt == 0xf8f7c0 && kFrontier31900GetErrorPlt == 0xf8f7b0 &&
                                 kFrontier31900GetActualCountPlt == 0xf8f7a0 && kFrontier31900DeletePlt == 0xf8f790 &&
                                 kFrontier31900JumpTable == 0xf96ec8 && kFrontier31900JumpTableEntries == 4);
    const auto hooks = poll_hooks();

    // state 0: no hooks, no writes, returns true
    {
        g_poll = PollTrace{};
        Object o = poll_object(0);
        const auto before = o.bytes;
        Frontier31900Hooks none{};
        auto r = frontier_31900_fios_poll({o.data()}, none);
        check("31900 idle", r.ok && r.settled && !r.is_done_called && before == o.bytes);
    }
    // not done: no writes, returns false, only IsDone is called
    for (std::uint32_t s : {1u, 2u, 3u, 4u, 9u}) {
        g_poll = PollTrace{};
        g_poll.done = false;
        Object o = poll_object(s);
        const auto before = o.bytes;
        auto r = frontier_31900_fios_poll({o.data()}, hooks);
        check("31900 pending", r.ok && !r.settled && r.is_done_called && !r.done && before == o.bytes &&
                                   g_poll.error_calls == 0 && g_poll.delete_calls == 0 && g_poll.is_done_op == 0x1234);
    }
    // state 1, error != 0: handle cleared, mode untouched
    {
        g_poll = PollTrace{};
        g_poll.error = 7; g_poll.count = 3;
        Object o = poll_object(1);
        o.put<std::uint8_t>(0xc, 0x08);
        auto r = frontier_31900_fios_poll({o.data()}, hooks);
        check("31900 s1 error", r.ok && r.settled && r.error == 7 && r.count == 3 && g_poll.size_calls == 0);
        check("31900 s1 error fields", o.get<std::uint32_t>(0x5c) == 7 && o.get<std::uint32_t>(0x58) == 3 &&
                                           o.get<std::uint32_t>(0x10) == 0 && o.get<std::uint32_t>(8) == 0xaaaa &&
                                           o.get<std::uint64_t>(0x18) == 1000);
        check("31900 s1 error tail", o.get<std::uint32_t>(0x54) == 0 && o.get<std::uint32_t>(0x50) == 0 &&
                                         g_poll.deleted_op == 0x1234 && r.deleted);
    }
    // state 1, success, size flag clear: mode dword = 1 (high dword kept), no size call
    {
        g_poll = PollTrace{};
        Object o = poll_object(1);
        o.put<std::uint8_t>(0xc, 0x07);  // bit 3 clear (the high dword's low byte)
        auto r = frontier_31900_fios_poll({o.data()}, hooks);
        check("31900 s1 ok no size", r.ok && r.settled && !r.size_called && g_poll.size_calls == 0);
        check("31900 s1 mode dword only", o.get<std::uint32_t>(8) == 1 && o.get<std::uint8_t>(0xc) == 0x07 &&
                                              o.get<std::uint32_t>(0x10) == 0x4040 && o.get<std::uint64_t>(0x18) == 1000);
    }
    // state 1, success, size flag set: cursor = file_size(self)
    {
        g_poll = PollTrace{};
        g_poll.size = 0x123456789ull;
        Object o = poll_object(1);
        o.put<std::uint8_t>(0xc, 0x08);
        auto r = frontier_31900_fios_poll({o.data()}, hooks);
        check("31900 s1 ok size", r.ok && r.size_called && o.get<std::uint64_t>(0x18) == 0x123456789ull &&
                                      o.get<std::uint32_t>(8) == 1);
        check("31900 s1 call order", g_poll.order == std::vector<int>({1, 2, 3, 4, 5}));
    }
    // the op handle is re-read after the virtual size call
    {
        g_poll = PollTrace{};
        g_poll.change_op_in_size = true;
        g_poll.op_after_size = 0x9999;
        Object o = poll_object(1);
        o.put<std::uint8_t>(0xc, 0x08);
        auto r = frontier_31900_fios_poll({o.data()}, hooks);
        check("31900 op re-read", r.ok && g_poll.deleted_op == 0x9999);
    }
    // state 1, success, size flag set, hook missing: fail-visible, object untouched
    {
        g_poll = PollTrace{};
        Object o = poll_object(1);
        o.put<std::uint8_t>(0xc, 0x08);
        const auto before = o.bytes;
        auto h = poll_hooks();
        h.file_size = nullptr;
        auto r = frontier_31900_fios_poll({o.data()}, h);
        check("31900 missing size fails untouched", !r.ok && std::string_view(r.missing_hook) == "file_size" &&
                                                        before == o.bytes && g_poll.delete_calls == 0);
        // with an error the size call is not reached, so the missing hook is irrelevant
        g_poll = PollTrace{};
        g_poll.error = 1;
        r = frontier_31900_fios_poll({o.data()}, h);
        check("31900 size hook not needed on error", r.ok && r.settled);
    }
    // state 2: handle cleared, mode qword cleared (clears bytes 8..15)
    {
        g_poll = PollTrace{};
        Object o = poll_object(2);
        auto r = frontier_31900_fios_poll({o.data()}, hooks);
        check("31900 s2", r.ok && r.settled && o.get<std::uint32_t>(0x10) == 0 && o.get<std::uint64_t>(8) == 0 &&
                              o.get<std::uint64_t>(0x18) == 1000 && g_poll.size_calls == 0);
    }
    // states 3 and 4: cursor += sign-extended count
    for (std::uint32_t s : {3u, 4u}) {
        g_poll = PollTrace{};
        g_poll.count = 512;
        Object o = poll_object(s);
        auto r = frontier_31900_fios_poll({o.data()}, hooks);
        check("31900 s3/4 add", r.ok && o.get<std::uint64_t>(0x18) == 1512 && o.get<std::uint32_t>(8) == 0xaaaa &&
                                    o.get<std::uint32_t>(0x10) == 0x4040);
        g_poll = PollTrace{};
        g_poll.count = -1;  // FHReadv's initial 0xFFFFFFFF marker: cdqe makes it -1
        o = poll_object(s);
        r = frontier_31900_fios_poll({o.data()}, hooks);
        check("31900 s3/4 negative count", r.ok && o.get<std::uint64_t>(0x18) == 999);
        g_poll = PollTrace{};
        g_poll.count = 1;
        o = poll_object(s);
        o.put<std::uint64_t>(0x18, ~0ull);
        r = frontier_31900_fios_poll({o.data()}, hooks);
        check("31900 s3/4 wraps", r.ok && o.get<std::uint64_t>(0x18) == 0);
    }
    // states outside the table: only the tail runs
    for (std::uint32_t s : {5u, 6u, 0x80000000u, 0xffffffffu}) {
        g_poll = PollTrace{};
        g_poll.error = 2; g_poll.count = 4;
        Object o = poll_object(s);
        auto r = frontier_31900_fios_poll({o.data()}, hooks);
        check("31900 default arm", r.ok && r.settled && o.get<std::uint32_t>(0x5c) == 2 && o.get<std::uint32_t>(0x58) == 4 &&
                                       o.get<std::uint32_t>(8) == 0xaaaa && o.get<std::uint32_t>(0x10) == 0x4040 &&
                                       o.get<std::uint64_t>(0x18) == 1000 && o.get<std::uint32_t>(0x54) == 0 &&
                                       g_poll.delete_calls == 1);
    }
    // each reached hook is fail-visible and leaves the object untouched
    struct Case { const char* name; int which; };
    for (const Case c : {Case{"op_is_done", 0}, Case{"op_get_error", 1}, Case{"op_get_actual_count", 2}, Case{"op_delete", 3}}) {
        g_poll = PollTrace{};
        Object o = poll_object(3);
        const auto before = o.bytes;
        auto h = poll_hooks();
        if (c.which == 0) h.op_is_done = nullptr;
        if (c.which == 1) h.op_get_error = nullptr;
        if (c.which == 2) h.op_get_actual_count = nullptr;
        if (c.which == 3) h.op_delete = nullptr;
        auto r = frontier_31900_fios_poll({o.data()}, h);
        check(c.name, !r.ok && r.missing_hook != nullptr && std::string_view(r.missing_hook) == c.name &&
                          before == o.bytes && g_poll.delete_calls == 0);
    }
    check("31900 null object fails", !frontier_31900_fios_poll({}, hooks).ok);
}

// ---------------------------------------------------------------------------------------- 0x47860
struct LockTrace {
    std::uint64_t* lock = nullptr;
    std::uint64_t release_at = 0;
    std::vector<std::uint64_t> pauses;
    std::vector<std::uint32_t> sleeps;
    std::vector<int> order;
    std::uint64_t held_seen = 0;
};
LockTrace g_lock;
void lock_pause(std::uint64_t attempt, void*) {
    g_lock.pauses.push_back(attempt);
    g_lock.order.push_back(2);
    if (attempt == g_lock.release_at) *g_lock.lock = 0;
}
void lock_sleep(std::uint32_t us, void*) { g_lock.sleeps.push_back(us); g_lock.order.push_back(1); }

void test_47860() {
    using namespace sotc;
    check("47860 constants", kFrontier47860Rva == 0x47860 && kFrontier47860Size == 0x80 &&
                                 kFrontier47860SlotAddress0 == 0x1594b48 && kFrontier47860SlotAddress1 == 0x1595bc0 &&
                                 kFrontier47860StoredFieldOffset == 0x28 && kFrontier47860LockPointerOffset == 0x70 &&
                                 kFrontier47860SpinThreshold == 0x2711 && kFrontier47860UsleepPlt == 0xf8fa20 &&
                                 kFrontier47860UsleepThunkRva == 0x3b570);

    // null lock pointer: plain store, no atomics, no hooks
    {
        Object o;
        const auto before = o.bytes;
        auto r = frontier_47860_locked_store({o.data()}, 0x1122334455667788ull);
        check("47860 null lock stores", r.ok && !r.locked && r.stored && o.get<std::uint64_t>(0x28) == 0x1122334455667788ull);
        auto expect = before;
        std::uint64_t v = 0x1122334455667788ull;
        std::memcpy(expect.data() + 0x28, &v, 8);
        check("47860 null lock only touches +0x28", expect == o.bytes);
    }
    // free lock: stored, lock released to 0, no spin
    {
        alignas(8) std::uint64_t lock = 0;
        Object o;
        o.put<std::uint64_t*>(0x70, &lock);
        auto r = frontier_47860_locked_store({o.data()}, 42);
        check("47860 free lock", r.ok && r.locked && r.stored && r.failed_attempts == 0 && lock == 0 &&
                                     o.get<std::uint64_t>(0x28) == 42 && r.usleep_calls == 0);
        check("47860 lock pointer kept", o.get<std::uint64_t*>(0x70) == &lock);
    }
    // contended below the threshold: pause per failure, no usleep, then success
    {
        alignas(8) std::uint64_t lock = 0;
        Object o;
        o.put<std::uint64_t*>(0x70, &lock);
        g_lock = LockTrace{};
        lock = kFrontier47860LockHeld;
        g_lock.lock = &lock;
        g_lock.release_at = 5;
        Frontier47860Hooks h{lock_pause, lock_sleep, nullptr};
        auto r = frontier_47860_locked_store({o.data()}, 7, h);
        check("47860 short spin", r.ok && r.stored && r.failed_attempts == 5 && r.usleep_calls == 0 &&
                                      g_lock.pauses == std::vector<std::uint64_t>({1, 2, 3, 4, 5}) &&
                                      g_lock.sleeps.empty() && lock == 0 && o.get<std::uint64_t>(0x28) == 7);
    }
    // a non-zero, non-held lock word is "not free" too: it is never compare-exchanged
    {
        alignas(8) std::uint64_t lock = 5;
        Object o;
        o.put<std::uint64_t*>(0x70, &lock);
        g_lock = LockTrace{};
        g_lock.lock = &lock;
        g_lock.release_at = 2;
        Frontier47860Hooks h{lock_pause, nullptr, nullptr};
        auto r = frontier_47860_locked_store({o.data()}, 8, h);
        check("47860 nonzero word is busy", r.ok && r.failed_attempts == 2 && lock == 0);
    }
    // contended past 0x2711: usleep(1) before the pause for every attempt >= 0x2711
    {
        alignas(8) std::uint64_t lock = kFrontier47860LockHeld;
        Object o;
        o.put<std::uint64_t*>(0x70, &lock);
        g_lock = LockTrace{};
        g_lock.lock = &lock;
        g_lock.release_at = 0x2711 + 4;
        Frontier47860Hooks h{lock_pause, lock_sleep, nullptr};
        auto r = frontier_47860_locked_store({o.data()}, 9, h);
        check("47860 long spin ok", r.ok && r.stored && r.failed_attempts == 0x2711 + 4);
        check("47860 usleep count", r.usleep_calls == 5 && g_lock.sleeps.size() == 5 &&
                                        g_lock.sleeps[0] == 1);
        // sequence at the threshold: ... pause(0x2710), usleep, pause(0x2711) ...
        bool order_ok = g_lock.order.size() == (0x2711u + 4u) + 5u;
        if (order_ok) {
            const std::size_t first_sleep = 0x2710;  // 0x2710 pauses precede the first usleep
            order_ok = g_lock.order[first_sleep - 1] == 2 && g_lock.order[first_sleep] == 1 &&
                       g_lock.order[first_sleep + 1] == 2;
        }
        check("47860 usleep precedes pause at threshold", order_ok);
        check("47860 pause at 0x2710 is not a sleep", g_lock.pauses[0x2710 - 1] == 0x2710);
    }
    // a lock held forever is reported, not stored, not released
    {
        alignas(8) std::uint64_t lock = kFrontier47860LockHeld;
        Object o;
        o.put<std::uint64_t*>(0x70, &lock);
        o.put<std::uint64_t>(0x28, 0x5555);
        auto r = frontier_47860_locked_store({o.data()}, 1);
        check("47860 overflow refused", !r.ok && r.spin_overflow && !r.stored && lock == kFrontier47860LockHeld &&
                                            o.get<std::uint64_t>(0x28) == 0x5555 &&
                                            r.failed_attempts == kFrontier47860MaxAttempts);
    }
    // misaligned lock pointer is refused untouched
    {
        alignas(16) std::uint8_t raw[32]{};
        Object o;
        o.put<std::uint8_t*>(0x70, raw + 3);
        auto r = frontier_47860_locked_store({o.data()}, 1);
        check("47860 misaligned lock refused", !r.ok && r.misaligned_lock && !r.stored &&
                                                   o.get<std::uint64_t>(0x28) == 0);
    }
    check("47860 null object fails", !frontier_47860_locked_store({}, 1).ok);
}

// ---------------------------------------------------------------------- 0x31310 composition
// The destructor body 0x31310 reaches 0x31900 (direct and vptr+0xa8), 0x318f0 (vptr+0xa0) and 0x31520
// (vptr+0x18, which waits through vptr+0xb0 = 0x319c0). Bind the five models together and run it.
struct Composition {
    Object* object = nullptr;
    int pending_polls = 2;
    int is_done_calls = 0;
    std::vector<std::uint32_t> waits;
    std::vector<std::uint32_t> closed;
    int succeeded_true = 0;
    int finish_calls = 0;
};
Composition g_comp;

bool comp_is_done(std::uint32_t, void*) { return ++g_comp.is_done_calls > g_comp.pending_polls; }
std::int32_t comp_error(std::uint32_t, void*) { return 0; }
std::int32_t comp_count(std::uint32_t, void*) { return 64; }
void comp_delete(std::uint32_t, void*) {}

sotc::Frontier31900Hooks comp_poll_hooks() {
    sotc::Frontier31900Hooks h{};
    h.op_is_done = comp_is_done;
    h.op_get_error = comp_error;
    h.op_get_actual_count = comp_count;
    h.op_delete = comp_delete;
    return h;
}

bool comp_poll(void*) {
    return sotc::frontier_31900_fios_poll({g_comp.object->data()}, comp_poll_hooks()).settled;
}
void comp_wait(std::uint32_t op) { g_comp.waits.push_back(op); }
void comp_succeeded(void*) {
    if (sotc::frontier_318f0_fios_succeeded({g_comp.object->data()}).value) ++g_comp.succeeded_true;
}
bool comp_319c0_poll(void*, void*) { return comp_poll(nullptr); }
bool comp_319c0_tail(void*, void*) {
    return sotc::frontier_318f0_fios_succeeded({g_comp.object->data()}).value;
}
void comp_319c0_wait(std::uint32_t op, void*) { comp_wait(op); }
void comp_31520_wait(void* object, void*) {
    sotc::Frontier319c0Hooks h{comp_319c0_poll, comp_319c0_wait, comp_319c0_tail, nullptr};
    (void)sotc::frontier_319c0_fios_wait(static_cast<std::uint8_t*>(object), h);
}
void comp_close_sync(std::uint64_t, std::uint32_t handle, void*) { g_comp.closed.push_back(handle); }
void comp_close(void*) {
    sotc::Frontier31520Hooks h{};
    h.wait = comp_31520_wait;
    h.close_sync = comp_close_sync;
    (void)sotc::frontier_31520_fios_close({g_comp.object->data()}, h);
}
void comp_finish(void*) { ++g_comp.finish_calls; }

void test_31310_composition() {
    using namespace sotc;
    g_comp = Composition{};
    Object o;
    g_comp.object = &o;
    o.put<std::uint32_t>(8, 1);
    o.put<std::uint32_t>(0x10, 11);
    o.put<std::uint32_t>(0x50, 0x77);
    o.put<std::uint32_t>(0x54, 4);
    o.put<std::uint32_t>(0x58, 0xffffffffu);
    o.put<std::uint64_t>(0x18, 100);

    void* vtable = nullptr;
    Vtable15953f8Slot18_31310Cells cells{};
    cells.vtable_field = &vtable;
    cells.op_handle = reinterpret_cast<std::uint32_t*>(o.data() + 0x50);
    cells.mode = reinterpret_cast<std::uint32_t*>(o.data() + 0x8);
    Vtable15953f8Slot18_31310Hooks hooks{};
    hooks.poll_direct = comp_poll;
    hooks.wait = comp_wait;
    hooks.poll_virtual = comp_poll;
    hooks.succeeded = comp_succeeded;
    hooks.close = comp_close;
    hooks.finish = comp_finish;
    const auto r = vtable_15953f8_slot18_31310(cells, hooks);
    check("31310 composition ok", r.ok && r.waits == 2 && r.succeeded_called && r.close_called);
    check("31310 composition waited on the op", g_comp.waits == std::vector<std::uint32_t>({0x77, 0x77}));
    check("31310 composition polled until done", g_comp.is_done_calls == 3);
    check("31310 composition folded the count", o.get<std::uint64_t>(0x18) == 164);
    check("31310 composition op released", o.get<std::uint32_t>(0x50) == 0 && o.get<std::uint32_t>(0x54) == 0);
    check("31310 composition succeeded read", g_comp.succeeded_true == 1);
    check("31310 composition closed the handle", g_comp.closed == std::vector<std::uint32_t>({11}));
    check("31310 composition handle cleared", o.get<std::uint32_t>(0x10) == 0 && o.get<std::uint64_t>(8) == 0);
    check("31310 composition finish", g_comp.finish_calls == 1);
}

}  // namespace

int main() {
    test_318f0();
    test_31520();
    test_319c0();
    test_31900();
    test_47860();
    test_31310_composition();
    if (failures == 0) {
        std::printf("OK test_frontier_target_models\n");
        return 0;
    }
    std::fprintf(stderr, "test_frontier_target_models: %d failure(s)\n", failures);
    return 1;
}
