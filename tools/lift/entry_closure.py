#!/usr/bin/env python3
"""Type every guest address and resolve the Init closure's indirect sites (CPU-01-AOT-CFG-CALLABLE-01).

Offline, read-only and stdlib-only. The private ELF (`local/re/eboot.elf`, sha256 d0723afb...
5861d3) is read through `slot_pin_util.Elf`, which verifies the digest; instructions come from the
Xcode `llvm-objdump` (`/usr/bin/objdump`) over the pinned bytes. Nothing is loaded, relocated,
executed or written into the pinned image.

Three questions are answered per address and per indirect site:

1. Entry kind (from the previous pass): exact FDE start, interior label, PLT stub, data in R+X,
   image data, outside image.
2. Callable shape for every FDE start: a normal SysV entry with a measured prologue
   (`frame_pointer` / `frame_less` / `leaf`), a direct or GOT `jmp` thunk, or a one-byte `ret` stub.
   The prologue's static stack delta is measured (`8*pushes + sub rsp`) so alignment before calls is
   `aligned`, `misaligned`, `dynamic` (`and rsp,-16`) or `n/a` for leaves.
3. Indirect sites in the `0x531d60` closure: a forward dataflow over each function's basic-block
   CFG meets predecessors, proves an absolute base (or a stack spill) where every reaching path
   agrees, and reads the pinned target from the slot. Every other site keeps its exact offset and a
   verbatim blocking reason.
4. The two retail-ABI entries hosted by `native/core/guest_thunks.cpp`, with the pinned body hash
   the checker re-derives.

The generated `native/core/guest_entry_table.inc` carries (rva, kind, callable) rows plus the
indirect summary; `docs/entry_closure.json` carries every site and every resolution/block reason;
`docs/ENTRY_CLOSURE.md` is the human report. `check_entry_closure.py` re-derives all of it.

Honesty: an interior address is a label/literal, not an error; a `retail_entry` means the ELF shows a
normal function entry, not that a native thunk with a matching signature has been written. No
indirect target is guessed: a site is resolved only when a same-basic-block definition yields an
absolute address and the slot contents land exactly on an FDE start.

Usage: entry_closure.py [--check]
"""

from __future__ import annotations

import bisect
import hashlib
import json
import re
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(ROOT / "scripts"))

from slot_pin_util import Elf  # noqa: E402

ENTRY_MAP = ROOT / "docs/entry_map.json"
OUT_JSON = ROOT / "docs/entry_closure.json"
OUT_DOC = ROOT / "docs/ENTRY_CLOSURE.md"
OUT_INC = ROOT / "native/core/guest_entry_table.inc"
OUT_THUNK_INC = ROOT / "native/core/guest_thunk_table.inc"
CLOSURE_JSON = ROOT / "local/validation/aot-cfg/closure_531d60.json"
ELF_PATH = ROOT / "local/re/eboot.elf"
OBJDUMP = "/usr/bin/objdump"
EH_FRAME_HDR = 0x150C08C
PLT_START = 0xF8F380
PLT_END = 0xF91740
CLOSURE_SEED = "0x531d60"
MAX_PROLOGUE = 18
SCAN_WINDOW = 128
# Retail-ABI thunks implemented in native/core/guest_thunks.cpp: (RVA, name, argument count).
PREVIOUS_TARGETS = (0x19F0, 0x25AA0, 0x2BDD0, 0x20D60, 0x528D90, 0x529150, 0x5296C0,
                    0xC2F0, 0x529A80, 0x538520, 0x538850, 0x538200, 0x5388C0, 0xC4F0, 0xC770)

KIND_ORDER = ("function_start", "function_interior", "plt", "executable_data", "image_data",
              "outside_image")
KIND_ENUM = {
    "function_start": "FunctionStart",
    "function_interior": "FunctionInterior",
    "plt": "Plt",
    "executable_data": "ExecutableData",
    "image_data": "ImageData",
    "outside_image": "OutsideImage",
}
KIND_HELP = {
    "function_start": "exact FDE start: a real function entry",
    "function_interior": "inside an FDE range but not its start: branch/jump label or embedded literal",
    "plt": "import stub in 0xf8f380..0xf91740: callable only through the resolved import",
    "executable_data": "R+X bytes with no FDE: literal pool or the read-only text tail",
    "image_data": "mapped by a non-executable PT_LOAD (globals, vtables, BSS)",
    "outside_image": "not mapped by any PT_LOAD",
}
CALLABLE_ORDER = ("retail_entry", "jmp_thunk", "ret_stub", "not_callable")
CALLABLE_ENUM = {
    "retail_entry": "RetailEntry",
    "jmp_thunk": "JmpThunk",
    "ret_stub": "RetStub",
    "not_callable": "NotCallable",
}
FRAME_ORDER = ("frame_pointer", "frame_less", "leaf")
ALIGN_ORDER = ("aligned", "misaligned", "dynamic", "n/a")
ALIAS = {"eax": "rax", "ebx": "rbx", "ecx": "rcx", "edx": "rdx", "esi": "rsi", "edi": "rdi",
         "ebp": "rbp", "esp": "rsp"}
BRANCHES = {"jmp", "jmpq", "je", "jne", "jz", "jnz", "jb", "jbe", "ja", "jae", "jl", "jle", "jg",
            "jge", "js", "jns", "jp", "jnp", "jo", "jno", "jrcxz", "loop", "loope", "loopne", "ret",
            "retq"}


def die(message: str, code: int = 1) -> None:
    print(f"FAIL {message}", file=sys.stderr)
    raise SystemExit(code)


def reg(name: str) -> str:
    value = name.strip().lstrip("%")
    return ALIAS.get(value, value)


def parse_fdes(e: Elf) -> list[tuple[int, int]]:
    hdr = e.raw(EH_FRAME_HDR, 12)
    ver, fenc, cenc, tenc = hdr[0], hdr[1], hdr[2], hdr[3]
    if (ver, fenc, cenc, tenc) != (1, 0x1B, 0x03, 0x3B):
        die(f"unexpected .eh_frame_hdr encodings {(ver, fenc, cenc, tenc)}")
    count = struct.unpack_from("<i", hdr, 8)[0]
    table = e.raw(EH_FRAME_HDR + 12, 8 * count)
    out = []
    for i in range(count):
        _loc, fde = struct.unpack_from("<ii", table, 8 * i)
        fa = EH_FRAME_HDR + fde
        rec = e.raw(fa, 16)
        start = struct.unpack_from("<i", rec, 8)[0] + fa + 8
        out.append((start, struct.unpack_from("<I", rec, 12)[0]))
    out.sort()
    return out


def parse_segments() -> list[tuple[int, int, int, int]]:
    elf = ELF_PATH.read_bytes()
    phoff = struct.unpack_from("<Q", elf, 0x20)[0]
    entsize = struct.unpack_from("<H", elf, 0x36)[0]
    count = struct.unpack_from("<H", elf, 0x38)[0]
    segments = []
    for i in range(count):
        off = phoff + i * entsize
        p_type, p_flags = struct.unpack_from("<II", elf, off)
        _poff, vaddr, _pa, _fsz, memsz, _align = struct.unpack_from("<QQQQQQ", elf, off + 8)
        if memsz:
            segments.append((p_type, p_flags, vaddr, memsz))
    return segments


def classify(rva: int, fdes: list[tuple[int, int]], starts: list[int],
             segments: list[tuple[int, int, int, int]]):
    k = bisect.bisect_right(starts, rva) - 1
    if k >= 0 and fdes[k][0] <= rva < fdes[k][0] + fdes[k][1]:
        if fdes[k][0] == rva:
            return "function_start", rva, fdes[k][1]
        return "function_interior", fdes[k][0], fdes[k][1]
    if PLT_START <= rva < PLT_END:
        return "plt", None, None
    for p_type, p_flags, vaddr, memsz in segments:
        if vaddr <= rva < vaddr + memsz:
            return ("executable_data" if p_flags & 1 else "image_data"), None, None
    return "outside_image", None, None


def owner(rows: list[dict]) -> tuple[str, str]:
    row = sorted(rows, key=lambda r: (r["header"], r["constant"]))[0]
    return row["constant"], row["header"]


# ---------------------------------------------------------------- disassembly

def merge_intervals(intervals: list[tuple[int, int]]) -> list[tuple[int, int]]:
    merged: list[tuple[int, int]] = []
    for start, end in sorted(intervals):
        if merged and start <= merged[-1][1]:
            merged[-1] = (merged[-1][0], max(merged[-1][1], end))
        else:
            merged.append((start, end))
    return merged


def disassemble(intervals: list[tuple[int, int]]) -> dict[int, tuple[str, str]]:
    """{addr: (mnemonic, operands)} for every instruction inside the merged intervals."""
    if not Path(OBJDUMP).exists():
        die("llvm-objdump is required for the callable/indirect analysis", 2)
    merged = merge_intervals(intervals)
    starts = [s for s, _ in merged]
    max_end = merged[-1][1]

    def inside(addr: int) -> bool:
        if addr < merged[0][0] or addr >= max_end:
            return False
        k = bisect.bisect_right(starts, addr) - 1
        return k >= 0 and addr < merged[k][1]

    fd, path = tempfile.mkstemp(prefix="eboot-", suffix=".dis")
    try:
        with open(fd, "w") as out:
            subprocess.run([OBJDUMP, "-d", str(ELF_PATH)], stdout=out, check=True)
        table: dict[int, tuple[str, str]] = {}
        with open(path, "r", errors="ignore") as text:
            for line in text:
                parts = line.split("\t")
                if len(parts) < 2:
                    continue
                try:
                    addr = int(parts[0].split(":")[0], 16)
                except ValueError:
                    continue
                if not inside(addr):
                    continue
                mnemonic = parts[1].strip()
                operands = parts[2].strip() if len(parts) > 2 else ""
                table[addr] = (mnemonic, operands)
        return table
    finally:
        try:
            Path(path).unlink()
        except OSError:
            pass


