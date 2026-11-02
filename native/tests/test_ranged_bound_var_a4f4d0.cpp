// CPU-01-RANGED-BOUND-VAR-A4F4D0-01: fixture for the shared console-variable constructor family
// 0xa4f4d0 / 0xa4fec0 / 0xa50280 (wrappers) over 0xa4f690 / 0xa50080 / 0xa50440 (leaves).
//
// Covers: the three pinned class rows and their constants; the full store sequence and the exact set
// of bytes each build writes (and the six regions it must leave alone); the value/default-from-cell
// step; the argument copy with the q[3] self-pointer rule; the functor payload/vptr/aux net state;
// the 0x20d60 registration and its order (last); and every refusal path, which must write nothing.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/ranged_bound_var_a4f4d0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;

template <class T>
void poison(T& obj) { std::memset(static_cast<void*>(&obj), kPoison, sizeof obj); }

template <class T>
bool region_poison(const T& obj, std::size_t off, std::size_t len) {
    const auto* b = reinterpret_cast<const std::uint8_t*>(&obj);
    for (std::size_t i = 0; i < len; ++i) if (b[off + i] != kPoison) return false;
    return true;
}

template <class T>
bool all_poison(const T& obj) { return region_poison(obj, 0, sizeof obj); }

std::uint32_t dword_at(const void* obj, std::size_t off) {
    std::uint32_t v;
    std::memcpy(&v, static_cast<const std::uint8_t*>(obj) + off, 4);
    return v;
}

SharedStr lit(const char* text) {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(std::strlen(text)), 0,
                     nullptr};
}

RangedVarArgs zero_args(std::uint64_t tag) {
    RangedVarArgs a{};
    a.fn_payload = 0;
    a.fn_vptr = tag;
    a.fn_aux = 0;
    return a;
}

// The manager insert contract probe: it must observe the object already fully built, because the
// wrapper's 0x20d60 call is the last step of the constructor.
struct Probe {
    static bool insert(void* manager, ConCmdBase& cmd) noexcept {
        auto* var = reinterpret_cast<ConCmdRangedVar*>(&cmd);
        ++calls;
        saw_final_vptr = var->base.vptr == kRangedBoundVarClasses[2].final_vptr;
        saw_value = var->value == 0 && var->default_value == 0;
        saw_bounds = var->min_engaged == 1 && var->max_engaged == 1;
        saw_fn = var->fn_vptr == kRangedBoundVarClasses[2].fn_vtbl;
        (void)manager;
        return true;
    }
    static int calls;
    static bool saw_final_vptr;
    static bool saw_value;
    static bool saw_bounds;
    static bool saw_fn;
};
int Probe::calls = 0;
bool Probe::saw_final_vptr = false;
bool Probe::saw_value = false;
bool Probe::saw_bounds = false;
bool Probe::saw_fn = false;

// Everything the two bodies share, in the order of the pinned store sequence.
void check_built(const ConCmdRangedVar& var, const RangedBoundVarClass& cls, std::uint32_t min_cell,
                 std::uint32_t max_cell, std::uint32_t value_cell, const void* value_ref) {    CHECK(var.base.vptr == cls.final_vptr);
    CHECK(var.base.flags == kRangedBoundVarFlags);
    CHECK(var.base.registered == 1);
    CHECK(var.value_ref == value_ref);
    CHECK(dword_at(&var, 0x68) == value_cell);                 // default from the cell
    CHECK(dword_at(&var, 0xd0) == value_cell);                 // value from the same cell
    CHECK(dword_at(&var, 0x6c) == min_cell);                   // min copied verbatim
    CHECK(dword_at(&var, 0x74) == max_cell);                   // max copied verbatim
    CHECK(var.min_engaged == 1 && var.max_engaged == 1);       // both bounds engaged
    CHECK(var.fn_vptr == cls.fn_vtbl);
    CHECK(var.fn_payload == 0 && var.fn_aux == 0);
    for (std::uint64_t q : var.args) CHECK(q == 0);
    // The six regions neither body ever writes.
    CHECK(region_poison(var, 0x19, 3));   // ConCmdBase registered-gap
    CHECK(region_poison(var, 0x71, 3));   // min pad
    CHECK(region_poison(var, 0x79, 7));   // max pad
    CHECK(region_poison(var, 0xa0, 0x10));  // functor inline buffer
    CHECK(region_poison(var, 0xb8, 8));   // functor pad
    CHECK(region_poison(var, 0xd4, 4));   // tail pad
}

