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

std::uint64_t slot78_e26(void*) { return 0x99; }
std::int32_t g_e26_status = 0;
std::int32_t slot90_e26(void*, const char* name, void*) {
    return name != nullptr ? g_e26_status : -1;
}
std::int32_t helper_e26(void*, void*, std::int32_t, void*, void*) { return 21; }

void test_e26f90_status_gate() {
    std::vector<std::uint8_t> storage(0x60, 0);
    GuestWindow window(storage.data(), 0xD0000, storage.size());
    std::uint64_t canary = 7;
    char buffer[16] = {};
    int local = 0;

    RetailFrame frame{};
    frame.set(RetailArg::rdi, 0xD0000);
    frame.set(RetailArg::rsi, 0xD0020);
    frame.set(RetailArg::rdx, 5);
    RetailCallContext context{};
    context.slot78_e26f90 = slot78_e26;
    context.slot90_e26f90 = slot90_e26;
    context.helper_e26f90 = helper_e26;
    context.host_canary = &canary;
    context.host_buffer = buffer;
    context.host_local = &local;

    g_e26_status = 0;
    const auto helper_path = call_resolved(sotc::kVtable15ed2a0SlotC0_e26f90Rva, frame, window,
                                           context);
    check("e26f90 helper path", helper_path.ok && helper_path.rax == 21);

    g_e26_status = 3;
    const auto early = call_resolved(sotc::kVtable15ed2a0SlotC0_e26f90Rva, frame, window, context);
    check("e26f90 early zero", early.ok && early.rax == 0);

    RetailCallContext no_scratch{};
    no_scratch.slot78_e26f90 = slot78_e26;
    no_scratch.slot90_e26f90 = slot90_e26;
    const auto missing = call_resolved(sotc::kVtable15ed2a0SlotC0_e26f90Rva, frame, window,
                                       no_scratch);
    check("e26f90 no scratch fails", !missing.ok &&
                                         missing.status == RetailCallStatus::out_of_range);
    g_e26_status = 0;
}

struct Element {
    const char* key;
    std::int32_t value;
};

const char* element_key_e1a(void* element) { return static_cast<Element*>(element)->key; }
std::int32_t element_value_e1a(void* element) { return static_cast<Element*>(element)->value; }
std::int32_t match_e1a(void*) { return 9; }

void test_e1a5c0_scan() {
    std::vector<std::uint8_t> storage(0x200, 0);
    GuestWindow window(storage.data(), 0xE0000, storage.size());
    Element first{"alpha", 1};
    Element second{"beta", 3};
    std::uint64_t array = 0xE00A0;
    std::uint64_t elements[2] = {reinterpret_cast<std::uint64_t>(&first),
                                 reinterpret_cast<std::uint64_t>(&second)};
    std::memcpy(storage.data() + 0x08, &array, sizeof(array));
    std::memcpy(storage.data() + 0xA0, elements, sizeof(elements));
    std::uint32_t count = 2;
    std::memcpy(storage.data() + 0x10, &count, sizeof(count));
    std::memcpy(storage.data() + 0x40, "beta", 5);

    RetailFrame frame{};
    frame.set(RetailArg::rdi, 0xE0000);
    frame.set(RetailArg::rsi, 0xE0040);
    frame.set(RetailArg::rdx, 3);
    RetailCallContext context{};
    context.element_key_e1a5c0 = element_key_e1a;
    context.element_value_e1a5c0 = element_value_e1a;
    context.element_match_e1a5c0 = match_e1a;
    const auto found = call_resolved(sotc::kVtable15e2028Slot70_e1a5c0Rva, frame, window, context);
    check("e1a5c0 found", found.ok && found.rax == 9);

    frame.set(RetailArg::rdx, 4);  // right key, wrong value
    const auto miss = call_resolved(sotc::kVtable15e2028Slot70_e1a5c0Rva, frame, window, context);
    check("e1a5c0 value mismatch", miss.ok && miss.rax == 1);

    std::memcpy(storage.data() + 0x40, "gamma", 6);
    frame.set(RetailArg::rdx, 3);
    const auto key_miss = call_resolved(sotc::kVtable15e2028Slot70_e1a5c0Rva, frame, window,
                                        context);
    check("e1a5c0 key mismatch", key_miss.ok && key_miss.rax == 1);

    frame.set(RetailArg::rsi, 0);
    const auto null_key = call_resolved(sotc::kVtable15e2028Slot70_e1a5c0Rva, frame, window,
                                        context);
    check("e1a5c0 null key", null_key.ok && null_key.rax == 1);
}

int g_slow_calls = 0;
int g_fast_calls = 0;
std::uint64_t* g_tamper_canary = nullptr;
void slow_e199(void*, void*) {
    ++g_slow_calls;
    if (g_tamper_canary != nullptr) {
        ++*g_tamper_canary;
    }
}
void fast_e199(void*, void*, void*, void*) { ++g_fast_calls; }

