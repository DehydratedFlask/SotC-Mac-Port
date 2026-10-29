# SAV-01 payload read primitive `0x8eba0` — E955

- Time: 2026-10-29 06:29 CDT (America/Chicago).
- Agent / integrator: `/root`; task `SAV-01-PAYLOAD-READ-PRIMITIVE-8EBA0-01`.
- Status: `STATIC_ANALYSIS_VERIFIED` within the read-only primitive, four direct callers, constructor-installed vtable slot, and import-thunk boundary; publication pending.

## Scope and observations

Read-only Ghidra 12.1.4 analysis of `0x8eba0`, the direct caller contexts, its constructor-installed callback, and the short-read import thunk. The pinned ELF SHA-256 is `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; the saved project was reopened with `-noanalysis -readOnly`.

The 187-byte / 46-instruction reader receives the object in RDI and a destination qword pointer in RSI. It calculates remaining bytes from raw fields `+0x110` and `+0x108`. When at least eight bytes remain, it reads a qword from the data pointer at `+0x120` plus the cursor and advances `+0x108` by eight. For shorter remaining spans, it copies the tail, invokes vtable slot `+0x10`, stores the callback result as the new length, resets the cursor, copies the remaining qword bytes from the start of the data, and updates the cursor and raw counter `+0x130`. The function returns void.

The installed vtable is `0x1595dc0`; RELRO slot `0x1595dd0` points to `0x4d4d0`, whose bytes `31 c0 c3` are `xor eax,eax; ret`. The four direct callsites are `0x543462` and `0x543697` in `0x542880`, `0x543ccd` in `0x543ba0`, and `0x5460ad` in `0x545d30`. Each call instruction and captured caller context matched the pinned ELF. Three sites reuse the local object passed to `0xa81180`; the fourth passes another local reader object.

## Bounds interpretation

The constructor `0x8db10` writes the payload pointer at `+0x120`, the supplied length at `+0x110` and `+0x118`, and zero at `+0x108`. This and the ordinary eight-byte path support a view-like memory-reader interpretation. It is not strict fail-closed short-read behavior: the callback returns zero, after which the reader wraps to the data start and performs a second copy without returning an error or checking a short-read result.

The six-byte thunk at `0xf8f470` jumps through import slot `0x1600028`. The local manifest identifies it as `memcpy` (symbol index 351, NID `Q3VBxCXhUHs`, status `contract-declared`; manifest SHA-256 `78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744`), but does not resolve the runtime binding. If the slot follows the declared memcpy contract, a fresh payload shorter than four bytes can make the second copy request exceed its declared span. Runtime binding and actual backing allocation were not verified, so this is a static risk, not a dynamic observation. The earlier profile-chunks “bounded payload sub-stream” statement is qualified: ordinary full-width reads are length-gated, but the short-read path wraps and does not report failure.

## Verification

- Exact read-only Ghidra command and script are in `local/validation/orch-20261029/SAV-01-PAYLOAD-READ-PRIMITIVE-8EBA0-01/attempt-01/commands.txt`; all three invocations exited 0 as the query script was expanded. Final log: `.../attempt-01/ghidra.log`.
- `python3 -I local/validation/orch-20261029/SAV-01-PAYLOAD-READ-PRIMITIVE-8EBA0-01/attempt-01/verify_read_primitive.py` exited 0. It byte-matched complete instruction bodies at `0x4d470` (35 B), `0x8eba0` (187 B), and `0xf8f470` (6 B), all four direct callsites and captured contexts, the raw RELRO pointer, and callback bytes. Output: `.../attempt-01/verification.txt`.
- The first verifier run used an instruction-record regex that also matched the `INSN` suffix in `VTABLE_TARGET_INSN`, incorrectly assigning target instructions to the reader. Anchoring to Ghidra's `> INSN|` marker fixed the parser; the replay passed. This was a verifier setup correction, not a semantic hypothesis failure.
- Analysis accounting: one bounded read-only analysis with three query invocations; one verifier parser correction; zero failed semantic hypotheses. Full report and private artifacts: `local/validation/orch-20261029/SAV-01-PAYLOAD-READ-PRIMITIVE-8EBA0-01/attempt-01/REPORT.md` and adjacent files.

## Remaining work

Computed callers, deeper import-slot relocation and runtime binding, payload ownership, other typed read primitives, malformed-input dynamic behavior, full save schema, native lift/wiring, authentic save/reload, and G1–G6 remain unresolved. No game code was executed and no runtime gate advanced.

## E956 publication closeout

Publication gates, commit identity, remote verification, and exact counts will be recorded here after the checkpoint is committed and pushed.
