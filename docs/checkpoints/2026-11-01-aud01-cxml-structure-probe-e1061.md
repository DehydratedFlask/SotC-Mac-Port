# E1061 — `shadow_sounds.cxml` structure probe: the E828 rejection was a 3-byte length header, not DTD

**Date:** 2026-11-01 (America/Chicago)
**Task:** `AUD-01-PAKN-CRMP-CXML-STRUCTURE-PROBE-01` (CXML half only; CRMP half stays as E828 verified it)
**Status:** **CXML structure VERIFIED** (`ARTIFACT_VERIFIED` for the measured bytes); **PREMISE CORRECTED**
**Failed tries:** 0 (the E828 parser rejection is explained below; it was a scope bug in the earlier probe, not a failed hypothesis about the bytes)

## Premise correction

E828 recorded that the member "was not accepted by the standard-library Expat parser configured to
reject DTDs" and left the root/element/attribute counts **UNKNOWN**. That wording invited two wrong
readings: that the bytes might be malformed XML, or that a DTD triggered the rejection.

Neither is true. Re-running the *same* Expat configuration (doctype-rejecting handlers, parameter
entity parsing `NEVER`, external entities refused) directly on the whole member produces
`ExpatError` code 4, `not well-formed (invalid token)`, **line 1, column 0** — i.e. it fails on the
first byte. The same configuration accepts the bytes from offset 3 to EOF unchanged. The document
contains **no DOCTYPE at all** (`doctype_declared: false`, 0 entity declarations, 0 external entity
references).

The member is **not** a bare XML file. It is a length-prefixed XML text container:

```
[3-byte minimal base-128 varint = 355715] [355715 bytes of UTF-8 XML 1.0 text]
```

`355715 = 355718 - 3`, exactly. The observed prefix bytes `83 db 15` are the *minimal* unsigned
base-128 ("LEB128") encoding of 355,715; a plain 24-bit reading of those three bytes gives
1,432,451 (LE) or 8,641,301 (BE), neither of which is the payload length, so the varint reading is
the specific match rather than a generic 3-byte integer. The tool's `--self-test` pins that
distinction.

## Live identity (E824 pins re-verified)

| Item | Value |
|---|---|
| Archive | `local/extracted/CUSA08809/pakN.psarc`, 12,838,532,222 B |
| Archive SHA-256 | `559f7a3fa66bc652d36c5f1a885b351e4aeb17dadbba06a0006520bfd325be73` (matches the E827 pin) |
| Member path | `sound/shadow/_cmn/shadow_sounds.cxml` |
| TOC entry | 59169 |
| Decoded size | 355,718 B (matches E824) |
| Decoded SHA-256 | `fc0b20fcd764f46b57142c9b30e51a524c31d936f476fc98cfb92492c0ee0c64` (matches E824) |

Read entirely in memory through `scripts/psarc.py` (`Psarc.entry_index` + `Psarc.read`, MD5(path)
pairing); nothing was written to disk and no member bytes are stored in the repository.

## Byte-level facts (OBSERVED)

| Property | Value |
|---|---|
| Envelope header | 3 B, hex `83db15`, minimal base-128 varint = 355,715 |
| Declared payload length | 355,715 B (actual 355,715 B; `length_matches: true`) |
| Payload encoding | UTF-8, **no BOM**, 0 NUL bytes, strict UTF-8 decode passes |
| Payload start | `<` at payload offset 0; XML declaration ends at payload offset 55 (`?>` at member offsets 56–57); first element start tag begins at member offset 58 |
| First 64 member bytes | 3 B header + 55 B XML declaration + first 6 B of the root start tag (structure only; no values) |
| Payload SHA-256 | `821e46ccfc15e47ce2bec13f27ab04bf09acb038834823a492b1ce4daa83213d` |
| Head-64 SHA-256 | `5c34dd2b5e737e83f6e6f17d3cbbae47827e4d9e75da765e50f05a0d6b0e6ee8` |

## XML structure (OBSERVED; names and counts only)

Expat (standard library, **non-validating**, parameter entities `NEVER`, external-entity handler
returns 0) accepted the payload to EOF. Because Expat is a conforming non-validating processor and
reached the end of the document, the *payload* is well-formed XML 1.0. That statement covers bytes
`[3, 355718)`; the member as a whole is not an XML document because of its length header.

