#include "../core/vtable_15e1ee0_slot_b0_e19980.hpp"

#include <cstdio>
#include <string_view>

namespace {

using sotc::Vtable15e1ee0SlotB0_e19980Args;
using sotc::Vtable15e1ee0SlotB0_e19980Cells;
using sotc::Vtable15e1ee0SlotB0_e19980Hooks;
using sotc::Vtable15e1ee0SlotB0_e19980Path;
using sotc::vtable_15e1ee0_slot_b0_e19980;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

struct Trace {
    int slow_calls = 0;
    int fast_calls = 0;
    void* slow_element = nullptr;
    void* fast_arg3 = nullptr;
    std::uint64_t* canary_to_corrupt = nullptr;
};

Trace g_trace;

void hook_slow(void* element, void*) {
    ++g_trace.slow_calls;
    g_trace.slow_element = element;
    if (g_trace.canary_to_corrupt != nullptr) {
        ++*g_trace.canary_to_corrupt;
    }
}
void hook_fast(void* arg3, void*, void*, void*) {
    ++g_trace.fast_calls;
    g_trace.fast_arg3 = arg3;
}

void test_constants() {
    check("rva", sotc::kVtable15e1ee0SlotB0_e19980Rva == 0xe19980);
    check("size", sotc::kVtable15e1ee0SlotB0_e19980Size == 0xa2);
    check("vtable", sotc::kVtable15e1ee0SlotB0_e19980VtableAddress == 0x15e1ee0);
    check("slot", sotc::kVtable15e1ee0SlotB0_e19980SlotOffset == 0xb0);
    check("reloc", sotc::kVtable15e1ee0SlotB0_e19980RelocAddress == 0x15e1f90);
    check("slow slot", sotc::kVtable15e1ee0SlotB0_e19980SlowSlot == 0xd8);
    check("global", sotc::kVtable15e1ee0SlotB0_e19980FastStoreGlobal == 0x3353478);
}

void test_fast_path() {
    g_trace = Trace{};
    std::uint64_t canary = 0xCAFEF00DULL;
    void* store_target = reinterpret_cast<void*>(0x1111);
    const void* store_value = reinterpret_cast<const void*>(0x2222);
    std::uintptr_t aligned3 = 0x1000;
    std::uintptr_t aligned4 = 0x2000;
    Vtable15e1ee0SlotB0_e19980Args args{};
    args.arg2 = reinterpret_cast<void*>(0x33);
    args.arg3 = reinterpret_cast<void*>(aligned3);
    args.arg4 = reinterpret_cast<void*>(aligned4);
    Vtable15e1ee0SlotB0_e19980Cells cells{};
    cells.canary = &canary;
    cells.fast_store_target = &store_target;
    cells.fast_store_value = &store_value;
    Vtable15e1ee0SlotB0_e19980Hooks hooks{};
    hooks.fast_helper = hook_fast;
    hooks.slow_vcall_d8 = hook_slow;
    const auto result = vtable_15e1ee0_slot_b0_e19980(args, cells, hooks);
    check("fast ok", result.ok);
    check("fast path", result.path == Vtable15e1ee0SlotB0_e19980Path::fast);
    check("fast helper", g_trace.fast_calls == 1 && g_trace.slow_calls == 0);
    check("fast arg3", g_trace.fast_arg3 == args.arg3);
    check("fast stored", result.stored && store_target == const_cast<void*>(store_value));
    check("fast canary", result.canary_checked && !result.canary_mismatch);
}

void test_slow_path_misaligned_arg3() {
    g_trace = Trace{};
    std::uint64_t canary = 1;
    Vtable15e1ee0SlotB0_e19980Args args{};
    args.arg2 = reinterpret_cast<void*>(0x44);
    args.arg3 = reinterpret_cast<void*>(0x1001);  // misaligned
    args.arg4 = reinterpret_cast<void*>(0x2000);
    Vtable15e1ee0SlotB0_e19980Cells cells{};
    cells.canary = &canary;
    Vtable15e1ee0SlotB0_e19980Hooks hooks{};
    hooks.fast_helper = hook_fast;
    hooks.slow_vcall_d8 = hook_slow;
    const auto result = vtable_15e1ee0_slot_b0_e19980(args, cells, hooks);
    check("slow3 path", result.path == Vtable15e1ee0SlotB0_e19980Path::slow);
    check("slow3 call", g_trace.slow_calls == 1 && g_trace.fast_calls == 0);
    check("slow3 element", g_trace.slow_element == args.arg2);
    check("slow3 ok", result.ok);
}

void test_slow_path_misaligned_arg4() {
    g_trace = Trace{};
    std::uint64_t canary = 1;
    Vtable15e1ee0SlotB0_e19980Args args{};
    args.arg3 = reinterpret_cast<void*>(0x1000);
    args.arg4 = reinterpret_cast<void*>(0x2004);  // misaligned
    Vtable15e1ee0SlotB0_e19980Cells cells{};
    cells.canary = &canary;
    Vtable15e1ee0SlotB0_e19980Hooks hooks{};
    hooks.slow_vcall_d8 = hook_slow;
    const auto result = vtable_15e1ee0_slot_b0_e19980(args, cells, hooks);
    check("slow4 path", result.path == Vtable15e1ee0SlotB0_e19980Path::slow);
    check("slow4 ok", result.ok);
}

void test_canary_mismatch_fails() {
    g_trace = Trace{};
    std::uint64_t canary = 5;
    g_trace.canary_to_corrupt = &canary;
    Vtable15e1ee0SlotB0_e19980Args args{};
    args.arg3 = reinterpret_cast<void*>(0x1001);
    Vtable15e1ee0SlotB0_e19980Cells cells{};
    cells.canary = &canary;
    Vtable15e1ee0SlotB0_e19980Hooks hooks{};
    hooks.slow_vcall_d8 = hook_slow;
    const auto result = vtable_15e1ee0_slot_b0_e19980(args, cells, hooks);
    check("canary mismatch fails", !result.ok);
    check("canary mismatch flag", result.canary_mismatch && result.canary_checked);
    g_trace.canary_to_corrupt = nullptr;
}

void test_missing_canary_fails() {
    const auto result = vtable_15e1ee0_slot_b0_e19980();
    check("missing canary fails", !result.ok);
    check("missing canary no path", result.path == Vtable15e1ee0SlotB0_e19980Path::none);
}

void test_path_names() {
    check("name slow", std::string_view(sotc::vtable_15e1ee0_slot_b0_e19980_path_name(
                            Vtable15e1ee0SlotB0_e19980Path::slow)) == "slow");
    check("name fast", std::string_view(sotc::vtable_15e1ee0_slot_b0_e19980_path_name(
                            Vtable15e1ee0SlotB0_e19980Path::fast)) == "fast");
}

}  // namespace

int main() {
    test_constants();
    test_fast_path();
    test_slow_path_misaligned_arg3();
    test_slow_path_misaligned_arg4();
    test_canary_mismatch_fails();
    test_missing_canary_fails();
    test_path_names();

    if (failures == 0) {
        std::printf("OK test_vtable_15e1ee0_slot_b0_e19980\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_15e1ee0_slot_b0_e19980: %d failure(s)\n", failures);
    return 1;
}
