# AUD-02-NGS2-RACKDESTROY-01 — function evidence root replay

- Time: 2026-10-20 03:57 CDT (America/Chicago).
- Agent / integrator: `/root`; worker `/root/aud02_rackquery_buffersize` supplied the serialized function result.
- Status: `STATIC_PATH_BYTE_VERIFIED` for one bounded static path; failed targeted analysis hypotheses 0; failed provider operations 0. Parent AUD-02 remains ACTIVE.
- Shared files: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, generated `docs/FOLDERS.md`, and this checkpoint. Root verifier and logs are private under `local/checkpoints/aud02-ngs2-rackdestroy-01-root-function-replay/`; provider output and snapshot remain private under `local/re/evidence/aud02-ngs2-rackdestroy-01/`.
- Gate: this is not a reachability, runtime, ABI, or audio claim. G1 NOT PASSED; G2 NOT RUN.

## Serialized REA result

After the preceding xrefs checkpoint was published, root released exactly one REA CLI 6.7.0 / Ghidra 12.1.4 `function` query at VMA `0x94a01f` against the pinned ELF. The exact invocation, environment, command timing, resource report, snapshot hashes, and raw capture are preserved in the worker's private `checkpoint-rea-function.md` (SHA-256 `79f516ad5541f66b36f04d2169f352ce4c63971b643940420a66270e1cdb9a89`). It exited 0 after 360.0274 seconds with Evidence ID `ev_2b6853d79e403ff02e771eeab42c0c247cf4848362fe344cb5eb0341d02df601`.

The raw function result SHA-256 is `b765436232d974a899d76eb92c69af25295b2d11c8babbeb43d6448e6c961cea`; run metadata SHA-256 is `505f0664ab4303892fb83c632a879b95afb52fccac50882bee041cdf0380a451`. Snapshot SHA-256 changed from `dbf6c71e670ca88c6278f1f07fee1564ec08a1195632973e166f7c55ad048f0c` to `935c0356ac87fd056720ffef51f0306a92f328355038a3ef0dac3955c4dd0caa`. The 66-file private dossier manifest SHA-256 is `ee7598e7ac57267e9ad98607f885f0616a01ad0ed10e4cf6a4533ae0b8abe4d3`.

## Independent root replay

From `local/re/evidence/aud02-ngs2-rackdestroy-01/`, root ran `shasum -a 256 -c SHA256SUMS`; exit 0, all 66 entries verified. Root replay log SHA-256: `ab85c566cc09d047d2e8d8cbcbc2cfa0c19282464a14de93877abb594b72458d`.

Root then ran from the repository root:

```sh
python3 -I local/checkpoints/aud02-ngs2-rackdestroy-01-root-function-replay/verify.py
```

Exit 0; status `ROOT_REPLAY_PASS_STATIC_PATH_BYTE_VERIFIED`; output SHA-256 `67646085b99a74286b6916b7a2062ba907df7b3f0c1b0ca026592910dff1f604`; stderr empty. This independent replay checks both input hashes, raw Evidence envelope and ID, provider/operation metadata, prior and current snapshots, Ghidra procedure/reference fields, and raw ELF bytes.

Observed static chain: VMA `0x94a01f`, file offset `0x94e01f`, bytes `e8dc736400` compute to thunk `0xf91400`. Thunk bytes at file offset `0xf95400` are `ff25eafb6600` and compute to import slot `0x1600ff0`. The raw relocation tuple at file offset `0x2de4510` identifies symbol 237 / `R_X86_64_JUMP_SLOT`. Ghidra attributes the call to `FUN_00949fe0` (body `0x949fe0`–`0x94a1d4`, 501 bytes), and classifies the outgoing edge as `UNCONDITIONAL_CALL`.

The local observations `MOV RDI, qword ptr [RAX + 0x78]` and `XOR ESI, ESI` immediately precede the call. They remain register and memory-expression observations without parameter meaning or ABI labels. The provider's empty `callers` and `unresolved_calls` arrays do not prove the absence of indirect or external callers.

## Remaining limits and next action

This verifies one static import-slot → thunk → direct-call path in the exact pinned ELF. It does not establish reachability, runtime slot binding, callee implementation, ABI, argument meaning, NGS2 semantics, audible output, or game behavior. No source or user game data changed; `INSTALL_LOG.md` is unchanged.

Next exact action after the checkpoint is published: release exactly one serialized REA `function` query for the existing VoiceGetStateFlags call candidate at `0x9534cb`, with a private snapshot and complete raw capture. Keep VoiceHandle paused after its timeout and keep all other provider work idle.

## Graft savings record

The recorded cumulative estimate for this orchestration remains approximately 379,269 tokens across 10 successful Graft calls. No dollar amount is inferred.

Shared index/check at this checkpoint: `python3 scripts/project_index.py > local/checkpoints/2026-10-20-root-aud02-current-project-index.log 2>&1` exited 0 and indexed 4,155 folders; log SHA-256 `ab2bf733540c2cb65b9628ed7faaa135583422723ea50bb37ac03c50cf2f671d`. `graft check > local/checkpoints/2026-10-20-root-aud02-current-graft-check.log 2>&1` exited 0; the wiring graph is in sync, while the meaning tier remains unbuilt with 1,225 nodes pending; log SHA-256 `f6496628c6608d20c699a233b45e90bafc20f298616a0926f2bf4d2b576a8fdf`.

Shared publication checks: staged `python3 -I scripts/audit_publish_paths.py` exited 0 and accepted six paths; full `python3 -I scripts/audit_publish_paths.py --all` exited 0 and accepted 614 tracked paths. Logs: `local/checkpoints/2026-10-20-root-aud02-publish-staged.log` (SHA-256 `baa1f2828be4b156834d10ce1ce7eefa2b6f4daf8362bbe1862c0433fb3be0a0`) and `local/checkpoints/2026-10-20-root-aud02-publish-all.log` (SHA-256 `7a38171ea3a5f7deb4c720c6c129d836b7576faebee8a6ece49b92a2b233ac45`). Both diff checks exited 0 with empty logs (SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). `git fetch origin` exited 0; `git rev-list --left-right --count HEAD...origin/main` reported `0 0`.
