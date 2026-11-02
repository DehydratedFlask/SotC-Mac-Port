#include "../core/vtable_resolved_entries.hpp"

#include <cstdio>
#include <string_view>

namespace {

using sotc::find_resolved_vtable_call_site;
using sotc::kResolvedVtableCallSiteCount;
using sotc::kResolvedVtableCallSites;
using sotc::resolved_vtable_target_count;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

void test_table_shape() {
    check("count", kResolvedVtableCallSiteCount == 91);
    check("distinct targets", resolved_vtable_target_count() == 37);
    bool sorted = true;
    for (std::size_t index = 1; index < kResolvedVtableCallSiteCount; ++index) {
        if (kResolvedVtableCallSites[index - 1].site >= kResolvedVtableCallSites[index].site) {
            sorted = false;
        }
    }
    check("sorted by site", sorted);
}

void test_lookup() {
    const auto* first = find_resolved_vtable_call_site(0x1254d);
    check("first found", first != nullptr);
    if (first != nullptr) {
        check("first vtable", first->vtable == 0x15949a0);
        check("first slot", first->slot == 0x88);
        check("first target", first->target == 0x187c0);
    }

    const auto* shared = find_resolved_vtable_call_site(0xa54b7b);
    const auto* shared_second = find_resolved_vtable_call_site(0xa54c92);
    check("shared found", shared != nullptr && shared_second != nullptr);
    check("shared target", shared != nullptr && shared->target == 0xc740);
    check("shared same vtable", shared != nullptr && shared_second != nullptr &&
                                    shared->vtable == shared_second->vtable);

    const auto* getter = find_resolved_vtable_call_site(0x21a695);
    check("getter found", getter != nullptr);
    if (getter != nullptr) {
        check("getter table", getter->vtable == 0x15cf248);
        check("getter slot", getter->slot == 0x68);
        check("getter target", getter->target == 0x9b7b80);
    }

    const auto* builder = find_resolved_vtable_call_site(0x96c533);
    check("builder found", builder != nullptr);
    if (builder != nullptr) {
        check("builder table", builder->vtable == 0x2dd09b0);
        check("builder slot", builder->slot == 0x08);
        check("builder target", builder->target == 0x9804c0);
    }

    check("unknown site", find_resolved_vtable_call_site(0xdeadbeef) == nullptr);
    check("zero site", find_resolved_vtable_call_site(0) == nullptr);
}

void test_known_targets_present() {
    const std::uint64_t expected[] = {0x187c0, 0x13d20, 0x20d60, 0xc740, 0x358c0, 0x2687b0, 0xc2e0,
                                      0xa40e20, 0xa460f0, 0x1bf710, 0xddf4b0, 0xe21420, 0xe1a5c0,
                                      0xe19980, 0xe16a00, 0xe24900, 0xe26f90,
                                      0x318b0, 0x31870, 0x31310, 0x4b0, 0x22a60,
                                      0x13d50, 0x13d60, 0x30e50, 0x488d0, 0xc2c0, 0xe19cb0,
                                      0xe19f70, 0xe1a040, 0xe19440, 0xe52cb0, 0xed4860,
                                      0x9b7b80, 0xd7c4b0, 0x6e92c0, 0x9804c0};
    for (const std::uint64_t target : expected) {
        bool found = false;
        for (std::size_t index = 0; index < kResolvedVtableCallSiteCount; ++index) {
            if (kResolvedVtableCallSites[index].target == target) {
                found = true;
            }
        }
        check("target present", found);
    }
}

}  // namespace

int main() {
    test_table_shape();
    test_lookup();
    test_known_targets_present();

    if (failures == 0) {
        std::printf("OK test_vtable_resolved_entries\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_resolved_entries: %d failure(s)\n", failures);
    return 1;
}
