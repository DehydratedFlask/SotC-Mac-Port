# AUD-01-XFF-01 paired XFF sound-bank structure claim

- Date/time: 2026-10-19 23:09 CDT (America/Chicago)
- Integrator: `/root`; implementation owner: `/root/audio_asset_survey`
- Task ID: AUD-01-XFF-01, characterize one `.hd`/`.bd` bank pair with a bounded structural reader
- Files changed in claim: `HANDOFF.md`, `docs/TASKS.md`, this checkpoint. No implementation code
  has changed for this slice.
- Prior failed tries: 0.

## Evidence and boundaries

The implementation agent's read-only scan verified the target pair through `Psarc.read()` against
the private asset manifest:

| Item | Archive | Entry | Offset | First block | Decoded bytes | SHA-256 |
|---|---|---:|---:|---:|---:|---|
| `cp3shadow/sound/se/com_sys.hd` | `pak2.psarc` | 1311 | 7,884,070 | 825 | 1,908 | `39ebb27c1d469853e12ffc439b17e8641d6258f9cc0f1029303c92bb9683ffe3` |
| `cp3shadow/sound/se/com_sys.bd` | `pak2.psarc` | 1310 | 6,989,994 | 664 | 3,672 | `137e64c118ada84b4dd9bd73bf9e4b7e9e1ef061fa2574dc304d5f7cdd1ebd81` |

The archived input is 16,557,548 bytes with SHA-256
`9c655de637baba5f2c35250a2a7024f405f2f95c19fb4739ff084fa95cbdf0b5`. Both decoded members begin
`78 66 66 00 00 00 00 00 00 00 00 00 01 00 00 00`; interpreting the first four bytes as `xff\0`
is direct ASCII observation. Other field meanings are open.

Graft-first navigation identified `Psarc.read` in `scripts/psarc.py`, including block truncation,
decompression and decoded-size checks. Command:
`graft ask "What existing PSARC APIs can safely read a named sound-bank member and what bounds checks are already established?" --source --in scripts/`.
The source graph is current; the optional deep meaning layer is not built.

## Bounded implementation and acceptance

Owned implementation paths:

- `scripts/audio_bank_header.py`
- `scripts/test_audio_bank_header.py`

First compare the pair and any equivalent pair found in the same existing manifest. Implement only
fields supported by repeated byte layout and bounded spans; preserve unknown field labels and
codec=`UNKNOWN`. Do not modify game files or write decoded content into public paths. The fixture
must cover a valid header, bad magic, truncation and any validated span/length errors.

Exact checks:

1. `python3 -I scripts/test_audio_bank_header.py`
2. `python3 -I scripts/audio_bank_header.py --archive local/extracted/CUSA08809/pak2.psarc --member cp3shadow/sound/se/com_sys.hd --member cp3shadow/sound/se/com_sys.bd`

The live command must report the manifest-matched archive/member identities and supported byte
structure only. A parser/fixture pass does not establish NGS2 semantics, codec, gameplay use, or
playback.

## Cross-check checkpoint — XFF length field observed

- Date/time: 2026-10-19 23:12 CDT; agent `/root/audio_asset_survey`; task AUD-01-XFF-01.
- Private files: `local/validation/20261019-231219-audio-bank-header-crosscheck/pair_crosscheck.py`,
  `pair_crosscheck.command.txt`, and `pair_crosscheck.log`. No public implementation source had
  changed at this checkpoint.
- Exact command: `PYTHONDONTWRITEBYTECODE=1 python3 -I local/validation/20261019-231219-audio-bank-header-crosscheck/pair_crosscheck.py`;
  exit 0. Invocation and output are preserved in the run directory.
- Observation: 25 pair groups / 50 live decoded members matched manifest paths, entry locators and
  hashes. Of 22 XFF pairs, the u32 little-endian word at offset `0x14` equals decoded size in
  44/44 members, with zero mismatches. First-16-byte prefixes have two observed variants; bytes
  4..19 remain opaque. An exploratory invocation first had a Python one-line syntax typo; this
  corrected reproducible command passed and was not a failed implementation hypothesis.
- Remaining risk: only XFF magic and the repeated length relation are identified. Codec and other
  field meanings remain unknown; no NGS2, gameplay or playback claim follows.
- Next exact action: implement and test a parser using only the XFF magic and verified length word,
  then run the claimed live pair check. Task ACTIVE; implementation attempts remain 0.

## Status

