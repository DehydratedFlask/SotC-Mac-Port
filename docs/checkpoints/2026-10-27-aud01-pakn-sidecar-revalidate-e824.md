# E824 — Revalidate three corrected pakN sidecar byte identities

**Date:** 2026-10-27 (America/Chicago)
**Integrator:** `/root`
**Task:** `AUD-01-METADATA-SIDECAR-PAKN-REVALIDATE-01`
**Checkpoint:** `E824`

## Scope and result

The task covered only the three exact paths in the corrected `pakN` manifest. The local archive `local/extracted/CUSA08809/pakN.psarc` is **12,838,532,222 bytes**, SHA-256 `559f7a3fa66bc652d36c5f1a885b351e4aeb17dadbba06a0006520bfd325be73`. The corrected manifest `local/manifests/assets-md5pairing-r30/CUSA08809-pakN.members.jsonl` is **12,644,864 bytes**, SHA-256 `4f7fba6bb85aa29712f502aa0c97ac0daa604dbe9766161d0f71f665b7c290ec`.

| Exact path | Entry | Decoded length | Decoded SHA-256 |
|---|---:|---:|---|
| `sound/shadow/_cmn/shadow_sounds.cxml` | 59169 | 355718 | `fc0b20fcd764f46b57142c9b30e51a524c31d936f476fc98cfb92492c0ee0c64` |
| `sound/shadow/_cmn/shadowrandomevents.crmp` | 57178 | 3393 | `14977afe6ba82f641e5e50e9ac5212e39dd07f35d6bdebe47ded3b54049dacd7` |
| `sound/shadow/_cmn/shadowstreams.crmp` | 38233 | 3295 | `d3f95c9e91071d234882116d23d91a3512c5149c9b1642fcccda0e7e02512fb9` |

For each member, the manifest row was unique and matched path, ID, size, offset, and block; `Psarc.members()` found exactly one path-to-ID pair, `entry_index(path)` returned the pinned ID, the TOC fields matched, and `Psarc.read(id)` returned the listed length. The task establishes only these three decoded byte identities. It does not establish format, schema, codec, precedence, routing, playback, or runtime behavior.

No payload bytes were written or extracted. The worker made no shared source or documentation edits. There were zero failed attempts and one successful bounded identity pass.

## Reproducibility and independent review

Worker verifier:

```sh
python3 -I local/validation/orch-20261027/aud01-pakn-sidecar-revalidate/verify_sidecars.py
```

The worker verifier exited 0 and wrote `report.json`, `worker.log`, and `verify_sidecars.py` privately. Root reran the same verifier in a fresh directory:

```sh
python3 -I local/validation/orch-20261027/aud01-pakn-sidecar-revalidate/verify_sidecars.py --out-dir local/validation/orch-20261027/aud01-pakn-sidecar-root-replay
python3 -I local/validation/orch-20261027/aud01-pakn-sidecar-root-replay/check_report.py
```

The root replay exited 0. `check_report.py` verified the worker/root JSON reports are byte-identical, all three expected entry IDs/lengths/digests, and uniqueness flags. Both reports have SHA-256 `94b6369353e1eaf27b4a29e81b4d66a482dc727906009c7ac90458ef658d921d`.

The imported decoder `scripts/psarc.py` has SHA-256 `58997e3b81b32ced7a8f06f780f7593b8f3111da2a59fa4fa05af5a83f4feee7`. The worktree hash matched `git show HEAD:scripts/psarc.py` at published E823 commit `8355662ef7d6d5090f541ea96f0914ab1d996f5b`; the source had no local diff. `/root/slot309_order_review` independently approved the report, checked the byte identities and no-extraction boundary, and confirmed this decoder hash binds the implementation.

Private evidence hashes:

- Worker report: `94b6369353e1eaf27b4a29e81b4d66a482dc727906009c7ac90458ef658d921d`
- Worker verifier: `c7a08c77e506e895639801871a7f25e43375a3c0b168f4201a04e5d41acdd65c`
- Worker log: `14c0eb9a8449641414d44a67e0910d24d5c2cee2864f79275634cbd0e41e1291`
- Root replay log: `03464a4cb70f324cae31e7d797fd195c75065f7e1f57d21e16ff6a32a7399cb6`
- Root report checker: `a204c819695b60f8cd93af4ff06675a81867a083c5018e9267d264ae82d107f7`
- Decoder source: `58997e3b81b32ced7a8f06f780f7593b8f3111da2a59fa4fa05af5a83f4feee7`

Detailed evidence remains private under `local/validation/orch-20261027/aud01-pakn-sidecar-revalidate/`, `aud01-pakn-sidecar-root-replay/`, and `e823/`.

## Status and next action

Status: **ARTIFACT_VERIFIED** for the three decoded byte identities. No game acceptance gate advances. Next exact action: inspect the E662 SELF segment-10 coverage gap and, if the recoverability boundary is bounded, claim a separate offline, read-only follow-up.

## Repository and publication gates

Commands run from the workspace root; logs are in `local/validation/orch-20261027/e824/`.

```sh
cmake -S native -B local/build/orch-r22
cmake --build local/build/orch-r22 -j 12
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh
trail build --check
python3 -I scripts/project_index.py
python3 -I scripts/project_index.py --check
```

Configure/build exited 0 (`ninja: no work to do`); CTest passed **431/431**; lift checkers passed **516**, skipped **0**, failed **0**; Trail graph check passed at **9,877 nodes / 13,470 edges**, deep layer not built; project-index generation/check passed at **31,933 folders**. These repository checks are not game boot/audio/save or Metal acceptance evidence.
