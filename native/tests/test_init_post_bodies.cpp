// Fixtures for the native 0x56dcb0, 0x5c64c0 and 0x7f7220 post-base Init bodies.
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "core/init_5d5410_body.hpp"
#include "core/init_560b60_body.hpp"
#include "core/init_a5cd90_body.hpp"
#include "core/init_7f6f90_body.hpp"
#include "core/init_946880_body.hpp"
#include "core/init_96f20_body.hpp"
#include "core/init_post_bodies.hpp"

#define CHECK(c)                                                                                   \
    do {                                                                                           \
        if (!(c)) {                                                                                \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);                       \
            std::exit(1);                                                                          \
        }                                                                                          \
    } while (0)

namespace {

struct Rec56 {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;
    std::uint64_t desc0 = 0;
    std::uint64_t desc1 = 0;
    int args[2] = {0, 0};
    void* object = nullptr;
    void* data = nullptr;
};
Rec56 g56;

sotc::InitPostCallOutcome executed_outcome(const char* detail = "fixture body executed") {
    sotc::InitPostCallOutcome o{};
    o.executed = true;
    std::snprintf(o.detail, sizeof o.detail, "%s", detail);
    return o;
}

sotc::InitPostCallOutcome blocked_outcome(const char* blocker, const char* detail) {
    sotc::InitPostCallOutcome o{};
    o.blocker = blocker;
    std::snprintf(o.detail, sizeof o.detail, "%s", detail);
    return o;
}

sotc::InitPostCallOutcome h56_a(void* object, const void* descriptor) {
    ++g56.a;
    g56.object = object;
    std::memcpy(&g56.desc0, descriptor, 8);
    std::memcpy(&g56.desc1, static_cast<const std::uint8_t*>(descriptor) + 8, 8);
    return executed_outcome("a5cd90 fixture");
}
sotc::InitPostCallOutcome h56_946880(int value) {
    if (g56.b < 2) g56.args[g56.b] = value;
    ++g56.b;
    return executed_outcome("946880 fixture");
}
sotc::InitPostCallOutcome h56_c(void* object, void* data) {
    ++g56.c;
    g56.object = object;
    g56.data = data;
    return executed_outcome("a5d990 fixture");
}
sotc::InitPostCallOutcome h56_d() {
    ++g56.d;
    return executed_outcome("560b60 fixture");
}

void test_56dcb0() {
    g56 = Rec56{};
    int object = 0;
    int data = 0;
    sotc::Init56dcb0BodyHooks hooks{};
    hooks.object = &object;
    hooks.data_cell = &data;
    hooks.call_a5cd90 = h56_a;
    hooks.call_946880 = h56_946880;
    hooks.call_a5d990 = h56_c;
    hooks.call_560b60 = h56_d;
    const auto r = sotc::run_init_56dcb0_body(hooks);
    CHECK(r.ok);
    CHECK(std::strcmp(r.blocked == nullptr ? "" : r.blocked, "") == 0);
    CHECK(r.calls_a5cd90 == 1 && r.calls_946880 == 2 && r.calls_a5d990 == 1 && r.calls_560b60 == 1);
    CHECK(g56.a == 1 && g56.b == 2 && g56.c == 1 && g56.d == 1);
    CHECK(g56.object == &object && g56.data == &data);
    CHECK(g56.desc0 == 0x101e7ac && g56.desc1 == 0x030a);
    CHECK(g56.args[0] == 1 && g56.args[1] == 0);
    CHECK(r.arg_946880[0] == 1 && r.arg_946880[1] == 0);
    CHECK(r.call_log.count == 5);
    CHECK(r.call_log.records[0].callee_rva == 0xa5cd90);
    CHECK(r.call_log.records[1].callee_rva == 0x946880 && r.call_log.records[1].args[0] == 1);
    CHECK(r.call_log.records[2].callee_rva == 0x946880 && r.call_log.records[2].args[0] == 0);
    CHECK(r.call_log.records[3].callee_rva == 0xa5d990);
    CHECK(r.call_log.records[4].callee_rva == 0x560b60);
    for (std::size_t i = 0; i < r.call_log.count; ++i) CHECK(r.call_log.records[i].executed);

    auto bad = hooks;
    bad.object = nullptr;
    const auto blocked = sotc::run_init_56dcb0_body(bad);
    CHECK(!blocked.ok);
    CHECK(std::strcmp(blocked.blocked, "object_missing") == 0);

    bad = hooks;
    bad.call_a5cd90 = [](void*, const void*) {
        return blocked_outcome("fixture_blocker", "observed refusal");
    };
    const auto partial = sotc::run_init_56dcb0_body(bad);
    CHECK(partial.ok && partial.calls_blocked == 1);
    CHECK(!partial.call_log.records[0].executed);
    CHECK(std::strcmp(partial.call_log.records[0].blocker, "fixture_blocker") == 0);
}

struct Rec7f {
    bool query = true;
    int calls = 0;
    int modes[2] = {0, 0};
    std::uintptr_t strings[2] = {0, 0};
};
Rec7f g7f;

sotc::InitPostCallOutcome h7f_query() {
    auto o = executed_outcome("query fixture");
    o.has_value = true;
    o.value = g7f.query ? 1 : 0;
    return o;
}
sotc::InitPostCallOutcome h7f_set(int mode, std::uintptr_t string_rva) {
    if (g7f.calls < 2) {
        g7f.modes[g7f.calls] = mode;
        g7f.strings[g7f.calls] = string_rva;
    }
    ++g7f.calls;
    return executed_outcome("set fixture");
}

struct Rec7f6 {
    std::array<std::string, 16> slots{};
};
Rec7f6 g7f6;

const char* h7f6_text(const char* path) {
    if (std::strstr(path, "remaster") != nullptr) return "CENTER_FULL_SCREEN,";
    if (std::strstr(path, "original") != nullptr) return "TARGET,";
    if (std::strstr(path, "missing") != nullptr) return "CENTER";
    if (std::strstr(path, "unknown") != nullptr) return "NOPE,";
    if (std::strstr(path, "bom") != nullptr) return "\xef\xbb\xbfTARGET,\r\n";
    return "STYLE,";
}

sotc::InitPostCallOutcome h7f6_load(const char* path, std::int64_t, int* slot) {
    if (path == nullptr || slot == nullptr) return blocked_outcome("args", "load args");
    for (std::size_t i = 0; i < g7f6.slots.size(); ++i) {
        if (!g7f6.slots[i].empty()) continue;
        g7f6.slots[i] = h7f6_text(path);
        *slot = static_cast<int>(i);
        return executed_outcome("fixture load");
    }
    return blocked_outcome("slots", "no slot");
}

sotc::InitPostCallOutcome h7f6_size(int slot, std::size_t* size) {
    if (size == nullptr || slot < 0 || static_cast<std::size_t>(slot) >= g7f6.slots.size() ||
        g7f6.slots[static_cast<std::size_t>(slot)].empty()) {
        return blocked_outcome("slot", "size slot");
    }
    *size = g7f6.slots[static_cast<std::size_t>(slot)].size();
    return executed_outcome("fixture size");
}

sotc::InitPostCallOutcome h7f6_copy(int slot, char* destination, std::size_t size) {
    if (destination == nullptr || slot < 0 ||
        static_cast<std::size_t>(slot) >= g7f6.slots.size() ||
        g7f6.slots[static_cast<std::size_t>(slot)].empty()) {
        return blocked_outcome("slot", "copy slot");
    }
    const std::string& text = g7f6.slots[static_cast<std::size_t>(slot)];
    if (size != text.size()) return blocked_outcome("size", "copy size");
    std::memcpy(destination, text.data(), size);
    return executed_outcome("fixture copy");
}

sotc::InitPostCallOutcome h7f6_close(int slot) {
    if (slot < 0 || static_cast<std::size_t>(slot) >= g7f6.slots.size() ||
        g7f6.slots[static_cast<std::size_t>(slot)].empty()) {
        return blocked_outcome("slot", "close slot");
    }
    g7f6.slots[static_cast<std::size_t>(slot)].clear();
    return executed_outcome("fixture close");
}

sotc::InitPostCallOutcome h7f6_fopen(const char*, const char*, std::FILE** file) {
    if (file == nullptr) return blocked_outcome("file", "fopen file");
    std::FILE* f = std::tmpfile();
    if (f == nullptr) return blocked_outcome("tmpfile", "tmpfile failed");
    const char text[] = "STYLE,";
    std::fwrite(text, 1, sizeof text - 1, f);
    *file = f;
    return executed_outcome("fixture tmpfile");
}

void test_7f6f90() {
    std::array<std::int32_t, 2> counts{};
    std::array<sotc::Staffroll7f6f90Entry, 2 * sotc::kStaffroll7f6f90MaxEntries> entries{};
    sotc::Staffroll7f6f90Hooks hooks{};
    hooks.load_56cdb0 = h7f6_load;
    hooks.size_56ceb0 = h7f6_size;
    hooks.copy_56cec0 = h7f6_copy;
    hooks.close_56ce60 = h7f6_close;
    hooks.counts = counts.data();
    hooks.entries = entries.data();
    hooks.entry_capacity = sotc::kStaffroll7f6f90MaxEntries;

    g7f6 = Rec7f6{};
    auto r = sotc::run_staffroll_7f6f90_body(0, "staffroll_remaster_us.txt", hooks);
    CHECK(r.ok && !r.used_fopen && r.entries == 1 && counts[0] == 1);
    CHECK(r.text_bytes == 19 && entries[0].index == 19);
    CHECK(entries[0].rest == r.text.get() + 19 && std::strcmp(entries[0].rest, "") == 0);
    CHECK(r.call_log.count == 5 && r.call_log.records[0].callee_rva == 0xf902a0 &&
          r.call_log.records[1].callee_rva == 0x56cdb0 &&
          r.call_log.records[4].callee_rva == 0x56ce60);

    r = sotc::run_staffroll_7f6f90_body(1, "staffroll_original_us.txt", hooks);
    CHECK(r.ok && !r.used_fopen && r.entries == 1 && counts[1] == 1);
    CHECK(r.text_bytes == 7 && entries[sotc::kStaffroll7f6f90MaxEntries].index == 36);

    g7f6 = Rec7f6{};
    counts.fill(0);
    entries.fill(sotc::Staffroll7f6f90Entry{});
    r = sotc::run_staffroll_7f6f90_body(0, "unknown.txt", hooks);
    CHECK(r.ok && r.entries == 1 && entries[0].index == -1);

    g7f6 = Rec7f6{};
    counts.fill(0);
    entries.fill(sotc::Staffroll7f6f90Entry{});
    r = sotc::run_staffroll_7f6f90_body(0, "missing.txt", hooks);
    CHECK(!r.ok && std::strcmp(r.blocked, "missing_comma") == 0);

    g7f6 = Rec7f6{};
    counts.fill(0);
    entries.fill(sotc::Staffroll7f6f90Entry{});
    r = sotc::run_staffroll_7f6f90_body(0, "bom.txt", hooks);
    CHECK(r.ok && r.entries == 1 && entries[0].index == 36);
    CHECK(entries[0].rest == r.text.get() + 10);
    CHECK(entries[0].rest[0] == '\0' && entries[0].rest[1] == '\0');

    auto missing = hooks;
    missing.load_56cdb0 = nullptr;
    g7f6 = Rec7f6{};
    r = sotc::run_staffroll_7f6f90_body(0, "staffroll_remaster_us.txt", missing);
    CHECK(!r.ok && std::strcmp(r.blocked, "fallback_contract_missing") == 0);

    auto bad_mode = sotc::run_staffroll_7f6f90_body(2, "staffroll_remaster_us.txt", hooks);
    CHECK(!bad_mode.ok && std::strcmp(bad_mode.blocked, "mode_out_of_range") == 0);

    auto fopen_hooks = hooks;
    fopen_hooks.fopen_f902a0 = h7f6_fopen;
    counts.fill(0);
    entries.fill(sotc::Staffroll7f6f90Entry{});
    auto direct = sotc::run_staffroll_7f6f90_body(0, "absent.bin", fopen_hooks);
    CHECK(direct.ok && direct.used_fopen && direct.entries == 1);
    CHECK(direct.text_bytes == 6 && entries[0].index == 35);
    CHECK(direct.call_log.count == 1 && direct.call_log.records[0].callee_rva == 0xf902a0);
}

void test_7f7220() {
    g7f = Rec7f{};
    sotc::Staffroll7f7220Hooks hooks{};
    hooks.query_5d06f0 = h7f_query;
    hooks.set_7f6f90 = h7f_set;

    g7f.query = true;
    auto r = sotc::run_staffroll_7f7220(hooks);
    CHECK(r.ok && r.query_result);
    CHECK(r.set_calls == 1 && g7f.calls == 1);
    CHECK(g7f.modes[0] == 1 && g7f.strings[0] == 0x128b878);
    CHECK(r.call_log.count == 2 && r.call_log.records[0].callee_rva == 0x5d06f0 &&
          r.call_log.records[1].callee_rva == 0x7f6f90);

    g7f = Rec7f{};
    g7f.query = false;
    r = sotc::run_staffroll_7f7220(hooks);
    CHECK(r.ok && !r.query_result);
    CHECK(r.set_calls == 2 && g7f.calls == 2);
    CHECK(g7f.modes[0] == 0 && g7f.strings[0] == 0x128b8ac);
    CHECK(g7f.modes[1] == 1 && g7f.strings[1] == 0x128b892);
    CHECK(r.call_log.count == 3);

    auto bad = hooks;
    bad.set_7f6f90 = nullptr;
    const auto blocked = sotc::run_staffroll_7f7220(bad);
    CHECK(!blocked.ok);
    CHECK(std::strcmp(blocked.blocked, "hook_7f6f90_missing") == 0);

    bad = hooks;
    bad.query_5d06f0 = [] { return blocked_outcome("query_blocked", "no global"); };
    const auto query_blocked = sotc::run_staffroll_7f7220(bad);
    CHECK(!query_blocked.ok);
    CHECK(std::strcmp(query_blocked.blocked, "query_5d06f0_blocked") == 0);
    CHECK(query_blocked.calls_blocked == 1 && query_blocked.call_log.count == 1);
}

struct Rec5c {
    int a = 0;
    int b = 0;
    int c = 0;
    bool tail = false;
};
Rec5c g5c;

sotc::InitPostCallOutcome h5c_a() {
    ++g5c.a;
    return executed_outcome("5b9f90 fixture");
}
sotc::InitPostCallOutcome h5c_b(std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t) {
    ++g5c.b;
    auto o = executed_outcome("96f20 fixture");
    o.has_value = true;
    o.value = 0x12345678;
    return o;
}
sotc::InitPostCallOutcome h5c_ret() {
    ++g5c.c;
    return executed_outcome("ret fixture");
}
sotc::InitPostCallOutcome h5c_5d5410() { return executed_outcome("5d5410 fixture"); }
sotc::InitPostCallOutcome h5c_5c91() {
    ++g5c.c;
    return executed_outcome("5c91e0 fixture");
}
sotc::InitPostCallOutcome h5c_tail_outcome() {
    ++g5c.c;
    g5c.tail = true;
    return executed_outcome("tail fixture");
}

void test_5c64c0() {
    g5c = Rec5c{};
    std::array<std::uint8_t, 0x88> owner{};
    std::array<std::uint8_t, 0x2500> source{};
    for (std::size_t i = 0; i < source.size(); ++i) {
        source[i] = static_cast<std::uint8_t>((i * 37u + 11u) & 0xffu);
    }
    void* source_ptr = source.data();
    std::memcpy(owner.data() + 0x80, &source_ptr, sizeof source_ptr);

    std::array<std::uint8_t, sotc::kInit5c64c0BssASize> bss_a{};
    std::array<std::uint8_t, sotc::kInit5c64c0BssBSize> bss_b{};
    std::array<std::uint8_t, sotc::kInit5c64c0BssCSize> bss_c{};
    std::uintptr_t cell = 0;

    sotc::Init5c64c0BodyHooks hooks{};
    hooks.owner = owner.data();
    hooks.source_size = source.size();
    hooks.bss_a = bss_a.data();
    hooks.bss_b = bss_b.data();
    hooks.bss_c = bss_c.data();
    hooks.cell = &cell;
    hooks.call_5b9f90 = h5c_a;
    hooks.call_96f20 = h5c_b;
    hooks.call_5ddcc0 = h5c_ret;
    hooks.call_5d57f0 = h5c_ret;
    hooks.call_5d5410 = h5c_5d5410;
    hooks.call_5d5760 = h5c_ret;
    hooks.call_5d5700 = h5c_ret;
    hooks.call_5c91e0 = h5c_5c91;
    hooks.tail_5cf130 = h5c_tail_outcome;

    const auto r = sotc::run_init_5c64c0_body(hooks);
    CHECK(r.ok);
    CHECK(r.source == source.data());
    CHECK(r.calls_5b9f90 == 1 && r.calls_96f20 == 1 && r.calls_after == 6 && r.tail_called);
    CHECK(g5c.a == 1 && g5c.b == 1 && g5c.c == 6 && g5c.tail);
    CHECK(r.cell_value == 0x12345678);
    CHECK(cell == 0x12345678);
    CHECK(r.call_log.count == 9);
    CHECK(r.call_log.records[0].callee_rva == 0x5b9f90);
    CHECK(r.call_log.records[1].callee_rva == 0x96f20);
    CHECK(r.call_log.records[8].callee_rva == 0x5cf130);

    // 0x5c64c0: two ymm stores of eight u32 1s, then the scalar fields.
    for (std::size_t off = 0; off <= 0x20; off += 0x20) {
        for (std::size_t i = 0; i < 8; ++i) {
            std::uint32_t value = 0;
            std::memcpy(&value, bss_a.data() + off + i * 4, sizeof value);
            CHECK(value == 1);
        }
    }
    std::uint32_t scalar = 0;
    for (std::size_t off : {0x40u, 0x44u, 0x6cu, 0x70u, 0x74u, 0x78u, 0x7cu, 0x80u}) {
        std::memcpy(&scalar, bss_a.data() + off, sizeof scalar);
        CHECK(scalar == 1);
    }
    std::memcpy(&scalar, bss_a.data() + 0x48, sizeof scalar);
    CHECK(scalar == 0);
    std::memcpy(&scalar, bss_a.data() + 0x4c, sizeof scalar);
    CHECK(scalar == 0);
    for (std::size_t off = 0x50; off <= 0x68; off += 4) {
        std::memcpy(&scalar, bss_a.data() + off, sizeof scalar);
        CHECK(scalar == 1);
    }
    for (std::uint8_t value : bss_b) CHECK(value == 0);

    // Store order: ymm2 at +0x2c, ymm1 at +0x20, ymm0 at +0. The final +0x40..+0x4b
    // therefore comes from source+0x24e0, while +0x20..+0x3f comes from source+0x24c0.
    for (std::size_t i = 0; i < 0x20; ++i) CHECK(bss_c[i] == source[0x24a0 + i]);
    for (std::size_t i = 0; i < 0x20; ++i) CHECK(bss_c[0x20 + i] == source[0x24c0 + i]);
    for (std::size_t i = 0; i < 0x0c; ++i) CHECK(bss_c[0x40 + i] == source[0x24e0 + i]);

    auto bad = hooks;
    bad.owner = nullptr;
    const auto blocked = sotc::run_init_5c64c0_body(bad);
    CHECK(!blocked.ok);
    CHECK(std::strcmp(blocked.blocked, "owner_missing") == 0);

    bad = hooks;
    void* no_source = nullptr;
    std::memcpy(owner.data() + 0x80, &no_source, sizeof no_source);
    const auto blocked_source = sotc::run_init_5c64c0_body(bad);
    CHECK(!blocked_source.ok);
    CHECK(std::strcmp(blocked_source.blocked, "source_null") == 0);

    void* restored_source = source.data();
    std::memcpy(owner.data() + 0x80, &restored_source, sizeof restored_source);
    bad = hooks;
    bad.call_96f20 = [](std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t) {
        return blocked_outcome("nested_body_not_lifted", "0x97a50 absent");
    };
    const auto partial = sotc::run_init_5c64c0_body(bad);
    CHECK(partial.ok && partial.calls_blocked == 1);
    CHECK(!partial.call_log.records[1].executed);
    CHECK(partial.cell_value == 0);
}

struct Rec946 {
    std::array<std::uint8_t, 0x1000> service{};
    std::array<char, 0x40> name = {"Player"};
    int locks = 0;
    int unlocks = 0;
    int records = 0;
    int copies = 0;
    int releases_a = 0;
    int releases_b = 0;
    bool valid_query = false;
};
Rec946 g946;

sotc::InitPostCallOutcome h946_query(std::uint32_t, sotc::Init98a450Result* out) {
    out->field50 = g946.valid_query ? 0 : -1;
    return executed_outcome("98a450 fixture");
}
sotc::InitPostCallOutcome h946_lock(std::uint32_t, void** handle) {
    ++g946.locks;
    *handle = g946.service.data();
    auto o = executed_outcome("986f30 fixture");
    o.has_value = true;
    o.value = 0;
    return o;
}
sotc::InitPostCallOutcome h946_unlock(void*) {
    ++g946.unlocks;
    auto o = executed_outcome("987720 fixture");
    o.has_value = true;
    o.value = 0;
    return o;
}
sotc::InitPostCallOutcome h946_get_name(void*, const void*) {
    auto o = executed_outcome("9882e0 fixture");
    o.has_value = true;
    o.value = 0;
    return o;
}
sotc::InitPostCallOutcome h946_get_record(void*, std::int32_t, void* buffer,
                                          std::size_t* size) {
    ++g946.records;
    struct Record {
        std::uint32_t index;
        std::uint32_t pad;
        const char* name;
    };
    static_assert(sizeof(Record) == 0x10);
    if (size != nullptr) {
        *size = sizeof(Record);
    } else {
        Record record{0, 0, g946.name.data()};
        std::memcpy(buffer, &record, sizeof record);
    }
    auto o = executed_outcome("988400 fixture");
    o.has_value = true;
    o.value = 0;
    return o;
}
sotc::InitPostCallOutcome h946_copy(void* dst, std::size_t capacity, const char* src) {
    ++g946.copies;
    std::snprintf(static_cast<char*>(dst), capacity, "%s", src);
    return executed_outcome("9556a0 fixture");
}
sotc::InitPostCallOutcome h946_release_a(void*) {
    ++g946.releases_a;
    auto o = executed_outcome("9878d0 fixture");
    o.has_value = true;
    o.value = 0;
    return o;
}
sotc::InitPostCallOutcome h946_release_b(void*) {
    ++g946.releases_b;
    auto o = executed_outcome("987260 fixture");
    o.has_value = true;
    o.value = 0;
    return o;
}
sotc::InitPostCallOutcome h946_direct(std::int32_t, std::int32_t, void*, std::int32_t) {
    return blocked_outcome("body_not_lifted", "944de0 fixture contract");
}
sotc::InitPostCallOutcome h946_log(std::int32_t, std::int32_t, std::int32_t, std::uint32_t,
                                   std::uint32_t, std::int32_t) {
    return executed_outcome("984700 fixture");
}

sotc::Init946880Hooks make_946_hooks(std::uint32_t* lock) {
    sotc::Init946880Hooks hooks{};
    hooks.lock = lock;
    const std::uint32_t id = 0x12345678u;
    hooks.service_id = id;
    hooks.query_98a450 = h946_query;
    hooks.direct_944de0 = h946_direct;
    hooks.log_984700 = h946_log;
    hooks.init_hooks.lock_service = h946_lock;
    hooks.init_hooks.unlock_service = h946_unlock;
    hooks.init_hooks.get_name = h946_get_name;
    hooks.init_hooks.get_record = h946_get_record;
    hooks.init_hooks.copy_name = h946_copy;
    hooks.init_hooks.release_a = h946_release_a;
    hooks.init_hooks.release_b = h946_release_b;
    return hooks;
}

void reset_946() {
    g946 = Rec946{};
    const std::uint32_t id = 0x12345678u;
    std::memcpy(g946.service.data(), &id, sizeof id);
    std::memcpy(g946.service.data() + sotc::kInit9469f0ServiceIdOff, &id, sizeof id);
}

void test_946880_chain() {
    reset_946();
    std::uint32_t lock = 0;
    const auto hooks = make_946_hooks(&lock);

    const auto inner =
        sotc::run_init_9469f0_body(1, nullptr, g946.service.data(), g946.service.size(),
                                   hooks.init_hooks);
    CHECK(inner.ok);
    CHECK(inner.status == 0);
    CHECK(inner.get_record_calls == 2 && inner.copy_calls == 2);
    CHECK(inner.call_log.count == 8);
    CHECK(inner.call_log.records[0].callee_rva == 0x986f30);
    CHECK(inner.call_log.records[1].callee_rva == 0x987720);
    CHECK(inner.call_log.records[2].callee_rva == 0x988400);
    CHECK(inner.call_log.records[3].callee_rva == 0x988400);
    CHECK(inner.call_log.records[4].callee_rva == 0x9556a0);
    CHECK(inner.call_log.records[5].callee_rva == 0x9556a0);
    CHECK(inner.call_log.records[6].callee_rva == 0x9878d0);
    CHECK(inner.call_log.records[7].callee_rva == 0x987260);
    CHECK(std::strcmp(inner.name0.data(), "Player") == 0);
    CHECK(std::strcmp(inner.name1.data(), "Player") == 0);
    CHECK(std::strcmp(reinterpret_cast<char*>(g946.service.data() + 0x1d0), "Player") == 0);
    CHECK(std::strcmp(reinterpret_cast<char*>(g946.service.data() + 0x240), "Player") == 0);

    reset_946();
    lock = 0;
    const auto outer = sotc::run_init_946880_body(1, g946.service.data(), g946.service.size(),
                                                  make_946_hooks(&lock));
    CHECK(outer.ok);
    CHECK(outer.status == 0);
    CHECK(outer.took_9469f0 && !outer.took_944de0);
    CHECK(outer.lock_acquired && lock == 0);
    CHECK(outer.inner.ok && outer.inner.call_log.count == 8);
    CHECK(outer.call_log.count == 2);
    CHECK(outer.call_log.records[0].callee_rva == 0x98a450);
    CHECK(outer.call_log.records[1].callee_rva == 0x9469f0);
    CHECK(outer.call_log.records[1].executed);
    CHECK(g946.locks == 1 && g946.unlocks == 1 && g946.records == 2 && g946.copies == 2);
    CHECK(g946.releases_a == 1 && g946.releases_b == 1);

    reset_946();
    lock = 0;
    g946.valid_query = true;
    const auto direct =
        sotc::run_init_946880_body(1, g946.service.data(), g946.service.size(),
                                   make_946_hooks(&lock));
    CHECK(!direct.ok);
    CHECK(!direct.took_9469f0 && !direct.took_944de0);
    CHECK(direct.status == -3 && lock == 0);
    CHECK(std::strcmp(direct.blocked, "body_not_lifted") == 0);
    CHECK(direct.call_log.count == 2);
    CHECK(direct.call_log.records[1].callee_rva == 0x944de0);

    reset_946();
    lock = 0;
    auto no_lock = make_946_hooks(nullptr);
    const auto missing = sotc::run_init_946880_body(
        1, g946.service.data(), g946.service.size(), no_lock);
    CHECK(!missing.ok && std::strcmp(missing.blocked, "lock_missing") == 0);

    reset_946();
    auto no_record = make_946_hooks(&lock);
    no_record.init_hooks.get_record = nullptr;
    const auto refused = sotc::run_init_9469f0_body(
        0, nullptr, g946.service.data(), g946.service.size(), no_record.init_hooks);
    CHECK(!refused.ok && std::strcmp(refused.blocked, "hook_missing") == 0);

    reset_946();
    lock = 0;
    auto bad_copy = make_946_hooks(&lock);
    bad_copy.init_hooks.copy_name = [](void*, std::size_t, const char*) {
        return blocked_outcome("copy_blocked", "fixture copy refusal");
    };
    const auto copy_failed = sotc::run_init_9469f0_body(
        0, nullptr, g946.service.data(), g946.service.size(), bad_copy.init_hooks);
    CHECK(!copy_failed.ok);
    CHECK(std::strcmp(copy_failed.blocked, "copy_blocked") == 0);
    CHECK(copy_failed.call_log.count == 7);
    CHECK(g946.releases_a == 1 && g946.releases_b == 1);

    reset_946();
    lock = 0;
    auto logging = make_946_hooks(&lock);
    logging.log_flag = true;
    const auto logged = sotc::run_init_946880_body(
        1, g946.service.data(), g946.service.size(), logging);
    CHECK(logged.ok && logged.status == 0);
    CHECK(logged.log_calls == 1);
    CHECK(logged.call_log.count == 3);
    CHECK(logged.call_log.records[2].callee_rva == 0x984700);
    CHECK(logged.call_log.records[2].args[5] == 1);

    reset_946();
    lock = 0;
    auto negative_id = make_946_hooks(&lock);
    negative_id.service_id = 0xffffffffu;
    const auto negative = sotc::run_init_946880_body(
        1, g946.service.data(), g946.service.size(), negative_id);
    CHECK(!negative.ok);
    CHECK(negative.call_log.count == 1);
    CHECK(negative.call_log.records[0].callee_rva == 0x944de0);
    CHECK(negative.call_log.records[0].args[0] == 1);
    CHECK(!negative.took_9469f0 && lock == 0);
}

struct Rec5d {
    std::array<std::uint8_t, 0x10000> arena{};
    std::size_t used = 0;
    int allocs = 0;
    int tails = 0;
    void* tail_dst = nullptr;
    std::uint32_t tail_tag = 0;
    std::uint64_t tail_packed = 0;
    std::uint32_t tail_count = 0;
    std::uint64_t tail_arg5 = 0;
    std::uint32_t tail_arg6 = 0;
    std::uint64_t tail_arg7 = 0;
};
Rec5d g5d;

sotc::InitPostCallOutcome h5d_alloc(std::uint32_t size, std::uint32_t,
                                     std::uint32_t alignment, std::uint32_t) {
    ++g5d.allocs;
    const std::size_t align = alignment != 0 ? alignment : 1u;
    const std::size_t start = (g5d.used + align - 1u) & ~(align - 1u);
    if (start + size > g5d.arena.size()) {
        return blocked_outcome("alloc_12820_exhausted", "fixture arena exhausted");
    }
    g5d.used = start + size;
    auto o = executed_outcome("alloc fixture");
    o.has_value = true;
    o.value = reinterpret_cast<std::uintptr_t>(g5d.arena.data() + start);
    return o;
}

sotc::InitPostCallOutcome h5d_tail(void* dst, std::uint32_t tag, std::uint64_t packed,
                                    std::uint32_t count, std::uint64_t arg5,
                                    std::uint32_t arg6, std::uint64_t arg7) {
    ++g5d.tails;
    g5d.tail_dst = dst;
    g5d.tail_tag = tag;
    g5d.tail_packed = packed;
    g5d.tail_count = count;
    g5d.tail_arg5 = arg5;
    g5d.tail_arg6 = arg6;
    g5d.tail_arg7 = arg7;
    return executed_outcome("tail fixture");
}

std::uint32_t read5d32(const std::array<std::uint8_t, sotc::kInit5d5410ObjectBytes>& a,
                       std::size_t off) {
    std::uint32_t v = 0;
    std::memcpy(&v, a.data() + off, sizeof v);
    return v;
}

std::uint64_t read5d64(const std::array<std::uint8_t, sotc::kInit5d5410StateBytes>& a,
                       std::size_t off) {
    std::uint64_t v = 0;
    std::memcpy(&v, a.data() + off, sizeof v);
    return v;
}

void test_5d5410_builders() {
    const auto a = sotc::run_init_8f520_body(0, 1, 7, 0, 0, 0, 7, 0, 0, 0, 0xff, 0xff);
    CHECK(a.q0 == 0x0000000000700793ull);
    CHECK(a.q1 == 0xffffff00ffffffffull);

    const auto b = sotc::run_init_8f520_body(1, 7, 7, 0, 0, 0, 7, 0, 0, 0, 0xff, 0xff);
    CHECK(b.q0 == 0x00000000007007f7ull);
    CHECK(b.q1 == 0xffffff00ffffffffull);

    const auto c = sotc::run_init_8f520_body(0, 4, 7, 0, 0, 0, 7, 0, 0, 0, 0xff, 0xff);
    CHECK(c.q0 == 0x00000000007007c3ull);
    CHECK(c.q1 == 0xffffff00ffffffffull);

    // The bit-30 predicate uses rsi|rcx, not rdx|rcx: (0,1,0) leaves bit30 clear and
    // (0,6,1) sets it.
    CHECK(sotc::run_init_8f1d0_body(0, 1, 0, 8) == 0x0000000820010001ull);
    CHECK(sotc::run_init_8f1d0_body(0, 1, 0, 0) == 0x0000000020010001ull);
    CHECK(sotc::run_init_8f1d0_body(0, 6, 1, 0xffffffffu) == 0xffffffff61060106ull);
}

void test_5d5410_chain() {
    g5d = Rec5d{};
    std::array<std::uint8_t, sotc::kInit5d5410ObjectBytes> obj_a{};
    std::array<std::uint8_t, sotc::kInit5d5410ObjectBytes> obj_b{};
    std::array<std::uint8_t, sotc::kInit5d5410StateBytes> state{};
    sotc::Init5d5410Hooks hooks{};
    hooks.object_a = obj_a.data();
    hooks.object_b = obj_b.data();
    hooks.global_owner = 0x5d5410c0deull;
    hooks.state = state.data();
    hooks.be.alloc_12820 = h5d_alloc;
    hooks.be.tail_87f3e0 = h5d_tail;

    const auto r = sotc::run_init_5d5410_body(hooks);
    CHECK(r.ok);
    CHECK(r.calls_8be60 == 2 && r.calls_8f520 == 3 && r.calls_8f1d0 == 3);
    CHECK(r.blocked_calls == 0 && r.blocked == nullptr);
    CHECK(r.owner == 0x5d5410c0deull);
    CHECK(g5d.allocs == 0 && g5d.tails == 0);

    for (const auto* obj : {&obj_a, &obj_b}) {
        CHECK(read5d32(*obj, 0x00) == 0u);
        CHECK(read5d32(*obj, 0x04) == 0x00100000u);
        CHECK(read5d32(*obj, 0x08) == 5u);
        CHECK(read5d32(*obj, 0x0c) == 0x20077facu);
        CHECK(read5d32(*obj, 0x10) == 0u);
        CHECK(read5d32(*obj, 0x18) == 2u);
        CHECK(read5d32(*obj, 0x1c) == 0x1cu);
        CHECK(read5d32(*obj, 0x20) == 0x50u);
    }

    CHECK(state == r.state);
    CHECK(read5d64(state, 0x00) == 0x0000000000700793ull);
    CHECK(read5d64(state, 0x08) == 0xffffff00ffffffffull);
    CHECK(read5d64(state, 0x10) == 0x0000000820010001ull);
    CHECK(read5d64(state, 0x18) == 0x00000000007007f7ull);
    CHECK(read5d64(state, 0x20) == 0xffffff00ffffffffull);
    CHECK(read5d64(state, 0x28) == 0x0000000020010001ull);
    CHECK(read5d64(state, 0x30) == 0x00000000007007c3ull);
    CHECK(read5d64(state, 0x38) == 0xffffff00ffffffffull);
    CHECK(read5d64(state, 0x40) == 0xffffffff61060106ull);

    auto bad = hooks;
    bad.state = nullptr;
    const auto missing = sotc::run_init_5d5410_body(bad);
    CHECK(!missing.ok);
    CHECK(std::strcmp(missing.blocked, "state_missing") == 0);

    bad = hooks;
    bad.object_b = nullptr;
    const auto no_b = sotc::run_init_5d5410_body(bad);
    CHECK(!no_b.ok);
    CHECK(std::strcmp(no_b.blocked, "object_b_missing") == 0);
}

void test_8be60_paths() {
    g5d = Rec5d{};
    std::array<std::uint8_t, sotc::kInit5d5410ObjectBytes> obj{};
    std::array<std::uint8_t, 0x40> src{};
    for (std::size_t i = 0; i < src.size(); ++i) src[i] = static_cast<std::uint8_t>(i + 1u);

    sotc::Init8be60Hooks hooks{};
    hooks.alloc_12820 = h5d_alloc;
    hooks.tail_87f3e0 = h5d_tail;
    hooks.tag_2dd9f84 = 0x11111111u;
    hooks.tag_15fe920 = 0x22222222u;
    hooks.tag_2ed1f54 = 0x33333333u;
    hooks.tag_2ed37a4 = 0x44444444u;

    // Observed 0x5d5410 path: a4 bit1 suppresses allocation; r9/a5 is null; stack0 is 0.
    auto r = sotc::run_init_8be60_body(obj.data(), 0x1c, 0x50, 1, 2, nullptr, 0, hooks);
    CHECK(r.ok);
    CHECK(r.alloc_calls == 0 && r.copy_calls == 0 && r.tail_calls == 0);
    CHECK(r.format == 0x10u);
    CHECK(r.object_word4 == 0x00100000u);
    CHECK(r.object_word8 == 5u);
    CHECK(r.object_word_c == 0x20077facu);
    CHECK(g5d.allocs == 0 && g5d.tails == 0);

    // Allocation + copy path through 0x12820 and 0xf8f470: a4=0 selects tag 0x2dd9f84.
    obj.fill(0);
    g5d = Rec5d{};
    r = sotc::run_init_8be60_body(obj.data(), 0x1a, 0x10, 2, 0, src.data(), 0, hooks);
    CHECK(r.ok);
    CHECK(r.alloc_calls == 1 && r.copy_calls == 1);
    CHECK(r.allocation != nullptr);
    CHECK(std::memcmp(r.allocation, src.data(), 0x20) == 0);
    CHECK(r.object_word8 == 0x20u);
    CHECK(r.object_word_c == 0x2000c004u);
    CHECK(r.object_word4 ==
          (static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(r.allocation) >> 32) &
           0xfffu));
    std::uint64_t object_ptr = 0;
    std::memcpy(&object_ptr, obj.data() + 0x10, sizeof object_ptr);
    CHECK(object_ptr == reinterpret_cast<std::uintptr_t>(r.allocation));

