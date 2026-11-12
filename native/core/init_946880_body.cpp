#include "init_946880_body.hpp"

#include <atomic>
#include <cstdio>
#include <cstring>
#include <vector>

namespace sotc {
namespace {

void add_call(InitPostCallLog& log, std::uint64_t callee, const char* caller,
              const std::uint64_t* args, std::size_t arg_count,
              const InitPostCallOutcome& outcome) noexcept {
    if (log.count >= kInitPostCallMax) return;
    InitPostCallRecord& record = log.records[log.count++];
    record.callee_rva = callee;
    record.caller = caller;
    record.executed = outcome.executed;
    record.has_return = outcome.has_value;
    record.return_value = outcome.value;
    record.blocker = outcome.executed ? nullptr : outcome.blocker;
    record.arg_count = static_cast<std::uint8_t>(
        arg_count > kInitPostCallArgMax ? kInitPostCallArgMax : arg_count);
    for (std::size_t i = 0; i < record.arg_count; ++i) record.args[i] = args[i];
    std::memcpy(record.detail, outcome.detail, sizeof record.detail);
    record.detail[sizeof record.detail - 1] = '\0';
}

std::int32_t outcome_status(const InitPostCallOutcome& outcome, std::int32_t fallback) noexcept {
    if (!outcome.has_value) return fallback;
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(outcome.value));
}

bool need_service(const void* service, std::size_t size, Init9469f0Result& out) noexcept {
    if (service != nullptr && size >= kInit9469f0ServiceIdOff + sizeof(std::uint32_t)) return true;
    out.ok = false;
    out.blocked = "service_global_invalid";
    return false;
}

bool need_service(const void* service, std::size_t size, Init946880Result& out) noexcept {
    if (service != nullptr && size >= kInit9469f0ServiceIdOff + sizeof(std::uint32_t)) return true;
    out.ok = false;
    out.blocked = "service_global_invalid";
    return false;
}

}  // namespace

