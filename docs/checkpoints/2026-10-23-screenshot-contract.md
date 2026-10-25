# Checkpoint — native screenshot seam (`attach_screenshot`)

- Local: 2026-10-23 (America/Chicago)
- Agent: integrator
- Task: `GPU-01-SCREENSHOT-CONTRACT-NATIVE-01`
- Gate: **FIXTURE_VERIFIED**; the actual GPU capture/encode remains a hook

## What is native

`native/core/screenshot_contract.*` implements the E597 decision table:

1. `renderer_present == false` -> no capture, synthetic `Command` entry;
2. capture `NoSurface`/`EncodeFailed` (or an empty PNG) -> synthetic entry, sink untouched;
3. capture `ok` -> `"SendFile"` best-effort push, list-B `("Screenshot","bug_screenshot.png")`,
   synthetic entry suppressed;
4. the sink's `false` result is recorded but never changes the attachment (the original discards it);
5. the attachment name is the literal `bug_screenshot.png`, not `"screenshot"` or `"bug_screenshot"`;
6. `frame_num` is passed through from the renderer's counter (the original's writer was not found).

`snapshot_bug_data` now accepts the seam: `ScreenshotHooks*` + `FrameImage` take precedence over the
legacy raw `query_screenshot`, and the result reports the sink outcome and encoder status. The
console `/app0/` path is a separate `console_screenshot_write_app0`, never used by the report block.

## Fixture

`test_screenshot_contract` 53 checks: no renderer, missing capture, no-surface, encode-failed,
ok+sink accepted, sink rejected, ok+empty payload, console write and its four failure surfaces,
frame_num pass-through, and "report path never calls the console writer". `test_snapshot_bug_data`
adds the two seam cases (ok -> list-B/no synthetic; no renderer -> synthetic).

## Checker

`check_fscreenshot_contract.py` pins the 1027 B encoder (`5889c316…`), the name literal, and the two
call sites (`0x1bdfc5` report, `0x169dd7` console).

## Limits

Pixels, the `scePngEnc*` calls, the socket transport, the surface creation/wait and `engine+0x70`'s
writer remain hooks. **No boot/G1 claim; the Metal renderer does not exist yet.**
