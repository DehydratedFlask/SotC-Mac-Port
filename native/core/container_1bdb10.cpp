#include "container_1bdb10.hpp"

namespace sotc {

void init_container_1bdb10(Container1beeb0& container) noexcept {
    // lea rax,[rdi+0x20] / lea rcx,[rdi+0x40] — the list-2 and list-3 sentinels are the
    // address of their own link pair, exactly like list-1.
    Node1beeb0* const sentinel1 = reinterpret_cast<Node1beeb0*>(&container.list1_prev);
    Node1beeb0* const sentinel2 = reinterpret_cast<Node1beeb0*>(&container.list2_prev);
    Node1beeb0* const sentinel3 = reinterpret_cast<Node1beeb0*>(&container.list3_prev);

    // list-1
    container.count1 = 0;
    container.list1_prev = sentinel1;
    container.list1_next = sentinel1;
    // list-2
    container.count2 = 0;
    container.list2_prev = sentinel2;
    container.list2_next = sentinel2;
    // list-3
    container.count3 = 0;
    container.list3_prev = sentinel3;
    container.list3_next = sentinel3;
    // +0x18 / +0x38 / +0x58 are NOT written by the original.
}

}  // namespace sotc
