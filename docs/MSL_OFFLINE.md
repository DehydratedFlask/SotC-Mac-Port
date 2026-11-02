# Offline MSL archive step (GPU-01 prep, offline only)

Status: tool behaviour **FIXTURE_VERIFIED** on two synthetic MSL shaders on this machine; the real
corpus front end is measured below (**STATIC_ANALYSIS_VERIFIED**). The last section measures the
lowering: **71 of the 77** per-lane-eligible programs in the sample-512 population lower to MSL and
all 71 compile offline, with placeholder or empty bindings — no pipeline state object, no render,
no runtime path, **G3 NOT RUN**.

## Where this sits

`tools/dev/gcn_to_msl.py` lowers a bounded, explicitly ABI-matched GCN program to ordinary
MSL text plus a JSON report; it never compiles anything. `tools/dev/msl_pipeline_archive.py`
is the next step, still offline: it freezes each generated MSL file into a content-addressed
AIR object, links one `.metallib` per stage, and records digests, sizes, compiler version and
exact command lines in a stable archive manifest. The archive is tooling output, not a
runtime input: nothing is linked into the game, and no Vulkan, guest GPU, disassembler or
Sony module is involved. The population it would eventually describe is the 34,392 unique
`.csdr` / 10,365 `.cmat` inventory in `docs/GPU01_PREP.md`; none of it is consumed yet.

## Input manifest

```json
{"schema": "msl-pipeline-input-v1",
 "entries": [{"id": "logo_vs", "stage": "vertex",
              "msl_source_path": "shaders/logo_vs.metal"}]}
```

Ids are unique `[A-Za-z0-9_.-]+`; `stage` is `vertex` or `fragment`; the source path must be
relative and resolve inside the manifest directory; unknown fields are rejected (a bare list
of entries is also accepted).

## Archive manifest (`<out>/msl_archive.json`)

Sorted keys, entries sorted by `id`, LF endings, no absolute paths (guarded at write time):

| Field | Meaning |
|---|---|
| `schema` | `msl-pipeline-archive-v1` |
| `toolchain` | first line of `xcrun -sdk macosx metal --version`; `sdk` is `macosx` |
| `counts` | `{entries, unique_air, stages[]}` |
| `entries[]` | `id`, `stage`, `msl_source` (declared relative path), `msl_sha256`, `msl_bytes`, `air{path,sha256,bytes}`, `command{argv[],line}` |
| `stages{vertex,fragment}` | `path`, `sha256`, `bytes`, `air_sha256[]` (sorted), `command{argv[],line}` |

Artifacts: `out/<msl_sha256>.air`, `out/<stage>.metallib`; commands run from the archive root,
so tokens stay relative (`../shaders/logo_vs.metal`).

## Commands

```sh
python3 -I tools/dev/msl_pipeline_archive.py --manifest in.json --out private/archive
python3 -I tools/dev/msl_pipeline_archive.py --manifest in.json --out private/archive --check
python3 -I tools/dev/test_msl_pipeline_archive.py -v
```

Exit codes: `0` ok, `1` failure (including a digest mismatch), `75` Metal toolchain
unavailable with the reason on stderr. `--check` first re-verifies the archived artifacts,
refuses a changed toolchain version, then recompiles into `out/.check/` and compares MSL,
AIR and metallib digests and byte sizes; it never rewrites the archive.

## What was actually run here

2026-11-01, macOS 27.0.1 (build 26A434) on arm64, Xcode 27.2, Metal Toolchain component
27B47. The component was absent at session start (`xcrun ... metal --version` reported
"cannot execute tool 'metal' due to missing Metal Toolchain"); it was installed with
`xcodebuild -downloadComponent MetalToolchain` (838.9 MB, asset `022-22476-056.dmg`).
Toolchain string: `Apple metal version 32023.922 (metalfe-32023.922.4)`.

Two synthetic hand-written shaders (336 B vertex, 276 B fragment) were compiled in a private
temp directory; two AIR files and two stage metallibs were produced:

```
fragment	demo_fragment	msl=752a717a63b67dc960210845655a9ca471e9beb406e7e468242534bc9a8cb828 air=77707a627dbe574c7a54e67c956251f4dd1a2fa0c9a5648998854b9a5ea3b4bd air_bytes=3248
vertex	demo_vertex	msl=939fd810ff78e92248d16b8d64b13da89c0c949c1242ef06381243ee4f4d256d air=145bfcbd26cca46fc5059fc7ec6a90739e238f9c94bbf4637bf834b5a7e0ed08 air_bytes=3296
metallib	fragment	out/fragment.metallib	sha256=6ee668cc18d90e906eac9c6cf9038d1acdcf30ae55d4f2e3f6f60de946d36913 bytes=3852
metallib	vertex	out/vertex.metallib	sha256=72a7cadc930c55f33974806cad4590cfe748ea7da553ffcf212683bc15386c04 bytes=3900
```

`--check` passed; a second archive of the same inputs was byte-identical; 9 unittest tests
passed (one source char mutated fails `--check`; no `xcrun`, and a fake `xcrun` without the
component, both exit 75).

## Real corpus coverage

`tools/dev/msl_corpus_coverage.py` measures how far the *front end* reaches: it samples `.csdr`
members deterministically from the pinned private paks, reads each through the validated PSARC
`MD5(path)` pairing, isolates every `OrbShdr` code span and asks `gcn_inventory.decode()`. It
never lowers, never executes and never invents an ABI.

```sh
python3 -I tools/dev/msl_corpus_coverage.py --self-test   # FIXTURE_VERIFIED, synthetic spans only
python3 -I tools/dev/msl_corpus_coverage.py               # default sample 64; the 512 command is below
```

Population 34,291 `.csdr` (pak0 3,309 + pak1 2,512 + pakN 28,470; all unique paths). The default
sample is **64**, deterministic even spacing `round(i*(N-1)/(k-1))` over the `(path, archive)`-
sorted list (first and last included, implied stride 535.8); the 512 run uses the same rule.

The compressed-prefix class is a known, older ceiling: E626 recorded **19,320** compressed-prefix
members rejected by the raw container reader across the whole corpus (see `docs/EVIDENCE.md`), and
no decompressor exists in this repository. It is therefore larger than any single decoder row, and
widening the opcode table cannot reach those members at all.

| Sample | compressed-prefix | raw, decode-error | raw, all programs decode | lowered |
|---:|---:|---:|---:|---:|
| 64 | 41 | 23 | 0 | **0** |
| 512 | 264 | 244 | 4 | **0** |

Programs (a member can hold a VS/PS pair): at 64, 31 attempted, 4 decoded, 27 decode-error; at
512, 367 attempted, 74 decoded, 293 decode-error (74 programs, 577 instructions, 73/74
per-lane eligible; 70 raw members yield at least one decoded program).

Top decode-error reasons before widening, n=512 / n=64: `unsupported encoding` 235 / 23,
`unsupported SMRD register/literal offset` 39 / 4, `unsupported VINTRP opcode 2` 10 / 0,
`unsupported VOP3 opcode 349` 3 / 0 - the widening log below carries the post-change numbers.

Supported-opcode share is **1.0** in both samples by construction (`decode()` rejects anything
else), so decode success - not this share - is the informative number.

**Zero real shaders were lowered.** `gcn_to_msl.py` admits a program only through a per-shader
ABI manifest (exact digest, named scalar views, stream bindings) and none exists for any retail
member, so every decodable program is `blocked-no-abi-manifest`; inventing bindings is out of
scope. Stage is the INF nibble label from filename co-occurrence, not verified semantics, and
compressed members are classified, never decompressed. Evidence: the census is
**STATIC_ANALYSIS_VERIFIED**, the tool self-test **FIXTURE_VERIFIED**; no Metal compile, no
pipeline object, **G3 NOT RUN**. Private output `local/validation/msl-coverage/coverage.json`
(+ `sample-512/`); full run log `local/validation/msl-coverage/REPORT.md`.

## Decoder widening (SOP2 / SOPC / VOPC / SMRD offsets)

`gcn_inventory.py` v2 adds the rows v1 rejected: SOP2 0-42 and 44 (12/13 reserved; 43
`s_cbranch_g_fork` is control transfer, not modelled), SOPC 0-15 (16 `s_setvskip` is VI-only),
VOPC 0-31 (f32 compare and `cmpx`), SMRD 8/9/12 `s_buffer_load_dword{,x2,x16}`, and the
`IMM=0`/`OFFSET=255` register-literal offset (one literal dword; the 39 formerly rejected spans
re-decode as 8-byte loads and fail later, at `0x44`/`0x40`/`0x3c`/`0x88`, not `0x24`-`0x2c`).
Skipped: VINTRP op 2 `v_interp_mov_f32` (48 first-blockers) because `gcn_to_msl.py` models every
VINTRP as P1/P2, so decoding it without a lowering change would admit the wrong shape; the
remaining VOP3/VOP2/VOP1/SOP1/MUBUF/DS/MTBUF opcodes below are out of scope for this card.

