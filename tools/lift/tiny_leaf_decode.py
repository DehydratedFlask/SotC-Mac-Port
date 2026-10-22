#!/usr/bin/env python3
"""Strict decoder + offline interpreter for tiny call-free leaves (CPU-01; private, needs local/re/eboot.elf).

Used by check_f14ccb0.py / check_fb30cf0.py / check_f158340.py. Only the exact encodings in ENC are
accepted (anything else raises DecodeError); decoding stops at the first `ret`. interpret() executes the
decoded instructions on a small byte-addressed memory image with rdi/rsi/rax and reports the return
register, the final memory, every access (address, width, locked) and whether anything outside the
declared accesses was touched. Nothing is executed on the host CPU; this is a ~10 opcode model.
"""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
M32 = 0xFFFFFFFF


class DecodeError(Exception):
    pass


def load_image():
    elf = (ROOT / 'local/re/eboot.elf').read_bytes()
    _, ph = d.program_headers(elf)
    return elf, d.Image(elf, ph)


def decode_one(c):
    """Return (length, mnemonic, operand) for the instruction at the start of bytes c."""
    if c[:3] == b'\xf0\xff\x47':
        return 4, 'lock inc dword [rdi+disp8]', c[3]
    if c[:4] == b'\xf0\x0f\xc1\x07':
        return 4, 'lock xadd [rdi],eax', None
    if c[0] == 0xC3:
        return 1, 'ret', None
    if c[:2] == b'\x89\x37':
        return 2, 'mov [rdi],esi', None
    if c[:3] == b'\xc6\x47\x04' and c[3] == 0:
        return 4, 'mov byte [rdi+4],0', None
    if c[:2] == b'\x89\xf0':
        return 2, 'mov eax,esi', None
    if c[0] == 0xB8:
        return 5, 'mov eax,imm32', struct.unpack_from('<I', c, 1)[0]
    if c[:3] == b'\x8d\x04\x30':
        return 3, 'lea eax,[rax+rsi]', None
    if c[:2] == b'\xff\xc8':
        return 2, 'dec eax', None
    if c[:2] == b'\xff\xc0':
        return 2, 'inc eax', None
    raise DecodeError(f'unknown encoding {c[:8].hex()}')


def decode(image, rva, max_len=16):
    code = image.read(rva, max_len + 8)
    out, pc = [], 0
    while True:
        n, name, arg = decode_one(code[pc:pc + 8])
        out.append((rva + pc, code[pc:pc + n].hex(), name, arg))
        pc += n
        if name == 'ret':
            break
        if pc > max_len:
            raise DecodeError('no ret within limit')
    return out, bytes(code[:pc])


def interpret(insns, base, mem, rdi, rsi=0):
    """Run decoded insns. mem: bytearray covering [base, base+len). Returns dict(eax, mem, accesses)."""
    rax = 0xDEADBEEF00000000   # upper bits poison: 32-bit writes must zero-extend
    rsi &= (1 << 64) - 1
    acc = []

    def ld(addr, w):
        acc.append(('r', addr, w, False))
        o = addr - base
        if o < 0 or o + w > len(mem):
            raise DecodeError(f'out-of-image read {addr:#x}')
        return int.from_bytes(mem[o:o + w], 'little')

    def st(addr, w, v, locked=False, kind='w'):
        acc.append((kind, addr, w, locked))
        o = addr - base
        if o < 0 or o + w > len(mem):
            raise DecodeError(f'out-of-image write {addr:#x}')
        mem[o:o + w] = (v & ((1 << (8 * w)) - 1)).to_bytes(w, 'little')

    for _, _, name, arg in insns:
        if name == 'ret':
            break
        elif name == 'lock inc dword [rdi+disp8]':
            a = rdi + arg
            acc.append(('rmw', a, 4, True))
            o = a - base
            v = int.from_bytes(mem[o:o + 4], 'little')
            mem[o:o + 4] = ((v + 1) & M32).to_bytes(4, 'little')
        elif name == 'lock xadd [rdi],eax':
            acc.append(('rmw', rdi, 4, True))
            o = rdi - base
            v = int.from_bytes(mem[o:o + 4], 'little')
            mem[o:o + 4] = ((v + (rax & M32)) & M32).to_bytes(4, 'little')
            rax = v                              # old value, zero-extended
        elif name == 'mov [rdi],esi':
            st(rdi, 4, rsi)
        elif name == 'mov byte [rdi+4],0':
            st(rdi + 4, 1, 0)
        elif name == 'mov eax,esi':
            rax = rsi & M32
        elif name == 'mov eax,imm32':
            rax = arg
        elif name == 'lea eax,[rax+rsi]':
            rax = ((rax & M32) + (rsi & M32)) & M32     # low 32 bits of rax+rsi, zero-extended
        elif name == 'dec eax':
            rax = ((rax & M32) - 1) & M32
        elif name == 'inc eax':
            rax = ((rax & M32) + 1) & M32
        else:
            raise DecodeError(name)
    return dict(eax=rax & M32, upper_zero=(rax >> 32) == 0, mem=bytes(mem), accesses=acc)


def s32(v):
    return v - (1 << 32) if v & 0x80000000 else v
