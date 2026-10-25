#!/usr/bin/env python3
"""Private offline reaching-origin observations over a hash-pinned cached CFG.

The public tool contains no game instructions or per-site CFG data. Its JSON output
is private evidence, never a codec, ABI, reachability, or runtime completion claim.
Calls clobber every tracked register unless --preserve-register explicitly names it.
LEA origins denote symbolic expressions at definition sites, not concrete addresses.
"""
from __future__ import annotations

import argparse
from collections import Counter, deque
from dataclasses import dataclass, replace
import hashlib
import json
from pathlib import Path
import re
import struct
import subprocess
import sys

EXPECTED_BASELINE = '32e866ef4213d42bf353c7709ea57383abe2e7f8523386f0fd472dd74fcd5b88'
EXPECTED_CACHE = '6e1a0862a285d9347cc0bd8d37910a9fd9dcb6278511e308e94d42c3441e9bd9'
EXPECTED_ELF = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
REGISTERS = ('rax', 'rbx', 'rcx', 'rdx', 'rsi', 'rdi', 'rbp', 'rsp') + tuple(f'r{i}' for i in range(8, 16))
ALIASES = {}
for family, names in zip(REGISTERS[:8], (
        ('rax', 'eax', 'ax', 'al', 'ah'), ('rbx', 'ebx', 'bx', 'bl', 'bh'),
        ('rcx', 'ecx', 'cx', 'cl', 'ch'), ('rdx', 'edx', 'dx', 'dl', 'dh'),
        ('rsi', 'esi', 'si', 'sil'), ('rdi', 'edi', 'di', 'dil'),
        ('rbp', 'ebp', 'bp', 'bpl'), ('rsp', 'esp', 'sp', 'spl'))):
    for name in names:
        ALIASES[name] = (family, 64 if name == family else 32 if name.startswith('e') else 16 if len(name) == 2 and name.endswith(('x', 'p', 'i')) else 8)
for index in range(8, 16):
    for suffix, width in (('', 64), ('d', 32), ('w', 16), ('b', 8)):
        ALIASES[f'r{index}{suffix}'] = (f'r{index}', width)
MNEMONIC_ALIASES = {'jz': 'je', 'jnz': 'jne', 'jc': 'jb', 'jnc': 'jae', 'cmovc': 'cmovb', 'sal': 'shl', 'movabs': 'mov', 'retq': 'ret', 'callq': 'call', 'jmpq': 'jmp'}
SAFE_READ_ONLY = {'cmp', 'test', 'nop', 'ret', 'int3', 'ud2', 'endbr64'}
VECTOR_ONLY = {'vmovups', 'vmovaps', 'movups', 'movaps', 'vxorps', 'xorps', 'vzeroupper'}
STACK_OPS = {'push', 'pop'}
KNOWN_FIRST_WRITES = {'mov', 'lea', 'xor', 'add', 'sub', 'and', 'or', 'shl', 'shr', 'sar', 'imul', 'inc', 'dec', 'neg', 'not', 'sbb', 'adc', 'movzx', 'movsx', 'movsxd', 'bswap'}


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


@dataclass(frozen=True)
class Instruction:
    address: int
    raw: bytes
    mnemonic: str
    operands: tuple[str, ...] = ()

    @property
    def end(self):
        return self.address + len(self.raw)


@dataclass(frozen=True, order=True)
class Origin:
    kind: str
    detail: str
    definition: int = -1
    assumptions: tuple[str, ...] = ()


def unknown(reason: str, address: int = -1):
    return frozenset({Origin('unknown', reason, address)})


def number(text: str):
    try:
        return int(text, 0)
    except ValueError:
        return None


