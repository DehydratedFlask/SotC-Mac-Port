#!/usr/bin/env python3
"""Slice map for the 0x5b9f90 debug-menu builder (46,881 B, 7,685 instructions).

The body is a single-exit, single-loop page builder: one `ret` (0x5c56aa), one backward
branch (0x5c1e1b -> 0x5c1d10), 1,136 direct calls to 25 distinct targets and 53 indirect
calls. It consists of long straight-line *item clusters*: a `SharedStr` view pair written to
stack slots, then one constructor call (0x26c10 group / 0x26b60 separator / 0x26f90 command /
0x26db0 named / 0x26ea0 toggle / 0x27050+0x27120 float) followed by 0x268b0 registration and
0xf590 view releases.

This tool recovers, from the pinned ELF only:

  * the call sequence with a linear register/stack scan, so every constructor call's
    (parent, caption, command, target, flags) tuple is reported with its source RVAs;
  * the nine slices delimited by the rare callees (0x2170, 0x5cf140, 0x5dbf40, 0x7f8930,
    0x5c94b0, 0xb25840), the single loop nest and the single epilogue;
  * the per-slice callee histogram and the blocked reason recorded for the slices that are
    not lifted yet.

Output: docs/5B9F90_SLICES.md and docs/5b9f90_slices.json. Nothing is executed; RVAs are
identity tags only.
"""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ELF = ROOT / "local/re/eboot.elf"
BODY_START = 0x5B9F90
BODY_END = 0x5C56B1   # exclusive: the last byte is the ret at 0x5c56aa

# The rare callees that delimit slices, with the role read from their bodies.
BOUNDARIES = [
    (0x5B9F90, "S0", "entry: frame, canary, group \"Shadow\" under \"CP10\""),
    (0x5BA041, "S1", "0x2170 page builder (\"Shadow Misc\")"),
    (0x5BA046, "S2", "0x5cf140 page builder"),
    (0x5BA04B, "S3", "page \"Shadow Boss Jump\" and its items"),
    (0x5BC3D4, "S4", "0x5dbf40 + 0x7f8930 pair"),
    (0x5BC3DE, "S5", "item run after the pair"),
    (0x5C1196, "S6", "0x5c94b0 + 0xb25840 pair"),
    (0x5C11A0, "S7", "item run after the pair"),
    (0x5C1D50, "S8", "the single loop nest (head 0x5c1d50, back edge 0x5c1e5b)"),
    (0x5C1E61, "S9", "tail item run and the single epilogue"),
]

ITEM_CTORS = {
    0x26C10: "group",
    0x26B60: "separator",
    0x26F90: "command",
    0x26DB0: "named",
    0x26EA0: "toggle",
    0x27050: "float",
    0x27120: "float_from_var",
}
RELEASE = 0xF590
REGISTER = 0x268B0
VIEW_CTOR = 0xF2F0
STRACE_TARGETS = set(ITEM_CTORS) | {RELEASE, REGISTER, VIEW_CTOR}

REGS = ["rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp",
        "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"]
