#!/usr/bin/env python3
"""Bounded store-to-load forwarding pass for the indirect-call frontier (CPU-01-AOT-CFG-SCALE-01).

`native/core/vtable_resolved_entries.hpp` records vtable-slot call sites in two evidence grades.
A **site-provable** row's call register is defined by the RIP-relative load of the table itself, so
the pinned image forces the target for that call. A **table-level** row has a sound table tie (the
slot qword, one type-8 dynamic relocation with the same addend, an FDE start) but the call register
is loaded from memory, so the site is not proven.

This pass tries exactly one bounded step past that wall. When the call register is reloaded from a
memory location that the *same function* stored earlier, the stored value is forwarded and the pass
asks whether the call then reads *the row's own slot address*.

Bounds, all enforced, no recursion and no interprocedural step:

* **one forwarding hop** - call register <- one load <- one earlier store to the same location.
  A store whose value itself came from memory is rejected, never followed.
* the store and the load are in the **same FDE**, and the store is found within `MAX_STORE_SCAN`
  instructions.
* the store must be a **single reaching definition** inside one straight-line region: between the
  store and the load there is no control flow, no call, no write to the location's base register,
  no store that could alias (only a different constant offset of the *same* base is tolerated), and
  no branch target may enter the region from outside.
* the stored value must be **static** within `MAX_VALUE_LINKS` register links: `lea TABLE(%rip)`
  (the register holds the table address), `mov G(%rip)` (the register holds the pointer stored at
  `G`), or `mov %reg2,%reg` / `lea K(%reg2),%reg` / `add $K,%reg` feeding one of those. The
  `lea TABLE(%rip); add $0x10` shape is the constructor vptr store seen across this image.
* the forwarded address must be the row's slot address: `stored_value + slot == row.vtable +
  row.slot`. Even then, the pinned image must corroborate it: a readable qword equal to the row's
  target, exactly one type-8 dynamic relocation there with that addend, and an FDE start at it.

Nothing is executed and no target is guessed. A row is reclassified only when every bound holds;
an unresolved or contradictory chain stays `table-level`, and the diagnostic forward it would have
produced is recorded as `unproven_read_would_be` - explicitly not evidence.

Usage:
  python3 tools/lift/store_to_load_forwarding.py                  # summary + per-row verdicts
  python3 tools/lift/store_to_load_forwarding.py --json OUT       # full rows (private: local/)
  python3 tools/lift/store_to_load_forwarding.py --self-test      # synthetic rule tests, no inputs
"""

from __future__ import annotations

import argparse
import hashlib
import importlib.util
import json
import re
import struct
import sys
from collections import Counter, defaultdict
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HEADER = ROOT / "native/core/vtable_resolved_entries.hpp"
DEFAULT_DIS = ROOT / "local/validation/registry-analysis-agent/all.dis"
DEFAULT_FDE = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"
DEFAULT_ELF = ROOT / "local/re/eboot.elf"
DEFAULT_JSON = ROOT / "local/validation/indirect-frontier/store-to-load-forwarding.json"

MAX_STORE_SCAN = 512     # instructions scanned backwards for the reaching store
MAX_VALUE_SCAN = 64      # instructions scanned backwards for one register link
MAX_VALUE_LINKS = 3      # register links allowed before the static table address

SITE_PROVABLE = "site-provable"
TABLE_LEVEL = "table-level"
GRADES = (SITE_PROVABLE, TABLE_LEVEL)

# --------------------------------------------------------------------------------------------------
# Row parsing
# --------------------------------------------------------------------------------------------------

ROW_RE = re.compile(
    r"\{0x([0-9a-f]+), kResolvedVtable_([0-9a-f]+), (0x[0-9a-f]+), "
    r"kResolvedVtableTarget_([0-9a-f]+)Rva\},?\s*//\s*(\S+)")


def parse_header_rows(text: str):
    """[{site, vtable, slot, target, grade}] in file order; an unknown grade token is fatal."""
    rows = []
    for match in ROW_RE.finditer(text):
        grade = match.group(5)
        if grade not in GRADES:
            raise SystemExit(f"unknown evidence grade {grade!r} for site {match.group(1)}")
        rows.append({
            "site": int(match.group(1), 16),
            "vtable": int(match.group(2), 16),
            "slot": int(match.group(3), 16),
            "target": int(match.group(4), 16),
            "grade": grade,
        })
    return rows


# --------------------------------------------------------------------------------------------------
# Instruction model (AT&T text as objdump prints it)
# --------------------------------------------------------------------------------------------------

