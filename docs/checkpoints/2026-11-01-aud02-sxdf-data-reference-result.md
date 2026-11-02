# E1060 — SXDF code-to-data reference: Ghidra and the pinned ELF agree

**Date:** 2026-11-01 20:20 CST
**Agent:** `/root` subagent (`AUD-02` lane)
**Task:** `AUD-02-SXDF-DATA-REFERENCE-GHIDRA-01`
**Status:** `STATIC_ANALYSIS_VERIFIED` (two providers agree on bytes and address mapping; no reader,
codec, audio-use, runtime or game-gate claim)

## Why this run could happen at all

E1012 blocked on a REA/Ghidra query that timed out at 330,000 ms before returning bytes, an image
base, or an Evidence ID, and E1013/E1014 could not recover either. Since then a working headless
pipeline exists (`docs/GHIDRA_PIPELINE.md`): `analyzeHeadless` cannot start a `.py` script in this
install, and `tools/re/ghidra/run_dump.py` is the entry point that starts PyGhidra and runs an
ordinary GhidraScript. This task used that entry point and nothing else.

**No re-import, no re-analysis, no decompilation, no xrefs, no asset read, no guest instruction
executed.** Two addresses were read.

## Program identity and image base (the fact E1012 could not get)

From the GhidraScript itself (`SXDF` lines in `sxdf-ref.txt`):

| Field | Value |
|---|---|
| `program_name` | `eboot.elf` |
| `language` / `compiler` | `x86:LE:64:default` / `gcc` |
| `image_base` | **`0x0`** |
| `executable_path` | `/Volumes/Extra/SoC Mac Native/local/re/eboot.elf` |
| `executable_md5` | `13520a0bf3f678df41544849b74d331f` |
| `executable_sha256` | `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3` |
| `function_count` | `38845` |

The SHA-256 is the pinned image, computed by the script from the file Ghidra recorded as the
program source, and independently by `shasum -a 256 local/re/eboot.elf`. Image base `0x0` means RVA
== address for every query here, so the E1012 procedure-order blocker (image base unknown before an
address query) is discharged rather than bypassed.

## Ghidra answer — instruction `0x9b5b10`

```
SXDF instruction_rva=0x9b5b10
SXDF instruction_address=0x9b5b10
SXDF instruction_present=yes
SXDF instruction_text=LEA RSI,[0x12ba74a]
SXDF instruction_mnemonic=LEA
SXDF instruction_length=7
SXDF instruction_bytes=488d35334c9000
SXDF instruction_fall_through=0x9b5b17
SXDF instruction_block=segment_0 perms=R-X
SXDF instruction_function=FUN_009b59c0@0x9b59c0
SXDF operand_count=2
SXDF operand_0=RSI
SXDF operand_0_object_0=Register:RSI
SXDF operand_1=[0x12ba74a]
SXDF operand_1_object_0=Scalar:0x12ba74a
SXDF instruction_reference_count=1
SXDF instruction_reference_0=0x12ba74a type=DATA
SXDF rip_target=0x12ba74a
SXDF rip_target_source=reference
SXDF byte_disp32=0x904c33
SXDF byte_target=0x12ba74a
```

Ghidra folded the rip-relative displacement into an absolute operand (`[0x12ba74a]`) and added one
**DATA** reference from the instruction to that address. The byte-level computation (`end of
instruction + sign-extended disp32`) produces the same target, so the reference is not an artifact
of the operand rendering.

Context only, not used below: the containing function is `FUN_009b59c0`, and the fall-through
`0x9b5b17` is `mov edx,4`. No body was decompiled or read.

## Ghidra answer — data `0x12ba74a`

```
SXDF data_rva=0x12ba74a
SXDF data_address=0x12ba74a
SXDF data_kind=inside-data
SXDF data_exact=none
SXDF data_containing=?? 46h    F
SXDF data_containing_start=0x12ba74a
SXDF data_containing_length=1
SXDF data_containing_type=undefined
SXDF data_block=segment_0 perms=R-X start=0x0 end=0x1593977
SXDF data_bytes_length=48
SXDF data_bytes=464f524d0056414770005358444600666d742000736d706c006661637400646174610063756520006164746c00494e46
SXDF data_ascii=FORM\0VAGp\0SXDF\0fmt \0smpl\0fact\0data\0cue \0adtl\0INF
```

Ghidra does **not** model a typed data item or string at `0x12ba74a`: the address sits in an
undefined-byte region, and the containing code unit is a one-byte `undefined` holding `0x46`
(`'F'`). The modelled content is therefore the byte run, not a data type. The 48 bytes are exactly
the E1007 window.

## Independent ELF cross-check (the authority)

`scripts/ps4_dynlink.py` (`program_headers` + `Image.read`), no disassembler in the loop:

