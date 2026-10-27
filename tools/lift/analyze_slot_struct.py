#!/usr/bin/env python3
"""Offline *reachability* structural dossier for one original function body.

Unlike tools/lift/survey_ctors.py (linear sweep, can run into the next function), this walks the
control-flow graph from the entry: fallthrough and direct branch/call targets are followed, an
indirect jump stops the walk at an unresolved edge, and `ret`/`ud2` terminate a block. That yields
the real body span plus the direct-call, forward-branch and rip-relative reference inventory.

Nothing is executed; the private pinned ELF is only read. Output is JSON for a dossier to cite.
"""
import argparse
import hashlib
import json
import re
import struct
import subprocess
import sys
from collections import deque
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
# Tab-split parsing: an instruction whose encoded bytes exactly fill objdump's byte column has no
# trailing space, which a `(?:[0-9a-f]{2} )+` pattern silently drops (that truncates long bodies at
# the first >=10-byte instruction). Split on the tab instead.
INSN = re.compile(r'^\s*([0-9a-f]+):\s+([0-9a-f ]+?)\s*\t(\S+)(?:\s+(.*))?$')
DIRECT = re.compile(r'^(0x[0-9a-f]+)')
WORK = Path('/private/var/folders/vx/5z8skhb97w32g817rv7hbh940000gn/T/opencode/slot_struct')
_cache = {}

# Imports whose recovery-known contract never returns. Without this list a fallthrough after a
# noreturn call walks into the padding and then into the next function, inflating the body span.
NORETURN = {
    '__stack_chk_fail', '__cxa_throw', '__cxa_rethrow', 'abort', 'exit', '_exit',
    '__assert_fail', 'longjmp', 'std::terminate', '_ZSt9terminatev',
}


# Longest x86-64 instruction is 15 bytes. A window is decoded with this much extra tail so that an
# instruction starting inside the window but ending past its edge is decoded whole.
OVERLAP = 16


def decode_from(image, addr, limit=0x400):
    """Disassemble a window starting exactly at addr; return {addr: instruction}.

    The table holds only instructions whose *start* lies in [addr, addr + limit). The bytes are read
    with OVERLAP extra tail bytes, so an instruction that straddles the window edge is decoded from
    its real bytes (not as a truncated `<unknown>` row followed by misaligned instructions), and the
    rows that start inside the overlap are discarded: the caller re-decodes from the first missing
    address, which is a true instruction boundary because the table ends on one.
    """
    if addr in _cache:
        return _cache[addr]
    WORK.mkdir(parents=True, exist_ok=True)
    blob = WORK / f'w{addr:x}.bin'
    trim = True
    try:
        try:
            data = image.read(addr, limit + OVERLAP)
        except ValueError:
            # Window ends at the segment edge: no bytes exist past it, so nothing can straddle.
            data = image.read(addr, limit)
            trim = False
        blob.write_bytes(data)
    except ValueError:
        _cache[addr] = {}
        return {}
    text = subprocess.run(
        [str(ROOT / 'scripts/disasm_range.sh'), str(blob), hex(addr)],
        check=True, capture_output=True, text=True).stdout
    table = {}
    for line in text.splitlines():
        m = INSN.match(line)
        if m:
            start = int(m.group(1), 16)
            if trim and start >= addr + limit:
                continue
            table[start] = {
                'addr': start,
                'bytes': bytes.fromhex(m.group(2).replace(' ', '')),
                'mnemonic': m.group(3),
                'operands': (m.group(4) or '').strip(),
            }
    _cache[addr] = table
    return table


def instr_at(image, addr):
    """Instruction at addr, or None.

    The 0x400-byte decode window can end before `addr` (a linear objdump run stops at the window
    edge), which would silently truncate a long straight-line body. Retry with a much larger window
    before concluding that the address is undecodable.
    """
    for table in _cache.values():
        if addr in table:
            return table[addr]
    table = decode_from(image, addr)
    if addr not in table:
        table = decode_from(image, addr, 0x10000)
    return table.get(addr)


def load_image():
    """Return (elf_bytes, image, name_by_nid, nid_at) for the pinned private ELF."""
    elf = (ROOT / 'local/re/eboot.elf').read_bytes()
    digest = hashlib.sha256(elf).hexdigest()
    if digest != ELF_SHA256:
        raise SystemExit(f'unexpected ELF sha256 {digest}')
    _, headers = d.program_headers(elf)
    image = d.Image(elf, headers)
    imports = json.loads((ROOT / 'local/re/imports.json').read_text())
    name_by_nid = {i['nid']: i.get('name') for i in imports['imports']}
    parsed = d.parse(elf)
    nid_at = {}
    for rel in parsed['relocs']:
        if rel['type'] in (1, 6, 7) and rel['symbol'] < len(parsed['symbols']):
            nid_at[rel['target']] = parsed['symbols'][rel['symbol']]['nid']
    return elf, image, name_by_nid, nid_at


