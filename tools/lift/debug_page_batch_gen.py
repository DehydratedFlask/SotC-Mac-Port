#!/usr/bin/env python3
"""Offline shape recovery / code generation for the CPU-01-STARTUP-BATCH-DEBUG-PAGE cluster.

WHAT THIS IS
    A read-only reader of the pinned private ELF (`local/re/eboot.elf`, sha256 d0723afb...5861d3)
    plus a transcription aid. It decodes one startup-constructor body with `llvm-objdump`, runs a
    straight-line abstract interpretation over it (registers + a byte-addressed stack image),
    recognises the debug-page row program documented in native/core/debug_page_var_item.hpp, and
    either prints or *emits* the per-slot data files (header, thin adapter, fixture, checker).

    Nothing is loaded, relocated or executed; no original RVA is called. Every number it emits is
    read from the pinned bytes, and every emitted checker re-reads those bytes at run time, so a
    transcription error fails the checker instead of silently pinning itself.

WHAT IT REFUSES
    A body is accepted only when *every* statement is one of the row kinds the model can emit:

      float row  : pre-store 0x15942b0, call 0xc300 (bounded) or 0x6aa00 (unbounded), the name and
                   description views, the functor `callq *0x18(%rax)` (bare-ret destroy), the two
                   value stores, 0x20d60, two 0xf590 releases, __cxa_atexit(0x20f0)
      bool row   : pre-store 0x1594218, call 0x20af0, the bool field stores (+0x60 reference,
                   +0x68 default, +0x69/+0x70/+0xa0 zeros, +0xb0 functor tag, +0xb8, +0xc0 value),
                   0x20d60, two 0xf590 releases, __cxa_atexit(0x20b0)
      item row   : 0x27050 (slider, explicit range) or 0x26ea0 (toggle, gate-derived range), two
                   0xf590 releases, __cxa_atexit(0x272d0)
      canary     : the guard load, the epilogue compare, one `jne` to the __stack_chk_fail call

    Any other callee, a second conditional branch, an indirect call that is not
    `callq *0x18(%rax)`, a view that is not a (rip constant, immediate length) pair, or a value the
    interpreter cannot resolve statically -> the body is REJECTED with a named reason and nothing is
    written.

Usage
    debug_page_batch_gen.py --show 518 [--show ...]      # recover and print the row program
    debug_page_batch_gen.py --emit 518 410 [...]         # write the four per-slot files
    debug_page_batch_gen.py --select                     # re-check every candidate of the batch

`--emit` is a transcription aid, not an oracle: the emitted checker re-reads the ELF and the emitted
fixture asserts the model's state, so both fail if a transcription is wrong.
"""

from __future__ import annotations

import argparse
import hashlib
import re
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))
from slot_pin_util import Elf  # noqa: E402

ELF_PATH = ROOT / "local/re/eboot.elf"
TABLE_TOP = 0x2DD70E0
EH_FRAME_HDR = 0x150C08C
DISC = "/usr/bin/objdump"

F590 = 0xF590
REGISTER = 0x20D60
ATEXIT = 0xF8F390
CHKFAIL = 0xF8F3A0
BASE_CTOR = 0x20AF0
BOUNDED = 0xC300
UNBOUNDED = 0x6AA00
SLIDER = 0x27050
TOGGLE = 0x26EA0
FLOAT_CLASS = 0x15942B0
BOOL_CLASS = 0x1594218
FLOAT_DTOR = 0x20F0
BOOL_DTOR = 0x20B0
ITEM_DTOR = 0x272D0
FLOAT_FN_BODIES = (0xC4A0, 0xC4C0, 0xC4E0, 0xC4F0)
GUARD_CELL = 0x15FE518
DSO = 0x1604218
ITEM_FLAGS = 0x30
VAR_FLAGS = 1
ROW_CTORS = {BOUNDED: "bounded_float", UNBOUNDED: "unbounded_float", BASE_CTOR: "bool",
             SLIDER: "slider", TOGGLE: "toggle"}
