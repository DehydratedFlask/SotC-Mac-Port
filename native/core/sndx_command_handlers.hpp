// CPU-01-SLOT306-HANDLERS-NATIVE-01: native model of the three `sndx_` console-command handlers
// built by startup slot 306 (0xa633b0) — sndx_reloadData (0xa5bf20), sndx_categoryMuteAll
// (0xa5bf50), sndx_categoryFilterAll (0xa5c020) — plus the 0xba40 bool-var setter model they
// share. Everything here is a STATIC model of observed shipped-code behaviour
// (docs/checkpoints/2026-10-26-slot306-handlers.md + REA/Ghidra corroboration): no original RVA is
// ever loaded, called or relocated; every original address below is an IDENTITY TAG. The engine
// object itself is not lifted — only the fields the handlers observe are modelled, and the four
// unlifted callees of the reload path (0x989450 / 0x14d2d0 / 0x95580 / 0xb096b0) plus the argument
// parser (0x1e4c0) and the change callback of 0xba40 stay explicit hooks.
//
// Observed contracts (all STATIC_ANALYSIS_VERIFIED; REA dossier evidence in
// local/validation/orch-20261026/rea/, private):
//   * Return codes are 32-bit bpe::ECmdExecuteResult values (0x21da0: 0 = done, 2 = usage printed,
//     any other non-zero = "failed to execute" log line). The sndx handlers return only 0 or 3.
//   * 0xa5bf20: 33 B. `if (engine == nullptr) return 3;` (no side effects, args never read); else
//     call 0xa5f380(engine); return 0.
//   * 0xa5bf50 / 0xa5c020: 192 B, byte-identical template apart from the walked engine vector.
//     `if (engine == nullptr) return 3;` and `if (args.record_count() != 1) return 3;` — the second
//     guard touches nothing. Then b = 0x1e4c0(args, 0, 0, 0); for every 0xc8-stride ConCmdBoolVar
//     in the engine vector call 0xba40(var, var->value_ref(+0x60), &b); return 0.
//     Mute vector: engine+0x8bd0 (begin) / +0x8bd8 (end). Filter vector: engine+0x72a8 / +0x72b0.
//   * 0xba40 (213 B): if the clamp-engaged byte (var+0x6a) is clear the new value passes through;
//     when engaged, bit 0 of the flags word (var+0x1c) selects the unordered boolean mode from the
//     ordered mode, which clamps against the bounds bytes (var+0x69, var+0x6b) and also compares
//     against the current stored value. If the clamped value differs from the stored byte it is
//     stored through the value reference (var+0x60) and the change callback
//     (member fn ptr var+0x70/+0x78/+0x80, virtual bit in +0x78) fires with
//     (this, var, value_ref, &new, &old). No change -> no store, no callback.
//   * 0x1e4c0 (280 B): named/indexed 16-byte-record argument lookup; the compare import is
//     strcasecmp (NID AV6ipCNa4Rw, computed by scripts/ps4_dynlink.nid) — case-insensitive — and
//     the numeric fallback import is _Stoul (NID zlfEH8FmyUA). Tokens "true" (0xf95a1e) and
//     "yes" (0xf95a23) -> 1; "false" (0xf95a27) and "no" (0xf95a2d) -> 0; otherwise
//     _Stoul(text, 0, 10) != 0. Out-of-range/absent entries return the caller's default.
//   * 0xa5f380 (279 B): gated by the engine's loaded byte (+0x00). Pass 1 walks the 0x38-stride
//     bank records (vector begin/end at engine+0x28/+0x30): handle (record+0x10) != -1 ->
//     0x989450(handle) and handle = -1; loaded byte (record+0x08) set -> 0x14d2d0(wrapper at
//     record+0x00) and the byte cleared. Pass 2 re-references records whose reference byte
//     (record+0x14) is set via 0x95580(record). Then 0xb096b0(engine+0x7270, engine+0x7250)
//     (LoadSoundTable(table, name)). Finally the 256 x 0x40 record array at engine+0x1c50 has the
//     8-byte field at record+0x38 of every record zeroed (0x4000 bytes total).
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

