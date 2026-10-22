#!/usr/bin/env python3
"""Strict decoders for the 0x52a990 string-hash helper family (CPU-01, offline analysis only).

Nothing from the game is executed as a product runtime; the pinned ELF bytes are decoded by exact
encoding (anything unlisted raises DecodeError) and, for the two tiny helper bodies, interpreted by
a ~15-opcode offline model so the native lift can be compared with the *machine code*, not with an
assumption about it.

  decode_helper(image, rva)  linear decode of a call-free helper: only the encodings in ENC are accepted
  run_helper(image, rva, string, rsi=...)  interpret rva (following the tail jmp) on a byte string
  decode_ctor(image, rva)    decode a name-hash ctor: lea/mov stores and `call 0x52a990` only
"""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/lift'))
import leaf_ctor_decode as base  # noqa: E402

DecodeError = base.DecodeError
load_image = base.load_image
ELF_SHA256 = base.ELF_SHA256
HELPER = 0x52a990
FNV_BODY = 0x161fb0
STEP_LIMIT = 10_000_000
M64 = (1 << 64) - 1
NOPW14 = bytes.fromhex('66666666662e0f1f840000000000')


def decode_one(image, pc):
    """Exact-encoding decode of one helper instruction: (length, mnemonic, operand)."""
    c = image.read(pc, 16)
    if c[0] == 0xBE:
        return 5, 'mov esi,imm32', struct.unpack_from('<I', c, 1)[0]
    if c[0] == 0xE9:
        return 5, 'jmp rel32', pc + 5 + struct.unpack_from('<i', c, 1)[0]
    if c[:2] == b'\x31\xc0':
        return 2, 'xor eax,eax', None
    if c[:3] == b'\x48\x85\xff':
        return 3, 'test rdi,rdi', None
    if c[0] == 0x74:
        return 2, 'je rel8', pc + 2 + struct.unpack_from('<b', c, 1)[0]
    if c[0] == 0x75:
        return 2, 'jne rel8', pc + 2 + struct.unpack_from('<b', c, 1)[0]
    if c[:2] == b'\x8a\x0f':
        return 2, 'mov cl,[rdi]', None
    if c[:2] == b'\x84\xc9':
        return 2, 'test cl,cl', None
    if c[:3] == b'\x48\xff\xc7':
        return 3, 'inc rdi', None
    if c[:2] == b'\x89\xf0':
        return 2, 'mov eax,esi', None
    if c[:14] == NOPW14:
        return 14, 'nopw', None
    if c[:3] == b'\x0f\xbe\xc9':
        return 3, 'movsx ecx,cl', None
    if c[:2] == b'\x31\xc1':
        return 2, 'xor ecx,eax', None
    if c[:2] == b'\x69\xc1':
        return 6, 'imul eax,ecx,imm32', struct.unpack_from('<I', c, 2)[0]
    if c[:3] == b'\x0f\xb6\x0f':
        return 3, 'movzx ecx,byte [rdi]', None
    if c[0] == 0xC3:
        return 1, 'ret', None
    raise DecodeError(f'unsupported encoding {c[:8].hex()} at {pc:#x}')


def decode_helper(image, rva, end):
    """Linear decode of [rva, end) into (pc, length, mnemonic, operand) tuples; must end exactly at end."""
    out, pc = [], rva
    while pc < end:
        n, name, arg = decode_one(image, pc)
        out.append((pc, n, name, arg))
        pc += n
    if pc != end:
        raise DecodeError(f'decode overran: ends at {pc:#x}, expected {end:#x}')
    return out


def run_helper(image, rva, data, rsi, null=False):
    """Interpret the pinned bytes from rva (the 0x52a990 wrapper or 0x161fb0) on a string.

    `data` is the string without its terminating NUL. `null` passes rdi = 0. Returns (eax, trace)."""
    base_addr = 0x7000_0000
    mem = bytes(data) + b'\0'
    reg = {'rax': 0xDEAD_BEEF_0000_0000, 'rcx': 0xCAFE_F00D_0000_0000, 'rdi': 0 if null else base_addr, 'rsi': rsi}
    zf, pc, steps, trace = False, rva, 0, []

    def rd(a):
        if not (base_addr <= a < base_addr + len(mem)):
            raise DecodeError(f'read outside the string at {a:#x}')
        return mem[a - base_addr]

    while True:
        steps += 1
        if steps > STEP_LIMIT:
            raise DecodeError('step limit')
        n, name, arg = decode_one(image, pc)
        trace.append(pc)
        nxt = pc + n
        if name == 'mov esi,imm32':
            reg['rsi'] = arg
        elif name == 'jmp rel32':
            nxt = arg
        elif name == 'xor eax,eax':
            reg['rax'], zf = 0, True
        elif name == 'test rdi,rdi':
            zf = reg['rdi'] == 0
        elif name == 'je rel8':
            nxt = arg if zf else nxt
        elif name == 'jne rel8':
            nxt = arg if not zf else nxt
        elif name == 'mov cl,[rdi]':
            reg['rcx'] = (reg['rcx'] & ~0xFF & M64) | rd(reg['rdi'])
        elif name == 'test cl,cl':
            zf = (reg['rcx'] & 0xFF) == 0
        elif name == 'inc rdi':
            reg['rdi'] = (reg['rdi'] + 1) & M64
            zf = reg['rdi'] == 0
        elif name == 'mov eax,esi':
            reg['rax'] = reg['rsi'] & 0xFFFFFFFF
        elif name == 'nopw':
            pass
        elif name == 'movsx ecx,cl':
            b = reg['rcx'] & 0xFF
            reg['rcx'] = (b - 256 if b >= 128 else b) & 0xFFFFFFFF
        elif name == 'xor ecx,eax':
            reg['rcx'] = (reg['rcx'] ^ reg['rax']) & 0xFFFFFFFF
            zf = reg['rcx'] == 0
        elif name == 'imul eax,ecx,imm32':
            reg['rax'] = (reg['rcx'] * arg) & 0xFFFFFFFF
        elif name == 'movzx ecx,byte [rdi]':
            reg['rcx'] = rd(reg['rdi'])
        elif name == 'ret':
            return reg['rax'] & 0xFFFFFFFF, trace
        pc = nxt


