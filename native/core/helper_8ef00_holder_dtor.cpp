#include "helper_8ef00_holder_dtor.hpp"

namespace sotc {

bool is_holder_dtor_twin(std::uint64_t rva) noexcept {
    for (const std::uint64_t twin : kHolderDtorTwins) if (twin == rva) return true;
    return false;
}

SharedReleaseF590Result holder_dtor_run(std::uint64_t rva, HolderObject28& object, const SharedReleaseF590Hooks& hooks) noexcept {
    if (!is_holder_dtor_twin(rva)) return {};
    object.vptr = kHolderDtorVptr;                       // lea rax,[class] ; add rax,0x10 ; mov [rdi],rax
    return shared_release_f590(object.holder, hooks);    // add rdi,8 ; jmp 0xf590
}

}  // namespace sotc
