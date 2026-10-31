// CPU-01-SLOT306-HANDLERS-NATIVE-01: fixture for the native sndx_ command-handler model
// (core/sndx_command_handlers). The contracts below are the observed shipped-code behaviour of
// docs/checkpoints/2026-10-26-slot306-handlers.md (static analysis; REA/Ghidra-corroborated), never
// a runtime claim. Sections:
//   1. tag resolution: the three handler RVAs map; unknown RVAs are refused
//   2. 0xba40 model: pass-through, ordered clamp (both boundary cases incl. the stored-value
//      comparisons), unordered boolean gates, change/no-change callback, value_ref stored
//   3. 0xa5f380 model: loaded-byte gate, pass-1 unregister/remove-ref per record (+0x10 handle,
//      +0x08 loaded byte, +0x00 wrapper), pass-2 re-reference (+0x14), load-table (+0x7270/+0x7250),
//      the 256x0x40 clear loop (field +0x38), missing-hook fail-closed, span guards
//   4. handlers: 0 / 3 returns, null engine, args count 0 and 2, mute/filter vector choice
//      (+0x8bd0 vs +0x72a8), stride-0xc8 walk of all 32 slots, parsed value flows to every var
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/concmd_vars.hpp"
#include "core/config_bool_lookup_1e4c0.hpp"
#include "core/sndx_command_handlers.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

// ---- test fixtures -------------------------------------------------------------------------------

struct VarSlot {
    // A raw 0xc8-byte var slot, matching the inline shape the engine-init builder writes and the
    // static bool vars of slot 306: +0x60 value ref (points at +0xc0 here), +0x68 default,
    // +0x69/+0x6b bounds, +0x6a clamp gate, +0x1c flags, +0xc0 the value byte.
    // (No alignas: the struct size must stay exactly 0xc8 so array stride matches the vector walk.)
    std::uint8_t slot[kSndxCategoryVarStride];

    VarSlot() { std::memset(slot, 0, sizeof(slot)); }
    void wire() {
        void* ref = slot + kSndxVarAdjacentValueOffset;
        std::memcpy(slot + kSndxVarValueRefOffset, &ref, sizeof(ref));
    }
    std::uint8_t value() const { return slot[kSndxVarAdjacentValueOffset]; }
    void set_byte(std::size_t off, std::uint8_t v) { slot[off] = v; }
};

struct CategoryBuffer {
    VarSlot vars[kSndxCategoryVarCount];
    CategoryBuffer() {
        for (auto& v : vars) v.wire();
    }
};

// One 0x38-stride bank record for the reload model: +0x00 wrapper, +0x08 loaded, +0x10 handle,
// +0x14 ref flag.
struct BankRecord {
    // (No alignas: sizeof must stay exactly 0x38 so the array stride matches the vector walk.)
    std::uint8_t record[kSndxBankRecordSize];
    BankRecord() { std::memset(record, 0, sizeof(record)); }
    void set_handle(std::int32_t h) { std::memcpy(record + kSndxBankHandleOffset, &h, sizeof(h)); }
    std::int32_t handle() const {
        std::int32_t h;
        std::memcpy(&h, record + kSndxBankHandleOffset, sizeof(h));
        return h;
    }
};

// A minimal engine buffer: loaded byte, bank vector (3 records), the two category vectors and the
// 256x0x40 clear array. Offsets per the model constants.
struct EngineBuffer {
    alignas(16) std::uint8_t engine[0x40000];  // big enough for +0x1c50 + 256*0x40 + margin
    BankRecord banks[3];

    EngineBuffer() {
        std::memset(engine, 0, sizeof(engine));
        void* b = banks;
        void* e = banks + 3;
        std::memcpy(engine + kSndxBankVectorBegin, &b, sizeof(b));
        std::memcpy(engine + kSndxBankVectorEnd, &e, sizeof(e));
    }
    void* base() { return engine; }
    std::uint8_t& loaded() { return engine[kSndxEngineLoadedByte]; }
    std::uint8_t& clear_field(std::size_t i) {
        return engine[kSndxClearArrayOffset + i * kSndxClearRecordSize + kSndxClearFieldOffset];
    }
};