```sh
python3 -I tools/dev/msl_corpus_coverage.py --sample 512 --overwrite --out local/validation/msl-coverage/after-opcodes
```

| Sample 512, same 512 members / 367 programs | before (v1) | after (v2) |
|---|---:|---:|
| programs decoded / attempted | 74 / 367 | **74 / 367** |
| first-failure `unsupported encoding` | 235 | **0** |
| first-failure median byte offset | `0x20` | `0x28` |

No additional program decoded: the newly accepted words sit in programs that hit an unmodelled
opcode a few dwords later. New top blockers were VOP3 349 179, VOP2 37 53, VINTRP 2 48 and SOPP 5
5 (a branch). **STATIC_ANALYSIS_VERIFIED**; decode says nothing about Metal behaviour; no lowering.

## Decoder widening 2 (VOP2 / VOP1 / SOP1 / VOP3 integer rows)

v3 adds VOP2 3/22/37 (`v_add_f32`, `v_lshrrev_b32`, `v_add_i32` + its VCC carry write), VOP1 36
`v_floor_f32`, SOP1 36 `s_and_saveexec_b64` (64-bit pair, EXEC read+write), VOP3 322/349
(`v_mad_i32_i24`, `v_sad_u32`: three real sources; ABS/CLAMP/OMOD/NEG are f32-only, and an integer
row setting any of them is rejected) and MUBUF 0 `buffer_load_format_x`. Skipped: SOPP 5
`s_cbranch_scc1` is control transfer (a linear decoder would invent fall-through) and VINTRP 2
still, because `gcn_to_msl.py` models every VINTRP as P1/P2.

```sh
python3 -I tools/dev/msl_corpus_coverage.py --sample 512 --overwrite --out local/validation/msl-coverage/after-opcodes-2
```

| Sample 512 | v2 | v3 |
|---|---:|---:|
| programs decoded / attempted | 74 / 367 | **75 / 367** |
| instructions / per-lane eligible | 577 / 73 of 74 | **654 / 73 of 75** |

Only `standard_2874784413` (392 B, 77 instructions) is new. Next blockers, no targeted row left:
MUBUF 14 `buffer_load_dwordx4` 111, VOPC 209 65, VOPC 212 51 (integer `v_cmpx_*`), VINTRP 2 48,
SOPP 5 5. Nothing is lowered: `gcn_to_msl.py` rejects all eight new mnemonics by name. 35 decoder
tests **FIXTURE_VERIFIED**, census **STATIC_ANALYSIS_VERIFIED**, G3 NOT RUN.

## Limits

- Synthetic MSL only: two hand-written files, not produced by `gcn_to_msl.py`; 0 of the
  34,392 unique `.csdr` were compiled and no `.cmat` was read.
- No pipeline state objects, render pipeline descriptors, reflection or argument tables; the
  two demo metallibs were archived but never loaded or executed, so there is no G3/Metal
  frame claim and no runtime path. No `native/` change and no build target.
- AIR embeds the resolved source path (`strings`): an identical build in another directory gave
  different AIR digests (`b57a1931…`, `7865c861…`) with identical metallib digests. Keep archives
  in private `local/`.
- Metallib linking was byte-identical across runs on this machine/toolchain only; the tool records
  what the compiler produced, not MSL semantics.

## Decoder widening 3 (MUBUF dword forms, VOPC integer cmpx)

- **Added:** MUBUF 12/13/14 (`buffer_load_dword`, `dwordx2`, `dwordx4`) with explicit component
  counts, plus VOPC 208–213 (`v_cmpx_{f,lt,eq,le,gt,ne}_u32`), values cross-checked against the
  pinned shadPS4 frontend table (`209 = V_CMPX_LT_U32`, `212 = V_CMPX_GT_U32`).
- **Corrected later (see "Decoder widening 7"):** this section originally also claimed 15/16/17 were
  the dword *stores*, taken from the Sea Islands list. The pinned PS4 table puts the dword stores at
  28–31 and 15 at `buffer_load_dwordx3`, so those three rows were wrong and have been replaced.
- **Test coverage:** `test_gcn_inventory.py` gained a positive case for each row and negatives for
  MUBUF 11/18 and VOPC 207/214, which still reject.
- **Measured at sample 512** (`--out local/validation/msl-coverage/after-opcodes-3`):

| Metric | before (v3) | after (v4) |
|---|---:|---:|
| Programs decoded / attempted | 75 / 367 | **162 / 367** |
| Instructions in decoded programs | 654 | **5,168** |
| Decode-error programs | 292 | **205** |
| Members with ≥1 decoded program | 71 | 95 |

- **New top blockers:** `SOPP 8` 117, `VINTRP 2` 48, `VOP3 350` 16, `SOPP 5` 5, `VOP2 5` 5.
- **Limit:** decoding still says nothing about Metal semantics; 0 real shaders are lowered, because
  no per-shader ABI manifest exists.

## Decoder widening 4 (VOP2/VOP3 rows, and why the biggest blocker is not a table row)

- **Added:** VOP2 5 `v_subrev_f32` and VOP3 350 `v_cvt_pk_u8_f32` (a three-source row, so it joins
  the scalar read-port rule with 321/322/349). Both opcode values are read straight from the pinned
  PS4 frontend table.
- **Measured at sample 512** (`--out local/validation/msl-coverage/after-opcodes-5`):

| Metric | v4 | v5 |
|---|---:|---:|
| Programs decoded / attempted | 162 / 367 | **183 / 367** |
| Instructions in decoded programs | 5,168 | **6,574** |
| Decode-error programs | 205 | **184** |
| Members with ≥1 decoded program | 95 | **116** |
| Per-lane eligible decoded programs | 74 / 162 | 74 / 183 |

- **The remaining ceiling is control flow, not the opcode table.** The top two blockers are
  `SOPP 8` (117) and `SOPP 5` (5) — together **122 of the 184 remaining decode errors**. The pinned
  PS4 SOPP table names them `s_cbranch_execz` and `s_cbranch_scc1`: conditional branches whose target
  is an immediate offset. A linear span decoder cannot represent that edge, and inventing fall-through
  would fabricate a program, so both stay rejected. The decoder now reports them as
  `unsupported control transfer SOPP opcode N (<name>)` so the census can separate "needs a CFG" from
  "needs another table row".
- **A wrong row was caught before commit.** The first attempt in this card mapped `SOPP 8` to
  `s_barrier` from the Sea Islands ISA list. Two independent checks contradicted it: the pinned PS4
  table puts `S_BARRIER` at **10**, and the 1,171 observed opcode-8 words carry small, varied
  immediates (2, 9, 10, 13, 14, 16, 21, 27, 36, 112 …) — branch offsets, not a barrier's required
  zero. The row was reverted; `S_BARRIER` is modelled at 10 with a `wave_barrier` per-lane blocker
  and a test, and opcode 8 is documented as a branch.
- **Still zero lowered:** decoding is not Metal semantics, and no per-shader ABI manifest exists.

## Basic-block decoding: what the branch rows were hiding

`gcn_inventory.decode()` refuses `s_branch`/`s_cbranch_*` by design, so 122 of the 184 remaining
decode errors were branch rows. `tools/dev/gcn_blocks.py` lifts that refusal *only* for structure: it
decodes with `allow_branches=True` (branch rows become explicit one-dword instructions), splits the
listing at every control-transfer row, and reports each branch's byte target
(`offset + 4 + simm16*4`). It never follows an edge, never claims reachability, and never executes.

```sh
python3 -I tools/dev/gcn_blocks.py --self-test   # 26 checks, synthetic spans only
```

`msl_corpus_coverage.py` now records a `block_decode` row per program (counters plus the decoder's own
error string), so the census can separate "needs a CFG" from "needs another table row".

**Measured at sample 512** (`local/validation/msl-coverage/blocks-512`, `blocks-512b`):

| Block-mode first blocker | before VOP2 15/27 | after |
|---|---:|---:|
| `VOP2 27` (`v_and_b32`) | 96 | 0 |
| `VINTRP 2` | 48 | **48** |
| `VOP2 15` (`v_min_f32`) | 24 | 0 |
| `VOP3 4` (`v_cmp_gt_f32`) | 0 | **43** |
| `VOPC 197` (`v_cmp_ne_u32`) | 0 | **33** |
| `VOP3 194` (`v_cmp_eq_u32`) | 0 | **19** |
| `VOP1 5` (`v_cvt_f32_i32`) | 0 | **13** |