    // a4 bits 3/4: allocate product+4 and write a zero terminator at +product.
    obj.fill(0);
    g5d = Rec5d{};
    r = sotc::run_init_8be60_body(obj.data(), 0x1c, 0x10, 2, 0x18, nullptr, 0, hooks);
    CHECK(r.ok);
    CHECK(r.alloc_calls == 1);
    CHECK(r.object_word4 ==
          (0x00107000u |
           (static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(r.allocation) >> 32) &
            0xfffu)));
    CHECK(r.object_word_c == 0x18077facu);
    std::uint32_t terminator = 1;
    std::memcpy(&terminator, static_cast<std::uint8_t*>(r.allocation) + 0x20, sizeof terminator);
    CHECK(terminator == 0u);

    // 0x8be9e jumps before r15 is initialized: refuse by name.
    obj.fill(0);
    g5d = Rec5d{};
    r = sotc::run_init_8be60_body(obj.data(), 0x1c, 0x10, 2, 0x800, nullptr, 0, hooks);
    CHECK(!r.ok);
    CHECK(std::strcmp(r.blocked, "r15_uninitialized_path") == 0);
    CHECK(r.object_word4 == 0u && r.object_word8 == 0u && r.object_word_c == 0u);
    CHECK(r.alloc_calls == 0);

    // Allocation refusal is a named blocker, not a fabricated pointer.
    auto bad = hooks;
    bad.alloc_12820 = [](std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t) {
        return blocked_outcome("fixture_alloc_blocked", "no arena");
    };
    r = sotc::run_init_8be60_body(obj.data(), 0x1a, 0x10, 2, 0, src.data(), 0, bad);
    CHECK(!r.ok);
    CHECK(std::strcmp(r.blocked, "fixture_alloc_blocked") == 0);
    CHECK(r.alloc_calls == 1);

    bad = hooks;
    bad.alloc_12820 = nullptr;
    r = sotc::run_init_8be60_body(obj.data(), 0x1a, 0x10, 2, 0, src.data(), 0, bad);
    CHECK(!r.ok);
    CHECK(std::strcmp(r.blocked, "alloc_12820_missing") == 0);

    // Tail path: stack0 != 0 reaches 0x87f3e0 with the exact packed destination/count.
    obj.fill(0);
    g5d = Rec5d{};
    r = sotc::run_init_8be60_body(obj.data(), 0x1b, 0x10, 2, 0, nullptr, 0x1234, hooks);
    CHECK(r.ok);
    CHECK(r.tail_calls == 1 && g5d.tails == 1);
    CHECK(g5d.tail_dst == obj.data() + 0x24);
    CHECK(g5d.tail_tag == 0x44444444u);
    CHECK(g5d.tail_packed == reinterpret_cast<std::uintptr_t>(r.allocation));
    CHECK(g5d.tail_count == 0x20u);
    CHECK(g5d.tail_arg5 == 0x1234u);
    CHECK(g5d.tail_arg6 == 4u);
    CHECK(g5d.tail_arg7 == 0u);
    CHECK(r.object_word4 ==
          (0x00100000u |
           (static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(r.allocation) >> 32) &
            0xfffu)));
    CHECK(r.object_word_c == 0x20027204u);

    bad = hooks;
    bad.tail_87f3e0 = nullptr;
    r = sotc::run_init_8be60_body(obj.data(), 0x1b, 0x10, 2, 0, nullptr, 1, bad);
    CHECK(!r.ok);
    CHECK(std::strcmp(r.blocked, "tail_87f3e0_missing") == 0);

    r = sotc::run_init_8be60_body(nullptr, 0x1c, 0x50, 1, 2, nullptr, 0, hooks);
    CHECK(!r.ok);
    CHECK(std::strcmp(r.blocked, "object_missing") == 0);
}

