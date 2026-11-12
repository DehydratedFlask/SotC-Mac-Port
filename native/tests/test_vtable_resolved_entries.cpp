#include "../core/vtable_resolved_entries.hpp"

#include <cstdio>
#include <string_view>

namespace {

using sotc::find_corrected_vptr_read;
using sotc::find_resolved_vtable_call_site;
using sotc::kCorrectedVptrReadCount;
using sotc::kCorrectedVptrReads;
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
    check("count", kResolvedVtableCallSiteCount == 83);
    check("distinct targets", resolved_vtable_target_count() == 32);
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

    // E1095: 0x113c5 reads TABLE(0x1595b60)+0x10+0x18 = 0x1595b88, i.e. slot 0x28, not 0x488d0.
    const auto* corrected = find_resolved_vtable_call_site(0x113c5);
    check("corrected found", corrected != nullptr);
    if (corrected != nullptr) {
        check("corrected table", corrected->vtable == 0x1595b60);
        check("corrected slot", corrected->slot == 0x28);
        check("corrected target", corrected->target == 0x13d20);
    }
    // The sibling site-provable row in the neighbouring table reads the same slot and target.
    const auto* sibling = find_resolved_vtable_call_site(0x11452);
    check("sibling found", sibling != nullptr && sibling->slot == 0x28 && sibling->target == 0x13d20);

    // The eight withdrawn rows must not resolve (their real targets have no body model).
    const std::uint64_t withdrawn[] = {0x113d6, 0x11462, 0x312ce, 0x312de, 0x312f0, 0x3133e, 0x3134e, 0x31360};
    for (const std::uint64_t site : withdrawn) {
        check("withdrawn site absent", find_resolved_vtable_call_site(site) == nullptr);
    }

    check("unknown site", find_resolved_vtable_call_site(0xdeadbeef) == nullptr);
    check("zero site", find_resolved_vtable_call_site(0) == nullptr);
}

void test_known_targets_present() {
    const std::uint64_t expected[] = {0x187c0, 0x13d20, 0x20d60, 0xc740, 0x358c0, 0x2687b0, 0xc2e0,
                                      0xa40e20, 0xa460f0, 0x1bf710, 0xddf4b0, 0xe21420, 0xe1a5c0,
                                      0xe19980, 0xe16a00, 0xe24900, 0xe26f90,
                                      0x4b0, 0x22a60,
                                      0x13d50, 0x30e50, 0xc2c0, 0xe19cb0,
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

void test_corrected_vptr_reads() {
    check("audit count", kCorrectedVptrReadCount == 9);
    std::size_t kept = 0;
    for (std::size_t index = 0; index < kCorrectedVptrReadCount; ++index) {
        const auto& row = kCorrectedVptrReads[index];
        // The call reads the object's vptr (TABLE + 0x10) plus the displacement.
        check("read address", row.read_address == row.old_vtable + 0x10 + row.old_slot);
        check("target moved", row.real_target != row.old_target);
        const auto* live = find_resolved_vtable_call_site(row.site);
        check("row_kept matches table", (live != nullptr) == row.row_kept);
        if (row.row_kept) {
            ++kept;
            check("kept row reads the real address",
                  live != nullptr && live->vtable + live->slot == row.read_address &&
                      live->target == row.real_target);
        }
        check("lookup", find_corrected_vptr_read(row.site) == &row);
    }
    check("only 0x113c5 kept", kept == 1);
    check("audit unknown", find_corrected_vptr_read(0x11452) == nullptr);
}

}  // namespace

int main() {
    test_table_shape();
    test_lookup();
    test_known_targets_present();
    test_corrected_vptr_reads();

    if (failures == 0) {
        std::printf("OK test_vtable_resolved_entries\n");
        return 0;
    }
    std::fprintf(stderr, "test_vtable_resolved_entries: %d failure(s)\n", failures);
    return 1;
}
