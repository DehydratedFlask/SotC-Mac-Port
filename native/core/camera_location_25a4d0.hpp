// Native model of the pure part of the `"Camera Location"` value chain (E573).
//
// Recovered:
//   - `0x50ba0..0x50ca4` (261 B, sha256 `28f72958…`) converts a 0x40 B transform's basis to three
//     Euler angles in DEGREES. It reads f[0],f[1],f[2],f[4],f[5],f[7],f[8] (column-major basis:
//     column 0 = forward, column 1 = left, column 2 = up) and uses `atan2f` twice and double
//     `atan2` once, then multiplies by 57.2957764f.
//   - `0x25a810..0x25a875` converts position `+0x24/+0x28/+0x2c` to double, calls `0x50ba0`, and
//     formats `"setCameraPos %.2f %.2f %.2f %.2f %.2f %.2f"` with
//     `(posX,posY,posZ, e0,e1,e2)`.
//   - The transform is `{float basis[3][3]; float position[3]; uint8_t tail[0x10];}` = 0x40 B.
//
// NOT modelled: the live objects that produce the transform (`0x1c70b0`, `0x1c0f40`, `0x216480`,
// the camera registry, the bound delegate) and the original refcounted String allocator. The
// formatter here returns a `std::string` with the same `%.2f` text; it assumes the C locale like
// the original `vsnprintf` calls. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace sotc {

inline constexpr std::uint64_t kCameraEulerRva = 0x50BA0;
inline constexpr std::size_t kCameraEulerSize = 261;
inline constexpr std::uint64_t kCameraEulerEndRva = 0x50CA4;
inline constexpr std::uint64_t kCameraFormatCallsiteRva = 0x25A875;
inline constexpr float kCameraRadToDeg = 57.2957764f;
inline constexpr float kCameraGimbalEpsilon = 1.90734863e-06f;  // 16 * FLT_EPSILON

struct CameraTransform {
    float basis[3][3];        // +0x00  contiguous columns: forward, left, up
    float position[3];        // +0x24  world position x,y,z
    std::uint8_t tail[0x10];  // +0x30  never read by this chain
};
static_assert(sizeof(CameraTransform) == 0x40);
static_assert(offsetof(CameraTransform, position) == 0x24);

struct EulerDeg {
    float x = 0.0f;  // rotation about X (roll)
    float y = 0.0f;  // rotation about Y (pitch)
    float z = 0.0f;  // rotation about Z (yaw)
};

struct CameraPose {
    float position[3]{};
    EulerDeg angles{};
};

// Exact lift of 0x50ba0. `basis` points at nine contiguous floats (the three columns above).
[[nodiscard]] EulerDeg euler_deg_from_basis(const float basis[3][3]) noexcept;

// Reads position + basis and returns the six values in format order.
[[nodiscard]] CameraPose camera_pose_of(const CameraTransform& transform) noexcept;

// `"setCameraPos %.2f %.2f %.2f %.2f %.2f %.2f"` with the six doubles in the recovered order.
[[nodiscard]] std::string format_camera_location(const CameraPose& pose);

}  // namespace sotc
