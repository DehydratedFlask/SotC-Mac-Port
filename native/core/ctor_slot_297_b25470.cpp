#include "ctor_slot_297_b25470.hpp"

#include "atomic_add_ref_14ccb0.hpp"

namespace sotc {

Slot297Hooks default_slot297_hooks() noexcept {
    return Slot297Hooks{&getter_14d880,
                        static_cast<void (*)(void*)>(&add_ref_14ccb0)};
}

const char* slot297_stop_name(Slot297Stop stop) noexcept {
    switch (stop) {
        case Slot297Stop::none: return "";
        case Slot297Stop::acquire: return "acquire";
        case Slot297Stop::retain: return "retain";
        case Slot297Stop::acquire_null: return "acquire(null)";
    }
    return "";
}

Slot297Result init_slot297_b25470(Slot297Table& table, const Slot297Hooks& hooks) noexcept {
    Slot297Result out;

    // Both calls are unconditional in the original. Without a modelled stage the measured order
    // cannot be reproduced, so fail before touching any record.
    if (hooks.acquire_shared_14d880 == nullptr) {
        out.stopped_at = Slot297Stop::acquire;
        return out;
    }
    if (hooks.retain_14ccb0 == nullptr) {
        out.stopped_at = Slot297Stop::retain;
        return out;
    }

    for (std::size_t i = 0; i < kSlot297Count; ++i) {
        Slot297Record& rec = table[i];

        // OBS 0xb25490: call 0x14d880. The original would store a null handle and then crash in
        // `lock inc [handle+0x30]`; stop before the record instead and report the prefix.
        Object14c9a0* handle = hooks.acquire_shared_14d880();
        if (handle == nullptr) {
            out.stopped_at = Slot297Stop::acquire_null;
            return out;
        }

        rec.node = handle;                                  // mov [rbx+r14-8],rax
        rec.flag110 = 0;                                    // mov byte [rbx+r14],0
        hooks.retain_14ccb0(static_cast<void*>(handle));    // call 0x14ccb0
        rec.flag00 = 0;                                     // mov byte [rbx+r14-0x110],0

        out.initialised = i + 1;
        out.shared = handle;
    }

    // OBS 0xb254c0..: __cxa_atexit(0xb24be0, nullptr, 0x1604218) is a documented contract; the
    // destructor bodies 0x14d2d0 / 0x14ccc0 are not lifted, so nothing is registered at exit.
    out.ok = true;
    out.stopped_at = Slot297Stop::none;
    return out;
}

}  // namespace sotc
