#include "helper_56b4b0_node_ref.hpp"

#include "atomic_add_ref_14ccb0.hpp"

namespace sotc {

void build_node_ref_56b4b0(NodeRef56b4b0& out) noexcept {
    const HashedStr& empty = static_empty_hashed_2ed3640();      // guard 0x2ed3668 ; 0xf730 ; 0x14c4a0 ; atexit(0x95610) omitted
    copy_hashed_14c3a0(out.id, empty);                            // call 0x14c3a0(rbx, 0x2ed3640)
    Object14c9a0* node = getter_14d880();                         // call 0x14d880
    out.node = node;                                              // mov [rbx+0x28], rax
    out.flag = 0;                                                 // mov byte [rbx+0x30], 0
    add_ref_14ccb0(static_cast<void*>(node));                     // jmp 0x14ccb0 with rdi = rax
}

}  // namespace sotc
