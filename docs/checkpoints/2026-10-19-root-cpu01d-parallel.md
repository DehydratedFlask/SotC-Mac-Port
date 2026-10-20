# Checkpoint: CPU-01d — two unguarded vector-copy fixtures

- Time / timezone: 2026-10-19 22:30 CDT (America/Chicago)
- Agent and integrator: `/root`; `/root/cpu01d_ce57b0` and `/root/cpu01d_ce0890` implement disjoint files; root is sole integrator.
- Task / gate / status: CPU-01d `0xce57b0` and `0xce0890`; ACTIVE, zero failed tries each; fixture-level only; G1 remains NOT PASSED.
- Prerequisite evidence / input and source hashes: exact ELF SHA256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; local `ctor-survey.json` contains 573 rows. Private `ctor-vec-copies.json` lists `0xce57b0` source/destination `0x12f8330`/`0x3351cb0` and `0xce0890` source/destination `0x12f7c00`/`0x3351c90`, each with bits `{~0,~0,~0,0}`. These manifest values are selection leads; each worker must independently confirm from ELF bytes.
- Owned files: worker `/root/cpu01d_ce57b0` owns `native/core/vec_ce57b0.{hpp,cpp}`, `native/tests/test_vec_ce57b0.cpp`, `tools/lift/check_fce57b0.py`; worker `/root/cpu01d_ce0890` owns `native/core/vec_ce0890.{hpp,cpp}`, `native/tests/test_vec_ce0890.cpp`, `tools/lift/check_fce0890.py`. Root owns CMake, startup registration, docs, and HANDOFF.
- Hypothesis and expected result: both 17-byte ctor bodies are unguarded 16-byte vector copies plus `ret`, with no calls. Exact RIP-relative targets and copied words must be computed from encoded displacements, not disassembler comments. Native functions should initialize their own distinct destination cells with matching bits.
- Exact command / result, CWD `/Volumes/Extra/SoC Mac Native`:
  ```sh
  python3 - <<'PY'
  import json
  from pathlib import Path
  survey = json.loads(Path('local/re/work/ctor-survey.json').read_text())
  copies = json.loads(Path('local/re/ctor-vec-copies.json').read_text())['entries']
  for i in (153, 155):
      row = survey[i]
      record = next(item for item in copies if item['ctor'] == row['ctor'])
      print(i, row['ctor'], row['bytes'], row['calls'], row['indirect_calls'], record)
  PY
  ```
  It exited 0 and found both entries to be 17 bytes with no direct/indirect calls; the manifest records distinct cells and mask bits. No build/test ran before the workers received the claim.
- Log / evidence paths: private `local/re/work/ctor-survey.json`, `local/re/ctor-vec-copies.json`; no new Ghidra/REA evidence was produced.
- Observation versus inference / coverage: survey metadata and manifest values are observed records. The target vector-copy interpretation is provisional until the workers verify exact instruction bytes, displacements, and data. No startup reachability or game behavior is claimed.
- Cumulative failed tries: 0 for each selected leaf; other CPU-01d failures, including `0xd71f60`, remain separate and unchanged.
- Remaining risks: standalone constructors do not establish an executable startup chain; exact target addresses must be recalculated from the ELF. CMake/registry integration is not part of worker ownership.
- Next exact action: publish this claim, dispatch the two workers, then review their fixture/test reports before root integrates CMake and registration.
- HANDOFF / TASKS / EVIDENCE / INSTALL_LOG / indexes: HANDOFF and TASKS updated; EVIDENCE unchanged pending exact checks; `INSTALL_LOG.md` unchanged; `python3 scripts/project_index.py` exited 0 and regenerated `docs/FOLDERS.md` (4,108 folders indexed); `graft check` passed for the configured source graph, with the optional deep meaning layer unbuilt. No folder created or moved.
