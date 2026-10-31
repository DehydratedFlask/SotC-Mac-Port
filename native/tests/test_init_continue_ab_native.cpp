// CPU-01-INIT-1FC80-CONTINUE-AB-01: native 0x1fc80 hit/miss feeds 0x1e4c0 on continue-B.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/config_bool_lookup_1e4c0.hpp"
#include "core/init_531d60_post_base_continue_a.hpp"
#include "core/lookup_1fc80.hpp"

#define CHECK(c)                                                                                   \
    do {                                                                                           \
        if (!(c)) {                                                                                \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);                       \
            std::exit(1);                                                                          \
        }                                                                                          \
    } while (0)

namespace {

using namespace sotc;

struct NamedElem {
    std::uint8_t bytes[kLookup1fc80Stride]{};
};

struct NameTable {
    std::uint8_t pad[8]{};
    void* begin = nullptr;
    void* end = nullptr;
};

void set_name(NamedElem& elem, const char* name) {
    const char* p = name;
    std::memcpy(elem.bytes + kLookup1fc80NameOff, &p, sizeof(p));
}

void set_bool_table(NamedElem& elem, ConfigBoolEntry16* begin, ConfigBoolEntry16* end) {
    std::memcpy(elem.bytes + kConfigBoolTableBeginOffset, &begin, sizeof(begin));
    std::memcpy(elem.bytes + kConfigBoolTableEndOffset, &end, sizeof(end));
}

}  // namespace

int main() {
    NamedElem elem{};
    set_name(elem, "nointro");
    ConfigBoolEntry16 row{"unused", "false"};
    set_bool_table(elem, &row, &row + 1);

    NameTable table{};
    table.begin = elem.bytes;
    table.end = elem.bytes + kLookup1fc80Stride;
    const void* table_ptr = &table;

    Init531d60PostBaseContinueAHooks hooks{};
    hooks.rsp20_slot = &table_ptr;
    hooks.nointro_needle = "nointro";

    {
        NameTable empty{};
        const void* empty_ptr = &empty;
        Init531d60PostBaseContinueAHooks miss_hooks = hooks;
        miss_hooks.rsp20_slot = &empty_ptr;
        const auto missed = run_init_continue_ab_native_lookups(miss_hooks);
        CHECK(missed.cont_a.ok);
        CHECK(missed.cont_a.rax_from_1fc80_known);
        CHECK(missed.cont_a.branch_outcome_modelled);
        CHECK(missed.cont_a.branch_taken);
        CHECK(missed.cont_a.rax_from_1fc80 == nullptr);
        CHECK(!missed.ran_continue_b);
    }

    {
        const auto hit = run_init_continue_ab_native_lookups(hooks);
        CHECK(hit.cont_a.ok);
        CHECK(hit.cont_a.rax_from_1fc80 == elem.bytes);
        CHECK(!hit.cont_a.branch_taken);
        CHECK(hit.ran_continue_b);
        CHECK(hit.cont_b.ok);
        CHECK(hit.cont_b.eax_from_1e4c0 == 0);
        CHECK(hit.cont_b.branch_outcome_modelled);
        CHECK(hit.cont_b.branch_taken);
    }

    {
        row.value = "true";
        const auto play = run_init_continue_ab_native_lookups(hooks);
        CHECK(play.ran_continue_b);
        CHECK(play.cont_b.eax_from_1e4c0 == 1);
        CHECK(play.cont_b.branch_outcome_modelled);
        CHECK(!play.cont_b.branch_taken);
    }

    {
        Init531d60PostBaseContinueAHooks counting{};
        counting.rsp20_slot = &table_ptr;
        const auto plain = init_531d60_post_base_continue_a(counting);
        CHECK(plain.ok);
        CHECK(!plain.branch_outcome_modelled);
        CHECK(!plain.rax_from_1fc80_known);
    }

    std::printf("OK init_continue_ab_native\n");
    return 0;
}