def canonical_operand(text: str, end: int) -> str:
    text = re.sub(r'<[^>]*>', '', text.lower()).split('#', 1)[0].strip()
    # Ghidra renders RIP-relative operands as their absolute effective addresses.
    def rip(match):
        displacement = int(match.group(2), 0)
        return '[' + hex(end + (displacement if match.group(1) == '+' else -displacement)) + ']'
    text = re.sub(r'\[rip\s*([+-])\s*(0x[0-9a-f]+|\d+)\]', rip, text)
    text = re.sub(r'(?<![a-z0-9_])(-?0x[0-9a-f]+|\d+)(?![a-z0-9_])', lambda m: str(int(m.group(), 0)), text)
    text = re.sub(r'\s+', '', text).replace('+-', '-')
    # Canonicalize commutative SIB multiplication, preserving every scale.
    text = re.sub(r'(?<![a-z0-9_])(\d+)\*([a-z][a-z0-9]*)', lambda m: m[2] + '*' + m[1] if m[2] in ALIASES else m[0], text)
    text = re.sub(r'([a-z][a-z0-9]*)\*1(?![0-9])', lambda m: m[1] if m[1] in ALIASES else m[0], text)
    return text


def parse_text(address: int, raw: bytes, text: str) -> Instruction:
    parts = text.strip().lower().split(None, 1)
    mnemonic = MNEMONIC_ALIASES.get(parts[0], parts[0])
    operands = tuple(canonical_operand(x, address + len(raw)) for x in parts[1].split(',')) if len(parts) > 1 else ()
    if mnemonic == 'nop':
        # Multi-byte NOP has no memory access; providers disagree on this
        # decorative operand width. Keep its complete addressing expression.
        operands = tuple(re.sub(r'^(?:word|dword)ptr', 'ptr', operand) for operand in operands)
    return Instruction(address, raw, mnemonic, operands)


def parse_objdump(text: str) -> list[Instruction]:
    result = []
    for line in text.splitlines():
        match = re.match(r'^\s*([0-9a-f]+):\s+((?:[0-9a-f]{2}\s+)+)\s*([^\s].*)$', line)
        if match:
            result.append(parse_text(int(match[1], 16), bytes.fromhex(match[2]), match[3]))
    return result


def elf_segments(raw: bytes):
    if len(raw) < 64 or raw[:7] != b'\x7fELF\x02\x01\x01' or struct.unpack_from('<H', raw, 18)[0] != 62:
        raise ValueError('expected little-endian ELF64 x86-64')
    offset = struct.unpack_from('<Q', raw, 32)[0]
    size, count = struct.unpack_from('<HH', raw, 54)
    if size < 56 or offset + size * count > len(raw):
        raise ValueError('invalid ELF program headers')
    segments = []
    for index in range(count):
        kind, flags, file_offset, address, _, file_size, memory_size, _ = struct.unpack_from('<IIQQQQQQ', raw, offset + size * index)
        if kind == 1 and flags & 1:
            if file_offset + file_size > len(raw) or file_size > memory_size:
                raise ValueError('invalid executable PT_LOAD')
            segments.append((address, file_offset, file_size))
    if not segments:
        raise ValueError('no executable PT_LOAD')
    return segments


def image_bytes(raw: bytes, segments, address: int, size: int):
    matches = [(offset + address - base) for base, offset, length in segments if base <= address and address + size <= base + length]
    if len(matches) != 1:
        raise ValueError('instruction is outside a unique file-backed executable PT_LOAD')
    return raw[matches[0]:matches[0] + size]