_LINE = re.compile(r"^\s*([0-9a-f]+):\s+(.*)$")
_INSN = re.compile(r"^([0-9a-z.]+)\s*(.*)$")
_COMMENT = re.compile(r"#\s*(0x[0-9a-f]+)")
_MEM = re.compile(r"(-?0x[0-9a-f]+)?\((%r[a-z0-9]+)(?:,\s*(%r[a-z0-9]+)\s*,\s*([1248]))?\)")
_RIP = re.compile(r"(-?0x[0-9a-f]+)?\(%rip\)")

_FAMILY: dict[str, str] = {}
for _base, _aliases in {
    "rax": "rax eax ax al ah",
    "rbx": "rbx ebx bx bl bh",
    "rcx": "rcx ecx cx cl ch",
    "rdx": "rdx edx dx dl dh",
    "rsi": "rsi esi si sil",
    "rdi": "rdi edi di dil",
    "rbp": "rbp ebp bp bpl",
    "rsp": "rsp esp sp spl",
    "rip": "rip eip ip",
}.items():
    for _alias in _aliases.split():
        _FAMILY[_alias] = _base
for _i in range(8, 16):
    for _suffix in ("", "d", "w", "b"):
        _FAMILY[f"r{_i}{_suffix}"] = f"r{_i}"

# Mnemonics whose trailing memory operand is read, never written.
_READ_ONLY = ("cmp", "test", "ucomis", "comis", "bt", "prefetch", "nop", "call", "j", "loop",
              "lea", "push", "ret")
# Mnemonics with more than one implicit destination.
_IMPLICIT_MULTI = {"mul": ("rax", "rdx"), "div": ("rax", "rdx"), "idiv": ("rax", "rdx"),
                   "push": ("rsp",), "leave": ("rsp", "rbp"), "enter": ("rsp", "rbp")}
# Mnemonics with exactly one implicit destination.
_IMPLICIT_SINGLE = {"cdq": "rdx", "cqo": "rdx", "cwd": "rdx", "cdqe": "rax", "cltq": "rax",
                    "cbw": "rax", "cwde": "rax"}

_STEMS = ("movabs", "movs", "movz", "mov", "lea", "add", "sub", "and", "or", "xor", "adc", "sbb",
          "sal", "shl", "sar", "shr", "rol", "ror", "imul", "mul", "div", "idiv", "neg", "not",
          "inc", "dec", "bswap", "pop", "push", "call", "test", "cmp", "ucomis", "comis", "bt",
          "prefetch", "nop", "ret", "xadd", "cmpxchg", "xchg", "leave", "enter", "stos", "movs")

PREFIXES = ("lock", "rep", "repz", "repnz", "data16", "bnd", "notrack")


@dataclass(frozen=True)
class Insn:
    address: int
    mnemonic: str
    operands: str

    @property
    def text(self) -> str:
        return f"{self.mnemonic} {self.operands}".strip()


def mnemonic_base(mnemonic: str) -> str:
    """`movq` -> `mov`, `jne` -> `jne` (branches keep their own name)."""
    if mnemonic.startswith("j"):
        return mnemonic
    for stem in _STEMS:
        if mnemonic.startswith(stem):
            return stem
    return mnemonic


def is_mov_form(mnemonic: str) -> bool:
    return mnemonic_base(mnemonic) in ("mov", "movs", "movz", "movabs")


def family(register: str | None) -> str | None:
    if register is None:
        return None
    return _FAMILY.get(register.lstrip("%"))


def strip_comment(text: str) -> str:
    return text.split("#", 1)[0].strip()


def operands_of(insn: Insn) -> list[str]:
    return [part.strip() for part in strip_comment(insn.operands).split(",") if part.strip()]


def comment_address(insn: Insn) -> int | None:
    match = _COMMENT.search(insn.operands)
    return int(match.group(1), 16) if match else None


def memory_operand(text: str):
    """(offset, base_register, has_index) for `OFF(reg)` / `OFF(reg,index,scale)`, else None."""
    match = _MEM.fullmatch(text.strip())
    if match is None:
        return None
    offset = int(match.group(1), 16) if match.group(1) else 0
    return (offset, match.group(2), match.group(3) is not None)


def rip_operand(text: str) -> bool:
    return _RIP.fullmatch(text.strip()) is not None


def same_location(left, right) -> bool:
    """Two `(offset, base, has_index)` triples name the same address."""
    if left is None or right is None:
        return False
    if left[0] != right[0] or left[2] or right[2]:
        return False
    return family(left[1]) is not None and family(left[1]) == family(right[1])


