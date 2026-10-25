# Checkpoint — camera-location chain characterized; pure Euler/format slice lifted

- Local: 2026-10-23 09:3x (America/Chicago)
- Agents: static-RE worker; integrator
- Task: `CPU-01-CAMERA-CHAIN-25A4D0-01`
- Gate: **G1 PARTIAL — pure math native; live pose chain blocked**
- Evidence: E573

## Chain dossiers

| function | extent | role |
|---|---|---|
| `0x1c70b0` | 8 B, sha `2064e587…` | pure getter `return *(void**)(this+0x2053e8)`; 37 callers |
| `0x1c0f40` | 166 B, sha `2b1aabdc…` | 8-slot registry query with lazy rebuild `0x1c0b40`; returns newest slot element whose owner flags pass `(flags & 0x240) != 0`, or the forced object |
| `0x216480` | 45 B, sha `ed5459d7…` | generic bound-member-function invoker; reads the 24-byte delegate at `[D+0x2e8]`; 170 callers |
| `0x50ba0` | 261 B, sha `28f72958…` | pure 3×3 basis → Euler degrees |

`0x1c70b0` is a generic member accessor, `0x1c0f40` is a slot query, and `0x216480` is an
engine-wide delegate invoker; none is camera-specific. The pose comes from a runtime delegate.

## Transform and formula

`CameraTransform` is 0x40 B: nine floats at `+0x00..0x20` (columns forward/left/up), position at
`+0x24/+0x28/+0x2c`, 0x10 unread bytes. `0x50ba0`:

```
sy = sqrt(f0*f0 + f1*f1)
if sy > 16*FLT_EPSILON:
  e0 = atan2f(f5, f8); e1 = atan2(-f2, sy); e2 = atan2f(f1, f0)
else:
  e0 = atan2f(-f7, f4); e1 = atan2(-f2, sy); e2 = 0
return { e0*57.2957764f, e1*57.2957764f, e2*57.2957764f }   // degrees
```

The six format arguments are `posX, posY, posZ, e0, e1, e2` (position first), formatted with
`"setCameraPos %.2f %.2f %.2f %.2f %.2f %.2f"` by `0x10a90` at `0x25a875`. A second consumer,
`GameCoreLoop::PrintCameraPos_command` (`0x2549e0`), confirms the order and semantics with
`"Camera Position: SetCameraPos …"`.

## Native work

- `native/core/camera_location_25a4d0.{hpp,cpp}`: `CameraTransform` (0x40 B), `EulerDeg`,
  `CameraPose`, `euler_deg_from_basis` (exact branch/constants/degrees), `camera_pose_of`,
  `format_camera_location` (C-locale `%.2f`, same order).
- Fixture `camera_location_25a4d0`: identity, yaw 90°, a general `Rz·Ry·Rx` round-trip within
  1e-3°, gimbal pitch +90°, and the exact formatted string (including the original `-0.00` pitch
  sign for the identity case).
- Checker `check_fcamera_location.py`: pins the 261 B body, `atan2f`/`atan2` call sites, RAD2DEG and
  `16*FLT_EPSILON` constants, the format literal and the `0x10a90` call.

## Limits

The live pose chain is **not** liftable without engine state: a `GameCoreLoop` with `[+0x2668]`, the
`shared_ptr<CameraObject>`, the 0x270 B slot registry, a populated slot 0, and the runtime delegate
at `[D+0x2e8]`. `0x1c0f40`, `0x216480`, the registry rebuild and the pose numbers remain contracts.
The native `format_camera_location` returns `std::string`, not the original refcounted String.
**G1 PARTIAL.**
