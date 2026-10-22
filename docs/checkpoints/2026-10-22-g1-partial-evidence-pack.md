# G1 PARTIAL evidence pack — recovered startup + Init-hash depth

- Date/time: 2026-10-22 10:11 CDT (America/Chicago).
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9 (Drive).
- Task: `CPU-01-G1-PARTIAL-EVIDENCE-01`; gate: **G1**; status: **PARTIAL** (not PASSED).
- Failed hypotheses/tries: 0.
- Tip captured: `148f8f577e41d16c3a4364edd2393e1ab0b82d28` on `cursor/init-hash-551140-9dfc` (PR #4).

## Claim boundary (honest)

**Claimed:** native ARM64 Mach-O harness runs `recovered_constructors()` for the production table
(**155** pairs; contiguous **0–94** including former first-null **slot 23**, gap **95** dormant,
and **96–151**), plus Init-hash registry fragment fixtures (fill + Init `0x531d60` install cell).
No guest CPU / eboot runtime in the harness.

**Not claimed:** G1 PASSED/COMPLETE; cold game boot; authentic audio; real save/reload; Metal
gameplay; full Init body; pool/listener half of `0x52a220`; heap hierarchy from PR #5 as part of
this tip’s binary.

## Private evidence (gitignored)

Directory: `local/validation/g1-partial-20261022/`

| Artifact | SHA-256 |
|---|---|
| `capture.py` | `4bd5f1807a22cd19873cef1e52399f5355925b48354a9f78b99efaa9175c5583` |
| `manifest.json` | `fc9ac6685dfadbfc330cc598adbee850359beab6c1cc4df123b2e6377136be1f` |

Exact command:

```sh
python3 -I local/validation/g1-partial-20261022/capture.py
```

Exit 0. Manifest retains env baselines, cmake configure/build, `file(1)` on ARM64 Mach-O test
binaries, ctest stdout/stderr, and ELF checker outputs (private `local/re/eboot.elf` required).

## Observed results

| Step | Result |
|---|---|
| Toolchain | arm64; macOS 27.0.1 (26A434); Apple M4 Pro; 12 CPUs; 48 GiB; Xcode 27.1 (27A9275) |
| Build | Debug/Ninja `local/build/native-g1partial-20261022` exit 0 |
| Binaries | `test_startup_ctors`, `test_registry_init_531d60`, `test_allocator_front` = Mach-O arm64 |
| CTest | **13/13 PASS** (`startup_ctors`, `slot23_names_fixture`, registry Init-hash suite, allocator front/page_*) |
| Checkers | `check_ctor_slots` + Init-hash/registry/allocator wrapper checkers all exit 0 → **155** pairs |

## Parallel citation (not built here)

Heap lane PR #5 tip `4d7e484` / evidence **E403** (lazy latch `0x2dfaaa0`) on
`cursor/heap-depth-5d26` — FIXTURE_VERIFIED there; **not** merged into this Drive tip; cited only
as helper depth available on the parallel branch.

## Unresolved edges (explicit)

1. idx **95** (`0xd71f60`) — dormant `NEEDS_PRINCIPAL`
2. cliff **152+** / exec **424** — demoted leaf/vec; not production
3. `0x52a220` pool/listener loop after `0x52a680` — NOT modelled
4. Remainder of Init `0x531d60` beyond registry fragment — NOT modelled
5. App-main virtual slots 4/5 → measured engine startup slice — incomplete
6. Heap hierarchy / lazy latch — on PR #5, not this tip binary
7. `0x12820`/`0x12af0` bodies — still contract/partial on respective lanes
8. Registry cell not invoked from `recovered_constructors` / app Run
9. G5 cold boot / audio / real save — NOT RUN
10. Metal gameplay — blocked pending G1→G3

## Gate status

- **G0:** PARTIAL (unchanged inventory baseline)
- **G1:** **PARTIAL** (this pack). Remains incomplete until VALIDATION pass criteria.
- **G2–G6:** NOT RUN

## Next exact action

Wire Init-hash / allocator depth into a measured app-main startup slice, or close a named
unresolved edge required for that slice. Do not treat pool tourism as G1 progress. Goal open.
