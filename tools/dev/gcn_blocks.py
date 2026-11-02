#!/usr/bin/env python3
"""Basic-block decoding for GFX7 spans that contain control-transfer rows.

`gcn_inventory.decode()` is deliberately linear: it refuses `s_branch`/`s_cbranch_*` because a
linear listing would have to invent fall-through past a taken branch. That refusal blocks 122 of
the 184 remaining decode errors in the sampled corpus, and none of them can be unblocked by another
opcode-table row.

The splitting lives in `gcn_inventory.decode_blocks` (the CFG-aware decode mode); this module is the
census-facing CLI and adapter around it. It decodes the span with `allow_branches=True` (branch rows
become explicit instructions), then splits the listing at every control-transfer row. Each branch is reported with its byte target
(`offset + 4 + simm16*4`, the GCN PC-relative rule). Nothing is followed: the tool never claims a
block is reachable, never relocates code, and never executes anything.

What this establishes: the span is a sequence of decodable blocks whose branch rows and targets are
known. What it does NOT establish: a control-flow graph (no reachability, dominance, or merge
analysis), a stack/ABI contract, or Metal semantics.

  python3 -I tools/dev/gcn_blocks.py --self-test
  python3 tools/dev/gcn_blocks.py --blocks program.bin
"""
from __future__ import annotations

import argparse
from dataclasses import asdict, dataclass
import hashlib
import importlib.util
import json
from pathlib import Path
import struct
import sys

_SPEC = importlib.util.spec_from_file_location('gcn_inventory', Path(__file__).with_name('gcn_inventory.py'))
gcn = importlib.util.module_from_spec(_SPEC)
sys.modules['gcn_inventory'] = gcn
_SPEC.loader.exec_module(gcn)

VERSION = 'gfx7-blocks-v2'
CONTROL_TRANSFER = frozenset(('control_transfer',))


@dataclass(frozen=True)
class Branch:
    offset: int
    mnemonic: str
    immediate: int      # signed dwords, exactly as encoded
    target: int         # byte offset of the branch destination
    target_in_span: bool
    target_on_instruction: bool


@dataclass(frozen=True)
class Block:
    start: int
    end: int            # exclusive
    instructions: int
    terminator: str | None


@dataclass(frozen=True)
class BlockProgram:
    blocks: tuple[Block, ...]
    branches: tuple[Branch, ...]
    ok: bool
    error_offset: int | None
    error_reason: str | None
    bytes: int
    instructions: int


def _signed16(value: int) -> int:
    return value - 0x10000 if value & 0x8000 else value


def blocks(code: bytes) -> BlockProgram:
    """Decode `code` block-by-block.

    The splitting and target arithmetic live in `gcn_inventory.decode_blocks`; this function only
    adapts its result to the tool's own dataclasses and turns a `DecodeError` into a status record.
    """
    try:
        program = gcn.decode_blocks(code)
    except gcn.DecodeError as exc:
        return BlockProgram(blocks=(), branches=(), ok=False,
                            error_offset=exc.offset, error_reason=exc.reason,
                            bytes=len(code), instructions=0)
    blocks_out = tuple(
        Block(start=block.start, end=block.end,
              instructions=len(block.instruction_offsets), terminator=block.terminator)
        for block in program.blocks
    )
    branches_out = tuple(
        Branch(offset=branch.offset, mnemonic=branch.mnemonic, immediate=branch.immediate,
               target=branch.target, target_in_span=branch.target_in_program,
               target_on_instruction=branch.target_on_instruction)
        for branch in program.branches
    )
    return BlockProgram(blocks=blocks_out, branches=branches_out, ok=True,
                        error_offset=None, error_reason=None,
                        bytes=len(code), instructions=len(program.instructions))


def summary(program: BlockProgram) -> dict:
    """Counters only; no instruction text, so this is safe to publish."""
    return {
        'schema': VERSION,
        'sha256': None,
        'bytes': program.bytes,
        'ok': program.ok,
        'error_offset': program.error_offset,
        'error_reason': program.error_reason,
        'instructions': program.instructions,
        'blocks': len(program.blocks),
        'branches': len(program.branches),
        'branches_in_span': sum(1 for b in program.branches if b.target_in_span),
        'branches_on_instruction': sum(1 for b in program.branches if b.target_on_instruction),
        'branch_mnemonics': sorted({b.mnemonic for b in program.branches}),
        'max_block_instructions': max((b.instructions for b in program.blocks), default=0),
    }


