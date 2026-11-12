# Startup slot 554 (`0x6efe0`) — decomposition through batch 13

Status: **COMPLETE for the slot 554 model; batches 1, 2a, 2b, the 2b continuation, and 3–13 are modelled and verified. Batch 13 is green under the full checker sweep.** The whole body is `[0x6efe0,0x8a618)` = 112,184 B (FDE end), 19,550 reachable
instructions, 2,162 direct calls, 436 atexit rows, 415 registrations, 436 object starts and 32
callees. No original RVA is executed.

## Landed batch 1 (evidence)

- Body extent `[0x6efe0,0x6f793)` = `0x7b3` B; sha256
  `483de6397f25b7f9f5038b4a00e61191e3399acdcec76d1ff98700dd804a3e47`.
- Page `0x2ea8ee8..0x2ea99b0` = `0xac8` B, initialized by `0x53fc0` (2974 B, 221 straight-line
  instructions, **187 stores**, no calls/branches). **2113 written / 647 holes**.
- Eight variables `0x2ea99b0..0x2eaa010` = `0x660` B:
  `backgroundColor` (int), `gpuMarkerLevel` (int), `globalRenderVarX/Y/Z/W` (floats),
  `profileEnableGpu`, `groupCulling` (bools). **1314 written / 318 holes**.
- Batch total `0x1128` B: **3427 written / 965 holes**; 8 registrations, 8 atexit rows,
  16 inert `0xf590` releases, 6 functor destroys.
- Constructors: `0x3a980` (int), `0x6aa00` (float), `0x20af0` (base), `0x20d60` (register),
  final vptrs `0x1594438` / `0x15943c0` / `0x1594348`; dtors `0xad70` / `0xad30` / `0xacf0`.
- Module `native/core/ctor_slot_554_6efe0.{hpp,cpp}` (page stores in
  `ctor_slot_554_page_init.inc`), fixture `native/tests/test_ctor_slot_554.cpp`, oracle
  `tools/lift/check_ctor_slot_554.py` (50 operand pins, body/page/callee hashes, 129 ELF + 32
  native mutants; both tamper modes exit 1).

## Landed batch 2a (evidence)

The original plan called batch 2 a uniform 36-object group. The ELF refutes that: objects 0..4 are
five `0xc0`-byte bools, and object 5 is a `0xd0`-byte `0x6abb0` ranged/functor object. Batch 2 was
therefore split. This landed prefix is the clean five-bool group; the remainder is 2b.

- Body extent `[0x6f793,0x6fc29)` = `0x496` B; sha256
  `adfcc16736d8440b779af9cf4b80b44e1acbf380aba41184fdd0d5faab94ec6c`; slot-table word
  `0x2dd70e0 - 554*8 = 0x2dd5f90` still holds `0x6efe0`.
- Five objects `0x2eaa010..0x2eaa3d0` = `0x3c0` B, each `0xc0`:
  `threadedCulling` (15, page+`0x12`), `threadedRendering` (17, `0x13`),
  `threadedSumbit` (14, `0x14`), `enableAsyncCompute` (18, `0x15`),
  `enableGPUCounters` (17, `0x16`).
- Body shape: 209 instructions, 25 direct calls — `0x20af0` x5, `0xf590` x10, `0x20d60` x5,
  `0xf8f390` x5 — and **40 direct stores**. No indirect call and no canary in this prefix.
- Per object: base `0x20af0`, vptr `0x15940b0`, value pointer to the page cell, cleared
  `+0x69/+0x70/+0xa0`, functor tag `0x1596898`, description release, final vptr `0x1594348`,
  byte from the page at `+0x68`, `0x20d60`, name release, `atexit(0xacf0)`.
- **162 written / 30 holes per object; 810 written / 150 holes for the batch.** Batch-1 plus
  batch-2a is `0x14e8` B and **4237 written / 1115 holes**; 13 registrations and 13 atexit rows
  are modelled.
- Module `native/core/ctor_slot_554_2a_6f793.{hpp,cpp}`, fixture
  `native/tests/test_ctor_slot_554_batch2a.cpp`, oracle
  `tools/lift/check_ctor_slot_554_batch2a.py` (30 operand pins, body/callee hashes, 81 ELF + 29
  native mutants; both tamper modes exit 1). The shared checker harness gained an optional
  `table_word` field so a sub-batch can name the slot entry separately from its body base; batch 1
  is unchanged.

## Landed batch 2b prefix (evidence)

Batch 2b is the mixed block after the five-bool group. The first eight objects are the `XPR
Occluder Debug Mode` wrapper object plus seven more bools; the prefix is clean because the next
object's `leaq` starts at `0x7036d`. The remaining 23 objects are **not** claimed here.

- Body extent `[0x6fc29,0x7036d)` = `0x744` B; sha256
  `9244a7d9ba3a205fcbe095c324106ac32c3dc2ab9f362257a1d2def3f4066d9b`; slot-table word
  `0x2dd5f90` still holds `0x6efe0`; pad byte at `0x7036d` is the next object's `leaq` opcode, not
  an alignment byte.
- Object 0 `0x2eaa3d0` (`XPR Occluder Debug Mode`, 23 bytes at `0xf9a8bb`, `0xd0` bytes):
  base `0x20af0`, page cell `+0x18`, low 0 / high 2, flag 1, page dword at `+0x68`, final vptr
  `0x15968e8`, `atexit(0x54b60)`. It is built by the `0x6abb0` outer wrapper (`0x18d` B,
  sha256 `242aa9fe5b26a4dca9060f681303fc14a074cb9f08299e264452c207e6ed39ad`) and its `0x6ad40`
  inner (`0x160` B, sha256 `e4e6ec135d1f3af7fa2163fc8b0d5657b0d5aa404d494d0533d569289dcf8c9b`).
  The wrapper family pins are this entry's own: caller/tag `0x15968b9`, inner mid table
  `0x1595e80` (address point `0x1595e90`), outer final table `0x15968d8` (address point
  `0x15968e8`), register `0x20d60`, canary cell `0x1593968`, stack-check fail `0xf8f3a0`. The model
  captures the five type-erased transfer stages, the view-destroy call and the registration/atexit
  sequence; it does not execute the wrapper.
