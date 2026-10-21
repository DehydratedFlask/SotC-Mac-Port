# Checkpoint: CPU-01-CTOR-FAMILY-MAP-01 — all 573 constructors classified by reachable-body idiom

- Time / timezone: 2026-10-21 00:35 CDT (America/Chicago), macOS 27.0.1 arm64
- Agent and integrator: orchestrator session (`deepseek-v4.1-flash-1017`), integrator-owned record.
- Task / gate / status: CPU-01-CTOR-FAMILY-MAP-01. **STATIC_ANALYSIS_VERIFIED** (offline manifest);
  no code lifted or registered by this task. G1 not passed.
- Prerequisite evidence / input and source hashes: `local/re/eboot.elf` sha256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; constructor table
  `0x2dd70e0`, 573 entries, sha256 `5917cb85…3ab28` (re-asserted by the loader).
- Owned / changed files: NEW `tools/lift/classify_ctors.py`; shared
  `tools/lift/analyze_slot_struct.py` (extracted `load_image()` / `build_report()` so the classifier
  reuses the same reachability walk, and an empty-walk case now raises instead of `IndexError`);
  private output `local/re/work/ctor-classes.json`; this checkpoint.
- Hypothesis and expected result: the constructor list is not homogeneous. Classifying every slot by
  its reachable body should show how much of the 573-slot list is already modelled, and whether the
  descriptor+mutex+atexit pattern seen at slots 10–19 is a local run or a large family.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native`
  - `python3 -I tools/lift/classify_ctors.py` (~27 s wall, offline; one `llvm-objdump` pass per
    block start, nothing executed)
  - Result summary: `descriptor_mutex_atexit 34, guarded_const 12, leaf_vec_copy 227,
    other 297, undecoded 3` (573 total).
- Exit / observed result:
  - **`descriptor_mutex_atexit` = 34 slots, and they are two contiguous runs: indices 10–19 and
    24–47.** Each calls the pthread mutex initializer `0xb30950` and either `malloc` or
    `__cxa_atexit`. Only slot 10 has a native callback today; slot 11 is the explicit null blocker;
    slots 12–19 and 24–47 have no native counterpart. Spans cluster at 79 B (13, 19, 24, 25, 27),
    ~205–284 B (12, 14–18, 26, 45) and 233 B (29–44) — i.e. three sub-shapes of one contract.
  - **`guarded_const` = 12 slots: 0, 1, 2, 7, 8, 64, 65, 66, 67, 115, 116, 117.** Slots 0/1/2/8
    (286 B) are the `math_constants` clones already lifted; 7 and 67 (39 B) are the lifted
    `vec_neg036`-style guarded splats; **64, 65, 66 (330/319/334 B) and 115, 116, 117
    (467/302/302 B) are unlifted and are not 39-byte one-block ctors** — each repeats a guarded
    constant block 8–11 times and writes the same `.bss` constant range (`0x3351ef8…0x3351f90`)
    that slots 0–2 write, plus a few private cells.
  - **`leaf_vec_copy` = 227**, exactly the previously known `ctor-vec-copies.json` population; all
    already have a native lift or a recorded disposition.
  - **`other` = 297** (the bulk of the remaining game constructors: real code, not constant
    initialisation).
  - **`undecoded` = 0 after correction.** The first run reported three (`0xa3ade0`, `0x83d800`,
    `0x7ce8d0`) because the objdump line parser dropped any instruction whose bytes exactly filled
    the byte column (no trailing space before the tab). With the parser splitting on tabs, all three
    decode and the count is `other 300` / `undecoded 0`. The earlier counts `other 297` +
    `undecoded 3` are superseded.
- Observation versus inference / coverage limitations:
  - Observation: class counts, index lists, spans and shared call/global facts, all re-derived from
    the pinned ELF. The manifest stores per-slot span, instruction count, return count, direct-call
    count, indirect-edge count and body SHA-256.
  - Observation (defect avoided): a naive `E8 rel32` scan over a fixed 0x600-byte window reported
    49 "family" slots, including slot 2 (`0xf53de0`) whose reachable body has **no** calls at all —
    the match came from the neighbouring function. The classifier therefore requires a reachable
    body; the window-scan number is wrong and is not used.
  - Inference: "descriptor + heap mutex + atexit" is the contract for all 34 family slots; the
    per-slot class identity is still unknown for every one of them.
  - Not verified: runtime ordering, whether every family slot is actually called on this build, and
    the semantics of the 297 `other` bodies.
- Cumulative failed tries and distinct hypotheses: 0 failed analysis hypotheses. Three tool defects
  were found by this task: fixed-window poisoning (above), an `IndexError` on an undecoded entry
  (now a clear error), and the byte-column parser truncation that produced the three false
  `undecoded` entries (now tab-split). The published counts are the corrected ones.
- Remaining risks / blockers: slot 11 still blocks the runner, so none of the 34 family slots can be
  exercised. The 34-family result also means a single native helper type would cover a third of the
  recovered prefix — but only after the slot-11 question is settled.
- Next exact action: use `ctor-classes.json` to (a) lift the six remaining `guarded_const` slots as
  one batch, and (b) decide the shared native helper for the 34-slot family once slot 11 is
  resolved.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: by the integrator in this session.
