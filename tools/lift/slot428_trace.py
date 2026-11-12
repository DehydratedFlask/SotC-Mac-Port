"""Byte-level symbolic tracer and block parser for startup slot 428 (RVA 0x52ec10, 5375 B; batch 19).

Independent of objdump for the SEMANTICS: it decodes the raw instruction BYTES (read through `raw(addr, n)`, so an `Elf.overlay` perturbs the decode) of the
straight-line body 0x52ec21..0x5300ea with a closed x86-64 decoder and replays every instruction symbolically. Registers hold ('c', n) constants, ('a', addr)
absolute addresses (.rodata strings, the static objects, code identity tags), ('s', off) rbp-relative stack addresses, ('h', addr) = the result of the
path-folding hash 0xa2ddc0 over the C string at addr, ('got', addr) the stack-guard GOT slot value and ('canary',). xmm registers hold two such 64-bit lanes
(a vector load from .rodata yields two constants read from the ELF). Static memory remembers exact-width stores so the one forwarded load in the slot
(`mov edi, [0x2efd610]`) is resolved; stack memory remembers the string views the callees receive. Calls become events with their register arguments (and the
stack views they point at); nothing is executed, no call is followed, no memory is read except .rodata vector constants and the stack-guard slot.

The decoder knows EXACTLY the encodings of this body (REX 89/8b/8d/c7/31/b8+r/e8, VEX.128 vmovups/vxorps) and nothing else; any other byte sequence raises
TraceRefused (nothing is skipped or guessed). `parse428` then folds the event list into the five phases of the slot and refuses any deviation.
"""
from __future__ import annotations

import struct

M64 = (1 << 64) - 1
START, END = 0x52EC21, 0x5300EA            # after `sub rsp, 0x48` / before the stack-guard epilogue (pinned separately as bytes)
GUARD_GOT = 0x15FE518
DSO = 0x1604218
ATEXIT, FNV, F590, ID_CTOR, AVAIL_CTOR = 0xF8F390, 0xA2DDC0, 0xF590, 0xA59620, 0xA42960


class TraceRefused(Exception):
    pass


def _s8(b: int) -> int:
    return b - 256 if b >= 128 else b


def _s32(b: bytes) -> int:
    return struct.unpack("<i", b)[0]


