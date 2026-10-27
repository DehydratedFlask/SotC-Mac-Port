# E823 — Claim three pakN sidecar identity rechecks

**Date:** 2026-10-27 (America/Chicago)
**Integrator:** `/root`
**Task:** `AUD-01-METADATA-SIDECAR-PAKN-REVALIDATE-01`
**Checkpoint:** `E823` (claim only)

## Corrected inputs and scope

The local archive `local/extracted/CUSA08809/pakN.psarc` is **12,838,532,222 bytes** and its freshly computed SHA-256 is `559f7a3fa66bc652d36c5f1a885b351e4aeb17dadbba06a0006520bfd325be73`, matching the documented pin in `docs/ASSETS.md`. The corrected manifest `local/manifests/assets-md5pairing-r30/CUSA08809-pakN.members.jsonl` has SHA-256 `4f7fba6bb85aa29712f502aa0c97ac0daa604dbe9766161d0f71f665b7c290ec`.

The task is limited to these manifest rows:

| Exact path | Entry | Size | Offset | Block |
|---|---:|---:|---:|---:|
| `sound/shadow/_cmn/shadow_sounds.cxml` | 59169 | 355718 | 2302782800 | 40425 |
| `sound/shadow/_cmn/shadowrandomevents.crmp` | 57178 | 3393 | 38205543 | 558 |
| `sound/shadow/_cmn/shadowstreams.crmp` | 38233 | 3295 | 38208936 | 559 |

The implementation contract is in `docs/ASSETS.md` and [the PSARC reconciliation checkpoint](2026-10-24-psarc-md5-reconciliation.md): resolve exact path strings with `Psarc.members()` / `entry_index(path)`, never `Psarc.paths()` ordinals. For each path, verify a unique manifest match and every listed locator field, read the exact entry in memory, then report only decoded length and SHA-256. Do not save or extract payload bytes. Stop if archive, manifest, path, entry, locator, or decoded-size identity disagrees.

No source or public asset files are in scope. This task establishes only the byte identities of these three decoded members; format, schema, codec, precedence, runtime routing, and playback remain unclaimed.

## Preclaim evidence and status

Commands and private logs:

```sh
stat -f 'bytes=%z path=%N' local/extracted/CUSA08809/pakN.psarc
shasum -a 256 local/extracted/CUSA08809/pakN.psarc
shasum -a 256 local/manifests/assets-md5pairing-r30/CUSA08809-pakN.members.jsonl
```

- Archive byte count: `local/validation/orch-20261027/e822/pakn-archive-stat.log`.
- Archive SHA-256: `local/validation/orch-20261027/e822/pakn-preclaim-hash.log`.
- Corrected manifest SHA-256: `local/validation/orch-20261027/e822/pakn-manifest-hash.log`.

Full archive hashing took about four minutes because the input is 12.8 GB. One redundant concurrent hash read was stopped; the remaining full-file hash completed and matched the documented archive identity. No member payload was decoded during preclaim.

The latest fetched GitHub-ref audit after E822 covered 9 branch refs, 12 PR refs, 0 tags, 15 unique tips, and **2,435 reachable commits**, with zero Claude/Anthropic author/committer identities or co-author/generated-by credit lines. The private stash was left unchanged per owner direction.

**Task status:** CLAIMED; **0 failed attempts**. The three member reads and decoded hashes are **NOT RUN**. Worker: `/root/app_next_packet_scout`, limited to a private report/log under `local/validation/orch-20261027/aud01-pakn-sidecar-revalidate/`; no shared-file changes or commits.

**Next exact action:** after this claim is published, run the three-row exact-identity verifier; root independently replays its hashes and locator checks before recording any result.

## Claim publication gates

These commands were run from the repository root. Logs are private under `local/validation/orch-20261027/e823/`.

```sh
cmake -S native -B local/build/orch-r22
cmake --build local/build/orch-r22 -j 12
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh
trail build --check
python3 -I scripts/project_index.py
python3 -I scripts/project_index.py --check
```

Configure/build exited 0 (`ninja: no work to do`); CTest passed **431/431**; lift checkers passed **516**, skipped **0**, failed **0**; Trail check passed at **9,877 nodes / 13,470 edges**, deep layer not built; project-index generation/check passed at **31,930 folders**. These repository gates do not verify the three PSARC members and do not advance audio/runtime acceptance.
