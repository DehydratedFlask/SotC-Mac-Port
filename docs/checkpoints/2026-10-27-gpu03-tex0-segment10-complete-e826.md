# E826 — SELF program-header 10 recoverability result

**Date:** 2026-10-27 (America/Chicago)
**Local time:** 15:13 CDT
**Integrator:** `/root`
**Task:** `GPU-03-CMSH-TEX0-SEGMENT10-RECOVERABILITY-01`
**Status:** **ARTIFACT_VERIFIED**, 0 failed tries

## Scope and observation

The task checked whether the existing pinned plaintext SELF parser maps a source descriptor to ELF
program-header 10, which E662 omitted from its reconstructed ELF. The worker and root each invoked
`parse_self` once in memory against the exact-hash local SELF. Neither wrote an ELF or inspected,
disassembled, or interpreted any segment contents. An independent reviewer approved the report.

Pinned identities:

- SELF: `local/extracted/CUSA08809/eboot.bin`, 65,314,052 bytes, SHA-256
  `21982727c6f97fd415b3af5e99c5e071619c4dcc2248787a701a31a45ab0a4e0`.
- Parser: `reference/ShadowOfTheColossus-PC/scripts/prepare.py`, checkout
  `70bb43859cd7ff181a6d409540b757866184d60d`, SHA-256
  `9459d439056e9ca61758ad8f3023f0c1e46b085dd1524644446af00374856890`.
- Existing wrapper: `scripts/unwrap_self.py`, SHA-256
  `165333d29aaab4fb2529a796bdaa796574402245e65579b85ecabee3784fc0d8`; the output-writing wrapper
  was not invoked.

Both results agree: ten SELF descriptor rows and eleven ELF program headers. The plaintext
descriptors target PHDRs 0, 1, 2, 8, and 9; none targets PHDR 10. PHDR 10 is:

| Field | Value |
|---|---:|
| Type | `0x6fffff01` |
| Flags | `0x0` |
| File offset | `0x3e30bf4` |
| Virtual / physical address | `0x0` / `0x0` |
| File / memory size | `0xb70` / `0x0` |
| Alignment | `0x10` |

`parse_self` returned a 65,214,308-byte ELF buffer in memory and reported `missing=[10]`. This
establishes only that this admitted plaintext reader cannot recover PHDR 10 from a mapped plaintext
descriptor. It does not establish the segment's meaning or contents. E662's TEX0 byte semantics
remain **UNKNOWN**.

## Reproduction and private evidence

The worker's exact shell invocation was:

```sh
python3 -I - local/extracted/CUSA08809/eboot.bin reference/ShadowOfTheColossus-PC/scripts/prepare.py local/validation/orch-20261027/gpu03-tex0-seg10/report.json <<'PY' > local/validation/orch-20261027/gpu03-tex0-seg10/parse_self.log 2>&1
```

The inline heredoc body was not persisted; the command's JSON report and output log are recorded
below. Root's persisted replay script is the complete reproducible invocation and independently
reran the same pinned parser in memory with:

```sh
python3 -I local/validation/orch-20261027/gpu03-tex0-seg10/root-replay/check_segment10.py
```

Worker report/log SHA-256:

- `local/validation/orch-20261027/gpu03-tex0-seg10/report.json` —
  `4a06cadf05be40a68e453bd0b84fbbb5a48cb2eb441e3a4b4247ca015e9f56b8`.
- `local/validation/orch-20261027/gpu03-tex0-seg10/parse_self.log` —
  `3bc57149b66bc25c6ee2e701242562fe1a4a7bf2de51011f3199acf6432e8f63`.

Root replay report/log/script SHA-256:

- `local/validation/orch-20261027/gpu03-tex0-seg10/root-replay/report.json` —
  `bf6f6f0ca8ea5324ea44a571c5e80f9b0fd57b17690db5852ab0c5d002f5ca5f`.
- `local/validation/orch-20261027/gpu03-tex0-seg10/root-replay/run.log` —
  `befe592b565b610a4c29e32a8df6ac2f0aab4057df31873c7cc159097bd9d944`.
- `local/validation/orch-20261027/gpu03-tex0-seg10/root-replay/check_segment10.py` —
  `1e9c835db1ba5e8568fb40bd4d02c2cfeced8d335bb910bdca29e95616d6c4c6`.

All outputs stay under ignored `local/`. The E825 claim commit is
`f7ee08fdb48e64e41cb6c0dff9285767884f9307`, pushed and fetch-verified on `main`.

## Attribution scope

After E825, a fresh scan covered 9 GitHub branch refs, 12 pull-request refs, 0 tags, 15 unique tips,
and **2,438 reachable commits**. It found zero Claude/Anthropic author or committer identities and
zero Claude/Anthropic credit lines. The user-selected private stash remains unchanged at
`92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`.

A separate broad attribution-policy scan exited 1 on unrelated pre-existing history: 1,068 commits
with the owner's hostname-derived `dehydratedflask@Johnnys-M4-Pro.local` identity and 2
`orchestrator@local` commits. Neither group contains Claude/Anthropic attribution. Those records
were preserved under the requested Claude-only cleanup scope. The concise Claude-scope result and
full broader output are `local/validation/orch-20261027/e825/remote-attribution-audit.log` and
`remote-full-policy-audit.log` respectively.

## Repository verification

Repository commands from the workspace root:

```sh
cmake -S native -B local/build/orch-r22
cmake --build local/build/orch-r22 -j 12
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh
trail build --check
python3 scripts/project_index.py
python3 scripts/project_index.py --check
```

Configure/build exited 0 (`ninja: no work to do`); CTest passed **431/431**; lift checkers passed
**516 / 0 skipped / 0 failed**; Trail is in sync at **9,877 nodes / 13,470 edges** (deep layer not
built); project index generated and checked current at **31,937 folders**. Logs are under
`local/validation/orch-20261027/e826/`. Publication path/identity checks and push/fetch remain
pending. These repository checks do not advance boot, audio, save, Metal or other real-game
acceptance.

**Next exact action:** assess the separate E824 sidecar structural-profile candidate against
`docs/ASSETS.md` and the completed E798 census, then claim it only if it does not duplicate existing
work. No source or dependency changes; no G0–G6 gate advances.
