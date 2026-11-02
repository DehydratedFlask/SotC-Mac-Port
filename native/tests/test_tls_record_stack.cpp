#include "../core/tls_record_stack.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string_view>

namespace {

using sotc::GuestThreads;
using sotc::TlsRecordStatus;
using sotc::tls_record_pop;
using sotc::tls_record_push;
using sotc::tls_alloc_site_peek_top;
using sotc::tls_alloc_site_push;
using sotc::tls_record_peek_top;
using sotc::TlsAllocSite;
using sotc::tls_record_read;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

void test_constants() {
    check("count offset", sotc::kTlsRecordCountOffset == 0x400);
    check("stride", sotc::kTlsRecordStride == 0x20);
    check("capacity", sotc::kTlsRecordCapacity == 32);
    check("sizes", sotc::kTlsRecordPush18000Size == 0x4d && sotc::kTlsRecordPop18000Size == 0x1f);
    check("capacity fits the block",
          sotc::kTlsRecordCountOffset + sizeof(std::uint32_t) <= GuestThreads::kBlockSize);
}

void test_push_layout_and_count_order() {
    GuestThreads threads;
    (void)threads.create(1);
    const std::uint8_t blob[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    const auto first = tls_record_push(threads, 1, 0xAAAABBBBCCCCDDDDULL, blob, 0x11223344);
    check("push ok", first.ok && first.status == TlsRecordStatus::ok);
    check("push index", first.push.index == 0 && first.push.count_after == 1);

    std::uint32_t count = 0;
    check("count written", threads.read_u32(1, sotc::kTlsRecordCountOffset, count) ==
                               sotc::TlsStatus::ok && count == 1);

    const auto read = tls_record_read(threads, 1, 0);
    check("read ok", read.ok && read.arg0 == 0xAAAABBBBCCCCDDDDULL);
    check("field at 0x18", read.field == 0x11223344);
    check("blob at 0x08", std::memcmp(read.blob, blob, sizeof(blob)) == 0);

    // Second record lands at +0x20 and does not disturb the first.
    const std::uint8_t second_blob[16] = {};
    const auto second = tls_record_push(threads, 1, 0x11ULL, second_blob, 0x55);
    check("second index", second.push.index == 1 && second.push.count_after == 2);
    const auto first_again = tls_record_read(threads, 1, 0);
    check("first unchanged", first_again.arg0 == 0xAAAABBBBCCCCDDDDULL &&
                                 first_again.field == 0x11223344);
    const auto second_read = tls_record_read(threads, 1, 1);
    check("second stored", second_read.ok && second_read.arg0 == 0x11 && second_read.field == 0x55);
}

void test_pop() {
    GuestThreads threads;
    (void)threads.create(2);
    const std::uint8_t blob[16] = {};
    (void)tls_record_push(threads, 2, 1, blob, 2);
    (void)tls_record_push(threads, 2, 3, blob, 4);
    const auto first_pop = tls_record_pop(threads, 2);
    check("pop ok", first_pop.ok && first_pop.count_after == 1);
    const auto second_pop = tls_record_pop(threads, 2);
    check("second pop", second_pop.ok && second_pop.count_after == 0);
    const auto underflow = tls_record_pop(threads, 2);
    check("pop at zero refused", !underflow.ok && underflow.status == TlsRecordStatus::out_of_range);
}

void test_capacity_refused() {
    GuestThreads threads;
    (void)threads.create(3);
    const std::uint8_t blob[16] = {};
    for (std::uint32_t i = 0; i < sotc::kTlsRecordCapacity; ++i) {
        const auto result = tls_record_push(threads, 3, i, blob, i);
        if (!result.ok) {
            check("capacity fill", false);
            return;
        }
    }
    const auto overflow = tls_record_push(threads, 3, 99, blob, 99);
    check("overflow refused", !overflow.ok && overflow.status == TlsRecordStatus::capacity);
    std::uint32_t count = 0;
    check("count untouched", threads.read_u32(3, sotc::kTlsRecordCountOffset, count) ==
                                 sotc::TlsStatus::ok && count == sotc::kTlsRecordCapacity);
    const auto last = tls_record_read(threads, 3, sotc::kTlsRecordCapacity - 1);
    check("last record intact", last.ok && last.arg0 == sotc::kTlsRecordCapacity - 1);
}

void test_unknown_thread_and_isolation() {
    GuestThreads threads;
    const std::uint8_t blob[16] = {};
    const auto missing = tls_record_push(threads, 42, 1, blob, 2);
    check("unknown thread", !missing.ok && missing.status == TlsRecordStatus::no_thread);
    check("unknown read", tls_record_read(threads, 42, 0).status == TlsRecordStatus::no_thread);
    check("unknown pop", tls_record_pop(threads, 42).status == TlsRecordStatus::no_thread);

    (void)threads.create(4);
    (void)threads.create(5);
    (void)tls_record_push(threads, 4, 0xDEAD, blob, 1);
    std::uint32_t count = 7;
    check("other thread untouched", threads.read_u32(5, sotc::kTlsRecordCountOffset, count) ==
                                        sotc::TlsStatus::ok && count == 0);
}

void test_peek_top_matches_the_consumer_idiom() {
    GuestThreads threads;
    (void)threads.create(6);
    check("empty stack", tls_record_peek_top(threads, 6).status == TlsRecordStatus::empty);

    const std::uint8_t blob[16] = {};
    (void)tls_record_push(threads, 6, 0x100, blob, 1);
    const auto first_top = tls_record_peek_top(threads, 6);
    check("first top", first_top.ok && first_top.arg0 == 0x100 && first_top.field == 1);

    (void)tls_record_push(threads, 6, 0x200, blob, 2);
    const auto second_top = tls_record_peek_top(threads, 6);
    check("second top", second_top.ok && second_top.arg0 == 0x200 && second_top.field == 2);

    // Peeking does not consume: the count is unchanged and pop returns the same record.
    std::uint32_t count = 0;
    (void)threads.read_u32(6, sotc::kTlsRecordCountOffset, count);
    check("peek is non-destructive", count == 2);
    (void)tls_record_pop(threads, 6);
    const auto after_pop = tls_record_peek_top(threads, 6);
    check("after pop", after_pop.ok && after_pop.arg0 == 0x100);
    (void)tls_record_pop(threads, 6);
    check("empty again", tls_record_peek_top(threads, 6).status == TlsRecordStatus::empty);
    check("unknown thread peek",
          tls_record_peek_top(threads, 99).status == TlsRecordStatus::no_thread);
}

void test_alloc_site_frames() {
    GuestThreads threads;
    (void)threads.create(7);
    check("empty site stack", tls_alloc_site_peek_top(threads, 7).status == TlsRecordStatus::empty);

    TlsAllocSite outer{};
    outer.owner = 0x1000;
    outer.function = 0x2000;
    outer.file = 0x3000;
    outer.line = 605;
    check("push outer", tls_alloc_site_push(threads, 7, outer).ok);
    const auto first = tls_alloc_site_peek_top(threads, 7);
    check("peek outer", first.ok && first.site.owner == 0x1000 && first.site.function == 0x2000 &&
                            first.site.file == 0x3000 && first.site.line == 605);

    TlsAllocSite inner{};
    inner.owner = 0x4000;
    inner.function = 0x5000;
    inner.file = 0x6000;
    inner.line = 46;
    check("push inner", tls_alloc_site_push(threads, 7, inner).ok);
    const auto top = tls_alloc_site_peek_top(threads, 7);
    check("peek inner", top.ok && top.site.owner == 0x4000 && top.site.file == 0x6000 &&
                            top.site.line == 46);
    // The peek is the "current allocation context": popping restores the outer frame.
    (void)tls_record_pop(threads, 7);
    const auto restored = tls_alloc_site_peek_top(threads, 7);
    check("restored outer", restored.ok && restored.site.owner == 0x1000 &&
                                restored.site.line == 605);
    check("unknown thread site",
          tls_alloc_site_peek_top(threads, 8).status == TlsRecordStatus::no_thread);
}

void test_status_names() {
    check("name ok", std::string_view(sotc::tls_record_status_name(TlsRecordStatus::ok)) == "ok");
    check("name capacity",
          std::string_view(sotc::tls_record_status_name(TlsRecordStatus::capacity)) == "capacity");
}

}  // namespace

int main() {
    test_constants();
    test_push_layout_and_count_order();
    test_pop();
    test_capacity_refused();
    test_unknown_thread_and_isolation();
    test_peek_top_matches_the_consumer_idiom();
    test_alloc_site_frames();
    test_status_names();

    if (failures == 0) {
        std::printf("OK test_tls_record_stack\n");
        return 0;
    }
    std::fprintf(stderr, "test_tls_record_stack: %d failure(s)\n", failures);
    return 1;
}