Two findings, both recorded as measurements rather than claims:

1. **The branch rows were masking plain table rows.** Admitting branches moved the first blocker of
   120 programs from a branch to `v_and_b32`/`v_min_f32`, which are ordinary two-source VOP2 rows.
   Those two rows are now modelled (values from the pinned PS4 table), and the corpus confirms the
   effect: block-mode first blockers shift from `VOP2 27`/`VOP2 15` to the comparison rows.
2. **The next ceiling is the comparison/conversion family**, not control flow: `VINTRP 2` (48),
   `VOP3 4` (43), `VOPC 197` (33), `VOP3 194` (19), `VOP1 5` (13). The `VOP3 1..197` rows are the
   VOPC-class compares in the VOP3 encoding (mask destination in `VDST`), so admitting them needs a
   mask-destination rule, and `v_cmpx_*` still has to tag `writes_exec_mask`.

**No sampled program is fully block-decodable yet** (0 extra programs beyond the 183 linear decodes),
because every branch-containing program still hits one of those later rows. The linear numbers are
unchanged at 183/367 decoded, 184 decode-error. That is the honest reading: branch support alone does
not move the census, it makes the next rows visible.

## Decoder widening 5 (compare rows, cndmask, conversions)

- **Added:** VOPC 192–199 integer compares (`v_cmp_{f,lt,eq,le,gt,ne,ge,t}_u32`, VCC destination) and
  VOPC 136 `v_cmp_class_f32`; VOP3-encoded compares 1/4/11/194/197 with an explicit
  **mask-destination rule** (`VDST` names VCC or an even SGPR pair, modifiers must be clear); VOP3 256
  `v_cndmask_b32` with its third operand validated as a scalar/VCC/EXEC mask; VOP1 5/11/42 and VOP2 38.
- **Fixed rule:** `writes_vcc_mask` for VOPC is now decided by an explicit EXEC-writing set
  (`cmpx`) instead of `op < 16`, which would have mislabelled the integer compares at 192–199.
- **Measured at sample 512** (`--out local/validation/msl-coverage/blocks-512d`):

| Metric | v5 | v6 |
|---|---:|---:|
| Programs decoded / attempted (linear) | 183 / 367 | **186 / 367** |
| Instructions in decoded programs | 6,574 | **6,786** |
| Decode-error programs | 184 | **181** |
| Per-lane eligible decoded programs | 74 / 183 | 76 / 186 |
| Block-decodable programs | 183 | **186** |

- **Block-mode first blockers after this card:** `VINTRP 2` 48, `VOP1 6` 48, `VOP2 0` 26,
  `VOP1 32` 22, `SOP2 96` 22, `MIMG 39` 2, `VOP3 287` 2. Every one still needs its own row checked
  against the pinned table; none is a control-flow problem any more.
- One test case had to be replaced rather than "fixed": `vop2(op=63)` is not a valid negative probe,
  because a VOP2 word with `op = 63` has the same top bits as a VOP1 instruction by construction, so
  it was really testing a VOP1 opcode. The negatives now use `vop2(39)`/`vop2(41)`.
- **Still zero lowered:** decoding is not Metal semantics, and no per-shader ABI manifest exists.

## Decoder widening 6 (SOPK misattribution fixed, next compare batch)

- **Bug fixed, not just a row added:** SOPK shares bits 31-30 with SOP2, and the decoder's SOP2 entry
  (`mask 0xc0000000`) matched first, so **every SOPK instruction was reported as a bogus
  "SOP2 opcode 96..127"** — 35 first blockers in block mode. `SOPK` now has its own entry matched
  before SOP2 (mask `0xf0000000`), its own field set, its own opcode table from the pinned PS4 SOPK
  block, and `s_cbranch_i_fork` is classified as a control transfer. Modelled: `s_movk_i32`,
  `s_cmovk_i32` (`reads_scc`), `s_cmpk_*` 3–14 and `s_addk_i32` (`writes_scc`), `s_mulk_i32`.
- **Rows added:** VOP2 0 `v_cndmask_b32` (VOP2 form selects with **implicit VCC**, so the row tags
  `reads_vcc_mask` and is a per-lane blocker), VOP2 16 `v_max_f32`, VOP2 26 `v_lshlrev_b32`;
  VOP1 6 `v_cvt_f32_u32`, 8 `v_cvt_i32_f32`, 32 `v_fract_f32`, 51 `v_sqrt_f32`;
  VOP3 3/6/136 (compares, mask destination) and VOPC 214/215 (`cmpx`, EXEC destination).
- **Measured at sample 512** (`local/validation/msl-coverage/blocks-512f`, `blocks-512g`): linear
  decode stays **186/367** (all of these rows sit behind branch rows), but the block-mode first
  blockers move twice:

| Block-mode first blocker | after v6 | after SOPK fix | after this batch |
|---|---:|---:|---:|
| `VOP1 6` | 48 | 0 | 0 |
| `VOP2 0` | 26 | 0 | 0 |
| `VOP1 32` | 22 | 0 | 0 |
| `SOP2 96` (really SOPK) | 0 | 35 → 0 | 0 |
| `VOP2 16` | 0 | 48 | **0** |
| `VOP3 6` | 0 | 42 | **0** |
| `MUBUF 29` | 0 | 0 | **45** |
| `SOP1 8` | 0 | 0 | **41** |

- **Still zero lowered:** decoding is not Metal semantics, and no per-shader ABI manifest exists.

## Decoder widening 7 (MUBUF numbering corrected, SOP1/void rows, buffer tags)

- **Second wrong-row incident, caught the same way.** The v4 card had labelled MUBUF 15/16/17 as the
  dword stores from the Sea Islands list. The pinned PS4 MUBUF table says otherwise: **15 is
  `buffer_load_dwordx3`**, the dword stores are **28–31** (with 31 = `buffer_store_dwordx3`), and 16/17
  are not dword stores at all. The rows are now 12/13/14/15 (loads, 1/2/4/3 components) and
  28/29/30/31 (stores, 1/2/4/3). 16/17 reject again. The earlier section is annotated rather than
  rewritten, so the mistake stays visible.
- **MUBUF tags were wrong too:** every MUBUF row was tagged `formatted_vertex_fetch`, including plain
  dword loads, stores and atomics. All MUBUF rows now carry `buffer_memory`, and only the format rows
  0–7 carry `formatted_vertex_fetch`.
- **Added:** SOP1 8 `s_not_b64` (64-bit pair rules), VOP3 272 `v_max_f32`, 362 `v_mul_hi_u32`,
  363 `v_mul_lo_i32`, and MUBUF 50/51 `buffer_atomic_add`/`buffer_atomic_sub`. Atomics decode but are
  **never per-lane eligible**: they add a `buffer_atomic` blocker, because memory ordering cannot be
  dropped in a straight-line per-lane conversion.
- **Measured at sample 512** (`blocks-512h`, `blocks-512k`): linear decode stays **186/367** (all of
  this sits behind branch rows). Block-mode first blockers move from `MUBUF 29` (45), `SOP1 8` (41),
  `VOP3 287` (12), `MUBUF 31` (9) to `VOP2 18` (54), `VINTRP 2` (48), `VOP3 287` (24), `VOP3 426` (14),
  `VOP2 32` (12), `VOP1 53` (6).
- **Still zero lowered:** decoding is not Metal semantics, and no per-shader ABI manifest exists.

## Decoder widening 8 (signed compare blocks, min/max, and one deliberate refusal)

- **Added:** VOP2 17/18/19 (`v_min_i32`, `v_max_i32`, `v_min_u32` — no carry, unlike `v_add_i32`),
  VOP1 53/54 (`v_sin_f32`, `v_cos_f32`), VOP3 426 `v_rcp_f32`, VOP3 128–135 (`v_cmp_*_i32`, VCC
  destination), and VOPC 128–135 / 144–151 (the signed compares and their `cmpx` forms; the block
  boundaries are taken from the pinned table, which puts the `i16` rows at 137–142 and
  `V_CMPX_CLASS_F32` at 152, and those stay unmodelled).
- **Deliberately refused — `VOP3 28` (`v_cmpx_nle_f32`).** The VOP3-encoded `cmpx` rows are not
  modelled: when the first rule tried required `VDST ∈ {EXEC, EXEC_HI}`, **14 sampled programs were
  rejected** for putting a different value there. Real compiler output therefore does not follow the
  obvious contract, and guessing it would repeat the SOPP-8/MUBUF mistake. The row stays out of the
  table, the rejection is recorded here, and the 14 programs remain blocked until the destination
  contract is established from evidence.
