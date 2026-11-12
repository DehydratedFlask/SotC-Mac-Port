#include "init_560b60_body.hpp"

#include <array>
#include <cstring>

namespace sotc {
namespace {

void add_call(InitPostCallLog& log, std::uint64_t callee, const std::uint64_t* args,
              std::size_t count, const InitPostCallOutcome& outcome) noexcept {
    if (log.count >= kInitPostCallMax) return;
    InitPostCallRecord& r = log.records[log.count++];
    r.callee_rva = callee;
    r.caller = "0x560b60";
    r.executed = outcome.executed;
    r.has_return = outcome.has_value;
    r.return_value = outcome.value;
    r.blocker = outcome.executed ? nullptr : outcome.blocker;
    r.arg_count = static_cast<std::uint8_t>(count > kInitPostCallArgMax ? kInitPostCallArgMax
                                                                        : count);
    for (std::size_t i = 0; i < r.arg_count; ++i) r.args[i] = args[i];
    std::memcpy(r.detail, outcome.detail, sizeof r.detail);
    r.detail[sizeof r.detail - 1] = '\0';
}

void store_u64(std::uint8_t* p, std::uint64_t value) noexcept {
    std::memcpy(p, &value, sizeof value);
}

void store_u16(std::uint8_t* p, std::uint16_t value) noexcept {
    std::memcpy(p, &value, sizeof value);
}

void zero(std::uint8_t* p, std::size_t size) noexcept {
    std::memset(p, 0, size);
}

bool state_ready(const Init560b60Hooks& hooks) noexcept {
    return hooks.descriptor != nullptr && hooks.object != nullptr &&
           hooks.buffer_2000 != nullptr && hooks.buffer_0400 != nullptr &&
           hooks.buffer_0200 != nullptr && hooks.global_10 != nullptr &&
           hooks.global_20 != nullptr && hooks.global_28 != nullptr &&
           hooks.global_30 != nullptr && hooks.global_50 != nullptr &&
           hooks.previous != nullptr && hooks.callbacks != nullptr &&
           hooks.format_out != nullptr && hooks.call_11bc0 != nullptr &&
           hooks.alloc_17070 != nullptr && hooks.alloc_17090 != nullptr &&
           hooks.free_170b0 != nullptr && hooks.free_170c0 != nullptr &&
           hooks.call_3ca330 != nullptr && hooks.call_273890 != nullptr &&
           hooks.call_14c0f0 != nullptr && hooks.release_f590 != nullptr &&
           hooks.call_56b540 != nullptr && hooks.call_95580 != nullptr &&
           hooks.call_56efb0 != nullptr;
}

void init_stack_descriptor(std::array<std::uint8_t, 0x20>& descriptor, void* owner,
                           std::uint64_t value) noexcept {
    descriptor.fill(0);
    std::memcpy(descriptor.data(), &owner, sizeof owner);
    store_u64(descriptor.data() + 8, value);
}

}  // namespace

Init560b60Result run_init_560b60_body(const Init560b60Hooks& hooks) noexcept {
    Init560b60Result out{};
    if (!state_ready(hooks)) {
        out.blocked = "state_missing";
        return out;
    }

    const InitPostCallOutcome descriptor_out = hooks.call_11bc0(hooks.descriptor);
    const std::uint64_t descriptor_args[3] = {kInit560b60DescriptorRva,
                                              kInit560b60DescriptorRva, 0};
    add_call(out.call_log, 0x11bc0, descriptor_args, 3, descriptor_out);
    if (!descriptor_out.executed) {
        out.blocked = "call_11bc0_refused";
        return out;
    }

    // 0x560b95..0x560bb1: copy 0x18 B from the 0x11bc0 result into the two globals.
    std::memcpy(hooks.global_10, hooks.descriptor, 16);
    std::memcpy(hooks.global_20, static_cast<const std::uint8_t*>(hooks.descriptor) + 16, 8);

    void* object = nullptr;
    const std::uint64_t object_size = kInit560b60ObjectBytes;
    const InitPostCallOutcome object_alloc = hooks.alloc_17070(kInit560b60ObjectBytes, &object);
    add_call(out.call_log, 0x17070, &object_size, 1, object_alloc);
    ++out.alloc_calls;
    if (!object_alloc.executed || object == nullptr) {
        out.blocked = "alloc_17070_failed";
        return out;
    }
    out.object = object;
    auto* obj = static_cast<std::uint8_t*>(object);

    // 0x560bb9..0x560c16: initialise the object's list headers and scalar tail.
    store_u64(obj + 0x00, reinterpret_cast<std::uintptr_t>(obj + 0x20));
    store_u64(obj + 0x08, reinterpret_cast<std::uintptr_t>(obj + 0x20));
    store_u64(obj + 0x10, reinterpret_cast<std::uintptr_t>(obj + 0x8020));
    store_u64(obj + 0x8028, reinterpret_cast<std::uintptr_t>(obj + 0x8040));
    store_u64(obj + 0x8020, reinterpret_cast<std::uintptr_t>(obj + 0x8040));
    store_u64(obj + 0x8030, reinterpret_cast<std::uintptr_t>(obj + 0x10040));
    store_u64(obj + 0x100a0, 0);
    store_u64(obj + 0x100b0, 0);
    zero(obj + 0x136c8, 16);
    store_u64(obj + 0x136d8, 0);

    // 0x560c21..0x560c90: three allocator-owned buffers, in the original order.
    auto alloc_buffer = [&](std::size_t size, std::uint8_t* storage,
                            InitPostCallOutcome (*alloc)(std::size_t, void**),
                            void** result) noexcept -> bool {
        void* pointer = nullptr;
        const std::uint64_t size_arg = size;
        const InitPostCallOutcome o = alloc(size, &pointer);
        add_call(out.call_log, 0x17090, &size_arg, 1, o);
        ++out.alloc_calls;
        if (!o.executed || pointer == nullptr) {
            out.blocked = "alloc_17090_failed";
            return false;
        }
        (void)storage;
        *result = pointer;
        return true;
    };

    if (!alloc_buffer(kInit560b60Buffer0Bytes, hooks.buffer_2000, hooks.alloc_17090,
                      &out.buffer_2000)) {
        return out;
    }
    store_u64(obj + 0x10040, reinterpret_cast<std::uintptr_t>(out.buffer_2000));
    store_u64(obj + 0x10048, reinterpret_cast<std::uintptr_t>(out.buffer_2000));
    store_u64(obj + 0x10050,
              reinterpret_cast<std::uintptr_t>(out.buffer_2000) + kInit560b60Buffer0Bytes);

    if (!alloc_buffer(kInit560b60Buffer1Bytes, hooks.buffer_0400, hooks.alloc_17090,
                      &out.buffer_0400)) {
        return out;
    }
    store_u64(obj + 0x10060, reinterpret_cast<std::uintptr_t>(out.buffer_0400));
    store_u64(obj + 0x10068, reinterpret_cast<std::uintptr_t>(out.buffer_0400));
    store_u64(obj + 0x10070,
              reinterpret_cast<std::uintptr_t>(out.buffer_0400) + kInit560b60Buffer1Bytes);

    if (!alloc_buffer(kInit560b60Buffer2Bytes, hooks.buffer_0200, hooks.alloc_17090,
                      &out.buffer_0200)) {
        return out;
    }
    store_u64(obj + 0x10080, reinterpret_cast<std::uintptr_t>(out.buffer_0200));
    store_u64(obj + 0x10088, reinterpret_cast<std::uintptr_t>(out.buffer_0200));
    store_u64(obj + 0x10090,
              reinterpret_cast<std::uintptr_t>(out.buffer_0200) + kInit560b60Buffer2Bytes);

    store_u16(obj + 0x136c0, 0);
    store_u16(obj + 0x136c2, 0);
    *(obj + 0x136c4) = 0;

    // 0x560cab..0x560d07: release the previous global object, then publish the new one.
    void* previous = *hooks.previous;
    if (previous != object && previous != nullptr) {
        auto* prev = static_cast<std::uint8_t*>(previous);
        constexpr std::uint32_t kFreeOffsets[4] = {0x136c8, 0x10080, 0x10060, 0x10040};
        for (std::uint32_t offset : kFreeOffsets) {
            std::uintptr_t pointer = 0;
            std::memcpy(&pointer, prev + offset, sizeof pointer);
            if (pointer == 0) continue;
            const std::uint64_t arg = pointer;
            const InitPostCallOutcome o = hooks.free_170c0(reinterpret_cast<void*>(pointer));
            add_call(out.call_log, 0x170c0, &arg, 1, o);
            ++out.free_calls;
            if (!o.executed) {
                out.blocked = "free_170c0_refused";
                return out;
            }
        }
        const std::uint64_t previous_arg = reinterpret_cast<std::uintptr_t>(previous);
        const InitPostCallOutcome free_object = hooks.free_170b0(previous);
        add_call(out.call_log, 0x170b0, &previous_arg, 1, free_object);
        ++out.free_calls;
        if (!free_object.executed) {
            out.blocked = "free_170b0_refused";
            return out;
        }
        out.previous_cleanup = true;
    }
    if (previous != object) *hooks.previous = object;

    // 0x560d0a..0x560d76: three 0x20-byte stack descriptors and the 0x3ca330 call.
    std::array<std::uint8_t, 0x20> stack_70{};
    std::array<std::uint8_t, 0x20> stack_90{};
    std::array<std::uint8_t, 0x20> stack_b0{};
    init_stack_descriptor(stack_70, obj, kInit560b60StackValue0);
    init_stack_descriptor(stack_90, obj, kInit560b60StackValue1);
    init_stack_descriptor(stack_b0, obj, kInit560b60StackValue2);
    const std::uint64_t stack_args[3] = {
        reinterpret_cast<std::uintptr_t>(stack_70.data()),
        reinterpret_cast<std::uintptr_t>(stack_90.data()),
        reinterpret_cast<std::uintptr_t>(stack_b0.data())};
    const InitPostCallOutcome stack_out =
        hooks.call_3ca330(stack_70.data(), stack_90.data(), stack_b0.data());
    add_call(out.call_log, 0x3ca330, stack_args, 3, stack_out);
    if (!stack_out.executed) {
        out.blocked = "call_3ca330_refused";
        return out;
    }

    // 0x560d7b..0x560d90: callback registration.
    const std::uint64_t register_args[3] = {kInit560b60Register0, kInit560b60Register1,
                                             kInit560b60Register2};
    const InitPostCallOutcome register_out = hooks.call_273890(
        kInit560b60Register0, kInit560b60Register1, kInit560b60Register2);
    add_call(out.call_log, 0x273890, register_args, 3, register_out);
    if (!register_out.executed) {
        out.blocked = "call_273890_refused";
        return out;
    }

    // 0x560d95..0x560df8: publish the three callbacks and build the fixed format input.
    hooks.callbacks[0] = kInit560b60Callback0;
    hooks.callbacks[1] = kInit560b60Callback1;
    hooks.callbacks[2] = kInit560b60Callback2;
    std::memcpy(out.callbacks, hooks.callbacks, sizeof out.callbacks);

    std::array<std::uint8_t, 0x20> format_input{};
    const std::uintptr_t format_string = kInit560b60FormatStringRva;
    const std::uint64_t format_length = kInit560b60FormatStringBytes;
    std::memcpy(format_input.data(), &format_string, sizeof format_string);
    std::memcpy(format_input.data() + 8, &format_length, sizeof format_length);
    // bytes +0x10..+0x1f are the original vmovdqu zero of xmm0.
    const std::uint64_t format_args[2] = {
        reinterpret_cast<std::uintptr_t>(hooks.format_out),
        reinterpret_cast<std::uintptr_t>(format_input.data())};
    const InitPostCallOutcome format_out =
        hooks.call_14c0f0(hooks.format_out, format_input.data());
    add_call(out.call_log, 0x14c0f0, format_args, 2, format_out);
    if (!format_out.executed) {
        out.blocked = "call_14c0f0_refused";
        return out;
    }

    // 0x560e00..0x560e41: publish the int and the 32-byte output block, releasing the old
    // 0x2f08930 contents and the two temporary holders in the original order.
    std::uint32_t format_value = 0;
    std::memcpy(&format_value, hooks.format_out, sizeof format_value);
    *hooks.global_28 = format_value;
    out.global_28 = format_value;

    const std::uint64_t release_global_arg = reinterpret_cast<std::uintptr_t>(hooks.global_30);
    const InitPostCallOutcome release_global = hooks.release_f590(hooks.global_30);
    add_call(out.call_log, 0xf590, &release_global_arg, 1, release_global);
    if (!release_global.executed) {
        out.blocked = "release_f590_global_refused";
        return out;
    }

    std::memcpy(hooks.global_30, hooks.format_out + 8, kInit560b60Global30Bytes);
    zero(hooks.format_out + 8, kInit560b60Global30Bytes);
    const std::uint64_t release_block_arg =
        reinterpret_cast<std::uintptr_t>(hooks.format_out + 8);
    const InitPostCallOutcome release_block = hooks.release_f590(hooks.format_out + 8);
    add_call(out.call_log, 0xf590, &release_block_arg, 1, release_block);
    if (!release_block.executed) {
        out.blocked = "release_f590_block_refused";
        return out;
    }

    const std::uint64_t release_input_arg =
        reinterpret_cast<std::uintptr_t>(format_input.data());
    const InitPostCallOutcome release_input = hooks.release_f590(format_input.data());
    add_call(out.call_log, 0xf590, &release_input_arg, 1, release_input);
    if (!release_input.executed) {
        out.blocked = "release_f590_input_refused";
        return out;
    }

    // 0x560e46..0x560e5e: probe the new global, optionally invoke 0x95580, then finish.
    bool take_branch = false;
    const std::uint64_t probe_arg = reinterpret_cast<std::uintptr_t>(hooks.global_28);
    const InitPostCallOutcome probe = hooks.call_56b540(hooks.global_28, &take_branch);
    add_call(out.call_log, 0x56b540, &probe_arg, 1, probe);
    if (!probe.executed) {
        out.blocked = "call_56b540_refused";
        return out;
    }
    if (take_branch) {
        const std::uint64_t branch_arg = reinterpret_cast<std::uintptr_t>(hooks.global_50);
        const InitPostCallOutcome branch = hooks.call_95580(hooks.global_50);
        add_call(out.call_log, 0x95580, &branch_arg, 1, branch);
        ++out.calls_95580;
        if (!branch.executed) {
            out.blocked = "call_95580_refused";
            return out;
        }
    }

    const InitPostCallOutcome finish = hooks.call_56efb0();
    add_call(out.call_log, 0x56efb0, nullptr, 0, finish);
    if (!finish.executed) {
        out.blocked = "call_56efb0_refused";
        return out;
    }

    out.calls_total = static_cast<int>(out.call_log.count);
    out.ok = true;
    return out;
}

}  // namespace sotc
