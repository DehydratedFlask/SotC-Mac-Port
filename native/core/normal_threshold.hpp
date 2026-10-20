// Native recovery of an observed fragment of the function at original RVA 0xe0e140 (compare block 0xe0e4c0..0xe0e4e2).
// Only this predicate is lifted — NOT the whole ~1.8 KB AVX function (see HANDOFF checkpoint 7). Names are ours.
#pragma once

namespace sotc {

// dot = ((a.y*b.y) + (a.x*b.x)) + (a.z*b.z), each operation rounded to float and NOT fused (the original uses
// separate vmulss/vaddss; native/CMakeLists.txt builds with -ffp-contract=off so this stays true).
// Result: dot > threshold. A NaN dot yields false (ucomiss unordered sets CF, so the original's `jbe` skips).
// With threshold = cos(3 degrees) (global at 0x33533b0 in the original) this is "the two unit vectors are < 3 degrees apart".
inline bool dot_exceeds(const float a[3], const float b[3], float threshold) noexcept {
    const float yy = a[1] * b[1];
    const float xx = b[0] * a[0];
    const float zz = a[2] * b[2];
    const float dot = (yy + xx) + zz;
    return dot > threshold;
}

}  // namespace sotc