def load_pinned(baseline: Path, cache: Path, elf: Path):
    values = []
    for path, expected in ((baseline, EXPECTED_BASELINE), (cache, EXPECTED_CACHE), (elf, EXPECTED_ELF)):
        raw = path.read_bytes()
        if digest(raw) != expected:
            raise ValueError(f'pinned input SHA256 mismatch: {path}')
        values.append(raw)
    previous, capture = (json.loads(x) for x in values[:2])
    if previous.get('elf_sha256') != EXPECTED_ELF or not capture.get('ok'):
        raise ValueError('input source identities disagree')
    data = capture['data']
    if data['subject']['digest']['sha256'] != EXPECTED_ELF or data['provider']['id'] != 'ghidra':
        raise ValueError('cache subject/provider identity disagrees')
    if previous['candidate_count'] != 15 or previous['analyzed'] != 15 or len(previous['callsites']) != 15:
        raise ValueError('baseline site coverage changed')
    if len({row['callsite'] for row in previous['callsites']}) != 15:
        raise ValueError('duplicate baseline sites')
    value = data['raw_result']['value']
    if len(value['basic_blocks']) != 109 or len(value['assembly']) != 1255:
        raise ValueError('cached CFG coverage changed')
    elf_segments(values[2])
    return previous, capture, values[2]


def decode_and_reconcile(previous: dict, capture: dict, raw: bytes, objdump: str, elf_path: str):
    segments = elf_segments(raw)
    instructions, commands = {}, []
    # Caller supplies the actual pinned ELF path, replacing the historical capture path.
    for function in previous['sweep_provenance'].values():
        for start, end in function['ranges_inclusive']:
            start, end = int(start, 16), int(end, 16) + 1
            argv = [objdump, '-d', '-M', 'intel', f'--start-address={start:#x}', f'--stop-address={end:#x}', elf_path]
            process = subprocess.run(argv, text=True, capture_output=True, check=False)
            commands.append({'argv': argv, 'exit': process.returncode, 'stdout_sha256': digest(process.stdout.encode()), 'stderr_sha256': digest(process.stderr.encode())})
            if process.returncode:
                raise ValueError(f'objdump exit {process.returncode}')
            decoded = parse_objdump(process.stdout)
            cursor = start
            for ins in decoded:
                if ins.address != cursor or not ins.raw or ins.end > end or '(bad)' in ins.mnemonic:
                    raise ValueError('non-contiguous or invalid disassembly')
                if image_bytes(raw, segments, ins.address, len(ins.raw)) != ins.raw:
                    raise ValueError('decoded instruction bytes differ from ELF')
                if ins.address in instructions:
                    raise ValueError('overlapping decoded ranges')
                instructions[ins.address] = ins
                cursor = ins.end
            if cursor != end:
                raise ValueError('disassembly did not cover full range')
    value = capture['data']['raw_result']['value']
    cached = set()
    for line in value['assembly']:
        address, text = line.split(':', 1)
        address = int(address, 16)
        decoded = instructions.get(address)
        if decoded is None or address in cached:
            raise ValueError('cached instruction boundary missing or duplicated')
        comparison = parse_text(address, decoded.raw, text)
        if comparison != decoded:
            raise ValueError(f'cache/ELF instruction disagreement at {address:#x}')
        cached.add(address)
    ranges = [(int(r['start'], 16), int(r['end'], 16) + 1) for r in value['procedure']['body']['ranges']]
    if cached != {address for address in instructions if any(start <= address < end for start, end in ranges)}:
        raise ValueError('cache/ELF function coverage differs')
    for row in previous['callsites']:
        ins = instructions[int(row['callsite'], 16)]
        if ins.raw.hex() != row['bytes'] or ins.mnemonic != 'call' or number(ins.operands[0]) != int(row['decoded_target'], 16):
            raise ValueError('baseline call bytes/target disagree')
        for definition in row['args'].values():
            if definition is None:
                continue
            ins = instructions[int(definition['addr'], 16)]
            if ins.raw.hex() != definition['bytes'] or ins != parse_text(ins.address, ins.raw, definition['text']):
                raise ValueError('baseline argument definition disagrees')
    return instructions, commands


@dataclass(frozen=True)
class Block:
    start: int
    end: int
    successors: tuple[int, ...]


