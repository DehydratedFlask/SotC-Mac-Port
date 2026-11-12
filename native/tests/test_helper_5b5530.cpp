// CPU-01-SLOT408-PREREQS: fixture for the slot 408 vector dtors. ELF pins: tools/lift/check_helper_5b5530.py. FIXTURE_VERIFIED.
#include <string>
#include <vector>

#include "core/helper_5b5530_vector_dtors.hpp"
#include "helper_test_util.hpp"

using namespace sotc;

namespace {
std::string g_log;
std::string tag(const char* what, const void* p) { char b[40]; std::snprintf(b, sizeof b, "%s@%p;", what, p); return b; }
void on_free1(void* p) { g_log += tag("f1", p); }
void on_free2(void* p) { g_log += tag("f2", p); }
void put(void* base, std::size_t off, const void* value) { std::memcpy(static_cast<std::uint8_t*>(base) + off, &value, sizeof value); }
void put_u64(void* base, std::size_t off, std::uint64_t value) { std::memcpy(static_cast<std::uint8_t*>(base) + off, &value, sizeof value); }
std::uint64_t get_u64(const void* base, std::size_t off) { std::uint64_t v; std::memcpy(&v, static_cast<const std::uint8_t*>(base) + off, sizeof v); return v; }
}  // namespace

int main() {
    CHECK(kSsoVectorDtor5b5530Rva == 0x5b5530 && kSsoVectorDtor5b5530Size == 142 && kInlineGuardDtor5b55c0Rva == 0x5b55c0);
    CHECK(kInlineGuardDtor5b55c0Size == 23 && kVec30Dtor5c7c00Rva == 0x5c7c00 && kVec30Dtor5c7c00Size == 127);
    CHECK(kSsoElementSize == 0x28 && kVec30ElementSize == 0x30 && kSsoInlineCapacity == 0xf && kVectorHeaderOffset == 8);
    const VectorDtorHooks hooks{on_free1, on_free2};

    // ---- 0x5b5530
    alignas(8) std::uint8_t obj[0x20] = {};
    g_log.clear();
    CHECK(sso_vector_dtor_5b5530(obj, hooks) && g_log.empty());              // begin == 0: nothing at all
    alignas(8) static std::uint8_t elems[3 * 0x28];
    std::memset(elems, 0xAA, sizeof elems);
    int heap_a = 0, heap_c = 0;
    put_u64(elems + 0x00, 0x20, 0x1f);  put(elems + 0x00, 0x08, &heap_a);    // element 0: heap string (cap 0x1f >= 0x10)
    put_u64(elems + 0x28, 0x20, 0x0f);                                       // element 1: SSO (cap 0xf)
    put_u64(elems + 0x50, 0x20, 0x10);  put(elems + 0x50, 0x08, &heap_c);    // element 2: exactly 0x10 -> heap
    put(obj, 8, elems); put(obj, 0x10, elems + sizeof elems); put(obj, 0x18, elems + sizeof elems);
    g_log.clear();
    CHECK(sso_vector_dtor_5b5530(obj, hooks));
    CHECK(g_log == tag("f1", &heap_a) + tag("f1", &heap_c) + tag("f1", elems));
    for (int i = 0; i < 3; ++i) {
        CHECK(get_u64(elems, i * 0x28 + 0x18) == 0 && get_u64(elems, i * 0x28 + 0x20) == 0xf && elems[i * 0x28 + 8] == 0);
        CHECK(elems[i * 0x28 + 0x10] == 0xAA);   // bytes +0x10..+0x17 are not touched
    }
    CHECK(get_u64(obj, 8) == 0 && get_u64(obj, 0x10) == 0 && get_u64(obj, 0x18) == 0);
    // empty-but-allocated vector (begin == end): the buffer is still freed
    put(obj, 8, elems); put(obj, 0x10, elems); put(obj, 0x18, elems);
    g_log.clear();
    CHECK(sso_vector_dtor_5b5530(obj, hooks) && g_log == tag("f1", elems) && get_u64(obj, 8) == 0);
    put(obj, 8, elems); put(obj, 0x10, elems);
    CHECK(!sso_vector_dtor_5b5530(obj, VectorDtorHooks{}) && get_u64(obj, 8) != 0);   // missing hook: nothing claimed

    // ---- 0x5b55c0
    alignas(8) std::uint8_t cell[0x28] = {};
    g_log.clear();
    CHECK(inline_guard_dtor_5b55c0(cell, hooks) && g_log.empty());           // null pointer
    put(cell, 0, elems); put(cell, 0x20, elems);
    CHECK(inline_guard_dtor_5b55c0(cell, hooks) && g_log.empty());           // pointer == inline storage: not freed
    put(cell, 0x20, cell + 0x28);
    CHECK(inline_guard_dtor_5b55c0(cell, hooks) && g_log == tag("f2", elems));
    CHECK(!inline_guard_dtor_5b55c0(cell, VectorDtorHooks{}));

    // ---- 0x5c7c00
    alignas(8) std::uint8_t obj30[0x20] = {};
    g_log.clear();
    CHECK(vec30_dtor_5c7c00(obj30, hooks) && g_log.empty());
    alignas(8) static std::uint8_t e30[3 * 0x30];
    std::memset(e30, 0xCC, sizeof e30);
    int b0 = 0, b2 = 0;
    put(e30 + 0x00, 0x18, &b0);
    put_u64(e30 + 0x30, 0x18, 0);                                            // element 1: no buffer, untouched
    put(e30 + 0x60, 0x18, &b2);
    put(obj30, 8, e30); put(obj30, 0x10, e30 + sizeof e30); put(obj30, 0x18, e30 + sizeof e30);
    g_log.clear();
    CHECK(vec30_dtor_5c7c00(obj30, hooks));
    CHECK(g_log == tag("f1", &b0) + tag("f1", &b2) + tag("f1", e30));
    CHECK(get_u64(e30, 0x18) == 0 && get_u64(e30, 0x20) == 0 && get_u64(e30, 0x28) == 0 && e30[0x10] == 0xCC);
    CHECK(get_u64(e30, 0x30 + 0x20) == 0xCCCCCCCCCCCCCCCCull);               // element 1 skipped entirely
    CHECK(get_u64(obj30, 8) == 0 && get_u64(obj30, 0x10) == 0 && get_u64(obj30, 0x18) == 0);
    std::puts("OK test_helper_5b5530");
    return 0;
}
