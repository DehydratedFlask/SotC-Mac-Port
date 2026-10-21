# AUD-02-REGION-DOSSIER-01 — 2026-10-20, `/root`

`STATIC_ANALYSIS_VERIFIED` (call-graph facts); all role labels below are **inference**. Worker (Agent subagent) built a hash-bound dossier of the audio code region from 34 stored Ghidra records plus 2 new REA 6.7.0 / Ghidra 12.1.4 `function` queries (`0x9ac390`: exit 0, 366.5 s; `0x997700`: exit 0, 359.5 s; pristine snapshot copies, hash-verified, no retry). Root replayed `verify_dossier.py` (exit 0, `PASS_STATIC_DOSSIER_VERIFIED`) and `shasum -c` (0 bad lines); the worker reports 24/24 verifier mutants rejected with the control passing. Private dir `local/re/evidence/aud02-region-dossier-01/` (`DOSSIER.md`, `dossier.json`, verifier `798e3006…c9fe`, `verification.json` `5a46dcaa…4438`, `SHA256SUMS` `c481cd6e…e09a`). Literal strings from the binary stay private and are not reproduced here.

## Call graph (36 functions, 26 direct E8 edges among them — byte-scan matched Ghidra)
- `FUN_009485a0` looks like audio **init** (module loads, config keys, Audio3d and NGS2 creation); `FUN_00949fe0` looks like the matching **shutdown**. They sit in adjacent relocated data-pointer slots; neither has a direct E8 caller.
- **Audio3d push thread:** init stores `FUN_00949f80`'s address in a global and hands thread-create a tiny trampoline at `0x954850` (`call [rdi+0x30]`). `FUN_00949f80` waits on an event flag and calls PortPush; shutdown sets its stop byte.
- **Mixer tick:** `FUN_009536d0` calls SystemRender, PortGetQueueLevel and PortAdvance, then signals the event flag the push thread waits on. `FUN_00997700` is a second driver of the same tick with no audio imports of its own.
- **AudioOut thread:** `FUN_009ab360` starts it (same trampoline pattern), body `FUN_009ac390`; `FUN_009abed0` stops it; `FUN_009ac390` calls the mixer tick, then AudioOutOutputs.
- **Voice/rack layer:** `FUN_0094a510`, `FUN_00949dc0`, `FUN_0094aaf0`, `FUN_0094b2c0` and the NGS2 rack functions.
- Observation: the immediate `0xbb80` is loaded into R8 just before `sceAudioOutOpen` (meaning not established). No function in the dossier directly calls a file, Fios or mmap import.

## Caveats
- Argument-register readings are unverified ABI assumptions; indirect callers of init/shutdown are unknown; 8 audio callsites outside the recorded functions (e.g. `0x950804`, `0x94dc21`) have heuristic function starts; the stored records for `FUN_0094ea10` and `FUN_0099f9d0` come from a non-pristine snapshot (bytes/edges re-checked against the ELF).
- Unknown: sample formats, channel layouts, call-argument meanings, sound-bank loading, rack/voice parameter layouts, runtime behavior.

## Next bounded step
Record `FUN_009975f0` and `FUN_009979af` (callers of the second tick driver) to see what feeds it; or attribute one of the 8 unrecorded callsites. Not claimed: any audio is produced or any design is validated. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