// Hook hosts that record what the model invoked.
struct BoolArgHost {
    bool result = false;
    std::size_t calls = 0;
    std::size_t last_index = 99;
    bool last_default = true;
};
bool bool_arg_lookup(void* h, const SndxArgsSpan& args, std::size_t index, bool default_value) {
    auto* host = static_cast<BoolArgHost*>(h);
    ++host->calls;
    host->last_index = index;
    host->last_default = default_value;
    CHECK(args.begin != nullptr && args.end != nullptr);
    return host->result;
}

struct ChangeHost {
    std::size_t calls = 0;
    bool last_new = false;
    bool last_old = false;
    void* last_var = nullptr;
    void* last_ref = nullptr;
};
void change_cb(void* h, void* var, void* ref, bool nv, bool ov) {
    auto* host = static_cast<ChangeHost*>(h);
    ++host->calls;
    host->last_new = nv;
    host->last_old = ov;
    host->last_var = var;
    host->last_ref = ref;
}

struct ReloadHost {
    std::size_t unregister_calls = 0;
    std::size_t remove_ref_calls = 0;
    std::size_t add_ref_calls = 0;
    std::size_t load_table_calls = 0;
    std::int32_t last_handle = 0;
    void* last_wrapper = nullptr;
    void* last_record = nullptr;
    void* last_table = nullptr;
    void* last_name = nullptr;
    bool fail = false;
};
bool rl_unregister(void* h, std::int32_t handle) {
    auto* host = static_cast<ReloadHost*>(h);
    ++host->unregister_calls;
    host->last_handle = handle;
    return !host->fail;
}
bool rl_remove_ref(void* h, void* wrapper) {
    auto* host = static_cast<ReloadHost*>(h);
    ++host->remove_ref_calls;
    host->last_wrapper = wrapper;
    return !host->fail;
}
bool rl_add_ref(void* h, void* record) {
    auto* host = static_cast<ReloadHost*>(h);
    ++host->add_ref_calls;
    host->last_record = record;
    return !host->fail;
}
bool rl_load_table(void* h, void* table, void* name) {
    auto* host = static_cast<ReloadHost*>(h);
    ++host->load_table_calls;
    host->last_table = table;
    host->last_name = name;
    return !host->fail;
}

SndxEngineReloadHooks full_reload_hooks(ReloadHost& host) {
    SndxEngineReloadHooks hooks;
    hooks.bank_unregister = &rl_unregister;
    hooks.bank_remove_reference = &rl_remove_ref;
    hooks.bank_add_reference = &rl_add_ref;
    hooks.load_sound_table = &rl_load_table;
    hooks.host = &host;
    return hooks;
}

void test_tag_resolution() {
    SndxCommandId id{};
    CHECK(sndx_command_id_for_tag(kSndxReloadDataHandlerRva, id) && id == SndxCommandId::reload_data);
    CHECK(sndx_command_id_for_tag(kSndxCategoryMuteAllHandlerRva, id) && id == SndxCommandId::category_mute_all);
    CHECK(sndx_command_id_for_tag(kSndxCategoryFilterAllHandlerRva, id) &&
          id == SndxCommandId::category_filter_all);
    CHECK(!sndx_command_id_for_tag(0x1234, id));
    CHECK(!sndx_command_id_for_tag(0, id));
}

