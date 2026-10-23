// Fixture for the native lift of original RVA 0x1290 (Init 0x531d60 callee).
// Asserts the recovered effect order, descriptor contents, the exact vcall argument
// (&A + 0x20) and the fail-visible behaviour when a subsystem hook is absent.
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "core/init_1290.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

struct Recorder {
    std::vector<int> order;
    const HandlerRegistration1290* reg_a = nullptr;
    const HandlerRegistration1290* reg_b = nullptr;
    void* self_arg = nullptr;
    void* owner_seen = nullptr;
};

Recorder g;

void hook_insert_a(void* owner, const HandlerRegistration1290& reg) {
    g.order.push_back(1);
    g.owner_seen = owner;
    g.reg_a = &reg;
}

void hook_vcall(void* self_arg) {
    g.order.push_back(2);
    g.self_arg = self_arg;
}

void hook_insert_b(void* owner, const HandlerRegistration1290& reg) {
    g.order.push_back(3);
    (void)owner;
    g.reg_b = &reg;
}

Init1290Hooks full_hooks() {
    Init1290Hooks h{};
    h.insert_1bde30 = hook_insert_a;
    h.vcall_1594060_slot3 = hook_vcall;
    h.insert_21cd0 = hook_insert_b;
    return h;
}

void reset() { g = Recorder{}; }

}  // namespace

int main() {
    // ELF-pinned constants.
    CHECK(kFn1290Rva == 0x1290);
    CHECK(kFn1290Size == 195);
    CHECK(kFn1290EndRva == 0x1353);
    CHECK(kFn1290RegAInvoke == 0x1a00 && kFn1290RegBInvoke == 0x1a10);
    CHECK(kFn1290RegAFn == 0xb10 && kFn1290RegBFn == 0xbb0);
    CHECK(kFn1290Table == 0x1594060);
    CHECK(sizeof(HandlerEntry1290) == 0x20);
    CHECK(offsetof(HandlerRegistration1290, self_arg) == 0x20);
    CHECK(offsetof(HandlerRegistration1290, table) == 0x40);

    int self_obj = 0;
    {
        reset();
        const Init1290Result r = init_1290(&self_obj, full_hooks());
        CHECK(r.ok);
        CHECK(r.insert_1bde30_count == 1);
        CHECK(r.vcall_count == 1);
        CHECK(r.insert_21cd0_count == 1);
        // Observed order: A insert -> vcall -> B insert.
        CHECK(g.order.size() == 3 && g.order[0] == 1 && g.order[1] == 2 && g.order[2] == 3);
        CHECK(g.reg_a == &r.reg_a);
        CHECK(g.reg_b == &r.reg_b);

        // Descriptor A: ctx=&self, invoke 0x1a00, reserved 0, fn 0xb10, table 0x1594060.
        CHECK(r.reg_a.entry.ctx == reinterpret_cast<std::uintptr_t>(&self_obj));
        CHECK(r.reg_a.entry.invoke == 0x1a00);
        CHECK(r.reg_a.entry.reserved == 0);
        CHECK(r.reg_a.entry.fn == 0xb10);
        CHECK(r.reg_a.table == 0x1594060);
        CHECK(r.reg_a.zero_48 == 0);

        // Descriptor B: same ctx, invoke 0x1a10, fn 0xbb0.
        CHECK(r.reg_b.entry.ctx == reinterpret_cast<std::uintptr_t>(&self_obj));
        CHECK(r.reg_b.entry.invoke == 0x1a10);
        CHECK(r.reg_b.entry.reserved == 0);
        CHECK(r.reg_b.entry.fn == 0xbb0);

        // The vcall receives exactly &A + 0x20, not &A.
        const void* self_arg_view = r.reg_a.self_arg;
        const void* reg_a_view = &r.reg_a;
        const void* reg_a_plus_20 =
            static_cast<const void*>(reinterpret_cast<const char*>(&r.reg_a) + 0x20);
        CHECK(g.self_arg == self_arg_view);
        CHECK(g.self_arg == reg_a_plus_20);
        CHECK(g.self_arg != reg_a_view);
        CHECK(r.vcall_self_arg == g.self_arg);
    }

    // A missing subsystem hook is a visible failure, not a fake success, and stops
    // the sequence exactly where the original would fault.
    {
        reset();
        Init1290Hooks h = full_hooks();
        h.insert_1bde30 = nullptr;
        const Init1290Result r = init_1290(&self_obj, h);
        CHECK(!r.ok);
        CHECK(r.insert_1bde30_count == 0 && r.vcall_count == 0 && r.insert_21cd0_count == 0);
        CHECK(g.order.empty());
    }
    {
        reset();
        Init1290Hooks h = full_hooks();
        h.vcall_1594060_slot3 = nullptr;
        const Init1290Result r = init_1290(&self_obj, h);
        CHECK(!r.ok);
        CHECK(r.insert_1bde30_count == 1 && r.vcall_count == 0 && r.insert_21cd0_count == 0);
        CHECK(g.order.size() == 1 && g.order[0] == 1);
    }
    {
        reset();
        Init1290Hooks h = full_hooks();
        h.insert_21cd0 = nullptr;
        const Init1290Result r = init_1290(&self_obj, h);
        CHECK(!r.ok);
        CHECK(r.insert_1bde30_count == 1 && r.vcall_count == 1 && r.insert_21cd0_count == 0);
        CHECK(g.order.size() == 2);
    }

    std::printf("init_1290: all checks passed (order A->vcall->B, 0x20 B entries, &A+0x20 vcall arg)\n");
    return 0;
}
