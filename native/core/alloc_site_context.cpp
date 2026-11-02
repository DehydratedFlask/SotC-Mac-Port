#include "alloc_site_context.hpp"

#include "tls_record_stack.hpp"

namespace sotc {

const char* allocator_source_name(AllocatorSource source) noexcept {
    switch (source) {
        case AllocatorSource::table:
            return "table";
        case AllocatorSource::tls_site:
            return "tls_site";
    }
    return "unknown";
}

AllocatorChoice allocator_for_thread(const GuestThreads& threads, std::uint32_t thread_id,
                                    std::int32_t index, bool flag,
                                    std::uint64_t fallback) noexcept {
    AllocatorChoice out{};
    out.table_address = kAllocatorTableAddress + static_cast<std::uint64_t>(index) * 8;

    // `cmpl $0x1, %ebx` then (in 0x11fb0) `testb %r14b,%r14b`.
    const bool guard = index == kAllocatorTlsIndex && flag;
    if (!guard) {
        out.allocator = fallback;
        return out;
    }
    out.tls_guard_passed = true;

    // The body also requires a non-null control block before reading the count. `GuestThreads`
    // always has a block for a live thread, so only the count guard is observable here.
    const TlsAllocSitePeek site = tls_alloc_site_peek_top(threads, thread_id);
    if (!site.ok) {
        // Empty stack or unknown thread: the retail code branches to the table slot.
        out.tls_stack_empty = site.status == TlsRecordStatus::empty;
        out.allocator = fallback;
        return out;
    }
    out.source = AllocatorSource::tls_site;
    out.allocator = site.site.owner;
    return out;
}

}  // namespace sotc
