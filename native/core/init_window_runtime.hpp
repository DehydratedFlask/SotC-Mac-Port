// Native runtime entry for the lifted startup Init window `0x531d60..0x532b0c`.
//
// This is deliberately a narrow driver: it starts at the recovered entry frame, runs the measured
// full Init slice (prologue 0x255180, lifted 0x3be050/0x1ab550, base 0x255130, the recovered
// post-base call sites, continue C/D/E) and then the native registry body/remainder. It reports the
// `0x532c1c` checkpoint only when the registry actually holds all 275 table entries.
//
// The post-base call chain now executes native bodies at every named site: 0x499f80 (193 B,
// allocator/CritSec contracts), 0x56dcb0 (132 B, four helper hooks), 0x5c64c0 (291 B, global
// buffers plus eight hooks), 0x1290 (195 B, with real list/vector container inserts), 0x7f7220
// (54 B, staffroll text selection), and the five one-byte `ret` targets including 0x56dca0.
// Each site reports its own observed state or a named blocked stage. The later engines also execute native models: 0xa473b0, 0xa54920 and 0x54b250. The one
// remaining engine boundary inside 0x54b250 is its un-lifted 0xa56560 host contract; no silent
// `return true` is hidden in the lift.
#pragma once

#include <cstdint>

#include "init_post_call_trace.hpp"

