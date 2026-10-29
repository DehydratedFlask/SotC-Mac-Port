# E1009 AUD-01 SXDF music sample-header correlation result — 2026-10-29

- **Agent:** /root; task AUD-01-SXDF-MUSIC-SAMPLE-HEADER-CORRELATION-01.
- **Claim basis:** E1008 claim commit 87f39bcfbedb4b47fad6b2fe0e037145b5ede269, fetched and matched against live main before this result.
- **Status:** ARTIFACT_VERIFIED; no product gate is advanced.
- **Scope:** revalidate one local PSARC identity, resolve one exact member through Psarc.members(), decode it only in memory, verify its full hash, and inspect its first 48 bytes against the E1007 static candidate. No extraction, parser/codec implementation, audio-use, route, playback, or runtime claim.

## Verified identities and bytes

- Archive local/extracted/CUSA08809/pakN.psarc: 12,838,532,222 bytes; streamed SHA-256 559f7a3fa66bc652d36c5f1a885b351e4aeb17dadbba06a0006520bfd325be73, matching the corrected summary.
- Corrected exact path sound/shadow/music.sxd1 maps uniquely to TOC entry 27715, offset 569,314,592, first block 13,967, decoded size 9,872.
- Decoded member SHA-256 b925ca03cb6dddf8e9d3448c24a2e2eeb0ac595067cd3bbf25087ec3e6692a94, matching the corrected manifest.
- Pinned E1007 ELF SHA-256 d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3. Its static 48-byte context at file offset 0x12be74a is FORM / VAGp / SXDF / fmt / smpl / fact / data / cue / adtl / INF.
- The member's first 48 bytes are 535844460400bb2d90260000201d460a711e4c81e3009f4893a0a0c501928b776d757369630000000000000000000000. The first four bytes SXDF exactly match the token at byte offset 10 of the ELF context. The complete 48-byte strings differ.

## Attempts and limits

1. The full-stream verifier checked archive SHA, ELF SHA, exact path/TOC metadata, decoded member size, and decoded member SHA, then exited 1 because it asserted equality of the entire 48-byte context and member prefix. The mismatch is preserved in the log; it is a rejected byte-equality expectation, not a semantic result.
2. A focused verifier re-read and re-hashed the full member in memory, confirmed the unique exact path and locator, and checked the four-byte token. It exited 0 with four_byte_tag_match=True and whole_48_byte_match=False.

Attempt count: 2; one rejected whole-48-byte equality expectation and one passed four-byte-tag check; 0 failed semantic hypotheses. The matching tag does not establish that the observed code reads this member or implements its format. The audio codec, framing, parser, engine route, and playback remain unknown. G2/G5 and the overall port remain incomplete.

## Reproduction

- Attempt 1: python3 -I -u local/validation/AUD-01-SXDF-MUSIC-SAMPLE-HEADER-CORRELATION-01/analysis/attempt-01/verify_music_member.py; full output and progress in the adjacent member-correlation.log.
- Attempt 2: python3 -I -u local/validation/AUD-01-SXDF-MUSIC-SAMPLE-HEADER-CORRELATION-01/analysis/attempt-02/verify_tag_prefix.py; output in tag-prefix.log and summary in REPORT.md.
- No dependency was installed. No asset was extracted or written; only hashes and the first 48 decoded bytes were emitted.

## Repository verification

- cmake -S native -B local/build/orch-r22: exit 0.
- cmake --build local/build/orch-r22 -j 12: exit 0.
- ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure: PASSED 487/487.
- SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh: PASSED 572 / 0 skipped / 0 failed.
- trail build --check: graph check OK; 10,787 meaning-tier nodes pending.
- python3 -I scripts/audit_publish_paths.py --all: PASSED 3,397 paths; staged python3 -I scripts/audit_publish_paths.py: PASSED 7/7 paths.
- python3 scripts/project_index.py and python3 scripts/project_index.py --check: 44,213 folders indexed and current.
- Configure/build/test/checker/index/path-audit logs are under local/validation/AUD-01-SXDF-MUSIC-SAMPLE-HEADER-CORRELATION-01/result-publication/attempt-01/.
- git diff --cached --check and precommit python3 -I scripts/check_commit_attribution.py passed; configured author and committer are Johnny Wang <dehydratedflask@users.noreply.github.com>. Postcommit attribution, commit whitespace, push, and fetched-ref verification remain.
