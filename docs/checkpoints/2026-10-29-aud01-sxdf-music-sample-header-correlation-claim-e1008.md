# E1008 — Live SXDF music sample-header correlation claim

**Date:** 2026-10-29 13:48 CDT
**Agent:** `/root`
**Task:** `AUD-01-SXDF-MUSIC-SAMPLE-HEADER-CORRELATION-01`
**Status:** `ACTIVE`

## Bounded scope

Live-verify the corrected `pakN.psarc` archive identity and exact `sound/shadow/music.sxd1` member identity from the corrected r30 manifest. Read the member in memory, verify its full decoded SHA-256, and inspect only the first 48 bytes. Compare the observed prefix with the E1007 candidate tag sequence at ELF RVA `0x12ba74a`.

Do not extract or write any asset bytes, query REA/Ghidra, implement a parser/decoder, or claim codec, game-audio use, routing, or playback.

## Starting identity

- Archive: `local/extracted/CUSA08809/pakN.psarc`, 12,838,532,222 bytes; corrected summary SHA-256 `559f7a3fa66bc652d36c5f1a885b351e4aeb17dadbba06a0006520bfd325be73`.
- Exact member: `sound/shadow/music.sxd1`, TOC entry 27715, decoded size 9,872 bytes, offset 569314592, first block 13967, expected member SHA-256 `b925ca03cb6dddf8e9d3448c24a2e2eeb0ac595067cd3bbf25087ec3e6692a94`.
- Metadata source: `local/manifests/assets-md5pairing-r30/audio-headers.json` and `CUSA08809-pakN.summary.json`; the entry has a manifest `magic16` beginning `SXDF`, but live bytes have not yet been revalidated.
- ELF candidate source is E1007: RVA `0x12ba74a` begins `FORM\0VAGp\0SXDF\0fmt \0smpl...`; E1007 does not establish audio semantics.

## Claim publication

Claim and E1007 result files are allowlisted documentation. No source or original game asset is changed. Required native/source verification and publication audits will be recorded under `local/validation/AUD-01-SXDF-MUSIC-SAMPLE-HEADER-CORRELATION-01/claim-publication/attempt-01/`.

## Next action and acceptance

Stream-hash the archive against its corrected summary; resolve the exact path using `Psarc.members()`; verify TOC ID, offset, block, size, and full member hash against the corrected manifest; compare the first 48 decoded bytes with E1007's literal sequence while retaining bytes only in memory. Attempt count: 0; 0 failed semantic hypotheses.
