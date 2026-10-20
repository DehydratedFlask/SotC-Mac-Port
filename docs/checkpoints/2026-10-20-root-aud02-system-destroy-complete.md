# AUD-02-NGS2-SYSDESTROY-01 — root replay checkpoint

- Time / timezone: 2026-10-20 02:48 CDT (America/Chicago).
- Agent / integrator: `/root`; root owns shared docs and publication.
- Task: `AUD-02-NGS2-SYSDESTROY-01`; parent `AUD-02`; status
  `STATIC_PATH_BYTE_VERIFIED`; failed targeted analysis/provider tries 0.
- Worker dossier: `local/re/evidence/aud02-ngs2-system-destroy-01/`; report SHA-256
  `6bc7575ad2bf75d7202c3c8d4b4d1e980129fa4a9f688ce90e65eefa31d81305`; verifier SHA-256
  `331f9a60ffcd57c8c46bdbe8fbeaca8754b864e54545d45af76f8fb35b7963ef`; checksum manifest
  SHA-256 `9f4a801bd2e38c81cd1cf4e65d86f05ff050441ba05ce80191cfe8b9774545e2`.
- Exact root replay commands:
  - `shasum -a 256 -c SHA256SUMS.txt` from CWD
    `local/re/evidence/aud02-ngs2-system-destroy-01`; exit 0, all 68 entries passed. Root log
    `local/re/evidence/aud02-ngs2-system-destroy-01-root-checksums.log`, SHA-256
    `a3362e9f866def4c25a223ebff3def3d614aada9c2d0189907abc2d4cd8c00a0`.
  - `python3 -I local/re/evidence/aud02-ngs2-system-destroy-01/verify.py > local/re/evidence/aud02-ngs2-system-destroy-01-root-verification.json 2> local/re/evidence/aud02-ngs2-system-destroy-01-root-verification.stderr.log`; exit 0, JSON status `PASS`, stderr empty. Root JSON SHA-256
    `fba2644bad55b233df937f8b9eb5bdf9738617bf99426d6f9b26a0da373e5223`.
  - `shasum -a 256 local/re/eboot.elf local/re/imports.json`; exact hashes matched the report
    and verifier. Root input-hash log SHA-256
    `7e4c64e39642707ef91b0654aad2e4e9399fb7eedf7f4fdbdd6f1054c23ba2e8`.
- Observation: the verifier reads the pinned ELF and manifest, repository parser outputs, and raw
  REA evidence. It verifies symbol 241 / NID `u-WrYDaJA3k`, the JUMP_SLOT at `0x1601010`, thunk
  bytes `ff25cafb6600` at `0xf91440`, and direct call bytes `e8698a6400` at `0x9489d2` in
  `FUN_009485a0`. The selected caller window `[0x9489c4,0x9489d7)` is 19 bytes with SHA-256
  `54c337645832313c74ad6a09ad772032eb1e2c6efb2da11a4c84f5de96e49014`. REA Evidence IDs:
  `ev_37ba3af33cc684de727dfb407d145e99b12d646a8d3f3fe219a31b2c07fbd6d1`,
  `ev_8d25dae90351719a7d1821323e698a4a8ffa881de1b90637fd50952e1d274320`, and
  `ev_3c328e924109833a20ed5f3ccb54b5395b4aa0561ab1974c4e77ff97a457385f`.
- The thunk query also reports an incoming call at `0x94a055` from `FUN_00949fe0`; it was not
  followed. Ghidra retains an adjacent computed-jump edge to `0xf91446` and slot indirection at
  `0x1601010`; instruction bytes only establish the observed RIP-relative jump. Reachability,
  runtime binding, indirect-target coverage, ABI, NGS2 semantics, audio, and game execution remain
  unknown.
- A first root hash command omitted shell quoting for an absolute path containing spaces and
  failed before hashing; the corrected repo-relative command above succeeded. An initial verifier
  redirection also omitted quoting and wrote its successful JSON to `/Volumes/Extra/SoC`; the file
  hash matched the expected `fba2644...` output, the root-created artifact was removed, and the
  verifier was rerun to the intended repo-relative output path. No input or user file was altered;
  these were shell bookkeeping errors, not analysis/provider failures. The exact corrected
  commands are in `local/re/evidence/aud02-ngs2-system-destroy-01-root-replay.command.txt`.
- Shared checkpoint-batch checks from the repository root:
  - `python3 scripts/project_index.py` exited 0, indexed 4,148 folders, and updated
    `docs/FOLDERS.md` to count 48 checkpoint files. Log
    `local/checkpoints/2026-10-20-root-aud02-three-callpaths-project-index.log`, SHA-256
    `9e7e2b6742a42ea454a0d23092e76314e2cf4a229c8e92231fedd41ce1da0ce8`.
  - `graft check` exited 0 (wiring graph in sync; deep layer unbuilt, 1,225 nodes pending); log
    `local/checkpoints/2026-10-20-root-aud02-three-callpaths-graft-check.log`, SHA-256
    `f6496628c6608d20c699a233b45e90bafc20f298616a0926f2bf4d2b576a8fdf`.
  - `git diff --check` and `git diff --cached --check` exited 0; both logs are empty with
    SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
  - `python3 -I scripts/audit_publish_paths.py` exited 0 and accepted seven allowlisted staged
    documentation paths; log SHA-256
    `17ae65a81b8ca5811b66eb5578226122a28d4f40b9d46f0a330d10a118d95102`. The `--all` audit
    exited 0 and accepted 604 tracked paths; log SHA-256
    `d572f74e033883821faee4cdc940e5abc9bb7ce349c8e5fffbd9418b4a861fbe`.
  - `git fetch origin` exited 0; `git rev-list --left-right --count HEAD...origin/main` returned
    `0 0`. Fetch output is empty; publish commands are in
    `local/checkpoints/2026-10-20-root-aud02-three-paths-publish-commands.txt`.
- Shared files changed: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, this checkpoint.
  `INSTALL_LOG.md` unchanged. Next exact action: claim the next two bounded unclaimed NGS2 import
  callpaths after checking the manifest and Stage 3 prerequisites. G1 remains NOT PASSED.
