// CPU-01-STARTUP-SLOT-568: see ctor_slot_568_207e0.hpp.
#include "ctor_slot_568_207e0.hpp"

namespace sotc {
namespace {

SharedStr literal_view(const char* text, std::size_t length) noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(length), 0,
                     nullptr};
}

SharedStr empty_literal_view() noexcept {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(kSlot568EmptyData), 0, 0, nullptr};
}

}  // namespace

Slot568Result init_slot568_207e0(Slot568Objects& objects, ConCmdRegistry& registry,
                                 const Slot568Hooks& hooks) noexcept {
    Slot568Result out{};
    Slot568Command& command = objects.command;

    const SharedStr name = literal_view(kSlot568Name, kSlot568NameLength);
    const SharedStr description = literal_view(kSlot568Description, kSlot568DescriptionLength);

    // 0x207ff..0x20897: name view, description, flags 2, 0x20af0; then the stack tail block.
    init_concmd_base_20af0(command.base, name, description, kSlot568CommandFlags);
    command.base.vptr = kSlot568CommandVptr;
    command.tail8 = kSlot568CommandTail8;
    command.tail10 = kSlot568CommandTail10;
    command.tail18 = kSlot568CommandHandler;
    // 0x208c1: the callback word is non-zero, so +0x60 is rewritten from the stack self to &tail0.
    command.tail0 = reinterpret_cast<std::uint64_t>(&command.tail0);

    // 0x208ce..0x208f5: zero the vector, then 0x20540(vec, 2).
    command.vec_begin = nullptr;
    command.vec_end = nullptr;
    command.vec_cap = nullptr;
    command.vec_begin = objects.tokens;
    command.vec_end = objects.tokens;
    command.vec_cap = objects.tokens + kSlot568TokenCount;
    out.reserved = true;
    out.reserve_count = kSlot568TokenCount;

    // First inline push at 0x20942: kind 4, "file", empty view at +0x28 through 0xf180.
    Slot568Token& first = objects.tokens[0];
    first.kind = 4;
    first.text = literal_view(kSlot568FileText, kSlot568FileLength);
    first.empty = empty_literal_view();
    command.vec_end = objects.tokens + 1;
    out.inline_push_first = true;
    out.tokens = 1;

    // 0x209a2..0x209d1: the token's empty view is compared with the 0xf730 static, released, then
    // replaced by a copy of that static. The release is inert (ctl == nullptr).
    const SharedStr& empty_static = empty_string_f730();
    if (empty_static.size != 0 || empty_static.aux != 0 || empty_static.ctl != nullptr) return out;
    if (reinterpret_cast<const void*>(&first.empty) !=
        reinterpret_cast<const void*>(&empty_static)) {
        if (!release_inert_view_f590(first.empty)) return out;
        // The original copies the static object at 0x2dd9f50, whose payload is the fixed byte
        // 0x2dd9f78. `empty_string_f730()` is the native stand-in for that object, so store the
        // original payload address here rather than the host buffer address.
        first.empty = empty_literal_view();
    }
    out.empty_normalized = true;

    // Second inline push at 0x20a25: kind 0, "inline", empty view, +0x48 = 0.
    Slot568Token& second = objects.tokens[1];
    second.kind = 0;
    second.text = literal_view(kSlot568InlineText, kSlot568InlineLength);
    second.empty = empty_literal_view();
    second.flag48 = 0;
    command.vec_end = objects.tokens + 2;
    out.inline_push_second = true;
    out.second_flag_written = true;
    out.tokens = 2;

    // 0x20a93: the command's vtable +0x10 is 0x20d60; the command is queued once.
    if (!register_concmd_20d60(command.base, registry).ok) return out;
    out.registered = true;
    out.registrations = 1;

    // At 0x2086a/0x2088a the body writes the two scratch words at rsp+0xd0/+0xd8. The +0x18
    // vtable slot is a one-byte `ret` at 0x207d0.
    objects.scratch.vtable = kSlot568ScratchVtable;
    objects.scratch.tag = kSlot568ScratchTag;
    if (hooks.noop_vcall != nullptr && !hooks.noop_vcall(hooks.noop_context, objects.scratch)) {
        return out;
    }
    out.noop_vcall = true;

    // 0x20aa1/0x20aab: description then name, both borrowed and inert.
    if (!release_inert_view_f590(description)) return out;
    if (!release_inert_view_f590(name)) return out;

    // 0x20ab5: one __cxa_atexit(0x4b0, command, &__dso_handle).
    if (!registry.atexit_log.record(kSlot568CommandDtor, &command)) return out;
    out.atexit_records = 1;

    out.first_flag_written = false;
    out.grow_path = false;
    out.canary_matched = true;
    out.ok = out.reserved && out.inline_push_first && out.inline_push_second && !out.grow_path &&
             out.empty_normalized && !out.first_flag_written && out.second_flag_written &&
             out.registered && out.noop_vcall && out.reserve_count == kSlot568TokenCount &&
             out.tokens == kSlot568TokenCount && out.registrations == 1 && out.atexit_records == 1;
    return out;
}

}  // namespace sotc
