# Checkpoint — full body analysis of Init `0x1290` (static only)

- Local: 2026-10-22 ~23:30 CDT (America/Chicago)
- Agent: `subagent-init1290-body-analysis` (CPU-01 analysis worker; no integrator files touched)
- Task: `CPU-01-INIT-1290-BODY-ANALYSIS-01` (attempt 1)
- Gate: **G1 PARTIAL — static body characterisation only, not PASSED**
- Evidence: `local/validation/init-1290-body-20261022/` (private, gitignored)
  `report.md`, `manifest.json`, `fn1290.s`, `fn1290.bin`, `tools/analyze.py`, `tools/elfwrap.py`

Method: `scripts/ps4_dynlink.py` byte reads from `local/re/eboot.elf`
(sha256 `d0723afb…` re-verified on read) + `/opt/homebrew/opt/llvm/bin/llvm-objdump` on a
minimal ELF wrapper of the byte span. Nothing was loaded, relocated or executed. No REA/Ghidra,
no installs, no downloads. Extracted bytes were treated as data only.

## OBS (byte-verified)

| Fact | Detail |
| --- | --- |
| Extent | `0x1290..0x1353` excl. = **195 B**; fn sha256 `185f03d382259a5e…`; 12 B nop pad before, 13 B after; next unit starts `0x1360` |
| Callers | exactly **one** `e8`-pattern site in `.text`: `0x531e03` (Init `0x531d60`), matching the existing 5 B stub; `rax` discarded (`0x531e08` is a `call`) |
| Frame | `push rbp; mov rbp,rsp; push r14; push rbx; sub rsp,0x80` → **0x90 B** total incl. pushes, `0x80` scratch, deepest local `rbp-0x88`; canary slot `rbp-0x18` |
| Blocks | 5 contiguous blocks covering all 195 B: B0 `1290-12fa` (106 B), B1 `12fa-1339` (63), B2 `1339-1342` (9), B3 `1342-134e` (12), B4 `134e-1353` (5); per-block sha256 in `manifest.json` |
| CFG | straight line; single `ret` at `0x134d`; fail edge `jne 0x134e` → `call 0xf8f3a0` (no `ret`). No loops, no jump table, no unwind helper |
| Direct calls | `0x12f5→0x1bde30`, `0x1334→0x21cd0`, `0x134e→0xf8f3a0` (`__stack_chk_fail`) |
| Indirect calls | exactly one: `0x1301 callq *0x18(%rax)`, `rax = [rbp-0x28] = 0x1594060`; slot `0x1594078` has `R_X86_64_RELATIVE` addend `0x19f0`, and `0x19f0` is a bare `ret` |
| Globals (all 8 RIP-relative operands) | `0x15fe518` (guard), `0x2ee68c8` (lea+load), `0x15fe5c0` (load), `0xb10` (lea of code), `0x1594060` (lea of table), `0x2dfcbc8` (lea+load), `0x15fe618` (load), `0xbb0` (lea of code). No other absolute operands |
| Guard cell | `0x15fe518` == imported `__stack_chk_guard` (`libkernel`, OBJECT, nid `f7uOxY9mM1U`, `R_X86_64_64` sym 525). **Two reads of the word through one cached pointer (`0x129e` slot→`r14`, `0x12c3`, `0x1339`), one spill, one `cmp`, zero writes.** Cannot modify the guard |
| Return value | **`void`**. Success path leaves `rax` = live guard word at `0x1339` (canary, not a result); fail path returns nothing. No incoming register is read (`rdi` first written `0x12df`, `rsi` `0x12bb`, `rdx` `0x12b4`; no `[rbp+0x10]` read) → ABI-clean `void f(void)` |
| Descriptor layout | both stack descriptors are 0x20 B: `+0x00 = &self`, `+0x08 = 0x1a00` / `0x1a10` (via reloc'd `0x15fe5c0`/`0x15fe618`), `+0x10 = 0`, `+0x18 = 0xb10` / `0xbb0` (code addresses) |
| Slot A extras | `-0x48` (A+0x20) is **never written here**, only read as `rdi` of the indirect call; `A+0x40 = 0x1594060`, `A+0x48 = 0` |
| Static slots | `0x2ee68c8` / `0x2dfcbc8` are BSS (**no relocation**), referenced **only by `lea`** (5 / 28 sites). `0x1c00e0` is `__cxa_atexit(0x1bdae0, &slot, 0x1604218)` after zero-init; `0x256fd2` stores a `new(0x60)` object into the first slot |

## INF (inference, graded)

- **High** — `0x1290` is a *registration* function: it builds two 0x20 B `{ctx, invoke, 0, fn}`
  descriptors and hands them to two engine singletons: `0x1bde30(this = *(0x2ee68c8), &A)` and
  `0x21cd0(this = *(0x2dfcbc8), &B)`; between them it calls the no-op 4th entry of the table at
  `0x1594060` with `this = A+0x20`.
- **High** — registration **copies**: `0x1bde30` → `add rdi,0x40` → `jmp 0x1beeb0` →
  `operator new(0x60)`; sibling `0x1bdeb0` walks a circular list whose sentinel is the object
  itself (`r15=[this]; while (r15 != this) r15=[r15]`).
- **Medium** — `0x1a00` / `0x1a10` (and sibling `0x1990`) are argument-shuffling trampolines for a
  single type-erased callable ABI (`fn` at `+0x18`); the exact consumer signature is unproven.
- **Medium** — callback bodies `0xb10` / `0xbb0` are themselves registration/lookup steps: both
  build `{ptr,len}` (string-view shaped) pairs and call `0x1bdb50`. Their rodata operands sit at
  `0xf919xx` / `0xf91cxx` (title + mode-key + debug-menu labels); **strings deliberately not
  copied into this checkpoint** — they are in the private `report.md` only.
- **Low** — `0x21cd0` is the sibling registrar for the second singleton; it pins two code
  pointers (`0xf95c06`, `0xf95df0`) plus int `0xea` into a 0x20 B local and forwards it to
  `0x3b390(obj+0x58, …)`.

## Coverage and limits

- Covered: 100 % of the 195 bytes (full linear disassembly, no undecoded bytes), all branches,
  all direct/indirect call sites, all RIP-relative operands, frame, ABI, guard pattern, return
  semantics, callee classification, xref census for 7 target addresses.
- Not covered: bodies of `0x1bde30`, `0x1beeb0`, `0x21cd0`, `0xb10`, `0xbb0`, `0x3b390` (read
  only ~30 instructions each, enough to classify); the class identity of the two singletons; the
  boundary of the `0x1594060` table (4-entry vtable + separate name array vs one 8-slot struct);
  whether `*(0x2ee68c8)` is non-null at `0x531e03` on the measured path.
- Scan limits: the xref census finds only instructions whose last 4 bytes are the `disp32`
  (misses `48 c7 05 disp32 imm32`-style stores); the caller census is an `e8`-pattern scan, so it
  cannot see an address-taken / indirect caller. Both are stated as evidence, not proof.
- **No runtime claim**: nothing executed; nullness, ordering and the no-op vcall are
  static facts about the pinned bytes only.
- Callee lift status: **none** of `0x1290`, `0x1bde30`, `0x21cd0`, `0xb10`, `0xbb0`, `0x1a00`,
  `0x1a10`, `0x1594060`, `0x2ee68c8`, `0x2dfcbc8` (or the `0x1bd***` family) is lifted in
  `native/core/`. `0xf8f3a0` exists only as a hook name (4 `*.hpp`); `0x1bde30` appears once as
  a prose comment.

## Proposed native design (ordinary native function, no guest state)

```cpp
// native/core/init_1290_registrations.hpp  (PROPOSED; nothing created by this task)
struct HandlerDescriptor {          // OBS 0x20 B, both descriptors
    void*         ctx;              // +0x00 = &self
    void        (*invoke)();        // +0x08 = 0x1a00 | 0x1a10
    std::uint64_t reserved;         // +0x10 = 0
    void        (*fn)();            // +0x18 = 0xb10 | 0xbb0
};
struct HandlerEntryA {              // OBS descriptor A's frame span is 0x50 B
    HandlerDescriptor desc;         // +0x00
    void*             self_arg;     // +0x20  (callee-written; rdi of the vcall)
    std::uint64_t     unused_28;    // +0x28  (not written by 0x1290)
    std::uint64_t     zero_30;      // +0x30 = 0
    std::uint64_t     unused_38;    // +0x38  (not written by 0x1290)
    const void*       table;        // +0x40 = 0x1594060
    std::uint64_t     zero_48;      // +0x48 = 0
};
struct Init1290Hooks {
    void (*insert_1bde30)(const HandlerDescriptor&);  // owner = *(0x2ee68c8)
    void (*vcall_1594060_slot3)(void* self);          // receives &A + 0x20
    void (*insert_21cd0)(const HandlerDescriptor&);   // owner = *(0x2dfcbc8)
};
// OBS order: A -> insert -> vcall -> B -> insert -> ret. Hooks are mandatory (null => FAIL).
[[nodiscard]] Init1290Result init_1290(const Init1290Hooks&) noexcept;
```

Fixture rules: three effects, that order, once each; `vcall` must receive `&A.self_arg`
(`&A + 0x20`), not `&A`; `void` return; guard check intentionally not modelled (it can only
abort, so it is not observable behaviour of a correct lift, and must not become a fake success
path).

## Top 3 blockers to lifting `0x1290` natively

1. **No native owner for the two singletons.** `*(0x2ee68c8)` and `*(0x2dfcbc8)` are BSS owning
   pointers (zero at load, filled by an installer elsewhere) to 0x60 B objects owning intrusive
   list sentinels at `+0x00`/`+0x40`. Lifting `0x1290` without the `0x1bd***` family
   (`0x1bdae0` dtor, `0x1bdb10`, `0x1bdb50`, `0x1bde30`, `0x1bdeb0`, `0x1bdf30`, `0x1beeb0`
   insert) means linking against a subsystem that does not exist, and the nullness of the slots
   at `0x531e03` is unproven.
2. **Descriptor ABI / copy size is inferred, not proven.** `{ctx, invoke, 0, fn}` is solid, but
   A's total span (0x50 vs 0x20) and whether the consumer copies 0x20 or 0x50 bytes is a guess;
   the `0x1594060` table boundary has two competing readings (4-slot vtable + separate name array
   vs one 8-slot descriptor). One more same-class vtable data point, or one runtime read of
   `0x1594060..0x15940a0`, settles both.
3. **The registered callbacks are not modelled.** `0xb10` and `0xbb0` are themselves
   registration/lookup steps whose bodies (`0x1bdb50`, 0x510 B frame for `0xbb0`) are unlifted.
   A lift that registers function pointers to unimplemented behaviour will diverge the first time
   the dispatcher fires, so the descriptor identity must be settled before wiring.

Non-blocker: the `__stack_chk_guard` path is fully understood (read-only, two reads, no writes)
and can be dropped from the lift.

## Exact next action

Lift **`0x1bde30` → `0x1beeb0`** (the insert path: `add rdi,0x40` + `operator new(0x60)` copy) as
the next bounded task — it is the one call that decides whether the 0x20 B descriptor or the
0x50 B span is copied, and therefore settles blocker 2 for descriptor A. If that is unavailable,
instead pin the `0x1594060` table boundary by locating a second vtable of the same class whose
4th slot is not a bare `ret`.

## Rows to add to `docs/INDEX.md` (integrator action — not edited by this worker)

```
| [Init 0x1290 body (2026-10-22)](checkpoints/2026-10-22-init-1290-body-analysis.md) | 195 B full body, guard read-only, void, 1 indirect no-op vcall, 2 descriptor registrations; E516; G1 PARTIAL |
```

and, under the evidence ledger, a matching `E516` entry summarising: ELF `d0723afb…`, fn sha
`185f03d382259a5e…`, 195 B at `0x1290`, block shas in the private manifest, no callee lifted,
gate G1 PARTIAL (static only, nothing executed).

## Risks

- Treating the inferred descriptor ABI as fact would put unverified field widths into `native/`.
- The two registrations are unconditional and ordered; if a later task models them in reverse
  order the fixture must catch it.
- Fixing the `0x1290` call-site stub to "call the body" before blockers 1–3 are resolved would
  replace one bounded stub with three unbounded dependencies.
