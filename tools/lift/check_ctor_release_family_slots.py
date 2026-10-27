#!/usr/bin/env python3
"""ELF byte-oracle for L5-RELEASE-FAMILY (native/core/ctor_release_family_slots).

Offline only: reads the pinned private ELF (local/re/eboot.elf) and the system disassembler
(`xcrun llvm-objdump`, text output only); never loads, relocates or executes an original RVA.

Slots covered (constructor table: slot i is the u64 at ELF file offset 0x2dda750 + (306-i)*8):
  name-object template (vptr, borrowed name view, 3 zero qwords, f590 on an inert stack view,
  __cxa_atexit): 371 372 405 433 470 472 481 483 484 485 487 529
  FNV-1a span hashes (0x161f80 over a stack view, f590 on the view): 434 435 438
  FNV-1a C-string hash tables (0xa2ddc0): 431 447

The script decodes each body with a small straight-line register/stack model (it only understands
the instructions that occur in these bodies; any other call or unknown store fails) and derives the
construction steps FROM THE ELF BYTES.  The native table in the header is then compared row by row
against that derivation, together with the module constants, body hashes/extents and every string.

Modes:
  (default)   full check, exit 0 on success
  --dump      print the derived steps as C++ table rows (used to author the native table)
  --tamper    run the full check against a deliberately altered native header; exits 1 when the
              divergence is detected (the mutation sanity proof)
"""
from __future__ import annotations

import hashlib
import re
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
TABLE_BASE = 0x2DDA750
FIRST_CTOR_SLOT = 306

F590 = 0xF590
ATEXIT = 0xF8F390
STACK_CHK = 0xF8F3A0
FNV_SPAN = 0x161F80
FNV_CSTR = 0xA2DDC0
DSO_HANDLE = 0x1604218
CANARY_GOT = 0x15FE518
FNV_BASIS = 0x811C9DC5

# slot -> (rva, body size, kind, body sha256)
SLOTS = {
    371: (0x70CDC0, 285, "name", None),
    372: (0x70A040, 165, "name", None),
    405: (0x5E7790, 309, "name", None),
    431: (0x523A00, 416, "cstr", None),
    433: (0x517850, 178, "name", None),
    434: (0x511C00, 174, "span", None),
    435: (0x510210, 180, "span", None),
    438: (0x4FBD80, 113, "span", None),
    447: (0x4C3410, 151, "cstr", None),
    470: (0x389A00, 311, "name", None),
    472: (0x3583C0, 289, "name", None),
    481: (0x2FD8F0, 178, "name", None),
    483: (0x2EF810, 178, "name", None),
    484: (0x2EEE10, 178, "name", None),
    485: (0x2EE5E0, 178, "name", None),
    487: (0x2CD060, 178, "name", None),
    529: (0x14C030, 178, "name", None),
}
ALLOWED_CALLS = {F590, ATEXIT, STACK_CHK, FNV_SPAN, FNV_CSTR}

# sha256 of each body (rva..rva+size), taken from the pinned ELF (--dump prints them).
BODY_SHA = {
    371: "0fbe2360920a754f1eea6ebfa7c58720ed805e03544b81edc5f58e28fb01c415", 372: "9d5a48c8dce07de7a562daa804ced9ee3a13cdf06fe4f16c2f29f8f64f37b4c1", 405: "144887d7fd528b437bfa0794819133229ae2c0a4bccd1ab866ecc9b88da6b985", 431: "919d6f6399f0fb79fc61fd1169340163f70f447bb9a043be606288834c2c4ac7", 433: "8612830b2c332e0a3d8806c9998de6de02b0a43d88b73dbabda11588d3645163", 434: "fab7347ba8c980ccef97fcb0dd28d3ad52c4d147e14aa71af0a376f70a1a6707", 435: "6b88425c63cddea3b83598a2ab654eadf643fbff1996622b93738ef2784ba75a",
    438: "1b14357e0107d3a9372d1b8a6ddfa0cd7e86e8efce7808ea970631d0bd152613", 447: "4af6818fbdd9eb9f9d8d1445a9ac76d4c7d7f9a991368810b57f28cb19b68df2", 470: "3837c476005b7606750a4c8fceb6ad49e67d4e3c1607534ede1dbc4a14dd9b7c", 472: "6ff97f6127b9804fbf40940f353ef481f2e4c384ea13f2979baafcefe7e5b406", 481: "fc471bd633c33c3acc25132391e2831e865227135f8ee4f31497b22c826fd14b", 483: "e8d2f5782df432fc02de579b05ad29cf0a684fdf68b88bc0c668d6eeb13bb575", 484: "ade123d19353fc4141a056f7067661aaebce349a5455ea320e04a36d872cec60",
    485: "264ce92ea12a1371571ccb11f3661e5b320090addc3ca550b49633fadb3db7f2", 487: "4b681275aef5edd48b1e9f50684c7cb4c640b51477268bb4c4c0045f7dc591aa", 529: "0afbf025783b2e118aca9dd71c2b6f2fb75f2b46101c905f595aca78c35931a4",
}

