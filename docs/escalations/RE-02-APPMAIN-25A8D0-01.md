# NEEDS_PRINCIPAL: RE-02-APPMAIN-25A8D0-01 — verifier mapping invariants

- Owner / principal / date: `/root/appmain_25a8d0` / `/root` / 2026-10-20.
- Requirement and failing gate: independently verify the full app-main body, calls, references, and vtable linkage against the pinned ELF's program headers; do not promote runtime reachability, class identity, or ABI.
- Input SHA256, source pins, original RVA / file offset / image base: ELF SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; target RVA `0x25a8d0`; final PT_LOAD #0 file offset `0x25e8d0`; image base `0`.
- Reproducer / expected result: `python3 -I local/re/evidence/re02-appmain-25a8d0/verify.py`; expected exit 0 with byte-matched output against worker result.
- Current tree changes and recovery location: no binary/source changes; private evidence at `local/re/evidence/re02-appmain-25a8d0/`; shared outcome at `docs/checkpoints/2026-10-20-root-re02-appmain-25a8d0-complete.md`.

| Failed try | Distinct hypothesis/change | Exact command / environment | Actual result | Log/evidence |
|---|---|---|---|---|
| 1 | All Ghidra non-stack data references must map to file-backed bytes. | Initial worker verifier run under `python3 -I`; pinned ELF SHA `d0723afb...61d3`. | Exit 1 at reference `0x2ea5170`, which is in PT_LOAD #2 `p_memsz` but beyond `p_filesz`; treating zero-fill memory as invalid file mapping was too strict. | `local/re/evidence/re02-appmain-25a8d0/verification-initial-note.md`, `verification-initial.stdout.log`, `verification-initial.stderr.log` |
| 2 | Each raw virtual address must have a unique file-backed mapping across all program headers. | Follow-up verifier run under `python3 -I`; same pinned ELF. | Exit 1 at `0xfc3c9a`: PT_LOAD #0 and a non-PT_LOAD header provide different file offsets. Updated policy to prefer PT_LOAD. | `local/re/evidence/re02-appmain-25a8d0/verification-pre-rip-supplement.*` and note above |
| 3 | The whole app-main body has one file-backed mapping across all headers. | `python3 -I local/re/evidence/re02-appmain-25a8d0/root-phdr-audit.py`. | Exit 1: PT_LOAD #0 maps body bytes to `0x25e8d0`; PHDR #8 (`p_type=0x61000000`) overlaps it at `0x30359d0`. | `local/re/evidence/re02-appmain-25a8d0/root-phdr-audit.py`, `.log`, `.stderr.log` |

- Smallest failing fixture and how to run it: the pinned ELF itself, hash above; run the private verifier and inspect its mapping assertions. No source patch or executable fixture is needed.
- Verified facts versus guesses: verified final code body is 1,021 bytes in PT_LOAD #0; PHDR #8 is an alternate raw file view but its meaning is unknown; three incoming words target `0x25a8d0`; two internal indirect calls remain unresolved. Guesses: none promoted to class/ABI semantics or runtime reachability.
- Why the previous approaches failed / missing information: raw program headers overlap and include zero-fill ranges; a single all-header uniqueness rule is invalid for this file. The correct code-byte rule is unique PT_LOAD; data mappings prefer PT_LOAD and may use one non-PT_LOAD mapping only if no PT_LOAD applies.
- Independent work that can continue: unrelated APP-01 folder-import cancellation task and other bounded DATA-02/AUD-01 tasks.
- Task register and HANDOFF checkpoint links: [task completion](../checkpoints/2026-10-20-root-re02-appmain-25a8d0-complete.md), [current handoff](../../HANDOFF.md).
- Principal reassessment / authorized next approach: on 2026-10-20, `/root` reviewed the three verifier-only failures and authorized one final corrected verifier plus an independent root replay; both exited 0 and produced byte-identical JSON. This completes the bounded task. No additional verifier/analysis attempt is authorized without a new principal decision.

This escalation is closed by principal review. The verified task status is `STATIC_ANALYSIS_VERIFIED`; the record remains to preserve the three failed verifier attempts and the fourth, authorized completion replay.
