// CPU-01-AOT-CFG-SCALE-01-BATCH25: ports of 0xa39eb0, 0xa39fd0 and 0xa38960. See the header.
#include "blob_parser_a38960.hpp"

#include <cstring>

namespace sotc {

bool blob_validate(BlobCursor& cursor, std::uint8_t status[2], unsigned depth) noexcept {
    if (depth > kBlobMaxDepth) return false;
    for (;;) {
        if (cursor.remaining == 0) return false;
        const std::uint32_t tag = *cursor.p++;
        --cursor.remaining;
        const std::uint32_t count = tag & kBlobCountMask;
        switch (tag & kBlobKindMask) {
            case kBlobKindElement:
            case kBlobKindLeaf:
                status[0] = static_cast<std::uint8_t>(status[0] & (count != 0));
                break;
            case kBlobKindGroup: {
                std::uint8_t sub[2] = {1, 0};
                if (!blob_validate(cursor, sub, depth + 1)) return false;
                status[0] = static_cast<std::uint8_t>(status[0] & (count != 0) & (sub[0] != 0));
                break;
            }
            case kBlobKindEnd:
                status[0] = static_cast<std::uint8_t>(status[0] & (status[1] ^ 1));
                break;
            default:
                if (tag == kBlobSentinel) {
                    status[0] = static_cast<std::uint8_t>(status[0] & status[1]);
                } else {
                    status[0] = 0;
                }
                break;
        }
        if (tag == kBlobKindEnd || tag == kBlobSentinel) return true;
    }
}

bool blob_accumulate(BlobCursor& cursor, BlobAccum& acc, unsigned depth) noexcept {
    if (depth > kBlobMaxDepth) return false;
    for (;;) {
        if (cursor.remaining == 0) return false;
        const std::uint32_t tag = *cursor.p++;
        --cursor.remaining;
        const std::uint32_t count = tag & kBlobCountMask;
        switch (tag & kBlobKindMask) {
            case kBlobKindGroup: {
                BlobAccum sub{};
                if (!blob_accumulate(cursor, sub, depth + 1)) return false;
                acc.b = 1 + sub.b * count + acc.b;
                acc.a += sub.a * count;
                acc.c += count * sub.c;
                break;
            }
            case kBlobKindLeaf:
                acc.b += 1;
                acc.a += count;
                break;
            case kBlobKindElement:
                acc.c += count;
                break;
            default:
                break;
        }
        if (tag == kBlobKindEnd || tag == kBlobSentinel) return true;
    }
}

namespace {

struct AllocState {
    const BlobHooks& hooks;
    BlobArena& arena;
    std::size_t control_used = 0;
    std::size_t data_used = 0;
    std::size_t allocations = 0;

    void* alloc(std::size_t bytes, bool payload) noexcept {
        if (hooks.allocate != nullptr) {
            void* p = hooks.allocate(bytes);
            if (p != nullptr) ++allocations;
            return p;
        }
        if (payload) {
            const std::size_t dwords = bytes / 4;
            if (data_used + dwords > arena.data.size()) return nullptr;
            std::uint32_t* p = arena.data.data() + data_used;
            data_used += dwords;
            ++allocations;
            return p;
        }
        if (control_used >= arena.control.size()) return nullptr;
        ++allocations;
        return &arena.control[control_used++];
    }

    void release(void* block) noexcept {
        if (hooks.release != nullptr) hooks.release(block);   // arena storage needs no free
    }
};

}  // namespace

BlobParseResult parse_blob_a38960(BlobHeader& out, const std::uint32_t* blob,
                                  std::size_t dwords, BlobArena& arena,
                                  const BlobHooks& hooks) noexcept {
    BlobParseResult result{};
    if ((hooks.allocate == nullptr) != (hooks.release == nullptr)) return result;

    out = BlobHeader{};

    std::uint8_t status[2] = {1, 1};
    BlobCursor cursor{blob, dwords};
    if (!blob_validate(cursor, status)) return result;
    out.valid = status[0] != 0 ? 1 : 0;

    // Each of the three passes starts from {a = 0, b = 1, c = 0} (`movq $0x100000000` into x/y
    // and a zero z), not from a zeroed accumulator.
    BlobAccum acc{};
    acc.b = 1;
    cursor = BlobCursor{blob, dwords};
    if (!blob_accumulate(cursor, acc)) return result;
    out.a = acc.c + acc.a;

    acc = BlobAccum{};
    acc.b = 1;
    cursor = BlobCursor{blob, dwords};
    if (!blob_accumulate(cursor, acc)) return result;
    out.size = acc.a;

    acc = BlobAccum{};
    acc.b = 1;
    cursor = BlobCursor{blob, dwords};
    if (!blob_accumulate(cursor, acc)) return result;
    out.bits = acc.b;
    out.bytes = (out.bits + 7) >> 3;
    out.total = out.size + out.bytes;

    AllocState alloc{hooks, arena};
    auto* first = static_cast<std::uint32_t*>(alloc.alloc(4, false));
    if (first == nullptr) return result;
    *first = 1;
    out.control = first;

    if (out.valid != 0) {
        // Scan for the terminator. The original has no bound; the port refuses at the caller's
        // stream length (and never past kBlobMaxDwords).
        std::size_t count = 0;
        while (count < dwords && count < kBlobMaxDwords && blob[count] != kBlobSentinel) ++count;
        if (count >= dwords || count >= kBlobMaxDwords) return result;
        ++count;   // the sentinel is copied too
        auto* data = static_cast<std::uint32_t*>(alloc.alloc(count * 4, true));
        if (data == nullptr) return result;
        std::memcpy(data, blob, count * 4);   // the 8/4/2/1 store chain plus 0xf8f470, exactly

        auto* second = static_cast<std::uint32_t*>(alloc.alloc(4, false));
        if (second == nullptr) return result;
        *second = 1;
        out.data = data;
        out.control = second;
        --(*first);
        if (*first == 0) {
            alloc.release(first);
            result.first_refcount_released = true;
        }
        result.payload_dwords = count;
    }

    result.allocations = alloc.allocations;
    result.ok = true;
    return result;
}

}  // namespace sotc
