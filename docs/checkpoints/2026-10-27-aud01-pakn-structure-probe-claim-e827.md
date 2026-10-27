# E827 — Claim bounded pakN sound-sidecar structure probe

**Date:** 2026-10-27 (America/Chicago)
**Local time:** 15:28 CDT
**Integrator:** `/root`
**Task:** `AUD-01-PAKN-CRMP-CXML-STRUCTURE-PROBE-01`
**Status:** **CLAIMED**, 0 attempts; payload profile **NOT RUN**

## Scope and prior evidence

E798 already covers path and token counts across all seven corrected manifests (170,451 rows and
1,221 candidates). E824 separately verifies the exact decoded identities and locators for these
three `pakN` sound-sidecar members. This task adds only byte-bound structure observations beyond
those completed results.

Live preclaim checks:

```sh
shasum -a 256 local/extracted/CUSA08809/pakN.psarc local/manifests/assets-md5pairing-r30/CUSA08809-pakN.members.jsonl scripts/psarc.py
stat -f '%z %N' local/extracted/CUSA08809/pakN.psarc local/manifests/assets-md5pairing-r30/CUSA08809-pakN.members.jsonl
```

Observed values match E824: `pakN.psarc` is 12,838,532,222 bytes, SHA-256
`559f7a3fa66bc652d36c5f1a885b351e4aeb17dadbba06a0006520bfd325be73`; the corrected manifest is
12,644,864 bytes, SHA-256 `4f7fba6bb85aa29712f502aa0c97ac0daa604dbe9766161d0f71f665b7c290ec`;
`scripts/psarc.py` SHA-256 is
`58997e3b81b32ced7a8f06f780f7593b8f3111da2a59fa4fa05af5a83f4feee7`.

| Exact path | Entry | Decoded size | Decoded SHA-256 |
|---|---:|---:|---|
| `sound/shadow/_cmn/shadow_sounds.cxml` | 59169 | 355,718 B | `fc0b20fcd764f46b57142c9b30e51a524c31d936f476fc98cfb92492c0ee0c64` |
| `sound/shadow/_cmn/shadowrandomevents.crmp` | 57178 | 3,393 B | `14977afe6ba82f641e5e50e9ac5212e39dd07f35d6bdebe47ded3b54049dacd7` |
| `sound/shadow/_cmn/shadowstreams.crmp` | 38233 | 3,295 B | `d3f95c9e91071d234882116d23d91a3512c5149c9b1642fcccda0e7e02512fb9` |

## Acceptance and limits

After the claim is published, a worker may use the documented read-only `Psarc` reader to verify
the exact manifest row, entry ID, decoded length and SHA-256 for each of the three members, then
profile each member in memory only.

- For CXML, report whether a standard-library XML parser accepts the document with DTDs rejected,
  plus root-name, element-name and attribute-name counts. Do not record attribute/text values.
- For each CRMP, report byte length, NUL and printable-ASCII byte counts, and printable-run start
  offsets and lengths only. Do not record string values or claim those runs have a particular
  encoding or meaning.
- Write a deterministic JSON report and command log under
  `local/validation/orch-20261027/aud01-pakn-structure-probe/`; do not extract or write member
  payload bytes. Root will replay independently and an independent reviewer will compare reports.

No schema, event meaning, codec, stream pairing, engine use, routing, playback, runtime or game
acceptance claim is in scope. If the XML is rejected, DTD-bearing, or a member cannot be uniquely
matched, stop that portion and report **UNKNOWN**. E798 and E824 remain the only prior results;
this claim itself does not verify any payload structure or advance a game gate.

## Repository verification before claim publication

Required repository commands:

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
**516 / 0 skipped / 0 failed**; Trail passed with **9,877 nodes / 13,470 edges** (deep layer not
built); project index regenerated and checked current at **31,940 folders**, including 848
checkpoint files. Private logs are under `local/validation/orch-20261027/e827/`. The first strict
project-index check exited 1 because the private inventory was stale after E827 evidence paths
were created; after precreating the remaining evidence paths, regeneration and strict check passed.
The first `git diff --cached --check` exited 2 on trailing Markdown hard-break spaces in the new
checkpoint header; removing those spaces made the final staged diff check pass. These were
publication-hygiene corrections, not task-analysis attempts. A later shell wrapper for capturing
the diff exit code tried to assign zsh's read-only `status` variable and exited 1 after running the
check; rerunning `git diff --cached --check` directly exited 0.

Precommit publication checks passed: staged path audit **7/7**; full tracked-path audit
**3,044/3,044**; current owner identity; default unpublished-range attribution; and cached diff
check. E826 remained `origin/main` before this claim commit. Logs are in the E827 validation
directory above.

## Publication baseline and worker boundary

E826 is published as owner commit `569506365f035d7cd5c24489257ade35e5d83c66` on `main`. The fresh
remote scan after E826 covered 9 branch refs, 12 PR-head refs, 0 tags, 15 unique tips and 2,439
reachable commits; it found zero Claude/Anthropic author/committer identities or assistant-credit
lines. Private `refs/stash` remains unchanged at
`92ceba574bd1e8445b9aa4f728f878fbfd4f57f8` by user instruction.

Root owns shared docs and publication. The worker may start only after this claim is committed and
pushed. The only permitted worker output is private evidence under the path above. No source,
reference, or dependency change is planned.

## E827 claim publication closeout

The claim was committed by the owner as `763c9178c7b12123eb432ea48a9aa76b3a2b7d15` (parent
`569506365f035d7cd5c24489257ade35e5d83c66`). Seven staged/committed paths and the full
**3,044-path** tracked tree passed publication-path audits; exact-range attribution, owner
identity, and committed-diff checks passed. `git push origin main` exited 0; fetch confirmed
`HEAD == origin/main == 763c9178c7b12123eb432ea48a9aa76b3a2b7d15`, clean.

The post-E827 scan fetched 9 branch refs and 12 PR-head refs (0 tags, 15 unique tips), covering
**2,440 reachable commits** with zero Claude/Anthropic author/committer identities and zero
assistant-credit lines. The user-selected stash remained
`92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`.

The worker and root replay result is recorded in [E828](2026-10-27-aud01-pakn-structure-probe-complete-e828.md):
all three E824 identities match; CRMP byte profiles agree; CXML structure remains UNKNOWN after
the DTD-rejecting XML parser did not accept the member.