namespace sotc {

inline constexpr std::uint64_t kInitWindowEntryRva = 0x531d60;
inline constexpr std::uint64_t kInitWindowEndRva = 0x532b0c;
inline constexpr std::uint64_t kInitWindowCheckpointRva = 0x532c1c;

struct InitWindowRuntimeResult {
    bool ok = false;
    bool entry_ok = false;
    bool fixture_loaded = false;
    bool early_ok = false;
    bool tail_ok = false;
    bool ctor_ok = false;
    bool base_ok = false;
    bool post_base_ok = false;
    bool continue_e_ok = false;
    bool registry_tail_ok = false;
    bool registry_body_ok = false;
    bool remainder_ok = false;
    int entry_units = 0;
    int entry_base_calls = 0;
    int entry_ret_nop_calls = 0;
    int registry_body_units = 0;
    std::uint32_t registry_entries = 0;
    std::uint32_t registry_status = 0;
    std::uint32_t boss_flag_after = 0;
    int body_499f80_calls = 0;
    bool body_499f80_ok = false;
    bool body_499f80_committed = false;
    bool body_499f80_object_live = false;
    bool body_499f80_buffer_a = false;
    bool body_499f80_buffer_b = false;
    int body_499f80_cleanup_failures = 0;
    const char* body_499f80_stage = "";
    int capture_body_calls = 0;
    bool capture_body_ok = false;
    int capture_step = 0;
    std::uint32_t capture_video_0d = 0;
    std::uint32_t capture_video_a007 = 0;
    int savedata_body_calls = 0;
    bool savedata_body_ok = false;
    int savedata_refusal = 0;
    int savedata_counters = 0;
    int savedata_items = 0;
    int stat_query_body_calls = 0;
    bool stat_query_body_ok = false;
    int stat_engine_calls = 0;
    std::uint32_t stat_engine_arg2 = 0;
    bool post_1290_ok = false;
    bool post_1290_a = false;
    bool post_1290_b = false;
    int post_1290_a_count = 0;
    int post_1290_b_count = 0;
    int post_1290_vcall = 0;
    bool post_56dcb0_ok = false;
    int post_56dcb0_calls = 0;
    int post_56dcb0_blocked = 0;
    int post_56dcb0_a5cd90 = 0;
    int post_56dcb0_946880 = 0;
    int post_56dcb0_a5d990 = 0;
    int post_56dcb0_560b60 = 0;
    std::uint64_t post_56dcb0_desc0 = 0;
    std::uint64_t post_56dcb0_desc1 = 0;
    bool post_946880_ok = false;
    std::int32_t post_946880_status = -3;
    bool post_946880_took_9469f0 = false;
    bool post_946880_inner_ok = false;
    int post_946880_inner_calls = 0;
    std::uint32_t post_946880_lock = 0;
    bool post_5d5410_ok = false;
    int post_5d5410_8be60 = 0;
    int post_5d5410_8f520 = 0;
    int post_5d5410_8f1d0 = 0;
    int post_5d5410_blocked = 0;
    const char* post_5d5410_blocked_reason = "";
    std::uint64_t post_5d5410_owner = 0;
    std::uint32_t post_5d5410_obj_a_04 = 0;
    std::uint32_t post_5d5410_obj_a_0c = 0;
    std::uint32_t post_5d5410_obj_b_04 = 0;
    std::uint32_t post_5d5410_obj_b_0c = 0;
    std::uint64_t post_5d5410_state[9] = {};
    bool post_96f20_ok = false;
    std::uint64_t post_96f20_value = 0;
    std::uint64_t post_96f20_alloc_bytes = 0;
    bool post_560b60_ok = false;
    int post_560b60_calls = 0;
    int post_560b60_alloc_calls = 0;
    std::uint64_t post_560b60_object = 0;
    std::uint64_t post_560b60_buffer0 = 0;
    std::uint64_t post_560b60_buffer1 = 0;
    std::uint64_t post_560b60_buffer2 = 0;
    bool post_560b60_previous_cleanup = false;
    std::uint32_t post_560b60_global28 = 0;
    bool post_a5cd90_ok = false;
    int post_a5cd90_calls = 0;
    int post_a5cd90_allocs = 0;
    std::uint32_t post_a5cd90_field69c8 = 0;
    std::uint32_t post_a5cd90_field08 = 0;
    std::uint64_t post_a5cd90_resource = 0;
    bool post_a5cd90_ready = false;
    std::uint8_t post_a5cd90_flag18 = 0;
    std::uint8_t post_a5cd90_flag20 = 0;
    std::uint64_t post_a5cd90_object_a4f8 = 0;
    std::uint8_t post_a5cd90_byte0 = 0;
    int post_a5cd90_once = 0;
    int post_a5cd90_user_id = 0;
    int post_a5cd90_log = 0;
    int post_a5cd90_trap = 0;
    bool post_5c64c0_ok = false;
    std::uint32_t post_5c64c0_bss_a_hash = 0;
    std::uint32_t post_5c64c0_bss_b_hash = 0;
    std::uint32_t post_5c64c0_bss_c_hash = 0;
    std::uint64_t post_5c64c0_cell = 0;
    int post_5c64c0_calls = 0;
    int post_5c64c0_blocked = 0;
    bool post_5c64c0_tail = false;
    bool post_7f7220_ok = false;
    bool post_7f6f90_ok = false;
    bool post_7f7220_branch = false;
    int post_7f7220_sets = 0;
    int post_7f7220_blocked = 0;
    int post_7f7220_mode0 = 0;
    std::uint64_t post_7f7220_str0 = 0;
    int post_7f7220_mode1 = 0;
    std::uint64_t post_7f7220_str1 = 0;
    int post_7f6f90_entries[2] = {};
    int post_7f6f90_index0[2] = {-1, -1};
    int post_7f6f90_rest0[2] = {-1, -1};
    std::uint64_t post_7f6f90_bytes[2] = {};
    bool post_7f6f90_fopen[2] = {};
    int post_7f6f90_blocked = 0;
    bool post_56db70_ok = false;
    int post_56db70_calls = 0;
    bool post_5b3d00_ok = false;
    int post_5b3d00_calls = 0;
    bool post_5b3cf0_ok = false;
    int post_5b3cf0_calls = 0;
    bool post_56fad0_ok = false;
    int post_56fad0_calls = 0;
    bool post_f90310_ok = false;
    int post_f90310_calls = 0;
    bool post_chain_complete = false;
    const char* post_chain_blocked = "";
    InitPostCallRecord post_calls[kInitPostCallMax]{};
    int post_call_count = 0;
    int post_calls_executed = 0;
    int post_calls_blocked = 0;
    const char* first_failed_stage = "";
};

// Resolves the private 275-entry input-table name fixture through `fixture_path` when non-null
// (the loader also honours SOTC_INPUTTABLE_NAMES_FIXTURE). The fixture is private user data; this
// function never embeds it.
[[nodiscard]] InitWindowRuntimeResult run_init_window_runtime(const char* fixture_path = nullptr) noexcept;

}  // namespace sotc
