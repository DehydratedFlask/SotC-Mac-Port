# CPU-01-CTOR10-E1EDE0-01 — direct helper characterization verified

- Date/time: 2026-10-20 00:46 CDT (America/Chicago).
- Integrator: `/root`; analyst: `/root/srv01_page_map_19660`.
- Task ID: `CPU-01-CTOR10-E1EDE0-01`; parent CPU-01; prerequisite E43/`CPU-01-CTOR10-ANALYSIS-01`.
- Status: `STATIC_ANALYSIS_VERIFIED`.
- Failed analysis hypotheses/tries: 0. Two checker assertions were corrected for E9 tail transfers and the observed branch destination; neither was a failed binary hypothesis.

## Analysis identity and exact commands

Input is the pinned local ELF `local/re/eboot.elf`, SHA-256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; reference clones and original
game files were not changed. REA 6.7.0 / Ghidra 12.1.4 produced Evidence ID
`ev_a78fb082a6584d31cebf0c320fc036205741f7093569f68e8355e90ce8237e0c`. The exact analysis command
and outputs are preserved in `local/re/evidence/rea-e1ede0/analysis.command.txt`; it exited 0 with
empty stderr. The distinct snapshot is `local/re/rea-e1ede0-snapshot.json` (SHA-256
`c298d5fd840d67b570bc683eb72b9a4ec6398ddbe1329d15db9ed719bee23bd1`); the shared baseline, E43 and
E46 snapshots remained unchanged.

The analyst's verifier command, rerun by root, was:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 -I local/re/evidence/rea-e1ede0/verify.py > local/re/evidence/rea-e1ede0/root-verification.stdout.log 2> local/re/evidence/rea-e1ede0/root-verification.stderr.log; result_code=$?; printf '%s\n' "$result_code" > local/re/evidence/rea-e1ede0/root-verification.exit.txt; exit "$result_code"
```

Root exit `0`; `root-verification.exit.txt` is `0`; stderr is empty. Root stdout SHA-256 and the
machine-readable `verification.json` SHA-256 both equal
`08e9f42004ad18be5599f5c9dfce903b7f71e992e70cedf23b37430d52a8e412`. Verifier SHA-256 is
`3550ae5c97202fd0c8e4ca86b86c60a00b2c7c6f16b4293683da710b22637b21`; report SHA-256 is
`fdbedbaaf50acbfcb16ef8e563ecd21fce126d06002d0de70e6007f0d39b37d2`.

## Verified observations

REA identifies `FUN_00e1ede0` over `0xe1ede0..0xe1ee45` (102 bytes; body SHA-256
`fe902c0318c3b14913f6c88d21e54d96a55bc56f1f4c78c3a9648922450add1d`). Root's verifier maps and
checks the body through the ELF PT_LOAD table, validates 92 incoming transfers (58 `E8 CALL rel32`,
34 `E9 JMP rel32` tail transfers), and confirms the examined caller setup at `0xf09f04` supplies the
`RSP+8` address after the caller-local guard path skips the separate `0xf09efb` indirect call.

Bytes in the helper copy input `RDI` to `RBX`, write pointer `0x15e2d70` through `[RBX]`, conditionally
call twice through `[RAX+0xd0]`, and clear qwords `[RBX+0x38]` / `[RBX+0x30]` after a returning call.
REA resolves no direct callees; the two computed calls at `0xe1ee0d` and `0xe1ee31` remain unresolved.
The referenced data addresses `0x15e2d60` and `0x15e2d70` fall in a PT_LOAD gap and their object bytes
are not available from this ELF. Relationship coverage is partial; incoming sites `0xf0026e` and
`0xf059ee` have no Ghidra caller-function record.

These are static byte/reference observations only. They do not assign a service or ABI role, establish
constructor reachability/execution, or prove a native implementation or G1. Apple's `objdump` emitted
its no-section-overlaps warning on the bounded commands because the ELF has no section table; those
commands still decoded PT_LOAD bytes and exited 0. No game execution, installation, or download.

## Shared checkpoint and next action

Shared files updated by root: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, and this completion
checkpoint. `python3 scripts/project_index.py`, `graft check`, path audits, whitespace checks, fetch,
commit and push are recorded in the following publication checkpoint in `HANDOFF.md`.

Exact next action: choose and claim the next bounded CPU-01 task only after reviewing unresolved
callee/data provenance; keep the two computed edges unresolved until independently established.
Failed analysis hypotheses/tries: 0.