FIXTURE_VERIFIED for this bounded slice, implementation try count 0. The pair has an observed
decoded-length word, but no header/data index relation beyond that has been validated. Exact format,
codec, NGS2 use and playback remain unknown.

## Implementation checkpoint — 2026-10-19 23:15 CDT

- Agent ID: `/root` (integrator); task ID: AUD-01-XFF-01; implementation owner:
  `/root/audio_asset_survey`.
- Files changed by the agent: `scripts/audio_bank_header.py`,
  `scripts/test_audio_bank_header.py`. Integrator changed `HANDOFF.md`, `docs/TASKS.md`,
  `docs/ASSETS.md`, `docs/EVIDENCE.md`, `docs/FOLDERS.md`, and this checkpoint.
- Exact acceptance commands:
  `python3 -I scripts/test_audio_bank_header.py` (exit 0, 6/6; log
  `local/validation/20261019-2315-root-aud01-xff-verification/test_audio_bank_header.log`);
  `python3 -I scripts/audio_bank_header.py --archive local/extracted/CUSA08809/pak2.psarc --member cp3shadow/sound/se/com_sys.hd --member cp3shadow/sound/se/com_sys.bd`
  (exit 0; live output log
  `local/validation/20261019-2315-root-aud01-xff-verification/audio_bank_header.log`).
- Additional checks: `graft grep "parse_xff_member"` found the parser and its tests; `graft check`
  exited 0, wiring graph in sync, deep meaning layer unbuilt. `python3 scripts/project_index.py`
  refreshed 4,126 folders; `python3 scripts/project_index.py --check` then passed. Initial index
  check exited 1 after the private verification directory was created; this was corrected by the
  refresh and was not an implementation hypothesis failure. `git diff --check` and
  `git diff --cached --check` passed. Logs are under
  `local/validation/20261019-2315-root-aud01-xff-verification/`.
- Observation: the pair command matched the archive summary and both private member identity rows.
  The report contains observed magic, opaque bytes 4..19, and the u32le@0x14 length equality only.
  No runtime decoder, gameplay use, or playback behavior was exercised.
- Remaining risk: codec, other field meanings, NGS2 contract and runtime consumption are unknown.
  Task status FIXTURE_VERIFIED; failed implementation tries: 0.
- Next exact action: stage only allowlisted source/docs, run both publication-path audits, check
  remote divergence, then commit and push `origin main`. After publication, claim the independent
  synthetic ELF parser fixture task before delegating it.

## Claim publication — 2026-10-19 23:10 CDT

- Commit `b17ffe2` (`docs: claim XFF sound bank structure slice`) was pushed from `8b10c09` to
  `origin main` before implementation began.
- `python3 scripts/project_index.py --check` reported 4,123 folders current; `git diff --check`
  passed; the staged path audit passed 5 files; the full tracked-path audit passed 567 files;
  `git diff --cached --check` passed; `git fetch origin main` showed no divergence.
- Root sent `/root/audio_asset_survey` the implementation assignment after the push. The worker
  owns only the two scripts listed above; the integrator owns all shared-document updates.

## Publication checkpoint — 2026-10-19 23:16 CDT

- Agent ID: `/root` (integrator); task ID: AUD-01-XFF-01; status FIXTURE_VERIFIED; failed
  implementation tries: 0.
- Files in the published batch: `scripts/audio_bank_header.py`,
  `scripts/test_audio_bank_header.py`, `HANDOFF.md`, `docs/ASSETS.md`, `docs/EVIDENCE.md`,
  `docs/FOLDERS.md`, `docs/TASKS.md`, and this checkpoint.
- Commit `7b2747b` (`feat: add bounded XFF bank header reader`) was pushed to `origin main`.
  The staged publish-path audit passed 8 files; `--all` passed 569 tracked paths; staged diff
  check passed; fetched remote showed no divergence before commit.
- Acceptance logs: six synthetic tests passed and the live `com_sys` pair matched archive summary
  plus both member manifest identities. Logs and exact commands remain under
  `local/validation/20261019-2315-root-aud01-xff-verification/`.
- Observation vs inference: only `xff\0`, opaque bytes 4..19, and the repeated u32le@0x14 decoded
  length relation are reported. Codec, NGS2 semantics, gameplay consumption and playback are not
  established.
- Next exact action: work from the newly published RE-02-DYNLINK-01 and
  CPU-01-CTOR-PREFIX-01 claims, with separate script/test and native constructor file ownership.
