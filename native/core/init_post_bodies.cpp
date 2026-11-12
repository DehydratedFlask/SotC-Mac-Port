#include "init_post_bodies.hpp"

#include <array>
#include <cstdio>
#include <cstring>

namespace sotc {
namespace {

std::uint32_t fnv1a(const std::uint8_t* bytes, std::size_t size) noexcept {
    std::uint32_t h = 2166136261u;
    for (std::size_t i = 0; i < size; ++i) {
        h ^= bytes[i];
        h *= 16777619u;
    }
    return h;
}

void store_u32(std::uint8_t* base, std::size_t off, std::uint32_t value) noexcept {
    std::memcpy(base + off, &value, sizeof value);
}

bool need(bool condition, Init56dcb0BodyResult& out, const char* blocked) noexcept {
    if (condition) return true;
    out.ok = false;
    out.blocked = blocked;
    return false;
}

bool need(bool condition, Init5c64c0BodyResult& out, const char* blocked) noexcept {
    if (condition) return true;
    out.ok = false;
    out.blocked = blocked;
    return false;
}

bool need(bool condition, Staffroll7f7220Result& out, const char* blocked) noexcept {
    if (condition) return true;
    out.ok = false;
    out.blocked = blocked;
    return false;
}

void add_call(InitPostCallLog& log, std::uint64_t callee, const char* caller,
              const std::uint64_t* args, std::size_t arg_count,
              const InitPostCallOutcome& outcome) noexcept {
    if (log.count >= kInitPostCallMax) return;
    InitPostCallRecord& r = log.records[log.count++];
    r.callee_rva = callee;
    r.caller = caller;
    r.executed = outcome.executed;
    r.has_return = outcome.has_value;
    r.return_value = outcome.value;
    r.blocker = outcome.executed ? nullptr : outcome.blocker;
    r.arg_count = static_cast<std::uint8_t>(arg_count > kInitPostCallArgMax
                                                ? kInitPostCallArgMax
                                                : arg_count);
    for (std::size_t i = 0; i < r.arg_count; ++i) r.args[i] = args[i];
    std::memcpy(r.detail, outcome.detail, sizeof r.detail);
    r.detail[sizeof r.detail - 1] = '\0';
}

}  // namespace

Init56dcb0BodyResult run_init_56dcb0_body(const Init56dcb0BodyHooks& hooks) noexcept {
    Init56dcb0BodyResult out{};
    out.object = hooks.object;
    out.data_cell = hooks.data_cell;
    if (!need(hooks.object != nullptr, out, "object_missing")) return out;
    if (!need(hooks.data_cell != nullptr, out, "data_cell_missing")) return out;
    if (!need(hooks.call_a5cd90 != nullptr, out, "hook_a5cd90_missing")) return out;
    if (!need(hooks.call_946880 != nullptr, out, "hook_946880_missing")) return out;
    if (!need(hooks.call_a5d990 != nullptr, out, "hook_a5d990_missing")) return out;
    if (!need(hooks.call_560b60 != nullptr, out, "hook_560b60_missing")) return out;

    // 0x56dcf2: the stack descriptor is {0x101e7ac, 0x030a}. The qword at -0x20 is zeroed and
    // then bytes 0x0a and 0x03 are written at -0x20/-0x1f, little-endian 0x030a.
    std::array<std::uint8_t, 16> descriptor{};
    std::memcpy(descriptor.data(), &out.descriptor_word0, sizeof out.descriptor_word0);
    std::memcpy(descriptor.data() + 8, &out.descriptor_word1, sizeof out.descriptor_word1);

    {
        const std::uint64_t args[] = {
            reinterpret_cast<std::uint64_t>(hooks.object), out.descriptor_word0,
            out.descriptor_word1};
        const InitPostCallOutcome o = hooks.call_a5cd90(hooks.object, descriptor.data());
        add_call(out.call_log, 0xa5cd90, "0x56dcb0", args, 3, o);
        ++out.calls_a5cd90;
        if (!o.executed) ++out.calls_blocked;
    }

    {
        const std::uint64_t args[] = {1};
        out.arg_946880[0] = 1;
        const InitPostCallOutcome o = hooks.call_946880(1);
        add_call(out.call_log, 0x946880, "0x56dcb0", args, 1, o);
        ++out.calls_946880;
        out.first_946880_arg_one = true;
        if (!o.executed) ++out.calls_blocked;
    }
    {
        const std::uint64_t args[] = {0};
        out.arg_946880[1] = 0;
        const InitPostCallOutcome o = hooks.call_946880(0);
        add_call(out.call_log, 0x946880, "0x56dcb0", args, 1, o);
        ++out.calls_946880;
        out.last_946880_value = 0;
        if (!o.executed) ++out.calls_blocked;
    }

    {
        const std::uint64_t args[] = {reinterpret_cast<std::uint64_t>(hooks.object),
                                      reinterpret_cast<std::uint64_t>(hooks.data_cell)};
        const InitPostCallOutcome o = hooks.call_a5d990(hooks.object, hooks.data_cell);
        add_call(out.call_log, 0xa5d990, "0x56dcb0", args, 2, o);
        ++out.calls_a5d990;
        if (!o.executed) ++out.calls_blocked;
    }

    {
        const InitPostCallOutcome o = hooks.call_560b60();
        add_call(out.call_log, 0x560b60, "0x56dcb0", nullptr, 0, o);
        ++out.calls_560b60;
        if (!o.executed) ++out.calls_blocked;
    }

    out.ok = true;
    out.blocked = nullptr;
    return out;
}

Init5c64c0BodyResult run_init_5c64c0_body(const Init5c64c0BodyHooks& hooks) noexcept {
    Init5c64c0BodyResult out{};
    if (!need(hooks.owner != nullptr, out, "owner_missing")) return out;
    if (!need(hooks.bss_a != nullptr, out, "bss_a_missing")) return out;
    if (!need(hooks.bss_b != nullptr, out, "bss_b_missing")) return out;
    if (!need(hooks.bss_c != nullptr, out, "bss_c_missing")) return out;
    if (!need(hooks.cell != nullptr, out, "cell_missing")) return out;
    if (!need(hooks.call_5b9f90 != nullptr, out, "hook_5b9f90_missing")) return out;
    if (!need(hooks.call_96f20 != nullptr, out, "hook_96f20_missing")) return out;
    if (!need(hooks.call_5ddcc0 != nullptr, out, "hook_5ddcc0_missing")) return out;
    if (!need(hooks.call_5d57f0 != nullptr, out, "hook_5d57f0_missing")) return out;
    if (!need(hooks.call_5d5410 != nullptr, out, "hook_5d5410_missing")) return out;
    if (!need(hooks.call_5d5760 != nullptr, out, "hook_5d5760_missing")) return out;
    if (!need(hooks.call_5d5700 != nullptr, out, "hook_5d5700_missing")) return out;
    if (!need(hooks.call_5c91e0 != nullptr, out, "hook_5c91e0_missing")) return out;
    if (!need(hooks.tail_5cf130 != nullptr, out, "tail_5cf130_missing")) return out;

    // 0x5c64c0: [[0x2ed3670]+0x80] is the source base. The body copies from +0x24a0.
    void* source = nullptr;
    std::memcpy(&source, static_cast<std::uint8_t*>(hooks.owner) + kInit5c64c0SourceOffset,
                sizeof source);
    out.source = source;
    if (!need(source != nullptr, out, "source_null")) return out;
    if (!need(hooks.source_size >= kInit5c64c0SourceCopyOffset + kInit5c64c0SourceCopyBytes,
              out, "source_too_small"))
        return out;

    // 0x5c64c4..0x5c6524: two 32-byte constants and the scalar defaults. The first constant is
    // eight u32 1s; the second is one u32 0 followed by seven u32 1s.
    for (std::size_t off = 0; off <= 0x20; off += 0x20) {
        for (std::size_t i = 0; i < 8; ++i) store_u32(hooks.bss_a, off + i * 4, 1);
    }
    store_u32(hooks.bss_a, 0x40, 1);
    store_u32(hooks.bss_a, 0x44, 1);
    for (std::size_t off = 0x6c; off <= 0x80; off += 4) store_u32(hooks.bss_a, off, 1);
    store_u32(hooks.bss_a, 0x4c, 0);
    for (std::size_t off = 0x50; off <= 0x68; off += 4) store_u32(hooks.bss_a, off, 1);
    store_u32(hooks.bss_a, 0x48, 0);

    {
        const InitPostCallOutcome o = hooks.call_5b9f90();
        add_call(out.call_log, 0x5b9f90, "0x5c64c0", nullptr, 0, o);
        // A partially lifted body reports the sub-slices it did execute. They are real executed
        // native records; the body's own record stays blocked.
        for (std::size_t i = 0; i < o.subrecord_count; ++i) {
            if (out.call_log.count >= kInitPostCallMax) break;
            out.call_log.records[out.call_log.count++] = o.subrecords[i];
        }
        ++out.calls_5b9f90;
        if (!o.executed) ++out.calls_blocked;
    }

    // 0x5c653e..0x5c6579: six zero ymm stores over [0x2fda300, 0x2fda3c0).
    std::memset(hooks.bss_b, 0, kInit5c64c0BssBSize);

    // 0x5c6587..0x5c65ab: ymm2 (+0x24cc) to +0x2c, ymm1 (+0x24c0) to +0x20, ymm0 (+0x24a0) to 0.
    const auto* src = static_cast<const std::uint8_t*>(source) + kInit5c64c0SourceCopyOffset;
    std::memcpy(hooks.bss_c + 0x2c, src + 0x2c, 0x20);
    std::memcpy(hooks.bss_c + 0x20, src + 0x20, 0x20);
    std::memcpy(hooks.bss_c, src, 0x20);

    // The original sets edi/esi/edx/ecx to 0x100/0x100/1/0 before this call.
    void* returned = nullptr;
    {
        const std::uint64_t args[] = {0x100, 0x100, 1, 0};
        const InitPostCallOutcome o = hooks.call_96f20(0x100, 0x100, 1, 0);
        add_call(out.call_log, 0x96f20, "0x5c64c0", args, 4, o);
        ++out.calls_96f20;
        if (o.executed && o.has_value) {
            returned = reinterpret_cast<void*>(static_cast<std::uintptr_t>(o.value));
        } else {
            ++out.calls_blocked;
        }
    }

    // 0x5c65b5: [0x2fecd68] = rax.
    *hooks.cell = reinterpret_cast<std::uintptr_t>(returned);
    out.cell_value = *hooks.cell;

    {
        const InitPostCallOutcome o = hooks.call_5ddcc0();
        add_call(out.call_log, 0x5ddcc0, "0x5c64c0", nullptr, 0, o);
        if (!o.executed) ++out.calls_blocked;
    }
    {
        const InitPostCallOutcome o = hooks.call_5d57f0();
        add_call(out.call_log, 0x5d57f0, "0x5c64c0", nullptr, 0, o);
        if (!o.executed) ++out.calls_blocked;
    }
    {
        const InitPostCallOutcome o = hooks.call_5d5410();
        add_call(out.call_log, 0x5d5410, "0x5c64c0", nullptr, 0, o);
        if (!o.executed) ++out.calls_blocked;
    }
    {
        const InitPostCallOutcome o = hooks.call_5d5760();
        add_call(out.call_log, 0x5d5760, "0x5c64c0", nullptr, 0, o);
        if (!o.executed) ++out.calls_blocked;
    }
    {
        const InitPostCallOutcome o = hooks.call_5d5700();
        add_call(out.call_log, 0x5d5700, "0x5c64c0", nullptr, 0, o);
        if (!o.executed) ++out.calls_blocked;
    }
    {
        const InitPostCallOutcome o = hooks.call_5c91e0();
        add_call(out.call_log, 0x5c91e0, "0x5c64c0", nullptr, 0, o);
        if (!o.executed) ++out.calls_blocked;
    }
    out.calls_after = 6;

    {
        const InitPostCallOutcome o = hooks.tail_5cf130();
        add_call(out.call_log, 0x5cf130, "0x5c64c0", nullptr, 0, o);
        out.tail_called = true;
        if (!o.executed) ++out.calls_blocked;
    }

    out.bss_a_hash = fnv1a(hooks.bss_a, kInit5c64c0BssASize);
    out.bss_b_hash = fnv1a(hooks.bss_b, kInit5c64c0BssBSize);
    out.bss_c_hash = fnv1a(hooks.bss_c, kInit5c64c0BssCSize);
    out.ok = true;
    out.blocked = nullptr;
    return out;
}

Staffroll7f7220Result run_staffroll_7f7220(const Staffroll7f7220Hooks& hooks) noexcept {
    Staffroll7f7220Result out{};
    if (!need(hooks.query_5d06f0 != nullptr, out, "hook_5d06f0_missing")) return out;
    if (!need(hooks.set_7f6f90 != nullptr, out, "hook_7f6f90_missing")) return out;

    {
        const InitPostCallOutcome o = hooks.query_5d06f0();
        add_call(out.call_log, 0x5d06f0, "0x7f7220", nullptr, 0, o);
        if (!o.executed || !o.has_value) {
            ++out.calls_blocked;
            out.ok = false;
            out.blocked = "query_5d06f0_blocked";
            return out;
        }
        out.query_result = (o.value & 0xffu) != 0;
    }

    if (out.query_result) {
        // 0x7f722d: lea 0x128b878; jmp 0x7f724b -> set(1, shadow).
        out.modes[0] = 1;
        out.strings[0] = kStaffrollShadowTextRva;
        const std::uint64_t args[] = {1, kStaffrollShadowTextRva};
        const InitPostCallOutcome o = hooks.set_7f6f90(1, kStaffrollShadowTextRva);
        add_call(out.call_log, 0x7f6f90, "0x7f7220", args, 2, o);
        if (!o.executed) ++out.calls_blocked;
        out.set_calls = 1;
    } else {
        // 0x7f7236: set(0, remaster); 0x7f7244: set(1, original).
        out.modes[0] = 0;
        out.strings[0] = kStaffrollRemasterTextRva;
        {
            const std::uint64_t args[] = {0, kStaffrollRemasterTextRva};
            const InitPostCallOutcome o = hooks.set_7f6f90(0, kStaffrollRemasterTextRva);
            add_call(out.call_log, 0x7f6f90, "0x7f7220", args, 2, o);
            if (!o.executed) ++out.calls_blocked;
        }
        out.modes[1] = 1;
        out.strings[1] = kStaffrollOriginalTextRva;
        {
            const std::uint64_t args[] = {1, kStaffrollOriginalTextRva};
            const InitPostCallOutcome o = hooks.set_7f6f90(1, kStaffrollOriginalTextRva);
            add_call(out.call_log, 0x7f6f90, "0x7f7220", args, 2, o);
            if (!o.executed) ++out.calls_blocked;
        }
        out.set_calls = 2;
    }
    out.ok = true;
    out.blocked = nullptr;
    return out;
}

}  // namespace sotc
