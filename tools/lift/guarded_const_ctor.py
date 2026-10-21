#!/usr/bin/env python3
"""Strict recognizer for one compiler-emitted idiom: guarded static-constant initializers.

Idiom (x86-64, each repetition 32 bytes in the observed function):
  lea rax,[rip+G]; cmp byte [rax],0; jne next; lea rcx,[rip+V]; mov qword [rax],1; mov dword [rcx],IMM
It is NOT a CPU model: bytes are matched against exact encodings and anything else stops the walk and
is reported as unlifted. The result is data (guard address, value address, 32-bit pattern) from which
ordinary native constants are generated. Offline analysis tool; nothing is executed.
"""
import argparse
import json
from pathlib import Path
import struct
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'scripts'))
import ps4_dynlink as d


def match(img, pc):
    code = img.read(pc, 32)
    if (code[0:3] != b'\x48\x8d\x05' or code[7:10] != b'\x80\x38\x00' or code[10] != 0x75
            or code[12:15] != b'\x48\x8d\x0d' or code[19:26] != b'\x48\xc7\x00\x01\x00\x00\x00'
            or code[26:28] != b'\xc7\x01' or code[11] != 32 - 12):
        return None
    guard = pc + 7 + struct.unpack('<i', code[3:7])[0]
    value = pc + 19 + struct.unpack('<i', code[15:19])[0]
    return dict(guard=guard, value=value, bits=struct.unpack('<I', code[28:32])[0]), pc + 32


def rel32(code, at, pc):
    """Absolute target of a rip-relative disp32 whose field is at code[at:at+4] and insn ends at pc+end."""
    return struct.unpack('<i', code[at:at + 4])[0]


def match_tail(img, pc):
    """The three vector-constant idioms that close the observed constructor (exact encodings)."""
    c = img.read(pc, 64)
    # P2: guarded {0xffffffff,0,0,0}: lea rax,G; cmp [rax],0; jne; lea rdx,V; mov ecx,-1; mov [rax],1; vmovd; vmovups [rdx]
    if (c[0:3] == b'\x48\x8d\x05' and c[7:10] == b'\x80\x38\x00' and c[10] == 0x75 and c[11] == 0x1b
            and c[12:15] == b'\x48\x8d\x15' and c[19:24] == b'\xb9\xff\xff\xff\xff'
            and c[24:31] == b'\x48\xc7\x00\x01\x00\x00\x00' and c[31:35] == b'\xc5\xf9\x6e\xc1'
            and c[35:39] == b'\xc5\xf8\x11\x02'):
        return dict(kind='guarded_vec_u32_lane0', guard=pc + 7 + rel32(c, 3, pc),
                    value=pc + 19 + rel32(c, 15, pc), bits=[0xffffffff, 0, 0, 0]), pc + 39
    # P3: guarded copy of a 16-byte rodata constant
    if (c[0:3] == b'\x48\x8d\x05' and c[7:10] == b'\x80\x38\x00' and c[10] == 0x75 and c[11] == 0x1a
            and c[12:16] == b'\xc5\xf8\x10\x05' and c[20:23] == b'\x48\x8d\x0d'
            and c[27:34] == b'\x48\xc7\x00\x01\x00\x00\x00' and c[34:38] == b'\xc5\xf8\x11\x01'):
        src = pc + 20 + rel32(c, 16, pc)
        return dict(kind='guarded_vec_copy', guard=pc + 7 + rel32(c, 3, pc), source=src,
                    value=pc + 27 + rel32(c, 23, pc), bits=list(struct.unpack('<4I', img.read(src, 16)))), pc + 38
    # P4: unguarded copy then ret
    if c[0:4] == b'\xc5\xf8\x10\x05' and c[8:12] == b'\xc5\xf8\x11\x05' and c[16] == 0xc3:
        src = pc + 8 + rel32(c, 4, pc)
        return dict(kind='vec_copy_then_ret', source=src, value=pc + 16 + rel32(c, 12, pc),
                    bits=list(struct.unpack('<4I', img.read(src, 16)))), pc + 17
    return None