struct Rec96 {
    int calls = 0;
    std::uint32_t args[10] = {};
    std::uintptr_t value = 0;
};
Rec96 g96;

sotc::InitPostCallOutcome h96_97a50(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2,
                                   std::uint32_t a3, std::uint32_t a4, std::uint32_t a5,
                                   std::uint32_t a6, std::uint32_t a7, std::uint32_t a8,
                                   std::uint32_t a9) {
    ++g96.calls;
    g96.args[0] = a0;
    g96.args[1] = a1;
    g96.args[2] = a2;
    g96.args[3] = a3;
    g96.args[4] = a4;
    g96.args[5] = a5;
    g96.args[6] = a6;
    g96.args[7] = a7;
    g96.args[8] = a8;
    g96.args[9] = a9;
    auto o = executed_outcome("97a50 fixture host allocation");
    o.has_value = true;
    o.value = g96.value;
    return o;
}

void test_96f20_body() {
    g96 = Rec96{};
    g96.value = 0x12345678u;
    sotc::Init96f20Hooks hooks{};
    hooks.call_97a50 = h96_97a50;
    const auto r = sotc::run_init_96f20_body(0x100, 0x80, 7, 9, hooks);
    CHECK(r.ok);
    CHECK(r.return_value == 0x12345678u);
    CHECK(r.call_log.count == 1);
    CHECK(r.call_log.records[0].callee_rva == 0x97a50);
    CHECK(r.call_log.records[0].executed);
    CHECK(r.call_log.records[0].arg_count == 6);
    CHECK(g96.calls == 1);
    CHECK(g96.args[0] == 0x100 && g96.args[1] == 0x80);
    CHECK(g96.args[2] == 1 && g96.args[3] == 1);
    CHECK(g96.args[4] == 7 && g96.args[5] == 9);
    CHECK(g96.args[6] == 0 && g96.args[7] == 0 && g96.args[8] == 0 && g96.args[9] == 0);

    auto bad = hooks;
    bad.call_97a50 = nullptr;
    const auto missing = sotc::run_init_96f20_body(0, 0, 0, 0, bad);
    CHECK(!missing.ok);
    CHECK(std::strcmp(missing.blocked, "call_97a50_missing") == 0);

    bad = hooks;
    bad.call_97a50 = [](std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t,
                        std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t,
                        std::uint32_t, std::uint32_t) {
        return blocked_outcome("fixture_blocker", "host refused 97a50");
    };
    const auto refused = sotc::run_init_96f20_body(0, 0, 0, 0, bad);
    CHECK(!refused.ok);
    CHECK(std::strcmp(refused.blocked, "call_97a50_refused") == 0);
    CHECK(refused.call_log.count == 1 && !refused.call_log.records[0].executed);
}