```
elf_bytes=65214308
elf_sha256=d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3
elf_entry=0xf120 phdrs=11
instr_rva=0x9b5b10 bytes=488d35334c9000
disp32=0x904c33
target=0x12ba74a
target_mapped=True
data48=464f524d0056414770005358444600666d742000736d706c006661637400646174610063756520006164746c00494e46
data48_escp=b'FORM\x00VAGp\x00SXDF\x00fmt \x00smpl\x00fact\x00data\x00cue \x00adtl\x00INF'
next_instr_rva=0x9b5b17 bytes=ba04000000
map addr=0x9b5b10 phdr=0 type=0x1 file_off=0x9b9b10
map addr=0x12ba74a phdr=0 type=0x1 file_off=0x12be74a
```

Third provider, the saved disassembly `local/validation/registry-analysis-agent/all.dis`, line
2,650,579:

```
  9b5b10:      	leaq	0x904c33(%rip), %rsi    # 0x12ba74a <PT_LOAD#0+0x12ba74a>
```

## Agreement

| Fact | Ghidra 12.1.4 | `ps4_dynlink` on the pinned ELF | `all.dis` |
|---|---|---|---|
| instruction bytes at `0x9b5b10` | `488d35334c9000` | `488d35334c9000` | `leaq 0x904c33(%rip), %rsi` |
| length | 7 | 7 | 7 |
| disp32 | `0x904c33` | `0x904c33` | `0x904c33` |
| computed target | `0x12ba74a` | `0x12ba74a` | `0x12ba74a` |
| 48 bytes at target | `464f524d…494e46` | `464f524d…494e46` | — (not a code range) |
| containing mapping | `segment_0` R-X | PHDR 0, file `0x12be74a` | `<PT_LOAD#0+0x12ba74a>` |

**They agree; there is no disagreement to report.** The 48 bytes are byte-identical across the two
providers, and the reference target is the same address in all three. What this establishes is
address correspondence and bytes only: `0x9b5b10` really is a 7-byte reference into the `FORM/VAGp/
SXDF` byte run. It does not establish a reader, a format claim, a codec, an audio route, or any
runtime use — E1007/E1009 already bounded the byte facts and remain the source for them.

## Checker

New: `tools/lift/check_sxdf_data_reference.py`. It pins the instruction bytes, the disp32 and the
computed target, the first 48 bytes at `0x12ba74a`, the containing-segment mapping, the program
identity (name, language, image base `0x0`, pinned ELF SHA-256, function count 38,845), and it
re-parses the Ghidra dump when one is present. It also asserts the Ghidra script stays bounded: the
script may not grow `DecompInterface`, `getReferencesTo`, or a code-unit iterator. `SKIP` exit 0
when all three private inputs are absent.

Measured 2026-11-01 20:24 CST:

| Run | Result |
|---|---|
| `python3 -I tools/lift/check_sxdf_data_reference.py` | exit 0 — `elf sha256 pinned, instruction bytes pinned, disp32/target pinned, data 48 bytes pinned, Ghidra identity pinned (base 0x0), Ghidra instruction/reference pinned, Ghidra data bytes pinned, saved disassembly line pinned` |
| Same, dump with the `data_bytes` first nibble mutated | exit 1 — `FAIL dump data_bytes='464f524e…', pinned '464f524d…'` |
| Same, dump with `rip_target=0x12ba74b` | exit 1 — `FAIL dump rip_target='0x12ba74b', pinned '0x12ba74a'` |
| Same, dump truncated to 20 lines | exit 1 — `FAIL dump is missing rip_target` |
| Same, copy of the ELF with one disp32 byte flipped (`--elf`) | exit 1 — `FAIL … is not the pinned image` (the SHA gate fires first) |
| Same, all three private inputs absent (`--elf`/`--dump`/`--saved-dis` at missing paths) | exit 0 — `SKIP check_sxdf_data_reference: pinned ELF / Ghidra dump / saved disassembly absent (address and identity pins checked)` |

The GhidraScript's own verdict lines, from the dump: `verdict_instruction_matches_e1007=yes`,
`verdict_target_matches_e1007=yes`, `verdict_ghidra_and_bytes_agree=yes`.

## Commands run (exact)

```sh
export JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home
export PATH="$JAVA_HOME/bin:$PATH"
export GHIDRA_INSTALL_DIR=/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC

# 1. Ghidra provider (persistent project, read-only, no analysis re-run)
./local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
  --project-dir local/ghidra/persistent-20261020 --project-name SotC \
  --script tools/re/ghidra/check_sxdf_ref.py \
  --out local/validation/aud02-sxdf-data-reference-ghidra-20261101/sxdf-ref.txt

# 2. Independent ELF provider (python3 -I - heredoc, stdlib only)
#    scripts/ps4_dynlink.py: program_headers(raw) -> Image(raw, ph).read(addr, size)
#    Output saved verbatim to local/validation/aud02-sxdf-data-reference-ghidra-20261101/elf-crosscheck.log
```

Artifacts (private, `local/`):

