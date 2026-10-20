# RE-02-APPMAIN-25A8D0-01 claim — 2026-10-20 01:03 CDT

- **Agent:** `/root` (integrator); private analysis worker `/root/appmain_25a8d0`, `gpt-6-luna`
  Extra High.
- **Task:** `RE-02-APPMAIN-25A8D0-01`, parent tasks `RE-02` and `CPU-01`.
- **Status / attempt count:** ACTIVE, claim prepared; 0 failed analysis hypotheses.
- **Prerequisites:** RE-01 identity record; existing byte-level app-main notes at
  `local/re/work/app-main-re02.md` and `.json`, linked to the unchanged ELF below.
- **Pinned input:** `local/re/eboot.elf`, SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
- **Existing observations:** app-main's vtable slot at `0x15c8cf0` contains target `0x25a8d0`;
  caller `0x536730` invokes the slot with `RSI=-1`. Existing dossier links strings in the target to
  `GameCoreLoop::Run`, but retains C++ type binding and `BPE_HANDLE` meaning as inference/open.
  Existing `local/re/work/a25a8d0.dis` SHA-256 is
  `09fa7535ac5d42bd616116fe8d51b5d5d0ea3b067e90285dd7c71dd1f47509fb`; it stops at an unknown
  instruction around `0x25aacc` and does not establish complete body coverage.
- **Bounded outcome:** characterize only the full body at original RVA `0x25a8d0`: identify its
  byte-backed end/range/hash, direct calls and data references, direct incoming caller/vtable
  linkage, and unresolved computed/control-flow edges. Produce a private report, machine-readable
  verifier and exact REA/Ghidra analysis recipe with Evidence ID, source digest, image base, tool
  versions, coverage and unresolved indirect calls.
- **Owned files:** worker may create only private files under
  `local/re/evidence/re02-appmain-25a8d0/`; `/root` owns `HANDOFF.md`, `docs/TASKS.md`,
  `docs/EVIDENCE.md`, `docs/FOLDERS.md`, and shared checkpoints.
- **Acceptance check:** root independently runs the worker's verifier against the pinned ELF; it
  must exit `0`, verify the exact PT_LOAD-backed function bytes/range/hash, and corroborate
  reported calls/data references and vtable/caller linkage against raw bytes. Any missing or
  unresolvable relationship stays explicitly open. Keep the task unverified until this rerun passes.
- **Limits:** static offline evidence only. No reachability/execution, class identity, handle or
  service ABI, runtime behavior, native implementation, or G1 claim.
- **Claim preparation command:**
  `date '+%Y-%m-%d %H:%M:%S %Z' && git status --short --branch && sha256sum local/re/eboot.elf local/re/work/app-main-re02.md local/re/work/app-main-re02.json local/re/work/a25a8d0.dis && sed -n '1,200p' local/re/work/app-main-re02.md && cat local/re/work/app-main-re02.json && tail -30 local/re/work/a25a8d0.dis`
- **Result:** exit `0`, local time `2026-10-20 01:03:40 CDT`; clean tree before edits at
  `ba7b4319f1ae0226a587ac04151b1d992f2941dd`. Hashes and observed/open distinctions are recorded
  in the current `HANDOFF.md` checkpoint.
- **Publication result:** `python3 scripts/project_index.py` exited `0` (`4,135 folders indexed;
  written`); `graft check` exited `0` (wiring graph synchronized; deep meaning layer unbuilt,
  1,219 nodes pending); `git diff --check` and `git diff --cached --check` passed; staged path
  audit accepted 4 paths; full audit accepted 593 paths; `git fetch origin main` succeeded and
  pre-commit divergence was `0 0`. Commit `a7904b9` (`docs: claim app-main function analysis`)
  pushed successfully (`ba7b431..a7904b9`). Post-push tree was clean, HEAD equals `origin/main` at
  `a7904b959a0329db3404f75e490f1c1b4a739b21`, divergence `0 0`.
