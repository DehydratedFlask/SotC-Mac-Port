# DATA-02 pakN archive hash claim

- Date/time: 2026-10-19 22:47 CDT (America/Chicago)
- Agent: `/root` (integrator); implementation worker: `/root/data02_probe`
- Task ID: DATA-02, bounded archive-identity checkpoint
- Files changed in this claim: `HANDOFF.md`, `docs/TASKS.md`, this checkpoint; source files remain untouched.
- Exact commands:
  - `graft ask "How does the PSARC summary generator decide whether to compute an archive SHA-256, and where can that policy be changed safely?" --source --in scripts/` — exit 0; 11,062 estimated tokens saved.
  - `graft skeleton scripts/psarc.py` — exit 0; 2,964 estimated tokens saved.
  - `sed -n '230,300p' scripts/psarc.py` — exit 0; observed the 1 GiB digest cutoff and whole-file read for smaller archives.
- Result/evidence path: `local/manifests/assets/CUSA08809-pakN.summary.json` currently has a null digest; private input is `local/extracted/CUSA08809/pakN.psarc` (12,838,532,222 bytes per the probe). No implementation logs exist yet.
- Observations: archive parsing succeeds and the summary is present; PSARC summary generation skips a SHA-256 at or above 1 GiB.
- Inference: a chunked stable-file hash can fill this single identity field without interpreting member data.
- Remaining risks: 12.8 GB one-time sequential read; TOC digest meaning, member formats, content hashes, and `pakN` encoding remain unknown. No game/runtime gate changes; G1 NOT PASSED, G2-G6 NOT RUN.
- Next exact action: implement chunked hashing with a synthetic multi-chunk test, regenerate the private pakN summary, and compare the digest independently with `shasum -a 256`.
- Task status: ACTIVE CLAIM; failed tries: 0.
