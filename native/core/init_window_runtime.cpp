#include "init_window_runtime.hpp"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "allocator_front.hpp"
#include "appmain_init_ctor_98a90_slice.hpp"
#include "capture_a473b0.hpp"
#include "container_1bdb10.hpp"
#include "critsec_3b310.hpp"
#include "ctor_slot_428_52ec10.hpp"
#include "init_1290.hpp"
#include "init_499f80_body.hpp"
#include "init_a5cd90_body.hpp"
#include "init_5b9f90_slices.hpp"
#include "init_a5d990_body.hpp"
#include "init_946880_body.hpp"
#include "init_5d5410_body.hpp"
#include "init_560b60_body.hpp"
#include "init_7f6f90_body.hpp"
#include "init_96f20_body.hpp"
#include "init_post_bodies.hpp"
#include "insert_1beeb0.hpp"
#include "insert_21cd0.hpp"
#include "init_531d60_entry_gaps.hpp"
#include "init_531d60_trivial_leaves.hpp"
#include "input_table_551140.hpp"
#include "inputtable_names_fixture.hpp"
#include "platform_tag_147e20.hpp"
#include "resource_pool.hpp"
#include "savedata_stats_a54920.hpp"
#include "stat_query_54b250.hpp"

namespace sotc {
namespace {

struct HostCaptureServices final : CaptureServices {
    int calls = 0;
    CaptureServiceStatus game_live_streaming_enable(bool) override {
        ++calls;
        return CaptureServiceStatus::ok;
    }
    CaptureServiceStatus screenshot_enable() override {
        ++calls;
        return CaptureServiceStatus::ok;
    }
    CaptureServiceStatus screenshot_disable() override {
        ++calls;
        return CaptureServiceStatus::ok;
    }
    CaptureServiceStatus video_recording_set_info(std::uint32_t, const void*, std::size_t) override {
        ++calls;
        return CaptureServiceStatus::ok;
    }
};

void* host_alloc(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) {
    return std::malloc(size);
}
bool host_free(void* p, std::uint32_t) {
    std::free(p);
    return true;
}
AllocDispatchContract host_alloc_contract() { return AllocDispatchContract{host_alloc, host_free}; }

struct HostStatsEngine {
    int calls = 0;
    std::uint32_t arg2 = 0;
};
HostStatsEngine* g_stats_engine = nullptr;

// Host state for the lifted 0x499f80 body. The three globals are offset-only stand-ins for
// 0x15ff9a8 / 0x1604988 / 0x2ef9fc8; the allocator dispatch and CritSec hooks are the explicit
// contracts named in init_499f80_body.hpp. The effective argument is self+0x3c70, so the default
// argument cell is never read on this path.
struct HostInit499f80 {
    void* store_cell = nullptr;
    void* old_object_cell = nullptr;
    Init499f80Globals globals{};
    Init499f80BodyHooks hooks{};
    bool configured = false;

    void configure(const AllocDispatchContract& alloc) {
        globals.default_arg = nullptr;
        globals.store_cell = &store_cell;
        globals.old_object_cell = &old_object_cell;
        hooks = default_init_499f80_body_hooks(alloc);
        configured = true;
    }
};

// Host state for the four post-base native bodies. The bodies are compiled native code; every
// un-lifted callee is a named hook and every global cell is a host-owned stand-in reported in the
// runtime result. 0x1290 uses the real 0x1beeb0 list insert and 0x21cd0 vector insert.
struct HostPostBodies {
    Container1beeb0 c1290_a{};
    Container21cd0 c1290_b{};
    std::array<Entry21cd0, 4> b1290_pool{};
    int begin_1290 = 0;
    int end_1290 = 0;
    int vcall_1290 = 0;
    Init1290Hooks hooks_1290{};
    Init1290Result result_1290{};

    std::array<std::uint8_t, 0x40> object_56{};
    std::array<std::uint8_t, 0x20> data_56{};
    Init56dcb0BodyHooks hooks_56{};
    Init56dcb0BodyResult result_56{};

    // 0x560b60 host-owned object and globals. The 0x136f0 object and the three buffers are
    // allocated by the body through the 0x17070/0x17090 contracts; the remaining cells stand
    // in for 0x2f08910..0x2f08960 and the three 0x2efe2d0 callbacks.
    std::array<std::uint8_t, kInit560b60DescriptorBytes> descriptor_560{};
    std::array<std::uint8_t, kInit560b60ObjectBytes> object_560{};
    std::array<std::uint8_t, kInit560b60Buffer0Bytes> buffer0_560{};
    std::array<std::uint8_t, kInit560b60Buffer1Bytes> buffer1_560{};
    std::array<std::uint8_t, kInit560b60Buffer2Bytes> buffer2_560{};
    std::array<std::uint8_t, 16> global10_560{};
    std::uint64_t global20_560 = 0;
    std::uint32_t global28_560 = 0;
    std::array<std::uint8_t, kInit560b60Global30Bytes> global30_560{};
    std::array<std::uint8_t, kInit560b60Global50Bytes> global50_560{};
    void* previous_560 = nullptr;
    std::array<std::uintptr_t, 3> callbacks_560{};
    std::array<std::uint8_t, kInit560b60FormatBytes> format_560{};
    Init560b60Hooks hooks_560{};
    Init560b60Result result_560{};

    // 0xa5cd90 SoundSystemSndx::Init host state. The object, caller parameter block, key,
    // resolver output, resource, task object and the two arenas are host-owned stand-ins; the
    // 16-byte descriptor mirrors the 0x56dcb0 caller construction {0x101e7ac, 0x030a}.
    std::array<std::uint8_t, kInitA5cd90ObjectBytes> object_a5{};
    std::array<std::uint8_t, 16> descriptor_a5{};
    std::array<std::uint8_t, kInitA5cd90KeyBytes> key_a5{};
    std::array<std::uint8_t, 0x18> resolved_a5{};
    std::array<std::uint8_t, 0x28> callback_a5{};
    std::array<std::uint8_t, 0x40> resource_a5{};
    std::array<std::uint8_t, 0x68> param_a5{};
    std::array<std::uint8_t, kInitA5cd90Object70Bytes> task_a5{};
    std::vector<std::uint8_t> arena_a5_0{};
    std::vector<std::uint8_t> arena_a5_1{};
    std::uint64_t registry_a5 = 0;
    std::uint8_t config_flag_a5 = 0;
    std::uint32_t arena_tag_a5 = 0;
    int once_a5_calls = 0;
    int arena_a5_allocs = 0;
    InitA5cd90Hooks hooks_a5{};
    InitA5cd90Result result_a5{};

    // 0xa5d990 host state. The engine object is the same object_a5 above; the two 0xc8-stride
    // vector storages and the 32-entry category-name table are host-owned. The name table stays
    // null because the original's table is runtime data (0x1604bf0) the host does not have, so
    // the per-index toggle path is fixture-covered rather than fabricated here.
    std::array<const char*, kSndxCategoryCount> names_a5d990{};
    std::array<std::uint8_t, kSndxCategoryCount * 0xc8> mute_a5d990{};
    std::array<std::uint8_t, kSndxCategoryCount * 0xc8> filter_a5d990{};
    ConCmdRegistry registry_a5d990{};
    StartupAtexitLog atexit_a5d990{};
    DebugMenuRegistry menu_a5d990{};
    SndxCategoryHooks categories_a5d990{};
    int menu_inserts_a5d990 = 0;
    InitA5d990Hooks hooks_a5d990{};
    InitA5d990Result result_a5d990{};

    // 0x5b9f90 slice state. The body registers into the process-wide root list ([0x2dfd4b8] is null
    // on startup), so this registry keeps the root path. The canary cell stands in for the stack
    // slot the body fills from [0x15fe518]; the two page builders are named host contracts.
    DebugMenuRegistry menu_5b9f90{};
    ConCmdRegistry registry_5b9f90{};
    StartupAtexitLog atexit_5b9f90{};
    std::array<DebugMenuItem, 64> items_5b9f90{};
    std::array<ConCmdBoolVar, 8> vars_5b9f90{};
    std::array<bool, 8> var_values_5b9f90{};
    std::array<bool, 8> var_guards_5b9f90{};
    std::array<bool, 8> toggle_values_5b9f90{};
    std::uint64_t canary_5b9f90 = 0;
    Init5b9f90Hooks hooks_5b9f90{};
    Init5b9f90Result result_5b9f90{};

    // 0x946880/0x9469f0 host state. The service block is a stand-in for 0x32950a8; the
    // record name is host-owned and the six service helpers are explicit contracts.
    std::array<std::uint8_t, 0x1000> service_946{};
    std::array<char, kInit9469f0NameBytes> user_name_946 = {"Player"};
    std::uint32_t lock_946 = 0;
    int record_946_calls = 0;
    int copy_946_calls = 0;
    int log_946_calls = 0;
    Init946880Hooks hooks_946{};
    Init946880Result result_946{};

    // 0x5d5410 chain host state. The two 0x30-byte format objects and the 0x48-byte
    // 0x3001618 record block stand in for their BSS RVAs; the allocator arena is only
    // reachable on the unobserved 0x8be60 allocation branches.
    std::array<std::uint8_t, kInit5d5410ObjectBytes> object_a_5d{};
    std::array<std::uint8_t, kInit5d5410ObjectBytes> object_b_5d{};
    std::array<std::uint8_t, kInit5d5410StateBytes> state_5d{};
    std::uint64_t owner_5d = 0;
    std::array<std::uint8_t, 0x4000> arena_5d{};
    std::size_t arena_5d_used = 0;
    int alloc_5d_calls = 0;
    int tail_5d_calls = 0;
    Init5d5410Hooks hooks_5d{};
    Init5d5410Result result_5d{};

    std::array<std::uint8_t, 0x88> owner_5c{};
    std::array<std::uint8_t, 0x2500> source_5c{};
    std::array<std::uint8_t, kInit5c64c0BssASize> bss_a_5c{};
    std::array<std::uint8_t, kInit5c64c0BssBSize> bss_b_5c{};
    std::array<std::uint8_t, kInit5c64c0BssCSize> bss_c_5c{};
    std::uintptr_t cell_5c = 0;
    std::vector<std::uint8_t> allocation_96;
    Init96f20Result result_96{};
    Init5c64c0BodyHooks hooks_5c{};
    Init5c64c0BodyResult result_5c{};

    // Host parent state for the exact 14-byte query body 0x5d06f0. The body loads
    // [0x2ff59c0], then [that+0x60], then one byte; both cells are reported in the call detail.
    struct QueryGlobal {
        std::array<std::uint8_t, 0x60> prefix{};
        bool* active = nullptr;
    } query_global_5d{};

    // Host-owned 0x2dca510 / 0x2ff2b10 cells for the exact 174-byte 0x5c91e0 body.
    std::array<std::uint8_t, kInit5c91e0SourceBytes> data_src_5c91{};
    std::array<std::uint8_t, kInit5c91e0DestinationBytes> data_dst_5c91{};

    bool staffroll_active = false;
    Staffroll7f7220Hooks hooks_7f{};
    Staffroll7f7220Result result_7f{};
    std::array<std::int32_t, 2> staffroll_counts{};
    std::array<Staffroll7f6f90Entry, 2 * kStaffroll7f6f90MaxEntries> staffroll_entries{};
    std::array<int, 2> staffroll_index0{{-1, -1}};
    std::array<int, 2> staffroll_rest0{{-1, -1}};
    std::array<std::size_t, 2> staffroll_text_bytes{};
    std::array<bool, 2> staffroll_used_fopen{};
    const char* staffroll_blocked = nullptr;

    int ret_56db70_calls = 0;
    int ret_5b3d00_calls = 0;
    int ret_5b3cf0_calls = 0;
    int ret_56fad0_calls = 0;
    int f90310_calls = 0;
};

bool host_stats_query(void*, std::uint32_t, std::uint32_t arg2, void*, std::uint64_t,
                      std::uint64_t) {
    if (g_stats_engine == nullptr) return false;
    ++g_stats_engine->calls;
    g_stats_engine->arg2 = arg2;
    return true;
}

// The three 0xa54920 calls receive the save-name records built by startup slot 428 at
// 0x2efd9a0/0x2efd9f8/0x2efda50. Reusing kSlot428SaveSpecs keeps the real names and flags
// instead of inventing host strings.
struct RuntimeSavedata {
    alignas(16) std::array<std::uint8_t, 0x700> parent{};
    std::array<std::uint8_t, kSavedataBlockSize> block{};
    std::array<std::uint8_t*, 4> map_slots{};
    std::uint32_t cursor = 0;
    std::array<SavedataStatsName, 3> names{};
    SavedataStatsInDescriptor in{};
    std::array<std::unique_ptr<SavedataStatsA54920Result>, 3> results{};
    int calls = 0;
    bool ok = true;
    int first_refusal = 0;

    void init() {
        auto* map = reinterpret_cast<SavedataDequeMap*>(
            parent.data() + kSavedataStatsParentMapOffset);
        map_slots.fill(nullptr);
        map_slots[1] = block.data();
        map->map = map_slots.data();
        map->map_capacity = map_slots.size();
        map->start_cur = block.data();
        map->start_first = block.data();
        map->start_last = block.data() + kSavedataBlockSize;
        map->start_node = &map_slots[1];
        map->finish_cur = block.data();
        map->finish_first = block.data();
        map->finish_last = block.data() + kSavedataBlockSize;
        map->finish_node = &map_slots[1];
        for (std::size_t i = 0; i < names.size(); ++i) {
            names[i].prefix = kSlot428SaveSpecs[i].name;
            names[i].max_count = kSlot428SaveSpecs[i].flag;
        }
    }

