// CPU-01-AOT-CFG-SCALE-01-BATCH27: see concmd_functor_6af60.hpp.
#include "concmd_functor_6af60.hpp"

#include <cstring>

namespace sotc {
namespace {

bool run_stage(const ConCmdFunctorHooks& hooks, ConCmdFunctor& object, unsigned stage,
               ConCmdFunctorResult& out) noexcept {
    if (hooks.transfer == nullptr) return true;
    if (!hooks.transfer(hooks.context, object, stage)) return false;
    ++out.transfers;
    return true;
}

}  // namespace

ConCmdFunctorResult init_concmd_functor_6b0f0(ConCmdFunctor& object, const SharedStr& name,
                                              const SharedStr& description, std::uint32_t flags,
                                              std::uint32_t low, std::uint32_t high,
                                              const ConCmdFunctorSpec& spec,
                                              const ConCmdFunctorView& view,
                                              const ConCmdFunctorHooks& hooks) noexcept {
    ConCmdFunctorResult out{};
    init_concmd_base_20af0(object.base, name, description, flags);
    object.base.vptr = kConCmdFunctorInnerVptr;   // 0x6b141
    object.spec = reinterpret_cast<std::uint64_t>(spec.address);
    object.low = low;
    object.one70 = 1;
    object.high = high;
    object.one78 = 1;
    std::memset(&object.view0, 0, 0x20);           // 0x6b164
    object.tag_c0 = kConCmdFunctorTagVptr;         // 0x6b16c
    object.view0 = view.q0;
    object.view8 = view.q8;
    object.view10 = view.q10;
    object.view18 = view.q18;
    if (object.view18 != 0) {
        object.view0 = reinterpret_cast<std::uint64_t>(&object.view0);   // 0x6b1b9
    }
    std::memset(object.region, 0, sizeof object.region);
    object.tag_c8 = 0;                             // 0x6b17e

    if (!run_stage(hooks, object, 0, out)) return out;   // copy  view -> temp1
    if (!run_stage(hooks, object, 1, out)) return out;   // clone temp1 -> temp2
    if (!run_stage(hooks, object, 2, out)) return out;   // construct +0xa0 from temp1
    if (!run_stage(hooks, object, 3, out)) return out;   // assign +0xa0 from temp2
    object.tag_c8 = view.tag48;                          // 0x6b219
    if (!run_stage(hooks, object, 4, out)) return out;   // destroy temp1
    out.ok = true;
    return out;
}

ConCmdFunctorResult init_concmd_functor_6af60(ConCmdFunctor& object, const SharedStr& name,
                                              const SharedStr& description, std::uint32_t flags,
                                              std::uint32_t low, std::uint32_t high,
                                              const ConCmdFunctorSpec& spec,
                                              const ConCmdFunctorView& view,
                                              ConCmdRegistry& registry,
                                              const ConCmdFunctorHooks& hooks) noexcept {
    ConCmdFunctorResult out =
        init_concmd_functor_6b0f0(object, name, description, flags, low, high, spec, view, hooks);
    if (!out.ok) return out;
    object.base.vptr = kConCmdFunctorFinalVptr;   // 0x6b0b7
    object.spec_word = spec.word;                 // 0x6b0bd
    const ConCmdRegisterResult reg = register_concmd_20d60(object.base, registry);
    out.registered = reg.ok;
    out.ok = reg.ok;
    return out;
}

}  // namespace sotc
