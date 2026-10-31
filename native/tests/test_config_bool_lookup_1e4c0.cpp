// CPU-01-INIT-1E4C0-SHARED-LOOKUP-01: fixture for config_bool_lookup_1e4c0 (E524/E1037).
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/config_bool_lookup_1e4c0.hpp"
#include "core/init_531d60_post_base_1e4c0_u1.hpp"
#include "core/init_531d60_post_base_continue_b.hpp"
#include "core/sndx_command_handlers.hpp"

#define CHECK(c)                                                                                   \
    do {                                                                                           \
        if (!(c)) {                                                                                \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);                       \
            std::exit(1);                                                                          \
        }                                                                                          \
    } while (0)

namespace {

using namespace sotc;

struct ConfigObject {
    std::uint8_t pad[kConfigBoolTableBeginOffset]{};
    ConfigBoolEntry16* begin = nullptr;
    ConfigBoolEntry16* end = nullptr;
};

void test_parse_tokens() {
    CHECK(parse_config_bool_cstr("true"));
    CHECK(parse_config_bool_cstr("TRUE"));
    CHECK(parse_config_bool_cstr("yes"));
    CHECK(parse_config_bool_cstr("Yes"));
    CHECK(!parse_config_bool_cstr("false"));
    CHECK(!parse_config_bool_cstr("FALSE"));
    CHECK(!parse_config_bool_cstr("no"));
    CHECK(!parse_config_bool_cstr("0"));
    CHECK(parse_config_bool_cstr("1"));
    CHECK(parse_config_bool_cstr("2"));
    CHECK(parse_config_bool_to_int01("true") == 1);
    CHECK(parse_config_bool_to_int01("no") == 0);
    CHECK(!parse_config_bool_cstr(nullptr));
}

void test_object_table() {
    ConfigBoolEntry16 rows[2] = {
        {"alpha", "false"},
        {"beta", "true"},
    };
    ConfigObject obj{};
    obj.begin = rows;
    obj.end = rows + 2;

    CHECK(config_bool_lookup_object(&obj, "beta", 99, 0) == 1);
    CHECK(config_bool_lookup_object(&obj, "missing", 1, 0) == 1);  // index 1 -> true
    CHECK(config_bool_lookup_object(&obj, nullptr, 0, 0) == 0);    // index 0 -> false
    CHECK(config_bool_lookup_object(&obj, nullptr, 9, 1) == 1);    // OOB -> default 1
    CHECK(config_bool_lookup_init_continue_b(&obj) == 0);          // index 0 default ecx=1 unused when value set
}

void test_args_span() {
    ConfigBoolEntry16 record{};
    record.name = "mute";
    record.value = "yes";
    SndxArgsSpan span{&record, reinterpret_cast<const std::uint8_t*>(&record) + kConfigBoolEntrySize};
    CHECK(span.record_count() == 1);
    CHECK(config_bool_lookup_args_span(span, 0, false));
    CHECK(!config_bool_lookup_args_span(span, 1, false));
    record.value = "0";
    CHECK(!config_bool_lookup_args_span(span, 0, true));
}

void test_sndx_native_hooks() {
    ConfigBoolEntry16 record{};
    record.value = "true";
    SndxArgsSpan span{&record, reinterpret_cast<const std::uint8_t*>(&record) + kConfigBoolEntrySize};
    const SndxGetBoolArgHooks hooks = default_native_sndx_get_bool_arg_hooks();
    CHECK(hooks.complete());
    CHECK(hooks.lookup(hooks.host, span, 0, false));
}

void test_continue_b_native_branch() {
    ConfigBoolEntry16 rows[1] = {{"nointro", "false"}};
    ConfigObject obj{};
    obj.begin = rows;
    obj.end = rows + 1;

    Init531d60PostBase1e4c0U1Hooks u1{};
    u1.incoming_rax_standin = &obj;
    u1.call_1e4c0 = [](void* rdi) -> int {
        return config_bool_lookup_init_continue_b(rdi);
    };

    const Init531d60PostBase1e4c0U1Result e4_false =
        init_531d60_post_base_1e4c0_u1(u1);
    Init531d60PostBaseContinueBHooks hooks{};
    hooks.incoming_eax_from_1e4c0_u1 = e4_false.eax_return;
    hooks.incoming_eax_known = e4_false.eax_known;

    const auto taken = init_531d60_post_base_continue_b(hooks);
    CHECK(taken.ok);
    CHECK(taken.eax_known);
    CHECK(taken.eax_from_1e4c0 == 0);
    CHECK(taken.branch_outcome_modelled);
    CHECK(taken.branch_taken);

    rows[0].value = "true";
    const Init531d60PostBase1e4c0U1Result e4_true = init_531d60_post_base_1e4c0_u1(u1);
    hooks.incoming_eax_from_1e4c0_u1 = e4_true.eax_return;
    hooks.incoming_eax_known = e4_true.eax_known;
    const auto fallthrough = init_531d60_post_base_continue_b(hooks);
    CHECK(fallthrough.eax_from_1e4c0 == 1);
    CHECK(fallthrough.branch_outcome_modelled);
    CHECK(!fallthrough.branch_taken);
}

}  // namespace

int main() {
    test_parse_tokens();
    test_object_table();
    test_args_span();
    test_sndx_native_hooks();
    test_continue_b_native_branch();
    std::printf("OK config_bool_lookup_1e4c0\n");
    return 0;
}