void test_e19980_paths() {
    std::vector<std::uint8_t> storage(0x120, 0);
    GuestWindow window(storage.data(), 0xF0000, storage.size());
    std::uint64_t canary = 11;
    const void* store_value = reinterpret_cast<const void*>(0x7777);
    char buffer[16] = {};

    RetailFrame frame{};
    frame.set(RetailArg::rdi, 0xF0000);
    frame.set(RetailArg::rsi, 0xF0020);
    frame.set(RetailArg::rdx, 0xF0040);  // aligned store target
    frame.set(RetailArg::rcx, 0xF0060);  // aligned
    frame.set(RetailArg::r8, 0xF0080);
    RetailCallContext context{};
    context.slow_e19980 = slow_e199;
    context.fast_e19980 = fast_e199;
    context.slow_buffer_e19980 = buffer;
    context.host_canary_e19980 = &canary;
    context.store_value_e19980 = &store_value;

    g_slow_calls = 0;
    g_fast_calls = 0;
    const auto fast = call_resolved(sotc::kVtable15e1ee0SlotB0_e19980Rva, frame, window, context);
    check("e19980 fast ok", fast.ok && g_fast_calls == 1 && g_slow_calls == 0);
    std::uint64_t stored = 0;
    std::memcpy(&stored, storage.data() + 0x40, sizeof(stored));
    check("e19980 fast store", stored == 0x7777);

    frame.set(RetailArg::rdx, 0xF0044);  // misaligned arg3 -> slow path
    g_slow_calls = 0;
    g_fast_calls = 0;
    const auto slow3 = call_resolved(sotc::kVtable15e1ee0SlotB0_e19980Rva, frame, window,
                                     context);
    check("e19980 slow arg3", slow3.ok && g_slow_calls == 1 && g_fast_calls == 0);

    frame.set(RetailArg::rdx, 0xF0040);
    frame.set(RetailArg::rcx, 0xF0064);  // misaligned arg4 -> slow path
    g_slow_calls = 0;
    const auto slow4 = call_resolved(sotc::kVtable15e1ee0SlotB0_e19980Rva, frame, window,
                                     context);
    check("e19980 slow arg4", slow4.ok && g_slow_calls == 1);

    frame.set(RetailArg::rcx, 0xF0064);  // slow path, and tamper with the canary inside the hook
    g_tamper_canary = &canary;
    const auto tampered = call_resolved(sotc::kVtable15e1ee0SlotB0_e19980Rva, frame, window,
                                        context);
    g_tamper_canary = nullptr;
    check("e19980 canary tamper", !tampered.ok &&
                                      tampered.status == RetailCallStatus::model_failed);
}

struct Provider21420 {
    void* object = nullptr;
    std::int32_t state = 0x0e;
    std::uint64_t kind = 1;
};

void* lookup_e214(void* provider, const char*) {
    return static_cast<Provider21420*>(provider)->object;
}
std::int32_t state_e214(void* object) { return static_cast<Provider21420*>(object)->state; }
std::uint64_t kind_e214(void* object) { return static_cast<Provider21420*>(object)->kind; }
const char* g_names[2] = {"alpha", "beta"};
std::int32_t count_e214(void*) { return 2; }
const char* name_e214(void*, std::int32_t index) {
    return g_names[index >= 0 && index < 2 ? index : 0];
}
void missing_e214(void*) {}
void found_e214(void*, std::int32_t) {}

void test_e21420_decision() {
    std::vector<std::uint8_t> storage(0x120, 0);
    GuestWindow window(storage.data(), 0x100000, storage.size());
    Provider21420 provider{};
    provider.object = &provider;
    std::uint64_t provider_guest = reinterpret_cast<std::uint64_t>(&provider);
    std::memcpy(storage.data() + 0x48, &provider_guest, sizeof(provider_guest));
    std::memcpy(storage.data() + 0x90, "beta", 5);

    RetailFrame frame{};
    frame.set(RetailArg::rdi, 0x100000);
    frame.set(RetailArg::rsi, 0x100090);
    RetailCallContext context{};
    context.lookup_e21420 = lookup_e214;
    context.state_e21420 = state_e214;
    context.kind_e21420 = kind_e214;
    context.count_e21420 = count_e214;
    context.name_e21420 = name_e214;
    context.missing_e21420 = missing_e214;
    context.found_e21420 = found_e214;

    const auto found = call_resolved(sotc::kVtable15e2028Slot2A0_e21420Rva, frame, window,
                                     context);
    check("e21420 found", found.ok && found.rax == 0);

    provider.state = 0x0d;
    const auto state_mismatch = call_resolved(sotc::kVtable15e2028Slot2A0_e21420Rva, frame, window,
                                              context);
    check("e21420 state mismatch", state_mismatch.ok && state_mismatch.rax == 0x13);

    provider.state = 0x0e;
    provider.kind = 2;
    const auto kind_mismatch = call_resolved(sotc::kVtable15e2028Slot2A0_e21420Rva, frame, window,
                                             context);
    check("e21420 kind mismatch", kind_mismatch.ok && kind_mismatch.rax == 0x1b);

    provider.kind = 1;
    std::memcpy(storage.data() + 0x90, "gamma", 6);
    const auto not_found = call_resolved(sotc::kVtable15e2028Slot2A0_e21420Rva, frame, window,
                                         context);
    check("e21420 not found", not_found.ok && not_found.rax == 0x14);

    frame.set(RetailArg::rsi, 0);
    const auto null_name = call_resolved(sotc::kVtable15e2028Slot2A0_e21420Rva, frame, window,
                                         context);
    check("e21420 null name", null_name.ok && null_name.rax == 0x14);
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

    // Every ABI row now has an adapter; a bare unknown address is the only remaining gap.
    frame.set(RetailArg::rdi, 0x50000);
    const auto not_in_table = call_resolved(0x12345678, frame, window);
    check("rva without abi row", !not_in_table.ok &&
                                     not_in_table.status == RetailCallStatus::unknown_target);
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
    test_e26f90_status_gate();
    test_e1a5c0_scan();
    test_e19980_paths();
    test_e21420_decision();
    test_failures();

    if (failures == 0) {
        std::printf("OK test_retail_call\n");
        return 0;
    }
    std::fprintf(stderr, "test_retail_call: %d failure(s)\n", failures);
    return 1;
}
