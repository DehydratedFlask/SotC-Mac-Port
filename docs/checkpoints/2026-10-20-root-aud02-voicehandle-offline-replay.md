# AUD-02-NGS2-RACK-VOICEHANDLE-01 — independent offline root replay

- Time: 2026-10-20 03:57 CDT (America/Chicago).
- Agent / integrator: `/root`; worker `/root/aud02_voiceflags_offline` supplied the private replay.
- Status: `ACTIVE/PARTIAL`; failed analysis hypotheses 0; provider failures 1 (earlier 330-second xrefs timeout). No provider operation was run during this replay.
- Shared files: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, generated `docs/FOLDERS.md`, and this checkpoint. Root-review artifacts are private under `local/checkpoints/aud02-ngs2-rack-voicehandle-01-root-offline-replay/`; the worker dossier was read only.
- Gate: no caller/function path accepted; G1 NOT PASSED, G2 NOT RUN.

## Exact replay and results

From `local/checkpoints/aud02-ngs2-rack-voicehandle-01-root-offline-replay/`, root ran:

```sh
shasum -a 256 -c SHA256SUMS
```

Exit 0; all 77 entries verified. Manifest SHA-256:
`f630ce238cc68d41b7bb37763141a4f31cc54e5c51b8bbd5e5c6035741bf0650`. Worker report SHA-256:
`49b8e3603fbbd4f8d62b02ffaac38c71b1571c27de604f8b34c4d4faae40c818`.

From the repository root, root independently ran:

```sh
python3 -I local/checkpoints/aud02-ngs2-rack-voicehandle-01-root-offline-replay/byte_replay.py
```

Exit 0; output SHA-256 `1d66c6e20a9fc111f811697803bbb68ac53e2a42180abb3d7a1b10c57d3e7f32`; stderr empty. Root also reran the repository parser with and without `--thunk 0xf91410` into its own replay directory and compared each output byte-for-byte against the worker outputs; both `cmp` checks exited 0.

The inputs match the pinned task identity: ELF SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; import manifest SHA-256 `78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744`. The raw candidate thunk at VMA `0xf91410` contains `ff25e2fb6600`, which computes to slot `0x1600ff8` (symbol 238 / NID `MwmHz8pAdAo`). The independent scan reproduced all 104 raw `E8 rel32` byte patterns and matched the worker's candidate tuple set.

## Limits and next action

Windowed `objdump` starting at `0xf91410` renders an indirect jump through the slot; a second window starting at `0x9496a0` renders bytes at `0x9496a8` as a call to the thunk. Both outputs warn that no section overlaps the selected range and instead decode a PT_LOAD byte window. This corroborates only how those bytes decode from the chosen offsets. It does not prove instruction boundaries, caller/function attribution, reachability, complete xrefs, runtime binding, ABI, NGS2 meaning, or audio behavior.

The prior Ghidra slot-xrefs timeout at 330 seconds yielded no Evidence ID or raw provider response and remains the only provider failure for this task. The two early root inventory-checker corrections are documented in the private replay report; they were checker-shape defects, not failed analysis hypotheses or provider attempts. No retry is released. Next exact action: continue an independent AUD-02 probe under the single-provider rule and leave VoiceHandle paused unless root later explicitly releases a retry.

No source, reference, game input, install, or runtime state changed. `INSTALL_LOG.md` is unchanged.

Shared index/check at this checkpoint: `python3 scripts/project_index.py > local/checkpoints/2026-10-20-root-aud02-current-project-index.log 2>&1` exited 0 and indexed 4,155 folders; log SHA-256 `ab2bf733540c2cb65b9628ed7faaa135583422723ea50bb37ac03c50cf2f671d`. `graft check > local/checkpoints/2026-10-20-root-aud02-current-graft-check.log 2>&1` exited 0; the wiring graph is in sync, while the meaning tier remains unbuilt with 1,225 nodes pending; log SHA-256 `f6496628c6608d20c699a233b45e90bafc20f298616a0926f2bf4d2b576a8fdf`.

Shared publication checks: staged `python3 -I scripts/audit_publish_paths.py` exited 0 and accepted six paths; full `python3 -I scripts/audit_publish_paths.py --all` exited 0 and accepted 614 tracked paths. Logs: `local/checkpoints/2026-10-20-root-aud02-publish-staged.log` (SHA-256 `baa1f2828be4b156834d10ce1ce7eefa2b6f4daf8362bbe1862c0433fb3be0a0`) and `local/checkpoints/2026-10-20-root-aud02-publish-all.log` (SHA-256 `7a38171ea3a5f7deb4c720c6c129d836b7576faebee8a6ece49b92a2b233ac45`). Both diff checks exited 0 with empty logs (SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). `git fetch origin` exited 0; `git rev-list --left-right --count HEAD...origin/main` reported `0 0`.
