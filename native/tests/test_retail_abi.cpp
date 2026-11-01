#include "../core/retail_abi.hpp"

#include <cstdio>
#include <string_view>

namespace {

using sotc::kRetailArgSpecCount;
using sotc::kRetailArgSpecs;
using sotc::RetailArg;
using sotc::RetailFrame;
using sotc::retail_arg_spec_for;
using sotc::retail_takes_at_least;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

void test_table_shape() {
    check("count", kRetailArgSpecCount == 16);
    bool sorted = true;
    for (std::size_t index = 1; index < kRetailArgSpecCount; ++index) {
        if (kRetailArgSpecs[index - 1].rva >= kRetailArgSpecs[index].rva) {
            sorted = false;
        }
    }
    check("sorted", sorted);
    bool counts_ok = true;
    for (std::size_t index = 0; index < kRetailArgSpecCount; ++index) {
        if (kRetailArgSpecs[index].argument_count > 6) {
            counts_ok = false;
        }
    }
    check("counts within six registers", counts_ok);
}

void test_lookup() {
    const auto* setter = retail_arg_spec_for(0x13d20);
    check("setter found", setter != nullptr);
    check("setter args", setter != nullptr && setter->argument_count == 2 &&
                             !setter->returns_value);

    const auto* no_ret = retail_arg_spec_for(0xa40e20);
    check("no-ret found", no_ret != nullptr && no_ret->argument_count == 0);

    const auto* scan = retail_arg_spec_for(0xe1a5c0);
    check("scan found", scan != nullptr && scan->argument_count == 4 && scan->returns_value);

    check("unknown rva", retail_arg_spec_for(0xdeadbeef) == nullptr);
    check("zero rva", retail_arg_spec_for(0) == nullptr);
}

void test_takes_at_least() {
    check("takes 1", retail_takes_at_least(0x20d60, 1));
    check("does not take 2", !retail_takes_at_least(0x20d60, 2));
    check("takes 5", retail_takes_at_least(0xe19980, 5));
    check("unknown takes none", !retail_takes_at_least(0xdeadbeef, 0));
}

void test_frame() {
    RetailFrame frame{};
    frame.set(RetailArg::rdi, 0x1111);
    frame.set(RetailArg::rsi, 0x2222);
    frame.set(RetailArg::rdx, 0x3333);
    check("frame rdi", frame.get(RetailArg::rdi) == 0x1111);
    check("frame rsi", frame.get(RetailArg::rsi) == 0x2222);
    check("frame rdx", frame.get(RetailArg::rdx) == 0x3333);
    check("frame rcx default", frame.get(RetailArg::rcx) == 0);
    check("frame rax default", frame.rax == 0);
}

}  // namespace

int main() {
    test_table_shape();
    test_lookup();
    test_takes_at_least();
    test_frame();

    if (failures == 0) {
        std::printf("OK test_retail_abi\n");
        return 0;
    }
    std::fprintf(stderr, "test_retail_abi: %d failure(s)\n", failures);
    return 1;
}
