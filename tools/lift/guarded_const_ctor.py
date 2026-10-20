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
