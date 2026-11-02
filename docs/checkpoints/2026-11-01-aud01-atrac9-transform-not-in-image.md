# Checkpoint — AUD-01-AUDIO-FORMAT-01 / E1077: the ATRAC9 transform is not in the executable

- **Local date/time:** 2026-11-01 (evening, local)
- **Agent ID:** subagent session for `AUD-01-AUDIO-FORMAT-01` (worker; **no** `HANDOFF.md` edit — see
  "For the integrator" below)
- **Task ID:** `AUD-01-AUDIO-FORMAT-01`
- **Attempt:** 1 of 3 for this sub-question (`0x94b2c0` characterisation). No prior failed try.
- **Status:** sub-question **complete** as a proved negative. ATRAC9 decode remains **NOT RUN**;
  G0–G6 unchanged.

## What was asked

Disassemble and characterise `0x94b2c0` — the sink the `.sxd2` stream path calls at `0x983b0f` — say
whether it is a decoder, a codec-backend dispatcher or a stub, enumerate its callers and decide which
are on the playback path, and determine whether an ATRAC9 decode is reachable from this executable at
all. **No decode was to be attempted.**

## Result (all `STATIC_ANALYSIS_VERIFIED`)

1. **Extent — from the FDE, not from control flow.** `[0x94b2c0, 0x94d8f2)` = **9,778 B**. Next FDE
   `0x94d900` (342 B); `0x94d8f1` is the trailing `nop` after the cold `call __stack_chk_fail` at
   `0x94d8ec`. **1,841 instructions / 258 basic blocks**, 27-case self-relative command jump table at
   `0x12abe20` (11 distinct targets), 74 direct calls to 18 targets, **0 indirect calls**.
2. **It is not a decoder.** `0x94b2c0` is `…\runtime\source\sndx\sndx_engine_ngs2_orbis.c`'s NGS2
   voice-parameter marshaller. Its only two imported callees are `sceNgs2RackGetVoiceHandle`
   (`0xf91410`) and `sceNgs2VoiceControl` (`0xf91460`). It builds
   `{u16 thisSize; u16 nextOffset; u32 paramId; payload…}` blocks and submits them.
3. **Cmd 0 — the whole `.sxd2` path — never touches payload bytes.** It reads only the descriptor's
   `+0x00`, `+0x04` (format), `+0xd8`, `+0xe8`, `+0xf8`, `+0x100`; it **never reads `+0x10` (payload
   pointer) or `+0x18` (length)**. It maps the format byte through a **second, different 67-entry
   table at `0x12abeb0`** onto an internal waveform-type code (`0x42` → `0x40`) and passes that as a
   scalar to NGS2.
4. **Callers: 13 `e8`, 0 `e9`, 0 raw qwords.** Eleven in `0x981aa0` (6,015 B), two in `0x9838b0`
   (`0x983b0f`, `0x983bd0`, both `cmd 0`). **Only those two are on the streamed `.sxd2` path.**
5. **The decoder host is `libSceNgs2`.** Backend initialiser `0x9485a0` loads sysmodule `0xa7`
   (logs ` Loaded Audio3d module`) and `0xb` (logs ` Loaded NGS2 module`), reads `maxAtrac9Decoders`
   (default `0x100`, clamp ≤ `0x400`) plus `numGrains`/`enableACP`/`numDecoderMemoryBlocks`/
   `maxVoiceChannels`/`maxSubmixers`, builds the `numDecoders=` description `0x12ac4b9`, creates the
   NGS2 system (`0x94932e`, name `"NGS2"`) and four racks (`0x3000`/`0x2000`/`0x1000`/`0x2000`), and
   pools 200-byte decoder/voice slots via `0x9a7220` whose only behaviour is `VOICE_FREE_DECODER`/
   `VOICE_FREE_CHANNEL` logging.
6. **Correction to E1074/E1076.** `maxAtrac9Decoders` **does** have a RIP-relative xref at
   **`0x94876a`**; the key starts at `0x12ac36e` and the earlier "no xref" was scanned at
   `0x12ac371`, the string's second character.
