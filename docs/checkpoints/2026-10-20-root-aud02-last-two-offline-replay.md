# AUD-02 — RackCreateWithAllocator root replay and final two offline packets

- Time / timezone: 2026-10-20 03:30 CDT (America/Chicago).
- Agent / integrator: `/root`; task `AUD-02`; shared-doc and REA/Ghidra slot owner.
- Gate / status: G1 NOT PASSED; G2 NOT RUN. Parent AUD-02 remains ACTIVE. Seven bounded static
  import-to-call paths are root-verified. `AUD-02-NGS2-RACKCREATE-ALLOC-01` is
  `STATIC_PATH_BYTE_VERIFIED`; `AUD-02-NGS2-RACKDESTROY-01` and
  `AUD-02-NGS2-VOICE-GETSTATEFLAGS-01` remain ACTIVE, offline prep only. Failed targeted
  analysis/provider tries are 0 for all three; a worker checker expectation correction and root
  command/CWD mistakes below are bookkeeping issues, not failed hypotheses.
- Shared files changed: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, generated
  `docs/FOLDERS.md`, and this checkpoint. All evidence/log outputs and game inputs remain private
  under `local/`. No source, reference clone, installation or public binary changed.

## RackCreateWithAllocator static path

Inputs are the exact reconstructed ELF SHA-256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3` and imports manifest
SHA-256 `78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744`. The single authorized
REA `function` operation at `0x9493f8` completed with exit 0 after 362.305 seconds (REA envelope
361,706 ms). REA CLI 6.7.0 / Ghidra 12.1.4 used the reassessed 8G heap / 1,200,000 ms startup plan
and profile digest `769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa`. Evidence ID:
`ev_0a94ff48112c32e17a86d8725de379b29930eff6a541576a14d47938a88b49cc`.

The worker's final private report SHA-256 is
`d28ba689df8f740d76586500ac244e038306d1129a2f06feac6a4139a57af8b3`; its 93-entry
`SHA256SUMS.txt` is `033e6c0a27ba842d4303f086566f5a855d2c35b685da0be67e4840a466542884`. Root ran
`shasum -a 256 -c SHA256SUMS.txt` from the dossier directory: exit 0, 93/93 entries OK; root log
`local/checkpoints/2026-10-20-root-aud02-rackcreate-allocator-manifest-replay.log`, SHA-256
`6810e569c250e35d3cda11bf8cf0c83accda9b6f459f3cc3c871b4e6aa40564e`. Root then ran
`python3 -I verify_path.py` from that directory: exit 0, status `STATIC_PATH_BYTE_VERIFIED`,
13 exact instruction-byte records; root JSON
`local/checkpoints/2026-10-20-root-aud02-rackcreate-allocator-verifier-replay.json`, SHA-256
`264c8f503b842ac16a4dd17b781056b5cf9a1dc47f114d9bd18565b1faa133d4`, stderr empty.

Observation: slot `0x1600fe8` (symbol 236 / NID `U546k6orxQo`) reaches thunk candidate `0xf913f0`
and byte-checked direct call `0x9493f8` in `FUN_009485a0`. REA reports four direct unconditional
calls from that function to the thunk at `0x9493f8`, `0x9494a8`, `0x949581`, and `0x94965f`; the
verifier checks the observed direct edges and 13 exact instruction ranges. Raw function output SHA
`7278fc647b30789de7e64316df3b4a012f479d12275feff77f90188b1f94249b`; post-query snapshot SHA
`56853bb2d0ead2b45e52b88a2095f976765437395284a047e4d36e49e7495a96`. `/usr/bin/time -l` reported
maximum resident size 4,957,175,808 bytes; captured stderr SHA-256
`7bd11cb3918691854a52b962d2651b2bc555e743eeb1135ff53a4f98c964d13b`.

Register and stack values around the selected instruction are recorded without ABI or parameter
labels. REA's `unresolved_calls=[]` is limited by its stated inability to represent targetless
computed/indirect flows as resolved references. The static path does not establish runtime slot
binding, reachability, ABI, argument meaning, NGS2 contract, audio behavior or game execution. A
checker expectation mismatch (`function` vs. the returned `analyze_function`) was corrected before
the passing verifier; failed targeted analysis/provider tries remain 0.

## RackDestroy offline packet root replay

The final private checkpoint SHA-256 is
`1149453e1a88a96cf2c16d974beab9c57fc6a913715786e9fa442554536b638c`; its 49-entry manifest SHA
is `cae2c1b9e7d8e1e86055f7c97934915c2d4a39795de129749d4c5dc1e394dbe3`. From repo root, root ran
`python3 -I local/re/evidence/aud02-ngs2-rackdestroy-01/verify_checksums.py` (exit 0, `PASS 49
dossier file checksums`) and `python3 -I local/re/evidence/aud02-ngs2-rackdestroy-01/verify_offline.py`
(exit 0, `PARTIAL_PRE_REA`). Root outputs are
`local/checkpoints/2026-10-20-root-aud02-rackdestroy-checksum-replay.json` (SHA-256
`974b37dda5690c49831e7f9253434ed55e415618d83901fe83c45e69b3697418`) and
`local/checkpoints/2026-10-20-root-aud02-rackdestroy-offline-verify.json` (SHA-256
`4fc84d722f7746e7ffa3cbb28ee768ead697e3913cac48c1d31a003cc740f9d5`); both stderr files are
empty.

Observation: symbol 237 / NID `lCqD7oycmIM` maps to slot `0x1600ff0`; the raw relocation bytes
and parser agree with candidate thunk `0xf91400`. Ten raw E8 byte patterns target it. Bounded
objdump slices decode four as `call` and six E8 bytes as part of F6 instructions, with requested
section-range warnings. Inference withheld: no caller/function/path is accepted; no REA/Ghidra
operation occurred, and instruction boundaries, reachability, runtime binding, ABI, NGS2 meaning
and audio behavior remain unknown.

The first root verifier invocation was `python3 -I verify_offline.py` from the dossier directory;
it exited 1 with `FileNotFoundError: [Errno 2] No such file or directory` for
`/Volumes/Extra/SoC Mac Native/local/re/evidence/aud02-ngs2-rackdestroy-01/local/re/eboot.elf`.
The script resolved the repo-relative input from the wrong working directory. Re-running the
documented command from repo root passed. This was a working-directory mistake, not a failed
analysis hypothesis or provider attempt.

## VoiceGetStateFlags offline packet root replay

The worker checkpoint SHA-256 is
`d3c7b495a22048ed7ea7bb953056fa5215396ca6939fe111a18126c5e86bfb3b`; its 49-entry manifest SHA
is `475be24ca83d961e28a65ab82084f98fabbdb71d972cf2b29119f8e496ca92ec`. Root ran
`shasum -a 256 -c SHA256SUMS` from the dossier directory (exit 0, 49/49 entries OK; log
`local/checkpoints/2026-10-20-root-aud02-voiceflags-manifest-replay.log`, SHA-256
`e588d92f49499a2e81e77a309c561947bf74d0fb5510b4abdbc8ccc52a687e74`). From repo root, root ran
the exact command:

```sh
python3 -I local/re/evidence/aud02-ngs2-voice-getstateflags-01/check_offline.py \
  --elf local/re/eboot.elf --manifest local/re/imports.json \
  --parser-base local/re/evidence/aud02-ngs2-voice-getstateflags-01/imports.reparsed.json \
  --parser-thunk local/re/evidence/aud02-ngs2-voice-getstateflags-01/imports.with-thunks.json \
  --out local/checkpoints/2026-10-20-root-aud02-voiceflags-offline-verify.json
