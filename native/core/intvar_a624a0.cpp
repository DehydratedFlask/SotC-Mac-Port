#include "intvar_a624a0.hpp"

#include <cstring>

namespace sotc {

namespace {

constexpr IntVarShapeInfo kShapes[2] = {
    {kIntVarWrapperListenerRva, kIntVarWorkerListenerRva, kIntVarFinalVptrListener,
     kIntVarWorkerVptrListener, kIntVarFnTableListener, kIntVarDtorListenerRva},
    {kIntVarWrapperShowActiveRva, kIntVarWorkerShowActiveRva, kIntVarFinalVptrShowActive,
     kIntVarWorkerVptrShowActive, kIntVarFnTableShowActive, kIntVarDtorShowActiveRva},
};

// A type-erased small callable as the stack temporaries in the original hold it: the payload word
// at functor +0x10, the table tag at +0x20, and the aux word at +0x28.
struct Fn {
    std::uint64_t payload = 0;
    std::uint64_t tag = 0;
    std::uint64_t aux = 0;
};

// Slot 0 (copy) and slot 1 (clone) of a lifted table: dst.payload = src.payload; dst.tag = table.
// The aux word is not touched. Slot 3 (destroy) is a bare `ret` and has no model beyond the lift check.
void slot_copy(std::uint64_t table, Fn& dst, const Fn& src) noexcept {
    dst.payload = src.payload;
    dst.tag = table;
}

// The shared four-step functor install used by the wrapper (onto its local functor) and by the worker
// (onto the object): given the source functor `src` and the destination `dst` whose current tag selects
// the temporary copy slot, reproduce
//   tmp1 <- clone(src) via src.tag ; tmp2 <- copy(tmp1) via tmp1.tag ; tmp1 <- copy(dst) via dst.tag ;
//   dst <- copy(tmp2) via tmp2.tag ; swap(dst.aux, tmp1.aux) ; destroy(tmp1) via tmp1.tag (bare ret).
// Returns false (and changes nothing) if any table on the way is not lifted.
bool install_functor(Fn& dst, const Fn& src) noexcept {
    if (!int_var_fn_table_lifted(src.tag) || !int_var_fn_table_lifted(dst.tag)) return false;
    Fn tmp1;
    Fn tmp2;
    tmp1.aux = src.aux;                   // movq 0x48(args) -> -0x60(%rbp)
    slot_copy(src.tag, tmp1, src);        // slot 1 (clone) of the argument's table
    slot_copy(tmp1.tag, tmp2, tmp1);      // slot 0 of tmp1's table
    const std::uint64_t dst_table = dst.tag;
    slot_copy(dst_table, tmp1, dst);      // slot 0 of the destination's (seed) table
    slot_copy(tmp2.tag, dst, tmp2);       // slot 0 of tmp2's table
    const std::uint64_t old_aux = dst.aux;  // swap(dst.aux, tmp1.aux)
    dst.aux = tmp1.aux;
    tmp1.aux = old_aux;
    // slot 3 of tmp1.tag: bare ret -> no effect.
    return int_var_fn_table_lifted(tmp1.tag);
}

}  // namespace

const IntVarShapeInfo& int_var_shape_info(IntVarShape shape) noexcept {
    return kShapes[shape == IntVarShape::listener ? 0 : 1];
}

bool int_var_fn_table_lifted(std::uint64_t table) noexcept {
    return table == kIntVarFnTableListener || table == kIntVarFnTableShowActive;
}

bool init_int_var_worker(IntVarShape shape, ConCmdRangedVar& var, const SharedStr& name,
                         std::int32_t min_value, std::int32_t max_value, std::uint32_t flags,
                         const RangedVarArgs& block, const SharedStr& description) noexcept {
    const IntVarShapeInfo& info = int_var_shape_info(shape);
    // The functor install runs unlifted clone code for any other table, after most of the stores.
    if (!int_var_fn_table_lifted(block.fn_vptr)) return false;

    // 0xa62696 / 0xa62a56: call 0x20af0(this, name, description, flags).
    init_concmd_base_20af0(var.base, name, description, flags);

    // 0xa6269b..0xa626ee (0xa62a5b..0xa62aae).
    var.base.vptr = info.worker_vptr;                       // [this] = table + 0x10
    var.value_ref = &var.value;                             // [this+0x60] = rdx (this+0xd0)
    var.min_value[0] = 0;                                   // byte [this+0x6c] = 0
    var.min_engaged = 1;                                    // byte [this+0x70] = 1
    std::memcpy(var.min_value, &min_value, 4);              // dword [this+0x6c] = *min
    var.max_value[0] = 0;                                   // byte [this+0x74] = 0
    var.max_engaged = 1;                                    // byte [this+0x78] = 1
    std::memcpy(var.max_value, &max_value, 4);              // dword [this+0x74] = *max
    std::memset(var.args, 0, sizeof var.args);              // ymm [this+0x80] = 0
    var.fn_vptr = info.fn_table;                            // [this+0xc0] = W
    var.fn_payload = 0;                                     // [this+0xb0] = 0
    var.fn_aux = 0;                                         // [this+0xc8] = 0

    // 0xa626f9..0xa62729: argument block copy (xmm at +0x88 first, then +0x80, then +0x98).
    var.args[1] = block.q[1];
    var.args[2] = block.q[2];
    var.args[0] = block.q[0];
    var.args[3] = block.q[3];
    if (block.q[3] != 0) var.args[0] = reinterpret_cast<std::uint64_t>(&var.args[0]);

    // 0xa6272c..0xa62798: functor install onto this+0xa0.
    Fn dst{var.fn_payload, var.fn_vptr, var.fn_aux};
    const Fn src{block.fn_payload, block.fn_vptr, block.fn_aux};
    if (!install_functor(dst, src)) return false;
    var.fn_payload = dst.payload;
    var.fn_vptr = dst.tag;
    var.fn_aux = dst.aux;
    return true;
}

IntVarCtorResult init_int_var(IntVarShape shape, ConCmdRangedVar& var, const SharedStr& name,
                              std::int32_t default_value, std::int32_t min_value,
                              std::int32_t max_value, std::uint32_t flags, const RangedVarArgs& args,
                              const SharedStr& description, ConCmdRegistry& registry) noexcept {
    IntVarCtorResult out{};
    const IntVarShapeInfo& info = int_var_shape_info(shape);

    // Refuse before the first write: the wrapper clones the caller's functor through `[args.tag + 8]`.
    if (!int_var_fn_table_lifted(args.fn_vptr)) {
        out.unlifted_functor = true;
        return out;
    }

    // [this] = final vptr (0xa62505): overwritten by the base constructor and restored below; the
    // worker is the only observer in between, and it overwrites it again.
    var.base.vptr = info.final_vptr;

    // Local copy of the argument block (0xa6250f..0xa625db).
    RangedVarArgs local{};
    std::memcpy(local.q, args.q, sizeof local.q);           // vmovups ymm; q[0] self-pointer below
    if (local.q[3] != 0) local.q[0] = reinterpret_cast<std::uint64_t>(&local.q[0]);
    Fn fn{0, info.fn_table, 0};                              // payload 0 (-0xa8), tag W (-0x98), aux 0 (-0x90)
    const Fn src{args.fn_payload, args.fn_vptr, args.fn_aux};
    if (!install_functor(fn, src)) {
        out.unlifted_functor = true;
        return out;
    }
    local.fn_payload = fn.payload;
    local.fn_vptr = fn.tag;
    local.fn_aux = fn.aux;

    // 0xa625f7: worker(this, name, &this.value, &min, &max, flags, &local, description).
    if (!init_int_var_worker(shape, var, name, min_value, max_value, flags, local, description)) {
        return out;
    }
    // 0xa6260a: destroy(local.fn) via [local.tag + 0x18] -> bare ret. Lifted by construction.
    if (!int_var_fn_table_lifted(local.fn_vptr)) return out;

    // 0xa6260d..0xa62627.
    var.base.vptr = info.final_vptr;                         // [this] = table + 0x10
    var.value = static_cast<std::uint32_t>(default_value);   // dword [this+0xd0] = *default
    var.default_value = static_cast<std::uint32_t>(default_value);  // dword [this+0x68] = *default
    out.registration = register_concmd_20d60(var.base, registry);   // call 0x20d60
    out.installed_fn_table = var.fn_vptr;
    out.ok = out.registration.ok;
    return out;
}

DebugMenuTarget int_var_menu_target(const ConCmdRangedVar& var) noexcept {
    DebugMenuTarget out{};
    out.object = &var;
    if (var.base.vptr == kIntVarFinalVptrListener || var.base.vptr == kIntVarFinalVptrShowActive) {
        out.slot3 = kIntVarSlot3Result;   // 0xa60ea0 / 0xa61940: mov eax,1; ret
        out.slot6 = kIntVarSlot6Result;   // 0xa60ef0 / 0xa61990: mov eax,1; ret
        out.known = true;
    }
    return out;
}

IntVarSiteResult init_slot306_int_var(std::size_t index, ConCmdRangedVar& var,
                                      ConCmdRegistry& registry) noexcept {
    IntVarSiteResult out{};
    if (index >= kSlot306IntVarSites.size()) return out;
    const Slot306IntVarSite& site = kSlot306IntVarSites[index];
    const IntVarShapeInfo& info = int_var_shape_info(site.shape);

    // Borrowed name view {literal, len, 0, 0} and the argument block the caller builds on its stack:
    // four zero qwords, functor payload 0, functor tag = the shape's table, functor aux 0.
    const SharedStr name{reinterpret_cast<const std::uint8_t*>(site.name),
                         static_cast<std::int64_t>(site.name_len), 0, nullptr};
    RangedVarArgs args{};
    args.fn_vptr = info.fn_table;

    out.ctor = init_int_var(site.shape, var, name, site.default_value, site.min_value, site.max_value,
                            kIntVarCtorFlags, args, empty_string_f730(), registry);
    if (out.ctor.unlifted_functor) return out;

    // 0xa634b0 / 0xa63580 / 0xa640db: `call [tag + 0x18]` on the argument functor: bare ret.
    out.arg_functor_destroyed = int_var_fn_table_lifted(args.fn_vptr);
    // 0xa634b6 / 0xa63586 / 0xa640e1: call 0xf590 on the name view (never carries a control counter).
    out.name_release_inert = release_inert_view_f590(name);
    // 0xa634d2 / 0xa63594 / 0xa640f3: __cxa_atexit(dtor, object, &__dso_handle).
    out.atexit_recorded = registry.atexit_log.record(info.dtor_rva, &var);

    out.ok = out.ctor.ok && out.arg_functor_destroyed && out.name_release_inert && out.atexit_recorded;
    return out;
}

}  // namespace sotc
