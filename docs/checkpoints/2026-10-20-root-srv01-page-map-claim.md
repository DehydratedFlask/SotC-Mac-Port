# SRV-01-PAGE-MAP-19660-01 — offline characterization claim

- Date/time: 2026-10-20 00:01 CDT (America/Chicago).
- Integrator: `/root`; assigned analyst: `/root/srv01_page_map_19660`.
- Task ID: SRV-01-PAGE-MAP-19660-01; parent SRV-01 / RE-02.
- Files changed at claim: `HANDOFF.md`, `docs/TASKS.md`, `docs/FOLDERS.md`, and this checkpoint.
  No native implementation or private analysis artifact has been changed by the claim.
- Failed hypotheses/tries: 0.

## Bounded scope

Characterize only the pinned local ELF function target at `0x19660` and enough direct callers,
callees, data references, and byte-level context to describe its observed control/data flow. This is
the next open SRV-01 page-map target. Nearby allocator comments and prior fixture lifts are clues,
not proof of this function's role. Do not label a bitmap/page allocator operation unless the bytes
and references support it. Do not write replacement source or change any Ghidra/REA baseline.

## Acceptance and limits

- Use the pinned `local/re/eboot.elf`; record its SHA-256, image base and REA/Ghidra versions.
- Use REA/Ghidra at `0x19660` and a distinct snapshot at `local/re/rea-19660-snapshot.json`.
- Preserve the exact REA command, analysis JSON/stderr, Evidence ID, function range/body hash,
  direct calls, data refs, unresolved flows, relationship coverage, and any independent raw-byte or
  disassembly verification under the existing ignored `local/re/evidence/` directory.
- Keep all binary-derived outputs private. Do not run the game, install or download tools, or alter
  `local/re/rea-snapshot.json`.
- A function dossier is static evidence only; it does not establish runtime behavior, reachability,
  service semantics, a native ABI, game startup, or G1.

## Claim checkpoint

- Exact target: `local/re/eboot.elf`, address `0x19660`.
- Exact command shape: pinned REA CLI `function local/re/eboot.elf 0x19660 --provider ghidra --snapshot local/re/rea-19660-snapshot.json --format json`; capture stdout/stderr and the exact environment in private command files.
- Exit/result: pending.
- Evidence/log paths: pending under ignored `local/re/evidence/`.
- Remaining risks: target boundary, function semantics, and relationship coverage are unknown.
- Next exact action: run one hash-bound REA/Ghidra inspection, export evidence, independently check raw bytes when available, and report limitations to root.
- Status: CLAIMED; 0 failed hypotheses/tries. Root owns shared docs, index, audits, commit, and push.

## Completion checkpoint — 2026-10-20 00:15 CDT

- Analyst/task: `/root/srv01_page_map_19660`; integrator and independent reviewer: `/root`.
- Task ID: SRV-01-PAGE-MAP-19660-01, parent SRV-01 / RE-02. Static characterization complete;
  no native implementation, runtime behavior, service semantics, or allocator identity claimed.
- Tracked files changed by root: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, and this
  existing checkpoint. Worker changed no shared source/docs. Private outputs remain under ignored
  `local/re/evidence/rea-19660/`; distinct snapshot is under ignored `local/re/`. No install or
  download occurred; `INSTALL_LOG.md` is unchanged.
- Pinned input: `local/re/eboot.elf`, 65,214,308 bytes, SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, x86-64 ELF64 little-endian.
  PT_LOAD mapping verifies address base 0 and maps `0x19660` to file offset `0x1d660`. REA 6.7.0,
  Ghidra 12.1.4, profile digest
  `769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa`; Evidence ID
  `ev_be5ca8d1f70434e1b56f9300aba0db39aa895878e4e311c026ed8d9d99a53359`. Distinct snapshot
  `local/re/rea-19660-snapshot.json` SHA-256
  `1fc68b01d0b6b8c1213f9e0c9e6968be1c944fbd184895a97fddd09ee961559e`; baseline remains
  `d80232c4fe22d7e75654eade8db44af093507a94723f80dd5ce67605dd382354`.
