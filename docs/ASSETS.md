# Asset archive findings (DATA-02, partial) — 2026-10-20

Reader: [`scripts/psarc.py`](../scripts/psarc.py) (read-only, bounds-checked, tests in
`scripts/test_psarc.py`). Private outputs: `local/manifests/assets/*.summary.json` and
`*.members.jsonl` (path, size, offset, first block, TOC digest). No member was extracted to disk.

## Observed (all 7 archives parse; member count = header entries - 1)

| Archive | Members | Notes |
|---|---|---|
| base cachedFiles | 40,213 | cmdl/cmat/fs/cmsh/cprt/anb |
| patch cachedfiles (01.01) | 40,216 | same paths + 3 new `*.ar.ctxt`; 35,734 of 40,213 shared paths differ in TOC digest |
| pak0 / pak1 / pak2 / pak3 | 6,918 / 5,630 / 1,313 / 12,053 | shaders, meshes, textures; pak2/pak3 hold `cp3shadow/` anim/model/sound-bank data |
| pakN | 64,108 | 12.79 GB uncompressed; 3 paths also appear in the patch archive |

Archive identity for the private 12,838,532,222-byte `pakN.psarc` was verified by a bounded
1 MiB streaming SHA-256 pass: `559f7a3fa66bc652d36c5f1a885b351e4aeb17dadbba06a0006520bfd325be73`.
The summary under `local/manifests/assets/` records the byte count and digest. The stream reader
checks file identity/size/timestamps before and after hashing and rejects observed replacement,
truncation, or growth; synthetic checks are in `scripts/test_psarc.py`.

- Format: v1.4, `zlib`, 30-byte entries, 64 KiB blocks, flags 0xc, manifest = NUL-separated paths
  (entry 0). A block whose stored size equals its decoded size is raw (rule needed for pak0).
- The TOC 16-byte value is neither MD5(path variants tried) nor MD5(content) on 39 sampled pak2
  members; its meaning is UNKNOWN. A full pakN metadata-only count finds 64,108 unique values for
  64,108 manifest rows, with no repeated values (E122). Uniqueness does not identify the algorithm,
  input bytes or integrity role. Do not use it as an integrity check.

## Overlay + case policy (DATA-02 slice, provisional)

Implemented in [`scripts/psarc.py`](../scripts/psarc.py) (`build_overlay` / `resolve_path`);
fixture: `python3 -I scripts/test_psarc.py`. Report:
`local/manifests/assets/CUSA08809-cachedFiles-overlay.json`.

| Rule | Value |
|---|---|
| Member path match | **exact** UTF-8 (case-sensitive) |
| Collision detection | Unicode **casefold** only; never merge on fold |
| Archive basename | **exact** — `cachedFiles.psarc` (base) ≠ `cachedfiles.psarc` (patch); keep separate extract dirs; never case-blind-copy |
| Mount order (cachedFiles pair) | **base → patch** (patch wins on exact-path collision) |

Observed on this title (from members.jsonl, no re-extract):

- base 40,213 / patch 40,216; shared 40,213; **only_base 0**; only_patch 3:
  `text/common/_cmn/ui.ar.ctxt`, `text/shadow/_cmn/story.ar.ctxt`,
  `text/shadow/_cmn/system.ar.ctxt`
- TOC digest differs on 35,734 shared paths; casefold collisions within/across layers: **0**
- The E75 audit of all seven member manifests found **0** exact duplicate path groups, **0** within-manifest `casefold()` collision groups and **0** differently spelled cross-archive pairs across all 21 archive pairs. This covers only these manifests; it does not prove filesystem/Fios case behavior or mount order.
- The E77 exact-path overlap inventory covers the same seven manifests and all 21 unordered pairs: **40,216** shared-path pair occurrences, with **4,704** equal and **35,512** different TOC-declared-size comparisons. Equal declared sizes do not prove equal member bytes; this is path/size metadata only and says nothing about archive payloads, overlay priority, Fios, mount order, or game use.
- The E123 TOC-field comparison covers those same 40,216 exact-path pair occurrences: **4,479** pairs have equal 16-byte values and **35,737** differ. This is an opaque metadata comparison; it does not prove content equality, digest semantics, overlay precedence, mount order, or game use.
- Status: **PROVISIONAL_OBS**. Patch contains every base path, so a patch-wins overlay is
  consistent with the TOC data; engine Fios/loader mount order is still **unproven** (RE/CPU).

## Audio-relevant members (AUD-01 input; codecs not identified)

`sound/shadow/{boss*,com_*,edp*}.sxd`, `music.sxd1/.sxd2`, `str_{amb,bgm,boss,event}.sxd1/.sxd2`
(pakN, ~1 MB total), `cp3shadow/sound/se/*.hd|.bd` (25 pairs, ~2.7 MB), `cp3shadow/sound/test_adpcm/*.sts_cp3`
(580 tiny files), `sound/shadow/_cmn/{shadow_sounds.cxml,shadowrandomevents.crmp,shadowstreams.crmp}`.
No `.at9`, `.wav`, `.ogg` or similar member exists. Total identifiable sound-container bytes are far
too small to be the whole soundtrack, so music/dialogue streams are NOT yet located.

## AUD-01 header classification and member identity (codec UNKNOWN)

Classifier: [`scripts/classify_audio_headers.py`](../scripts/classify_audio_headers.py)
(tests: `scripts/test_audio_headers.py`). Output:
`local/manifests/assets/audio-headers.json` (86 members across cachedFiles / patch
cachedfiles / pak2 / pak3 / pakN). Each row includes the full decoded-member SHA-256 and PSARC
entry ID, byte offset and first block, independently checked against `Psarc.read()`; the sample
magic and entropy remain prefix observations. **Codec = UNKNOWN for every member. No NGS2
inference.** Tests: `python3 -I scripts/test_audio_headers.py` (12 pass).
**Publish boundary:** the JSON is game-derived and stays under gitignored `local/`; the
classifier/test scripts under `scripts/` may be published (see `scripts/audit_publish_paths.py`).

