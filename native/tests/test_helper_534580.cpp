// CPU-01-SLOT408-PREREQS: fixture for the cell dtors 0x534580 / 0x13a0d0. ELF pins: tools/lift/check_helper_534580.py.
#include "core/helper_534580_cell_dtors.hpp"
#include "helper_test_util.hpp"

using namespace sotc;

namespace {
int g_free2 = 0, g_vdel = 0;
void* g_last = nullptr;
void on_free2(void* p) { ++g_free2; g_last = p; }
void on_vdel(void* p) { ++g_vdel; g_last = p; }
}  // namespace

int main() {
    CHECK(kCellDtor534580Rva == 0x534580 && kCellDtor534580Size == 14 && kCellDtor13a0d0Rva == 0x13a0d0 && kCellDtor13a0d0Size == 37);
    CHECK(kCellDtorFreeType2Rva == 0x170c0 && kCellDtor13a0d0VtableSlotOffset == 8);
    const CellDtorHooks hooks{on_free2, on_vdel};
    int a = 0;

    void* cell = nullptr;
    CHECK(cell_dtor_534580(&cell, hooks) && g_free2 == 0);
    cell = &a;
    CHECK(cell_dtor_534580(&cell, hooks) && g_free2 == 1 && g_last == &a);
    CHECK(cell == &a);                                     // 0x534580 does not clear the cell
    CHECK(!cell_dtor_534580(&cell, CellDtorHooks{}));      // reachable free, no hook: honest false
    CHECK(g_free2 == 1 && g_vdel == 0);

    cell = nullptr;
    CHECK(cell_dtor_13a0d0(&cell, hooks) && g_vdel == 0);
    cell = &a;
    CHECK(cell_dtor_13a0d0(&cell, hooks) && g_vdel == 1 && g_last == &a && cell == nullptr);   // 0x13a0d0 clears the cell
    cell = &a;
    CHECK(!cell_dtor_13a0d0(&cell, CellDtorHooks{}) && cell == &a && g_vdel == 1);
    std::puts("OK test_helper_534580");
    return 0;
}
