# E1052 — slot-309 save-dispatch jump-table investigation

**Date:** 2026-10-31 02:28 CDT. **Agent:** `/root`. **Task:**
`CPU-01-SLOT309-SAVE-DISPATCH-JUMPTABLES-01`. **Status:** PARTIAL; stopped at the E911 boundary.
**Failed attempts:** 1 (the Ghidra MCP overview timeout); no failed semantic hypothesis.

## Scope and artifact

This reassessment resumed the E911-stopped save-dispatch route from E1020. The only target was the
owner's pinned, local ELF `local/re/eboot.elf`, SHA-256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, 65,214,308 bytes,
ELF64 x86-64, 11 program headers. No executable code ran. No game input, product source, or test
file changed.

The two raw table verifications were run by:

```sh
python3 -I local/validation/slot309-save-dispatch-e1052/verify_tables.py > local/validation/slot309-save-dispatch-e1052/table-verification.json
```

Exit 0. Verifier SHA-256 is `d98f074ffb0c15d7dd0038331c938261b2f16f07e988b171a752d3d8d8f16345`; its JSON result SHA-256 is
`0e252c02bb6cd9a4d7c291354dfd3760beb3e937a081c9a4c3c2db9f89563ace`. It verifies the source
ELF digest, unique file-backed PT_LOAD mapping, complete raw spans, signed 32-bit displacements,
calculated target RVAs, and both rel32 call bytes. PT_LOAD phdr 0 maps `file_offset = VMA +
0x4000`: table A VA `0x12c3280` maps to file offset `0x12c7280`, and table B VA `0x12c3324`
maps to `0x12c7324`. A separate Ghidra image-base overview was not obtained; the MCP overview
timed out, so no Ghidra image base is claimed.

Rizin 0.9.1 (`darwin-arm-64`, Homebrew `0.9.1_2`) corroborated the dispatch instructions and
selected code. The saved outputs are `dispatcher-a.rizin.txt`, `dispatcher-b.rizin.txt`,
`case-a.rizin.txt`, and `case-b.rizin.txt` in the private E1052 directory. Read-only commands:

```sh
rizin -q -c 's 0xa48670; pd 20' local/re/eboot.elf
rizin -q -c 's 0xa4af90; pd 20' local/re/eboot.elf
rizin -q -c 's 0xa488ea; pd 12' local/re/eboot.elf
rizin -q -c 's 0xa4bd94; pd 20' local/re/eboot.elf
```

## Table A — dispatch at `0xa4869b`

Raw table VA `0x12c3280`, file offset `0x12c7280`, 17 signed 32-bit entries / 68 bytes;
span SHA-256 `a3600a89082581918660653c7553ac07fad2084a9fea5d6f8c457ad68672d29f`.
The base-relative targets in raw entry order are:

| Entry / Ghidra case | Target | Entry / Ghidra case | Target |
|---:|---:|---:|---:|
| 0 / 1 | `0xa4869d` | 9 / 10 | `0xa48d5b` |
| 1 / 2 | `0xa488ea` | 10 / 11 | `0xa4904a` |
| 2 / 3 | `0xa48ad1` | 11 / 12 | `0xa48db1` |
| 3 / 4 | `0xa48b81` | 12 / 13 | `0xa48de3` |
| 4 / 5 | `0xa48cd3` | 13 / 14 | `0xa49148` |
| 5 / 6 | `0xa48eb9` | 14 / 15 | `0xa4892b` |
| 6 / 7 | `0xa48f96` | 15 / 16 | `0xa48e4c` |
| 7 / 8 | `0xa48cfb` | 16 / 17 | `0xa489dc` |
| 8 / 9 | `0xa49011` |  |  |

Rizin shows the signed-index dispatch is bounded to 17 entries. Ghidra 12.1.4's typed switch
evidence agrees: dispatch `0xa4869b`, data source `0x12c3280`, 17 four-byte entries, cases 1–17,
default target `0xa48ea0`.

## Table B — dispatch at `0xa4afba`

Raw table VA `0x12c3324`, file offset `0x12c7324`, 19 signed 32-bit entries / 76 bytes;
span SHA-256 `ce23f910e30a26fa0d64c8e8a6a5fb270f5171c5a9525c89597be99b8d4764f5`.
Rizin shows a signed-index dispatch bounded to 0–18. The complete raw target order is:

| Entry | Target | Entry | Target | Entry | Target |
|---:|---:|---:|---:|---:|---:|
| 0 | `0xa4afbc` | 7 | `0xa4bc66` | 14 | `0xa4b864` |
| 1 | `0xa4b20a` | 8 | `0xa4b56d` | 15 | `0xa4b904` |
| 2 | `0xa4b314` | 9 | `0xa4b689` | 16 | `0xa4b257` |
| 3 | `0xa4b94c` | 10 | `0xa4bc87` | 17 | `0xa4bd94` |
| 4 | `0xa4b4e1` | 11 | `0xa4b701` | 18 | `0xa4be31` |
| 5 | `0xa4ba94` | 12 | `0xa4b800` |  |  |
| 6 | `0xa4bb61` | 13 | `0xa4bcc0` |  |  |

Ghidra identifies the same dispatch instruction and data source, but its typed table contains only
15 entries, cases 2–16, with default `0xa4be82`. Those 15 targets match the first 15 raw entries;
it does not map the final four raw entries (`0xa4b904`, `0xa4b257`, `0xa4bd94`, `0xa4be31`). Treat
the raw parser and Rizin as the evidence for the complete 19-entry table. The Ghidra table model
is partial here. The reason for the narrower typed mapping is unknown.

