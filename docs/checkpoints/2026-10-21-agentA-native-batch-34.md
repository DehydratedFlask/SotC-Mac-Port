# Checkpoint: CPU-01-LEAF-CTOR-BATCH-34 — twelve call-free leaf ctors lifted (indices 172–466)

- Time / timezone: 2026-10-21 (CDT, America/Chicago), macOS 27 arm64
- Agent and integrator: worker `agent-a` (Claude Sonnet 5.5); integrator is the orchestrator session.
  `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `INSTALL_LOG.md`, indexes were **not** edited;
  nothing was committed or pushed.
- Task / gate / status: CPU-01-LEAF-CTOR-BATCH-34. Status: **COMPLETE — 12 lifts, 0 failed tries,
  FIXTURE_VERIFIED**. None registered in `startup_ctors`; G1 not passed.
- Input: `local/re/eboot.elf` sha256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`
  (asserted by every checker). Candidate list from `local/re/work/ctor-classes.json`
  (`class == other`, no direct/indirect calls, index >= 150, no existing lift).
- Files added: `native/core/leaf_<rva>.{hpp,cpp}`, `native/tests/test_leaf_<rva>.cpp`,
  `tools/lift/check_f<rva>.py` for the 12 RVAs below; shared `tools/lift/leaf_ctor_decode.py`
  (strict exact-encoding store decoder) and `tools/lift/leaf_check_common.py` (tie harness).
  Edited (additive): `native/CMakeLists.txt` (12 sources + 12 test targets, inserted after
  `avx_d5a280`), `native/README.md` (12 rows + checker names + decoder/harness note).

## Lifted bodies

| Index | RVA | Bytes / insns | Stores | Region (BSS) | Notes |
|---|---|---|---|---|---|
| 172 | `0xcab080` | 93 / 13 | 6 x 16 | `0x3351b20..0x3351b80` | same 5 rodata copies + zero as `0xd8fa40`-style |
| 174 | `0xca18d0` | 93 / 13 | 6 x 16 | `0x3351ab0..0x3351b10` | same shape |
| 176 | `0xc94ae0` | 93 / 13 | 6 x 16 | `0x3351a40..0x3351aa0` | same shape |
| 285 | `0xb692a0` | 93 / 13 | 6 x 16 | `0x3350d30..0x3350d90` | same shape |
| 179 | `0xc811b0` | 33 / 5 | 2 x 16 | `0x3351a00..0x3351a20` | two rodata copies |
| 321 | `0x8a0440` | 49 / 7 | 3 x 16 | `0x3294f40..0x3294f70` | three rodata copies (address range differs: `0x329xxxx`) |
| 237 | `0xc05980` | 31 / 5 | 16 + 2 x 1 | `0x3351290..0x33512a2` | `{~0,~0,~0,0}` mask plus two byte stores of 4 |
| 187 | `0xc6f520` | 66 / 9 | 2 x 32, 16, 8 | `0x3351930..0x3351988` | `movabs` -1.0f<<32 store plus ymm copies |
| 188 | `0xc6e9d0` | 149 / 19 | 6 x 32, 2 x 16, 2 x 4 | `0x3351830..0x3351930` | ymm chain; 4-byte stores 0 and -1.0f at `0x3351880/84` |
| 304 | `0xa816d0` | 47 / 7 | 4 x 32 (unaligned, overlapping), 4 | `0x32fb1c0..0x32fb220` | all zero; 3-byte gaps at `+0x29..0x2c` and `+0x59..0x5c` are never written |
| 466 | `0x3ca7c0` | 38 / 8 | 3 x 32 zero | `0x2ef6f88..0x2ef6fe8` | zero fills of three 32-byte cells at 0x20 stride (never-written gaps: none) |
| 229 | `0xc0d380` | 92 / 13 | 32, 2 x 16, 5 x 8 | `0x3351320..0x33513c0` | `[rax+disp]` stores from a `lea` base; constants 0x20/0x18 |

Per-body SHA-256 and the ordered store lists are pinned in each checker.

## Method and verification

- Each body was disassembled from the pinned ELF with `scripts/disasm_range.sh` (tab-split parse;
  its `#` comments were ignored, targets re-derived from `rip + next_ip + disp`). The lifts come from
  `tools/lift/leaf_ctor_decode.py`, which matches instruction **encodings** (VEX2 vmovups/vmovss/
  vxorps, `lea`, `movabs`, `mov [mem],reg/imm`, `ret`) and raises on anything else; its instruction
  boundaries were cross-checked against llvm-objdump for all 12 bodies (identical (pc, length) list).