| Family (OBS label) | Exts | Count | Magic (first bytes) | Entropy (bits/byte, 4 KiB sample) | Notes |
|---|---|---|---|---|---|
| `hd-bd-ascii-xff` | `.hd`/`.bd` | 44 | ASCII `xff\0` (`78666600…`) | 2.90–7.80 | Dominant SE bank pair form in pak2/pak3 |
| `hd-bd-near-empty-3ebe7b` | `.hd`/`.bd` | 4 | `3ebe7b00…` then mostly zeros | 2.52–3.02 | pak2 stubs; trivial inflate is non-evidence |
| `bd-ascii-qath` | `.bd` | 1 | ASCII `qath` | ~4.67 | `cp3shadow/sound/se/edp01.bd` only |
| `other-e5000000` | `.hd` | 1 | `e5000000…` | ~5.62 | `boss22.hd` outlier |
| `sxd-fbad9897` | `.sxd` | 2 | `fbad9897…` | ~5.76–5.82 | base + patch `sound/shadow/bridge.sxd` |
| `sxd-high-entropy-8c0x` | `.sxd`/`.sxd1`/`.sxd2` | 17 | `8c0a…` or `8c06…` | 6.45–7.88 | **Subset** of pakN only — not all pakN sound members |
| `sxd-le-small-ints` | `.sxd`/`.sxd1`/`.sxd2` | 12 | LE u32 ≤ 0x20 at +0/+4 | 4.24–6.20 | Structured / low-entropy pakN |
| `sxd-ascii-80AE` | `.sxd` | 3 | ASCII `80AE` | 6.98–7.28 | e.g. `boss02.sxd`, `boss24.sxd`, `sotc_buss.sxd` |
| other singletons | `.sxd`/`.sxd1` | 2 | `00020000…`, `00050000…` | 4.11 / 7.61 | `com_sys.sxd`; `str_boss.sxd1` embeds path-like ASCII |

Private pair-structure cross-check (2026-10-19): all 25 `.hd`/`.bd` pair groups (50 members)
matched manifest paths, PSARC locators and full hashes. The 22 `xff\0` pairs (44 members) all
store their decoded member size as little-endian u32 at byte offset `0x14` (44/44, zero
mismatches). The prefix has two observed variants; the byte at offset 12 varies, so bytes 4..19
remain opaque. Reproduction:
`PYTHONDONTWRITEBYTECODE=1 python3 -I local/validation/20261019-231219-audio-bank-header-crosscheck/pair_crosscheck.py`;
log: `local/validation/20261019-231219-audio-bank-header-crosscheck/pair_crosscheck.log`.
This establishes a repeated length relation only; codec, bank semantics, NGS2 behavior and game use
remain UNKNOWN.

Bounded reader: [`scripts/audio_bank_header.py`](../scripts/audio_bank_header.py), with six
synthetic fixtures in [`scripts/test_audio_bank_header.py`](../scripts/test_audio_bank_header.py).
The reader verifies the archive summary, exact member paths, manifest locators and hashes before
reporting the XFF magic, opaque bytes 4..19 and the u32le@0x14 decoded-length match. Root's
acceptance logs are in `local/validation/20261019-2315-root-aud01-xff-verification/`. This is
fixture and offline asset evidence only; it does not establish runtime decoding or playback.

Private `.sts_cp3` identity comparison (2026-10-19):
`local/validation/aud01-sts-identity-01.py` re-reads all 290 exact paths from each base/patch
`cachedFiles` layer through `Psarc.read()`, checking archive summary identity and member ID/path/
offset/first-block/decoded-size locators against the private manifests. The path sets match exactly;
all 290 decoded SHA-256 hashes differ and 285 sizes change (5 sizes match). Base/patch decoded
totals are 242,310 / 241,080 bytes; each layer ranges from 0–3,060 bytes. The full size-frequency
tables and per-member identifiers/hashes are private in
`local/validation/aud01-sts-identity-01.json` (SHA-256
`566275c3c104dfc473f8cb3c227641a16b10e2b0da7ffb2811e22b00f877e654`). Root's repeat log matched
the two worker logs. These are identity and byte observations only: the directory/extension names
do not establish an ADPCM codec, schema, gameplay use, NGS2 behavior, or playback.

One paired byte-layout probe (2026-10-20) examined only
`cp3shadow/sound/test_adpcm/b_info_01.sts_cp3`, in memory through the live `Psarc.read()` API. The
base member is 344 bytes; the patch member is 561 bytes. At equal absolute offsets, 39 positions
match and 305 differ; the common prefix is 4 bytes and suffix is 2 bytes. An exhaustive maximal
substring comparison found 101 exact spans of at least 4 bytes at shifted offsets; the longest is
47 bytes at base `[228,275)` / patch `[382,429)`. The replayable report and root verification are
private under `local/re/evidence/aud01-sts-byte-probe-01/`; see E55 and the
[completion checkpoint](checkpoints/2026-10-20-root-aud01-sts-byte-probe-complete.md). These byte
ranges do not establish record boundaries or a schema. Format and codec remain UNKNOWN; there is no
NGS2, runtime-use, patch-precedence, or playback conclusion.

Correction vs earlier note: pakN sound members do **not** all begin `8c0a`. zlib/xz probes on
sampled heads do not yield meaningful payloads (trivial 1-byte inflate rejected). Next: recover
the game's own reader (RE-02/Fios/ULT), not codec guessing.
