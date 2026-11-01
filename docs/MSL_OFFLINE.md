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
python3 -I tools/dev/msl_corpus_coverage.py               # default sample 64
python3 -I tools/dev/msl_corpus_coverage.py --sample 512 --out local/validation/msl-coverage/sample-512
```

Population 34,291 `.csdr` (pak0 3,309 + pak1 2,512 + pakN 28,470; 34,291 unique paths). The
default sample is **64**, chosen by deterministic even spacing `round(i*(N-1)/(k-1))` over the
`(path, archive)`-sorted list (first and last included, implied stride 535.8); the 512 run uses
the same rule.

| Sample | compressed-prefix | raw, decode-error | raw, all programs decode | lowered |
|---:|---:|---:|---:|---:|
| 64 | 41 | 23 | 0 | **0** |
| 512 | 264 | 244 | 4 | **0** |

Programs (a member can hold a VS/PS pair): at 64, 31 attempted, 4 decoded, 27 decode-error; at
512, 367 attempted, 74 decoded, 293 decode-error. The 4 programs decoded at n=64 are all 32-byte,
5-instruction, type-0 (INF: PS) `depth`/`shadow` shaders (`EXP 1, SOP1 2, SOPP 2`), so no
*member* is fully decodable there; at n=512, 70 raw members yield at least one decoded program
and 4 decode completely (74 programs, 577 instructions, 73/74 per-lane eligible).

Top decode-error reasons, n=512 / n=64: `unsupported encoding` 235 / 23, `unsupported SMRD
register/literal offset` 39 / 4, `unsupported VINTRP opcode 2` 10 / 0, `unsupported VOP3 opcode
349` 3 / 0. "Unsupported encoding" is the first word outside the bounded table (decode never
resynchronizes): 289 of 293 failures stop between byte `0x14` and `0x2c`.

Supported-opcode share (fraction of a decoded program's instructions whose encoding+opcode is in
the imported `gcn_inventory.OPCODES`): **min 1.0, median 1.0, max 1.0** in both samples. That
distribution is degenerate by construction - `decode()` rejects anything else - so the decode
success rate, not this share, is the informative number.

**Zero real shaders were lowered.** `gcn_to_msl.py` admits a program only through a per-shader
ABI manifest (exact digest, named scalar views, stream bindings) and none exists for any retail
member, so every decodable program is `blocked-no-abi-manifest`; inventing bindings is out of
scope. Stage is the INF nibble label from filename co-occurrence, not verified semantics, and
compressed members are classified, never decompressed. Evidence: the census is
**STATIC_ANALYSIS_VERIFIED**, the tool self-test **FIXTURE_VERIFIED**; no Metal compile, no
pipeline object, **G3 NOT RUN**. Private output `local/validation/msl-coverage/coverage.json`
(+ `sample-512/`); full run log `local/validation/msl-coverage/REPORT.md`.

## Limits

- Synthetic MSL only: two hand-written files, not produced by `gcn_to_msl.py`; 0 of the
  34,392 unique `.csdr` were compiled and no `.cmat` was read.
- No pipeline state objects, render pipeline descriptors, reflection or argument tables; the
  two demo metallibs were archived but never loaded or executed, so there is no G3/Metal
  frame claim and no runtime path. No `native/` change and no build target.
- AIR embeds the resolved source path (`strings`): an identical build in another directory gave
  different AIR digests (`b57a1931…`, `7865c861…`) with identical metallib digests, and
  `-fdebug-prefix-map`/`-ffile-prefix-map` changed nothing. Keep archives in private `local/`.
- Metallib linking was byte-identical across runs on this machine/toolchain only, not across OS
  or toolchain versions; and the tool records what the compiler produced, not MSL semantics.