void section_constants() {
    CHECK(kRangedBoundVarClassCount == 3 && kRangedBoundVarClasses.size() == 3);
    CHECK(kRangedBoundWrapperSize == 0x1b4 && kRangedBoundLeafSize == 0x160);
    CHECK(kRangedBoundWrapperInstructions == 99 && kRangedBoundLeafInstructions == 90);
    CHECK(kRangedBoundWrapperIndirectCalls == 6 && kRangedBoundLeafIndirectCalls == 5);
    CHECK(kRangedBoundFunctorSlotCalls == 11);
    CHECK(kRangedBoundVarFlags == 1);
    CHECK(kRangedBoundRegisterRva == 0x20d60 && kRangedBoundBaseCtorRva == 0x20af0);
    CHECK(kRangedBoundStackChkFailRva == 0xf8f3a0);

    const std::uint64_t want[3][8] = {
        {0xa4f4d0, 0xa4f690, 0x15d1a20, 0x15d1b78, 0x15d1ba8, 0xa4f4c0, 0xa47d20, 0xa4f7f0},
        {0xa4fec0, 0xa50080, 0x15d1a98, 0x15d1c30, 0x15d1c60, 0xa4feb0, 0xa48240, 0xa501e0},
        {0xa50280, 0xa50440, 0x15d1b10, 0x15d1cc8, 0x15d1cf8, 0xa50270, 0xa4ab50, 0xa505a0},
    };
    for (std::size_t i = 0; i < 3; ++i) {
        CHECK(kRangedBoundVarClasses[i].wrapper_rva == want[i][0]);
        CHECK(kRangedBoundVarClasses[i].leaf_rva == want[i][1]);
        CHECK(kRangedBoundVarClasses[i].mid_vptr == want[i][2]);
        CHECK(kRangedBoundVarClasses[i].fn_vtbl == want[i][3]);
        CHECK(kRangedBoundVarClasses[i].final_vptr == want[i][4]);
        CHECK(kRangedBoundVarClasses[i].functor_destroy_rva == want[i][5]);
        CHECK(kRangedBoundVarClasses[i].dtor_rva == want[i][6]);
        CHECK(kRangedBoundVarClasses[i].non_deleting_dtor_rva == want[i][7]);
        // The leaf's frame ends exactly where the class's non-deleting destructor starts.
        CHECK(kRangedBoundVarClasses[i].leaf_rva + kRangedBoundLeafSize ==
              kRangedBoundVarClasses[i].non_deleting_dtor_rva);
        // `ranged_bound_var_class_for` returns the pinned row itself, never a copy.
        CHECK(ranged_bound_var_class_for(want[i][0]) == &kRangedBoundVarClasses[i]);
        // Distinct identities: an accidental duplicate would make the row guards meaningless.
        for (std::size_t j = 0; j < i; ++j) {
            CHECK(kRangedBoundVarClasses[j].mid_vptr != kRangedBoundVarClasses[i].mid_vptr);
            CHECK(kRangedBoundVarClasses[j].final_vptr != kRangedBoundVarClasses[i].final_vptr);
            CHECK(kRangedBoundVarClasses[j].fn_vtbl != kRangedBoundVarClasses[i].fn_vtbl);
        }
    }
    CHECK(ranged_bound_var_class_for(0xa4f4d4) == nullptr);
    CHECK(ranged_bound_var_class_for(0) == nullptr);
}

void section_build_three() {
    const std::uint32_t max_cell[3] = {5, 0x12, 0x14};
    // One registry for all three, exactly like the startup path: one pending list, initialised once.
    ConCmdRegistry reg;
    std::array<ConCmdRangedVar, 3> vars{};
    std::array<std::int32_t, 3> cells{};
    for (std::size_t i = 0; i < 3; ++i) {
        const RangedBoundVarClass& cls = kRangedBoundVarClasses[i];
        ConCmdRangedVar& var = vars[i];
        poison(var);
        const RangedBoundVarResult r =
            init_ranged_bound_var_a4f4d0(var, cls, lit("name"), &cells[i], 0, max_cell[i],
                                         kRangedBoundVarFlags, zero_args(cls.fn_vtbl), lit("desc"), reg);
        CHECK(r.ok);
        CHECK(r.functor_slot_calls == kRangedBoundFunctorSlotCalls);
        CHECK(r.value_from_cell);
        CHECK(r.registration.ok && r.registration.path == ConCmdRegisterPath::pending_list);
        CHECK(r.registration.list_initialised == (i == 0));
        check_built(var, cls, 0, max_cell[i], 0, &cells[i]);
        CHECK(var.base.name.size == 4 && std::memcmp(var.base.name.data, "name", 4) == 0);
        CHECK(var.base.description.size == 4 && std::memcmp(var.base.description.data, "desc", 4) == 0);
        CHECK(reg.pending.size() == i + 1);
        CHECK(reg.atexit_log.size() == 0);   // no __cxa_atexit lives in this family
    }
    // FIFO link order: the three objects are queued in construction order.
    CHECK(reg.pending.first() == &vars[0].base.pending);
}