ALLOWED_CALLEES = {F590, REGISTER, ATEXIT, CHKFAIL} | set(ROW_CTORS)

_LINE = re.compile(r"^\s*([0-9a-f]+):\s+((?:[0-9a-f]{2}\s)+)\s*(.*\S)\s*$")
_CONT = re.compile(r"^\s+((?:[0-9a-f]{2}\s)+)\s*(.*\S)\s*$")
_HEX = re.compile(r"0x[0-9a-f]+")
_RIPCOMMENT = re.compile(r"#\s*(0x[0-9a-f]+)")
_ALIAS = {"edi": "rdi", "esi": "rsi", "edx": "rdx", "ecx": "rcx", "r8d": "r8", "r9d": "r9",
          "eax": "rax", "ax": "rax", "al": "rax", "ebx": "rbx", "bx": "rbx", "bl": "rbx",
          "r12d": "r12", "r13d": "r13", "r14d": "r14", "r15d": "r15", "cx": "rcx", "dx": "rdx"}
ARGS6 = ("rdi", "rsi", "rdx", "rcx", "r8", "r9")
CLOBBER = set(ARGS6) | {"rax", "r10", "r11", "xmm0", "xmm1", "ymm0", "ymm1"}
MEM = re.compile(r"^(-?0x[0-9a-f]+|-?\d+)?\(%([a-z0-9]+)\)$")
IMM = re.compile(r"^\$(-?0x[0-9a-f]+|-?\d+)$")


class GenError(Exception):
    """The body is not a program the shared model can emit; the message names the reason."""


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def fde_of(e: Elf, rva: int) -> tuple[int, int, int]:
    """(size, fde_record_addr, record_length) of the FDE that starts at `rva`."""
    hdr = e.raw(EH_FRAME_HDR, 12)
    if (hdr[0], hdr[1], hdr[2], hdr[3]) != (1, 0x1B, 0x03, 0x3B):
        raise GenError("eh_frame_hdr encodings")
    count = struct.unpack_from("<i", hdr, 8)[0]
    table = e.raw(EH_FRAME_HDR + 12, 8 * count)
    for i in range(count):
        loc, fde = struct.unpack_from("<ii", table, 8 * i)
        if EH_FRAME_HDR + loc == rva:
            addr = EH_FRAME_HDR + fde
            rec = e.raw(addr, 16)
            return struct.unpack_from("<I", rec, 12)[0], addr, struct.unpack_from("<I", rec, 0)[0]
    raise GenError(f"no eh_frame_hdr entry for 0x{rva:x}")


def disassemble(rva: int, size: int) -> list[dict]:
    proc = subprocess.run([DISC, "-d", "--show-raw-insn", f"--start-address={rva:#x}",
                           f"--stop-address={rva + size:#x}", str(ELF_PATH)],
                          capture_output=True, text=True, timeout=120)
    if proc.returncode != 0:
        raise GenError(f"objdump failed: {proc.stderr[-200:]}")
    out: list[dict] = []
    for line in proc.stdout.splitlines():
        m = _LINE.match(line)
        if m:
            addr = int(m.group(1), 16)
            if not (rva <= addr < rva + size):
                continue
            text = m.group(3)
            if text.endswith(">:"):
                continue
            out.append({"addr": addr, "bytes": bytes.fromhex(m.group(2).replace(" ", "")),
                        "text": text})
            continue
        c = _CONT.match(line)
        if c and out:
            out[-1]["bytes"] += bytes.fromhex(c.group(1).replace(" ", ""))
            out[-1]["text"] += " " + c.group(2)
    for ins in out:
        parts = ins["text"].split(None, 1)
        ins["mnem"] = parts[0]
        ins["ops"] = parts[1] if len(parts) > 1 else ""
        m = _RIPCOMMENT.search(ins["ops"])
        ins["rip"] = int(m.group(1), 16) if ("(%rip)" in ins["ops"] and m) else None
        ins["len"] = len(ins["bytes"])
    if not out:
        raise GenError("empty disassembly")
    return out


