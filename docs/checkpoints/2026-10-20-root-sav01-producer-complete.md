# SAV-01-PRODUCER-01 — 2026-10-20, `/root`

`STATIC_ANALYSIS_VERIFIED`. Second function-level step toward SAV-01. Worker (Agent subagent, resumed once after a session-limit interruption) traced what the Save and Load paths hash, write and check, using stored Ghidra records, the pinned ELF bytes and one new REA 6.7.0 / Ghidra 12.1.4 `function` query (`0xa56080`: exit 0, 360.04 s, `ev_6bcb37c8011e95bac2a7e0a8222c3b31d19fc2030cfd0428bd99c0c699f299c4`, pristine snapshot, no retry; the second permitted query was deliberately not spent because the remaining unknown is a vtable target one function query cannot resolve). Root replayed `verify_producer.py` (exit 0; the worker reports 27/27 verifier mutations caught) and `shasum -c` (0 bad lines). Private dir `local/re/evidence/sav01-producer-01/` (verifier `572d49ef…2cdf`, `verification.json` `442f3bbb…e838`, `SHA256SUMS` `f6a19c2f…7553`). Nothing was tested against a real or synthetic save.

## Byte-verified facts (obj = `[task+0x28]`)
- **Save (`FUN_00a4ab90`, case 10):** body hash = `FUN_00161f50(FUN_00a51330([obj+0x60]), dword [obj+0x10], 0x811c9dc5)`, stored at `[obj+0xc]`; then `fwrite(obj+8, 1, 0x10)` (header) and `fwrite(data, 1, [obj+0x10])` (body). Only the body is hashed, not the 16-byte header. A failed write returns an error code.
- **Load header:** `fread(obj+8, 1, 0x10)` must return 0x10; then header dword 0 must equal 1, header dword 2 (`[obj+0x10]`) must be ≤ `0x6400000` (unsigned), plus a debug-global gate; failures return a distinct error code. No check on header dword 3 or dword 1 at this stage.
- **Load body:** `FUN_00a51180` (exactly one direct caller, the Load path) allocates a `0x18`-byte buffer object of `[obj+0x10]` bytes, replacing `[obj+0x60]`; the body `fread` must return `[obj+0x10]`; the FNV-1a-form hash of the body must equal header dword 1 (`[obj+0xc]`), with a debug-global gate that forces failure.
- **Where header fields come from:** `FUN_00a56080` (Save request) creates the object with header dword 0 = 1, dword 1 = 0, size 0, dword 3 = `[[this+8]]`; a virtual call `[vtable+0x50]` fills a stream; `FUN_00a55450` then sets `[obj+0x10]` to the stream length (`dword [stream+0x28]`), copies the stream bytes into a new buffer at `[obj+0x60]` and sets task mode 2. `FUN_00a558f0` and `FUN_00a55d00` use the same initialization with modes 3 and 4; a whole-image scan finds only these three writers of `[obj+8] = 1`.

## Inference (not verified)
- The body is whatever the serializer wrote into the stream; header dword 2 is the stream's running byte count; mode 2 means Saving (via the dispatcher mapping in the region dossier).

## Unknown
- The serializer at `[vtable+0x50]` and its class (the 8-slot vtable run at `0x15c98b8` is not it); meaning of `[this+8]`; producers of the strings at `obj+0x18/+0x28/+0x38`; the body layout.

## Next bounded step
Resolve the vtable holding slots `+0x40`/`+0x50` for the `this` passed to `FUN_00a56080`; the three game-level callers are `0x5400f3` (in `0x53ffe0`), `0x5452d3` (in `0x5451c0`) and `0x54b4c4` (in `0x54b470`); the relocation table is already decoded in the verifier. Then spend one REA query on the slot `+0x50` target. Not claimed: any serializer exists natively, any save/load works, any checksum matches a real save. G0 PARTIAL, G1 NOT PASSED, G2-G6 NOT RUN.
