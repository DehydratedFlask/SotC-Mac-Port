# AUD-02 NGS2 RackQuery poison-edge review (E964)

Local date/time: 2026-10-29 08:03 CDT (America/Chicago). Agent: `/root`.

Task: `AUD-02-NGS2-RACKQUERY-POISON-EDGE-01`, parent `AUD-02-NGS2-RACKQUERY-BUFFERSIZE-01`.

## Result

**STATIC_ANALYSIS_VERIFIED** for the terminal-edge question. The call at `0x9507a2` is reached by a conditional branch after a guard comparison and targets the declared `libkernel::__stack_chk_fail` import through the Ghidra thunk at `0xf8f3a0`. Ghidra marks the thunk and its target no-return. The imported implementation and runtime binding were not inspected.

The RDX origin at the single RackQuery callsite `0x94ebc2` remains **UNKNOWN**. This task does not resolve the parent audio contract, and makes no ABI, codec, playback, or runtime claim.

## Pinned identity and reused REA evidence

- ELF: `local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, x86-64, image base `0`.
- Reused REA Evidence: `ev_0fd925bf55ace3db6ec209c0b8b14dc5ed51ac17a80e18f6352aa8534aac4f3c`, Ghidra 12.1.4, procedure `FUN_0094ea10`. Its prior limitation correctly noted that the `hasNoReturn` FunctionManager flag by itself is not independent proof of callee behavior.
- Existing cached-CFG source `scripts/aud02_rackquery_cfg.py` SHA-256 `2efe06fda47e04c162f00d8aff28bacf9247ee21fadceffd22458fb99e9d44fc`; test source SHA-256 `243319d979a0cb92b61d6d15dfa7e50fa2ef5ff6f27917d04d0584e6a73944fc`; pinned one-site output `local/validation/rdx-cfg-harden-20261025/new/strict.json` SHA-256 `27aeb87ed5a5c48c4132114ccee4253db5866adca1aaacbf0fd86b2934e9fb86`.
- The import map `local/re/imports.json` SHA-256 `78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744` binds to the same ELF hash and maps import `__stack_chk_fail` to libkernel NID `Ou3iL1abvng`, slot `0x15fffc0`, status `contract-declared`.

## Edge verification

Ghidra 12.1.4 read the existing saved project using `-noanalysis -readOnly`; it was not asked to re-analyze or modify the project. The relevant PT_LOAD instruction bytes independently match the pinned ELF:

| Address | Bytes | Observation |
|---|---|---|
| `0x950786` | `49 8b 0e` | Load value through `R14` |
| `0x950789` | `48 3b 8c 24 a0 0f 00 00` | Compare with stack slot `[rsp+0xfa0]` |
| `0x950791` | `75 0f` | Conditional branch to `0x9507a2` |
| `0x9507a2` | `e8 f9 eb 63 00` | Call `0xf8f3a0` |
| `0xf8f3a0` | `ff 25 1a 0c 67 00` | Indirect jump through import slot `0x15fffc0` |
| `0xf8f3a6` | `68 01 00 00 00 e9 d0 ff ff ff` | Push relocation index, jump to resolver stub `0xf8f380` |
| `0xf8f380` | `ff 35 22 0c 67 00 ff 25 24 0c 67 00` | 12-byte GOT resolver stub |

Ghidra identifies the `0xf8f3a0` function as `thunk_FUN_00f8f3a6`, with `hasNoReturn=true`; its target `FUN_00f8f3a6` is also marked no-return. The lower resolver stub at `0xf8f380` is marked `hasNoReturn=false`. The import table, guard compare, and conditional mismatch edge explain the terminal stack-check path. The loaded value shown for `0x15fffc0` is a relocated import-slot view; it is **not** claimed to be a file-backed pointer literal. The actual libkernel implementation is outside this ELF, so this is static edge attribution rather than runtime proof.

## RDX re-evaluation

The byte/Ghidra context at `0x94eb78` contains `MOV RDX,RBX`, and the call at `0x94ebc2` is `CALL 0xf91420`. That local instruction sequence does not establish the register’s reaching origin across the function’s gaps. The current strict cached-CFG record for this one callsite reports:

- RDX `status=unknown`, `has_unknown=true`.
- Call-clobber origin at `0x94f183`.
- Unverified indirect-successor entries at `0x94eb4c`, `0x94f190`, `0x94f19b`, and `0x94f1ac`.

The terminal edge at `0x9507a2` is not itself reported as an RDX definition. The value remains unknown until the unresolved predecessor and call-clobber edges are independently resolved. Parent task `AUD-02` remains ACTIVE.

## Reproduction and private artifacts

Read-only Ghidra command:

```sh
env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless local/ghidra/persistent-20261020 SotC -process eboot.elf -noanalysis -readOnly -scriptPath local/validation/AUD-02-NGS2-RACKQUERY-POISON-EDGE-01/attempt-01 -postScript QueryNgsPoisonEdge.java
```

Independent check:

```sh
python3 -I local/validation/AUD-02-NGS2-RACKQUERY-POISON-EDGE-01/attempt-01/verify_ngs_poison_edge.py
```

The verifier exited 0 and wrote `verification.json`. Its SHA-256 is `e44edc8560f0a91d7db3e6524b95a04fa355d0bab4633fbb340f41436c8eb6e1`. The Ghidra query script SHA-256 is `036449a67141c974626676c2fed594ccd2d689183c467992d224bc8617243a20`; final stdout SHA-256 is `a55930a428da658cacc9ce4ef5cde1ea8b44bdfeed09733849da90882cc6dff0`; stderr SHA-256 is `93ae9505012d21e5af7312b18fb50afd57b07d5fe73b9dcdb119133cd3531d4d`. All are private under `local/validation/AUD-02-NGS2-RACKQUERY-POISON-EDGE-01/attempt-01/`.

`readelf -lW local/re/eboot.elf` was unavailable (`readelf: command not found`). The independent verifier parsed the ELF64 program headers directly, confirmed 11 headers, and restricted raw instruction-byte mapping to file-backed `PT_LOAD` entries. An earlier broad segment filter also matched an auxiliary non-`PT_LOAD` header; its ambiguous output was discarded. Neither exploratory result is used in the claims above.

Accounting: one supported static hypothesis; zero failed semantic hypotheses; three successful read-only Ghidra captures; one independent verifier pass. The prior REA MCP `list_documents({})` request timed out and was cancelled before any target was opened; details remain in E963.

## Result-publication validation

- `cmake -S native -B local/build/orch-r22`: exit 0.
- `cmake --build local/build/orch-r22 -j 12`: exit 0.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure`: exit 0, 487/487 passed.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: exit 0, 572 passed / 0 skipped / 0 failed.
- `trail build --check`: exit 0; wiring graph in sync, 10,779 deep-layer nodes pending.
- `python3 scripts/project_index.py`: exit 0, 43,089 folders indexed.
- `python3 -I scripts/audit_publish_paths.py`: exit 0, staged audit 6/6 paths.
- `python3 -I scripts/audit_publish_paths.py --all`: exit 0, all-path audit 3,368/3,368 paths.
- `python3 -I scripts/check_commit_attribution.py`: exit 0; `git diff --cached --check`: exit 0.

Exact logs are under `local/validation/AUD-02-NGS2-RACKQUERY-POISON-EDGE-01/result-publication/attempt-01/`. Commit/push/fetch verification follows in the HANDOFF publication closeout.
