"""Closed byte-level decoder for a straight-line "hash a rodata string, store the dword" run (startup slot 445, RVA 0x4cfe80; batch 20).

The run is a sequence of
    lea  rdi|rbx|r14|r15, [rip+d]      (48/4c 8d /r, mod=00 rm=101)
    mov  rdi, r14|r15                  (48/4c 89 /r, mod=11)
    call 0xa2ddc0                      (e8 rel32; the path-folding FNV hash, tail-jumps to 0x162020)
    mov  [rbx+disp], eax               (89 /r, rm=rbx, reg=eax, mod 00/01/10)
    mov  rcx, [rip+d]                  (48 8b 0d; the stack-guard GOT slot load the compiler interleaves; only the pinned slot)
and NOTHING ELSE: any other byte sequence raises HashStoreRefused (nothing is skipped or guessed). The decoder reads raw bytes through `raw(addr, n)` (so an
`Elf.overlay` perturbs the decode) and tracks symbolic register values: ('a', address) absolute addresses and ('h', string address) = the hash result.
It never executes, loads or calls anything.  Calling convention facts used (pinned elsewhere by the checker): 0xa2ddc0 is `mov esi,0x811c9dc5 ; jmp 0x162020` and
0x162020 pushes and restores rbx / r12 / r14 / r15 / rbp, so rbx, r14 and r15 survive the call while rax, rcx, rdx, rsi, rdi, r8-r11 do not.
"""
from __future__ import annotations

import struct

FNV_ENTRY = 0xA2DDC0
GUARD_GOT = 0x15FE518
RBX, RDI, R14, R15, RCX = 3, 7, 14, 15, 1


class HashStoreRefused(Exception):
    pass


def _s32(b: bytes) -> int:
    return struct.unpack("<i", b)[0]


def replay(raw, start: int, end: int, str_lo: int = 0xF90000, str_hi: int = 0x1594000) -> dict:
    """Replay [start, end) and return {'stores': [(call_addr, store_addr, dest, string_addr)], 'nins': n}."""
    g: dict[int, tuple] = {}
    eax = None
    pending = None
    stores = []
    nins = 0
    guard_seen = 0
    at = start
    while at < end:
        b = raw(at, 8)
        n = 0
        if b[0] in (0x48, 0x4C) and b[1] == 0x8D and (b[2] & 0xC7) == 0x05:                       # lea r64, [rip+d32]
            reg = ((b[2] >> 3) & 7) | ((b[0] & 4) << 1)
            if reg not in (RDI, RBX, R14, R15):
                raise HashStoreRefused(f"lea into an unexpected register {reg} @{at:#x}")
            n = 7
            g[reg] = ("a", at + n + _s32(b[3:7]))
        elif b[0] in (0x48, 0x4C) and b[1] == 0x89 and (b[2] >> 6) == 3:                           # mov r64, r64
            src, dst = ((b[2] >> 3) & 7) | ((b[0] & 4) << 1), (b[2] & 7) | ((b[0] & 1) << 3)
            if dst != RDI or src not in (R14, R15) or src not in g:
                raise HashStoreRefused(f"mov r64,r64 is not rdi <- r14/r15 @{at:#x}")
            n = 3
            g[RDI] = g[src]
        elif b[0] == 0xE8:                                                                         # call rel32
            n = 5
            tgt = (at + n + _s32(b[1:5])) & 0xFFFFFFFFFFFFFFFF
            if tgt != FNV_ENTRY:
                raise HashStoreRefused(f"call to {tgt:#x}, only 0xa2ddc0 is allowed @{at:#x}")
            if pending is not None:
                raise HashStoreRefused(f"a hash result is not stored before the next call @{at:#x}")
            s = g.get(RDI)
            if s is None or s[0] != "a" or not (str_lo <= s[1] < str_hi):
                raise HashStoreRefused(f"rdi is not a rodata string address at the call @{at:#x}")
            pending = (at, s[1])
            eax = ("h", s[1])
            g.pop(RDI, None)
            g.pop(RCX, None)                                                                       # caller-saved: clobbered by the call
        elif b[0] == 0x89 and (b[1] & 0x38) == 0 and (b[1] & 7) == 3 and (b[1] >> 6) in (0, 1, 2):  # mov [rbx+disp], eax
            mod = b[1] >> 6
            if mod == 0:
                n, disp = 2, 0
            elif mod == 1:
                n, disp = 3, struct.unpack("<b", b[2:3])[0]
            else:
                n, disp = 6, _s32(b[2:6])
            if eax is None or pending is None:
                raise HashStoreRefused(f"store of eax without a pending hash result @{at:#x}")
            base = g.get(RBX)
            if base is None or base[0] != "a":
                raise HashStoreRefused(f"store through an undefined rbx @{at:#x}")
            stores.append((pending[0], at, base[1] + disp, pending[1]))
            pending = None
            eax = None
        elif b[0] == 0x48 and b[1] == 0x8B and b[2] == 0x0D:                                       # mov rcx, [rip+d32] (stack-guard GOT slot)
            n = 7
            slot = at + n + _s32(b[3:7])
            if slot != GUARD_GOT:
                raise HashStoreRefused(f"rip-relative load of {slot:#x} is not the stack-guard slot @{at:#x}")
            guard_seen += 1
            g[RCX] = ("got", slot)
        else:
            raise HashStoreRefused(f"unsupported instruction bytes {bytes(b).hex()} @{at:#x}")
        nins += 1
        at += n
    if at != end:
        raise HashStoreRefused(f"decode overran the range end ({at:#x} != {end:#x})")
    if pending is not None:
        raise HashStoreRefused("the last hash result is not stored inside the range")
    if guard_seen > 1:
        raise HashStoreRefused("more than one stack-guard load")
    return {"stores": stores, "nins": nins, "guard_loads": guard_seen}
