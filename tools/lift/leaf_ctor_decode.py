#!/usr/bin/env python3
"""Strict straight-line store decoder for call-free leaf constructor bodies (CPU-01, batch 34).

Offline analysis only; nothing is executed. Every instruction is matched by exact encoding and
anything not listed raises DecodeError, so a body is only reported when every byte is understood.
Supported subset (exactly what the 12 batch-34 bodies use):
  VEX2 vmovups xmm/ymm load+store, vmovss load, vxorps zero idiom (all-same register),
  lea r64,[rip+d]; movabs r64,imm64; mov [mem],r64; mov qword/dword/byte [mem],imm; ret.
Memory operands: [rip+d32], [reg+disp8/disp32] where reg is rax/rcx/rdx (a rip-derived constant).
Loads are only accepted from file-backed bytes of the pinned ELF image; the result is the ordered
list of stores (destination, width, little-endian bytes) plus the sources read.
"""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
GPR = {0: 'rax', 1: 'rcx', 2: 'rdx'}


class DecodeError(Exception):
    pass


def load_image(elf_path=None):
    elf = Path(elf_path or ROOT / 'local/re/eboot.elf').read_bytes()
    _, headers = d.program_headers(elf)
    return elf, d.Image(elf, headers)


def _mem(c, i, pc, imm_len, gpr):
    """Parse the memory operand whose modrm byte is c[i]; return (address, bytes consumed from i)."""
    modrm = c[i]
    mod, rm = modrm >> 6, modrm & 7
    if mod == 0 and rm == 5:
        disp = struct.unpack_from('<i', c, i + 1)[0]
        return pc + i + 5 + imm_len + disp, 5
    if rm == 4 or mod == 3:
        raise DecodeError(f'unsupported addressing modrm {modrm:#04x} at {pc:#x}')
    if rm not in GPR:
        raise DecodeError(f'unsupported base register {rm} at {pc:#x}')
    base = gpr.get(GPR[rm])
    if base is None:
        raise DecodeError(f'{GPR[rm]} is not a known address at {pc:#x}')
    if mod == 0:
        return base, 1
    if mod == 1:
        return base + struct.unpack_from('<b', c, i + 1)[0], 2
    return base + struct.unpack_from('<i', c, i + 1)[0], 5