    int index_for(const void* name_struct) const {
        const auto rva = reinterpret_cast<std::uintptr_t>(name_struct);
        if (rva == kInit531d60PostBaseContinueEName1Rva) return 0;
        if (rva == kInit531d60PostBaseContinueEName2Rva) return 1;
        if (rva == kInit531d60PostBaseContinueEName3Rva) return 2;
        return -1;
    }
};

RuntimeSavedata* g_savedata = nullptr;

void real_a54920(void* parent, void* name_struct, void* descriptor) {
    if (g_savedata == nullptr) return;
    RuntimeSavedata& rs = *g_savedata;
    ++rs.calls;
    const int index = rs.index_for(name_struct);
    if (index < 0 || parent == nullptr) {
        rs.ok = false;
        if (rs.first_refusal == 0) rs.first_refusal = 0xFF;
        return;
    }
    if (descriptor != nullptr) {
        std::memcpy(&rs.in, descriptor, sizeof rs.in);
    } else {
        rs.in = SavedataStatsInDescriptor{};
    }

    SavedataStatsName& name = rs.names[static_cast<std::size_t>(index)];
    SavedataStatsA54920Args args{};
    args.parent = parent;
    args.sibling_cursor = &rs.cursor;
    args.alloc = host_alloc_contract();
    args.names = {&name, &name, &name};
    args.descriptor = &rs.in;

    auto result = std::make_unique<SavedataStatsA54920Result>(savedata_stats_a54920(args, {}));
    if (!result->ok) {
        rs.ok = false;
        if (rs.first_refusal == 0) rs.first_refusal = static_cast<int>(result->refusal);
    }
    rs.results[static_cast<std::size_t>(index)] = std::move(result);
}

void entry_ret_nop() { ret_nop_56dca0(); }
int hook_1e4c0(void*) { return 0xA6; }
void hook_4c4950(std::uint8_t) {}

void ctor_a30(void* p) {
    std::memset(p, 0, 0xa30);
    *static_cast<std::uint32_t*>(p) = 0xa30a30a3u;
}

class RuntimeHeap final : public IAllocator {
  public:
    explicit RuntimeHeap(PageOwnerMap& owners) : owners_(owners) {}
    ~RuntimeHeap() override {
        for (void* p : live_) {
            owners_.remove(reinterpret_cast<std::uintptr_t>(p));
            std::free(p);
        }
    }
    void* allocate(std::size_t size, std::size_t align, const AllocDesc&) override {
        void* p = nullptr;
        const std::size_t a = align < sizeof(void*) ? sizeof(void*) : align;
        if (posix_memalign(&p, a, size) != 0) return nullptr;
        owners_.add(reinterpret_cast<std::uintptr_t>(p), size, 2);
        live_.push_back(p);
        return p;
    }
    void deallocate(void* p, const AllocDesc&) override {
        for (auto it = live_.begin(); it != live_.end(); ++it) {
            if (*it == p) {
                owners_.remove(reinterpret_cast<std::uintptr_t>(p));
                std::free(p);
                live_.erase(it);
                return;
            }
        }
    }
    bool recover_from_failure() override { return false; }

  private:
    PageOwnerMap& owners_;
    std::vector<void*> live_;
};

struct KeyDeallocator final : SharedStrDeallocatorF590 {
    void deallocate(const void* p, SharedStrDeallocationKindF590) noexcept override {
        std::free(const_cast<void*>(p));
    }
};

KeyDeallocator g_key_dealloc;

void* key_alloc(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) {
    return std::malloc(size);
}
bool key_dealloc(void* p, std::uint32_t) { std::free(p); return true; }
void key_destroy(void* p, int) { std::free(p); }
void key_free_control(void* p) { std::free(p); }
void* pool_alloc(std::size_t size) { return std::malloc(size); }
bool pool_dealloc(void* p) { std::free(p); return true; }
bool pool_state(ResourceWrapper&, std::uint32_t) { return true; }

void install_key_hooks(Vslot40AttachCommonHooks& c) {
    c.key_alloc.alloc.allocate = key_alloc;
    c.key_alloc.alloc.deallocate = key_dealloc;
    c.platform_tag = platform_tag_147e20_default;
    c.release.destroy = key_destroy;
    c.release.free_control = key_free_control;
}

void install_attach(Vslot40AttachCommonHooks& c, ResourcePool& pool) {
    c.pool = &pool;
    c.pool_hooks.allocate = pool_alloc;
    c.pool_hooks.deallocate = pool_dealloc;
    c.pool_hooks.state_write = pool_state;
    install_key_hooks(c);
}

struct Boot {
    InputTable551140* table = nullptr;
    AllocatorFront* alloc = nullptr;
    void* self = nullptr;
    void* setter = nullptr;
    RuntimeSavedata savedata{};
    HostCaptureServices capture{};
    HostStatsEngine engine{};
    HostInit499f80 init49980{};
    HostPostBodies post{};
    StatsEngine54b250 stats_contract{host_stats_query};
    AppmainInitCtor98a90SliceResult full{};
};

thread_local Boot* g_boot = nullptr;

void* host_node_alloc(std::size_t size) { return std::calloc(1, size); }
void host_node_free(void* ptr) { std::free(ptr); }
void hook_1290_begin(void*, const void*) { ++g_boot->post.begin_1290; }
void hook_1290_end(void*) { ++g_boot->post.end_1290; }
void hook_1290_vcall(void*) { ++g_boot->post.vcall_1290; }

InitPostCallOutcome post_executed(const char* detail) noexcept {
    InitPostCallOutcome o{};
    o.executed = true;
    std::snprintf(o.detail, sizeof o.detail, "%s", detail);
    return o;
}

InitPostCallOutcome post_blocked(const char* blocker, const char* detail) noexcept {
    InitPostCallOutcome o{};
    o.executed = false;
    o.blocker = blocker;
    std::snprintf(o.detail, sizeof o.detail, "%s", detail);
    return o;
}

InitPostCallOutcome host_946_query(std::uint32_t service_id, Init98a450Result* out) {
    if (out == nullptr) return post_blocked("query_output_missing", "0x98a450: null result");
    out->field50 = -1;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x98a450 host query: service=0x%x result+0x50=-1 (invalid handle); "
                  "0x9469f0 branch selected",
                  service_id);
    return post_executed(detail);
}

InitPostCallOutcome host_946_lock(std::uint32_t service_id, void** handle) {
    if (g_boot == nullptr || handle == nullptr) {
        return post_blocked("runtime_missing", "0x986f30: no host handle cell");
    }
    *handle = g_boot->post.service_946.data();
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x986f30 host lock: service=0x%x handle=%p",
                  service_id, *handle);
    auto o = post_executed(detail);
    o.has_value = true;
    o.value = 0;
    return o;
}

InitPostCallOutcome host_946_unlock(void* handle) {
    if (g_boot == nullptr) return post_blocked("runtime_missing", "0x987720: no runtime state");
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x987720 host unlock: handle=%p", handle);
    auto o = post_executed(detail);
    o.has_value = true;
    o.value = 0;
    return o;
}

InitPostCallOutcome host_946_get_name(void* handle, const void* arg2) {
    if (g_boot == nullptr) return post_blocked("runtime_missing", "0x9882e0: no runtime state");
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x9882e0 host name query: handle=%p arg2=%p", handle,
                  arg2);
    auto o = post_executed(detail);
    o.has_value = true;
    o.value = 0;
    return o;
}

struct Host946Record {
    std::uint32_t index;
    std::uint32_t pad;
    const char* name;
};
static_assert(sizeof(Host946Record) == kInit9469f0RecordBytes);

InitPostCallOutcome host_946_get_record(void* handle, std::int32_t id, void* buffer,
                                       std::size_t* size) {
    if (g_boot == nullptr) return post_blocked("runtime_missing", "0x988400: no runtime state");
    auto& p = g_boot->post;
    ++p.record_946_calls;
    if (size != nullptr) {
        *size = sizeof(Host946Record);
        char detail[kInitPostCallDetailBytes];
        std::snprintf(detail, sizeof detail,
                      "0x988400 host record query: handle=%p id=%d size=0x%zx", handle, id,
                      *size);
        auto o = post_executed(detail);
        o.has_value = true;
        o.value = 0;
        return o;
    }
    if (buffer == nullptr) return post_blocked("record_buffer_missing", "0x988400: null fill");
    Host946Record record{};
    record.index = 0;
    record.name = p.user_name_946.data();
    std::memcpy(buffer, &record, sizeof record);
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x988400 host record fill: handle=%p id=%d index=%u name=%s", handle, id,
                  record.index, record.name);
    auto o = post_executed(detail);
    o.has_value = true;
    o.value = 0;
    return o;
}

InitPostCallOutcome host_946_copy_name(void* dst, std::size_t capacity, const char* src) {
    if (g_boot == nullptr || dst == nullptr || src == nullptr) {
        return post_blocked("copy_arguments_missing", "0x9556a0: null destination/source");
    }
    ++g_boot->post.copy_946_calls;
    std::snprintf(static_cast<char*>(dst), capacity, "%s", src);
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x9556a0 host bounded copy: capacity=0x%zx source=%s",
                  capacity, src);
    return post_executed(detail);
}

InitPostCallOutcome host_946_release_a(void* handle) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x9878d0 host refcount release: handle=%p", handle);
    auto o = post_executed(detail);
    o.has_value = true;
    o.value = 0;
    return o;
}

InitPostCallOutcome host_946_release_b(void* handle) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x987260 host refcount release: handle=%p", handle);
    auto o = post_executed(detail);
    o.has_value = true;
    o.value = 0;
    return o;
}

InitPostCallOutcome host_946_log(std::int32_t a, std::int32_t b, std::int32_t status,
                                 std::uint32_t message0, std::uint32_t message1,
                                 std::int32_t value) {
    if (g_boot == nullptr) return post_blocked("runtime_missing", "0x984700: no runtime state");
    ++g_boot->post.log_946_calls;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x984700 host log contract: a=%d b=%d status=%d messages=0x%x/0x%x "
                  "value=%d",
                  a, b, status, message0, message1, value);
    return post_executed(detail);
}

InitPostCallOutcome host_946_direct(std::int32_t first, std::int32_t value, void* third,
                                    std::int32_t fourth) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x944de0 body not lifted: 399 B; args %d,%d,%p,%d; valid 0x98a450 handle "
                  "branch is outside the observed invalid-handle path",
                  first, value, third, fourth);
    return post_blocked("body_not_lifted", detail);
}

// 0xa5cd90 SoundSystemSndx::Init host seams. Every callee is an engine/libc contract that is
// not lifted; the native body owns the branches, argument setup and stores. The 2 MiB first
// arena, the 0x030a descriptor mode, the callback block and the task constants are pinned from
// the body. The resource object, the 0x100000 second-arena size and the once state are host
// state, named as such in each detail.
std::uint64_t a5_load64(const void* p) {
    std::uint64_t value = 0;
    std::memcpy(&value, p, sizeof value);
    return value;
}

