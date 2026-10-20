# AUD-02-NGS2-VOICEHANDLE-INTERVAL-950850-01 — 2026-10-20, `/root`

One REA 6.7.0 / Ghidra 12.1.4 `function` query at `0x950cca` (exit 0, 372.45 s, private snapshot copy, no retry), Evidence ID `ev_d3f59e9168f462fdfb3813c864d456cb5b10f75013bc0601351ba7fb14d989f9`. Root replayed `verify_interval.py` (exit 0) and `shasum -c SHA256SUMS` (all OK). Dir: `local/re/evidence/aud02-ngs2-rack-voicehandle-01/remaining-candidates-2/`.

- `FUN_00950850`, span `0x950850..0x951deb` (5532 B), 7 non-contiguous ranges totalling 5491 B (6 small gaps, no candidates). Matches the heuristic interval exactly.
- Ghidra reports 27 unconditional-call edges to thunk `0xf91410`, equal to the 27 heuristic candidates; all decode from the ELF as `E8 rel32` to `0xf91410` and lie in body ranges.
- VoiceHandle tally now: 104 raw candidates = 6 (`FUN_009485a0`) + 23 + 13 + 27 Ghidra-confirmed = 69 Ghidra-confirmed (counting the first callsite family), 35 heuristic-only across 17 intervals (largest `0x951df0..0x952e01`, 7), 0 unattributed.
- Hashes: `verify_interval.py` `8a857e54…0728`, `verification.json` `77e661db…1a4a`, `SHA256SUMS` `0ceb3a20…9125`.
- Not established: reachability, runtime binding, ABI/semantics; provider callers are observations only; 35 candidates lack Ghidra evidence. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
