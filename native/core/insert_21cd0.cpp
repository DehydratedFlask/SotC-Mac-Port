#include "insert_21cd0.hpp"

#include <cstring>

namespace sotc {

Insert21cd0Result insert_21cd0(Container21cd0& container,
                               const Descriptor21cd0& descriptor,
                               const Insert21cd0Hooks& hooks) noexcept {
    Insert21cd0Result out{};

    // Step 2 — begin thunk 0x3b390(owner+0x58, &aggregate). The original builds a
    // 0x20 B stack aggregate {code1=0xf95df0, code2=0xf95c06, int=0xea} and
    // forwards it through a GOT/PLT import stub. This can only abort; modelled as
    // an optional observable hook.
    if (hooks.begin != nullptr) {
        // Reconstruct the original aggregate so the test can observe the arguments.
        struct BeginAggregate {
            std::uintptr_t code1;
            std::uintptr_t code2;
            std::uint32_t exception_code;
            // Implicit 4 B padding to 24 B; the original stack aggregate is 20 B
            // ({code1,code2,int 0xea}) but the C++ ABI pads to 24 B for alignment.
        };
        static_assert(sizeof(BeginAggregate) == 24);
        BeginAggregate agg{};
        agg.code1 = kInsert21cd0Code1;
        agg.code2 = kInsert21cd0Code2;
        agg.exception_code = kInsert21cd0ExceptionCode;
        hooks.begin(&container.lock_handle, &agg);
    }
    out.begin_called = (hooks.begin != nullptr);

    // Step 3 — capacity check: owner[0x40] (bump) vs owner[0x48] (end).
    container.sync_bump_end();
    if (container.count >= container.capacity) {
        // Step 3b — grow path: call 0x23210(owner+0x38, &B).
        if (hooks.grow == nullptr) return out;  // no grow hook → visible fail
        if (!hooks.grow(container, descriptor)) return out;  // allocation failure
    } else {
        // Step 3a — fast path: bump-allocate and copy into the pre-allocated buffer.
        Entry21cd0* const slot = container.data + container.count;

        // Zero the slot first (vxorps xmm0,xmm0 → vmovups [slot],xmm0: 16 B).
        // The zero covers slot[0x00] and slot[0x08], which are then overwritten, so
        // the net observable effect is that slot[0x10] (offset) is first zeroed then
        // set from the descriptor.
        std::memset(slot, 0, sizeof(Entry21cd0));

        // Copy descriptor[0x08..0x18) → slot[0x08..0x18): invoke + offset (vmovdqu 16 B).
        slot->invoke = descriptor.invoke;
        slot->offset = descriptor.reserved;

        // Copy descriptor[0x00] → slot[0x00]: ctx (mov 8 B, after zero).
        slot->ctx = descriptor.ctx;

        // Copy descriptor[0x18] → slot[0x18]: fn (mov 8 B).
        slot->fn = descriptor.fn;

        // Bump the pointer: owner[0x40] += 0x20.
        ++container.count;
        container.sync_bump_end();

        out.entry = slot;
    }

    // Step 3 continued — self-link slot[0x00] = &slot[0x00] iff fn != 0.
    if (descriptor.fn != 0) {
        out.self_linked = true;
        // Locate the entry we just inserted.
        Entry21cd0* const slot = container.data + (container.count - 1);
        slot->ctx = reinterpret_cast<std::uintptr_t>(&slot->ctx);
    }

    // Step 4 — end thunk 0x3b3c0(owner+0x58).
    if (hooks.end != nullptr) {
        hooks.end(&container.lock_handle);
    }
    out.end_called = (hooks.end != nullptr);

    out.ok = true;
    out.count = container.count;
    if (out.entry == nullptr) {
        // Set entry pointer if we went through the grow path (where the hook
        // already inserted into data[]).
        out.entry = container.data + (container.count - 1);
    }
    return out;
}

}  // namespace sotc
