# Checkpoint — Init continue third virtual-call +0x18

- Local: 2026-10-22 ~16:14 CDT (America/Chicago)
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9
- Task: `CPU-01-INIT-531D60-POST-BASE-VCALL18-U3-01` (attempt 1)
- Gate: **G1 PARTIAL — not PASSED**

## Triage (why this edge)

First always-on measured Init edge after E470 third `0x532ca0` call-site.
OBS `0x532194..0x5321a2` (14 B): `mov rax,[rsp+0xb70]` / `mov rdi,r14` /
`call [rax+0x18]`. Prefer this virtual-call continue over `0x532ca0` body /
stack-prep / vcall-callee tourism, over trampoline/`0x52eb50` /
`0x26800`/`0xf590` / staffroll bodies, and over deepen `0x1290` /
`0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` /
`0x6a5370`.

## Claimed

| Item | Detail |
| --- | --- |
| Fragment | `0x532194..0x5321a2` (14 B) sha256 `d73419c7…` |
| Hooks | `call_vslot18` (1) |
| Stop | before next unit `mov rdi,[rbx]` @ `0x5321a2` |

## Not modelled

Stack/vtable producers; virtual-call callee body; later `0x532ca0` units;
`0x532ca0` body / stack prep; trampoline / string-helper / staffroll / PLT
bodies; helper bodies `0x1290` / `0x5c64c0` / `0x56dcb0` / `0x499f80` /
`0x140d90` / `0x14c2f0` / `0x6a5370`.

## Commands / results

| Check | Result |
| --- | --- |
| ELF | `check_f531d60_post_base_vcall18_u3.py` OK |
| ctest | `appmain_init_ctor_98a90_slice` PASS |
| Private | `local/validation/appmain-init-post-base-vcall18-u3-20261022/` (manifest `6ae83587…` @ `d547668`; capture `ea75a4d2…`) |
| Pairs | still **155** |

## Measured

`pbvcu3=1` + `pb532u3=1` + mid_reg=4 → 275 registry; alloc_ops=13;
tip AllocatorFront+FakeHeap; PR #5 cite-only.

## Risks / next

Fourth `0x532ca0` unit / always-on after `0x5321a2`, or bounded `0x1290` /
`0x5c64c0` / `0x56dcb0` / `0x499f80` / `0x140d90` / `0x14c2f0` /
`0x6a5370`. Prefer call-site continue over bodies. Goal open.
