# Checkpoint — list-A reader and drain found inside `0x1bdf30`; duplicate risk closed

- Local: 2026-10-23 08:4x (America/Chicago)
- Agents: static-RE worker; integrator byte replay
- Task: `CPU-01-APPINFO-LIST-READER-01`
- Gate: **G1 PARTIAL — static + fixture only**
- Evidence: E569

## The reader

The list-A reader is **not a separate function**: it is the second half of the dispatcher
`0x1bdf30`, at `0x1be14d..0x1be222` (byte-pinned by `tools/lift/check_fapp_info_collect.py`).

```
1be14d  mov rbx,[r13]        ; r13 = container base (list A), reloaded at 0x1bdfca
1be151  cmp rbx,r13
1be196  mov r12,[rbx+0x30]   ; value holder
1be1f2  mov rdx,[rbx+0x10]   ; name holder
1be215  mov rbx,[rbx]        ; advance
```

It serializes **every** entry into a report object (`0xab1b20` FieldEntry, `"name"` literal at
`0xfb3e34`) and stores the report into `[game+0x48]` (`0x25dba8`). There is **no comparison key and
no lookup**: no code anywhere compares a list-A name against a literal. `0x2ee68c8` has exactly
five references image-wide (`0x12a5`, `0x1c00e0` atexit, `0x256fad`, `0x25db80`, `0x25dc6c`), and
zero immediates/data pointers/RELA addends.

## The drain

`0x1be3f4..0x1be456` deletes every list-A node whose `dword [node+0x50] == 1`:

```
1be400  cmp dword [r14+0x50],1 ; jne advance
1be407  r14 = [r14]            ; advance first
1be40a  rbx = [r14+8]          ; victim = prev->next
        unlink victim
1be424  call 0xf590(victim+0x30)
1be42d  call 0xf590(victim+0x10)
1be43a  call 0x170c0(victim)
1be43f  dec qword [r12+0x10]
```

So the flag split is now fully explained: **`0x1bdf30` deletes `flag == 1` (live app-info entries)
after serializing them; `0x1bdeb0` deletes `flag == 0` (none produced).** This closes the E561
"flag == 0 producer not found" anomaly.

## Re-entry / duplication

Dispatch is gated at `0x25db56` on `*(0x2eed610+0x60) != 0`; `0x2eed610` is the console command
**`snapshotBugData`** (registered `0x263c99..0x263cf5`). It is not per-frame in the normal case. Even
if it runs twice, it cannot duplicate entries: every call drains the flag==1 list. The earlier
"second dispatch would duplicate" risk is **withdrawn**.

The other dispatcher caller `0x1be5cf` is dead (container `0x1be5a0` has no callers/relocations).

## List B

List B (`+0x20`) is written by `0x1bdc80` with **0x50 B nodes** only when `arg2 != 0`, read at
`0x1be2f7..0x1be3e0`, and cleared at `0x1be49d`. This is a third node size; the container dtor's
list-B loop only touches `+0x10/+0x30`, which is still layout-compatible, but the 0x58 `Node1bdeb0`
type is not the right type for list B. Recorded as open.

## Native work

`native/core/app_info_collect.{hpp,cpp}` models the list-A half: FIFO visit of every entry, then
drain `flag == 1` with the recovered advance/unlink/release/free/count order. `RegistryOwner::collect`
delegates to it. Fixture `app_info_collect` covers mixed flags, second-call empty list, missing
hooks, and a last-reference release failure. Checker `check_fapp_info_collect.py` pins the reader,
drain, flag test, two `0xf590` calls, `0x170c0` free and the 64-bit decrement.

## Verification

`app_info_collect` passes ASan+UBSan; `check_fapp_info_collect.py` exits 0 on the pinned ELF. Full
round counts are in the round-25 verification checkpoint.

## Limits

The report/FieldEntry construction, list-B read/clear, `0x1bdc80` and the `snapshotBugData` gate are
not native. **G1 PARTIAL.**