def split_ops(ops: str) -> list[str]:
    out, depth, cur = [], 0, ""
    for ch in ops.split("#")[0]:
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
        if ch == "," and depth == 0:
            out.append(cur.strip())
            cur = ""
        else:
            cur += ch
    if cur.strip():
        out.append(cur.strip())
    return out


def reg_of(tok: str) -> str | None:
    t = tok.strip()
    if not t.startswith("%"):
        return None
    name = t[1:]
    return _ALIAS.get(name, name)


class Write:
    """One stored byte range in the stack image."""
    __slots__ = ("ident", "value", "start", "width")

    def __init__(self, ident: int, value, start: int, width: int) -> None:
        self.ident, self.value, self.start, self.width = ident, value, start, width


class Machine:
    """Straight-line constant propagation over registers and a byte-addressed stack image.

    A register value is ('imm', n) | ('rip', addr) | ('addr', base, disp) | ('zero',), or absent
    (unknown). Stack cells remember the single write that produced them so a read only succeeds when
    the requested range is covered by one store (or by a run of zero stores).
    """

    def __init__(self) -> None:
        self.reg: dict[str, tuple] = {}
        self.stack: dict[tuple[str, int], Write] = {}
        self.fields: dict[tuple[int, int], tuple[int, int]] = {}
        self.calls: list[dict] = []
        self._write_id = 0

    # -- reads -----------------------------------------------------------------
    def stack_read(self, base: str, off: int, width: int):
        cells = [self.stack.get((base, off + i)) for i in range(width)]
        if any(c is None for c in cells):
            return None
        first = cells[0]
        if all(c is first for c in cells) and width == first.width and off == first.start:
            return first.value
        if all(c.value == ("imm", 0) for c in cells):
            return ("imm", 0)
        return None

    def view(self, rval) -> tuple[tuple[int, int], dict] | None:
        if not isinstance(rval, tuple) or rval[0] != "addr":
            return None
        _, base, off = rval
        data = self.stack_read(base, off, 8)
        size = self.stack_read(base, off + 8, 8)
        aux = self.stack_read(base, off + 0x10, 8)
        ctl = self.stack_read(base, off + 0x18, 8)
        if not (isinstance(data, tuple) and data[0] == "rip"):
            return None
        if not (isinstance(size, tuple) and size[0] == "imm"):
            return None
        if aux != ("imm", 0) or ctl != ("imm", 0):
            return None
        return data, {"data": data[1], "size": size[1]}

    def field(self, obj: int, disp: int):
        cell = self.fields.get((obj, disp))
        return None if cell is None else cell[0]

    # -- run -------------------------------------------------------------------
    def run(self, ins: list[dict]) -> None:
        for item in ins:
            self.step(item)

    def step(self, item: dict) -> None:
        mnem, ops = item["mnem"], item["ops"]
        parts = split_ops(ops)
        dst = parts[-1] if parts else ""
        src = parts[0] if len(parts) > 1 else ""
        addr = item["addr"]

        if mnem.startswith("call"):
            if ops.lstrip().startswith("*"):
                if not re.fullmatch(r"\*0x18\(%r[a-z0-9]+\)", ops.strip()):
                    raise GenError(f"indirect call not `*0x18(%rax)` at 0x{addr:x}: {ops.strip()}")
                self.calls.append({"addr": addr, "target": None, "args": self._args()})
            else:
                target = _HEX.match(ops.strip())
                if target is None:
                    raise GenError(f"call without a direct target at 0x{addr:x}")
                t = int(target.group(0), 16)
                if t not in ALLOWED_CALLEES:
                    raise GenError(f"callee 0x{t:x} at 0x{addr:x} is not a modelled row statement")
                self.calls.append({"addr": addr, "target": t, "args": self._args()})
            for r in CLOBBER:
                self.reg.pop(r, None)
            return

        if mnem.startswith("v") or mnem in ("movups", "movaps", "movdqu", "movdqa", "movss",
                                            "movsd", "movd", "vmov"):
            self._vector(item, parts, dst, src)
            return

        if mnem in ("mov", "movl", "movq", "movabs", "movw", "movb") and dst.startswith("%"):
            r = reg_of(dst)
            self.reg[r] = self._value(src, item)
            return

        if mnem in ("lea", "leaq"):
            r = reg_of(dst)
            if r is None:
                return
            m = MEM.match(src)
            if m and m.group(2) == "rip":
                if item["rip"] is None:
                    raise GenError(f"lea with unresolved rip operand at 0x{addr:x}")
                self.reg[r] = ("rip", item["rip"])
            elif m and m.group(2) in ("rsp", "rbp"):
                self.reg[r] = ("addr", m.group(2), int(m.group(1) or "0", 0))
            else:
                self.reg[r] = ("unknown",)
            return

        if mnem in ("xor", "xorl", "xorq", "xorps", "xorpd", "pxor", "vxorps", "vxorpd"):
            if len(parts) >= 2 and reg_of(parts[0]) == reg_of(parts[1]) and reg_of(parts[0]):
                self.reg[reg_of(parts[0])] = ("zero",) if "xmm" in parts[0] or "ymm" in parts[0] \
                    else ("imm", 0)
            return

        m = MEM.match(dst)
        if m:
            base, off = m.group(2), int(m.group(1) or "0", 0)
            width = {"movb": 1, "movw": 2, "movl": 4, "mov": 8, "movq": 8}.get(mnem)
            value = self._value(src, item)
            if mnem in ("movd", "movd"):
                width = 4
            if width is None:
                return
            self._store(base, off, width, value)
            return

        if dst.startswith("%"):
            r = reg_of(dst)
            if r and not r.startswith(("xmm", "ymm")):
                self.reg[r] = ("unknown",)
        self._clobber_flags(dst, src)

    def _clobber_flags(self, dst: str, src: str) -> None:
        return

    def _vector(self, item: dict, parts: list[str], dst: str, src: str) -> None:
        mnem = item["mnem"]
        if dst.startswith("%"):
            r = reg_of(dst)
            if r and r.startswith(("xmm", "ymm")):
                self.reg[r] = self._value(src, item)
            return
        m = MEM.match(dst)
        if not m:
            return
        base, off = m.group(2), int(m.group(1) or "0", 0)
        width = {"vmovss": 4, "vmovsd": 8, "movss": 4, "movsd": 8, "movd": 4, "vmovd": 4}.get(mnem)
        if width is None:
            width = 32 if "ymm" in src else 16
        value = self._value(src, item)
        self._store(base, off, width, value)

    def _value(self, src: str, item: dict):
        if IMM.match(src):
            return ("imm", int(IMM.match(src).group(1), 0))
        r = reg_of(src)
        if r is not None:
            v = self.reg.get(r)
            return v if v is not None else ("unknown",)
        m = MEM.match(src)
        if m and m.group(2) == "rip" and item["rip"] is not None:
            return ("rip", item["rip"])
        return ("unknown",)

    def _store(self, base: str, off: int, width: int, value) -> None:
        self._write_id += 1
        if base in ("rsp", "rbp"):
            w = Write(self._write_id, value, off, width)
            for i in range(width):
                self.stack[(base, off + i)] = w
            return
        holder = self.reg.get(base)
        if isinstance(holder, tuple) and holder[0] == "rip":
            self.fields[(holder[1], off)] = (value, width)

    def _args(self) -> dict:
        return {r: self.reg.get(r) for r in ARGS6}