struct Rec560 {
    int calls_11bc0 = 0;
    int allocs = 0;
    int frees = 0;
    int stack_calls = 0;
    int register_calls = 0;
    int format_calls = 0;
    int releases = 0;
    int probes = 0;
    int branches = 0;
    int finals = 0;
    std::size_t alloc_sizes[4] = {};
    std::uint8_t* object = nullptr;
    std::uint8_t* buffer0 = nullptr;
    std::uint8_t* buffer1 = nullptr;
    std::uint8_t* buffer2 = nullptr;
    std::uint64_t stack_owner = 0;
    std::uint64_t stack_value = 0;
    std::uint64_t format_string = 0;
    std::uint64_t format_length = 0;
    bool take = false;
};
Rec560 g560;

std::uint64_t load_u64_test(const void* p) {
    std::uint64_t value = 0;
    std::memcpy(&value, p, sizeof value);
    return value;
}

sotc::InitPostCallOutcome h560_11bc0(const void*) {
    ++g560.calls_11bc0;
    return executed_outcome("11bc0 host descriptor");
}
sotc::InitPostCallOutcome h560_alloc_object(std::size_t size, void** out) {
    if (g560.allocs < 4) g560.alloc_sizes[g560.allocs] = size;
    ++g560.allocs;
    *out = g560.object;
    return executed_outcome("17070 host object");
}
sotc::InitPostCallOutcome h560_alloc_buffer(std::size_t size, void** out) {
    if (g560.allocs < 4) g560.alloc_sizes[g560.allocs] = size;
    ++g560.allocs;
    if (size == sotc::kInit560b60Buffer0Bytes) *out = g560.buffer0;
    else if (size == sotc::kInit560b60Buffer1Bytes) *out = g560.buffer1;
    else if (size == sotc::kInit560b60Buffer2Bytes) *out = g560.buffer2;
    else *out = nullptr;
    return executed_outcome("17090 host buffer");
}
sotc::InitPostCallOutcome h560_free(void*) {
    ++g560.frees;
    return executed_outcome("170b0/170c0 host free");
}
sotc::InitPostCallOutcome h560_3ca330(void* a, void*, void*) {
    ++g560.stack_calls;
    g560.stack_owner = load_u64_test(a);
    g560.stack_value = load_u64_test(static_cast<std::uint8_t*>(a) + 8);
    return executed_outcome("3ca330 host stack descriptors");
}
sotc::InitPostCallOutcome h560_273890(std::uintptr_t, std::uintptr_t, std::uintptr_t) {
    ++g560.register_calls;
    return executed_outcome("273890 host callback registration");
}
sotc::InitPostCallOutcome h560_14c0f0(std::uint8_t* out, const std::uint8_t* input) {
    ++g560.format_calls;
    g560.format_string = load_u64_test(input);
    g560.format_length = load_u64_test(input + 8);
    std::uint32_t value = 0x2a;
    std::memcpy(out, &value, sizeof value);
    for (std::size_t i = 0; i < sotc::kInit560b60Global30Bytes; ++i) {
        out[8 + i] = static_cast<std::uint8_t>(i + 1);
    }
    return executed_outcome("14c0f0 host format");
}
sotc::InitPostCallOutcome h560_release(void*) {
    ++g560.releases;
    return executed_outcome("f590 host release observation");
}
sotc::InitPostCallOutcome h560_56b540(void*, bool* take) {
    ++g560.probes;
    *take = g560.take;
    return executed_outcome("56b540 host probe");
}
sotc::InitPostCallOutcome h560_95580(void*) {
    ++g560.branches;
    return executed_outcome("95580 host branch");
}
sotc::InitPostCallOutcome h560_56efb0() {
    ++g560.finals;
    return executed_outcome("56efb0 host finalizer");
}

