#include "insert_1bde30.hpp"

namespace sotc {

Insert1beeb0Result insert_1bde30(Container1beeb0& container,
                                 const Descriptor1beeb0& descriptor,
                                 const Insert1beeb0Alloc& alloc) noexcept {
    // mov rax,rsi / add rdi,0x40 / mov rsi,rdi / mov rdx,rax / jmp 0x1beeb0
    // The sentinel is the container's list-3 link pair; the insert recomputes it from the
    // container reference, which is equivalent because &container.list3_prev == container+0x40.
    return insert_1beeb0(container, descriptor, alloc);
}

}  // namespace sotc
