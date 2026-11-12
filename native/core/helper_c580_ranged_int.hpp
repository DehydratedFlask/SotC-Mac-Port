// CPU-01-HELPER-CALLEE-MODELS-01: family-admitting wrapper for the ranged-int constructor 0xc580
// (0x15f B, [0xc580, 0xc6df)). The body itself is `init_ranged_int_var_c580` (ranged_int_var_c580.hpp); this
// layer replaces its two-tag whitelist (0x15d1d80, 0x15944a0) with the ELF-derived int functor family in
// helper_functor_tags.hpp (37 class tables whose four slots are the lifted bodies 0xc720/0xc740/0xc760/0xc770).
//
// Why that is exact: 0xc580 reaches the caller's functor only through `call [tag + 8]` (clone) while it
// installs the functor, and the call site then runs `call [tag + 0x18]` (destroy). In every family table clone
// is 0xc740 (stores the payload and the class tag 0x15944a0 into the destination) and destroy is the bare-ret
// 0xc770, so the object ends with {fn_vptr = 0x15944a0, payload, aux} no matter which family tag the caller
// supplied. The wrapper therefore normalises the caller tag to 0x15944a0 before delegating, and reports
// `arg_destroy_ok` for the call-site destroy. A tag outside the family is refused with nothing written (the
// original would run an unread clone slot).
//
// Unblocks (caller tag proven by tools/lift/check_helper_c580.py): startup slot 430 (0x529df0), tag 0x15c8450.
// FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED; no original RVA is executed.
#pragma once

#include <cstdint>

#include "helper_functor_tags.hpp"
#include "ranged_int_var_c580.hpp"

namespace sotc {

struct HelperCtorResult {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;
    bool arg_destroy_ok = false;   // `call [caller_tag + 0x18]` at the call site is the bare-ret destroy
};

// 0xc580 for any int-family caller functor. `args.fn_vptr` is the caller's tag.
[[nodiscard]] HelperCtorResult helper_c580_ranged_int(ConCmdRangedVar& var, const SharedStr& name, void* value_ref,
                                                      std::int32_t min_value, std::int32_t max_value,
                                                      std::uint32_t flags, const RangedVarArgs& args,
                                                      const SharedStr& description) noexcept;

}  // namespace sotc