// ---- identity tags (never called) ----------------------------------------------------------------
inline constexpr std::uint64_t kSndxReloadDataHandlerRva = 0xA5BF20;
inline constexpr std::uint64_t kSndxCategoryMuteAllHandlerRva = 0xA5BF50;
inline constexpr std::uint64_t kSndxCategoryFilterAllHandlerRva = 0xA5C020;
inline constexpr std::uint64_t kSndxEngineCellRva = 0x32F8FF0;     // qword holding the engine pointer
inline constexpr std::uint64_t kSndxReloadCalleeRva = 0xA5F380;
inline constexpr std::uint64_t kSndxBoolArgLookupRva = 0x1E4C0;
inline constexpr std::uint64_t kSndxBoolVarSetRva = 0xBA40;
inline constexpr std::uint64_t kSndxCmdExecuteRva = 0x21DA0;
inline constexpr std::uint64_t kSndxResourceUnregisterRva = 0x989450;
inline constexpr std::uint64_t kSndxResourceRemoveRefRva = 0x14D2D0;
inline constexpr std::uint64_t kSndxBankAddRefRva = 0x95580;
inline constexpr std::uint64_t kSndxLoadSoundTableRva = 0xB096B0;
// Engine fields the reload callee observes (the engine object itself stays opaque):
inline constexpr std::size_t kSndxEngineLoadedByte = 0x00;
inline constexpr std::size_t kSndxBankVectorBegin = 0x28;
inline constexpr std::size_t kSndxBankVectorEnd = 0x30;
inline constexpr std::size_t kSndxBankRecordSize = 0x38;
inline constexpr std::size_t kSndxBankHandleOffset = 0x10;       // int; -1 when not registered
inline constexpr std::size_t kSndxBankLoadedOffset = 0x08;       // byte
inline constexpr std::size_t kSndxBankWrapperOffset = 0x00;      // qword; RemoveReference argument
inline constexpr std::size_t kSndxBankRefFlagOffset = 0x14;      // byte; pass-2 re-reference gate
inline constexpr std::size_t kSndxSoundTableOffset = 0x7270;
inline constexpr std::size_t kSndxSoundTableNameOffset = 0x7250;
inline constexpr std::size_t kSndxClearArrayOffset = 0x1C50;     // 256 x 0x40 records
inline constexpr std::size_t kSndxClearArrayCount = 256;
inline constexpr std::size_t kSndxClearRecordSize = 0x40;
inline constexpr std::size_t kSndxClearFieldOffset = 0x38;       // 8 bytes zeroed per record
// Category vectors the Mute/Filter handlers walk:
inline constexpr std::size_t kSndxMuteVectorOffset = 0x8BD0;     // begin; end at +0x08
inline constexpr std::size_t kSndxFilterVectorOffset = 0x72A8;   // begin; end at +0x08
inline constexpr std::size_t kSndxCategoryVarCount = 32;
inline constexpr std::size_t kSndxCategoryVarStride = 0xC8;
// Argument records are 16 bytes (the count guard is (end - begin) >> 4 == 1).
inline constexpr std::size_t kSndxArgRecordSize = 0x10;
inline constexpr std::uint32_t kSndxCmdResultDone = 0;
inline constexpr std::uint32_t kSndxCmdResultRejected = 3;

enum class SndxCommandId : std::uint8_t {
    reload_data,        // sndx_reloadData
    category_mute_all,  // sndx_categoryMuteAll
    category_filter_all // sndx_categoryFilterAll
};

// Resolves a handler identity tag (0xa5bf20 / 0xa5bf50 / 0xa5c020) to its native command id.
// Unknown tags return false: nothing in the model dispatches through an unmodelled RVA.
[[nodiscard]] bool sndx_command_id_for_tag(std::uint64_t handler_rva, SndxCommandId& out) noexcept;

// The 0x21da0/0x1e4c0 argument vector: 16-byte records between begin and end.
struct SndxArgsSpan {
    const void* begin = nullptr;
    const void* end = nullptr;
    [[nodiscard]] std::size_t record_count() const noexcept;
};

// One category vector the Mute/Filter handlers walk (0xc8-stride ConCmdBoolVar slots) and the
// 0xa5d990 category builder appends to. `cap` is the vector capacity end; the handlers only read
// [begin, end), the builder grows through the 0xa62bc0 hook when end == cap.
struct SndxCategoryVectorSpan {
    void* begin = nullptr;
    void* end = nullptr;
    void* cap = nullptr;
    [[nodiscard]] std::size_t var_count() const noexcept;   // (end - begin) / 0xc8 when aligned
    [[nodiscard]] bool aligned() const noexcept;
    [[nodiscard]] std::size_t capacity() const noexcept;    // (cap - begin) / 0xc8
};

// ---- hooks (fail closed; a missing hook is a named failure, never silent success) -----------------

// 0x1e4c0 model. `lookup` may implement the observed semantics, or leave null and let
// run_sndx_handler use config_bool_lookup_args_span (same token/decimal rules).
struct SndxGetBoolArgHooks {
    bool (*lookup)(void* host, const SndxArgsSpan& args, std::size_t index, bool default_value) = nullptr;
    void* host = nullptr;
    [[nodiscard]] bool complete() const noexcept { return lookup != nullptr; }
};

// 0xba40 change callback: fires only when the clamped value differs from the stored byte, with
// (var, value_ref, new_value, old_value). The member-pointer/virtual-bit mechanics of the
// original (+0x70/+0x78/+0x80) are the host's to provide.
struct SndxBoolChangeHook {
    void (*changed)(void* host, void* var, void* value_ref, bool new_value, bool old_value) = nullptr;
    void* host = nullptr;
    [[nodiscard]] bool complete() const noexcept { return changed != nullptr; }
};