| Metric | Value |
|---|---|
| Root element | `Sounds` |
| Elements (total) | 8,002 |
| Distinct element names | 2 |
| `Sound` | 8,001 |
| `Sounds` | 1 |
| Max depth | 2 |
| Attribute occurrences | 12,346 |
| Distinct attribute names | 7 |
| `name` | 8,001 |
| `globalInstVars` | 1,480 |
| `testOccl` | 1,214 |
| `occlRadius` | 1,214 |
| `occlYOffset` | 422 |
| `ignore` | 14 |
| `trackVel` | 1 |
| DOCTYPE / entities / external refs | none / 0 / 0 |

Every `Sound` element is empty (`/>`); the only nested level is `Sounds` → `Sound`.

## Independent cross-check (no parser)

A separate private script counted bytes without Expat and reproduced every number:
`<` = `>` = 8,004 (1 declaration + 8,002 element tags + 1 root end tag), `/>` = 8,001,
`</Sounds>` = 1, `<Sounds>` = 1, start-tag names `Sound` 8,001 / `Sounds` 1, the same attribute-name
census, nesting depth 2 with a final depth of 0 (balanced). Two independent methods, identical
counts.

## Tool

New `tools/dev/cxml_probe.py` (fail-closed, in-memory, stdlib only):

- splits the envelope (base-128 varint, with minimality/truncation/overlong rejections) and requires
  the declared length to equal the remaining byte count;
- rejects non-`<` payloads, NUL bytes inside the payload, invalid UTF-8 and empty members;
- parses with a non-validating Expat configured with parameter entities `NEVER` and an
  external-entity handler that refuses and returns 0, so no file or network resource is opened;
- emits element/attribute **names** and counts, max depth, and boolean DOCTYPE/entity facts; it has
  no character-data handler and never records attribute values;
- `--expect-size` / `--expect-sha256` make identity failure a hard rejection (exit 1).

`python3 -I tools/dev/cxml_probe.py --self-test` → **PASS, 33/33 checks** (synthetic fixtures only):
plain and length-prefixed documents, varint width/value cases, length mismatch, truncated,
non-minimal and overlong varints, empty member, non-XML bytes, NUL payload, invalid UTF-8, malformed
XML, size/hash mismatch, the audited `83db15` header shape and the fact that a 24-bit LE/BE reading
cannot match, BOM reporting, external-entity refusal, external-DTD non-request, and assertions that
no synthetic attribute value appears in any report.

## Limits (do not extend these)

- The varint-length envelope is established for **this member only**. One exact observation is not a
  corpus rule; no other `.cxml` was read in this task.
- Counts are structural metadata. No element/attribute values, no character data, no text nodes were
  captured or written; no payload bytes exist in any tracked file.
- No schema, no meaning of `Sound`/`name`/`globalInstVars`, no loader, no codec, no audio-engine
  routing, no playback and no runtime claim. No game gate advanced.
- The E879 `xmllint` invocation stays out of scope and was not repeated.

## Private evidence (may be deleted)

SHA-256 of every private artifact, recorded so the run can be reproduced or the files discarded:

| File | SHA-256 |
|---|---|
| `tools/dev/cxml_probe.py` (tracked) | `b2be70c3afa19cba9fce359a2d2702b689a8cafe8ca2bcf27f8aba76e2e2f011` |
| `report.json` | `6bd85bd4ae385c50e309c5c1e2277636876320575f6f51235ca7d00aca0b2e78` |
| `probe.log` | `74ff3408171765d336cf4d4ca9ef861a7e797113e13ce1d1b7025eff2d1e9666` |
| `selftest.log` | `1a0131ff5949776cf7fc664ddf5c39c5407901319d6a06a7e418e97da1db2802` |
| `crosscheck.py` | `bd09b054787af165c274d93bc05313c72f8d627e3df395370e43b152a6e33c80` |
| `crosscheck.log` | `c3b615825dbe78af892a2b854436e715628c5ff853ee486504056c2800f74644` |
| `archive-sha256.log` | `6d242dad238ea2d37faaece086423a5d52dc365558820bc7968bccf508ff1a03` |

The archive SHA-256 was recomputed here in a single `shasum -a 256` pass and matches the pin
(`559f7a3fa66bc652d36c5f1a885b351e4aeb17dadbba06a0006520bfd325be73`). An earlier scratch script that
printed a 256-byte head dump was deleted after use; the surviving files contain counts, names, sizes
and hashes only.
