# AUD-01-STS-BYTE-PROBE-01 — ARTIFACT_VERIFIED

- **Date/time / agent / task:** 2026-10-20 01:15 CDT; `/root`; `AUD-01-STS-BYTE-PROBE-01`.
- **Status / attempts:** ARTIFACT_VERIFIED; 0 failed analysis hypotheses. Parent AUD-01 remains
  ACTIVE. Format/schema/codec and game/runtime claims remain UNCLASSIFIED.
- **Scope:** one paired member only:
  `cp3shadow/sound/test_adpcm/b_info_01.sts_cp3`. No member payload was extracted, written, or
  emitted. No audio decoding, playback, or game execution occurred.
- **Private artifacts:** report
  `local/re/evidence/aud01-sts-byte-probe-01/report.md` SHA-256
  `76ba1c935ba33282c1a59a1a0ba6b434a8f6688623628c6672a5bf0908c1ebe3`; replay verifier
  `verify.py` SHA-256 `4deea41d8f63928ec049b61244667a279a1b4e857927d95eb8f6bb1ca1cfed2b`;
  root JSON `root-verification.json` SHA-256
  `5fae4da501c239e8583bd13effb7d4d5ca68df77932b2e65d819621eabff2471` and empty
  `root-verification.stderr.log`. All are under `local/re/evidence/aud01-sts-byte-probe-01/`.
- **Inputs:** identity JSON SHA-256
  `566275c3c104dfc473f8cb3c227641a16b10e2b0da7ffb2811e22b00f877e654`; base archive
  `local/extracted/CUSA08809/cachedFiles.psarc`, 37,159,164 bytes, SHA-256
  `373828b9a6e5c689a66b05a01aa14b295026976438aa7796e9baa789ae03dda0`; patch archive
  `local/extracted/CUSA08809-patch/cachedfiles.psarc`, 37,274,125 bytes, SHA-256
  `656d6870a159013921d036d0b7cf18c679bfc62fe5862b31332c459ff7da05d0`.
- **Member identities:** both path-to-entry mappings resolve to ID `10472`. Base offset
  `29,129,692`, first block `27,003`, decoded size `344`, SHA-256
  `d6d74c4ca3cfae66db345551dc22670604d155475a99aa40d6f3a12463056ee7`. Patch offset
  `33,892,856`, first block `34,696`, decoded size `561`, SHA-256
  `42eb5a005793f67c3dcd2761ec75ad33f3e83a2d863a30daaf79513ebbe0eb86`.
- **Exact verification command / result:**
  `python3 -I local/re/evidence/aud01-sts-byte-probe-01/verify.py > local/re/evidence/aud01-sts-byte-probe-01/root-verification.json 2> local/re/evidence/aud01-sts-byte-probe-01/root-verification.stderr.log`
  exited `0`; stderr is empty. `cmp -s
  local/re/evidence/aud01-sts-byte-probe-01/worker-verification.json
  local/re/evidence/aud01-sts-byte-probe-01/root-verification.json` exited `0`; both JSON files
  hash to `5fae4da501c239e8583bd13effb7d4d5ca68df77932b2e65d819621eabff2471`. Root also hashed the
  report, verifier, and live source `scripts/psarc.py`; they match the worker's report/verifier
  digests and source digest `3bef8a66705c34ab43c321b688ec675cc3cd7aa0e07ec3afdb0f956e3e80cdc9`.
- **Byte observations:** equal absolute offsets across `[0,344)` contain 39 equal and 305
  different positions; common prefix is 4 bytes, common suffix is 2. The exhaustive scan reports
  101 maximal exact substrings of at least 4 bytes at shifted offsets; longest is 47 bytes at base
  `[228,275)` / patch `[382,429)`. The patch has 217 bytes beyond the base member's end. These are
  literal byte relationships; they do not establish record boundaries or fields.
- **Source/tool evidence:** worker used Graft 0.21.1 query
  `graft ask "How does Psarc.read() locate and decode a PSARC member, and what does it return?" --source --in scripts/`
  (exit `0`, about 10,585 tokens saved), hit `scripts/psarc.py:L230-L255`; the live source span
  and hash are preserved under the private task folder. Verifier reads through live
  `Psarc.paths()`/`Psarc.read()` and checks archive hashes/sizes, summaries, exact manifest rows,
  locators, member length/hash, and archive identity before/after reading. Root and worker outputs
  match byte-for-byte.
- **Files changed:** private verifier/report/logs only; shared files `HANDOFF.md`, `docs/TASKS.md`,
  `docs/EVIDENCE.md`, `docs/ASSETS.md`, generated `docs/FOLDERS.md`, and this checkpoint. No source,
  reference, or game-input files changed.
- **Pre-commit publication checks:** `python3 scripts/project_index.py` exited `0` and reported
  `4,137 folders indexed; written`; `git diff --check` and `git diff --cached --check` passed;
  staged path audit accepted 7 allowlisted documentation paths and full audit accepted 595 paths.
  `git fetch origin main` exited `0`; pre-commit divergence was `0 0`. The preceding shared-doc
  commit was `ecd5938` (`docs: record app-main Ghidra result`), already pushed. Private artifacts
  remain excluded. Commit `ed3db01` (`docs: verify paired STS byte layout`) pushed successfully
  (`ecd5938..ed3db01`). Post-push `git status --short` was empty; HEAD and `origin/main` both resolve
  to `ed3db01348a20d6e33154068b4f05b1b49c1fac5`, divergence `0 0`.
- **Next exact action:** continue the independent raw-byte/PT_LOAD verification of
  `RE-02-APPMAIN-25A8D0-01`; after its result, update `docs/EVIDENCE.md`, `docs/TASKS.md`, and
  `HANDOFF.md`. G1 remains NOT PASSED.
