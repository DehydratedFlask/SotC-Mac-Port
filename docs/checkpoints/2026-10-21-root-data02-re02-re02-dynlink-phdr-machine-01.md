# Checkpoint: RE-02-DYNLINK-PHDR-MACHINE-01 — `program_headers` identity/truncation rejections pinned

- Time / timezone: 2026-10-21 00:19 CDT (America/Chicago)
- Agent and integrator: worker `data02-re02-fixtures` (`deepseek-v4.1-flash-1017`); integrator: orchestrator session
- Task / gate / status: `RE-02-DYNLINK-PHDR-MACHINE-01`; RE-02 synthetic parser fixture; `FIXTURE_VERIFIED`
- Prerequisite evidence / input and source hashes: repo `HEAD` `06ce0f4`; `scripts/ps4_dynlink.py` unmodified and byte-identical to `HEAD`, SHA-256 `3c3785bbb8fde61bb85aa98202b570b57c42a16b932f90aaedf28decca99ff30`.
- Owned / changed files: `scripts/test_ps4_dynlink.py` only (added `elf_header`/`program_header` fixture helpers and three tests). No parser change.
- Hypothesis and expected result: `program_headers` accepts only `EI_CLASS=2`, `EI_DATA=1`, `EI_VERSION=1`, `e_machine=62`, and rejects class-32-bit, big-endian and x86-32 fixtures with the exact `ValueError('expected little-endian x86-64 ELF')`. Truncated input must raise `ValueError` from the `span` guard (message prefix `out of bounds: `) instead of indexing past the buffer or raising `IndexError`/`struct.error`.
- Exact command, CWD and nonsecret environment: CWD `/Volumes/Extra/SoC Mac Native/scripts`, `python3 -I -m unittest discover -s . -p 'test_ps4_dynlink.py' -v` (Python 3.15.0). The literal `-m unittest test_ps4_dynlink` form fails to import under `-I`, see `/private/var/folders/vx/5z8skhb97w32g817rv7hbh940000gn/T/opencode/b8/m-form-error.log`.
- Exit / observed result: exit 0, `Ran 17 tests ... OK`. Cases: header cut at 0/32/63 bytes, an intact header with the phdr table cut 8 bytes short, and `e_phnum=0xffff` with one phdr present — all raise `ValueError` whose message starts with `out of bounds: `. The happy path is also asserted (`entry == 0`, one phdr with `vaddr=0x1000`, `memsz=8`), so the rejections are not vacuous.
- Log, capture and REA evidence paths/IDs: `test_ps4_dynlink.after.log` SHA-256 `75cef996c133af32cd0b92db1cbaef127ed79af288caeac67ad0a1ce6447f049`; mutation proof `mutation_check.log` SHA-256 `24942501845ade961d4f2118ed0db7610df31ff6872f22bc445d2bc24df3425b` (temp dir outside repo).
- Observation versus inference / coverage limitations: neutralising the ident/machine check makes the class/data/machine test fail (`B7: CAUGHT`); the harness restored `ps4_dynlink.py` byte-identically. `program_headers` had no test at all before this task. Synthetic only; the real SELF/ELF reconstruction, PHDR type meanings and loader behaviour remain out of scope.
- Cumulative failed tries and distinct hypotheses: 0 failed tries.
- Remaining risks / blockers: the truncation assertions check the `out of bounds: ` prefix, not the full byte counts, so an unrelated refactor of `span` formatting would not break them; the identity message is pinned exactly.
- Next exact action: integrator records the row and refreshes `python3 scripts/project_index.py`.
- HANDOFF.md / TASKS / EVIDENCE / INSTALL_LOG / indexes updated: NOT updated by this worker; no install facts changed.
