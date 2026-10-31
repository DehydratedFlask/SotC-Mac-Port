// CPU-01-INIT-53299D-PIN-01: merge je fall-through pin 0x53299d..0x5329ae after continue C.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/init_531d60_post_base_continue_c_after_merge.hpp"

#define CHECK(c)                                                                                   \
    do {                                                                                           \
        if (!(c)) {                                                                                \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);                       \
            std::exit(1);                                                                          \
        }                                                                                          \
    } while (0)

namespace {

using namespace sotc;

}  // namespace

int main() {
    CHECK(kInit531d60PostBaseContinueCAfterMergeRva == 0x53299d);
    CHECK(kInit531d60PostBaseContinueCAfterMergeEndRva == 0x5329ae);
    CHECK(kInit531d60PostBaseContinueCAfterMergeSize == 17);
    CHECK(kInit531d60PostBaseContinueCEndRva == kInit531d60PostBaseContinueCAfterMergeRva);

    {
        const auto plain = run_init_continue_c_after_merge({});
        CHECK(plain.cont_c.ok);
        CHECK(plain.ran_fallthrough);
        CHECK(plain.fallthrough.ok);
        CHECK(plain.fallthrough.unit_executed == 4);
        CHECK(plain.fallthrough.end_rva == 0x5329ae);
        CHECK(!plain.cont_c.merge_branch_outcome_modelled);
    }

    {
        std::uint8_t flag = 1;
        Init531d60PostBaseContinueCHooks c_hooks{};
        c_hooks.merge_flag_byte = &flag;
        const auto fall = run_init_continue_c_after_merge(c_hooks);
        CHECK(fall.cont_c.ok);
        CHECK(fall.cont_c.merge_branch_outcome_modelled);
        CHECK(!fall.cont_c.merge_branch_taken);
        CHECK(fall.ran_fallthrough);
        CHECK(fall.fallthrough.ok);
        CHECK(fall.fallthrough.call_count == 1);
    }

    {
        std::uint8_t flag = 0;
        Init531d60PostBaseContinueCHooks c_hooks{};
        c_hooks.merge_flag_byte = &flag;
        const auto taken = run_init_continue_c_after_merge(c_hooks);
        CHECK(taken.cont_c.ok);
        CHECK(taken.cont_c.merge_branch_taken);
        CHECK(!taken.ran_fallthrough);
    }

    std::printf("OK init_continue_c_after_merge\n");
    return 0;
}
