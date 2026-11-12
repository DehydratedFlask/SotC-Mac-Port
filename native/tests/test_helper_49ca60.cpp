// CPU-01-HELPER-CALLEE-MODELS-04: fixture for the dword ranged console-variable constructors 0x49ca60 / 0x49cc60 (startup slot 451).
// ELF pins: tools/lift/check_helper_49ca60.py. FIXTURE_VERIFIED. Cross-checked against the already-native float twin 0xc300.
#include <cstring>

#include "core/helper_49ca60_ranged_dword_var.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

namespace {

void run_variant(RangedDwordVariant variant, std::uint64_t vptr_table, std::uint64_t cls) {
    CHECK(ranged_dword_vptr(variant) == vptr_table + 0x10 && ranged_dword_class(variant) == cls);
    ConCmdRangedVar v;
    poison(v);
    std::uint32_t cell = 0;
    const std::uint8_t lo[4] = {1, 0, 0, 0};
    const std::uint8_t hi[4] = {0x10, 0x27, 0, 0};
    RangedVarArgs args = make_args(cls, 0x1234, 0x77, 0, 0);
    const SharedStr name = view("gRange");
    const SharedStr desc = view("");
    const RangedDwordVarResult r = helper_ranged_dword_var_49ca60(v, variant, name, &cell, lo, hi, 1, args, desc);
    CHECK(r.ok && r.refusal == HelperRefusal::none);
    CHECK(v.base.vptr == vptr_table + 0x10 && v.base.flags == 1 && v.base.name.size == 6);
    CHECK(v.value_ref == &cell);
    CHECK(std::memcmp(v.min_value, lo, 4) == 0 && v.min_engaged == 1 && std::memcmp(v.max_value, hi, 4) == 0 && v.max_engaged == 1);
    CHECK(v.args[0] == 0 && v.args[1] == 0x1111 && v.args[2] == 0x2222 && v.args[3] == 0);
    CHECK(v.fn_payload == 0x1234 && v.fn_vptr == cls && v.fn_aux == 0x77);
    // the callee never writes the value cell, the default, or the bytes the original leaves alone
    CHECK(cell == 0);
    const auto* raw = reinterpret_cast<const std::uint8_t*>(&v);
    CHECK(raw[offsetof(ConCmdRangedVar, gap71)] == kPoison && raw[offsetof(ConCmdRangedVar, gap79)] == kPoison);
    CHECK(raw[offsetof(ConCmdRangedVar, value)] == kPoison);
    CHECK(raw[offsetof(ConCmdRangedVar, fn_inline)] == kPoison && raw[offsetof(ConCmdRangedVar, gapb8)] == kPoison);

    // q[3] != 0: the first qword becomes the address of the copy inside the object (self reference), q[1..3] are copied.
    ConCmdRangedVar w;
    poison(w);
    RangedVarArgs sargs = make_args(cls, 5, 6, 0xdead, 0x49ca60);
    CHECK(helper_ranged_dword_var_49ca60(w, variant, name, &cell, lo, hi, 0, sargs, desc).ok);
    CHECK(w.args[0] == reinterpret_cast<std::uint64_t>(&w.args[0]) && w.args[3] == 0x49ca60 && w.args[1] == 0x1111);
    CHECK(w.base.flags == 0);

    // refusals write nothing
    ConCmdRangedVar x;
    poison(x);
    RangedVarArgs bad = make_args(variant == RangedDwordVariant::a_49ca60 ? kRangedDwordBClass : kRangedDwordAClass);
    RangedDwordVarResult rr = helper_ranged_dword_var_49ca60(x, variant, name, &cell, lo, hi, 1, bad, desc);
    CHECK(!rr.ok && rr.refusal == HelperRefusal::unadmitted_arg_functor && all_poison(x));
    RangedVarArgs zero = make_args(0);
    rr = helper_ranged_dword_var_49ca60(x, variant, name, &cell, lo, hi, 1, zero, desc);
    CHECK(!rr.ok && rr.refusal == HelperRefusal::unadmitted_arg_functor && all_poison(x));
    rr = helper_ranged_dword_var_49ca60(x, variant, name, &cell, nullptr, hi, 1, args, desc);
    CHECK(!rr.ok && rr.refusal == HelperRefusal::wrong_helper && all_poison(x));
    rr = helper_ranged_dword_var_49ca60(x, variant, name, &cell, lo, nullptr, 1, args, desc);
    CHECK(!rr.ok && rr.refusal == HelperRefusal::wrong_helper && all_poison(x));
}

}  // namespace

int main() {
    CHECK(kRangedDwordACtorRva == 0x49ca60 && kRangedDwordBCtorRva == 0x49cc60 && kRangedDwordCtorSize == 352);
    CHECK(kRangedDwordAVptrTable == 0x15bfb20 && kRangedDwordBVptrTable == 0x15bfb98);
    CHECK(kRangedDwordAClass == 0x15bfce8 && kRangedDwordBClass == 0x15bfd80);
    CHECK(kRangedDwordAFnCopyRva == 0x49cc00 && kRangedDwordAFnCloneRva == 0x49cc20 && kRangedDwordAFnMoveRva == 0x49cc40 &&
          kRangedDwordAFnDestroyRva == 0x49cc50);
    CHECK(kRangedDwordBFnCopyRva == 0x49ce00 && kRangedDwordBFnCloneRva == 0x49ce20 && kRangedDwordBFnMoveRva == 0x49ce40 &&
          kRangedDwordBFnDestroyRva == 0x49ce50);
    run_variant(RangedDwordVariant::a_49ca60, kRangedDwordAVptrTable, kRangedDwordAClass);
    run_variant(RangedDwordVariant::b_49cc60, kRangedDwordBVptrTable, kRangedDwordBClass);

    // The float twin 0xc300 is the same body with different constants: with equal inputs the object differs only in vptr and class.
    ConCmdRangedVar a, f;
    poison(a);
    poison(f);
    std::uint32_t cell = 0;
    const float flo = 0.25f, fhi = 4.0f;
    std::uint8_t lob[4], hib[4];
    std::memcpy(lob, &flo, 4);
    std::memcpy(hib, &fhi, 4);
    const SharedStr name = view("gTwin");
    const SharedStr desc = view("");
    CHECK(helper_ranged_dword_var_49ca60(a, RangedDwordVariant::a_49ca60, name, &cell, lob, hib, 1,
                                         make_args(kRangedDwordAClass), desc).ok);
    CHECK(init_ranged_float_var_c300(f, name, &cell, flo, fhi, 1, make_args(kRangedFloatArgFnVtbl), desc));
    CHECK(std::memcmp(a.min_value, f.min_value, 4) == 0 && std::memcmp(a.max_value, f.max_value, 4) == 0);
    CHECK(a.min_engaged == f.min_engaged && a.max_engaged == f.max_engaged && a.fn_payload == f.fn_payload && a.fn_aux == f.fn_aux);
    CHECK(std::memcmp(a.args, f.args, sizeof a.args) == 0 && a.fn_vptr != f.fn_vptr && a.base.vptr != f.base.vptr);
    std::puts("OK test_helper_49ca60");
    return 0;
}