void test_bool_var_set_0xba40() {
    // Pass-through: clamp gate clear -> value stored through the +0x60 ref, callback fires.
    {
        VarSlot v;
        v.wire();
        ChangeHost ch;
        SndxBoolChangeHook cb;
        cb.changed = &change_cb;
        cb.host = &ch;
        CHECK(sndx_bool_var_set_0xba40(v.slot, true, cb));
        CHECK(v.value() == 1);
        CHECK(ch.calls == 1 && ch.last_new && !ch.last_old && ch.last_ref == v.slot + 0xc0);
        // No change: no store, no callback.
        CHECK(!sndx_bool_var_set_0xba40(v.slot, true, cb));
        CHECK(ch.calls == 1);
        CHECK(sndx_bool_var_set_0xba40(v.slot, false, cb));
        CHECK(v.value() == 0 && ch.calls == 2 && !ch.last_new && ch.last_old);
    }
    // Ordered mode, plain clamp: bounds 0..1, gate engaged, flags bit set.
    {
        VarSlot v;
        v.wire();
        v.set_byte(kSndxVarClampEngagedOffset, 1);
        std::uint32_t flags = 1;
        std::memcpy(v.slot + kSndxVarFlagsOffset, &flags, sizeof(flags));
        v.set_byte(kSndxVarMinBoundOffset, 0);
        v.set_byte(kSndxVarMaxBoundOffset, 1);
        CHECK(sndx_bool_var_set_0xba40(v.slot, true, {}));
        CHECK(v.value() == 1);  // 1 <= max, no clamp
        CHECK(!sndx_bool_var_set_0xba40(v.slot, true, {}));  // no change now
        CHECK(sndx_bool_var_set_0xba40(v.slot, false, {}));
        CHECK(v.value() == 0);  // 0 >= min
    }
    // Ordered mode boundary case (REA-observed): max < new && stored == max -> value becomes min.
    {
        VarSlot v;
        v.wire();
        v.set_byte(kSndxVarClampEngagedOffset, 1);
        std::uint32_t flags = 1;
        std::memcpy(v.slot + kSndxVarFlagsOffset, &flags, sizeof(flags));
        v.set_byte(kSndxVarMinBoundOffset, 2);
        v.set_byte(kSndxVarMaxBoundOffset, 5);
        v.slot[kSndxVarAdjacentValueOffset] = 5;  // stored == max
        CHECK(sndx_bool_var_set_0xba40(v.slot, 9, {}));
        CHECK(v.value() == 2);  // clamped to max, then stored==max flips to min
    }
    // Ordered mode boundary case: new < min && stored == min -> value stays max.
    {
        VarSlot v;
        v.wire();
        v.set_byte(kSndxVarClampEngagedOffset, 1);
        std::uint32_t flags = 1;
        std::memcpy(v.slot + kSndxVarFlagsOffset, &flags, sizeof(flags));
        v.set_byte(kSndxVarMinBoundOffset, 4);
        v.set_byte(kSndxVarMaxBoundOffset, 7);
        v.slot[kSndxVarAdjacentValueOffset] = 4;  // stored == min
        CHECK(sndx_bool_var_set_0xba40(v.slot, 1, {}));
        CHECK(v.value() == 7);  // below min with stored==min -> max
    }
    // Unordered boolean gates (REA-observed sequence): lower forces true, upper gates the result.
    {
        VarSlot v;
        v.wire();
        v.set_byte(kSndxVarClampEngagedOffset, 1);
        v.set_byte(kSndxVarMinBoundOffset, 1);  // lower
        v.set_byte(kSndxVarMaxBoundOffset, 0);  // upper clear -> value always 0
        CHECK(!sndx_bool_var_set_0xba40(v.slot, true, {}));
        CHECK(v.value() == 0);
        v.set_byte(kSndxVarMaxBoundOffset, 1);  // upper set
        CHECK(sndx_bool_var_set_0xba40(v.slot, false, {}));
        CHECK(v.value() == 1);  // lower forces true, upper keeps it
    }
    // Missing value ref: fail closed, nothing stored, no callback.
    {
        VarSlot v;  // not wired: +0x60 stays null
        ChangeHost ch;
        SndxBoolChangeHook cb;
        cb.changed = &change_cb;
        cb.host = &ch;
        CHECK(!sndx_bool_var_set_0xba40(v.slot, true, cb));
        CHECK(ch.calls == 0);
        CHECK(!sndx_bool_var_set_0xba40(nullptr, true, cb));
    }
}