InitPostCallOutcome host_a5_9854f0(std::uintptr_t callback) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x9854f0 host callback registration: fn=0x%llx",
                  static_cast<unsigned long long>(callback));
    return post_executed(detail);
}
InitPostCallOutcome host_a5_994dd0(std::uintptr_t first, std::uintptr_t second) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x994dd0 host callback pair: 0x%llx/0x%llx",
                  static_cast<unsigned long long>(first),
                  static_cast<unsigned long long>(second));
    return post_executed(detail);
}
InitPostCallOutcome host_a5_994e70(std::uintptr_t config_rva) {
    // The argument is an original-image string RVA used as an identity tag. Reading it as a host
    // pointer is a wild dereference (the 2026-11-10 tree segfaulted at 0x12c4f51 here).
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x994e70 host config apply: string rva=0x%llx (tag only, bytes not read)",
                  static_cast<unsigned long long>(config_rva));
    return post_executed(detail);
}
InitPostCallOutcome host_a5_991c70(const void* block) {
    if (g_boot == nullptr) return post_blocked("runtime_missing", "0x991c70: no runtime");
    auto& p = g_boot->post;
    std::memcpy(p.callback_a5.data(), block, p.callback_a5.size());
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x991c70 host callback/model block: 0x%llx/0x%llx/0x%llx/0x%llx/0x%llx",
                  static_cast<unsigned long long>(a5_load64(p.callback_a5.data())),
                  static_cast<unsigned long long>(a5_load64(p.callback_a5.data() + 8)),
                  static_cast<unsigned long long>(a5_load64(p.callback_a5.data() + 0x10)),
                  static_cast<unsigned long long>(a5_load64(p.callback_a5.data() + 0x18)),
                  static_cast<unsigned long long>(a5_load64(p.callback_a5.data() + 0x20)));
    return post_executed(detail);
}
InitPostCallOutcome host_a5_12820(std::size_t size, std::uint32_t tag, std::uint32_t align,
                                  std::uint32_t flags, void** out) {
    if (g_boot == nullptr || out == nullptr) {
        return post_blocked("runtime_missing", "0x12820: no runtime/output");
    }
    if (size == 0 || size > 0x400000) {
        return post_blocked("alloc_12820_size", "0x12820 host arena bounds");
    }
    auto& p = g_boot->post;
    auto& arena = p.arena_a5_allocs == 0 ? p.arena_a5_0 : p.arena_a5_1;
    ++p.arena_a5_allocs;
    try {
        arena.assign(size, 0);
    } catch (...) {
        return post_blocked("alloc_12820_throw", "0x12820 host arena allocation failed");
    }
    *out = arena.data();
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x12820 host arena: size=0x%zx tag=0x%x align=0x%x flags=0x%x -> %p",
                  size, tag, align, flags, *out);
    InitPostCallOutcome o = post_executed(detail);
    o.has_value = true;
    o.value = reinterpret_cast<std::uintptr_t>(*out);
    return o;
}
InitPostCallOutcome host_a5_992210(void* buffer, std::size_t size, std::uint32_t align,
                                   std::uint32_t flags, std::int32_t* result) {
    if (buffer == nullptr || result == nullptr) {
        return post_blocked("runtime_missing", "0x992210: buffer/result missing");
    }
    std::memset(buffer, 0, size);
    *result = 0;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x992210 host zero-fill: %p size=0x%zx align=0x%x flags=0x%x -> 0",
                  buffer, size, align, flags);
    InitPostCallOutcome o = post_executed(detail);
    o.has_value = true;
    o.value = 0;
    return o;
}
InitPostCallOutcome host_a5_14c2f0(void* key_out) {
    if (g_boot == nullptr || key_out == nullptr) {
        return post_blocked("runtime_missing", "0x14c2f0: key output missing");
    }
    auto& p = g_boot->post;
    std::memset(p.key_a5.data(), 0, p.key_a5.size());
    const std::uint64_t token = 0x4141414141414141ULL;
    std::memcpy(p.key_a5.data(), &token, sizeof token);
    std::memcpy(key_out, p.key_a5.data(), p.key_a5.size());
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x14c2f0 host resource key: %p token=0x%llx",
                  key_out, static_cast<unsigned long long>(token));
    return post_executed(detail);
}
InitPostCallOutcome host_a5_resolve(void* out, void* registry, const void* key) {
    if (g_boot == nullptr || out == nullptr) {
        return post_blocked("runtime_missing", "0x2ee2cf8+0x18: output missing");
    }
    auto& p = g_boot->post;
    std::memset(out, 0, p.resolved_a5.size());
    void* resource = p.resource_a5.data();
    std::memcpy(out, &resource, sizeof resource);
    static_cast<std::uint8_t*>(out)[8] = 1;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x2ee2cf8+0x18 host resolver: registry=%p key=%p -> resource=%p ready=1",
                  registry, key, resource);
    InitPostCallOutcome o = post_executed(detail);
    o.has_value = true;
    o.value = reinterpret_cast<std::uintptr_t>(resource);
    return o;
}
InitPostCallOutcome host_a5_addref(void* pointer) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x14ccb0 host addref: %p", pointer);
    return post_executed(detail);
}
InitPostCallOutcome host_a5_release(void* pointer) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x14ccc0 host release: %p", pointer);
    return post_executed(detail);
}
InitPostCallOutcome host_a5_destroy(void* pointer) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x14d2d0 host destroy: %p", pointer);
    return post_executed(detail);
}
InitPostCallOutcome host_a5_notify(void* pointer) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x95580 host notify: field=%p", pointer);
    return post_executed(detail);
}
InitPostCallOutcome host_a5_release_key(void* pointer) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0xf590 host key release: %p", pointer);
    return post_executed(detail);
}
InitPostCallOutcome host_a5_once(bool* initialized) {
    if (g_boot == nullptr || initialized == nullptr) {
        return post_blocked("runtime_missing", "0xf8f3d0: no runtime/flag");
    }
    auto& p = g_boot->post;
    *initialized = p.once_a5_calls == 0;
    ++p.once_a5_calls;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0xf8f3d0 host once at 0x2ed3668: initialized=%d",
                  *initialized ? 1 : 0);
    return post_executed(detail);
}
InitPostCallOutcome host_a5_thread(std::uint64_t* thread) {
    if (thread == nullptr) return post_blocked("runtime_missing", "0xf730: output missing");
    *thread = 0x5a17c0de5a17c0deULL;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0xf730 host thread token: 0x%llx",
                  static_cast<unsigned long long>(*thread));
    return post_executed(detail);
}
InitPostCallOutcome host_a5_mutex(std::uintptr_t lock, std::uint64_t value,
                                  std::uint64_t owner) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x14c4a0 host mutex init: lock=0x%llx value=0x%llx owner=0x%llx",
                  static_cast<unsigned long long>(lock),
                  static_cast<unsigned long long>(value),
                  static_cast<unsigned long long>(owner));
    return post_executed(detail);
}
InitPostCallOutcome host_a5_key_create(std::uintptr_t function, std::uintptr_t lock,
                                       std::uintptr_t data) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0xf8f390 host key create: fn=0x%llx lock=0x%llx data=0x%llx",
                  static_cast<unsigned long long>(function),
                  static_cast<unsigned long long>(lock),
                  static_cast<unsigned long long>(data));
    return post_executed(detail);
}
InitPostCallOutcome host_a5_once_end(std::uintptr_t key) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0xf8f3e0 host once end: 0x%llx",
                  static_cast<unsigned long long>(key));
    return post_executed(detail);
}
InitPostCallOutcome host_a5_name_resource(void* resource, std::uint32_t name_id,
                                          std::uintptr_t lock) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x14cf20 host resource name: resource=%p id=0x%x lock=0x%llx",
                  resource, name_id, static_cast<unsigned long long>(lock));
    return post_executed(detail);
}
InitPostCallOutcome host_a5_current(std::uint64_t* current) {
    if (current == nullptr) return post_blocked("runtime_missing", "0x14d880: no output");
    *current = 0;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x14d880 host current token: 0");
    return post_executed(detail);
}
InitPostCallOutcome host_a5_param(void* object, std::uint64_t* param_out) {
    if (g_boot == nullptr || param_out == nullptr) {
        return post_blocked("runtime_missing", "0x14d7b0: no runtime/output");
    }
    auto& p = g_boot->post;
    std::memset(p.param_a5.data(), 0, p.param_a5.size());
    *param_out = 0;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x14d7b0 host param query: object=%p -> [0x60]=0",
                  object);
    return post_executed(detail);
}
InitPostCallOutcome host_a5_status(std::uintptr_t param, std::int32_t* status) {
    if (status == nullptr) return post_blocked("runtime_missing", "0x9892d0: no status");
    *status = 0;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x9892d0 host status: param=0x%llx -> 0",
                  static_cast<unsigned long long>(param));
    InitPostCallOutcome o = post_executed(detail);
    o.has_value = true;
    o.value = 0;
    return o;
}
InitPostCallOutcome host_a5_size(std::int32_t status, std::uint64_t* size) {
    if (size == nullptr) return post_blocked("runtime_missing", "0x946590: no output");
    *size = 0x100000;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x946590 host size handshake: status=%d -> 0x100000 (host value)", status);
    return post_executed(detail);
}
InitPostCallOutcome host_a5_region(std::int32_t status, void* region, std::uint64_t size) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x946650 host region bind: status=%d region=%p size=0x%llx", status, region,
                  static_cast<unsigned long long>(size));
    return post_executed(detail);
}
InitPostCallOutcome host_a5_mode(std::uint8_t low, std::uint8_t high) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x994070 host descriptor mode bytes: 0x%02x/0x%02x",
                  low, high);
    return post_executed(detail);
}
InitPostCallOutcome host_a5_user_id(std::uint64_t* value, std::int32_t* result) {
    if (value == nullptr || result == nullptr) {
        return post_blocked("runtime_missing", "0xf901e0: output missing");
    }
    *value = 0;
    *result = 0;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0xf901e0 host UserID check: result=0 value=0");
    return post_executed(detail);
}
InitPostCallOutcome host_a5_success_log(std::int32_t code, std::uint64_t value) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x994640 host success log: code=%d value=0x%llx",
                  code, static_cast<unsigned long long>(value));
    return post_executed(detail);
}
InitPostCallOutcome host_a5_log_condition(bool* should_log) {
    if (should_log == nullptr) return post_blocked("runtime_missing", "log_condition");
    *should_log = false;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "inline 0x2ea5170 queue check: empty; error log not emitted");
    return post_executed(detail);
}
InitPostCallOutcome host_a5_error_log(std::int32_t a, std::int32_t b, const void* block,
                                      std::int32_t c, std::int32_t d,
                                      std::uintptr_t message) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x3e7e0 host error log: %d/0x%x block=%p %d/%d message=0x%llx", a, b, block,
                  c, d, static_cast<unsigned long long>(message));
    return post_executed(detail);
}
InitPostCallOutcome host_a5_alloc_task(std::size_t size, void** out) {
    if (g_boot == nullptr || out == nullptr) {
        return post_blocked("runtime_missing", "0x17070: no runtime/output");
    }
    if (size != kInitA5cd90Object70Bytes) {
        return post_blocked("alloc_17070_size", "0x17070 host task accepts only 0x70");
    }
    auto& p = g_boot->post;
    std::memset(p.task_a5.data(), 0, p.task_a5.size());
    *out = p.task_a5.data();
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x17070 host task object: size=0x70 -> %p", *out);
    InitPostCallOutcome o = post_executed(detail);
    o.has_value = true;
    o.value = reinterpret_cast<std::uintptr_t>(*out);
    return o;
}
InitPostCallOutcome host_a5_task(void* task, const void* descriptor, std::uintptr_t name,
                                 std::uint32_t line, std::uint32_t arg4, std::uint32_t arg5,
                                 std::uint64_t stack_arg) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x3b5c0 host task init: task=%p desc=%p name=0x%llx line=%u args=%u/%u "
                  "stack=0x%llx",
                  task, descriptor, static_cast<unsigned long long>(name), line, arg4, arg5,
                  static_cast<unsigned long long>(stack_arg));
    return post_executed(detail);
}
InitPostCallOutcome host_a5_virtual_release(void* pointer) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "virtual [old]+0x8 release: %p", pointer);
    return post_executed(detail);
}
std::uint64_t host_a5_canary() { return 0x5a17c0de5a17c0deULL; }
InitPostCallOutcome host_a5_trap() {
    return post_executed("0xf8f3a0 stack-check trap (pinned, fixture-only)");
}

InitPostCallOutcome hook_56_a(void*, const void* descriptor) {
    if (g_boot == nullptr) return post_blocked("runtime_missing", "0xa5cd90: no runtime state");
    auto& p = g_boot->post;
    p.hooks_a5.descriptor = static_cast<const std::uint8_t*>(descriptor);
    p.result_a5 = run_init_a5cd90_body(p.hooks_a5);
    const auto& r = p.result_a5;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "native 0xa5cd90 (1143 B): calls=%d allocs=%d field69c8=%u field08=%u "
                  "resource=%p ready=%d flag18=%u flag20=%u a4f8=0x%llx byte0=%u once=%d "
                  "user_id=%d log=%d trap=%d",
                  r.calls_total, r.alloc_calls, r.field_69c8, r.field_08, r.resource,
                  r.resource_ready ? 1 : 0, r.flag_18, r.flag_20,
                  static_cast<unsigned long long>(r.object_a4f8), r.byte0, r.once_calls,
                  r.user_id_check, r.log_calls, r.trap_calls);
    if (!r.ok) {
        return post_blocked(r.blocked != nullptr ? r.blocked : "native_blocked", detail);
    }
    return post_executed(detail);
}
InitPostCallOutcome hook_56_946880(int value) {
    if (g_boot == nullptr) {
        return post_blocked("runtime_missing", "0x946880: no runtime state");
    }
    auto& p = g_boot->post;
    p.result_946 = run_init_946880_body(value, p.service_946.data(), p.service_946.size(),
                                        p.hooks_946);
    const auto& r = p.result_946;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "native 0x946880 (353 B): value=%d status=%d branch=%s lock=%u "
                  "inner_ok=%d inner_calls=%zu log=%d call_log=%zu",
                  value, r.status, r.took_9469f0 ? "0x9469f0" : (r.took_944de0 ? "0x944de0" : "none"),
                  p.lock_946, r.inner.ok ? 1 : 0, r.inner.call_log.count, r.log_calls,
                  r.call_log.count);
    InitPostCallOutcome o = r.ok ? post_executed(detail)
                                 : post_blocked(r.blocked != nullptr ? r.blocked : "native_blocked",
                                                detail);
    o.has_value = r.ok;
    o.value = static_cast<std::uint64_t>(static_cast<std::uint32_t>(r.status));
    return o;
}
bool hook_a5d990_menu_insert(void* parent, DebugMenuItem&) noexcept {
    if (g_boot == nullptr || parent == nullptr) return false;
    ++g_boot->post.menu_inserts_a5d990;
    return true;
}
bool hook_a5d990_destroy(void*, DebugMenuItem&) noexcept {
    // 0x272d0 is pinned, never executed; the host owns the item lifetime.
    return true;
}
bool hook_a5d990_grow(void*, SndxCategoryVectorId, SndxCategoryVectorSpan&) noexcept {
    // The runtime pre-sizes both vectors to 32 slots, so the 0xa62bc0 growth path is never
    // reached here; it is fixture-covered (engine_category_vars).
    return false;
}
InitPostCallOutcome hook_56_c(void*, void*) {
    if (g_boot == nullptr) return post_blocked("runtime_missing", "0xa5d990: no runtime state");
    auto& p = g_boot->post;
    p.result_a5d990 = run_init_a5d990_body(p.hooks_a5d990);
    const auto& r = p.result_a5d990;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "native 0xa5d990 (2709 B): mute=%zu filter=%zu toggles=%zu stage=%d "
                  "separators=%d commands=%d registered=%d destroys=%d dead_unnamed=%d",
                  r.mute_built, r.filter_built, r.toggle_items, static_cast<int>(r.stage),
                  r.separators, r.commands, r.registered, r.destroys,
                  r.dead_unnamed_block ? 1 : 0);
    if (!r.ok) {
        return post_blocked(r.blocked != nullptr ? r.blocked : "native_blocked", detail);
    }
    return post_executed(detail);
}
// 0x560b60 host seams. The 0x136f0 object and the three buffers are host-owned; the
// allocator hooks return those exact ranges only for the sizes observed at 0x560b99,
// 0x560c21, 0x560c46 and 0x560c6b. No unobserved allocation is fabricated.
InitPostCallOutcome host_560_descriptor(const void* descriptor) {
    if (g_boot == nullptr) return post_blocked("runtime_missing", "0x11bc0: no runtime state");
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x11bc0 host descriptor: %p; body copies 0x18 B to 0x2f08910/0x2f08920",
                  descriptor);
    return post_executed(detail);
}
InitPostCallOutcome host_560_alloc_object(std::size_t size, void** out) {
    if (g_boot == nullptr) return post_blocked("runtime_missing", "0x17070: no runtime state");
    auto& p = g_boot->post;
    if (size != kInit560b60ObjectBytes || out == nullptr) {
        return post_blocked("alloc_17070_size_mismatch",
                            "0x17070 host object only accepts 0x136f0");
    }
    *out = p.object_560.data();
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x17070 host object: size=0x%zx -> %p", size, *out);
    InitPostCallOutcome o = post_executed(detail);
    o.has_value = true;
    o.value = reinterpret_cast<std::uintptr_t>(*out);
    return o;
}
InitPostCallOutcome host_560_alloc_buffer(std::size_t size, void** out) {
    if (g_boot == nullptr) return post_blocked("runtime_missing", "0x17090: no runtime state");
    auto& p = g_boot->post;
    void* pointer = nullptr;
    if (size == kInit560b60Buffer0Bytes) pointer = p.buffer0_560.data();
    else if (size == kInit560b60Buffer1Bytes) pointer = p.buffer1_560.data();
    else if (size == kInit560b60Buffer2Bytes) pointer = p.buffer2_560.data();
    if (out == nullptr || pointer == nullptr) {
        return post_blocked("alloc_17090_size_mismatch",
                            "0x17090 host buffer only accepts 0x2000/0x400/0x200");
    }
    *out = pointer;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x17090 host buffer: size=0x%zx -> %p", size, pointer);
    InitPostCallOutcome o = post_executed(detail);
    o.has_value = true;
    o.value = reinterpret_cast<std::uintptr_t>(pointer);
    return o;
}
InitPostCallOutcome host_560_free(void* pointer) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "host free observation: %p (host storage retained)", pointer);
    return post_executed(detail);
}
InitPostCallOutcome host_560_stack(void* a, void* b, void* c) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x3ca330 host stack descriptors: %p/%p/%p", a, b, c);
    return post_executed(detail);
}
InitPostCallOutcome host_560_register(std::uintptr_t a, std::uintptr_t b, std::uintptr_t c) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x273890 host registration: 0x%llx/0x%llx/0x%llx",
                  static_cast<unsigned long long>(a), static_cast<unsigned long long>(b),
                  static_cast<unsigned long long>(c));
    return post_executed(detail);
}
InitPostCallOutcome host_560_format(std::uint8_t* out, const std::uint8_t* input) {
    if (out == nullptr || input == nullptr) {
        return post_blocked("format_output_missing", "0x14c0f0: output/input missing");
    }
    std::uint32_t value = 0;
    std::memcpy(out, &value, sizeof value);
    std::memset(out + 8, 0, kInit560b60Global30Bytes);
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x14c0f0 host format: input=%p; writes int 0 and 32-byte zero block", input);
    return post_executed(detail);
}
InitPostCallOutcome host_560_release(void* pointer) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0xf590 host release observation: %p", pointer);
    return post_executed(detail);
}
InitPostCallOutcome host_560_probe(void* pointer, bool* take_branch) {
    if (take_branch == nullptr) {
        return post_blocked("probe_output_missing", "0x56b540: branch output missing");
    }
    *take_branch = false;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x56b540 host probe: %p -> false; 0x95580 branch not taken", pointer);
    return post_executed(detail);
}
InitPostCallOutcome host_560_branch(void* pointer) {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail, "0x95580 host branch observation: %p", pointer);
    return post_executed(detail);
}
InitPostCallOutcome host_560_final() {
    return post_executed("0x56efb0 host finalizer observation");
}

