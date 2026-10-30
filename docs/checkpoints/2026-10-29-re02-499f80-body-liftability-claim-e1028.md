# E1028 — claim `0x499f80` app-main Init body liftability

Date: 2026-10-29 22:23 CDT
Agent: `/root`
Task: `RE-02-APPMAIN-499F80-BODY-LIFTABILITY-01`
Status: `ACTIVE` — no analysis attempts

## Starting evidence

The E1027 follow-up triage identified one concrete unresolved direct edge in the app-main Init body:

- Caller `0x531d60`, callsite `0x531def`, target `0x499f80`.
- The transfer is reached only on the branch where the preceding call to `0x255130` at `0x531dda` returns nonzero.
- The caller sets `RDI = R12 + 0x3c70` at `0x531de7`.
- Existing task `CPU-01-INIT-531D60-POST-BASE-499F80-01` verifies the callsite fixture and stops before the target body; `499f80_body` remains unmodelled (E446).
- Private triage note: `local/validation/orch-20261029/RE-02-APPMAIN-INIT-TRIAGE-01-NEXT/REPORT.md`, SHA-256 `7e0d4c9b3d3b245396978633c13d6a24a95fbf9a2c957fe793ec2635cf91cda1`.

## Scope and acceptance

Use the pinned Ghidra project read-only to characterize only the function body beginning at `0x499f80`. Record a Ghidra Evidence ID, exact body range and hash, branches, direct and indirect calls, field/global writes, and unresolved transfers. Independently decode the same bytes from the pinned ELF and match the body identity. Assess whether a small ordinary-native lift has a defensible boundary; stop at the first unresolved indirect or subsystem edge. Preserve the caller's success condition and `RDI` setup.

Do not infer engine/world semantics from function names or call order. Do not execute game code, edit reference clones, alter source/launcher files, or treat fixture success as boot evidence. If a safe lift boundary is not established, report the exact unresolved dependency and stop.

## Inputs

- Pinned ELF: `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
- Existing app-main Init disassembly: `local/validation/appmain-agent/init_531d60.dis`, SHA-256 `45e99411a19e1009dc90b89ca4647e5b9bba30b240da37582cab7be552a4fc2c`.
- Existing parent task: `RE-02-APPMAIN-INIT-TRIAGE-01`.

Private report location: `local/validation/orch-20261029/RE-02-APPMAIN-499F80-BODY-LIFTABILITY-01/REPORT.md`.

## Claim checkpoint verification

The working source baseline has not changed since E1027. Its configure/build, CTest 487/487, lift 572/0/0, and Trail wiring-graph results are in `local/validation/orch-20261029/RE-02-APPMAIN-B11290-SYSMODULE-ARGS-01/publication/`. For this documentation-only claim, `python3 scripts/project_index.py` and `python3 scripts/project_index.py --check` both passed at 44,256/current. Claim publication logs are under `local/validation/orch-20261029/RE-02-APPMAIN-499F80-BODY-LIFTABILITY-01/claim-publication/`.
