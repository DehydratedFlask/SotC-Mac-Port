// CPU-01-SLOT408-PREREQS: fixture for the slot 451 holder dtors. ELF pins: tools/lift/check_helper_499e90.py.
#include <string>
#include <vector>

#include "core/helper_499e90_slot451_dtors.hpp"
#include "helper_test_util.hpp"

using namespace sotc;

namespace {
std::string g_log;
void note(const char* what, void* p) { char b[40]; std::snprintf(b, sizeof b, "%s@%p;", what, p); g_log += b; }
void on_free1(void* p) { note("f1", p); }
void on_free2(void* p) { note("f2", p); }
void on_crit(void* p) { note("cs", p); }
std::string tag(const char* what, void* p) { char b[40]; std::snprintf(b, sizeof b, "%s@%p;", what, p); return b; }
}  // namespace

int main() {
    CHECK(kCritsecDtor3b380Rva == 0x3b380 && kTailDtor499e50Rva == 0x499e50 && kBufferDtor499e60Rva == 0x499e60);
    CHECK(kNodeDtor499e90Rva == 0x499e90 && kSixBufferDtor49c9e0Rva == 0x49c9e0);
    CHECK(kSixBufferOffsets49c9e0[0] == 0xb8 && kSixBufferOffsets49c9e0[5] == 0x18);
    const Slot451DtorHooks hooks{on_free1, on_free2, on_crit};
    alignas(8) std::uint8_t buf[6][4];

    // 0x3b380: exactly the critical-section destroy of the object
    alignas(8) std::uint8_t obj[0xc0] = {};
    g_log.clear();
    CHECK(critsec_dtor_3b380(obj, hooks) && g_log == tag("cs", obj));
    CHECK(!critsec_dtor_3b380(obj, Slot451DtorHooks{}));

    // 0x499e60: [obj+8] freed (type 2) only when non-null, then the critsec destroy
    g_log.clear();
    CHECK(buffer_dtor_499e60(obj, hooks) && g_log == tag("cs", obj));
    void* p = buf[0];
    std::memcpy(obj + 8, &p, sizeof p);
    g_log.clear();
    CHECK(buffer_dtor_499e60(obj, hooks) && g_log == tag("f2", buf[0]) + tag("cs", obj));

    // 0x49c9e0: six buffers in offset order 0xb8,0x98,0x78,0x58,0x38,0x18 (null ones skipped), then the critsec destroy
    std::memset(obj, 0, sizeof obj);
    for (int i = 0; i < 6; ++i) { void* q = buf[i]; std::memcpy(obj + kSixBufferOffsets49c9e0[i], &q, sizeof q); }
    void* nul = nullptr;
    std::memcpy(obj + 0x78, &nul, sizeof nul);
    std::string want;
    for (int i = 0; i < 6; ++i) if (i != 2) want += tag("f2", buf[i]);
    want += tag("cs", obj);
    g_log.clear();
    CHECK(six_buffer_dtor_49c9e0(obj, hooks) && g_log == want);
    g_log.clear();
    CHECK(tail_dtor_499e50(obj, hooks) && g_log == want);          // the thunk is the same body
    CHECK(!six_buffer_dtor_49c9e0(obj, Slot451DtorHooks{on_free1, nullptr, on_crit}));

    // 0x499e90: null slot value is a no-op; otherwise outer(+0x28), inner(+8), critsec, free type 1, slot cleared
    void* slot = nullptr;
    g_log.clear();
    CHECK(node_dtor_499e90(&slot, hooks) && g_log.empty());
    alignas(8) std::uint8_t node[0x40] = {};
    slot = node;
    g_log.clear();
    CHECK(node_dtor_499e90(&slot, hooks) && g_log == tag("cs", node) + tag("f1", node) && slot == nullptr);
    void* outer = buf[1];
    void* inner = buf[2];
    std::memcpy(node + 0x28, &outer, sizeof outer);
    std::memcpy(node + 0x08, &inner, sizeof inner);
    slot = node;
    g_log.clear();
    CHECK(node_dtor_499e90(&slot, hooks) && g_log == tag("f2", buf[1]) + tag("f2", buf[2]) + tag("cs", node) + tag("f1", node));
    CHECK(slot == nullptr);
    slot = node;
    CHECK(!node_dtor_499e90(&slot, Slot451DtorHooks{}) && slot == node);   // missing hooks: nothing claimed
    std::puts("OK test_helper_499e90");
    return 0;
}