class Tracer:
    def __init__(self, raw):
        self.raw = raw
        self.g: dict[int, tuple] = {5: ("s", 0)}     # rbp is the frame pointer
        self.x: dict[int, tuple] = {}
        self.static: dict[int, tuple] = {}
        self.stack: dict[int, tuple] = {}
        self.events: list[dict] = []
        self.insns: list[tuple[int, int, str]] = []
        self.rodata_reads: list[tuple[int, int]] = []

    # ---- operand helpers
    def _modrm(self, code, p, at, R, B):
        """(reg, rm_or_mem, extra_len) with extra_len = bytes after the modrm byte (disp). rm_or_mem is ('r', n) or ('m', kind, a, b)."""
        m = code[p]
        mod, reg, rm = m >> 6, ((m >> 3) & 7) | (R << 3), m & 7
        if mod == 3:
            return reg, ("r", rm | (B << 3)), 0
        if rm == 4:
            raise TraceRefused(f"SIB byte not supported @{at:#x}")
        if mod == 0 and rm == 5:
            return reg, ("m", "rip", _s32(code[p + 1:p + 5]), 0), 4
        base = rm | (B << 3)
        if mod == 0:
            return reg, ("m", "reg", base, 0), 0
        if mod == 1:
            return reg, ("m", "reg", base, _s8(code[p + 1])), 1
        return reg, ("m", "reg", base, _s32(code[p + 1:p + 5])), 4

    def _ea(self, mem, end, at):
        """Effective address value of a memory operand; `end` = address of the NEXT instruction (rip base)."""
        if mem[1] == "rip":
            return ("a", end + mem[2])
        v = self.g.get(mem[2])
        if v is not None and v[0] == "got" and mem[3] == 0:
            return v
        if v is None or v[0] not in ("a", "s"):
            raise TraceRefused(f"memory operand through an untracked register {mem[2]} @{at:#x}")
        return (v[0], v[1] + mem[3])

    def _val(self, r, at, width):
        v = self.g.get(r)
        if v is None:
            raise TraceRefused(f"read of an undefined register {r} @{at:#x}")
        if width == 4:
            if v[0] == "c":
                return ("c", v[1] & 0xFFFFFFFF)
            if v[0] == "h":
                return v
            raise TraceRefused(f"32-bit read of a non-constant register @{at:#x}")
        return v

    def _store(self, at, ea, width, lanes):
        if ea[0] == "a":
            self.static[ea[1]] = (width, lanes)
            self.events.append(dict(kind="store", insn=at, target=ea[1], width=width, lanes=lanes))
        elif ea[0] == "s":
            self.stack[ea[1]] = (width, lanes)
        else:
            raise TraceRefused(f"store through an unsupported address @{at:#x}")

    def _load(self, at, ea, width):
        if ea[0] == "a" and ea[1] == GUARD_GOT and width == 8:
            return ("got", GUARD_GOT)
        if ea[0] == "got":
            return ("canary",)
        if ea[0] == "a" and ea[1] in self.static:
            w, lanes = self.static[ea[1]]
            if w == width and len(lanes) == 1:
                return lanes[0]
        raise TraceRefused(f"load that is not a forwarded static store @{at:#x}")

    def _xv(self, r, at):
        v = self.x.get(r)
        if v is None:
            raise TraceRefused(f"xmm{r} undefined @{at:#x}")
        return v

    # ---- one instruction; returns its length
    def step(self, at: int) -> int:
        code = self.raw(at, 16)
        p = 0
        rex = 0
        if 0x40 <= code[0] <= 0x4F:
            rex, p = code[0], 1
        W, R, X, B = (rex >> 3) & 1, (rex >> 2) & 1, (rex >> 1) & 1, rex & 1
        if X:
            raise TraceRefused(f"REX.X not supported @{at:#x}")
        op = code[p]
        if code[0] == 0xC5:
            return self._vex(at, code)
        if op in (0x89, 0x8B, 0x8D):
            reg, rm, extra = self._modrm(code, p + 1, at, R, B)
            ln = p + 2 + extra
            if rm[0] == "r":
                if op != 0x89 or not W:
                    raise TraceRefused(f"register form not supported @{at:#x}")
                self.g[rm[1]] = self._val(reg, at, 8)
                self.insns.append((at, ln, "movq"))
                return ln
            ea = self._ea(rm, at + ln, at)
            if op == 0x89:
                self._store(at, ea, 8 if W else 4, (self._val(reg, at, 8 if W else 4),))
                self.insns.append((at, ln, "movq" if W else "movl"))
            elif op == 0x8B:
                self.g[reg] = self._load(at, ea, 8 if W else 4)
                self.insns.append((at, ln, "movq" if W else "movl"))
            else:
                if not W:
                    raise TraceRefused(f"32-bit lea @{at:#x}")
                self.g[reg] = ea
                self.insns.append((at, ln, "leaq"))
            return ln
        if op == 0xC7:
            reg, rm, extra = self._modrm(code, p + 1, at, R, B)
            if (reg & 7) != 0 or rm[0] != "m":
                raise TraceRefused(f"c7 form not supported @{at:#x}")
            ln = p + 2 + extra + 4
            imm = _s32(code[p + 2 + extra:p + 6 + extra])
            ea = self._ea(rm, at + ln, at)
            self._store(at, ea, 8 if W else 4, (("c", imm & M64) if W else ("c", imm & 0xFFFFFFFF),))
            self.insns.append((at, ln, "movq" if W else "movl"))
            return ln
        if 0xB8 <= op <= 0xBF:
            r = (op - 0xB8) | (B << 3)
            if W:
                self.g[r] = ("c", struct.unpack("<Q", code[p + 1:p + 9])[0])
                self.insns.append((at, p + 9, "movabsq"))
                return p + 9
            self.g[r] = ("c", struct.unpack("<I", code[p + 1:p + 5])[0])
            self.insns.append((at, p + 5, "movl"))
            return p + 5
        if op == 0x31 and not rex:
            m = code[p + 1]
            if (m >> 6) != 3 or ((m >> 3) & 7) != (m & 7):
                raise TraceRefused(f"xor of two different registers @{at:#x}")
            self.g[m & 7] = ("c", 0)
            self.insns.append((at, 2, "xorl"))
            return 2
        if op == 0xE8 and not rex:
            target = at + 5 + _s32(code[1:5])
            self._call(at, target)
            self.insns.append((at, 5, "callq"))
            return 5
        raise TraceRefused(f"instruction class not supported: {code[:8].hex()} @{at:#x}")

    def _vex(self, at, code):
        b1 = code[1]
        Rv, vvvv, L, pp, opc = 1 - (b1 >> 7), (~(b1 >> 3)) & 0xF, (b1 >> 2) & 1, b1 & 3, code[2]
        if L or pp != 0:
            raise TraceRefused(f"VEX form not supported @{at:#x}")
        m = code[3]
        mod, reg = m >> 6, ((m >> 3) & 7) | (Rv << 3)
        if opc == 0x57:                                        # vxorps xmm, xmm, xmm of one register with itself
            if mod != 3 or not (reg == vvvv == (m & 7)):
                raise TraceRefused(f"vxorps of different registers @{at:#x}")
            self.x[reg] = (("c", 0), ("c", 0))
            self.insns.append((at, 4, "vxorps"))
            return 4
        if opc in (0x10, 0x11) and mod != 3:
            _, rm, extra = self._modrm(code, 3, at, 0, 0)
            ln = 4 + extra
            ea = self._ea(rm, at + ln, at)
            if opc == 0x10:                                    # load from .rodata: both lanes are CONSTANTS read from the image
                if ea[0] != "a":
                    raise TraceRefused(f"vector load from a non-address @{at:#x}")
                data = self.raw(ea[1], 16)
                self.rodata_reads.append((ea[1], 16))
                self.x[reg] = (("c", struct.unpack("<Q", data[:8])[0]), ("c", struct.unpack("<Q", data[8:])[0]))
            else:
                lo, hi = self._xv(reg, at)
                self._store(at, ea, 16, (lo, hi))
            self.insns.append((at, ln, "vmovups"))
            return ln
        raise TraceRefused(f"VEX opcode {opc:02x} not supported @{at:#x}")

    def _view(self, v):
        """The 32 B string view {data, size, aux, ctl} a stack address points at, or None."""
        if v is None or v[0] != "s":
            return None
        o = v[1]
        a, b, c = self.stack.get(o), self.stack.get(o + 8), self.stack.get(o + 16)
        if not (a and b and c and a[0] == 8 and b[0] == 8 and c[0] == 16):
            raise TraceRefused(f"stack view at {o:#x} is not three stores (8, 8, 16)")
        return (a[1][0], b[1][0], c[1][0], c[1][1])

    def _call(self, at, target):
        args = {n: self.g.get(r) for n, r in (("rdi", 7), ("rsi", 6), ("rdx", 2))}
        ev = dict(kind="call", insn=at, target=target, rdi=args["rdi"], rsi=args["rsi"], rdx=args["rdx"])
        if target in (ID_CTOR, AVAIL_CTOR, F590):
            ev["views"] = {n: self._view(args[n]) for n in ("rdi", "rsi", "rdx") if args[n] is not None and args[n][0] == "s"}
        self.events.append(ev)
        if target == F590 and args["rdi"] is not None and args["rdi"][0] == "s":
            # the release consumes the stack view: its three stores are dead, so a later view at the same slot must be rebuilt by NEW stores
            # (a stale entry would otherwise mask a missing or displaced store)
            o = args["rdi"][1]
            for k in (o, o + 8, o + 16):
                self.stack.pop(k, None)
        for r in (0, 1, 2, 6, 7, 8, 9, 10, 11):
            self.g.pop(r, None)
        if target == FNV:
            if args["rdi"] is None or args["rdi"][0] != "a":
                raise TraceRefused(f"hash of a non-address @{at:#x}")
            self.g[0] = ("h", args["rdi"][1])

    def run(self, start=START, end=END):
        a = start
        while a < end:
            a += self.step(a)
        if a != end:
            raise TraceRefused(f"trace does not end on an instruction boundary: {a:#x} vs {end:#x}")
        return self


