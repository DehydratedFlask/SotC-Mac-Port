# AUD-02-NGS2-VOICEHANDLE-INTERVALS-04 — 2026-10-20, `/root`

Two sequential REA 6.7.0 / Ghidra 12.1.4 `function` queries (both exit 0, 364.46 s and 359.61 s, private snapshot copy, no retry). Root replayed `verify_intervals.py` (exit 0, `PASS_STATIC_INTERVALS_ATTRIBUTION`) and `shasum -c SHA256SUMS` (all OK). Dir: `local/re/evidence/aud02-ngs2-rack-voicehandle-01/remaining-candidates-5/`.

- `FUN_0094a200` (query `0x94a26b`): `0x94a200..0x94a33b` + `0x94a340..0x94a4c3`, 704 B; exact match; upgraded `0x94a26b, 0x94a42f`. `ev_90aaba85420bc0396c1b1996e923121fde223552dacaf5a018a15376096993f9`.
- `FUN_0094dc50` (query `0x94dd78`): `0x94dc50..0x94de8f`, 576 B; exact match; upgraded `0x94dd78, 0x94de15`. `ev_8e3c876cb801f20455666c53c9ba14e65bccdfc586c1f0d47098837065634a8b`.
- Tally: 92 of 104 candidates Ghidra-confirmed; 12 heuristic-only across 11 intervals; 0 unattributed.
- Hashes: `verify_intervals.py` `03bdfd22…599e`, `verification.json` `dff691bb…9d6f`, `SHA256SUMS` `5ebe2f27…c38b`.
- Not established: reachability, runtime binding, ABI/NGS2 semantics; provider callers are observations only; 12 candidates lack Ghidra evidence. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
