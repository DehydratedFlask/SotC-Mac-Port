#!/usr/bin/env python3
"""Read the PS4 dynamic-linking tables of a plaintext ELF and emit an import manifest.

Offline analysis only: nothing is loaded, relocated or executed. Stdlib only.
Format references (not runtime code): reference/shadPS4/src/core/module.cpp (tags, EncodeId)
and reference/ShadowOfTheColossus-PC/scripts/prepare.py (table spans). Every offset is checked.
"""
import argparse
import collections
import hashlib
import json
from pathlib import Path
import re
import struct

PT_LOAD, PT_DYNAMIC = 1, 2
PT_SCE_DYNLIBDATA, PT_SCE_RELRO = 0x61000000, 0x61000010
DT_SCE = dict(NEEDED_MODULE=0x6100000f, IMPORT_LIB=0x61000015, EXPORT_LIB=0x61000013,
              MODULE_INFO=0x6100000d, JMPREL=0x61000029, PLTRELSZ=0x6100002d,
              RELA=0x6100002f, RELASZ=0x61000031, STRTAB=0x61000035, STRSZ=0x61000037,
              SYMTAB=0x61000039, SYMTABSZ=0x6100003f, PLTGOT=0x61000027)
RELOC_NAMES = {1: 'R_X86_64_64', 6: 'R_X86_64_GLOB_DAT', 7: 'R_X86_64_JUMP_SLOT', 8: 'R_X86_64_RELATIVE'}
SYMBOL_TYPES = {0: 'NOTYPE', 1: 'OBJECT', 2: 'FUNC', 6: 'TLS'}
B64 = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-'


def span(data, offset, size):
    if offset < 0 or size < 0 or offset + size > len(data):
        raise ValueError(f'out of bounds: {offset:#x}+{size:#x} / {len(data):#x}')
    return data[offset:offset + size]


def unpack(fmt, data, offset):
    return struct.unpack(fmt, span(data, offset, struct.calcsize(fmt)))


def encode_id(value):
    """Same encoding as shadPS4 EncodeId: one to three base-64 digits."""
    if value < 0x40:
        return B64[value]
    if value < 0x1000:
        return B64[(value >> 6) & 63] + B64[value & 63]
    return B64[(value >> 12) & 63] + B64[(value >> 6) & 63] + B64[value & 63]


def program_headers(elf):
    header = unpack('<16sHHIQQQIHHHHHH', elf, 0)
    if header[0][:7] != b'\x7fELF\x02\x01\x01' or header[2] != 62:
        raise ValueError('expected little-endian x86-64 ELF')
    if header[9] != 56:
        raise ValueError('expected ELF64 program header size 56')
    keys = ('type', 'flags', 'offset', 'vaddr', 'paddr', 'filesz', 'memsz', 'align')
    ph = [dict(zip(keys, unpack('<IIQQQQQQ', elf, header[5] + i * 56))) for i in range(header[10])]
    return header[4], ph


class Image:
    """vaddr -> file bytes over the admitted loadable segments."""

    def __init__(self, elf, ph):
        self.elf = elf
        self.loads = [p for p in ph if p['type'] in (PT_LOAD, PT_SCE_RELRO)]

    def mapped(self, vaddr, size=1):
        return any(p['vaddr'] <= vaddr and vaddr + size <= p['vaddr'] + p['memsz'] for p in self.loads)

    def read(self, vaddr, size):
        for p in self.loads:
            if p['vaddr'] <= vaddr and vaddr + size <= p['vaddr'] + p['filesz']:
                return span(self.elf, p['offset'] + vaddr - p['vaddr'], size)
        raise ValueError(f'vaddr {vaddr:#x}+{size} not file-backed in a loadable segment')


