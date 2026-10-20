# AUD-02-NGS2-VOICE-GETSTATE-01 — root replay checkpoint

- Time / timezone: 2026-10-20 02:47 CDT (America/Chicago).
- Agent / integrator: `/root`; root owns shared docs and publication.
- Task: `AUD-02-NGS2-VOICE-GETSTATE-01`; parent `AUD-02`; status
  `STATIC_PATH_BYTE_VERIFIED`; failed targeted analysis/provider tries 0.
- Worker dossier: `local/re/evidence/aud02-ngs2-voice-getstate-01/`; report SHA-256
  `3fc8ecd9980b9e6430b8ad13771b770df95aaebde1818d6c08a5284c92dd83b6`; verifier SHA-256
  `88e80e1683f61726366b402748e2772ce1e79d939f5e15f3e9c15e89e3cc62d2`; checksum manifest SHA-256
  `c1b7e334dfa56ba8e2ab9bed14c3a1ddca759eda40873040a537d32390711546`.
- Exact root replay commands and results:
  - `shasum -a 256 -c SHA256SUMS.txt` from CWD
    `local/re/evidence/aud02-ngs2-voice-getstate-01`; exit 0, all 167 entries passed. Root log
    `local/re/evidence/aud02-ngs2-voice-getstate-01-root-checksums.log`, SHA-256
    `6823cfad8cad71f0136ef402d192a70a2938e8db3045b3b5448d72cdca316a3f`.
  - `python3 -I local/re/evidence/aud02-ngs2-voice-getstate-01/verify_path.py --elf local/re/eboot.elf --manifest local/re/imports.json --thunk 0xf91470 --callsite 0x94a27d --pre-start 0x94a270 --pre-end 0x94a282 > local/re/evidence/aud02-ngs2-voice-getstate-01-root-verification.json 2> local/re/evidence/aud02-ngs2-voice-getstate-01-root-verification.stderr.log`; exit 0, JSON status `PASS`, stderr empty. Root JSON SHA-256
    `6c062f90413f3aeb3283cc2dff80f2a6ed31b9bee2bbcd510afc368294a11e37`.
  - `python3 -I local/re/evidence/aud02-ngs2-voice-getstate-01-root-rea-review.py > local/re/evidence/aud02-ngs2-voice-getstate-01-root-rea-review.json 2> local/re/evidence/aud02-ngs2-voice-getstate-01-root-rea-review.stderr.log`; exit 0, JSON status `PASS`, stderr empty. Review script SHA-256
    `284ddb0bb9b8dd58b7716a8521e73e21c35d6457244c6ae6fc800ad222e2f68b`; output SHA-256
    `cd7024fd43546c2b56423f08629c8b5ed017b54d083456e3b07faeddddd1a2aa`.
  - `shasum -a 256 local/re/eboot.elf local/re/imports.json`; hashes matched the pinned ELF
    `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3` and manifest
    `78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744`. Captured root hash log
    SHA-256 `7e4c64e39642707ef91b0654aad2e4e9399fb7eedf7f4fdbdd6f1054c23ba2e8`.
- Observation: independent parser/checker and REA audit agree on the path `0x1601028`, symbol
  index 244 / NID `-TOuuAQ-buE` → thunk `0xf91470` → direct call `0x94a27d` in
  `FUN_0094a200`. Raw direct-call bytes are `e8ee716400`; thunk bytes `ff25b2fb6600` resolve to
  the slot. The selected 18-byte pre-call range `[0x94a270,0x94a282)` has SHA-256
  `d36a8e77ba5f6b2de1daba0dbdb15472a7d7a3ca5859f3e94e21321d4edb5209`. REA Evidence IDs:
  `ev_d97708e1afeaa1bfd6ffb28334a35ce08d4c195d9f0947fddb5769210cdac4a1`,
  `ev_5217b06df0de0cec23cf13c060f5a3687aee147d75b5d928ffb3c57ed634e0e0`, and
  `ev_38a4580321f83c2aa6fe6b3d32976c62c0f036fa083cf96cd8ce2138a4530ce6`.
- The thunk query also reports a second direct-call reference at `0x94a440`; it was not
  byte-checked. Ghidra labels the destination as adjacent `0xf91476`, while raw bytes establish
  the selected call to `0xf91470` and its RIP-relative jump to the slot. Computed/indirect targets
  remain unknown despite an empty normalized unresolved-call list for this one function. No ABI,
  arguments, runtime binding/reachability, NGS2 meaning, audio, or game execution is inferred.
- A first root verifier invocation omitted required `--elf` and `--manifest` arguments and exited
  2; its usage error is preserved in
  `local/re/evidence/aud02-ngs2-voice-getstate-01-root-verifier-missing-args.log` (SHA-256
  `1296f45b3a2c5e7ba172301f0bec8c12324c5a6070a303bbb2cbf2a9c52c3517`). The corrected invocation
  above is the passing result. This was a command/setup error, not a failed analysis/provider try.
- Shared files changed: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, this checkpoint.
  `INSTALL_LOG.md` unchanged. Batch project-index, Graft, path-audit and divergence evidence is
  recorded in the [SystemDestroy root replay checkpoint](2026-10-20-root-aud02-system-destroy-complete.md).
  Next exact action: claim the next
  two bounded unclaimed NGS2 import callpaths after checking the manifest and Stage 3 prerequisites.
  G1 remains NOT PASSED.