void test_engine_reload() {
    // Loaded byte clear: whole body skipped, success.
    {
        EngineBuffer e;
        ReloadHost rh;
        SndxReloadResult r = run_sndx_engine_reload(e.base(), full_reload_hooks(rh));
        CHECK(r.stage == SndxReloadStage::skipped_not_loaded && !r.ok());
        CHECK(rh.unregister_calls == 0 && rh.load_table_calls == 0);
    }
    // Full pass: 3 records with distinct fields.
    {
        EngineBuffer e;
        e.loaded() = 1;
        e.banks[0].set_handle(7);
        e.banks[1].set_handle(-1);
        e.banks[2].set_handle(42);
        e.banks[1].record[kSndxBankLoadedOffset] = 1;      // loaded -> remove-ref
        e.banks[1].record[kSndxBankWrapperOffset] = 0;     // wrapper qword (checked via hook arg)
        e.banks[2].record[kSndxBankRefFlagOffset] = 1;     // pass-2 re-reference
        e.clear_field(0) = 0xAB;
        e.clear_field(255) = 0xCD;
        ReloadHost rh;
        SndxReloadResult r = run_sndx_engine_reload(e.base(), full_reload_hooks(rh));
        CHECK(r.stage == SndxReloadStage::ok && r.ok());
        CHECK(r.records_unregistered == 2);      // handles 7 and 42
        CHECK(rh.unregister_calls == 2 && rh.last_handle == 42);
        CHECK(e.banks[0].handle() == -1 && e.banks[2].handle() == -1);  // handle := -1 after unregister
        CHECK(r.references_removed == 1 && rh.remove_ref_calls == 1);
        CHECK(e.banks[1].record[kSndxBankLoadedOffset] == 0);
        CHECK(r.references_added == 1 && rh.add_ref_calls == 1 &&
              rh.last_record == e.banks[2].record);
        CHECK(r.sound_table_loaded && rh.load_table_calls == 1);
        CHECK(rh.last_table == e.engine + kSndxSoundTableOffset);
        CHECK(rh.last_name == e.engine + kSndxSoundTableNameOffset);
        CHECK(r.clear_array_zeroed);
        CHECK(e.clear_field(0) == 0 && e.clear_field(255) == 0);
        // Every one of the 256 records' +0x38 field was zeroed.
        for (std::size_t i = 0; i < kSndxClearArrayCount; ++i) CHECK(e.clear_field(i) == 0);
    }
    // Hook failure and missing hooks fail closed without side-effect claims.
    {
        EngineBuffer e;
        e.loaded() = 1;
        e.banks[0].set_handle(3);
        ReloadHost rh;
        rh.fail = true;
        SndxReloadResult r = run_sndx_engine_reload(e.base(), full_reload_hooks(rh));
        CHECK(r.stage == SndxReloadStage::hooks_missing && !r.ok());
        CHECK(r.records_unregistered == 0);  // hook refused: no handle write-back
        CHECK(e.banks[0].handle() == 3);
        SndxEngineReloadHooks empty{};
        CHECK(run_sndx_engine_reload(e.base(), empty).stage == SndxReloadStage::hooks_missing);
    }
    // Span guards: end < begin and misaligned sizes are refused.
    {
        EngineBuffer e;
        e.loaded() = 1;
        void* b = e.banks;
        void* bad = static_cast<char*>(b) - 8;
        std::memcpy(e.engine + kSndxBankVectorEnd, &bad, sizeof(bad));
        ReloadHost rh;
        CHECK(run_sndx_engine_reload(e.base(), full_reload_hooks(rh)).stage ==
              SndxReloadStage::span_invalid);
        void* e1 = static_cast<char*>(b) + 5;  // not 0x38-aligned
        std::memcpy(e.engine + kSndxBankVectorEnd, &e1, sizeof(e1));
        CHECK(run_sndx_engine_reload(e.base(), full_reload_hooks(rh)).stage ==
              SndxReloadStage::span_invalid);
    }
}

