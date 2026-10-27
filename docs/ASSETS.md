# Asset archive findings (DATA-02, partial) — 2026-10-24

The PSARC member mapping is corrected: member TOC keys are **MD5 of the exact UTF-8
manifest path**, while TOC and manifest orders differ. The old positional pairing assigned
valid decoded bytes to the wrong paths. Claims below use the corrected private manifests in
`local/manifests/assets-md5pairing-r30/`; the historical `local/manifests/assets/` and hash-bound
`local/validation/` artifacts are preserved, not silently replaced. Historical payload claims
that used positional IDs are withdrawn as path-specific evidence.

Reader: [`scripts/psarc.py`](../scripts/psarc.py), read-only and bounds-checked. Use
`Psarc.members()` for `(exact_path, TOC_entry_id)` pairs or `Psarc.entry_index(path)` for lookup.
`Psarc.paths()` returns manifest order only; never use its ordinal as an entry ID. The
[reconciliation checkpoint](checkpoints/2026-10-24-psarc-md5-reconciliation.md) records the private
replay, script tests, and corrections. No decoded member payloads were written by that replay.

## Archive inventory and path-key semantics

| Archive | Members | MD5(path) matches |
|---|---:|---:|
| base cachedFiles | 40,213 | 40,213 |
| patch cachedfiles (01.01) | 40,216 | 40,216 |
| pak0 | 6,918 | 6,918 |
| pak1 | 5,630 | 5,630 |
| pak2 | 1,313 | 1,313 |
| pak3 | 12,053 | 12,053 |
| pakN | 64,108 | 64,108 |
| Total | 170,451 | 170,451 |

All seven observed archives have v1.4 headers, `zlib`, 30-byte TOC entries, 64 KiB blocks,
flags `0xc`, MD5-sorted member TOCs, and alphabetically ordered, NUL-separated path manifests
(entry 0). A stored block whose size equals its decoded size is raw. Pairing itself does not
require sorted input and rejects missing TOC keys, duplicate TOC keys, and multiple paths
claiming one entry. Manifest path counts must cover all member entries.

The MD5 input has no inserted leading slash or terminator, case folding, Unicode normalization,
whitespace stripping, slash collapse, or dot-segment removal. Synthetic fixtures distinguish
those spellings. This identifies the lookup key in this corpus; **it is not a content digest or
integrity check**. The earlier negative MD5(path) samples used incorrect path/entry pairs and
are withdrawn. Earlier payload-MD5 samples do not establish named-member identity.

The private `pakN.psarc` is 12,838,532,222 bytes, SHA-256
`559f7a3fa66bc652d36c5f1a885b351e4aeb17dadbba06a0006520bfd325be73`.
Archive hashes are checked with bounded 1 MiB reads and before/after file identity checks;
that is archive identity evidence only. No executable or Sony module is run.

## Exact-path overlaps and overlay policy

Corrected metadata across seven manifests and all 21 unordered archive pairs gives:

| Comparison | Count |
|---|---:|
| Shared exact-path pair occurrences | 40,216 |
| Equal path key and equal declared size | 40,215 |
| Equal path key and different declared size | 1 |
| Different path key, either size relation | 0 |
| Distinct exact paths / distinct TOC path keys | 130,235 / 130,235 |
| TOC keys associated with multiple distinct exact paths | 0 |
| Repeated exact-path/key associations / associated rows | 40,216 / 80,432 |

The E75 path-only result survives the pairing correction: zero exact duplicate paths within
an archive, zero within-archive distinct-string `casefold()` collision groups, and zero
cross-archive differently spelled casefold-equivalent pairs. E77's former size margins,
E123's value-equality margins, E126's distinct-path value reuse, and E239's cross-tab were
computed on mispaired metadata. They remain historical reproductions of those inputs and
are superseded by the corrected counts above.

The base/patch cachedFiles pair shares 40,213 exact paths; every shared member has the same
TOC path key, declared size, and decoded bytes in the live replay. There are zero base-only
paths and three patch-only paths: `text/common/_cmn/ui.ar.ctxt`,
`text/shadow/_cmn/story.ar.ctxt`, and `text/shadow/_cmn/system.ar.ctxt`.
The former “35,734 differing TOC digests” claim was a pairing artifact.

The helper's provisional overlay policy remains exact UTF-8 path matching, Unicode casefold
for collision detection only, and base then patch with patch winning exact collisions.
This is a caller-selected policy; the engine's Fios/loader mount order remains **unproven**.
Keep base `cachedFiles.psarc` and patch `cachedfiles.psarc` in separate extraction directories;
archive basenames are exact and must not be collapsed by case-insensitive copying.

## Corrected audio-path inventory (codec UNKNOWN)