Init9469f0Result run_init_9469f0_body(std::int32_t value, void* arg2, void* service_global,
                                      std::size_t service_size,
                                      const Init9469f0Hooks& hooks) noexcept {
    Init9469f0Result out{};
    if (service_global == nullptr) {
        // The retail body's null-service guard returns -3 without touching a handle.
        out.ok = true;
        out.status = -3;
        return out;
    }
    if (!need_service(service_global, service_size, out)) return out;
    if (hooks.lock_service == nullptr || hooks.unlock_service == nullptr ||
        hooks.get_record == nullptr || hooks.copy_name == nullptr ||
        hooks.release_a == nullptr || hooks.release_b == nullptr) {
        out.blocked = "hook_missing";
        return out;
    }

    // 0x946b25..0x946b36: both releases run on every post-lock path. The helper keeps that
    // contract even when the path is a host blocker, instead of returning early.
    auto finish = [&](std::int32_t status, const char* blocker) -> Init9469f0Result {
        const std::uint64_t release_args[] = {reinterpret_cast<std::uint64_t>(out.handle)};
        const InitPostCallOutcome release_a = hooks.release_a(out.handle);
        add_call(out.call_log, 0x9878d0, "0x9469f0", release_args, 1, release_a);
        const InitPostCallOutcome release_b = hooks.release_b(out.handle);
        add_call(out.call_log, 0x987260, "0x9469f0", release_args, 1, release_b);
        const char* release_blocker = nullptr;
        if (!release_a.executed) {
            release_blocker = release_a.blocker != nullptr ? release_a.blocker : "release_a_refused";
        }
        if (!release_b.executed) {
            release_blocker = release_b.blocker != nullptr ? release_b.blocker : "release_b_refused";
        }
        out.status = status;
        out.blocked = blocker != nullptr ? blocker : release_blocker;
        out.ok = out.blocked == nullptr;
        return out;
    };

    std::uint32_t service_id = 0;
    std::memcpy(&service_id,
                static_cast<const std::uint8_t*>(service_global) + kInit9469f0ServiceIdOff,
                sizeof service_id);
    const std::uint64_t lock_args[] = {service_id};
    const InitPostCallOutcome lock = hooks.lock_service(service_id, &out.handle);
    add_call(out.call_log, 0x986f30, "0x9469f0", lock_args, 1, lock);
    if (!lock.executed) {
        out.blocked = lock.blocker != nullptr ? lock.blocker : "lock_refused";
        return out;
    }
    out.status = outcome_status(lock, 0);
    if (out.handle == nullptr) {
        out.blocked = "handle_null";
        return out;
    }
    if (out.status < 0) return out;

    const std::uint64_t unlock_args[] = {reinterpret_cast<std::uint64_t>(out.handle)};
    const InitPostCallOutcome unlock = hooks.unlock_service(out.handle);
    add_call(out.call_log, 0x987720, "0x9469f0", unlock_args, 1, unlock);
    if (!unlock.executed) {
        return finish(out.status, unlock.blocker != nullptr ? unlock.blocker : "unlock_refused");
    }

    // rsi (arg2) is zero on both observed Init calls; the 0x9882e0 path remains explicit.
    if (arg2 != nullptr) {
        if (hooks.get_name == nullptr) return finish(0, "hook_9882e0_missing");
        const std::uint64_t name_args[] = {reinterpret_cast<std::uint64_t>(out.handle),
                                           reinterpret_cast<std::uint64_t>(arg2)};
        const InitPostCallOutcome name = hooks.get_name(out.handle, arg2);
        add_call(out.call_log, 0x9882e0, "0x9469f0", name_args, 2, name);
        if (!name.executed) {
            return finish(out.status,
                          name.blocker != nullptr ? name.blocker : "get_name_refused");
        }
        const std::int32_t name_status = outcome_status(name, 0);
        if (name_status < 0) {
            if (hooks.log_error == nullptr) return finish(name_status, "hook_947e30_missing");
            const std::uint64_t log_args[] = {
                static_cast<std::uint64_t>(static_cast<std::uint32_t>(name_status)), 0x35c, 0,
                reinterpret_cast<std::uint64_t>(arg2)};
            const InitPostCallOutcome log =
                hooks.log_error(name_status, 0x35c, "Unknown Buss/FX patch name (%s)\n", arg2);
            add_call(out.call_log, 0x947e30, "0x9469f0", log_args, 4, log);
            if (!log.executed) {
                return finish(name_status,
                              log.blocker != nullptr ? log.blocker : "log_error_refused");
            }
            return finish(outcome_status(log, name_status), nullptr);
        }
    }

    std::size_t record_size = 0;
    const std::uint64_t size_args[] = {reinterpret_cast<std::uint64_t>(out.handle),
                                       static_cast<std::uint64_t>(
                                           static_cast<std::uint32_t>(value)),
                                       reinterpret_cast<std::uint64_t>(&record_size)};
    const InitPostCallOutcome size = hooks.get_record(out.handle, value, nullptr, &record_size);
    ++out.get_record_calls;
    add_call(out.call_log, 0x988400, "0x9469f0", size_args, 3, size);
    if (!size.executed) {
        return finish(out.status,
                      size.blocker != nullptr ? size.blocker : "record_size_refused");
    }
    const std::int32_t size_status = outcome_status(size, 0);
    if (size_status < 0) return finish(size_status, nullptr);
    if (record_size < kInit9469f0RecordBytes) {
        return finish(out.status, "record_too_small");
    }
    // The retail code uses an alloca-sized buffer; cap the host allocation and fail closed
    // rather than letting a malformed contract size terminate this noexcept body.
    if (record_size > kInit9469f0MaxRecordBytes) {
        return finish(out.status, "record_size_too_large");
    }

    std::vector<std::uint8_t> buffer(record_size);
    const std::uint64_t fill_args[] = {reinterpret_cast<std::uint64_t>(out.handle),
                                       static_cast<std::uint64_t>(
                                           static_cast<std::uint32_t>(value)),
                                       reinterpret_cast<std::uint64_t>(buffer.data())};
    const InitPostCallOutcome fill =
        hooks.get_record(out.handle, value, buffer.data(), nullptr);
    ++out.get_record_calls;
    add_call(out.call_log, 0x988400, "0x9469f0", fill_args, 3, fill);
    if (!fill.executed) {
        return finish(out.status,
                      fill.blocker != nullptr ? fill.blocker : "record_fill_refused");
    }
    const std::int32_t fill_status = outcome_status(fill, 0);
    if (fill_status < 0) return finish(fill_status, nullptr);

    std::uint32_t index = 0;
    const char* source_name = nullptr;
    std::memcpy(&index, buffer.data() + kInit9469f0RecordIndexOff, sizeof index);
    std::memcpy(&source_name, buffer.data() + kInit9469f0RecordNameOff, sizeof source_name);
    out.record_index = index;
    if (source_name == nullptr) {
        source_name = "";
        out.default_name = true;
    }
    // The retail body has no range check; this host guard prevents a native out-of-bounds
    // write and reports the unmodelled record as a named blocker.
    if (index >= 0x10) return finish(out.status, "record_index_out_of_range");
    const std::size_t stride_offset = static_cast<std::size_t>(index) * kInit9469f0ServiceStride;
    if (service_size < kInit9469f0Name1Off + stride_offset + kInit9469f0NameBytes) {
        return finish(out.status, "service_name_out_of_range");
    }

    auto* const bytes = static_cast<std::uint8_t*>(service_global);
    const std::uint64_t copy0_args[] = {
        reinterpret_cast<std::uint64_t>(bytes + kInit9469f0Name0Off + stride_offset),
        kInit9469f0NameBytes, reinterpret_cast<std::uint64_t>(source_name)};
    const InitPostCallOutcome copy0 =
        hooks.copy_name(bytes + kInit9469f0Name0Off + stride_offset, kInit9469f0NameBytes,
                        source_name);
    ++out.copy_calls;
    add_call(out.call_log, 0x9556a0, "0x9469f0", copy0_args, 3, copy0);
    if (!copy0.executed) {
        return finish(out.status,
                      copy0.blocker != nullptr ? copy0.blocker : "copy_name_refused");
    }
    std::memcpy(out.name0.data(), bytes + kInit9469f0Name0Off + stride_offset,
                kInit9469f0NameBytes);
    out.name0[kInit9469f0NameBytes - 1] = '\0';

    const std::uint64_t copy1_args[] = {
        reinterpret_cast<std::uint64_t>(bytes + kInit9469f0Name1Off + stride_offset),
        kInit9469f0NameBytes, reinterpret_cast<std::uint64_t>(source_name)};
    const InitPostCallOutcome copy1 =
        hooks.copy_name(bytes + kInit9469f0Name1Off + stride_offset, kInit9469f0NameBytes,
                        source_name);
    ++out.copy_calls;
    add_call(out.call_log, 0x9556a0, "0x9469f0", copy1_args, 3, copy1);
    if (!copy1.executed) {
        return finish(out.status,
                      copy1.blocker != nullptr ? copy1.blocker : "copy_name_refused");
    }
    std::memcpy(out.name1.data(), bytes + kInit9469f0Name1Off + stride_offset,
                kInit9469f0NameBytes);
    out.name1[kInit9469f0NameBytes - 1] = '\0';

    return finish(0, nullptr);
}