def split_ops(operands: str) -> tuple[str, str]:
    code = operands.split("#")[0].strip()
    if "," in code:
        src, dst = code.rsplit(",", 1)
        return src.strip(), dst.strip()
    return code, ""


def absolute_operand(ins: dict[int, tuple[str, str]], addrs: list[int], index: int) -> int | None:
    """Effective address for a RIP-relative operand, using the next instruction address."""
    operands = split_ops(ins[addrs[index]][1])[0]
    match = re.match(r"^(-?0x[0-9a-f]+)?\(%rip\)$", operands)
    if match is None:
        return None
    disp = int(match.group(1), 16) if match.group(1) else 0
    nxt = addrs[index + 1] if index + 1 < len(addrs) else addrs[index]
    return nxt + disp


# ---------------------------------------------------------------- callable shape

def callable_shape(ins: dict[int, tuple[str, str]], addrs: list[int], start: int,
                   size: int) -> dict:
    window = [a for a in addrs if start <= a < start + min(SCAN_WINDOW, max(size, 1))]
    if not window or window[0] != start:
        return {"callable": "retail_entry", "form": "unknown", "target": None,
                "frame": "leaf", "align": "n/a", "pushes": 0, "sub_rsp": 0, "canary": False,
                "endbr64": False}
    i = 0
    endbr64 = False
    if ins[window[0]][0] == "endbr64":
        endbr64 = True
        i = 1
    if i >= len(window):
        return {"callable": "retail_entry", "form": "unknown", "target": None, "frame": "leaf",
                "align": "n/a", "pushes": 0, "sub_rsp": 0, "canary": False, "endbr64": endbr64}
    mnemonic, operands = ins[window[i]]
    if mnemonic in ("jmp", "jmpq") and not operands.startswith("*"):
        match = re.match(r"^(0x[0-9a-f]+)", operands)
        target = int(match.group(1), 16) if match else None
        return {"callable": "jmp_thunk", "form": "direct", "target": target, "frame": "thunk",
                "align": "n/a", "pushes": 0, "sub_rsp": 0, "canary": False, "endbr64": endbr64}
    if mnemonic in ("jmp", "jmpq") and "(%rip)" in operands:
        ea = absolute_operand(ins, window, i)
        return {"callable": "jmp_thunk", "form": "got", "target": ea, "frame": "thunk",
                "align": "n/a", "pushes": 0, "sub_rsp": 0, "canary": False, "endbr64": endbr64}
    if mnemonic in ("ret", "retq"):
        return {"callable": "ret_stub", "form": "ret", "target": None, "frame": "ret",
                "align": "n/a", "pushes": 0, "sub_rsp": 0, "canary": False, "endbr64": endbr64}

    pushes = 0
    sub_rsp = 0
    frame_pointer = False
    dynamic = False
    canary = False
    j = i
    steps = 0
    while j < len(window) and steps < MAX_PROLOGUE:
        mnemonic, operands = ins[window[j]]
        src, dst = split_ops(operands)
        if mnemonic == "pushq" and dst == "" and src.startswith("%"):
            pushes += 1
        elif mnemonic == "subq" and dst == "%rsp" and src.startswith("$0x"):
            sub_rsp += int(src[1:], 16)
        elif mnemonic == "movq" and dst == "%rbp" and src == "%rsp":
            frame_pointer = True
        elif mnemonic == "andq" and dst == "%rsp":
            dynamic = True
        elif mnemonic == "movq" and src == "%fs:0x28":
            canary = True
        elif mnemonic in ("movq", "movl", "leaq") and ("(%rsp)" in dst or dst == "%rsp"):
            pass  # prologue spill/save
        else:
            break
        j += 1
        steps += 1

    delta = 8 * pushes + sub_rsp
    frame = "frame_pointer" if frame_pointer else ("frame_less" if (pushes or sub_rsp) else "leaf")
    if frame == "leaf":
        align = "n/a"
    elif dynamic:
        align = "dynamic"
    else:
        align = "aligned" if (8 + delta) % 16 == 0 else "misaligned"
    return {"callable": "retail_entry", "form": frame, "target": None, "frame": frame,
            "align": align, "pushes": pushes, "sub_rsp": sub_rsp, "canary": canary,
            "endbr64": endbr64, "stack_delta": delta}


# ---------------------------------------------------------------- indirect sites

def op_form(operands: str) -> tuple[str, str | None, int, str]:
    """(form, base register, displacement, detail) for an indirect operand."""
    code = operands.split("#")[0].strip()
    indexed = re.match(r"^\*(-?(?:0x[0-9a-f]+)?)\(,%([a-z0-9]+),8\)$", code)
    if indexed:
        return "indexed_table", None, 0, code
    if "(%" in code and "," in code.split("(", 1)[1]:
        return "indexed_table", None, 0, code
    mem = re.match(r"^\*(-?(?:0x[0-9a-f]+)?)\(%([a-z0-9]+)\)$", code)
    if mem:
        disp = int(mem.group(1), 16) if mem.group(1) else 0
        return "slot", reg(mem.group(2)), disp, code
    direct = re.match(r"^\*%([a-z0-9]+)$", code)
    if direct:
        return "register", reg(direct.group(1)), 0, code
    if code.startswith("*") and "(%rip)" in code:
        return "rip", None, 0, code
    return "other", None, 0, code


