# GPU-01 prep — offline shader/material member names

Date: 2026-10-19 (America/Chicago). Agent: `bc-3fe140af`.
Scope: **names only** from `local/manifests/assets/*.members.jsonl`. No member
extraction, no codec/GCN invent, no Metal, no edits to `native/core/` or
`scripts/psarc.py`. Left `docs/ASSETS.md` alone (DATA-02 / AUD-01 contested).

## Filter

| Include | Why |
|---|---|
| `.csdr` | Cooked shader members. Dominate pak0/pak1/pakN; paths under `materials/!cookedshaders/`, `coredata/enginesupport/shaders/`, `particles/`. Matches ASSETS “shaders” label for those paks. |
| `.cmat` | Material members. Only in base/patch `cachedFiles`; paths under `materials/` and `*/materials/`. |

Excluded: every other extension (`.cmsh`, `.ctxr`, `.cmdl`, …). Path-keyword hits that
are not `.csdr`/`.cmat` are noted below as related, not counted in the corpus.

Dedup: unique by exact path string across the seven archive manifests. Base and patch
`cachedFiles` share the same `.cmat`/`.csdr` path set (listed once; both archives recorded
in the private JSONL). Overlay winner is DATA-02’s problem, not claimed here.

## Counts

| Metric | Value |
|---|---|
| Raw manifest rows matching filter | 55,223 |
| Unique paths (`.csdr` + `.cmat`) | **44,757** |
| Unique `.csdr` | **34,392** |
| Unique `.cmat` | **10,365** |
| Paths present in >1 archive file | 10,466 (the cachedFiles set) |
| `.csdr` basenames with `_shipping` | 17,196 |
| `.csdr` basenames with 8-hex hash suffix | 1,414 |

### Raw rows by archive (before path dedup)

| Archive | `.csdr` | `.cmat` | Total |
|---|---:|---:|---:|
| CUSA08809-pakN | 28,470 | 0 | 28,470 |
| CUSA08809-cachedFiles | 101 | 10,365 | 10,466 |
| CUSA08809-patch-cachedfiles | 101 | 10,365 | 10,466 |
| CUSA08809-pak0 | 3,309 | 0 | 3,309 |
| CUSA08809-pak1 | 2,512 | 0 | 2,512 |
| CUSA08809-pak2 | 0 | 0 | 0 |
| CUSA08809-pak3 | 0 | 0 | 0 |

## Sample paths

`.csdr`:

- `coredata/enginesupport/shaders/_ps4/ambientshadows/cs_ambientshadow_69d4a900.csdr`
- `coredata/enginesupport/shaders/_ps4/ambientshadows/cs_ambientshadow_69d4a900_shipping.csdr`
- `materials/!cookedshaders/version57/...` (largest bucket: 21,976 unique under this prefix tree in raw rows)
- `particles/shadow/colossus/...emitter_0_emit.csdr`

`.cmat`:

- `coredata/enginesupport/materials/_ps4/default.cmat`
- `materials/objects/rock/...`
- `cp3shadow/materials/_ps4/...`
- `materials/shadow/characters/...`

## Related (not in corpus)

Three `.cslt` members under `coredata/enginesupport/shaders/_cmn/` in pakN only
(`engine-shaders`, `lightprobe-shaders`, `particle-shaders`). Path says “shaders”;
extension is not `.csdr`/`.cmat`, so they are sidelined until a format pass.

## Private outputs

| File | Contents |
|---|---|
| `local/re/work/gpu01-shader-material-members.jsonl` | One JSON object per unique path: `path`, `ext`, `archives`, sizes/md5s by archive |
| `local/re/work/gpu01-shader-material-summary.json` | Filter, counts, prefixes, samples, related list |

## Status / next

Prep inventory: **DONE** (names). Full GPU-01 (stages, GCN, layouts, scene provenance,
offline Metal conversion) remains blocked on deeper RE/DATA work. Do not treat this list
as a Metal pipeline corpus.
