# AUD-02 — RackDestroy xrefs root replay

- Time / timezone: 2026-10-20 03:44 CDT (America/Chicago).
- Agent / integrator: `/root`; task `AUD-02-NGS2-RACKDESTROY-01`; root owns shared docs and provider serialization.
- Task status: `ACTIVE/PARTIAL`; failed analysis hypotheses 0; failed provider operations 0. One REA/Ghidra xrefs operation completed successfully. Parent `AUD-02` remains ACTIVE. G1 NOT PASSED; G2 NOT RUN.
- Shared files changed: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, generated `docs/FOLDERS.md`, and this checkpoint. Private files updated: the worker checkpoint, captured xrefs output/stderr/run metadata, REA snapshot, refreshed manifest in `local/re/evidence/aud02-ngs2-rackdestroy-01/`, plus the root replay logs under `local/checkpoints/`. No source, reference, input, install, or runtime files changed.

## Operation and root replay

The pinned ELF is SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. The worker's single authorized REA CLI 6.7.0 / Ghidra 12.1.4 xrefs operation queried slot `0x1600ff0`, with `GHIDRA_HEADLESS_MAXMEM=8G` and `REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000`. It exited 0 after 355.508127 seconds and produced Evidence ID `ev_bf1d4dc66179c44215ce55bea3b845b93bb0952f7aeb86d2a2484453bd3826a8`. It returned only address `0xf91400`; reference kind and caller metadata were not provided. The after-query snapshot SHA-256 is `dbf6c71e670ca88c6278f1f07fee1564ec08a1195632973e166f7c55ad048f0c`.

Root replay commands and results:

```sh
cd 'local/re/evidence/aud02-ngs2-rackdestroy-01' && shasum -a 256 -c SHA256SUMS
python3 -I local/re/evidence/aud02-ngs2-rackdestroy-01/verify_rea_xrefs.py
```

The 58-entry dossier manifest replay exited 0 (all entries OK); manifest SHA-256 `e6a9951de28e9145ab652a53a42e66a31f0acb67d8a1c48331cd2697417f08ac`. Root's verifier exited 0 with `REA_XREFS_CAPTURED_NO_CALLER_KIND`; it checked the Evidence ID, provider/profile, ELF hash, returned address, snapshot, and exact thunk. Root output hashes: manifest replay log `8dbf530a07eaaf31b4e9c1bb0c6520513b2d7dce5c1585fbd5fcf94525b10221`; verifier JSON `0baf57e242e07bb01523f20e58d95051e0da72bfc85c5f53fcb851675ec63fe5`; verifier stderr is empty (SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). Logs are `local/checkpoints/2026-10-20-root-aud02-rackdestroy-xrefs-manifest-replay.log`, `...-xrefs-verifier.json`, and `...-xrefs-verifier.stderr.log`.

Worker capture hashes: `checkpoint-rea-xrefs.md` `b0063a87288437fc5ddcf9fb7ae365ab8bb915e45e44b1c72ba2d712ffda557b`; `xrefs-run.json` `ff398733963b9a1bcc66a80dce7acf4823b8c28b2c916f4cbb68e2016d7f53e3`; raw stdout `de09c79b6410ef7ab7d0aba1e60dacbe956783085c4187204b5481feba164842`; raw stderr `6ae1c9037b51312bde4ee56d63e6877ac5a9364a3fa001fb52aaa351d26bd586`; snapshot `dbf6c71e670ca88c6278f1f07fee1564ec08a1195632973e166f7c55ad048f0c`; refreshed 58-entry manifest `e6a9951de28e9145ab652a53a42e66a31f0acb67d8a1c48331cd2697417f08ac`. The exact command, timestamps, duration, and resource report are in the private worker checkpoint and `xrefs-run.json`. `/usr/bin/time -l` reported 355.50 real, 368.88 user, 60.91 sys, maximum resident size 4,699,373,568 bytes, 1,003,395 page reclaims, 3,022 page faults, zero swaps, and peak memory footprint 379,475,768 bytes.

## Observation, limits, and next action

The thunk at VMA `0xf91400`, file offset `0xf95400`, has bytes `ff25eafb6600`, which resolve to import slot `0x1600ff0`. This agrees with the offline relocation/parser candidate. The result is address-only and does not establish a game callsite, caller function, instruction boundary, argument/dataflow meaning, reachability, runtime binding, ABI, NGS2 contract, or audio behavior. The existing offline checker remains `PARTIAL_PRE_REA`; raw E8/call candidates remain unaccepted until attribution and byte verification.

Next exact action after this checkpoint is published: release exactly one serialized REA `function` query against the pinned ELF for the existing bounded call candidate at `0x94a01f`, using the same Ghidra profile and private RackDestroy snapshot. Keep all other provider work idle; root must replay the response and verify any candidate path against raw ELF bytes. If that query returns no useful caller/function evidence, record the result and choose the next step from captured evidence; do not infer a path.

Root ran `python3 scripts/project_index.py > local/checkpoints/2026-10-20-root-aud02-rackdestroy-xrefs-project-index.log 2>&1` (exit 0; 4,153 folders indexed); log SHA-256 `918116aa9871335a97c4e0af6050770e434086ef9c71156dc6593e9d99258549`. Root ran `graft check > local/checkpoints/2026-10-20-root-aud02-rackdestroy-xrefs-graft-check.log 2>&1` (exit 0; wiring graph in sync; deep meaning layer unbuilt, 1,225 nodes pending); log SHA-256 `f6496628c6608d20c699a233b45e90bafc20f298616a0926f2bf4d2b576a8fdf`. Before commit, run staged/full path audits, diff checks, and a fresh origin divergence check. Keep `tools/graft/` unstaged. `INSTALL_LOG.md` is unchanged. No audio, runtime, or G1 claim follows from this checkpoint.

Recorded Graft savings this orchestration turn: approximately 376,740 tokens across nine successful calls. RackQuery's earlier estimate was not retained verbatim and is excluded.
