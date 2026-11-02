// CPU-01-499F80-BODY-NATIVE-01: fixture for the 193 B body. Covers the happy path, the replacement path
// and every host failure-visible stage (object allocation, CritSec init, buffer A, buffer B).
//
// The retail body has no error branches (E1046), so these stages are host detection only. This fixture
// asserts the two facts that policy decision depends on: nothing is rolled back automatically, and the
// still-live resources are reported so a caller can take ownership.
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

namespace {

struct TestAlloc {
    std::vector<void*> live;
    std::vector<std::uint32_t> alloc_flags;
    std::vector<std::uint32_t> free_tags;
    int fail_at_call = -1;   // 0-based index of the allocate call that returns null
    int alloc_calls = 0;

    static TestAlloc& inst() {
        static TestAlloc self;
        return self;
    }

    static void* allocate(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t flags) {
        TestAlloc& self = inst();
        const int call = self.alloc_calls++;
        if (self.fail_at_call == call) return nullptr;
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
        for (auto it = self.live.begin(); it != self.live.end(); ++it) {
            if (*it == ptr) {
                self.live.erase(it);
                std::free(ptr);
                return true;
            }
        }
        return false;
    }

    static void reset() {
        for (void* p : inst().live) std::free(p);
        inst() = TestAlloc{};
    }
};

sotc::AllocDispatchContract test_contract() {
    sotc::AllocDispatchContract c{};
    c.allocate = &TestAlloc::allocate;
    c.deallocate = &TestAlloc::deallocate;
    return c;
}

// CritSec init hook that fails on demand; the real hook is used otherwise.
int g_critsec_init_calls = 0;
bool g_critsec_init_result = true;

bool critsec_init_hook(void* obj) noexcept {
    ++g_critsec_init_calls;
    if (!g_critsec_init_result) return false;
    return sotc::critsec_init_at(obj) == 0;
}

sotc::Init499f80BodyHooks make_hooks() {
    sotc::Init499f80BodyHooks hooks = sotc::default_init_499f80_body_hooks(test_contract());
    hooks.critsec_init = critsec_init_hook;
    return hooks;
}

std::uint64_t load_u64(const void* p) {
    std::uint64_t v = 0;
    std::memcpy(&v, p, sizeof(v));
    return v;
}

}  // namespace

