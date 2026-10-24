// Fixture for the pure camera math: 0x50ba0 Euler conversion and the six-double formatter.
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "core/camera_location_25a4d0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
bool near(float a, float b, float eps = 1e-4f) { return std::fabs(a - b) <= eps; }

CameraTransform identity() {
    CameraTransform t{};
    t.basis[0][0] = 1.0f;
    t.basis[1][1] = 1.0f;
    t.basis[2][2] = 1.0f;
    return t;
}

// Column-major R = Rz(z) * Ry(y) * Rx(x), the convention the original formula inverts.
CameraTransform from_euler(float x_deg, float y_deg, float z_deg) {
    const double d2r = 3.14159265358979323846 / 180.0;
    const double cx = std::cos(x_deg * d2r), sx = std::sin(x_deg * d2r);
    const double cy = std::cos(y_deg * d2r), sy = std::sin(y_deg * d2r);
    const double cz = std::cos(z_deg * d2r), sz = std::sin(z_deg * d2r);
    // R = Rz * Ry * Rx; columns are the transformed basis vectors.
    const double r[3][3] = {
        {cz * cy, cz * sy * sx - sz * cx, cz * sy * cx + sz * sx},
        {sz * cy, sz * sy * sx + cz * cx, sz * sy * cx - cz * sx},
        {-sy, cy * sx, cy * cx},
    };
    CameraTransform t{};
    for (int col = 0; col < 3; ++col) {
        for (int row = 0; row < 3; ++row) {
            t.basis[col][row] = static_cast<float>(r[row][col]);
        }
    }
    return t;
}
}  // namespace

int main() {
    CHECK(kCameraEulerRva == 0x50ba0);
    CHECK(kCameraEulerSize == 261);
    CHECK(sizeof(CameraTransform) == 0x40);

    // Identity: all angles zero.
    {
        const CameraTransform t = identity();
        const EulerDeg e = euler_deg_from_basis(t.basis);
        CHECK(near(e.x, 0.0f) && near(e.y, 0.0f) && near(e.z, 0.0f));
    }

    // Pure yaw 90 degrees.
    {
        const CameraTransform t = from_euler(0.0f, 0.0f, 90.0f);
        const EulerDeg e = euler_deg_from_basis(t.basis);
        CHECK(near(e.x, 0.0f) && near(e.y, 0.0f) && near(e.z, 90.0f));
    }

    // General rotation round-trips within 1e-3 degrees.
    {
        const CameraTransform t = from_euler(17.189f, -40.107f, 63.025f);
        const EulerDeg e = euler_deg_from_basis(t.basis);
        CHECK(near(e.x, 17.189f, 1e-3f));
        CHECK(near(e.y, -40.107f, 1e-3f));
        CHECK(near(e.z, 63.025f, 1e-3f));
    }

    // Gimbal lock: pitch +90 -> y=+90, z=0.
    {
        const CameraTransform t = from_euler(22.918f, 90.0f, 63.025f);
        const EulerDeg e = euler_deg_from_basis(t.basis);
        CHECK(near(e.y, 90.0f, 1e-3f));
        CHECK(near(e.z, 0.0f, 1e-4f));
    }

    // Six-double formatting order and %.2f text.
    {
        CameraTransform t = identity();
        t.position[0] = 1.0f;
        t.position[1] = 2.0f;
        t.position[2] = 3.0f;
        const CameraPose pose = camera_pose_of(t);
        CHECK(pose.position[0] == 1.0f && pose.position[1] == 2.0f && pose.position[2] == 3.0f);
        CHECK(format_camera_location(pose) ==
              "setCameraPos 1.00 2.00 3.00 0.00 -0.00 0.00");
    }

    return 0;
}
