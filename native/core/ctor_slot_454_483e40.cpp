// CPU-01-AOT-CFG-SCALE-01-BATCH26 (slot 454): see ctor_slot_454_483e40.hpp.
#include "ctor_slot_454_483e40.hpp"

#include <cstring>

#include "ctor_slot_family_items.hpp"

namespace sotc {
namespace {

bool init_tree(RbEraseTree& tree, std::size_t header_bytes, const RbEraseHooks& hooks) noexcept {
    return rb_tree_init(tree, header_bytes, hooks);
}

bool init_bool_var(Slot454BoolVar& var, const char* name, std::size_t length, std::uint8_t value,
                   ConCmdRegistry& registry) noexcept {
    const SharedStr name_view = family_view(name, length);
    const SharedStr description = family_view("", 0);
    init_concmd_base_20af0(var.base, name_view, description, kSlot454Flags);
    var.base.vptr = kSlot454BoolVptr;
    var.tail.value_ref = reinterpret_cast<std::uint64_t>(&var.tail.value);
    var.tail.default_value = value;
    var.tail.zero69 = 0;
    std::memset(var.tail.zero70, 0, sizeof var.tail.zero70);
    var.tail.zeroA0 = 0;
    var.tail.tagB0 = kSlot454BoolFnTagValue;
    var.tail.zeroB8 = 0;
    var.tail.value = value;
    if (!release_inert_view_f590(description)) return false;
    if (!register_concmd_20d60(var.base, registry).ok) return false;
    if (!release_inert_view_f590(name_view)) return false;
    return true;
}

}  // namespace

Slot454Result init_slot454_483e40(Slot454Objects& objects, ConCmdRegistry& registry,
                                  const Slot454Hooks& hooks) noexcept {
    Slot454Result out{};

    // ---- 1. 0x2ef85d8 through 0x3a980 (0x483e58..0x483f7a) ---------------------------------
    {
        Slot454LocalPlayer& object = objects.local_player;
        const SharedStr name = family_view("network_LocalPlayerId", kSlot454LocalPlayerNameLength);
        const SharedStr description = family_view("", 0);
        init_concmd_base_20af0(object.base, name, description, kSlot454Flags);
        object.base.vptr = kSlot454LocalPlayerVptr;
        object.value_ref = reinterpret_cast<std::uint64_t>(&object.value);
        object.default_value = -1;
        object.zero6c = 0;
        object.zero70 = 0;
        object.zero74 = 0;
        object.zero78 = 0;
        object.functor0 = 0;
        object.functor8 = 0;
        object.functor10 = 0;
        object.functor18 = 0;
        object.functor_region = 0;
        object.zeroB0 = 0;
        object.functor_vptr = kSlot454LocalPlayerFunctorVptr;
        object.zeroC8 = 0;
        object.value = -1;
        if (hooks.functor_transfer) {
            for (int i = 0; i < 5; ++i) {
                if (!hooks.functor_transfer(hooks.functor_context, object)) return out;
                ++out.functor_transfers;
            }
        }
        if (!release_inert_view_f590(description)) return out;
        if (!register_concmd_20d60(object.base, registry).ok) return out;
        if (!release_inert_view_f590(name)) return out;
        if (!registry.atexit_log.record(kSlot454LocalPlayerDtor, &object)) return out;
        ++out.atexit_records;
        ++out.registered_commands;
    }

    // ---- 2. 0x2ef86b0 "network_Interpolation" (0x483f7a..0x484087) -------------------------
    if (!init_bool_var(objects.interpolation, "network_Interpolation",
                       kSlot454InterpolationNameLength, 1, registry)) {
        return out;
    }
    if (!registry.atexit_log.record(kSlot454BoolDtor, &objects.interpolation)) return out;
    ++out.atexit_records;
    ++out.registered_commands;

    // ---- 3. 0x2ef8778 "network_ShowObjectBandwidth" (0x484087..0x484181) -------------------
    if (!init_bool_var(objects.show_bandwidth, "network_ShowObjectBandwidth",
                       kSlot454ShowBandwidthNameLength, 0, registry)) {
        return out;
    }
    if (!registry.atexit_log.record(kSlot454BoolDtor, &objects.show_bandwidth)) return out;
    ++out.atexit_records;
    ++out.registered_commands;

    // ---- 4. four records at 0x2ef8840 + 0x38*i (0x484181..0x4842a0) -------------------------
    for (std::size_t i = 0; i < kSlot454RecordCount; ++i) {
        Slot454Record& record = objects.records[i];
        const SharedStr view = family_view(kSlot454RecordText[i], kSlot454RecordTextLength[i]);
        record.text = view;
        record.kind = kSlot454RecordKind[i];
        record.zero28 = 0;
        record.zero30 = 0;
        record.flag31 = 0;
        if (!release_inert_view_f590(view)) return out;
        ++out.records;
    }
    // One registration for the whole 0xe0-byte array; 0x482920 unregisters +0/+0x38/+0x70/+0xa8.
    if (!registry.atexit_log.record(kSlot454RecordsDtor, objects.records.data())) return out;
    ++out.atexit_records;

    // ---- 5. trees A/B: 0x17070(0x38), self links, colour/nil 1 (0x4842a2..0x48432f) --------
    if (!init_tree(objects.tree_a, 0x38, hooks.rb)) return out;
    if (!init_tree(objects.tree_b, 0x38, hooks.rb)) return out;
    out.trees = 2;
    if (!registry.atexit_log.record(kSlot454TreeDtorA, &objects.tree_a)) return out;
    if (!registry.atexit_log.record(kSlot454TreeDtorA, &objects.tree_b)) return out;
    out.atexit_records += 2;

    // ---- 6. properties view 0x2ef8950 (0x484338..0x484364) ---------------------------------
    objects.properties = family_view("SServerObjectProperties", kSlot454PropertiesLength);
    if (!registry.atexit_log.record(kSlot454ViewDtor, &objects.properties)) return out;
    ++out.atexit_records;

    // ---- 7. tree C: 0x17070(0x1a8), self links, colour/nil 1 (0x48436e..0x4843b0) ----------
    if (!init_tree(objects.tree_c, 0x1a8, hooks.rb)) return out;
    ++out.trees;
    if (!registry.atexit_log.record(kSlot454TreeDtorB, &objects.tree_c)) return out;
    ++out.atexit_records;

    // The canary compare (0x4843b9..0x4843d1) has no native counterpart: the two guard words are
    // the same local in the original, so the compare is satisfied by construction.
    out.canary_matched = true;
    out.ok = out.registered_commands == 3 && out.records == kSlot454RecordCount && out.trees == 3 &&
             out.atexit_records == 8 && out.canary_matched;
    return out;
}

}  // namespace sotc
