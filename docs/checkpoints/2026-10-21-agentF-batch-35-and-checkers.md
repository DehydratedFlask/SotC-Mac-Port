# Checkpoint: CPU-01-LEAF-CTOR-BATCH-35 (seven leaf ctors) + stale-checker repair (all `check_*.py` exit 0)

- Time / timezone: 2026-10-21 (CDT, America/Chicago), macOS 27 arm64
- Agent: worker `agent-f` (Claude Sonnet 5.5); integrator is the orchestrator session.
  `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `INSTALL_LOG.md` and indexes were **not** edited
  (`scripts/project_index.py` was not run for the same reason); nothing was committed or pushed.
- Task / status: CPU-01-LEAF-CTOR-BATCH-35 **COMPLETE, 7 lifts, 0 failed tries**, FIXTURE_VERIFIED
  (one unit-test oracle bug in my generated `test_leaf_c16b30` was fixed before first green, see below;
  it was a test defect, not a failed hypothesis about the binary). None registered in `startup_ctors`;
  G1 not passed. STALE-CHECKERS-01 **COMPLETE**: every `tools/lift/check_*.py` exits 0.
- Input: `local/re/eboot.elf` sha256 `d0723afb...5861d3` (asserted by every checker).

## Part 1 - lifted bodies

| Index | RVA | Bytes / insns | Shape | Region | Checker |
|---|---|---|---|---|---|
| 193 | `0xc675f0` | 115 / 19 | 9 x 16-byte stores (rodata copies, zero, vmovss word) | `0x3351760..0x33517f0` (144, fully covered) | `check_fc675f0.py` |
| 225 | `0xc16b30` | 186 / 26 | ymm zero fills + qword imms, later stores overlay earlier ones (19 stores) | `0x33513f0..0x3351530` (320, fully covered) | `check_fc16b30.py` |
| 320 | `0x8f3d90` | 257 / 33 | 16 x 16-byte rodata copies | `0x3294f90..0x3295090` (256, fully covered) | `check_f8f3d90.py` |
| 526 | `0x163a40` | 129 / 16 | 10 stores, 24 uncovered bytes | `0x2ee3c60..0x2ee3cd8` (120) | `check_f163a40.py` |
| 319 | `0x9b75a0` | 50 / 10 | 256-iteration loop, two overlapping 16-byte zero stores per 0x28-byte record + zero dword at +0x2800 | `0x32ef798..0x32f1f94` (10236, 7172 covered, 3064 never written) | `check_f9b75a0.py` |
| 417 | `0x575d40` | 29 / 7 | 20-byte copy BSS `0x2eea480` -> BSS `0x2f21520` | `0x2f21520..0x2f21534` (20) | `check_f575d40.py` |
| 548 | `0x99350` | 288 / 38 | 25 stores: constants + 24-byte run-time object from BSS `0x2dd9fa8` + address constant `0x2dd9f84` | `0x2ed3680..0x2ed37a4` (292, 279 covered) | `check_f99350.py` |

Native files: `native/core/leaf_<rva>.{hpp,cpp}`, `native/tests/test_leaf_<rva>.cpp`; CMake sources/tests
and README rows added (7 + 2).

### Decisions on the three non-constant bodies (inference marked)

- **`0x9b75a0` (loop)**: pure zero stores, so a bounded native loop over a `std::array`. The trip count
  (256) is derived by the simulator from the decoded `mov eax,8 / add rax,0x28 / cmp rax,0x2808 / jne`, not
  pinned by hand. Offset 0 of the native struct is table offset 8; the first 8 table bytes, the 12-byte gaps
  between record windows and 4 bytes at table +0x27fc are never written (poison-tested).
- **`0x575d40`**: the source is BSS (run-time state), so it is an **explicit parameter**:
  `init_leaf_575d40(const Cell20& source, Cell20& dest)`. Both loads happen before either store, matching the
  original, so aliasing is well defined (tested). Nothing from the source cell is embedded.
- **`0x99350`**: two run-time inputs, both explicit parameters of `init_leaf_99350(cells, Leaf99350Inputs)`:
  `object[24]` (bytes at BSS `0x2dd9fa8..0x2dd9fc0`, copied as 16 + 8 bytes) and `anchor` (the 64-bit value
  stored at `cells+0xf8`; the original stores the *address* `0x2dd9f84`, which has no meaning natively, so the
  caller supplies it). The body re-reads the dword it stored at `0x2ed37a0` (`0x00f2e009`) and stores it again at
  `0x2ed374c`; the simulator forwards that load from the earlier store, and the native code writes the constant.
  Inference only: what `0x2dd9f84`/`0x2dd9fa8` are is not analysed (`0x2dd9fa8` is 36 bytes after `0x2dd9f84`).
- Whether these inputs are non-zero at constructor time (what writes `0x2eea480`/`0x2dd9fa8` earlier) is **not
  analysed**; the lifts are correct for any input, which is why they are parameters.

## Method and verification

- `tools/lift/leaf_ctor_decode.py` (batch 34) is unchanged and still rejects non-file-backed loads. Four bodies
  (`0xc675f0`, `0xc16b30`, `0x8f3d90`, `0x163a40`) decode with it and reuse `leaf_check_common.py`.
- New `tools/lift/leaf_ctor_decode_sym.py` (strict simulator, exact encodings, unknown bytes raise) adds: SIB
  `[rcx+rax]`, `mov r32/r64,[mem]`, `mov [mem],r32/r64`, `mov eax,imm32`, `add rax,imm8`, `cmp rax,imm32`,
  `jne rel8`, step limit 100000. Bytes are tracked as ints (known; only read-only first PT_LOAD loads are trusted),
  `S:<addr>` (run-time bytes; BSS/.data are never assumed to hold their file image) or `A:<value>:<k>` (address
  constant). New `leaf_check_common_sym.py` pins body sha256/length/static and simulated instruction counts, the
  ordered store list (compact strings; 513-store loop pinned by count + sha256), the sources, region base/length/
  coverage, the `ctor-classes.json` row (`other`, 0 calls, same span/instruction count), and compares the native
  `--dump` (given-input bytes declared in the dump, zero-initialised and 0xAA-poisoned) with the decode.
  The dump must cover exactly the decoded run-time source bytes and address constants, and the test inputs must
  not be degenerate (>=16 distinct source bytes; address bytes non-zero and not 0xAA).
- Instruction boundaries of all 7 bodies were cross-checked against llvm-objdump (identical (pc, length) lists);
  for the four straight-line bodies the sym simulator's stores equal the batch-34 decoder's.
- Unit tests: each `test_leaf_<rva>` has an independent oracle (ordered 32-bit-lane replay for `0xc16b30`,
  formula-based coverage for the loop, byte-wise oracle + aliasing/zero-source for the copy, constant image +
  three run-time spans over six inputs for `0x99350`), a 0xAA-poisoned untouched-bytes check and idempotence.
- Mutation checks (all four reverted afterwards; unit test and checker both exit 1 under mutation, 0 restored):
  `leaf_99350.cpp` object tail read `+0x10 -> +0x08`; `leaf_575d40.cpp` tail read `+0x10 -> +0x0c`;
  `leaf_9b75a0.cpp` 256 -> 255 records; `leaf_163a40.cpp` one constant `0xff7fffff -> 0xff7ffffe`.
- Failed attempt, recorded for honesty: the first `test_leaf_c16b30` (generated like batch 34) checked each
  store's lanes against the final image, which fails where a later store overwrites an earlier one (`0xc16b30`
  has overlapping stores at `+0x80`, `+0x100`). Replaced by an ordered replay oracle; no change to the lift.

## Skipped (not genuine call-free leaves)

- `0x56fae0` (402 bytes, index 419): straight-line stores for ~40 instructions, then **`jmp 0x323d0`** (tail call,
  atexit-style registration) - not lifted.
- `0x8bd40` (72 bytes, index 553): pointer setup, then **`jmp 0xf8f390`** (tail call, same family as batch 34's
  skipped `0xb2b6b0` etc.) - not lifted. Both report `returns == 0` in the classifier for this reason.
- The remaining "other" call-free bodies that tail-jump (batch 34 list) stay skipped.

## Part 2 - stale checkers

1. `check_fb747f0.py` / `check_fb76950.py`: **the native lifts exist** (`native/core/vec_b747f0.{hpp,cpp}`,
   `vec_b76950.{hpp,cpp}`, `native/tests/test_vec_b747f0.cpp`, `test_vec_b76950.cpp`, last touched by commit
   `79ba84b`) but were never added to `native/CMakeLists.txt`. Added sources (after `vec_b743c0.cpp`) and the two
   `add_executable/add_test` blocks, plus README rows. Both checkers hard-coded `local/build/native`; they now read
   `SOTC_NATIVE_BUILD` (default unchanged). The assertions are untouched. Result: both exit 0; `vec_b747f0` and
   `vec_b76950` ctest entries pass.
2. `check_math_ctor_clones.py`: truth from the ELF (re-derived with `guarded_const_ctor.lift`): `0xf6cad0` and the
   clones `0xf66640`, `0xf53de0`, `0xf3d9d0` use the **same** 8 guard/value cell pairs
   (`0x3351ef8/ef4` .. `0x3351f48/f40`, lane0 `0x3351f60/f50`, xyz `0x3351f80/f70`). What is distinct per body is the
   rodata source of the guarded xyz copy (`0x132e660`, `0x132d240`, `0x132cf30`, `0x132c190`) and the destination of
   the final unguarded xyz copy (`0x3356a90`, `0x3356a40`, `0x33569f0`, `0x3356820`). The old assertion
   `recs[0]['value'] != base_recs[0]['value']` contradicted this (this is the verified "share one range" finding).
   The checker now pins the shared cell layout and per-body source/destination, asserts clones share the guarded
   cells with `0xf6cad0` and have pairwise-distinct sources and destinations, and reads `SOTC_NATIVE_BUILD`.
   Fails-on-wrong proof: with `s/(0x3351ef8, 0x3351ef4)/(0x3351ef9, 0x3351ef4)/`, `0xf66640` destination set to
   `0x3356a90`, and `0xf53de0` destination set to `0x3351ef4`, the checker exited 1 (AssertionError at the matching
   body) each time; the old (HEAD) checker also exits 1 against the ELF. Temporary mutant files were removed.

## Commands and results (CWD `/Volumes/Extra/SoC Mac Native`, build dir `local/build/native-agent-f` only)

- `cmake -S native -B local/build/native-agent-f -G Ninja` -> exit 0 (log `local/build/native-agent-f-configure.log`)
- `cmake --build local/build/native-agent-f -j 8` -> exit 0 (logs `local/build/native-agent-f-build.log`,
  `-build2.log`, `-batch35-build.log`; no warnings in the compiler output grep)
- `ctest --test-dir local/build/native-agent-f --output-on-failure` -> exit 0, **290/290 pass**
  (`local/build/native-agent-f-batch35-ctest.log`): 281 tree baseline + 7 leaf lifts + 2 re-registered vec tests.
- `SOTC_NATIVE_BUILD=local/build/native-agent-f python3 -I tools/lift/check_f<rva>.py` for the 7 new checkers,
  `check_fb747f0.py`, `check_fb76950.py`, `check_math_ctor_clones.py` -> exit 0 each.
- Loop over **all 286** `tools/lift/check_*.py` with `python3 -I` and `SOTC_NATIVE_BUILD=local/build/native-agent-f`:
  **286 pass, 0 fail** (279 before + 7 new; 244 of them resolve `local/build/native` themselves, see Remaining risks for the mirror run against this build). Includes `check_ctor_slots.py` (108 registered RVA/index pairs; slot 11 still
  the first unresolved).
- `python3 -I scripts/audit_publish_paths.py --all` -> exit 0 (nothing staged or committed).

## Observation versus inference

- Observation: bytes, destinations, widths, constants, loop bounds and run-time source addresses of the seven
  bodies; ELF-level shared-cell fact for the math clones; vec lifts present on disk but absent from CMake.
- Inference: cell meanings; that `0x2dd9f84` is a pointer to some BSS object; that the loop's table is 256 records of
  0x28 bytes (record interpretation not analysed).
- Not verified: what populates the run-time sources before these constructors would run; consumers of any region;
  `startup_ctors` order. Nothing registered.

## Remaining risks

- `native/CMakeLists.txt`/`README.md` are shared files; my edits are additive blocks (sources after `leaf_c0d380.cpp`
  and `vec_b743c0.cpp`; tests after their `add_test` lines; README rows after `leaf_c0d380` and `vec_cd0420`).
- 244 older checkers hard-code `local/build/native/test_*` (only ~45 honour `SOTC_NATIVE_BUILD`), so the plain
  286/286 run also ties those 244 to whatever is in `local/build/native`, not necessarily this tree's build. To prove
  them against `native-agent-f`, all 286 checkers were additionally run from a mirror root
  (`<scratchpad>/mirror`: copy of `tools/lift`, symlinks to `scripts`, `native`, `docs`, `local/re`, and
  `local/build/native -> native-agent-f`, `SOTC_NATIVE_BUILD` unset): **286 pass, 0 fail**. A follow-up could make
  those 244 files read `SOTC_NATIVE_BUILD` (mechanical `exe = ROOT / 'local/build/native/test_...'` rewrite); not done
  here to avoid a 244-file churn.
- `scripts/project_index.py` and trail `onlyDirs` (needs `native`, `launcher` scopes) were not run/changed.

## Recommended TASKS.md rows

- `CPU-01-LEAF-CTOR-BATCH-35` - COMPLETE / FIXTURE_VERIFIED; 7 lifts (`0xc675f0`, `0xc16b30`, `0x9b75a0`, `0x575d40`,
  `0x8f3d90`, `0x163a40`, `0x99350`); `0x56fae0` and `0x8bd40` skipped as tail-jump bodies; attempts 0.
- `CHECKERS-STALE-01` - COMPLETE; `fb747f0`/`fb76950` CMake registration + env build dir, math-clone checker corrected
  to the verified shared-cell fact; 286/286 `tools/lift/check_*.py` pass.
- `CPU-01-LEAF-CTOR-BATCH-36` (optional) - classify remaining call-free "other" bodies that end in `jmp`
  (tail-call targets `0x323d0`, `0xf8f390`, `0xf8f4f0` unresolved) and decide registration policy for lifted leaves.

## Next exact action

Integrator: record this checkpoint in HANDOFF/TASKS/EVIDENCE, run `python3 scripts/project_index.py` and
`trail build --check` (add `native`/`launcher` scopes explicitly), then commit the allowlisted paths
(`native/core`, `native/tests`, `native/CMakeLists.txt`, `native/README.md`, `tools/lift`, this checkpoint) and push.
