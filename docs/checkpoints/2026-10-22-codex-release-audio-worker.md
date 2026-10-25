# Release/audio worker checkpoints and constructor scout

2026-10-22 22:28 CDT (America/Chicago); integrator /root. Workers
/root/release_implementer, /root/audio_cfg_implementer; scout /root/ctor_failure_scout.

## CPU-01-SHARED-STR-RELEASE-F590-01

Worker verified; independent review pending. Attempt1 succeeds; zero failed semantic hypotheses.
New files shared_str_release_f590.hpp/.cpp, test_shared_str_release_f590.cpp and check_ff590.py;
root owns only CMake source/test entries. Full helper semantics retain required typed deallocation,
seq-cst old-count gate, callback order/kinds, post-callback ctl reload and untouched fields.
Allocator0x12af0 stays external; no Init registration or game execution.

Exact commands: cmake -S native -B local/build/task2-release -G Ninja -DCMAKE_BUILD_TYPE=Debug
-DSOTC_SANITIZE=ON; cmake --build local/build/task2-release --target test_shared_str_release_f590
-j6; ASAN_OPTIONS=detect_leaks=0 ctest --test-dir local/build/task2-release
-R '^shared_str_release_f590$' --output-on-failure; ASAN_OPTIONS=detect_leaks=0 python3 -I
tools/lift/check_ff590.py --elf '/Volumes/Extra/SoC Mac Native/local/re/eboot.elf'
--native '<managed-worktree>/local/build/task2-release/test_shared_str_release_f590'.
All exit0; focused1/1,10 independent byte-oracle vectors each with2 register poisons. Five
native mutants compile but both native fixture and independent oracle reject every one.
Exact mutant argv: local/validation/task2-release/mutants.json.

REA default Ghidra startup timed out330000ms, returned no code Evidence ID. A queued wrapper
request was drained on close; its outcome was not observed. Empty exported bundle has0records;
REA observation remains UNAVAILABLE. Direct Ghidra recovery command python3 -I
local/validation/task2-release/run_ghidra.py exits0 in3.807s, uses existing project readOnly/
noanalysis,8G and no import/config change. Exact argv/env/times in ghidra-run.json.
Evidence ghidra-f590-evidence.txt SHA256
61fbad04cc41cbf81131adba1ea576d3f3e26d521c89cc0fd996fc65e8147778 verifies original SHA,
image base0,73B helper/10B wrapper,26/2 instructions and no computed flows in those ranges.
Private full report: .superpowers/sdd/2026-10-22-codex-parallel-foundations-plan/task-2-report.md;
artifact/evidence hashes in local/validation/task2-release/manifest.json. Remaining risks: external
allocator behavior, native field mapping inference; no gameplay/TSan/leak-detection proof.

## AUD-02-NGS2-RACKQUERY-RDX-CFG-01

Worker verified; independent review pending. Two failed replay hypotheses; third succeeded.
New scripts aud02_rackquery_cfg.py and test_aud02_rackquery_cfg.py; original args tool unchanged.
Exact synthetic command python3 -I scripts/test_aud02_rackquery_cfg.py exits0,20/20.
Successful command (managed worktree):

```
python3 -I scripts/aud02_rackquery_cfg.py --baseline '/Volumes/Extra/SoC Mac Native/local/validation/aud02-root-replay/rackquery-args.json' --cache '/Volumes/Extra/SoC Mac Native/local/re/evidence/aud02-ngs2-rackquery-buffersize-01/function-0094ebc2/function-94ebc2.stdout.json' --elf '/Volumes/Extra/SoC Mac Native/local/re/eboot.elf' --out .superpowers/sdd/2026-10-22-codex-parallel-foundations-plan/task-3-private/run-3.json --preserve-register rbx --preserve-register rsp
```

Repeat differs only in output path run-3-repeat.json; both exit0; cmp exits0, output SHA256
af0d97de0fca93868eb17168ce04694894d1d027c886e02a6469df22cd33e887. Both prior failed commands
and their exact presentation differences/ruling remain in task-3-report.md and private logs.
1255/1255 cached instructions and15/15 baseline sites byte-check;109 blocks;14 cached/1baseline-only.
All11 previously unresolved sites remain unknown in strict mode. Explicit RBX/RSP preservation
assumptions produce candidates WITH unknowns for all11, six LEA roots; none is proved pointer/ABI.
Two indirect-successor sets and one terminal fallthrough remain uncertain. Public files contain
only generic tooling/synthetic fixtures; raw code/CFG/per-site reports remain private.
Full report and manifest under .superpowers/sdd/2026-10-22-codex-parallel-foundations-plan/.

## Constructor scout

Read-only diagnosis complete,0 implementation attempts/checks. At ff7b186 the two failing tests
still expect pre-slot23 blocking. dfdc2f6(slot23 callback) and0bbc1a8(24–47) are ancestors; no
new source task changed startup_ctors or these tests. CMake omits the private fixture environment
for the two tests. Candidate CPU-01-CTOR-REGISTRATION-EXPECTATIONS-01 would update only those
two tests and root-owned CMake env entries to assert actual full completion while retaining
all existing explicit missing-fixture, null-callback, generic failure and foreign-descriptor gates.
No change is authorized until the principal's separately recorded clean-baseline check finishes.

Next actions: scoped spec/quality reviews for Tasks2/3; baseline isolation, bounded reconciliation
if justified; root integration evidence then normal publication and approved metadata cleanup.
G0/G1 PARTIAL; G2–G6 NOT RUN. No boot/audio/real-save/Metal gameplay claim.
