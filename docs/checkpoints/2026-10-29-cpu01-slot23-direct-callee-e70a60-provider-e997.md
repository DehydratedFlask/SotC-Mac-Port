# E997 — E995 slot-23 direct-callee provider timeout

Date: 2026-10-29 12:10 CDT
Agent: `/root`
Task: `CPU-01-SLOT23-DIRECT-CALLEE-0E70A60-01`
Parent: `CPU-01-SLOT23-CONSUMERS-01`

## Attempt and result

Starting tree: clean `main` at `829a193491ce3f4d1433f002092fef466ae67752`, matching `origin/main`. Pinned input `local/re/eboot.elf` SHA-256 is `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0.

REA successfully opened the pinned ELF. The only analysis request was `mcp__rea__inspect_native_instruction({address: "0xe70a60"})`. It started at approximately 12:04:45 CDT and returned at 12:10:15 CDT after 330,000 ms with `provider_timeout` / `failure_kind=timeout` and `failure_message="Ghidra startup deadline elapsed"`. REA 6.7.0, Ghidra 12.1.4, profile digest `769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa`, run ID `5df90b86-e170-4434-981b-c6a571d2679c`. The response had `output_incomplete=true`, `diagnostic_truncated=true`, configured JVM heap 2G, process ID 26510, and partial startup/import logs. It returned no Evidence ID, instruction bytes, decode, or first-transfer boundary; coverage is zero instructions and the entry remains unresolved.

The task packet says to stop after a provider setup failure. No LLVM/raw-byte substitute, retry, or transfer-target inspection was performed. This is one attempted provider query and zero failed semantic hypotheses. The exact structured response and temporary Ghidra log paths are preserved privately in `local/validation/CPU-01-SLOT23-DIRECT-CALLEE-0E70A60-01/analysis/attempt-01/rea-timeout.json`; the private `result.md` and `preflight.txt` hashes are `450e9c2e3669075bdc92a6ca4625ecf93acf205c72eb0ceeed197f0895cc1595` and `79365b32cdde3f79e49e39f3572d65e398ff550ead1f772f5e6eec5b837485eb`, respectively. The timeout JSON SHA-256 is `15329ac1406c05e1a476924da46007207822a28b60046b7797c4c0ec624b9f6a`.

## Status and limits

The provider request is `FAILED`; the task is `BLOCKED` pending principal reassessment if REA/Ghidra readiness changes. Do not retry under the current task packet. E981's conditional local path to the call at `0xe5c3f5` remains verified, but incoming startup reachability and the provenance of `R13d == 0x2b` are unknown. Slot 23 remains unregistered; the parent remains PARTIAL and G1 remains PARTIAL. No product gate advanced.

No game asset/save was read, no game execution occurred, and no source implementation changed. Next exact action: continue independent E996 save-dispatch analysis; reassess E995 only after a concrete provider-readiness change and principal authorization.