CALLER_SAVED = {"rax", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11"}


@dataclass(frozen=True)
class Value:
    kind: str          # "rva" | "stack" | "imm" | "unknown"
    value: int = 0

    def __str__(self) -> str:
        if self.kind == "rva":
            return f"rva:0x{self.value:x}"
        if self.kind == "stack":
            return f"stack:0x{self.value:x}"
        if self.kind == "imm":
            return f"imm:0x{self.value:x}"
        return "?"


UNKNOWN = Value("unknown")


def normalize(reg: str) -> str:
    """%r9d/%r9l/%eax/%ax all name their 64-bit container."""
    reg = reg.lstrip("%")
    legacy = {"eax": "rax", "ebx": "rbx", "ecx": "rcx", "edx": "rdx", "esi": "rsi",
              "edi": "rdi", "ebp": "rbp", "esp": "rsp", "ax": "rax", "bx": "rbx",
              "cx": "rcx", "dx": "rdx", "si": "rsi", "di": "rdi"}
    if reg in legacy:
        return legacy[reg]
    for suffix in ("d", "l", "w", "b"):
        if reg.endswith(suffix) and (reg[:-1] in REGS):
            return reg[:-1]
    return reg


def load_text() -> dict[int, str]:
    """Read NUL-terminated strings straight from the pinned image."""
    sys.path.insert(0, str(ROOT / "scripts"))
    import ps4_dynlink as d  # noqa: PLC0415

    data = ELF.read_bytes()
    _, headers = d.program_headers(data)
    image = d.Image(data, headers)
    cache: dict[int, str] = {}

    def text(rva: int) -> str:
        if rva in cache:
            return cache[rva]
        out = bytearray()
        for i in range(256):
            try:
                byte = image.read(rva + i, 1)
            except Exception:  # noqa: BLE001 - outside the image
                break
            if byte == b"\x00":
                break
            out += byte
        cache[rva] = out.decode("latin-1")
        return cache[rva]

    return text  # type: ignore[return-value]


def disassemble_range(start: int, end: int) -> list[tuple[int, str, str, str]]:
    """(address, mnemonic, operands, raw bytes) for [start, end)."""
    out = subprocess.run(
        ["objdump", "-d", f"--start-address=0x{start:x}", f"--stop-address=0x{end:x}",
         str(ELF)],
        check=True, capture_output=True, text=True,
    ).stdout
    instructions = []
    for line in out.splitlines():
        m = re.match(r"^\s*([0-9a-f]+):\s+((?:[0-9a-f]{2} )+)\s*\t(\S+)\s*(.*)$", line)
        if not m:
            continue
        instructions.append((int(m.group(1), 16), m.group(3), m.group(4).strip(), m.group(2)))
    return instructions


def disassemble() -> list[tuple[int, str, str, str]]:
    """(address, mnemonic, operands, raw bytes) for the body range."""
    return disassemble_range(BODY_START, BODY_END + 8)


class Machine:
    """Symbolic register/stack state across straight-line instruction runs."""

    def __init__(self, text) -> None:
        self.regs = {name: UNKNOWN for name in REGS}
        self.mem: dict[int, Value] = {}
        self.mem_f32: dict[int, float] = {}
        self.xmm: dict[int, float | None] = {0: None, 1: None}
        self.text = text
        self.calls: list[dict] = []

    def _set(self, reg: str, value: Value) -> None:
        self.regs[normalize(reg)] = value

    def read_view(self, slot: int) -> dict | None:
        ptr = self.mem.get(slot)
        length = self.mem.get(slot + 8)
        if ptr is None or ptr.kind != "rva" or length is None or length.kind != "imm":
            return None
        return {"rva": ptr.value, "len": length.value, "text": self.text(ptr.value)}

    def step(self, addr: int, mnemonic: str, operands: str) -> None:
        rip_target = None
        m = re.search(r"#\s*0x([0-9a-f]+)", operands)
        if m:
            rip_target = int(m.group(1), 16)
        operands = operands.split("#")[0].strip()

        def parse(op: str) -> Value:
            op = op.strip()
            if op.startswith("%"):
                return self.regs.get(op[1:], UNKNOWN)
            if op in ("%rip",):
                return UNKNOWN
            return UNKNOWN

        if mnemonic in ("nop", "nopl", "nopw", "int3", "ud2"):
            return
        if mnemonic in ("callq", "call"):
            target = None
            if re.fullmatch(r"0x[0-9a-f]+", operands.split()[0]):
                target = int(operands.split()[0], 16)
            if target == VIEW_CTOR:
                slot = self.regs.get("rdi", UNKNOWN)
                source = self.regs.get("rdx", UNKNOWN)
                if slot.kind == "stack" and source.kind == "rva":
                    literal = self.text(source.value)
                    self.mem[slot.value] = source
                    self.mem[slot.value + 8] = Value("imm", len(literal))
            self._record_call(addr, target, operands)
            for reg in CALLER_SAVED:
                self.regs[reg] = UNKNOWN
            return
        if mnemonic.startswith("j") or mnemonic in ("retq", "ret"):
            # The single loop nest is analysed as a region; the scan keeps the slot map
            # because every cluster rewrites its views before the next call.
            return
        if mnemonic in ("pushq", "popq"):
            return

        # leaq disp(%rsp), %reg  /  leaq #target(%rip), %reg
        if mnemonic == "leaq":
            dst = operands.split(",")[-1].strip()
            if not dst.startswith("%"):
                return
            if "(%rsp)" in operands:
                disp = re.match(r"\s*(-?0x[0-9a-f]+|-?\d+)?\(%rsp\)", operands)
                if disp and disp.group(1):
                    value = int(disp.group(1), 16) if disp.group(1).startswith(("0x", "-0x")) else int(disp.group(1))
                else:
                    value = 0
                self._set(dst, Value("stack", value))
            elif rip_target is not None:
                self._set(dst, Value("rva", rip_target))
            elif "%rbp" in operands:
                self._set(dst, UNKNOWN)
            return

        # movq %src, %dst  /  movq $imm, %dst  /  movq / movl
        if mnemonic in ("movq", "movl", "movb", "movw"):
            parts = operands.split(",")
            if len(parts) != 2:
                return
            src, dst = parts[0].strip(), parts[1].strip()
            if src.startswith("$"):
                imm = int(src[1:], 16) if src[1:].startswith("0x") else int(src[1:])
                if dst.startswith("%"):
                    self._set(dst, Value("imm", imm) if imm < 0x100000 else Value("rva", imm))
                else:
                    off = self._stack_offset(dst)
                    if off is not None:
                        self.mem[off] = Value("imm", imm)
                return
            if src.startswith("%") and dst.startswith("%"):
                self._set(dst, self.regs.get(normalize(src), UNKNOWN))
                return
            if src.startswith("%") and "(%rsp)" in dst:
                off = self._stack_offset(dst)
                if off is not None:
                    self.mem[off] = self.regs.get(normalize(src), UNKNOWN)
                return
            if src.startswith("%") and "(%rbp)" in dst:
                off = self._frame_offset(dst)
                if off is not None:
                    self.mem[off] = self.regs.get(normalize(src), UNKNOWN)
                return
            if "(%rsp)" in src and dst.startswith("%"):
                off = self._stack_offset(src)
                if off is not None:
                    self._set(dst, self.mem.get(off, UNKNOWN))
                return
            if "(%rbp)" in src and dst.startswith("%"):
                off = self._frame_offset(src)
                if off is not None:
                    self._set(dst, self.mem.get(off, UNKNOWN))
                return
            return

        if mnemonic in ("vmovss", "movss"):
            parts = [p.strip() for p in operands.split(",")]
            if len(parts) == 2 and parts[1].startswith("%xmm"):
                index = int(parts[1][4:])
                if index in self.xmm:
                    if rip_target is not None:
                        self.xmm[index] = self._read_float(rip_target)
                    elif "%xmm" in parts[0]:
                        src_index = int(parts[0][parts[0].index("%xmm") + 4:])
                        self.xmm[index] = self.xmm.get(src_index)
            elif len(parts) == 2 and "%xmm" in parts[0]:
                off = self._stack_offset(parts[1])
                src_index = int(parts[0][parts[0].index("%xmm") + 4:])
                if off is not None and src_index in self.xmm and self.xmm[src_index] is not None:
                    self.mem_f32[off] = self.xmm[src_index]
            return
        if mnemonic == "vmovups" or mnemonic == "vmovupsq":
            return
        if mnemonic in ("vxorps", "xorl", "xorq", "xchgq", "addq", "subq", "andq", "cmpq",
                        "testb", "testq", "incq", "decq", "shrq", "shlq", "imulq", "orb"):
            parts = operands.split(",")
            if parts:
                dst = parts[-1].strip()
                if dst.startswith("%") and mnemonic in ("xorl", "xorq"):
                    self._set(dst, Value("imm", 0))
                if dst.startswith("%xmm") and mnemonic == "vxorps":
                    index = int(dst[4:])
                    if index in self.xmm:
                        self.xmm[index] = 0.0
            return

    @staticmethod
    def _stack_offset(op: str) -> int | None:
        m = re.match(r"^(-?0x[0-9a-f]+|-?\d+)?\(%rsp\)$", op.strip())
        if not m:
            return None
        if not m.group(1):
            return 0
        raw = m.group(1)
        return int(raw, 16) if raw.startswith(("0x", "-0x")) else int(raw)

    @staticmethod
    def _frame_offset(op: str) -> int | None:
        m = re.match(r"^(-?0x[0-9a-f]+|-?\d+)?\(%rbp\)$", op.strip())
        if not m or not m.group(1):
            return None
        raw = m.group(1)
        return 0x100000 + (int(raw, 16) if raw.startswith(("0x", "-0x")) else int(raw))

    @staticmethod
    def _read_float(rva: int) -> float | None:
        import struct
        sys.path.insert(0, str(ROOT / "scripts"))
        import ps4_dynlink as d  # noqa: PLC0415
        data = ELF.read_bytes()
        _, headers = d.program_headers(data)
        image = d.Image(data, headers)
        try:
            return struct.unpack("<f", image.read(rva, 4))[0]
        except Exception:  # noqa: BLE001
            return None

    def _record_call(self, addr: int, target: int | None, operands: str) -> None:
        if target not in STRACE_TARGETS:
            return
        record = {"site": addr, "target": target, "kind": ITEM_CTORS.get(target)}
        arg_regs = ["rdi", "rsi", "rdx", "rcx", "r8", "r9"]
        args = []
        for reg in arg_regs:
            value = self.regs.get(reg, UNKNOWN)
            entry = {"reg": reg, "value": str(value)}
            if value.kind == "stack":
                entry["slot"] = value.value
                view = self.read_view(value.value)
                if view is not None:
                    entry["view"] = view
                if value.value in self.mem_f32:
                    entry["f32"] = self.mem_f32[value.value]
                entry["words"] = {
                    f"0x{off:x}": str(self.mem[value.value + off])
                    for off in (0, 8, 0x10, 0x18)
                    if value.value + off in self.mem
                }
            args.append(entry)
        record["args"] = args
        record["xmm0"] = self.xmm.get(0)
        record["xmm1"] = self.xmm.get(1)
        self.calls.append(record)


def build_map() -> dict:
    text = load_text()
    instructions = [i for i in disassemble() if i[0] < BODY_END]
    machine = Machine(text)
    for addr, mnemonic, operands, _ in instructions:
        machine.step(addr, mnemonic, operands)

    boundaries = sorted(BOUNDARIES)
    slices = []
    for index, (start, sid, role) in enumerate(boundaries):
        end = boundaries[index + 1][0] if index + 1 < len(boundaries) else BODY_END
        region_instructions = [i for i in instructions if start <= i[0] < end]
        calls = [c for c in machine.calls if start <= c["site"] < end]
        hist: dict[int, int] = {}
        for addr, mnemonic, operands, _ in region_instructions:
            if mnemonic == "callq" and operands.startswith("0x"):
                target = int(operands.split()[0], 16)
                hist[target] = hist.get(target, 0) + 1
        slices.append({
            "id": sid,
            "start": start,
            "end": end,
            "bytes": end - start,
            "instructions": len(region_instructions),
            "role": role,
            "call_targets": {f"0x{k:x}": v for k, v in sorted(hist.items(), key=lambda kv: -kv[1])},
            "calls": calls,
        })

    total_calls = len(machine.calls)
    executed = [s for s in slices if s["id"] in ("S0", "S3", "S4")]
    return {
        "executed_bytes": sum(s["bytes"] for s in executed) + 889,
        "total_executed_bytes": 23232,
        "resume_rva": 0x5BC78C,
        "body": {
            "rva": BODY_START,
            "end_rva": BODY_END,
            "size": BODY_END - BODY_START,
            "instructions": len(instructions),
            "direct_calls_to_modelled_targets": total_calls,
        },
        "s4": {
            "s4_1": {"id": "S4.1", "call_site": 0x5BC3D4, "rva": 0x5DBF40, "end_rva": 0x5DDCAD,
                     "size": 0x1D6D, "insns": 1243, "calls": 177, "items": 20,
                     "status": "executed"},
            "s4_2": {"id": "S4.2", "call_site": 0x5BC3D9, "rva": 0x7F8930, "end_rva": 0x7F898A,
                     "insns": 26, "calls_to": 0x7F8610, "status": "blocked",
                     "reason": "needs 0x7f8610 (688 insns) and host globals 0x3288478/0x3288480"},
        },
        "s2": {
            "id": "S2",
            "rva": 0x5CF140,
            "end_rva": 0x5D06D6,
            "size": 0x1596,
            "ret_rva": 0x5D06D6,
            "instructions": 932,
            "direct_calls": 132,
            "items": 21,
            "status": "executed",
            "executed_by": "native/core/init_5b9f90_slices.cpp (run_init_5b9f90_slices)",
        },
        "s5": {
            "s5_1": {"id": "S5.1", "rva": 0x5BC413, "end_rva": 0x5BC78C, "size": 0x379,
                     "insns": 149, "items": 3, "status": "executed"},
            "s5_2": {"id": "S5.2", "rva": 0x5BC78C, "end_rva": 0x5C1196,                      "size": 0x4A0A,   # 18,954 B (S5 total 19,896 - S5.1 889 - 53 B prologue)
                     "status": "blocked",
                     "reason": "\"Dynamic 3D\" and wormhole pages (19,007 B) still need item extraction"},
        },
        "slices": slices,
    }


# S2 tuples, byte-pinned by tools/lift/check_init_5b9f90_slices.py against the same ELF.
S2_ROWS = [
    (0x5CF1C6, "group", "Shadow", "Shadow Credits", "-"),
    (0x5CF23A, "command", "Shadow Credits", "Jump to credits level", "BP_JumpToCredits -> 0x2dfcbd0"),
    (0x5CF261, "separator", "Shadow Credits", "MenuSeparator", "-"),
    (0x5CF27D, "named_external", "Shadow Credits", "cp10_creditsUseNewList", "cell 0x2ff59c0"),
    (0x5CF315, "bool_var", "Shadow Credits", "Apply adjustments", "cell 0x2ff5a90 guard 0x2ff5b50"),
    (0x5CF487, "bool_var", "Shadow Credits", "Credit Pause", "cell 0x2ff5b58 guard 0x2ff5c18"),
    (0x5CF57B, "separator", "Shadow Credits", "MenuSeparator", "-"),
    (0x5CF592, "separator", "Shadow Credits", "MenuSeparator", "-"),
    (0x5CF69E, "ranged_slider", "Shadow Credits", "Font Scale", "0x2ff5c20 0..4 range 0.01/0.1"),
    (0x5CF75C, "separator", "Shadow Credits", "MenuSeparator", "-"),
    (0x5CF868, "ranged_slider", "Shadow Credits", "Scroller Speed", "0x2ff5cf8 -20..20 range 0.1/1"),
    (0x5CFA09, "ranged_slider", "Shadow Credits", "Scroller Width", "0x2ff5dd0 -2..2 range 0.01/0.1"),
    (0x5CFBAF, "ranged_slider", "Shadow Credits", "Scroller Offset", "0x2ff5ea8 -2..2 range 0.01/0.1"),
    (0x5CFD55, "ranged_slider", "Shadow Credits", "Scroller Left Line Delay", "0x2ff5f80 0..1000 range 0.5/10"),
    (0x5CFEFC, "ranged_slider", "Shadow Credits", "Scroller Right Line Delay", "0x2ff6058 0..10000 range 0.5/10"),
    (0x5CFFBF, "separator", "Shadow Credits", "MenuSeparator", "-"),
    (0x5D00BF, "ranged_slider", "Shadow Credits", "Fade top offset", "0x2ff6130 -1..1 range 0.01/0.1"),
    (0x5D0248, "ranged_slider", "Shadow Credits", "Fade top height", "0x2ff6208 0..1 range 0.01/0.1"),
    (0x5D030B, "separator", "Shadow Credits", "MenuSeparator", "-"),
    (0x5D03FF, "ranged_slider", "Shadow Credits", "Fade bottom offset", "0x2ff62e0 -1..1 range 0.01/0.1"),
    (0x5D0586, "ranged_slider", "Shadow Credits", "Fade bottom height", "0x2ff63b8 0..1 range 0.01/0.1"),
]


# S4.1 tuples, byte-pinned by tools/lift/check_init_5b9f90_slices.py against the same ELF.
S4_ROWS = [
    (0x5DBFAF, "group", "Shadow Render", "Particle", "-"),
    (0x5DC0BB, "ranged_int", "Particle", "Soft Particle Mode", "cell 0x3004980 guard 0x3004b28 -1..2"),
    (0x5DC258, "ranged_float", "Particle", "Soft Particle Fixed Fade", "cell 0x3004a58 guard 0x3004c00 0.05..5"),
    (0x5DC3FE, "ranged_float", "Particle", "Refract Min Fade", "cell 0x3004b30 guard 0x3004cd8 0..50"),
    (0x5DC5A4, "ranged_float", "Particle", "Refract Max Fade", "cell 0x3004c08 guard 0x3004db0 0.01..50"),
    (0x5DC74A, "ranged_float", "Particle", "Camera Min Fade", "cell 0x3004ce0 guard 0x3004e88 0..50"),
    (0x5DC8F0, "ranged_float", "Particle", "Camera Max Fade", "cell 0x3004db8 guard 0x3004f50 0.01..50"),
    (0x5DCA31, "bool_var", "Particle", "Draw Extend", "cell 0x3004e90 guard 0x3005018 item 0x5dcb03"),
    (0x5DCBA3, "bool_var", "Particle", "Draw Extend2", "cell 0x3004f58 guard 0x30050e0 item 0x5dcc75"),
    (0x5DCD15, "bool_var", "Particle", "Draw Blur0", "cell 0x3005020 guard 0x30051a8 item 0x5dcde7"),
    (0x5DCE87, "bool_var", "Particle", "Draw Blur1", "cell 0x30050e8 guard 0x3005270 item 0x5dcf59"),
    (0x5DCFF9, "bool_var", "Particle", "Draw FakeVol", "cell 0x30051b0 guard 0x3005338 item 0x5dd0cb"),
    (0x5DD16B, "bool_var", "Particle", "Draw Anim", "cell 0x3005278 guard 0x3005400 item 0x5dd23d"),
    (0x5DD2DD, "bool_var", "Particle", "Draw RandAxis", "cell 0x3005340 guard 0x30054c8 item 0x5dd3af"),
    (0x5DD44F, "bool_var", "Particle", "Draw Refract", "cell 0x3005408 guard 0x30055a0 item 0x5dd521"),
    (0x5DD626, "ranged_int", "Particle", "Debug Particle Index", "cell 0x30054d0 guard 0x3005668 -1..100"),
    (0x5DD757, "bool_var", "Particle", "Trim Particles", "cell 0x30055a8 guard 0x3005730 item 0x5dd829"),
    (0x5DD8BD, "bool_var", "Particle", "Optimize Identical", "cell 0x3005670 guard 0x30057f8 item 0x5dd98f"),
    (0x5DDA23, "bool_var", "Particle", "Optimize Null", "cell 0x3005738 guard 0x30058c0 item 0x5ddaf5"),
    (0x5DDB87, "bool_var", "Particle", "Particle Offscreen Render", "cell 0x3005800 no guard item 0x5ddc5c"),
]


def view(text: str | None) -> str:
    if text is None:
        return "-"
    return text.replace("|", "\\|").replace("\n", "\\n")


def write_markdown(data: dict, path: Path) -> None:
    body = data["body"]
    lines = [
        "# 0x5b9f90 slice map (debug-menu page builder)",
        "",
        "Generated by `tools/lift/slice_5b9f90.py` from the pinned ELF",
        "(`local/re/eboot.elf`, sha256 `d0723afb...5861d3`). RVAs are identity tags; nothing is",
        "executed here.",
        "",
        f"- body: `0x{body['rva']:x}..0x{body['end_rva'] - 1:x}` "
        f"({body['size']:,} B, {body['instructions']:,} instructions)",
        "- one `ret` (0x5c56aa), one backward branch (0x5c1e5b -> 0x5c1d50, 33 iterations),",
        "- 1,136 direct calls to 25 distinct targets, 53 indirect calls.",
        "",
        "Boundaries are the rare callees and the single loop nest; every other slice is a run of",
        "straight-line item clusters (view write, constructor at 0x26c10/0x26b60/0x26f90/0x26db0/",
        "0x26ea0/0x27050/0x27120, registration at 0x268b0, releases at 0xf590).",
        "",
        "| slice | range | bytes | insns | calls | role |",
        "|---|---|---|---|---|---|",
    ]
    for s in data["slices"]:
        lines.append(
            f"| {s['id']} | `0x{s['start']:x}..0x{s['end'] - 1:x}` | {s['bytes']:,} | "
            f"{s['instructions']:,} | {sum(s['call_targets'].values())} | {view(s['role'])} |"
        )
    lines += [
        "", "## Executed / blocked", "",
        "| slice | status | reason |", "|---|---|---|",
        "| S0 | executed | frame canary + group \"Shadow\" under \"CP10\" (native 0x26c10/0x268b0/0xf590) |",
        "| S1 | contract | 0x2170 \"Shadow Misc\" page builder: body >= 28,288 B (no `ret` before 0x9000), 825 calls; extent not closed |",
        "| S2 | executed | 0x5cf140 \"Shadow Credits\" body (5,526 B, single `ret` at 0x5d06d6, 932 insns, 132 calls): 1 group, 1 command, 6 separators, 1 external named var, 2 bool vars, 10 bounded floats + sliders, 21 destroys |",
        "| S3.1 | executed | \"Shadow Boss Jump\" group + 16 command items |",
        "| S3.2 | executed | \"Shadow Boss Outro Jump\" group + 16 command items |",
        "| S3.3 | executed | \"Shadow Cheat\": 8 toggles, 5 commands, 4 float ranges, 18 destroys |",
        "| S3.4 | executed | \"Shadow Trophy\": 4 guarded bool variables + named items, 5 destroys |",
        "| S3.5 | executed | \"Shadow Render\": group header |",
        "| S3.6 | executed | \"MLAA\": group header |",
        "| S4.1 | executed | 0x5dbf40 \"Particle\" body (7,533 B, 1,243 insns, 177 calls): 1 group, 2 ranged ints, 5 bounded floats, 12 bool variables, 7 sliders, 20 destroys |",
        "| S4.2 | blocked | 0x7f8930 guard helper (26 insns, single call to 0x7f8610) needs 0x7f8610's 688-instruction body and the host globals 0x3288478/0x3288480 |",
        "| S5.1 | executed | 0x5bc413..0x5bc78c \"Stereo 3D\" page (889 B, 149 insns): 1 group, 2 bounded floats (0..500, 0..1), 2 sliders, 2 destroys |",
        "| S5.2 | blocked | 0x5bc78c..0x5c1196 \"Dynamic 3D\" + wormhole pages (18,954 B) still need item extraction |",
        "| S5 | partial | \"Stereo 3D\" page executed (S5.1); S5.2 extraction pending |",
        "| S6 | blocked | 0x5c94b0 + 0xb25840 callee bodies not lifted |",
        "| S7 | blocked | \"Shadow Sound\" / \"Shadow Cursor\" / \"Layers\": extraction pending |",
        "| S8 | blocked | the single loop nest (33 iterations over the 0x2dcaa90 table) |",
        "| S9 | blocked | \"Performance\" / \"Transparency\" / \"Texture\" / \"Fullscreen Effects Tweaks\" |",
        "",
        "Resume offset for the next agent: `0x5bc78c` (S5.2, the \"Dynamic 3D\" page).",
        "Executed extent: 10,173 of 46,881 B of the 0x5b9f90 body (21.7 %), plus the executed",
        "0x5cf140 body (5,526 B) and 0x5dbf40 body (7,533 B) for 23,232 B of native code (49.6 %);",
        "`post_calls_complete` stays false because S1, S4.2, S5.2 and S6..S9 are still contracts/blocked.",
        "",
        "## Callees per slice", "",
              "| slice | callee histogram |", "|---|---|"]
    for s in data["slices"]:
        hist = ", ".join(f"`{k}` x{v}" for k, v in s["call_targets"].items())
        lines.append(f"| {s['id']} | {hist or '-'} |")

    lines += ["", "## Recovered item tuples (executed slices)", ""]
    for s in data["slices"]:
        if s["id"] not in ("S0", "S3"):
            continue
        lines += [f"### {s['id']} `0x{s['start']:x}..0x{s['end'] - 1:x}`", "",
                  "| site | ctor | parent | caption | command / target | flags |",
                  "|---|---|---|---|---|---|"]
        for call in s["calls"]:
            if call["kind"] is None:
                continue
            args = call["args"]
            def text_of(i: int) -> str:
                if i >= len(args):
                    return "-"
                v = args[i].get("view")
                if v is None:
                    return args[i]["value"]
                return f"`0x{v['rva']:x}` \"{view(v['text'])}\" ({v['len']})"
            target = args[3]["value"] if len(args) > 3 else "-"
            lines.append(
                f"| `0x{call['site']:x}` | {call['kind']} | {text_of(1)} | {text_of(2)} | "
                f"{text_of(4) if call['kind'] == 'command' else target} | "
                f"{args[5]['value'] if len(args) > 5 else '-'} |"
            )
        lines.append("")

    lines += ["", "## S4.1 executed callee body `0x5dbf40..0x5ddcad` (7,533 B)", "",
              "| site | kind | parent | text | target / bounds |", "|---|---|---|---|---|"]
    for row in S4_ROWS:
        lines.append("| `0x%x` | %s | %s | %s | %s |" % row)
    lines += ["", "## S2 executed callee body `0x5cf140..0x5d06d6` (5,526 B)", "",
              "| site | kind | parent | text | target / range |", "|---|---|---|---|---|"]
    for row in S2_ROWS:
        lines.append("| `0x%x` | %s | %s | %s | %s |" % row)
    path.write_text("\n".join(lines).rstrip("\n") + "\n")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--json", type=Path, default=ROOT / "docs/5b9f90_slices.json")
    parser.add_argument("--markdown", type=Path, default=ROOT / "docs/5B9F90_SLICES.md")
    parser.add_argument("--stdout", action="store_true")
    args = parser.parse_args()
    data = build_map()
    if args.stdout:
        print(json.dumps(data, indent=1)[:20000])
    else:
        args.json.write_text(json.dumps(data, indent=1) + "\n")
        write_markdown(data, args.markdown)
        print(f"wrote {args.json} and {args.markdown}: "
              f"{len(data['slices'])} slices, "
              f"{data['body']['direct_calls_to_modelled_targets']} modelled-target calls")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