- Native code: `init_leaf_<rva>(Leaf<Rva>&)` rewrites the covered bytes of a plain
  `std::array<uint8_t,N>` in the original store order with 8/4/1-byte native writes; bytes no store
  covers are never touched. No guest state, no embedded original bytes beyond the recovered constants.
- Tests: each `test_leaf_<rva>` compares to the full expected image, per-store 32-bit lane oracles,
  a poisoned-buffer run (covered bytes rewritten, uncovered bytes stay 0xAA), idempotence, and a
  `--dump` JSON (`bytes` zero-initialised, `bytes_poisoned`).
- Checkers: each re-decodes from the ELF, asserts body sha256/length/instruction count/store list/
  region image, the `ctor-classes.json` row (`other`, 0 direct/indirect calls), README row, and
  compares the native `--dump` including poisoned coverage.
- Mutation check: corrupting one constant in `leaf_cab080.cpp` made both the unit test and
  `check_fcab080.py` fail (exit 1); the file was restored and both pass.

## Commands and results (CWD `/Volumes/Extra/SoC Mac Native`)

- `cmake -S native -B local/build/native-agent-a -G Ninja` → exit 0
- `cmake --build local/build/native-agent-a -j 8` → exit 0 (log `local/build/native-agent-a-batch34-build.log`)
- `ctest --test-dir local/build/native-agent-a --output-on-failure` → exit 0, **280/280 pass**
  (log `local/build/native-agent-a-batch34-ctest.log`); this tree already carried 268 tests
  incl. other agents' uncommitted `name_hash`/`save_stream`/`save_stream_file`; this batch adds 12.
- `SOTC_NATIVE_BUILD=local/build/native-agent-a python3 -I tools/lift/check_f<rva>.py` for all 12 → exit 0
- `python3 -I tools/lift/check_ctor_slots.py` → exit 0 (108 registered RVA/index pairs; slot 11 first unresolved)
- All `tools/lift/check_*.py` (279 scripts): 276 pass; 3 fail, **none related to this batch**:
  `check_fb747f0.py`, `check_fb76950.py` (look for `local/build/native/test_vec_b747f0|b76950`; those test
  targets are not in `native/CMakeLists.txt`), and `check_math_ctor_clones.py` (asserts clone `0xf66640`
  has BSS cells *distinct* from `0xf6cad0`, which contradicts the HANDOFF finding that these guarded
  constructors share one cell range; it also hard-codes `local/build/native`). None of those files or
  `guarded_const_ctor.py` was touched.

## Not lifted / skipped (and why)

- Classifier "call-free" counts only `call` instructions. Bodies ending in `jmp` are tail calls and
  were **not** lifted: `0xb2b6b0`, `0xacc880`, `0xa46b70`, `0x5e7c10`, `0x51bc20`, `0x3600f0` all tail-jump
  to `0x323d0` / `0xf8f390` (atexit-style registration) or `0xf8f4f0` (memset-style); unresolved callee.
  `0x56fae0` and `0x8bd40` (also `returns == 0`) were not examined.
- True leaf bodies still unlifted (cheap candidates for batch 35): `0xc675f0` (115 B, ten 16-byte
  stores through `lea rax`), `0xc16b30` (186 B, ymm zero fills plus qword immediates),
  `0x9b75a0` (a 256-iteration loop zeroing two overlapping 16-byte stores per 0x28-byte element, then
  a dword at +0x2800), `0x575d40` (20-byte BSS-to-BSS copy `0x2eea480 -> 0x2f21520`; source is runtime
  state, not a constant), `0x8f3d90` (257 B), `0x163a40` (129 B), `0x99350` (288 B). The last three
  were not disassembled.
- Failed tries: 0. Open decode ambiguities: none.

## Observation versus inference

- Observation: exact bytes, destinations, widths and constants of the 12 bodies; all verified against
  the ELF by independent re-decode and against the native lift by `--dump`.
- Inference: the 4 identical-shape bodies (`0xcab080`, `0xca18d0`, `0xc94ae0`, `0xb692a0`) are clones of
  one source constructor over different BSS cells, with different rodata source constants (not the
  same values); no consumer of any of these cells was analysed. Cell meanings remain unknown.
- Not verified: run-time mutation of the rodata sources; consumers of the BSS cells; `startup_ctors`
  ordering.

## Remaining risks

- The three stale checkers above fail independently of this batch and should be triaged by the owner.
- `native/CMakeLists.txt` at the end of the file currently has duplicated `test_save_stream_file` /
  `add_test(NAME save_stream ...)` entries from another agent's in-progress edit (not mine); CMake still
  configured and built.

## Next exact action

Integrator: commit the 12 lifts (allowlisted paths only), update TASKS/HANDOFF/EVIDENCE, run
`python3 scripts/project_index.py` and the path audit. Next leaf batch: the seven remaining true leaf
bodies above.