void test_560b60_body() {
    std::array<std::uint8_t, sotc::kInit560b60ObjectBytes> object{};
    std::array<std::uint8_t, sotc::kInit560b60ObjectBytes> previous_object{};
    std::array<std::uint8_t, sotc::kInit560b60Buffer0Bytes> buffer0{};
    std::array<std::uint8_t, sotc::kInit560b60Buffer1Bytes> buffer1{};
    std::array<std::uint8_t, sotc::kInit560b60Buffer2Bytes> buffer2{};
    std::array<std::uint8_t, sotc::kInit560b60DescriptorBytes> descriptor{};
    for (std::size_t i = 0; i < descriptor.size(); ++i) {
        descriptor[i] = static_cast<std::uint8_t>(0x40 + i);
    }
    std::array<std::uint8_t, 16> global10{};
    std::uint64_t global20 = 0;
    std::uint32_t global28 = 0;
    std::array<std::uint8_t, sotc::kInit560b60Global30Bytes> global30{};
    std::array<std::uint8_t, sotc::kInit560b60Global50Bytes> global50{};
    std::array<std::uintptr_t, 3> callbacks{};
    std::array<std::uint8_t, sotc::kInit560b60FormatBytes> format{};
    void* previous = nullptr;

    g560 = Rec560{};
    g560.object = object.data();
    g560.buffer0 = buffer0.data();
    g560.buffer1 = buffer1.data();
    g560.buffer2 = buffer2.data();
    sotc::Init560b60Hooks hooks{};
    hooks.descriptor = descriptor.data();
    hooks.object = object.data();
    hooks.buffer_2000 = buffer0.data();
    hooks.buffer_0400 = buffer1.data();
    hooks.buffer_0200 = buffer2.data();
    hooks.global_10 = global10.data();
    hooks.global_20 = &global20;
    hooks.global_28 = &global28;
    hooks.global_30 = global30.data();
    hooks.global_50 = global50.data();
    hooks.previous = &previous;
    hooks.callbacks = callbacks.data();
    hooks.format_out = format.data();
    hooks.call_11bc0 = h560_11bc0;
    hooks.alloc_17070 = h560_alloc_object;
    hooks.alloc_17090 = h560_alloc_buffer;
    hooks.free_170b0 = h560_free;
    hooks.free_170c0 = h560_free;
    hooks.call_3ca330 = h560_3ca330;
    hooks.call_273890 = h560_273890;
    hooks.call_14c0f0 = h560_14c0f0;
    hooks.release_f590 = h560_release;
    hooks.call_56b540 = h560_56b540;
    hooks.call_95580 = h560_95580;
    hooks.call_56efb0 = h560_56efb0;

    const auto r = sotc::run_init_560b60_body(hooks);
    CHECK(r.ok);
    CHECK(r.calls_total == 13);
    CHECK(r.alloc_calls == 4);
    CHECK(r.free_calls == 0);
    CHECK(r.calls_95580 == 0);
    CHECK(!r.previous_cleanup);
    CHECK(previous == object.data());
    CHECK(r.object == object.data());
    CHECK(r.buffer_2000 == buffer0.data());
    CHECK(r.buffer_0400 == buffer1.data());
    CHECK(r.buffer_0200 == buffer2.data());
    CHECK(g560.calls_11bc0 == 1 && g560.allocs == 4 && g560.frees == 0);
    CHECK(g560.stack_calls == 1 && g560.stack_owner == reinterpret_cast<std::uintptr_t>(object.data()));
    CHECK(g560.stack_value == sotc::kInit560b60StackValue0);
    CHECK(g560.register_calls == 1 && g560.format_calls == 1);
    CHECK(g560.format_string == sotc::kInit560b60FormatStringRva);
    CHECK(g560.format_length == sotc::kInit560b60FormatStringBytes);
    CHECK(g560.releases == 3 && g560.probes == 1 && g560.branches == 0 && g560.finals == 1);
    CHECK(g560.alloc_sizes[0] == sotc::kInit560b60ObjectBytes);
    CHECK(g560.alloc_sizes[1] == sotc::kInit560b60Buffer0Bytes);
    CHECK(g560.alloc_sizes[2] == sotc::kInit560b60Buffer1Bytes);
    CHECK(g560.alloc_sizes[3] == sotc::kInit560b60Buffer2Bytes);
    CHECK(std::memcmp(global10.data(), descriptor.data(), 16) == 0);
    CHECK(global20 == load_u64_test(descriptor.data() + 16));
    CHECK(global28 == 0x2a);
    CHECK(r.global_28 == 0x2a);
    for (std::size_t i = 0; i < sotc::kInit560b60Global30Bytes; ++i) {
        CHECK(global30[i] == static_cast<std::uint8_t>(i + 1));
    }
    for (std::size_t i = 0; i < sotc::kInit560b60Global30Bytes; ++i) CHECK(format[8 + i] == 0);
    CHECK(callbacks[0] == sotc::kInit560b60Callback0);
    CHECK(callbacks[1] == sotc::kInit560b60Callback1);
    CHECK(callbacks[2] == sotc::kInit560b60Callback2);
    CHECK(r.callbacks[0] == sotc::kInit560b60Callback0);
    CHECK(load_u64_test(object.data() + 0x00) ==
          reinterpret_cast<std::uintptr_t>(object.data() + 0x20));
    CHECK(load_u64_test(object.data() + 0x08) ==
          reinterpret_cast<std::uintptr_t>(object.data() + 0x20));
    CHECK(load_u64_test(object.data() + 0x10) ==
          reinterpret_cast<std::uintptr_t>(object.data() + 0x8020));
    CHECK(load_u64_test(object.data() + 0x8020) ==
          reinterpret_cast<std::uintptr_t>(object.data() + 0x8040));
    CHECK(load_u64_test(object.data() + 0x8028) ==
          reinterpret_cast<std::uintptr_t>(object.data() + 0x8040));
    CHECK(load_u64_test(object.data() + 0x8030) ==
          reinterpret_cast<std::uintptr_t>(object.data() + 0x10040));
    CHECK(load_u64_test(object.data() + 0x10040) ==
          reinterpret_cast<std::uintptr_t>(buffer0.data()));
    CHECK(load_u64_test(object.data() + 0x10050) ==
          reinterpret_cast<std::uintptr_t>(buffer0.data() + buffer0.size()));
    CHECK(load_u64_test(object.data() + 0x10060) ==
          reinterpret_cast<std::uintptr_t>(buffer1.data()));
    CHECK(load_u64_test(object.data() + 0x10070) ==
          reinterpret_cast<std::uintptr_t>(buffer1.data() + buffer1.size()));
    CHECK(load_u64_test(object.data() + 0x10080) ==
          reinterpret_cast<std::uintptr_t>(buffer2.data()));
    CHECK(load_u64_test(object.data() + 0x10090) ==
          reinterpret_cast<std::uintptr_t>(buffer2.data() + buffer2.size()));
    CHECK(load_u64_test(object.data() + 0x100a0) == 0);
    CHECK(load_u64_test(object.data() + 0x100b0) == 0);
    CHECK(load_u64_test(object.data() + 0x136d8) == 0);
    CHECK(r.call_log.count == 13);
    CHECK(r.call_log.records[0].callee_rva == 0x11bc0);
    CHECK(r.call_log.records[1].callee_rva == 0x17070);
    CHECK(r.call_log.records[2].callee_rva == 0x17090);
    CHECK(r.call_log.records[5].callee_rva == 0x3ca330);
    CHECK(r.call_log.records[6].callee_rva == 0x273890);
    CHECK(r.call_log.records[7].callee_rva == 0x14c0f0);
    CHECK(r.call_log.records[8].callee_rva == 0xf590);
    CHECK(r.call_log.records[10].callee_rva == 0xf590);
    CHECK(r.call_log.records[11].callee_rva == 0x56b540);
    CHECK(r.call_log.records[12].callee_rva == 0x56efb0);

    // Previous-object branch: four buffer releases then the object release.
    previous_object.fill(0);
    const std::uintptr_t fake0 = 0x1111;
    const std::uintptr_t fake1 = 0x2222;
    const std::uintptr_t fake2 = 0x3333;
    const std::uintptr_t fake3 = 0x4444;
    std::memcpy(previous_object.data() + 0x136c8, &fake0, sizeof fake0);
    std::memcpy(previous_object.data() + 0x10080, &fake1, sizeof fake1);
    std::memcpy(previous_object.data() + 0x10060, &fake2, sizeof fake2);
    std::memcpy(previous_object.data() + 0x10040, &fake3, sizeof fake3);
    previous = previous_object.data();
    g560.frees = 0;
    g560.allocs = 0;
    const auto cleanup = sotc::run_init_560b60_body(hooks);
    CHECK(cleanup.ok && cleanup.previous_cleanup);
    CHECK(cleanup.free_calls == 5 && g560.frees == 5);
    CHECK(previous == object.data());

    // Conditional 0x95580 branch.
    previous = object.data();
    g560.branches = 0;
    g560.allocs = 0;
    g560.take = true;
    const auto branch = sotc::run_init_560b60_body(hooks);
    CHECK(branch.ok && branch.calls_95580 == 1 && g560.branches == 1);
    CHECK(branch.call_log.count == 14);
    CHECK(branch.call_log.records[12].callee_rva == 0x95580);
    CHECK(branch.call_log.records[13].callee_rva == 0x56efb0);
    g560.take = false;

    auto bad = hooks;
    bad.global_30 = nullptr;
    const auto missing = sotc::run_init_560b60_body(bad);
    CHECK(!missing.ok);
    CHECK(std::strcmp(missing.blocked, "state_missing") == 0);

    bad = hooks;
    bad.alloc_17090 = [](std::size_t, void**) {
        return blocked_outcome("fixture_blocker", "allocator refused");
    };
    g560.allocs = 0;
    const auto refused = sotc::run_init_560b60_body(bad);
    CHECK(!refused.ok);
    CHECK(std::strcmp(refused.blocked, "alloc_17090_failed") == 0);
}

