# Slot-309 raw-literal census (E894)

- **Date:** 2026-10-28
- **Task:** `CPU-01-STARTUP-SLOT-309-RAW-LITERAL-CENSUS-01`
- **Owner:** `/root`; independent read-only ELF verification by `/root/slot309_elf_verify`
- **Checkpoint time / agent:** 2026-10-28 02:40 CDT, `/root`; repository validation checkpoint 02:43 CDT, `/root`
- **Starting state:** clean `main == origin/main == dd46675db9c3e07999564b826482a102b1a0289d`
- **Status:** `ARTIFACT_VERIFIED` for a negative raw-literal consumer-search route; the parent slot-309 characterization remains `ACTIVE`.
- **Files changed for this checkpoint:** `AGENTS.md`, `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and this checkpoint file. Private scripts and reports stay under ignored `local/validation/orch-20261028/e894/`.

## Scope

Search every file-backed byte of the pinned local ELF for all unaligned little-endian 32-bit and 64-bit integers in the half-open VMA literal range `[0x15d5b60,0x15d5bd0)`. This is distinct from E883's saved Ghidra ReferenceManager sweep and E818's dynamic RELA population analysis. The scan does not decode instructions and cannot discover RIP-relative, computed, indirect, or runtime-created references.

The input was `local/re/eboot.elf`, 65,214,308 bytes, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.

## Commands and results

1. `python3 -I local/validation/orch-20261028/e894/slot309_raw_literal_scan.py` — exit 0. The complete overlapping-byte search found 12 u32 and 12 u64 matches at the same 12 file offsets, covering literals from `0x15d5b60` through `0x15d5bc8`. The raw report is private at `local/validation/orch-20261028/e894/slot309-raw-literal-report.json`, SHA-256 `723cf9c1c05f16d655f38b41502f62667e1e4ef5f14517097a2f98dfaa97c160`.
2. `python3 -I local/validation/orch-20261028/e894/slot309_elf_reloc_map.py > local/validation/orch-20261028/e894/slot309-elf-reloc-map-stdout.json` — exit 0. All 12 unique hit offsets fall within program header 8 (raw type `0x61000000`, file range `[0x2ddb100,0x3e30bb0)`) and repeat at 24-byte intervals. At each offset, the first qword is the literal and the adjacent qwords decode as `r_info=8` and addend under the ELF64 Rela-shaped format. The full map is private at `local/validation/orch-20261028/e894/slot309-elf-reloc-map.json`, SHA-256 `1a949c1f68fc8b53a524985abd9380bbdb430f022ea563d2bbb7802506731e3a`.

The ELF header exposes 11 program-header entries and no section-header table (`e_shoff=0`, `e_shnum=0`), so this checkpoint makes no section-name mapping claim. The wrapper destinations `0x15d5b80/0x15d5b88` map through program header 1 (raw type `0x61000010`, file start `0x1598000`, VMA start `0x1594000`) to file offsets `0x15d9b80/0x15d9b88`. Their relocation-shaped records are at file offsets `0x3dc03b0/0x3dc03c8`, with addends `0xb11360/0xb11370`, matching the existing E818 rows 692840/692841. This supports classifying the raw matches as relocation metadata destinations; they do not demonstrate a table consumer.

An initial mapper run exited 1 because it assumed the PHDR 8 file start was aligned to the 24-byte record origin. The first hit is offset by 8 modulo 24 from the segment start, while subsequent hits share the 24-byte stride. The mapper was corrected to validate that repeated stride and the record fields without inventing an absolute index. This was a parser-origin assumption, not a failed semantic analysis try. The corrected result was independently checked against the pinned input and report hashes; no source or game code was executed.

## Interpretation and limits

Observed: all raw scan offsets, bytes, ELF header fields, program-header ranges, and adjacent qwords. Inferred: the 24-byte triples are ELF64 Rela records and type number 8 corresponds to the x86-64 relative relocation. The wrapper pair agrees with E818's previously verified RELA entries. The scan does not establish table consumption, call order, ownership, runtime reachability, or native-liftability, and cannot rule out references that use other addressing or dispatch forms.

Keep slot 309 unregistered, its native-lift boundary unresolved, and G1 `PARTIAL`. APP-01 VoiceOver attempt 4 remains waiting for manual Mac unlock; CUA still reports locked and no VoiceOver setting changed. No game/runtime acceptance gate advanced.

## Repository validation and publication

Before publication: `cmake -S native -B local/build/orch-r22` and `cmake --build local/build/orch-r22 -j 12` passed, exit 0 (`ninja: no work to do`); `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` passed **431/431**; `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` passed **516/0/0**. `trail build --check` reported the wiring graph current at **9,877 nodes**; the deep summary layer remains unbuilt. `python3 scripts/project_index.py` and `python3 -I scripts/project_index.py --check` passed at **32,062/current**. Staged/all-path audits passed **7/7** and **3,111/3,111**; the default attribution check passed for empty `origin/main..HEAD`; cached/worktree `git diff --check` passed. Logs are under `local/validation/orch-20261028/e894/`.

Remote preflight: `git fetch origin main` exited 0; `HEAD`, `origin/main`, and `FETCH_HEAD` were all `dd46675db9c3e07999564b826482a102b1a0289d`; no incoming or local commits were listed. The local author/committer identity is Johnny Wang `<dehydratedflask@users.noreply.github.com>`. `refs/stash` remains `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`.

The owner-authored checkpoint commit, exact-range attribution check, fast-forward push, and live-ref verification are recorded in the subsequent E895 publication closeout.
