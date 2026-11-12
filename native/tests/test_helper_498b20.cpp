// CPU-01-SLOT408-PREREQS: fixture for the ranged var dtors. ELF pins: tools/lift/check_helper_498b20.py.
#include <string>
#include <vector>

#include "core/helper_498b20_ranged_var_dtors.hpp"
#include "helper_test_util.hpp"

using namespace sotc;

namespace {
std::string g_log;
std::uint64_t g_vtbl = 0;
void* g_fn = nullptr;
void* g_base = nullptr;
void on_fn(std::uint64_t vtbl, void* block) { g_log += "fn;"; g_vtbl = vtbl; g_fn = block; }
void on_base(void* object) { g_log += "base;"; g_base = object; }
}  // namespace

int main() {
    CHECK(kRangedVarDtorFnOffset == 0xa0 && kRangedVarDtorFnVptrOffset == 0xc0 && kRangedVarDtorDestroySlotOffset == 0x18);
    CHECK(kRangedVarBaseDtorRva == 0x20b50 && kRangedVarObjectSize == 0xc8 && kRangedVarDtors.size() == 5);
    CHECK(find_ranged_var_dtor(0x498b20)->vptr == 0x15bfb20 + 0x10 && find_ranged_var_dtor(0x498b60)->vptr == 0x15bfb98 + 0x10);
    CHECK(find_ranged_var_dtor(0x23d20)->vptr == 0x1594fd0 + 0x10 && find_ranged_var_dtor(0x13a090)->vptr == 0x1598240 + 0x10);
    CHECK(find_ranged_var_dtor(0x140010)->vptr == 0x15982c8 && find_ranged_var_dtor(0x140011) == nullptr);
    const RangedVarDtorHooks hooks{on_fn, on_base};

    for (const RangedVarDtorRow& row : kRangedVarDtors) {
        alignas(8) std::uint8_t object[kRangedVarObjectSize];
        std::memset(object, helper_test::kPoison, sizeof object);
        const std::uint64_t fn_vtbl = 0x15d5960 + row.size;
        std::memcpy(object + 0xc0, &fn_vtbl, sizeof fn_vtbl);
        g_log.clear();
        g_vtbl = 0;
        CHECK(ranged_var_dtor_run(row.rva, object, hooks));
        std::uint64_t vptr;
        std::memcpy(&vptr, object, sizeof vptr);
        CHECK(vptr == row.vptr);                                 // the vptr store
        CHECK(g_log == "fn;base;");                               // functor destroy before the base dtor
        CHECK(g_vtbl == fn_vtbl && g_fn == object + 0xa0 && g_base == object);
        for (std::size_t i = 8; i < sizeof object; ++i) if (i < 0xc0 || i >= 0xc8) CHECK(object[i] == helper_test::kPoison);   // only the vptr is written; +0xc0 is the input tag
    }

    alignas(8) std::uint8_t other[kRangedVarObjectSize] = {};
    CHECK(!ranged_var_dtor_run(0x20f0, other, hooks));           // outside the five
    CHECK(!ranged_var_dtor_run(0x498b20, other, RangedVarDtorHooks{on_fn, nullptr}));
    for (std::uint8_t b : other) CHECK(b == 0);
    std::puts("OK test_helper_498b20");
    return 0;
}
