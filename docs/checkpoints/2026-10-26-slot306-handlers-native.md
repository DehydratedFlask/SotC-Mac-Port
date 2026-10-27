# Slot 306 command handlers — native model + REA corroboration (CPU-01-SLOT306-HANDLERS-NATIVE-01)

Date: 2026-10-26. Orchestrator: GLM-5.3 `/root`. The background subagent assigned this lane died
twice to client transport errors (ECONNRESET) with zero files written; the orchestrator implemented
the lane directly. No original code was executed, loaded or relocated at any point.

## Deliverables (all green on the combined tree)

- `native/core/sndx_command_handlers.{hpp,cpp}`: native model of the three `sndx_` handlers —
  `run_sndx_handler` (returns the observed 0/3 `bpe::ECmdExecuteResult` codes; null-engine guard
  for all three; Mute/Filter args-count guard `(end-begin)>>4 == 1`; missing hooks and malformed
  spans fail closed with a named stage), `sndx_bool_var_set_0xba40` (clamp gate +0x6a, mode bit 0
  of the flags word +0x1c, ordered-mode bounds +0x69/+0x6b including the current-value boundary
  cases, unordered boolean gate sequence, store through the +0x60 value reference, change-gated
  callback hook), `run_sndx_engine_reload` (loaded-byte gate; pass-1 unregister + handle := -1 /
  remove-reference + loaded-byte clear per 0x38-stride record at the bank vector engine+0x28/+0x30;
  pass-2 re-reference of +0x14-flagged records; LoadSoundTable(+0x7270, +0x7250); the 256×0x40
  clear array at +0x1c50 with its +0x38 field zeroed), and `sndx_command_id_for_tag` (the only
  bridge from a handler RVA identity tag to the native model; unknown tags are refused).
- `native/tests/test_sndx_command_handlers.cpp` (446 lines): tag resolution; the full 0xba40 case
  matrix; the reload walk (gate, both passes, load-table args, clear loop over all 256 records,
  hook-refusal leaves the handle unwritten, span guards); handler cases 0, 3, null engine, args
  count 0 and 2 (nothing touched), Mute-vs-Filter vector choice with the 0xc8-stride walk of all
  32 slots, parsed-value propagation, no-change ⇒ no callback, missing-hook fail-closed, misaligned
  spans. Passes 1/1; full CTest **402/402**.
- `tools/lift/check_fconcmd_handlers_slot306.py`: ELF-tie checker — 12 body hashes/extents re-read
  from the pinned ELF (matching the private manifest), 27 exact instruction-byte pins with decoded
  call targets and displacements (args-count `cmpl $1`, vector loads `0x8bd0/0x8bd8` and
  `0x72a8/0x72b0`, stride `addq $0xc8`, value-ref `movq 0x60(%rbx)`, setter/lookup/unregister/
  addref/loadtable call sites, the `-1` handle clear, the `0x1c88` clear store and `0x4000` bound),
  the four token strings (`true`/`yes`/`false`/`no` at 0xf95a1e/0xf95a23/0xf95a27/0xf95a2d), the two
  import NIDs recomputed with `scripts/ps4_dynlink.nid` and cross-checked against the JMPREL slots
  the PLT thunks push, the native constants, the tag wiring and the CMake registration. Exit 0; a
  tampered pin exits 1.
- Wiring in `native/core/fconcmd_object_slot306.hpp`: three `static_assert`s bind
  `kSlot306CommandSites[].handler_rva` to the new module's tag constants, and the header docs now
  state the handlers' behaviour is modelled natively while the tags remain identity-only and are
  never executed. Full CTest **402/402** and lift checkers **487/0/0** after wiring.

## REA corroboration (root lane, per the owner's direction to use the REA skill)

The connected REA MCP server's Ghidra provider times out on this 65 MB ELF (its 330 s default
startup deadline — same observation as the 2026-10-25 session). The CLI from the same pinned
`rea-agents@6.7.0` package with `GHIDRA_INSTALL_DIR` (from the opencode MCP config),
`GHIDRA_MAXMEM=8G` and `REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000` completed the import, and focused
`rea function` dossiers were saved under `local/validation/orch-20261026/rea/` (private; Evidence
IDs `ev_4e282afb…` for 0xa5f380 and `ev_89827f58…` for 0x21da0 in the saved JSON envelopes):

- `0xa5bf50` / `0xa5c020`: Ghidra pseudocode matches the checkpoint contract exactly — return 3
  default, `DAT_032f8ff0 != 0` guard, args `(end-begin)>>4 == 1` guard, `FUN_0001e4c0(args,0,0,0)`,
  `FUN_0000ba40(var, var+0x60, &b)` loop with stride 200 (=0xc8), Mute `+0x8bd0/+0x8bd8`, Filter
  `+0x72a8/+0x72b0`, stack-protector scaffolding only. Sole incoming references are the two
  handler-RVA stores in the slot-306 body (`0xa64796/0xa647b2`, `0xa64882/0xa6489e`).
- `0xba40`: full gate/store/callback structure confirmed, plus a nuance now modelled: the ordered
  clamp variant also compares against the currently stored value in both boundary cases.
- `0x1e4c0`: token compares + numeric fallback confirmed. The two INF-open questions from the
  2026-10-26 handlers checkpoint are now OBS: the compare import is **strcasecmp** (NID
  `AV6ipCNa4Rw`, computed with `scripts/ps4_dynlink.nid("strcasecmp")` and cross-checked by
  `name_by_hash` over the pinned reference clones) — i.e. the token compare is case-insensitive —
  and the numeric import is **`_Stoul`** (NID `zlfEH8FmyUA`). Token set: "true"/"yes" → 1,
  "false"/"no" → 0, else `_Stoul(text, 0, 10) != 0`.
- `0xa5f380`: bank-record walk confirmed (records 0x38 at the +0x28/+0x30 vector; handle at +0x10
  with the −1 sentinel, written back after unregister; loaded byte +0x08 cleared after
  remove-reference; pass-2 +0x14 re-reference; `LoadSoundTable(engine+0x7270, engine+0x7250)`;
  the 256×0x40 clear loop). Ghidra renders the cleared field base as `+0x1c88`, which is exactly
  the checkpoint's array base `+0x1c50` plus the `+0x38` field offset — both readings agree.
- `0x21da0`: result codes confirmed (0 = success + flag updates; non-zero → "Command %s failed to
  execute!"; 2 → additional "Usage: %s" via the object's vtable slot).
- `0xa5bf20`: Ghidra created no function object (33 bytes, referenced only as data), so the
  corroboration is byte-level instead: body SHA-256 `e77baf97d82167d5…` matches the manifest, and
  the instruction decode matches the contract (`lea` of the engine cell 0x32f8ff0; `mov eax,3`;
  `test rdi,rdi; je` early-return; single `call 0xa5f380`; `xor eax,eax; ret`).

## Status

- **CPU-01-SLOT306-HANDLERS-NATIVE-01: FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED** at the
  handler-model scope. 0 failed hypotheses (three compile slips in the first build were fixed
  before any semantic check; the two test-fixture corrections — alignas stride padding and unset
  bank handles — were test bugs, not model bugs).
- Still open (named, not silent): the 0x1e4c0 record layout itself (the host supplies the parse
  hook), the non-null ConCmd-manager path of 0x20d80, invoker thunk 0x1990 internals, and the four
  reload callees' bodies. No runtime/G1 claim; G1 remains PARTIAL.
- Next exact action: the category-vars lane (0xa5cd90 builder, contract already decoded from the
  private listing), then docs/gates/audits and the owner questions.