struct RecA5 {
    int c9854 = 0, c994dd0 = 0, c994e70 = 0, c991c70 = 0;
    int alloc12820 = 0, zero992210 = 0, c14c2f0 = 0, resolve = 0;
    int c14ccb0 = 0, c14ccc0 = 0, c14d2d0 = 0, c95580 = 0, release_key = 0;
    int once = 0, thread = 0, mutex = 0, keycreate = 0, onceend = 0, nameres = 0;
    int c14d880 = 0, c14d7b0 = 0, c9892d0 = 0, c946590 = 0, c946650 = 0;
    int c994070 = 0, c901e0 = 0, c94640 = 0, logcond = 0, c3e7e0 = 0;
    std::uintptr_t config_tag = 0;
    int c17070 = 0, c3b5c0 = 0, virtual_release = 0, trap = 0;
    int size_count = 0;
    std::size_t sizes[4] = {};
    std::uintptr_t resource = 0;
    std::uintptr_t old_release = 0;
    std::uintptr_t task = 0;
    bool flip_canary = false;
    int canary_calls = 0;
    std::array<std::uint8_t, 16> desc{};
    std::array<std::uint8_t, 0x28> block{};
};
RecA5 gA5;
std::array<std::uint8_t, sotc::kInitA5cd90Arena0Bytes> gA5_arena0{};
std::array<std::uint8_t, 0x100000> gA5_arena1{};
std::array<std::uint8_t, 0x40> gA5_resource{};
std::array<std::uint8_t, sotc::kInitA5cd90Object70Bytes> gA5_task{};
std::array<std::uint8_t, sotc::kInitA5cd90KeyBytes> gA5_key{};

