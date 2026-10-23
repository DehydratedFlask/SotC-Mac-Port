# Checkpoint — screenshot / GPU-readback contract

- Local: 2026-10-23 (America/Chicago)
- Agent: static-analysis worker
- Task: `GPU-01-READBACK-CONTRACT-01`
- Gate: **STATIC_ANALYSIS_VERIFIED**; implementation not started

## What `0x961a0` is

It is the screenshot-to-PNG encoder (`0x961a0..0x965a3`, 1027 B, sha256 `5889c316…d0d01b`), not a
generic GPU readback. Its four imports resolve through the GOT to
`scePngEncQueryMemorySize`/`Create`/`Encode`/`Delete`. It captures a surface through `0xa2aa0`
(1920x1080, or 3840x2160 when `renderer+0x3c == 2`), waits on `0x91090`, takes the CPU image at
`surface+0xc4`, then either `memcpy`s or repacks 32-bit 2/10/10/10 to 4x8-bit. It returns the
encoded PNG `{data, ctrl}` plus an `int64` size. `0` ok / `1` fail; the `eax == 2` branch is dead.

## Sink

The bug-report path has no directory or filename. `0x1bdf30` builds a `"SendFile"` blob named
`bug_screenshot.png` and pushes it into `*(0x2ea8978)` -> `0x4deb0` -> `0x4b1d0`
(`magic|version|kind|size|body`, magic "BPEE"/"EEPB", version 6). `0x4deb0` calls
`sceNetGetSockInfo`: it is a socket-backed remote-command channel, not a file. Only the console
`screenshot` command writes a real file, normalized to `/app0/...` by `0x30570`.

## frameNum

`*(int32_t*)(engine + 0x70)`, read in `0xa70c30`; `engine = *(void**)(ctx+0x8)`; the holder owner
comes from `[*(engine+0x2668)+0x08]`; `engine+0x2668` is a per-tick stack object installed by
`0x25c080` at `0x25c298`. Two other readers corroborate an ordinal. **The writer of `engine+0x70`
was not found** in three store-pattern scans: an explicit blind spot.

## Proposed contract

One `capture_frame(FrameImage{rgba8,w,h,stride,frame_num}) -> {Status, png}` hook, plus
`send_file(name,payload)` and `write_app0_file(path,payload)`; `attach_screenshot()` returns
`{attached, name="bug_screenshot.png", synthetic_command, sent_to_sink}` with the six proven rules
(null renderer, capture failure -> synthetic command, success -> blob+sink then list-B entry, sink
result discarded).

## Limits

Pixels, encoder bytes, socket transport and the `engine+0x70` writer remain contracts. No boot or
G1 claim.
