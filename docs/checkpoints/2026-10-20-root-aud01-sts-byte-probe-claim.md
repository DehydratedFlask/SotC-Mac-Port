# AUD-01-STS-BYTE-PROBE-01 claim — 2026-10-20 01:07 CDT

- **Agent:** `/root` (integrator); private worker `/root/aud01_sts_byte_probe`, `gpt-6-luna`
  Extra High.
- **Task:** `AUD-01-STS-BYTE-PROBE-01`, parent `AUD-01`; prerequisites DATA-01 and
  `AUD-01-STS-IDENTITY-01`.
- **Status / attempt count:** ACTIVE, claim prepared; 0 failed analysis hypotheses.
- **Bounded pair:** `cp3shadow/sound/test_adpcm/b_info_01.sts_cp3`, selected as the first
  lexicographically ordered hash-different pair where both decoded lengths are at least 256 bytes.
  Read each decoded member in memory from its original archive through the existing bounds-checked
  `Psarc.read()` reader; do not extract or write payload bytes.
- **Pinned archive/member identities:** Existing private identity artifact
  `local/validation/aud01-sts-identity-01.json`, SHA-256
  `566275c3c104dfc473f8cb3c227641a16b10e2b0da7ffb2811e22b00f877e654`.
  Base `local/extracted/CUSA08809/cachedFiles.psarc`, SHA-256
  `373828b9a6e5c689a66b05a01aa14b295026976438aa7796e9baa789ae03dda0`, member ID `10472`,
  decoded length `344`, member SHA-256
  `d6d74c4ca3cfae66db345551dc22670604d155475a99aa40d6f3a12463056ee7`. Patch
  `local/extracted/CUSA08809-patch/cachedfiles.psarc`, SHA-256
  `656d6870a159013921d036d0b7cf18c679bfc62fe5862b31332c459ff7da05d0`, member ID `10472`,
  decoded length `561`, member SHA-256
  `42eb5a005793f67c3dcd2761ec75ad33f3e83a2d863a30daaf79513ebbe0eb86`.
- **Bounded outcome:** document only byte-supported layout, offsets, boundaries, and differences
  observed within this pair. Provide a replayable private verifier that re-reads the pair and
  recomputes reported values. If bytes do not establish a schema, report that limitation and leave
  format `UNCLASSIFIED`.
- **Owned files:** worker may write only below
  `local/re/evidence/aud01-sts-byte-probe-01/`; `/root` owns shared task/evidence/handoff docs.
- **Acceptance check:** root independently reruns the private verifier; it must exit `0`, match
  both full archive and decoded-member hashes, and corroborate every reported offset/length/boundary
  from live `Psarc.read()` bytes. Keep codec/container/game-use claims unclassified unless
  separately supported with independent evidence.
- **Limits:** directory/extension labels do not prove ADPCM, container schema, patch precedence,
  game use, NGS2 behavior, or playback. No audio decoding or game execution is part of this task.
- **Claim preparation command:**
  ```sh
  python3 - <<'PY'
  import json
  p='local/validation/aud01-sts-identity-01.json'
  d=json.load(open(p))
  rows=sorted(d['comparison']['paired_members'], key=lambda x:x['path'])
  row=next(x for x in rows if x['base']['decoded_bytes']>=256 and x['patch']['decoded_bytes']>=256 and not x['same_sha256'])
  print('path='+row['path'])
  for layer in ('base','patch'):
      m=row[layer]
      print(layer, 'id='+str(m['id']), 'offset='+str(m['offset']), 'first_block='+str(m['first_block']), 'bytes='+str(m['decoded_bytes']), 'sha256='+m['sha256'])
  for layer in ('base','patch'):
      a=d['layers'][layer]['archive']
      print(layer, 'archive='+a['path'], 'bytes='+str(a['bytes']), 'sha256='+a['sha256'])
  PY
  ```