7. **Answer: a decode is not reachable from this executable.** No codec library is imported at all
   (34 libraries / 588 imports; no `libSceAudiocodec`, no `libSceAjm`); the config word
   `[wave+0x5f8]` is read at exactly five sites and all five feed a `0x94b2c0` descriptor or the
   geometry finaliser; the `bextr` ATRAC9 arithmetic exists at exactly two functions image-wide
   (`0x983ea0`, `0x9a8390`). The transform is inside `libSceNgs2` (**labelled inference** — the codec
   is outside the image). **Consequence: the port has no decoder to lift and must supply a native
   ATRAC9 implementation behind the `0x94b2c0` cmd-0 contract.**

## Files changed

- `docs/AUDIO_FORMAT.md` — new section "RESOLVED — the ATRAC9 transform is not in this executable
  (E1077)"; corrected the inherited "maxAtrac9Decoders has no xref" line in §8 of the E1074 section;
  updated the two superseded "still open" items, the unknowns table, the limits and the reproduction
  block.
- `docs/EVIDENCE.md` — new row **E1077**.
- `docs/TASKS.md` — `AUD-01-AUDIO-FORMAT-01` row extended with the E1077 result.
- `local/validation/AUD-01-AUDIO-FORMAT-01/analysis/` — new **read-only** probes (private, gitignored):
  `probe_94b2c0.py` (fde / self-relative table / words), `census_calls.py` (image-wide `e8`/`e9`/qword
  census), `dump_body.py`, `blocks.py`, `reach.py`. None writes payload bytes.

## Exact commands and results

```sh
python3 -I local/validation/AUD-01-AUDIO-FORMAT-01/analysis/probe_94b2c0.py fde 0x94b2c0
# 0x94b2c0 -> FDE [0x94b2c0, 0x94d8f2) = 9778 B
python3 -I local/validation/AUD-01-AUDIO-FORMAT-01/analysis/census_calls.py 0x94b2c0
# e8: 13, e9: 0, qwords: 0   (47,606,392 file-backed bytes scanned)
python3 -I local/validation/AUD-01-AUDIO-FORMAT-01/analysis/blocks.py 0x94b2c0 0x94d8f2
# 1841 instructions, 258 basic blocks
python3 -I local/validation/AUD-01-AUDIO-FORMAT-01/analysis/reach.py 0x94b2c0 0x94d8f2 \
    0x94b8b1 0x94bd6f 0x94c713 0x94c866 0x94c9d1 0x94c9f1 0x94b331 0x94b357 0x94caf3 0x94cde8
# per-arm exclusive bytes: 2374 / 2289 / 339 / 923 / 32 / 258 / 834 / 513 / 757 / 900
python3 -I scripts/audit_publish_paths.py --all
# PUBLISH PATH AUDIT OK (4066 path(s))
```

`NOT RUN`: `cmake`/`ctest`/`tools/lift/run_all_checkers.sh` (out of scope for a docs-only
characterisation and explicitly excluded by the task); no native code, no build.

## Observations vs inference

- **Observed:** every address, byte, table entry, FDE extent, import name, call census and count
  above. Also observed: the reject arm logs and *continues*; the "Unknown parameter" default arm logs
  with code −7 and then **returns 0** (a `−7` return was an early misreading of `edi`, corrected
  before publication).
- **Inference (labelled in the doc):** that `libSceNgs2` performs the ATRAC9 transform. It is the only
  reading consistent with the evidence, but the codec is outside the image and is **not** verified.

## Remaining risks

- The `enableACP`/`maxAtrac9Decoders` **runtime values** are unknown: both are read through
  `0x959eb0` from a game-supplied config object, so whether the Sampler rack actually receives a
  non-zero decoder count at run time cannot be decided statically.
- `0x9a8390`'s `format == 0x42` branch is only exercised with config2 (field `0x0C`) present, which
  never happens in this corpus; its behaviour with a non-null config2 is unread.
- The 960-byte block's `242/481/720` entropy phase (E1076) is still unexplained. `0x94b2c0` is now
  excluded as the boundary-finder, so the next candidate is the stream reader `0x990fe0`/`0x9910e0`
  (254/259 B) and the NGS2 side.