[`scripts/classify_audio_headers.py`](../scripts/classify_audio_headers.py) now uses MD5-paired
TOC IDs. The corrected `audio-headers.json` has 86 members across base/patch cachedFiles,
pak2, pak3, and pakN, with full decoded-member SHA-256, actual TOC entry ID, offset, and first
block. The live replay reproduces this manifest. The total is **2,073,162,522 decoded bytes**;
the old ~3.7 MB inventory and “too small for the soundtrack” inference are withdrawn.

| Extension | Count | Decoded bytes | Observed prefix |
|---|---:|---:|---|
| `.sxd` | 26 | 313,744,960 | `SXDF` |
| `.sxd1` | 5 | 18,191,856 | `SXDF` |
| `.sxd2` | 5 | 1,714,959,920 | `SXDS` |
| `.hd` | 25 | 344,458 | varying little-endian-looking words |
| `.bd` | 25 | 25,921,328 | first eight bytes zero |

All 86 codec labels remain **UNKNOWN**. File names and magic observations do not establish
framing, codec configuration, channels, sample rate, loop/seek semantics, audio-engine use,
or playback. A case-insensitive path-token census of this corrected manifest (E789) finds two
`music` rows and two `bgm` rows; `dialog`, `voice`, and `stream` occur in zero paths. The four
music/BGM path-name candidates are:

| Archive | TOC entry | Decoded size | Manifest path |
|---|---:|---:|---|
| `pakN.psarc` | 27715 | 9,872 B | `sound/shadow/music.sxd1` |
| `pakN.psarc` | 37843 | 172,367,136 B | `sound/shadow/music.sxd2` |
| `pakN.psarc` | 61642 | 2,461,232 B | `sound/shadow/str_bgm.sxd1` |
| `pakN.psarc` | 17150 | 514,125,344 B | `sound/shadow/str_bgm.sxd2` |

These are path-name candidates only. They do not establish content format, codec, dialogue
coverage, engine routing, or playback. No game-audio gate is promoted.

An E791 hash-bound review of existing saved Ghidra/REA text exports found `.sxd` only as a
FourCC row in the platform-tag registry: `0xa60d60` registers it with `_cmn`, and the owner
installer `0x2569d0` calls that registration at `0x256b1e`. This is platform-tag setup, not an
asset reader or decoder. The apparent `SXDS` substring in import NID `m0iS6jNsXds` resolves to
`sched_get_priority_min`; no `SXDF`/`SXDS` reader evidence was found in the scanned exports.
That scoped no-hit does not prove a reader is absent from unexported analysis or other evidence
types. The audio codec and framing remain **UNKNOWN**; see the [E791 checkpoint](checkpoints/2026-10-27-aud01-sxdf-reader-census.md).

The 290 `.sts_cp3` paths in each cachedFiles layer are all 120 bytes, and every base/patch
pair is byte-identical. The selected `b_info_01.sts_cp3` is 120 bytes in each layer, at actual
TOC IDs 13,300 (base) and 13,302 (patch). E55's 344/561-byte probe and the former all-290
hash/size differences inspected other entries and are withdrawn as STS evidence. Likewise,
`sound/shadow/bridge.sxd` is a byte-identical 144-byte member in both layers at TOC ID 2,581;
E42's 619/638-byte comparison was misattributed. Historical reports remain preserved.

## Generic XFF prefix reader; former audio attribution withdrawn

The former AUD-01 XFF “sound banks” were non-audio paths assigned the wrong TOC IDs. None of
the 86 corrected audio-path members begins `xff\0`. **AUD-01-XFF-01's audio-bank premise is
withdrawn**, including its path-specific live acceptance claims. Existing synthetic parser
checks still verify their bounded byte relation.

[`scripts/audio_bank_header.py`](../scripts/audio_bank_header.py) retains its legacy filename
and error class, but is explicitly a **generic XFF prefix/length reader**. It requires an
explicit member identity manifest, verifies the archive and MD5-paired member locators and
SHA-256, and accepts only a complete `xff\0` member whose u32le at `0x14` equals the input
length. Bytes `0x04..0x13` remain opaque. Reports identify `DATA-02-XFF-PREFIX-01` and
`generic_xff_prefix`; they do not claim an audio format, decoder, or runtime use.

The 44 historical XFF observations can still be reproduced at their original TOC IDs, with
the same decoded hashes and length relation, but their corrected extensions are
`.anb` (18), `.nmo` (15), `.clb` (5), `.nto` (4), and `.skb` (2). A generic structural
observation does not restore the
withdrawn audio attribution. Next: recover actual format readers and engine contracts;
codec, audio output, selected-scene resolution, and game boot remain open.