## Selected consumer paths

REA 6.7.0 CLI with Ghidra 12.1.4, `GHIDRA_HEADLESS_MAXMEM=8G`,
`REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000`, and the same profile digest
`769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa` completed both function
queries. Exact commands and retained evidence:

```sh
env GHIDRA_INSTALL_DIR=/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000 /Users/dehydratedflask/.npm/_npx/a1722018f28f9df2/node_modules/.bin/rea function local/re/eboot.elf 0xa48280 --provider ghidra --snapshot local/validation/slot309-save-dispatch-e1052/rea-snapshot.json --format json > local/validation/slot309-save-dispatch-e1052/function-a48280.json 2> local/validation/slot309-save-dispatch-e1052/function-a48280.stderr.log
env GHIDRA_INSTALL_DIR=/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000 /Users/dehydratedflask/.npm/_npx/a1722018f28f9df2/node_modules/.bin/rea function local/re/eboot.elf 0xa4ab90 --provider ghidra --snapshot local/validation/slot309-save-dispatch-e1052/rea-snapshot.json --format json > local/validation/slot309-save-dispatch-e1052/function-a4ab90.json 2> local/validation/slot309-save-dispatch-e1052/function-a4ab90.stderr.log
```

Both exited 0; stderr files were empty. Evidence IDs are
`ev_b4dad997b5301ab161d20faebc71711b44872f71e616c522e8bcc5ba4bfef190` (`0xa48280`) and
`ev_cb71a5e09fd4ffa2fdb88aefa1badbf1d873898cd291c878238376b8f2d73ca4` (`0xa4ab90`). The first
JSON SHA-256 is `93fa721ffdf9efd2ab19c25db45cfae303b5c5cdda70c04bdaf47559b54f35d9`; the second is
`79d23921ff8b7fe8fcc7344a51704e743fccbb73f36db77f84b0207ae65368d7`. Final private snapshot
SHA-256 is `03333707710267fba786146962a0f1034aee7167babd6972d680dc525a5ce887`.

- Ghidra dossier `FUN_00a48280` covers 8,276 bytes in 15 non-contiguous ranges and 279 CFG
  blocks. Its case 2 block at `0xa488ea` has a conditional retry loop followed by direct call
  `0xa4890c -> 0xa4a600`. The same function has direct call `0xa4947b -> 0xa4a940`; no unique
  table-case predicate for this common path was isolated.
- Ghidra dossier `FUN_00a4ab90` covers 6,125 bytes in 20 non-contiguous ranges and 203 CFG
  blocks. Case 3 (`0xa4b20a`) reaches direct call `0xa4b23a -> 0xa4a600`. Direct call
  `0xa4bf6d -> 0xa4a940` is also in this function, but neither Ghidra's typed table nor the saved
  Rizin case excerpt associates that call with a raw table entry. The Rizin excerpt from
  `0xa4bd94` ends at `0xa4be03`; it does not reach the call. An exploratory walk of Ghidra's
  BasicBlockModel edges passed through a switch again and was not accepted as a case-to-call proof:
  the raw target lies at the reported block endpoint, just outside the Ghidra function-body end,
  and Ghidra's typed table omits that entry. Do not claim a table-selected path to `0xa4a940`.
- Both Ghidra dossiers report partial callee coverage because of one unresolved targetless call:
  `0xa4879b` in `FUN_00a48280`, and `0xa4b07c` in `FUN_00a4ab90`. The source of either call
  remains unknown.

## Stop decision and status

These observations partially characterize the bounded table/case route. They establish both raw
tables, table A's case-2 call to `0xa4a600`, table B's case-3 call to `0xa4a600`, and direct
`0xa4a940` calls in the two functions. They do not identify a table case for table B's
`0xa4a940` call, nor establish a producer, ownership, caller, or ordering edge tying either
consumer to slot-309 initializer `0xa505e0`. E1020 and E1030 leave the dispatcher owner/startup
order unresolved. The E911 stop condition is met: stop this route, keep slot 309 unregistered, and
resume only after a new concrete edge or principal reassessment. Parent
`CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01` remains active/partial; G1 remains PARTIAL;
G2/G5/G6 remain NOT RUN. No game behavior is claimed.

The initial Ghidra MCP overview attempt failed after 330,000 ms with configured 2G heap and
produced no E1052 Evidence ID; details are in [attempt 1](2026-10-31-slot309-save-dispatch-jumptables-e1052-attempt1.md).

## Repository verification before publication

The required root commands completed with these results; logs are private under
`local/validation/slot309-save-dispatch-e1052/`:

| Command | Result |
|---|---|
| `cmake -S native -B local/build/orch-r22` | Exit 0 |
| `cmake --build local/build/orch-r22 -j 12` | Exit 0; Ninja reported no work |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | Exit 0; 495/495 passed |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | Exit 0; 578 passed, 0 skipped, 0 failed |
| `trail build --check` | Exit 0; wiring graph OK at 10,951 nodes; deep/meaning layer not built |
| `python3 -I scripts/audit_publish_paths.py` | Exit 0; staged audit accepted 9 paths |
| `python3 -I scripts/audit_publish_paths.py --all` | Exit 0; all-path audit accepted 3,463 paths |
| `python3 -I scripts/check_commit_attribution.py` | Exit 0; attribution audit OK |
| `python3 scripts/project_index.py` | Exit 0; 44,296 folders indexed |

`git diff --cached --check` also exited 0. These are repository/fixture/static checks only; they
do not change the E911 stop decision or advance a game gate.
