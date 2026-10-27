# F590 replacement mutation set accepted at fixture scope

Local date/time: 2026-10-27 00:22 CDT (America/Chicago). Integrator/principal: `/root`.

Task: `CPU-01-SHARED-STR-RELEASE-F590-01`, parallel-foundations plan Task 2.

## Principal reassessment

The written plan requires meaningful mutants but does not name the lost original five as fixed
cases. The 2026-10-25 recovery checkpoint explicitly leaves the task active until either those
cases are recovered or a replacement plan is separately reassessed. I accept the E691 replacement
set as satisfying the written meaningful-mutants criterion for this bounded fixture task. This is
not a claim that the replacements are equivalent to, or replay, the missing original five.

## Evidence checked

- Live hashes match the E691 report: `native/core/shared_str_release_f590.cpp`
  `91741a8ff959dc7d8a37da026b930145d04551a39c294bb778b15ce8c5b37e45`,
  `native/tests/test_shared_str_release_f590.cpp`
  `7e6e1dac8d523fa1137e94b0f9f1a2752a0d961af08d2a5bc9ebde723575fce1`, and
  `tools/dev/f590_replacement_mutation_check.py`
  `2869b988cd0fff3872596f24c4944845f9a7c99f13f756fe9673e81a07e72ac4`.
- Existing private report `local/validation/f590-replacement-mutants-20261025-rv/report.json`
  SHA-256 `7b639eccc8cd5a2cee2e3930f9247c61d0e2a0841563764ea6eea24fb83b5b6f` labels itself as a
  replacement set. Its control passed, the pinned byte oracle exited 0, and **12/12** mutants were
  killed: 11 by the sanitized unit test and `M02_relaxed_ordering` only by disassembly-ordering.
  The report records the tracked tree untouched. E691 retains the original command and earlier
  run results; this closeout did not rerun tests.
- The mutation cases cover skipped/misordered atomic release, incorrect prior-count comparisons
  including zero/negative/wrap cases, missing deallocation, wrong kinds, missing control reload,
  null-control handling, and clearing fields that must remain untouched. The fixture also covers
  callback clearing/replacing `ctl`, ordering/kinds, and concurrent decrements as recorded in E680
  and E691.

## Limits and status

`M02_relaxed_ordering` is not detected by the single-threaded test or byte oracle. Its kill depends
on this pinned arm64 toolchain lowering the control to one `ldaddal` and the relaxed mutant to
`ldadd`; toolchain changes must fail visibly and be reassessed. The original five mutants remain
unrecovered and were not tested. The `0x12af0` allocator remains an external dependency. This is
**FIXTURE_VERIFIED** for the F590 leaf acceptance only; parent `CPU-01`, G1, native startup, and
game boot/audio/save gates remain open. No source or test was changed here and no game executable
was run.

Exact root review commands:

```sh
shasum -a 256 tools/dev/f590_replacement_mutation_check.py native/core/shared_str_release_f590.cpp native/core/shared_str_release_f590.hpp native/tests/test_shared_str_release_f590.cpp local/validation/f590-replacement-mutants-20261025-rv/report.json
python3 - <<'PY'
import json
p='local/validation/f590-replacement-mutants-20261025-rv/report.json'
d=json.load(open(p)); rs=d['results']
assert d['tracked_tree_untouched']
assert rs[0]['result']=='PASS' and rs[0]['oracle']['exit']==0
assert len(rs[1:])==12 and all(r['result']=='KILLED' for r in rs[1:])
print('control PASS; 12/12 replacement mutants KILLED; tracked tree untouched')
PY
```

Both exited 0; report hash and live source hashes are listed above. Project index, publish-path
audits, and owner attribution audit are recorded in the corresponding `HANDOFF.md` checkpoint.
