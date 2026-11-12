// CPU-01-AOT-CFG-SCALE-01-BATCH21 (slot 570): see ctor_slot_570_16d80.hpp.
#include "ctor_slot_570_16d80.hpp"

#include <cstring>

namespace sotc {

namespace {

// The three bespoke preamble tables and their identity records, in body order
// (0x16e84, 0x16eb1, 0x16eee). No original RVA is executed.
bool write_preamble(Slot570Objects& o, ConCmdRegistry& registry, Slot570Result& result) noexcept {
    for (std::size_t i = 0; i < kSlot570FlagCount; ++i) o.flags[i] = kSlot570FlagValue;

    for (Slot570ZeroCell& cell : o.cells) {
        cell.zero = 0;
        // cell.not_written is deliberately left alone: the body does not store to +0x04.
        std::memset(cell.tail, 0, sizeof cell.tail);
    }

    // `xchgq %rax, 0x2dd9ff0(%rip)` with %rax = 0: the old cell value is loaded and discarded.
    result.xchg_previous = o.xchg_cell;
    o.xchg_cell = 0;

    Slot570Sentinel& s = o.sentinel;
    s.qword0 = 0;
    // s.gap08 is never written.
    std::memset(s.zeros10, 0, sizeof s.zeros10);
    s.self = kSlot570SentinelSelf;
    s.ones = kSlot570SentinelOnes;
    s.dword30 = 0;
    s.capacity = kSlot570SentinelCapacity;

    std::memset(o.zero_block2.zeros, 0, sizeof o.zero_block2.zeros);
    o.zero_block2.qword10 = 0;

    o.triple.a = kSlot570TripleSelf0;
    o.triple.b = kSlot570TripleSelf1;
    o.triple.back = static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&o.var));

    const bool r0 = registry.atexit_log.record(kSlot570IdentityDtor0, &o.xchg_cell);
    const bool r1 = registry.atexit_log.record(kSlot570IdentityDtor1, &o.zero_block2);
    const bool r2 = registry.atexit_log.record(kSlot570IdentityDtor2, &o.triple);
    result.atexit_records = (r0 ? 1u : 0u) + (r1 ? 1u : 0u) + (r2 ? 1u : 0u);
    result.preamble_ok = r0 && r1 && r2;
    return result.preamble_ok;
}

}  // namespace

Slot570Result init_slot570_16d80(Slot570Objects& objects, ConCmdRegistry& registry,
                                 DebugMenuRegistry& menu, std::uint64_t fn_tag) noexcept {
    Slot570Result out{};
    if (fn_tag != kSlot570BoolFnTag) return out;   // no other tag's slot bodies were checked here

    if (!write_preamble(objects, registry, out)) return out;

    // `mem_Verify`: name measured by 0xf2f0, the shared empty description (length 0), the site's own
    // argument-functor tag, value 0 and default byte 0. The description view is released between the
    // base ctor and the register-once guard (0x16fa0 before 0x16fb6), i.e. early release.
    const BoolVarResult var = lane_m6::init_bool_var_literal(
        objects.var, kSlot570BoolName, kSlot570BoolNameLength, kSlot570BoolEmptyDescription,
        kSlot570BoolEmptyDescriptionLength, fn_tag, registry, kSlot570BoolValue, kSlot570BoolDefault,
        /*early_description_release=*/true);
    out.bool_var_ok = var.ok;
    if (!var.ok) return out;

    // 0x26ea0(item, label, text, target, flags); the body releases text then label, then records the
    // item destructor.
    const DebugMenuTarget target = debug_menu_target_for(objects.var);
    const FamilyItemResult item = family_toggle_item_26ea0(
        objects.toggle, kSlot570ToggleLabel, kSlot570ToggleLabelLength, kSlot570ToggleText,
        kSlot570ToggleTextLength, target, kSlot570ToggleFlags, registry, menu);
    out.toggle_ok = item.ok();
    if (!item.ok()) return out;

    // Exactly the records this body performs: the three identity tables, the bool and the item. The
    // menu registry's own first-use record is not this slot's.
    out.atexit_records += (var.atexit_recorded ? 1u : 0u) + (item.atexit_recorded ? 1u : 0u);
    out.ok = out.preamble_ok && out.bool_var_ok && out.toggle_ok &&
             out.atexit_records == kSlot570AtexitCount;
    return out;
}

}  // namespace sotc
