// CPU-01-HELPER-CALLEE-MODELS-03: fixture for the node-reference builder 0x56b4b0 (startup slot 422).
// ELF pins: tools/lift/check_helper_56b4b0.py. FIXTURE_VERIFIED.
#include <atomic>

#include "core/helper_56b4b0_node_ref.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

int main() {
    CHECK(kNodeRef56b4b0Rva == 0x56b4b0 && kNodeRef56b4b0Size == 138);
    CHECK(kNodeRefStaticHashedRva == 0x2ed3640 && kNodeRefStaticGuardRva == 0x2ed3668 && kNodeRefStaticDtorRva == 0x95610);
    CHECK(sizeof(NodeRef56b4b0) == 0x38);

    // the singleton is not yet built in this process: the getter's own first-use add plus the tail add_ref = 2
    CHECK(registry_cell_2ee3b00().load() == nullptr);
    NodeRef56b4b0 a;
    poison(a);
    build_node_ref_56b4b0(a);
    Object14c9a0* node = registry_cell_2ee3b00().load();
    CHECK(node != nullptr && a.node == node && a.flag == 0);
    CHECK(node->a30 == 2);

    // id is a copy of the static empty hashed string (hash, view, aux, ctl); the empty string has no control block
    const HashedStr& empty = static_empty_hashed_2ed3640();
    CHECK(a.id.hash == empty.hash);
    CHECK(a.id.str.data == empty.str.data && a.id.str.size == empty.str.size && a.id.str.aux == empty.str.aux);
    CHECK(a.id.str.ctl == empty.str.ctl);

    // bytes never written stay untouched: HashedStr padding 0x04..0x07 and 0x31..0x37
    const auto* bytes = reinterpret_cast<const std::uint8_t*>(&a);
    for (int i = 4; i < 8; ++i) CHECK(bytes[i] == kPoison);
    for (std::size_t i = 0x31; i < 0x38; ++i) CHECK(bytes[i] == kPoison);

    // second build: the getter returns the existing node without adding, only the tail add_ref counts
    NodeRef56b4b0 b;
    poison(b);
    build_node_ref_56b4b0(b);
    CHECK(b.node == node && node->a30 == 3);

    // a record with a non-null ctl in the static would bump it; the empty string must not touch any counter
    CHECK(empty.str.ctl == nullptr);
    std::puts("OK test_helper_56b4b0");
    return 0;
}