def written_families(insn: Insn) -> frozenset[str]:
    """Register families this instruction writes. Over-approximates; a call writes %rax."""
    mnemonic = insn.mnemonic
    if mnemonic in PREFIXES:
        return frozenset()
    base = mnemonic_base(mnemonic)
    if base == "call":
        return frozenset({"rax"})
    if base in ("test", "cmp", "ucomis", "comis", "bt", "prefetch", "nop", "ret"):
        return frozenset()
    if base.startswith("j") or base == "loop":
        return frozenset()
    if base in _IMPLICIT_SINGLE:
        return frozenset({_IMPLICIT_SINGLE[base]})
    if base in _IMPLICIT_MULTI:
        written = set(_IMPLICIT_MULTI[base])
        parts = operands_of(insn)
        if base in ("mul", "imul") and len(parts) > 1:
            dest = family(parts[-1])
            if dest is not None:
                written.add(dest)
        return frozenset(written)
    if mnemonic.startswith("v") or mnemonic.startswith(("ps", "pxor", "punpck", "shuf")):
        return frozenset()          # SSE/AVX never writes a GPR in this encoding set
    parts = operands_of(insn)
    if not parts:
        return frozenset()
    if base in ("pop", "xadd", "cmpxchg", "xchg"):
        dest = family(parts[0])
        return frozenset({dest}) if dest else frozenset()
    dest = family(parts[-1])
    return frozenset({dest}) if dest else frozenset()


def memory_destination(insn: Insn):
    """`(offset, base, has_index)` when this instruction stores to memory, else None."""
    if insn.mnemonic in PREFIXES:
        return None
    base = mnemonic_base(insn.mnemonic)
    if base.startswith(_READ_ONLY) or base in ("stos", "movs"):
        return None
    parts = operands_of(insn)
    if not parts:
        return None
    if len(parts) == 1 and base not in ("inc", "dec", "neg", "not", "shl", "shr", "sar", "rol",
                                        "ror", "sal", "xadd", "cmpxchg", "xchg"):
        return None
    return memory_operand(parts[-1])


def is_call(insn: Insn) -> bool:
    return mnemonic_base(insn.mnemonic) == "call"


def is_control_flow(insn: Insn) -> bool:
    base = mnemonic_base(insn.mnemonic)
    return base.startswith("j") or base in ("loop", "ret", "syscall", "int", "hlt", "ud2")


def branch_target(insn: Insn) -> int | None:
    """Direct branch/jump target from the leading operand, else None."""
    base = mnemonic_base(insn.mnemonic)
    if not (base.startswith("j") or base == "loop"):
        return None
    parts = operands_of(insn)
    if not parts:
        return None
    head = parts[0].split()[0]
    if head.startswith("*"):
        return None
    try:
        return int(head, 16)
    except ValueError:
        return None


def parse_disassembly(path: Path, fdes) -> dict[int, list[Insn]]:
    """{FDE start: [Insn]} for every instruction that belongs to an FDE range."""
    bodies: dict[int, list[Insn]] = defaultdict(list)
    with open(path, "r", encoding="utf-8", errors="replace") as handle:
        for line in handle:
            parsed = _LINE.match(line)
            if parsed is None:
                continue
            address = int(parsed.group(1), 16)
            owner = fdes.find(address)
            if owner is None:
                continue
            insn = _INSN.match(parsed.group(2).strip())
            if insn is None:
                continue
            bodies[owner[0]].append(Insn(address, insn.group(1), insn.group(2).strip()))
    return bodies


# --------------------------------------------------------------------------------------------------
# The pass
# --------------------------------------------------------------------------------------------------

class Chain:
    """Diagnostics for one row: what was followed, where it stopped, and why."""

    def __init__(self, site: int, grade: str):
        self.site = site
        self.grade = grade
        self.blocker: str | None = None
        self.blocker_address: int | None = None
        self.load_address: int | None = None
        self.store_address: int | None = None
        self.location: tuple[int, str, bool] | None = None
        self.value: int | None = None
        self.value_form: str | None = None
        self.value_chain: list[str] = []
        self.would_read: int | None = None
        self.read_target: int | None = None
        self.row_slot_address: int | None = None

    @property
    def proven(self) -> bool:
        return self.blocker is None and self.value is not None

    def block(self, blocker: str, address: int | None = None) -> "Chain":
        if self.blocker is None:
            self.blocker = blocker
            self.blocker_address = address
        return self

    def as_dict(self) -> dict:
        def address(value):
            return None if value is None else f"0x{value:x}"

        return {
            "site": address(self.site),
            "grade": self.grade,
            "verdict": "proven" if self.proven else "unproven",
            "blocker": self.blocker,
            "blocker_address": address(self.blocker_address),
            "load_address": address(self.load_address),
            "store_address": address(self.store_address),
            "location": None if self.location is None else {
                "offset": f"0x{self.location[0]:x}",
                "base": self.location[1],
                "indexed": self.location[2],
            },
            "stored_value": address(self.value),
            "stored_value_form": self.value_form,
            "stored_value_chain": self.value_chain,
            "row_slot_address": address(self.row_slot_address),
            # Diagnostic only, never evidence: what the refused forward would have read.
            "unproven_read_would_be": address(self.would_read),
            "unproven_read_target": address(self.read_target),
        }