Init946880Result run_init_946880_body(std::int32_t value, void* service_global,
                                      std::size_t service_size,
                                      const Init946880Hooks& hooks) noexcept {
    Init946880Result out{};
    if (hooks.lock == nullptr) {
        out.blocked = "lock_missing";
        return out;
    }
    if (!need_service(service_global, service_size, out)) return out;

    std::atomic_ref<std::uint32_t> lock(*hooks.lock);
    std::uint32_t observed = lock.load(std::memory_order_seq_cst);
    while (static_cast<std::int32_t>(observed) >= 0) {
        if (lock.compare_exchange_weak(observed, observed + 1, std::memory_order_seq_cst)) break;
    }
    if (static_cast<std::int32_t>(observed) < 0) {
        out.ok = true;
        out.status = -3;
        return out;
    }
    out.lock_acquired = true;

    auto run_direct = [&](std::int32_t first) {
        if (hooks.direct_944de0 == nullptr) {
            out.blocked = "hook_944de0_missing";
            return;
        }
        const std::uint64_t direct_args[] = {
            static_cast<std::uint64_t>(static_cast<std::uint32_t>(first)),
            static_cast<std::uint64_t>(static_cast<std::uint32_t>(value)), 0, 0};
        const InitPostCallOutcome direct = hooks.direct_944de0(first, value, nullptr, 0);
        add_call(out.call_log, 0x944de0, "0x946880", direct_args, 4, direct);
        if (!direct.executed) {
            out.blocked = direct.blocker != nullptr ? direct.blocker : "direct_refused";
        } else {
            out.took_944de0 = true;
            out.status = outcome_status(direct, value);
        }
    };

    // 0x9468fd..0x946918: a service id of -1 skips 0x98a450 and calls 0x944de0 with first=1.
    if (hooks.service_id == 0xffffffffu) {
        run_direct(1);
    } else if (hooks.query_98a450 == nullptr) {
        out.blocked = "hook_98a450_missing";
    } else {
        Init98a450Result query{};
        const std::uint64_t query_args[] = {hooks.service_id};
        const InitPostCallOutcome q = hooks.query_98a450(hooks.service_id, &query);
        add_call(out.call_log, 0x98a450, "0x946880", query_args, 1, q);
        if (!q.executed) {
            out.blocked = q.blocker != nullptr ? q.blocker : "query_refused";
        } else if (query.field50 != -1) {
            run_direct(0);
        } else {
            const Init9469f0Result inner =
                run_init_9469f0_body(value, nullptr, service_global, service_size,
                                     hooks.init_hooks);
            out.inner = inner;
            out.took_9469f0 = true;
            const std::uint64_t inner_args[] = {static_cast<std::uint64_t>(
                                                    static_cast<std::uint32_t>(value)),
                                                0};
            InitPostCallOutcome inner_outcome{};
            inner_outcome.executed = inner.ok && inner.blocked == nullptr;
            inner_outcome.has_value = inner.ok;
            inner_outcome.value = static_cast<std::uint64_t>(
                static_cast<std::uint32_t>(inner.status));
            inner_outcome.blocker = inner.blocked;
            std::snprintf(inner_outcome.detail, sizeof inner_outcome.detail,
                          "native 0x9469f0: status=%d calls=%zu", inner.status,
                          inner.call_log.count);
            add_call(out.call_log, 0x9469f0, "0x946880", inner_args, 2, inner_outcome);
            if (!inner.ok || inner.blocked != nullptr) {
                out.blocked = inner.blocked != nullptr ? inner.blocked : "inner_not_ok";
            } else {
                out.status = inner.status;
            }
        }
    }

    std::uint32_t after = lock.load(std::memory_order_seq_cst);
    bool decremented = false;
    while (static_cast<std::int32_t>(after) >= 0) {
        if (lock.compare_exchange_weak(after, after - 1, std::memory_order_seq_cst)) {
            decremented = true;
            break;
        }
    }
    // The retail unlock path leaves ebx at -3 if the lock observed a negative value.
    if (!decremented) out.status = -3;

    if (out.blocked == nullptr && (hooks.log_flag || hooks.log_counter != 0)) {
        if (hooks.log_984700 == nullptr) {
            out.blocked = "hook_984700_missing";
        } else {
            const std::uint64_t log_args[] = {
                0, 0x2000,
                static_cast<std::uint64_t>(static_cast<std::uint32_t>(out.status)),
                0x12ab869, 0x12ab87c,
                static_cast<std::uint64_t>(static_cast<std::uint32_t>(value))};
            const InitPostCallOutcome log =
                hooks.log_984700(0, 0x2000, out.status, 0x12ab869, 0x12ab87c, value);
            ++out.log_calls;
            add_call(out.call_log, 0x984700, "0x946880", log_args, 6, log);
            if (!log.executed) {
                out.blocked = log.blocker != nullptr ? log.blocker : "log_refused";
            }
        }
    }

    out.ok = out.blocked == nullptr;
    return out;
}

}  // namespace sotc