def decode_ctor(image, rva):
    """Decode a 0x52a990 name-hash ctor. Returns dict(records, end, insns, calls).

    records: ordered list of dict(name_addr, ptr_cell, hash_cell). Only these encodings are accepted:
      push rbp; mov rbp,rsp; push rbx; push rax; add rsp,8; pop rbx; pop rbp; ret;
      lea rbx,[rip+d]; lea rdi,[rip+d]; mov [rbx],rdi; mov [rbx+8],eax;
      mov [rip+d],rdi; mov [rip+d],eax; call rel32 (target must be 0x52a990).
    rax is only ever the result of the last call; rdi must hold a lea'd address at each call."""
    pc, rbx, rdi, rax = rva, None, None, None
    insns, records, pending, calls = [], [], {}, []
    steps = 0

    while True:
        steps += 1
        if steps > 64:
            raise DecodeError('ctor body too long for the strict decoder')
        c = image.read(pc, 16)
        if c[:3] == b'\x48\x89\xe5':
            n, nm = 3, 'mov rbp,rsp'
        elif c[0] in (0x55, 0x53, 0x50, 0x5B, 0x5D):
            n, nm = 1, {0x55: 'push rbp', 0x53: 'push rbx', 0x50: 'push rax', 0x5B: 'pop rbx', 0x5D: 'pop rbp'}[c[0]]
        elif c[:4] == b'\x48\x83\xc4\x08':
            n, nm = 4, 'add rsp,8'
        elif c[:3] == b'\x48\x8d\x1d':
            n, nm = 7, 'lea rbx,[rip+d]'
            rbx = pc + 7 + struct.unpack_from('<i', c, 3)[0]
        elif c[:3] == b'\x48\x8d\x3d':
            n, nm = 7, 'lea rdi,[rip+d]'
            rdi = pc + 7 + struct.unpack_from('<i', c, 3)[0]
        elif c[:3] == b'\x48\x89\x3b':
            n, nm = 3, 'mov [rbx],rdi'
            if rbx is None or rdi is None:
                raise DecodeError(f'unknown rbx/rdi at {pc:#x}')
            pending[rbx] = dict(name_addr=rdi, ptr_cell=rbx)
        elif c[:3] == b'\x89\x43\x08':
            n, nm = 3, 'mov [rbx+8],eax'
            if rax != pending[rbx]['name_addr']:
                raise DecodeError(f'eax at {pc:#x} is not the hash of the string stored at [rbx]')
            records.append(dict(pending[rbx], hash_cell=rbx + 8, call=rax))
            rax = None
        elif c[:3] == b'\x48\x89\x3d':
            n, nm = 7, 'mov [rip+d],rdi'
            cell = pc + 7 + struct.unpack_from('<i', c, 3)[0]
            if rdi is None:
                raise DecodeError(f'rdi unknown at {pc:#x}')
            pending[cell] = dict(name_addr=rdi, ptr_cell=cell)
        elif c[:2] == b'\x89\x05':
            n, nm = 6, 'mov [rip+d],eax'
            cell = pc + 6 + struct.unpack_from('<i', c, 2)[0]
            if rax is None:
                raise DecodeError(f'eax not a call result at {pc:#x}')
            owner = [k for k, v in pending.items() if v['name_addr'] == rax and k + 8 == cell]
            if len(owner) != 1:
                raise DecodeError(f'hash store at {pc:#x} matches no pending name cell')
            records.append(dict(pending[owner[0]], hash_cell=cell, call=rax))
            rax = None
        elif c[0] == 0xE8:
            n, nm = 5, 'call rel32'
            tgt = pc + 5 + struct.unpack_from('<i', c, 1)[0]
            if tgt != HELPER:
                raise DecodeError(f'call to {tgt:#x}, not {HELPER:#x}')
            if rdi is None:
                raise DecodeError(f'rdi unknown at call {pc:#x}')
            calls.append(pc)
            rax = rdi          # token: "hash of the string at this address"
            rdi = None
        elif c[0] == 0xC3:
            insns.append((pc, 1, 'ret'))
            return dict(records=records, end=pc + 1, insns=insns, calls=calls)
        else:
            raise DecodeError(f'unsupported encoding {c[:8].hex()} at {pc:#x}')
        insns.append((pc, n, nm))
        pc += n