| Path | SHA-256 |
|---|---|
| `local/validation/aud02-sxdf-data-reference-ghidra-20261101/sxdf-ref.txt` | `b2bce2e195448d6f32f9ac67e6785939deddd21fff6d49ed00fc631a6606ca10` |
| `.../elf-crosscheck.log` | `f3a3ce736951df8c52a26abbdbafad09e9594f1aaf6b0da729a50aa6f43306a6` |
| `.../run.log` | `5ddf7f8fde0a8eeb59fa55f52934fcdb5a6021992afc5f4bb0920e429feeff28` |

## Attempts

- **Script attempt 1 — FAILED (tool API, not a hypothesis).** `memory.getBytes(Address, int)` has no
  matching overload on this install's `MemoryMapDB`:
  `TypeError: No matching overloads found for …MemoryMapDB.getBytes(GenericAddress,JInt)`. The script
  now reads bytes one at a time through `getByte(Address)`. That log was overwritten by the
  successful run; the traceback is quoted from the run output.
- **Run 2 — PASSED**, exit 0. Re-run against the same project produced a byte-identical dump
  (`RERUN_IDENTICAL=yes`), so the Ghidra side is deterministic on this frozen project.

No provider query timed out and no semantic hypothesis failed, so the three-try escalation rule is
not engaged. E1012's single failed provider query stays on the task's tally as history.

## Limits

- The Ghidra project is the 2026-10-20 import; this run proves the file it names still hashes to the
  pinned image and that the two addresses read the same as the file. It does not re-verify that the
  in-database byte image was never edited after import. The output nonetheless matches the ELF byte
  for byte, which is the property that matters here.
- Ghidra's `data_kind=inside-data` / `data_exact=none` is a statement about Ghidra's markup, not
  about the file: no `.rodata` string type was applied there.
- Function `FUN_009b59c0` is only the containing body. No claim is made about what it does.
- Still UNKNOWN and out of scope: format, codec, audio route, playback, and whether any code path
  actually consumes this byte run at run time.

## Files changed

| File | Change |
|---|---|
| `tools/re/ghidra/check_sxdf_ref.py` | new — two-address GhidraScript |
| `tools/lift/check_sxdf_data_reference.py` | new — byte/identity checker |
| `docs/checkpoints/2026-11-01-aud02-sxdf-data-reference-result.md` | new — this checkpoint |
| `docs/TASKS.md` | `AUD-02-SXDF-DATA-REFERENCE-GHIDRA-01` row updated |
| `docs/EVIDENCE.md` | `E1060` row added |
| `docs/FOLDERS.md` | regenerated by `project_index.py` (also carries peer-owned counts, see above) |

No `native/`, `launcher/`, `reference/`, `HANDOFF.md`, `tools/dev/*`, `tools/re/ghidra/{run_dump,
dump_functions,probe_project}.py`, or `tools/lift/indirect_frontier.py` change. No git
commit/tag/push.

**Next exact action:** nothing further for this task. A separate task could ask whether anything
references the surrounding `FUN_009b59c0` path, but that is a new scope and needs a new claim.

## Publication / tree checks

| Command | Result |
|---|---|
| `python3 scripts/project_index.py` | exit 0 — `48031 folders indexed; written` |
| `python3 scripts/project_index.py --check` | **exit 1** — `Stale indexes: local/indexes/folders.json`. Cause measured, not this task: a peer was writing `local/build/consvar/**` while the index was produced. Recomputing the inventory showed 71 differing folders at that instant — 70 under `local/` (build products) and `native/core` 1500→1503 (peer files). The private index was regenerated again after this checkpoint; the public `docs/FOLDERS.md` write matched. |
| `python3 -I scripts/audit_publish_paths.py --all` | exit 0 — `PUBLISH PATH AUDIT OK (3996 path(s))` |
| `python3 -I scripts/audit_publish_paths.py` (staged mode) | **NOT RUN** — it needs an index; this worker does not stage or commit, so the integrator runs it at commit time |
| `cmake -S native …` / build / CTest | **NOT RUN** — no `native/`, `launcher/`, CMake or toolchain input changed in this task; the integrator runs the gates at commit |
| `tools/lift/run_all_checkers.sh` | **NOT RUN** — prohibited by the task brief; `check_sxdf_data_reference.py` is discovered by its `tools/lift/check_*.py` glob and is not excluded by the `check_all_*` / `*util*` filters, which was verified by reading the script, not by running it |
| `git commit` / `tag` / `push` | **NOT RUN** — prohibited; the tree carries uncommitted changes for the integrator |

`docs/FOLDERS.md` was regenerated by `project_index.py`. Its diff also carries peer-owned counts
(`tools/dev` 19→21 from `tools/dev/cxml_probe.py`, and `native/core`), so the integrator should
attribute only the `docs/checkpoints` 1010→1011, `tools/lift` 769→771 and `tools/re/ghidra` 3→4
lines to this task.
