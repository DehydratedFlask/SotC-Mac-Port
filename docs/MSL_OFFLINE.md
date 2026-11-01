# Offline MSL archive step (GPU-01 prep, offline only)

Status: tool behaviour **FIXTURE_VERIFIED** on two synthetic MSL shaders on this machine; the real
corpus front end is measured below (**STATIC_ANALYSIS_VERIFIED**, zero shaders lowered); no real
`.csdr` compiled, no pipeline state object, **G3 NOT RUN**.

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

- **Added:** MUBUF 12/13/14 (`buffer_load_dword`, `dwordx2`, `dwordx4`) and 15/16/17
  (`buffer_store_dword`, `x2`, `x4`) with explicit component counts (1/2/4 — GFX7 has no
  `dwordx3`), plus VOPC 208–213 (`v_cmpx_{f,lt,eq,le,gt,ne}_u32`), values cross-checked against the
  pinned shadPS4 frontend table (`209 = V_CMPX_LT_U32`, `212 = V_CMPX_GT_U32`).
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