def transfer(ins: Instruction, state: dict, preserved: frozenset[str]):
    state = dict(state)
    mnemonic, ops = ins.mnemonic, ins.operands
    if mnemonic == 'call':
        for reg in REGISTERS:
            state[reg] = frozenset(replace(origin, assumptions=tuple(sorted(set(origin.assumptions) | {reg}))) for origin in state[reg]) if reg in preserved else unknown('call-clobber', ins.address)
        return state
    if mnemonic.startswith('j') or mnemonic in SAFE_READ_ONLY or mnemonic in VECTOR_ONLY:
        return state
    if mnemonic in STACK_OPS:
        old = state['rsp']
        state['rsp'] = frozenset({Origin('stack-expression', mnemonic, ins.address, tuple(sorted({a for x in old for a in x.assumptions})))}) | frozenset(x for x in old if x.kind == 'unknown')
        if mnemonic == 'pop' and ops and ops[0] in ALIASES:
            state[ALIASES[ops[0]][0]] = unknown('stack-load-not-tracked', ins.address)
        return state
    destination = ALIASES.get(ops[0]) if ops else None
    if destination is None:
        # Only these enumerated memory/vector writes have no implicit GPR outputs.
        if mnemonic in {'mov', 'lea'} and ops and ('[' in ops[0] or re.fullmatch(r'[xyz]mm\d+', ops[0])):
            return state
        return {reg: unknown('unsupported-implicit-write', ins.address) for reg in REGISTERS}
    family, width = destination
    if width < 32:
        state[family] = unknown('partial-register-write', ins.address)
        return state
    if mnemonic == 'mov' and len(ops) == 2:
        source = ALIASES.get(ops[1])
        immediate = number(ops[1])
        if source and width == 64 and source[1] == 64:
            result = state[source[0]]
        elif immediate is not None:
            result = frozenset({Origin('immediate', str(immediate & ((1 << width) - 1)), ins.address)})
        elif source and width == 32 and source[1] == 32 and all(x.kind == 'immediate' for x in state[source[0]]):
            result = frozenset(replace(x, detail=str(int(x.detail) & 0xffffffff)) for x in state[source[0]])
        else:
            result = unknown('memory-load-or-unsupported-copy', ins.address)
    elif mnemonic == 'lea' and width == 64 and len(ops) == 2:
        dependencies = {ALIASES[x][0] for x in re.findall(r'\b[a-z][a-z0-9]*\b', ops[1]) if x in ALIASES}
        inherited = frozenset(x for reg in dependencies for x in state[reg] if x.kind == 'unknown')
        assumptions = tuple(sorted({a for reg in dependencies for x in state[reg] for a in x.assumptions}))
        result = frozenset({Origin('lea-expression', ops[1], ins.address, assumptions)}) | inherited
    elif mnemonic == 'xor' and len(ops) == 2 and ops[0] == ops[1]:
        result = frozenset({Origin('immediate', '0', ins.address)})
    elif family == 'rsp' and width == 64 and mnemonic in {'add', 'sub', 'and'} and len(ops) == 2 and number(ops[1]) is not None:
        result = frozenset({Origin('stack-expression', mnemonic + ':' + ops[1], ins.address, tuple(sorted({a for x in state[family] for a in x.assumptions})))}) | frozenset(x for x in state[family] if x.kind == 'unknown')
    else:
        result = unknown('unsupported-register-write', ins.address)
    state[family] = result
    # Recognized first-destination instructions have no other GPR outputs. Unknown
    # instructions and implicit forms (e.g. one-operand IMUL) invalidate all GPRs.
    if mnemonic not in KNOWN_FIRST_WRITES or (mnemonic == 'imul' and len(ops) < 2):
        return {reg: unknown('unsupported-implicit-write', ins.address) for reg in REGISTERS}
    return state