def lift(elf_path, start):
    elf = Path(elf_path).read_bytes()
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)
    pc, out = start, []
    while True:
        try:
            m = match(img, pc)
        except ValueError:
            m = None
        if not m:
            break
        rec, pc = m
        out.append(rec)
    tail = []
    while True:
        try:
            m = match_tail(img, pc)
        except ValueError:
            m = None
        if not m:
            break
        rec, pc = m
        tail.append(rec)
    return out, pc, img.read(pc, 8).hex(), tail


# ---------------------------------------------------------------------------
# Whole-body store decoder for the larger members of the same family.
#
# Several ctors in the 0xdd.../0xd3... range are the same guarded-constant
# pattern followed by a straight-line run of unguarded constant stores. This
# walker recognises each instruction *by exact encoding* and evaluates it into
# an ordered list of (destination, width, bytes) stores plus the guarded-cell
# records. It is deliberately not a general CPU model: any byte sequence that
# is not one of the encodings below raises DecodeError instead of guessing.
# ---------------------------------------------------------------------------


class DecodeError(Exception):
    pass


def _disp32(code, at):
    return struct.unpack_from('<i', code, at)[0]


class _Stores:
    """Symbolic state: gpr -> absolute address or 8 loaded bytes; vec reg -> bytes."""

    def __init__(self):
        self.gpr = {}
        self.vec = {}
        self.records = []

    def store(self, dest, data):
        self.records.append(dict(kind='store', dest=dest, width=len(data), bytes=data.hex()))

    def addr(self, reg):
        value = self.gpr.get(reg)
        if not isinstance(value, tuple) or value[0] != 'addr':
            raise DecodeError(f'{reg} is not a known rip-relative address: {value!r}')
        return value[1]

    def loaded(self, reg):
        value = self.gpr.get(reg)
        if not isinstance(value, tuple) or value[0] != 'bytes':
            raise DecodeError(f'{reg} does not hold loaded bytes: {value!r}')
        return value[1]

    def vector(self, reg, size):
        value = self.vec.get(reg)
        if value is None or len(value) < size:
            raise DecodeError(f'{reg} does not hold {size} bytes: {value!r}')
        return value[:size]


_GPR_NAMES = {0x05: 'rax', 0x0D: 'rcx', 0x15: 'rdx'}
_VEC_NAMES = {0: 'xmm0', 1: 'xmm1', 2: 'xmm2', 3: 'xmm3', 4: 'xmm4', 5: 'xmm5',
              6: 'xmm6', 7: 'xmm7', 8: 'xmm8', 9: 'xmm9'}


def _vec_name(modrm):
    return _VEC_NAMES[(modrm >> 3) & 7]


def _mem_operand(st, code, modrm, pc):
    """Absolute destination for a VEX mod=00 memory operand: rip-relative or [rax]/[rdx]."""
    mod, rm = modrm >> 6, modrm & 7
    if mod == 0 and rm == 5:
        return pc + 8 + _disp32(code, 4)      # c5 xx op modrm disp32
    if mod == 0 and rm == 0:
        return st.addr('rax')
    if mod == 0 and rm == 2:
        return st.addr('rdx')
    raise DecodeError(f'unsupported modrm {modrm:#04x} at {pc:#x}')