int main() {
    using sotc::Init499f80Globals;
    using sotc::Init499f80Stage;
    using sotc::run_init_499f80_body;

    CHECK(sotc::kInit499f80BodyRva == 0x499f80);
    CHECK(sotc::kInit499f80BodySize == 193 && sotc::kInit499f80BodyFdeLength == 193);
    CHECK(sotc::kInit499f80ObjectAllocSize == 0x50 && sotc::kInit499f80BufferChunkSize == 0xa00);

    // ---- host contract rejections happen before any write --------------------------------
    {
        TestAlloc::reset();
        void* store = nullptr;
        void* old_obj = nullptr;
        void* default_arg = reinterpret_cast<void*>(0x1234);
        Init499f80Globals g{};
        g.default_arg = default_arg;
        g.store_cell = nullptr;                  // missing caller cell
        g.old_object_cell = &old_obj;

        const auto r = run_init_499f80_body(nullptr, g, make_hooks());
        CHECK(!r.ok && r.stage == Init499f80Stage::globals_missing && store == nullptr);
        CHECK(std::strcmp(r.blocked, "globals") == 0);
        CHECK(!r.store_cell_written && TestAlloc::inst().alloc_calls == 0);
    }

    // ---- happy path: retail ordering, tags and offsets -----------------------------------
    {
        TestAlloc::reset();
        const std::size_t sidecars_before = sotc::critsec_live_sidecars();
        void* store = nullptr;
        void* old_obj = nullptr;
        void* default_arg = reinterpret_cast<void*>(0x1234);
        Init499f80Globals g{};
        g.default_arg = default_arg;
        g.store_cell = &store;
        g.old_object_cell = &old_obj;

        g_critsec_init_calls = 0;
        g_critsec_init_result = true;
        const auto r = run_init_499f80_body(nullptr, g, make_hooks());
        CHECK(r.ok && r.stage == Init499f80Stage::none && r.blocked == nullptr);
        CHECK(r.arg_used == default_arg && store == default_arg);   // arg fallback, then the store
        CHECK(r.store_cell_written && r.object_live && r.critsec_live);
        CHECK(r.new_object != nullptr && old_obj == r.new_object);
        CHECK(r.previous_object == nullptr);
        CHECK(g_critsec_init_calls == 1);

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

        // ---- replacement path: tag-2, tag-2, destroy, tag-1, then the cell write ----------
        const void* first_object = r.new_object;
        const void* first_buffer_a = r.buffer_a;
        const void* first_buffer_b = r.buffer_b;
        TestAlloc& mutable_alloc = TestAlloc::inst();
        mutable_alloc.free_tags.clear();
        mutable_alloc.alloc_flags.clear();
        const std::size_t alloc_calls_before = static_cast<std::size_t>(mutable_alloc.alloc_calls);

        const auto r2 = run_init_499f80_body(reinterpret_cast<void*>(0xABCD), g, make_hooks());
        CHECK(r2.ok && r2.arg_used == reinterpret_cast<void*>(0xABCD) && store == r2.arg_used);
        CHECK(r2.previous_object == first_object);
        CHECK(old_obj == r2.new_object && r2.new_object != first_object);
        CHECK(mutable_alloc.alloc_calls == static_cast<int>(alloc_calls_before) + 3);
        CHECK(mutable_alloc.free_tags.size() == 3);
        CHECK(mutable_alloc.free_tags[0] == 2 && mutable_alloc.free_tags[1] == 2 &&
              mutable_alloc.free_tags[2] == 1);                     // B, A, object
        // The old object's bytes are gone (freed), so only the sidecar count can prove the old CritSec
        // was destroyed: the process holds exactly one sidecar again, for the new object.
        CHECK(sotc::critsec_live_sidecars() == sidecars_before + 1);
        (void)first_buffer_a;
        (void)first_buffer_b;

        // Fixture owns the live replacement object after the test.
        CHECK(sotc::critsec_destroy_at(r2.new_object) == 0);
        TestAlloc::reset();
        CHECK(sotc::critsec_live_sidecars() == sidecars_before);
    }

    // ---- failure stages: visible status, no rollback, live state reported ---------------
    struct Case {
        int fail_at_call;
        bool critsec_fails;
        Init499f80Stage stage;
        const char* name;
        std::size_t live_allocations;   // untracked allocations still held by the caller
        bool critsec_live;
        bool has_buffer_a;
    };
    const Case cases[] = {
        {0, false, Init499f80Stage::alloc_object_failed, "alloc_object", 0, false, false},
        {1, false, Init499f80Stage::alloc_buffer_a_failed, "alloc_buf_a", 1, true, false},
        {2, false, Init499f80Stage::alloc_buffer_b_failed, "alloc_buf_b", 2, true, true},
        {-1, true, Init499f80Stage::critsec_init_failed, "critsec_init", 1, false, false},
    };

    for (const Case& c : cases) {
        TestAlloc::reset();
        const std::size_t sidecars_before = sotc::critsec_live_sidecars();
        void* store = nullptr;
        void* old_obj = nullptr;
        void* default_arg = reinterpret_cast<void*>(0x1234);
        Init499f80Globals g{};
        g.default_arg = default_arg;
        g.store_cell = &store;
        g.old_object_cell = &old_obj;

        // For the CritSec-init case only the first allocation is allowed to succeed.
        g_critsec_init_calls = 0;
        g_critsec_init_result = !c.critsec_fails;
        TestAlloc::inst().fail_at_call = c.fail_at_call;

        const auto r = run_init_499f80_body(nullptr, g, make_hooks());
        CHECK(!r.ok && r.stage == c.stage);
        CHECK(std::strcmp(r.blocked, c.name) == 0);
        // The store cell is written before the first allocation, exactly like 0x499f9c.
        CHECK(r.store_cell_written && store == default_arg);
        // The replacement cell is untouched and nothing was released: no silent rollback.
        CHECK(r.previous_object == nullptr && old_obj == nullptr);
        CHECK(TestAlloc::inst().free_tags.empty());
        CHECK(TestAlloc::inst().live.size() == c.live_allocations);
        CHECK(r.object_live == (c.live_allocations > 0));
        CHECK(r.critsec_live == c.critsec_live);
        CHECK((r.buffer_a != nullptr) == c.has_buffer_a);
        CHECK(r.buffer_b == nullptr);
        CHECK(sotc::critsec_live_sidecars() == sidecars_before + (c.critsec_live ? 1 : 0));
        if (r.critsec_live) CHECK(sotc::critsec_destroy_at(r.new_object) == 0);   // caller's job

        // The fixture takes ownership of what the model left live.
        TestAlloc::reset();
        CHECK(sotc::critsec_live_sidecars() == sidecars_before);
        g_critsec_init_result = true;
    }

    // ---- missing hooks are visible before any write -------------------------------------
    {
        TestAlloc::reset();
        void* store = nullptr;
        void* old_obj = nullptr;
        Init499f80Globals g{};
        g.default_arg = nullptr;
        g.store_cell = &store;
        g.old_object_cell = &old_obj;
        sotc::Init499f80BodyHooks hooks{};
        const auto r = run_init_499f80_body(nullptr, g, hooks);
        CHECK(!r.ok && r.stage == Init499f80Stage::hooks_missing);
        CHECK(std::strcmp(r.blocked, "hooks") == 0);
        CHECK(store == nullptr && TestAlloc::inst().alloc_calls == 0);
    }

    TestAlloc::reset();
    std::puts("OK test_init_499f80_body");
    return 0;
}
