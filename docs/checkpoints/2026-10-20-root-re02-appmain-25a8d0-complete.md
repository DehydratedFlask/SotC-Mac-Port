# RE-02-APPMAIN-25A8D0-01 — bounded app-main static dossier complete

- Time: 2026-10-20 01:22 CDT (America/Chicago).
- Agent / integrator: `/root`.
- Task: `RE-02-APPMAIN-25A8D0-01`; parent RE-02 and CPU-01 remain ACTIVE.
- Status: `STATIC_ANALYSIS_VERIFIED`; G1 remains NOT PASSED.
- Files changed in this checkpoint: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/FOLDERS.md`, this checkpoint, and the escalation record. No source files changed. Private REA output remains under `local/re/evidence/re02-appmain-25a8d0/`.

## Input, evidence, and root replay

- Exact local ELF: `local/re/eboot.elf`, 65,214,308 bytes, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
- REA CLI 6.7.0 / Ghidra 12.1.4 Evidence ID: `ev_167b8234fc082d63d7a17a8ea4ee218ff0b8559c2397945ae599ba0c657b72ba`; exact REA command and stdout/stderr/exit are in the private task folder.
- Report SHA-256: `f8634cf2eca0b4bd1fcfcba4d8fa96ce628170743d9be6e50f1124519edf20c3`.
- Verifier SHA-256: `45fa51c850e3aba9a91bf6ca8d0d2f5fd403e5430639da79f165ad2985f6d85b`.
- Worker verifier result SHA-256: `173a22dab0846535c1947627942087e98fe1a09c535b5db0615d6bdfa9b356cd`.
- Exact root command:
  `python3 -I local/re/evidence/re02-appmain-25a8d0/verify.py > local/re/evidence/re02-appmain-25a8d0/root-verification.json 2> local/re/evidence/re02-appmain-25a8d0/root-verification.stderr.log; result_code=$?; printf '%s\n' "$result_code" > local/re/evidence/re02-appmain-25a8d0/root-verification.exit.txt; exit "$result_code"`
- Root replay exited `0`; stderr is empty; `cmp -s local/re/evidence/re02-appmain-25a8d0/verification-result.json local/re/evidence/re02-appmain-25a8d0/root-verification.json` exited `0`. Root output SHA-256 matches the worker result.

## Verified observations

- Ghidra reports a contiguous body `0x25a8d0..0x25accc` inclusive: 1,021 bytes, 218 instructions, 24 basic blocks, 24 direct calls to 11 unique targets, and 15 direct branch edges.
- The body is backed by PT_LOAD #0 at file offset `0x25e8d0`; body SHA-256 is `8d23572ada7de95be980c85cfa0faa873efe69c1226c141f01c02f19b1fffb08`. The final instruction at `0x25acc8` ends exactly at the claimed range boundary.
- All 70 reported data references and three incoming pointer words were checked against raw bytes and program-header mappings. The word at `0x15c8cf0` is `0x25a8d0`.
- Static instructions at `0x536730` set RSI to `-1`, pass the constructed object in RDI, and call through `[RAX+0x18]`; the inspected constructor stores address point `0x15c8cd8` into that object. These bytes establish static linkage only.
- The body also has a file-backed view via PHDR #8 (`p_type=0x61000000`) at alternate file offset `0x30359d0`. Its platform meaning remains unclassified. Code validation uses PT_LOAD #0; for data references the verifier prefers PT_LOAD where present and uses a unique non-PT_LOAD mapping only when no PT_LOAD covers the address.

## Failed verifier tries and principal reassessment

Three distinct verifier assumptions failed; none was an analysis hypothesis about game behavior:

| Try | Hypothesis/change and targeted check | Expected / actual | Evidence |
|---|---|---|---|
| 1 | First worker verifier required every non-stack data reference to map to file bytes. | Expected a file-backed target; Ghidra reference `0x2ea5170` lies in PT_LOAD #2 memory beyond `p_filesz` (zero-fill). The verifier was corrected to distinguish file-backed bytes from zero-fill memory. | `verification-initial-note.md`, `verification-initial.stdout.log`, `verification-initial.stderr.log` |
| 2 | A subsequent verifier treated multiple raw program-header file views as ambiguous for all data pointers. | Expected unique raw mapping; address `0xfc3c9a` has a PT_LOAD #0 view and a non-PT_LOAD view at a different file offset. The verifier was corrected to prefer PT_LOAD. | `verification-pre-rip-supplement.*`; exact history retained in `verification-initial-note.md` |
| 3 | Root's separate PHDR audit asserted one file-backed mapping for the whole function across all headers. | Expected one mapping; found PT_LOAD #0 at `0x25e8d0` and PHDR #8 at `0x30359d0`. The all-header uniqueness assertion was rejected; PHDR #8 is retained as unclassified metadata, not used for code bytes. | `root-phdr-audit.py`, `root-phdr-audit.log`, `root-phdr-audit.stderr.log` |

At the third failed verifier try the task reached the escalation threshold. Principal `/root`
reassessed and authorized one final corrected verification/replay only, because all failures were
mapping-policy coverage issues and the target body/calls could be checked against PT_LOAD bytes.
That final worker verifier and independent root replay passed. No further analysis scope was
authorized. The task register records three verifier failures and zero failed analysis hypotheses;
the [escalation file](../escalations/RE-02-APPMAIN-25A8D0-01.md) preserves the decision.

## Limits and next action

Two indirect calls remain targetless: `0x25aa06 [RAX+0x20]` and `0x25ac9f [RAX+0x28]`.
Ghidra's callee coverage is partial. The literal strings `GameCoreLoop::Run(BPE_HANDLE)` and
associated path/text are observations, not a proven C++ class, ABI, or semantic contract. No
runtime call, constructor reachability, behavior, native implementation, game execution, audio,
save/load, or G1 result is claimed.

Exact next action: claim and fix APP-01 folder-import cancellation classification in LauncherCore,
then run its focused Swift package tests. Parent RE-02 remains ACTIVE.
