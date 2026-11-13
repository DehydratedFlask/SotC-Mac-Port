// sotc_init_window - native arm64 executable that runs the recovered startup Init window and
// reports the observed checkpoint as one JSON object. This is the runtime entry; the fixture
// tests remain separate. The 275-entry input-table fixture is private user data supplied with
// --fixture or SOTC_INPUTTABLE_NAMES_FIXTURE.
#include <cstdio>
#include <cstring>
#include <string>

#include "core/guest_entry_table.hpp"
#include "core/guest_thunks.hpp"
#include "core/init_window_runtime.hpp"

namespace {

void usage(const char* program) {
    std::fprintf(stderr,
                 "usage: %s [--fixture <path>]\n"
                 "  Runs 0x531d60..0x532b0c through the native Init slice, then requires the\n"
                 "  0x532c1c registry checkpoint (275 entries).\n",
                 program);
}

void print_json_string(const char* text) {
    std::putchar('"');
    for (const unsigned char* p = reinterpret_cast<const unsigned char*>(text); *p != 0; ++p) {
        switch (*p) {
            case '"': std::fputs("\\\"", stdout); break;
            case '\\': std::fputs("\\\\", stdout); break;
            case '\n': std::fputs("\\n", stdout); break;
            case '\r': std::fputs("\\r", stdout); break;
            case '\t': std::fputs("\\t", stdout); break;
            default:
                if (*p < 0x20) {
                    std::printf("\\u%04x", static_cast<unsigned>(*p));
                } else {
                    std::putchar(*p);
                }
                break;
        }
    }
    std::putchar('"');
}

void print_entry_map() {
    const sotc::GuestEntryCounts& c = sotc::guest_entry_counts();
    const sotc::GuestIndirectSummary& indirect = sotc::guest_entry_indirect();
    std::printf("\"entry_map\":{\"entries\":%u,\"function_start\":%u,"
                "\"function_interior\":%u,\"plt\":%u,\"executable_data\":%u,"
                "\"image_data\":%u,\"outside_image\":%u,\"dispatchable\":%u,"
                "\"retail_entry\":%u,\"jmp_thunk\":%u,\"ret_stub\":%u,"
                "\"not_callable\":%u,\"indirect\":{\"functions\":%u,\"sites\":%u,"
                "\"resolved\":%u,\"blocked\":%u},\"lookups\":{",
                c.entries, c.function_start, c.function_interior, c.plt, c.executable_data,
                c.image_data, c.outside_image, c.function_start, c.retail_entry, c.jmp_thunk,
                c.ret_stub, c.not_callable, indirect.functions, indirect.sites, indirect.resolved,
                indirect.blocked);
    const std::uint64_t samples[] = {0x531d60, 0x5b9f90, 0x1353, 0xf8f390, 0x1014a80, 0x1594d90,
                                     0x12345678};
    for (std::size_t i = 0; i < sizeof(samples) / sizeof(samples[0]); ++i) {
        const sotc::GuestEntry* entry = sotc::guest_entry_find(samples[i]);
        std::printf("%s\"0x%llx\":\"%s\"", i == 0 ? "" : ",",
                    static_cast<unsigned long long>(samples[i]),
                    entry ? sotc::guest_entry_kind_name(entry->kind) : "unknown");
    }
    std::fputs("}},", stdout);
}

struct ThunkProbe {
    bool ok;
    int calls;
    unsigned setter_value;
    unsigned register_flag;
    int hook_calls;
    bool dispatch_ok;
    bool previous_targets_ok;
    int previous_targets_called;
    unsigned byte_setter_value;
    unsigned long long dispatch_site;
    unsigned long long dispatch_target;
    unsigned dispatch_flag;
    int dispatch_hook_calls;
};

int g_thunk_hook_calls = 0;

void thunk_register_hook(void*, void*) {
    ++g_thunk_hook_calls;
}

ThunkProbe run_guest_thunk_probes() {
    ThunkProbe p{};
    p.calls = 0;
    alignas(8) unsigned char setter[0x40] = {};
    const std::uint64_t setter_args[6] = {reinterpret_cast<std::uint64_t>(setter), 0x2a, 0, 0, 0, 0};
    std::uint64_t result = 0;
    const bool setter_called = sotc::guest_thunk_invoke(0x13d20, setter_args, &result);
    std::memcpy(&p.setter_value, setter + 0x1c, sizeof(p.setter_value));
    if (setter_called) ++p.calls;

    alignas(8) unsigned char command[0x40] = {};
    const std::uint64_t command_args[6] = {reinterpret_cast<std::uint64_t>(command), 0, 0, 0, 0, 0};
    g_thunk_hook_calls = 0;
    sotc::guest_set_concmd_register_hook(thunk_register_hook);
    const bool first_called = sotc::guest_thunk_invoke(0x20d60, command_args, &result);
    if (first_called) ++p.calls;
    const bool second_called = sotc::guest_thunk_invoke(0x20d60, command_args, &result);
    if (second_called) ++p.calls;
    sotc::guest_set_concmd_register_hook(nullptr);
    p.register_flag = command[0x18];
    p.hook_calls = g_thunk_hook_calls;

    // CFG-resolved site 0x28471: read slot 0x20 of the host vtable image, invoke 0x20d60.
    const sotc::GuestDispatchSite& site = sotc::guest_dispatch_site();
    std::size_t vtable_size = 0;
    const std::uint8_t* vtable = sotc::guest_dispatch_vtable(&vtable_size);
    alignas(8) unsigned char dispatch_object[0x40] = {};
    const std::uint64_t dispatch_args[6] = {
        reinterpret_cast<std::uint64_t>(dispatch_object), 0, 0, 0, 0, 0};
    std::uint64_t dispatch_target = 0;
    g_thunk_hook_calls = 0;
    sotc::guest_set_concmd_register_hook(thunk_register_hook);
    const bool dispatched = sotc::guest_dispatch_slot(
        vtable, vtable_size, static_cast<std::size_t>(site.slot_offset), dispatch_args,
        &result, &dispatch_target);
    sotc::guest_set_concmd_register_hook(nullptr);
    p.dispatch_ok = dispatched && dispatch_target == 0x20d60 && dispatch_object[0x18] == 1 &&
                    g_thunk_hook_calls == 1 && site.site_rva == 0x28471 &&
                    site.target_rva == 0x20d60 && site.slot_offset == 0x20;
    p.dispatch_site = site.site_rva;
    p.dispatch_target = dispatch_target;
    p.dispatch_flag = dispatch_object[0x18];
    p.dispatch_hook_calls = g_thunk_hook_calls;

    // The 15 targets the previous pass resolved: the 14 one-byte stubs must leave a canary object
    // unchanged; 0x20d60 is asserted by the dispatch block above.
    std::size_t target_count = 0;
    const std::uint64_t* targets = sotc::guest_resolved_targets(&target_count);
    bool previous_ok = targets != nullptr && target_count == 15;
    for (std::size_t i = 0; previous_ok && i < target_count; ++i) {
        if (targets[i] == 0x20d60) continue;
        alignas(8) unsigned char canary[0x40];
        std::memset(canary, 0xa5, sizeof(canary));
        const std::uint64_t canary_args[6] = {
            reinterpret_cast<std::uint64_t>(canary), 0, 0, 0, 0, 0};
        std::uint64_t canary_result = 1;
        previous_ok = sotc::guest_thunk_invoke(targets[i], canary_args, &canary_result);
        for (unsigned char byte : canary) previous_ok = previous_ok && byte == 0xa5;
        if (previous_ok) ++p.previous_targets_called;
    }
    p.previous_targets_ok = previous_ok && p.previous_targets_called == 14;

    // 0xa431e0 is proven only by a direct caller: `movb %sil,0x51(%rdi)`. It must write 0xff at
    // +0x51 and leave the neighboring bytes untouched.
    alignas(8) unsigned char byte_canary[0x60];
    std::memset(byte_canary, 0x5a, sizeof(byte_canary));
    const std::uint64_t byte_args[2] = {
        reinterpret_cast<std::uint64_t>(byte_canary), 0x1ff};
    std::uint64_t byte_result = 1;
    const bool byte_ok = sotc::guest_thunk_invoke(0xa431e0, byte_args, &byte_result) &&
                         byte_result == 0 && byte_canary[0x51] == 0xff &&
                         byte_canary[0x50] == 0x5a && byte_canary[0x52] == 0x5a;
    p.byte_setter_value = byte_canary[0x51];
    p.previous_targets_ok = p.previous_targets_ok && byte_ok;

    p.ok = setter_called && p.setter_value == 0x2a && first_called && second_called &&
           p.register_flag == 1 && p.hook_calls == 1 && p.dispatch_ok &&
           p.previous_targets_ok && byte_ok && sotc::guest_thunk_count() == 91;
    return p;
}

void print_guest_thunks(const ThunkProbe& p) {
    std::printf("\"guest_thunks\":{\"registered\":%zu,\"calls\":%d,\"setter_value\":%u,"
                "\"register_flag\":%u,\"hook_calls\":%d,\"ok\":%s,"
                "\"dispatch\":{\"site\":\"0x%llx\",\"target\":\"0x%llx\","
                "\"flag\":%u,\"hook_calls\":%d,\"ok\":%s},"
                "\"previous_targets\":{\"total\":15,\"called\":%d,\"ok\":%s},"
                "\"byte_setter\":%u},",
                sotc::guest_thunk_count(), p.calls, p.setter_value, p.register_flag, p.hook_calls,
                p.ok ? "true" : "false", p.dispatch_site, p.dispatch_target, p.dispatch_flag,
                p.dispatch_hook_calls, p.dispatch_ok ? "true" : "false",
                p.previous_targets_called, p.previous_targets_ok ? "true" : "false",
                p.byte_setter_value);
}

void print_post_calls(const sotc::InitWindowRuntimeResult& r) {
    std::printf("\"post_5b9f90\":{\"native\":%s,\"slices\":%d,\"groups\":%d,\"commands\":%d,"
                "\"toggles\":%d,\"floats\":%d,\"named_vars\":%d,\"registered\":%d,"
                "\"releases\":%d,\"destroys\":%d,\"separators\":%d,\"sliders\":%d,"
                "\"int_sliders\":%d,\"s4\":%s,\"s4_items\":%d,\"s4_bytes\":%d,"
                "\"s5\":%s,\"s5_items\":%d,\"s5_bytes\":%d,"
                "\"contracts\":%d,\"resume\":\"0x%llx\",\"complete\":%s},",
                r.post_5b9f90_ok ? "true" : "false", r.post_5b9f90_slices,
                r.post_5b9f90_groups, r.post_5b9f90_commands, r.post_5b9f90_toggles,
                r.post_5b9f90_floats, r.post_5b9f90_named_vars, r.post_5b9f90_registered,
                r.post_5b9f90_releases, r.post_5b9f90_destroys, r.post_5b9f90_separators,
                r.post_5b9f90_sliders, r.post_5b9f90_int_sliders,
                r.post_5b9f90_s4 ? "true" : "false", r.post_5b9f90_s4_items,
                r.post_5b9f90_s4_bytes, r.post_5b9f90_s5 ? "true" : "false",
                r.post_5b9f90_s5_items, r.post_5b9f90_s5_bytes, r.post_5b9f90_contracts,
                static_cast<unsigned long long>(r.post_5b9f90_resume_rva),
                r.post_5b9f90_complete ? "true" : "false");
    std::printf("\"post_call_count\":%d,\"post_calls_executed\":%d,"
                "\"post_calls_blocked\":%d,\"post_calls_complete\":%s,\"post_calls\":[",
                r.post_call_count, r.post_calls_executed, r.post_calls_blocked,
                r.post_calls_blocked == 0 ? "true" : "false");
    for (int i = 0; i < r.post_call_count; ++i) {
        const sotc::InitPostCallRecord& c = r.post_calls[i];
        if (i != 0) std::putchar(',');
        std::printf("{\"callee\":\"0x%llx\",\"caller\":\"%s\",\"state\":\"%s\",",
                    static_cast<unsigned long long>(c.callee_rva), c.caller,
                    c.executed ? "executed_native" : "blocked");
        std::printf("\"args\":[");
        for (std::uint8_t a = 0; a < c.arg_count; ++a) {
            if (a != 0) std::putchar(',');
            std::printf("\"0x%llx\"", static_cast<unsigned long long>(c.args[a]));
        }
        std::printf("],\"return\":");
        if (c.has_return) {
            std::printf("\"0x%llx\"", static_cast<unsigned long long>(c.return_value));
        } else {
            std::fputs(c.executed ? "\"void\"" : "\"none\"", stdout);
        }
        std::fputs(",\"blocker\":", stdout);
        if (c.blocker != nullptr) {
            print_json_string(c.blocker);
        } else {
            std::fputs("null", stdout);
        }
        std::fputs(",\"detail\":", stdout);
        print_json_string(c.detail);
        std::putchar('}');
    }
    std::putchar(']');
}

}  // namespace

