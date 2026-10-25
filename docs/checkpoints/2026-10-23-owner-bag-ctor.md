# Checkpoint — owner bag derived ctor

- Local: 2026-10-23 (America/Chicago)
- Agent: static-RE worker; integrator
- Task: `CPU-01-OWNER-BAG-CTOR-01`
- Gate: **STATIC_ANALYSIS_VERIFIED**

## `0xa2afb0` (441 B, sha `252b37a3…`)

1. Base ctor `0xac4de0` (380 B): vptr `0x15d4fd8`, mutex `+0x08`, `+0x10 = 0`, four tree sentinels
   at `+0x20/+0x38/+0x50/+0x68` with counts at `+0x28/+0x40/+0x58/+0x70`, zeroes `+0x80..0x90`.
2. `scePadInit()`.
3. `sceUserServiceGetInitialUser(&initial)`.
4. `sceSystemGestureInitializePrimitiveTouchRecognizer(0)`.
5. One unconditional `0xc88` per-user input device via `0xa2ba50(obj, initial)`.
6. `sceUserServiceGetLoginUserIdList(&ids[4])`; on failure, skip to `+0x10 = 1`.
7. Up to three more `0xc88` devices from the login ids, with negative-id skips, a dedupe against the
   initial user only, and an `r13 <= 2` cap. With four distinct login ids the **last is dropped**.
8. `+0x10 = 1` last, on every path.

## Correction to E576

The four `0xc88` objects are **per-user input devices** (`scePadOpen` + gesture + touch recognizer),
not language tables. They have **zero effect** on `insert_a`/`insert_b`. The native `OwnerPropBag`
multimap model is therefore complete for the property-map path; the input-device lifetime, mutex,
IME block `+0x130..+0x178`, vectors and `+0x98..+0xa0` remain contracts.

## Why no code change

The property path does not read any of the opaque fields. A native `owner_bag_construct` would only
add a user-selection/input-device model, which is outside the report/startup slice. The correction is
recorded so future agents do not lift four nonexistent "language tables".

## Limits

The input-device object `0xa2ba50` (291 B) and its dtor `0xa2bb80` (47 B) are not lifted. The
`+0x10` mode and the `*(0x3305568)` republish remain caller-supplied in the native bag.
