#include "../core/alloc_site_context.hpp"
#include "../core/tls_record_stack.hpp"

#include <cstdint>
#include <cstdio>
#include <string_view>

namespace {

using sotc::AllocatorChoice;
using sotc::AllocatorSource;
using sotc::GuestThreads;
using sotc::TlsAllocSite;
using sotc::allocator_for_thread;
using sotc::tls_alloc_site_push;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

void push_site(GuestThreads& threads, std::uint32_t id, std::uint64_t owner, std::uint32_t line) {
    TlsAllocSite site{};
    site.owner = owner;
    site.function = 0x1000;
    site.file = 0x2000;
    site.line = line;
    (void)tls_alloc_site_push(threads, id, site);
}

void test_constants() {
    check("table address", sotc::kAllocatorTableAddress == 0x2dfaab0);
    check("tls index", sotc::kAllocatorTlsIndex == 1);
    check("source names", std::string_view(sotc::allocator_source_name(AllocatorSource::table)) ==
                              "table" &&
                              std::string_view(sotc::allocator_source_name(
                                  AllocatorSource::tls_site)) == "tls_site");
}

void test_guard_requires_index_one_and_flag() {
    GuestThreads threads;
    (void)threads.create(1);
    push_site(threads, 1, 0xAAAA, 10);

    const auto wrong_index = allocator_for_thread(threads, 1, 2, true, 0xFA110000ULL);
    check("index 2 uses the table", wrong_index.source == AllocatorSource::table &&
                                        !wrong_index.tls_guard_passed);
    check("index 2 slot", wrong_index.table_address == sotc::kAllocatorTableAddress + 2 * 8);

    const auto no_flag = allocator_for_thread(threads, 1, 1, false, 0xBEEF);
    check("flag clear uses the table", no_flag.source == AllocatorSource::table &&
                                           !no_flag.tls_guard_passed && no_flag.allocator == 0xBEEF);

    const auto ok = allocator_for_thread(threads, 1, 1, true, 0xBEEF);
    check("guard passes", ok.tls_guard_passed);
    check("tls site wins", ok.source == AllocatorSource::tls_site && ok.allocator == 0xAAAA);
}

void test_empty_stack_falls_back() {
    GuestThreads threads;
    (void)threads.create(2);
    const auto empty = allocator_for_thread(threads, 2, 1, true, 0xCAFE);
    check("empty falls back", empty.source == AllocatorSource::table && empty.allocator == 0xCAFE);
    check("empty recorded", empty.tls_stack_empty && empty.tls_guard_passed);

    const auto unknown = allocator_for_thread(threads, 99, 1, true, 0xD00D);
    check("unknown thread falls back",
          unknown.source == AllocatorSource::table && unknown.allocator == 0xD00D);
    check("unknown thread is not 'empty'", !unknown.tls_stack_empty);
}

void test_nested_frames_select_the_top_one() {
    GuestThreads threads;
    (void)threads.create(3);
    push_site(threads, 3, 0x1111, 1);
    push_site(threads, 3, 0x2222, 2);
    const auto inner = allocator_for_thread(threads, 3, 1, true, 0);
    check("top frame wins", inner.source == AllocatorSource::tls_site && inner.allocator == 0x2222);
    (void)sotc::tls_record_pop(threads, 3);
    const auto outer = allocator_for_thread(threads, 3, 1, true, 0);
    check("pop restores the outer frame",
          outer.source == AllocatorSource::tls_site && outer.allocator == 0x1111);
    (void)sotc::tls_record_pop(threads, 3);
    const auto after = allocator_for_thread(threads, 3, 1, true, 0xFEED);
    check("empty after pops", after.source == AllocatorSource::table && after.allocator == 0xFEED);
}

void test_threads_do_not_share_the_choice() {
    GuestThreads threads;
    (void)threads.create(4);
    (void)threads.create(5);
    push_site(threads, 4, 0x4444, 4);
    const auto four = allocator_for_thread(threads, 4, 1, true, 0);
    const auto five = allocator_for_thread(threads, 5, 1, true, 0x5555);
    check("thread 4 uses its frame", four.source == AllocatorSource::tls_site &&
                                         four.allocator == 0x4444);
    check("thread 5 falls back", five.source == AllocatorSource::table &&
                                     five.allocator == 0x5555 && five.tls_stack_empty);
}

}  // namespace

int main() {
    test_constants();
    test_guard_requires_index_one_and_flag();
    test_empty_stack_falls_back();
    test_nested_frames_select_the_top_one();
    test_threads_do_not_share_the_choice();

    if (failures == 0) {
        std::printf("OK test_alloc_site_context\n");
        return 0;
    }
    std::fprintf(stderr, "test_alloc_site_context: %d failure(s)\n", failures);
    return 1;
}
