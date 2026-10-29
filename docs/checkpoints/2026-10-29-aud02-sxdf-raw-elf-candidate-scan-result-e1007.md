# E1007 — SXDF/SXDS raw ELF candidate scan result

**Date:** 2026-10-29 13:48 CDT
**Agent:** `/root` + independent `/root/audio_lane_triage` replay
**Task:** `AUD-02-SXDF-RAW-ELF-CANDIDATE-SCAN-01`
**Status:** `ARTIFACT_VERIFIED`

## Result

The pinned `local/re/eboot.elf` is 65,214,308 bytes with SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. It is ELF64 little-endian, has 11 program headers, and has no section table. PHDR 0 is a file-backed R-X PT_LOAD mapping file `[0x4000,0x1597978)` to RVA `[0,0x1593978)`; PHDR 2 is RW and begins at file `0x1608000` / RVA `0x1604000`.

An overlapping full-file scan found `SXDF` 8 times and `FDXS`, `SXDS`, and `SDXS` zero times. All eight `SXDF` hits map uniquely through PHDR 0:

| File offset | RVA | Classification in bounded window |
|---:|---:|---|
| `0x989992` | `0x985992` | Immediate in `cmp` at `0x98598c` |
| `0x989a7c` | `0x985a7c` | Immediate in `cmp` at `0x985a78` |
| `0x98e354` | `0x98a354` | Immediate in `cmp` at `0x98a352` |
| `0x99128e` | `0x98d28e` | Immediate in `cmp` at `0x98d28b` |
| `0x9b7bea` | `0x9b3bea` | Immediate in `mov` at `0x9b3be6` |
| `0x9b7d3d` | `0x9b3d3d` | Immediate in `cmp` at `0x9b3d3b` |
| `0x9b9afb` | `0x9b5afb` | Unresolved overlap inside `incl 0x44585338(%rcx)` and following branch-prefix bytes |
| `0x12be754` | `0x12ba754` | String-like data inside `FORM\0VAGp\0SXDF\0fmt \0smpl\0fact\0data\0cue \0adtl\0INF` |

Root's Rizin 0.9.1 plus Homebrew LLVM 23.1.3 and an independent Rizin 0.9.1 plus Xcode Apple LLVM 21.0.0 replay agree. The 48-byte window around `0x9b5afb` corrected an initial hit-aligned decode that had started at a non-instruction boundary. A nearby `lea` at `0x9b5b10` computes the data base `0x12ba74a`; the next instruction loads length 4. This establishes candidate byte/address correspondence only. It does not establish a reader, decoder, codec, audio route, or runtime use.

## Verification, limits, and next step

- The formal raw scan and exact bytes are reproducible using the private script and logs under `local/validation/AUD-02-SXDF-RAW-ELF-CANDIDATE-SCAN-01/analysis/attempt-01/REPORT.md`.
- The task was artifact-verified only. No REA/Ghidra query, PSARC payload access, source edit, or game execution occurred. No product gate advances; audio format and playback remain unknown.
- The corrected manifest identifies an exact `sound/shadow/music.sxd1` candidate in `pakN.psarc`. The separately claimed E1008 task will live-verify archive/member identity and compare only its first 48 decoded bytes in memory.