def step(img, st, pc):
    """Decode exactly one instruction at pc; return the next pc."""
    c = img.read(pc, 16)
    # --- guarded repetitions (exact encodings; unchanged family matcher) ----
    m = match(img, pc)
    if m:
        rec, nxt = m
        st.records.append(dict(kind='guarded_scalar', guard=rec['guard'], value=rec['value'],
                               width=4, bits=rec['bits']))
        return nxt
    c = img.read(pc, 64)
    if (c[0:3] == b'\x48\x8d\x05' and c[7:10] == b'\x80\x38\x00' and c[10] == 0x75 and c[11] == 0x1b
            and c[12:15] == b'\x48\x8d\x15' and c[19:24] == b'\xb9\xff\xff\xff\xff'
            and c[24:31] == b'\x48\xc7\x00\x01\x00\x00\x00' and c[31:35] == b'\xc5\xf9\x6e\xc1'
            and c[35:39] in (b'\xc5\xf8\x11\x02', b'\xc5\xfa\x7f\x02')):
        st.records.append(dict(
            kind='guarded_lane0', guard=pc + 7 + _disp32(c, 3), value=pc + 19 + _disp32(c, 15),
            bits=[0xffffffff, 0, 0, 0], store='vmovups' if c[35:39] == b'\xc5\xf8\x11\x02' else 'vmovdqu'))
        return pc + 39
    if (c[0:3] == b'\x48\x8d\x05' and c[7:10] == b'\x80\x38\x00' and c[10] == 0x75 and c[11] == 0x1a
            and c[12:16] in (b'\xc5\xf8\x10\x05', b'\xc5\xfa\x6f\x05') and c[20:23] == b'\x48\x8d\x0d'
            and c[27:34] == b'\x48\xc7\x00\x01\x00\x00\x00'
            and c[34:38] in (b'\xc5\xf8\x11\x01', b'\xc5\xfa\x7f\x01')):
        src = pc + 20 + _disp32(c, 16)
        st.records.append(dict(
            kind='guarded_copy', guard=pc + 7 + _disp32(c, 3), value=pc + 27 + _disp32(c, 23),
            source=src, bits=list(struct.unpack('<4I', img.read(src, 16))),
            load='vmovups' if c[12:16] == b'\xc5\xf8\x10\x05' else 'vmovdqu',
            store='vmovups' if c[34:38] == b'\xc5\xf8\x11\x01' else 'vmovdqu'))
        return pc + 38
    # --- scalar moves -------------------------------------------------------
    if c[0] == 0x48 and c[1] in (0x8D, 0x8B, 0x89) and c[2] in _GPR_NAMES:
        reg = _GPR_NAMES[c[2]]
        target = pc + 7 + _disp32(c, 3)
        if c[1] == 0x8D:                        # lea reg,[rip+d]
            st.gpr[reg] = ('addr', target)
        elif c[1] == 0x8B:                      # mov reg,[rip+d]
            st.gpr[reg] = ('bytes', img.read(target, 8))
        else:                                   # mov [rip+d],reg
            st.store(target, st.loaded(reg))
        return pc + 7
    if c[0:2] == b'\xc7\x05':                   # mov dword [rip+d],imm32
        st.store(pc + 10 + _disp32(c, 2), struct.pack('<I', struct.unpack_from('<I', c, 6)[0]))
        return pc + 10
    if c[0] == 0xB9:                            # mov ecx,imm32
        st.gpr['ecx'] = ('bytes', struct.pack('<I', struct.unpack_from('<I', c, 1)[0]))
        return pc + 5
    if c[0] == 0x31 and c[1] == 0xC0:           # xor eax,eax
        st.gpr['rax'] = ('bytes', b'\0' * 8)
        return pc + 2
    # --- vector moves (VEX, 3-byte prefix c5) ------------------------------
    if c[0] == 0xC5:
        p1, op = c[1], c[2]
        if (p1, op) == (0xF8, 0x10):            # vmovups xmm,[m128]
            dest = _vec_name(c[3])
            st.vec[dest] = img.read(pc + 8 + _disp32(c, 4), 16)
            return pc + 8
        if (p1, op) == (0xFA, 0x6F):            # vmovdqu xmm,[m128]
            dest = _vec_name(c[3])
            st.vec[dest] = img.read(pc + 8 + _disp32(c, 4), 16)
            return pc + 8
        if (p1, op) == (0xFC, 0x10):            # vmovups ymm,[m256]
            dest = _vec_name(c[3])
            st.vec[dest] = img.read(pc + 8 + _disp32(c, 4), 32)
            return pc + 8
        if (p1, op) == (0xF8, 0x11):            # vmovups [m128],xmm
            dest = _mem_operand(st, c, c[3], pc)
            st.store(dest, st.vector(_vec_name(c[3]), 16))
            return pc + (8 if (c[3] >> 6) == 0 and (c[3] & 7) == 5 else 4)
        if (p1, op) == (0xFC, 0x11):            # vmovups [m256],ymm
            dest = _mem_operand(st, c, c[3], pc)
            st.store(dest, st.vector(_vec_name(c[3]), 32))
            return pc + (8 if (c[3] >> 6) == 0 and (c[3] & 7) == 5 else 4)
        if (p1, op) == (0xFA, 0x11):            # vmovss [m32],xmm
            dest = _mem_operand(st, c, c[3], pc)
            st.store(dest, st.vector(_vec_name(c[3]), 4))
            return pc + (8 if (c[3] >> 6) == 0 and (c[3] & 7) == 5 else 4)
        if (p1, op) == (0xFA, 0x10):            # vmovss xmm,[m32]
            st.vec[_vec_name(c[3])] = img.read(pc + 8 + _disp32(c, 4), 4) + b'\0' * 12
            return pc + 8
        if (p1, op) == (0xF8, 0x13):            # vmovlps [m64],xmm
            dest = _mem_operand(st, c, c[3], pc)
            st.store(dest, st.vector(_vec_name(c[3]), 8))
            return pc + (8 if (c[3] >> 6) == 0 and (c[3] & 7) == 5 else 4)
        if (p1, op) == (0xF9, 0xD6):            # vmovq [m64],xmm
            dest = _mem_operand(st, c, c[3], pc)
            st.store(dest, st.vector(_vec_name(c[3]), 8))
            return pc + (8 if (c[3] >> 6) == 0 and (c[3] & 7) == 5 else 4)
        if (p1, op) == (0xF9, 0x6E) and c[3] == 0xC1:   # vmovd xmm0,ecx
            st.vec['xmm0'] = st.loaded('ecx') + b'\0' * 12
            return pc + 4
        if (p1, op) == (0xF9, 0x76) and c[3] == 0xC0:
            st.vec['xmm0'] = b'\xff' * 16      # vpcmpeqd xmm0,xmm0,xmm0
            return pc + 4
        if (p1, op) == (0xF1, 0x76) and c[3] == 0xC9:
            st.vec['xmm1'] = b'\xff' * 16      # vpcmpeqd xmm1,xmm1,xmm1
            return pc + 4
        if (p1, op) == (0xF8, 0x57) and c[3] == 0xC0:
            st.vec['xmm0'] = b'\0' * 16        # vxorps xmm0,xmm0,xmm0
            return pc + 4
        if (p1, op) == (0xF0, 0x57) and c[3] == 0xC9:
            st.vec['xmm1'] = b'\0' * 16        # vxorps xmm1,xmm1,xmm1
            return pc + 4
    if c[0] == 0xC3:
        return None
    raise DecodeError(f'unrecognised encoding at {pc:#x}: {c[:16].hex(" ")}')


def decode_body(elf_path, start, limit=4096):
    """Walk one constructor body from `start` to its ret; return (records, end_pc)."""
    elf = Path(elf_path).read_bytes()
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)
    st = _Stores()
    pc = start
    while pc - start <= limit:
        nxt = step(img, st, pc)
        if nxt is None:
            return st.records, pc + 1
        if nxt <= pc:
            raise DecodeError(f'non-advancing decode at {pc:#x}')
        pc = nxt
    raise DecodeError(f'no ret within {limit} bytes of {start:#x}')


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('elf', type=Path)
    ap.add_argument('start', type=lambda v: int(v, 0))
    a = ap.parse_args()
    recs, stop, nxt, tail = lift(a.elf, a.start)
    print(json.dumps(dict(start=hex(a.start), count=len(recs), stopped_at=hex(stop), next_bytes=nxt,
                          tail=tail, constants=[dict(guard=hex(r['guard']), value=hex(r['value']),
                                          bits=hex(r['bits']), as_float=struct.unpack('<f', struct.pack('<I', r['bits']))[0])
                                     for r in recs]), indent=1))


if __name__ == '__main__':
    main()