sotc::InitPostCallOutcome hA5_9854(std::uintptr_t) {
    ++gA5.c9854;
    return executed_outcome("9854 fixture");
}
sotc::InitPostCallOutcome hA5_994dd0(std::uintptr_t, std::uintptr_t) {
    ++gA5.c994dd0;
    return executed_outcome("994dd0 fixture");
}
sotc::InitPostCallOutcome hA5_994e70(std::uintptr_t config_rva) {
    ++gA5.c994e70;
    gA5.config_tag = config_rva;
    return executed_outcome("994e70 fixture");
}
sotc::InitPostCallOutcome hA5_991c70(const void* block) {
    ++gA5.c991c70;
    std::memcpy(gA5.block.data(), block, gA5.block.size());
    return executed_outcome("991c70 fixture");
}
sotc::InitPostCallOutcome hA5_12820(std::size_t size, std::uint32_t, std::uint32_t,
                                   std::uint32_t, void** out) {
    ++gA5.alloc12820;
    if (gA5.size_count < 4) gA5.sizes[gA5.size_count++] = size;
    *out = size == sotc::kInitA5cd90Arena0Bytes ? gA5_arena0.data() : gA5_arena1.data();
    auto o = executed_outcome("12820 fixture");
    o.has_value = true;
    o.value = reinterpret_cast<std::uintptr_t>(*out);
    return o;
}
sotc::InitPostCallOutcome hA5_992210(void* buffer, std::size_t size, std::uint32_t,
                                     std::uint32_t, std::int32_t* result) {
    ++gA5.zero992210;
    std::memset(buffer, 0, size);
    *result = 0;
    return executed_outcome("992210 fixture");
}
sotc::InitPostCallOutcome hA5_14c2f0(void* key_out) {
    ++gA5.c14c2f0;
    std::memcpy(key_out, gA5_key.data(), gA5_key.size());
    return executed_outcome("14c2f0 fixture");
}
sotc::InitPostCallOutcome hA5_resolve(void* out, void*, const void*) {
    ++gA5.resolve;
    std::memset(out, 0, 0x18);
    gA5.resource = reinterpret_cast<std::uintptr_t>(gA5_resource.data());
    std::memcpy(out, &gA5.resource, sizeof gA5.resource);
    static_cast<std::uint8_t*>(out)[8] = 1;
    auto o = executed_outcome("resolver fixture");
    o.has_value = true;
    o.value = gA5.resource;
    return o;
}
sotc::InitPostCallOutcome hA5_14ccb0(void*) {
    ++gA5.c14ccb0;
    return executed_outcome("14ccb0 fixture");
}
sotc::InitPostCallOutcome hA5_14ccc0(void*) {
    ++gA5.c14ccc0;
    return executed_outcome("14ccc0 fixture");
}
sotc::InitPostCallOutcome hA5_14d2d0(void*) {
    ++gA5.c14d2d0;
    return executed_outcome("14d2d0 fixture");
}
sotc::InitPostCallOutcome hA5_95580(void*) {
    ++gA5.c95580;
    return executed_outcome("95580 fixture");
}
sotc::InitPostCallOutcome hA5_f590(void*) {
    ++gA5.release_key;
    return executed_outcome("f590 fixture");
}
sotc::InitPostCallOutcome hA5_once(bool* initialized) {
    ++gA5.once;
    *initialized = gA5.once == 1;
    return executed_outcome("once fixture");
}
sotc::InitPostCallOutcome hA5_f730(std::uint64_t* thread) {
    ++gA5.thread;
    *thread = 0x1234;
    return executed_outcome("f730 fixture");
}
sotc::InitPostCallOutcome hA5_14c4a0(std::uintptr_t, std::uint64_t, std::uint64_t) {
    ++gA5.mutex;
    return executed_outcome("14c4a0 fixture");
}
sotc::InitPostCallOutcome hA5_f8f390(std::uintptr_t, std::uintptr_t, std::uintptr_t) {
    ++gA5.keycreate;
    return executed_outcome("f8f390 fixture");
}
sotc::InitPostCallOutcome hA5_f8f3e0(std::uintptr_t) {
    ++gA5.onceend;
    return executed_outcome("f8f3e0 fixture");
}
sotc::InitPostCallOutcome hA5_14cf20(void*, std::uint32_t, std::uintptr_t) {
    ++gA5.nameres;
    return executed_outcome("14cf20 fixture");
}
sotc::InitPostCallOutcome hA5_14d880(std::uint64_t* current) {
    ++gA5.c14d880;
    *current = 0;
    return executed_outcome("14d880 fixture");
}
sotc::InitPostCallOutcome hA5_14d7b0(void*, std::uint64_t* param) {
    ++gA5.c14d7b0;
    *param = 0;
    return executed_outcome("14d7b0 fixture");
}
sotc::InitPostCallOutcome hA5_9892d0(std::uintptr_t, std::int32_t* status) {
    ++gA5.c9892d0;
    *status = 0;
    return executed_outcome("9892d0 fixture");
}
sotc::InitPostCallOutcome hA5_946590(std::int32_t, std::uint64_t* size) {
    ++gA5.c946590;
    *size = 0x100000;
    return executed_outcome("946590 fixture");
}
sotc::InitPostCallOutcome hA5_946650(std::int32_t, void*, std::uint64_t) {
    ++gA5.c946650;
    return executed_outcome("946650 fixture");
}
sotc::InitPostCallOutcome hA5_994070(std::uint8_t, std::uint8_t) {
    ++gA5.c994070;
    return executed_outcome("994070 fixture");
}
sotc::InitPostCallOutcome hA5_f901e0(std::uint64_t* value, std::int32_t* result) {
    ++gA5.c901e0;
    *value = 0;
    *result = 0;
    return executed_outcome("f901e0 fixture");
}
sotc::InitPostCallOutcome hA5_994640(std::int32_t, std::uint64_t) {
    ++gA5.c94640;
    return executed_outcome("994640 fixture");
}
sotc::InitPostCallOutcome hA5_log_condition(bool* should_log) {
    ++gA5.logcond;
    *should_log = false;
    return executed_outcome("log condition fixture");
}
sotc::InitPostCallOutcome hA5_3e7e0(std::int32_t, std::int32_t, const void*, std::int32_t,
                                   std::int32_t, std::uintptr_t) {
    ++gA5.c3e7e0;
    return executed_outcome("3e7e0 fixture");
}
sotc::InitPostCallOutcome hA5_17070(std::size_t size, void** out) {
    ++gA5.c17070;
    if (gA5.size_count < 4) gA5.sizes[gA5.size_count++] = size;
    if (size != sotc::kInitA5cd90Object70Bytes) {
        return blocked_outcome("task_size", "unexpected task size");
    }
    *out = gA5_task.data();
    gA5.task = reinterpret_cast<std::uintptr_t>(*out);
    auto o = executed_outcome("17070 fixture");
    o.has_value = true;
    o.value = gA5.task;
    return o;
}
sotc::InitPostCallOutcome hA5_3b5c0(void*, const void*, std::uintptr_t, std::uint32_t,
                                   std::uint32_t, std::uint32_t, std::uint64_t) {
    ++gA5.c3b5c0;
    return executed_outcome("3b5c0 fixture");
}
sotc::InitPostCallOutcome hA5_virtual_release(void* pointer) {
    ++gA5.virtual_release;
    gA5.old_release = reinterpret_cast<std::uintptr_t>(pointer);
    return executed_outcome("virtual release fixture");
}
std::uint64_t hA5_canary() {
    if (gA5.flip_canary) return static_cast<std::uint64_t>(++gA5.canary_calls);
    return 0x5a17c0deULL;
}
sotc::InitPostCallOutcome hA5_trap() {
    ++gA5.trap;
    return executed_outcome("f8f3a0 fixture");
}