- **Also refused:** `VOP2 32` `v_madmk_f32` carries a **mandatory** inline literal that the current
  literal rule (a 255 selector) cannot express, and `VOP3 287` `v_mac_f32` has an ambiguous addend
  between the VOP2 and VOP3 forms. Both stay unmodelled with a test proving they reject.
- **Measured at sample 512** (`blocks-512l` → `blocks-512n`): linear decode stays **186/367**, because
  every row in this card still sits behind a branch row. Block-mode first blockers move through four
  successive heads — `VOP2 18` (54) → `VOPC 148` (54) → `VOP3 129` (54) — and settle at
  `VINTRP 2` (48), `VOP3 287` (30), `VOP2 32` (15), `VOP3 28` (14), `MIMG 39` (5).
- **Still zero lowered.**

## VINTRP 2 admitted — and the linear ceiling becomes control flow

`v_interp_mov_f32` was the last "needs a lowering rule" row. It is now decoded **and** lowerable, but
only when the manifest says how:

- **Decoder:** VINTRP 2 is admitted when `vsrc ∈ {0, 1, 2}` — the three selectors the pinned PS4
  translator accepts — and carries the `per_vertex_parameter` requirement. Any other selector still
  rejects the whole program.
- **Lifter:** a new `interpolation: "supplied_vertex_values"` mode lowers it to
  `dst = u.param{attr}_{vertex}.{chan}`, where **`vertex_select` in the manifest** maps the VSRC code
  to `p10`/`p20`/`p0`. The mapping is manifest-supplied on purpose: the pinned translator asserts
  `src < 3` and maps the three selectors through its own table, while the ISA summary describes them
  as P10/P20/P0, and those two descriptions do not line up one-for-one. An optional
  `vertex_mov_subtract_p10` list reproduces the translator's "subtract P10" step for the selectors it
  names. Missing mode, missing map, unmapped selector, a non-vertex name, or a malformed list all
  raise.
- **Also added:** VOP3 282 `v_lshlrev_b32` and VOP1 17 `v_cvt_f32_ubyte0`, the two rows the newly
  admitted programs hit next.

**Measured at sample 512** (`blocks-512o` → `blocks-512p`): linear decode stays **186/367**, because
every program that moved past `VINTRP 2` then hit a branch. What changed is the *composition* of the
remaining errors:

| Blocker | before | after |
|---|---:|---:|
| `VOP3 282` / `VOP1 17` / `VOP3 2xx` rows | — | 0 |
| `VINTRP 2` | 48 | **0** |
| `SOPP 8` (`s_cbranch_execz`) | 117 | 117 |
| `SOPP 4` (`s_cbranch_scc0`) | 5 | **48** |
| `SOPP 5` (`s_cbranch_scc1`) | 5 | 5 |

So **170 of the 181 remaining linear decode errors are control transfers** (117 + 48 + 5); only 11
are opcode rows. The linear number can no longer be moved by adding rows — it needs a CFG-aware
decoder that represents branch edges.

- **Still zero lowered:** the new mode is a rule the manifest must opt into, and no real shader has a
  manifest.

## CFG-aware decode promoted into the inventory

