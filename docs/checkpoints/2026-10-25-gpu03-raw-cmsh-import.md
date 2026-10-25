# GPU-03 raw CSMH member import — 2026-10-25

## Result

`GPU-03-CMSH-RAW-IMPORT-01` is `ARTIFACT_VERIFIED`. This checkpoint verifies only private extraction of
the two already-pinned raw `.cmsh` members; it does not claim that the C++ parser accepted them or that
Metal rendered them.

The user authorized feeding these identified local raw members into the native parser and private Metal
harness. Import followed the repository's corrected PSARC MD5(path) mapping via `Psarc.entry_index`,
then read each exact entry with `Psarc.read`. The raw members and importer remain under `local/`.

| Member | Archive | Entry | Bytes | SHA-256 |
|---|---|---:|---:|---|
| `coredata/enginesupport/models/_cmn/cube_1m.cmsh` | `local/extracted/CUSA08809/cachedFiles.psarc` | 31789 | 850 | `44738dbf663af252898bad5582f2b48f1aadf6fbaab403c20880410b79a380b7` |
| `coredata/enginesupport/models/_cmn/cone.cmsh` | `local/extracted/CUSA08809/pak0.psarc` | 5574 | 1,462 | `5456afadabe96b9f7f02c503dc0ed5410652ccd32601c19e8aeeaaf36c88600c` |

## Verification

- `python3 -I local/orchestrator/gpu03-r31/asset-derived/import_raw_cmsh.py` — exit 0; both exact
  members matched the size and SHA-256 pins from E629.
- `shasum -a 256 local/orchestrator/gpu03-r31/asset-derived/*.cmsh` — both on-disk files match the
  imported member hashes.
- The importer writes only to `local/orchestrator/gpu03-r31/asset-derived/`; it did not modify the
  original `.pkg` or archive files. No dependency was installed.

## Limits and next work

No raw member bytes have yet been passed to `parse_cmsh_mesh`; the documented POS0 format-code assumption
still needs to match the actual samples. No asset-derived Metal frame or PNG has been rendered. L1 and
G3 remain incomplete. The next rendering slice will use the default shader pair, explicit harness normal
`[0.5, 0.5, 1.0]`, CPU-projected silhouette/depth checks, Metal API/GPU validation, and a private PNG
written directly under `local/`, without an engine frame number or the screenshot attachment seam.
