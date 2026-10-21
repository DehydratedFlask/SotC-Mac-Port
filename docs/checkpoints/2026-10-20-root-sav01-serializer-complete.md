# SAV-01-SERIALIZER-01 — 2026-10-20, `/root`

`STATIC_ANALYSIS_VERIFIED`. Third function-level SAV-01 step: the serializer behind the `[vtable+0x50]` call is identified and its container format decoded from bytes. Worker (Agent subagent) used the relocation table, stored records, the pinned ELF and 2 new REA 6.7.0 / Ghidra 12.1.4 `function` queries (`0x543f40`: exit 0, 369.2 s, `ev_b6015f644b5b3d8b12f4772042b3f63ccbf682a5d22cd6295cb8a6db9067a4e1`; `0xa810b0`: exit 0, 367.3 s, `ev_89b1f7c7107525f54a266d9d6193a13b9aa2b97bed54975bde938852b2f1086e`; pristine snapshot, no retry; Ghidra's pseudocode/references matched the offline decode). One worker launch failed on its own shell-syntax error before any REA call (logged in its `work/`). Root replayed `verify_serializer.py` (exit 0; worker reports 55/55 mutations caught) and `shasum -c` (0 bad lines over 38 files). Private dir `local/re/evidence/sav01-serializer-01/` (verifier `d91f4294…1fb2`, `verification.json` `0dfd301a…1b7c`, `SHA256SUMS` `2515ed8a…87b0`). Field/class name literals from the binary stay private and are not reproduced here.

## Observations (byte/relocation verified)
- The three callers of the Save request resolve to **two concrete save-object classes** (descriptor ids 4 and `0xd`; vtables `0x15c97a8` and `0x15c9830`; slot `+0x40` targets `0x541350`/`0x5459e0`, slot `+0x50` targets `0x543f40`/`0x548d30`) plus one call through the abstract base (`0x15c98b8`, whose `+0x40/+0x50` slots are the pure-virtual stub). Both classes derive from the base via constructor `0xa55f70` (two direct callers), are heap singletons (sizes `0x490` and `0x460`, globals `0x2eff448`/`0x2eff450`), and header dword 3 is the descriptor id; descriptors are static-initialized in `.bss`.
- Slot `+0x40` fills the save-request strings at `obj+0x18/+0x28/+0x38/+0x48` (resolves the earlier `[this+8]` and string-producer unknowns).
- **Body container = a tagged-chunk stream.** Each chunk: 12 header bytes — u32 name hash, u16 `0xBEEF`, u16 tag, u32 payload length (back-patched) — then the payload. The name hash is FNV-1a-32 (seed `0x811c9dc5`) over the field name after per-character mapping through the imported `_Getptoupper`. The stream's byte-swap flag defaults to 0.
- Progress-type object: 14 chunk-writing sites (one in a loop; a 257-iteration indexed-name loop; one chunk is a u64-length blob). Profile-type object: 27 sites plus 5 helper functions (options-like data). Chunk names are recorded privately.

## Inference only
- The tag→type mapping (tags 4/8/10/12/13) and the individual name hashes.

## Unknown
- Contents of the large "game data" and "options" blobs (producers `0x7e5bb0` / `0x7ddb60`); how the stream is flushed into the saved bytes; who writes the descriptors and when; the orphan `0x53ff50`; the meaning of `r9d` = `0xc`/4.

## Next bounded step
Decode the game-data blob producer `0x7e5bb0` and the stream flush slot `[vtable+0x10]` of vtable `0x15d2450`. Not claimed: any native serializer exists, any save/load works, any hash matches a real save. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