NATIVE_HEADER = ROOT / "native/core/ctor_release_family_slots.hpp"
NATIVE_SOURCE = ROOT / "native/core/ctor_release_family_slots.cpp"
NATIVE_TEST = ROOT / "native/tests/test_ctor_release_family_slots.cpp"

NATIVE_CONSTANTS = {
    "kReleaseFamilyDsoHandleRva": DSO_HANDLE,
    "kReleaseFamilyAtexitRva": ATEXIT,
    "kReleaseFamilyF590Rva": F590,
    "kReleaseFamilyFnvSpanRva": FNV_SPAN,
    "kReleaseFamilyFnvCstrRva": FNV_CSTR,
    "kReleaseFamilyFnvBasis": FNV_BASIS,
}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(f"FAIL {message}")


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


# ----------------------------------------------------------------------------------------------
# disassembly + straight-line model
# ----------------------------------------------------------------------------------------------
LINE_RE = re.compile(r"^\s*([0-9a-f]+):\s*(.*?)\s*$")
COMMENT_RE = re.compile(r"#\s*(0x[0-9a-f]+)")


def disassemble(rva: int, stop: int):
    out = subprocess.run(
        ["xcrun", "llvm-objdump", "-d", "--no-show-raw-insn", f"--start-address={rva:#x}",
         f"--stop-address={stop:#x}", str(ROOT / "local/re/eboot.elf")],
        capture_output=True, text=True, check=True).stdout
    insns = []
    for line in out.splitlines():
        m = LINE_RE.match(line)
        if m is None or not line.startswith("  "):
            continue
        text = re.sub(r"<[^>]*>", "", m.group(2))
        insns.append((int(m.group(1), 16), text.replace("\t", " ").strip()))
    return insns


def reg_name(token: str) -> str:
    return token.strip().lstrip("%")


def canon(reg: str) -> str:
    """Map sub-register names used by these bodies to their 64-bit register."""
    return {"eax": "rax", "ebx": "rbx", "esi": "rsi", "edi": "rdi"}.get(reg, reg)


MEM_RE = re.compile(r"^(-?0x[0-9a-f]+|-?\d+)?\((%\w+)(?:,[^)]*)?\)$")


