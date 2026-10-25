# GPU-03 CMSH TEX0 format-code-6 evidence

Date: 2026-10-25 06:24 CDT
Integrator: Codex `/root`
Task: `GPU-03-CMSH-TEX0-FORMAT-6-EVIDENCE-01`
Status: **CLOSED / UNKNOWN**, attempt 0; no semantic hypothesis failed

## Bounded scope

Trace the already observed TEX0 descriptor format code 6 to an independent offline shipped-code or
authoritative format source for byte-to-coordinate semantics. Inputs are only the previously
hash-pinned uncompressed `cube_1m.cmsh` and `cone.cmsh` under ignored `local/`. This is read-only:
do not add a TEX0 decoder or change the synthetic XY-to-UV mapping in the E658 fixture. If no
independent semantic source is found, close the survey as `UNKNOWN` rather than infer from values.

## Starting survey observations

The existing private survey verified both input hashes against E639/E658 and recorded each 24-byte
descriptor plus its stream span using parser-style `base + stored_offset` arithmetic. Both files
have a four-record table beginning at file offset `0x4e` and an eight-byte `0xff` sentinel at
`0xae`. The parser data start is `0xb6`; stored stream offsets share base `0x46`.

| File | Tag file offset | Format | Flags | Stored offset | Size | Computed file span | Span SHA-256 |
|---|---:|---:|---:|---:|---:|---|---|
| `cube_1m.cmsh` | `0x96` | 6 | 2 | `0x250` | 96 B | `[0x296,0x2f6)` | `c97732092653f3fd2e76d7bf72a99f3e00ce3fcef3afdb2d363e185ae2b91065` |
| `cone.cmsh` | `0x96` | 6 | 2 | `0x3d0` | 168 B | `[0x416,0x4be)` | `fd126498e069530ca39144521d4dbae76d85a1a0044aeafc15eb5ad8ca75b7b5` |

The cube has 24 parsed POS0 positions and 96 TEX0 bytes; the cone has 42 positions and 168 TEX0
bytes. The observed ratio is four bytes per position. This does not establish a component encoding,
component order, normalization, orientation, or renderer binding. E640/E649/E658 verify position /
index parsing and harness rendering, not TEX0 semantics. The production parser range-checks the
stream but leaves its payload opaque.

Exact survey commands:

```sh
shasum -a 256 local/orchestrator/gpu03-r31/asset-derived/cube_1m.cmsh local/orchestrator/gpu03-r31/asset-derived/cone.cmsh > local/validation/gpu03-cmsh-tex0-survey-20261025/input-hashes.log 2>&1
python3 -I local/validation/gpu03-cmsh-tex0-survey-20261025/survey_tex0.py > local/validation/gpu03-cmsh-tex0-survey-20261025/survey.log 2>&1
```

Input hashes matched the pins. Full descriptor output is in
`local/validation/gpu03-cmsh-tex0-survey-20261025/survey.log`; raw input hashes are in
`input-hashes.log`. Both stay private and untracked.

## Read-only shipped-code trace

The source SELF is 65,314,052 B, SHA-256
`21982727c6f97fd415b3af5e99c5e071619c4dcc2248787a701a31a45ab0a4e0`. REA's direct `open_binary`
attempt rejected the SELF container (`unsupported binary format`) before analysis. Existing
`local/re/eboot.elf` is 65,214,308 B, SHA-256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. Its private identity sidecar
binds it to that exact SELF, records x86-64 / entry `0xf120` / lowest PT_LOAD vaddr 0, and notes
omitted SELF segment 10. The target is a plaintext-segment reconstruction only, with no source
recovery or execution.

Ghidra 12.1.4 / REA 6.7.0 used profile digest
`769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa` (`ghidra-default`, ephemeral
source-immutable import). The direct MCP `binary_overview({})` request timed out at 330,000 ms with a 2 GiB heap; the
terminal REA CLI completed after receiving the verified install directory, an 8 GiB heap, and a
1,200,000 ms startup deadline. The successful operations are hash-bound to the ELF:

- `ev_51f10a737f14992cc6215e62b361b86a073fdce4a36455feb6d18635bc8a78b4`: entry function at
  `0xf120`, establishing an initial successful provider operation.
- `ev_636842f7a7f6798ff8f85b8100e75e06cd12d54fa6a38ebc5128a05d15ef5551`: `.cmsh` trace returns six
  Ghidra-defined path strings at `0xfa8e63`, `0xfa8e95`, `0xfa8ec6`, `0xfa8ef5`, `0xfa8f27`, and
  `0xfa8f5d`; all six references resolve into one function.
- `ev_0f5c74d9ff766304f337d18aa97b419ba6fa03f84562046fee62c7f04ec82d8c`: `FUN_00143f50` at
  `0x143f50`, body `[0x143f50,0x144290)`, 832 B. Its decompilation is a repeated asset-path
  registration sequence through `FUN_0000f2f0` and `thunk_FUN_00f8f396`; it does not consume mesh
  stream bytes. The complete outgoing-reference list has 170 entries (44 DATA, 84 PARAM, 42
  UNCONDITIONAL_CALL), all 42 calls resolved directly to those two helpers. The dossier reports
  complete coverage for resolved function-body references; REA notes targetless computed flows may
  be omitted.

A read-only scan of the reconstructed ELF found no ASCII `TEX0`, `POS0`, `NRM0`, `TAN0`, or `CMSH`
  tags, while finding six `.cmsh` fragments. This does not exclude hashed/packed tags, parser logic
  in the omitted SELF segment, or other indirect code. The pinned PC reference checkout at
  `70bb43859cd7ff181a6d409540b757866184d60d` had no `.cmsh`/TEX0 implementation in the scoped
  lookup. Its closest GCN/Vulkan vertex-input code does not link to this file format.

## Outcome and next action

- Observation: E660 descriptor fields, ranges, and span digests are verified. The shipped-code
  trace proves model resource paths are registered in `FUN_00143f50`; it does not reach a TEX0
  descriptor decoder.
- Unknown: format-code-6 storage type, component order, scale/bias, UV orientation, binding, and
  whether the engine consumes the stream in the target material path. Four bytes per parsed position
  remains a size relationship only.
- Outcome: close `GPU-03-CMSH-TEX0-FORMAT-6-EVIDENCE-01` as **UNKNOWN**. Keep TEX0 opaque and leave
  the verified E658 synthetic XY-to-UV fixture unchanged. Reopen only with a byte-bound authoritative
  definition or a checked shipped-code consumer path.
- Commands, REA Evidence JSON, snapshot, provider diagnostics, and a no-code private run record are
  under ignored `local/validation/gpu03-cmsh-tex0-survey-20261025/`; see `rea-tex0-analysis-record.md`.
  A Ghidra `binary_overview` timeout and standard stripped-ELF overlap/decompiler warnings remain
  evidence limitations. `local/ghidra/persistent-20261020` was not touched.
- No parser/source edits, decoder changes, or runtime claims. The read-only analysis itself required
  no task-specific tests. Publication gates are recorded in the current `HANDOFF.md`; the task claim
  was published on `main` as `0c29f26ae36d5741ca693a45fc3d1ecab5adccbe`; E661 records its path and
  attribution audits.