def nearest_definition(body: list[Insn], start: int, register: str, limit: int) -> int | None:
    want = family(register)
    if want is None:
        return None
    stop = max(start - limit, -1)
    for index in range(start - 1, stop, -1):
        if want in written_families(body[index]):
            return index
    return None


def stored_value(body: list[Insn], store_index: int, read_u64, chain: Chain):
    """(value, form) when the stored register is static, else (None, None) with a blocker."""
    parts = operands_of(body[store_index])
    if not parts or family(parts[0]) is None:
        chain.block("store-source-not-register", body[store_index].address)
        return None, None
    register = parts[0]
    adjust = 0
    links = 0
    cursor = store_index
    while True:
        index = nearest_definition(body, cursor, register, MAX_VALUE_SCAN)
        if index is None:
            chain.block("stored-value-no-definition", body[cursor].address)
            return None, None
        insn = body[index]
        parts = operands_of(insn)
        if len(parts) != 2:
            chain.block("stored-value-unclassified", insn.address)
            return None, None
        base = mnemonic_base(insn.mnemonic)
        if rip_operand(parts[0]) and (base == "lea" or is_mov_form(insn.mnemonic)):
            address = comment_address(insn)
            if address is None:
                chain.block("stored-value-no-rip-target", insn.address)
                return None, None
            if base == "lea":
                chain.value_form = "address"
                chain.value_chain.append(f"lea 0x{address:x} at 0x{insn.address:x}")
            else:
                value = read_u64(address)
                if not value:
                    chain.block("stored-value-global-empty", insn.address)
                    return None, None
                chain.value_form = "value"
                chain.value_chain.append(f"mov [0x{address:x}] at 0x{insn.address:x}")
            if adjust:
                chain.value_chain.append(f"+0x{adjust:x}")
            return address + adjust if base == "lea" else value + adjust, chain.value_form
        if base == "lea":
            inner = memory_operand(parts[0])
            if inner is None or inner[2]:
                chain.block("stored-value-unclassified", insn.address)
                return None, None
            adjust += inner[0]
            register = inner[1]
        elif base == "add":
            if not parts[0].startswith("$"):
                chain.block("stored-value-add-not-immediate", insn.address)
                return None, None
            try:
                adjust += int(parts[0][1:], 16)
            except ValueError:
                chain.block("stored-value-add-not-immediate", insn.address)
                return None, None
        elif is_mov_form(insn.mnemonic) and family(parts[0]) is not None:
            register = parts[0]
        elif memory_operand(parts[0]) is not None:
            # The stored pointer came from memory: a second forwarding hop, which this pass refuses.
            chain.block("stored-value-is-memory-hop", insn.address)
            return None, None
        else:
            chain.block("stored-value-not-static", insn.address)
            return None, None
        links += 1
        if links > MAX_VALUE_LINKS:
            chain.block("stored-value-link-limit", insn.address)
            return None, None
        cursor = index


class Associated:
    """The image/table facts a forwarded slot must satisfy (injectable for the self-test)."""

    def __init__(self, reloc_at, fde_owner, fde_contains=None):
        self._reloc_at = reloc_at
        self._fde_owner = fde_owner
        self._fde_contains = fde_contains or fde_owner

    def relocations(self, address: int):
        return [row for row in self._reloc_at(address) if row.get("type") == 8]

    def fde_owner(self, address):
        """(start, length) when `address` is exactly an FDE start, else None."""
        return self._fde_owner(address)

    def fde_contains(self, address):
        """(start, length) of the range containing `address`, else None."""
        return self._fde_contains(address)


