#include "retail_call.hpp"

#include "retail_cells.hpp"

namespace sotc {

const char* retail_call_status_name(RetailCallStatus status) noexcept {
    switch (status) {
        case RetailCallStatus::ok:
            return "ok";
        case RetailCallStatus::unknown_target:
            return "unknown_target";
        case RetailCallStatus::no_window:
            return "no_window";
        case RetailCallStatus::out_of_range:
            return "out_of_range";
        case RetailCallStatus::model_failed:
            return "model_failed";
    }
    return "unknown";
}

RetailCallOutcome call_resolved(std::uint64_t rva, RetailFrame& frame, GuestWindow& window,
                                const RetailCallContext& context) noexcept {
    RetailCallOutcome out{};
    if (retail_arg_spec_for(rva) == nullptr) {
        out.status = RetailCallStatus::unknown_target;
        return out;
    }
    if (!window.valid()) {
        out.status = RetailCallStatus::no_window;
        return out;
    }

    const std::uint64_t self = frame.get(RetailArg::rdi);
    const std::uint64_t arg1 = frame.get(RetailArg::rsi);

    switch (rva) {
        case kVtable1594dc0Slot20_20d60Rva: {
            const auto marshalled = marshal_20d60(window, self, context.global_20d60);
            if (!marshalled.ok) {
                out.status = marshalled.status == GuestStatus::no_window
                                 ? RetailCallStatus::no_window
                                 : RetailCallStatus::out_of_range;
                return out;
            }
            Vtable1594dc0Slot20_20d60Hooks hooks{};
            hooks.tail_call = context.tail_20d60;
            const auto result = vtable_1594dc0_slot20_20d60(
                reinterpret_cast<void*>(static_cast<std::uintptr_t>(self)), marshalled.cells, hooks);
            if (!result.ok) {
                out.status = RetailCallStatus::model_failed;
                return out;
            }
            out.rax = 0;
            out.status = RetailCallStatus::ok;
            out.ok = true;
            return out;
        }
        case kVtable15955c0Slot08_358c0Rva: {
            const auto marshalled = marshal_358c0(window, self, arg1);
            if (!marshalled.ok) {
                out.status = marshalled.status == GuestStatus::no_window
                                 ? RetailCallStatus::no_window
                                 : RetailCallStatus::out_of_range;
                return out;
            }
            const auto result = vtable_15955c0_slot08_358c0(marshalled.cells);
            if (!result.ok) {
                out.status = RetailCallStatus::model_failed;
                return out;
            }
            out.rax = 0;
            out.status = RetailCallStatus::ok;
            out.ok = true;
            return out;
        }
        case kVtable1594ae8Slot28_13d20Rva: {
            const auto marshalled = marshal_13d20(window, self);
            if (!marshalled.ok) {
                out.status = marshalled.status == GuestStatus::no_window
                                 ? RetailCallStatus::no_window
                                 : RetailCallStatus::out_of_range;
                return out;
            }
            const auto result = vtable_1594ae8_slot28_13d20(
                static_cast<std::uint32_t>(frame.get(RetailArg::rsi) & 0xFFFFFFFFu),
                marshalled.cells);
            if (!result.ok) {
                out.status = RetailCallStatus::model_failed;
                return out;
            }
            out.rax = 0;
            out.status = RetailCallStatus::ok;
            out.ok = true;
            return out;
        }
        case kVtable15949a0Slot88_187c0Rva: {
            const auto marshalled = marshal_187c0(window, self);
            if (!marshalled.ok) {
                out.status = marshalled.status == GuestStatus::no_window
                                 ? RetailCallStatus::no_window
                                 : RetailCallStatus::out_of_range;
                return out;
            }
            Vtable15949a0Slot88_187c0Hooks hooks{};
            hooks.pre_call_47620 = context.pre_187c0;
            hooks.alloc = context.alloc;
            const auto result = vtable_15949a0_slot88_187c0(marshalled.cells, hooks);
            if (!result.ok) {
                out.status = RetailCallStatus::model_failed;
                return out;
            }
            out.rax = 0;
            out.status = RetailCallStatus::ok;
            out.ok = true;
            return out;
        }
        case kVtable159c908Slot58_2687b0Rva: {
            const auto marshalled = marshal_2687b0(window, self);
            if (!marshalled.ok) {
                out.status = marshalled.status == GuestStatus::no_window
                                 ? RetailCallStatus::no_window
                                 : RetailCallStatus::out_of_range;
                return out;
            }
            Vtable159c908Slot58_2687b0Hooks hooks{};
            hooks.pre_call_2687d0 = context.pre_2687b0;
            hooks.alloc = context.alloc;
            const auto result = vtable_159c908_slot58_2687b0(marshalled.cells, hooks);
            if (!result.ok) {
                out.status = RetailCallStatus::model_failed;
                return out;
            }
            out.rax = 0;
            out.status = RetailCallStatus::ok;
            out.ok = true;
            return out;
        }
        case kVtable1598e10Slot18_1bf710Rva: {
            const auto marshalled = marshal_1bf710(window, self);
            if (!marshalled.ok) {
                out.status = marshalled.status == GuestStatus::no_window
                                 ? RetailCallStatus::no_window
                                 : RetailCallStatus::out_of_range;
                return out;
            }
            Vtable1598e10Slot18_1bf710Hooks hooks{};
            hooks.pre_call_1c0010 = context.pre_1bf710;
            hooks.alloc = context.alloc;
            const auto result = vtable_1598e10_slot18_1bf710(marshalled.cells, hooks);
            if (!result.ok) {
                out.status = RetailCallStatus::model_failed;
                return out;
            }
            out.rax = 0;
            out.status = RetailCallStatus::ok;
            out.ok = true;
            return out;
        }
        case kVtable15d1678SlotE8_c2e0Rva: {
            const auto marshalled = marshal_c2e0(window, self, arg1);
            if (!marshalled.ok) {
                out.status = marshalled.status == GuestStatus::no_window
                                 ? RetailCallStatus::no_window
                                 : RetailCallStatus::out_of_range;
                return out;
            }
            const auto result = vtable_15d1678_slot_e8_c2e0(marshalled.cells);
            if (!result.ok) {
                out.status = RetailCallStatus::model_failed;
                return out;
            }
            out.rax = 0;
            out.status = RetailCallStatus::ok;
            out.ok = true;
            return out;
        }
        case kVtable15d1d80Slot08_c740Rva: {
            const auto marshalled = marshal_c740(window, self, arg1);
            if (!marshalled.ok) {
                out.status = marshalled.status == GuestStatus::no_window
                                 ? RetailCallStatus::no_window
                                 : RetailCallStatus::out_of_range;
                return out;
            }
            const auto result = vtable_15d1d80_slot08_c740(marshalled.cells);
            if (!result.ok) {
                out.status = RetailCallStatus::model_failed;
                return out;
            }
            out.rax = 0;
            out.status = RetailCallStatus::ok;
            out.ok = true;
            return out;
        }
        case kVtable15d1678Slot68_a40e20Rva: {
            // Pure no-op: the model reads nothing, so no cells are marshalled.
            const auto result = vtable_15d1678_slot_68_a40e20();
            if (!result.ok) {
                out.status = RetailCallStatus::model_failed;
                return out;
            }
            out.rax = 0;
            out.status = RetailCallStatus::ok;
            out.ok = true;
            return out;
        }
        case kVtable15f2f08Slot38_e16a00Rva: {
            const auto marshalled = marshal_e16a00(window, self, arg1);
            if (!marshalled.ok) {
                out.status = marshalled.status == GuestStatus::no_window
                                 ? RetailCallStatus::no_window
                                 : RetailCallStatus::out_of_range;
                return out;
            }
            Vtable15f2f08Slot38_e16a00Hooks hooks{};
            hooks.call_slot18 = context.slot18_e16a00;
            hooks.tail_slot20 = context.slot20_e16a00;
            const auto result = vtable_15f2f08_slot38_e16a00(marshalled.cells, hooks);
            if (!result.ok) {
                out.status = RetailCallStatus::model_failed;
                return out;
            }
            out.rax = 0;
            out.status = RetailCallStatus::ok;
            out.ok = true;
            return out;
        }
        case kVtable15e0b48Slot28_ddf4b0Rva: {
            const auto marshalled = marshal_ddf4b0(window, self);
            if (!marshalled.ok) {
                out.status = marshalled.status == GuestStatus::no_window
                                 ? RetailCallStatus::no_window
                                 : RetailCallStatus::out_of_range;
                return out;
            }
            Vtable15e0b48Slot28_ddf4b0Hooks hooks{};
            hooks.pre_call_ddf310 = context.pre_ddf4b0;
            hooks.fetch_manager = context.fetch_manager_ddf4b0;
            hooks.tail_vcall18 = context.tail_ddf4b0;
            const auto result = vtable_15e0b48_slot28_ddf4b0(marshalled.cells, hooks);
            if (!result.ok) {
                out.status = RetailCallStatus::model_failed;
                return out;
            }
            out.rax = 0;
            out.status = RetailCallStatus::ok;
            out.ok = true;
            return out;
        }
        case kVtable15d1678Slot280_a460f0Rva: {
            const auto marshalled = marshal_a460f0(window, self);
            if (!marshalled.ok) {
                out.status = marshalled.status == GuestStatus::no_window
                                 ? RetailCallStatus::no_window
                                 : RetailCallStatus::out_of_range;
                return out;
            }
            Vtable15d1678Slot280_a460f0Hooks hooks{};
            hooks.helper_vcall18 = context.helper_a460f0;
            hooks.post_call_20b50 = context.post_a460f0;
            hooks.alloc = context.alloc;
            const auto result = vtable_15d1678_slot280_a460f0(marshalled.cells, hooks);
            if (!result.ok) {
                out.status = RetailCallStatus::model_failed;
                return out;
            }
            out.rax = 0;
            out.status = RetailCallStatus::ok;
            out.ok = true;
            return out;
        }
        case kVtable15e2d60SlotD0_e24900Rva: {
            const auto marshalled = marshal_e24900(window, self, arg1);
            if (!marshalled.ok) {
                out.status = marshalled.status == GuestStatus::no_window
                                 ? RetailCallStatus::no_window
                                 : RetailCallStatus::out_of_range;
                return out;
            }
            Vtable15e2d60SlotD0_e24900Hooks hooks{};
            hooks.slot28 = context.slot28_e24900;
            hooks.slot48 = context.slot48_e24900;
            hooks.sub_slot40 = context.sub40_e24900;
            hooks.result_slot_b8 = context.status_e24900;
            hooks.result_slot10 = context.release_e24900;
            const auto result = vtable_15e2d60_slot_d0_e24900(marshalled.cells, hooks);
            if (!result.ok) {
                out.status = RetailCallStatus::model_failed;
                return out;
            }
            out.rax = static_cast<std::uint64_t>(static_cast<std::uint32_t>(result.status));
            out.status = RetailCallStatus::ok;
            out.ok = true;
            return out;
        }
        case kVtable15ed2a0SlotC0_e26f90Rva: {
            const auto marshalled = marshal_e26f90(
                window, self, arg1, static_cast<std::int32_t>(frame.get(RetailArg::rdx) & 0xFFFFFFFFu),
                context.host_canary, context.host_buffer, context.host_local);
            if (!marshalled.ok) {
                out.status = marshalled.status == GuestStatus::no_window
                                 ? RetailCallStatus::no_window
                                 : RetailCallStatus::out_of_range;
                return out;
            }
            Vtable15ed2a0SlotC0_e26f90Hooks hooks{};
            hooks.slot78 = context.slot78_e26f90;
            hooks.slot90 = context.slot90_e26f90;
            hooks.helper = context.helper_e26f90;
            const auto result = vtable_15ed2a0_slot_c0_e26f90(marshalled.cells, hooks);
            if (!result.ok) {
                out.status = RetailCallStatus::model_failed;
                return out;
            }
            out.rax = static_cast<std::uint64_t>(static_cast<std::uint32_t>(result.status));
            out.status = RetailCallStatus::ok;
            out.ok = true;
            return out;
        }
        case kVtable15e2028Slot70_e1a5c0Rva: {
            const auto marshalled =
                marshal_e1a5c0(window, self, arg1,
                               static_cast<std::int32_t>(frame.get(RetailArg::rdx) & 0xFFFFFFFFu));
            if (!marshalled.ok) {
                out.status = marshalled.status == GuestStatus::no_window
                                 ? RetailCallStatus::no_window
                                 : RetailCallStatus::out_of_range;
                return out;
            }
            Vtable15e2028Slot70_e1a5c0Hooks hooks{};
            hooks.element_key = context.element_key_e1a5c0;
            hooks.element_value = context.element_value_e1a5c0;
            hooks.element_match = context.element_match_e1a5c0;
            const auto result = vtable_15e2028_slot70_e1a5c0(marshalled.cells, hooks);
            if (!result.ok) {
                out.status = RetailCallStatus::model_failed;
                return out;
            }
            out.rax = static_cast<std::uint64_t>(static_cast<std::uint32_t>(result.code));
            out.status = RetailCallStatus::ok;
            out.ok = true;
            return out;
        }
        default:
            // The target has an ABI row but no cell adapter yet.
            out.status = RetailCallStatus::unknown_target;
            return out;
    }
}

}  // namespace sotc
