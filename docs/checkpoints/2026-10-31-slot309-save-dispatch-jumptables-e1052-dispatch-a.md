# E1052 — Ghidra dossier for slot-309 save dispatcher A

**Date:** 2026-10-31 02:19 CDT. **Agent:** `/root`. **Task:**
`CPU-01-SLOT309-SAVE-DISPATCH-JUMPTABLES-01`. **Status:** ACTIVE. **Failed attempts:** 1.

## Exact command and artifact identity

The REA CLI query exited 0 after approximately 5 minutes 54 seconds:

```sh
env GHIDRA_INSTALL_DIR=/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000 /Users/dehydratedflask/.npm/_npx/a1722018f28f9df2/node_modules/.bin/rea function local/re/eboot.elf 0xa48280 --provider ghidra --snapshot local/validation/slot309-save-dispatch-e1052/rea-snapshot.json --format json > local/validation/slot309-save-dispatch-e1052/function-a48280.json 2> local/validation/slot309-save-dispatch-e1052/function-a48280.stderr.log
```

Pinned target SHA-256 is `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`
(ELF64 x86-64). Provider is Ghidra 12.1.4 through REA 6.7.0; analysis-profile digest is
`769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa`, with loader selected
automatically from the ELF header. Ghidra Evidence ID:
`ev_b4dad997b5301ab161d20faebc71711b44872f71e616c522e8bcc5ba4bfef190`. Query JSON SHA-256:
`93fa721ffdf9efd2ab19c25db45cfae303b5c5cdda70c04bdaf47559b54f35d9`; stderr was empty. The
private snapshot SHA-256 is
`6e2f257bc1c33eddea9ba8d5beea6d22d507e145a2795991d8212b532fe56287`.

REA's whole-binary overview had timed out on the prior attempt, so a separate Ghidra `image_base`
and source-mapping inventory was not returned. The independent ELF-header parser verifies
PT_LOAD phdr 0 maps file offsets to virtual addresses with `file_offset = VMA + 0x4000`; table
VA `0x12c3280` is file offset `0x12c7280`. This raw mapping is not attributed to Ghidra.

## Observations

- Ghidra identifies exact entry `0xa48280`, function `FUN_00a48280`, with 8,276 body bytes in 15
  non-contiguous ranges spanning `0xa48280..0xa4a36a`; its CFG has 279 blocks.
- Its typed jump-table evidence at dispatch `0xa4869b` identifies data source `0x12c3280`,
  17 entries of 4 bytes, and the following mapping. The raw parser's corresponding 17 signed
  relative entries and span SHA-256 are independently verified (`a3600a89082581918660653c7553ac07fad2084a9fea5d6f8c457ad68672d29f`).

  | Case | Target | Case | Target | Case | Target |
  |---:|---:|---:|---:|---:|---:|
  | 1 | `0xa4869d` | 7 | `0xa48f96` | 13 | `0xa48de3` |
  | 2 | `0xa488ea` | 8 | `0xa48cfb` | 14 | `0xa49148` |
  | 3 | `0xa48ad1` | 9 | `0xa49011` | 15 | `0xa4892b` |
  | 4 | `0xa48b81` | 10 | `0xa48d5b` | 16 | `0xa48e4c` |
  | 5 | `0xa48cd3` | 11 | `0xa4904a` | 17 | `0xa489dc` |
  | 6 | `0xa48eb9` | 12 | `0xa48db1` |  |  |

- Case 2's Ghidra pseudocode includes a conditional retry loop, followed by direct call
  `0xa4890c -> 0xa4a600`; Ghidra's ReferenceManager classifies it as an unconditional direct
  call. This establishes static case/call connectivity, not runtime case selection or invocation.
- This same function has another direct call `0xa4947b -> 0xa4a940`. The current dossier does not
  identify a unique switch-case predicate for that common state-machine path. Do not assign it to
  a specific table case.
- Ghidra reports one decoded call at `0xa4879b` without a ReferenceManager destination, and the
  function dossier marks callee relationship coverage partial for that unresolved call. The
  function body is non-contiguous; Ghidra pseudocode is decompiler output, not original source.

## Boundary and next action

The dossier does not show a call, ownership edge, or ordering relation to slot-309 initializer
`0xa505e0`. Existing E1020 evidence also leaves that relation open. Keep slot 309 unregistered and
G1 PARTIAL. The next bounded action is the same REA CLI query for dispatcher `0xa4ab90`, using the
private snapshot; stop after its table and selected direct calls are reconciled if no concrete
edge/order to `0xa505e0` is found.
