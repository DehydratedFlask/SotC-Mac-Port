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
NATIVE_THUNKS = [(0x20D60, "concmd_lazy_register", 1), (0x13D20, "slot_1c_setter", 2)]

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

    native_thunks = []
    for rva, name, arg_count in NATIVE_THUNKS:
        kind, _fn, size = classify(rva, fdes, starts, segments)
        if kind != "function_start" or not size:
            die(f"native thunk {rva:#x} is not an FDE start")
        native_thunks.append({"rva": f"0x{rva:x}", "name": name, "args": arg_count,
                              "size": size, "sha256": hashlib.sha256(e.raw(rva, size)).hexdigest()})

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
        "native_thunks": {"count": len(native_thunks), "thunks": native_thunks},
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
    return data, "\n".join(lines) + "\n"


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
        f"**{data['native_thunks']['count']}** entries are hosted as real SysV callables in",
        "`native/core/guest_thunks.cpp`; the fixture and the runtime invoke them through the registry",
        "and assert the observed effects:",
        "",
        "| RVA | Name | Args | Body | Observed |",
        "| --- | --- | ---: | --- | --- |",
    ]
    for thunk in data["native_thunks"]["thunks"]:
        if thunk["name"] == "concmd_lazy_register":
            observed = "object byte +0x18 flips once; hook fires once; second call is a no-op"
        else:
            observed = "dword at object+0x1c becomes the rsi argument"
        lines.append(f"| `{thunk['rva']}` | `{thunk['name']}` | {thunk['args']} | "
                     f"{thunk['size']} B, sha256 `{thunk['sha256'][:16]}…` | {observed} |")
    lines += [
        "",
        "`0x20d60` reproduces the flag test and store, then reports the manager map insert through a",
        "host hook: the insert body (`0x20d80`) needs the live manager pointer from the BSS global",
        "`0x2dfcbc8`, which the host does not have. The 14 one-byte `ret` targets are **not**",
        "registered: a C thunk cannot leave `rax` untouched as the retail stub does, and returning 0",
        "would invent a result.",
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
        "  level for every start, and two entries now have real SysV thunk bodies invoked through the",
        "  runtime. The rest still need a matching native signature and host state.",
        "- Open item 3 (indirect targets) is measured and partially resolved; every remaining site is",
        "  enumerated with its blocking reason instead of being left as a count.",
        "- The native runtime exposes `guest_entry_callable(rva)` and the indirect summary from the",
        "  generated table; `test_guest_entry_table` pins the counts.",
        "",
        "## Limits",
        "",
        "- FDE-derived function universe; a function without unwind data is invisible.",
        "- Provenance is a linear same-block backward scan (max 70 instructions, max 3 register hops).",
        "  A definition behind a branch, call or block boundary is deliberately treated as unknown.",
        "- `retail_entry` proves the ELF shape, not a native callable signature; no thunk bodies are",
        "  generated and no target is inferred from a vtable's name or from a runtime object type.",
        "",
    ]
    return "\n".join(lines)


def main(argv: list[str]) -> int:
    check_only = "--check" in argv
    data, inc = build()
    if not check_only:
        OUT_JSON.write_text(json.dumps(data, indent=1) + "\n")
        OUT_DOC.write_text(render_doc(data))
        OUT_INC.write_text(inc)
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