InitPostCallOutcome hook_56_d() {
    if (g_boot == nullptr) return post_blocked("runtime_missing", "0x560b60: no runtime state");
    auto& p = g_boot->post;
    p.result_560 = run_init_560b60_body(p.hooks_560);
    const auto& r = p.result_560;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "native 0x560b60 (803 B): calls=%d allocs=%d frees=%d object=%p "
                  "buffers=%p/%p/%p prev_cleanup=%d global28=%u",
                  r.calls_total, r.alloc_calls, r.free_calls, r.object, r.buffer_2000,
                  r.buffer_0400, r.buffer_0200, r.previous_cleanup ? 1 : 0, r.global_28);
    if (!r.ok) {
        return post_blocked(r.blocked != nullptr ? r.blocked : "native_blocked", detail);
    }
    return post_executed(detail);
}
void run_56_body() {
    if (g_boot != nullptr) g_boot->post.result_56 = run_init_56dcb0_body(g_boot->post.hooks_56);
}

bool hook_5b9f90_release(void*, const SharedStr&) noexcept {
    // Every view in these slices is a borrowed literal, so 0xf590 has nothing to free; the host
    // records the observation.
    return true;
}
InitPostCallOutcome hook_5b9f90_page_builder(void*, std::uint64_t rva, const char* name) noexcept {
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x%llx page builder \"%s\": named host contract, callee body not lifted",
                  static_cast<unsigned long long>(rva), name != nullptr ? name : "?");
    return post_executed(detail);
}
InitPostCallOutcome hook_5c_a() noexcept {
    if (g_boot == nullptr) return post_blocked("runtime_missing", "0x5b9f90: no runtime state");
    auto& p = g_boot->post;
    p.result_5b9f90 = run_init_5b9f90_slices(p.hooks_5b9f90);
    const auto& r = p.result_5b9f90;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "native 0x5b9f90 slices S0/S3.1..S3.6 (%zu of 46881 B): groups=%d commands=%d "
                  "toggles=%d floats=%d named_vars=%d registered=%d releases=%d destroys=%d "
                  "contracts=%d canary=%d resume=0x%llx remaining=%zu B",
                  static_cast<std::size_t>(kInit5b9f90ExecutedBytes), r.groups, r.commands,
                  r.toggles, r.float_items, r.named_vars, r.registered, r.releases, r.destroys,
                  r.contracts,
                  r.canary_stored ? 1 : 0,                   static_cast<unsigned long long>(r.resume_rva),
                  static_cast<std::size_t>(kInit5b9f90Size - kInit5b9f90ExecutedBytes));
    InitPostCallOutcome o;  // the body's own record stays blocked: 37,607 B are unlifted
    if (!r.ok) {
        o = post_blocked(r.blocked != nullptr ? r.blocked : "slice_failed", detail);
    } else {
        o = post_blocked("body_partially_lifted", detail);
    }
    o.subrecords = r.records.data();
    o.subrecord_count = static_cast<std::uint8_t>(r.record_count);
    return o;
}
InitPostCallOutcome host_97a50(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2,
                               std::uint32_t a3, std::uint32_t a4, std::uint32_t a5,
                               std::uint32_t a6, std::uint32_t a7, std::uint32_t a8,
                               std::uint32_t a9) {
    if (g_boot == nullptr) return post_blocked("runtime_missing", "0x97a50: no runtime state");
    auto& p = g_boot->post;
    p.allocation_96.assign(kInit96f20CalleeAllocationBytes, 0);
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x97a50 host object: args=0x%x,0x%x,%u,%u,0x%x,0x%x,%u,%u,%u,%u; "
                  "observed 0x97af7 allocation=0x%zx -> %p (callee fields not lifted)",
                  a0, a1, a2, a3, a4, a5, a6, a7, a8, a9,
                  kInit96f20CalleeAllocationBytes, p.allocation_96.data());
    InitPostCallOutcome o = post_executed(detail);
    o.has_value = true;
    o.value = reinterpret_cast<std::uintptr_t>(p.allocation_96.data());
    return o;
}
InitPostCallOutcome hook_5c_96(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2,
                               std::uint32_t a3) {
    if (g_boot == nullptr) return post_blocked("runtime_missing", "0x96f20: no runtime state");
    Init96f20Hooks hooks{};
    hooks.call_97a50 = host_97a50;
    auto& p = g_boot->post;
    p.result_96 = run_init_96f20_body(a0, a1, a2, a3, hooks);
    const auto& r = p.result_96;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "native 0x96f20 (41 B): args=0x%x,0x%x,0x%x,0x%x -> 0x97a50 host "
                  "allocation %llu B at %p",
                  a0, a1, a2, a3,
                  static_cast<unsigned long long>(p.allocation_96.size()),
                  p.allocation_96.empty() ? nullptr : p.allocation_96.data());
    if (!r.ok) {
        return post_blocked(r.blocked != nullptr ? r.blocked : "native_blocked", detail);
    }
    InitPostCallOutcome o = post_executed(detail);
    o.has_value = true;
    o.value = r.return_value;
    return o;
}
InitPostCallOutcome hook_5c_ret() {
    return post_executed("1-byte FDE body: ret; no register or memory effect");
}
// 0x12820 is the real game allocator; this host contract is a bounded arena that reports
// every call. The measured 0x5d5410 path sets a4 bit1 on both 0x8be60 calls and never
// reaches it. Failure is a named blocker, not a fabricated pointer.
InitPostCallOutcome host_5d_alloc(std::uint32_t size, std::uint32_t tag,
                                  std::uint32_t alignment, std::uint32_t flags) {
    if (g_boot == nullptr) return post_blocked("runtime_missing", "0x12820: no runtime state");
    auto& p = g_boot->post;
    ++p.alloc_5d_calls;
    const std::size_t align = alignment != 0 ? alignment : 1u;
    const std::size_t start = (p.arena_5d_used + align - 1u) & ~(align - 1u);
    if (start + size > p.arena_5d.size()) {
        char detail[kInitPostCallDetailBytes];
        std::snprintf(detail, sizeof detail,
                      "host arena 0x12820 exhausted: size=%u tag=0x%x align=0x%x used=%zu/%zu",
                      size, tag, alignment, p.arena_5d_used, p.arena_5d.size());
        return post_blocked("alloc_12820_arena_exhausted", detail);
    }
    p.arena_5d_used = start + size;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "host arena 0x12820: size=%u tag=0x%x align=0x%x flags=0x%x -> +%zu",
                  size, tag, alignment, flags, start);
    InitPostCallOutcome o = post_executed(detail);
    o.has_value = true;
    o.value = static_cast<std::uint64_t>(
        reinterpret_cast<std::uintptr_t>(p.arena_5d.data() + start));
    return o;
}

// 0x87f3e0 is a 5-byte thunk to 0xf906d0, which jumps through the import slot at
// 0x1600958. The destination and all six arguments are recorded, but the platform tail
// itself is not lifted, so the contract refuses rather than pretending to copy.
InitPostCallOutcome host_5d_tail(void* dst, std::uint32_t tag, std::uint64_t packed,
                                 std::uint32_t count, std::uint64_t arg5,
                                 std::uint32_t arg6, std::uint64_t arg7) {
    if (g_boot == nullptr) return post_blocked("runtime_missing", "0x87f3e0: no runtime state");
    ++g_boot->post.tail_5d_calls;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "0x87f3e0 -> 0xf906d0 import slot 0x1600958 not lifted: dst=%p tag=0x%x "
                  "packed=0x%llx count=%u arg5=0x%llx arg6=0x%x arg7=0x%llx",
                  dst, tag, static_cast<unsigned long long>(packed), count,
                  static_cast<unsigned long long>(arg5), arg6,
                  static_cast<unsigned long long>(arg7));
    return post_blocked("platform_tail_not_lifted", detail);
}

InitPostCallOutcome hook_5d5410() {
    if (g_boot == nullptr) {
        return post_blocked("runtime_missing", "0x5d5410: no runtime state");
    }
    auto& p = g_boot->post;
    p.hooks_5d.object_a = p.object_a_5d.data();
    p.hooks_5d.object_b = p.object_b_5d.data();
    p.hooks_5d.global_owner = p.owner_5d;
    p.hooks_5d.state = p.state_5d.data();
    p.hooks_5d.be.alloc_12820 = host_5d_alloc;
    p.hooks_5d.be.tail_87f3e0 = host_5d_tail;
    p.result_5d = run_init_5d5410_body(p.hooks_5d);
    const auto& r = p.result_5d;
    std::uint64_t state00 = 0, state10 = 0, state40 = 0;
    std::memcpy(&state00, p.state_5d.data() + 0x00, sizeof state00);
    std::memcpy(&state10, p.state_5d.data() + 0x10, sizeof state10);
    std::memcpy(&state40, p.state_5d.data() + 0x40, sizeof state40);
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "native 0x5d5410 (420 B): 8be60=%d 8f520=%d 8f1d0=%d blocked=%d "
                  "owner=0x%llx obj_a=0x%x/0x%x obj_b=0x%x/0x%x "
                  "state00=0x%llx state10=0x%llx state40=0x%llx",
                  r.calls_8be60, r.calls_8f520, r.calls_8f1d0, r.blocked_calls,
                  static_cast<unsigned long long>(r.owner), r.inner_a.object_word4,
                  r.inner_a.object_word_c, r.inner_b.object_word4,
                  r.inner_b.object_word_c,
                  static_cast<unsigned long long>(state00),
                  static_cast<unsigned long long>(state10),
                  static_cast<unsigned long long>(state40));
    InitPostCallOutcome o = r.ok
        ? post_executed(detail)
        : post_blocked(r.blocked != nullptr ? r.blocked : "native_blocked", detail);
    o.has_value = r.ok;
    o.value = r.ok ? 1u : 0u;
    return o;
}

std::uint32_t read_u32_5c91(const std::uint8_t* p) noexcept {
    std::uint32_t v = 0;
    std::memcpy(&v, p, sizeof v);
    return v;
}
std::uint64_t read_u64_5c91(const std::uint8_t* p) noexcept {
    std::uint64_t v = 0;
    std::memcpy(&v, p, sizeof v);
    return v;
}
void write_u32_5c91(std::uint8_t* p, std::uint32_t v) noexcept {
    std::memcpy(p, &v, sizeof v);
}

InitPostCallOutcome hook_5c91e0() {
    if (g_boot == nullptr) return post_blocked("runtime_missing", "0x5c91e0: no runtime state");
    auto& p = g_boot->post;
    std::uint8_t* s = p.data_src_5c91.data();
    std::uint8_t* d = p.data_dst_5c91.data();
    const std::uint64_t q8 = read_u64_5c91(s + 8);
    const std::uint64_t q10 = read_u64_5c91(s + 0x10);
    const std::uint64_t q18 = read_u64_5c91(s + 0x18);
    const std::uint64_t q20 = read_u64_5c91(s + 0x20);
    const std::uint64_t q28 = read_u64_5c91(s + 0x28);
    const std::uint64_t q30 = read_u64_5c91(s + 0x30);
    write_u32_5c91(d + 0x00, read_u32_5c91(s + 0x04));
    write_u32_5c91(d + 0x04, static_cast<std::uint32_t>(q8));
    write_u32_5c91(d + 0x08, static_cast<std::uint32_t>(q8 >> 32));
    write_u32_5c91(d + 0x0c, static_cast<std::uint32_t>(q10));
    write_u32_5c91(d + 0x20, static_cast<std::uint32_t>(q30));
    write_u32_5c91(d + 0x24, static_cast<std::uint32_t>(q30 >> 32));
    write_u32_5c91(d + 0x10, static_cast<std::uint32_t>(q10 >> 32));
    write_u32_5c91(d + 0x14, static_cast<std::uint32_t>(q18));
    write_u32_5c91(d + 0x18, static_cast<std::uint32_t>(q18 >> 32));
    write_u32_5c91(d + 0x1c, static_cast<std::uint32_t>(q20));
    write_u32_5c91(s + 0x54, static_cast<std::uint32_t>(q20 >> 32));
    write_u32_5c91(s + 0x58, static_cast<std::uint32_t>(q28));
    write_u32_5c91(s + 0x5c, static_cast<std::uint32_t>(q28 >> 32));
    write_u32_5c91(s + 0x84, 0x3fc00000u);
    write_u32_5c91(s + 0x98, 0x3fc00000u);
    write_u32_5c91(s + 0x80, 0x3f400000u);
    write_u32_5c91(s + 0x94, 0x3f400000u);
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "read 0x%zx-byte source, wrote 0x%zx-byte destination; dst[0]=0x%x "
                  "dst[0x20]=0x%x dst[0x24]=0x%x; "
                  "src[0x54]=0x%x src[0x80]=0x%x src[0x84]=0x%x",
                  kInit5c91e0SourceBytes, p.data_dst_5c91.size(), read_u32_5c91(d + 0x00),
                  read_u32_5c91(d + 0x20), read_u32_5c91(d + 0x24),
                  read_u32_5c91(s + 0x54), read_u32_5c91(s + 0x80), read_u32_5c91(s + 0x84));
    return post_executed(detail);
}
void run_5c_body() {
    if (g_boot != nullptr) g_boot->post.result_5c = run_init_5c64c0_body(g_boot->post.hooks_5c);
}

