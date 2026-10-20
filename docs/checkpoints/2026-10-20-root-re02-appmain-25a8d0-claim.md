# RE-02-APPMAIN-25A8D0-01 claim — 2026-10-20 01:03 CDT

- **Agent:** `/root` (integrator); private analysis worker to be assigned after claim publication.
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
- **Next exact action:** dispatch one `gpt-6-luna` Extra High worker for private analysis only under
  `local/re/evidence/re02-appmain-25a8d0/`. Root will review and rerun its verifier before recording
  completion evidence.
