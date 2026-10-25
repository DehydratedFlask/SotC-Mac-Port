# Checkpoint — cross-process save/load persistence

- Local: 2026-10-23 (America/Chicago)
- Agent: orchestrator
- Task: `SAV-02-RELAUNCH-01`
- Gate: **FIXTURE_VERIFIED**; G2/G5 remain **NOT RUN**

## What it proves

`native/tests/test_save_relaunch.cpp` writes a progress slot with the native encoder in one process
(`--write <root>`), exits, and loads + enumerates it in two **fresh** processes (`--read <root>`)
that share only the directory on disk. The parent checks the data/meta files exist, both children
exit 0, and a second reader still succeeds (loading does not consume or mutate the slot).

The object round-trips every `ProgressV1` field (game data, 257 indexed values, 48 flags, 5 words,
3 booleans, 2 floats, ids) plus the `USR-META` title/subtitle/detail/`userParam`. The blob flag bits
are made consistent with the flag array before encoding, because `encode_progress_body` derives the
blob bits from `flags`.

## Why it matters

The previous transaction tests round-trip through `save_io_posix()` inside one process. This one
adds the host persistence claim across a process boundary: a save survives process exit and is
readable by a killable, fresh core process. It is still synthetic data produced by the native
encoder — not a platform-written save and not a retail-format import.

## Evidence

`test_save_relaunch`: 7 parent checks, child-write 3 checks, child-read 10 checks (twice). At the
round tip the full suite is 381/381 with ASan+UBSan.

## Limits

No real PS4 `USR-DATA`/metadata pair exists in the workspace, so SAV-04 acceptance remains
**NOT RUN** (`docs/SAVE_ACCEPTANCE.md`, `tools/dev/save_inspect.py`). No launcher save UI, no
platform mount, no dialog milestones.