- Objects 1..7 at `0x2eaa4a0..0x2eaa9e0` (seven `0xc0` bools):
  `enableNonPower2DepthOccluders` (29, page+`0x1c`), `dumpXPRInfoStats` (16, `0x1d`),
  `useSunShadowIndices` (19, `0x1e`), `doIndexBufferCulling` (20, `0x1f`),
  `doSunShadowIndexBufferCulling` (29, `0x20`),
  `enableSmallPrimitiveIndexBufferCulling` (38, `0x21`),
  `forceUseLastIndexBufferCullingResults` (37, `0x22`). Their constructors are the batch-2a bool
  shape: base `0x20af0`, mid vptr `0x15940b0`, final vptr `0x1594348`, functor tag `0x1596898`,
  `atexit(0xacf0)`.
- Body shape: **328 instructions**, **39 direct calls** (`0x6abb0` x1, `0x20af0` x7, `0xf590` x16,
  `0x20d60` x7, `0xf8f390` x8), one indirect `call [rax+0x18]` view destroy, **63 direct stores**
  into the seven bools and **54 stack stores** for constructor arguments. No conditional branch
  and no canary in this prefix.
- Object 0 is **195 written / 13 holes** (`+0x19..+0x1b`, `+0x71..+0x73`, `+0x79..+0x7f`); each
  bool is **162 written / 30 holes**; the prefix is **1329 written / 223 holes** of `0x610`.
- Module `native/core/ctor_slot_554_2b_6fc29.{hpp,cpp}`, fixture
  `native/tests/test_ctor_slot_554_batch2b.cpp`, oracle
  `tools/lift/check_ctor_slot_554_batch2b.py` (81 operand pins, body/wrapper/inner/callee hashes,
  193 ELF + 49 native mutants; both tamper modes exit 1).
- Batches 1 + 2a + 2b accumulate `0x138d` body bytes and `0x1af8` object bytes, **5566 written /
  1338 holes**, 21 registrations and 21 atexit rows.

## Landed batch 2b continuation (evidence)

This closes batch 2b: the 23 objects after the eight-object prefix, `0x2eaa9e0..0x2eabb80`
(`0x11a0` B), body `[0x7036d,0x718bb)` = `0x154e` B, sha256
`5d14edc8d2d0985e0a523e6fa8b25f313b4924c338d75dc1e82f05f740421ca0`; the slot-table word
`0x2dd5f90` still holds `0x6efe0`. 955 instructions, 111 direct calls, six indirect
`call [reg+0x18]` functor destroys, 332 stores (157 object + 175 stack), no conditional branch.
Families: four `0xd0` wrapper-family objects (`ShowRenderTarget` and `Show Ambient RenderTarget` on
`0x6af60`/`0x6b0f0`, `ShaderDebugCollisionMode` on `0x6b2f0`/`0x6b480`, `ShaderDebugMode` on
`0x6b680`/`0x6b810`), one `0xd0` unbounded int (`RenderTargetDisplayMip`, `0x3a980`), one `0xd0`
bounded float (`miscDebugShaderFloat0`, `0xc300`, arg-fn alias `0x1596878`), seventeen `0xc0`
bools at page+`0x0..0x8`/`0x4c..0x50`/`0xa8d..0xa8f`. Byte maps: wrapper **195 written / 13
holes**, int **165 / 43**, float **171 / 37**, bool **162 / 30**; batch **3870 written / 642
holes**. ELF checks correct the draft in three places: `Show Ambient RenderTarget`'s low bound is
`0xf` (not 0); `0x6b2f0` and `0x6b680` publish `0x1596a18` / `0x1596ab0` (table + `0x10`), like
`0x6af60`; and `0x1596878` is accepted only because its four relocations resolve to the same
`0x15d57e0`/`0x1594318` helpers — it is now a native alias in `concmd_vars`.
Module `native/core/ctor_slot_554_2b_cont_7036d.{hpp,cpp}`, fixture
`test_ctor_slot_554_batch2b_cont`, oracle `check_ctor_slot_554_batch2b_cont.py` (274 operand pins,
body/callee hashes, 621 ELF + 48 native mutants; both tamper modes exit 1). The spec-table rows for
`0x6b2f0`/`0x6b680` carry `0x1596a18`/`0x1596ab0` (table + `0x10`); the fixture and oracle pin those
final vptrs and all four wrapper bounds. Batches 1 + 2a + 2b are `0x28db` body bytes and `0x2c98`
object bytes, **9436 written / 1980 holes**, 44 registrations and 44 atexit rows.

## Landed batch 3 (evidence)