- **Result:** command exited `0` and selected the pair listed above. Identity artifact SHA-256
  `566275c3c104dfc473f8cb3c227641a16b10e2b0da7ffb2811e22b00f877e654` was rechecked at
  2026-10-20 01:06 CDT. A preliminary listing script used a nonexistent `layers.*.members` key
  and exited `1`; reading the actual `comparison.paired_members` schema corrected the lookup.
  No format hypothesis/analysis attempt was made by that failed listing script.
- **Next exact action:** regenerate the project folder index; run Graft freshness check; run staged
  and full publication path audits, whitespace checks and fetch/divergence check; commit and push
  these allowlisted claim files; then dispatch one `gpt-6-luna` Extra High worker.
- **Publication result:** project index exited `0` (`4,136 folders indexed; written`); Graft check
  exited `0` (wiring graph synchronized, meaning tier unbuilt with 1,219 nodes pending); staged and
  cached whitespace checks passed; staged audit accepted 5 paths; full audit accepted 594 paths;
  fetch succeeded with pre-commit divergence `0 0`. Commit `e51b60e` (`docs: claim paired audio
  format probe`) pushed (`5008163..e51b60e`). Post-push tree was clean and HEAD equaled `origin/main`
  at `e51b60edc638ed056bdfcfe00073a6b2a9ea5bbd`, divergence `0 0`.
- **Next exact action:** receive the private report/verifier under
  `local/re/evidence/aud01-sts-byte-probe-01/`; root will independently rerun it before changing
  the task status.
- **Dispatch checkpoint:** the worker was dispatched after publication commit `e51b60e` and
  follow-up record `452b55a`. Its task is independent of the local ELF/REA analysis.

## 2026-10-20 01:11 CDT checkpoint — base archive digest correction

- **Agent / task / status:** `/root`; `AUD-01-STS-BYTE-PROBE-01`; ACTIVE, 0 failed analysis
  hypotheses. The earlier claim text contained a base archive SHA-256 transcription error; it is
  corrected above. The original text said `373828b9a6e5c689a6a05a01aa14b295026976438aa7796e9baa789ae03dda0`;
  the verified value is `373828b9a6e5c689a66b05a01aa14b295026976438aa7796e9baa789ae03dda0`.
- **Files changed:** `HANDOFF.md` and this shared checkpoint. No source/reference/game inputs
  changed. Root's private output is
  `local/validation/aud01-sts-base-digest-recheck-2026-10-20.log`.
- **Exact checks:**
  `shasum -a 256 local/validation/aud01-sts-identity-01.json local/extracted/CUSA08809/cachedFiles.psarc local/extracted/CUSA08809-patch/cachedfiles.psarc`
  exited `0`; the identity JSON and both live archives hashed to the values recorded above. A
  targeted JSON read using
  `python3 -c "import json; d=json.load(open('local/validation/aud01-sts-identity-01.json')); print('Identity JSON archive hashes:'); print('base', d['layers']['base']['archive']['sha256']); print('patch', d['layers']['patch']['archive']['sha256'])"`
  also exited `0` and matched both live archive digests. See the private log for the digest output.
- **Observation vs inference:** the verified identity JSON and current archive bytes agree on the
  corrected base digest. This corrects the pin used by the probe; it says nothing about the
  `.sts_cp3` format or runtime behavior. The worker's probe remains in progress and no payload bytes
  were written to disk.
- **Publication state:** before this correction root regenerated `docs/FOLDERS.md` with
  `python3 scripts/project_index.py` (exit `0`, `4,136 folders indexed; written`), fetched
  `origin main` (exit `0`), and verified `HEAD...origin/main` divergence `0 0` at `46e7b8d`.
  Commit `46e7b8d` (`docs: update active analysis handoff`) was already pushed and the tree was
  clean. This checkpoint's documentation edits still require path audits, commit, push, and
  post-push verification.
- **Risks / next exact action:** no analysis hypothesis failed. Continue the worker's in-memory
  `Psarc.read()` probe using the corrected archive identity; root independently reruns its verifier
  before recording evidence or changing task status. G1 remains NOT PASSED.