def analyze_site(row: dict, body: list[Insn], associated: Associated, read_u64) -> Chain:
    """One row -> Chain. Every bound in the module docstring is checked here."""
    chain = Chain(row["site"], row["grade"])
    chain.row_slot_address = row["vtable"] + row["slot"]
    index = next((i for i, insn in enumerate(body) if insn.address == row["site"]), None)
    if index is None:
        return chain.block("site-not-in-body")

    call = body[index]
    match = re.fullmatch(r"\*0x([0-9a-f]+)\((%r[a-z0-9]+)\)", strip_comment(call.operands))
    if match is None:
        return chain.block("not-a-slot-call")
    slot = int(match.group(1), 16)

    definition = nearest_definition(body, index, match.group(2), MAX_STORE_SCAN)
    if definition is None:
        return chain.block("call-register-has-no-definition")
    insn = body[definition]
    chain.load_address = insn.address
    parts = operands_of(insn)
    if len(parts) != 2:
        return chain.block("call-register-definition-not-a-load")
    if (mnemonic_base(insn.mnemonic) == "lea" or is_mov_form(insn.mnemonic)) and \
            rip_operand(parts[0]):
        # The register is already defined by a static table load: the strict predicate's job.
        return chain.block("already-site-provable")
    if not is_mov_form(insn.mnemonic):
        return chain.block("call-register-definition-not-a-load")
    location = memory_operand(parts[0])
    if location is None:
        return chain.block("call-register-definition-not-a-load")
    if location[2]:
        return chain.block("load-address-is-indexed")
    chain.location = location
    base_family = family(location[1])

    # 1. Nearest earlier store to the same location. A repointed base means the location itself
    #    changed, so an earlier store under the old value cannot reach this load.
    store_index = None
    stop_address = None
    stop = max(definition - MAX_STORE_SCAN, -1)
    for probe in range(definition - 1, stop, -1):
        if base_family in written_families(body[probe]):
            stop_address = body[probe].address
            break
        if same_location(memory_destination(body[probe]), location):
            store_index = probe
            break
    if store_index is None:
        return chain.block("no-store-to-location-in-fde", stop_address)
    chain.store_address = body[store_index].address

    # 2. Forward the stored value first, so a refused chain still records what it would have read.
    probe_chain = Chain(row["site"], row["grade"])
    value, form = stored_value(body, store_index, read_u64, probe_chain)
    chain.value_chain = probe_chain.value_chain
    chain.value_form = probe_chain.value_form
    if value is not None:
        chain.value = value
        chain.would_read = value + slot
        chain.read_target = read_u64(chain.would_read)

    # 3. The store must be a single reaching definition inside one straight-line region. A branch
    #    that targets an instruction between the store and the load only breaks the region when it
    #    starts outside it; an internal loop still executes the store first.
    entering = set()
    for branch in body:
        target = branch_target(branch)
        if target is None or not (chain.store_address < target <= insn.address):
            continue
        if not (chain.store_address < branch.address <= insn.address):
            entering.add(target)
    for probe in range(store_index + 1, definition + 1):
        step = body[probe]
        if is_call(step):
            return chain.block("call-between-store-and-load", step.address)
        if is_control_flow(step):
            return chain.block("control-flow-between-store-and-load", step.address)
        if base_family in written_families(step):
            return chain.block("base-rewritten-between-store-and-load", step.address)
        if step.address in entering:
            return chain.block("alternate-entry-into-region", step.address)
        destination = memory_destination(step)
        if destination is not None and not (
                not destination[2] and family(destination[1]) == base_family
                and destination[0] != location[0]):
            return chain.block("aliasing-store-between-store-and-load", step.address)

    # 4. The stored value must be static.
    if value is None:
        chain.blocker = probe_chain.blocker
        chain.blocker_address = probe_chain.blocker_address
        return chain

    # 5. The forwarded address must be the row's slot, corroborated by the pinned image.
    forwarded = value + slot
    if forwarded != chain.row_slot_address:
        return chain.block("forwarded-slot-is-not-the-row-slot")
    if chain.read_target != row["target"]:
        return chain.block("forwarded-target-is-not-the-row-target")
    if len(associated.relocations(forwarded)) != 1:
        return chain.block("no-single-type-8-relocation")
    if associated.fde_owner(chain.read_target) is None:
        return chain.block("target-is-not-an-fde-start")
    return chain


def analyze(rows, bodies, read_u64, associated: Associated) -> dict:
    chains = []
    for row in rows:
        owner = associated.fde_contains(row["site"])
        body = bodies.get(owner[0] if owner else None, [])
        chains.append(analyze_site(row, body, associated, read_u64))
    counts = Counter()
    by_grade = Counter()
    for chain in chains:
        counts["proven" if chain.proven else "unproven"] += 1
        by_grade[(chain.grade, "proven" if chain.proven else "unproven")] += 1
    return {"chains": chains, "counts": counts, "by_grade": by_grade}