def solve(blocks: list[Block], instructions: dict[int, Instruction], entry: int, preserved=frozenset()):
    """Least fixed point over observed edges; union keeps alternatives through loops.

    Missing decodable edges seed unknown input at the omitted destination. Dangling
    and unbounded flows seed unknown inputs everywhere. Observed indirect successor
    sets remain an explicit unverified graph assumption in the returned diagnostics.
    """
    preserved = frozenset(preserved)
    if not preserved <= set(REGISTERS):
        raise ValueError('preservation requires canonical GPR names')
    by_start = {b.start: b for b in blocks}
    if len(by_start) != len(blocks) or entry not in by_start:
        raise ValueError('duplicate blocks or missing entry')
    members, owner = {}, {}
    for block in sorted(blocks, key=lambda b: b.start):
        if block.start >= block.end:
            raise ValueError('empty/inverted basic block')
        chunk = [ins for address, ins in sorted(instructions.items()) if block.start <= address < block.end]
        cursor = block.start
        for ins in chunk:
            if ins.address != cursor or ins.end > block.end or ins.address in owner:
                raise ValueError('block instructions overlap or leave a gap')
            owner[ins.address] = block.start
            cursor = ins.end
        if cursor != block.end:
            raise ValueError('block boundary disagrees with instruction ends')
        members[block.start] = chunk
    if set(owner) != set(instructions):
        raise ValueError('instructions outside cached blocks')
    predecessors = {start: set() for start in by_start}
    diagnostics = []
    gaps = {start: set() for start in by_start}
    broad_gap = False
    for block in blocks:
        for target in block.successors:
            if target not in by_start:
                diagnostics.append({'kind': 'dangling-successor', 'block': hex(block.start), 'target': hex(target)})
                broad_gap = True
            else:
                predecessors[target].add(block.start)
        last = members[block.start][-1]
        expected = set()
        if last.mnemonic == 'jmp':
            target = number(last.operands[0]) if last.operands else None
            if target is None:
                diagnostics.append({'kind': 'indirect-successor-set-unverified', 'block': hex(block.start)})
                if not block.successors:
                    broad_gap = True
            else:
                expected.add(target)
        elif last.mnemonic.startswith('j'):
            target = number(last.operands[0]) if last.operands else None
            if target is None:
                broad_gap = True
                diagnostics.append({'kind': 'unknown-conditional-target', 'block': hex(block.start)})
            else:
                expected.update((target, last.end))
        elif last.mnemonic not in {'ret', 'ud2', 'int3'}:
            if last.end in by_start:
                expected.add(last.end)
            else:
                diagnostics.append({'kind': 'terminal-fallthrough-not-modelled', 'block': hex(block.start)})
                broad_gap = True
        for target in expected - set(block.successors):
            diagnostics.append({'kind': 'missing-edge', 'block': hex(block.start), 'target': hex(target)})
            if target in by_start:
                gaps[target].add('missing-cfg-edge')
            else:
                broad_gap = True
        # Unexpected edges are retained as conservative additional predecessors.
        if expected and set(block.successors) - expected:
            diagnostics.append({'kind': 'additional-cached-edge', 'block': hex(block.start)})
    observed = set()
    pending = [entry]
    while pending:
        start = pending.pop()
        if start in observed:
            continue
        observed.add(start)
        pending.extend(x for x in by_start[start].successors if x in by_start)
    for start in by_start:
        if start not in observed:
            gaps[start].add('disconnected-cached-block')
        if not predecessors[start] and start != entry:
            gaps[start].add('no-cached-predecessor')
        if broad_gap:
            gaps[start].add('unbounded-or-terminal-cfg-gap')
    seeds = {}
    for start in by_start:
        seeds[start] = {reg: frozenset(Origin('unknown', reason, start) for reason in gaps[start]) for reg in REGISTERS}
    for reg in REGISTERS:
        origin = Origin('symbolic-entry-rsp', 'rsp') if reg == 'rsp' else Origin('unknown', 'entry-register:' + reg)
        seeds[entry][reg] |= frozenset({origin})
    incoming = {start: {reg: frozenset() for reg in REGISTERS} for start in by_start}
    outgoing = {start: {reg: frozenset() for reg in REGISTERS} for start in by_start}
    queue, queued = deque(sorted(by_start)), set(by_start)
    transfers = 0
    while queue:
        start = queue.popleft()
        queued.remove(start)
        merged = {reg: seeds[start][reg] | frozenset(origin for pred in predecessors[start] for origin in outgoing[pred][reg]) for reg in REGISTERS}
        state = merged
        for ins in members[start]:
            state = transfer(ins, state, preserved)
        incoming[start] = merged
        transfers += 1
        if state != outgoing[start]:
            outgoing[start] = state
            for target in sorted(by_start[start].successors):
                if target in by_start and target not in queued:
                    queue.append(target)
                    queued.add(target)
    before = {}
    for start in sorted(by_start):
        state = incoming[start]
        for ins in members[start]:
            before[ins.address] = state
            state = transfer(ins, state, preserved)
    return before, {'block_count': len(blocks), 'instruction_count': len(instructions), 'transfer_count': transfers,
                    'diagnostics': sorted(diagnostics, key=lambda d: json.dumps(d, sort_keys=True)),
                    'disconnected_blocks': len(set(by_start) - observed),
                    'graph_completeness': 'NOT_ESTABLISHED',
                    'preserved_register_assumption': sorted(preserved),
                    'origin_scope': 'conditional on observed cached edges and instruction transfer subset'}