- Exact REA command (from repository root; exit 0; JSON 221,300 bytes; stderr empty):

  ```sh
  env GHIDRA_INSTALL_DIR=/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000 /Users/dehydratedflask/.npm/_npx/a1722018f28f9df2/node_modules/.bin/rea function local/re/eboot.elf 0x19660 --provider ghidra --snapshot local/re/rea-19660-snapshot.json --format json > local/re/evidence/rea-19660/analysis.json 2> local/re/evidence/rea-19660/analysis.stderr.log
  ```

- Worker independently mapped the PT_LOAD and captured full disassembly with
  `xcrun llvm-objdump -p local/re/eboot.elf` and
  `xcrun llvm-objdump -d --start-address=0x19660 --stop-address=0x197c5 local/re/eboot.elf`;
  body extraction command was
  `dd if=local/re/eboot.elf of=local/re/evidence/rea-19660/body-bytes.bin bs=1 skip=$((0x1d660)) count=357`.
  Body SHA-256 is `0061d51dbdb2987c464b28817d85a14d9f0e117db7c23ecbff117acb797e89bf`; independent
  objdump ends with `retq` at `0x197c4`. Detailed commands, stderr, and output hashes are in the
  private `local/re/evidence/rea-19660/analysis-summary.md`.
- Root independent verifier exact command:
  `PYTHONDONTWRITEBYTECODE=1 python3 -I local/re/evidence/rea-19660/root-verify.py > local/re/evidence/rea-19660/root-verification.json 2> local/re/evidence/rea-19660/root-verification.stderr.log`
  exited 0; result is `PASS`, JSON SHA-256
  `edd138e0caf9786ad77c104923bbd908b37c21eede685fc08f59081249726c4d`, stderr empty. Verifier
  SHA-256 `08215b3dd940154d38f699069e1329c723bbe9506a11d5ea1e9916e74d3c33f3`. It checks the live ELF
  PT_LOAD mapping/body against the raw extract, body and raw-prefix hashes, REA identity/range and
  call references, distinct/baseline snapshots, report hash, and the independent disassembly end.
- Repository indexing/graph checks: `python3 scripts/project_index.py` exited 0 and reported
  `4129 folders indexed; written`. `graft check` exited 0 with `graph check: OK — the wiring graph
  is in sync with the code`; the separate deep meaning tier remains unbuilt. No source files or
  directory scopes changed in this task.
- Static observation: Ghidra range is inclusive `0x19660..0x197c4` (357 bytes), one direct callee
  `0x19e00` at `0x196f8`, and six resolved incoming call sites across four caller functions. The body
  contains pointer/list-like field writes, a counter branch, a refill-like call path, and record
  initialization with `0x70` stride. No explicit bitmap-bit update is visible. The function's role,
  state validity, runtime path, and caller/callee contracts remain unresolved. REA reports no
  unresolved calls in the function and coverage `complete` only for resolved Ghidra references;
  computed/indirect flows remain unknown. No page-bitmap allocator label is justified.
- Tool issues, not analysis failures: `command -v rea; rea --version; rea function --help` exited
  127 because REA was not on PATH; the pinned CLI above then worked. `readelf -h -l` exited 127;
  `xcrun -f llvm-readelf` and `llvm-readobj` exited 72; `llvm-objdump` supplied headers/disassembly.
  Copying the ephemeral Ghidra log after the REA process exited returned 1 because its temporary
  directory had been removed. REA JSON and empty stderr are retained; no persistent headless log is
  claimed. No analysis hypotheses failed; failed hypotheses/implementation tries: 0.
- Observation versus inference: root's verifier independently confirms file bytes and Ghidra's
  recorded boundaries/references; the behavior description remains a static inference from
  disassembly and does not establish semantics or execution. No game was run; G1 remains NOT PASSED.
- Status: STATIC_ANALYSIS_VERIFIED; parent SRV-01 remains ACTIVE, page-bitmap/Fios semantics open.
  Next exact action: select a distinct bounded source/data-supported task; do not infer a native
  allocator or service contract from this function's name, neighboring comments, or stride alone.