# --------------------------------------------------------------------------------------------------
# Self-test: synthetic instructions and a synthetic image, no private inputs
# --------------------------------------------------------------------------------------------------

class FakeImage:
    """Little-endian qword cell map; a missing cell is unreadable like an unmapped page."""

    def __init__(self, cells: dict[int, int]):
        self.cells = cells

    def read_u64(self, address: int):
        return self.cells.get(address)


class FakeRanges:
    def __init__(self, starts):
        self.starts = sorted(starts)

    def find(self, address: int):
        owner = None
        for start in self.starts:
            if start <= address:
                owner = start
            else:
                break
        return None if owner is None else (owner, 0x100000)


TABLE = 0x1596000
VPTR = TABLE + 0x10
SLOT = 0x18
TARGET = 0x30000
FUNCTION = 0x1000


def _insn(address: int, text: str) -> Insn:
    mnemonic, _, operands = text.partition(" ")
    return Insn(address, mnemonic, operands.strip())


def _self_test() -> int:
    failures = []

    def check(name, source, row, cells, expect_verdict, expect_blocker=None, expect_value=None,
              relocs=None):
        body = [_insn(address, text) for address, text in source]
        image = FakeImage(cells)
        ranges = FakeRanges([FUNCTION, TARGET])

        def reloc_at(address):
            if relocs is not None:
                return relocs.get(address, [])
            return [{"type": 8, "addend": cells[address]}] if address in cells else []

        def fde_owner(address):
            found = ranges.find(address) if address else None
            return found if found and found[0] == address else None

        chain = analyze_site(row, body, Associated(reloc_at, fde_owner, ranges.find),
                             image.read_u64)
        verdict = "proven" if chain.proven else "unproven"
        problems = []
        if verdict != expect_verdict:
            problems.append(f"verdict {verdict} != {expect_verdict}")
        if expect_blocker is not None and chain.blocker != expect_blocker:
            problems.append(f"blocker {chain.blocker} != {expect_blocker}")
        if expect_value is not None and chain.value != expect_value:
            problems.append(f"value {chain.value and hex(chain.value)} != {hex(expect_value)}")
        print(f"{'FAIL' if problems else 'ok  '} {name:38s} {verdict:8s} {chain.blocker or ''}"
              + ("  <- " + "; ".join(problems) if problems else ""))
        if problems:
            failures.append(name)

    def row_for(site, table, slot, target, grade=TABLE_LEVEL):
        return {"site": site, "vtable": table, "slot": slot, "target": target, "grade": grade}

    # The base-relative slot and the vptr-relative slot are different addresses. Both are populated
    # so a test can tell the two readings apart.
    cells = {TABLE + SLOT: TARGET, VPTR + SLOT: TARGET}

    direct = [
        (0x1000, "leaq 0x1596000(%rip), %rax   # 0x1596000"),
        (0x1010, "movq %rax, (%r12)"),
        (0x1018, "movq (%r12), %rax"),
        (0x1020, "callq *0x18(%rax)"),
    ]
    check("positive: lea table, store, reload", direct,
          row_for(0x1020, TABLE, SLOT, TARGET), cells, "proven", expect_value=TABLE)

    ctor = [
        (0x1000, "leaq 0x1596000(%rip), %rax   # 0x1596000"),
        (0x1008, "movq %rdi, %rbx"),
        (0x1010, "addq $0x10, %rax"),
        (0x1014, "movq %rax, (%rbx)"),
        (0x1018, "movq (%rbx), %rax"),
        (0x1020, "movq %rbx, %rdi"),
        (0x1028, "callq *0x18(%rax)"),
    ]
    check("positive: lea base; add 0x10 -> vptr", ctor,
          row_for(0x1028, VPTR, SLOT, TARGET), cells, "proven", expect_value=VPTR)
    check("negative: base-relative slot is not the row", ctor,
          row_for(0x1028, TABLE, SLOT, TARGET), cells, "unproven",
          expect_blocker="forwarded-slot-is-not-the-row-slot")

    via_global = [
        (0x1000, "movq 0x2f0000(%rip), %rax   # 0x2f0000"),
        (0x1010, "movq %rax, (%r12)"),
        (0x1018, "movq (%r12), %rax"),
        (0x1020, "callq *0x18(%rax)"),
    ]
    check("positive: mov G(%rip) table pointer", via_global,
          row_for(0x1020, TABLE, SLOT, TARGET), {0x2f0000: TABLE, TABLE + SLOT: TARGET},
          "proven", expect_value=TABLE)

    with_call = [
        (0x1000, "leaq 0x1596000(%rip), %rax   # 0x1596000"),
        (0x1010, "movq %rax, (%r12)"),
        (0x1014, "callq 0x2000"),
        (0x1018, "movq (%r12), %rax"),
        (0x1020, "callq *0x18(%rax)"),
    ]
    check("negative: call between store and load", with_call,
          row_for(0x1020, TABLE, SLOT, TARGET), cells, "unproven",
          expect_blocker="call-between-store-and-load")

    repointed = [
        (0x1000, "leaq 0x1596000(%rip), %rax   # 0x1596000"),
        (0x1010, "movq %rax, (%r12)"),
        (0x1014, "movq (%rdi), %r12"),
        (0x1018, "movq (%r12), %rax"),
        (0x1020, "callq *0x18(%rax)"),
    ]
    check("negative: base repointed before store", repointed,
          row_for(0x1020, TABLE, SLOT, TARGET), cells, "unproven",
          expect_blocker="base-repointed-before-store")

    branched = [
        (0x1000, "leaq 0x1596000(%rip), %rax   # 0x1596000"),
        (0x1010, "movq %rax, (%r12)"),
        (0x1014, "je 0x1018"),
        (0x1018, "movq (%r12), %rax"),
        (0x1020, "callq *0x18(%rax)"),
    ]
    check("negative: control flow between", branched,
          row_for(0x1020, TABLE, SLOT, TARGET), cells, "unproven",
          expect_blocker="control-flow-between-store-and-load")

    alternate = [
        (0x0ff0, "je 0x1018"),
        (0x1000, "leaq 0x1596000(%rip), %rax   # 0x1596000"),
        (0x1010, "movq %rax, (%r12)"),
        (0x1018, "movq (%r12), %rax"),
        (0x1020, "callq *0x18(%rax)"),
    ]
    check("negative: alternate entry into region", alternate,
          row_for(0x1020, TABLE, SLOT, TARGET), cells, "unproven",
          expect_blocker="alternate-entry-into-region")

    alias = [
        (0x1000, "leaq 0x1596000(%rip), %rax   # 0x1596000"),
        (0x1010, "movq %rax, (%r12)"),
        (0x1014, "movq %rdi, (%rbx)"),
        (0x1018, "movq (%r12), %rax"),
        (0x1020, "callq *0x18(%rax)"),
    ]
    check("negative: aliasing store through other base", alias,
          row_for(0x1020, TABLE, SLOT, TARGET), cells, "unproven",
          expect_blocker="aliasing-store-between-store-and-load")

    neighbour = [
        (0x1000, "leaq 0x1596000(%rip), %rax   # 0x1596000"),
        (0x1010, "movq %rax, (%r12)"),
        (0x1014, "movq $0x0, 0x8(%r12)"),
        (0x1018, "movq (%r12), %rax"),
        (0x1020, "callq *0x18(%rax)"),
    ]
    check("positive: neighbour field store allowed", neighbour,
          row_for(0x1020, TABLE, SLOT, TARGET), cells, "proven", expect_value=TABLE)

    second_hop = [
        (0x1000, "movq (%rdi), %rcx"),
        (0x1010, "movq %rcx, (%r12)"),
        (0x1018, "movq (%r12), %rax"),
        (0x1020, "callq *0x18(%rax)"),
    ]
    check("negative: stored value is a memory hop", second_hop,
          row_for(0x1020, TABLE, SLOT, TARGET), cells, "unproven",
          expect_blocker="stored-value-is-memory-hop")

    already = [
        (0x1000, "leaq 0x1596000(%rip), %rax   # 0x1596000"),
        (0x1008, "callq *0x18(%rax)"),
    ]
    check("neutral: nearest definition is the table", already,
          row_for(0x1008, TABLE, SLOT, TARGET, SITE_PROVABLE), cells, "unproven",
          expect_blocker="already-site-provable")

    check("negative: forwarded slot holds no target", direct,
          row_for(0x1020, TABLE, SLOT, 0x777777), {}, "unproven",
          expect_blocker="forwarded-target-is-not-the-row-target")
    check("negative: two type-8 relocations at the slot", direct,
          row_for(0x1020, TABLE, SLOT, TARGET), cells, "unproven",
          expect_blocker="no-single-type-8-relocation",
          relocs={TABLE + SLOT: [{"type": 8, "addend": TARGET},
                                 {"type": 8, "addend": TARGET}]})
    check("negative: target is not an FDE start", direct,
          row_for(0x1020, TABLE, SLOT, 0x4444), {TABLE + SLOT: 0x4444}, "unproven",
          expect_blocker="target-is-not-an-fde-start")

    print()
    if failures:
        print(f"SELF-TEST FAILED: {len(failures)} case(s): {', '.join(failures)}")
        return 1
    print("SELF-TEST PASSED: 15/15 synthetic cases")
    return 0


