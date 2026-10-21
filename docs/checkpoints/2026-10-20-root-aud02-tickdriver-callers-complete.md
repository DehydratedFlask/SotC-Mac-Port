# AUD-02-TICKDRIVER-CALLERS-01 — 2026-10-20, `/root`

`STATIC_ANALYSIS_VERIFIED` (facts); classifications are **inference**. Worker (Agent subagent) characterized what feeds the second mixer-tick driver using stored records, the pinned ELF and 2 new REA 6.7.0 / Ghidra 12.1.4 `function` queries (`FUN_009975f0`: exit 0, 365.8 s, `ev_79d81a0f3bb1090dcdfc6f8ed5bf67c35f4382c1a04124ebf5db41149cc55b61`; `FUN_00997970`: exit 0, 365.2 s, `ev_2029eac0d4e6c1522a1d1d47074f4b64f69bf46954ef6b5978b131665ed92e84`; pristine snapshot copies, no retry). Root replayed `verify_tickdriver.py` (exit 0; 16 claims / 169 supports; worker reports 38/38 mutants rejected, control passing) and `shasum -c` (0 bad lines). Private dir `local/re/evidence/aud02-tickdriver-callers-01/` (verifier `57574977…`, `verification.json` `c2876812…`, `SHA256SUMS` `ed7bddbd…`; start with `REPORT.md`). Literal strings stay private.

## Observations
- `FUN_009975f0` (270 B) is a guarded API entry: it bumps a shared guard counter (`0x2dd0a78`), forwards its three arguments unchanged to `FUN_00997700`, drops the counter, and optionally logs. **No static reference to its entry exists** (no E8, E9, RIP-relative, relocation or absolute-pointer reference found).
- `FUN_00997700` (604 B) calls the mixer tick `FUN_009536d0` only when its leftover count is zero, then copies via `FUN_00955120`; if its state word shows a call already in progress it returns an error without waiting. No thread, wait, sleep or indirect call. Its only caller is `FUN_009975f0`.
- `FUN_00997970` (89 B) makes one fixed one-descriptor tick call; its only caller is the wait loop in `FUN_00986a50` (1 ms sleep per iteration, skips the call while the state word still holds its initial "not started" value). "`FUN_009979af`" from the earlier dossier is the call instruction at `0x9979af` inside `FUN_00997970`, not a function entry (a correction of the earlier dossier's wording).

## Inference
- The `FUN_00997700` path is a request-driven pull on the caller's thread — not a registered per-frame callback, a thread body or a one-shot. `FUN_00997970` is a per-iteration pump inside a wait loop. 185 of 228 sibling guarded entries (heuristic function starts) also have no static reference, so these may be unused API surface; whether the second driver ever runs is unknown.

## Unknown
- Who sets the guard counter (starts negative in the image); descriptor field meanings; what `FUN_00986a50` waits for and who calls it (static callers at `0x986e1c` and `0x989556`); table-relative or runtime-resolved calls were not scanned.

## Next bounded step
Record `FUN_00986a50`'s two callers, or locate the guard-counter initializer (cmpxchg from -1 to 0). Not claimed: any audio is produced. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