- **Worker preflight:** required project docs were read. The query
  `graft ask "Does any reference code establish the app-main meaning of vtable slot 0x15c8cf0?"
  --source --in reference/ShadowOfTheColossus-PC` exited `1` because no files under that reference
  prefix are indexed. The task concerns local ELF bytes; this is a Graft scope/tool limitation, not
  a failed analysis hypothesis. The worker reports no file changes and is proceeding with
  process-local REA/Ghidra CLI. Attempts remain 0.
- **Progress update:** the worker reports fresh REA 6.7.0 / Ghidra 12.1.4 analysis in progress.
  The preserved Ghidra log has generic body-overlap repair failures and p-code warnings elsewhere;
  its address-specific search had no `0x25a8d0` match. No target-specific conclusion follows from
  that absence; function Evidence has not yet been exported. The worker is continuing with
  raw-byte/PT_LOAD checks. Failed analysis hypotheses remain 0.
- **Next exact action:** review the private report/verifier under
  `local/re/evidence/re02-appmain-25a8d0/`; independently rerun it before recording completion
  evidence.

## 2026-10-20 01:14 CDT checkpoint — Ghidra function result returned

- **Agent / task / status:** `/root`; `RE-02-APPMAIN-25A8D0-01`; ACTIVE, 0 failed analysis
  hypotheses. Worker `/root/appmain_25a8d0` reports successful REA/Ghidra completion; root has not
  yet validated the function range against raw ELF bytes.
- **Private files changed by worker:** artifacts under
  `local/re/evidence/re02-appmain-25a8d0/`, including `rea-function.json`,
  `rea-function.exit.txt`, `rea-function.stderr.log`, `rea-command.txt`, and the completed
  `objdump-window.txt` / `objdump-window.command.txt` / `objdump-window.exit.txt`. Root changed
  only this checkpoint and `HANDOFF.md`.
- **Exact command / result:** the exact REA invocation is preserved in
  `local/re/evidence/re02-appmain-25a8d0/rea-command.txt`; its exit file contains `0`, and stderr
  is 0 bytes. The returned Evidence ID is
  `ev_167b8234fc082d63d7a17a8ea4ee218ff0b8559c2397945ae599ba0c657b72ba`.
  `xcrun llvm-objdump -d --x86-asm-syntax=intel --start-address=0x25a8d0 --stop-address=0x25b200 "/Volumes/Extra/SoC Mac Native/local/re/eboot.elf"`
  exited `0`; exact output and command are stored beside the REA result.
- **Worker-observed fields:** Ghidra reports one contiguous body from `0x25a8d0` through
  `0x25accc` inclusive, 1,021 bytes, 218 assembly entries and 24 basic blocks. It lists 24 call
  sites / 11 unique resolved callees, two targetless indirect calls at `0x25aa06` and `0x25ac9f`,
  and 3 incoming data references to the entry, including vtable slot `0x15c8cf0`. Callee
  relationship coverage is PARTIAL. These are tool observations pending raw-byte/reference review;
  no class contract, caller reachability, or runtime behavior follows.
- **Remaining risks / next exact action:** target-specific body bytes, terminal instruction,
  reference targets, vtable linkage, and unresolved-call coverage remain unverified. Receive the
  worker's completed verifier, rerun it independently against the pinned ELF, and inspect preserved
  logs before updating EVIDENCE or task status. G1 remains NOT PASSED.
- **Follow-up record publication:** commit `addc714` (`docs: record app-main claim publication`)
  was pushed. The tree was clean with HEAD equal to `origin/main` at
  `addc714b0e4a4fbf0ef8369943f1d7a1cd7052e1`, divergence `0 0`.
- **Latest shared checkpoint publication:** commit `5008163` (`docs: record claim publication
  state`) was pushed (`addc714..5008163`); it recorded the clean post-push state at
  `5008163a4b29129125785b5f1f6763eea7874c8e`, divergence `0 0`.