# --------------------------------------------------------------------------------------------------
# CLI
# --------------------------------------------------------------------------------------------------

def load_module(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with open(path, "rb") as handle:
        for chunk in iter(lambda: handle.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--header", type=Path, default=HEADER)
    parser.add_argument("--dis", type=Path, default=DEFAULT_DIS)
    parser.add_argument("--fde", type=Path, default=DEFAULT_FDE)
    parser.add_argument("--elf", type=Path, default=DEFAULT_ELF)
    parser.add_argument("--json", type=Path, default=None)
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args(argv)

    if args.self_test:
        return _self_test()

    for path in (args.header, args.dis, args.fde, args.elf):
        if not path.is_file():
            raise SystemExit(f"missing input {path}")

    census = load_module("cfg_census", ROOT / "tools/lift/cfg_census.py")
    fdes, _duplicates = census.load_fde_ranges(args.fde)
    rows = parse_header_rows(args.header.read_text())
    bodies = parse_disassembly(args.dis, fdes)

    sys.path.insert(0, str(ROOT / "scripts"))
    import ps4_dynlink as dyn  # noqa: E402

    elf = args.elf.read_bytes()
    _segments, headers = dyn.program_headers(elf)
    image = dyn.Image(elf, headers)
    reloc_at = defaultdict(list)
    for row in dyn.parse(elf)["relocs"]:
        reloc_at[row["target"]].append(row)

    def read_u64(address: int):
        try:
            raw = image.read(address, 8)
        except Exception:
            return None
        return struct.unpack("<Q", raw)[0] if len(raw) == 8 else None

    def fde_owner(address):
        found = fdes.find(address) if address else None
        return found if found and found[0] == address else None

    associated = Associated(reloc_at.get, fde_owner, fdes.find)
    result = analyze(rows, bodies, read_u64, associated)
    chains = result["chains"]
    table_level = [chain for chain in chains if chain.grade == TABLE_LEVEL]
    proven_from_table_level = [chain for chain in table_level if chain.proven]

    print(f"rows: {len(rows)}  ({len(rows) - len(table_level)} site-provable / "
          f"{len(table_level)} table-level before the pass)")
    print(f"table-level rows analysed: {len(table_level)}")
    print(f"table-level rows proven by store-to-load forwarding: {len(proven_from_table_level)}")
    print(f"proofs after the pass: {sum(1 for chain in chains if chain.proven)}")
    print()
    blockers = Counter(chain.blocker for chain in table_level)
    print("table-level blocker histogram:")
    for name, count in blockers.most_common():
        print(f"  {name:44s} {count}")
    print()
    print("per-row verdicts (table-level rows):")
    for chain in table_level:
        extra = f" row-slot={chain.row_slot_address:#x}"
        if chain.would_read is not None:
            extra += f" would-read={chain.would_read:#x}"
            if chain.read_target is not None:
                extra += f"->{chain.read_target:#x}"
        print(f"  {chain.site:#08x} {'PROVEN  ' if chain.proven else 'unproven'} "
              f"{chain.blocker or '':42s}{extra}")

    if args.json:
        document = {
            "schema": "sotc-store-to-load-forwarding/1",
            "inputs": {
                "header_sha256": sha256_file(args.header),
                "dis_sha256": sha256_file(args.dis),
                "fde_sha256": sha256_file(args.fde),
                "elf_sha256": sha256_file(args.elf),
            },
            "bounds": {"max_store_scan": MAX_STORE_SCAN, "max_value_scan": MAX_VALUE_SCAN,
                       "max_value_links": MAX_VALUE_LINKS, "forwarding_hops": 1},
            "rows": len(rows),
            "site_provable_before": len(rows) - len(table_level),
            "table_level_before": len(table_level),
            "table_level_proven": len(proven_from_table_level),
            "site_provable_after": sum(1 for chain in chains if chain.proven),
            "blockers": dict(sorted(blockers.items())),
            "chains": [chain.as_dict() for chain in chains],
        }
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(json.dumps(document, indent=2, sort_keys=True) + "\n")
        print(f"\nwrote {args.json}")

    print("STATIC_ANALYSIS_VERIFIED only: nothing was executed; a proven row is still not a "
          "reachability claim.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
