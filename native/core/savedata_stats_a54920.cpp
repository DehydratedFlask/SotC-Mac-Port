#include "savedata_stats_a54920.hpp"

#include <cstdio>
#include <cstring>

#include "ctor_slots_322_334.hpp"
#include "debug_menu_item.hpp"
#include "savedata_family.hpp"

namespace sotc {
namespace {

SharedStr view_of(const char* text, std::size_t len) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(len), 0,
                     nullptr};
}

}  // namespace

SavedataStatsA54920Result savedata_stats_a54920(const SavedataStatsA54920Args& args,
                                                const SavedataStatsA54920Hooks& hooks) noexcept {
    SavedataStatsA54920Result out{};

    // Refuse before the first write: the original dereferences both records.
    for (const SavedataStatsName* name : args.names) {
        if (name == nullptr) {
            out.refusal = SavedataStatsRefusal::null_name;
            return out;
        }
    }
    if (args.descriptor == nullptr) {
        out.refusal = SavedataStatsRefusal::null_descriptor;
        return out;
    }

    // ---- three 0xd8 counter objects (0xa54950 .. 0xa54ca2) -------------------
    constexpr const char* kFormats[kSavedataStatsCounterCount] = {
        kSavedataStatsFormatMaxCount, kSavedataStatsFormatSavesUsed, kSavedataStatsFormatMemUsed};
    constexpr std::size_t kFormatLens[kSavedataStatsCounterCount] = {
        kSavedataStatsFormatMaxCountLen, kSavedataStatsFormatSavesUsedLen,
        kSavedataStatsFormatMemUsedLen};

    for (std::size_t i = 0; i < kSavedataStatsCounterCount; ++i) {
        SavedataStatsName& name = *args.names[i];
        std::array<char, kSavedataStatsNameBufferSize>& buffer = out.names[i];
        std::memset(buffer.data(), 0, buffer.size());
        // The listing passes the literal 0x11/0x12/0x10 in edx: that is the snprintf size,
        // including the terminating NUL, not a format length to add one to.
        std::snprintf(buffer.data(), kFormatLens[i], kFormats[i], name.prefix);

        const std::uint32_t max_count = (i == 0) ? name.max_count : 0u;
        out.max_counts[i] = max_count;

        const SharedStr name_view = view_of(buffer.data(), std::strlen(buffer.data()));
        const SharedStr description = view_of("", 0);   // pinned empty rodata byte 0x12c41fe
        RangedVarArgs block{};                          // four zero qwords
        block.fn_vptr = kSavedataStatsArgFnTable;       // 0x15d1d80, slots 0xc720..0xc770

        ConCmdRangedVar& counter = out.counters[i];
        out.worker_ok[i] = init_unbounded_int_var_worker_3a980(
            counter, name_view, kSavedataStatsCtorFlags, block, description);
        if (!out.worker_ok[i]) {
            out.refusal = SavedataStatsRefusal::worker_rejected;
            return out;
        }
        out.description_view_inert[i] = release_inert_view_f590(description);
        // `call [0x15d1d80+0x18]`: the table is admitted above; the slot body is the bare ret 0xc770.
        (void)unbounded_int_fn_table_lifted(block.fn_vptr);

        counter.base.vptr = kSavedataStatsCounterVptr;   // 0xa54a3f
        counter.value = max_count;                       // +0xd0
        counter.default_value = max_count;               // +0x68

        const ConCmdRegisterResult registration = register_concmd_20d60(counter.base, out.concmd);
        out.registered[i] = registration.ok;
        if (!registration.ok) {
            out.refusal = SavedataStatsRefusal::registration_failed;
            return out;
        }

        // 0xa54a52: replace the caller's slot, destroying the previous non-null object.
        void*& slot = (i == 0) ? name.parent_slot40 : (i == 1) ? name.parent_slot48 : name.parent_slot50;
        if (slot != &counter) {
            if (slot != nullptr) {
                out.old_parent_slot_destroyed[i] = true;
                if (hooks.parent_slot_dtor != nullptr) {
                    hooks.parent_slot_dtor(slot);
                }
            }
            slot = &counter;
            out.parent_slot_replaced[i] = true;
        }
        out.name_view_inert[i] = release_inert_view_f590(name_view);
    }

    // ---- three 0x26ea0 toggles (0xa54ca7 .. 0xa54e2d) ------------------------
    constexpr const char* kTextFormats[kSavedataStatsConVarCount] = {
        kSavedataStatsConVarMaxCount, kSavedataStatsConVarSavesUsed,
        kSavedataStatsConVarMemUsed};
    constexpr std::size_t kTextLens[kSavedataStatsConVarCount] = {
        kSavedataStatsConVarMaxCountLen, kSavedataStatsConVarSavesUsedLen,
        kSavedataStatsConVarMemUsedLen};

    for (std::size_t i = 0; i < kSavedataStatsConVarCount; ++i) {
        std::array<char, kSavedataStatsNameBufferSize>& text = out.item_texts[i];
        std::snprintf(text.data(), kTextLens[i], kTextFormats[i], args.names[i]->prefix);
        const SharedStr label = family_view(kSavedataStatsLabel, kSavedataStatsLabelLen);
        const SharedStr text_view = view_of(text.data(), std::strlen(text.data()));
        const DebugMenuTarget target = unbounded_int_menu_target(out.counters[i]);

        DebugMenuItem& item = out.items[i];
        out.item_ctor_ok[i] = init_debug_menu_item_26ea0(item, label, text_view, target,
                                                         kSavedataStatsItemFlags);
        if (!out.item_ctor_ok[i]) {
            out.refusal = SavedataStatsRefusal::item_failed;
            return out;
        }
        const DebugMenuRegisterResult registration =
            register_debug_menu_item_268b0(item, out.menu, out.concmd.atexit_log);
        out.item_registered[i] = registration.ok;
        if (!registration.ok) {
            out.refusal = SavedataStatsRefusal::item_failed;
            return out;
        }
        (void)release_inert_view_f590(text_view);
        (void)release_inert_view_f590(label);
    }

    // 0xa54e10 .. 0xa54e28: the three 0x272d0 records are deferred until every item exists.
    for (std::size_t i = 0; i < kSavedataStatsConVarCount; ++i) {
        if (out.concmd.atexit_log.record(kDebugMenuItemDtorRva, &out.items[i])) {
            ++out.atexit_count;
        }
    }

    // ---- 0xa55050 sibling and list attach (0xa54e31 .. 0xa5501c) ------------
    if (args.parent == nullptr) {
        out.refusal = SavedataStatsRefusal::null_parent;
        return out;
    }
    if (args.sibling_cursor == nullptr) {
        out.refusal = SavedataStatsRefusal::null_cursor;
        return out;
    }
    const SavedataSlotAllocResult slot = savedata_slot_alloc_a55050(
        static_cast<std::uint8_t*>(args.parent), args.sibling_cursor);   // 0xa54e31
    if (!slot.ok) {
        out.refusal = SavedataStatsRefusal::sibling_allocation_failed;
        return out;
    }
    out.sibling_slot_found = slot.found;
    out.sibling_slot_index = slot.index;
    out.descriptor = reinterpret_cast<SavedataStatsDescriptor*>(slot.slot);
    SavedataStatsDescriptor& descriptor = *out.descriptor;

    // 0xa54e36: 0x28 B node whose owner is the THIRD call's name record (r12 at that point).
    out.node.owner = args.names[2];
    out.node.zero18 = 0;
    out.node.index20 = kSavedataStatsListIndex;
    out.child.self0 = &out.child;
    out.child.self8 = &out.child;
    out.child.self10 = &out.child;
    out.child.one18 = 1;
    out.child.one19 = 1;
    out.node.child = &out.child;

    // 0xa54e79..0xa54ea3: the previous head is cleared through the real 0xa57c10 model with
    // self = previous+8, then its child and the node itself are freed through 0x170b0.
    SavedataStatsListNode* previous = descriptor.head38;
    if (previous != nullptr && previous != &out.node) {
        const SavedataTreeClearResult cleared = savedata_tree_clear_a57c10(
            reinterpret_cast<std::uint8_t*>(previous) + 8, args.alloc);
        if (!cleared.ok) {
            out.refusal = SavedataStatsRefusal::previous_clear_failed;
            return out;
        }
        if (!free_170b0(previous->child, args.alloc).ok) {
            out.refusal = SavedataStatsRefusal::previous_free_failed;
            return out;
        }
        if (!free_170b0(previous, args.alloc).ok) {
            out.refusal = SavedataStatsRefusal::previous_free_failed;
            return out;
        }
        out.previous_head_destroyed = true;
    }
    descriptor.head38 = &out.node;   // +0x38
    out.descriptor_head_installed = true;
    descriptor.sentinel10 = kSavedataStatsListSentinel;
    descriptor.type1c = kSavedataStatsListType;

    const SavedataStatsInDescriptor& in = *args.descriptor;
    descriptor.field48 = in.field00;
    std::memcpy(descriptor.field50, &in.field08, sizeof descriptor.field50);
    descriptor.field60 = in.field18;
    if (descriptor.field60 != 0) {
        descriptor.field48 = reinterpret_cast<std::uintptr_t>(&descriptor.field48);
    }

    // The five calls the body makes through the incoming/sibling objects. Only the count and order are
    // observable here; a host supplies the bodies.
    for (int slot_index = 0; slot_index < 5; ++slot_index) {
        if (hooks.descriptor_vcall != nullptr) {
            hooks.descriptor_vcall(slot_index, &descriptor.subobject68, &descriptor);
        }
        ++out.descriptor_vcall_count;
    }
    descriptor.fn90 = in.field48;

    // 0xa54f78..0xa5501c: append the sibling pointer to the deque at parent+0x5e8. A full finish
    // block grows the real deque map through 0xa586a0, then 0x17090 allocates the next 0x100 B
    // block. The slot pointer is stored at the old finish.cur exactly as the body does.
    auto* const deque = reinterpret_cast<SavedataDequeMap*>(
        static_cast<std::uint8_t*>(args.parent) + kSavedataStatsParentMapOffset);
    if (deque->finish_cur == nullptr || deque->finish_last == nullptr ||
        deque->finish_node == nullptr) {
        out.refusal = SavedataStatsRefusal::invalid_parent_map;
        return out;
    }
    if (deque->finish_cur + sizeof(void*) == deque->finish_last) {
        const SavedataGrowResult grown = savedata_grow_a586a0(deque, 1, 1, args.alloc);
        out.vector_grow_called = true;
        if (!grown.ok) {
            out.refusal = SavedataStatsRefusal::vector_grow_failed;
            return out;
        }
        const AllocWrapperResult block = alloc_17090(kSavedataBlockSize, args.alloc);
        if (!block.ok) {
            out.refusal = SavedataStatsRefusal::vector_block_failed;
            return out;
        }
        auto* const block_bytes = static_cast<std::uint8_t*>(block.pointer);
        deque->finish_node[1] = block_bytes;
        *reinterpret_cast<SavedataStatsDescriptor**>(deque->finish_cur) = out.descriptor;
        deque->finish_node += 1;
        deque->finish_first = block_bytes;
        deque->finish_last = block_bytes + kSavedataBlockSize;
        deque->finish_cur = block_bytes;
        out.vector_block_allocated = true;
    } else {
        *reinterpret_cast<SavedataStatsDescriptor**>(deque->finish_cur) = out.descriptor;
        deque->finish_cur += sizeof(void*);
    }
    out.parent_vector_appended = true;

    bool ok = out.descriptor != nullptr && out.descriptor_head_installed &&
              out.parent_vector_appended &&
              out.atexit_count == static_cast<int>(kSavedataStatsConVarCount) &&
              out.descriptor_vcall_count == 5;
    for (std::size_t i = 0; i < kSavedataStatsCounterCount; ++i) {
        ok = ok && out.worker_ok[i] && out.name_view_inert[i] && out.description_view_inert[i] &&
             out.registered[i] && out.parent_slot_replaced[i] && out.item_ctor_ok[i] &&
             out.item_registered[i];
    }
    out.ok = ok;
    return out;
}

}  // namespace sotc
