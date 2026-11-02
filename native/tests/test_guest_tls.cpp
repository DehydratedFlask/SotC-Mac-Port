#include "../core/guest_tls.hpp"

#include <cstdint>
#include <cstdio>
#include <string_view>

namespace {

using sotc::GuestThreads;
using sotc::TlsStatus;

int failures = 0;

void check(std::string_view name, bool passed) {
    if (!passed) {
        std::fprintf(stderr, "FAIL %.*s\n", static_cast<int>(name.size()), name.data());
        ++failures;
    }
}

void test_constants() {
    check("block covers the largest observed offset",
          GuestThreads::kBlockSize >= 0x400 + sizeof(std::uint64_t));
    check("block size is a power of two", (GuestThreads::kBlockSize & (GuestThreads::kBlockSize - 1)) == 0);
    check("thread cap", GuestThreads::kMaxThreads >= 2);
}

void test_thread_pointer_is_unique_per_thread() {
    GuestThreads threads(0x1000'0000ULL);
    check("create first", threads.create(7) == TlsStatus::ok);
    check("create second", threads.create(9) == TlsStatus::ok);
    std::uint64_t first = 0;
    std::uint64_t second = 0;
    check("pointer first", threads.thread_pointer(7, first) == TlsStatus::ok);
    check("pointer second", threads.thread_pointer(9, second) == TlsStatus::ok);
    check("pointers differ", first != second);
    check("pointer in window", first >= 0x1000'0000ULL &&
                                   second < 0x1000'0000ULL + 2ULL * GuestThreads::kBlockSize);
    std::uint64_t missing = 0;
    check("unknown pointer", threads.thread_pointer(11, missing) == TlsStatus::no_thread);
}

void test_read_write_at_observed_offsets() {
    GuestThreads threads;
    (void)threads.create(1);
    // 0x8, 0x10 and 0x400 are the offsets the binary actually dereferences.
    check("write 0x8", threads.write_u64(1, 0x8, 0x1122334455667788ULL) == TlsStatus::ok);
    check("write 0x10", threads.write_u32(1, 0x10, 0xAABBCCDDu) == TlsStatus::ok);
    check("write 0x400", threads.write_u8(1, 0x400, 0x5A) == TlsStatus::ok);

    std::uint64_t wide = 0;
    std::uint32_t mid = 0;
    std::uint8_t byte = 0;
    check("read 0x8", threads.read_u64(1, 0x8, wide) == TlsStatus::ok &&
                          wide == 0x1122334455667788ULL);
    check("read 0x10", threads.read_u32(1, 0x10, mid) == TlsStatus::ok && mid == 0xAABBCCDDu);
    check("read 0x400", threads.read_u8(1, 0x400, byte) == TlsStatus::ok && byte == 0x5A);
}

void test_two_threads_do_not_alias() {
    GuestThreads threads;
    (void)threads.create(1);
    (void)threads.create(2);
    check("write alpha", threads.write_u64(1, 0x10, 0xAAAAULL) == TlsStatus::ok);
    check("write beta", threads.write_u64(2, 0x10, 0xBBBBULL) == TlsStatus::ok);
    std::uint64_t alpha = 0;
    std::uint64_t beta = 0;
    check("alpha value", threads.read_u64(1, 0x10, alpha) == TlsStatus::ok && alpha == 0xAAAAULL);
    check("beta value", threads.read_u64(2, 0x10, beta) == TlsStatus::ok && beta == 0xBBBBULL);
    // A fresh thread sees zeroes at the same offset rather than another thread's data.
    (void)threads.create(3);
    std::uint64_t fresh = 1;
    check("fresh thread is zeroed", threads.read_u64(3, 0x10, fresh) == TlsStatus::ok && fresh == 0);
}

void test_bounds_and_lifecycle() {
    GuestThreads threads;
    (void)threads.create(4);
    std::uint64_t value = 0;
    check("last qword fits",
          threads.write_u64(4, GuestThreads::kBlockSize - sizeof(std::uint64_t), 1) ==
              TlsStatus::ok);
    check("past the end", threads.read_u64(4, GuestThreads::kBlockSize - 4, value) ==
                              TlsStatus::out_of_range);
    std::uint8_t small = 0;
    check("offset beyond block",
          threads.read_u8(4, GuestThreads::kBlockSize + 1, small) == TlsStatus::out_of_range);
    check("duplicate create", threads.create(4) == TlsStatus::already_exists);
    check("destroy", threads.destroy(4) == TlsStatus::ok);
    check("destroyed is gone", !threads.alive(4));
    check("access after destroy", threads.read_u64(4, 0x8, value) == TlsStatus::no_thread);
    check("destroy unknown", threads.destroy(4) == TlsStatus::no_thread);
}

void test_capacity() {
    GuestThreads threads;
    for (std::uint32_t id = 0; id < GuestThreads::kMaxThreads; ++id) {
        if (threads.create(id) != TlsStatus::ok) {
            check("capacity fill", false);
            return;
        }
    }
    check("live count", threads.live_count() == GuestThreads::kMaxThreads);
    check("over capacity", threads.create(GuestThreads::kMaxThreads) == TlsStatus::too_many_threads);
    check("destroy frees a slot", threads.destroy(0) == TlsStatus::ok &&
                                     threads.create(GuestThreads::kMaxThreads) == TlsStatus::ok);
}

void test_status_names() {
    check("name ok", std::string_view(sotc::tls_status_name(TlsStatus::ok)) == "ok");
    check("name no_thread", std::string_view(sotc::tls_status_name(TlsStatus::no_thread)) ==
                                "no_thread");
}

}  // namespace

int main() {
    test_constants();
    test_thread_pointer_is_unique_per_thread();
    test_read_write_at_observed_offsets();
    test_two_threads_do_not_alias();
    test_bounds_and_lifecycle();
    test_capacity();
    test_status_names();

    if (failures == 0) {
        std::printf("OK test_guest_tls\n");
        return 0;
    }
    std::fprintf(stderr, "test_guest_tls: %d failure(s)\n", failures);
    return 1;
}