def _self_test() -> int:
    checks = 0

    def check(name: str, condition: bool) -> None:
        nonlocal checks
        if not condition:
            raise AssertionError(f'self-test failed: {name}')
        checks += 1

    def words(*fields) -> bytes:
        text = ''.join(format(value, f'0{width}b') for width, value in fields)
        assert len(text) == 32
        return int(text, 2).to_bytes(4, 'little')

    def sop(op: int, imm: int = 0) -> bytes:
        return words((9, 0b101111111), (7, op), (16, imm & 0xffff))

    def vop2(op: int = 3, dst: int = 9, src1: int = 5, src0: int = 257) -> bytes:
        return words((1, 0), (6, op), (8, dst), (8, src1), (9, src0))

    def sop2_literal(op: int, literal: int) -> bytes:
        return words((2, 0b10), (7, op), (7, 6), (8, 10), (8, 255)) + literal.to_bytes(4, 'little')

    end = sop(1)

    # Straight line: one block ending at s_endpgm, no branches.
    straight = blocks(vop2() + vop2(4) + end)
    check('straight ok', straight.ok)
    check('straight one block', len(straight.blocks) == 1)
    check('straight terminator', straight.blocks[0].terminator == 's_endpgm')
    check('straight no branch', straight.branches == ())

    # One forward branch over two instructions: two blocks, target lands on an instruction.
    forward = blocks(sop(5, 2) + vop2() + vop2(4) + end)
    check('forward ok', forward.ok)
    check('forward two blocks', len(forward.blocks) == 2)
    check('forward terminator', forward.blocks[0].terminator == 's_cbranch_scc1')
    check('forward branch mnemonic', forward.branches[0].mnemonic == 's_cbranch_scc1')
    check('forward target', forward.branches[0].target == 0x0C)
    check('forward target in span', forward.branches[0].target_in_span)
    check('forward target on instruction', forward.branches[0].target_on_instruction)

    # Backward branch: negative immediate, target 0.
    backward = blocks(sop(8, -1) + vop2() + end)
    check('backward immediate', backward.branches[0].immediate == -1)
    check('backward target zero', backward.branches[0].target == 0)
    check('backward target on instruction', backward.branches[0].target_on_instruction)

    # Target past the end is reported, never followed.
    outside = blocks(end[:0] + sop(5, 64) + end)
    check('outside ok', outside.ok)
    check('outside target not in span', not outside.branches[0].target_in_span)
    check('outside one branch', len(outside.branches) == 1)

    # Strict linear decoding still refuses the same span: branch support is opt-in and local.
    try:
        gcn.decode(sop(5, 2) + end)
    except gcn.DecodeError as exc:
        check('strict still refuses branch', 'control transfer' in exc.reason)
    else:
        raise AssertionError('self-test failed: strict decode accepted a branch')

    # An unsupported opcode fails the whole span even in block mode.
    broken = blocks(sop(5, 1) + (0xbf900000).to_bytes(4, 'little') + end)
    check('broken not ok', not broken.ok)
    check('broken offset', broken.error_offset == 4)
    check('broken reason', 'SOOP' not in (broken.error_reason or ''))

    # A program without a final s_endpgm still fails.
    check('no endpgm fails', not blocks(vop2()).ok)

    # Literal-carrying instruction before a branch: sizes are honoured, so the branch is found.
    literal_program = blocks(sop2_literal(0, 0xdeadbeef) + sop(5, 1) + end)
    check('literal ok', literal_program.ok)
    check('literal branch found', len(literal_program.branches) == 1)
    check('literal branch offset', literal_program.branches[0].offset == 8)
    check('literal instruction count', literal_program.instructions == 3)

    print(f'gcn blocks self-test: {checks} checks passed (synthetic spans only)')
    return 0


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('--self-test', action='store_true')
    parser.add_argument('--blocks', type=Path, help='exact little-endian program span')
    parser.add_argument('--json', action='store_true', help='full block/branch listing')
    args = parser.parse_args(argv)

    if args.self_test:
        return _self_test()
    if args.blocks is None:
        parser.error('pass --self-test or --blocks <span>')
    code = args.blocks.read_bytes()
    program = blocks(code)
    document = summary(program)
    document['sha256'] = hashlib.sha256(code).hexdigest()
    if args.json:
        document['block_listing'] = [asdict(b) for b in program.blocks]
        document['branch_listing'] = [asdict(b) for b in program.branches]
    print(json.dumps(document, indent=2, sort_keys=True))
    return 0 if program.ok else 1


if __name__ == '__main__':
    raise SystemExit(main())
