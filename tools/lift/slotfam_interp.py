#!/usr/bin/env python3
"""Offline symbolic interpreter for the short startup-constructor bodies (L3-SLOTS-322-334).

Reads bytes of the pinned private ELF only (through slot_pin_util.Elf, which supports a byte overlay
for mutation tests), disassembles them with the repository's objdump wrapper (scripts/disasm_range.sh)
and walks the straight-line control flow with a tiny value tracker. Nothing is loaded, relocated or
executed: callee bodies are NOT interpreted, a `call` is only RECORDED with the register arguments and
a snapshot of the stack words the arguments point at. The result is the ordered list of
  * stores (absolute addresses and stack slots, with size and value),
  * calls (direct target or indirect source, arguments, stack snapshot),
which the per-slot checkers compare against the constants of the native module. An unmodelled
instruction, an unknown branch condition or an unreadable value raises SystemExit (fail closed), so a
mutated body cannot slip through as "unchanged".
"""

from __future__ import annotations

import re
import struct
import subprocess
import sys
import tempfile
from dataclasses import dataclass, field
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from slot_pin_util import ROOT, Elf, require  # noqa: E402

INSN = re.compile(r"^\s*([0-9a-f]+):\s+([0-9a-f ]+?)\s*\t(\S+)(?:\s+(.*))?$")

