// CPU-01-HELPER-CALLEE-MODELS-04: see helper_140b00_unbounded_int_twin.hpp.
#include "helper_140b00_unbounded_int_twin.hpp"

#include "helper_3a980_unbounded_int.hpp"

namespace sotc {

HelperCtorResult helper_unbounded_int_twin_140b00(ConCmdRangedVar& var, UnboundedIntTwinVariant variant, const SharedStr& name,
                                                  std::uint32_t flags, const RangedVarArgs& block, const SharedStr& description) noexcept {
    // Same body as 0x3a980 outside the class pair: the install reaches the caller functor only through `call [tag + 8]`. The private seed class has
    // the int family's four slot shapes; the shared model takes a family tag and the stored class is re-tagged afterwards.
    const std::uint64_t seed = unbounded_int_twin_seed_class(variant);
    const bool private_class = block.fn_vptr == seed;
    RangedVarArgs lifted = block;
    if (private_class) lifted.fn_vptr = kIntFunctorClassTag;
    HelperCtorResult out = helper_3a980_unbounded_int(var, name, flags, lifted, description);
    if (!out.ok) return out;                                       // nothing written
    var.base.vptr = unbounded_int_twin_mid_vptr(variant);          // lea mid table ; add 0x10 ; [this] = rax
    if (private_class) var.fn_vptr = seed;                         // the seed copy / clone slots store the seed class into the object
    return out;
}

}  // namespace sotc
