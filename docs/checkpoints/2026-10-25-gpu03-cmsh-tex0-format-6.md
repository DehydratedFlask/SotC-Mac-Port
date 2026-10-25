# GPU-03 CMSH TEX0 format-code-6 evidence

Date: 2026-10-25 05:46 CDT
Integrator: Codex `/root`
Task: `GPU-03-CMSH-TEX0-FORMAT-6-EVIDENCE-01`
Status: **ACTIVE**, attempt 0; descriptor layout observed, TEX0 semantics **UNKNOWN**

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

## Evidence and next action

- Observation: exact raw descriptor fields, byte ranges, sizes, and computed span digests above.
- Inference: four bytes per parsed position is a size relationship only; no UV decoding is claimed.
- Unknown: format-code-6 storage type, component order, scale/bias, UV orientation, shader binding,
  and whether the engine consumes this stream in the target material path.
- Exact next action: compute SHA-256 for `local/extracted/CUSA08809/eboot.bin`, open that exact
  binary in the available REA Ghidra provider using its ephemeral project, then search the
  descriptor-code consumer path. Record the executable digest, provider version/profile, image
  base, source locations, coverage, and unresolved indirect calls. Do not touch
  `local/ghidra/persistent-20261020`.
- No source edits or tests have been made for this task. The existing E658 fixture and its
  synthetic UV behavior remain unchanged.
