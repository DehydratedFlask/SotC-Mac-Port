# Checkpoint: AUD-01-AUDIO-FORMAT-01 — ATRAC9 config word provenance re-derived, checker added

- Time / timezone: 2026-11-01, local (Mac host); worker under the orchestrator
- Agent and integrator: worker (ATRAC9 config lane); integrator = orchestrator (owns HANDOFF.md/TASKS/EVIDENCE)
- Task / gate / status: AUD-01-AUDIO-FORMAT-01 / ATRAC9 config recovery / PARTIAL (provenance `STATIC_ANALYSIS_VERIFIED` + `ARTIFACT_VERIFIED`; decode `NOT RUN`)
- Prerequisite evidence / input and source hashes: E1074 (prior recovery), E1075; pinned ELF SHA-256
  `d0723afb…61d3` (image base 0, PT_LOAD0 file offset 0x4000, RELRO phdr type 0x61000010 at 0x1594000,
  no section headers, 11 phdrs); read-only `.sxd1`/`.sxd2` from the original archive via `scripts/psarc.py`
- Owned / changed files: `tools/lift/check_atrac9_config_provenance.py` (new, untracked),
  `docs/AUDIO_FORMAT.md` (amended), `docs/checkpoints/2026-11-01-atrac9-config.md` (this file).
  Not edited by me: HANDOFF.md, docs/TASKS.md, docs/EVIDENCE.md, native/CMakeLists.txt.
- Hypothesis and expected result: `[wave+0x5f8]` is an arena offset to a u32 stored in the WAVE record
  (field 0x0A), not an id into an image table; one word per record; geometry derivable and consistent
  with segment lengths across all 566 format-0x42 records.
- Exact command, CWD and nonsecret environment: CWD = workspace root;
  `python3 -I tools/lift/check_atrac9_config_provenance.py` (add `--require-private` to make a missing
  private ELF/archive exit 2). Without the private inputs it prints SKIP and exits 0 so
  `tools/lift/run_all_checkers.sh` stays green.
- Exit / observed result: exit 0; 65 checks PASS; 40/40 mutation probes detected; ~10 s.
- Log, capture and REA evidence paths/IDs: prior private script
  `local/validation/AUD-01-AUDIO-FORMAT-01/analysis/check_atrac9_config.py` (48 checks PASS, unmodified);
  LLVM objdump (`--triple=x86_64`) cross-check is check E14 of the new checker. Proposed new ID: E1076.
  No payload bytes, decoded samples or audio were written anywhere.
- Observation versus inference / coverage limitations:
  - Observed (code): `0x987950 = id ? [ctx+0x20]+id : inline`; field loop `{u8 id,u8 count,s16 size}`,
    21-entry table at 0x12b5274; field 0x0A handler 0x980f29 stores `[obj+0x5f8]` and `[obj+0x600]`;
    `spec[+0x28]` kinds 0/1/2 (0x980f39 / 0x9809dc / 0x980aa1); arm 0x98413e..0x9842cf; bextr
    extractions (start 20/len 4, start 17/len 3, start 5/len 11, `(cfg>>3)&3`); finaliser 0x9840f8;
    tables 0x12b5700 and 0x12b5720; no config word anywhere in the image.
  - Observed (assets): all 566 format-0x42 records carry field 0x0A, six distinct words
    (0xFE761DF0:266, 0xFE761DE0:149, 0xFE740FF0:123, 0xFE740FE0:8, 0xFE7A17F0:3, 0xFE7007F0:17);
    music.sxd1 is 33/33 at 0xFE761DF0; derived block bytes equal the gcd of segment lengths per
    (channels, word) group; max end == .sxd2 size in all five files; field 0x0A only in ATRAC9 records.
  - Inference: ATRAC9 field names for the config bits (tag, rate index, channel config, validation bit,
    frame bytes - 1, superframe index) from the public layout; the 0x12b5720 table is "channels the
    word contributes" (matches the channel count in all six words), correcting the earlier label.
  - Not covered: producers of spec kinds 1 and 2 (virtual call); where the ATRAC9 transform executes
    (0x94b2c0 / NGS2 / `maxAtrac9Decoders`); the +-2 byte phase of entropy minima at 242/481/720;
    codec-internal frame layout; any decode.
- Cumulative failed tries and distinct hypotheses: 0 failed tries toward escalation on the recovery
  question. Checker-development defects (wrong probe patch offsets, byte-pin lengths, finaliser SHA
  scope) were fixed within the checker and are not task tries.
- Remaining risks / blockers: ATRAC9 decode and audible output remain `NOT RUN`; the 242 vs 240 spacing
  is unexplained and could indicate a stream wrapper the arm does not model; spec kinds 1/2 producers
  unknown (not on the WAVE-record path).
- Next exact action: first decode attempt (count it as try 1) with FFmpeg `atrac9` or libatrac9 using
  word 0xFE761DF0 on a music.sxd2 slice: exact 240-byte frame boundaries first, then 242/481/720
  offsets; in-memory only, no payload persisted; report attempt count and result.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: not by this worker. Paste-ready text
  was returned to the orchestrator for HANDOFF.md, docs/TASKS.md and docs/EVIDENCE.md (E1076). No
  install occurred (INSTALL_LOG unchanged). `python3 scripts/project_index.py` to be run by the
  integrator after the new files land.
