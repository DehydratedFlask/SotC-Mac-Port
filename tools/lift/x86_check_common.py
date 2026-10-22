#!/usr/bin/env python3
"""Shared plumbing for the check_f161f80 / f14c3a0 / f14c4a0 / ff730 / f14c9a0 / f14d880 / fb25470 binary ties
(private; needs local/re/eboot.elf and a built native test, see leaf_check_common.native_executable).

pin()      body bytes SHA-256 + exact linear decode length + instruction count + padding after the body
machine()  fresh strict interpreter (x86_strict.Machine) with the PLT thunks resolved by NID
two_poisons() runs a scenario with two register/ymm poison values and requires identical observable results
"""
import hashlib
import json
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import leaf_check_common as base  # noqa: E402
import x86_strict as X  # noqa: E402

require, ROOT = base.require, base.ROOT
THUNKS = {0xF8F3D0: '__cxa_guard_acquire', 0xF8F3E0: '__cxa_guard_release', 0xF8F390: '__cxa_atexit'}
DSO = 0x1604218          # last argument of every __cxa_atexit registration seen here (__dso_handle)


# rva: (size, sha256, instruction count or None when the strict decoder does not cover the body)
PINS = {
    0x161f80: (38, '552c900d77771ee72cb1c8ed6f39bd510460ae7b614d92b654f1a85ca6031599', 14),
    0x14c3a0: (40, 'f58aaac0928d1cea7b3d14c2d716859c0031ee0f2f755b8675d99998022ed4fb', 12),
    0x14c4a0: (66, '1737b4626bd4f2a28e984c083ba705f849812b245ba1e3ff568f19f846674566', 22),
    0xf730: (114, '03645245094b2faf0ccf36aaa9ec2fa186eca6d01ac1162203b7e0af5d41bb78', 23),
    0xf7b0: (73, '1e7da61ccdc2bc47c92c291047e615af7bbdc1b131ed4e0f5ff1836119dbc676', 26),
    0x95610: (9, '9c17e5ff513b2a38755953fcb8cde409728133a56b0c193ee13ab2771ebc96ea', 2),
    0x14c9a0: (464, '91c83a0d5d613fa291dbaf66316b994b4cd4c3c5400cae42e3fe3f97288ce883', 115),
    0x14d880: (121, 'b7586580add23db86b9d050ada729b080b534dba3bd2af1f62c8fa55869ffff4', 32),
    0x14cb70: (154, '2cb68ebe0f581612118147a09acba73594a33ce0a8fd345ef79b979d98a56f11', None),
    0xb25470: (105, '8e2300c708f4ab249fdcc39e38b4b57d31d4b4457909b7d349bf6e7ff1670023', 23),
}


class Ctx:
    def __init__(self):
        self.elf, self.image = X.load_image()
        require(hashlib.sha256(self.elf).hexdigest() == X.ELF_SHA256, 'unexpected ELF SHA256')
        self.got = X.import_names(self.elf)
        probe = X.Machine(self.image, self.got)
        for t, want in THUNKS.items():
            require(probe.thunk(t) == want, f'thunk {t:#x} is {probe.thunk(t)}, expected {want}')

    def pin(self, rva, size, sha, ninsns, mnemonics=None):
        body = self.image.read(rva, size)
        require(hashlib.sha256(body).hexdigest() == sha, f'{rva:#x} body changed: {hashlib.sha256(body).hexdigest()}')
        insns = X.decode_range(self.image, rva, size)
        require(len(insns) == ninsns, f'{rva:#x}: {len(insns)} instructions decoded, expected {ninsns}')
        require(insns[-1].mn in ('ret', 'jmp'), f'{rva:#x} does not end in ret/jmp')
        if mnemonics is not None:
            require([i.mn for i in insns] == mnemonics, f'{rva:#x} mnemonic sequence changed')
        pad = self.image.read(rva + size, 1)[0]
        require(pad in (0x90, 0xCC, 0x55), f'{rva:#x}: byte after the body is {pad:#x} (padding or the next prologue expected)')
        return insns

    def pin_named(self, rva):
        size, sha, n = PINS[rva]
        if n is None:
            body = self.image.read(rva, size)
            require(hashlib.sha256(body).hexdigest() == sha, f'{rva:#x} body changed')
            return None
        return self.pin(rva, size, sha, n)

    def machine(self, seed=None, poison=0, scratch=()):
        return X.Machine(self.image, self.got, seed, poison, scratch)


def native(name, args=()):
    exe = base.native_executable(name)
    return json.loads(subprocess.run([str(exe), '--dump', *map(str, args)], check=True, capture_output=True, text=True).stdout)


def readme_check(*needles):
    readme = (ROOT / 'native/README.md').read_text()
    for n in needles:
        require(n in readme, f'README is missing {n!r}')


def writes_set(m):
    return sorted({a for a, _ in m.writes})


def hexat(m, addr, n):
    return X.mem_bytes(m, addr, n).hex()


def le(m, addr, n):
    return int.from_bytes(X.mem_bytes(m, addr, n), 'little')


def two_poisons(fn):
    """fn(poison) -> comparable value; both poisons must agree (nothing depends on an unset register)."""
    a, b = fn(0x00), fn(0x5A)
    require(a == b, 'result depends on an uninitialised register / ymm (two poisons disagree)')
    return a


# ---- addresses shared by the 0x14c9a0 / 0x14d880 / 0xb25470 checkers (cold BSS windows) -----------------------
OBJ, OBJ_GUARD, CELL, MANAGER = 0x2ee3b10, 0x2ee3c10, 0x2ee3b00, 0x2ee2d00
HS, HS_GUARD = 0x2ed3640, 0x2ed3668
EMPTY, EMPTY_GUARD = 0x2dd9f50, 0x2dd9f70
ARRAY, ARRAY_BYTES, STRIDE = 0x332f650, 0x1b580, 0x118
SLOTS_MASK = (0x08, 0x10, 0x78, 0xd0)


def cold_seed(obj_bg, side_bg, extra=None):
    """Cold BSS image: object window `obj_bg`, hashed-static + 0xf730 windows `side_bg`, every guard byte 0, cell null."""
    seed = {}
    for a in range(OBJ, OBJ + 0x100):
        seed[a] = obj_bg
    for a in range(HS, HS + 0x28):
        seed[a] = side_bg
    for a in range(EMPTY, EMPTY + 0x30):
        seed[a] = side_bg
    seed.update({OBJ_GUARD: 0, HS_GUARD: 0, EMPTY_GUARD: 0, CELL: 0})
    seed.update(extra or {})
    return seed


def masked_hex(raw, slots):
    b = bytearray(raw)
    for s in slots:
        b[s:s + 8] = bytes(8)
    return bytes(b).hex()


def import_trace(m):
    """Program-order list of (name, rdi, rsi, rdx) for every import call plus the in-image callees' targets."""
    out = []
    for name, target, a in m.calls:
        if name == 'in-image':
            out.append((name, target))
        elif name == '__cxa_atexit':
            out.append((name, a['rdi'], a['rsi'], a['rdx']))
        else:
            out.append((name, a['rdi']))          # guard calls: only the guard address is an argument
    return out
