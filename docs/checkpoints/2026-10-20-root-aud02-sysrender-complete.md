# AUD-02-NGS2-SYSRENDER-01 — root replay checkpoint

- Time / timezone: 2026-10-20 02:41 CDT (America/Chicago).
- Agent / integrator: `/root`; root owns shared docs and publication.
- Task: `AUD-02-NGS2-SYSRENDER-01`; parent `AUD-02`; status `STATIC_PATH_BYTE_VERIFIED`; failed targeted tries 0.
- Worker dossier: `local/re/evidence/aud02-ngs2-sysrender-01/`; report SHA-256
  `31e61f62bdd7c9ed19eea043f9394831b1dfb911c2facc64fc13229fc16a1a10`; verifier SHA-256
  `5b3841349d8e5fea91f342fea75926d967f29913c9369b0a52f04836006de57f`; checksum manifest
  SHA-256 `91445d2b87e2b22837791945c25c8b38e851185c534d7ce423cf5bc608ecc4a8`.
- Exact root commands:
  - `shasum -a 256 -c SHA256SUMS` from CWD
    `local/re/evidence/aud02-ngs2-sysrender-01`; exit 0, all 72 listed files passed. Root log
    `local/re/evidence/aud02-ngs2-sysrender-01-root-checksums.log`, SHA-256
    `9165511f7e5f710b1afba940bb029530d3ab82918cc61c5cfe27b20ea2998eb8`.
  - `python3 -I local/re/evidence/aud02-ngs2-sysrender-01/verify_path.py > local/re/evidence/aud02-ngs2-sysrender-01-root-verification.json 2> local/re/evidence/aud02-ngs2-sysrender-01-root-verification.stderr.log`; exit 0,
    JSON status `PASS`, stderr empty. Root JSON SHA-256
    `74c6df656f5c618c59074e9bc315dec433dd2a56d34abc981c5f07e01e834538`; empty stderr SHA-256
    `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
  - `shasum -a 256 local/re/eboot.elf local/re/imports.json`; exact hashes matched the pinned
    values in the report and verifier: ELF
    `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, import manifest
    `78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744`.
- Observation: independently reviewed verifier logic and reran it against the live pinned ELF.
  It parses ELF64 headers and PT_LOAD/PT_DYNAMIC/PT_SCE_DYNLIBDATA ranges; verifies the exact
  JUMP_SLOT relocation and raw NID for symbol 242; checks live parser slot-to-thunk output and
  REA query envelopes; calculates thunk `0xf91450` → slot `0x1601018` from RIP-relative bytes;
  calculates direct `CALL rel32` at `0x95383c` → thunk; and matches the executable-segment scan
  candidate and captured caller bytes.
- Verified static path: import slot `0x1601018`, symbol index 242 / NID `i0VnXM-C9fc` → thunk
  `0xf91450` → direct call `0x95383c` in Ghidra function `FUN_009536d0`, body `[0x9536d0,
  0x953abf]`, 1,008 bytes. REA Evidence IDs:
  `ev_90786a1998b746a670a1e01d847201edaddc9751c589406f0126cfb6edd7c421`,
  `ev_62e509cad7286b27bc9176aeb8ad91e10f39728f6ded8009f3fdfd18947ee023`, and
  `ev_8fc84b9bed999bc2e81fd2df1d8d0bc7a96c5e5193546f1aa9ef63cc4f3526a4`.
- Limits: both xrefs projections omit edge kind; direct edge classes come from independently
  checked instruction bytes. Ghidra reports partial callee coverage and six call references or
  decoded calls without destinations. Other indirect pointers/calls are unresolved. No ABI,
  arguments, NGS2 behavior, runtime slot binding/use, call reachability, audio, or game execution
  is inferred. AUD-02 remains active and G1 remains NOT PASSED.
- Initial root checksum invocation used repository-root CWD and exited 1 because manifest paths
  are relative to the dossier; rerunning from the dossier CWD passed. This was a root bookkeeping
  error, not a failed analysis/provider try. The worker separately recorded and corrected its
  zsh read-only `status` variable error during checksum wrapper bookkeeping; it did not invoke
  REA and is not a targeted failed try.
- Pre-publication checks from repository root: `python3 scripts/project_index.py` exited 0,
  reported 4,148 folders indexed, and updated `docs/FOLDERS.md` to count 46 checkpoint files;
  its output log `local/checkpoints/2026-10-20-root-aud02-sysrender-project-index.log` has
  SHA-256 `9e7e2b6742a42ea454a0d23092e76314e2cf4a229c8e92231fedd41ce1da0ce8`. `graft check`
  exited 0 (wiring graph in sync; deep meaning layer unbuilt with 1,225 nodes pending); log
  `local/checkpoints/2026-10-20-root-aud02-sysrender-graft-check.log` SHA-256
  `f6496628c6608d20c699a233b45e90bafc20f298616a0926f2bf4d2b576a8fdf`. `git diff --check`
  exited 0; empty log SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`. The
  checkpoint command list is `local/checkpoints/2026-10-20-root-aud02-sysrender-checkpoint-commands.txt`.
- Shared files changed: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, this checkpoint.
  `INSTALL_LOG.md` unchanged. No source, game, reference, or private evidence was published.
  Exact next action: wait for VoiceGetState and SystemDestroy dossier completion, root replay each,
  then update this active checkpoint and dispatch another bounded task if an agent slot frees.