InitPostCallOutcome hook_7f_query() {
    if (g_boot == nullptr || g_boot->post.query_global_5d.active == nullptr) {
        return post_blocked("parent_global_missing",
                            "0x5d06f0: [0x2ff59c0]+0x60 has no host owner");
    }
    const bool active = *g_boot->post.query_global_5d.active;
    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "host parent [0x2ff59c0]+0x60 byte=%u; query returns %s", active ? 1u : 0u,
                  active ? "true" : "false");
    InitPostCallOutcome o = post_executed(detail);
    o.has_value = true;
    o.value = active ? 1 : 0;
    return o;
}
std::array<std::string, 16> g_staffroll_slots;
std::vector<std::unique_ptr<char[]>> g_staffroll_texts;

const char* staffroll_string_for_rva(std::uintptr_t rva) noexcept {
    switch (rva) {
        case 0x128b878:
            return "cp10_shadow_staffroll.txt";
        case 0x128b892:
            return "staffroll_original_us.txt";
        case 0x128b8ac:
            return "staffroll_remaster_us.txt";
        default:
            return nullptr;
    }
}

const char* staffroll_fallback_text(const char* path) noexcept {
    if (std::strstr(path, "remaster") != nullptr) return "CENTER_FULL_SCREEN,";
    if (std::strstr(path, "original") != nullptr) return "TARGET,";
    if (std::strstr(path, "cp10_shadow") != nullptr) return "STYLE,";
    return "CENTER,";
}

InitPostCallOutcome host_staffroll_load(const char* path, std::int64_t, int* slot) {
    if (path == nullptr || slot == nullptr) {
        return post_blocked("staffroll_load_args_missing", "0x56cdb0 host load: null arg");
    }
    for (std::size_t i = 0; i < g_staffroll_slots.size(); ++i) {
        if (!g_staffroll_slots[i].empty()) continue;
        g_staffroll_slots[i] = staffroll_fallback_text(path);
        *slot = static_cast<int>(i);
        char detail[kInitPostCallDetailBytes];
        std::snprintf(detail, sizeof detail, "host fallback load %s -> slot %zu (%zu B)", path, i,
                      g_staffroll_slots[i].size());
        return post_executed(detail);
    }
    return post_blocked("staffroll_slots_exhausted",
                        "0x56cdb0 host load: all 16 resource slots are live");
}

InitPostCallOutcome host_staffroll_size(int slot, std::size_t* size) {
    if (size == nullptr || slot < 0 || static_cast<std::size_t>(slot) >= g_staffroll_slots.size() ||
        g_staffroll_slots[static_cast<std::size_t>(slot)].empty()) {
        return post_blocked("staffroll_slot_invalid", "0x56ceb0 host size: invalid slot");
    }
    *size = g_staffroll_slots[static_cast<std::size_t>(slot)].size();
    return post_executed("host fallback size");
}

InitPostCallOutcome host_staffroll_copy(int slot, char* destination, std::size_t size) {
    if (destination == nullptr || slot < 0 ||
        static_cast<std::size_t>(slot) >= g_staffroll_slots.size() ||
        g_staffroll_slots[static_cast<std::size_t>(slot)].empty()) {
        return post_blocked("staffroll_slot_invalid", "0x56cec0 host copy: invalid slot");
    }
    const std::string& text = g_staffroll_slots[static_cast<std::size_t>(slot)];
    if (size != text.size()) {
        return post_blocked("staffroll_copy_size_mismatch",
                            "0x56cec0 host copy: size changed after 0x56ceb0");
    }
    std::memcpy(destination, text.data(), size);
    return post_executed("host fallback copy");
}

InitPostCallOutcome host_staffroll_close(int slot) {
    if (slot < 0 || static_cast<std::size_t>(slot) >= g_staffroll_slots.size() ||
        g_staffroll_slots[static_cast<std::size_t>(slot)].empty()) {
        return post_blocked("staffroll_slot_invalid", "0x56ce60 host close: invalid slot");
    }
    g_staffroll_slots[static_cast<std::size_t>(slot)].clear();
    return post_executed("host fallback close");
}

InitPostCallOutcome hook_7f_set(int mode, std::uintptr_t string_rva) {
    if (g_boot == nullptr) {
        return post_blocked("parent_missing", "0x7f6f90: no host parent");
    }
    const char* string = staffroll_string_for_rva(string_rva);
    if (string == nullptr) {
        char detail[kInitPostCallDetailBytes];
        std::snprintf(detail, sizeof detail, "0x7f6f90: unknown string RVA 0x%llx",
                      static_cast<unsigned long long>(string_rva));
        return post_blocked("string_rva_unknown", detail);
    }

    Staffroll7f6f90Hooks hooks{};
    hooks.load_56cdb0 = host_staffroll_load;
    hooks.size_56ceb0 = host_staffroll_size;
    hooks.copy_56cec0 = host_staffroll_copy;
    hooks.close_56ce60 = host_staffroll_close;
    hooks.counts = g_boot->post.staffroll_counts.data();
    hooks.entries = g_boot->post.staffroll_entries.data();
    hooks.entry_capacity = kStaffroll7f6f90MaxEntries;

    Staffroll7f6f90Result result = run_staffroll_7f6f90_body(mode, string, hooks);
    if (!result.ok) {
        g_boot->post.staffroll_blocked = result.blocked;
        char detail[kInitPostCallDetailBytes];
        std::snprintf(detail, sizeof detail, "0x7f6f90 mode=%d string=%s blocked=%s", mode,
                      string, result.blocked != nullptr ? result.blocked : "unknown");
        return post_blocked(result.blocked != nullptr ? result.blocked : "body_blocked", detail);
    }

    const int entries = result.entries;
    int index0 = -1;
    int rest0 = -1;
    if (entries > 0) {
        const Staffroll7f6f90Entry& first =
            g_boot->post.staffroll_entries[static_cast<std::size_t>(mode) *
                                               kStaffroll7f6f90MaxEntries];
        index0 = first.index;
        if (first.rest != nullptr && result.text != nullptr) {
            rest0 = static_cast<int>(first.rest - result.text.get());
        }
    }
    g_boot->post.staffroll_index0[static_cast<std::size_t>(mode)] = index0;
    g_boot->post.staffroll_rest0[static_cast<std::size_t>(mode)] = rest0;
    g_boot->post.staffroll_text_bytes[static_cast<std::size_t>(mode)] = result.text_bytes;
    g_boot->post.staffroll_used_fopen[static_cast<std::size_t>(mode)] = result.used_fopen;
    if (result.text != nullptr) g_staffroll_texts.push_back(std::move(result.text));

    char detail[kInitPostCallDetailBytes];
    std::snprintf(detail, sizeof detail,
                  "native 0x7f6f90 mode=%d entries=%d index0=%d rest0=%d bytes=%zu "
                  "fopen=%d",
                  mode, entries, index0, rest0, result.text_bytes, result.used_fopen ? 1 : 0);
    return post_executed(detail);
}
void run_7f_body() {
    if (g_boot != nullptr) g_boot->post.result_7f = run_staffroll_7f7220(g_boot->post.hooks_7f);
}

// The four targets 0x56db70 / 0x5b3d00 / 0x5b3cf0 / 0x56fad0 are one-byte `ret` bodies. These
// native functions have exactly that observable behavior: no input, no store, return. The call
// sites invoke them through the existing hook slots, and each invocation is counted.
void native_ret_56db70_body() {
    if (g_boot != nullptr) ++g_boot->post.ret_56db70_calls;
}
void native_ret_5b3d00_body() {
    if (g_boot != nullptr) ++g_boot->post.ret_5b3d00_calls;
}
void native_ret_5b3cf0_body() {
    if (g_boot != nullptr) ++g_boot->post.ret_5b3cf0_calls;
}
void native_ret_56fad0_body() {
    if (g_boot != nullptr) ++g_boot->post.ret_56fad0_calls;
}

// 0xf90310 is a PLT/Sony import (sceSystemServiceHideSplashScreen), not a game body. The host
// service is a named native seam; its invocation is observed, not claimed as a lifted body.
void host_f90310_splash_hide() {
    if (g_boot != nullptr) ++g_boot->post.f90310_calls;
}

