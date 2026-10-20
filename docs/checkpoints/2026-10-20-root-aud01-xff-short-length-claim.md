# Checkpoint: AUD-01-XFF-DECLARED-LENGTH-SHORTER-01 — claim

- Time / timezone: 2026-10-20 05:14 CDT
- Agent and integrator: `/root`; root owns shared docs and Git
- Task / gate / status: `AUD-01-XFF-DECLARED-LENGTH-SHORTER-01`; claimed, `ACTIVE`; no game gate implication
- Prerequisite evidence / input and source hashes: `AUD-01-XFF-01` is `FIXTURE_VERIFIED`; parser SHA-256 `bdad7d192b0ecdb929daf5147a139e7545fd5c546f01ed4ffb4005f2cea9f9c0`; test SHA-256 `cffef962db441ef64c8435b53205b9106e5fb3af6360d14e26a751df5a1fcb4c`
- Owned / changed files: worker owns only `scripts/test_audio_bank_header.py`; root owns `HANDOFF.md`, `docs/TASKS.md`, and this checkpoint
- Hypothesis and expected result: a valid `xff\0` member whose u32le declared length at offset `0x14` is smaller than the full member buffer is rejected with `AudioBankFormatError`; no parser change expected
- Exact command, CWD and nonsecret environment: from `/Volumes/Extra/SoC Mac Native`, `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_bank_header.py`
- Exit / observed result: baseline exit 0; 6 existing tests pass
- Log, capture and REA evidence paths/IDs: `local/checkpoints/root-2026-10-20-xff-short-length-claim-baseline.log`, SHA-256 `dcd2147edfda2291b456dfb6d9f3021f0fed07760d5a1c8869fc5446f30e28fe`
- Observation versus inference / coverage limitations: Graft source evidence shows the parser requires declared size equal to member length and current synthetic tests cover an oversized word and wrong byte order. The missing smaller-word case is a coverage inference; any passing test will establish only this synthetic rejection, not codec or playback behavior
- Cumulative failed tries and distinct hypotheses: 0; baseline is not an implementation try
- Remaining risks / blockers: audio format/codec, NGS2 behavior, runtime use and gameplay playback remain unknown; G1 NOT PASSED; G2 NOT RUN
- Publication checks: `python3 -I scripts/audit_publish_paths.py` exited 0 for 4 staged paths; log `local/checkpoints/root-2026-10-20-xff-short-length-claim-publish-audit-staged.log`, SHA-256 `bf0cd63a37385613169f23d20b945f577a08420aeac746f86fb2844bf160f313`; `python3 -I scripts/audit_publish_paths.py --all` exited 0 for 635 tracked paths; log `local/checkpoints/root-2026-10-20-xff-short-length-claim-publish-audit-all.log`, SHA-256 `1e5d538c25e2e8d8141a2c31cf2208fb2ff2abea74c7595e229a348ce5c16728`; `git fetch origin` showed 0/0 divergence
- Next exact action: commit and push this claim, then dispatch the one-file worker
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: HANDOFF and TASKS updated; no EVIDENCE or INSTALL_LOG fact changed; `python3 scripts/project_index.py` exited 0 with 4,166 folders (`local/checkpoints/root-2026-10-20-xff-short-length-claim-project-index.log`, SHA-256 `f5e40539e482e9841a3f5631ca3e0e53f76cea9afef85423aaf910c0a8ba55aa`); generated `docs/FOLDERS.md` lists 77 checkpoint files; `graft check` exited 0 at 1,256 nodes / 530 source files, optional deep meaning layer unbuilt (`local/checkpoints/root-2026-10-20-xff-short-length-claim-graft-check.log`, SHA-256 `b5b180ea9f5998a135db6348d61e80f98fe4dbda52710ac602a6515c1c9dcf16`); `git diff --check` passed