class Model:
    def __init__(self):
        self.regs: dict[str, object] = {}
        self.xmm: dict[str, object] = {}
        self.mem: dict[tuple, tuple] = {}     # ('abs', a) / ('stk', off) -> (width, value)
        self.events: list[tuple] = []
        self.hash_count = 0

    # -- operand helpers
    def mem_key(self, operand: str, comment: int | None):
        if comment is not None and "(%rip)" in operand:
            return ("abs", comment)
        m = MEM_RE.match(operand)
        require(m is not None, f"unparsed memory operand {operand!r}")
        disp = int(m.group(1), 0) if m.group(1) else 0
        base = reg_name(m.group(2))
        if base == "rbp":
            return ("stk", disp)
        value = self.regs.get(base)
        require(isinstance(value, int), f"memory base {base} is not a known constant ({operand})")
        return ("abs", value + disp)

    def value_of(self, operand: str):
        operand = operand.strip()
        if operand.startswith("$"):
            return int(operand[1:], 0)
        return self.regs.get(canon(reg_name(operand)))

    def store(self, key, width, value):
        self.mem[key] = (width, value)
        if key[0] == "abs" and width == 4:
            self.events.append(("dstore", key[1], value))

    def read_q(self, space: str, addr: int):
        """Value of the qword at (space, addr) from exact 8 B stores or covering 16 B stores."""
        for (sp, base), (width, value) in self.mem.items():
            if sp != space or not (base <= addr < base + width):
                continue
            if width == 8 and base == addr:
                return value
            if width == 16 and isinstance(value, tuple):
                if value == ("zero",):
                    return 0
                if value[0] == "pair" and (addr - base) in (0, 8):
                    return value[1 + (addr - base) // 8]
        raise SystemExit(f"FAIL no qword store covers {space}:{addr:#x}")

    def feed(self, addr: int, text: str):
        mnem, _, rest = text.partition(" ")
        comment = None
        cm = COMMENT_RE.search(rest)
        if cm:
            comment = int(cm.group(1), 16)
        if "#" in rest:
            rest = rest[: rest.index("#")].strip()
        ops = [o.strip() for o in re.split(r",\s*(?![^()]*\))", rest)] if rest else []
        if mnem in ("pushq", "popq", "retq", "jne", "cmpq", "subq"):
            return
        if mnem == "addq":
            src, dst = ops
            dstr = canon(reg_name(dst))
            if dstr == "rsp":
                return
            cur = self.regs.get(dstr)
            require(isinstance(cur, int) and src.startswith("$"), f"addq on unknown reg at {addr:#x}")
            self.regs[dstr] = cur + int(src[1:], 0)
            return
        if mnem == "leaq":
            src, dst = ops
            dstr = canon(reg_name(dst))
            if "(%rip)" in src:
                require(comment is not None, f"rip lea without target at {addr:#x}")
                self.regs[dstr] = comment
            else:
                m = MEM_RE.match(src)
                require(m is not None and reg_name(m.group(2)) == "rbp", f"lea form at {addr:#x}")
                self.regs[dstr] = ("stackaddr", int(m.group(1), 0))
            return
        if mnem == "movq" or mnem == "movl":
            src, dst = ops
            if src.startswith("$"):
                imm = int(src[1:], 0)
                if dst.startswith("%"):
                    self.regs[canon(reg_name(dst))] = imm
                else:
                    self.store(self.mem_key(dst, comment), 8 if mnem == "movq" else 4, imm)
                return
            if src.startswith("%") and dst.startswith("%"):
                self.regs[canon(reg_name(dst))] = self.regs.get(canon(reg_name(src)))
                return
            if src.startswith("%"):
                width = 8 if mnem == "movq" else 4
                self.store(self.mem_key(dst, comment), width, self.regs.get(canon(reg_name(src))))
                return
            # load
            if comment == CANARY_GOT and "(%rip)" in src:
                self.regs[canon(reg_name(dst))] = ("canary_got", comment)
                return
            if src.startswith("(") and canon(reg_name(dst)) == "rax":
                self.regs["rax"] = ("canary", 0)
                return
            raise SystemExit(f"FAIL unsupported load at {addr:#x}: {text}")
        if mnem == "vxorps":
            self.xmm[reg_name(ops[2])] = ("zero",)
            return
        if mnem == "vmovq":
            src, dst = ops
            require(dst.startswith("%xmm"), f"vmovq form at {addr:#x}")
            self.xmm[reg_name(dst)] = ("lo", self.regs.get(canon(reg_name(src))))
            return
        if mnem == "vpunpcklqdq":
            a, b, dst = ops
            lo = self.xmm.get(reg_name(b))
            hi = self.xmm.get(reg_name(a))
            require(lo and hi and lo[0] == "lo" and hi[0] == "lo", f"punpck inputs at {addr:#x}")
            # at&t order: vpunpcklqdq src2, src1, dst -> dst = [src1.lo, src2.lo]
            self.xmm[reg_name(dst)] = ("pair", lo[1], hi[1])
            return
        if mnem in ("vmovdqu", "vmovups"):
            src, dst = ops
            require(src.startswith("%xmm") and not dst.startswith("%"), f"xmm store form at {addr:#x}")
            self.store(self.mem_key(dst, comment), 16, self.xmm.get(reg_name(src)))
            return
        if mnem == "callq":
            target = int(rest.split()[0], 16)
            require(target in ALLOWED_CALLS, f"unexpected call target {target:#x} at {addr:#x}")
            self.on_call(addr, target)
            return
        raise SystemExit(f"FAIL unsupported instruction at {addr:#x}: {text}")

    def holder_at(self, ptr):
        require(isinstance(ptr, tuple) and ptr[0] == "stackaddr", "release holder must be a stack view")
        base = ptr[1]
        data = self.read_q("stk", base)
        size = self.read_q("stk", base + 8)
        require(self.read_q("stk", base + 0x10) == 0, "stack view aux must be 0")
        require(self.read_q("stk", base + 0x18) == 0, "stack view control must be null (inert f590)")
        return data, size

    def on_call(self, addr, target):
        rdi = self.regs.get("rdi")
        if target == STACK_CHK:
            self.events.append(("stack_chk",))
        elif target == F590:
            data, size = self.holder_at(rdi)
            self.events.append(("release", data, size))   # ctl/aux are the 16 B zero store
        elif target == ATEXIT:
            self.events.append(("atexit", rdi, self.regs.get("rsi"), self.regs.get("rdx")))
        elif target == FNV_SPAN:
            data, size = self.holder_at(rdi)
            seed = self.regs.get("rsi")
            self.regs["rax"] = ("hash", self.hash_count)
            self.events.append(("fnv_span", self.hash_count, data, size, seed))
            self.hash_count += 1
        elif target == FNV_CSTR:
            self.regs["rax"] = ("hash", self.hash_count)
            self.events.append(("fnv_cstr", self.hash_count, rdi))
            self.hash_count += 1


def derive(slot: int, rva: int, size: int):
    insns = disassemble(rva, rva + size + 16)
    starts = [a for a, _ in insns]
    require(starts[0] == rva, f"slot {slot}: first instruction")
    require(rva + size in starts, f"slot {slot}: size {size} must end on an instruction boundary")
    body = [(a, t) for a, t in insns if a < rva + size]
    model = Model()
    for addr, text in body:
        model.feed(addr, text)
    last_addr, last_text = body[-1]
    require(last_text.startswith("callq 0xf8f3a0") or last_text == "retq",
            f"slot {slot}: last instruction must be __stack_chk_fail or ret, found {last_text!r}")
    return model


def read_cstr(img, rva: int) -> bytes:
    out = bytearray()
    while True:
        b = img.read(rva + len(out), 1)[0]
        if b == 0:
            return bytes(out)
        out.append(b)


def derive_steps(img, slot: int, kind: str, model: Model):
    """Convert the straight-line events into the construction steps of the native table."""
    steps = []
    events = [e for e in model.events if e[0] != "stack_chk"]
    if kind == "name":
        body_events = [e for e in events if e[0] in ("release", "atexit")]
        require(len(body_events) % 2 == 0, f"slot {slot}: release/atexit pairs")
        for i in range(0, len(body_events), 2):
            ev, at = body_events[i], body_events[i + 1]
            require(ev[0] == "release" and at[0] == "atexit", f"slot {slot}: release then atexit")
            _, data, length = ev
            _, dtor, glob, dso = at
            require(dso == DSO_HANDLE, f"slot {slot}: atexit dso handle {dso}")
            require(isinstance(glob, int) and isinstance(dtor, int), f"slot {slot}: atexit operands")
            vptr = model.read_q("abs", glob)
            require(isinstance(vptr, int), f"slot {slot}: vptr store at {glob:#x}")
            require(model.read_q("abs", glob + 8) == data, f"slot {slot}: name data @+8")
            require(model.read_q("abs", glob + 0x10) == length, f"slot {slot}: name len @+0x10")
            for off in (0x18, 0x20, 0x28):
                require(model.read_q("abs", glob + off) == 0, f"slot {slot}: zero qword @+{off:#x}")
            text = read_cstr(img, data)
            require(len(text) == length, f"slot {slot}: string length {length} vs strlen {len(text)}")
            steps.append(("name_object", glob, vptr, text, length, dtor, 0, data))
        stores = [e for e in events if e[0] == "dstore"]
        require(not stores, f"slot {slot}: unexpected dword stores")
    else:
        pending = {}
        n_calls = n_rel = 0
        for ev in events:
            if ev[0] == "fnv_span":
                require(kind == "span", f"slot {slot}: span hash in cstr slot")
                _, n, data, length, seed = ev
                require(seed == FNV_BASIS, f"slot {slot}: span seed {seed}")
                pending[n] = ("span_hash", read_cstr(img, data)[:length], length, seed, data)
                n_calls += 1
            elif ev[0] == "fnv_cstr":
                require(kind == "cstr", f"slot {slot}: cstr hash in span slot")
                _, n, rdi = ev
                require(isinstance(rdi, int), f"slot {slot}: cstr operand")
                text = read_cstr(img, rdi)
                pending[n] = ("cstr_hash", text, len(text), FNV_BASIS, rdi)
                n_calls += 1
            elif ev[0] == "release":
                n_rel += 1
            elif ev[0] == "dstore":
                _, addr, value = ev
                if isinstance(value, tuple) and value[0] == "hash":
                    k, text, length, seed, trva = pending.pop(value[1])
                    steps.append((k, addr, 0, text, length, 0, seed, trva))
                else:
                    require(value == 0, f"slot {slot}: constant dword {value}")
                    steps.append(("const_zero", addr, 0, b"", 0, 0, 0, 0))
            else:
                raise SystemExit(f"FAIL slot {slot}: unexpected event {ev[0]}")
        require(not pending, f"slot {slot}: hash result never stored")
        if kind == "span":
            require(n_rel == n_calls, f"slot {slot}: one f590 per hashed stack view")
        else:
            require(n_rel == 0, f"slot {slot}: no f590 in cstr slot")
    return steps


def cpp_string(raw: bytes) -> str:
    out = []
    for b in raw:
        if b in (0x22, 0x5C):
            out.append("\\" + chr(b))
        elif 0x20 <= b < 0x7F:
            out.append(chr(b))
        else:
            out.append(f"\\x{b:02x}\"\"")
    return '"' + "".join(out) + '"'


def native_rows(header_text: str):
    m = re.search(r"kReleaseFamilySteps\{\{(.*?)\n\}\};", header_text, re.DOTALL)
    require(m is not None, "cannot find kReleaseFamilySteps")
    row_re = re.compile(
        r"\{ReleaseFamilyKind::(\w+),\s*(\d+),\s*(0x[0-9a-fA-F]+),\s*(0x[0-9a-fA-F]+),\s*"
        r"(\"(?:[^\"\\]|\\.)*\"),\s*(\d+),\s*(0x[0-9a-fA-F]+),\s*(0x[0-9a-fA-F]+),\s*"
        r"(0x[0-9a-fA-F]+)\}")
    rows = []
    for r in row_re.finditer(m.group(1)):
        text = r.group(5)[1:-1].encode().decode("unicode_escape").encode("latin-1")
        rows.append((r.group(1), int(r.group(2)), int(r.group(3), 16), int(r.group(4), 16), text,
                     int(r.group(6)), int(r.group(7), 16), int(r.group(8), 16), int(r.group(9), 16)))
    return rows


def native_slot_rows(header_text: str):
    m = re.search(r"kReleaseFamilySlots\{\{(.*?)\n\}\};", header_text, re.DOTALL)
    require(m is not None, "cannot find kReleaseFamilySlots")
    row_re = re.compile(r"\{(\d+),\s*(0x[0-9a-fA-F]+),\s*(\d+),\s*(\d+),\s*(\d+)\}")
    return [tuple(int(x, 0) for x in r.groups()) for r in row_re.finditer(m.group(1))]


def collect(elf: bytes):
    _, phs = d.program_headers(elf)
    img = d.Image(elf, phs)
    per_slot = {}
    for slot, (rva, size, kind, _) in sorted(SLOTS.items()):
        word = struct.unpack_from("<Q", elf, TABLE_BASE + (FIRST_CTOR_SLOT - slot) * 8)[0]
        require(word == rva, f"slot {slot}: constructor table word {word:#x} != {rva:#x}")
        body = img.read(rva, size)
        model = derive(slot, rva, size)
        steps = derive_steps(img, slot, kind, model)
        per_slot[slot] = (body, steps)
    return img, per_slot


def main() -> int:
    argv = sys.argv[1:]
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(digest(elf) == ELF_SHA256, "unexpected ELF SHA256")
    img, per_slot = collect(elf)

    if "--dump" in argv:
        for slot, (body, steps) in sorted(per_slot.items()):
            print(f"// slot {slot} rva {SLOTS[slot][0]:#x} size {SLOTS[slot][1]} sha {digest(body)}")
            for s in steps:
                print("  ", f"{{ReleaseFamilyKind::{s[0]}, {slot}, {hex(s[1])}, {hex(s[2])}, {cpp_string(s[3])}, {s[4]}, {hex(s[5])}, {hex(s[6])}, {hex(s[7])}}},")
        return 0

    # body hashes
    for slot, (body, steps) in per_slot.items():
        pinned = BODY_SHA[slot]
        require(digest(body) == pinned, f"slot {slot}: body sha256 {digest(body)} != pinned {pinned}")

    header = NATIVE_HEADER.read_text()
    if "--tamper" in argv:
        # alter one global RVA digit in the first step row: a correct checker must now fail
        header = re.sub(r"(\{ReleaseFamilyKind::\w+,\s*\d+,\s*0x)([0-9a-fA-F])", r"\g<1>f", header, count=1)
    for name, value in NATIVE_CONSTANTS.items():
        m = re.search(rf"\b{name}\s*=\s*(0x[0-9a-fA-F]+|\d+)", header)
        require(m is not None and int(m.group(1), 0) == value, f"native constant {name}")

    rows = native_rows(header)
    slot_rows = native_slot_rows(header)
    require(len(slot_rows) == len(SLOTS), f"slot row count {len(slot_rows)} != {len(SLOTS)}")
    cursor = 0
    derived_all = []
    for (slot, rva, size, first, count), expected_slot in zip(slot_rows, sorted(SLOTS)):
        require(slot == expected_slot, f"slot row order: {slot} vs {expected_slot}")
        er, es, ek, _ = SLOTS[slot]
        require(rva == er and size == es, f"slot {slot}: native rva/size")
        _, steps = per_slot[slot]
        require(first == cursor and count == len(steps),
                f"slot {slot}: native step range {first}+{count} vs derived {len(steps)}")
        cursor += count
        for step in steps:
            derived_all.append((slot, step))
    require(len(rows) == cursor, f"native step rows {len(rows)} != derived {cursor}")
    for i, (row, (slot, step)) in enumerate(zip(rows, derived_all)):
        kind, rslot, glob, aux, text, length, dtor, seed, trva = row
        exp = (step[0], slot, step[1], step[2], step[3], step[4], step[5], step[6], step[7])
        got = (kind, rslot, glob, aux, text, length, dtor, seed, trva)
        require(got == exp, f"native step row #{i} diverges from the ELF derivation: {got} vs {exp}")

    # every written global is a mapped, non-file-backed (BSS) cell and no two steps share a cell
    seen = set()
    for slot, step in derived_all:
        width = 0x30 if step[0] == "name_object" else 4
        require(img.mapped(step[1], width), f"slot {slot}: global {step[1]:#x} must be mapped")
        try:
            img.read(step[1], width)
            raise SystemExit(f"FAIL slot {slot}: global {step[1]:#x} must be BSS (not file-backed)")
        except ValueError:
            pass
        require(step[1] not in seen, f"slot {slot}: duplicate global {step[1]:#x}")
        seen.add(step[1])

    source = NATIVE_SOURCE.read_text()
    for token in ("init_release_family_slot", "release_family_stage_name"):
        require(token in header + source, f"native entry point {token}")
    require(NATIVE_TEST.is_file(), "missing test fixture")
    cmake = (ROOT / "native/CMakeLists.txt").read_text()
    require("core/ctor_release_family_slots.cpp" in cmake, "CMake source registration")
    require("add_test(NAME ctor_release_family_slots COMMAND test_ctor_release_family_slots)" in cmake,
            "CMake test registration")

    n_obj = sum(1 for _, s in derived_all if s[0] == "name_object")
    n_span = sum(1 for _, s in derived_all if s[0] == "span_hash")
    n_cstr = sum(1 for _, s in derived_all if s[0] in ("cstr_hash", "const_zero"))
    print(f"OK check_ctor_release_family_slots: ELF sha ok; {len(SLOTS)} table words + body hashes "
          f"pinned; {len(rows)} derived steps ({n_obj} name objects, {n_span} span hashes, "
          f"{n_cstr} cstr/const hashes) equal the native table row by row")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
