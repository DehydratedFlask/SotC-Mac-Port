# Slot 11 structural analysis (`0x00f09160`) — same initializer pattern as slot 10

- Time/agent: 2026-10-21 00:10 CDT, orchestrator session, sole integrator. Base `1122f57`.
- Task CPU-01-CTOR11-01: STATIC_ANALYSIS_VERIFIED (structure only). Slot 11 is **not** registered
  natively and remains the explicit startup blocker.
- Gates unchanged: G0 PARTIAL; G1–G6 NOT RUN.

## What was established

Slot 11 builds a `0x1e0`-byte stack object, constructs it with `FUN_00f06410` (vptr `0x15fa770`),
calls the vtable-slot-0 destructor, publishes the vptr into global `0x3356068`, creates a 16-byte
mutex at `0x3356078` with the same recursive/priority-inherit attributes, and registers the same
`FUN_00e44a10` cleanup through `FUN_00f8f390`.

The published class is **`ClothingPhysicalMeshParameters`**: consumer `FUN_00ed5880` installs the
published vptr and its misalignment branch formats that class name at RVA `0x1322b23`.
`FUN_00f08bc0` is the get-or-create accessor (mutex `0x3356078`, flag `0x3356070`, singleton
`0x3356080`, creator `FUN_00f06cd0`), exactly mirroring slot 10's accessor shape.

The class constructor `FUN_00f06410` is 1,021 bytes and initializes many small records through
`FUN_00f06a40`; it is materially more complex than `ClothingPreviewParam`. Registering slot 11
without that contract would require an opaque placeholder, which the project forbids.

## Files

- Private packet: `local/re/evidence/cpu01-ctor11-01/` (`report.md`, `verify.py`,
  `verification.json` 48/48, `ghidra-slot11.txt`, `ghidra-slot11-class.txt`, `ghidra-refs.txt`, `ghidra-slot11-consumers.txt`, `SHA256SUMS`).
- Private script: `local/re/work/ghidra-scripts/Slot11Analysis.java`.
- Logs: `local/checkpoints/root-20261020-slot11-console.log`,
  `root-20261020-slot11class-console.log`.

## Verification

```sh
PYTHONDONTWRITEBYTECODE=1 python3 -I local/re/evidence/cpu01-ctor11-01/verify.py
# exit 0, 48/48 assertions
```

Byte anchors checked: body hashes for `0xf09160`, `0xf06410`, `0xf06340`; LEA targets
`0x3356068`/`0x3356078`/`0xe44a10`/`0x1604218`/`0x15fa770`; stores at `0xf09225`, `0xf0923e`,
`0xf06430`; call targets `0xf06410`, `0xb30a10`, `0xf90210`, `0xb30950`, `0xf8f390`; constructor
table indices 11 (`0xf09160`) and 12 (`0xf06340`).

## Observations versus inference

Observed: all bytes, hashes, targets and table identities above.

Inference: that `0x15fa770` is the vtable header word and `0x15fa780` the address point (same shape
as slot 10); that the stack object is discarded after vptr capture; that `FUN_00b30950` applies the
same recursive/PI attributes (it is the identical helper, so this is strong but still not a runtime
observation).

## Remaining risks

- `FUN_00f06410` record layout, field meanings and virtual surface are unrecovered (the class name is known).
- The static consumer `FUN_00ed5880` exists, but runtime reachability and ordering are unknown.
- Existing `0xd71f60` NEEDS_PRINCIPAL remains untouched.

## Next exact action

Recover `FUN_00f06410`'s class contract before registering slot 11, or continue AUD-02/SAV-01
static tracks. Do not register slot 11 as a mutex-only no-op.

## Attempt counts

0 failed semantic hypotheses; 0 failed analysis queries. One verifier opcode-table omission
(`48 8d 3d`) was corrected before the passing run.