int main(int argc, char** argv) {
    const char* fixture = nullptr;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--fixture") == 0) {
            if (i + 1 >= argc) {
                usage(argv[0]);
                return 2;
            }
            fixture = argv[++i];
        } else if (std::strcmp(argv[i], "--help") == 0 || std::strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return 0;
        } else {
            usage(argv[0]);
            return 2;
        }
    }

    const sotc::InitWindowRuntimeResult r = sotc::run_init_window_runtime(fixture);
    const ThunkProbe thunks = run_guest_thunk_probes();
    std::printf(
        "{\"schema\":\"sotc.init_window.v5\",\"ok\":%s,\"entry_rva\":\"0x531d60\","
        "\"window_end_rva\":\"0x532b0c\",\"checkpoint_rva\":\"0x532c1c\","
        "\"entry_units\":%d,\"entry_base_calls\":%d,\"early\":%s,\"tail\":%s,\"ctor\":%s,"
        "\"base\":%s,\"post_base\":%s,\"continue_e\":%s,\"registry_tail\":%s,"
        "\"registry_body\":%s,\"registry_body_units\":%d,\"registry_entries\":%u,"
        "\"registry_status\":%u,\"remainder\":%s,\"boss_flag_after\":%u,"
        "\"post_chain_complete\":%s,\"post_chain_blocked\":\"%s\","
        "\"post_1290\":{\"native\":%s,\"a\":%s,\"b\":%s,\"a_count\":%d,\"b_count\":%d,"
        "\"vcall\":%d},"
        "\"post_56dcb0\":{\"native\":%s,\"calls\":%d,\"blocked\":%d,\"a5cd90\":%d,\"946880\":%d,"
        "\"a5d990\":%d,\"560b60\":%d,\"desc0\":\"0x%llx\",\"desc1\":\"0x%llx\"},"
        "\"post_946880\":{\"native\":%s,\"status\":%d,\"took_9469f0\":%s,"
        "\"inner_ok\":%s,\"inner_calls\":%d,\"lock\":%u},"
        "\"post_5d5410\":{\"native\":%s,\"8be60\":%d,\"8f520\":%d,\"8f1d0\":%d,"
        "\"blocked\":%d,\"reason\":\"%s\",\"owner\":\"0x%llx\","
        "\"obj_a_04\":\"0x%x\",\"obj_a_0c\":\"0x%x\",\"obj_b_04\":\"0x%x\","
        "\"obj_b_0c\":\"0x%x\",\"state\":[\"0x%llx\",\"0x%llx\",\"0x%llx\","
        "\"0x%llx\",\"0x%llx\",\"0x%llx\",\"0x%llx\",\"0x%llx\",\"0x%llx\"]},"
        "\"post_56dca0\":{\"native\":%s,\"bytes\":1,\"calls\":%d},"
        "\"post_5c64c0\":{\"native\":%s,\"blocked\":%d,\"bss_a\":\"0x%08x\",\"bss_b\":\"0x%08x\","
        "\"bss_c\":\"0x%08x\",\"cell\":\"0x%llx\",\"calls\":%d,\"tail\":%s},"
        "\"post_96f20\":{\"native\":%s,\"value\":\"0x%llx\",\"alloc_bytes\":%llu,\"callee\":\"0x97a50\",\"callee_host\":true},"
        "\"post_560b60\":{\"native\":%s,\"calls\":%d,\"allocs\":%d,\"object\":\"0x%llx\",\"b0\":\"0x%llx\",\"b1\":\"0x%llx\",\"b2\":\"0x%llx\",\"prev_cleanup\":%s,\"global28\":%u},"
        "\"post_a5cd90\":{\"native\":%s,\"calls\":%d,\"allocs\":%d,\"field69c8\":%u,\"field08\":%u,\"resource\":\"0x%llx\",\"ready\":%s,\"flag18\":%u,\"flag20\":%u,\"a4f8\":\"0x%llx\",\"byte0\":%u,\"once\":%d,\"user_id\":%d,\"log\":%d,\"trap\":%d},"
        "\"post_7f7220\":{\"native\":%s,\"blocked\":%d,\"branch\":%s,\"sets\":%d,\"mode0\":%d,"
        "\"str0\":\"0x%llx\",\"mode1\":%d,\"str1\":\"0x%llx\"},"
        "\"post_7f6f90\":{\"native\":%s,\"blocked\":%d,\"entries0\":%d,\"index0\":%d,\"rest0\":%d,\"bytes0\":%d,\"fopen0\":%s,\"entries1\":%d,\"index1\":%d,\"rest1\":%d,\"bytes1\":%d,\"fopen1\":%s},"
        "\"post_56db70\":{\"native\":%s,\"bytes\":1,\"calls\":%d},"
        "\"post_5b3d00\":{\"native\":%s,\"bytes\":1,\"calls\":%d},"
        "\"post_5b3cf0\":{\"native\":%s,\"bytes\":1,\"calls\":%d},"
        "\"post_56fad0\":{\"native\":%s,\"bytes\":1,\"calls\":%d},"
        "\"post_f90310\":{\"host_service\":%s,\"calls\":%d,\"provider\":\"splash_hide\"},"
        "\"post_base_sites\":[\"0x56dcb0\",\"0x56dca0\",\"0x5c64c0\",\"0x1290\",\"0x56db70\",\"0x5b3d00\",\"0x5b3cf0\",\"0x7f7220\",\"0x56fad0\",\"0xf90310\"],"
        "\"executed_bodies\":[\"0x499f80\",\"0xa473b0\",\"0xa54920\",\"0x54b250\"],"
        "\"executed_body_calls\":{\"0x499f80\":%d,\"0xa473b0\":%d,\"0xa54920\":%d,\"0x54b250\":%d},"
        "\"executed_body_ok\":{\"0x499f80\":%s,\"0xa473b0\":%s,\"0xa54920\":%s,\"0x54b250\":%s},"
        "\"body_499f80_committed\":%s,\"body_499f80_object_live\":%s,"
        "\"body_499f80_buffer_a\":%s,\"body_499f80_buffer_b\":%s,"
        "\"body_499f80_cleanup_failures\":%d,\"body_499f80_stage\":\"%s\","
        "\"capture_step\":%d,\"capture_video_0d\":%u,\"capture_video_a007\":%u,"
        "\"savedata_refusal\":%d,\"savedata_counters\":%d,\"savedata_items\":%d,"
        "\"stat_engine_calls\":%d,\"stat_engine_arg2\":%u,"
        "\"stub_hooks\":[],\"stub_calls\":{},"
        "\"contract_boundaries\":\"0x499f80, 0x56dcb0, 0x5c64c0, 0x1290, 0x7f7220 and the nested 0x5d5410 chain execute native bodies at their Init sites; 0x5d5410's 0x8be60/0x8f520/0x8f1d0 callees and their 0x865xxx/0x87cxxx setters are native; the 0x8be60 0x12820 allocator and 0x87f3e0->0xf906d0 platform tail are explicit host contracts, and the 0x8f628/0x8f276 stack-check traps are pinned but never called; 0x56db70/0x5b3d00/0x5b3cf0/0x56fad0 execute their one-byte-ret native equivalents; 0x56dcb0's four helpers, 0x5c64c0's eight helpers, 0x7f7220's query hook, 0x7f6f90's four-call fallback, 0x499f80's 0x12820/0x12af0 allocator dispatch, 0x54b250's engine 0xa56560 and the capture platform services are explicit host contracts; 0xf90310 is a PLT/Sony splash-hide host service, not a lifted game body; 0x1290's singleton owners are host-owned while its list/vector inserts are native\","
        "\"first_failed_stage\":\"%s\",",
        r.ok ? "true" : "false", r.entry_units, r.entry_base_calls,
        r.early_ok ? "true" : "false", r.tail_ok ? "true" : "false",
        r.ctor_ok ? "true" : "false", r.base_ok ? "true" : "false",
        r.post_base_ok ? "true" : "false", r.continue_e_ok ? "true" : "false",
        r.registry_tail_ok ? "true" : "false", r.registry_body_ok ? "true" : "false",
        r.registry_body_units, r.registry_entries, r.registry_status,
        r.remainder_ok ? "true" : "false", r.boss_flag_after,
        r.post_chain_complete ? "true" : "false", r.post_chain_blocked,
        r.post_1290_ok ? "true" : "false", r.post_1290_a ? "true" : "false",
        r.post_1290_b ? "true" : "false", r.post_1290_a_count, r.post_1290_b_count,
        r.post_1290_vcall,
        r.post_56dcb0_ok ? "true" : "false", r.post_56dcb0_calls, r.post_56dcb0_blocked,
        r.post_56dcb0_a5cd90, r.post_56dcb0_946880, r.post_56dcb0_a5d990,
        r.post_56dcb0_560b60, static_cast<unsigned long long>(r.post_56dcb0_desc0),
        static_cast<unsigned long long>(r.post_56dcb0_desc1),
        r.post_946880_ok ? "true" : "false", r.post_946880_status,
        r.post_946880_took_9469f0 ? "true" : "false",
        r.post_946880_inner_ok ? "true" : "false", r.post_946880_inner_calls,
        r.post_946880_lock,
        r.post_5d5410_ok ? "true" : "false", r.post_5d5410_8be60, r.post_5d5410_8f520,
        r.post_5d5410_8f1d0, r.post_5d5410_blocked, r.post_5d5410_blocked_reason,
        static_cast<unsigned long long>(r.post_5d5410_owner), r.post_5d5410_obj_a_04,
        r.post_5d5410_obj_a_0c, r.post_5d5410_obj_b_04, r.post_5d5410_obj_b_0c,
        static_cast<unsigned long long>(r.post_5d5410_state[0]),
        static_cast<unsigned long long>(r.post_5d5410_state[1]),
        static_cast<unsigned long long>(r.post_5d5410_state[2]),
        static_cast<unsigned long long>(r.post_5d5410_state[3]),
        static_cast<unsigned long long>(r.post_5d5410_state[4]),
        static_cast<unsigned long long>(r.post_5d5410_state[5]),
        static_cast<unsigned long long>(r.post_5d5410_state[6]),
        static_cast<unsigned long long>(r.post_5d5410_state[7]),
        static_cast<unsigned long long>(r.post_5d5410_state[8]),
        r.entry_ret_nop_calls == 1 ? "true" : "false", r.entry_ret_nop_calls,
        r.post_5c64c0_ok ? "true" : "false", r.post_5c64c0_blocked, r.post_5c64c0_bss_a_hash,
        r.post_5c64c0_bss_b_hash, r.post_5c64c0_bss_c_hash,
        static_cast<unsigned long long>(r.post_5c64c0_cell), r.post_5c64c0_calls,
        r.post_5c64c0_tail ? "true" : "false",
        r.post_96f20_ok ? "true" : "false",
        static_cast<unsigned long long>(r.post_96f20_value),
        static_cast<unsigned long long>(r.post_96f20_alloc_bytes),
        r.post_560b60_ok ? "true" : "false", r.post_560b60_calls,
        r.post_560b60_alloc_calls,
        static_cast<unsigned long long>(r.post_560b60_object),
        static_cast<unsigned long long>(r.post_560b60_buffer0),
        static_cast<unsigned long long>(r.post_560b60_buffer1),
        static_cast<unsigned long long>(r.post_560b60_buffer2),
        r.post_560b60_previous_cleanup ? "true" : "false", r.post_560b60_global28,
        r.post_a5cd90_ok ? "true" : "false", r.post_a5cd90_calls,
        r.post_a5cd90_allocs, r.post_a5cd90_field69c8, r.post_a5cd90_field08,
        static_cast<unsigned long long>(r.post_a5cd90_resource),
        r.post_a5cd90_ready ? "true" : "false", r.post_a5cd90_flag18,
        r.post_a5cd90_flag20,
        static_cast<unsigned long long>(r.post_a5cd90_object_a4f8),
        r.post_a5cd90_byte0, r.post_a5cd90_once, r.post_a5cd90_user_id,
        r.post_a5cd90_log, r.post_a5cd90_trap,
        r.post_7f7220_ok ? "true" : "false", r.post_7f7220_blocked,
        r.post_7f7220_branch ? "true" : "false",
        r.post_7f7220_sets, r.post_7f7220_mode0,
        static_cast<unsigned long long>(r.post_7f7220_str0), r.post_7f7220_mode1,
        static_cast<unsigned long long>(r.post_7f7220_str1),
        r.post_7f6f90_ok ? "true" : "false", r.post_7f6f90_blocked,
        r.post_7f6f90_entries[0], r.post_7f6f90_index0[0], r.post_7f6f90_rest0[0],
        static_cast<int>(r.post_7f6f90_bytes[0]),
        r.post_7f6f90_fopen[0] ? "true" : "false",
        r.post_7f6f90_entries[1], r.post_7f6f90_index0[1], r.post_7f6f90_rest0[1],
        static_cast<int>(r.post_7f6f90_bytes[1]),
        r.post_7f6f90_fopen[1] ? "true" : "false",
        r.post_56db70_ok ? "true" : "false", r.post_56db70_calls,
        r.post_5b3d00_ok ? "true" : "false", r.post_5b3d00_calls,
        r.post_5b3cf0_ok ? "true" : "false", r.post_5b3cf0_calls,
        r.post_56fad0_ok ? "true" : "false", r.post_56fad0_calls,
        r.post_f90310_ok ? "true" : "false", r.post_f90310_calls,
        r.body_499f80_calls, r.capture_body_calls, r.savedata_body_calls,
        r.stat_query_body_calls,
        r.body_499f80_ok ? "true" : "false", r.capture_body_ok ? "true" : "false",
        r.savedata_body_ok ? "true" : "false", r.stat_query_body_ok ? "true" : "false",
        r.body_499f80_committed ? "true" : "false",
        r.body_499f80_object_live ? "true" : "false",
        r.body_499f80_buffer_a ? "true" : "false",
        r.body_499f80_buffer_b ? "true" : "false",
        r.body_499f80_cleanup_failures, r.body_499f80_stage,
        r.capture_step, r.capture_video_0d, r.capture_video_a007,
        r.savedata_refusal, r.savedata_counters, r.savedata_items,
        r.stat_engine_calls, r.stat_engine_arg2,
        r.ok ? "" : (r.first_failed_stage[0] ? r.first_failed_stage : "unknown"));
    print_entry_map();
    print_guest_thunks(thunks);
    print_post_calls(r);
    std::fputs("}\n", stdout);
    return (r.ok && thunks.ok) ? 0 : 3;
}
