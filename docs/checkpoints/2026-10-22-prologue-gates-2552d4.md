# Prologue debug gates `0x2552d4` (G1 PARTIAL)

- Date/time: 2026-10-22 ~11:30 CDT (America/Chicago).
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9 (Drive).
- Task: `CPU-01-PROLOGUE-GATES-2552D4-01`; gate: **G1**; status: **PARTIAL** (not PASSED).
- Failed hypotheses/tries: 0.
- Tip: `f5bdcb0` on `cursor/init-hash-551140-9dfc` (PR #4). Private capture
  `manifest.json` sha256 `11416201…`; `capture.py` `8622b026…`.

## Triage

Prefer debug gates after `0x2552cd`: always-on prologue epilogue on measured Init path.
Chose over vslot / `0xf08` / ensure migrate / non-null list.

## Claim boundary

**Claimed:** OBS `0x2552d4..0x255318` (68 B) control flow + empty-path `0xba40` (173 B to
first ret): cells `0x2dfce70` / `0x2dfcf40` / `0x2eec840`; a30 cell `0x2dfd4b8` vslot+0x10
hooked. Wired as default at end of `run_prologue_255180_tail`.

**Not claimed:** stack-chk PLT `0xf8f3a0`; ba40 non-empty listener invoke / `+0x1c` branch;
bodies of vslot+0x48 / a30 vslot+0x10; ensure migrate; non-null list; app vslots; `0xf08`;
G1 PASSED; PR #5 merge.

## Private evidence

`local/validation/appmain-init-prologue-gates-20261022/`

```sh
python3 -I local/validation/appmain-init-prologue-gates-20261022/capture.py
```

## Observed

| Step | Result |
|---|---|
| ELF | `check_f2552d4_gates.py` OK |
| CTest | `appmain_init_ctor_98a90_slice` PASS |
| Measured | gate_byte=0 (flag≠0); ba40 empty; 275 registry |
| Pairs | still **155** |

## Next exact action

App vslot `0x534130` / `0x533100`, or `0xf08`, or ensure migrate / non-null list.
Goal remains open.
