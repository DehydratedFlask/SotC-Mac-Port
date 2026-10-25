#include "camera_location_25a4d0.hpp"

#include <cmath>
#include <cstdio>

namespace sotc {

EulerDeg euler_deg_from_basis(const float basis[3][3]) noexcept {
    // Contiguous columns: f[0..2] forward, f[3..5] left, f[6..8] up.
    const float f0 = basis[0][0];
    const float f1 = basis[0][1];
    const float f2 = basis[0][2];
    const float f4 = basis[1][1];
    const float f5 = basis[1][2];
    const float f7 = basis[2][1];
    const float f8 = basis[2][2];

    const float sy = std::sqrt(f0 * f0 + f1 * f1);

    float e0 = 0.0f;
    float e1 = 0.0f;
    float e2 = 0.0f;
    if (sy > kCameraGimbalEpsilon) {
        e0 = std::atan2(f5, f8);
        e1 = static_cast<float>(
            std::atan2(-static_cast<double>(f2), static_cast<double>(sy)));
        e2 = std::atan2(f1, f0);
    } else {
        e0 = std::atan2(-f7, f4);
        e1 = static_cast<float>(
            std::atan2(-static_cast<double>(f2), static_cast<double>(sy)));
        e2 = 0.0f;
    }

    return {e0 * kCameraRadToDeg, e1 * kCameraRadToDeg, e2 * kCameraRadToDeg};
}

CameraPose camera_pose_of(const CameraTransform& transform) noexcept {
    CameraPose pose{};
    pose.position[0] = transform.position[0];
    pose.position[1] = transform.position[1];
    pose.position[2] = transform.position[2];
    pose.angles = euler_deg_from_basis(transform.basis);
    return pose;
}

std::string format_camera_location(const CameraPose& pose) {
    char buffer[512];
    const int written = std::snprintf(
        buffer, sizeof(buffer), "setCameraPos %.2f %.2f %.2f %.2f %.2f %.2f",
        static_cast<double>(pose.position[0]), static_cast<double>(pose.position[1]),
        static_cast<double>(pose.position[2]), static_cast<double>(pose.angles.x),
        static_cast<double>(pose.angles.y), static_cast<double>(pose.angles.z));
    if (written < 0) {
        return {};
    }
    return std::string(buffer);
}

}  // namespace sotc
