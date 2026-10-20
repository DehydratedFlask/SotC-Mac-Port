// Oracles: (1) double-precision geometry for clear-cut cases, (2) the specified float operation order, proven to
// matter by finding inputs where another order gives a different float, (3) NaN/equality edge semantics.
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <random>

#include "core/normal_threshold.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

static void unit(float out[3], double ax, double ay, double az) {
    double n = std::sqrt(ax * ax + ay * ay + az * az);
    out[0] = float(ax / n); out[1] = float(ay / n); out[2] = float(az / n);
}

int main() {
    const double deg = 3.14159265358979323846 / 180.0;
    const float thr = static_cast<float>(std::cos(3.0 * deg));
    float up[3]; unit(up, 0, 0, 1);

    // Clear-cut: 1 degree and 2 degrees apart are inside; 4 and 10 degrees are outside.
    for (double angle : {0.0, 1.0, 2.0}) {
        float v[3]; unit(v, std::sin(angle * deg), 0, std::cos(angle * deg));
        CHECK(sotc::dot_exceeds(up, v, thr));
    }
    for (double angle : {4.0, 10.0, 90.0, 180.0}) {
        float v[3]; unit(v, std::sin(angle * deg), 0, std::cos(angle * deg));
        CHECK(!sotc::dot_exceeds(up, v, thr));
    }

    // Edge semantics: equality is NOT exceeding; NaN is not exceeding.
    const float one[3] = {1, 0, 0};
    CHECK(!sotc::dot_exceeds(one, one, 1.0f));
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const float with_nan[3] = {nan, 0, 0};
    CHECK(!sotc::dot_exceeds(with_nan, one, -1e30f));
    CHECK(!sotc::dot_exceeds(one, one, nan));

    // Operation order matters: find inputs where ((y+x)+z) differs from (x+(y+z)) as floats, then require the
    // specified order by picking a threshold between the two results.
    std::mt19937 rng(12345);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    bool found = false;
    for (int i = 0; i < 200000 && !found; ++i) {
        float a[3] = {dist(rng), dist(rng), dist(rng)}, b[3] = {dist(rng), dist(rng), dist(rng)};
        volatile float yy = a[1] * b[1], xx = b[0] * a[0], zz = a[2] * b[2];
        const float original = (yy + xx) + zz;
        const float other = xx + (yy + zz);
        if (original == other) continue;
        found = true;
        const float lo = original < other ? original : other, hi = original < other ? other : original;
        const float mid = lo + (hi - lo) / 2;                 // strictly between, or equals lo/hi when adjacent
        if (mid != lo && mid != hi) CHECK(sotc::dot_exceeds(a, b, mid) == (original > mid));
        CHECK(sotc::dot_exceeds(a, b, std::nextafterf(original, -INFINITY)));    // original > its predecessor
        CHECK(!sotc::dot_exceeds(a, b, original));                               // original > original is false
    }
    CHECK(found);
    std::puts("normal_threshold: all checks passed");
    return 0;
}