void section_value_cell() {
    const RangedBoundVarClass& cls = kRangedBoundVarClasses[0];
    // A non-zero cell: the wrapper copies it twice (value and default) and leaves min/max alone.
    for (std::uint32_t cell : {0u, 1u, 7u, 0xdeadbeefu}) {
        ConCmdRangedVar var;
        poison(var);
        ConCmdRegistry reg;
        std::uint32_t live = cell;
        const RangedBoundVarResult r =
            init_ranged_bound_var_a4f4d0(var, cls, lit("n"), &live, 3, 9, 1,
                                         zero_args(cls.fn_vtbl), lit("d"), reg);
        CHECK(r.ok);
        CHECK(var.value == cell && var.default_value == cell);
        CHECK(dword_at(&var, 0x6c) == 3 && dword_at(&var, 0x74) == 9);
        CHECK(var.value_ref == &live);
        // The object never aliases its own +0xd0: the caller's cell is an external object.
        CHECK(var.value_ref != static_cast<const void*>(&var.value));
    }
}

void section_args_copy() {
    const RangedBoundVarClass& cls = kRangedBoundVarClasses[1];
    {
        // q[3] == 0: all four qwords are copied verbatim.
        ConCmdRangedVar var;
        poison(var);
        ConCmdRegistry reg;
        std::int32_t cell = 0;
        RangedVarArgs args = zero_args(cls.fn_vtbl);
        args.q[0] = 0x11;
        args.q[1] = 0x22;
        args.q[2] = 0x33;
        args.q[3] = 0;
        const RangedBoundVarResult r =
            init_ranged_bound_var_a4f4d0(var, cls, lit("n"), &cell, 0, 1, 1, args, lit("d"), reg);
        CHECK(r.ok);
        CHECK(var.args[0] == 0x11 && var.args[1] == 0x22 && var.args[2] == 0x33 && var.args[3] == 0);
    }
    {
        // q[3] != 0: the leaf rewrites q[0] with the object's own +0x80 address.
        ConCmdRangedVar var;
        poison(var);
        ConCmdRegistry reg;
        std::int32_t cell = 0;
        RangedVarArgs args = zero_args(cls.fn_vtbl);
        args.q[0] = 0x11;
        args.q[3] = 0x1;
        const RangedBoundVarResult r =
            init_ranged_bound_var_a4f4d0(var, cls, lit("n"), &cell, 0, 1, 1, args, lit("d"), reg);
        CHECK(r.ok);
        CHECK(var.args[0] == reinterpret_cast<std::uint64_t>(&var.args[0]));
        CHECK(var.args[3] == 1);
    }
    {
        // The functor net state: payload and aux follow the caller, vptr is the class tag.
        for (const RangedBoundVarClass& row : kRangedBoundVarClasses) {
            ConCmdRangedVar var;
            poison(var);
            ConCmdRegistry reg;
            std::int32_t cell = 0;
            RangedVarArgs args = zero_args(row.fn_vtbl);
            args.fn_payload = 0x1122334455667788ull;
            args.fn_aux = 0x99aabbccddeeff00ull;
            const RangedBoundVarResult r =
                init_ranged_bound_var_a4f4d0(var, row, lit("n"), &cell, 0, 1, 1, args, lit("d"), reg);
            CHECK(r.ok);
            CHECK(var.fn_payload == args.fn_payload && var.fn_aux == args.fn_aux);
            CHECK(var.fn_vptr == row.fn_vtbl);
        }
    }
}