void configure_post(HostPostBodies& p) {
    init_container_1bdb10(p.c1290_a);
    p.c1290_b = Container21cd0{};
    p.c1290_b.data = p.b1290_pool.data();
    p.c1290_b.count = 0;
    p.c1290_b.capacity = p.b1290_pool.size();
    p.c1290_b.lock_handle = 0;
    p.c1290_b.sync_bump_end();
    p.hooks_1290 = Init1290Hooks{};
    p.hooks_1290.container_a = &p.c1290_a;
    p.hooks_1290.alloc_a.alloc = host_node_alloc;
    p.hooks_1290.alloc_a.dealloc = host_node_free;
    p.hooks_1290.container_b = &p.c1290_b;
    p.hooks_1290.insert_b_hooks.begin = hook_1290_begin;
    p.hooks_1290.insert_b_hooks.end = hook_1290_end;
    p.hooks_1290.insert_b_hooks.grow = nullptr;
    p.hooks_1290.vcall_1594060_slot3 = hook_1290_vcall;

    p.hooks_56 = Init56dcb0BodyHooks{};
    p.hooks_56.object = p.object_56.data();
    p.hooks_56.data_cell = p.data_56.data();
    p.hooks_56.call_a5cd90 = hook_56_a;
    p.hooks_56.call_946880 = hook_56_946880;
    p.hooks_56.call_a5d990 = hook_56_c;
    p.hooks_56.call_560b60 = hook_56_d;

    p.descriptor_560.fill(0);
    p.global10_560.fill(0);
    p.global20_560 = 0;
    p.global28_560 = 0;
    p.global30_560.fill(0);
    p.global50_560.fill(0);
    p.previous_560 = nullptr;
    p.callbacks_560.fill(0);
    p.format_560.fill(0);
    p.result_560 = Init560b60Result{};
    p.hooks_560 = Init560b60Hooks{};
    p.hooks_560.descriptor = p.descriptor_560.data();
    p.hooks_560.object = p.object_560.data();
    p.hooks_560.buffer_2000 = p.buffer0_560.data();
    p.hooks_560.buffer_0400 = p.buffer1_560.data();
    p.hooks_560.buffer_0200 = p.buffer2_560.data();
    p.hooks_560.global_10 = p.global10_560.data();
    p.hooks_560.global_20 = &p.global20_560;
    p.hooks_560.global_28 = &p.global28_560;
    p.hooks_560.global_30 = p.global30_560.data();
    p.hooks_560.global_50 = p.global50_560.data();
    p.hooks_560.previous = &p.previous_560;
    p.hooks_560.callbacks = p.callbacks_560.data();
    p.hooks_560.format_out = p.format_560.data();
    p.hooks_560.call_11bc0 = host_560_descriptor;
    p.hooks_560.alloc_17070 = host_560_alloc_object;
    p.hooks_560.alloc_17090 = host_560_alloc_buffer;
    p.hooks_560.free_170b0 = host_560_free;
    p.hooks_560.free_170c0 = host_560_free;
    p.hooks_560.call_3ca330 = host_560_stack;
    p.hooks_560.call_273890 = host_560_register;
    p.hooks_560.call_14c0f0 = host_560_format;
    p.hooks_560.release_f590 = host_560_release;
    p.hooks_560.call_56b540 = host_560_probe;
    p.hooks_560.call_95580 = host_560_branch;
    p.hooks_560.call_56efb0 = host_560_final;

    p.object_a5.fill(0);
    p.descriptor_a5.fill(0);
    {
        const std::uint64_t name = kInitA5cd90DescriptorNameRva;
        const std::uint16_t mode = 0x030a;
        std::memcpy(p.descriptor_a5.data(), &name, sizeof name);
        std::memcpy(p.descriptor_a5.data() + 8, &mode, sizeof mode);
    }
    p.resolved_a5.fill(0);
    p.callback_a5.fill(0);
    p.resource_a5.fill(0);
    p.param_a5.fill(0);
    p.task_a5.fill(0);
    p.arena_a5_0.clear();
    p.arena_a5_1.clear();
    p.registry_a5 = kInitA5cd90RegistryRva;
    p.config_flag_a5 = 0;
    p.arena_tag_a5 = 0;
    p.once_a5_calls = 0;
    p.arena_a5_allocs = 0;
    p.result_a5 = InitA5cd90Result{};
    p.hooks_a5 = InitA5cd90Hooks{};
    p.hooks_a5.object = p.object_a5.data();
    p.hooks_a5.descriptor = p.descriptor_a5.data();
    p.hooks_a5.registry = &p.registry_a5;
    p.hooks_a5.config_flag = &p.config_flag_a5;
    p.hooks_a5.arena_tag = &p.arena_tag_a5;
    p.hooks_a5.call_9854f0 = host_a5_9854f0;
    p.hooks_a5.call_994dd0 = host_a5_994dd0;
    p.hooks_a5.call_994e70 = host_a5_994e70;
    p.hooks_a5.call_991c70 = host_a5_991c70;
    p.hooks_a5.alloc_12820 = host_a5_12820;
    p.hooks_a5.call_992210 = host_a5_992210;
    p.hooks_a5.call_14c2f0 = host_a5_14c2f0;
    p.hooks_a5.resolve_resource = host_a5_resolve;
    p.hooks_a5.call_14ccb0 = host_a5_addref;
    p.hooks_a5.call_14ccc0 = host_a5_release;
    p.hooks_a5.call_14d2d0 = host_a5_destroy;
    p.hooks_a5.call_95580 = host_a5_notify;
    p.hooks_a5.release_f590 = host_a5_release_key;
    p.hooks_a5.once_2ed3668 = host_a5_once;
    p.hooks_a5.call_f730 = host_a5_thread;
    p.hooks_a5.call_14c4a0 = host_a5_mutex;
    p.hooks_a5.call_f8f390 = host_a5_key_create;
    p.hooks_a5.call_f8f3e0 = host_a5_once_end;
    p.hooks_a5.call_14cf20 = host_a5_name_resource;
    p.hooks_a5.call_14d880 = host_a5_current;
    p.hooks_a5.call_14d7b0 = host_a5_param;
    p.hooks_a5.call_9892d0 = host_a5_status;
    p.hooks_a5.call_946590 = host_a5_size;
    p.hooks_a5.call_946650 = host_a5_region;
    p.hooks_a5.call_994070 = host_a5_mode;
    p.hooks_a5.call_f901e0 = host_a5_user_id;
    p.hooks_a5.call_994640 = host_a5_success_log;
    p.hooks_a5.log_condition = host_a5_log_condition;
    p.hooks_a5.call_3e7e0 = host_a5_error_log;
    p.hooks_a5.alloc_17070 = host_a5_alloc_task;
    p.hooks_a5.call_3b5c0 = host_a5_task;
    p.hooks_a5.virtual_release = host_a5_virtual_release;
    p.hooks_a5.canary = host_a5_canary;
    p.hooks_a5.trap_f8f3a0 = host_a5_trap;

    // 0xa5d990: the engine object's two category vectors point at host storage with end == begin
    // and capacity for all 32 slots; the loop fills them.
    {
        std::uint8_t* begin = p.mute_a5d990.data();
        std::uint8_t* cap = begin + p.mute_a5d990.size();
        std::memcpy(p.object_a5.data() + 0x8bd0, &begin, sizeof begin);
        std::memcpy(p.object_a5.data() + 0x8bd8, &begin, sizeof begin);
        std::memcpy(p.object_a5.data() + 0x8be0, &cap, sizeof cap);
        std::uint8_t* filter_begin = p.filter_a5d990.data();
        std::uint8_t* filter_cap = filter_begin + p.filter_a5d990.size();
        std::memcpy(p.object_a5.data() + 0x72a8, &filter_begin, sizeof filter_begin);
        std::memcpy(p.object_a5.data() + 0x72b0, &filter_begin, sizeof filter_begin);
        std::memcpy(p.object_a5.data() + 0x72b8, &filter_cap, sizeof filter_cap);
    }
    p.categories_a5d990.name_alloc.alloc.allocate = key_alloc;
    p.categories_a5d990.name_alloc.alloc.deallocate = key_dealloc;
    p.categories_a5d990.name_dealloc = &g_key_dealloc;
    p.categories_a5d990.grow = hook_a5d990_grow;
    p.menu_a5d990.parent.parent = &p.menu_a5d990;
    p.menu_a5d990.parent.insert = hook_a5d990_menu_insert;
    p.hooks_a5d990.engine = p.object_a5.data();
    p.hooks_a5d990.category_names = p.names_a5d990.data();
    p.hooks_a5d990.registry = &p.registry_a5d990;
    p.hooks_a5d990.categories = &p.categories_a5d990;
    p.hooks_a5d990.menu = &p.menu_a5d990;
    p.hooks_a5d990.atexit = &p.atexit_a5d990;
    p.hooks_a5d990.separator_alloc.alloc.allocate = key_alloc;
    p.hooks_a5d990.separator_alloc.alloc.deallocate = key_dealloc;
    p.hooks_a5d990.destroy_item = hook_a5d990_destroy;
    p.hooks_a5d990.host = &p.result_a5d990;

    p.menu_5b9f90.parent.parent = nullptr;   // [0x2dfd4b8] is null on the startup path
    p.menu_5b9f90.parent.insert = nullptr;
    p.hooks_5b9f90.host = &p.result_5b9f90;
    p.hooks_5b9f90.menu = &p.menu_5b9f90;
    p.hooks_5b9f90.atexit = &p.atexit_5b9f90;
    p.hooks_5b9f90.registry = &p.registry_5b9f90;
    p.hooks_5b9f90.toggle_value_storage = p.toggle_values_5b9f90.data();
    p.hooks_5b9f90.toggle_value_capacity = p.toggle_values_5b9f90.size();
    p.hooks_5b9f90.var_storage = p.vars_5b9f90.data();
    p.hooks_5b9f90.var_capacity = p.vars_5b9f90.size();
    p.hooks_5b9f90.var_value_storage = p.var_values_5b9f90.data();
    p.hooks_5b9f90.var_guard_storage = p.var_guards_5b9f90.data();
    p.hooks_5b9f90.var_guard_capacity = p.var_guards_5b9f90.size();
    p.hooks_5b9f90.canary_cell = &p.canary_5b9f90;
    p.hooks_5b9f90.canary_token = 0x5B9F90CA11ULL;
    p.hooks_5b9f90.item_storage = p.items_5b9f90.data();
    p.hooks_5b9f90.item_capacity = p.items_5b9f90.size();
    p.hooks_5b9f90.release_view = hook_5b9f90_release;
    p.hooks_5b9f90.page_builder = hook_5b9f90_page_builder;

    p.allocation_96.clear();
    p.result_96 = Init96f20Result{};

    p.service_946.fill(0);
    const std::uint32_t service_id_946 = 0x12345678u;
    std::memcpy(p.service_946.data(), &service_id_946, sizeof service_id_946);
    std::memcpy(p.service_946.data() + kInit9469f0ServiceIdOff, &service_id_946,
                sizeof service_id_946);
    p.lock_946 = 0;
    p.record_946_calls = 0;
    p.copy_946_calls = 0;
    p.log_946_calls = 0;
    p.hooks_946 = Init946880Hooks{};
    p.hooks_946.lock = &p.lock_946;
    p.hooks_946.service_id = service_id_946;
    p.hooks_946.query_98a450 = host_946_query;
    p.hooks_946.direct_944de0 = host_946_direct;
    p.hooks_946.log_984700 = host_946_log;
    p.hooks_946.init_hooks.lock_service = host_946_lock;
    p.hooks_946.init_hooks.unlock_service = host_946_unlock;
    p.hooks_946.init_hooks.get_name = host_946_get_name;
    p.hooks_946.init_hooks.get_record = host_946_get_record;
    p.hooks_946.init_hooks.copy_name = host_946_copy_name;
    p.hooks_946.init_hooks.release_a = host_946_release_a;
    p.hooks_946.init_hooks.release_b = host_946_release_b;
    p.hooks_946.init_hooks.log_error = nullptr;

    p.object_a_5d.fill(0);
    p.object_b_5d.fill(0);
    p.state_5d.fill(0);
    // [0x2ed33f0] is read into rdi before every 0x8f520/0x8f1d0 call. Both callees
    // overwrite rdi before any use, so the host supplies a stable token, not a live object.
    p.owner_5d = 0x5d5410c0deULL;
    p.arena_5d.fill(0);
    p.arena_5d_used = 0;
    p.alloc_5d_calls = 0;
    p.tail_5d_calls = 0;
    p.hooks_5d = Init5d5410Hooks{};
    p.hooks_5d.be.tag_2dd9f84 = 0;
    p.hooks_5d.be.tag_15fe920 = 0;
    p.hooks_5d.be.tag_2ed1f54 = 0;
    p.hooks_5d.be.tag_2ed37a4 = 0;

    for (std::size_t i = 0; i < p.source_5c.size(); ++i) {
        p.source_5c[i] = static_cast<std::uint8_t>((i * 37u + 11u) & 0xffu);
    }
    void* source = p.source_5c.data();
    std::memcpy(p.owner_5c.data() + kInit5c64c0SourceOffset, &source, sizeof source);
    p.hooks_5c = Init5c64c0BodyHooks{};
    p.hooks_5c.owner = p.owner_5c.data();
    p.hooks_5c.source_size = p.source_5c.size();
    p.hooks_5c.bss_a = p.bss_a_5c.data();
    p.hooks_5c.bss_b = p.bss_b_5c.data();
    p.hooks_5c.bss_c = p.bss_c_5c.data();
    p.hooks_5c.cell = &p.cell_5c;
    p.hooks_5c.call_5b9f90 = hook_5c_a;
    p.hooks_5c.call_96f20 = hook_5c_96;
    p.hooks_5c.call_5ddcc0 = hook_5c_ret;
    p.hooks_5c.call_5d57f0 = hook_5c_ret;
    p.hooks_5c.call_5d5410 = hook_5d5410;
    p.hooks_5c.call_5d5760 = hook_5c_ret;
    p.hooks_5c.call_5d5700 = hook_5c_ret;
    p.hooks_5c.call_5c91e0 = hook_5c91e0;
    p.hooks_5c.tail_5cf130 = hook_5c_ret;

    // 0x5c91e0 reads 0x2dca510 and writes 0x2ff2b10. The initial source bytes are the pinned
    // .data values at those RVAs; the body itself performs the copy and the four source rewrites.
    static constexpr std::uint8_t kData5c91e0[kInit5c91e0SourceBytes] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x3f, 0x00, 0x00, 0x10, 0x40, 0x14, 0xae,
        0xa7, 0x3f, 0xec, 0x51, 0x78, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x40,
        0x85, 0xeb, 0xb1, 0x3f, 0x71, 0x3d, 0x8a, 0x3f, 0xd7, 0xa3, 0x90, 0x3f, 0x00, 0x00,
        0x80, 0x3f, 0x9a, 0x99, 0x19, 0x40, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f,
        0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x40, 0x01, 0x00,
        0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x9a, 0x99, 0x99, 0x3e, 0x33, 0x33, 0x33, 0x3f,
        0x00, 0x00, 0x20, 0x41, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x41, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x10, 0x41, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00,
        0x80, 0x3f, 0x00, 0x00, 0x20, 0x41};
    std::memcpy(p.data_src_5c91.data(), kData5c91e0, sizeof kData5c91e0);
    p.data_dst_5c91.fill(0);

    p.staffroll_active = false;
    p.staffroll_counts.fill(0);
    p.staffroll_entries.fill(Staffroll7f6f90Entry{});
    p.staffroll_index0 = {{-1, -1}};
    p.staffroll_rest0 = {{-1, -1}};
    p.staffroll_text_bytes.fill(0);
    p.staffroll_used_fopen.fill(false);
    p.staffroll_blocked = nullptr;
    g_staffroll_slots.fill(std::string{});
    g_staffroll_texts.clear();
    p.query_global_5d.active = &p.staffroll_active;
    p.hooks_7f = Staffroll7f7220Hooks{};
    p.hooks_7f.query_5d06f0 = hook_7f_query;
    p.hooks_7f.set_7f6f90 = hook_7f_set;
}