# ---------------------------------------------------------------- body recognition

def bits_at(e: Elf, addr: int) -> int:
    return struct.unpack("<I", e.read(addr, 4))[0]


def string_at(e: Elf, addr: int) -> bytes:
    out = bytearray()
    for i in range(0, 4096):
        b = e.read(addr + i, 1)[0]
        if b == 0:
            return bytes(out)
        out.append(b)
    raise GenError(f"unterminated string at 0x{addr:x}")


class Body:
    def __init__(self, e: Elf, index: int) -> None:
        self.e = e
        self.index = index
        self.rva = e.u64(TABLE_TOP - 8 * index)
        self.size, self.fde_addr, self.fde_reclen = fde_of(e, self.rva)
        self.table_word = TABLE_TOP - 8 * index
        self.body = e.read(self.rva, self.size)
        self.sha = sha256(self.body)
        self.ins = disassemble(self.rva, self.size)
        self.m = Machine()
        self.m.run(self.ins)
        self._check_envelope()
        self.rows = self._rows()

    # -- envelope ------------------------------------------------------------
    def _check_envelope(self) -> None:
        jcc = [i for i in self.ins if i["mnem"].startswith("j") and not i["mnem"].startswith("jmp")]
        if len(jcc) != 1:
            raise GenError(f"{len(jcc)} conditional branches (the shape has exactly one: the canary)")
        branch = jcc[0]
        if branch["mnem"] not in ("jne", "jne "):
            raise GenError(f"the only branch is `{branch['mnem']}`, not jne")
        target = int(_HEX.match(split_ops(branch["ops"])[0]).group(0), 16)
        chk = [c for c in self.m.calls if c["target"] == CHKFAIL]
        if len(chk) != 1 or chk[0]["addr"] != target:
            raise GenError(f"the branch target 0x{target:x} is not the single __stack_chk_fail call")
        self.branch = (branch["addr"], target)
        functors = [c for c in self.m.calls if c["target"] is None]
        self.functor_sites = [c["addr"] for c in functors]
        # the guard must be read on entry and re-read in the epilogue from the same cell
        guard_reads = [i for i in self.ins if "(%rip)" in i["ops"] and i["rip"] == GUARD_CELL]
        if len(guard_reads) < 2:
            raise GenError(f"{len(guard_reads)} reads of the stack-guard cell (need entry + epilogue)")
        self.guard_reads = [i["addr"] for i in guard_reads]

    # -- rows ----------------------------------------------------------------
    def _rows(self) -> list[dict]:
        calls = self.m.calls
        starts = [n for n, c in enumerate(calls) if c["target"] in ROW_CTORS]
        if not starts:
            raise GenError("no modelled row constructor")
        rows = []
        for k, start in enumerate(starts):
            end = starts[k + 1] if k + 1 < len(starts) else len(calls)
            window = calls[start:end]
            # the row's own atexit is the first ATEXIT call in the window
            atexits = [n for n, c in enumerate(window) if c["target"] == ATEXIT]
            if not atexits:
                raise GenError(f"row {k} (0x{window[0]['addr']:x}) has no __cxa_atexit")
            row_calls = window[: atexits[0] + 1]
            rest = window[atexits[0] + 1:]
            if rest:
                targets = ", ".join(f"0x{c['target']:x}" for c in rest if c["target"])
                raise GenError(f"row {k} has statements after its __cxa_atexit: {targets}")
            rows.append(self._row(k, row_calls, self.ins[0]["addr"], self.rva + self.size))
        return rows

    def _row(self, k: int, window: list[dict], body_start: int, body_end: int) -> dict:
        e = self.e
        start_call = window[0]
        kind = ROW_CTORS[start_call["target"]]
        seq = [c["target"] for c in window]
        counts = {t: seq.count(t) for t in set(seq)}
        atexit = next(c for c in window if c["target"] == ATEXIT)
        dtor = atexit["args"]["rdi"]
        obj = atexit["args"]["rsi"]
        if not (isinstance(dtor, tuple) and dtor[0] == "rip"):
            raise GenError(f"row {k}: __cxa_atexit destructor is not a constant")
        if not (isinstance(obj, tuple) and obj[0] == "rip"):
            raise GenError(f"row {k}: __cxa_atexit object is not a constant")
        dso = atexit["args"]["rdx"]
        if dso != ("rip", DSO):
            raise GenError(f"row {k}: __cxa_atexit dso handle is not 0x{DSO:x}")
        row = {"kind": kind, "dtor": dtor[1], "object": obj[1], "seq": seq,
               "atexit_site": atexit["addr"], "start_site": start_call["addr"],
               "calls": [c["addr"] for c in window]}
        if kind in ("bounded_float", "unbounded_float"):
            self._float_row(row, start_call, counts, k)
        elif kind == "bool":
            self._bool_row(row, start_call, counts, k, window)
        else:
            self._item_row(row, start_call, counts, k, window)
        return row

    # -- per kind ------------------------------------------------------------
    def _float_row(self, row: dict, ctor: dict, counts: dict, k: int) -> None:
        e = self.e
        bounded = row["kind"] == "bounded_float"
        need = {F590: 2, REGISTER: 1}
        for target, n in need.items():
            if counts.get(target, 0) != n:
                raise GenError(f"row {k}: {counts.get(target, 0)} calls to 0x{target:x} (need {n})")
        args = ctor["args"]
        if args["rdi"] != ("rip", row["object"]):
            raise GenError(f"row {k}: constructor `this` is not the atexit object")
        name = self.m.view(args["rsi"])
        if name is None:
            raise GenError(f"row {k}: name argument is not a (rip constant, immediate length) view")
        name_ptr, name_len = name[0][1], name[1]["size"]
        if bounded:
            value_ref, min_arg, max_arg = args["rdx"], args["rcx"], args["r8"]
            stack_args = ctor.get("stack_args", [])
            if len(stack_args) < 2:
                raise GenError(f"row {k}: the bounded ctor has {len(stack_args)} stack arguments (need 2)")
            args_block, desc_arg = stack_args[0], stack_args[1]
        else:
            value_ref, args_block, desc_arg = args["rdx"], args["r8"], args["r9"]
        if not (isinstance(value_ref, tuple) and value_ref[0] == "addr"):
            raise GenError(f"row {k}: the value reference is not a stack cell")
        if bounded:
            def stack_bits(rval, what):
                if not isinstance(rval, tuple) or rval[0] != "addr":
                    raise GenError(f"row {k}: the {what} argument is not a stack cell")
                v = self.m.stack_read(rval[1], rval[2], 4)
                if not (isinstance(v, tuple) and v[0] == "imm"):
                    raise GenError(f"row {k}: the {what} cell is not a constant")
                return v[1] & 0xFFFFFFFF
            lo_bits = stack_bits(min_arg, "min")
            hi_bits = stack_bits(max_arg, "max")
        else:
            lo_bits = hi_bits = 0
        desc = self.m.view(desc_arg)
        if desc is None:
            raise GenError(f"row {k}: description argument is not a borrowed view")
        if not (isinstance(args_block, tuple) and args_block[0] == "addr"):
            raise GenError(f"row {k}: the argument functor block is not a stack address")
        tag = self.m.stack_read(args_block[1], args_block[2] + 0x40, 8)
        if not (isinstance(tag, tuple) and tag[0] == "rip"):
            raise GenError(f"row {k}: the argument functor tag is not a constant")
        q = [self.m.stack_read(args_block[1], args_block[2] + 8 * i, 8) for i in range(4)]
        if any(v != ("imm", 0) for v in q):
            raise GenError(f"row {k}: the argument q[0..3] block is not zero")
        for body in FLOAT_FN_BODIES:
            pass
        for off, want in zip((0x0, 0x8, 0x10, 0x18), FLOAT_FN_BODIES):
            got = e.reloc_target(tag[1] + off)
            if got != want:
                raise GenError(f"row {k}: argument tag 0x{tag[1]:x}+{off:#x} -> 0x{got:x} (want 0x{want:x})")
        functor = next(c for c in self.m.calls if c["target"] is None and c["addr"] in row["calls"])
        origin = self.m.calls.index(functor)
        value_sites = []
        for n, c in enumerate(self.m.calls):
            pass
        fields = {disp: self.m.field(row["object"], disp) for disp in (0x68, 0xD0)}
        for disp in (0x68, 0xD0):
            cell = fields[disp]
            if not (isinstance(cell, tuple) and cell[0] == "imm"):
                raise GenError(f"row {k}: the +{disp:#x} value store is not a constant")
        if fields[0x68][1] != fields[0xD0][1]:
            raise GenError(f"row {k}: +0x68 and +0xd0 disagree")
        row.update({
            "name": name_ptr, "name_len": name_len,
            "description": desc[1]["data"], "description_len": desc[1]["size"],
            "lo_bits": lo_bits, "hi_bits": hi_bits, "value_bits": fields[0xD0][1] & 0xFFFFFFFF,
            "fn_tag": tag[1],
            "ctor_site": ctor["addr"],
            "functor_site": functor["addr"],
            "register_site": next(c["addr"] for c in self.m.calls if c["target"] == REGISTER
                                  and c["addr"] in row["calls"]),
        })

    def _bool_row(self, row: dict, ctor: dict, counts: dict, k: int, window: list[dict]) -> None:
        for target, n in ((F590, 2), (REGISTER, 1)).items():
            if counts.get(target, 0) != n:
                raise GenError(f"row {k}: {counts.get(target, 0)} calls to 0x{target:x} (need {n})")
        args = ctor["args"]
        if args["rdi"] != ("rip", row["object"]):
            raise GenError(f"row {k}: base constructor `this` is not the atexit object")
        if args["rcx"] != ("imm", VAR_FLAGS):
            raise GenError(f"row {k}: base constructor flags {args['rcx']} != 1")
        name = self.m.view(args["rsi"])
        desc = self.m.view(args["rdx"])
        if name is None or desc is None:
            raise GenError(f"row {k}: the bool row's views are not borrowed (rip, immediate) pairs")
        tag = self.m.field(row["object"], 0xB0)
        value = self.m.field(row["object"], 0xC0)
        default = self.m.field(row["object"], 0x68)
        for what, cell in (("functor tag", tag), ("value", value), ("default", default)):
            if cell is None or cell[0] not in ("imm", "rip"):
                raise GenError(f"row {k}: the bool {what} is not a constant store")
        row.update({
            "name": name[0][1], "name_len": name[1]["size"],
            "description": desc[1]["data"], "description_len": desc[1]["size"],
            "fn_tag": tag[0][1], "value": value[0][1] & 0xFF, "default_value": default[0][1] & 0xFF,
            "ctor_site": ctor["addr"],
            "register_site": next(c["addr"] for c in window if c["target"] == REGISTER),
        })

    def _item_row(self, row: dict, ctor: dict, counts: dict, k: int, window: list[dict]) -> None:
        for target, n in ((F590, 2),).items():
            if counts.get(target, 0) != n:
                raise GenError(f"row {k}: {counts.get(target, 0)} calls to 0x{f:x} (need {n})"
                               if False else f"row {k}: {counts.get(target, 0)} f590 releases (need {n})")
        args = ctor["args"]
        if args["rdi"] != ("rip", row["object"]):
            raise GenError(f"row {k}: item constructor `this` is not the atexit object")
        if args["r8"] != ("imm", ITEM_FLAGS):
            raise GenError(f"row {k}: item flags {args['r8']} != 0x30")
        label = self.m.view(args["rsi"])
        text = self.m.view(args["rdx"])
        if label is None or text is None:
            raise GenError(f"row {k}: the item views are not borrowed (rip, immediate) pairs")
        target = args["rcx"]
        if not (isinstance(target, tuple) and target[0] == "rip"):
            raise GenError(f"row {k}: the item target is not a constant object")
        row.update({"label": label[0][1], "label_len": label[1]["size"],
                    "text": text[0][1], "text_len": text[1]["size"],
                    "target_object": target[1], "ctor_site": ctor["addr"]})
        if row["kind"] == "slider":
            for name, rv in (("min", args.get("xmm0")), ("max", args.get("xmm1"))):
                if not (isinstance(rv, tuple) and rv[0] == "f32"):
                    raise GenError(f"row {k}: the slider {name} is not a constant rodata float")
                row["range_" + name + "_bits"] = bits_at(self.e, rv[1])