void section_refusals_write_nothing() {
    for (const RangedBoundVarClass& row : kRangedBoundVarClasses) {
        // (a) a functor tag that is not this instantiation's.
        for (const RangedBoundVarClass& other : kRangedBoundVarClasses) {
            if (other.fn_vtbl == row.fn_vtbl) continue;
            ConCmdRangedVar var;
            poison(var);
            ConCmdRegistry reg;
            std::int32_t cell = 0;
            const RangedBoundVarResult r = init_ranged_bound_var_a4f4d0(
                var, row, lit("n"), &cell, 0, 1, 1, zero_args(other.fn_vtbl), lit("d"), reg);
            CHECK(!r.ok && all_poison(var) && reg.pending.size() == 0 && reg.atexit_log.size() == 0);
        }
        // (b) a descriptor the caller assembled from two different rows.
        RangedBoundVarClass foreign = row;
        for (const RangedBoundVarClass& other : kRangedBoundVarClasses) {
            if (other.final_vptr == row.final_vptr) continue;
            foreign.final_vptr = other.final_vptr;
            ConCmdRangedVar var;
            poison(var);
            ConCmdRegistry reg;
            std::int32_t cell = 0;
            const RangedBoundVarResult r = init_ranged_bound_var_a4f4d0(
                var, foreign, lit("n"), &cell, 0, 1, 1, zero_args(row.fn_vtbl), lit("d"), reg);
            CHECK(!r.ok && all_poison(var) && reg.pending.size() == 0);
            foreign = row;
        }
        // (c) a null value cell: the wrapper dereferences it twice in the original.
        ConCmdRangedVar var;
        poison(var);
        ConCmdRegistry reg;
        const RangedBoundVarResult r = init_ranged_bound_var_a4f4d0(
            var, row, lit("n"), nullptr, 0, 1, 1, zero_args(row.fn_vtbl), lit("d"), reg);
        CHECK(!r.ok && all_poison(var) && reg.pending.size() == 0);
    }
}

void section_registration() {
    const RangedBoundVarClass& cls = kRangedBoundVarClasses[2];
    ConCmdRangedVar var;
    poison(var);
    ConCmdRegistry reg;
    std::int32_t cell = 0;
    const RangedBoundVarResult r = init_ranged_bound_var_a4f4d0(
        var, cls, lit("n"), &cell, 0, 1, 1, zero_args(cls.fn_vtbl), lit("d"), reg);
    CHECK(r.ok && reg.pending.size() == 1 && reg.pending.first() == &var.base.pending);
    // 0x20d60 is a register-once guard: a second call is a no-op, not a duplicate link.
    const ConCmdRegisterResult again = register_concmd_20d60(var.base, reg);
    CHECK(again.ok && again.path == ConCmdRegisterPath::already_registered);
    CHECK(reg.pending.size() == 1);

    // A non-null manager cell without an insert contract fails visibly and links nothing.
    ConCmdRangedVar var2;
    poison(var2);
    ConCmdRegistry reg2;
    int token = 0;
    reg2.manager.manager = &token;
    const RangedBoundVarResult r2 = init_ranged_bound_var_a4f4d0(
        var2, cls, lit("n"), &cell, 0, 1, 1, zero_args(cls.fn_vtbl), lit("d"), reg2);
    CHECK(!r2.ok && !r2.registration.ok && r2.registration.path == ConCmdRegisterPath::manager);
    CHECK(reg2.pending.size() == 0 && var2.base.registered == 1);

    // Registration happens LAST: the insert hook sees the fully built object.
    Probe::calls = 0;
    Probe::saw_final_vptr = Probe::saw_value = Probe::saw_bounds = Probe::saw_fn = false;
    ConCmdRangedVar var3;
    poison(var3);
    ConCmdRegistry reg3;
    reg3.manager.manager = &token;
    reg3.manager.insert = &Probe::insert;
    const RangedBoundVarResult r3 = init_ranged_bound_var_a4f4d0(
        var3, cls, lit("n"), &cell, 0, 1, 1, zero_args(cls.fn_vtbl), lit("d"), reg3);
    CHECK(r3.ok && r3.registration.path == ConCmdRegisterPath::manager);
    CHECK(Probe::calls == 1 && Probe::saw_final_vptr && Probe::saw_value && Probe::saw_bounds &&
          Probe::saw_fn);
}

}  // namespace

int main() {
    section_constants();
    section_build_three();
    section_value_cell();
    section_args_copy();
    section_refusals_write_nothing();
    section_registration();
    std::puts("OK test_ranged_bound_var_a4f4d0");
    return 0;
}