void test_handlers() {
    CategoryBuffer mute;
    CategoryBuffer filter;
    SndxCategoryVectorSpan mute_span{mute.vars, mute.vars + kSndxCategoryVarCount};
    SndxCategoryVectorSpan filter_span{filter.vars, filter.vars + kSndxCategoryVarCount};
    EngineBuffer engine;

    SndxHandlerHooks hooks;
    BoolArgHost arg_host;
    hooks.get_bool_arg.lookup = &bool_arg_lookup;
    hooks.get_bool_arg.host = &arg_host;
    ChangeHost change_host;
    hooks.change_callback.changed = &change_cb;
    hooks.change_callback.host = &change_host;
    ReloadHost reload_host;
    hooks.engine_reload = full_reload_hooks(reload_host);

    const std::uint8_t one_record[16] = {};
    const std::uint8_t two_records[32] = {};
    SndxArgsSpan one{one_record, one_record + 16};
    SndxArgsSpan zero{one_record, one_record};
    SndxArgsSpan two{two_records, two_records + 32};

    // Null engine: 3 for all three ids, no hook interaction.
    {
        SndxHandlerRun r = run_sndx_handler(SndxCommandId::reload_data, nullptr, one, mute_span,
                                            filter_span, hooks);
        CHECK(r.code == kSndxCmdResultRejected && r.stage == SndxHandlerStage::null_engine);
        CHECK(reload_host.unregister_calls == 0);
        r = run_sndx_handler(SndxCommandId::category_mute_all, nullptr, one, mute_span, filter_span,
                             hooks);
        CHECK(r.code == kSndxCmdResultRejected && r.stage == SndxHandlerStage::null_engine);
        r = run_sndx_handler(SndxCommandId::category_filter_all, nullptr, one, mute_span,
                             filter_span, hooks);
        CHECK(r.code == kSndxCmdResultRejected && r.stage == SndxHandlerStage::null_engine);
        CHECK(arg_host.calls == 0 && change_host.calls == 0);
    }
    // Mute/Filter argument-count guard: 0 and 2 records -> 3, nothing touched.
    {
        for (SndxCommandId id : {SndxCommandId::category_mute_all, SndxCommandId::category_filter_all}) {
            SndxHandlerRun r = run_sndx_handler(id, engine.base(), zero, mute_span, filter_span, hooks);
            CHECK(r.code == kSndxCmdResultRejected && r.stage == SndxHandlerStage::args_count);
            r = run_sndx_handler(id, engine.base(), two, mute_span, filter_span, hooks);
            CHECK(r.code == kSndxCmdResultRejected && r.stage == SndxHandlerStage::args_count);
        }
        CHECK(arg_host.calls == 0 && change_host.calls == 0);
    }
    // Reload handler: engine loaded byte clear -> 0, skipped; set -> full model -> 0.
    {
        SndxHandlerRun r = run_sndx_handler(SndxCommandId::reload_data, engine.base(), zero, mute_span,
                                            filter_span, hooks);
        CHECK(r.code == kSndxCmdResultDone && r.reload.stage == SndxReloadStage::skipped_not_loaded);
        engine.loaded() = 1;
        engine.banks[0].set_handle(11);
        engine.banks[1].set_handle(-1);  // unset records must carry the not-registered sentinel
        engine.banks[2].set_handle(-1);
        r = run_sndx_handler(SndxCommandId::reload_data, engine.base(), one, mute_span, filter_span,
                             hooks);
        CHECK(r.code == kSndxCmdResultDone && r.reload.ok() && r.reload.records_unregistered == 1);
        CHECK(engine.banks[0].handle() == -1);
        // Missing reload hooks: fail closed -> 3.
        SndxHandlerHooks partial = hooks;
        partial.engine_reload = SndxEngineReloadHooks{};
        r = run_sndx_handler(SndxCommandId::reload_data, engine.base(), one, mute_span, filter_span,
                             partial);
        CHECK(r.code == kSndxCmdResultRejected && r.stage == SndxHandlerStage::reload_hooks_missing);
    }
    // Mute: one arg -> parsed value applied to all 32 mute slots only, callback per change.
    {
        arg_host.result = true;
        SndxHandlerRun r = run_sndx_handler(SndxCommandId::category_mute_all, engine.base(), one,
                                            mute_span, filter_span, hooks);
        CHECK(r.code == kSndxCmdResultDone && r.stage == SndxHandlerStage::ok);
        CHECK(arg_host.calls == 1 && arg_host.last_index == 0 && !arg_host.last_default);
        CHECK(r.vars_applied == kSndxCategoryVarCount);
        for (std::size_t i = 0; i < kSndxCategoryVarCount; ++i) CHECK(mute.vars[i].value() == 1);
        for (std::size_t i = 0; i < kSndxCategoryVarCount; ++i) CHECK(filter.vars[i].value() == 0);
        CHECK(change_host.calls == kSndxCategoryVarCount);  // every mute var changed 0 -> 1
        // Second run: no change -> 0 callbacks more.
        std::size_t before = change_host.calls;
        r = run_sndx_handler(SndxCommandId::category_mute_all, engine.base(), one, mute_span,
                             filter_span, hooks);
        CHECK(r.code == kSndxCmdResultDone && r.vars_applied == kSndxCategoryVarCount);
        CHECK(change_host.calls == before);
        // Filter: applies to the filter vector only.
        arg_host.result = false;
        r = run_sndx_handler(SndxCommandId::category_filter_all, engine.base(), one, mute_span,
                             filter_span, hooks);
        CHECK(r.code == kSndxCmdResultDone && r.vars_applied == kSndxCategoryVarCount);
        for (std::size_t i = 0; i < kSndxCategoryVarCount; ++i) CHECK(filter.vars[i].value() == 0);
        CHECK(mute.vars[0].value() == 1);  // untouched
    }
    // Missing get_bool_arg hook: native span lookup (E1037) still runs mute/filter.
    {
        ConfigBoolEntry16 record{};
        record.value = "false";
        const std::uint8_t* base = reinterpret_cast<const std::uint8_t*>(&record);
        SndxArgsSpan native_one{base, base + kConfigBoolEntrySize};
        SndxHandlerHooks partial = hooks;
        partial.get_bool_arg = SndxGetBoolArgHooks{};
        const std::size_t calls_before = arg_host.calls;
        SndxHandlerRun r = run_sndx_handler(SndxCommandId::category_mute_all, engine.base(),
                                            native_one, mute_span, filter_span, partial);
        CHECK(r.code == kSndxCmdResultDone && r.stage == SndxHandlerStage::ok);
        CHECK(arg_host.calls == calls_before);
        for (std::size_t i = 0; i < kSndxCategoryVarCount; ++i) CHECK(mute.vars[i].value() == 0);
    }
    // Misaligned vector: defensive refusal.
    {
        SndxCategoryVectorSpan bad{mute.vars, reinterpret_cast<char*>(mute.vars) + 5};
        SndxHandlerRun r = run_sndx_handler(SndxCommandId::category_mute_all, engine.base(), one, bad,
                                            filter_span, hooks);
        CHECK(r.code == kSndxCmdResultRejected && r.stage == SndxHandlerStage::vector_invalid);
        SndxCategoryVectorSpan reversed{mute.vars + 1, mute.vars};
        r = run_sndx_handler(SndxCommandId::category_mute_all, engine.base(), one, reversed,
                             filter_span, hooks);
        CHECK(r.code == kSndxCmdResultRejected && r.stage == SndxHandlerStage::vector_invalid);
    }
}

}  // namespace

int main() {
    test_tag_resolution();
    test_bool_var_set_0xba40();
    test_engine_reload();
    test_handlers();
    std::printf("OK sndx_command_handlers\n");
    return 0;
}