def parse(elf):
    entry, ph = program_headers(elf)
    dyn_ph = next(p for p in ph if p['type'] == PT_DYNAMIC)
    lib_ph = next(p for p in ph if p['type'] == PT_SCE_DYNLIBDATA)
    tags, multi = {}, collections.defaultdict(list)
    for pos in range(dyn_ph['offset'], dyn_ph['offset'] + dyn_ph['filesz'], 16):
        tag, value = unpack('<QQ', elf, pos)
        if tag == 0:
            break
        tags[tag] = value
        multi[tag].append(value)
    else:
        raise ValueError('unterminated PT_DYNAMIC segment')
    blob = span(elf, lib_ph['offset'], lib_ph['filesz'])
    strings = span(blob, tags[DT_SCE['STRTAB']], tags[DT_SCE['STRSZ']])

    def string(off):
        span(strings, off, 1)
        return strings[off:strings.index(0, off)].decode('ascii')

    modules = {(v >> 48): dict(name=string(v & 0xffffffff), id=v >> 48, enc=encode_id(v >> 48),
                               version=[(v >> 40) & 0xff, (v >> 32) & 0xff])
               for v in multi[DT_SCE['NEEDED_MODULE']]}
    libraries = {(v >> 48): dict(name=string(v & 0xffffffff), id=v >> 48, enc=encode_id(v >> 48),
                                 version=(v >> 32) & 0xffff)
                 for v in multi[DT_SCE['IMPORT_LIB']]}
    exports = {(v >> 48): dict(name=string(v & 0xffffffff), id=v >> 48, enc=encode_id(v >> 48))
               for v in multi[DT_SCE['EXPORT_LIB']]}
    raw = span(blob, tags[DT_SCE['SYMTAB']], tags[DT_SCE['SYMTABSZ']])
    if len(raw) % 24:
        raise ValueError('bad symbol table size')
    symbols = []
    for pos in range(0, len(raw), 24):
        name, info, other, shndx, value, size = unpack('<IBBHQQ', raw, pos)
        full = string(name)
        nid, _, rest = full.partition('#')
        lib_enc, _, mod_enc = rest.partition('#')
        symbols.append(dict(index=pos // 24, nid=nid, lib_enc=lib_enc, mod_enc=mod_enc, raw=full,
                            type=SYMBOL_TYPES.get(info & 15, str(info & 15)), bind=info >> 4,
                            defined=bool(shndx), shndx=shndx, value=value, size=size))
    relocs = []
    for off_tag, size_tag in (('JMPREL', 'PLTRELSZ'), ('RELA', 'RELASZ')):
        table = span(blob, tags[DT_SCE[off_tag]], tags[DT_SCE[size_tag]])
        if len(table) % 24:
            raise ValueError(f'bad {off_tag} size')
        for pos in range(0, len(table), 24):
            target, info, addend = unpack('<QQq', table, pos)
            relocs.append(dict(table=off_tag, target=target, type=info & 0xffffffff,
                               symbol=info >> 32, addend=addend))
    return dict(entry=entry, ph=ph, tags=tags, modules=modules, libraries=libraries,
                exports=exports, symbols=symbols, relocs=relocs, strtab_size=len(strings))


def load_contracts(shadps4_src):
    """NID -> (name, library, module) from the shadPS4 LIB_FUNCTION/LIB_OBJECT declarations."""
    pat = re.compile(r'LIB_(?:FUNCTION|OBJ)\(\s*"([^"]+)"\s*,\s*"([^"]+)"\s*,\s*\d+\s*,\s*'
                     r'"([^"]+)"\s*,\s*([A-Za-z_0-9:]+)')
    table = collections.defaultdict(list)
    for path in sorted(Path(shadps4_src).rglob('*.cpp')):
        for nid, lib, mod, sym in pat.findall(path.read_text(errors='replace')):
            table[nid].append(dict(symbol=sym, library=lib, module=mod, file=str(path)))
    return table


def resolve_thunk(img, addr, got_sites):
    """Decode `jmp qword ptr [rip+disp32]` (FF 25) at addr and map its GOT slot to a relocation."""
    code = img.read(addr, 6)
    if code[:2] != b'\xff\x25':
        return dict(address=hex(addr), kind='not-a-plt-jmp', bytes=code.hex())
    slot = addr + 6 + struct.unpack('<i', code[2:6])[0]
    return dict(address=hex(addr), kind='jmp-got', got_slot=hex(slot), relocation=got_sites.get(slot))


def nid(name):
    """PS4 symbol NID: first 8 bytes of SHA-1(name + salt), reversed, base64 with '/' -> '-'."""
    import base64
    salt = bytes.fromhex('518d64a635ded8c1e6b039b1c3e55230')
    digest = hashlib.sha1(name.encode('ascii') + salt).digest()[:8][::-1]
    return base64.b64encode(digest).decode().rstrip('=').replace('/', '-')


def name_by_hash(nids, roots):
    """NID -> identifier by hashing every identifier-like token in the given text trees.

    The NID is a truncated SHA-1, so a hit is strong (not formal) evidence of the name;
    it proves the spelling only, never the semantics, module version or signature.
    """
    found, wanted = {}, set(nids)
    for root in roots:
        for path in sorted(Path(root).rglob('*')):
            if path.suffix not in ('.cpp', '.h', '.c', '.inc', '.py', '.md', '.txt') or not path.is_file():
                continue
            for tok in set(re.findall(r'[A-Za-z_][A-Za-z0-9_]{3,80}', path.read_text(errors='replace'))):
                n = nid(tok)
                if n in wanted and n not in found:
                    found[n] = tok
    return found


