# Checkpoint — SaveData transaction (`SAV-02-TRANSACTION-01`)

- Local: 2026-10-23 (America/Chicago)
- Agent: code worker; integrator verification
- Task: `SAV-02-TRANSACTION-01`
- Gate: **FIXTURE_VERIFIED**; no real save read/written, no boot, G2/G5 NOT RUN

## What is native

- `ProgressV1`: opaque 15,576-byte game-data blob + 257 unlock entries + 48 flags + 5 words +
  3 bools + 2 floats + the id vector, with `progress_check` returning the game's 1-based ordinals.
- Body codec: 48 flags, 13 named rows, 257 indexed rows; `20005 + 4N` bytes; strict tiling and the
  original's lenient first-match walk (neither reproduces the original's NULL/OOB hazards).
- Outer container `{1, FNV-1a-32(body), size, 4}` over `SAVEDATA0<suffix>/USR-DATA`.
- `save_load`, `save_store`, `save_check_backup`, `save_restore_backup`, `save_enumerate`.
- `SaveIoOps` seam with POSIX and deterministic in-memory backends (partial write, write fail,
  rename fail, BUSY, ENOSPC).
- Atomic temp+fsync+rename; the previous generation is copied to `USR-DATA.bak` **before** commit;
  BUSY retries are bounded (the original's unbounded loop and delete-before-write are not copied).

## Golden correction

The contract's `19748 + 4N` omitted the 257 one-byte varint string lengths. The actual body is
`20005 + 4N`; the N=2 golden is **20,013 B**, FNV **`0x5ce4db99`**, sha256 `58a62b7c…`. An
independent Python re-derivation (`local/re/evidence/savedata-transaction/verify_golden.py`) is
byte-identical to the C++ encoder. The flag/blob contradiction is corrected (flags derived from the
blob).

## Fixtures

20 test functions, 206 `CHECK` sites, 28 of the 29 planned cases; `USR-META` is N/A because no
metadata surface exists. Extra: empty root, bad suffix, missing ops/mount, corrupt backup,
`keep_backup=false`, enumerate filtering, out-unchanged-on-failure, padded payload, unlock 3,
retag, restore write failure.

## Verification

- Full `ctest`: 366/366 passed.
- Sweep: 463/0/0, including `check_fsave_transaction.py` (pins the `0x7e5bb0` producer, the
  `0x3260bb8`/`0x3cd8` BSS blob, the mount2 body and the 1-7 ordinals).
- Mutation M1 (hash compare short-circuited) fails F03; M2 (write destination before rename) fails
  the atomicity check. Both restored; green again.
- Publish audit exit 0 (2,598 paths); attribution exit 0.

## Limits

Dialog progress milestones and corrupt-offer-restore, `USR-META`/`icon0.png`/`0x530`/SetParam/
SaveIcon/Delete, the original-save import flag, power-loss and cross-process locking proofs, and
the 15,576-byte blob semantics remain contracts. **No save/load works claim.**
