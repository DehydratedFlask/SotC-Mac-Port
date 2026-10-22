# Checkpoint — Init continue second 0x532ed0-series virtual-call +0x18

- Local: 2026-10-22 ~16:43 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-INIT-531D60-POST-BASE-VCALL18-532ED0-U2-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why this edge)

First always-on measured Init edge after E478 second `0x532ed0` call-site.
OBS `0x532369..0x532377` (14 B): `mov rax,[rsp+0xa30]` / `mov rdi,r14` /
`call [rax+0x18]`. Prefer this virtual-call continue over `0x532ed0` body /
stack-prep / vcall-callee / `0x532ca0` tourism, over trampoline/`0x52eb50` /
`0x26800`/`0xf590` / staffroll bodies, and over deepen `0x1290` /
`0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` /
`0x6a5370`.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x532369..0x532377` (14 B) sha256 `a70f516c…` |
| Hooks | `call_vslot18` (1) |
| Stop | before `mov edi,0x460` / call `0x17070` @ `0x532377` |

## Not modelled

Stack/vtable producers; virtual-call callee body; later helpers
(next call after end is `0x17070`, not another `0x532ed0`);
`0x532ed0` body / stack prep; trampoline / string-helper / staffroll / PLT
bodies; helper bodies `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` /
`0x140d90` / `0x14c2f0` / `0x6a5370`.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f531d60_post_base_vcall18_532ed0_u2.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-post-base-vcall18-532ed0-u2-20261022/` (manifest `e8b3e1e4…` @ `c1ba3bd`; capture `4f6aec67…`) |
| Pairs | still **155** |

## Measured

`pbvcedu2=1` + `pb532edu2=1` + mid_reg=4 → 275 registry; alloc_ops=13;
tip AllocatorFront+FakeHeap; PR #5 cite-only.

## Risks / next

`0x17070` call-site / always-on after `0x532377`, or bounded `0x1290` /
`0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` /
`0x6a5370`. Prefer call-site continue over bodies. Goal open.