// The four unlifted callees of 0xa5f380 (see the header comment for the exact per-record fields).
struct SndxEngineReloadHooks {
    // record+0x10 handle != -1: unregister(handle); the model writes -1 back after the call.
    bool (*bank_unregister)(void* host, std::int32_t handle) = nullptr;
    // record+0x08 loaded byte set: remove_reference(wrapper at record+0x00); model clears the byte.
    bool (*bank_remove_reference)(void* host, void* wrapper) = nullptr;
    // pass 2, record+0x14 reference byte set: add_reference(record).
    bool (*bank_add_reference)(void* host, void* record) = nullptr;
    // LoadSoundTable(engine+0x7270, engine+0x7250).
    bool (*load_sound_table)(void* host, void* table, void* name) = nullptr;
    void* host = nullptr;
    [[nodiscard]] bool complete() const noexcept;
};

// Full handler hook set.
struct SndxHandlerHooks {
    SndxGetBoolArgHooks get_bool_arg{};
    SndxBoolChangeHook change_callback{};   // used by sndx_bool_var_set_0xba40
    SndxEngineReloadHooks engine_reload{};
};

// ---- 0xba40 native model -------------------------------------------------------------------------

// Applies the observed 0xba40 semantics to the opaque `var` (raw byte offsets, exactly the loads
// the original performs — the model does not require the slot to be a C++ ConCmdBoolVar):
// clamp gate at +0x6a, mode bit 0 of the flags word at +0x1c, bounds +0x69/+0x6b (the ordered mode
// also compares against the current stored value through the +0x60 value reference), store through
// the value reference, and the change-gated callback hook. Returns whether the value changed (and
// the callback, when provided, fired).
[[nodiscard]] bool sndx_bool_var_set_0xba40(void* var, bool new_value,
                                            const SndxBoolChangeHook& callback) noexcept;

// Byte offsets of 0xba40 inside the var slot (documented for hosts; 0x60/0x68/0x69/0xc0 agree
// with the ConCmdBoolVar layout pinned in concmd_vars.hpp).
inline constexpr std::size_t kSndxVarFlagsOffset = 0x1C;
inline constexpr std::size_t kSndxVarValueRefOffset = 0x60;
inline constexpr std::size_t kSndxVarDefaultOffset = 0x68;
inline constexpr std::size_t kSndxVarMinBoundOffset = 0x69;
inline constexpr std::size_t kSndxVarClampEngagedOffset = 0x6A;
inline constexpr std::size_t kSndxVarMaxBoundOffset = 0x6B;
inline constexpr std::size_t kSndxVarCallbackOffset = 0x70;   // 0x20-byte member-fn pointer
inline constexpr std::size_t kSndxVarAdjacentValueOffset = 0xC0;

// ---- 0xa5f380 native model -----------------------------------------------------------------------

enum class SndxReloadStage : std::uint8_t {
    ok,                    // full model ran (or the loaded-byte guard skipped it)
    skipped_not_loaded,    // engine loaded byte (+0x00) clear: nothing happens, still success
    hooks_missing,
    span_invalid,          // bank vector end < begin or not 0x38-aligned (defensive; the original
                           // trusts the engine — this model refuses to walk garbage)
};

struct SndxReloadResult {
    SndxReloadStage stage = SndxReloadStage::ok;
    std::size_t records_unregistered = 0;
    std::size_t references_removed = 0;
    std::size_t references_added = 0;
    bool sound_table_loaded = false;
    bool clear_array_zeroed = false;
    [[nodiscard]] bool ok() const noexcept { return stage == SndxReloadStage::ok; }
};

// Runs the observed reload sequence against `engine` (opaque; only the offsets above are read).
[[nodiscard]] SndxReloadResult run_sndx_engine_reload(void* engine,
                                                     const SndxEngineReloadHooks& hooks) noexcept;

// ---- the three handlers --------------------------------------------------------------------------

enum class SndxHandlerStage : std::uint8_t {
    ok,
    null_engine,            // engine cell value is null -> 3 (both guards, no side effects)
    args_count,             // Mute/Filter: record count != 1 -> 3, nothing touched
    get_bool_arg_missing,
    value_ref_missing,      // a walked var has no +0x60 value reference (fail closed mid-walk)
    vector_invalid,         // category vector not 0xc8-aligned (defensive, see above)
    reload_hooks_missing,
    reload_span_invalid,
    reload_stage,           // reload ran but reported a non-ok stage (fail closed -> 3)
};

struct SndxHandlerRun {
    std::uint32_t code = kSndxCmdResultRejected;   // the ECmdExecuteResult value returned
    SndxHandlerStage stage = SndxHandlerStage::ok;
    std::size_t vars_applied = 0;                  // bool_var_set calls that completed
    SndxReloadResult reload{};                    // reload_data only
};

// `engine` is the value of the engine cell (0x32f8ff0), not its address. `mute`/`filter` are the
// engine category vectors (engine+0x8bd0 / +0x72a8 spans) — required only for the Mute/Filter
// ids. Missing hooks and malformed spans fail closed with a named stage; the return code stays
// the observed 0/3 contract (0 only on the fully modelled success path).
[[nodiscard]] SndxHandlerRun run_sndx_handler(SndxCommandId id, const void* engine, const SndxArgsSpan& args,
                                             const SndxCategoryVectorSpan& mute,
                                             const SndxCategoryVectorSpan& filter,
                                             const SndxHandlerHooks& hooks) noexcept;

}  // namespace sotc
