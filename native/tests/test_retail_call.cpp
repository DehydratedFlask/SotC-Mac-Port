#include "../core/retail_call.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string_view>
#include <vector>

namespace {

using sotc::AllocDispatchContract;
using sotc::call_resolved;
using sotc::GuestWindow;
using sotc::RetailArg;
using sotc::RetailCallContext;
using sotc::RetailCallStatus;
using sotc::RetailFrame;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

struct Trace {
    int tails = 0;
    int frees = 0;
    int pre_calls = 0;
};

Trace g_trace;

void hook_tail(void*, void*) { ++g_trace.tails; }
void hook_pre(void*) { ++g_trace.pre_calls; }
void hook_pre_2687(void*) { ++g_trace.pre_calls; }
void hook_pre_1bf7(void*) { ++g_trace.pre_calls; }
bool deallocate(void*, std::uint32_t) {
    ++g_trace.frees;
    return true;
}

void test_20d60_success() {
    g_trace = Trace{};
    std::vector<std::uint8_t> storage(0x80, 0);
    GuestWindow window(storage.data(), 0x10000, storage.size());
    std::uint64_t global_value = 0xABCD;
    std::memcpy(storage.data() + 0x40, &global_value, sizeof(global_value));

    RetailFrame frame{};
    frame.set(RetailArg::rdi, 0x10000);
    RetailCallContext context{};
    context.tail_20d60 = hook_tail;
    context.global_20d60 = 0x10040;
    const auto outcome = call_resolved(sotc::kVtable1594dc0Slot20_20d60Rva, frame, window, context);
    check("20d60 ok", outcome.ok && outcome.status == RetailCallStatus::ok);
    check("20d60 rax", outcome.rax == 0 && frame.rax == 0);
    check("20d60 flag", storage[0x18] == 1);
    check("20d60 tail", g_trace.tails == 1);
    check("20d60 name", std::string_view(sotc::retail_call_status_name(outcome.status)) == "ok");
}

void test_13d20_writes_guest_dword() {
    std::vector<std::uint8_t> storage(0x80, 0);
    GuestWindow window(storage.data(), 0x20000, storage.size());
    RetailFrame frame{};
    frame.set(RetailArg::rdi, 0x20000);
    frame.set(RetailArg::rsi, 0xDEADBEEF);
    const auto outcome = call_resolved(sotc::kVtable1594ae8Slot28_13d20Rva, frame, window);
    check("13d20 ok", outcome.ok);
    std::uint32_t stored = 0;
    std::memcpy(&stored, storage.data() + 0x1c, sizeof(stored));
    check("13d20 stored", stored == 0xDEADBEEF);
}

void test_358c0_copies_source() {
    std::vector<std::uint8_t> storage(0x100, 0);
    GuestWindow window(storage.data(), 0x30000, storage.size());
    std::uint64_t source_value = 0x1122334455667788ULL;
    std::memcpy(storage.data() + 0x90, &source_value, sizeof(source_value));
    RetailFrame frame{};
    frame.set(RetailArg::rdi, 0x30000);
    frame.set(RetailArg::rsi, 0x30080);
    const auto outcome = call_resolved(sotc::kVtable15955c0Slot08_358c0Rva, frame, window);
    check("358c0 ok", outcome.ok);
    std::uint64_t copy = 0;
    std::memcpy(&copy, storage.data() + 0x10, sizeof(copy));
    check("358c0 copy", copy == source_value);
    std::uint64_t table = 0;
    std::memcpy(&table, storage.data() + 0x20, sizeof(table));
    check("358c0 table", table == sotc::kVtable15955c0Slot08_358c0InstalledTable);
}

void test_187c0_requires_alloc() {
    g_trace = Trace{};
    std::vector<std::uint8_t> storage(0x80, 0);
    GuestWindow window(storage.data(), 0x40000, storage.size());
    RetailFrame frame{};
    frame.set(RetailArg::rdi, 0x40000);

    const auto without = call_resolved(sotc::kVtable15949a0Slot88_187c0Rva, frame, window);
    check("187c0 no alloc fails", !without.ok &&
                                      without.status == RetailCallStatus::model_failed);

    AllocDispatchContract contract{};
    contract.deallocate = deallocate;
    RetailCallContext context{};
    context.pre_187c0 = hook_pre;
    context.alloc = &contract;
    const auto with = call_resolved(sotc::kVtable15949a0Slot88_187c0Rva, frame, window, context);
    check("187c0 with alloc ok", with.ok);
    check("187c0 hooks", g_trace.pre_calls == 1 && g_trace.frees == 1);
}

void test_one_arg_teardowns() {
    g_trace = Trace{};
    std::vector<std::uint8_t> storage(0x80, 0);
    GuestWindow window(storage.data(), 0x60000, storage.size());
    RetailFrame frame{};
    frame.set(RetailArg::rdi, 0x60000);
    AllocDispatchContract contract{};
    contract.deallocate = deallocate;

    const auto without_2687 = call_resolved(sotc::kVtable159c908Slot58_2687b0Rva, frame, window);
    check("2687 no alloc fails", !without_2687.ok &&
                                     without_2687.status == RetailCallStatus::model_failed);

    RetailCallContext context{};
    context.pre_2687b0 = hook_pre_2687;
    context.pre_1bf710 = hook_pre_1bf7;
    context.alloc = &contract;
    const auto with_2687 = call_resolved(sotc::kVtable159c908Slot58_2687b0Rva, frame, window,
                                         context);
    check("2687 ok", with_2687.ok);
    const auto with_1bf7 = call_resolved(sotc::kVtable1598e10Slot18_1bf710Rva, frame, window,
                                         context);
    check("1bf710 ok", with_1bf7.ok);
    check("teardown hooks", g_trace.pre_calls == 2 && g_trace.frees == 2);

    frame.set(RetailArg::rdi, 0x90000);
    const auto out_of_range = call_resolved(sotc::kVtable159c908Slot58_2687b0Rva, frame, window,
                                            context);
    check("2687 range fails", !out_of_range.ok &&
                                  out_of_range.status == RetailCallStatus::out_of_range);
}

void test_copy_targets_and_noop() {
    std::vector<std::uint8_t> storage(0x120, 0);
    GuestWindow window(storage.data(), 0x70000, storage.size());
    std::uint64_t source_value = 0xCAFEBABE12345678ULL;
    std::memcpy(storage.data() + 0x110, &source_value, sizeof(source_value));
    RetailFrame frame{};
    frame.set(RetailArg::rdi, 0x70000);
    frame.set(RetailArg::rsi, 0x70100);

    const auto c2e0 = call_resolved(sotc::kVtable15d1678SlotE8_c2e0Rva, frame, window);
    check("c2e0 ok", c2e0.ok);
    std::uint64_t c2e0_copy = 0;
    std::memcpy(&c2e0_copy, storage.data() + 0x10, sizeof(c2e0_copy));
    check("c2e0 copied", c2e0_copy == source_value);

    const auto c740 = call_resolved(sotc::kVtable15d1d80Slot08_c740Rva, frame, window);
    check("c740 ok", c740.ok);
    std::uint64_t c740_table = 0;
    std::memcpy(&c740_table, storage.data() + 0x20, sizeof(c740_table));
    check("c740 table", c740_table == sotc::kVtable15d1d80Slot08_c740InstalledTable);

    const auto noop = call_resolved(sotc::kVtable15d1678Slot68_a40e20Rva, frame, window);
    check("noop ok", noop.ok && noop.rax == 0);
}

void hook_slot18(void*, void*) {}
void hook_slot20(void*, void*) {}
void hook_pre_ddf(void* self) { (void)self; }
void* g_manager = reinterpret_cast<void*>(0x1234);
void* fetch_manager() { return g_manager; }
void hook_tail_ddf(void*, void*) {}

void test_dual_vcall_and_manager() {
    std::vector<std::uint8_t> storage(0x100, 0);
    GuestWindow window(storage.data(), 0x80000, storage.size());
    const void* vtable = reinterpret_cast<const void*>(0x50000);
    std::memcpy(storage.data(), &vtable, sizeof(vtable));
    RetailFrame frame{};
    frame.set(RetailArg::rdi, 0x80000);
    frame.set(RetailArg::rsi, 0x80040);
    RetailCallContext context{};
    context.slot18_e16a00 = hook_slot18;
    context.slot20_e16a00 = hook_slot20;
    const auto dual = call_resolved(sotc::kVtable15f2f08Slot38_e16a00Rva, frame, window, context);
    check("e16a00 ok", dual.ok);

    RetailCallContext manager_context{};
    manager_context.pre_ddf4b0 = hook_pre_ddf;
    manager_context.fetch_manager_ddf4b0 = fetch_manager;
    manager_context.tail_ddf4b0 = hook_tail_ddf;
    const auto manager = call_resolved(sotc::kVtable15e0b48Slot28_ddf4b0Rva, frame, window,
                                       manager_context);
    check("ddf4b0 ok", manager.ok);

    RetailCallContext missing_manager{};
    missing_manager.pre_ddf4b0 = hook_pre_ddf;
    const auto missing = call_resolved(sotc::kVtable15e0b48Slot28_ddf4b0Rva, frame, window,
                                       missing_manager);
    check("ddf4b0 no manager fails", !missing.ok &&
                                         missing.status == RetailCallStatus::model_failed);

    frame.set(RetailArg::rdi, 0x90000);
    const auto range = call_resolved(sotc::kVtable15f2f08Slot38_e16a00Rva, frame, window, context);
    check("e16a00 range fails", !range.ok && range.status == RetailCallStatus::out_of_range);
}

void hook_helper_a46(void*) {}
void hook_post_a46(void*) {}

void test_a460f0_teardown() {
    std::vector<std::uint8_t> storage(0x100, 0);
    GuestWindow window(storage.data(), 0xA0000, storage.size());
    const void* vtable = reinterpret_cast<const void*>(0x66000);
    const void* helper = reinterpret_cast<const void*>(0x66010);
    std::memcpy(storage.data(), &vtable, sizeof(vtable));
    std::memcpy(storage.data() + 0xc0, &helper, sizeof(helper));

    RetailFrame frame{};
    frame.set(RetailArg::rdi, 0xA0000);
    const auto without_alloc = call_resolved(sotc::kVtable15d1678Slot280_a460f0Rva, frame, window);
    check("a460f0 no alloc fails", !without_alloc.ok &&
                                       without_alloc.status == RetailCallStatus::model_failed);

    AllocDispatchContract contract{};
    contract.deallocate = deallocate;
    RetailCallContext context{};
    context.helper_a460f0 = hook_helper_a46;
    context.post_a460f0 = hook_post_a46;
    context.alloc = &contract;
    g_trace = Trace{};
    const auto ok = call_resolved(sotc::kVtable15d1678Slot280_a460f0Rva, frame, window, context);
    check("a460f0 ok", ok.ok);
    check("a460f0 free", g_trace.frees == 1);
    std::uint64_t installed = 0;
    std::memcpy(&installed, storage.data(), sizeof(installed));
    check("a460f0 vtable installed", installed == sotc::kVtable15d1678Slot280_a460f0InstalledTable);

    frame.set(RetailArg::rdi, 0xB0000);
    const auto range = call_resolved(sotc::kVtable15d1678Slot280_a460f0Rva, frame, window, context);
    check("a460f0 range fails", !range.ok && range.status == RetailCallStatus::out_of_range);
}

void* slot28_e249(void*) { return reinterpret_cast<void*>(0x1234); }
std::uint32_t slot48_e249(void*) { return 7; }
void* sub40_e249(void*, void*, std::uint32_t) { return reinterpret_cast<void*>(0x5678); }
std::int32_t status_e249(void*, void*) { return 0; }
void release_e249(void*) {}

void test_e24900_sub_dispatch() {
    std::vector<std::uint8_t> storage(0x80, 0);
    GuestWindow window(storage.data(), 0xC0000, storage.size());
    std::uint64_t sub = 0xC0040;
    std::memcpy(storage.data() + 0x08, &sub, sizeof(sub));

    RetailFrame frame{};
    frame.set(RetailArg::rdi, 0xC0000);
    frame.set(RetailArg::rsi, 0xC0020);
    RetailCallContext context{};
    context.slot28_e24900 = slot28_e249;
    context.slot48_e24900 = slot48_e249;
    context.sub40_e24900 = sub40_e249;
    context.status_e24900 = status_e249;
    context.release_e24900 = release_e249;
    const auto zero = call_resolved(sotc::kVtable15e2d60SlotD0_e24900Rva, frame, window, context);
    check("e24900 ok", zero.ok);
    check("e24900 rax", zero.rax == 0);
    std::uint64_t stored = 0;
    std::memcpy(&stored, storage.data() + 0x20, sizeof(stored));
    check("e24900 cleared", stored == 0);

    const auto range = call_resolved(sotc::kVtable15e2d60SlotD0_e24900Rva, frame, window);
    check("e24900 missing hooks fail", !range.ok &&
                                           range.status == RetailCallStatus::model_failed);
}

void test_failures() {
    std::vector<std::uint8_t> storage(0x40, 0);
    GuestWindow window(storage.data(), 0x50000, storage.size());
    RetailFrame frame{};

    const auto unknown = call_resolved(0xdeadbeef, frame, window);
    check("unknown target", !unknown.ok && unknown.status == RetailCallStatus::unknown_target);

    frame.set(RetailArg::rdi, 0x90000);
    const auto range = call_resolved(sotc::kVtable1594dc0Slot20_20d60Rva, frame, window);
    check("range fails", !range.ok && range.status == RetailCallStatus::out_of_range);

    GuestWindow empty;
    const auto nowhere = call_resolved(sotc::kVtable1594dc0Slot20_20d60Rva, frame, empty);
    check("no window fails", !nowhere.ok && nowhere.status == RetailCallStatus::no_window);

    frame.set(RetailArg::rdi, 0x50000);
    const auto adapted_but_no_cells = call_resolved(0xe19980, frame, window);
    check("abi without adapter", !adapted_but_no_cells.ok &&
                                      adapted_but_no_cells.status ==
                                          RetailCallStatus::unknown_target);
}

}  // namespace

int main() {
    test_20d60_success();
    test_13d20_writes_guest_dword();
    test_358c0_copies_source();
    test_187c0_requires_alloc();
    test_one_arg_teardowns();
    test_copy_targets_and_noop();
    test_dual_vcall_and_manager();
    test_a460f0_teardown();
    test_e24900_sub_dispatch();
    test_failures();

    if (failures == 0) {
        std::printf("OK test_retail_call\n");
        return 0;
    }
    std::fprintf(stderr, "test_retail_call: %d failure(s)\n", failures);
    return 1;
}