def decode(image, rva):
    """Walk from rva to its ret. Returns dict(stores, sources, insns, end)."""
    gpr, vec = {}, {}
    stores, sources, insns = [], [], []
    pc = rva

    def store(dest, data):
        stores.append(dict(dest=dest, width=len(data), bytes=bytes(data).hex()))

    def load(addr, size):
        try:
            data = image.read(addr, size)
        except ValueError as exc:
            raise DecodeError(f'load from non-file-backed {addr:#x}+{size} at {pc:#x}') from exc
        sources.append(dict(addr=addr, size=size, bytes=data.hex()))
        return data

    while True:
        c = image.read(pc, 16)
        n = None
        if c[0] == 0xC3:
            insns.append((pc, 1, 'ret'))
            return dict(stores=stores, sources=sources, insns=insns, end=pc + 1)
        if c[0] == 0xC5:                                   # VEX2
            p1, op = c[1], c[2]
            if p1 & 0x80 == 0:
                raise DecodeError(f'high register in VEX at {pc:#x}')
            vvvv, length256, pp = (~(p1 >> 3)) & 0xF, (p1 >> 2) & 1, p1 & 3
            modrm = c[3]
            reg = (modrm >> 3) & 7
            size = 32 if length256 else 16
            if op == 0x57 and pp == 0 and modrm >> 6 == 3 and vvvv == reg == (modrm & 7):
                vec[reg] = bytes(size) + bytes(32 - size)          # vxorps zero idiom
                n, name = 4, f'vxorps v{reg}'
            elif op in (0x10, 0x11) and pp == 0:                    # vmovups
                addr, used = _mem(c, 3, pc, 0, gpr)
                n = 3 + used
                if op == 0x10:
                    vec[reg] = load(addr, size) + bytes(32 - size)
                else:
                    if reg not in vec:
                        raise DecodeError(f'v{reg} unset at {pc:#x}')
                    store(addr, vec[reg][:size])
                name = f'vmovups v{reg}/{size}'
            elif op == 0x10 and pp == 2 and not length256:          # vmovss load (upper lanes zeroed)
                addr, used = _mem(c, 3, pc, 0, gpr)
                n = 3 + used
                vec[reg] = load(addr, 4) + bytes(28)
                name = f'vmovss v{reg}'
            elif op == 0x11 and pp == 2 and not length256:          # vmovss store
                addr, used = _mem(c, 3, pc, 0, gpr)
                n = 3 + used
                store(addr, vec[reg][:4])
                name = f'vmovss store v{reg}'
            else:
                raise DecodeError(f'unsupported VEX {c[:5].hex()} at {pc:#x}')
        elif c[0] == 0x48 and c[1] == 0x8D and (c[2] & 0xC7) == 0x05 and ((c[2] >> 3) & 7) in GPR:
            r = (c[2] >> 3) & 7
            gpr[GPR[r]] = pc + 7 + struct.unpack_from('<i', c, 3)[0]
            n, name = 7, f'lea {GPR[r]}'
        elif c[0] == 0x48 and 0xB8 <= c[1] <= 0xBF and (c[1] - 0xB8) in GPR:
            r = c[1] - 0xB8
            gpr[GPR[r]] = struct.unpack_from('<Q', c, 2)[0]
            gpr[GPR[r] + '_value'] = True
            n, name = 10, f'movabs {GPR[r]}'
        elif c[0] == 0x48 and c[1] == 0x89 and ((c[2] >> 3) & 7) in GPR:       # mov [mem], r64
            r = (c[2] >> 3) & 7
            addr, used = _mem(c, 2, pc, 0, gpr)
            if not gpr.get(GPR[r] + '_value'):
                raise DecodeError(f'{GPR[r]} does not hold a movabs value at {pc:#x}')
            store(addr, struct.pack('<Q', gpr[GPR[r]]))
            n, name = 2 + used, f'mov [mem],{GPR[r]}'
        elif c[0] == 0x48 and c[1] == 0xC7 and (c[2] >> 3) & 7 == 0:           # mov qword [mem], simm32
            addr, used = _mem(c, 2, pc, 4, gpr)
            imm = struct.unpack_from('<i', c, 2 + used)[0]
            store(addr, struct.pack('<q', imm))
            n, name = 2 + used + 4, 'mov qword imm'
        elif c[0] == 0xC7 and (c[1] >> 3) & 7 == 0:                            # mov dword [mem], imm32
            addr, used = _mem(c, 1, pc, 4, gpr)
            store(addr, c[1 + used:1 + used + 4])
            n, name = 1 + used + 4, 'mov dword imm'
        elif c[0] == 0xC6 and (c[1] >> 3) & 7 == 0:                            # mov byte [mem], imm8
            addr, used = _mem(c, 1, pc, 1, gpr)
            store(addr, c[1 + used:1 + used + 1])
            n, name = 1 + used + 1, 'mov byte imm'
        else:
            raise DecodeError(f'unrecognised encoding at {pc:#x}: {c.hex(" ")}')
        insns.append((pc, n, name))
        pc += n


def region_image(stores, fill=0):
    """(base, bytes) of the span covered by the stores, bytes outside any store set to fill."""
    base = min(s['dest'] for s in stores)
    end = max(s['dest'] + s['width'] for s in stores)
    buf = bytearray([fill]) * (end - base)
    for s in stores:
        buf[s['dest'] - base:s['dest'] - base + s['width']] = bytes.fromhex(s['bytes'])
    return base, bytes(buf)


if __name__ == '__main__':
    elf, image = load_image()
    for arg in sys.argv[1:]:
        r = decode(image, int(arg, 16))
        print(arg, r['end'] - int(arg, 16), len(r['stores']), 'stores')