```

Exit 0, `PASS_OFFLINE_CHECKS`. Root JSON SHA-256 is
`7eb8c944f62bc1a680b7fd3f6628075e4858eced9f8dc90e71c58bb3a82e9431`; stderr is empty.

Observation: symbol 245 / NID `rEh728kXk3w` maps to slot `0x1601030`; the parser and raw scan
agree on candidate thunk `0xf91480`. One raw E8 pattern at `0x9534cb` targets it. Inference
withheld: no instruction boundary or caller/function attribution is established, and no REA/Ghidra
call occurred. Runtime binding/use, reachability, ABI, NGS2 meaning and audio behavior remain
unknown.

Two initial root CLI attempts were command-shape errors, not task failures: the guessed
`verify_checksums.py` file does not exist in this dossier, and `check_offline.py` requires `--elf`
and `--manifest`. The missing-script command exited 2; stderr is retained at
`local/checkpoints/2026-10-20-root-aud02-voiceflags-checksum-replay.stderr.log` (SHA-256
`32a7885099f43a5e799da2279fd3067069bc43146df3ddf79130db836c131882`). The no-argument command
also exited 2 and returned `usage: check_offline.py [-h] --elf ELF --manifest MANIFEST
[--parser-base PARSER_BASE] [--parser-thunk PARSER_THUNK] [--out OUT]` followed by
`error: the following arguments are required: --elf, --manifest`; that emitted usage text was
overwritten by the successful rerun. The recipe command above passed. No failed analysis hypotheses
or provider attempts were added.

## Shared checkpoint result and next action

Root ran `python3 scripts/project_index.py` after creating this checkpoint: exit 0, 4,153 folders
indexed, and generated `docs/FOLDERS.md` now records 55 checkpoint files. Its private log is
`local/checkpoints/2026-10-20-root-aud02-last-two-offline-replay-project-index.log`, SHA-256
`918116aa9871335a97c4e0af6050770e434086ef9c71156dc6593e9d99258549`. `graft check` exited 0 with
the configured wiring graph in sync. The deep layer remains unbuilt with 1,225 nodes pending; this
is not semantic coverage. Its private log is
`local/checkpoints/2026-10-20-root-aud02-last-two-offline-replay-graft-check.log`, SHA-256
`f6496628c6608d20c699a233b45e90bafc20f298616a0926f2bf4d2b576a8fdf`.

Remaining risks: RackDestroy and VoiceGetStateFlags remain offline-prep only. RackQuery and
VoiceHandle remain serialized behind the single-provider rule; VoiceHandle remains paused after
its earlier startup timeout. No runtime, audio, ABI or game-start claim has been made.

Next exact action: publish this checkpoint with staged/full path audits and a fresh remote-divergence
check. `git diff --check` and `git diff --cached --check` exited 0; combined empty log SHA-256
`e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`, status file SHA-256
`9a271f2a916b0b6ee6cecb2426f0b3206ef074578be55d9bc94f6f3fe3ab86aa`. The staged path audit exited
0 and accepted five allowlisted docs paths; log SHA-256
`324d5902d00aec96d2dd12196333713d77bb5b1e77d4418dbb89e9aa3bdac542`. The full audit exited 0 and
accepted 611 tracked paths; log SHA-256
`304365a9895be92eb2e418a2a704263e74e89a0c58e0dd0dd724c47ef3b1f1ed`. After `git fetch origin`,
`git rev-list --left-right --count HEAD...origin/main` reported `0 0`. `git status --short` listed
only the five intended staged docs paths plus the pre-existing untracked `tools/graft/` cache, which
remains unstaged.

Next exact action: commit and push the five audited docs paths, then authorize exactly one RackDestroy
slot-xrefs query for `0x1600ff0`. Root will replay the response/snapshot and only then choose a
bounded follow-up. Keep all other provider work idle.

## Graft savings record

Recorded successful Graft output estimates for this orchestration turn: root 177,653 tokens over two
calls; RackCreate prep 174,445 over two calls; RackDestroy prep 2,727 over one call; VoiceGetStateFlags
prep 21,915 over four calls. Total: approximately 376,740 tokens over nine calls. RackQuery's earlier
Graft estimate was not retained verbatim and is excluded. No value was inferred from a failed query.
