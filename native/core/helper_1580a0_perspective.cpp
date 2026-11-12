// CPU-01-HELPER-CALLEE-MODELS-04: see helper_1580a0_perspective.hpp. Single-precision arithmetic in the exact instruction order of the body.
#include "core/helper_1580a0_perspective.hpp"

#include <cstring>

namespace sotc {
namespace {

float bits_to_float(std::uint32_t bits) noexcept {
    float value;
    std::memcpy(&value, &bits, sizeof value);
    return value;
}

float negate(float value) noexcept {                    // vxorps with the 0x80000000 mask
    std::uint32_t bits;
    std::memcpy(&bits, &value, sizeof bits);
    bits ^= kPerspectiveSignBits;
    return bits_to_float(bits);
}

}  // namespace

PerspectiveResult helper_1580a0_perspective(float* out, const float* fov, float aspect, float near_plane, float far_plane,
                                            std::uint32_t flag, TanfFn tanf_hook) noexcept {
    PerspectiveResult result;
    if (tanf_hook == nullptr) { result.refusal = PerspectiveRefusal::no_tanf_hook; return result; }
    if (out == nullptr) { result.refusal = PerspectiveRefusal::null_out; return result; }
    if (fov == nullptr) { result.refusal = PerspectiveRefusal::null_fov; return result; }

    result.tan_argument = *fov * bits_to_float(kPerspectiveHalfBits);        // vmulss
    const float t = tanf_hook(result.tan_argument);                          // callq 0xf8f410
    result.tan_result = t;
    const float cot = bits_to_float(kPerspectiveOneBits) / t;                // vdivss
    const float x_scale = cot / aspect;                                      // vdivss -0x1c(%rbp)

    float depth_scale;
    float denominator;
    if ((flag & 0xffu) != 0) {                                               // testb %r14b, %r14b ; jne path
        denominator = near_plane - far_plane;
        depth_scale = near_plane / denominator;
    } else {
        denominator = far_plane - near_plane;
        depth_scale = far_plane / denominator;
    }
    const float depth_offset = negate(near_plane * far_plane) / denominator;

    out[0] = x_scale;
    out[1] = out[2] = out[3] = out[4] = 0.0f;
    out[5] = cot;
    out[6] = out[7] = out[8] = out[9] = 0.0f;
    out[10] = depth_scale;
    out[11] = bits_to_float(kPerspectiveOneBits);
    out[12] = 0.0f;
    out[13] = 0.0f;
    out[14] = depth_offset;
    out[15] = 0.0f;
    result.ok = true;
    return result;
}

PerspectiveResult slot408_perspective_site(float* out, TanfFn tanf_hook) noexcept {
    const float fov = bits_to_float(kSlot408FovBits);
    return helper_1580a0_perspective(out, &fov, bits_to_float(kSlot408AspectBits), bits_to_float(kSlot408NearBits),
                                     bits_to_float(kSlot408FarBits), kSlot408Flag, tanf_hook);
}

}  // namespace sotc
