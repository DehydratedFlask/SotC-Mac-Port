#include "engine_category_vars.hpp"

#include <cstdio>
#include <cstring>

namespace sotc {

SndxCategoryVectorSpan sndx_category_vectors_from_engine(const void* engine,
                                                         SndxCategoryVectorId id) noexcept {
    SndxCategoryVectorSpan span{};
    if (engine == nullptr) return span;
    const auto* base = static_cast<const char*>(engine) +
                       (id == SndxCategoryVectorId::mute ? kSndxMuteVectorOffset
                                                          : kSndxFilterVectorOffset);
    std::memcpy(&span.begin, base, sizeof(void*));
    std::memcpy(&span.end, base + 8, sizeof(void*));
    std::memcpy(&span.cap, base + 0x10, sizeof(void*));
    return span;
}

bool sndx_format_category_name(char (&buffer)[kSndxCategoryNameCapacity],
                              SndxCategoryVectorId id, std::size_t index) noexcept {
    const char* format =
        id == SndxCategoryVectorId::mute ? "sndxCategory_Mute_%d" : "sndxCategory_Filter_%d";
    const int written = std::snprintf(buffer, sizeof buffer, format, static_cast<int>(index));
    return written > 0 && static_cast<std::size_t>(written) < sizeof buffer;
}

namespace {

// The pending-list nodes this build registered form one contiguous tail block (the loop appends
// mute[j], filter[j] per index and nothing else registers in between). When a growth hook moves
// a span's buffer, that block's links and the moved slots' value references dangle. The original
// performs grow + construct inside the unlifted 0xa62bc0, so its exact relocation mechanics are
// UNKNOWN; this model detaches the block before the hook, then re-creates the same observable
// end state: identical slot order, value_ref pointing at each slot's own +0xc0, links intact.
constexpr std::size_t kSndxPendingNodeOffset = 0x08;  // offsetof(ConCmdBase, pending)

ConCmdListNode* pending_node_of(void* slot) noexcept {
    return reinterpret_cast<ConCmdListNode*>(static_cast<char*>(slot) + kSndxPendingNodeOffset);
}

// Walks to the node before the last `block` entries of the pending list (the head sentinel when
// the block is the whole list) and detaches the block: pred->next = head, head.prev = pred.
ConCmdListNode* detach_build_block(ConCmdRegistry& registry, std::size_t block) noexcept {
    const std::size_t total = registry.pending.size();
    ConCmdListNode* pred = &registry.pending.head;
    if (block < total) {
        for (std::size_t k = 0; k < total - block; ++k) pred = pred->next;
    } else {
        pred = &registry.pending.head;   // block covers everything; predecessor is the sentinel
    }
    registry.pending.head.prev = block == 0 ? registry.pending.head.prev : pred;
    if (block != 0) pred->next = &registry.pending.head;
    return pred;
}

// Re-links the detached block after a growth in the exact construction order
// (mute[0], filter[0], mute[1], ...), fixing the relocated span's value references.
void relink_build_block(ConCmdRegistry& registry, ConCmdListNode* pred,
                        const SndxCategoryVectorSpan& mute, const SndxCategoryVectorSpan& filter,
                        std::size_t mute_built, std::size_t filter_built,
                        SndxCategoryVectorId moved) noexcept {
    ConCmdListNode* prev = pred;
    const auto link = [&prev](ConCmdListNode* node) noexcept {
        node->prev = prev;
        prev->next = node;
        prev = node;
    };
    for (std::size_t j = 0; j < mute_built || j < filter_built; ++j) {
        if (j < mute_built) {
            char* slot = static_cast<char*>(mute.begin) + j * kSndxCategoryVarStride;
            if (moved == SndxCategoryVectorId::mute) {
                *reinterpret_cast<void**>(slot + kSndxVarValueRefOffset) =
                    slot + kSndxVarAdjacentValueOffset;   // value_ref -> own +0xc0
            }
            link(pending_node_of(slot));
        }
        if (j < filter_built) {
            char* slot = static_cast<char*>(filter.begin) + j * kSndxCategoryVarStride;
            if (moved == SndxCategoryVectorId::filter) {
                *reinterpret_cast<void**>(slot + kSndxVarValueRefOffset) =
                    slot + kSndxVarAdjacentValueOffset;
            }
            link(pending_node_of(slot));
        }
    }
    prev->next = &registry.pending.head;
    registry.pending.head.prev = prev;
}

}  // namespace

SndxCategoriesResult build_sndx_category_vars(SndxCategoryVectorSpan& mute,
                                              SndxCategoryVectorSpan& filter,
                                              const char* const* category_names,
                                              ConCmdRegistry& registry,
                                              const SndxCategoryHooks& hooks) noexcept {
    SndxCategoriesResult out{};
    if (!hooks.complete()) {
        out.stage = SndxCategoriesStage::hooks_missing;
        return out;
    }
    // The relocation fixup below only models vectors this build fills from empty; a non-empty
    // entry span would carry registered nodes this model cannot rebase. Fail closed.
    if (mute.var_count() != 0 || filter.var_count() != 0) {
        out.stage = SndxCategoriesStage::span_not_empty;
        return out;
    }

    const SharedStr description{reinterpret_cast<const std::uint8_t*>(""), 0, 0, nullptr};
    for (std::size_t i = 0; i < kSndxCategoryCount; ++i) {
        for (int which = 0; which < 2; ++which) {
            const SndxCategoryVectorId id = which == 0 ? SndxCategoryVectorId::mute
                                                       : SndxCategoryVectorId::filter;
            SndxCategoryVectorSpan& span = which == 0 ? mute : filter;

            char text[kSndxCategoryNameCapacity];
            if (!sndx_format_category_name(text, id, i)) {
                out.stage = SndxCategoriesStage::name_overflow;
                out.failed_index = i;
                return out;
            }
            SharedStr name{};
            if (!init_shared_str_f1a0(name, text, hooks.name_alloc)) {
                out.stage = SndxCategoriesStage::name_alloc_failed;
                out.failed_index = i;
                return out;
            }

            if (span.end == span.cap) {  // OBS: the append grows through 0xa62bc0 first
                // Detach this build's pending-list block (the spans' nodes are its tail),
                // grow, then rebase the moved slots and relink the block in construction order.
                ConCmdListNode* pred =
                    detach_build_block(registry, out.mute_built + out.filter_built);
                if (!hooks.grow(hooks.host, id, span)) {
                    // No relocation happened: restore the detached block exactly as it was.
                    relink_build_block(registry, pred, mute, filter, out.mute_built,
                                       out.filter_built, id);
                    out.stage = SndxCategoriesStage::grow_failed;
                    out.failed_index = i;
                    return out;
                }
                ++out.grow_calls;
                relink_build_block(registry, pred, mute, filter, out.mute_built, out.filter_built,
                                   id);
            }
            auto* const slot = static_cast<ConCmdBoolVar*>(span.end);
            const BoolVarResult built =
                init_bool_var_vector(*slot, name, description, kSndxCategoryVarFnVtbl, registry,
                                     hooks.name_dealloc, 0, 0);
            span.end = static_cast<char*>(span.end) + kSndxCategoryVarStride;
            if (!built.ok || !built.registration.ok) {
                out.stage = SndxCategoriesStage::construct_failed;
                out.failed_index = i;
                return out;
            }
            if (which == 0) {
                ++out.mute_built;
            } else {
                ++out.filter_built;
            }
        }

        // OBS: the conditional per-index menu toggles — non-null, non-empty category name only.
        if (category_names != nullptr && category_names[i] != nullptr &&
            category_names[i][0] != '\0' && hooks.menu_toggle != nullptr) {
            hooks.menu_toggle(hooks.host, i, category_names[i],
                              static_cast<char*>(mute.end) - kSndxCategoryVarStride,
                              static_cast<char*>(filter.end) - kSndxCategoryVarStride);
            ++out.menu_toggles;
        }
    }
    out.ok = out.mute_built == kSndxCategoryCount && out.filter_built == kSndxCategoryCount;
    out.stage = SndxCategoriesStage::ok;
    return out;
}

}  // namespace sotc
