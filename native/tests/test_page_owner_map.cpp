// Contract tests for PageOwnerMap (analogue of 0x19a50; OBS: id<2 => no owner, foreign => none).
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "core/page_owner_map.hpp"

#define CHECK(c)                                                              \
    do {                                                                      \
        if (!(c)) {                                                           \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);  \
            std::exit(1);                                                     \
        }                                                                     \
    } while (0)

int main() {
    using sotc::PageOwnerMap;
    using S = PageOwnerMap::Status;
    PageOwnerMap m;
    CHECK(m.lookup(nullptr) == std::nullopt);
    CHECK(m.range_count() == 0);

    CHECK(m.add(0x100000, 0x10000, 2) == S::ok);
    CHECK(m.lookup(reinterpret_cast<void*>(0x100000)) == 2u);
    CHECK(m.lookup(reinterpret_cast<void*>(0x10ffff)) == 2u);       // last byte
    CHECK(m.lookup(reinterpret_cast<void*>(0x110000)) == std::nullopt);  // one past the end
    CHECK(m.lookup(reinterpret_cast<void*>(0xfffff)) == std::nullopt);   // one before

    // Reserved ids and degenerate ranges are rejected without change.
    CHECK(m.add(0x200000, 0x1000, 0) == S::bad_owner_id);
    CHECK(m.add(0x200000, 0x1000, 1) == S::bad_owner_id);
    CHECK(m.add(0x200000, 0x1000, 256) == S::bad_owner_id);
    CHECK(m.add(0x200000, 0, 3) == S::empty_or_wrapping_range);
    CHECK(m.add(~std::uintptr_t{0} - 0x10, 0x100, 3) == S::empty_or_wrapping_range);
    CHECK(m.range_count() == 1);

    // Overlap in every geometry.
    CHECK(m.add(0x100000, 0x10000, 3) == S::overlaps_existing);  // identical
    CHECK(m.add(0x108000, 0x1000, 3) == S::overlaps_existing);   // inside
    CHECK(m.add(0xf0000, 0x10001, 3) == S::overlaps_existing);   // straddles start
    CHECK(m.add(0x10ffff, 0x10, 3) == S::overlaps_existing);     // straddles end
    CHECK(m.add(0xf0000, 0x30000, 3) == S::overlaps_existing);   // contains
    CHECK(m.add(0x110000, 0x10000, 255) == S::ok);               // adjacent is fine
    CHECK(m.add(0xf0000, 0x10000, 4) == S::ok);                  // adjacent below
    CHECK(m.lookup(reinterpret_cast<void*>(0x110000)) == 255u);
    CHECK(m.lookup(reinterpret_cast<void*>(0xf8000)) == 4u);
    CHECK(m.range_count() == 3);

    CHECK(m.remove(0x100001) == S::not_found);  // must be exact start
    CHECK(m.remove(0x100000) == S::ok);
    CHECK(m.lookup(reinterpret_cast<void*>(0x100000)) == std::nullopt);
    CHECK(m.remove(0x100000) == S::not_found);
    CHECK(m.add(0x100000, 0x10000, 5) == S::ok);  // reuse after removal
    CHECK(m.lookup(reinterpret_cast<void*>(0x100010)) == 5u);
    std::puts("page_owner_map OK");
    return 0;
}
