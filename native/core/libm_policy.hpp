// libm policy L1 (decided 2026-10-19, CPU-01): recovered code calls these wrappers, never <cmath> directly,
// so every transcendental call site is greppable and its tolerance is explicit.
//
//  * Result = correctly-rounded-in-practice: evaluate in double with the host libm, round ONCE to float.
//  * The original (Dinkumware-derived _FSin) is NOT bit-reproduced: reproducing it needs its algorithm, which can
//    only come from disassembling/executing a Sony module (not authorised) — so bit-equality is neither claimed
//    nor testable. Fixtures assert <= 1 ULP against an independently computed correctly-rounded oracle.
//  * cos(3 deg) at 0x33533b0 was checked (CPU-01, 2026-10-19): its only direct reader uses it as a `dot > threshold` test,
//    so a 1-ULP difference can only flip a borderline comparison; it is not hashed or stored.
//  * Values that feed hashes, saves, replays or network state must NOT rely on this wrapper; flag them
//    (grep "libm-sensitive") and revisit with a bit-exact table or a recovered algorithm.
#pragma once
#include <cmath>
#include <cstdlib>
#include <cstdio>

namespace sotc::libm {

// Original `_FSin(float x, unsigned quadrant_offset, int /*always 0 at observed sites*/)`:
// offset 0 -> sin(x), 1 -> cos(x) (inferred from 148 sin-pattern / 110 cos-pattern call sites, see
// tools/lift/fsin_callsites.py). Other offsets exist at ~125 unclassified sites and are NOT implemented:
// fail loudly rather than guess.
inline float fsin(float x, unsigned quadrant_offset) noexcept {
    switch (quadrant_offset) {
        case 0: return static_cast<float>(std::sin(static_cast<double>(x)));
        case 1: return static_cast<float>(std::cos(static_cast<double>(x)));
        default:
            std::fprintf(stderr, "sotc::libm::fsin: unrecovered quadrant offset %u\n", quadrant_offset);
            std::abort();
    }
}

}  // namespace sotc::libm
