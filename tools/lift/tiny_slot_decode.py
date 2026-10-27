#!/usr/bin/env python3
"""Minimal straight-line x86-64 interpreter for the tiny startup-constructor bodies (lane L1-TINY).

Offline only: it reads bytes from the pinned private ELF through scripts/ps4_dynlink.Image and never
loads, relocates or executes anything. It supports exactly the opcodes that occur in the tiny
constructor bodies checked by check_ctor_tiny_atexit_slots.py / check_ctor_tiny_misc_slots.py and
FAILS on any other byte, so a changed body cannot be silently misread.

`run(img, rva, size)` returns a list of events:
  ('store', dest, size, value)   dest: int address | ('ret', callee, offset); size 1/4/8/16
  ('xchg',  dest, size, value)   value is what the register held before the exchange
  ('call',  target, args)        args = (rdi, rsi, rdx, rcx); target int
  ('tail',  target, args)        a final jmp rel32
  ('ret',)
Register values are ints, None (unknown/clobbered) or ('ret', callee) for a call result.
"""

from __future__ import annotations

import struct

REG = ["rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi",
       "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"]
CLOBBERED = ("rax", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11")


class DecodeError(Exception):
    pass


def _s32(b: bytes, at: int) -> int:
    return struct.unpack("<i", b[at:at + 4])[0]


def run(img, rva: int, size: int):
    body = img.read(rva, size)
    regs = {name: None for name in REG}
    xmm0 = None  # None unknown, 0 zero, ('q', v) low qword only, ('dup', v) both qwords
    events = []
    i = 0
    n = len(body)

    def fail(msg):
        raise DecodeError(f"{rva:#x}+{i:#x}: {msg} (bytes {body[i:i + 8].hex()})")

    def resolve(mem):
        kind = mem[0]
        if kind == "abs":
            return mem[1]
        _, reg, off = mem
        v = regs[reg]
        if isinstance(v, int):
            return v + off
        if isinstance(v, tuple) and v[0] == "ret":
            return ("ret", v[1], off)
        fail(f"unresolved base register {reg}")

    def modrm_mem(pos, rex, tail_len):
        """Decode a memory modrm at body[pos]; returns (reg_field, mem, next_pos). rip-relative
        targets are relative to the end of the instruction, which is pos + disp + tail_len."""
        m = body[pos]
        mod, reg, rm = m >> 6, (m >> 3) & 7, m & 7
        reg |= 8 if rex & 4 else 0
        rmx = rm | (8 if rex & 1 else 0)
        pos += 1
        if mod == 3:
            return reg, ("reg", REG[rmx]), pos
        if rm == 4:
            fail("SIB addressing not supported")
        if mod == 0 and rm == 5:
            disp = _s32(body, pos)
            pos += 4
            return reg, ("abs", rva + pos + tail_len + disp), pos
        if mod == 0:
            return reg, ("mem", REG[rmx], 0), pos
        if mod == 1:
            off = struct.unpack("<b", body[pos:pos + 1])[0]
            return reg, ("mem", REG[rmx], off), pos + 1
        fail("disp32 base addressing not supported")

    while i < n:
        rex = 0
        op = body[i]
        if 0x40 <= op <= 0x4F:
            rex = op
            i += 1
            op = body[i]
        w = bool(rex & 8)
        start_i = i

        if 0x50 <= op <= 0x57 or 0x58 <= op <= 0x5F:      # push / pop (frame only)
            i += 1
        elif op == 0x89:                                     # mov r/m, r
            reg, mem, j = modrm_mem(i + 1, rex, 0)
            if mem[0] == "reg":
                regs[mem[1]] = regs[REG[reg]]
            else:
                dest = resolve(mem)
                events.append(("store", dest, 8 if w else 4, regs[REG[reg]]))
            i = j
        elif op == 0x8D:                                     # lea r, [rip+d]
            m = body[i + 1]
            if (m >> 6) != 0 or (m & 7) != 5:
                fail("lea form not supported")
            reg = ((m >> 3) & 7) | (8 if rex & 4 else 0)
            disp = _s32(body, i + 2)
            regs[REG[reg]] = rva + i + 6 + disp
            i += 6
        elif op == 0x31:                                     # xor r32, r32 (self)
            m = body[i + 1]
            if (m >> 6) != 3 or ((m >> 3) & 7) != (m & 7):
                fail("xor form not supported")
            regs[REG[(m & 7) | (8 if rex & 1 else 0)]] = 0
            i += 2
        elif 0xB8 <= op <= 0xBF:                             # mov r32, imm32
            regs[REG[(op - 0xB8) | (8 if rex & 1 else 0)]] = struct.unpack("<I", body[i + 1:i + 5])[0]
            i += 5
        elif op == 0xC7:                                     # mov r/m, imm32
            reg, mem, j = modrm_mem(i + 1, rex, 4)
            if reg != 0:
                fail("c7 /r != 0")
            imm = _s32(body, j)
            dest = resolve(mem)
            events.append(("store", dest, 8 if w else 4, imm & ((1 << (64 if w else 32)) - 1)))
            i = j + 4
        elif op == 0xC6:                                     # mov r/m8, imm8
            reg, mem, j = modrm_mem(i + 1, rex, 1)
            if reg != 0 or mem[0] == "reg":
                fail("c6 form not supported")
            dest = resolve(mem)
            events.append(("store", dest, 1, body[j]))
            i = j + 1
        elif op == 0x87:                                     # xchg r/m, r (memory form)
            reg, mem, j = modrm_mem(i + 1, rex, 0)
            if mem[0] == "reg":
                fail("xchg reg,reg not supported")
            dest = resolve(mem)
            events.append(("xchg", dest, 8 if w else 4, regs[REG[reg]]))
            i = j
        elif op == 0x83:                                     # add rsp, imm8 (frame only)
            if body[i + 1] != 0xC4:
                fail("0x83 form not supported")
            i += 3
        elif op == 0xE8 or op == 0xE9:                       # call / jmp rel32
            target = rva + i + 5 + _s32(body, i + 1)
            args = (regs["rdi"], regs["rsi"], regs["rdx"], regs["rcx"])
            if op == 0xE8:
                events.append(("call", target, args))
                for name in CLOBBERED:
                    regs[name] = None
                regs["rax"] = ("ret", target)
                i += 5
            else:
                events.append(("tail", target, args))
                i += 5
                if i != n:
                    fail("bytes after a tail jmp")
        elif op == 0xC3:
            events.append(("ret",))
            i += 1
            if i != n:
                fail("bytes after ret")
        elif op == 0xC5:                                     # 2-byte VEX
            v1, o = body[i + 1], body[i + 2]
            if (v1, o) == (0xF8, 0x57) and body[i + 3] == 0xC0:      # vxorps xmm0,xmm0,xmm0
                xmm0 = 0
                i += 4
            elif (v1, o) in ((0xF8, 0x11), (0xFA, 0x7F)):            # vmovups / vmovdqu store xmm0
                reg, mem, j = modrm_mem(i + 3, 0, 0)
                if reg != 0 or mem[0] == "reg":
                    fail("vex store form not supported")
                dest = resolve(mem)
                events.append(("store", dest, 16, xmm0))
                i = j
            elif (v1, o) == (0xF9, 0x70) and body[i + 3] == 0xC0 and body[i + 4] == 0x44:
                if not (isinstance(xmm0, tuple) and xmm0[0] == "q"):
                    fail("vpshufd without vmovq source")
                xmm0 = ("dup", xmm0[1])
                i += 5
            else:
                fail("VEX2 form not supported")
        elif op == 0xC4:                                     # vmovq xmm0, rax
            if body[i:i + 5] != bytes.fromhex("c4e1f96ec0"):
                fail("VEX3 form not supported")
            xmm0 = ("q", regs["rax"])
            i += 5
        else:
            fail(f"opcode {op:#x} not supported")
        if i == start_i:
            fail("no progress")
    return events


def import_name(parsed, img, plt):
    """NID of the import behind a PLT thunk `jmp [rip+d]` (ff 25) via its GOT relocation."""
    thunk = img.read(plt, 6)
    if thunk[:2] != b"\xff\x25":
        return None
    got = plt + 6 + _s32(thunk, 2)
    for r in parsed["relocs"]:
        if r["target"] == got and r["type"] in (1, 6, 7):
            return parsed["symbols"][r["symbol"]]["nid"]
    return None
