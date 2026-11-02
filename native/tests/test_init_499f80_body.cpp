// CPU-01-499F80-BODY-NATIVE-01 / CPU-01-499F80-HOST-FAILURE-ROLLBACK-01: fixture for the 193 B body.
//
// The retail body has no error branches (E1046). Every failure below is host detection, and every release
// is the host rollback policy selected in E1055, not recovered retail error semantics.
//
// Covered: the happy path, the replacement path, both pre-write host rejections, every candidate creation
// failure (object allocation, CritSec init, each buffer allocation), every cleanup failure (each candidate
// buffer free, candidate CritSec destroy / EBUSY, candidate object free), and every previous-object
// cleanup failure (each old buffer free, old CritSec destroy / EBUSY, old object free), including a retry
// after each one.
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/critsec_3b310.hpp"
#include "core/init_499f80_body.hpp"

#define CHECK(c)                                                                                   \
    do {                                                                                           \
        if (!(c)) {                                                                                \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);                       \
            std::exit(1);                                                                          \
        }                                                                                          \
    } while (0)

#define CASE_CHECK(label, c)                                                                       \
    do {                                                                                           \
        if (!(c)) {                                                                                \
            std::fprintf(stderr, "FAIL [%s] %s:%d %s\n", label, __FILE__, __LINE__, #c);            \
            std::exit(1);                                                                          \
        }                                                                                          \
    } while (0)

namespace {

// Host allocator with injection. A rejected free keeps the pointer owned (it stays in `live`), so a
// rollback retry can free it later; freeing a pointer that is not live counts as a double free.
struct TestAlloc {
    std::vector<void*> live;
    std::vector<std::uint32_t> alloc_flags;
    std::vector<std::uint32_t> free_tags;
    int fail_alloc_at_call = -1;
    int alloc_calls = 0;
    int fail_free_at_call = -1;    // 0-based index among non-null deallocate calls
    int fail_free_at_call2 = -1;   // second index, for a trigger plus a rollback failure
    int free_calls = 0;
    int rejected_frees = 0;
    int double_frees = 0;

    static TestAlloc& inst() {
        static TestAlloc self;
        return self;
    }

    static void* allocate(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t flags) {
        TestAlloc& self = inst();
        const int call = self.alloc_calls++;
        if (self.fail_alloc_at_call == call) return nullptr;
        void* p = std::malloc(size);
        if (p != nullptr) {
            self.live.push_back(p);
            self.alloc_flags.push_back(flags);
        }
        return p;
    }

    static bool deallocate(void* ptr, std::uint32_t tag) {
        TestAlloc& self = inst();
        if (ptr == nullptr) return true;
        self.free_tags.push_back(tag);
        const int call = self.free_calls++;
        if (self.fail_free_at_call == call || self.fail_free_at_call2 == call) {
            ++self.rejected_frees;   // the pointer stays live and owned
            return false;
        }
        for (auto it = self.live.begin(); it != self.live.end(); ++it) {
            if (*it == ptr) {
                self.live.erase(it);
                std::free(ptr);
                return true;
            }
        }
        ++self.double_frees;
        return false;
    }

    static void reset() {
        for (void* p : inst().live) std::free(p);
        inst() = TestAlloc{};
    }

    static bool is_live(void* p) {
        for (void* q : inst().live) {
            if (q == p) return true;
        }
        return false;
    }
};

sotc::AllocDispatchContract test_contract() {
    sotc::AllocDispatchContract c{};
    c.allocate = &TestAlloc::allocate;
    c.deallocate = &TestAlloc::deallocate;
    return c;
}

// CritSec hooks that inject errno-style rejections and otherwise drive the real native model.
int g_init_result = 0;   // 0 = success, else the code returned instead of registering
int g_init_calls = 0;
int g_destroy_error = 0;          // non-zero: every destroy is rejected with this code
int g_destroy_fail_at_call = -1;  // 0-based destroy call index rejected with EBUSY
int g_destroy_calls = 0;

int critsec_init_hook(void* obj) noexcept {
    ++g_init_calls;
    if (g_init_result != 0) return g_init_result;
    return sotc::critsec_init_at(obj);
}

int critsec_destroy_hook(void* obj) noexcept {
    const int call = g_destroy_calls++;
    const int error = g_destroy_error != 0 ? g_destroy_error : EBUSY;
    if (g_destroy_fail_at_call == call) return error;
    if (g_destroy_error != 0) return error;
    return sotc::critsec_destroy_at(obj);
}

void reset_hooks() {
    g_init_result = 0;
    g_init_calls = 0;
    g_destroy_error = 0;
    g_destroy_fail_at_call = -1;
    g_destroy_calls = 0;
}

void allow_everything() {
    TestAlloc::inst().fail_alloc_at_call = -1;
    TestAlloc::inst().fail_free_at_call = -1;
    TestAlloc::inst().fail_free_at_call2 = -1;
    reset_hooks();
}

sotc::Init499f80BodyHooks make_hooks() {
    sotc::Init499f80BodyHooks hooks = sotc::default_init_499f80_body_hooks(test_contract());
    hooks.critsec_init = critsec_init_hook;
    hooks.critsec_destroy = critsec_destroy_hook;
    hooks.critsec_init_failure_atomic = true;   // the fixture's hooks wrap the failure-atomic native model
    return hooks;
}

struct Harness {
    void* store = nullptr;
    void* old_obj = nullptr;
    void* default_arg = reinterpret_cast<void*>(0x1234);
    sotc::Init499f80Globals g{};

    Harness() {
        g.default_arg = default_arg;
        g.store_cell = &store;
        g.old_object_cell = &old_obj;
    }
};

std::uint64_t load_u64(const void* p) {
    std::uint64_t v = 0;
    std::memcpy(&v, p, sizeof(v));
    return v;
}

void expect_cleanup(const char* label, const sotc::Init499f80BodyResult& r,
                    sotc::Init499f80Resource resource, sotc::Init499f80Cleanup status,
                    std::uint32_t tag = 0, int error = 0) {
    const auto& entry = r.record(resource);
    if (entry.status != status || entry.tag != tag || entry.error != error) {
        std::fprintf(stderr,
                     "  resource %d: got status %d tag %u error %d; want status %d tag %u error %d\n",
                     static_cast<int>(resource), static_cast<int>(entry.status), entry.tag,
                     entry.error, static_cast<int>(status), tag, error);
    }
    CASE_CHECK(label, entry.status == status);
    CASE_CHECK(label, entry.tag == tag);
    CASE_CHECK(label, entry.error == error);
}

// A failed creation must leave nothing owned, nothing allocated and no sidecar behind.
void expect_fully_rolled_back(const char* label, const sotc::Init499f80BodyResult& r) {
    CASE_CHECK(label, r.rolled_back && !r.committed);
    CASE_CHECK(label, !r.object_live && !r.critsec_live);
    CASE_CHECK(label, r.buffer_a == nullptr && r.buffer_b == nullptr);
    CASE_CHECK(label, r.cleanup_complete && r.cleanup_failures == 0);
    CASE_CHECK(label, TestAlloc::inst().live.empty());
    CASE_CHECK(label, TestAlloc::inst().double_frees == 0);
}

// Destroys the CritSec of a committed result and drops every host allocation.
void discard_committed(const sotc::Init499f80BodyResult& r) {
    CHECK(r.critsec_live);
    CHECK(sotc::critsec_destroy_at(r.new_object) == 0);
    TestAlloc::reset();
}

struct Live {
    void* object = nullptr;
    void* buffer_a = nullptr;
    void* buffer_b = nullptr;
};

// Commits one object through the body and prepares the log for the call under test.
Live create_previous(Harness& h, const sotc::Init499f80BodyHooks& hooks) {
    const auto r = sotc::run_init_499f80_body(nullptr, h.g, hooks);
    CHECK(r.ok && r.committed && h.old_obj == r.new_object && r.critsec_live);
    const Live live{r.new_object, r.buffer_a, r.buffer_b};
    TestAlloc& alloc = TestAlloc::inst();
    alloc.alloc_flags.clear();
    alloc.free_tags.clear();
    alloc.alloc_calls = 0;
    alloc.free_calls = 0;
    alloc.rejected_frees = 0;
    alloc.double_frees = 0;
    allow_everything();
    return live;
}

// Runs the body once more and requires a commit.
sotc::Init499f80BodyResult run_committing(Harness& h, const sotc::Init499f80BodyHooks& hooks,
                                          void* arg) {
    const auto r = sotc::run_init_499f80_body(arg, h.g, hooks);
    CHECK(r.ok && r.committed && h.old_obj == r.new_object);
    return r;
}

}  // namespace

int main() {
    using sotc::Init499f80Cleanup;
    using sotc::Init499f80Globals;
    using sotc::Init499f80Resource;
    using sotc::Init499f80Stage;
    using sotc::rollback_init_499f80_candidate;
    using sotc::run_init_499f80_body;

    CHECK(sotc::kInit499f80BodyRva == 0x499f80);
    CHECK(sotc::kInit499f80BodySize == 193 && sotc::kInit499f80BodyFdeLength == 193);
    CHECK(sotc::kInit499f80ObjectAllocSize == 0x50 && sotc::kInit499f80BufferChunkSize == 0xa00);
    CHECK(sotc::kInit499f80CleanupCount == 8);

    // ---- host contract rejections happen before any write --------------------------------
    {
        TestAlloc::reset();
        reset_hooks();
        void* store = nullptr;
        void* old_obj = nullptr;
        Init499f80Globals g{};
        g.default_arg = reinterpret_cast<void*>(0x1234);
        g.store_cell = nullptr;                  // missing caller cell
        g.old_object_cell = &old_obj;

        const auto r = run_init_499f80_body(nullptr, g, make_hooks());
        CHECK(!r.ok && r.stage == Init499f80Stage::globals_missing && store == nullptr);
        CHECK(std::strcmp(r.blocked, "globals") == 0);
        CHECK(!r.store_cell_written && TestAlloc::inst().alloc_calls == 0);
    }
    {
        TestAlloc::reset();
        reset_hooks();
        Harness h;
        sotc::Init499f80BodyHooks hooks{};       // no hooks at all
        const auto r = run_init_499f80_body(nullptr, h.g, hooks);
        CHECK(!r.ok && r.stage == Init499f80Stage::hooks_missing);
        CHECK(std::strcmp(r.blocked, "hooks") == 0);
        CHECK(h.store == nullptr && TestAlloc::inst().alloc_calls == 0);
    }
    {
        TestAlloc::reset();
        reset_hooks();
        Harness h;
        sotc::Init499f80BodyHooks hooks = make_hooks();
        hooks.critsec_init_failure_atomic = false;   // the rollback may not assume it
        const auto r = run_init_499f80_body(nullptr, h.g, hooks);
        CHECK(!r.ok && r.stage == Init499f80Stage::hooks_not_failure_atomic);
        CHECK(std::strcmp(r.blocked, "hooks_atomic") == 0);
        CHECK(h.store == nullptr && TestAlloc::inst().alloc_calls == 0);
        CHECK(!r.store_cell_written && !r.store_cell_restored);
    }

    // ---- happy path: retail ordering, tags and offsets -----------------------------------
    {
        TestAlloc::reset();
        reset_hooks();
        const std::size_t sidecars_before = sotc::critsec_live_sidecars();
        Harness h;

        const auto r = run_init_499f80_body(nullptr, h.g, make_hooks());
        CHECK(r.ok && r.stage == Init499f80Stage::none && r.blocked == nullptr);
        CHECK(r.arg_used == h.default_arg && h.store == h.default_arg);   // arg fallback, then the store
        CHECK(r.store_cell_written && !r.store_cell_restored && r.store_cell_prior == nullptr);
        CHECK(r.committed && !r.rolled_back && r.cleanup_complete && r.cleanup_failures == 0);
        CHECK(r.object_live && r.critsec_live);
        CHECK(r.new_object != nullptr && h.old_obj == r.new_object);
        CHECK(r.previous_object == nullptr);
        CHECK(g_init_calls == 1);

        // 0x17070(0x50) flags 1, then two 0x17090(0xa00) with flags 2.
        const auto& alloc = TestAlloc::inst();
        CHECK(alloc.alloc_flags.size() == 3);
        CHECK(alloc.alloc_flags[0] == 1 && alloc.alloc_flags[1] == 2 && alloc.alloc_flags[2] == 2);
        CHECK(alloc.free_tags.empty());                              // no replacement yet

        auto* obj = static_cast<std::uint8_t*>(r.new_object);
        CHECK(*reinterpret_cast<std::uint32_t*>(obj + 0x48) == 0);
        CHECK(*reinterpret_cast<void**>(obj + 0x08) == r.buffer_a);
        CHECK(*reinterpret_cast<void**>(obj + 0x10) == r.buffer_a);
        CHECK(*reinterpret_cast<void**>(obj + 0x18) ==
              static_cast<std::uint8_t*>(r.buffer_a) + sotc::kInit499f80BufferChunkSize);
        CHECK(*reinterpret_cast<void**>(obj + 0x28) == r.buffer_b);
        CHECK(*reinterpret_cast<void**>(obj + 0x30) == r.buffer_b);
        CHECK(*reinterpret_cast<void**>(obj + 0x38) ==
              static_cast<std::uint8_t*>(r.buffer_b) + sotc::kInit499f80BufferChunkSize);
        CHECK(load_u64(obj) != 0);                                   // CritSec handle token
        CHECK(sotc::critsec_live_sidecars() == sidecars_before + 1);
        // A committed call releases nothing, so every record is `not_owned`.
        for (std::size_t i = 0; i < sotc::kInit499f80CleanupCount; ++i) {
            CHECK(r.cleanup[i].status == Init499f80Cleanup::not_owned);
        }

        // ---- replacement path: tag-2, tag-2, destroy, tag-1, then the cell write ----------
        const void* first_object = r.new_object;
        TestAlloc& mutable_alloc = TestAlloc::inst();
        mutable_alloc.free_tags.clear();
        mutable_alloc.alloc_flags.clear();
        mutable_alloc.alloc_calls = 0;
        reset_hooks();

        auto r2 = run_init_499f80_body(reinterpret_cast<void*>(0xABCD), h.g, make_hooks());
        CHECK(r2.ok && r2.arg_used == reinterpret_cast<void*>(0xABCD) && h.store == r2.arg_used);
        CHECK(r2.previous_object == first_object);
        CHECK(r2.committed && !r2.rolled_back && r2.cleanup_complete && r2.cleanup_failures == 0);
        CHECK(h.old_obj == r2.new_object && r2.new_object != first_object);
        CHECK(mutable_alloc.alloc_calls == 3);
        CHECK(mutable_alloc.free_tags.size() == 3);
        CHECK(mutable_alloc.free_tags[0] == 2 && mutable_alloc.free_tags[1] == 2 &&
              mutable_alloc.free_tags[2] == 1);                     // B, A, object
        expect_cleanup("replacement", r2, Init499f80Resource::previous_buffer_b,
                       Init499f80Cleanup::released, 2);
        expect_cleanup("replacement", r2, Init499f80Resource::previous_buffer_a,
                       Init499f80Cleanup::released, 2);
        expect_cleanup("replacement", r2, Init499f80Resource::previous_critsec,
                       Init499f80Cleanup::released);
        expect_cleanup("replacement", r2, Init499f80Resource::previous_object,
                       Init499f80Cleanup::released, 1);
        // The old object's bytes are gone (freed), so only the sidecar count can prove the old CritSec
        // was destroyed: the process holds exactly one sidecar again, for the new object.
        CHECK(sotc::critsec_live_sidecars() == sidecars_before + 1);
        CHECK(g_destroy_calls == 1);

        // A committed result is the caller's: the retry helper must refuse it and release nothing.
        const std::size_t frees_before_refusal = mutable_alloc.free_tags.size();
        CHECK(!rollback_init_499f80_candidate(r2, make_hooks()));
        CHECK(mutable_alloc.free_tags.size() == frees_before_refusal);
        CHECK(r2.object_live && h.old_obj == r2.new_object);

        discard_committed(r2);
        CHECK(sotc::critsec_live_sidecars() == sidecars_before);
    }

    // ---- creation failures: full rollback, speculative store restored ---------------------
    {
        const char* label = "alloc_object_failed";
        TestAlloc::reset();
        reset_hooks();
        const std::size_t sidecars_before = sotc::critsec_live_sidecars();
        Harness h;
        TestAlloc::inst().fail_alloc_at_call = 0;

        const auto r = run_init_499f80_body(nullptr, h.g, make_hooks());
        CASE_CHECK(label, !r.ok && r.stage == Init499f80Stage::alloc_object_failed);
        CASE_CHECK(label, std::strcmp(r.blocked, "alloc_object") == 0);
        CASE_CHECK(label, r.store_cell_written && r.store_cell_prior == nullptr);
        CASE_CHECK(label, r.store_cell_restored && h.store == nullptr);
        CASE_CHECK(label, r.previous_object == nullptr && h.old_obj == nullptr);
        CASE_CHECK(label, r.new_object == nullptr && r.arg_used == h.default_arg);
        CASE_CHECK(label, TestAlloc::inst().free_tags.empty());
        expect_fully_rolled_back(label, r);
        expect_cleanup(label, r, Init499f80Resource::candidate_object, Init499f80Cleanup::not_owned);

        allow_everything();
        const auto retry = run_committing(h, make_hooks(), nullptr);
        CASE_CHECK(label, h.store == h.default_arg);
        CASE_CHECK(label, sotc::critsec_live_sidecars() == sidecars_before + 1);
        discard_committed(retry);
        CASE_CHECK(label, sotc::critsec_live_sidecars() == sidecars_before);
    }
    {
        const char* label = "critsec_init_failed";
        TestAlloc::reset();
        reset_hooks();
        const std::size_t sidecars_before = sotc::critsec_live_sidecars();
        Harness h;
        g_init_result = EAGAIN;

        const auto r = run_init_499f80_body(nullptr, h.g, make_hooks());
        CASE_CHECK(label, !r.ok && r.stage == Init499f80Stage::critsec_init_failed);
        CASE_CHECK(label, std::strcmp(r.blocked, "critsec_init") == 0);
        CASE_CHECK(label, r.critsec_init_error == EAGAIN);
        CASE_CHECK(label, r.store_cell_restored && h.store == nullptr);
        // The hook is asserted failure-atomic, so the object is freed without a destroy call.
        CASE_CHECK(label, g_destroy_calls == 0);
        CASE_CHECK(label, TestAlloc::inst().free_tags.size() == 1);
        CASE_CHECK(label, TestAlloc::inst().free_tags[0] == 1);
        expect_cleanup(label, r, Init499f80Resource::candidate_critsec, Init499f80Cleanup::not_owned);
        expect_cleanup(label, r, Init499f80Resource::candidate_object,
                       Init499f80Cleanup::released, 1);
        expect_fully_rolled_back(label, r);
        CASE_CHECK(label, sotc::critsec_live_sidecars() == sidecars_before);

        allow_everything();
        const auto retry = run_committing(h, make_hooks(), nullptr);
        discard_committed(retry);
        CASE_CHECK(label, sotc::critsec_live_sidecars() == sidecars_before);
    }
    {
        const char* label = "alloc_buffer_a_failed";
        TestAlloc::reset();
        reset_hooks();
        const std::size_t sidecars_before = sotc::critsec_live_sidecars();
        Harness h;
        TestAlloc::inst().fail_alloc_at_call = 1;

        const auto r = run_init_499f80_body(nullptr, h.g, make_hooks());
        CASE_CHECK(label, !r.ok && r.stage == Init499f80Stage::alloc_buffer_a_failed);
        CASE_CHECK(label, std::strcmp(r.blocked, "alloc_buf_a") == 0);
        CASE_CHECK(label, r.store_cell_restored && h.store == nullptr);
        CASE_CHECK(label, g_destroy_calls == 1);
        CASE_CHECK(label, TestAlloc::inst().free_tags.size() == 1);
        CASE_CHECK(label, TestAlloc::inst().free_tags[0] == 1);
        expect_cleanup(label, r, Init499f80Resource::candidate_buffer_a,
                       Init499f80Cleanup::not_owned);
        expect_cleanup(label, r, Init499f80Resource::candidate_critsec,
                       Init499f80Cleanup::released);
        expect_cleanup(label, r, Init499f80Resource::candidate_object,
                       Init499f80Cleanup::released, 1);
        expect_fully_rolled_back(label, r);
        CASE_CHECK(label, sotc::critsec_live_sidecars() == sidecars_before);

        allow_everything();
        const auto retry = run_committing(h, make_hooks(), nullptr);
        discard_committed(retry);
        CASE_CHECK(label, sotc::critsec_live_sidecars() == sidecars_before);
    }
    {
        const char* label = "alloc_buffer_b_failed";
        TestAlloc::reset();
        reset_hooks();
        const std::size_t sidecars_before = sotc::critsec_live_sidecars();
        Harness h;
        TestAlloc::inst().fail_alloc_at_call = 2;

        const auto r = run_init_499f80_body(nullptr, h.g, make_hooks());
        CASE_CHECK(label, !r.ok && r.stage == Init499f80Stage::alloc_buffer_b_failed);
        CASE_CHECK(label, std::strcmp(r.blocked, "alloc_buf_b") == 0);
        CASE_CHECK(label, r.store_cell_restored && h.store == nullptr);
        CASE_CHECK(label, TestAlloc::inst().free_tags.size() == 2);
        CASE_CHECK(label, TestAlloc::inst().free_tags[0] == 2);
        CASE_CHECK(label, TestAlloc::inst().free_tags[1] == 1);
        expect_cleanup(label, r, Init499f80Resource::candidate_buffer_b,
                       Init499f80Cleanup::not_owned);
        expect_cleanup(label, r, Init499f80Resource::candidate_buffer_a,
                       Init499f80Cleanup::released, 2);
        expect_cleanup(label, r, Init499f80Resource::candidate_critsec,
                       Init499f80Cleanup::released);
        expect_cleanup(label, r, Init499f80Resource::candidate_object,
                       Init499f80Cleanup::released, 1);
        expect_fully_rolled_back(label, r);
        CASE_CHECK(label, sotc::critsec_live_sidecars() == sidecars_before);

        allow_everything();
        const auto retry = run_committing(h, make_hooks(), nullptr);
        discard_committed(retry);
        CASE_CHECK(label, sotc::critsec_live_sidecars() == sidecars_before);
    }

    // ---- previous-object cleanup: every rejection keeps the old object reachable ----------
    // E1: the old buffer B release is rejected. The old object stays in the cell, the candidate is rolled
    // back, and a later call frees only what is still owned.
    {
        const char* label = "previous_buffer_b_rejected";
        TestAlloc::reset();
        reset_hooks();
        const std::size_t sidecars_before = sotc::critsec_live_sidecars();
        Harness h;
        const Live old = create_previous(h, make_hooks());
        TestAlloc::inst().fail_free_at_call = 0;   // first free is old+0x28 (tag 2)

        auto r = run_init_499f80_body(reinterpret_cast<void*>(0xBEEF), h.g, make_hooks());
        CASE_CHECK(label, !r.ok && r.stage == Init499f80Stage::previous_cleanup_failed);
        CASE_CHECK(label, std::strcmp(r.blocked, "previous_cleanup") == 0);
        CASE_CHECK(label, !r.committed && h.old_obj == old.object);      // old object reachable
        CASE_CHECK(label, r.previous_object == old.object);
        CASE_CHECK(label, r.store_cell_restored && h.store == h.default_arg);   // prior value
        CASE_CHECK(label, TestAlloc::inst().free_tags.size() == 5);
        CASE_CHECK(label, TestAlloc::inst().free_tags[0] == 2);          // old B: rejected
        CASE_CHECK(label, TestAlloc::inst().free_tags[1] == 2);          // old A: released
        CASE_CHECK(label, TestAlloc::inst().free_tags[2] == 2);          // candidate B
        CASE_CHECK(label, TestAlloc::inst().free_tags[3] == 2);          // candidate A
        CASE_CHECK(label, TestAlloc::inst().free_tags[4] == 1);          // candidate object
        expect_cleanup(label, r, Init499f80Resource::previous_buffer_b,
                       Init499f80Cleanup::failed, 2);
        expect_cleanup(label, r, Init499f80Resource::previous_buffer_a,
                       Init499f80Cleanup::released, 2);
        expect_cleanup(label, r, Init499f80Resource::previous_critsec,
                       Init499f80Cleanup::released);
        expect_cleanup(label, r, Init499f80Resource::previous_object,
                       Init499f80Cleanup::held);
        expect_cleanup(label, r, Init499f80Resource::candidate_buffer_b,
                       Init499f80Cleanup::released, 2);
        expect_cleanup(label, r, Init499f80Resource::candidate_buffer_a,
                       Init499f80Cleanup::released, 2);
        expect_cleanup(label, r, Init499f80Resource::candidate_object,
                       Init499f80Cleanup::released, 1);
        CASE_CHECK(label, r.rolled_back && !r.cleanup_complete && r.cleanup_failures == 1);
        CASE_CHECK(label, TestAlloc::inst().live.size() == 2);           // old object + old buffer B
        CASE_CHECK(label, TestAlloc::inst().is_live(old.object));
        CASE_CHECK(label, TestAlloc::inst().is_live(old.buffer_b));
        // The old buffer B field is kept: it is the retry handle for the rejected release.
        CASE_CHECK(label, *reinterpret_cast<void**>(static_cast<std::uint8_t*>(old.object) + 0x28) ==
                          old.buffer_b);
        // Both old buffer A and the old CritSec were released.
        CASE_CHECK(label, *reinterpret_cast<void**>(static_cast<std::uint8_t*>(old.object) + 0x08) ==
                          nullptr);
        CASE_CHECK(label, load_u64(old.object) == 0);
        CASE_CHECK(label, sotc::critsec_live_sidecars() == sidecars_before);   // only the old sidecar was
                                                                              // destroyed; the candidate one
                                                                              // was rolled back too

        allow_everything();                       // also drops the destroy-call counter
        const auto retry = run_committing(h, make_hooks(), reinterpret_cast<void*>(0xFEED));
        CASE_CHECK(label, g_destroy_calls == 0);   // empty handle: no destroy in the retry
        CASE_CHECK(label, !TestAlloc::inst().is_live(old.object));
        CASE_CHECK(label, !TestAlloc::inst().is_live(old.buffer_a));
        CASE_CHECK(label, TestAlloc::inst().double_frees == 0);
        discard_committed(retry);
        CASE_CHECK(label, sotc::critsec_live_sidecars() == sidecars_before);
    }
    // E2: the old buffer A release is rejected.
    {
        const char* label = "previous_buffer_a_rejected";
        TestAlloc::reset();
        reset_hooks();
        const std::size_t sidecars_before = sotc::critsec_live_sidecars();
        Harness h;
        const Live old = create_previous(h, make_hooks());
        TestAlloc::inst().fail_free_at_call = 1;   // second free is old+0x08 (tag 2)

        auto r = run_init_499f80_body(reinterpret_cast<void*>(0xBEEF), h.g, make_hooks());
        CASE_CHECK(label, !r.ok && r.stage == Init499f80Stage::previous_cleanup_failed);
        CASE_CHECK(label, h.old_obj == old.object && r.store_cell_restored);
        expect_cleanup(label, r, Init499f80Resource::previous_buffer_b,
                       Init499f80Cleanup::released, 2);
        expect_cleanup(label, r, Init499f80Resource::previous_buffer_a,
                       Init499f80Cleanup::failed, 2);
        expect_cleanup(label, r, Init499f80Resource::previous_critsec,
                       Init499f80Cleanup::released);
        expect_cleanup(label, r, Init499f80Resource::previous_object, Init499f80Cleanup::held);
        CASE_CHECK(label, r.rolled_back && r.cleanup_failures == 1 && !r.cleanup_complete);
        CASE_CHECK(label, TestAlloc::inst().free_tags.size() == 5);
        CASE_CHECK(label, TestAlloc::inst().is_live(old.buffer_a));       // rejected: still owned
        CASE_CHECK(label, !TestAlloc::inst().is_live(old.buffer_b));      // released
        CASE_CHECK(label, *reinterpret_cast<void**>(static_cast<std::uint8_t*>(old.object) + 0x08) ==
                          old.buffer_a);
        CASE_CHECK(label, *reinterpret_cast<void**>(static_cast<std::uint8_t*>(old.object) + 0x28) ==
                          nullptr);

        allow_everything();
        const auto retry = run_committing(h, make_hooks(), reinterpret_cast<void*>(0xFEED));
        CASE_CHECK(label, !TestAlloc::inst().is_live(old.buffer_a));
        CASE_CHECK(label, TestAlloc::inst().double_frees == 0);
        discard_committed(retry);
        CASE_CHECK(label, sotc::critsec_live_sidecars() == sidecars_before);
    }
    // E3: the old CritSec destroy is rejected with EBUSY; the containing object must not be freed.
    {
        const char* label = "previous_critsec_ebusy";
        TestAlloc::reset();
        reset_hooks();
        const std::size_t sidecars_before = sotc::critsec_live_sidecars();
        Harness h;
        const Live old = create_previous(h, make_hooks());
        g_destroy_fail_at_call = 0;   // the first destroy is the old object

        auto r = run_init_499f80_body(reinterpret_cast<void*>(0xBEEF), h.g, make_hooks());
        CASE_CHECK(label, !r.ok && r.stage == Init499f80Stage::previous_cleanup_failed);
        CASE_CHECK(label, h.old_obj == old.object && r.store_cell_restored);
        expect_cleanup(label, r, Init499f80Resource::previous_buffer_b,
                       Init499f80Cleanup::released, 2);
        expect_cleanup(label, r, Init499f80Resource::previous_buffer_a,
                       Init499f80Cleanup::released, 2);
        expect_cleanup(label, r, Init499f80Resource::previous_critsec,
                       Init499f80Cleanup::failed, 0, EBUSY);
        expect_cleanup(label, r, Init499f80Resource::previous_object, Init499f80Cleanup::held);
        CASE_CHECK(label, r.rolled_back && r.cleanup_failures == 1 && !r.cleanup_complete);
        CASE_CHECK(label, TestAlloc::inst().is_live(old.object));
        CASE_CHECK(label, load_u64(old.object) != 0);          // the handle token is retained
        CASE_CHECK(label, sotc::critsec_live_sidecars() == sidecars_before + 1);
        CASE_CHECK(label, TestAlloc::inst().free_tags.size() == 5);

        allow_everything();
        const auto retry = run_committing(h, make_hooks(), reinterpret_cast<void*>(0xFEED));
        CASE_CHECK(label, !TestAlloc::inst().is_live(old.object));
        CASE_CHECK(label, TestAlloc::inst().double_frees == 0);
        discard_committed(retry);
        CASE_CHECK(label, sotc::critsec_live_sidecars() == sidecars_before);
    }
    // E4: the old object free (tag 1) is rejected.
    {
        const char* label = "previous_object_rejected";
        TestAlloc::reset();
        reset_hooks();
        const std::size_t sidecars_before = sotc::critsec_live_sidecars();
        Harness h;
        const Live old = create_previous(h, make_hooks());
        TestAlloc::inst().fail_free_at_call = 2;   // third free is the old object (tag 1)

        auto r = run_init_499f80_body(reinterpret_cast<void*>(0xBEEF), h.g, make_hooks());
        CASE_CHECK(label, !r.ok && r.stage == Init499f80Stage::previous_cleanup_failed);
        CASE_CHECK(label, h.old_obj == old.object && r.store_cell_restored);
        expect_cleanup(label, r, Init499f80Resource::previous_buffer_b,
                       Init499f80Cleanup::released, 2);
        expect_cleanup(label, r, Init499f80Resource::previous_buffer_a,
                       Init499f80Cleanup::released, 2);
        expect_cleanup(label, r, Init499f80Resource::previous_critsec,
                       Init499f80Cleanup::released);
        expect_cleanup(label, r, Init499f80Resource::previous_object,
                       Init499f80Cleanup::failed, 1);
        CASE_CHECK(label, r.rolled_back && r.cleanup_failures == 1 && !r.cleanup_complete);
        CASE_CHECK(label, TestAlloc::inst().is_live(old.object));
        CASE_CHECK(label, TestAlloc::inst().live.size() == 1);
        CASE_CHECK(label, TestAlloc::inst().free_tags.size() == 6);
        CASE_CHECK(label, TestAlloc::inst().free_tags[2] == 1);   // rejected old object free
        CASE_CHECK(label, load_u64(old.object) == 0);             // destroy succeeded

        allow_everything();                       // also drops the destroy-call counter
        const auto retry = run_committing(h, make_hooks(), reinterpret_cast<void*>(0xFEED));
        CASE_CHECK(label, g_destroy_calls == 0);   // empty handle: skipped
        CASE_CHECK(label, !TestAlloc::inst().is_live(old.object));
        CASE_CHECK(label, TestAlloc::inst().double_frees == 0);
        discard_committed(retry);
        CASE_CHECK(label, sotc::critsec_live_sidecars() == sidecars_before);
    }
    // E5: the candidate buffer B free is rejected while rolling back.
    {
        const char* label = "candidate_buffer_b_rejected";
        TestAlloc::reset();
        reset_hooks();
        Harness h;
        const Live old = create_previous(h, make_hooks());
        TestAlloc::inst().fail_free_at_call = 0;    // old B: the rejection that triggers rollback
        TestAlloc::inst().fail_free_at_call2 = 2;   // candidate B

        auto r = run_init_499f80_body(reinterpret_cast<void*>(0xBEEF), h.g, make_hooks());
        CASE_CHECK(label, !r.ok && r.stage == Init499f80Stage::previous_cleanup_failed);
        CASE_CHECK(label, !r.rolled_back && r.object_live && !r.critsec_live);
        CASE_CHECK(label, r.buffer_a == nullptr && r.buffer_b != nullptr);
        CASE_CHECK(label, r.new_object != nullptr && r.previous_object == old.object);
        CASE_CHECK(label, r.cleanup_failures == 2 && !r.cleanup_complete);
        expect_cleanup(label, r, Init499f80Resource::previous_buffer_b,
                       Init499f80Cleanup::failed, 2);
        expect_cleanup(label, r, Init499f80Resource::candidate_buffer_b,
                       Init499f80Cleanup::failed, 2);
        expect_cleanup(label, r, Init499f80Resource::candidate_buffer_a,
                       Init499f80Cleanup::released, 2);
        expect_cleanup(label, r, Init499f80Resource::candidate_critsec,
                       Init499f80Cleanup::released);
        expect_cleanup(label, r, Init499f80Resource::candidate_object,
                       Init499f80Cleanup::held);
        CASE_CHECK(label, TestAlloc::inst().free_tags.size() == 4);   // no object free at all
        CASE_CHECK(label, TestAlloc::inst().is_live(old.buffer_b));
        CASE_CHECK(label, TestAlloc::inst().is_live(r.new_object));
        CASE_CHECK(label, TestAlloc::inst().is_live(r.buffer_b));

        // Retry only the candidate: the tag-2 free now succeeds and the object is freed last.
        const int free_calls_before = TestAlloc::inst().free_calls;
        allow_everything();
        CASE_CHECK(label, rollback_init_499f80_candidate(r, make_hooks()));
        CASE_CHECK(label, r.rolled_back && !r.object_live && r.buffer_b == nullptr);
        expect_cleanup(label, r, Init499f80Resource::candidate_buffer_b,
                       Init499f80Cleanup::released, 2);
        expect_cleanup(label, r, Init499f80Resource::candidate_object,
                       Init499f80Cleanup::released, 1);
        CASE_CHECK(label, TestAlloc::inst().free_calls == free_calls_before + 2);
        CASE_CHECK(label, TestAlloc::inst().free_tags.back() == 1);
        // A second retry is a no-op: no allocator call, no destroy.
        const int frees_after = TestAlloc::inst().free_calls;
        const int destroys_after = g_destroy_calls;
        CASE_CHECK(label, rollback_init_499f80_candidate(r, make_hooks()));
        CASE_CHECK(label, TestAlloc::inst().free_calls == frees_after);
        CASE_CHECK(label, g_destroy_calls == destroys_after);
        CASE_CHECK(label, TestAlloc::inst().double_frees == 0);

        const auto retry = run_committing(h, make_hooks(), reinterpret_cast<void*>(0xFEED));
        CASE_CHECK(label, !TestAlloc::inst().is_live(old.buffer_b));
        CASE_CHECK(label, TestAlloc::inst().double_frees == 0);
        discard_committed(retry);
    }
    // E6: the candidate buffer A free is rejected while rolling back.
    {
        const char* label = "candidate_buffer_a_rejected";
        TestAlloc::reset();
        reset_hooks();
        Harness h;
        const Live old = create_previous(h, make_hooks());
        TestAlloc::inst().fail_free_at_call = 0;    // old B: rollback trigger
        TestAlloc::inst().fail_free_at_call2 = 3;   // candidate A

        auto r = run_init_499f80_body(reinterpret_cast<void*>(0xBEEF), h.g, make_hooks());
        CASE_CHECK(label, !r.ok && r.stage == Init499f80Stage::previous_cleanup_failed);
        CASE_CHECK(label, r.object_live && r.buffer_a != nullptr && r.buffer_b == nullptr);
        CASE_CHECK(label, r.cleanup_failures == 2 && !r.cleanup_complete && !r.rolled_back);
        expect_cleanup(label, r, Init499f80Resource::candidate_buffer_a,
                       Init499f80Cleanup::failed, 2);
        expect_cleanup(label, r, Init499f80Resource::candidate_buffer_b,
                       Init499f80Cleanup::released, 2);
        expect_cleanup(label, r, Init499f80Resource::candidate_object,
                       Init499f80Cleanup::held);

        allow_everything();
        CASE_CHECK(label, rollback_init_499f80_candidate(r, make_hooks()));
        CASE_CHECK(label, r.rolled_back && r.buffer_a == nullptr && !r.object_live);
        CASE_CHECK(label, TestAlloc::inst().double_frees == 0);
        CASE_CHECK(label, rollback_init_499f80_candidate(r, make_hooks()));   // idempotent

        const auto retry = run_committing(h, make_hooks(), reinterpret_cast<void*>(0xFEED));
        CASE_CHECK(label, TestAlloc::inst().double_frees == 0);
        discard_committed(retry);
        (void)old;
    }
    // E7: the candidate CritSec destroy is rejected with EBUSY; the object must stay allocated.
    {
        const char* label = "candidate_critsec_ebusy";
        TestAlloc::reset();
        reset_hooks();
        Harness h;
        const Live old = create_previous(h, make_hooks());
        TestAlloc::inst().fail_free_at_call = 0;   // old B: rollback trigger
        g_destroy_fail_at_call = 1;                // destroy #0 is the old object, #1 the candidate

        auto r = run_init_499f80_body(reinterpret_cast<void*>(0xBEEF), h.g, make_hooks());
        CASE_CHECK(label, !r.ok && r.stage == Init499f80Stage::previous_cleanup_failed);
        CASE_CHECK(label, r.object_live && r.critsec_live);
        CASE_CHECK(label, r.buffer_a == nullptr && r.buffer_b == nullptr);
        expect_cleanup(label, r, Init499f80Resource::candidate_buffer_b,
                       Init499f80Cleanup::released, 2);
        expect_cleanup(label, r, Init499f80Resource::candidate_buffer_a,
                       Init499f80Cleanup::released, 2);
        expect_cleanup(label, r, Init499f80Resource::candidate_critsec,
                       Init499f80Cleanup::failed, 0, EBUSY);
        expect_cleanup(label, r, Init499f80Resource::candidate_object,
                       Init499f80Cleanup::held);
        CASE_CHECK(label, r.cleanup_failures == 2 && !r.cleanup_complete);
        // Never a tag-1 free after a rejected destroy.
        for (std::uint32_t tag : TestAlloc::inst().free_tags) CASE_CHECK(label, tag == 2);
        CASE_CHECK(label, TestAlloc::inst().is_live(r.new_object));
        CASE_CHECK(label, load_u64(r.new_object) != 0);

        allow_everything();
        CASE_CHECK(label, rollback_init_499f80_candidate(r, make_hooks()));
        CASE_CHECK(label, r.rolled_back && !r.critsec_live && !r.object_live);
        expect_cleanup(label, r, Init499f80Resource::candidate_critsec,
                       Init499f80Cleanup::released);
        expect_cleanup(label, r, Init499f80Resource::candidate_object,
                       Init499f80Cleanup::released, 1);
        CASE_CHECK(label, TestAlloc::inst().double_frees == 0);

        const auto retry = run_committing(h, make_hooks(), reinterpret_cast<void*>(0xFEED));
        CASE_CHECK(label, TestAlloc::inst().double_frees == 0);
        discard_committed(retry);
        (void)old;
    }
    // E8: the candidate object free is rejected; a retry frees only the object.
    {
        const char* label = "candidate_object_rejected";
        TestAlloc::reset();
        reset_hooks();
        Harness h;
        const Live old = create_previous(h, make_hooks());
        TestAlloc::inst().fail_free_at_call = 0;    // old B: rollback trigger
        TestAlloc::inst().fail_free_at_call2 = 4;   // candidate object (tag 1)

        auto r = run_init_499f80_body(reinterpret_cast<void*>(0xBEEF), h.g, make_hooks());
        CASE_CHECK(label, !r.ok && r.stage == Init499f80Stage::previous_cleanup_failed);
        CASE_CHECK(label, r.object_live && !r.critsec_live);
        expect_cleanup(label, r, Init499f80Resource::candidate_critsec,
                       Init499f80Cleanup::released);
        expect_cleanup(label, r, Init499f80Resource::candidate_object,
                       Init499f80Cleanup::failed, 1);
        CASE_CHECK(label, r.buffer_a == nullptr && r.buffer_b == nullptr);

        allow_everything();
        const int free_calls_before = TestAlloc::inst().free_calls;
        CASE_CHECK(label, rollback_init_499f80_candidate(r, make_hooks()));
        CASE_CHECK(label, TestAlloc::inst().free_calls == free_calls_before + 1);
        CASE_CHECK(label, TestAlloc::inst().free_tags.back() == 1);
        CASE_CHECK(label, r.rolled_back && !r.object_live);
        CASE_CHECK(label, TestAlloc::inst().double_frees == 0);

        const auto retry = run_committing(h, make_hooks(), reinterpret_cast<void*>(0xFEED));
        discard_committed(retry);
        (void)old;
    }

    TestAlloc::reset();
    reset_hooks();
    CHECK(sotc::critsec_live_sidecars() == 0);
    std::puts("OK test_init_499f80_body");
    return 0;
}