AppmainInitCtor98a90SliceResult run_full_slice(Boot& boot) {
    Prologue255180Hooks eh{};
    eh.ctor_a30_264e50 = ctor_a30;
    Prologue255180TailHooks th{};
    Ctor98a90_3be050Hooks ch{};
    BaseInitialize255130Hooks bh{};
    ResourcePool pool{&g_key_dealloc};
    install_attach(bh.vslot_40_shadow_attach_detail.common, pool);
    install_attach(bh.vslot_40_skels_attach_detail.common, pool);
    install_attach(bh.vslot_40_anims_attach_detail.common, pool);
    install_attach(bh.vslot_40_banks_attach_detail.common, pool);
    install_attach(bh.vslot_40_streams_attach_detail.common, pool);
    install_attach(bh.vslot_40_randomevents_attach_detail.common, pool);
    install_attach(bh.vslot_40_bossmusic_attach_detail.common, pool);
    install_attach(bh.vslot_40_cp3characters_attach_detail.common, pool);
    install_attach(bh.vslot_40_cp3shadowprecache_attach_detail.common, pool);
    bh.vslot_40_tail_140d90_detail.pool_hooks = bh.vslot_40_shadow_attach_detail.common.pool_hooks;
    bh.vslot_40_tail_140d90_detail.slot = reinterpret_cast<LockedResourceRef*>(
        static_cast<std::uint8_t*>(boot.self) + kVslot40Tail140d90SlotOff);

    std::array<std::uint8_t, 16> avx_s{}, avx_d0{}, avx_d1{};
    for (std::size_t i = 0; i < avx_s.size(); ++i) avx_s[i] = static_cast<std::uint8_t>(0xA0 + i);
    Init531d60PostBaseAvxBssCells avx{avx_s.data(), avx_d0.data(), avx_d1.data()};

    std::array<std::uint8_t, 32> ymm_s{};
    std::array<std::uint8_t, 0x30> ymm_4278{}, ymm_42dc{}, ymm_a8{}, ymm_430c{};
    std::array<std::uint8_t, 12> ymm_426c{};
    std::array<std::uint8_t, 4> ymm_4268{};
    Init531d60PostBaseYmmBssCells ymm{ymm_s.data(), ymm_4278.data(), ymm_426c.data(),
                                       ymm_42dc.data(), ymm_4268.data()};
    Init531d60PostBaseYmmIntercellCells yic{ymm_4278.data(), ymm_42dc.data(), ymm_a8.data(),
                                             ymm_430c.data()};
    std::array<std::uint8_t, 16> xmm_s{};
    std::array<std::uint8_t, 4> xmm_42d8{};
    std::array<std::uint8_t, 0x24> xmm_430c{};
    Init531d60PostBaseXmmSeedCells xmm{xmm_s.data(), xmm_42d8.data(), xmm_430c.data()};
    Init531d60PostBaseYmmReloadCells yr{ymm_430c.data(), ymm_430c.data()};

    std::array<std::uint8_t, 8> lv_src{};
    std::array<std::uint8_t, 16> lv_xmm{};
    std::array<void*, 1> lv_rbx_cell{nullptr};
    void* lv_rbx = nullptr;
    Init531d60PostBaseLeaVmovqCells lv{lv_src.data(), lv_xmm.data(), lv_rbx_cell.data(), &lv_rbx};

    std::array<std::uint8_t, 0x460> o450{};
    std::array<std::uint8_t, 0x490> o448{};
    void* cell450 = nullptr;
    void* cell448 = nullptr;
    Init531d60PostBaseCell2eff450Cells c450{o450.data(), &cell450};
    Init531d60PostBaseCell2eff448Cells c448{o448.data(), &cell448};
    std::array<void*, 1> r13_cell{nullptr};
    void* r13 = nullptr;
    Init531d60PostBaseLea32f4fe0Cells lea32{r13_cell.data(), &r13};

    void* cell32f84f8 = nullptr;
    std::array<std::uint8_t, 0x30> contA_obj{}, contA_rsp{};
    std::array<std::uint8_t, 0x30> contC_a{}, contC_b{};
    std::uint8_t flag1 = 0, flag2 = 1, boss_flag = 0xCC;
    void* contD_cell = nullptr;
    Init531d60PostBaseContinueDHooks cd{};
    cd.cell_32f79a8 = &contD_cell;
    cd.capture_services = &boot.capture;

    Init531d60PostBaseContinueEHooks ce{};
    ce.incoming_r13 = boot.savedata.parent.data();
    ce.boss_flag_2dcdf14 = &boss_flag;
    ce.call_a54920 = real_a54920;
    ce.cell_2eff450 = &cell450;
    ce.stat_engine = &boot.stats_contract;

    // Execute the lifted 0x499f80 body (193 B) instead of the call-site-only no-op. The body
    // receives self+0x3c70 and host-owned global cells; its allocations and CritSec are real.
    Init531d60PostBase499f80Hooks ph499{};
    ph499.body_globals = &boot.init49980.globals;
    ph499.body_hooks = boot.init49980.hooks;

    Init531d60PostBase56dcb0Hooks p56{};
    p56.call_56dcb0 = run_56_body;
    Init531d60PostBase5c64c0Hooks p5c{};
    p5c.call_5c64c0 = run_5c_body;
    Init531d60PostBase7f7220Hooks p7f{};
    p7f.call_7f7220 = run_7f_body;
    Init531d60PostBase56db70Hooks p56db{};
    p56db.call_56db70 = native_ret_56db70_body;
    Init531d60PostBase5b3d00Hooks p5b3d{};
    p5b3d.call_5b3d00 = native_ret_5b3d00_body;
    Init531d60PostBase5b3cf0Hooks p5b3c{};
    p5b3c.call_5b3cf0 = native_ret_5b3cf0_body;
    Init531d60PostBase56fad0Hooks p56fa{};
    p56fa.call_56fad0 = native_ret_56fad0_body;
    Init531d60PostBaseF90310Hooks pf9{};
    pf9.call_f90310 = host_f90310_splash_hide;

    Init531d60PostBaseContinueBHooks cb{};
    cb.cell_32f84f8 = &cell32f84f8;
    cb.call_1e4c0 = hook_1e4c0;
    cb.call_4c4950 = hook_4c4950;

    Init531d60PostBase1e4c0U1Hooks pb1e4{};
    pb1e4.call_1e4c0 = hook_1e4c0;
    Init531d60PostBase4c4950U1Hooks pb4c{};
    pb4c.call_4c4950 = hook_4c4950;

    Init531d60PostBaseContinueCHooks cc{};
    cc.incoming_r15 = contA_obj.data();
    void* cell_a_obj = contC_a.data();
    void* cell_b_obj = contC_b.data();
    void* cell_c_obj = contA_rsp.data();
    void* cell_d_obj = contA_rsp.data() + 0x10;
    cc.cell_32f8b80 = &cell_a_obj;
    cc.cell_32f6d18 = &cell_b_obj;
    cc.cell_2efe318 = &cell_c_obj;
    cc.cell_2efe340 = &cell_d_obj;
    cc.flag_byte_iter1 = &flag1;
    cc.flag_byte_iter2 = &flag2;

    std::array<std::uint8_t, 0x200> reg_stack{};
    Init531d60PostBaseRegistryBodyHooks rb{};
    rb.stack_rsp = reg_stack.data();

    return run_appmain_init_ctor_98a90_slice(
        *boot.table, *boot.alloc, boot.self, nullptr, eh, th, ch, bh, boot.setter, 0, 0x11, 0x22,
        0x33, /*singleton*/ {}, /*499f80*/ ph499, /*56dcb0*/ p56, /*5c64c0*/ p5c,
        /*1290*/ {}, /*56db70*/ p56db, /*5b3d00*/ p5b3d, /*5b3cf0*/ p5b3c,
        /*avx*/ avx, /*7f*/ p7f, /*56fa*/ p56fa,
        /*f9*/ pf9, /*ymm*/ ymm, /*ic*/ yic, /*xmm*/ xmm, /*yr*/ yr, /*26800*/ {}, /*sbj*/ {},
        /*sboj*/ {}, /*52*/ {}, /*lea*/ lv, /*532ca0*/ {}, /*v18*/ {}, /*532ca0u2*/ {},
        /*v18u2*/ {}, /*532ca0u3*/ {}, /*v18u3*/ {}, /*532ca0u4*/ {}, /*v18u4*/ {},
        /*532ca0u5*/ {}, /*v18u5*/ {}, /*532ed0*/ {}, /*v18ed*/ {}, /*532ed0u2*/ {},
        /*v18edu2*/ {}, /*17070*/ {}, /*544*/ {}, /*c450*/ c450, /*c450h*/ {}, /*17070u2*/ {},
        /*53fd10*/ {}, /*c448*/ c448, /*c448h*/ {}, /*v38*/ {}, /*lea32*/ lea32,
        /*a2caf0*/ {}, /*a2ed00*/ {}, /*f590a*/ {}, /*f590au2*/ {}, /*a2ed00u2*/ {},
        /*f590au3*/ {}, /*f590au4*/ {}, /*a2ed00u3*/ {}, /*f590au5*/ {}, /*f590au6*/ {},
        /*a2ed00u4*/ {}, /*f590au7*/ {}, /*f590au8*/ {}, /*a2ed00u5*/ {}, /*f590au9*/ {},
        /*f590au10*/ {}, /*a2ed00u6*/ {}, /*f590au11*/ {}, /*f590au12*/ {}, /*a2ed00u7*/ {},
        /*f590au13*/ {}, /*f590au14*/ {}, /*a2ea*/ {}, /*a2e5*/ {}, /*a543*/ {}, /*1fc80*/ {},
        /*1e4c0*/ pb1e4, /*4c4950*/ pb4c, /*a52b70*/ {}, /*a599e0*/ {}, /*a431e0*/ {},
        /*contA*/ {}, /*contB*/ cb, /*contC*/ cc, /*contD*/ cd, /*contE*/ ce, /*regTail*/ {},
        /*regBody*/ rb, /*alt*/ {}, /*init1290*/ boot.post.hooks_1290);
}

bool entry_base_callback(void* self, void* arg) {
    (void)self;
    (void)arg;
    g_boot->full = run_full_slice(*g_boot);
    return g_boot->full.base_ok;
}

// Host teardown of the committed object returned by the lifted 0x499f80 body. The body transfers
// ownership to its caller at the 0x49a033 store; this releases it after the observed values are
// copied into the runtime result. It is not a retail effect and is not counted as one.
void release_init_499f80_committed(Boot& boot) noexcept {
    auto& body = boot.full.post_base_499f80.body;
    if (!body.committed) return;
    const AllocDispatchContract alloc = host_alloc_contract();
    if (body.buffer_b != nullptr) (void)free_170c0(body.buffer_b, alloc);
    if (body.buffer_a != nullptr) (void)free_170c0(body.buffer_a, alloc);
    if (body.critsec_live) (void)critsec_destroy_at(body.new_object);
    if (body.object_live) (void)free_170b0(body.new_object, alloc);
    body.buffer_a = nullptr;
    body.buffer_b = nullptr;
    body.critsec_live = false;
    body.object_live = false;
    if (boot.init49980.globals.store_cell != nullptr) {
        *boot.init49980.globals.store_cell = nullptr;
    }
    if (boot.init49980.globals.old_object_cell != nullptr) {
        *boot.init49980.globals.old_object_cell = nullptr;
    }
}

const char* first_failure(const InitWindowRuntimeResult& r) {
    if (!r.fixture_loaded) return "fixture";
    if (r.entry_ret_nop_calls != 1) return "post_56dca0";
    if (!r.entry_ok) return "entry_0x531d60";
    if (!r.early_ok) return "early_0x255180";
    if (!r.tail_ok) return "tail_0x255180";
    if (!r.ctor_ok) return "ctor_0x3be050";
    if (!r.base_ok) return "base_0x255130";
    if (!r.body_499f80_ok) return "body_0x499f80";
    if (!r.post_56dcb0_ok) return "post_56dcb0";
    if (!r.post_5c64c0_ok) return "post_5c64c0";
    if (!r.post_1290_ok) return "post_1290";
    if (!r.post_56db70_ok) return "post_56db70";
    if (!r.post_5b3d00_ok) return "post_5b3d00";
    if (!r.post_5b3cf0_ok) return "post_5b3cf0";
    if (!r.post_7f7220_ok) return "post_7f7220";
    if (!r.post_56fad0_ok) return "post_56fad0";
    if (!r.post_f90310_ok) return "post_f90310";
    if (!r.post_base_ok) return "post_base_0x531ddf";
    if (!r.capture_body_ok) return "capture_0xa473b0";
    if (!r.savedata_body_ok) return "savedata_0xa54920";
    if (!r.stat_query_body_ok) return "stat_query_0x54b250";
    if (!r.continue_e_ok) return "continue_e_0x5329ae";
    if (!r.registry_tail_ok) return "registry_tail_0x532b0c";
    if (!r.registry_body_ok) return "registry_body_0x532b19";
    if (!r.remainder_ok) return "remainder";
    if (r.registry_entries != kInputTable551140Count ||
        r.registry_status != static_cast<std::uint32_t>(RegistryInit531d60Status::ok))
        return "registry_state";
    return "";
}

void append_post_calls(InitWindowRuntimeResult& out, const InitPostCallLog& log) noexcept {
    for (std::size_t i = 0;
         i < log.count && static_cast<std::size_t>(out.post_call_count) < kInitPostCallMax; ++i) {
        out.post_calls[out.post_call_count++] = log.records[i];
        if (log.records[i].executed) {
            ++out.post_calls_executed;
        } else {
            ++out.post_calls_blocked;
        }
    }
}

}  // namespace