void init_a5_hooks(sotc::InitA5cd90Hooks& hooks) {
    hooks.object = nullptr;  // filled by caller
    hooks.descriptor = gA5.desc.data();
    hooks.registry = gA5_resource.data();
    hooks.config_flag = reinterpret_cast<const std::uint8_t*>(&gA5.c9854);
    hooks.arena_tag = reinterpret_cast<const std::uint32_t*>(&gA5.c994dd0);
    hooks.call_9854f0 = hA5_9854;
    hooks.call_994dd0 = hA5_994dd0;
    hooks.call_994e70 = hA5_994e70;
    hooks.call_991c70 = hA5_991c70;
    hooks.alloc_12820 = hA5_12820;
    hooks.call_992210 = hA5_992210;
    hooks.call_14c2f0 = hA5_14c2f0;
    hooks.resolve_resource = hA5_resolve;
    hooks.call_14ccb0 = hA5_14ccb0;
    hooks.call_14ccc0 = hA5_14ccc0;
    hooks.call_14d2d0 = hA5_14d2d0;
    hooks.call_95580 = hA5_95580;
    hooks.release_f590 = hA5_f590;
    hooks.once_2ed3668 = hA5_once;
    hooks.call_f730 = hA5_f730;
    hooks.call_14c4a0 = hA5_14c4a0;
    hooks.call_f8f390 = hA5_f8f390;
    hooks.call_f8f3e0 = hA5_f8f3e0;
    hooks.call_14cf20 = hA5_14cf20;
    hooks.call_14d880 = hA5_14d880;
    hooks.call_14d7b0 = hA5_14d7b0;
    hooks.call_9892d0 = hA5_9892d0;
    hooks.call_946590 = hA5_946590;
    hooks.call_946650 = hA5_946650;
    hooks.call_994070 = hA5_994070;
    hooks.call_f901e0 = hA5_f901e0;
    hooks.call_994640 = hA5_994640;
    hooks.log_condition = hA5_log_condition;
    hooks.call_3e7e0 = hA5_3e7e0;
    hooks.alloc_17070 = hA5_17070;
    hooks.call_3b5c0 = hA5_3b5c0;
    hooks.virtual_release = hA5_virtual_release;
    hooks.canary = hA5_canary;
    hooks.trap_f8f3a0 = hA5_trap;
}

void test_a5cd90_body() {
    std::array<std::uint8_t, sotc::kInitA5cd90ObjectBytes> object{};
    gA5 = RecA5{};
    const std::uint64_t name = 0x101e7ac;
    const std::uint16_t mode = 0x030a;
    std::memcpy(gA5.desc.data(), &name, sizeof name);
    std::memcpy(gA5.desc.data() + 8, &mode, sizeof mode);

    sotc::InitA5cd90Hooks hooks{};
    init_a5_hooks(hooks);
    hooks.object = object.data();

    const auto r = sotc::run_init_a5cd90_body(hooks);
    CHECK(r.ok);
    CHECK(r.calls_total == 34);
    CHECK(r.alloc_calls == 3);
    CHECK(r.call_log.count == 34);
    CHECK(r.field_69c8 == 0 && r.field_08 == 0);
    CHECK(r.resource_ready);
    CHECK(r.resource == gA5_resource.data());
    CHECK(r.flag_18 == 0 && r.flag_20 == 1);
    CHECK(r.object_a4f8 == reinterpret_cast<std::uintptr_t>(gA5_task.data()));
    CHECK(r.byte0 == 1);
    CHECK(r.once_calls == 2);
    CHECK(r.user_id_check == 0 && r.log_calls == 0 && r.trap_calls == 0);
    CHECK(gA5.sizes[0] == sotc::kInitA5cd90Arena0Bytes);
    CHECK(gA5.sizes[1] == 0x100000);
    CHECK(gA5.sizes[2] == sotc::kInitA5cd90Object70Bytes);
    CHECK(gA5.nameres == 1 && gA5.c95580 == 2);
    CHECK(gA5.c14d2d0 == 2 && gA5.c14ccc0 == 2 && gA5.c14ccb0 == 2);
    CHECK(gA5.c9892d0 == 1 && gA5.c946590 == 1 && gA5.c946650 == 1);
    // The fixture's config flag byte is 1 by this point, so the set-flag variant is selected.
    CHECK(gA5.config_tag == sotc::kInitA5cd90ConfigFlagSetRva);  // tag only, never dereferenced
    CHECK(gA5.c17070 == 1 && gA5.c3b5c0 == 1);
    CHECK(gA5.once == 2 && gA5.thread == 1 && gA5.mutex == 1);
    CHECK(gA5.keycreate == 1 && gA5.onceend == 1 && gA5.nameres == 1);
    std::uint64_t block0 = 0;
    std::memcpy(&block0, gA5.block.data(), sizeof block0);
    CHECK(block0 == sotc::kInitA5cd90Callback0);
    CHECK(std::memcmp(object.data() + 0x10, &gA5.resource, sizeof gA5.resource) == 0);
    CHECK(object[0x18] == 0 && object[0x20] == 1 && object[0] == 1);
    CHECK(r.call_log.records[0].callee_rva == 0x9854f0);
    CHECK(r.call_log.records[3].callee_rva == 0x991c70);
    CHECK(r.call_log.records[6].callee_rva == 0x14c2f0);
    CHECK(r.call_log.records[8].callee_rva == 0x14ccb0);
    CHECK(r.call_log.records[14].callee_rva == 0x14cf20);
    CHECK(r.call_log.records[17].callee_rva == 0x95580);
    CHECK(r.call_log.records[27].callee_rva == 0x12820);
    CHECK(r.call_log.records[32].callee_rva == 0x17070);
    CHECK(r.call_log.records[33].callee_rva == 0x3b5c0);

    // Replacement path: this+0x10 old != resource, this+0x18 set, old task at +0xa4f8.
    gA5.c14d880 = 0;
    gA5.c14d2d0 = 0;
    gA5.virtual_release = 0;
    const std::uint64_t old_resource = 0x1111;
    const std::uint64_t old_task = 0x2222;
    std::memcpy(object.data() + 0x10, &old_resource, sizeof old_resource);
    std::memcpy(object.data() + 0xa4f8, &old_task, sizeof old_task);
    object[0x18] = 1;
    object[0x20] = 1;
    const auto replace = sotc::run_init_a5cd90_body(hooks);
    CHECK(replace.ok);
    CHECK(gA5.c14d880 == 1);
    CHECK(gA5.c14d2d0 >= 1);
    CHECK(gA5.virtual_release == 1 && gA5.old_release == old_task);
    CHECK(replace.object_a4f8 == reinterpret_cast<std::uintptr_t>(gA5_task.data()));
    CHECK(object[0x18] == 0);

    // Descriptor flag zero: the whole resource block is skipped.
    object.fill(0);
    std::uint64_t zero_name = 0;
    std::memcpy(gA5.desc.data(), &zero_name, sizeof zero_name);
    gA5.c3b5c0 = 0;
    const auto skip = sotc::run_init_a5cd90_body(hooks);
    CHECK(skip.ok);
    CHECK(skip.calls_total == 11);
    CHECK(skip.resource == nullptr && !skip.resource_ready);
    CHECK(skip.field_08 == 0 && skip.field_69c8 == 0);
    CHECK(skip.byte0 == 1 && skip.object_a4f8 != 0);

    // Canary mismatch invokes the pinned trap hook and is reported.
    object.fill(0);
    gA5.flip_canary = true;
    gA5.canary_calls = 0;
    const auto canary = sotc::run_init_a5cd90_body(hooks);
    CHECK(canary.ok && canary.stack_check_failed);
    CHECK(canary.trap_calls == 1 && gA5.trap == 1);

    auto bad = hooks;
    bad.canary = nullptr;
    const auto missing = sotc::run_init_a5cd90_body(bad);
    CHECK(!missing.ok);
    CHECK(std::strcmp(missing.blocked, "state_missing") == 0);

    bad = hooks;
    bad.alloc_12820 = [](std::size_t, std::uint32_t, std::uint32_t, std::uint32_t, void**) {
        return blocked_outcome("fixture_blocker", "allocator refused");
    };
    const auto refused = sotc::run_init_a5cd90_body(bad);
    CHECK(!refused.ok);
    CHECK(std::strcmp(refused.blocked, "alloc_12820_refused") == 0);
}

}  // namespace

int main() {
    CHECK(sotc::kInit56dcb0BodySize == 132);
    CHECK(sotc::kInit5c64c0BodySize == 291);
    CHECK(sotc::kStaffroll7f7220Size == 54);
    CHECK(sotc::kStaffroll7f6f90Size == 656);
    CHECK(sotc::kInit946880BodySize == 353);
    CHECK(sotc::kInit9469f0BodySize == 360);
    CHECK(sotc::kInit5d5410BodySize == 0x1a4);
    CHECK(sotc::kInit8be60BodySize == 0x223);
    CHECK(sotc::kInit8f520BodySize == 0x10d);
    CHECK(sotc::kInit8f1d0BodySize == 0xab);
    CHECK(sotc::kInit96f20BodySize == 41);
    CHECK(sotc::kInit560b60BodySize == 0x323);
    CHECK(sotc::kInitA5cd90BodySize == 0x477);
    test_96f20_body();
    test_560b60_body();
    test_a5cd90_body();
    test_56dcb0();
    test_946880_chain();
    test_5d5410_builders();
    test_5d5410_chain();
    test_8be60_paths();
    test_7f7220();
    test_7f6f90();
    test_5c64c0();
    std::puts("init_post_bodies: ok");
    return 0;
}
