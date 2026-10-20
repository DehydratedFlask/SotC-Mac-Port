# AUD-02-NGS2-RACKCREATE-ALLOC-01 — slot-xrefs provider result and root replay

- Time / timezone: 2026-10-20 03:14 CDT (America/Chicago).
- Agent / integrator: `/root`; worker `/root/aud02_rackcreate_allocator` owns the private dossier.
- Task / gate / status: `AUD-02-NGS2-RACKCREATE-ALLOC-01`, parent `AUD-02`; `ACTIVE`, G2 NOT RUN.
  Slot-xrefs completed; no call path has been accepted. Failed targeted analysis/provider tries: 0.
- Prerequisite identity: `local/re/eboot.elf` SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; import manifest SHA-256
  `78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744`.
- Shared files changed: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, generated
  `docs/FOLDERS.md`, and this checkpoint. Worker artifacts and root replay logs remain private under
  `local/re/evidence/`; no source, pinned reference, executable, installation, or game input changed.

## Provider result

The worker ran one REA CLI `xrefs` operation for slot `0x1600fe8`, with Ghidra 12.1.4, REA 6.7.0,
headless heap `8G`, startup allowance `1200000 ms`, and profile digest
`769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa`. The exact captured argv and
environment are in `local/re/evidence/aud02-ngs2-rackcreate-allocator-01/rea-slot-xrefs.command.json`.
The command was:

```sh
env GHIDRA_INSTALL_DIR=/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC \
  JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home \
  GHIDRA_HEADLESS_MAXMEM=8G REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000 \
  /Users/dehydratedflask/.npm/_npx/a1722018f28f9df2/node_modules/.bin/rea xrefs \
  '/Volumes/Extra/SoC Mac Native/local/re/eboot.elf' 0x1600fe8 --provider ghidra \
  --snapshot '/Volumes/Extra/SoC Mac Native/local/re/evidence/aud02-ngs2-rackcreate-allocator-01/rea-snapshot.json' \
  --format json --full-output
```

From repository root, the worker capture wrapper exited 0 after 356.056 seconds; the REA envelope
reports `355605 ms`. Raw stdout SHA-256 is
`b959853f17d35367971e56b4ad0931f6e13c0622651ce42581de0574da6c60dc`; stderr is empty
(`e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). Snapshot SHA-256 is
`fcfcb491bc807975795e40928ccc8d9b038929cbb80c36949e3ece69fc0b667b`. REA Evidence ID is
`ev_35cf538b6a13e81f4049ce4be968e5f36a6d8dd8f4f786094ca82802cf713bc4`; both raw and normalized
results contain only `0xf913f0` for the exact pinned ELF and slot. The address-only projection
explicitly omits reference kinds and has `relationship_coverage: null`.

The worker's RSS sampler captured 117 samples over the final 117.5 seconds, from
`2026-10-20T08:08:43Z` through `08:10:40Z`; sampled maximum was `4,459,056 KiB` at
`08:10:39.883972Z`. Sampling began after the request was underway, so this is not a full-run peak.
The raw TSV SHA-256 is
`9fca97edfcd9795a11ceb439a06aae4d48b5d46bc70baa521ea43c191a1b2301`. No separate Ghidra/script
log was found under the checked temp/cache roots after CLI close; the REA temporary project is
ephemeral and was deleted on close.

## Independent root replay

Root ran the response/snapshot checker through the worker's capture wrapper:

```sh
python3 -I local/re/evidence/aud02-ngs2-rackcreate-allocator-01/capture.py \
  root-xrefs-verify -- python3 -I -c '<assert raw Evidence fields, input digest, result, and snapshot record binding>'
```

The saved argv is in `root-xrefs-verify.command.json`; CWD was the repository root. Exit 0 in
0.021 seconds; JSON result is `root_replay=PASS`, `snapshot_target=PASS`, provider `Ghidra 12.1.4`,
and raw result `["0xf913f0"]`. Root verifier stdout SHA-256 is
`7dad93ca9ca0866864964ddafdfd525a172f548a0bd35ba74769bd8f4e0d1c77`; stderr is empty. The
snapshot stores Evidence under `evidence_bundle.records`; an earlier root checker guessed a
top-level `evidence` key and raised `KeyError`. That was a checker-shape assumption only; the
corrected replay passed and did not count as an analysis/provider failure.

Root independently ran `shasum -a 256 -c SHA256SUMS.txt` from the private dossier. Exit 0; all 78
manifest entries reported `OK`. Saved output
`local/checkpoints/2026-10-20-root-aud02-rackcreate-xrefs-manifest-check.log`, SHA-256
`ba1511356d2462bec61aa4167a0bfc9e48854514063d410bc82b6b6e2afa01ad`. Current private manifest
SHA-256 is `8258f3fba8ffc4768af48e8d46b3f2eb2b71f7184b1b5b00d5604e5c74fb8001`; worker report SHA-256
is `635220e1fb8de8ee19eef3702f6549ed096e62521cecd96b0b27b105819646a3`.

## Limits and next action

Observation: Ghidra returned address `0xf913f0` for slot `0x1600fe8`, and that matches the offline
`FF 25` thunk candidate. Inference withheld: because this xrefs projection does not include
reference kind, it does not yet establish the slot-to-thunk transfer, any caller, instruction
boundary, function attribution, reachability, ABI, runtime binding, NGS2 contract, or audio use.
The five offline `E8` byte patterns are candidates only.

The next exact action, after this checkpoint is published, is one REA `function` query at candidate
`0x9493f8` against the same ELF using the changed `8G` / `1200000 ms` plan. Capture raw output,
stderr, exit, duration, Evidence ID, snapshot, and RSS observations; do not try another candidate
without recording why. Keep the provider slot serialized. Then finish and root-replay the private
byte verifier and report. RackQuery and VoiceHandle remain pending/paused. G1 remains NOT PASSED;
G2 is NOT RUN.

The preceding checkpoint `e0d8a8f` (`docs: record NGS2 provider timeout checkpoint`) was pushed to
`origin/main` from `e3042f9` before this follow-up checkpoint. No implementation or game-runtime
claim is made here.