`gcn_blocks` began as a census-only tool. The block splitting and branch-target arithmetic now live
in `gcn_inventory.decode_blocks()` — the same all-or-nothing contract as `decode()`, with one change
of representation: **a control-transfer row is an instruction that ends its block**, and its byte
target is computed (`offset + size + simm16*4`, GCN's PC-relative rule). The listing still never
assumes fall-through past a taken branch; the block after a terminator is simply the next run of
bytes. `gcn_blocks.py` is now the CLI/adapter over that mode, and the coverage census calls the
inventory directly.

`BasicBlock` carries `start`, `end`, `instruction_offsets`, `terminator` and the matching
`BranchTarget`; `BranchTarget` records `target_in_program` and `target_on_instruction`, so a branch
into the middle of an instruction is *reported*, never repaired.

**Measured at sample 512** (`blocks-512q`):

| Metric | before | after |
|---|---:|---:|
| Linear decode | 186 / 367 | 186 / 367 |
| **Block-decodable programs** | 186 | **240** |
| Programs decodable as blocks but not linearly | 0 | **54** |
| Branch rows in the sample | — | **333** |
| Branch targets inside the program | — | **333 / 333** |
| Branch targets on an instruction boundary | — | **333 / 333** |

So **54 of the 181 branch-blocked programs are fully decodable once blocks are represented**, and
every branch in the sample lands on a real instruction start — the control-flow structure is
well-formed, it simply is not linear.

Remaining block-mode first blockers: `VOP3 287` (30, `v_mac_f32`, deliberately unmodelled for its
ambiguous addend), `VOP1 7` (28), `VOP2 32` (15, `v_madmk_f32` and its mandatory literal), **14
programs whose declared span carries bytes after `s_endpgm`**, `VOP3 28` (14, the `cmpx` row refused
on evidence) and `VOP3 328` (6).

*(Superseded by "Decoder widening 9" below: `VOP1 7`, `VOP1 37`, `VOP3 328` and nine other rows are
modelled in `v17`, and the paired-blob split now covers 43 members instead of 14.)*

- **Still zero lowered.** Block-decodability is structure, not semantics.

### The "bytes after s_endpgm" programs are paired shaders

The 14 programs whose declared span carried bytes past `s_endpgm` are not malformed: each blob holds
**two programs back to back**. The census now searches for a split point where both halves decode,
and finds one in **14 of 14** cases (split offsets 212/228/236/252/348/360/376/384 — the first
stage's size, which varies).

So the honest decodable count is **240 single programs + 14 pairs = 254 of 367**, and the container
model "one `OrbShdr` blob = one program" is wrong for these members — it is a VS/PS pair in one blob.
The census records this as `programs_paired_halves_decode` and `paired_split_offsets` rather than
silently counting the members as undecodable.

- **Still zero lowered:** a shader pair is twice the structure, not twice the semantics.

### Paired blobs are now split into their stages

The census splits a blob at the recorded cut point when **both** halves decode, so each stage is
counted as its own program; a blob with no such cut stays whole and keeps its failure visible.

**Measured at sample 512** (`blocks-512s` vs `blocks-512q`):

| Metric | before | after |
|---|---:|---:|
| Program rows attempted | 367 | **381** |
| Decoded (linear) | 186 | **200** |
| Decode errors | 181 | 181 |
| Block-decodable | 240 | **268** |
| Members split into stages | — | **14** (→ 28 stage rows) |

Only **14 of the 28** stage rows decode linearly: the large first stage is still branch-blocked,
while the second stage is a tiny program that decodes. Every one of the 28 is block-decodable, so the
block-level view is now **268 of 381 rows (70 %)** against 240 of 367 before the split.

What this changes and what it does not: the census now describes the container correctly (a blob can
hold a VS/PS pair), and 14 more programs are decodable. It does not move `lowered` — still **0** —
because lowering needs a per-shader ABI manifest that does not exist for any real shader.

### Why `lowered` is still zero: what one real manifest would need

The lowerer refuses any program without a per-shader ABI manifest, and the manifest has to name
things the shader does not contain. Measured on the largest per-lane-eligible program in the sample
(`shadow_809795858_shipping.csdr`, 316 B, 65 instructions, encodings EXP 2 / MUBUF 5 / SMRD 7 /
SOP1 2 / SOPP 7 / VOP1 13 / VOP2 24 / VOP3 5):

| Manifest slot | Observed registers |
|---|---|
| scalar memory bases (`SMRD` sbase) | **s0, s2, s8, s9** |
| buffer descriptors (`MUBUF` srsrc) | **s20, s24, s28** |
| export targets | **12** (clip) and **32** (param0) |
| vector inputs | 13 `VOP1` + 24 `VOP2` + 5 `VOP3` results, several read from `v0..v3` before being written |

The shader names the *register slots*; it does not say which buffer or memory each slot points at.
That mapping lives in the draw state (the PM4/register writes and the resource tables), which we do
not have without either a state trace from the running game or owner-supplied binding data. Inventing
a plausible mapping would produce MSL that compiles and renders the wrong thing — the exact failure
mode this pipeline was built to avoid. So the blocker is precise: **0 lowered because 0 manifests,
and a manifest cannot be derived from shader bytes alone.**

## Decoder widening 9 (VOP conversions, bitfield and float rows; 2026-11-01)

Decoder `v16` → `v17`. Measured before (`local/validation/msl-coverage/r26-baseline`) and after
(`local/validation/msl-coverage/r26-v17b`), both at deterministic sample 512, same archives, same
machine, same command (`python3 tools/dev/msl_corpus_coverage.py --sample 512 --out <dir>`). Both
`coverage.json` files record the decoder's SHA-256 and the sampled index list: the baseline's decoder
hash is the committed `v16` file (`e3bd4608…`) and the after-run's is this `v17` file (`8a24cd2f…`),
and the two index lists are identical — so the numbers below are one sample, not two samples.

- **Rows added (12):** VOP1 7 `v_cvt_u32_f32`, VOP1 37 `v_exp_f32`, VOP1 39 `v_log_f32`;
  VOP3 259 `v_add_f32`, 260 `v_sub_f32`, 271 `v_min_f32`, 423 `v_log_f32`,
  323 `v_mad_u32_u24`, 328 `v_bfe_u32`, 344 `v_med3_i32`, 361 `v_mul_lo_u32`;
  and VOP3 196 `v_cmp_gt_u32` (mask destination, so it joins the compare rule as well as the table).
  Every name, opcode value and operand count was read from
  `reference/shadPS4/src/shader_recompiler/frontend/format.cpp`, with each lookup bounded by the next
  `InstructionFormat…` declaration and cross-checked a second time against the same clone's
  `frontend/opcodes.h` enum values (VOP1 7/37/39, VOP3 259/260/271/323/328/344/361/423, VOPC 196).
- **The operand counts are the pinned table's own `src_count`**, not an ISA recollection:
  `InstFormat` is `{inst_class, inst_category, src_count, dst_count, src_type, dst_type}`, and the
  declarations read 1 source for VOP1 7/37/39 and VOP3 423, 2 for VOP3 259/260/271/361/196, 3 for
  VOP3 323/328/344.
- **A real defect was found while reading those counts.** VOP3 423 `v_log_f32` is a **one-source**
  row, but the decoder treated every non-three-source VOP3 row as two-source. Because selector 0 is
  both `v0` and `s0`, that could count an unused SRC1 field as a second scalar source and reject a
  valid program on the GFX7 read-port rule. `VOP3_UNARY` now exists and covers 423 **and 426**
  (`v_rcp_f32`, already modelled, misclassified the same way). This is a **latent** correctness fix,
  not a measured gain: re-running the same sample with `VOP3_UNARY` emptied leaves 410 / 230 / 329
  unchanged, so no sampled unary row currently carries two distinct scalar selectors in SRC0/SRC1.
  The shape is still tested both ways — two distinct scalars in SRC0/SRC1 of a unary row decode, and
  the identical pair on the genuine two-source `v_add_f32` still fails with
  `multiple distinct scalar sources`.
- **Refused, not guessed (all still refused after this card):**
  - VOP3 25/28/212 (`v_cmpx_nge_f32`, `v_cmpx_nle_f32`, `v_cmpx_gt_u32`): the VOP3-encoded `cmpx`
    family. Real programs put a non-EXEC value in VDST, so the destination contract is not
    established. Between them they are **23 of the 81** remaining block-mode first blockers.
  - VOP3 287 `v_mac_f32` (31 blockers): the pinned table declares `src = 2`, but a MAC row reads
    its destination, so whether the operand set is `{VDST, SRC0, SRC1}` or something else is not
    settled by the table.
  - VOP2 32 `v_madmk_f32` (15): mandatory inline literal that the 255-selector literal rule cannot
    express.
  - VOP1 43 `v_rcp_iflag_f32` (1): the row exists for the integer-flag mode. The only pinned
    consumer folds it to plain `V_RCP_F32` (`frontend/translate/vector_alu.cpp`), which is that
    translator's approximation, not evidence about the flag. Refused rather than folded.
- **Deferred, not refused:** MIMG 39 `image_sample_lz` (5), MIMG 1 `image_load_mip` (3) and
  MIMG 8 `image_store` (1). The pinned table gives all of them `src = 4`, `VectorMemory`, so their
  *operand count* is not the open question — their **address shape** is (LZ takes no derivatives,
  LOAD_MIP takes a mip coordinate, STORE writes memory). The decoder validates no coordinate count,
  so naming them now would name rows whose operand shape it does not check. That is the next card.
- **Also still a first blocker (2 programs):** the decoder's own rule
  `modifier field set on a compare row`. That is not a missing row; it is a validation rule, and it
  stays until the modifier semantics on compares are settled.

| Metric (sample 512) | v16 | v17 |
|---|---:|---:|
| Sampled members | 512 | 512 |
| Members: blocked-no-abi-manifest / decode-error / not-a-shader | 69 / 179 / 264 | **70 / 178 / 264** |
| Program rows attempted | 381 | **410** |
| Decoded (linear) | 200 | **230** |
| Decode-error (linear) | 181 | **180** |
| Block-decodable | 268 | **329** |
| Block-decodable share | 70.3 % | **80.2 %** |
| Instructions across decoded programs | 6,828 | **6,933** |
| Per-lane-eligible decoded programs | 76 / 200 | **77 / 230** |
| Members split into stages (stage rows) | 14 (28) | **43 (86)** |
| Branch rows in block mode | 361 | **489** |
| **Lowered** | **0** | **0** |

- **Linear error composition is unchanged where it matters:** 170 of the 180 remaining linear errors
  are control transfers (`SOPP 8` 117, `SOPP 4` 48, `SOPP 5` 5) and the other 10 are opcode rows
  (`VOP3 287` 3, `MIMG 1` 2, `MIMG 39` 2, `MIMG 8` 1, `VOP2 32` 1, `VOP3 212` 1).
  **Adding table rows can no longer move the linear number much** — 10 programs, all of them inside
  the refusal list or the deferred image card.
- **Block-mode first blockers:** `VOP3 287` 30 → **31**, `VOP1 7` 28 → **0**, `VOP2 32` 15 → 15,
  `VOP3 28` 14 → **17**, `VOP3 328` 6 → **0**. The top five after the card are `VOP3 287` 31,
  `VOP3 28` 17, `VOP2 32` 15, `VOP3 25` 5 and `MIMG 39` 5; the rest of the list is `MIMG 1` 3,
  `modifier field set on a compare row` 2, `MIMG 8` 1, `VOP3 212` 1, `VOP1 43` 1. **70 of the 81
  remaining block-mode blockers are deliberate refusals**, the other 11 are the deferred image card
  or that validation rule.
- **Where the measured gain actually came from** — attributed by re-keying every program row on
  `(member path, code sha256)`, not assumed:
  - 29 rows present before are **gone**: the whole spans of 29 paired blobs that used to be reported
    as one undecodable program. The new rows let `split_stages` find a cut point where both halves
    decode, so those members are now two programs each — **58 new stage rows, all block-decodable and
    29 of them linear**. This is the container model becoming correct, unlocked by the rows. (Patch
    harness over the same sample, `local/validation/msl-coverage/whatif_rows.py`: `VOP1 7` alone
    unlocked 25 of the 29 splits and `VOP3 328` the other 4.)
  - 352 rows exist both before and after; of those exactly **3 became block-decodable and 1 became
    linearly decodable**. No shared row regressed.
  - So of `decoded +30`, **29 are new stage rows and 1 is a row that stopped failing**; of
    `block-decodable +61`, **58 are new stage rows and 3 stopped failing**. Stated the other way
    round: the twelve rows themselves moved four program rows, and the rest of the number is the
    container repair they made possible.
- **Tests:** `test_gcn_inventory.py` 48 → **50** (positive cases for every added row, a positive and
  a negative for the unary-source fix, rejections for VOP3 25/28/212 and VOP1 43). Two negative
  probes had to **move rather than be loosened**, because the rows they probed are now modelled:
  `vop1(op=7)` → `vop1(op=9)` (`v_mov_fed_b32`) and `vop1(op=37)`/`vop3(op=323)` →
  `vop1(op=38)` (`v_log_clamp_f32`)/`vop3(op=324)` (`v_cubeid_f32`).
- **Still zero lowered.** Not one real shader has an ABI manifest; decoding more rows does not
  create one, and no lowering was attempted or claimed for any row in this card.


## ABI manifests: the schema, the holes, and why `lowered` is still 0 (2026-11-01)

Task `GPU-01-MSL-OFFLINE-01`. The sections above measure the blocker precisely: `gcn_to_msl.py`
refuses every program without a per-shader ABI manifest, and no retail member has one because the
shader names register **slots** (`SMRD` `sbase`, `MUBUF` `srsrc`, `EXP` `target`) while the draw
state owns what each slot addresses. This card builds the missing organ: a versioned schema, a
generator that emits the exact unfilled form, a validator with per-slot diagnostics, and the
measured hole list for a bounded sample.

`tools/dev/msl_abi_manifest.py` decodes and counts. It never lowers, never compiles, never executes
and never guesses a binding.

```sh
python3 -I tools/dev/msl_abi_manifest.py --self-test              # 76 checks, synthetic spans only
python3 -I tools/dev/msl_abi_manifest.py --program span.bin --stage vs            # the hole list
python3 -I tools/dev/msl_abi_manifest.py --program span.bin --stage vs --json     # same, as JSON
python3 -I tools/dev/msl_abi_manifest.py --program span.bin --stage vs --skeleton form.json
python3 -I tools/dev/msl_abi_manifest.py --program span.bin --manifest filled.json
python3 -I tools/dev/msl_abi_manifest.py --census --sample 512 --top 20 \
    --out local/validation/msl-abi-holes
```

Exit codes: `0` ok, `1` failure (a binding problem, or a shape blocker that no manifest can fix).
The census output and any `--skeleton` path inside this repo must resolve under private `local/`.

### Schema `msl-abi-manifest-v1`

One JSON document per **shader stage**. Unknown fields are rejected and every leaf is type-checked.
Sections default to empty, but a section the program needs and the manifest omits fails with one
`missing-*` diagnostic per slot rather than one generic error.

| Field | Shape | Meaning |
|---|---|---|
| `schema` | `"msl-abi-manifest-v1"` | version gate |
| `shader` | `{id, stage, sha256, bytes}` | `id` a stable name, `stage` `vs`/`ps`, `sha256` the exact program digest, `bytes` its length; the digest is the only tie to a real program |
| `provenance` | `{bindings, note?}` | `state-trace`, `owner-supplied` or `synthetic`; the note is free text |
| `scalar_bases` | `{"N": {kind, view, note?}}` | an `SMRD` `sbase*2`: `kind` is `pointer` (2 scalar cells, `s_load_*`) or `descriptor` (4 cells, `s_buffer_load_*`), `view` names the memory it addresses |
| `buffer_descriptors` | `{"N": {kind:"descriptor", view, format, field, note?}}` | a `MUBUF` `srsrc*4`: `format` is `explicit_float2/3/4`, `field` one of the lowering's named case fields |
| `exports` | `{"T": {semantic, sources?, note?}}` | an `EXP` target: `semantic` names the MRT/param, `sources` optionally pins the four VGPRs the program must feed it from |
| `initial_scalars` | `{"N": binding}` | a scalar read before being written (constant-buffer input, `s124` coefficient selector) |
| `initial_vectors` | `{"N": binding}` | a vector read before being written; `vertex_index` for a formatted-fetch address, `float`+`field`+`component` for a supplied value |
| `views` | `{name: {offset: binding}}` | the dword contents each `view` must expose, keyed by the dword offset the `SMRD` immediate names |
| `policy` | `{floating_point, interpolation?, vertex_select?, vertex_mov_subtract_p10?}` | the knobs `gcn_to_msl.Lowerer` already exposes: the `supplied_float_coefficients`/`supplied_vertex_values` mode, the VSRC→`p10`/`p20`/`p0` map, and its optional subtract list |

`binding` is one of `{kind:"pointer"|"descriptor", view}`, `{kind:"float4", field}`,
`{kind:"float", field, component}`, `{kind:"vertex_index"}`, `{kind:"interpolation_control"}`.
Those are exactly the five cell constructors `Lowerer.bind()` accepts, so a filled manifest is
**projectable** onto the dict the lowerer consumes (`to_lowerer_stage_abi()`, and
`to_lowerer_pair()` for a VS/PS pair). Nothing is lowered by that projection; it is a dict
transformation, and it is the reason a validated manifest is the only missing input on the path.

### Generator: the form, not the answer

`--skeleton` emits the same document with every hole present and `null`. Worked example, measured
on the largest per-lane-eligible program in the sample (`shadow_809795858_shipping.csdr`, 316 B,
65 instructions, `sha256` `98163a1c…cd86`):

```json
{"schema": "msl-abi-manifest-v1",
 "shader": {"id": "shadow_809795858_shipping", "stage": "vs", "sha256": "98163a1c…", "bytes": 316},
 "scalar_bases": {"0": null},
 "buffer_descriptors": {},
 "exports": {"12": null, "32": null},
 "initial_scalars": {"2": null},
 "initial_vectors": {"0": {"kind": "vertex_index"}, "3": null},
 "views": {},
 "policy": {"floating_point": "nearest_even_ftz32_rtz16"}}
```

Six holes, and the shape of each is measured rather than assumed: `s0` is the `sbase*2` of both
root `s_load` rows (`0x1c`, `0x20`), `12`/`32` are the two `EXP` targets (`0x120`, `0x130`), `s2`
and `v3` are read at `0x14` before any write, and `v0` carries the formatted-fetch address at
`0x44`/`0x64` — that one is pre-filled because `MUBUF idxen=1` on a format load *is* the vertex
index, not because a resource was guessed. Only two *bindings* are pre-filled on that basis
(`vertex_index`, and `interpolation_control` for `s124`); the third pre-filled value is
`policy.floating_point`, which has exactly one accepted profile. `vertex_select` stays `null`
because the pinned translator and the ISA summary disagree on the VSRC→vertex mapping.

### Validator: fail-closed, per slot

`--manifest` checks a filled document against the same decoded program. Every diagnostic names the
slot, the requirement and the instruction offset that established it; there is no generic
"invalid".

| Code | Fires when |
|---|---|
| `missing-scalar-base` / `missing-buffer-descriptor` | the program reads that slot and the manifest does not bind it (message carries the sites) |
| `missing-export-target` | the program writes that `EXP` target and the manifest does not declare it |
| `missing-initial-vector` / `missing-initial-scalar` | a register is read before being written and no binding exists |
| `missing-policy` | the encoding forces an interpolation mode or a `vertex_select` entry that the policy omits |
| `missing-view` / `missing-view-dword` | a scalar load reads a dword the named view does not define |
| `unused-scalar-base` / `unused-buffer-descriptor` / `unused-export-target` / `unused-initial-*` | a binding names a slot the program never touches |
| `unused-view-entry` / `unused-policy` | a declared entry no load reads, or a knob the program has no row for |
| `wrong-kind` | a base bound as `descriptor` that the program reads as a `pointer` (or the converse), or a `vertex_index`/`interpolation_control` cell bound as something else |
| `wrong-format` | the descriptor's declared `format` is not the component count the `MUBUF` row asks for |
| `wrong-section` | a binding sits in the wrong section for how the program uses the slot |
| `derived-slot` | the slot is used, but an earlier instruction writes it, so no direct binding can satisfy `Lowerer.pair()` |
| `export-source-mismatch` | `exports[T].sources` disagrees with the `vsrc0..3` the `EXP` row actually names |
| `unfilled-slot` | the value is still `null` |
| `digest` / `bytes` / `stage` / `schema` / `unknown-field` / `slot-key` / `view-name` / `invalid-binding` / `not-an-object` | structural |
| `shape-blocker` | the program cannot be lowered by *any* manifest; reported as `BLOCKED`, never as a binding failure |

Example on real data (a deliberately wrong manifest for the shader above):

```
FAILED: scalar_base s0: scalar_base s0 is read as a pointer at 0x1c, 0x20 but is bound as
        'descriptor'; the lowering reads 2 consecutive scalar cells for a pointer
FAILED: s20: scalar_base s20 is used by this program as buffer_descriptors, not as scalar_bases
FAILED: s24: buffer_descriptor s24 is used at 0x44 but written by an earlier instruction; a direct
        binding cannot satisfy the lowering, so the program is shape-blocked rather than
        binding-blocked
FAILED: 33: export_target 33 is bound but this program never uses it
```

### The measured hole list (sample 512, deterministic, private output)

`--census` re-reads the same 512 members as the v17 coverage card, with the same
`sample_indices` rule, so the two censuses describe one population. It reproduces the v17 numbers
exactly — 410 program rows, **180 not linear**, **153 per-lane blocked**, **77 per-lane eligible**
(153 + 77 = the 230 programs v17 decodes) — and then adds the hole list the coverage card could not
express.

| Aggregate | 77 eligible | 20 largest |
|---|---:|---:|
| Programs analysed | 77 | 20 |
| **manifest-only** (a filled manifest would be the only missing input) | **0** | **0** |
| shape-blocked (a manifest cannot help) | 77 | 20 |
| distinct scalar-base slots | **1** (`s0`) | 1 (`s0`) |
| distinct buffer-descriptor slots | **0** | 0 |
| distinct export targets | **3** (`9`, `12`, `32`) | 3 |
| distinct initial-vector slots | **2** (`v0`, `v3`) | 2 |
| distinct initial-scalar slots | **1** (`s2`) | 1 |
| distinct policy knobs | 0 | 0 |
| scalar view dword requests | 5 | 5 |
| code bytes min/median/max | 32 / 32 / 316 | 32 / 32 / 316 |
| instructions min/median/max | 5 / 5 / 65 | 5 / 5 / 65 |

The population is thin at the tail: the 20 largest are **8 substantial programs (92–316 B,
18–65 instructions) plus 12 five-instruction 32-byte pixel shaders** whose only `EXP` row is
target 9 with `en=0` (no components enabled). The eight substantial ones are all vertex shaders,
and between them they are waiting on **exactly one root pointer, two export targets and three
registers**:

| Program | Bytes | Insns | Binding holes | of which | view dwords | derived slots | shape blockers |
|---|---|---:|---:|---|---:|---:|---:|
| `shadow_809795858_shipping` | 316 | 65 | 6 | `s0`; `EXP 12`,`32`; `v0`,`v3`; `s2` | 2 | 6 | 22 |
| `depthfadeout_256418196_shipping` | 300 | 61 | 5 | `EXP 12`,`32`; `v0`,`v3`; `s2` | 0 | 5 | 21 |
| `depthfadeout_3097466771_shipping` | 276 | 55 | 6 | `s0`; `EXP 12`,`32`; `v0`,`v3`; `s2` | 2 | 7 | 11 |
| `standard_3883884949` | 264 | 55 | 4 | `s0`; `EXP 12`,`32`; `v0` | 1 | 7 | 8 |
| `depth_102051268_shipping` | 252 | 50 | 3 | `EXP 12`,`32`; `v0` | 0 | 5 | 6 |
| `depth_438780663_shipping` | 252 | 50 | 3 | `EXP 12`,`32`; `v0` | 0 | 5 | 6 |
| `depth_673325337` | 252 | 50 | 3 | `EXP 12`,`32`; `v0` | 0 | 5 | 6 |
| `vs_main_5228ca40` | 92 | 18 | 3 | `EXP 12`,`32`; `v0` | 0 | 0 | 11 |

### What that measurement actually says

**A filled manifest would unlock none of them, and the reason is in the lowering, not the ABI.**
Every eligible program carries at least one shape blocker; the histogram over the 77 is
`export-contract` 77, `export-shape` 69, `unsupported-row` 37, `derived-base` 26,
`derived-descriptor` 14, `stream-shape` 6.

1. **The export contract is synthetic.** `Lowerer.run` ends with
   `required = {12,32,33} if stage=='vs' else {0}` and compares for equality, so a vertex shader
   that writes clip plus one param — all eight substantial ones write exactly `12` and `32` — is
   rejected before any binding is read. The tail programs export only target `9` with `en=0` — no
   components enabled, the usual null-export idiom — which the `ps` branch (`required = {0}`) does
   not accept either.
2. **Retail shaders are two-level.** Every descriptor and every scalar base that is *not* the root
   is written by a preceding `s_load_dwordx4 s[…], [s0+…]` before it is read, and `Lowerer.pair()`
   requires the bound cells to be untouched. So the directly bindable ABI surface of a retail VS
   collapses to its **root pointer**: across the whole 77-program sample that is 1 scalar base
   (`s0`) and 0 buffer descriptors. Supporting the rest is a lowering rule (a memory-resident
   descriptor), not a manifest field.
3. **The row set is a 7-mnemonic subset.** `v_cvt_f32_f16`, `v_lshrrev_b32`, `v_sad_u32` and the
   integer rows are outside `LOWERED_ARITHMETIC`, so a manifest cannot reach those programs.
4. **`MUBUF` offsets are real.** Two rows in the largest program carry a nonzero `offset`, which the
   current stream contract forbids.

So the honest ordering of the remaining work is: **lowering shape first, ABI bindings second.**
What this card removes is the excuse that the form was unknown: the holes are enumerated, typed and
validated, and the day a state trace or an owner supplies the root pointer, the two export targets
and the three registers above, the manifest is the only thing left to fill in.

One recorded number needs a correction while it is being reused: the earlier card's
"scalar memory bases `s0`, `s2`, `s8`, `s9`" are the 6-bit **`sbase` field**, not SGPR numbers —
`gcn_inventory` decodes `base_sgpr = sbase*2`, so those four rows address `s0`, `s4`, `s16`, `s18`.
The same program's `MUBUF` descriptor slots (`s20`, `s24`, `s28`) were recorded correctly, but all
three are *loaded* rather than bindable, which is exactly the two-level finding above.

Private output: `local/validation/msl-abi-holes/abi_holes.json` (slots, counts, digests, offsets and
the decoder's own strings only — no payload bytes, no listings). Two consecutive runs of one tool
version produced byte-identical JSON, and the file records that tool version's own SHA-256, so the
numbers above are reproducible from the same inputs. Census **STATIC_ANALYSIS_VERIFIED**, self-test
**FIXTURE_VERIFIED**; **0 lowered**, nothing compiled, no pipeline state, **G3 NOT RUN**.

## Lowering shape: the export model, the row set, and two-level addressing (2026-11-01)

Task `GPU-01-MSL-OFFLINE-01`. The section above measured **0 of 77** per-lane-eligible programs
manifest-only: the ABI holes were tiny, but every program was also blocked by the *lowering's* shape.
This card built the three missing rules and re-measured on the same deterministic sample-512
population. **71 of the 77 now lower to MSL, and all 71 of those compile** with
`xcrun -sdk macosx metal` (`Apple metal version 32023.922`, `air64-apple-darwin27.0.0`).

```sh
python3 -I tools/dev/msl_abi_manifest.py --self-test     # 99 checks, synthetic spans only
python3 -I tools/dev/msl_lower_ladder.py --self-test     # 6 checks, synthetic spans only
python3 -I tools/dev/msl_lower_ladder.py --sample 512 \
    --out local/validation/msl-shape-ladder/step3 --compile
```

### 1. The export model: a sink indexed by target (`export_sink: 'target-array'`)

The old contract was `required = {12,32,33} if vs else {0}`, compared for equality: a closed
synthetic harness profile, not a shader fact. What the encoding actually states, from the pinned
translator (`frontend/translate/export.cpp`) and the pinned attribute table (`ir/attribute.h`):

- `en` is a **component enable mask**; component `j` is fed by `vsrc<j>`, and a component whose bit
  is clear is not written by that row.
- The compressed form packs two fp16 components per payload dword: `en & 1` exports R,G from
  `vsrc0`, `en >> 2 & 1` exports B,A from `vsrc1`, and the pinned translator asserts it for render
  targets and depth only.
- `en == 0` and the **Null** target (9) export nothing.
- `vm` is the **valid mask** (it writes EXEC); `done` marks "last export" but retail vertices put
  `done = 1` on the position export and then write `param0` with `done = 0`, so `done` is recorded
  and never used as an ordering contract.
- Targets: 0–7 MRT, 8 depth, 9 null, 12–15 clip positions, 32–63 params.

Implemented in `gcn_to_msl.Lowerer`: a target-indexed sink `StageExports { rt[8]; depth[1];
pos[4]; param[32]; }` (45 `float4` per lane) with **one write per enabled component**, and
`generate_stage()` for a single real stage plus a compute kernel that copies the sink into a
target-indexed output array. The `named` sink (the old `{12,32,33}`/`{0}` harness) is unchanged and
still used by the pinned pair/logo profiles.

- **Measured:** `export-contract` 77 → **0**; `export-shape` 69 → **0**. The 69 affected programs are
  32-byte pixel shaders whose only `EXP` is `target 9, en = 0, compr = 0, done = 1, vm = 1` — a null
  export with the valid mask, which after this model needs **no binding at all**.
- **Refusals kept, and one that is modelled rather than refused:** a valid-mask export on a *vertex*
  stage is refused (the pinned translator only discards for Fragment); a compressed export outside a
  render-target/depth target is refused (the pinned `ASSERT_MSG`). `vm` **with** components is
  modelled: it writes EXEC, a per-lane-eligible program cannot write EXEC (the decoder gates every
  `writes_exec_mask` row out), so EXEC is exactly the covered-lane set the harness evaluates and the
  write is the identity on every lane it visits. The wave-level effect on *uncovered* lanes is
  stated as outside this bounded model and is recorded per program as `valid_mask_exports`.
- **Not claimed:** that a target-array sink is what Metal should receive. It is the faithful
  *export* model, not a raster pipeline; there is no render pipeline state object anywhere here.

### 2. The row set (`unsupported-row` 37 → 0)

Ten ordinary rows, each value read from `reference/shadPS4/src/shader_recompiler/frontend/format.cpp`
bounded by the next `InstructionFormat…` declaration and each semantic read from the same clone's
`frontend/translate/vector_alu.cpp`:

| Row | Encoding / opcode | Definition used |
|---|---|---|
| `v_and_b32` | VOP2 27 | `a & b` |
| `v_add_i32` / `v_sub_i32` | VOP2 37 / 38 | `a + b`, `a - b`; the VCC carry is not emitted because no lowerable program can read VCC |
| `v_lshlrev_b32` / `v_lshrrev_b32` | VOP2 26 / 22 (VOP3 282 for shl) | `value << (amount & 31)`, `value >> (amount & 31)` with **`src0` the amount and `vsrc1` the value** — the REV convention, taken from the pinned translator, not from the mnemonic |
| `v_mad_u32_u24` | VOP3 323 | `(a & 0xffffff) * (b & 0xffffff) + c` |
| `v_sad_u32` | VOP3 349 | `max(a,b) - min(a,b) + c` (unsigned) |
| `v_cvt_f32_i32` | VOP1 5 | `float(as_type<int>(x))` |
| `v_cvt_f32_f16` | VOP1 11 | `half_float(x & 65535u)` |
| `v_add_f32` | VOP2 3 / VOP3 259 | `add32(a, b)` (symmetry with `v_sub_f32`; no sampled program needs it) |

Integer rows carry a `uint` cell holding the **bit pattern**, so a later float row reading one is an
`as_type<float>` reinterpretation — which is also how the inline-constant file behaves. A structural
tie now exists between the two tools: `gcn_to_msl.ARITHMETIC_ROWS` is the table
`Lowerer.arithmetic()` enforces, and `msl_abi_manifest`'s `LOWERED_ARITHMETIC` is checked against it
by the manifest self-test, so the two cannot drift apart silently.

**Still refused** (decodable, named, and deliberately without a lowering row): `v_sin_f32`,
`v_cos_f32`, `v_fract_f32`, `v_exp_f32`, `v_log_f32`, `v_floor_f32`, `v_sqrt_f32`, `v_rcp_f32`, and
every VOP3-encoded `cmpx` (25/28/212), `v_mac_f32` (287), `v_madmk_f32` (VOP2 32) and
`v_rcp_iflag_f32` (43) — unchanged from the previous card, with a test that they still raise.

### 3. Two-level addressing: the *lowering* already accepted it, the *analysis* did not

The previous section asked whether the lowering can take a memory-resident descriptor. It can, and
it always could: `views[parent][dword] = {"kind": "descriptor", "view": W}` produces exactly the four
cells that `s_load_dwordx4 s[20:23], [s16+4]` stores into `s20..23`, and the next `MUBUF` then
resolves `pair(20, 'descriptor')` to `W`. `tools/dev/test_gcn_to_msl.py` now lowers such a program end
to end. The gap was the **analysis**: `_Analysis` called any slot that a load had written a shape
blocker (`derived-base`/`derived-descriptor`), and the validator refused a binding for it.

Implemented:

- A per-slot **producer classification** (`_producer`), because retail shaders re-use SGPR numbers:
  - `direct` — every read happens at or before the slot's first write (the overwrite's result is
    not read), so the manifest binds the slot;
  - `resident` — every read happens after the **same single** `SMRD` load, so the manifest declares
    the memory that load reads;
  - `multiple-incarnations` / `opaque` — everything else (see the boundaries below).
- A new hole section `resident_slots` carrying the measured `via` slot, parent dword offset and
  width. The manifest expresses one as `{"kind": …, "via": "<slot>", "offset": N, "view": …}` in
  `scalar_bases` or `buffer_descriptors`; `to_lowerer_stage_abi()` materialises the implied
  `pointer`/`descriptor` cell in the parent view, and `_check_views` counts those implied cells as
  dword coverage. New diagnostics: `wrong-producer` (the manifest names a different `via`/`offset`
  than the encoding), `unresolved-resident-parent`, `resident-cell-conflict`.
- **Measured:** `derived-base` 26 → 0 and `derived-descriptor` 14 → 0 as *blockers*; they are holes
  now. Three programs additionally hit `resident-kind-conflict` (below), which is why the count of
  newly lowerable programs is 1 rather than 4.

### The ladder

Same 512 members, same 410 program rows, same 77 per-lane-eligible programs as the v17 and E1081
censuses (the ladder tool reproduces those counters exactly: 264 container-rejected members,
410 = 180 not-linear + 153 per-lane-blocked + 77 analysed).

| Step | Rule added | No longer shape-blocked | Actually lowered | Compiled |
|---|---|---:|---:|---:|
| 0 | E1081 baseline | 0 / 77 | 0 | — |
| 1 | target-indexed export sink | **69 / 77** | 69 | — |
| 2 | + the ten arithmetic rows | **70 / 77** | 70 | — |
| 3 | + memory-resident cells | **71 / 77** | **71** | **71 / 71** |
| — | blocked | 6 | 6 | — |

Steps 1–2 are counted from the baseline's own per-program blocker list: a program becomes
manifest-only when its remaining kinds are gone, and neither step can add a blocker. Step 3 is a
*measured* run (`local/validation/msl-shape-ladder/step3/ladder.json`). A counterfactual "derived-*
removed and nothing else changed" would be 74; the difference is exactly the two new boundaries
below, which are more precise statements about programs the old taxonomy called `derived-*`.

Of the 71 lowered, **69 fill zero bindings** — they are the 32-byte null-export pixel shaders, whose
manifest is genuinely empty. The other two are `vs_main_5228ca40` (92 B, 18 instructions, 3
placeholder bindings) and `standard_3883884949` (264 B, 55 instructions, 11 placeholder bindings).
The placeholder bindings come from `msl_abi_manifest.synthetic_fill()`, which exists to prove the
form closes; **a placeholder binding is not an ABI claim** and the generated MSL for those two
shaders renders nothing meaningful.

### What still blocks, and why

| Kind | Programs | What it is |
|---|---:|---|
| `stream-shape` | 3 | Three sub-cases, all of them "the fetch address is draw state": a **nonzero `MUBUF` immediate offset**, a **raw `buffer_load_dword*`** rather than a formatted fetch (its address register is a byte offset the shader computed, i.e. the index stride), or a **1-component** fetch the stream contract has no format for. The three sampled programs carry the first two at once (`buffer_load_dword{,x3,x4}` at `+16`/`+32` through a computed address, alongside one `buffer_load_format_*` at `+0`). Fixing it needs a `(descriptor, offset)` stream key **and** a stated stride — state the manifest does not have. |
| `resident-kind-conflict` | 3 | The same 16 bytes are read twice: as two 64-bit pointers (`s16:17`, `s18:19`) and as a `V#` (`s20:23`, which is also a `s_buffer_load` base). This is *not* a hardware conflict — one memory read two ways — but the cell model types a dword once, so one declaration cannot be both. |
| `multiple-incarnations` | 4 | An SGPR number re-used across incarnations: `s0` is the bound root, then `s[0:1]` is loaded from it, then `s[0:15]` is overwritten by an `s_load_dwordx16`. Reads straddle the writes, so one slot would have to name more than one resource. The manifest is keyed by slot; this needs a key of `(slot, defining offset)`, i.e. schema v2. |

Two more rules were added to the analysis so it cannot claim a program is manifest-only that the
lowerer then refuses: a non-formatted `MUBUF` (above) and `stream-format-conflict` (one descriptor
read with two component counts, which a single bound `format` cannot serve). The latter fires **0**
times in this sample; it is a tightening of the analysis, not a measured gain.

### Verification, and what is *not* established

- `python3 -m unittest discover -s tools/dev -p 'test_*.py'` → **109 tests OK**
  (`test_gcn_to_msl` 24 → **34**); `msl_abi_manifest --self-test` 76 → **102 checks**;
  `msl_lower_ladder --self-test` 6 checks; `gcn_inventory`'s 50 tests unchanged.
- **71 of 71** generated MSL files compile (`xcrun -sdk macosx metal -c`, exit 0, no diagnostics).
  Two independent `--compile` runs produced identical MSL digests and identical compile results;
  a run without `--compile` produced identical row identities and statuses, so the measurement is
  deterministic on this machine and toolchain.
- The bindings are placeholders except where a program has no holes; the stage is the INF filename
  nibble, not verified semantics; **a compiled MSL file proves syntax and types, not that the
  shader is correct**. There is no pipeline state object, no Metal pipeline, no render, no runtime
  path, and **G3 NOT RUN**.
- Counters: **STATIC_ANALYSIS_VERIFIED** for the census and the per-program attribution,
  **ARTIFACT_VERIFIED** for the compiled AIR under `local/validation/msl-shape-ladder/step3/msl/`,
  **FIXTURE_VERIFIED** for the three self-tests and the 34 unit tests.
- Private outputs: `local/validation/msl-shape-ladder/{step0,step3,step3b,step3c}` plus
  `step0/baseline_rows.json` (the committed-HEAD analyzer's per-program blocker list, used for the
  step-1/step-2 arithmetic). No payload byte, listing or shader text was written outside `local/`.