- Ghidra was **not** used (no re-import permitted); every claim comes from the byte-level disassembly
  plus an FDE/`getFunctionContaining`-equivalent range check, so a Ghidra body-coverage artefact
  cannot have influenced it.

## Next exact action

AUD-01: **do not** look for a decoder in this image — there is none. The next useful measurement is
either (a) the stream reader pair `0x990fe0`/`0x9910e0` to recover how many bytes of a 960-byte block
the stream object hands on before `0x9910e0` consumes them, or (b) a native ATRAC9 implementation
specified against the `0x94b2c0` cmd-0 contract and `0x9a8390`'s per-channel offset arithmetic. A
decode attempt with an independent decoder (FFmpeg `atrac9`/`libatrac9`, `0xFE761DF0`, 960-byte unit)
is still open and still **NOT RUN**, but it is no longer blocked on "where does the transform
happen".

## For the integrator (ready-to-paste HANDOFF paragraph)

> **2026-11-01 — AUD-01-AUDIO-FORMAT-01 / E1077 — the ATRAC9 transform is not in this executable
> (subagent checkpoint; docs only, no build).** `0x94b2c0` is characterised: FDE
> `[0x94b2c0,0x94d8f2)` = **9,778 B / 1,841 instructions / 258 basic blocks**, a 27-case
> self-relative command jump table at `0x12abe20` (11 targets), 74 direct calls to 18 targets and
> **0 indirect calls**, with exactly two imported callees — **`sceNgs2RackGetVoiceHandle`**
> (`0xf91410`) and **`sceNgs2VoiceControl`** (`0xf91460`). It is
> `…\sndx\sndx_engine_ngs2_orbis.c`'s NGS2 voice-parameter marshaller: **cmd 0 — the whole `.sxd2`
> path — reads only `+0x00/+0x04/+0xd8/+0xe8/+0xf8/+0x100` of the block descriptor and never the
> payload pointer `+0x10` or length `+0x18`**, maps the format byte through a *second, different*
> 67-entry table at `0x12abeb0` (`0x42`→`0x40`) and submits scalar NGS2 parameters. **13 callers**
> (byte census: 13 `e8`, 0 `e9`, 0 qwords): eleven in `0x981aa0`, two in `0x9838b0` (`0x983b0f`,
> `0x983bd0`) — **only those two are on the streamed `.sxd2` path**. The decoder host is
> `libSceNgs2`: `0x9485a0` loads sysmodule `0xa7` (` Loaded Audio3d module`) and `0xb`
> (` Loaded NGS2 module`), reads `maxAtrac9Decoders` (default `0x100`, clamp ≤ `0x400`) and builds the
> `numGrains=/numVoices=/numDecoders=/numDecoderMemoryBlocks=` description `0x12ac4b9` before
> `sceNgs2SystemCreateWithAllocator` and four rack creations. **Correction to E1074/E1076:
> `maxAtrac9Decoders` does have a RIP-relative xref at `0x94876a`** — the key starts at `0x12ac36e`,
> and the earlier "no xref" was scanned at `0x12ac371`, the string's second character. **Conclusion:
> an ATRAC9 decode is not reachable from this executable** — no codec library is imported at all
> (34 libraries / 588 imports), `[wave+0x5f8]` is read at exactly five sites and all five feed a
> `0x94b2c0` descriptor or the geometry finaliser, and the `bextr` ATRAC9 arithmetic exists at exactly
> two functions (`0x983ea0`, `0x9a8390`). The transform is inside `libSceNgs2` (**labelled inference**,
> codec outside the image); **the port has no decoder to lift and must supply a native one behind the
> `0x94b2c0` cmd-0 contract.** `STATIC_ANALYSIS_VERIFIED`; ATRAC9 decode still **NOT RUN**; no
> payload byte written; G0–G6 unchanged. Files: `docs/AUDIO_FORMAT.md`, `docs/EVIDENCE.md` (E1077),
> `docs/TASKS.md`, plus read-only probes in `local/validation/AUD-01-AUDIO-FORMAT-01/analysis/`
> (`probe_94b2c0.py`, `census_calls.py`, `dump_body.py`, `blocks.py`, `reach.py`).
> `python3 -I scripts/audit_publish_paths.py --all` → OK (4,066 paths).
