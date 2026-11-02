#include "ranged_bound_var_a4f4d0.hpp"

#include <cstring>

namespace sotc {

const RangedBoundVarClass* ranged_bound_var_class_for(std::uint64_t wrapper_rva) noexcept {
    for (const RangedBoundVarClass& cls : kRangedBoundVarClasses) {
        if (cls.wrapper_rva == wrapper_rva) return &cls;
    }
    return nullptr;
}

namespace {

// A descriptor the caller assembled by hand must still be one of the three pinned rows: the leaf
// stores `mid_vptr` / `fn_vtbl` at fixed offsets and the wrapper stores `final_vptr`, so an
// unpinned triple would claim a class whose vtable slots were never read.
bool is_pinned_class(const RangedBoundVarClass& cls) noexcept {
    for (const RangedBoundVarClass& known : kRangedBoundVarClasses) {
        if (known.mid_vptr == cls.mid_vptr && known.fn_vtbl == cls.fn_vtbl &&
            known.final_vptr == cls.final_vptr) {
            return true;
        }
    }
    return false;
}

}  // namespace

RangedBoundVarResult init_ranged_bound_var_a4f4d0(ConCmdRangedVar& var,
                                                  const RangedBoundVarClass& cls,
                                                  const SharedStr& name, void* value_ref,
                                                  std::uint32_t min_cell, std::uint32_t max_cell,
                                                  std::uint32_t flags, const RangedVarArgs& args,
                                                  const SharedStr& description,
                                                  ConCmdRegistry& registry) noexcept {
    RangedBoundVarResult out{};

    // Refusals write nothing. (a) The wrapper dispatches the caller's functor class twice before the
    // leaf and the leaf dispatches the class tag again; a tag that is not the instantiation's would
    // run a body nobody read. (b) The descriptor must be a pinned row. (c) The wrapper dereferences
    // value_ref twice (0xa4f64a / 0xa4f652); a null cell would fault in the original.
    if (args.fn_vptr != cls.fn_vtbl) return out;
    if (!is_pinned_class(cls)) return out;
    if (value_ref == nullptr) return out;

    // The wrapper's own `[this] = final vptr` at 0xa4f535 is not reproduced: the leaf overwrites it
    // with the mid vptr 30 instructions later and nothing in between reads it. The final store at
    // 0xa4f63d below is the one that survives.
    // Leaf 0xa4f6c6: 0x20af0(this, name, description, flags).
    init_concmd_base_20af0(var.base, name, description, flags);

    // Leaf 0xa4f6e1..0xa4f71e, in body order.
    var.base.vptr = cls.mid_vptr;              // [this] = mid vptr (0xa4f6e1)
    var.value_ref = value_ref;                 // [this+0x60] = rdx (0xa4f6e4)
    var.min_value[0] = 0;                      // byte [this+0x6c] = 0 (0xa4f6e8, then overwritten)
    var.min_engaged = 1;                       // byte [this+0x70] = 1 (0xa4f6ec)
    std::memcpy(var.min_value, &min_cell, 4);  // dword [this+0x6c] = *min (0xa4f6f0)
    var.max_value[0] = 0;                      // byte [this+0x74] = 0 (0xa4f6f6, then overwritten)
    var.max_engaged = 1;                       // byte [this+0x78] = 1 (0xa4f6fa)
    std::memcpy(var.max_value, &max_cell, 4);  // dword [this+0x74] = *max (0xa4f6fe)
    std::memset(var.args, 0, sizeof var.args); // ymm [this+0x80] = 0 (0xa4f704)
    var.fn_vptr = cls.fn_vtbl;                 // qword [this+0xc0] = class tag (0xa4f70c)
    var.fn_payload = 0;                        // qword [this+0xb0] = 0 (0xa4f713)
    var.fn_aux = 0;                            // qword [this+0xc8] = 0 (0xa4f71e)

    // Leaf 0xa4f729..0xa4f7c8: argument copy + the functor dance, collapsed to its net state. The
    // 5 leaf dispatches plus the wrapper's 6 are the kRangedBoundFunctorSlotCalls the original makes.
    install_ranged_fn_from_args(var, args, cls.fn_vtbl);
    out.functor_slot_calls = kRangedBoundFunctorSlotCalls;

    // Wrapper 0xa4f63d..0xa4f657: final vptr, then value / default from the same cell.
    var.base.vptr = cls.final_vptr;            // [this] = final vptr (0xa4f63d)
    std::uint32_t cell = 0;
    // The original loads the cell twice (0xa4f64a and 0xa4f652); it is a stack temp that nothing else
    // writes, so the model reads it once and stores the same value in both fields.
    std::memcpy(&cell, value_ref, sizeof cell);
    var.value = cell;                          // dword [this+0xd0] (0xa4f64c)
    var.default_value = cell;                  // dword [this+0x68] (0xa4f654)
    out.value_from_cell = true;

    // Wrapper 0xa4f657: 0x20d60(this) - the register-once guard, queued on the pending list while
    // the manager cell is null (it is, for the whole of static initialisation).
    out.registration = register_concmd_20d60(var.base, registry);
    out.ok = out.registration.ok;
    return out;
}

}  // namespace sotc
