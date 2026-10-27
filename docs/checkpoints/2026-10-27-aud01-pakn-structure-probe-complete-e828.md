# E828 — Bounded pakN sidecar structure probe

**Date:** 2026-10-27 (America/Chicago)
**Local time:** 15:41 CDT
**Integrator:** `/root`
**Task:** `AUD-01-PAKN-CRMP-CXML-STRUCTURE-PROBE-01`
**Status:** **PARTIAL**, 0 failed analysis tries; one worker profile and one independent root replay

## Scope and pinned inputs

E798 already covered path-token metadata across seven corrected manifests; E824 verified the
decoded identities and locators below. E827 rehashed the full `pakN.psarc` and confirmed its size
and SHA-256 before claiming this structure task. Worker and root then used the read-only `Psarc`
reader to read only these three members into memory and discarded the buffers after profiling.

| Exact path | Entry | Offset | First block | Decoded size | Decoded SHA-256 |
|---|---:|---:|---:|---:|---|
| `sound/shadow/_cmn/shadow_sounds.cxml` | 59169 | 2,302,782,800 | 40,425 | 355,718 B | `fc0b20fcd764f46b57142c9b30e51a524c31d936f476fc98cfb92492c0ee0c64` |
| `sound/shadow/_cmn/shadowrandomevents.crmp` | 57178 | 38,205,543 | 558 | 3,393 B | `14977afe6ba82f641e5e50e9ac5212e39dd07f35d6bdebe47ded3b54049dacd7` |
| `sound/shadow/_cmn/shadowstreams.crmp` | 38233 | 38,208,936 | 559 | 3,295 B | `d3f95c9e91071d234882116d23d91a3512c5149c9b1642fcccda0e7e02512fb9` |

Live input pins at claim time: archive `pakN.psarc`, 12,838,532,222 B, SHA-256
`559f7a3fa66bc652d36c5f1a885b351e4aeb17dadbba06a0006520bfd325be73`; corrected manifest,
12,644,864 B, SHA-256 `4f7fba6bb85aa29712f502aa0c97ac0daa604dbe9766161d0f71f665b7c290ec`; decoder
`scripts/psarc.py`, SHA-256
`58997e3b81b32ced7a8f06f780f7593b8f3111da2a59fa4fa05af5a83f4feee7`. The claim-time hash
command output is `local/validation/orch-20261027/e827/preclaim-input-hashes.log`, SHA-256
`1dc1bbae70ab4d6786c89623aafac8a381413d98a2f143adf12e0ed1661782ad`.

## Results

Worker command, exit 0:

```sh
python3 -I local/validation/orch-20261027/aud01-pakn-structure-probe/profile.py > local/validation/orch-20261027/aud01-pakn-structure-probe/worker.log 2>&1
```

Root independently replayed and compared the reports, both exit 0:

```sh
python3 -I local/validation/orch-20261027/aud01-pakn-structure-probe/root-replay/check_structure.py
python3 -I local/validation/orch-20261027/aud01-pakn-structure-probe/root-replay/compare_reports.py
```

All three entries matched their exact E824 path, ID, manifest row, offset, block, decoded size,
and SHA-256. The CXML member was not accepted by the standard-library Expat parser configured to
reject DTDs. Worker status is `UNKNOWN_NOT_WELL_FORMED_XML`; root independently records `UNKNOWN`
(`ExpatError`). Root, element-name and attribute-name counts remain **UNKNOWN**. The parser result
does not prove the bytes are malformed XML or establish another format.

The CRMP byte profiles agree exactly. `shadowrandomevents.crmp` has 183 NUL bytes, 1,670 printable
ASCII bytes in the inclusive `0x20..0x7e` range, and 497 maximal contiguous printable runs.
`shadowstreams.crmp` has 168 NUL bytes, 1,526 printable ASCII bytes in that range, and 564 runs.
Both reports contain zero-based run offsets and lengths only; they do not contain printable run
contents.

Independent review by `/root/slot309_order_review` and `/root/e826_docs_audit` approved the worker,
root replay, and comparison. Neither reviewer read payload bytes. The reports/scripts/logs contain
no payload writes, extracted bytes, XML text/attribute values, or CRMP run contents.

Worker/root private evidence and SHA-256:

- `local/validation/orch-20261027/aud01-pakn-structure-probe/profile.py` —
  `5ac9393cd68b66666332e5d8f5c47b4e6bf14a7c41500c399a1d004d85bd7018`.
- Worker `report.json` —
  `45855681fa81c472e40fc493ffa17d77344f5ebe87cff3194f760a9ad031f806`.
- Worker `worker.log` —
  `a9e6dd0d8f88510246c92e703264e65163f24446bb0ad80ea9a157a71a9d6f13`.
- Root `root-replay/check_structure.py` —
  `1558c3d7c87fabf50ebdb9d2bf9c4611f108c53dd54a648106c0adcf1435171f`.
- Root `root-replay/report.json` —
  `d873bfa05748f8fa41ab39aa3ae24d6f52d3c50f1829f6dd35440e07e9b7fa3f`.
- Root `root-replay/run.log` —
  `846ab9daf90fbdcc85e20da9ea9a39b078e21fc4ce83b850dfbbbbbf669f319b`.
- Comparator `root-replay/compare_reports.py` —
  `cdf8a7089abed41984e8f532cb3f75a367ca37f9ff23c112165e486492f47f2f`.
- Root `root-replay/compare.log` —
  `0e339f54fba8457e8075091a00bdca730a09d783860755d23200a0179e174da2`.
- Initial comparator assertion log —
  `b644a329e0da9332f54da54ec811caf2f957c067233134df6f7f0ba2ef239087`.

The comparator initially required a per-member false flag that the worker schema did not include.
It exited 1 before comparing output. The persisted comparator was corrected to assert the worker's
global `string_values_recorded: false` field and to verify all expected offsets/blocks, then reran
successfully. This was a comparator-schema correction, not a failed payload-profile attempt.

## Limits and disposition

The overall task is **PARTIAL** because CXML root/name counts remain UNKNOWN. Exact member
identities and CRMP byte observations are **ARTIFACT_VERIFIED**. No schema, event meaning, codec,
stream pairing, audio-engine use, routing, playback, runtime, or game acceptance gate is established.
No failed analysis hypothesis occurred, so no principal escalation applies.

## Repository and publication verification

Repository commands run from the workspace root:

```sh
cmake -S native -B local/build/orch-r22
cmake --build local/build/orch-r22 -j 12
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh
trail build --check
python3 scripts/project_index.py
python3 scripts/project_index.py --check
```

Configure/build exited 0 (`ninja: no work to do`); CTest passed **431/431**; lift checkers passed
**516 / 0 skipped / 0 failed**; Trail graph check passed at **9,877 nodes / 13,470 edges** (deep
layer not built); project index regenerated and checked current at **31,941 folders**, with 849
checkpoint files. Logs are under `local/validation/orch-20261027/e828/`. These checks do not
advance boot, audio, save, Metal, or any other real-game gate.

Precommit publication commands also passed:

```sh
python3 -I scripts/audit_publish_paths.py
python3 -I scripts/audit_publish_paths.py --all
python3 -I scripts/check_commit_attribution.py
python3 -I scripts/check_commit_attribution.py --identity
git diff --cached --check
```

The staged audit passed **8/8** paths and the full tracked-path audit passed **3,045/3,045**.
Unpublished-range attribution, configured owner identity, project-index `--check` (**31,941
folders**), and cached diff checks passed. Logs are under
`local/validation/orch-20261027/e828/`. Commit, push, fetch, and post-push remote scan are verified
after the publication step.
