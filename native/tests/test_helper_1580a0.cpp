// CPU-01-HELPER-CALLEE-MODELS-04: fixture for the perspective builder 0x1580a0 (startup slot 408 and six other callers). ELF pins:
// tools/lift/check_helper_1580a0.py. FIXTURE_VERIFIED: the hook returns scripted values, so the arithmetic is checked against hand-derived floats.
#include <cmath>
#include <cstring>

#include "core/helper_1580a0_perspective.hpp"
#include "helper_test_util.hpp"

using namespace sotc;
using namespace helper_test;

namespace {

float g_last_arg = -1.0f;
int g_calls = 0;
float g_script = 1.0f;

float scripted_tanf(float x) { g_last_arg = x; ++g_calls; return g_script; }
float real_tanf(float x) { return std::tan(x); }

std::uint32_t bits(float f) { std::uint32_t b; std::memcpy(&b, &f, 4); return b; }

}  // namespace

int main() {
    CHECK(kPerspective1580a0Rva == 0x1580a0 && kPerspective1580a0Size == 0xc0);
    CHECK(kPerspectiveTanfPltRva == 0xf8f410 && kPerspectiveTanfGotRva == 0x15ffff8 && kPerspectiveMatrixFloats == 16);
    CHECK(kPerspectiveHalfBits == 0x3f000000 && kPerspectiveOneBits == 0x3f800000 && kPerspectiveSignBits == 0x80000000);
    CHECK(kSlot408PerspectiveCallRva == 0x5c8082 && kSlot408PerspectiveOutRva == 0x2fda41c && kSlot408Flag == 1);

    // ---- flag == 0 : conventional depth. tan(fov/2) is scripted to 0.5, so cot = 2 ---------------------------------------
    {
        float m[16];
        std::memset(m, 0xA5, sizeof m);
        const float fov = 1.0f;
        g_calls = 0; g_script = 0.5f;
        const PerspectiveResult r = helper_1580a0_perspective(m, &fov, 2.0f, 1.0f, 101.0f, 0, scripted_tanf);
        CHECK(r.ok && r.refusal == PerspectiveRefusal::none && g_calls == 1 && g_last_arg == 0.5f);
        CHECK(r.tan_argument == 0.5f && r.tan_result == 0.5f);
        CHECK(m[0] == 1.0f && m[5] == 2.0f && m[11] == 1.0f);                         // 2 / 2, 2, 1
        CHECK(m[10] == 101.0f / 100.0f);                                               // far / (far - near)
        CHECK(m[14] == -(1.0f * 101.0f) / 100.0f);                                     // -(near * far) / (far - near)
        for (int i : {1, 2, 3, 4, 6, 7, 8, 9, 12, 13, 15}) CHECK(bits(m[i]) == 0);     // positive zero, not poison
    }
    // ---- flag != 0 : the two planes swap roles in m[10] and the denominator --------------------------------------------------
    {
        float m[16];
        std::memset(m, 0xA5, sizeof m);
        const float fov = 1.0f;
        g_script = 0.5f;
        CHECK(helper_1580a0_perspective(m, &fov, 2.0f, 1.0f, 101.0f, 1, scripted_tanf).ok);
        CHECK(m[10] == 1.0f / (1.0f - 101.0f));                                        // near / (near - far)
        CHECK(m[14] == -(1.0f * 101.0f) / (1.0f - 101.0f));                            // numerator unchanged, denominator flipped
        CHECK(m[0] == 1.0f && m[5] == 2.0f && m[11] == 1.0f);
    }
    // ---- only the low byte of edx is tested -------------------------------------------------------------------------------
    {
        float a[16], b[16], c[16];
        const float fov = 0.9f;
        g_script = 0.75f;
        CHECK(helper_1580a0_perspective(a, &fov, 1.5f, 0.5f, 100.0f, 0x100u, scripted_tanf).ok);     // low byte 0
        CHECK(helper_1580a0_perspective(b, &fov, 1.5f, 0.5f, 100.0f, 0u, scripted_tanf).ok);
        CHECK(std::memcmp(a, b, sizeof a) == 0);
        CHECK(helper_1580a0_perspective(c, &fov, 1.5f, 0.5f, 100.0f, 0x1u, scripted_tanf).ok);
        CHECK(c[10] != a[10]);
        CHECK(helper_1580a0_perspective(c, &fov, 1.5f, 0.5f, 100.0f, 0xff00ff, scripted_tanf).ok && c[10] == 0.5f / (0.5f - 100.0f));
    }
    // ---- the half-angle is what tanf sees, even for a real tanf --------------------------------------------------------------
    {
        float m[16];
        const float fov = 3.0f;
        const PerspectiveResult r = helper_1580a0_perspective(m, &fov, 1.0f, 1.0f, 2.0f, 0, real_tanf);
        CHECK(r.ok && r.tan_argument == 1.5f && std::fabs(m[5] - 1.0f / std::tan(1.5f)) < 1e-6f);
    }
    // ---- slot 408 site: exact constants through the helper ------------------------------------------------------------------
    {
        float m[16];
        std::memset(m, 0xA5, sizeof m);
        g_calls = 0; g_script = 1.0f;
        const PerspectiveResult r = slot408_perspective_site(m, scripted_tanf);
        CHECK(r.ok && g_calls == 1 && g_last_arg == 0.7853982f * 0.5f);   // pi/4 halved
        CHECK(m[5] == 1.0f && m[0] == 1.0f / 1.7777778f);                                // tan scripted to 1: cot = 1
        CHECK(m[10] == 0.5f / (0.5f - 4200.0f));                                          // flag 1: near / (near - far)
        CHECK(m[14] == -(0.5f * 4200.0f) / (0.5f - 4200.0f));
        CHECK(m[11] == 1.0f);
        float direct[16];
        const float fov = 0.7853982f;
        CHECK(helper_1580a0_perspective(direct, &fov, 1.7777778f, 0.5f, 4200.0f, 1, scripted_tanf).ok);
        CHECK(std::memcmp(direct, m, sizeof m) == 0);
        CHECK(bits(fov) == kSlot408FovBits && bits(1.7777778f) == kSlot408AspectBits);
        CHECK(bits(0.5f) == kSlot408NearBits && bits(4200.0f) == kSlot408FarBits);
    }
    // ---- fail-visible refusals leave the output untouched --------------------------------------------------------------------
    {
        float m[16];
        std::memset(m, 0xA5, sizeof m);
        float snapshot[16];
        std::memcpy(snapshot, m, sizeof m);
        const float fov = 1.0f;
        g_calls = 0;
        CHECK(helper_1580a0_perspective(m, &fov, 1, 1, 2, 0, nullptr).refusal == PerspectiveRefusal::no_tanf_hook);
        CHECK(helper_1580a0_perspective(nullptr, &fov, 1, 1, 2, 0, scripted_tanf).refusal == PerspectiveRefusal::null_out);
        CHECK(helper_1580a0_perspective(m, nullptr, 1, 1, 2, 0, scripted_tanf).refusal == PerspectiveRefusal::null_fov);
        CHECK(!slot408_perspective_site(m, nullptr).ok);
        CHECK(g_calls == 0 && std::memcmp(m, snapshot, sizeof m) == 0);
    }
    std::puts("OK test_helper_1580a0");
    return 0;
}