def describe(origins):
    ordered = sorted(origins)
    known = [x for x in ordered if x.kind != 'unknown']
    has_unknown = any(x.kind == 'unknown' for x in ordered)
    signatures = {(x.kind, x.detail, x.definition if x.kind != 'immediate' else -1) for x in known}
    status = 'unknown' if not known else 'conditional-origins-with-unknown' if has_unknown else 'conditional-unique-origin' if len(signatures) == 1 else 'conditional-multiple-origins'
    return {'status': status, 'has_unknown': has_unknown,
            'origins': [{'kind': x.kind, 'detail': x.detail, 'definition': hex(x.definition) if x.definition >= 0 else None,
                         'preservation_assumptions': list(x.assumptions)} for x in ordered]}


def analyze(previous, capture, instructions, preserved=frozenset()):
    value = capture['data']['raw_result']['value']
    blocks = [Block(int(b['start'], 16), int(b['end'], 16), tuple(int(s, 16) for s in b['successors'])) for b in value['basic_blocks']]
    scope = {int(line.split(':', 1)[0], 16) for line in value['assembly']}
    before, summary = solve(blocks, {a: ins for a, ins in instructions.items() if a in scope}, int(value['procedure']['address'], 16), preserved)
    sites = []
    for baseline in sorted(previous['callsites'], key=lambda row: int(row['callsite'], 16)):
        address = int(baseline['callsite'], 16)
        covered = address in before
        sites.append({'callsite': baseline['callsite'], 'function': baseline['function'],
                      'coverage': 'cached-cfg' if covered else 'baseline-only-no-cached-cfg',
                      'baseline_arg3_slot': baseline['arg3_slot'],
                      'baseline_arg3_unresolved': baseline['arg3_slot'] is None,
                      'registers': {reg: describe(before[address][reg] if covered else unknown('no-cached-cfg')) for reg in ('rdi', 'rsi', 'rdx')}})
    previous_unknown = [s for s in sites if s['baseline_arg3_unresolved']]
    return {'cfg': summary, 'callsites': sites, 'aggregate': {
        'site_count': len(sites), 'cfg_site_count': sum(s['coverage'] == 'cached-cfg' for s in sites),
        'baseline_only_site_count': sum(s['coverage'] != 'cached-cfg' for s in sites),
        'baseline_unresolved_arg3_count': len(previous_unknown),
        'previously_unresolved_arg3_status_counts': dict(sorted(Counter(s['registers']['rdx']['status'] for s in previous_unknown).items()))}}


