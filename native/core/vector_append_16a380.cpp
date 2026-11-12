// CPU-01-STARTUP-SLOT-524-HELPER: see vector_append_16a380.hpp.
#include "vector_append_16a380.hpp"

#include <cstring>

namespace sotc {
namespace {

SharedStr measured_literal(const char* text) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text),
                     static_cast<std::int64_t>(std::strlen(text)), 0, nullptr};
}

bool vector_sane(const ConCmdCommand& owner) noexcept {
    const auto begin = reinterpret_cast<std::uintptr_t>(owner.args_begin);
    const auto end = reinterpret_cast<std::uintptr_t>(owner.args_end);
    const auto cap = reinterpret_cast<std::uintptr_t>(owner.args_cap);
    if (begin == 0 || end == 0 || cap == 0) {
        return begin == 0 && end == 0 && cap == 0;
    }
    if (begin > end || end > cap) return false;
    if ((end - begin) % sizeof(ConCmdArgToken) != 0) return false;
    return (cap - end) % sizeof(ConCmdArgToken) == 0;
}

}  // namespace

const VirtualResolutionLabel* find_virtual_resolution_label(std::uint32_t key) noexcept {
    for (const VirtualResolutionLabel& row : kVirtualResolutionLabels) {
        if (row.text == nullptr) return nullptr;   // sentinel first: its key is never compared
        if (row.key == key) return &row;
    }
    return nullptr;
}

const char* vector_append_stage_name(VectorAppendStage stage) noexcept {
    switch (stage) {
        case VectorAppendStage::none: return "none";
        case VectorAppendStage::validate: return "validate";
        case VectorAppendStage::inline_push: return "inline_push";
        case VectorAppendStage::grow: return "grow";
        case VectorAppendStage::key_store: return "key_store";
        case VectorAppendStage::label_lookup: return "label_lookup";
        case VectorAppendStage::label_replace: return "label_replace";
        case VectorAppendStage::canary: return "canary";
    }
    return "";
}

VectorAppendResult16a380 append_arg_token_16a380(ConCmdCommand& owner, const SharedStr& name,
                                                 std::uint32_t key,
                                                 const VectorAppendHooks16a380& hooks) noexcept {
    VectorAppendResult16a380 out{};
    out.key = key;

    // 0x16a393: the cookie the body re-checks at 0x16a582.
    const std::uint64_t guard_entry = hooks.stack_guard != nullptr ? *hooks.stack_guard : 0;

    // The original does not validate its arguments; a null name with a non-zero length or a
    // malformed vector would be undefined behaviour. The model refuses instead of guessing.
    if (name.size < 0 || (name.size > 0 && name.data == nullptr) || !vector_sane(owner)) {
        out.blocked = VectorAppendStage::validate;
        return out;
    }

    // 0x16a3b4..0x16a407: end/cap compare, then either the inline store sequence or 0x20640.
    const auto end = reinterpret_cast<std::uintptr_t>(owner.args_end);
    const auto cap = reinterpret_cast<std::uintptr_t>(owner.args_cap);
    out.token_index =
        owner.args_begin == nullptr
            ? 0
            : (end - reinterpret_cast<std::uintptr_t>(owner.args_begin)) / sizeof(ConCmdArgToken);

    ConCmdArgToken* token = nullptr;
    if (end < cap) {
        token = reinterpret_cast<ConCmdArgToken*>(owner.args_end);
        out.inline_push = true;

        // 0x16a3c4: kind = 1 (dword store; +0x04..+0x07 keep the host bytes).
        token->kind = 1;
        // 0x16a3ca..0x16a3e9: 16 B name, aux, ctl, `lock inc` when ctl != null.
        token->name = name;
        if (token->name.ctl != nullptr) {
            __atomic_add_fetch(token->name.ctl, 1, __ATOMIC_SEQ_CST);
        }
        // 0x16a3f0: 0xf180(token+0x28) builds the empty SharedStr.
        init_slot_f180(&token->text);
        // 0x16a3f5..0x16a400: end = token + 0x50.
        owner.args_end = reinterpret_cast<FConCmdArgRecord*>(token + 1);
    } else {
        // 0x16a409..0x16a41d: 0x20640(owner+0x80, &kind, name); then reload end.
        out.grow_path = true;
        if (hooks.grow == nullptr || !hooks.grow(hooks.grow_context, owner, name, 1)) {
            out.blocked = VectorAppendStage::grow;
            return out;
        }
        token = reinterpret_cast<ConCmdArgToken*>(owner.args_end) - 1;
        out.inline_push = false;
    }

    // 0x16a424: the key dword lands at token+0x48; +0x4c..+0x4f are not written.
    token->key = key;

    // 0x16a42b..0x16a52f: the table scan.
    const VirtualResolutionLabel* row = find_virtual_resolution_label(key);

    // 0x16a531 / 0x16a54a: a known key builds a measured view of its label; the sentinel path
    // formats "%d" with the literal 2. Both are borrowed into a 0x20 B stack object.
    SharedStr view{};
    if (row != nullptr) {
        out.label_found = true;
        view = measured_literal(row->text);
        out.label = row->text;
        out.label_length = static_cast<std::size_t>(view.size);
    } else {
        out.fallback_taken = true;
        view = measured_literal(kVirtualResolutionFallback);
        out.label = kVirtualResolutionFallback;
        out.label_length = 1;
    }

    // 0x16a555..0x16a579: compare token+0x28 with the stack view, release it, copy the 32 bytes,
    // zero the local, then release the local. 0xf180 leaves ctl null so both releases are no-ops.
    if (&token->text != &view) {
        if (!release_inert_view_f590(token->text)) {
            out.blocked = VectorAppendStage::label_replace;
            return out;
        }
        token->text = view;
        out.text_normalized = true;
    }

    // 0x16a582..0x16a59a: cookie compare.
    if (hooks.stack_guard != nullptr) {
        out.canary_checked = true;
        out.canary_matched = *hooks.stack_guard == guard_entry;
        if (!out.canary_matched) {
            out.blocked = VectorAppendStage::canary;
            return out;
        }
    }

    out.ok = true;
    return out;
}

}  // namespace sotc
