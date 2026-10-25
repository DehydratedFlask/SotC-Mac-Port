#include "alloc_wrappers_17070.hpp"

namespace sotc {
namespace {
AllocWrapperResult free_with_tag(void* ptr, std::uint32_t tag,
                                 const AllocDispatchContract& contract) noexcept {
    AllocWrapperResult out{};
    out.tag = tag;
    out.pointer = ptr;
    if (ptr == nullptr) {
        out.ok = true;  // the dispatcher's own null check makes this a no-op
        return out;
    }
    if (contract.deallocate == nullptr) {
        return out;
    }
    out.ok = contract.deallocate(ptr, tag);
    out.dispatched = true;
    return out;
}
}  // namespace

AllocWrapperResult alloc_17070(std::size_t size,
                               const AllocDispatchContract& contract) noexcept {
    AllocWrapperResult out{};
    out.size = size;
    out.tag = 1;
    if (contract.allocate == nullptr) {
        return out;
    }
    out.pointer = contract.allocate(size, /*esi*/ 1, /*edx*/ 8, /*ecx*/ 1);
    out.dispatched = true;
    out.ok = (out.pointer != nullptr);
    return out;
}

AllocWrapperResult alloc_17090(std::size_t size,
                               const AllocDispatchContract& contract) noexcept {
    AllocWrapperResult out{};
    out.size = size;
    out.tag = 1;
    if (contract.allocate == nullptr) {
        return out;
    }
    out.pointer = contract.allocate(size, /*esi*/ 1, /*edx*/ 8, /*ecx*/ 2);
    out.dispatched = true;
    out.ok = (out.pointer != nullptr);
    return out;
}

AllocWrapperResult free_170b0(void* ptr, const AllocDispatchContract& contract) noexcept {
    return free_with_tag(ptr, 1, contract);
}

AllocWrapperResult free_170c0(void* ptr, const AllocDispatchContract& contract) noexcept {
    return free_with_tag(ptr, 2, contract);
}

}  // namespace sotc