def private_output_path(path: Path):
    """Refuse per-site output unless Git confirms the destination is ignored."""
    resolved = path.expanduser().resolve()
    ancestor = resolved.parent
    while not ancestor.exists() and ancestor != ancestor.parent:
        ancestor = ancestor.parent
    process = subprocess.run(['git', '-C', str(ancestor), 'check-ignore', '--quiet', '--no-index', '--', str(resolved)], capture_output=True)
    if process.returncode != 0:
        raise ValueError('per-site output must be inside a Git-ignored private destination')
    return resolved


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline', type=Path, required=True)
    parser.add_argument('--cache', type=Path, required=True)
    parser.add_argument('--elf', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--objdump', help='existing llvm-objdump path; otherwise resolved with xcrun')
    parser.add_argument('--preserve-register', action='append', choices=REGISTERS, default=[], help='explicit unverified call-preservation assumption (repeatable)')
    options = parser.parse_args(argv)
    try:
        destination = private_output_path(options.out)
        inputs = (options.baseline.resolve(), options.cache.resolve(), options.elf.resolve())
        if destination in inputs or any(destination.exists() and destination.samefile(source) for source in inputs):
            raise ValueError('output must not overwrite any input')
        previous, capture, raw = load_pinned(*inputs)
        objdump = options.objdump or subprocess.check_output(['xcrun', '--find', 'llvm-objdump'], text=True).strip()
        version = subprocess.check_output([objdump, '--version'], text=True).strip().splitlines()
        instructions, commands = decode_and_reconcile(previous, capture, raw, objdump, str(inputs[2]))
        strict = analyze(previous, capture, instructions)
        assumed = analyze(previous, capture, instructions, frozenset(options.preserve_register)) if options.preserve_register else None
        report = {'status': 'STATIC_CONDITIONAL_OBSERVATIONS_ONLY',
                  'inputs': {'baseline_sha256': EXPECTED_BASELINE, 'cache_sha256': EXPECTED_CACHE, 'elf_sha256': EXPECTED_ELF},
                  'cache_evidence_id': capture['data']['evidence_id'], 'provider': capture['data']['provider'],
                  'rea_version': '6.7.0 (capture recipe; not independently queried)',
                  'elf_executable_segments': [{'virtual_address': hex(a), 'file_offset': o, 'file_size': n} for a, o, n in elf_segments(raw)],
                  'independent_decode': {'version': version, 'commands': commands, 'all_instruction_bytes_checked': True,
                                         'cache_instructions_reconciled': len(capture['data']['raw_result']['value']['assembly']),
                                         'baseline_sites_byte_checked': len(previous['callsites']), 'decoded_instruction_count': len(instructions)},
                  'strict': strict, 'with_explicit_preservation_assumption': assumed,
                  'limitations': value_limitations(capture)}
        destination.parent.mkdir(parents=True, exist_ok=True)
        destination.write_text(json.dumps(report, indent=2, sort_keys=True) + '\n')
        print(json.dumps({'status': report['status'], 'strict': strict['aggregate'], 'assumed': assumed['aggregate'] if assumed else None, 'output_sha256': digest(destination.read_bytes())}, sort_keys=True))
        return 0
    except (OSError, ValueError, KeyError, TypeError, subprocess.SubprocessError) as error:
        print(f'FAIL: {error}', file=sys.stderr)
        return 2


def value_limitations(capture):
    return list(capture['data']['raw_result']['value']['limitations']) + [
        'Cached successor lists are observations, not proof of complete control flow or feasible paths.',
        'All paths are merged; branch conditions and memory contents are not evaluated.',
        'LEA expressions identify definition sites and symbolic operands, not concrete pointer values or allocation/ABI structure.',
        'Memory loads, unsupported writes and partial-register writes yield explicit unknowns; memory stores are not tracked.',
        'Calls clobber every GPR by default; any preserved registers are explicit unverified assumptions, including rsp.',
        'The baseline-only site has no cached CFG and remains unknown in this continuation.',
        'No codec, ABI, reachability, playback, native boot or runtime completion is established.']


if __name__ == '__main__':
    raise SystemExit(main())
