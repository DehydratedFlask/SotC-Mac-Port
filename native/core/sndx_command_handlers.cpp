#include "config_bool_lookup_1e4c0.hpp"
#include "sndx_command_handlers.hpp"

#include <cstring>

#include "concmd_vars.hpp"

namespace sotc {
namespace {

std::uint8_t byte_at(const void* var, std::size_t offset) noexcept {
    std::uint8_t v = 0;
    std::memcpy(&v, static_cast<const std::uint8_t*>(var) + offset, 1);
    return v;
}

void* qword_at(const void* var, std::size_t offset) noexcept {
    void* v = nullptr;
    std::memcpy(&v, static_cast<const std::uint8_t*>(var) + offset, sizeof(v));
    return v;
}

std::uint32_t dword_at(const void* var, std::size_t offset) noexcept {
    std::uint32_t v = 0;
    std::memcpy(&v, static_cast<const std::uint8_t*>(var) + offset, sizeof(v));
    return v;
}

std::int32_t int_at(const void* var, std::size_t offset) noexcept {
    return static_cast<std::int32_t>(dword_at(var, offset));
}

void store_byte(void* var, std::size_t offset, std::uint8_t value) noexcept {
    std::memcpy(static_cast<std::uint8_t*>(var) + offset, &value, 1);
}

void store_int(void* var, std::size_t offset, std::int32_t value) noexcept {
    std::memcpy(static_cast<std::uint8_t*>(var) + offset, &value, sizeof(value));
}

// Pass-1 of 0xa5f380 for one bank record. Returns false only when a required hook is missing
// (fail closed; the original cannot fail).
bool reload_pass_one_record(void* engine, void* record, const SndxEngineReloadHooks& hooks,
                            SndxReloadResult& out) noexcept {
    const std::int32_t handle = int_at(record, kSndxBankHandleOffset);
    if (handle != -1) {
        if (hooks.bank_unregister == nullptr) return false;
        if (!hooks.bank_unregister(hooks.host, handle)) return false;
        store_int(record, kSndxBankHandleOffset, -1);  // OBS: handle := -1 after unregister
        ++out.records_unregistered;
    }
    if (byte_at(record, kSndxBankLoadedOffset) != 0) {
        if (hooks.bank_remove_reference == nullptr) return false;
        if (!hooks.bank_remove_reference(hooks.host, qword_at(record, kSndxBankWrapperOffset))) {
            return false;
        }
        store_byte(record, kSndxBankLoadedOffset, 0);  // OBS: loaded byte cleared after remove-ref
        ++out.references_removed;
    }
    (void)engine;
    return true;
}

}  // namespace

bool sndx_command_id_for_tag(std::uint64_t handler_rva, SndxCommandId& out) noexcept {
    switch (handler_rva) {
        case kSndxReloadDataHandlerRva: out = SndxCommandId::reload_data; return true;
        case kSndxCategoryMuteAllHandlerRva: out = SndxCommandId::category_mute_all; return true;
        case kSndxCategoryFilterAllHandlerRva: out = SndxCommandId::category_filter_all; return true;
        default: return false;
    }
}

std::size_t SndxArgsSpan::record_count() const noexcept {
    if (end == nullptr || begin == nullptr || end < begin) return 0;
    const std::size_t bytes = static_cast<std::size_t>(static_cast<const char*>(end) -
                                                       static_cast<const char*>(begin));
    return bytes / kSndxArgRecordSize;
}

std::size_t SndxCategoryVectorSpan::var_count() const noexcept {
    if (end == nullptr || begin == nullptr || end < begin) return 0;
    const std::size_t bytes = static_cast<std::size_t>(static_cast<char*>(end) -
                                                       static_cast<char*>(begin));
    return bytes / kSndxCategoryVarStride;
}

bool SndxCategoryVectorSpan::aligned() const noexcept {
    if (end == nullptr || begin == nullptr || end < begin) return false;
    const std::size_t bytes = static_cast<std::size_t>(static_cast<char*>(end) -
                                                       static_cast<char*>(begin));
    return bytes % kSndxCategoryVarStride == 0;
}

std::size_t SndxCategoryVectorSpan::capacity() const noexcept {
    if (cap == nullptr || begin == nullptr || cap < begin) return 0;
    const std::size_t bytes = static_cast<std::size_t>(static_cast<char*>(cap) - static_cast<char*>(begin));
    return bytes / kSndxCategoryVarStride;
}

bool SndxEngineReloadHooks::complete() const noexcept {
    return bank_unregister != nullptr && bank_remove_reference != nullptr &&
           bank_add_reference != nullptr && load_sound_table != nullptr;
}

bool sndx_bool_var_set_0xba40(void* var, bool new_value, const SndxBoolChangeHook& callback) noexcept {
    if (var == nullptr) return false;
    void* value_ref = qword_at(var, kSndxVarValueRefOffset);
    if (value_ref == nullptr) return false;   // fail closed: 0xba40 always stores through +0x60
    std::uint8_t value = new_value ? 1 : 0;
    if (byte_at(var, kSndxVarClampEngagedOffset) != 0) {
        const std::uint8_t stored = byte_at(value_ref, 0);   // OBS: compare/store via the value ref
        const std::uint8_t min_bound = byte_at(var, kSndxVarMinBoundOffset);
        const std::uint8_t max_bound = byte_at(var, kSndxVarMaxBoundOffset);
        if ((dword_at(var, kSndxVarFlagsOffset) & 1) != 0) {
            // Ordered mode (REA-observed): clamp against the bounds, with the current stored value
            // participating in the boundary cases.
            if (max_bound < value) {
                value = max_bound;
                if (stored == max_bound) value = min_bound;
            } else if (value < min_bound) {
                value = max_bound;
                if (stored != min_bound) value = min_bound;
            }
        } else {
            // Unordered boolean mode (REA-observed gate sequence).
            bool result = new_value;
            const bool lower = min_bound != 0;
            const bool upper = max_bound != 0;
            if (lower) result = true;
            if (!new_value) result = lower;
            if (result) result = upper;
            value = (upper && result) ? 1 : 0;
        }
    }
    const std::uint8_t old_value = byte_at(value_ref, 0);
    if (value == old_value) return false;     // OBS: no change -> no store, no callback
    std::memcpy(value_ref, &value, 1);
    if (callback.changed != nullptr) {
        callback.changed(callback.host, var, value_ref, value != 0, old_value != 0);
    }
    return true;
}

SndxReloadResult run_sndx_engine_reload(void* engine, const SndxEngineReloadHooks& hooks) noexcept {
    SndxReloadResult out;
    if (engine == nullptr) {
        out.stage = SndxReloadStage::hooks_missing;  // unreachable on the original path (guard above)
        return out;
    }
    if (byte_at(engine, kSndxEngineLoadedByte) == 0) {
        out.stage = SndxReloadStage::skipped_not_loaded;  // OBS: the loaded byte gates the whole body
        return out;
    }
    if (!hooks.complete()) {
        out.stage = SndxReloadStage::hooks_missing;
        return out;
    }
    void* begin = qword_at(engine, kSndxBankVectorBegin);
    void* end = qword_at(engine, kSndxBankVectorEnd);
    if (end < begin) {
        out.stage = SndxReloadStage::span_invalid;
        return out;
    }
    const std::size_t bytes = static_cast<std::size_t>(static_cast<char*>(end) - static_cast<char*>(begin));
    if (bytes % kSndxBankRecordSize != 0) {
        out.stage = SndxReloadStage::span_invalid;
        return out;
    }
    const std::size_t count = bytes / kSndxBankRecordSize;
    for (std::size_t i = 0; i < count; ++i) {
        void* record = static_cast<char*>(begin) + i * kSndxBankRecordSize;
        if (!reload_pass_one_record(engine, record, hooks, out)) {
            out.stage = SndxReloadStage::hooks_missing;
            return out;
        }
    }
    for (std::size_t i = 0; i < count; ++i) {  // OBS pass 2: re-reference flagged records
        void* record = static_cast<char*>(begin) + i * kSndxBankRecordSize;
        if (byte_at(record, kSndxBankRefFlagOffset) != 0) {
            if (!hooks.bank_add_reference(hooks.host, record)) {
                out.stage = SndxReloadStage::hooks_missing;
                return out;
            }
            ++out.references_added;
        }
    }
    if (!hooks.load_sound_table(hooks.host, static_cast<char*>(engine) + kSndxSoundTableOffset,
                                static_cast<char*>(engine) + kSndxSoundTableNameOffset)) {
        out.stage = SndxReloadStage::hooks_missing;
        return out;
    }
    out.sound_table_loaded = true;
    for (std::size_t i = 0; i < kSndxClearArrayCount; ++i) {  // OBS: 256 x 0x40, field +0x38 zeroed
        std::memset(static_cast<char*>(engine) + kSndxClearArrayOffset + i * kSndxClearRecordSize +
                        kSndxClearFieldOffset,
                    0, 8);
    }
    out.clear_array_zeroed = true;
    return out;
}

SndxHandlerRun run_sndx_handler(SndxCommandId id, const void* engine, const SndxArgsSpan& args,
                                const SndxCategoryVectorSpan& mute, const SndxCategoryVectorSpan& filter,
                                const SndxHandlerHooks& hooks) noexcept {
    SndxHandlerRun run;
    if (engine == nullptr) {  // OBS: both guards return 3 with no side effects
        run.code = kSndxCmdResultRejected;
        run.stage = SndxHandlerStage::null_engine;
        return run;
    }
    switch (id) {
        case SndxCommandId::reload_data: {
            // OBS: args are never read; the whole body is the reload callee behind the null guard.
            const SndxReloadResult reload = run_sndx_engine_reload(const_cast<void*>(engine),
                                                                  hooks.engine_reload);
            run.reload = reload;
            switch (reload.stage) {
                case SndxReloadStage::ok:
                case SndxReloadStage::skipped_not_loaded:
                    run.code = kSndxCmdResultDone;
                    return run;
                case SndxReloadStage::hooks_missing:
                    run.stage = SndxHandlerStage::reload_hooks_missing;
                    return run;
                case SndxReloadStage::span_invalid:
                    run.stage = SndxHandlerStage::reload_span_invalid;
                    return run;
            }
            run.stage = SndxHandlerStage::reload_stage;
            return run;
        }
        case SndxCommandId::category_mute_all:
        case SndxCommandId::category_filter_all: {
            if (args.record_count() != 1) {  // OBS: (end - begin) >> 4 == 1, touching nothing
                run.code = kSndxCmdResultRejected;
                run.stage = SndxHandlerStage::args_count;
                return run;
            }
            const bool requested =
                hooks.get_bool_arg.complete()
                    ? hooks.get_bool_arg.lookup(hooks.get_bool_arg.host, args, 0, false)
                    : config_bool_lookup_args_span(args, 0, false);
            const SndxCategoryVectorSpan& span =
                id == SndxCommandId::category_mute_all ? mute : filter;
            if (!span.aligned()) {
                run.stage = SndxHandlerStage::vector_invalid;
                return run;
            }
            const std::size_t count = span.var_count();
            for (std::size_t i = 0; i < count; ++i) {
                void* var = static_cast<char*>(span.begin) + i * kSndxCategoryVarStride;
                if (qword_at(var, kSndxVarValueRefOffset) == nullptr) {
                    run.stage = SndxHandlerStage::value_ref_missing;   // fail closed, partial walk
                    return run;
                }
                (void)sndx_bool_var_set_0xba40(var, requested, hooks.change_callback);
                ++run.vars_applied;
            }
            run.code = kSndxCmdResultDone;
            return run;
        }
    }
    return run;  // unreachable
}

}  // namespace sotc