def build_report(rva, image, name_by_nid, nid_at, label='', max_instructions=20000):
    """Reachability report for one function entry. Pure w.r.t. the caller's image handle."""
    seen, queue = {}, deque([rva])
    calls, indirect, refs, stores, branches, returns = [], [], [], [], [], 0
    while queue and len(seen) < max_instructions:
        pc = queue.popleft()
        if pc in seen or pc < rva:
            continue
        ins = instr_at(image, pc)
        if ins is None:
            continue
        seen[pc] = ins
        a, mnem, ops, raw = ins['addr'], ins['mnemonic'], ins['operands'], ins['bytes']
        nxt = a + len(raw)
        is_terminal = mnem in ('ret', 'ud2')
        if is_terminal:
            returns += 1
        if mnem == 'call' or mnem == 'jmp':
            m = DIRECT.match(ops)
            if m:
                target = int(m.group(1), 16)
                thunk = None
                try:
                    head = image.read(target, 6)
                except ValueError:
                    head = b''
                if head[:2] == b'\xff\x25':
                    slot = target + 6 + struct.unpack('<i', head[2:6])[0]
                    nid = nid_at.get(slot)
                    thunk = {'got_slot': hex(slot), 'nid': nid, 'name': name_by_nid.get(nid)}
                entry = {'at': hex(a), 'target': hex(target), 'thunk': thunk}
                if mnem == 'call':
                    calls.append(entry)
                    if (thunk or {}).get('name') in NORETURN:
                        continue
                    queue.append(nxt)
                else:
                    if thunk is not None:
                        # Tail call through an import thunk: the callee is external; do not walk it.
                        entry['tail_call'] = True
                        branches.append(entry)
                        continue
                    if not (rva <= target < rva + 0x4000):
                        entry['leaves_body'] = True
                    branches.append(entry)
                    queue.append(target)
                continue
            indirect.append({'at': hex(a), 'mnemonic': mnem, 'operands': ops,
                             'kind': 'call' if mnem == 'call' else 'jump'})
            if mnem == 'call':
                queue.append(nxt)          # an indirect call still returns
            continue
        if mnem.startswith('j') and mnem != 'jmp':
            m = DIRECT.match(ops)
            if m:
                target = int(m.group(1), 16)
                branches.append({'at': hex(a), 'target': hex(target), 'conditional': True})
                queue.append(target)
            else:
                indirect.append({'at': hex(a), 'mnemonic': mnem, 'operands': ops})
            queue.append(nxt)
            continue
        if not is_terminal:
            for m in re.finditer(r'\[rip ([+-]) 0x([0-9a-f]+)\]', ops):
                disp = int(m.group(2), 16) * (1 if m.group(1) == '+' else -1)
                target = a + len(raw) + disp
                try:
                    image.mapped(target, 1)
                    file_backed = True
                except ValueError:
                    file_backed = False
                entry = {'at': hex(a), 'target': hex(target), 'file_backed': file_backed,
                         'mnemonic': mnem, 'operands': ops}
                refs.append(entry)
                head = ops.split(',')[0].strip()
                if '[' in head and not head.startswith('lea'):
                    stores.append(entry)
            queue.append(nxt)

    addrs = sorted(seen)
    if not addrs:
        raise SystemExit(f'no instruction decoded at {rva:#x}')
    lo, hi = addrs[0], addrs[-1]
    # Body = [entry, end of last reachable block], assuming no cold blocks before the entry.
    end = max(a + len(seen[a]['bytes']) for a in addrs)
    body = image.read(lo, end - lo)
    return {
        'label': label,
        'elf_sha256': ELF_SHA256,
        'entry_rva': hex(rva),
        'lowest_reachable': hex(lo),
        'end_exclusive': hex(end),
        'span_bytes': end - lo,
        'body_sha256': hashlib.sha256(body).hexdigest(),
        'reachable_instructions': len(addrs),
        'blocks_terminated_by_ret': returns,
        'direct_calls': calls,
        'branch_edges': branches,
        'indirect_transfers': indirect,
        'rip_relative_refs': refs,
        'rip_relative_stores': stores,
        'outside_image_refs': [r for r in refs if not r['file_backed']],
    }



def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('rva', type=lambda v: int(v, 0))
    ap.add_argument('--label', default='')
    ap.add_argument('--max-instructions', type=int, default=20000)
    args = ap.parse_args()
    _, image, name_by_nid, nid_at = load_image()
    print(json.dumps(build_report(args.rva, image, name_by_nid, nid_at, args.label,
                                  args.max_instructions), indent=1))


if __name__ == '__main__':
    main()