def walk_ctors(img, relocs, start, limit=100000):
    """Walk a -1-terminated pointer list downward from `start`, applying RELATIVE relocations.

    Mirrors the loop observed in the CRT helper (reverse walk, skip 0, stop at -1).
    """
    rel = {r['target']: r['addend'] for r in relocs if r['type'] == 8}
    out, addr = [], start
    while len(out) < limit:
        raw = struct.unpack('<Q', img.read(addr, 8))[0]
        if raw == 0xffffffffffffffff and addr not in rel:
            return dict(start=hex(start), sentinel=hex(addr), entries=out)
        value = rel.get(addr, raw)
        if value:
            out.append(dict(slot=hex(addr), target=hex(value), relocated=addr in rel))
        addr -= 8
    raise ValueError('constructor list not terminated')


def build(elf_path, contracts_src=None, thunks=(), ctor_start=None, name_roots=()):
    elf = Path(elf_path).read_bytes()
    parsed = parse(elf)
    img = Image(elf, parsed['ph'])
    contracts = load_contracts(contracts_src) if contracts_src else {}
    syms = parsed['symbols']
    counts = collections.Counter(RELOC_NAMES.get(r['type'], r['type']) for r in parsed['relocs'])
    sites = collections.defaultdict(list)
    got_sites = {}
    for r in parsed['relocs']:
        if r['type'] == 8:
            if not img.mapped(r['target'], 8):
                raise ValueError(f"relocation outside image: {r['target']:#x}")
            continue
        if r['type'] not in RELOC_NAMES or r['symbol'] >= len(syms):
            raise ValueError(f'unsupported relocation {r}')
        sites[r['symbol']].append(r['target'])
        got_sites[r['target']] = dict(symbol=r['symbol'], type=RELOC_NAMES[r['type']],
                                      nid=syms[r['symbol']]['nid'], addend=r['addend'])
    mod_by_enc = {m['enc']: m['name'] for m in parsed['modules'].values()}
    lib_by_enc = {l['enc']: l['name'] for l in parsed['libraries'].values()}
    hashed = name_by_hash({syms[i]['nid'] for i in sites}, name_roots) if name_roots else {}
    imports, classes = [], collections.Counter()
    for idx, targets in sorted(sites.items()):
        s = syms[idx]
        match = contracts.get(s['nid'], [])
        sel = [c for c in match if c['library'] == lib_by_enc.get(s['lib_enc'])] or match
        status = ('defined-in-image' if s['defined'] else
                  'contract-declared' if sel else
                  'name-by-nid-hash-only' if s['nid'] in hashed else 'UNRESOLVED-no-name')
        classes[status] += 1
        imports.append(dict(symbol_index=idx, nid=s['nid'], name=hashed.get(s['nid']), type=s['type'], defined=s['defined'],
                            library=lib_by_enc.get(s['lib_enc']), module=mod_by_enc.get(s['mod_enc']),
                            sites=len(targets), first_site=hex(min(targets)), status=status,
                            candidates=[dict(symbol=c['symbol'], library=c['library'], module=c['module'])
                                        for c in sel[:3]]))
    return dict(
        elf_sha256=hashlib.sha256(elf).hexdigest(), entry=hex(parsed['entry']),
        dynamic_tags=sorted(hex(t) for t in parsed['tags']),
        modules=sorted(parsed['modules'].values(), key=lambda m: m['id']),
        libraries=sorted(parsed['libraries'].values(), key=lambda l: l['id']),
        exports=sorted(parsed['exports'].values(), key=lambda e: e['id']),
        symbol_count=len(syms), relocation_counts=dict(counts),
        import_status_counts=dict(classes), imports=imports,
        thunks=[resolve_thunk(img, int(t, 0), got_sites) for t in thunks],
        constructors=walk_ctors(img, parsed['relocs'], ctor_start) if ctor_start else None), parsed


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('elf', type=Path)
    ap.add_argument('--out', type=Path, required=True)
    ap.add_argument('--contracts', type=Path, help='shadPS4 src/core/libraries (reference only)')
    ap.add_argument('--ctors-from', type=lambda v: int(v, 0), help='top slot of the -1-terminated ctor list')
    ap.add_argument('--unresolved', type=Path, help='write unresolved imports as JSON lines')
    ap.add_argument('--name-roots', type=Path, nargs='*', default=[], help='text trees to hash for NID names')
    ap.add_argument('--thunk', action='append', default=[], help='PLT stub address to resolve')
    args = ap.parse_args()
    manifest, _ = build(args.elf, args.contracts, args.thunk, args.ctors_from, args.name_roots)
    args.out.write_text(json.dumps(manifest, indent=1) + '\n')
    if args.unresolved:
        with args.unresolved.open('w') as f:
            for i in manifest['imports']:
                if i['status'] != 'contract-declared':
                    f.write(json.dumps(i) + '\n')
    print(f"{len(manifest['imports'])} relocated symbols; {manifest['relocation_counts']}; "
          f"{manifest['import_status_counts']}; modules={len(manifest['modules'])} "
          f"libraries={len(manifest['libraries'])}")


if __name__ == '__main__':
    main()