CALLER_SAVED = {"rax", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11"}


def _new_state() -> dict:
    return {"regs": {}, "slots": {}, "delta": 0, "rbp_base": None, "origins": {}}


def _copy_state(s: dict) -> dict:
    return {"regs": dict(s["regs"]), "slots": dict(s["slots"]), "delta": s["delta"],
            "rbp_base": s["rbp_base"], "origins": dict(s["origins"])}


def _merge_states(a: dict | None, b: dict | None) -> dict | None:
    """Meet of two block-entry states: a value survives only when every path agrees on it."""
    if a is None:
        return _copy_state(b) if b is not None else None
    if b is None:
        return _copy_state(a)
    out = _new_state()
    if a["delta"] == b["delta"]:
        out["delta"] = a["delta"]
    else:
        out["delta"] = None
        out["origins"]["__delta__"] = "stack_delta_join"
    out["rbp_base"] = a["rbp_base"] if a["rbp_base"] == b["rbp_base"] else None
    for name, value in a["regs"].items():
        if value is not None and b["regs"].get(name) == value:
            out["regs"][name] = value
    for offset, value in a["slots"].items():
        if value is not None and b["slots"].get(offset) == value:
            out["slots"][offset] = value
    for name, origin in a["origins"].items():
        if name not in out["regs"] and b["origins"].get(name) == origin:
            out["origins"][name] = origin
    if "__delta__" not in out["origins"] and b["origins"].get("__delta__"):
        out["origins"]["__delta__"] = b["origins"]["__delta__"]
    return out


def _stack_key(base: str, disp: int, st: dict) -> int | None:
    if base == "rsp":
        return None if st["delta"] is None else st["delta"] + disp
    if base == "rbp":
        return None if st["rbp_base"] is None else st["rbp_base"] + disp
    return None


def _eval_source(src: str, st: dict, ins: dict[int, tuple[str, str]], window: list[int],
                 position: dict[int, int], idx: int, e: "Elf", width: int) -> int | None:
    if src.startswith("$0x"):
        try:
            return int(src[1:], 16)
        except ValueError:
            return None
    if src.startswith("$"):
        return None
    if src.startswith("%"):
        return st["regs"].get(reg(src))
    if "(%rip)" in src:
        ea = absolute_operand(ins, window, idx)
        if ea is None:
            return None
        if width == 32:
            return ea
        try:
            return e.u64(ea)
        except Exception:
            return None
    mem = re.match(r"^(-?0x[0-9a-f]+)?\(%([a-z0-9]+)\)$", src)
    if mem is None:
        return None
    disp = int(mem.group(1), 16) if mem.group(1) else 0
    base = reg(mem.group(2))
    key = _stack_key(base, disp, st)
    if key is not None:
        return st["slots"].get(key)
    base_value = st["regs"].get(base)
    if base_value is None:
        return None
    try:
        return e.u64(base_value + disp)
    except Exception:
        return None


def _invalidate(st: dict, name: str, origin: str) -> None:
    st["regs"].pop(name, None)
    st["origins"][name] = origin


def _transfer(addr: int, mnemonic: str, operands: str, st: dict,
              ins: dict[int, tuple[str, str]], window: list[int],
              position: dict[int, int], e: "Elf") -> None:
    idx = position[addr]
    src, dst = split_ops(operands)
    if mnemonic in ("pushq", "push"):
        value = _eval_source(src, st, ins, window, position, idx, e, 64)
        st["delta"] = None if st["delta"] is None else st["delta"] - 8
        if st["delta"] is not None and value is not None:
            st["slots"][st["delta"]] = value
        return
    if mnemonic in ("popq", "pop"):
        key = _stack_key("rsp", 0, st)
        value = st["slots"].get(key) if key is not None else None
        st["delta"] = None if st["delta"] is None else st["delta"] + 8
        if dst.startswith("%"):
            name = reg(dst)
            if value is None:
                _invalidate(st, name, "pop_unknown")
            else:
                st["regs"][name] = value
        return
    if mnemonic in ("call", "callq"):
        for name in CALLER_SAVED:
            _invalidate(st, name, "call_clobber")
        return
    if mnemonic in BRANCHES:
        return
    if mnemonic in ("subq", "sub") and dst == "%rsp" and src.startswith("$0x"):
        if st["delta"] is not None:
            st["delta"] -= int(src[1:], 16)
        return
    if mnemonic in ("addq", "add") and dst == "%rsp" and src.startswith("$0x"):
        if st["delta"] is not None:
            st["delta"] += int(src[1:], 16)
        return
    if mnemonic == "movq" and src == "%rsp" and dst == "%rbp":
        st["rbp_base"] = st["delta"]
        st["regs"].pop("rbp", None)
        st["origins"]["rbp"] = "frame_pointer"
        return
    if mnemonic == "movq" and src == "%rbp" and dst == "%rsp":
        if st["delta"] is not None and st["rbp_base"] is not None:
            st["delta"] = st["rbp_base"]
        else:
            st["delta"] = None
        return
    if mnemonic in ("movq", "movl", "mov", "movabsq", "leaq", "lea"):
        width = 32 if mnemonic in ("movl",) else 64
        if mnemonic in ("leaq", "lea") and "(%rip)" in src:
            # lea yields the address itself, not the qword stored there.
            value = absolute_operand(ins, window, idx)
        elif mnemonic in ("leaq", "lea") and src.startswith("$0x"):
            value = int(src[1:], 16)
        elif mnemonic in ("leaq", "lea") and "(%rsp)" in src or mnemonic in ("leaq", "lea") and "(%rbp)" in src:
            value = None
        else:
            value = _eval_source(src, st, ins, window, position, idx, e, width)
        if dst.startswith("%"):
            name = reg(dst)
            if value is None:
                origin = "stack_address" if "(%rsp)" in src or "(%rbp)" in src else (
                    ("object_load:" + src) if "(" in src else mnemonic + "_source")
                _invalidate(st, name, origin)
            else:
                st["regs"][name] = value & 0xFFFFFFFF if mnemonic in ("movl",) else value
                st["origins"].pop(name, None)
            return
        mem_dst = re.match(r"^(-?0x[0-9a-f]+)?\(%([a-z0-9]+)\)$", dst)
        if mem_dst is not None:
            disp = int(mem_dst.group(1), 16) if mem_dst.group(1) else 0
            key = _stack_key(reg(mem_dst.group(2)), disp, st)
            if key is not None:
                if value is None:
                    st["slots"].pop(key, None)
                else:
                    st["slots"][key] = value & 0xFFFFFFFF if mnemonic in ("movl",) else value
        return
    if mnemonic.startswith(("test", "cmp", "bt", "ucomi", "comi", "ptest")):
        return
    if mnemonic in ("xorq", "xorl", "xor") and src == dst and src.startswith("%"):
        st["regs"][reg(src)] = 0
        st["origins"].pop(reg(src), None)
        return
    if mnemonic in ("addq", "subq", "add", "sub") and dst.startswith("%"):
        name = reg(dst)
        value = st["regs"].get(name)
        addend = _eval_source(src, st, ins, window, position, idx, e, 64) if src.startswith("%") else (
            int(src[1:], 16) if src.startswith("$0x") else None)
        if value is not None and addend is not None:
            st["regs"][name] = value + addend if mnemonic.startswith("add") else value - addend
        else:
            _invalidate(st, name, mnemonic + "_writes")
        return
    if dst.startswith("%"):
        _invalidate(st, reg(dst), mnemonic + "_writes")


def _cfg_blocks(window: list[int], ins: dict[int, tuple[str, str]]) -> tuple[list[list[int]], dict[int, int]]:
    position = {a: i for i, a in enumerate(window)}
    leaders = {window[0]}
    for k, a in enumerate(window):
        mnemonic, operands = ins[a]
        if mnemonic in BRANCHES:
            if k + 1 < len(window):
                leaders.add(window[k + 1])
            if mnemonic not in ("ret", "retq"):
                match = re.match(r"^(0x[0-9a-f]+)", operands)
                if match is not None and int(match.group(1), 16) in position:
                    leaders.add(int(match.group(1), 16))
    ordered = sorted(leaders)
    starts = {a: i for i, a in enumerate(ordered)}
    blocks: list[list[int]] = [[] for _ in ordered]
    block_of: dict[int, int] = {}
    for a in window:
        k = bisect.bisect_right(ordered, a) - 1
        blocks[k].append(a)
        block_of[a] = k
    return blocks, block_of


def _successors(block: list[int], blocks_by_start: dict[int, int],
                ins: dict[int, tuple[str, str]], window: list[int],
                position: dict[int, int]) -> list[int]:
    last = block[-1]
    mnemonic, operands = ins[last]
    out: list[int] = []
    fallthrough = None
    at = position[last]
    nxt = window[at + 1] if at + 1 < len(window) else None
    # fallthrough exists only when the next address is the start of the next block
    for start, index in blocks_by_start.items():
        if start == nxt:
            fallthrough = index
            break
    if mnemonic in ("ret", "retq"):
        return []
    if mnemonic in ("jmp", "jmpq"):
        match = re.match(r"^(0x[0-9a-f]+)", operands)
        if match is not None:
            target = int(match.group(1), 16)
            if target in blocks_by_start:
                out.append(blocks_by_start[target])
        return out
    if mnemonic in BRANCHES:
        match = re.match(r"^(0x[0-9a-f]+)", operands)
        if match is not None:
            target = int(match.group(1), 16)
            if target in blocks_by_start:
                out.append(blocks_by_start[target])
        if fallthrough is not None:
            out.append(fallthrough)
        return out
    if fallthrough is not None:
        out.append(fallthrough)
    return out


def analyze_indirect(e: Elf, ins: dict[int, tuple[str, str]], addrs: list[int],
                     functions: list[tuple[int, int, int]], starts: list[int]) -> dict:
    """Resolve indirect sites with forward dataflow over each function's basic-block CFG.

    Each block entry carries the meet of its predecessors: absolute register values and stack-slot
    spills survive only when every reaching path defined the same value. Calls clobber caller-saved
    registers but not stack slots (a callee cannot write its caller's frame). Sites whose base is
    still unknown keep the verbatim reason recorded at the join or load that lost it.
    """
    site_records = []
    reason_counts: dict[str, int] = {}
    resolved_counts: dict[str, int] = {}

    def read_qword(addr: int | None) -> tuple[int | None, str | None]:
        if addr is None:
            return None, "no_absolute_address"
        try:
            return e.u64(addr), None
        except Exception:
            return None, "not_file_backed"

    for fn, start, size in functions:
        window = [a for a in addrs if start <= a < start + size]
        if not window or window[0] != start:
            continue
        position = {a: i for i, a in enumerate(window)}
        blocks, block_of = _cfg_blocks(window, ins)
        by_start = {block[0]: i for i, block in enumerate(blocks)}
        entry_states: list[dict | None] = [None] * len(blocks)
        entry_states[block_of[start]] = _new_state()
        changed = True
        passes = 0
        while changed and passes < 32:
            changed = False
            passes += 1
            for index, block in enumerate(blocks):
                state = entry_states[index]
                if state is None:
                    continue
                current = _copy_state(state)
                for addr in block[:-1]:
                    _transfer(addr, ins[addr][0], ins[addr][1], current, ins, window, position, e)
                last = block[-1]
                _transfer(last, ins[last][0], ins[last][1], current, ins, window, position, e)
                for succ in _successors(block, by_start, ins, window, position):
                    merged = _merge_states(entry_states[succ], current)
                    if merged is not None and entry_states[succ] != merged:
                        entry_states[succ] = merged
                        changed = True
        if passes >= 32:
            die(f"CFG dataflow did not converge in function {fn:#x}")

        for index, block in enumerate(blocks):
            state = entry_states[index]
            if state is None:
                continue
            current = _copy_state(state)
            for addr in block:
                mnemonic, operands = ins[addr]
                if mnemonic in ("call", "callq", "jmp", "jmpq") and operands.strip().startswith("*"):
                    form, base, disp, detail = op_form(operands)
                    record = {"function": f"0x{fn:x}", "site": f"0x{addr:x}", "mnemonic": mnemonic,
                              "operand": detail, "form": form, "status": "blocked", "target": None,
                              "reason": None, "evidence": None}
                    target = None
                    kind = None
                    if form == "other":
                        record["reason"] = "unsupported_operand_form"
                    elif form == "indexed_table":
                        record["reason"] = "indexed_jump_table_index_not_bounded"
                    elif form == "rip":
                        match = re.search(r"#\s*(0x[0-9a-f]+)", operands)
                        if match is None:
                            record["reason"] = "rip_operand_without_address"
                        else:
                            ea = int(match.group(1), 16)
                            value, why = read_qword(ea)
                            if value is None:
                                record["reason"] = "global_" + why
                                record["evidence"] = f"global 0x{ea:x}"
                            elif value == 0:
                                record["reason"] = "zero_initialized_global"
                                record["evidence"] = f"global 0x{ea:x}"
                            elif value in starts:
                                target, kind = value, "global_pointer"
                                record["evidence"] = f"global 0x{ea:x} -> 0x{value:x}"
                            else:
                                record["reason"] = "global_holds_non_entry"
                                record["evidence"] = f"global 0x{ea:x} -> 0x{value:x}"
                    else:
                        if form == "slot" and base in ("rsp", "rbp"):
                            frame = _stack_key(base, disp, current)
                            spilled = current["slots"].get(frame) if frame is not None else None
                            if spilled is not None and spilled in starts:
                                target, kind = spilled, "stack_slot"
                                record["evidence"] = f"stack 0x{frame:x} -> 0x{spilled:x}"
                            else:
                                record["reason"] = "stack_slot_not_entry"
                                record["evidence"] = f"stack {disp:#x}(%{base})"
                            value = None
                        else:
                            value = current["regs"].get(base) if base is not None else None
                        if value is not None:
                            if form == "register":
                                if value in starts:
                                    target, kind = value, "register"
                                    record["evidence"] = f"{base} = 0x{value:x}"
                                else:
                                    record["reason"] = "register_value_not_entry"
                                    record["evidence"] = f"{base} = 0x{value:x}"
                            else:
                                slot = value + disp
                                slot_value, why = read_qword(slot)
                                if slot_value is None:
                                    record["reason"] = "slot_" + why
                                    record["evidence"] = f"slot 0x{slot:x}"
                                elif slot_value in starts:
                                    target, kind = slot_value, "vtable_slot"
                                    record["evidence"] = f"base 0x{value:x} slot 0x{disp:x} -> 0x{slot_value:x}"
                                elif slot_value == 0:
                                    record["reason"] = "slot_holds_zero"
                                    record["evidence"] = f"slot 0x{slot:x}"
                                else:
                                    record["reason"] = "slot_holds_non_entry"
                                    record["evidence"] = f"slot 0x{slot:x} -> 0x{slot_value:x}"
                        elif record["reason"] is None:
                            origin = current["origins"].get(base) if base is not None else None
                            if origin is None:
                                record["reason"] = "unresolved_base"
                            elif origin == "stack_address":
                                record["reason"] = "base_stack_object"
                            else:
                                record["reason"] = "base_" + origin
                            if origin is not None:
                                record["evidence"] = f"{base}: {origin}"
                    if target is not None:
                        record.update(status="resolved", target=f"0x{target:x}", kind=kind)
                        resolved_counts[kind] = resolved_counts.get(kind, 0) + 1
                    else:
                        reason_counts[record["reason"]] = reason_counts.get(record["reason"], 0) + 1
                    site_records.append(record)
                _transfer(addr, mnemonic, operands, current, ins, window, position, e)

        recorded_here = {int(r["site"], 16) for r in site_records
                         if r["function"] == f"0x{fn:x}"}
        for addr in window:
            if addr in recorded_here:
                continue
            mnemonic, operands = ins[addr]
            if mnemonic in ("call", "callq", "jmp", "jmpq") and operands.strip().startswith("*"):
                form, _base, _disp, detail = op_form(operands)
                record = {"function": f"0x{fn:x}", "site": f"0x{addr:x}", "mnemonic": mnemonic,
                          "operand": detail, "form": form, "status": "blocked", "target": None,
                          "reason": "unreachable_block", "evidence": None}
                reason_counts["unreachable_block"] = reason_counts.get("unreachable_block", 0) + 1
                site_records.append(record)

    site_records.sort(key=lambda r: int(r["site"], 16))
    return {
        "functions": len(functions),
        "sites": len(site_records),
        "resolved": sum(resolved_counts.values()),
        "blocked": len(site_records) - sum(resolved_counts.values()),
        "analysis": "cfg_forward",
        "resolved_kinds": dict(sorted(resolved_counts.items())),
        "blocked_reasons": dict(sorted(reason_counts.items(), key=lambda kv: (-kv[1], kv[0]))),
        "sites_detail": site_records,
    }


# ---------------------------------------------------------------- native thunks

def _exact_body(ins: dict[int, tuple[str, str]], window: list[int], start: int,
                size: int) -> list[tuple[int, str, str]]:
    body = []
    at = window.index(start) if start in window else -1
    if at < 0:
        return body
    end = start + size
    k = at
    while k < len(window) and window[k] < end:
        body.append((window[k], ins[window[k]][0], ins[window[k]][1]))
        k += 1
    return body


def _rax_live_after(ins: dict[int, tuple[str, str]], window: list[int],
                    sites: set[int]) -> dict[int, bool]:
    """True when rax is read on some path after the instruction before being redefined.

    `ret` counts as a read (the caller may consume the return), so a value merely passed through
    to the function's own caller is not treated as dead.
    """
    blocks, _ = _cfg_blocks(window, ins)
    by_start = {block[0]: i for i, block in enumerate(blocks)}
    position = {a: i for i, a in enumerate(window)}
    successors = [_successors(block, by_start, ins, window, position) for block in blocks]
    live_in = [False] * len(window)
    live_out: dict[int, bool] = {a: False for a in window}

    def rax_use_def(mnemonic: str, operands: str) -> tuple[bool, bool]:
        def has_rax(text: str) -> bool:
            return re.search(r"%(?:r|e)?ax\b|%al\b", text) is not None
        src, dst = split_ops(operands)
        use = False
        define = False
        if mnemonic in ("ret", "retq"):
            use = True
        if mnemonic in ("call", "callq"):
            if has_rax(src):
                use = True
            return use, True
        if mnemonic.startswith(("cmp", "test")):
            return (has_rax(src) or has_rax(dst)), False
        if has_rax(src):
            use = True
        if dst.startswith("%"):
            define = has_rax(dst)
        elif has_rax(dst):
            use = True
        if mnemonic.startswith("pop") and has_rax(dst):
            use, define = False, True
        if mnemonic.startswith("push") and has_rax(src):
            use = True
        return use, define

    changed = True
    while changed:
        changed = False
        for bi in range(len(blocks) - 1, -1, -1):
            out = any(live_in[position[window[s]]] for s in successors[bi])
            for addr in reversed(blocks[bi]):
                i = position[addr]
                live_out[addr] = out
                use, define = rax_use_def(ins[addr][0], ins[addr][1])
                new = use or (out and not define)
                if live_in[i] != new:
                    live_in[i] = new
                    changed = True
                out = new
    return {site: live_out[site] for site in sites}


def qword_reference_count(rva: int) -> int:
    """Occurrences of the 8-byte little-endian address anywhere in the pinned image file."""
    raw = ELF_PATH.read_bytes()
    needle = struct.pack("<Q", rva)
    count = 0
    pos = 0
    while True:
        at = raw.find(needle, pos)
        if at < 0:
            return count
        count += 1
        pos = at + 1


def _scan_full_fde_direct_calls(e: Elf, fdes: list[tuple[int, int]],
                                targets: set[int]) -> dict:
    """Find direct `call rel32` sites over every FDE range, not only mapped starts.

    A raw byte scan narrows the candidates; every candidate caller body is then decoded from its
    FDE range and the site is accepted only when the decoded instruction is a direct call to the
    wanted target. Liveness runs over that complete caller body.
    """
    starts = [start for start, _ in fdes]
    sizes = dict(fdes)
    candidates: dict[int, list[tuple[int, int]]] = {}
    raw_candidates = 0
    for start, size in fdes:
        blob = e.raw(start, size)
        pos = 0
        while True:
            at = blob.find(b"\xe8", pos, len(blob) - 4)
            if at < 0:
                break
            target = start + at + 5 + struct.unpack_from("<i", blob, at + 1)[0]
            if target in targets:
                raw_candidates += 1
                candidates.setdefault(start, []).append((start + at, target))
            pos = at + 1
    sites: list[dict] = []
    decoded_instructions = 0
    if candidates:
        ins = disassemble([(fn, fn + sizes[fn]) for fn in candidates])
        decoded_instructions = len(ins)
        decoded_addrs = sorted(ins)
        for fn, raw_sites in sorted(candidates.items()):
            wanted = {site for site, _ in raw_sites}
            lo = bisect.bisect_left(decoded_addrs, fn)
            hi = bisect.bisect_left(decoded_addrs, fn + sizes[fn])
            window = decoded_addrs[lo:hi]
            result = _rax_live_after(ins, window, wanted)
            for site, target in raw_sites:
                decoded = ins.get(site)
                if decoded is None or decoded[0] not in ("call", "callq"):
                    continue
                match = re.match(r"^(0x[0-9a-f]+)(?: <[^>]*>)?$", decoded[1].strip())
                if match is None or int(match.group(1), 16) != target:
                    continue
                sites.append({"target": target, "function": fn, "site": site,
                              "live": result[site]})
    return {
        "summary": {"fde_ranges": len(fdes), "bytes": sum(sizes.values()),
                    "raw_candidates": raw_candidates, "candidate_callers": len(candidates),
                    "decoded_instructions": decoded_instructions,
                    "validated_sites": len(sites),
                    "targets_with_sites": len({s["target"] for s in sites})},
        "sites": sites,
    }


def _plan_native_thunks(e: Elf, ins: dict[int, tuple[str, str]], addrs: list[int],
                        kinds: dict[int, tuple[str, int | None, int | None]],
                        fde_sizes: dict[int, int], start_set: set[int],
                        indirect: dict | None, direct_scan: dict) -> tuple[list[dict], list[dict], dict, bytes]:
    """Register only entries whose complete FDE body is one of the exact host-free patterns."""
    sites_by_target: dict[int, list[dict]] = {}
    live: dict[tuple[int, int], bool] = {}
    if indirect:
        by_function: dict[int, list[dict]] = {}
        for record in indirect["sites_detail"]:
            if record["status"] != "resolved":
                continue
            by_function.setdefault(int(record["function"], 16), []).append(record)
            sites_by_target.setdefault(int(record["target"], 16), []).append(record)
        for fn, records in by_function.items():
            size = fde_sizes.get(fn, 0)
            if not size:
                continue
            window = [a for a in addrs if fn <= a < fn + size]
            if not window or window[0] != fn:
                continue
            result = _rax_live_after(ins, window, {int(r["site"], 16) for r in records})
            for record in records:
                live[(fn, int(record["site"], 16))] = result[int(record["site"], 16)]

    # Direct `call rel32` sites from the full-FDE scan. These are not part of the indirect
    # frontier, but they are equally valid liveness evidence for the target's return.
    for entry in direct_scan["sites"]:
        fn = int(entry["function"])
        site = int(entry["site"])
        live[(fn, site)] = entry["live"]
        sites_by_target.setdefault(int(entry["target"]), []).append(
            {"function": f"0x{fn:x}", "site": f"0x{site:x}", "direct": True})

    rows: list[dict] = []
    blocked: list[dict] = []
    # One-byte `ret` entries can be reached as vtable targets without appearing in the entry map,
    # so the candidate set is the mapped starts plus every resolved target.
    stub_candidates = set(start_set) | set(sites_by_target)
    stubs = sorted(rva for rva in stub_candidates
                   if fde_sizes.get(rva) == 1 and e.raw(rva, 1) == b"\xc3")

    def add(rva: int, name: str, kind: str, args: int, effect: str, fields: dict) -> None:
        size = int(fde_sizes.get(rva, 0) or 0)
        rows.append({"rva": f"0x{rva:x}", "name": name, "kind": kind, "args": args,
                     "size": size, "sha256": hashlib.sha256(e.raw(rva, size)).hexdigest(),
                     "effect": effect, **fields})

    def direct_count(rva: int) -> int:
        return sum(1 for s in sites_by_target.get(rva, []) if s.get("direct"))

    def indirect_count(rva: int) -> int:
        return sum(1 for s in sites_by_target.get(rva, []) if not s.get("direct"))

    for rva in sorted(stubs):
        sites = sites_by_target.get(rva, [])
        if not sites:
            qrefs = qword_reference_count(rva)
            reason = "no_direct_call_in_55790_fdes_no_resolved_indirect_target"
            if qrefs:
                reason += f"_qword_refs_{qrefs}"
            blocked.append({"rva": f"0x{rva:x}", "symbol": None, "reason": reason,
                            "direct_call_sites": 0, "indirect_call_sites": 0,
                            "call_sites": 0, "image_qword_references": qrefs})
            continue
        if any(live[(int(s["function"], 16), int(s["site"], 16))] for s in sites):
            when = [s["site"] for s in sites
                    if live[(int(s["function"], 16), int(s["site"], 16))]]
            blocked.append({"rva": f"0x{rva:x}", "symbol": None,
                            "reason": "return_value_read_at_" + ",".join(when)})
            continue
        add(rva, "retail_void_stub", "void_noop", 0,
            f"no effect; rax unused at all {len(sites)} resolved call sites",
            {"call_sites": len(sites), "direct_call_sites": direct_count(rva),
             "indirect_call_sites": indirect_count(rva), "rax_live_sites": []})

    for rva in sorted(start_set):
        size = int(kinds[rva][2] or 0)
        if size == 0 or size > 32:
            continue
        window = [a for a in addrs if rva <= a < rva + size]
        body = _exact_body(ins, window, rva, size)
        if len(body) < 2 or body[-1][1] not in ("ret", "retq"):
            continue
        mnemonics = [insn[1] for insn in body]
        src, dst = split_ops(body[0][2])
        if mnemonics == ["ret"] or mnemonics == ["retq"]:
            continue
        if len(body) == 2 and mnemonics[0] == "movl" and src.startswith("$0x") and dst == "%eax":
            imm = int(src[1:], 16)
            add(rva, f"const_u32_{rva:x}", "const_u32", 0,
                f"returns the pinned zero-extended immediate 0x{imm:x}", {"immediate": f"0x{imm:x}"})
            continue
        if len(body) == 2 and mnemonics[0] == "xorl" and src == "%eax" and dst == "%eax":
            add(rva, "zero_u32", "zero_u32", 0, "returns 0", {"immediate": "0x0"})
            continue
        if len(body) == 2 and mnemonics[0] == "movl" and src == "%esi":
            match = re.match(r"^(-?0x[0-9a-f]+)\(%rdi\)$", dst)
            if match:
                offset = int(match.group(1), 16)
                sites = sites_by_target.get(rva, [])
                if not sites:
                    blocked.append({"rva": f"0x{rva:x}", "symbol": None,
                                    "reason": "setter_return_value_caller_owned_no_call_site"})
                elif any(live[(int(s["function"], 16), int(s["site"], 16))] for s in sites):
                    blocked.append({"rva": f"0x{rva:x}", "symbol": None,
                                    "reason": "setter_return_value_read_at_call_site"})
                else:
                    add(rva, f"set_u32_{rva:x}", "set_u32", 2,
                        f"stores the low 32 bits of rsi at object+0x{offset:x}; rax unused at "
                        f"all {len(sites)} resolved call sites",
                        {"offset": f"0x{offset:x}", "call_sites": len(sites),
                         "direct_call_sites": direct_count(rva),
                         "indirect_call_sites": indirect_count(rva), "rax_live_sites": []})
                continue
        if len(body) == 2 and mnemonics[0] == "movb" and src == "%sil":
            match = re.match(r"^(-?0x[0-9a-f]+)\(%rdi\)$", dst)
            if match:
                offset = int(match.group(1), 16)
                sites = sites_by_target.get(rva, [])
                if not sites:
                    blocked.append({"rva": f"0x{rva:x}", "symbol": None,
                                    "reason": "byte_setter_return_value_caller_owned_no_call_site"})
                elif any(live[(int(s["function"], 16), int(s["site"], 16))] for s in sites):
                    when = [s["site"] for s in sites
                            if live[(int(s["function"], 16), int(s["site"], 16))]]
                    blocked.append({"rva": f"0x{rva:x}", "symbol": None,
                                    "reason": "byte_setter_return_value_read_at_" + ",".join(when)})
                else:
                    add(rva, f"set_u8_{rva:x}", "set_u8", 2,
                        f"stores the low 8 bits of rsi at object+0x{offset:x}; rax unused at "
                        f"all {len(sites)} mapped call sites",
                        {"offset": f"0x{offset:x}", "call_sites": len(sites),
                         "direct_call_sites": direct_count(rva),
                         "indirect_call_sites": indirect_count(rva), "rax_live_sites": []})
                continue
        # 20 B vtable-pair constructor: copy rsi+0x10 into rdi+0x10 and a pinned constant into rdi+0x20.
        if len(body) == 5 and mnemonics[4] in ("ret", "retq") and mnemonics[0] in ("movq", "leaq"):
            ops = [split_ops(x[2]) for x in body]
            ok = False
            const = None
            if (mnemonics[0] == "movq" and ops[0] == ("0x10(%rsi)", "%rax")
                    and mnemonics[1] == "leaq" and ops[1][1] == "%rcx"
                    and mnemonics[2] == "movq" and ops[2] == ("%rax", "0x10(%rdi)")
                    and mnemonics[3] == "movq" and ops[3] == ("%rcx", "0x20(%rdi)")):
                ok = True
                const = absolute_operand(ins, window, 1)
            elif (mnemonics[0] == "leaq" and ops[0][1] == "%rax"
                    and mnemonics[1] == "movq" and ops[1] == ("%rax", "0x20(%rdi)")
                    and mnemonics[2] == "movq" and ops[2] == ("0x10(%rsi)", "%rax")
                    and mnemonics[3] == "movq" and ops[3] == ("%rax", "0x10(%rdi)")):
                ok = True
                const = absolute_operand(ins, window, 0)
            if ok and const is not None:
                add(rva, f"vtable_pair_{rva:x}", "vtable_pair_ctor", 2,
                    f"stores rsi+0x10 at object+0x10 and constant 0x{const:x} at object+0x20; "
                    f"returns rsi+0x10", {"copy_offset": "0x10", "const_offset": "0x20",
                                          "constant": f"0x{const:x}"})
                continue

    # The ConCmd lazy registration has a host hook for the unmodelled map insert.
    add(0x20D60, "concmd_lazy_register", "concmd_lazy_register", 1,
        "flips object+0x18 once and reports the 0x20d80 map insert through a host hook",
        {"custom": True})

    blocked += [
        {"rva": "0x20d80", "symbol": None,
         "reason": "needs_live_manager_pointer_from_bss_0x2dfcbc8",
         "direct_call_sites": direct_count(0x20D80),
         "indirect_call_sites": indirect_count(0x20D80),
         "call_sites": direct_count(0x20D80) + indirect_count(0x20D80),
         "image_qword_references": qword_reference_count(0x20D80)},
        {"rva": "0x9844b0", "symbol": None,
         "reason": "tail_jump_needs_unmodelled_callee_and_rodata_leaq",
         "direct_call_sites": direct_count(0x9844B0),
         "indirect_call_sites": indirect_count(0x9844B0),
         "call_sites": direct_count(0x9844B0) + indirect_count(0x9844B0),
         "image_qword_references": qword_reference_count(0x9844B0)},
    ]
    blocked.sort(key=lambda item: int(item["rva"], 16))
    rows.sort(key=lambda row: int(row["rva"], 16))

    base = 0x1594DC0
    slot = 0x20
    target = e.u64(base + slot)
    if target != 0x20D60:
        die(f"dispatch vtable slot {base + slot:#x} holds {target:#x}, not 0x20d60")
    dispatch_bytes = e.raw(base, 0x28)
    dispatch = {"site": "0x28471", "function": "0x27990", "vtable": f"0x{base:x}",
                "slot": f"0x{slot:x}", "target": f"0x{target:x}",
                "vtable_sha256": hashlib.sha256(dispatch_bytes).hexdigest()}
    return rows, blocked, dispatch, dispatch_bytes


def _render_thunk_inc(rows: list[dict], dispatch: dict, dispatch_bytes: bytes) -> str:
    lines = [
        "// Generated by tools/lift/entry_closure.py; do not edit.",
        "// Exact host-free leaf bodies from the pinned ELF (sha256 d0723afb...5861d3).",
    ]
    for row in rows:
        if row.get("custom"):
            continue
        rva = int(row["rva"], 16)
        fn = f"thunk_{rva:x}_{row['kind']}"
        lines.append(f"static std::uint64_t {fn}(std::uint64_t a0, std::uint64_t a1,")
        lines.append("                                     std::uint64_t, std::uint64_t,")
        lines.append("                                     std::uint64_t, std::uint64_t) {")
        if row["kind"] == "void_noop":
            lines.append(f"    // body: ret (0x{rva:x}); rax is unused at every resolved call site")
            lines.append("    (void)a0; (void)a1;")
            lines.append("    return 0;")
        elif row["kind"] == "const_u32":
            lines.append(f"    (void)a0; (void)a1;")
            lines.append(f"    return {int(row['immediate'], 16)}ULL;")
        elif row["kind"] == "zero_u32":
            lines.append("    (void)a0; (void)a1;")
            lines.append("    return 0;")
        elif row["kind"] == "set_u32":
            offset = int(row["offset"], 16)
            lines.append("    if (a0 == 0) return 0;")
            lines.append("    const std::uint32_t value = static_cast<std::uint32_t>(a1);")
            lines.append(f"    std::memcpy(reinterpret_cast<void*>(a0 + 0x{offset:x}), &value, sizeof(value));")
            lines.append("    return 0;")
        elif row["kind"] == "set_u8":
            offset = int(row["offset"], 16)
            lines.append("    if (a0 == 0) return 0;")
            lines.append("    const std::uint8_t value = static_cast<std::uint8_t>(a1);")
            lines.append(f"    std::memcpy(reinterpret_cast<void*>(a0 + 0x{offset:x}), &value, sizeof(value));")
            lines.append("    return 0;")
        elif row["kind"] == "vtable_pair_ctor":
            const = int(row["constant"], 16)
            lines.append("    if (a0 == 0 || a1 == 0) return 0;")
            lines.append("    std::uint64_t copied = 0;")
            lines.append("    std::memcpy(&copied, reinterpret_cast<const void*>(a1 + 0x10), sizeof(copied));")
            lines.append("    std::memcpy(reinterpret_cast<void*>(a0 + 0x10), &copied, sizeof(copied));")
            lines.append(f"    const std::uint64_t constant = 0x{const:x}ULL;")
            lines.append("    std::memcpy(reinterpret_cast<void*>(a0 + 0x20), &constant, sizeof(constant));")
            lines.append("    return copied;")
        else:
            lines.append("    (void)a0; (void)a1;")
            lines.append("    return 0;")
        lines.append("}")
    lines.append("")
    lines.append("static constexpr GuestThunk kGuestLeafThunks[] = {")
    for row in rows:
        if row.get("custom"):
            continue
        rva = int(row["rva"], 16)
        fn = f"thunk_{rva:x}_{row['kind']}"
        lines.append(f"    {{0x{rva:x}ULL, \"{row['name']}\", {fn}, {row['args']}}},")
    lines.append("};")
    lines.append("")
    lines.append("// The 15 indirect targets the earlier pass resolved; the runtime invokes each one.")
    lines.append("static constexpr std::uint64_t kGuestResolvedTargets[] = {")
    for i in range(0, len(PREVIOUS_TARGETS), 5):
        chunk = ", ".join(f"0x{rva:x}ULL" for rva in PREVIOUS_TARGETS[i:i + 5])
        lines.append(f"    {chunk},")
    lines.append("};")
    lines.append("")
    lines.append(f"static constexpr GuestDispatchSite kGuestDispatchSite = "
                 f"{{0x{int(dispatch['site'], 16):x}ULL, 0x{int(dispatch['function'], 16):x}ULL, "
                 f"0x{int(dispatch['target'], 16):x}ULL, 0x{int(dispatch['vtable'], 16):x}ULL, "
                 f"0x{int(dispatch['slot'], 16):x}ULL}};")
    lines.append("static constexpr std::uint8_t kGuestDispatchVtable[] = {")
    for i in range(0, len(dispatch_bytes), 8):
        chunk = ", ".join(f"0x{b:02x}" for b in dispatch_bytes[i:i + 8])
        lines.append(f"    {chunk},")
    lines.append("};")
    lines.append("static constexpr std::size_t kGuestDispatchVtableSize = sizeof(kGuestDispatchVtable);")
    return "\n".join(lines) + "\n"


# ---------------------------------------------------------------- build

def build() -> tuple[dict, str]:
    e = Elf()
    fdes = parse_fdes(e)
    if len(fdes) != 55790:
        die(f"FDE count {len(fdes)} != 55790")
    if fdes[0][0] != 0x80 or fdes[-1][0] + fdes[-1][1] != 0xF8F321:
        die(f"unexpected FDE extent {fdes[0][0]:#x}..{fdes[-1][0] + fdes[-1][1]:#x}")
    total = sum(size for _, size in fdes)
    if total != 15839273:
        die(f"FDE total bytes {total} != 15839273")
    starts = [start for start, _ in fdes]
    segments = parse_segments()

    entry_map = json.loads(ENTRY_MAP.read_text())
    rows = entry_map["entries"]
    kinds: dict[int, tuple[str, int | None, int | None]] = {}
    for key in rows:
        rva = int(key, 16)
        kinds[rva] = classify(rva, fdes, starts, segments)
    if len(kinds) != entry_map["totals"]["distinct_rvas"]:
        die("entry count disagrees with entry_map totals")
    start_set = {rva for rva in kinds if kinds[rva][0] == "function_start"}

    closure = None
    closure_missing: list[str] = []
    frontier: list[tuple[int, int, int]] = []
    if CLOSURE_JSON.exists():
        reached = json.loads(CLOSURE_JSON.read_text())["closure"]["reached_list"]
        closure_rvas = sorted(int(x, 16) for x in reached)
        with_entry = [rva for rva in closure_rvas if rva in start_set]
        closure_missing = [f"0x{rva:x}" for rva in closure_rvas if rva not in start_set]
        closure = {"seed": CLOSURE_SEED, "functions": len(closure_rvas),
                   "with_native_entry": len(with_entry),
                   "without_native_entry": len(closure_rvas) - len(with_entry),
                   "percent": round(100.0 * len(with_entry) / len(closure_rvas), 1),
                   "missing_sample": closure_missing[:16]}
        frontier_json = json.loads(CLOSURE_JSON.read_text())["closure"]["indirect_frontier"]
        for item in frontier_json:
            fn = int(item["fn"], 16)
            k = bisect.bisect_right(starts, fn) - 1
            if k < 0 or fdes[k][0] != fn:
                die(f"frontier function {fn:#x} is not an FDE start")
            frontier.append((fn, fdes[k][0], fdes[k][1]))

    intervals = [(rva, rva + min(SCAN_WINDOW, size)) for rva, size in
                 ((rva, kinds[rva][2]) for rva in start_set)]
    intervals += [(start, start + size) for _fn, start, size in frontier]
    ins = disassemble(intervals)
    addrs = sorted(ins)

    callable_counts = {name: 0 for name in CALLABLE_ORDER}
    frame_counts = {name: 0 for name in FRAME_ORDER}
    align_counts = {name: 0 for name in ALIGN_ORDER}
    shapes: dict[int, dict] = {}
    for rva in sorted(start_set):
        shape = callable_shape(ins, addrs, rva, int(kinds[rva][2] or 1))
        shapes[rva] = shape
        callable_counts[shape["callable"]] += 1
        if shape["callable"] == "retail_entry":
            frame_counts[shape["frame"]] += 1
            align_counts[shape["align"]] += 1
    for rva in kinds:
        if rva not in start_set:
            callable_counts["not_callable"] += 1

    indirect = analyze_indirect(e, ins, addrs, frontier, starts) if frontier else None
    resolved_targets = ({int(record["target"], 16) for record in indirect["sites_detail"]
                         if record["status"] == "resolved"} if indirect else set())
    sizes_all = dict(fdes)
    small_starts = {rva for rva in start_set
                    if sizes_all.get(rva, 0) in (1, 4, 5)}
    direct_scan = _scan_full_fde_direct_calls(
        e, fdes, small_starts | resolved_targets | {0x20D80, 0x9844B0})

    thunk_rows, thunk_blocked, dispatch, dispatch_bytes = _plan_native_thunks(
        e, ins, addrs, kinds, {s: size for s, size in fdes}, start_set, indirect, direct_scan)

    counts = {kind: 0 for kind in KIND_ORDER}
    samples: dict[str, list[dict]] = {kind: [] for kind in KIND_ORDER}
    interior = []
    for rva in sorted(kinds):
        kind, fn_start, fn_size = kinds[rva]
        counts[kind] += 1
        constant, header = owner(rows[f"0x{rva:x}"])
        if len(samples[kind]) < 8:
            samples[kind].append({"rva": f"0x{rva:x}", "constant": constant, "header": header})
        if kind == "function_interior":
            interior.append({"rva": f"0x{rva:x}", "function": f"0x{fn_start:x}", "size": fn_size,
                             "offset": rva - fn_start, "constant": constant, "header": header})

    target_named = {kind: 0 for kind in KIND_ORDER}
    target_entries = 0
    for rva in sorted(kinds):
        names = [r["constant"] for r in rows[f"0x{rva:x}"]]
        if not any("Target" in n or n.endswith("Rva") or n.endswith("RVA") for n in names):
            continue
        target_entries += 1
        target_named[kinds[rva][0]] += 1

    thunk_targets = []
    for rva in sorted(start_set):
        shape = shapes[rva]
        if shape["callable"] == "jmp_thunk":
            thunk_targets.append({"rva": f"0x{rva:x}", "form": shape["form"],
                                  "slot": f"0x{shape['target']:x}" if shape["target"] else None})
    stub_rows = [f"0x{rva:x}" for rva in sorted(start_set)
                 if shapes[rva]["callable"] == "ret_stub"]

    data = {
        "schema": "sotc-entry-closure/2",
        "inputs": {
            "elf_sha256": "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3",
            "entry_map": "docs/entry_map.json",
            "closure": "local/validation/aot-cfg/closure_531d60.json" if closure else None,
            "eh_frame_hdr": f"0x{EH_FRAME_HDR:x}",
            "plt": f"0x{PLT_START:x}..0x{PLT_END:x}",
            "disassembler": "Apple LLVM objdump 21.0.0 (/usr/bin/objdump)",
        },
        "fde": {"count": len(fdes), "min_start": f"0x{fdes[0][0]:x}",
                "max_end": f"0x{fdes[-1][0] + fdes[-1][1]:x}", "total_bytes": total},
        "totals": dict({"entries": len(kinds)}, **counts),
        "callable": {"counts": callable_counts, "frame": frame_counts, "align": align_counts,
                     "thunks": thunk_targets, "ret_stubs": stub_rows,
                     "samples": {name: [f"0x{rva:x}" for rva in sorted(start_set)
                                        if shapes[rva]["callable"] == name][:8]
                                 for name in ("retail_entry", "jmp_thunk", "ret_stub")}},
        "target_named": dict({"entries": target_entries}, **target_named),
        "closure": closure,
        "indirect": indirect,
        "native_thunks": {"count": len(thunk_rows), "thunks": thunk_rows,
                          "blocked": thunk_blocked, "dispatch": dispatch,
                          "scan": direct_scan["summary"],
                          "previous_targets": [f"0x{rva:x}" for rva in PREVIOUS_TARGETS]},
        "samples": samples,
        "interior": interior,
    }

    lines = [
        "// Generated by tools/lift/entry_closure.py; do not edit.",
        "// Pinned ELF sha256 d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3,",
        "// .eh_frame_hdr @ 0x150c08c (55,790 FDE ranges) and docs/entry_map.json (1,957 RVAs).",
        "// kind: 1 function start, 2 interior label, 3 PLT stub, 4 data in R+X, 5 image data.",
        "// callable: 1 retail entry, 2 jmp thunk, 3 ret stub, 4 not callable.",
        "static constexpr GuestEntry kGuestEntryTable[] = {",
    ]
    for rva in sorted(kinds):
        shape = shapes.get(rva)
        callable_name = shape["callable"] if shape else "not_callable"
        lines.append(f"    {{0x{rva:012x}ULL, GuestEntryKind::{KIND_ENUM[kinds[rva][0]]}, "
                     f"GuestEntryCallable::{CALLABLE_ENUM[callable_name]}}},")
    lines.append("};")
    if indirect:
        lines.append("static constexpr GuestIndirectSummary kGuestIndirectSummary = "
                     f"{{{indirect['functions']}u, {indirect['sites']}u, "
                     f"{indirect['resolved']}u, {indirect['blocked']}u}};")
    return data, "\n".join(lines) + "\n", _render_thunk_inc(thunk_rows, dispatch, dispatch_bytes)


# ---------------------------------------------------------------- doc

def render_doc(data: dict) -> str:
    totals = data["totals"]
    fde = data["fde"]
    closure = data["closure"]
    callable_data = data["callable"]
    indirect = data["indirect"]
    total = totals["entries"]
    cc = callable_data["counts"]
    lines = [
        "# Entry typing, callable shapes and the indirect frontier (CPU-01-AOT-CFG-CALLABLE-01)",
        "",
        "**Evidence label: STATIC_ANALYSIS_VERIFIED.** Generated by `tools/lift/entry_closure.py`;",
        "`tools/lift/check_entry_closure.py` re-derives every number from the pinned ELF",
        "(`sha256 d0723afb...5861d3`) and `docs/entry_map.json`. Nothing here was executed.",
        "",
        "This is the successor to the 2026-11-13 entry-map typing pass: it answers what each mapped",
        "address is, what shape its entry has, and which of the Init closure's indirect calls can be",
        "resolved from the pinned binary.",
        "",
        "## Inputs",
        "",
        f"- FDE universe: `.eh_frame_hdr` at `{data['inputs']['eh_frame_hdr']}` -> **{fde['count']:,}** ranges, "
        f"`{fde['min_start']}..{fde['max_end']}`, {fde['total_bytes']:,} B.",
        f"- Mapped RVAs: **{total:,}** distinct addresses from `docs/entry_map.json`.",
        f"- Disassembly: `{data['inputs']['disassembler']}` over the pinned image.",
        "",
        "## Address kinds",
        "",
        "| Kind | Entries | Share | Meaning |",
        "| --- | ---: | ---: | --- |",
    ]
    for kind in KIND_ORDER:
        lines.append(f"| `{kind}` | **{totals[kind]:,}** | {100.0 * totals[kind] / total:.1f} % | {KIND_HELP[kind]} |")
    lines += [
        "",
        "## Callable shape of every function start",
        "",
        f"All **{totals['function_start']:,}** exact FDE starts were decoded at their entry:",
        "",
        "| Callable | Count | Evidence |",
        "| --- | ---: | --- |",
        f"| `retail_entry` | **{cc['retail_entry']:,}** | normal SysV entry with a measured prologue |",
        f"| `jmp_thunk` | **{cc['jmp_thunk']:,}** | first instruction is a direct `jmp rel32` or `jmp *GOT(%rip)` |",
        f"| `ret_stub` | **{cc['ret_stub']:,}** | first instruction is `ret` (1-byte function) |",
        f"| `not_callable` | **{cc['not_callable']:,}** | interior label, PLT stub or data row |",
        "",
        "Prologue shapes of the retail entries:",
        "",
        "| Frame | Count | Alignment before calls | Count |",
        "| --- | ---: | --- | ---: |",
    ]
    for frame in FRAME_ORDER:
        align = callable_data["align"]
        if frame == "leaf":
            lines.append(f"| `leaf` | **{callable_data['frame'][frame]:,}** | `n/a` | {align['n/a']:,} |")
        elif frame == "frame_pointer":
            lines.append(f"| `frame_pointer` | **{callable_data['frame'][frame]:,}** | "
                         f"`aligned` {align['aligned']:,}, `misaligned` {align['misaligned']:,}, "
                         f"`dynamic` {align['dynamic']:,} |")
        else:
            lines.append(f"| `frame_less` | **{callable_data['frame'][frame]:,}** | (combined above) |")
    lines += [
        "",
        "Alignment is `(8 + 8*pushes + sub rsp) % 16 == 0`, i.e. the ABI's 16-byte call alignment",
        "after the return address; `dynamic` means `and rsp,-16` realignment. A leaf has no",
        "prologue, so alignment is not applicable (it makes no calls of its own).",
        "",
        f"The **{cc['jmp_thunk']}** thunks are real aliases, not bodies:",
        "",
    ]
    for thunk in callable_data["thunks"]:
        lines.append(f"- `{thunk['rva']}` -> {thunk['form']} `{thunk['slot']}`")
    lines += [
        "",
        "The `ret_stub` rows are the one-byte `ret` functions already executed on the Init path",
        "(`0x56dca0`, `0x56db70`, `0x5b3d00`, `0x5b3cf0`, `0x56fad0` are among them).",
        "",
        "## Native retail-ABI thunks",
        "",
        f"**{data['native_thunks']['count']}** entries are hosted as real SysV callables: the generated",
        "`native/core/guest_thunk_table.inc` plus the custom `0x20d60` body. The runtime and",
        "`test_guest_thunks` invoke them through the registry and assert the observed object delta or",
        "return value:",
        "",
        "| Kind | Count | Registered only when | Observed effect |",
        "| --- | ---: | --- | --- |",
    ]
    thunk_kinds: dict[str, int] = {}
    for thunk in data["native_thunks"]["thunks"]:
        thunk_kinds[thunk["kind"]] = thunk_kinds.get(thunk["kind"], 0) + 1
    kind_proof = {
        "void_noop": ("backward rax liveness proves every resolved caller ignores the return",
                      "no effect; registered only for dead-return stubs"),
        "const_u32": ("the whole body is `movl $imm,%eax; retq`", "returns the pinned immediate"),
        "zero_u32": ("the whole body is `xorl %eax,%eax; retq`", "returns 0"),
        "set_u32": ("whole body is one store + ret; return dead at every mapped site",
                    "stores rsi at object+offset"),
        "set_u8": ("whole body is `movb %sil,disp(%rdi); retq`; direct/indirect return dead",
                   "stores the low byte of rsi at object+offset"),
        "vtable_pair_ctor": ("whole body is two stores + ret, constant from a RIP `lea`",
                             "copies rsi+0x10 and installs the pinned address"),
        "concmd_lazy_register": ("flag test/store reproduced; map insert is a host hook",
                                 "flag flips once, hook fires once, second call no-op"),
    }
    for kind, count in sorted(thunk_kinds.items()):
        proof, effect = kind_proof.get(kind, ("exact leaf body", kind))
        lines.append(f"| `{kind}` | **{count}** | {proof} | {effect} |")
    previous = ["0x19f0", "0x25aa0", "0x2bdd0", "0x20d60", "0x528d90", "0x529150",
                "0x5296c0", "0xc2f0", "0x529a80", "0x538520", "0x538850", "0x538200",
                "0x5388c0", "0xc4f0", "0xc770"]
    states = {thunk["rva"]: thunk["kind"] for thunk in data["native_thunks"]["thunks"]}
    lines += ["", "The 15 RVAs the earlier pass resolved are all accounted for:", ""]
    for rva in previous:
        if rva in states:
            lines.append(f"- `{rva}` — registered as `{states[rva]}` (invoked by the fixture/runtime).")
        else:
            entry = next((x for x in data["native_thunks"]["blocked"] if x["rva"] == rva), None)
            lines.append(f"- `{rva}` — NOT registered: {entry['reason'] if entry else 'missing'}.")
    lines += [
        "",
        f"The **{len(data['native_thunks']['blocked'])}** entries that stay non-callable keep their exact",
        "RVA and reason in `entry_closure.json` under `native_thunks.blocked`:",
        "",
    ]
    for reason, count in sorted({item["reason"]: sum(1 for x in data["native_thunks"]["blocked"]
                                                     if x["reason"] == item["reason"])
                                  for item in data["native_thunks"]["blocked"]}.items()):
        lines.append(f"- `{reason}` — **{count}**")
    lines += [
        "",
        "The direct-call scan now covers the **full FDE-derived code range**, not only mapped starts:",
        f"**{data['native_thunks']['scan']['fde_ranges']:,}** ranges / "
        f"**{data['native_thunks']['scan']['bytes']:,} B** are scanned for `call rel32`, "
        f"**{data['native_thunks']['scan']['raw_candidates']:,}** candidate sites are found, and the",
        f"**{data['native_thunks']['scan']['candidate_callers']:,}** candidate caller bodies",
        f"(**{data['native_thunks']['scan']['decoded_instructions']:,}** instructions) are decoded and",
        "validated. This registers `0xa1cc0` from its only caller, the direct call at `0x256c38` in",
        "`0x2569d0`; entries with no validated caller keep a measured blocker.",
        "",
        "## Dispatch through a resolved indirect site",
        "",
    ]
    dispatch = data["native_thunks"]["dispatch"]
    lines += [
        f"Site `{dispatch['site']}` in `{dispatch['function']}` calls `*0x{int(dispatch['slot'], 16):x}(%rax)`.",
        f"The host vtable image at `{dispatch['vtable']}` (sha256 `{dispatch['vtable_sha256'][:16]}…`)",
        f"holds `{dispatch['target']}` at that slot. `guest_dispatch_slot` reads the slot from that host",
        "image and invokes the native body; the runtime and fixture observe object `+0x18 = 1` and one",
        "hook call (second dispatch is a no-op), not merely the lookup result.",
        "",
        "## Indirect frontier of the Init closure",
        "",
    ]
    if indirect:
        lines += [
            f"The `{closure['seed']}` closure contains **{indirect['functions']}** functions with at least one",
            f"indirect call/jump: **{indirect['sites']} sites**. **{indirect['resolved']} are resolved** to an exact",
            f"pinned target and **{indirect['blocked']} are blocked** with a recorded reason.",
            "",
            "| Resolution | Count | Meaning |",
            "| --- | ---: | --- |",
        ]
        resolution_text = {
            "register": "CFG state proves the register is an FDE start",
            "vtable_slot": "CFG state proves an absolute base; slot contents are an FDE start",
            "stack_slot": "a stack spill holds an FDE start",
            "global_pointer": "a file-backed global points at an FDE start",
        }
        for kind, count in indirect["resolved_kinds"].items():
            lines.append(f"| `{kind}` | **{count}** | {resolution_text.get(kind, kind)} |")
        lines += [
            "",
            "The analysis is a forward dataflow over each function's basic-block CFG: block entries",
            "carry the meet of their predecessors, calls clobber caller-saved registers but not stack",
            "slots, and a value survives a join only when every reaching path defined the same value.",
            "",
            "| Blocking reason | Sites | Verbatim meaning |",
            "| --- | ---: | --- |",
        ]
        for reason, count in indirect["blocked_reasons"].items():
            if reason == "base_stack_object":
                text = "base is the address of a stack object whose vtable is built at run time"
            elif reason == "base_addq_writes":
                text = "base register is written by add/sub with an unproven operand"
            elif reason == "unresolved_base":
                text = "no reaching definition for the base register"
            elif reason.startswith("base_object_load:"):
                text = "base is loaded from object memory: `" + reason.split(":", 1)[1] + "`"
            elif reason == "stack_slot_not_entry":
                text = "the stack spill does not hold an FDE start"
            elif reason == "unreachable_block":
                text = "the CFG proves the containing block unreachable from the entry"
            else:
                text = reason.replace("_", " ")
            lines.append(f"| `{reason}` | **{count}** | {text} |")
        lines += [
            "",
            "Every site (function, site RVA, operand, status, target or reason) is listed in",
            "`entry_closure.json` under `indirect.sites_detail`; nothing is guessed. A resolution",
            "requires the CFG meet to prove an absolute base (or a stack spill) and the slot contents",
            "to be an exact FDE start.",
        ]
    else:
        lines.append("The private closure census was not present, so the indirect frontier was not measured.")
    lines += [
        "",
        "## What this changes",
        "",
        "- `ENTRY_MAP.md`'s open item 2 (address → callable retail-ABI entry) is answered at the shape",
        f"  level for every start, and **{data['native_thunks']['count']}** entries now have real SysV",
        "  thunk bodies; one indirect site executes through the host vtable image end to end.",
        "- Open item 3 (indirect targets) is measured and partially resolved; every remaining site is",
        "  enumerated with its blocking reason instead of being left as a count.",
        "- The native runtime exposes `guest_entry_callable(rva)` and the indirect summary from the",
        "  generated table; `test_guest_entry_table` pins the counts.",
        "",
        "## Limits",
        "",
        "- FDE-derived function universe; a function without unwind data is invisible.",
        "- Provenance is the forward basic-block CFG dataflow described above; a value merged from",
        "  unequal paths, loaded from object memory or built on a stack object stays unknown.",
        "- Thunk registration covers only complete leaf bodies with an exact host-free pattern. The",
        "  other retail entries still need a native signature, host state or a larger lifted body.",
        "",
    ]
    return "\n".join(lines)


def main(argv: list[str]) -> int:
    check_only = "--check" in argv
    data, inc, thunk_inc = build()
    if not check_only:
        OUT_JSON.write_text(json.dumps(data, indent=1) + "\n")
        OUT_DOC.write_text(render_doc(data))
        OUT_INC.write_text(inc)
        OUT_THUNK_INC.write_text(thunk_inc)
    totals = data["totals"]
    closure = data["closure"]
    indirect = data["indirect"]
    print(f"{'checked' if check_only else 'wrote'} entry closure: entries={totals['entries']} "
          f"start={totals['function_start']} interior={totals['function_interior']} plt={totals['plt']} "
          f"xdata={totals['executable_data']} data={totals['image_data']} outside={totals['outside_image']} "
          f"retail={data['callable']['counts']['retail_entry']} thunks={data['callable']['counts']['jmp_thunk']} "
          f"stubs={data['callable']['counts']['ret_stub']} "
          f"indirect={indirect['resolved'] if indirect else '?'}/{indirect['sites'] if indirect else '?'} "
          f"closure={closure['with_native_entry'] if closure else '?'}/"
          f"{closure['functions'] if closure else '?'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
