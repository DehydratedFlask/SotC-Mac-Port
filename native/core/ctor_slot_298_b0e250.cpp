#include "ctor_slot_298_b0e250.hpp"

#include <cstring>

namespace sotc {

namespace {
constexpr char kEmpty[] = "";
}  // namespace

Slot298Result init_slot298_b0e250(Slot298Object& obj, ConCmdRegistry& registry) noexcept {
    Slot298Result out{};

    // 0xb0e27d..0xb0e315: stack views and the 0x50-byte argument block.
    SharedStr name{reinterpret_cast<const std::uint8_t*>(kSlot298Name),
                   static_cast<std::int64_t>(sizeof kSlot298Name - 1), 0, nullptr};
    SharedStr description{reinterpret_cast<const std::uint8_t*>(kEmpty), 0, 0, nullptr};
    RangedVarArgs args{};
    args.fn_payload = 0;                  // [rsp+0x70] = 0
    args.fn_vptr = kRangedEnumFnVtbl;     // [rsp+0x80] = 0x15d5960
    args.fn_aux = 0;                      // [rsp+0x88] = 0

    // 0xb0e2f4: [obj] = 0x15d58f8 (overwritten by the constructor chain).
    obj.base.vptr = kSlot298FinalVptr;

    // 0xb0e31e: call 0xb0e060(obj, name, obj+0xd0, 1, &args, description).
    out.ctor_ok = init_ranged_var_b0e060(obj, name, &obj.value, 1, args, description);
    if (!out.ctor_ok) return out;

    // 0xb0e326: call 0xf590(description).
    const bool desc_inert = release_inert_view_f590(description);

    // 0xb0e32b..0xb0e336: call [[rsp+0x80]+0x18] on the argument functor. The vtable is
    // 0x15d5960 (checked above), whose slot 3 is 0xb0e240 = `ret`: no effect.
    out.functor_destroy_noop = args.fn_vptr == kRangedEnumFnVtbl;

    // 0xb0e339..0xb0e350.
    obj.base.vptr = kSlot298FinalVptr;    // [obj] = r13
    obj.value = 0;                        // dword [obj+0xd0] = 0
    obj.default_value = 0;                // dword [obj+0x68] = 0
    out.registration = register_concmd_20d60(obj.base, registry);   // call 0x20d60

    // 0xb0e358: call 0xf590(name).
    out.views_inert = desc_inert && release_inert_view_f590(name);

    // 0xb0e35d..0xb0e36e: __cxa_atexit(0xb0cb40, obj, &dso).
    out.atexit_recorded = registry.atexit_log.record(kSlot298DtorRva, &obj);

    out.ok = out.ctor_ok && out.functor_destroy_noop && out.registration.ok && out.views_inert &&
             out.atexit_recorded;
    return out;
}

}  // namespace sotc