R64 = ["rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi"] + [f"r{i}" for i in range(8, 16)]
R32 = ["eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"] + [f"r{i}d" for i in range(8, 16)]
R8 = {"al": "rax", "cl": "rcx", "dl": "rdx", "bl": "rbx", "sil": "rsi", "dil": "rdi"}
SUB32 = dict(zip(R32, R64))
ARG_REGS = ["rdi", "rsi", "rdx", "rcx", "r8", "r9"]
CALLER_SAVED = ["rax", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11"]
MASK64 = (1 << 64) - 1


class Unknown(Exception):
    pass


@dataclass
class Insn:
    addr: int
    length: int
    mnem: str
    ops: str


@dataclass
class Call:
    at: int
    target: object          # int (direct), or ("indirect", description)
    args: list
    stack: dict             # arg register -> list of 8 qwords read at the pointed stack address


@dataclass
class Trace:
    stores: list = field(default_factory=list)     # (kind, addr, size, value) in program order
    calls: list = field(default_factory=list)
    mem: dict = field(default_factory=dict)        # (kind, addr) -> (size, value)
    regs: dict = field(default_factory=dict)
    visited: list = field(default_factory=list)
    branches: list = field(default_factory=list)   # (addr, mnemonic, target, taken) of every conditional / jmp
    events: list = field(default_factory=list)     # program-order interleaving: ("store", i) / ("call", i)

    def abs_u(self, addr: int, size: int):
        e = self.mem.get(("abs", addr))
        if e is None or e[0] != size:
            return None
        return e[1]


def decode(elf: Elf, rva: int, size: int) -> list[Insn]:
    with tempfile.TemporaryDirectory(prefix="slotfam_") as tmp:
        blob = Path(tmp) / "w.bin"
        blob.write_bytes(elf.read(rva, size + 16))
        text = subprocess.run([str(ROOT / "scripts/disasm_range.sh"), str(blob), hex(rva)],
                              check=True, capture_output=True, text=True).stdout
    out = []
    for line in text.splitlines():
        m = INSN.match(line)
        if not m:
            continue
        addr = int(m.group(1), 16)
        if addr >= rva + size:
            continue
        ops = (m.group(4) or "").split("#")[0].strip()
        out.append(Insn(addr, len(bytes.fromhex(m.group(2).replace(" ", ""))), m.group(3), ops))
    return out


def _split(ops: str) -> list[str]:
    parts, depth, cur = [], 0, ""
    for ch in ops:
        if ch == "[":
            depth += 1
        elif ch == "]":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append(cur.strip())
            cur = ""
        else:
            cur += ch
    if cur.strip():
        parts.append(cur.strip())
    return parts


SIZES = {"byte": 1, "word": 2, "dword": 4, "qword": 8, "xmmword": 16, "ymmword": 32}


class Interp:
    def __init__(self, elf: Elf, insns: list[Insn], entry: int) -> None:
        self.elf = elf
        self.by_addr = {i.addr: i for i in insns}
        self.t = Trace()
        self.regs: dict = {r: ("uninit", r) for r in R64}
        self.regs["rsp"] = ("rsp", 0)
        self.regs["rbp"] = ("rbp", 0)
        self.vec: dict = {}
        self.flag: object = None   # ("zf", bool) or None
        self.entry = entry

    # ---- operands ---------------------------------------------------------------------------------
    def reg_read(self, name: str):
        if name in self.regs:
            return self.regs[name]
        if name in SUB32:
            v = self.regs[SUB32[name]]
            return (v & 0xFFFFFFFF) if isinstance(v, int) else ("sub", name)
        if name in R8:
            v = self.regs[R8[name]]
            return (v & 0xFF) if isinstance(v, int) else ("sub", name)
        raise Unknown(f"register {name}")

    def reg_write(self, name: str, value) -> None:
        if name in self.regs:
            self.regs[name] = value
        elif name in SUB32:
            self.regs[SUB32[name]] = (value & 0xFFFFFFFF) if isinstance(value, int) else ("sub", name)
        else:
            raise Unknown(f"register write {name}")

    def addr_of(self, mem: str, insn: Insn):
        m = re.search(r"\[(.*)\]", mem)
        require(m is not None, f"no memory operand in {mem!r}")
        expr = m.group(1).replace(" ", "")
        if expr.startswith("rip"):
            disp = int(expr[3:], 0) if len(expr) > 3 else 0
            return ("abs", insn.addr + insn.length + disp)
        terms = re.findall(r"([+-]?)([a-z0-9]+(?:\*\d)?)", expr)
        base = None
        disp = 0
        for sign, tok in terms:
            if "*" in tok:
                raise Unknown(f"index operand {mem!r}")
            if tok in R64:
                require(sign in ("", "+") and base is None, f"complex address {mem!r}")
                base = self.reg_read(tok)
            else:
                disp += int(tok, 0) * (-1 if sign == "-" else 1)
        if isinstance(base, int):
            return ("abs", (base + disp) & MASK64)
        if isinstance(base, tuple) and base[0] in ("rbp", "rsp"):
            return (base[0], base[1] + disp)
        if isinstance(base, tuple) and base[0] == "load" and disp == 0:
            return ("canary", base[1])          # the stack-protector cell reached through a GOT slot
        raise Unknown(f"address base {base!r} in {mem!r}")

    def mem_write(self, key, size: int, value) -> None:
        kind, addr = key
        for (k, a), (s, _) in list(self.t.mem.items()):
            if k == kind and a != addr and a < addr + size and addr < a + s:
                del self.t.mem[(k, a)]          # overlapped entry replaced
        self.t.mem[(kind, addr)] = (size, value)
        self.t.stores.append((kind, addr, size, value))
        self.t.events.append(("store", len(self.t.stores) - 1))

    def mem_read(self, key, size: int):
        kind, addr = key
        if kind == "canary":
            return ("canary", addr)
        e = self.t.mem.get((kind, addr))
        if e is not None:
            if e[0] != size:
                raise Unknown(f"size-mismatched read at {key}")
            return e[1]
        if kind == "abs":
            r = self.elf.relocs.get(addr)
            if r is not None and r["type"] == 8 and size == 8:
                return r["addend"]
            return ("load", addr)
        return ("uninit", key)

    # ---- value helpers ----------------------------------------------------------------------------
    def value(self, op: str, insn: Insn):
        op = op.strip()
        if op in R64 or op in SUB32 or op in R8:
            return self.reg_read(op)
        if re.fullmatch(r"-?0x[0-9a-fA-F]+|-?\d+", op):
            return int(op, 0) & MASK64
        raise Unknown(f"value operand {op!r}")

    def size_of(self, op: str) -> int:
        m = re.match(r"(byte|word|dword|qword|xmmword|ymmword) ptr", op)
        return SIZES[m.group(1)] if m else 0

    def deref(self, ptr, size: int):
        """Load through a tracked pointer value (canary pointer etc.)."""
        if isinstance(ptr, tuple) and ptr[0] == "load":
            return ("canary", ptr[1])
        if isinstance(ptr, int):
            return self.mem_read(("abs", ptr), size)
        raise Unknown(f"deref of {ptr!r}")

    # ---- execution --------------------------------------------------------------------------------
    def run(self) -> Trace:
        pc = self.entry
        steps = 0
        while True:
            steps += 1
            require(steps < 4000, "interpreter step limit")
            insn = self.by_addr.get(pc)
            if insn is None:
                raise SystemExit(f"FAIL pc {pc:#x} not on a decoded instruction boundary")
            self.t.visited.append(pc)
            nxt = pc + insn.length
            m, ops = insn.mnem, _split(insn.ops)
            try:
                nxt = self.step(insn, m, ops, nxt)
            except Unknown as exc:
                raise SystemExit(f"FAIL unmodelled `{m} {insn.ops}` at {pc:#x}: {exc}")
            if nxt is None:
                break
            pc = nxt
        self.t.regs = dict(self.regs)
        return self.t

    def step(self, insn: Insn, m: str, ops: list[str], nxt: int):
        if m in ("push", "pop", "nop", "pause", "lfence", "endbr64"):
            return nxt
        if m == "ret":
            return None
        if m in ("sub", "and", "add") and ops[0] == "rsp":
            return nxt
        if m == "lea" and ops[0] == "rsp":
            return nxt
        if m == "xor" and ops[0] == ops[1]:
            self.reg_write(ops[0], 0)
            return nxt
        if m == "mov" or m == "movabs":
            dst, src = ops
            if "[" in dst:
                key = self.addr_of(dst, insn)
                size = self.size_of(dst)
                val = self.value(src, insn)
                if isinstance(val, int) and size < 8:
                    val &= (1 << (8 * size)) - 1
                self.mem_write(key, size, val)
            elif "[" in src:
                key = self.addr_of(src, insn)
                size = self.size_of(src) or 8
                if key[0] == "abs" and not isinstance(self.mem_read(key, size), int):
                    # load through a rip-relative GOT-like cell: symbolic pointer
                    self.reg_write(dst, ("load", key[1]))
                else:
                    self.reg_write(dst, self.mem_read(key, size))
            else:
                self.reg_write(dst, self.value(src, insn))
            return nxt
        if m == "lea":
            dst, src = ops
            kind, addr = self.addr_of(src, insn)
            self.reg_write(dst, addr if kind == "abs" else (kind, addr))
            return nxt
        if m == "add" or m == "sub":
            dst, src = ops
            a, b = self.reg_read(dst), self.value(src, insn)
            require(isinstance(a, int) and isinstance(b, int), "arith on non-constant")
            if m == "sub":
                b = -b
            self.reg_write(dst, (a + b) & MASK64)
            return nxt
        if m in ("vxorps", "vpxor") and ops[0] == ops[1] == ops[2]:
            self.vec[ops[0]] = (0, 0, 0, 0)
            return nxt
        if m in ("vmovups", "vmovdqu", "vmovdqa", "vmovaps", "vmovapd", "vmovdqu64"):
            dst, src = ops
            if "[" in dst:
                key = self.addr_of(dst, insn)
                words = self.vec_get(src)
                n = 4 if src.startswith("ymm") else 2
                for i in range(n):
                    self.mem_write((key[0], key[1] + 8 * i), 8, words[i])
            else:
                key = self.addr_of(src, insn)
                n = 4 if dst.startswith("ymm") else 2
                self.vec[dst] = tuple(self.mem_read((key[0], key[1] + 8 * i), 8) for i in range(n)) + (0,) * (4 - n)
            return nxt
        if m == "vmovq":
            dst, src = ops
            if "[" in src:
                key = self.addr_of(src, insn)
                self.vec[dst] = (self.mem_read(key, 8), 0, 0, 0)
            elif "[" in dst:
                key = self.addr_of(dst, insn)
                self.mem_write(key, 8, self.vec_get(src)[0])
            else:
                raise Unknown("vmovq form")
            return nxt
        if m == "vextractf128":
            dst, src, imm = ops
            require(int(imm, 0) == 1, "vextractf128 lane")
            w = self.vec_get(src)
            self.vec[dst] = (w[2], w[3], 0, 0)
            return nxt
        if m == "vpextrq":
            dst, src, imm = ops
            self.reg_write(dst, self.vec_get(src)[int(imm, 0)])
            return nxt
        if m == "test":
            require(ops[0] in R64 and ops[1] == ops[0], "only `test r64, r64` is modelled")
            a = self.reg_read(ops[0])
            b = a
            require(isinstance(a, int), "test on non-constant")
            self.flag = ("zf", a == 0)
            return nxt
        if m == "cmp":
            a = ops[0]
            canary = any(("[" in o) or o in ("rax",) for o in ops)
            va = self.reg_read(a) if "[" not in a else None
            vb = None
            if len(ops) > 1 and "[" in ops[1]:
                vb = self.mem_read(self.addr_of(ops[1], insn), 8)
            elif len(ops) > 1 and "[" not in ops[1]:
                vb = self.value(ops[1], insn)
            if isinstance(va, tuple) and va[0] == "canary":
                require(vb == va, f"stack-check compare reads {vb!r}, not the saved canary {va!r}")
                self.flag = ("zf", True)       # canary == saved canary: epilogue falls through
                return nxt
            if isinstance(va, int) and isinstance(vb, int):
                self.flag = ("zf", va == vb)
                return nxt
            raise Unknown(f"compare of {va!r} and {vb!r}")
        if m in ("je", "jne", "jmp"):
            target = int(ops[0].split()[0], 0)
            if m == "jmp":
                self.t.branches.append((insn.addr, m, target, True))
                return target
            require(self.flag is not None, "branch without flags")
            zf = self.flag[1]
            taken = zf if m == "je" else (not zf)
            self.t.branches.append((insn.addr, m, target, taken))
            return target if taken else nxt
        if m == "call":
            return self.do_call(insn, ops[0], nxt)
        raise Unknown("mnemonic not modelled")

    def vec_get(self, name: str):
        require(name in self.vec, f"vector register {name} read before write")
        return self.vec[name]

    def do_call(self, insn: Insn, target_op: str, nxt: int):
        if re.fullmatch(r"0x[0-9a-f]+(\s.*)?", target_op):
            target = int(target_op.split()[0], 0)
        elif target_op in R64:
            target = self.reg_read(target_op)
        elif "[" in target_op:
            target = self.mem_read(self.addr_of(target_op, insn), 8)
        else:
            raise Unknown(f"call operand {target_op!r}")
        args = [self.regs[r] for r in ARG_REGS]
        stack = {}
        for r, v in zip(ARG_REGS, args):
            if isinstance(v, tuple) and v[0] in ("rbp", "rsp"):
                stack[r] = [self.t.mem.get((v[0], v[1] + 8 * i), (8, ("uninit",)))[1] for i in range(12)]
        self.t.calls.append(Call(insn.addr, target, args, stack))
        self.t.events.append(("call", len(self.t.calls) - 1))
        for r in CALLER_SAVED:
            self.regs[r] = ("uninit", f"call@{insn.addr:x}")
        self.vec.clear()
        return nxt


def trace_digest(t: Trace) -> str:
    """sha256 of the complete recorded walk: executed pcs, every store (including stack cells), every call with
    its register arguments and stack snapshots, and every branch (taken or not, with its target)."""
    import hashlib
    import json
    calls = [(c.at, repr(c.target), [repr(a) for a in c.args],
              sorted((r, [repr(w) for w in ws]) for r, ws in c.stack.items())) for c in t.calls]
    stores = [(k, a, sz, repr(v)) for (k, a, sz, v) in t.stores]
    blob = json.dumps({"pcs": t.visited, "stores": stores, "calls": calls, "branches": t.branches},
                      sort_keys=True).encode()
    return hashlib.sha256(blob).hexdigest()


def run_body(elf: Elf, rva: int, size: int) -> Trace:
    return Interp(elf, decode(elf, rva, size), rva).run()


def is_ptr(v, kind: str) -> bool:
    return isinstance(v, tuple) and v[0] == kind