# ------------------------------------------------------------------------------------------------------------------------------------
# block parser
# ------------------------------------------------------------------------------------------------------------------------------------
REC_OFFS = (0x00, 0x08, 0x10, 0x18, 0x28, 0x2C, 0x30, 0x34, 0x38, 0x40, 0x50)
REC_WIDTHS = (4, 8, 8, 16, 4, 4, 4, 4, 4, 16, 8)


def _fail(msg):
    raise SystemExit(f"FAIL slot 428 block parse: {msg}")


def _want(cond, msg):
    if not cond:
        _fail(msg)


def parse428(raw):
    """Fold the traced events into the phases. Returns a dict with the static store list and the typed blocks (all from the ELF bytes)."""
    try:
        t = Tracer(raw).run()
    except TraceRefused as err:
        raise SystemExit(f"FAIL slot 428 trace refused: {err}")
    ev = t.events
    i = 0
    stores = []
    while ev[i]["kind"] == "store":
        stores.append(ev[i])
        i += 1

    def atexit(dtor):
        nonlocal i
        c = ev[i]
        _want(c["kind"] == "call" and c["target"] == ATEXIT, f"event {i}: expected __cxa_atexit")
        _want(c["rdi"] == ("a", dtor) and c["rdx"] == ("a", DSO), f"event {i}: atexit arguments (dtor {dtor:#x}, dso)")
        i += 1
        return c["rsi"]

    first_obj = atexit(0x52E980)
    _want(first_obj == ("c", 0), "first atexit object is null")
    out = dict(trace=t, stores=stores, atexit=[(0x52E980, None)], recs=[], ids=[], avails=[])
    # phase B: five 0x58 B records
    for _ in range(5):
        block = []
        while not (ev[i]["kind"] == "call" and ev[i]["target"] == ATEXIT):
            block.append(ev[i])
            i += 1
        obj = atexit(0x52EAA0)
        _want(obj is not None and obj[0] == "a", "record atexit object is an address")
        base = obj[1]
        sts = [e for e in block if e["kind"] == "store"]
        calls = [e for e in block if e["kind"] == "call"]
        _want(len(sts) == len(REC_OFFS) and len(calls) == 4 and all(c["target"] == FNV for c in calls), "record block shape (11 stores, 4 hash calls)")
        by = {s["target"] - base: s for s in sts}
        _want(sorted(by) == list(REC_OFFS) and [by[o]["width"] for o in REC_OFFS] == list(REC_WIDTHS), "record store offsets / widths")
        hashes = []
        for k, o in enumerate((0x28, 0x2C, 0x30, 0x34)):
            lane = by[o]["lanes"][0]
            _want(lane[0] == "h" and lane == ("h", calls[k]["rdi"][1]), f"record hash {k} stores the result of call {k}")
            hashes.append(lane[1])
        name = by[0x08]["lanes"][0]
        _want(by[0x00]["lanes"][0][0] == "c" and name[0] == "a" and by[0x10]["lanes"][0][0] == "c" and by[0x38]["lanes"][0][0] == "c", "record id / name / length / flag lanes")
        _want(by[0x18]["lanes"] == (("c", 0), ("c", 0)) and by[0x40]["lanes"] == (("c", 0), ("c", 0)) and by[0x50]["lanes"] == (("c", 0),), "record zero fields")
        out["recs"].append(dict(addr=base, id=by[0x00]["lanes"][0][1], name=name[1], name_len=by[0x10]["lanes"][0][1], flag=by[0x38]["lanes"][0][1], hashes=hashes))
        out["atexit"].append((0x52EAA0, base))
    # phase C: 37 id entries
    while ev[i]["kind"] == "call" and ev[i]["target"] == ID_CTOR:
        c = ev[i]
        _want(c["rdi"][0] == "a" and c["rsi"][0] == "c" and c["rdx"][0] == "s", "id entry call arguments")
        view = c["views"]["rdx"]
        _want(view[0][0] == "a" and view[1][0] == "c" and view[2] == ("c", 0) and view[3] == ("c", 0), "id entry source is a borrowed view {ptr, len, 0, 0}")
        r = ev[i + 1]
        _want(r["kind"] == "call" and r["target"] == F590 and r["rdi"] == c["rdx"], "id entry source released through 0xf590")
        i += 2
        obj = atexit(0x52EB00)
        _want(obj == c["rdi"], "id entry atexit object is the entry")
        out["ids"].append(dict(addr=c["rdi"][1], id=c["rsi"][1], text=view[0][1], text_len=view[1][1]))
        out["atexit"].append((0x52EB00, c["rdi"][1]))
    # phase D: availability entries
    while i < len(ev) and ev[i]["kind"] == "call" and ev[i]["target"] == AVAIL_CTOR:
        c = ev[i]
        _want(c["rdi"][0] == "a" and c["rsi"][0] == "s" and c["rdx"][0] == "s", "availability entry call arguments")
        va, vb = c["views"]["rsi"], c["views"]["rdx"]
        for v in (va, vb):
            _want(v[0][0] == "a" and v[1][0] == "c" and v[2] == ("c", 0) and v[3] == ("c", 0), "availability source is a borrowed view")
        r1, r2 = ev[i + 1], ev[i + 2]
        _want(r1["target"] == F590 and r1["rdi"] == c["rdx"] and r2["target"] == F590 and r2["rdi"] == c["rsi"], "availability views released (b, then a)")
        i += 3
        obj = atexit(0x52EB10)
        _want(obj == c["rdi"], "availability atexit object is the entry")
        out["avails"].append(dict(addr=c["rdi"][1], a=va[0][1], a_len=va[1][1], b=vb[0][1], b_len=vb[1][1]))
        out["atexit"].append((0x52EB10, c["rdi"][1]))
    _want(i == len(ev), "no events after the last availability entry")
    _want(len(out["recs"]) == 5 and len(out["ids"]) == 37 and len(out["avails"]) == 6, "block counts 5 / 37 / 6")
    _want(len(out["atexit"]) == 49, "49 __cxa_atexit calls")
    return out