InitWindowRuntimeResult run_init_window_runtime(const char* fixture_path) noexcept {
    InitWindowRuntimeResult out{};
    if (fixture_path != nullptr && fixture_path[0] != '\0') {
        ::setenv(kInputtableNamesFixtureEnv, fixture_path, 1);
    }

    InputTable551140 table{};
    init_input_table_551140(table);
    InputtableNamesFixture fixture{};
    if (!load_inputtable_names_fixture_default(fixture) ||
        !apply_inputtable_names_fixture(table, fixture)) {
        out.first_failed_stage = first_failure(out);
        return out;
    }
    out.fixture_loaded = true;

    AllocatorFront alloc;
    RuntimeHeap heap(alloc.owners());
    if (!alloc.set_heap(1, &heap) || !alloc.set_heap(2, &heap)) {
        out.first_failed_stage = "allocator_heap";
        return out;
    }

    std::vector<std::uint8_t> app(0x5000, 0);
    std::array<std::uint8_t, 0x60> setter{};
    Boot boot{};
    boot.table = &table;
    boot.alloc = &alloc;
    boot.self = app.data();
    boot.setter = setter.data();

    boot.savedata.init();
    boot.init49980.configure(host_alloc_contract());
    configure_post(boot.post);
    g_savedata = &boot.savedata;
    g_stats_engine = &boot.engine;
    g_boot = &boot;
    const InitEntryGapsHooks entry_hooks{entry_base_callback, entry_ret_nop};
    const InitEntryGapsResult entry = init_531d60_entry_gaps(app.data(), entry_hooks);
    g_boot = nullptr;
    g_savedata = nullptr;
    g_stats_engine = nullptr;

    const auto& full = boot.full;
    out.entry_ok = entry.ok;
    out.entry_units = entry.units_executed;
    out.entry_base_calls = entry.base_call_count;
    out.entry_ret_nop_calls = entry.ret_nop_count;
    out.early_ok = full.early.ok;
    out.tail_ok = full.tail.ok;
    out.ctor_ok = full.ctor.ok;
    out.base_ok = full.base_ok;
    out.post_base_ok = full.post_base_499f80.ok && full.post_base_56dcb0.ok &&
                       full.post_base_5c64c0.ok && full.post_base_avx_bss.ok &&
                       full.post_base_7f7220.ok && full.post_base_56fad0.ok &&
                       full.post_base_f90310.ok && full.post_base_ymm_bss.ok &&
                       full.post_base_ymm_intercell.ok && full.post_base_xmm_seed.ok &&
                       full.post_base_ymm_reload.ok && full.post_base_26800_f590.ok &&
                       full.post_base_sbj.ok && full.post_base_sboj.ok && full.post_base_52eb50.ok &&
                       full.post_base_lea_vmovq.ok && full.post_base_532ca0.ok &&
                       full.post_base_vcall18.ok && full.post_base_1fc80_u1.ok &&
                       full.post_base_continue_a.ok && full.post_base_continue_b.ok &&
                       full.post_base_continue_c.ok && full.post_base_continue_d.ok;
    out.continue_e_ok = full.post_base_continue_e.ok;
    out.registry_tail_ok = full.post_base_registry_tail.ok;
    out.registry_body_ok = full.post_base_registry_body.ok;
    out.remainder_ok = full.remainder.trivial_ok;
    out.registry_body_units = full.post_base_registry_body.unit_executed;
    out.registry_entries = full.remainder.registry.entry_count;
    out.registry_status = static_cast<std::uint32_t>(full.remainder.registry.status);
    out.boss_flag_after = full.post_base_continue_e.boss_flag_after;
    const auto& body499 = full.post_base_499f80.body;
    out.body_499f80_calls = full.post_base_499f80.call_499f80_count;
    out.body_499f80_ok = full.post_base_499f80.body_native && body499.ok;
    out.body_499f80_committed = body499.committed;
    out.body_499f80_object_live = body499.object_live;
    out.body_499f80_buffer_a = body499.buffer_a != nullptr;
    out.body_499f80_buffer_b = body499.buffer_b != nullptr;
    out.body_499f80_cleanup_failures = static_cast<int>(body499.cleanup_failures);
    out.body_499f80_stage = body499.blocked != nullptr ? body499.blocked : "";

    const auto& r1290 = full.init_1290;
    out.post_1290_ok = full.init_1290_native && r1290.ok && r1290.insert_a_executed &&
                       r1290.insert_b_executed && r1290.vcall_count == 1;
    out.post_1290_a = r1290.insert_a_executed;
    out.post_1290_b = r1290.insert_b_executed;
    out.post_1290_a_count = static_cast<int>(r1290.inserted_a.count);
    out.post_1290_b_count = static_cast<int>(r1290.inserted_b.count);
    out.post_1290_vcall = r1290.vcall_count;

    const auto& r56 = boot.post.result_56;
    out.post_56dcb0_ok = r56.ok && full.post_base_56dcb0.call_56dcb0_count == 1;
    out.post_56dcb0_calls = r56.calls_a5cd90 + r56.calls_946880 + r56.calls_a5d990 +
                            r56.calls_560b60;
    out.post_56dcb0_blocked = r56.calls_blocked;
    out.post_56dcb0_a5cd90 = r56.calls_a5cd90;
    out.post_56dcb0_946880 = r56.calls_946880;
    out.post_56dcb0_a5d990 = r56.calls_a5d990;
    out.post_56dcb0_560b60 = r56.calls_560b60;
    out.post_56dcb0_desc0 = r56.descriptor_word0;
    out.post_56dcb0_desc1 = r56.descriptor_word1;
    const auto& r946 = boot.post.result_946;
    out.post_946880_ok = r946.ok;
    out.post_946880_status = r946.status;
    out.post_946880_took_9469f0 = r946.took_9469f0;
    out.post_946880_inner_calls = static_cast<int>(r946.inner.call_log.count);
    out.post_946880_inner_ok = r946.inner.ok;
    out.post_946880_lock = boot.post.lock_946;

    const auto& r5d = boot.post.result_5d;
    out.post_5d5410_ok = r5d.ok;
    out.post_5d5410_8be60 = r5d.calls_8be60;
    out.post_5d5410_8f520 = r5d.calls_8f520;
    out.post_5d5410_8f1d0 = r5d.calls_8f1d0;
    out.post_5d5410_blocked = r5d.blocked_calls;
    out.post_5d5410_blocked_reason = r5d.blocked != nullptr ? r5d.blocked : "";
    out.post_5d5410_owner = r5d.owner;
    out.post_5d5410_obj_a_04 = r5d.inner_a.object_word4;
    out.post_5d5410_obj_a_0c = r5d.inner_a.object_word_c;
    out.post_5d5410_obj_b_04 = r5d.inner_b.object_word4;
    out.post_5d5410_obj_b_0c = r5d.inner_b.object_word_c;
    for (std::size_t i = 0; i < 9; ++i) {
        std::memcpy(&out.post_5d5410_state[i], r5d.state.data() + i * 8, sizeof(std::uint64_t));
    }

    const auto& r5b = boot.post.result_5b9f90;
    out.post_5b9f90_ok = r5b.ok;
    out.post_5b9f90_slices = r5b.executed_slices;
    out.post_5b9f90_groups = r5b.groups;
    out.post_5b9f90_commands = r5b.commands;
    out.post_5b9f90_toggles = r5b.toggles;
    out.post_5b9f90_floats = r5b.float_items;
    out.post_5b9f90_named_vars = r5b.named_vars;
    out.post_5b9f90_registered = r5b.registered;
    out.post_5b9f90_releases = r5b.releases;
    out.post_5b9f90_destroys = r5b.destroys;
    out.post_5b9f90_contracts = r5b.contracts;
    out.post_5b9f90_resume_rva = r5b.resume_rva;
    out.post_5b9f90_complete = r5b.complete;

    const auto& r5c = boot.post.result_5c;
    out.post_5c64c0_ok = r5c.ok && full.post_base_5c64c0.call_5c64c0_count == 1;
    out.post_5c64c0_bss_a_hash = r5c.bss_a_hash;
    out.post_5c64c0_bss_b_hash = r5c.bss_b_hash;
    out.post_5c64c0_bss_c_hash = r5c.bss_c_hash;
    out.post_5c64c0_cell = r5c.cell_value;
    out.post_5c64c0_calls = r5c.calls_5b9f90 + r5c.calls_96f20 + r5c.calls_after +
                            (r5c.tail_called ? 1 : 0);
    out.post_5c64c0_blocked = r5c.calls_blocked;
    out.post_5c64c0_tail = r5c.tail_called;

    const auto& r96 = boot.post.result_96;
    out.post_96f20_ok = r96.ok;
    out.post_96f20_value = r96.return_value;
    out.post_96f20_alloc_bytes = boot.post.allocation_96.size();
    const auto& r560 = boot.post.result_560;
    out.post_560b60_ok = r560.ok;
    out.post_560b60_calls = r560.calls_total;
    out.post_560b60_alloc_calls = r560.alloc_calls;
    out.post_560b60_object = reinterpret_cast<std::uintptr_t>(r560.object);
    out.post_560b60_buffer0 = reinterpret_cast<std::uintptr_t>(r560.buffer_2000);
    out.post_560b60_buffer1 = reinterpret_cast<std::uintptr_t>(r560.buffer_0400);
    out.post_560b60_buffer2 = reinterpret_cast<std::uintptr_t>(r560.buffer_0200);
    out.post_560b60_previous_cleanup = r560.previous_cleanup;
    out.post_560b60_global28 = r560.global_28;
    const auto& ra5 = boot.post.result_a5;
    out.post_a5cd90_ok = ra5.ok;
    out.post_a5cd90_calls = ra5.calls_total;
    out.post_a5cd90_allocs = ra5.alloc_calls;
    out.post_a5cd90_field69c8 = ra5.field_69c8;
    out.post_a5cd90_field08 = ra5.field_08;
    out.post_a5cd90_resource = reinterpret_cast<std::uintptr_t>(ra5.resource);
    out.post_a5cd90_ready = ra5.resource_ready;
    out.post_a5cd90_flag18 = ra5.flag_18;
    out.post_a5cd90_flag20 = ra5.flag_20;
    out.post_a5cd90_object_a4f8 = ra5.object_a4f8;
    out.post_a5cd90_byte0 = ra5.byte0;
    out.post_a5cd90_once = ra5.once_calls;
    out.post_a5cd90_user_id = ra5.user_id_check;
    out.post_a5cd90_log = ra5.log_calls;
    out.post_a5cd90_trap = ra5.trap_calls;

    const auto& r7f = boot.post.result_7f;
    out.post_7f7220_ok = r7f.ok && full.post_base_7f7220.call_7f7220_count == 1;
    out.post_7f7220_branch = r7f.query_result;
    out.post_7f7220_sets = r7f.set_calls;
    out.post_7f7220_blocked = r7f.calls_blocked;
    out.post_7f7220_mode0 = r7f.modes[0];
    out.post_7f7220_str0 = r7f.strings[0];
    out.post_7f7220_mode1 = r7f.modes[1];
    out.post_7f7220_str1 = r7f.strings[1];
    out.post_7f6f90_entries[0] = boot.post.staffroll_counts[0];
    out.post_7f6f90_index0[0] = boot.post.staffroll_index0[0];
    out.post_7f6f90_rest0[0] = boot.post.staffroll_rest0[0];
    out.post_7f6f90_bytes[0] = boot.post.staffroll_text_bytes[0];
    out.post_7f6f90_fopen[0] = boot.post.staffroll_used_fopen[0];
    out.post_7f6f90_entries[1] = boot.post.staffroll_counts[1];
    out.post_7f6f90_index0[1] = boot.post.staffroll_index0[1];
    out.post_7f6f90_rest0[1] = boot.post.staffroll_rest0[1];
    out.post_7f6f90_bytes[1] = boot.post.staffroll_text_bytes[1];
    out.post_7f6f90_fopen[1] = boot.post.staffroll_used_fopen[1];
    out.post_7f6f90_blocked = boot.post.staffroll_blocked != nullptr ? 1 : 0;
    out.post_7f6f90_ok = out.post_7f7220_ok && r7f.set_calls == 2 &&
                         boot.post.staffroll_blocked == nullptr;

    append_post_calls(out, r56.call_log);
    append_post_calls(out, r5c.call_log);
    append_post_calls(out, r7f.call_log);

    const auto& s56db = full.post_base_56db70;
    out.post_56db70_ok = s56db.ok && s56db.call_56db70_count == 1 &&
                         boot.post.ret_56db70_calls == 1;
    out.post_56db70_calls = boot.post.ret_56db70_calls;
    const auto& s5b3d = full.post_base_5b3d00;
    out.post_5b3d00_ok = s5b3d.ok && s5b3d.call_5b3d00_count == 1 &&
                         boot.post.ret_5b3d00_calls == 1;
    out.post_5b3d00_calls = boot.post.ret_5b3d00_calls;
    const auto& s5b3c = full.post_base_5b3cf0;
    out.post_5b3cf0_ok = s5b3c.ok && s5b3c.call_5b3cf0_count == 1 &&
                         boot.post.ret_5b3cf0_calls == 1;
    out.post_5b3cf0_calls = boot.post.ret_5b3cf0_calls;
    const auto& s56fa = full.post_base_56fad0;
    out.post_56fad0_ok = s56fa.ok && s56fa.call_56fad0_count == 1 &&
                         boot.post.ret_56fad0_calls == 1;
    out.post_56fad0_calls = boot.post.ret_56fad0_calls;
    out.post_f90310_ok = full.post_base_f90310.ok &&
                         full.post_base_f90310.call_f90310_count == 1 &&
                         boot.post.f90310_calls == 1;
    out.post_f90310_calls = boot.post.f90310_calls;

    out.post_chain_complete = out.entry_ret_nop_calls == 1 && out.post_1290_ok &&
                              out.post_56dcb0_ok && out.post_5c64c0_ok &&
                              out.post_7f7220_ok && out.post_56db70_ok &&
                              out.post_5b3d00_ok && out.post_5b3cf0_ok &&
                              out.post_56fad0_ok && out.post_f90310_ok;
    if (out.entry_ret_nop_calls != 1) {
        out.post_chain_blocked = "0x56dca0";
    } else if (!out.post_56dcb0_ok) {
        out.post_chain_blocked = r56.blocked != nullptr ? r56.blocked : "0x56dcb0";
    } else if (!out.post_5c64c0_ok) {
        out.post_chain_blocked = r5c.blocked != nullptr ? r5c.blocked : "0x5c64c0";
    } else if (!out.post_1290_ok) {
        out.post_chain_blocked = "0x1290";
    } else if (!out.post_56db70_ok) {
        out.post_chain_blocked = "0x56db70";
    } else if (!out.post_5b3d00_ok) {
        out.post_chain_blocked = "0x5b3d00";
    } else if (!out.post_5b3cf0_ok) {
        out.post_chain_blocked = "0x5b3cf0";
    } else if (!out.post_7f7220_ok) {
        out.post_chain_blocked = r7f.blocked != nullptr ? r7f.blocked : "0x7f7220";
    } else if (!out.post_56fad0_ok) {
        out.post_chain_blocked = "0x56fad0";
    } else if (!out.post_f90310_ok) {
        out.post_chain_blocked = "0xf90310";
    }

    out.capture_body_calls = full.post_base_continue_d.call_count;
    out.capture_body_ok = full.post_base_continue_d.capture.ok;
    out.capture_step = full.post_base_continue_d.capture.step;
    out.capture_video_0d = full.post_base_continue_d.capture.video_value_0d;
    out.capture_video_a007 = full.post_base_continue_d.capture.video_value_a007;
    out.savedata_body_calls = boot.savedata.calls;
    out.savedata_body_ok = boot.savedata.ok && boot.savedata.calls == 3;
    out.savedata_refusal = boot.savedata.first_refusal;
    for (const auto& result : boot.savedata.results) {
        if (!result) continue;
        for (std::size_t i = 0; i < kSavedataStatsCounterCount; ++i) {
            if (result->registered[i]) ++out.savedata_counters;
            if (result->item_registered[i]) ++out.savedata_items;
        }
    }
    out.stat_query_body_calls = full.post_base_continue_e.stat_query.engine_calls;
    out.stat_query_body_ok = full.post_base_continue_e.stat_query_ran &&
                             full.post_base_continue_e.stat_query.ok;
    out.stat_engine_calls = boot.engine.calls;
    out.stat_engine_arg2 = boot.engine.arg2;
    out.ok = out.entry_ok && out.entry_ret_nop_calls == 1 && out.early_ok && out.tail_ok &&
             out.ctor_ok && out.base_ok &&
             out.body_499f80_ok && out.post_1290_ok && out.post_56dcb0_ok && out.post_5c64c0_ok &&
             out.post_5d5410_ok && out.post_7f7220_ok && out.post_chain_complete &&
             out.post_base_ok &&
             out.continue_e_ok && out.registry_tail_ok && out.registry_body_ok && out.remainder_ok &&
             out.capture_body_ok && out.savedata_body_ok && out.stat_query_body_ok &&
             out.registry_entries == kInputTable551140Count &&
             out.registry_status == static_cast<std::uint32_t>(RegistryInit531d60Status::ok);
    if (!out.ok) out.first_failed_stage = first_failure(out);
    release_init_499f80_committed(boot);
    return out;
}

}  // namespace sotc