The 36 objects `0x2eabb80..0x2ead6b0` (`0x1b30` B), body `[0x718bb,0x739e8)` = `0x1b30` B,
sha256 `1f981b72ce2bddb02b4e8215358dc272877dcf95c5f93af522dae2762dc4c91d`; the slot-table word
`0x2dd5f90` still holds `0x6efe0`. 1,494 instructions, 179 direct calls, three indirect
`call [reg+0x18]` destroys, 472 stores (303 object + 169 stack), no conditional branch. Families:
thirty-three `0xc0` bools, two `0xd0` bounded floats (`particleTimeScale` page+`0x74` bounds
`0..2.0f`; `particleWaterFogScale` page+`0x78` bounds `0..10.0f`) and one `0xd0` wrapper
(`showParticle_Stats`, `0x6ba10` outer / `0x6bba0` inner, final vptr `0x1596b48`, tag `0x1596b18`,
inner vptr `0x1596070`, dtor `0x54ca0`, page+`0x8c`, bounds `0..3`). Byte maps: bool **162/30**,
float **171/37**, wrapper **195/13**; batch **5,883 written / 1,077 holes**. The ELF corrects the
inherited span: the 36 constructions end at `0x2ead6b0` (batch 4's first object), not `0x2ead5f0`.
Module `native/core/ctor_slot_554_batch3_718bb.{hpp,cpp}`, fixture `test_ctor_slot_554_batch3`,
oracle `check_ctor_slot_554_batch3.py` (366 operand pins, body/callee hashes, 821 ELF + 56 native
mutants; both tamper modes exit 1). Batches 1 + 2a + 2b + 2b-cont + 3 are `0x440b` body bytes and
`0x47c8` object bytes, **15,319 written / 3,057 holes**, 80 registrations and 80 atexit rows.

## Landed batch 4 (evidence)

The 36 objects `0x2ead6b0..0x2eaf2a0` (`0x1bf0` B), body `[0x739e8,0x75d22)` = `0x233a` B,
sha256 `7efb56aa8a91ebf7aea65f47da73d66972f52fe7c074346fd9ed7ebc4526ea06`; the slot-table word
`0x2dd5f90` still holds `0x6efe0`. 1,568 instructions, **178 direct calls + 15 indirect**
(thirteen `call [reg+0x18]` functor destroys and two wrapper view destroys), **522 stores (215
object + 307 stack)**, no conditional branch. Call histogram: `0xc300` x12, `0xc580` x1, `0x20af0`
x21, `0x20d60` x34, `0x6bda0` x1, `0x6c130` x1, `0xf590` x72, `0xf8f390` x36. Families: twenty-one
`0xc0` bools (base `0x20af0`, mid `0x15940b0`, final `0x1594348`, tag `0x1596898`, atexit `0xacf0`);
twelve `0xc300` bounded floats (final `0x15943c0`, arg-fn `0x1596878`, atexit `0xad30`); one
`0xc580` bounded dword `particleNumShadowCascades` (page+`0x80`, bounds `0..4`, mid `0x15941a0`,
final `0x1594438`, class tag `0x15944a0`, arg-fn `0x1596858`, atexit `0xad70`); and two `0xd0`
wrappers — `particleResolution` (`0x6bda0` outer 0x18c sha `677c9581…` / `0x6bf30` inner 0x15f sha
`151a853c…`, inner vptr `0x15960e8`, final `0x1596be0`, tag `0x1596bb0`, dtor `0x54ce0`, page+`0x84`,
bounds `0..2`) and `r_showLightProbes` (`0x6c130` outer 0x18c sha `9216b8be…` / `0x6c2c0` inner
0x15f sha `f7841dbd…`, inner vptr `0x1596160`, final `0x1596c78`, tag `0x1596c48`, dtor `0x54d20`,
page+`0x174`, bounds `0..4`). Byte maps: bool **162/30**, float **171/37**, dword **171/37**,
wrapper **195/13**; batch **6,015 written / 1,137 holes**. The two wrappers are the only objects
that do not call `0x20d60` directly (34 direct registrations); they register inside their own outer
body, which is the `wrapper_call` hook.

ELF corrections to the inherited table: (1) the object span ends at `0x2eaf2a0`, not `0x2eaf1e0`
— the latter is the *start* of the last object (`r_debugDrawSuperShinyMode`) and `0x2eaf2a0` is
batch 5's first object; the `0x1bf0` size column was already right. (2) The plan's 193 calls are
178 direct + 15 indirect, not 193 direct. (3) The column's 178 "direct stores" is the direct-call
count copied into the wrong column; the real object stores are 215 (522 including 307 stack
stores). (4) There is no pad byte: `0x75d22` holds `48 8d 1d 77 …` (`leaq`, batch 5's first
instruction), so the two batches abut. Float and dword bounds do not sit at the start of a
4-byte rodata dword: the floats travel in 16-byte constant-pool vectors (the oracle reads the
16-byte window at each rip target) and the dword bound travels as an immediate in the body.

Module `native/core/ctor_slot_554_batch4_739e8.{hpp,cpp}`, fixture `test_ctor_slot_554_batch4`
(36 registrations/atexit rows, 72 inert releases, two wrapper calls, two view destroys, the
1,137-hole sentinel sweep, all fail-closed hook stages and a byte-identical replay), oracle
`check_ctor_slot_554_batch4.py` (408 operand pins, body/callee hashes, 911 ELF + 87 native
mutants; both tamper modes exit 1). Batches 1 + 2a + 2b + 2b-cont + 3 + 4 are `0x6745` body bytes
and `0x63b8` object bytes, **21,334 written / 4,194 holes**, 116 registrations and 116 atexit rows.

## Landed batch 5 (evidence)

The 36 objects `0x2eaf2a0..0x2eb0f10` (`0x1c70` B), body `[0x75d22,0x781c5)` = `0x24a3` B,
sha256 `475a12b6035f378fc7e4b0de1ee7dc7c91991b4fc2fa5014a8f6626b0f0bda77`; the slot-table word
`0x2dd5f90` still holds `0x6efe0`. 1,628 instructions, **179 direct calls + 23 indirect** (22 functor
destroys and the wrapper view destroy), **516 stores (161 object + 355 stack)**, no conditional
branch; the final instruction is `callq 0xf8f390`, ending exactly at `0x781c5`, batch 6's first
byte. Families: thirteen `0xc0` bools (base `0x20af0`, mid `0x15940b0`, final `0x1594348`, tag
`0x1596898`, atexit `0xacf0`); twenty-two `0xd0` `0xc300` bounded floats (final vptr `0x15943c0`,
arg-fn `0x1596878`, class vtable `0x1594318`, atexit `0xad30`); and one `0xd0` wrapper
`showBakedLightingType` (`0x6c4c0` outer 0x190 / `0x6c650` inner 0x160, inner vptr `0x15961d8`,
final `0x1596d10`, tag `0x1596ce0`, dtor `0x54d60`, page+`0x178`, bounds `0..4`). Bounds: #8
`0.1..4096`, #13–20 and #28 `0..1`, #21–24 `0..100`, #26/#27/#30 `0..1000`, #29 `0..360`, #31–34
`1..100`; wrapper `0..4`. Byte maps: bool **162/30**, float **171/37**, wrapper **195/13**; batch
**6,063 written / 1,217 holes**. ELF corrections to the inherited row: the block ends at `0x2eb0f10`,
not `0x2eb0e50` (the latter is the last object's start); the inherited 202 calls are 179 direct +
23 indirect; the inherited 117 "direct stores" is not a store count — the real object stores are
161 (516 including stack). The float final vptr is corrected to `0x15943c0`, not `0x15942b0`
(E1409). Module `native/core/ctor_slot_554_batch5_75d22.{hpp,cpp}`, fixture
`test_ctor_slot_554_batch5`, oracle `check_ctor_slot_554_batch5.py` (420 operand pins, body/callee
hashes, 929 ELF + 70 native mutants; both tamper modes exit 1). Batches 1 + 2a + 2b + 2b-cont + 3 +
4 + 5 are `0x8be8` body bytes and `0x8028` object bytes, **27,397 written / 5,411 holes**, 152
registrations and 152 atexit rows.

## Landed batch 6 (evidence)

The 36 objects `0x2eb0f10..0x2eb2ba0` (`0x1c90` B), body `[0x781c5,0x7a68c)` = `0x24c7` B, sha256
`6885fb038c46d9331592b4f6fd03719044eb11ad062d987a29a6823a5d6d2c87`; the slot-table word `0x2dd5f90`
still holds `0x6efe0`. 1,636 instructions, **177 direct calls + 25 indirect** (22 functor destroys
and three wrapper view destroys), **509 stores (143 object + 366 stack)**, no branch of any kind;
`0x7a68c` holds `48 8d 1d 0d 85 e3 02` (batch 7's `leaq`), so there is no pad byte. Call histogram:
`0xc300` x22, `0x20af0` x11, `0x20d60` x33, `0x6c850` x1, `0x6cbe0` x1, `0x6cf70` x1, `0xf590` x72,
`0xf8f390` x36. Families: eleven `0xc0` bools (base `0x20af0`, mid `0x15940b0`, final `0x1594348`, tag
`0x1596898`, atexit `0xacf0`); twenty-two `0xd0` `0xc300` bounded floats (final vptr `0x15943c0`,
arg-fn `0x1596878`, class vtable `0x1594318`, atexit `0xad30`); and three `0xd0` wrappers —
`r_rainDebugDraw` (`0x6c850` outer 0x190 / `0x6c9e0` inner 0x160, inner vptr `0x1596250`, final
`0x1596da8`, tag `0x1596d78`, dtor `0x54da0`, page+`0x194`, bounds `0..5`), `AO_Mode`
(`0x6cbe0`/`0x6cd70`, inner vptr `0x15962b8`, final `0x1596e30`, tag `0x1596e10`, dtor `0x54de0`,
page+`0x19c`, bounds `0..3`) and `BakedAOMode` (`0x6cf70`/`0x6d100`, inner vptr `0x1596330`, final
`0x1596ec8`, tag `0x1596ea8`, dtor `0x54e20`, page+`0x1a0`, bounds `0..1`). Bounds: #1 `0..100`, #2
`0.01..180`, #3–9 `0..1000`, #10–12 `0..100`, #13/#14/#16 `0..10`, #15/#17/#18 `0..1`, #19 `0..1000`,
#20 `0..5000`, #21 `0..2000`, #22 `1..1000`; wrappers `0..5`, `0..3`, `0..1`. Byte maps: bool
**162/30**, float **171/37**, wrapper **195/13**; batch **6,129 written / 1,183 holes**. ELF
corrections to the inherited row: the block ends at `0x2eb2ba0`, not `0x2eb2ad0` (the latter is the
last object's start, `BakedAO_FadeRange`); the inherited 202 calls are 177 direct + 25 indirect —
202 is the total; the inherited 103 "direct stores" is not a store count, the real object stores
are 143 (509 including stack). Module `native/core/ctor_slot_554_batch6_781c5.{hpp,cpp}`, fixture
`test_ctor_slot_554_batch6` (every field, 36 registrations/atexit rows, 72 inert views, three
wrapper calls carrying their own RVAs, three view destroys, the 1,183-hole sentinel sweep,
fail-closed stages at registration/atexit/view release/wrapper call/view destroy — the last two stop
at `built == 18` — and a byte-identical replay), oracle `check_ctor_slot_554_batch6.py` (430 operand
pins, body/callee hashes, 957 ELF + 83 native mutants; both tamper modes exit 1). Batches 1 + 2a +
2b + 2b-cont + 3 + 4 + 5 + 6 are `0xb0af` body bytes and `0x9cb8` object bytes, **33,526 written /
6,594 holes**, 188 registrations and 188 atexit rows.

## Landed batch 7 (evidence)

The 36 objects `0x2eb2ba0..0x2eb4868` (`0x1cc8` B), body `[0x7a68c,0x7cb5b)` = `0x24cf` B, sha256
`33c4a4266ddd0ad8bf19a08aeb262aeaf3c2cc0882187168a84497931e783663`; the slot-table word `0x2dd5f90`
still holds `0x6efe0`. 1,631 instructions, **179 direct calls + 27 indirect** (19 float functor
destroys, four int destroys, three dword destroys and one wrapper view destroy), **508 stores
(137 object + 371 stack)**, no branch of any kind; `0x7cb5b` is batch 8's `leaq 0x2eb4868`, so there
is no pad byte. Call histogram: `0xc300` x19, `0xc580` x3, `0x3a980` x4, `0x20af0` x9, `0x20d60`
x35, `0x6d300` x1, `0xf590` x72, `0xf8f390` x36. Families: nine `0xc0` bools (base `0x20af0`, mid
`0x15940b0`, final `0x1594348`, tag `0x1596898`, atexit `0xacf0`); nineteen `0xd0` `0xc300` bounded
floats (final vptr **`0x15943c0`** — the hoisted `0x10(%rsp)` slot, arg-fn `0x1596878`, class vtable
`0x1594318`, atexit `0xad30`); four `0xd0` `0x3a980` unbounded ints (final `0x1594438`, arg-fn
`0x1596858`, atexit `0xad70`); three `0xd8` `0xc580` **object-hosted** bounded dwords
(`progSamplerAnisoLevel_Cinematic`/`_BasePS4`/`_Perf`; the ctor receives `object+0xd0`, so there is
no page read; final `0x1594c08` = table `0x1594bf8` + `0x10`, atexit `0x18a60`, bounds `0..0x10`);
and one `0xd0` wrapper `fog_superSampleMode` (`0x6d300` outer 0x190 sha
`da3a787b35be635a…` / `0x6d490` inner 0x160 sha `1a761e293771d3ed…`, inner vptr `0x15963a8`, final
`0x1596f70`, tag `0x1596f40`, dtor `0x54e60`, page+`0x1f8`, bounds `0..2`). Bounds: #0 `-4..4`, #2
`0..100`, #3 `0..50`, #4 `0..100`, #5 `0..50`, #6 `0..1000`, #7/#8 `-1..1`, #9 `1..1.25`, #10 `0..10`,
#11/#12 `0..1`, #13 `0..1`, #14 `0..1.01`, #15 `0..100`, #16 `0..100`, #17 `10..5000`, #18
`0.1..4`; wrapper `0..2`. Byte maps: bool **162/30**, float **171/37**, int **165/43**, dword
**175/41**, wrapper **195/13**; batch **6,087 written / 1,281 holes**. ELF corrections to the
inherited row: the block ends at `0x2eb4868`, not `0x2eb47a8` (the latter is the last object's
start); the inherited 206 calls are 179 direct + 27 indirect — 206 is the total; the inherited 88
"direct stores" is not a store count, the real object stores are 137 (508 including stack); and the
three dword objects have no page cell at all because `0xc580` is handed `object+0xd0`. Module
`native/core/ctor_slot_554_batch7_7a68c.{hpp,cpp}`, fixture `test_ctor_slot_554_batch7` (every
field, 36 registrations/atexit rows, 72 inert views, one wrapper call carrying its own RVA, one
view destroy, the 1,281-hole sentinel sweep, fail-closed stages at registration/atexit/view
release/wrapper call/view destroy — the wrapper-call refusal stops at `built == 13` — and a
byte-identical replay), oracle `check_ctor_slot_554_batch7.py` (425 operand pins, body/callee
hashes, 943 ELF + 89 native mutants; both tamper modes exit 1). Batches 1 + 2a + 2b + 2b-cont + 3 +
4 + 5 + 6 + 7 are `0xd57e` body bytes and `0xb980` object bytes, **39,613 written / 7,875 holes**,
224 registrations and 224 atexit rows.

## Landed batch 11 (evidence)

The 36 objects `0x2eb9ee8..0x2ebbbc8` (`0x1ce0` B), body `[0x83ad1,0x86052)` = `0x2581` B, sha256
`0f7577aea5c97031cbb980639f39465d633ed3ddba4e3667a043da524873e848`; the slot-table word `0x2dd5f90`
still holds `0x6efe0`. 1,670 instructions, **179 direct calls + 30 indirect** (26 float functor
destroys, three dword functor destroys and one wrapper view destroy), **508 stores (112 object +
396 stack)**, no branch; `0x86052` holds batch 12's `leaq 0x2ebbbc8`, so there is no pad byte.
Families: six `0xc0` bools (base `0x20af0`, mid `0x15940b0`, final `0x1594348`, tag `0x1596898`,
atexit `0xacf0`); twenty-six `0xd0` `0xc300` bounded floats (final `0x15943c0`, arg-fn `0x1596878`,
class `0x1594318`, atexit `0xad30`; page cells `+0x394..+0x40c`; bounds `0..0.05`, `0..0.1`, `0..1`,
`0..2`, `0..10`, `0..100`); three `0xd0` `0xc580` **page-cell** bounded dwords (no object-hosted
`+0xd0` cell: `r_aaForceJitterIndex` page+`0x3b4`, `-1..100`;
`r_forceViewportHeightForPixelSizeCalc` page+`0x42c`, `-1..2160`; `r_forceLod` page+`0x430`,
`-1..10`; final `0x1594c08`, arg-fn `0x1596858`, class `0x15944a0`, atexit `0xad70`, min/max full
dwords at `+0x6c/+0x74`); and one `0xd0` wrapper `r_aaMethod` (`0x6e4d0` outer 0x190 sha
`332bde1c…` / `0x6e660` inner 0x160 sha `09f5a105…`, inner vptr `0x1596600`, final `0x1597258`, tag
`0x1597238`, dtor `0x54fa0`, page+`0x3a8`, bounds `0..1`). Call histogram: `0xf590` 72, `0xf8f390`
36, `0x20d60` 35, `0xc300` 26, `0x20af0` 6, `0xc580` 3, `0x6e4d0` 1. Byte maps: bool **162/30**,
float **171/37**, dword **171/37**, wrapper **195/13**; batch **6,126 written / 1,266 holes**.
All 36 name strings from the `0xf9c396` table match their blocks, from
`r_ui_HdrConsistentUIWhitePoint` through `r_forceLod`.

ELF corrections to the inherited row: the object span ends at `0x2ebbbc8`, not `0x2ebbaf8` (the
latter is the last object's start); the inherited 209 calls are 179 direct + 30 indirect; the
inherited 64 "direct stores" is not a store count — the real object stores are 112 (508 including
stack). The strict store rule (mov-family memory writes + all `vmovups`) also recounts batch 10 at
497/119/378; batch 10's fixture, oracle, result row and evidence now carry the corrected counts
instead of the landed 481/119/362.

Module `native/core/ctor_slot_554_batch11_83ad1.{hpp,cpp}`, fixture
`test_ctor_slot_554_batch11` (every field, 36 registrations/atexit rows, 72 inert views, one wrapper
call carrying its own RVA, one view destroy, the 1,266-hole sentinel sweep, fail-closed stages —
wrapper-call refusal stops at `built == 7` — and byte-identical replay), oracle
`check_ctor_slot_554_batch11.py` (484 operand pins, body/callee hashes, 1,067 ELF + 84 native
mutants; both tamper modes exit 1). Batches 1 + 2a + 2b + 2b-cont + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 +
11 are `0x16a75` body bytes / `0x12ce0` object bytes, **64,135 written / 12,889 holes**, 368
registrations and 368 atexit rows.

## Batch decomposition

The later batches are cut at `leaq` boundaries; batch 13 (`0x8849d..`, starting at object
`0x2ebd7f8`) is **modelled and verified**. Its body is
`[0x8849d,0x8a618)`; the 31 block variables occupy `0x2ebd7f8..0x2ebf0d0` (`0x18d8` B) and the
separate `0xd0` `option_unavailable` command starts at `0x2ebf0d0` (32 starts, `0x19a8` B). The
`calls` and `direct stores` columns below are measured from the pinned body where a batch is closed; **byte-hole accounting for a batch is a lift-time gate** because its
constructors determine which bytes are written. No hole count is invented here. Batches 1, 2a, 2b (both parts), and 3–13 are closed. Rows 6–13 have measured body/object ends;
batch 13 ends at the FDE boundary `0x8a618`, after the final stack-canary branch.

| Batch | Objects | Count | Object span | Body extent | Calls | Direct stores |
|---|---|---:|---:|---|---:|---:|
| 3 | `0x2eabb80..0x2ead6b0` | 36 | `0x1b30` | `0x718bb..0x739e8` | 182 | 303 |
| 4 | `0x2ead6b0..0x2eaf2a0` | 36 | `0x1bf0` | `0x739e8..0x75d22` | 178 direct + 15 indirect | 215 |
| 5 | `0x2eaf2a0..0x2eb0f10` | 36 | `0x1c70` | `0x75d22..0x781c5` | 179 direct + 23 indirect | 161 |
| 6 | `0x2eb0f10..0x2eb2ba0` | 36 | `0x1c90` | `0x781c5..0x7a68c` | 177 direct + 25 indirect | 143 |
| 7 | `0x2eb2ba0..0x2eb4868` | 36 | `0x1cc8` | `0x7a68c..0x7cb5b` | 179 direct + 27 indirect | 137 |
| 8 | `0x2eb4868..0x2eb6538` | 36 | `0x1cd0` | `0x7cb5b..0x7f098` | 177 direct + 29 indirect | 115 |
| 9 | `0x2eb6538..0x2eb8218` | 36 | `0x1ce0` | `0x7f098..0x8160c` | 178 direct + 30 indirect | 110 object (506 total) |
| 10 | `0x2eb8218..0x2eb9ee8` | 36 | `0x1cd0` | `0x8160c..0x83ad1` | 179 direct + 29 indirect | 119 object (497 total) |
| 11 | `0x2eb9ee8..0x2ebbbc8` | 36 | `0x1ce0` | `0x83ad1..0x86052` | 179 direct + 30 indirect | 112 object (508 total) |
| 12 | `0x2ebbbc8..0x2ebd7f8` | 36 | `0x1c30` | `0x86052..0x8849d` | 180 direct + 19 indirect | 191 object (518 total) |
| 13 | `0x2ebd7f8..0x2ebf0d0` + dummy `0x2ebf0d0..0x2ebf1a0` | 31 + 1 | `0x18d8` + `0xd0` = `0x19a8` | `0x8849d..0x8a618` | 161 direct + 25 indirect | 457 total (120 object + 337 stack) |

### Batch 2b continuation result

The former 2b remainder is closed: 23 objects, `0x11a0` object bytes, body `0x154e` B with 955
instructions, 111 direct calls, six indirect functor destroys and **3870 written / 642 holes**.
Full batch 2b is 31 objects, `0x17b0` object bytes, body `0x1c92`, 1,283 instructions and 157 calls
(150 direct + 7 indirect).

### Batch 8 result

Batch 8 is closed: 36 objects, `0x1cd0` object bytes (`0x2eb4868..0x2eb6538`), body
`[0x7cb5b,0x7f098)` = `0x253d` B, sha256
`ff7cbbd941da544771e5212252595b1a0be8477d85ac8d3914b11455ea79d9bb`. The body has 1,659
instructions, 177 direct calls, 29 indirect `call [rax+0x18]` functor destroys and 505 stores
(115 object + 390 stack), with no branch of any kind. Families: seven `0xc0` bools, twenty-six
`0xd0` `0xc300` bounded floats, and three `0xd0` wrappers (`r_waterDebugDraw` 0x6c850/0x6c9e0;
`r_tonemapMethodSDR`/`HDR` 0x6d690/0x6d820). Call histogram: `0xf590` 72, `0xf8f390` 36,
`0x20d60` 33, `0xc300` 26, `0x20af0` 7, `0x6d690` 2, `0x6c850` 1. Byte maps: bool **162/30**,
float **171/37**, wrapper **195/13**; batch **6,165 written / 1,211 holes**. Bounds include
`0..100` (wave speed/transition band), `0..10` (dampening/foam/highwater/slope), `0..1000`
(tessellation ranges), `0..20` (ACES/Hable white point) and wrapper `0..5`. Wrapper pins:
water inner vptr `0x1596250`, final `0x1596da8`, tag `0x1596d78`, dtor `0x54da0`; tonemap inner
vptr `0x1596430`, final `0x1597008`, tag `0x1596fd8`, dtor `0x54ea0`. The inherited object end
`0x2eb6468` is the last object's start; the real block ends at `0x2eb6538` (batch 9's `leaq`).

The fixture covers all 36 fields, 36 registrations/atexit rows, 72 inert views, three wrapper
calls carrying their own RVAs, three view destroys, the 1,211-hole sentinel sweep, fail-closed
stages (wrapper-call refusal stops at `built == 2`; view-destroy refusal fires after the wrapper
call) and byte-identical replay. `check_ctor_slot_554_batch8.py` covers 423 operand pins, the
body/callee hashes, and rejects 939 ELF + 78 native mutants; both tamper modes exit 1. Batches 1 +
2a + 2b + 2b-cont + 3 + 4 + 5 + 6 + 7 + 8 are `0xfabb` body bytes / `0xd650` object bytes,
**45,778 written / 9,086 holes**, 260 registrations and 260 atexit rows.

### Batch 9 result

Batch 9 is closed: 36 objects, `0x1ce0` object bytes (`0x2eb6538..0x2eb8218`), body
`[0x7f098,0x8160c)` = `0x2574` B, sha256
`d3b8db529400729e574b4dc3d1e000b515123b58439efdaa5c41b5160fcbc6f6`. The body has 1,660
instructions, 178 direct calls, 30 indirect `call [rax+0x18]` functor destroys and 506 stores
(110 object + 396 stack), with no branch of any kind. Families: six `0xc0` bools, twenty-eight
`0xd0` `0xc300` bounded floats, and two `0xd0` wrappers (`r_showRenderSettings`
0x6da20/0x6dbb0; `r_showRenderSettingsValues` 0x6ddb0/0x6df40). Call histogram: `0xf590` 72,
`0xf8f390` 36, `0x20d60` 34, `0xc300` 28, `0x20af0` 6, `0x6da20` 1, `0x6ddb0` 1. Byte maps:
bool **162/30**, float **171/37**, wrapper **195/13**; batch **6,150 written / 1,242 holes**.
Wrapper pins: settings inner vptr `0x15964a8`, final `0x15970a0`, tag `0x1597070`, dtor `0x54ee0`,
page+`0x2ec`, bounds `0..2`; values inner vptr `0x1596520`, final `0x1597138`, tag `0x1597108`,
dtor `0x54f20`, page+`0x2f0`, bounds `0..1`. The inherited object end `0x2eb8158` is the last
object's start; the real block ends at `0x2eb8218` (batch 10's `leaq`). The inherited 208 calls
are 178 direct + 30 indirect; the inherited 58 "direct stores" is not a store count (110 object
stores, 506 including stack).

The fixture covers all 36 fields, 36 registrations/atexit rows, 72 inert views, two wrapper calls
carrying their own RVAs, two view destroys, the 1,242-hole sentinel sweep, fail-closed stages and
byte-identical replay. `check_ctor_slot_554_batch9.py` covers 428 operand pins, the body/callee
hashes, and rejects 949 ELF + 78 native mutants; both tamper modes exit 1. Batches 1 + 2a + 2b +
2b-cont + 3 + 4 + 5 + 6 + 7 + 8 + 9 are `0x1202f` body bytes / `0xf330` object bytes, **51,928
written / 10,328 holes**, 296 registrations and 296 atexit rows.

### Batch 10 result

Batch 10 is closed: 36 objects, `0x1cd0` object bytes (`0x2eb8218..0x2eb9ee8`), body
`[0x8160c,0x83ad1)` = `0x24c5` B, sha256
`94191f4eb264bb0c1b9230a3ed65b266b3e45b8d8eb8a97ca2c5cb49059dd27b`. The body has 1,641
instructions, 179 direct calls, 29 indirect `call [rax+0x18]` functor/view destroys and **497 stores**
(119 object + 378 stack; strict recount, correcting the landed 481/119/362), with no branch. Families: seven `0xc0` bools, twenty-two `0xd0` `0xc300`
bounded floats, four `0xd0` `0x6aa00` unbounded floats, two `0xd0` `0x3a980` unbounded ints and one
`0xd0` wrapper (`r_fullscreenBlurEnabled` 0x6e140/0x6e2d0). Call histogram: `0xf590` 72,
`0xf8f390` 36, `0x20d60` 35, `0xc300` 22, `0x20af0` 7, `0x6aa00` 4, `0x3a980` 2, `0x6e140` 1.
Byte maps: bool **162/30**, bounded float **171/37**, unbounded float **165/43**, int **165/43**,
wrapper **195/13**; batch **6,081 written / 1,295 holes**. Wrapper pins: inner vptr `0x1596598`,
final `0x15971d0`, tag `0x15971a0`, dtor `0x54f60`, page+`0x4d0`, bounds `0..2`. The inherited
object end `0x2eb9e18` is the last object's start; the real block ends at `0x2eb9ee8` (the body's
end). The inherited 208 calls are 179 direct + 29 indirect; the inherited 84 "direct stores" is not
a store count (119 object; 497 including stack under the strict recount).

The fixture covers all 36 fields, 36 registrations/atexit rows, 72 inert views, one wrapper call
carrying its RVA, one view destroy, the 1,295-hole sentinel sweep, fail-closed stages and
byte-identical replay. `check_ctor_slot_554_batch10.py` covers 475 operand pins, the body/callee
hashes, and rejects 1,051 ELF + 87 native mutants; both tamper modes exit 1. Batches 1 + 2a + 2b +
2b-cont + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 are `0x144f4` body bytes / `0x11000` object bytes,
**58,009 written / 11,623 holes**, 332 registrations and 332 atexit rows.

### Batch 11 result

Batch 11 is closed: 36 objects, `0x1ce0` object bytes (`0x2eb9ee8..0x2ebbbc8`), body
`[0x83ad1,0x86052)` = `0x2581` B, sha256
`0f7577aea5c97031cbb980639f39465d633ed3ddba4e3667a043da524873e848`. The body has 1,670
instructions, 179 direct calls, 30 indirect destroys and 508 stores (112 object + 396 stack), with
no branch. Families: six `0xc0` bools, twenty-six `0xd0` `0xc300` bounded floats, three `0xd0`
`0xc580` page-cell dwords and one `0xd0` wrapper (`r_aaMethod` 0x6e4d0/0x6e660). Byte maps: bool
**162/30**, float **171/37**, dword **171/37**, wrapper **195/13**; batch **6,126 written / 1,266
holes**. The inherited object end `0x2ebbaf8` is the last object's start; the real block ends at
`0x2ebbbc8` (batch 12's `leaq`). The inherited 209 calls are 179 direct + 30 indirect; the
inherited 64 "direct stores" is not a store count (112 object stores, 508 including stack).

The fixture covers all 36 fields, 36 registrations/atexit rows, 72 inert views, one wrapper call
carrying its RVA, one view destroy, the 1,266-hole sentinel sweep, fail-closed stages and
byte-identical replay. `check_ctor_slot_554_batch11.py` covers 484 operand pins, the body/callee
hashes, and rejects 1,067 ELF + 84 native mutants; both tamper modes exit 1. Batches 1 + 2a + 2b +
2b-cont + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 + 11 are `0x16a75` body bytes / `0x12ce0` object bytes,
**64,135 written / 12,889 holes**, 368 registrations and 368 atexit rows.

### Batch 12 result

Batch 12 is closed: 36 objects, `0x1c30` object bytes (`0x2ebbbc8..0x2ebd7f8`), body
`[0x86052,0x8849d)` = `0x244b` B, sha256
`634603b5c627d875d067453f32a541cd83a789b32a310c3eae071d8e210ec9ba`. The body has 1,613
instructions, 180 direct calls, 19 indirect destroys and 518 stores (191 object + 327 stack), with
no branch. Families: seventeen `0xc0` bools, seventeen `0xd0` `0xc300` bounded floats and two `0xd0`
unbounded wrappers (`r_furStats` 0x6e800/0x6ebe0 and `r_furDebugMode` 0x6e9f0/0x6ede0). Byte maps:
bool **162/30**, float **171/37**, wrapper **195/13**; batch **6,051 written / 1,165 holes**. All 36
name strings from the `0xf9c749` table match their blocks, from `r_pixelSizeOverrideEnable` through
`r_furAirResist`; the next name is `r_furWaterResist` at `0xf9ca33`.

The inherited body end `0x8849d` is confirmed (it holds batch 13's `leaq` of `0x2ebd7f8`); the
inherited last object start `0x2ebd728` is also confirmed, and the block ends at `0x2ebd7f8`. The
inherited 199 calls and 148 stores were wrong: measured 180 direct + 19 indirect calls (72 `0xf590`,
36 `0xf8f390`, 36 `0x20d60`, 17 `0x20af0`, 17 `0xc300`, one each `0x6e800`/`0x6e9f0`; the
indirect are 17 float functor destroys and two wrapper view destroys) and 518 strict stores (191
object + 327 stack).

The fixture covers all 36 fields, 36 registrations/atexit rows, 72 inert views, two wrapper calls
carrying their RVAs, two view destroys, the 1,165-hole sentinel sweep, fail-closed stages and
byte-identical replay. `check_ctor_slot_554_batch12.py` covers 459 operand pins, the body/callee
hashes, and rejects 1,027 ELF + 99 native mutants; both tamper modes exit 1. Batches 1 + 2a + 2b +
2b-cont + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 + 11 + 12 are `0x18ec0` body bytes / `0x14910` object
bytes, **70,186 written / 14,054 holes**, 404 registrations and 404 atexit rows.

### Dependencies and order

Already native and reusable: `0x3a980`, `0x6aa00`, `0xc300`, `0xc580`, `0x20af0`, `0x20d60`,
`0xf590`, `0xf2f0`, `0xad30`, `0xad70`, `0xacf0`, `0xf8f390`, `0x6af60`, `0x6abb0` /
`0x6ad40`, `0x6e140` / `0x6e2d0` and now `0x6e800` / `0x6ebe0` and `0x6e9f0` / `0x6ede0`. Batch
13 must pin any new wrapper entry it encounters before use.

Order is strictly increasing object/batch order so the page header and each registration/atexit
sequence stay in body order. Each batch must: (1) classify every object's constructor family, (2)
model the constructor stores first, (3) add the body's final-vptr/value/register/release/atexit
sequence, (4) record an exact written/hole mask, and (5) add its own
`check_ctor_slot_554_batchN.py` before the row is marked complete. Slot 554 stays PARTIAL until
batch 13 closes.

### Batch 13 result

Batch 13 is **modelled and verified**. Body `[0x8849d,0x8a618)` = `0x217b` B; 1,501 instructions,
161 direct + 25 indirect calls, 457 stores (120 object + 337 stack), two branches. The measured
layout is 31 block variables `0x2ebd7f8..0x2ebf0d0` (`0x18d8` B: 7 bools, 14 `0xc300` floats,
6 `0xc580` dwords — 3 page-cell `0xd0` and 3 object-owned `0xd8` — and 4 wrappers) plus a separate
`0xd0` `option_unavailable` command at `0x2ebf0d0`; the inherited `0x1af8` span was wrong and the measured `0x19a8` span is used.
The 32 starts, names, page cells, bounds, wrapper final/tag pins and c580 owned rows are recorded in
the checkpoint and `docs/checkpoints/2026-11-08-startup-slot-554-batch13-mask-trace.md`. Block byte map: bool **1,134/210**, c300
**2,394/518**, c580 page-cell **513/111**, c580 owned **525/123**, wrappers **780/52**; block
**5,346 written / 1,014 holes**. The body makes 6 direct `0xc580` calls for 6 c580 objects and 32 direct `0xf8f390` calls for 32 starts; both reconcile exactly. The model, fixture, oracle and CMake wiring were added; the full checker sweep is **906/0/0**.

## Known gaps

- The body contains 415 registrations and 436 atexit rows; all **415/415** and **436/436** are modelled.
- The page header's 647 holes are never written by `0x53fc0`; they remain `.bss` zeros in the
  original and sentinel bytes in the fixture.
- `0x8a514` is the only `0xf2f0` call and lies inside the landed batch 13.
- The 17-wrapper family is normalised-equivalent in structure but is not a single constant body;
  each entry's vtable/tag immediates must be pinned before use. `0x6af60`, `0x6abb0`, `0x6b2f0`, `0x6b680`, `0x6bda0`, `0x6c130`,  `0x6c4c0`/`0x6c650`, `0x6c850`/`0x6c9e0`, `0x6cbe0`/`0x6cd70`, `0x6cf70`/`0x6d100`,
  `0x6d300`/`0x6d490`, `0x6d690`/`0x6d820`, `0x6da20`/`0x6dbb0`, `0x6ddb0`/`0x6df40`,
  `0x6e140`/`0x6e2d0` and `0x6e4d0`/`0x6e660` are native so far.
- Batch 13 (`0x8849d..`) is modelled and verified: 31 block variables plus the `0xd0`
  `option_unavailable` dummy at `0x2ebf0d0`, `0x19a8` total object stride; dummy/owned masks are
  149/59 and 175/41 from the store trace; sweep **906/0/0**.
- No boot, audio, Metal or save/load claim is made.
