#include "init_a5cd90_body.hpp"

#include <array>
#include <cstring>

namespace sotc {
namespace {

void add_call(InitPostCallLog& log, std::uint64_t callee, const std::uint64_t* args,
              std::size_t count, const InitPostCallOutcome& outcome) noexcept {
    if (log.count >= kInitPostCallMax) return;
    InitPostCallRecord& r = log.records[log.count++];
    r.callee_rva = callee;
    r.caller = "0xa5cd90";
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

std::uint64_t load_u64(const void* p) noexcept {
    std::uint64_t value = 0;
    std::memcpy(&value, p, sizeof value);
    return value;
}

std::uint16_t load_u16(const void* p) noexcept {
    std::uint16_t value = 0;
    std::memcpy(&value, p, sizeof value);
    return value;
}

void store_u64(void* p, std::uint64_t value) noexcept {
    std::memcpy(p, &value, sizeof value);
}

void store_u32(void* p, std::uint32_t value) noexcept {
    std::memcpy(p, &value, sizeof value);
}

bool state_ready(const InitA5cd90Hooks& hooks) noexcept {
    return hooks.object != nullptr && hooks.descriptor != nullptr &&
           hooks.registry != nullptr && hooks.config_flag != nullptr &&
           hooks.arena_tag != nullptr && hooks.call_9854f0 != nullptr &&
           hooks.call_994dd0 != nullptr && hooks.call_994e70 != nullptr &&
           hooks.call_991c70 != nullptr && hooks.alloc_12820 != nullptr &&
           hooks.call_992210 != nullptr && hooks.call_14c2f0 != nullptr &&
           hooks.resolve_resource != nullptr && hooks.call_14ccb0 != nullptr &&
           hooks.call_14ccc0 != nullptr && hooks.call_14d2d0 != nullptr &&
           hooks.call_95580 != nullptr && hooks.release_f590 != nullptr &&
           hooks.once_2ed3668 != nullptr && hooks.call_f730 != nullptr &&
           hooks.call_14c4a0 != nullptr && hooks.call_f8f390 != nullptr &&
           hooks.call_f8f3e0 != nullptr && hooks.call_14cf20 != nullptr &&
           hooks.call_14d880 != nullptr && hooks.call_14d7b0 != nullptr &&
           hooks.call_9892d0 != nullptr && hooks.call_946590 != nullptr &&
           hooks.call_946650 != nullptr && hooks.call_994070 != nullptr &&
           hooks.call_f901e0 != nullptr && hooks.call_994640 != nullptr &&
           hooks.log_condition != nullptr && hooks.call_3e7e0 != nullptr &&
           hooks.alloc_17070 != nullptr && hooks.call_3b5c0 != nullptr &&
           hooks.virtual_release != nullptr && hooks.canary != nullptr &&
           hooks.trap_f8f3a0 != nullptr;
}

// The two once/mutex registration copies at 0xa5cec9..0xa5cf37 and 0xa5cf3b..0xa5cf8e are the
// same five-call sequence. The native body keeps both copies and their two distinct 0x14cf20
// ownership rules.
template <typename Record>
bool run_once_sequence(const InitA5cd90Hooks& hooks, InitA5cd90Result& out,
                       bool& initialized, Record&& record) noexcept {
    const std::uint64_t once_args[] = {kInitA5cd90OnceKeyRva};
    const InitPostCallOutcome once = hooks.once_2ed3668(&initialized);
    if (!record(0xf8f3d0, once_args, 1, once)) {
        out.blocked = "call_f8f3d0_refused";
        return false;
    }
    if (!initialized) return true;

    std::uint64_t thread = 0;
    const InitPostCallOutcome thread_out = hooks.call_f730(&thread);
    if (!record(0xf730, nullptr, 0, thread_out)) {
        out.blocked = "call_f730_refused";
        return false;
    }
    const std::uint64_t mutex_args[] = {kInitA5cd90OnceDataRva, 0, thread};
    const InitPostCallOutcome mutex_out =
        hooks.call_14c4a0(kInitA5cd90OnceDataRva, 0, thread);
    if (!record(0x14c4a0, mutex_args, 3, mutex_out)) {
        out.blocked = "call_14c4a0_refused";
        return false;
    }
    const std::uint64_t key_args[] = {kInitA5cd90KeyCreateFnRva, kInitA5cd90OnceDataRva,
                                      kInitA5cd90KeyCreateDataRva};
    const InitPostCallOutcome key_out = hooks.call_f8f390(
        kInitA5cd90KeyCreateFnRva, kInitA5cd90OnceDataRva, kInitA5cd90KeyCreateDataRva);
    if (!record(0xf8f390, key_args, 3, key_out)) {
        out.blocked = "call_f8f390_refused";
        return false;
    }
    const std::uint64_t end_args[] = {kInitA5cd90OnceKeyRva};
    const InitPostCallOutcome end_out = hooks.call_f8f3e0(kInitA5cd90OnceKeyRva);
    if (!record(0xf8f3e0, end_args, 1, end_out)) {
        out.blocked = "call_f8f3e0_refused";
        return false;
    }
    return true;
}

}  // namespace

InitA5cd90Result run_init_a5cd90_body(const InitA5cd90Hooks& hooks) noexcept {
    InitA5cd90Result out{};
    if (!state_ready(hooks)) {
        out.blocked = "state_missing";
        return out;
    }

    auto* obj = hooks.object;
    auto record = [&](std::uint64_t callee, const std::uint64_t* args, std::size_t count,
                      const InitPostCallOutcome& o) {
        add_call(out.call_log, callee, args, count, o);
        ++out.calls_total;
        return o.executed;
    };

    // 0xa5cdae..0xa5cdf8: callback registration and the config string selected by the
    // [0x32f9730]+0x60 byte (0 -> the "...0000011" variant, non-zero -> "...0000001").
    {
        const std::uint64_t args[] = {0xa5d210};
        const InitPostCallOutcome o = hooks.call_9854f0(0xa5d210);
        if (!record(0x9854f0, args, 1, o)) {
            out.blocked = "call_9854f0_refused";
            return out;
        }
    }
    {
        const std::uint64_t args[] = {0xa5c630, 0xa5c440};
        const InitPostCallOutcome o = hooks.call_994dd0(0xa5c630, 0xa5c440);
        if (!record(0x994dd0, args, 2, o)) {
            out.blocked = "call_994dd0_refused";
            return out;
        }
    }
    {
        const bool zero = *hooks.config_flag == 0;
        const std::uintptr_t selected =
            zero ? kInitA5cd90ConfigFlagZeroRva : kInitA5cd90ConfigFlagSetRva;
        const std::uint64_t args[] = {selected};
        const InitPostCallOutcome o = hooks.call_994e70(selected);
        if (!record(0x994e70, args, 1, o)) {
            out.blocked = "call_994e70_refused";
            return out;
        }
    }

    // 0xa5cdfd..0xa5ce41: the five-pointer callback/model block.
    {
        std::array<std::uint8_t, 0x28> block{};
        store_u64(block.data() + 0x00, kInitA5cd90Callback0);
        store_u64(block.data() + 0x08, kInitA5cd90Callback1);
        store_u64(block.data() + 0x10, kInitA5cd90Callback2);
        store_u64(block.data() + 0x18, kInitA5cd90Model0);
        store_u64(block.data() + 0x20, kInitA5cd90Model1);
        const std::uint64_t args[] = {reinterpret_cast<std::uintptr_t>(block.data())};
        const InitPostCallOutcome o = hooks.call_991c70(block.data());
        if (!record(0x991c70, args, 1, o)) {
            out.blocked = "call_991c70_refused";
            return out;
        }
    }

    // 0xa5ce46..0xa5ce77: 2 MiB arena, zero-initialised, size class int stored at this+0x69c8.
    {
        void* arena0 = nullptr;
        const std::uint32_t tag = *hooks.arena_tag;
        const std::uint64_t args[] = {kInitA5cd90Arena0Bytes, tag, 0x10, 0};
        const InitPostCallOutcome o =
            hooks.alloc_12820(kInitA5cd90Arena0Bytes, tag, 0x10, 0, &arena0);
        ++out.alloc_calls;
        if (!record(0x12820, args, 4, o)) {
            out.blocked = "alloc_12820_refused";
            return out;
        }
        if (arena0 == nullptr) {
            out.blocked = "alloc_12820_null";
            return out;
        }
        out.arena0_bytes = kInitA5cd90Arena0Bytes;
        std::int32_t size_class = 0;
        const std::uint64_t zero_args[] = {reinterpret_cast<std::uintptr_t>(arena0),
                                           kInitA5cd90Arena0Bytes, 0x10, 0};
        const InitPostCallOutcome zero_out =
            hooks.call_992210(arena0, kInitA5cd90Arena0Bytes, 0x10, 0, &size_class);
        if (!record(0x992210, zero_args, 4, zero_out)) {
            out.blocked = "call_992210_refused";
            return out;
        }
        store_u32(obj + 0x69c8, static_cast<std::uint32_t>(size_class));
        out.field_69c8 = static_cast<std::uint32_t>(size_class);
    }

    // 0xa5ce7e..0xa5d0bf: conditional resource lookup, registration and field replacement.
    if (load_u64(hooks.descriptor) != 0) {
        std::array<std::uint8_t, kInitA5cd90KeyBytes> key{};
        {
            const std::uint64_t args[] = {reinterpret_cast<std::uintptr_t>(key.data())};
            const InitPostCallOutcome o = hooks.call_14c2f0(key.data());
            if (!record(0x14c2f0, args, 1, o)) {
                out.blocked = "call_14c2f0_refused";
                return out;
            }
        }

        std::array<std::uint8_t, 0x18> resolved{};
        {
            const std::uint64_t args[] = {reinterpret_cast<std::uintptr_t>(resolved.data()),
                                          reinterpret_cast<std::uintptr_t>(hooks.registry),
                                          reinterpret_cast<std::uintptr_t>(key.data())};
            const InitPostCallOutcome o =
                hooks.resolve_resource(resolved.data(), hooks.registry, key.data());
            if (!record(kInitA5cd90RegistryRva, args, 3, o)) {
                out.blocked = "resolve_resource_refused";
                return out;
            }
        }
        auto* resource = reinterpret_cast<void*>(load_u64(resolved.data()));
        const bool ready = resolved[8] != 0;
        out.resource = resource;
        out.resource_ready = ready;

        {
            const std::uint64_t args[] = {reinterpret_cast<std::uintptr_t>(resource)};
            const InitPostCallOutcome o = hooks.call_14ccb0(resource);
            if (!record(0x14ccb0, args, 1, o)) {
                out.blocked = "call_14ccb0_refused";
                return out;
            }
        }

        if (ready) {
            bool first_once = false;
            if (!run_once_sequence(hooks, out, first_once, std::move(record))) return out;
            ++out.once_calls;

            // 0xa5cf21 is reached from both first-block once outcomes (byte set or 0xf8f3d0
            // returning zero), so the name call is unconditional here. `bl` becomes 1.
            {
                const std::uint64_t args[] = {reinterpret_cast<std::uintptr_t>(resource),
                                              kInitA5cd90ResourceNameId,
                                              kInitA5cd90OnceDataRva};
                const InitPostCallOutcome o = hooks.call_14cf20(
                    resource, kInitA5cd90ResourceNameId, kInitA5cd90OnceDataRva);
                if (!record(0x14cf20, args, 3, o)) {
                    out.blocked = "call_14cf20_refused";
                    return out;
                }
            }

            bool second_once = false;
            if (!run_once_sequence(hooks, out, second_once, std::move(record))) return out;
            ++out.once_calls;
            // `bl` is now 1, so the second copy's 0xa5cfa6 name call is skipped.

            if (obj[0x20] != 0) {
                const std::uint64_t old = load_u64(obj + 0x10);
                std::uint64_t current = 0;
                {
                    const InitPostCallOutcome o = hooks.call_14d880(&current);
                    if (!record(0x14d880, nullptr, 0, o)) {
                        out.blocked = "call_14d880_refused";
                        return out;
                    }
                }
                const bool replace = (old == current) || (old != reinterpret_cast<std::uint64_t>(resource));
                if (replace) {
                    if (obj[0x18] != 0) {
                        const std::uint64_t args[] = {old};
                        const InitPostCallOutcome o = hooks.call_14d2d0(
                            reinterpret_cast<void*>(old));
                        if (!record(0x14d2d0, args, 1, o)) {
                            out.blocked = "call_14d2d0_old_refused";
                            return out;
                        }
                        obj[0x18] = 0;
                    }
                    store_u64(obj + 0x10, reinterpret_cast<std::uintptr_t>(resource));
                    const std::uint64_t add_args[] = {
                        reinterpret_cast<std::uintptr_t>(resource)};
                    const InitPostCallOutcome add = hooks.call_14ccb0(resource);
                    if (!record(0x14ccb0, add_args, 1, add)) {
                        out.blocked = "call_14ccb0_replace_refused";
                        return out;
                    }
                    const std::uint64_t old_args[] = {old};
                    const InitPostCallOutcome rel =
                        hooks.call_14ccc0(reinterpret_cast<void*>(old));
                    if (!record(0x14ccc0, old_args, 1, rel)) {
                        out.blocked = "call_14ccc0_old_refused";
                        return out;
                    }
                }
            } else {
                obj[0x20] = 1;
                store_u64(obj + 0x10, reinterpret_cast<std::uintptr_t>(resource));
                obj[0x18] = 0;
                const std::uint64_t args[] = {reinterpret_cast<std::uintptr_t>(resource)};
                const InitPostCallOutcome o = hooks.call_14ccb0(resource);
                if (!record(0x14ccb0, args, 1, o)) {
                    out.blocked = "call_14ccb0_install_refused";
                    return out;
                }
            }

            // 0xa5d00b..0xa5d01e: notify the field, destroy and release the looked-up object.
            {
                const std::uint64_t args[] = {
                    reinterpret_cast<std::uintptr_t>(obj + 0x10)};
                const InitPostCallOutcome o = hooks.call_95580(obj + 0x10);
                if (!record(0x95580, args, 1, o)) {
                    out.blocked = "call_95580_refused";
                    return out;
                }
            }
            {
                const std::uint64_t args[] = {reinterpret_cast<std::uintptr_t>(resource)};
                const InitPostCallOutcome o = hooks.call_14d2d0(resource);
                if (!record(0x14d2d0, args, 1, o)) {
                    out.blocked = "call_14d2d0_refused";
                    return out;
                }
            }
            {
                const std::uint64_t args[] = {reinterpret_cast<std::uintptr_t>(resource)};
                const InitPostCallOutcome o = hooks.call_14ccc0(resource);
                if (!record(0x14ccc0, args, 1, o)) {
                    out.blocked = "call_14ccc0_refused";
                    return out;
                }
            }

            // 0xa5d023..0xa5d056: ready-gated destroy, key release and second notify.
            if (ready) {
                const std::uint64_t args[] = {reinterpret_cast<std::uintptr_t>(resource)};
                const InitPostCallOutcome o = hooks.call_14d2d0(resource);
                if (!record(0x14d2d0, args, 1, o)) {
                    out.blocked = "call_14d2d0_ready_refused";
                    return out;
                }
            }
            {
                const std::uint64_t args[] = {reinterpret_cast<std::uintptr_t>(resource)};
                const InitPostCallOutcome o = hooks.call_14ccc0(resource);
                if (!record(0x14ccc0, args, 1, o)) {
                    out.blocked = "call_14ccc0_ready_refused";
                    return out;
                }
            }
            {
                const std::uint64_t args[] = {reinterpret_cast<std::uintptr_t>(key.data())};
                const InitPostCallOutcome o = hooks.release_f590(key.data());
                if (!record(0xf590, args, 1, o)) {
                    out.blocked = "release_f590_refused";
                    return out;
                }
            }
            {
                const std::uint64_t args[] = {
                    reinterpret_cast<std::uintptr_t>(obj + 0x10)};
                const InitPostCallOutcome o = hooks.call_95580(obj + 0x10);
                if (!record(0x95580, args, 1, o)) {
                    out.blocked = "call_95580_second_refused";
                    return out;
                }
            }

            // 0xa5d05b..0xa5d0ba: status query, size handshake and second arena.
            std::uint64_t param = 0;
            {
                const std::uint64_t args[] = {load_u64(obj + 0x10),
                                              reinterpret_cast<std::uintptr_t>(&param)};
                const InitPostCallOutcome o =
                    hooks.call_14d7b0(reinterpret_cast<void*>(load_u64(obj + 0x10)), &param);
                if (!record(0x14d7b0, args, 2, o)) {
                    out.blocked = "call_14d7b0_refused";
                    return out;
                }
            }
            std::int32_t status = 0;
            {
                const std::uint64_t args[] = {param, 0, 0, 0};
                const InitPostCallOutcome o = hooks.call_9892d0(param, &status);
                if (!record(0x9892d0, args, 4, o)) {
                    out.blocked = "call_9892d0_refused";
                    return out;
                }
            }
            store_u32(obj + 0x08, static_cast<std::uint32_t>(status));
            out.field_08 = static_cast<std::uint32_t>(status);

            std::uint64_t second_size = 0;
            {
                const std::uint64_t args[] = {static_cast<std::uint64_t>(status),
                                              reinterpret_cast<std::uintptr_t>(&second_size)};
                const InitPostCallOutcome o = hooks.call_946590(status, &second_size);
                if (!record(0x946590, args, 2, o)) {
                    out.blocked = "call_946590_refused";
                    return out;
                }
            }
            void* arena1 = nullptr;
            {
                const std::uint32_t tag = *hooks.arena_tag;
                const std::uint64_t args[] = {second_size, tag, 0x10, 0};
                const InitPostCallOutcome o =
                    hooks.alloc_12820(second_size, tag, 0x10, 0, &arena1);
                ++out.alloc_calls;
                if (!record(0x12820, args, 4, o)) {
                    out.blocked = "alloc_12820_second_refused";
                    return out;
                }
                if (arena1 == nullptr) {
                    out.blocked = "alloc_12820_second_null";
                    return out;
                }
                out.arena1_bytes = static_cast<std::size_t>(second_size);
            }
            {
                const std::uint64_t args[] = {static_cast<std::uint64_t>(status),
                                              reinterpret_cast<std::uintptr_t>(arena1),
                                              second_size};
                const InitPostCallOutcome o = hooks.call_946650(status, arena1, second_size);
                if (!record(0x946650, args, 3, o)) {
                    out.blocked = "call_946650_refused";
                    return out;
                }
            }
        } else {
            // 0xa5cf39..0xa5cf93: the not-ready path still runs the second once copy and the
            // key/notify cleanup through 0xa5d036..0xa5d056, without registering a name.
            // 0xa5cf39 clears bl; only the second once copy runs on this path.
            const bool first_once = false;
            bool second_once = false;
            if (!run_once_sequence(hooks, out, second_once, std::move(record))) return out;
            ++out.once_calls;
            if (!first_once) {
                const std::uint64_t args[] = {reinterpret_cast<std::uintptr_t>(resource),
                                              kInitA5cd90ResourceNameId,
                                              kInitA5cd90OnceDataRva};
                const InitPostCallOutcome o = hooks.call_14cf20(
                    resource, kInitA5cd90ResourceNameId, kInitA5cd90OnceDataRva);
                if (!record(0x14cf20, args, 3, o)) {
                    out.blocked = "call_14cf20_not_ready_refused";
                    return out;
                }
            }
            {
                const std::uint64_t args[] = {
                    reinterpret_cast<std::uintptr_t>(obj + 0x10)};
                const InitPostCallOutcome o = hooks.call_95580(obj + 0x10);
                if (!record(0x95580, args, 1, o)) {
                    out.blocked = "call_95580_not_ready_refused";
                    return out;
                }
            }
            {
                const std::uint64_t args[] = {reinterpret_cast<std::uintptr_t>(resource)};
                const InitPostCallOutcome o = hooks.call_14d2d0(resource);
                if (!record(0x14d2d0, args, 1, o)) {
                    out.blocked = "call_14d2d0_not_ready_refused";
                    return out;
                }
            }
            {
                const std::uint64_t args[] = {reinterpret_cast<std::uintptr_t>(resource)};
                const InitPostCallOutcome o = hooks.call_14ccc0(resource);
                if (!record(0x14ccc0, args, 1, o)) {
                    out.blocked = "call_14ccc0_not_ready_refused";
                    return out;
                }
            }
            {
                const std::uint64_t args[] = {reinterpret_cast<std::uintptr_t>(resource)};
                const InitPostCallOutcome o = hooks.call_14ccc0(resource);
                if (!record(0x14ccc0, args, 1, o)) {
                    out.blocked = "call_14ccc0_not_ready_second_refused";
                    return out;
                }
            }
            {
                const std::uint64_t args[] = {reinterpret_cast<std::uintptr_t>(key.data())};
                const InitPostCallOutcome o = hooks.release_f590(key.data());
                if (!record(0xf590, args, 1, o)) {
                    out.blocked = "release_f590_not_ready_refused";
                    return out;
                }
            }
            {
                const std::uint64_t args[] = {
                    reinterpret_cast<std::uintptr_t>(obj + 0x10)};
                const InitPostCallOutcome o = hooks.call_95580(obj + 0x10);
                if (!record(0x95580, args, 1, o)) {
                    out.blocked = "call_95580_not_ready_second_refused";
                    return out;
                }
            }
        }
    }

    // 0xa5d0c6..0xa5d0d2: the descriptor's u16 mode is passed as two bytes.
    {
        const std::uint16_t mode = load_u16(hooks.descriptor + 8);
        const std::uint64_t args[] = {static_cast<std::uint8_t>(mode & 0xff),
                                      static_cast<std::uint8_t>(mode >> 8)};
        const InitPostCallOutcome o = hooks.call_994070(
            static_cast<std::uint8_t>(mode & 0xff), static_cast<std::uint8_t>(mode >> 8));
        if (!record(0x994070, args, 2, o)) {
            out.blocked = "call_994070_refused";
            return out;
        }
    }

    // 0xa5d0d7..0xa5d160: UserID check, success log or the fixed error descriptor.
    {
        std::uint64_t value = 0;
        std::int32_t result = 0;
        const std::uint64_t args[] = {reinterpret_cast<std::uintptr_t>(&value),
                                      reinterpret_cast<std::uintptr_t>(&result)};
        const InitPostCallOutcome o = hooks.call_f901e0(&value, &result);
        out.user_id_check = result;
        if (!record(0xf901e0, args, 2, o)) {
            out.blocked = "call_f901e0_refused";
            return out;
        }
        if (result == 0) {
            const std::uint64_t log_args[] = {0, value};
            const InitPostCallOutcome log = hooks.call_994640(0, value);
            if (!record(0x994640, log_args, 2, log)) {
                out.blocked = "call_994640_refused";
                return out;
            }
        } else {
            // The two global loads at 0xa5d0e7..0xa5d108 are inline, not a call; the host
            // predicate only supplies the resulting branch and is not added to the trace.
            bool should_log = false;
            const InitPostCallOutcome condition = hooks.log_condition(&should_log);
            if (!condition.executed) {
                out.blocked = "log_condition_refused";
                return out;
            }
            if (should_log) {
                std::array<std::uint8_t, kInitA5cd90LogDescriptorBytes> block{};
                store_u64(block.data() + 0x00, kInitA5cd90SourcePathRva);
                store_u64(block.data() + 0x08, kInitA5cd90FunctionNameRva);
                store_u32(block.data() + 0x10, static_cast<std::uint32_t>(kInitA5cd90ErrorLine));
                const std::uint64_t log_args[] = {
                    2, 0xf, reinterpret_cast<std::uintptr_t>(block.data()), 0, 1,
                    kInitA5cd90ErrorMessageRva};
                const InitPostCallOutcome log = hooks.call_3e7e0(
                    2, 0xf, block.data(), 0, 1, kInitA5cd90ErrorMessageRva);
                if (!record(0x3e7e0, log_args, 6, log)) {
                    out.blocked = "call_3e7e0_refused";
                    return out;
                }
                ++out.log_calls;
            }
        }
    }

    // 0xa5d165..0xa5d1e1: 0x70-byte task object, descriptor and publication at this+0xa4f8.
    void* task = nullptr;
    {
        const std::uint64_t args[] = {kInitA5cd90Object70Bytes};
        const InitPostCallOutcome o = hooks.alloc_17070(kInitA5cd90Object70Bytes, &task);
        ++out.alloc_calls;
        if (!record(0x17070, args, 1, o)) {
            out.blocked = "alloc_17070_refused";
            return out;
        }
        if (task == nullptr) {
            out.blocked = "alloc_17070_null";
            return out;
        }
    }
    {
        std::array<std::uint8_t, kInitA5cd90TaskDescriptorBytes> descriptor{};
        store_u64(descriptor.data() + 0x00, reinterpret_cast<std::uintptr_t>(obj));
        store_u64(descriptor.data() + 0x08, kInitA5cd90TaskConstant0);
        store_u64(descriptor.data() + 0x10, kInitA5cd90TaskConstant1);
        store_u64(descriptor.data() + 0x18, 0);
        const std::uint64_t args[] = {reinterpret_cast<std::uintptr_t>(task),
                                      reinterpret_cast<std::uintptr_t>(descriptor.data()),
                                      kInitA5cd90TaskSourceRva, kInitA5cd90TaskLine,
                                      kInitA5cd90TaskArg4, 1};
        const InitPostCallOutcome o = hooks.call_3b5c0(
            task, descriptor.data(), kInitA5cd90TaskSourceRva, kInitA5cd90TaskLine,
            kInitA5cd90TaskArg4, 1, kInitA5cd90TaskStackArg);
        if (!record(0x3b5c0, args, 6, o)) {
            out.blocked = "call_3b5c0_refused";
            return out;
        }
    }
    {
        const std::uint64_t old = load_u64(obj + 0xa4f8);
        if (old != reinterpret_cast<std::uint64_t>(task)) {
            if (old != 0) {
                const std::uint64_t args[] = {old};
                const InitPostCallOutcome o =
                    hooks.virtual_release(reinterpret_cast<void*>(old));
                if (!record(0x0, args, 1, o)) {
                    out.blocked = "virtual_release_refused";
                    return out;
                }
            }
            store_u64(obj + 0xa4f8, reinterpret_cast<std::uintptr_t>(task));
        }
    }
    out.object_a4f8 = load_u64(obj + 0xa4f8);

    // 0xa5d1e1: publish the ready byte after the canary comparison.
    obj[0] = 1;
    out.byte0 = obj[0];
    out.flag_18 = obj[0x18];
    out.flag_20 = obj[0x20];

    const std::uint64_t canary0 = hooks.canary();
    const std::uint64_t canary1 = hooks.canary();
    if (canary0 != canary1) {
        out.stack_check_failed = true;
        ++out.trap_calls;
        const InitPostCallOutcome trap = hooks.trap_f8f3a0();
        if (!record(0xf8f3a0, nullptr, 0, trap)) {
            out.blocked = "trap_f8f3a0_refused";
            return out;
        }
    }

    out.ok = true;
    return out;
}

}  // namespace sotc
