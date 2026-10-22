#!/usr/bin/env python3
"""Offline x86-64 argument-setup (rdi/rsi/rdx) extraction for raw callsites of the
sceNgs2RackQueryBufferSize import thunk (0xf91420, slot 0x1601000) in the pinned local ELF.

Static observation only: linear-sweep decoding of fixed function byte ranges with llvm-objdump,
instruction-boundary membership of each candidate, and the last in-block writer of rdi/rsi/rdx.
It makes no claim about reachability, runtime binding, ABI or NGS2 semantics.

usage: python3 -I scripts/aud02_rackquery_args.py OUTDIR
"""
from __future__ import annotations

import hashlib
import json
import re
import struct
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ELF = ROOT / "local/re/eboot.elf"
IMPORTS = ROOT / "local/re/imports.json"
OFFLINE_SCAN = ROOT / "local/re/evidence/aud02-ngs2-rackquery-buffersize-01/offline-scan.json"
EXPECTED_ELF = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
EXPECTED_IMPORTS = "78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744"
THUNK = 0xF91420
# Inclusive ends, as recorded by REA/Ghidra in the existing evidence.
FUNCTIONS = {
    "FUN_0094ea10": [(0x94EA10, 0x94F150), (0x94F160, 0x94F369), (0x94F370, 0x9507A6)],
    "FUN_0099f9d0": [(0x99F9D0, 0x99FC70)],
}

FAMILIES = {
    "rdi": ("rdi", "edi", "di", "dil"),
    "rsi": ("rsi", "esi", "si", "sil"),
    "rdx": ("rdx", "edx", "dx", "dl"),
    "rcx": ("rcx", "ecx", "cx", "cl"),
    "rax": ("rax", "eax", "ax", "al"),
    "rbx": ("rbx", "ebx", "bx", "bl"),
}
for n in range(8, 16):
    FAMILIES[f"r{n}"] = (f"r{n}", f"r{n}d", f"r{n}w", f"r{n}b")
REG_TO_FAMILY = {alias: fam for fam, aliases in FAMILIES.items() for alias in aliases}
ARG_REGS = ("rdi", "rsi", "rdx")
# Mnemonics whose first operand is (at least) written when it is a plain register.
WRITES_FIRST = {"mov", "movabs", "lea", "xor", "movzx", "movsx", "movsxd", "movups", "add", "sub",
                "and", "or", "shl", "shr", "sar", "imul", "neg", "not", "inc", "dec", "cmovne", "cmove",
                "cmovs", "cmovns", "cmova", "cmovb", "cmovae", "cmovbe", "cmovg", "cmovl", "cmovge",
                "cmovle", "pop", "sbb", "adc"}
UNCOND_FLOW_END = {"call", "ret", "jmp", "retq", "int3", "ud2"}
SEXY_COMMENT = re.compile(r"\s+#.*$|\s+<[^>]*>\s*$")


@dataclass(frozen=True)
class Insn:
    addr: int
    raw: bytes
    mnem: str
    ops: tuple[str, ...]
    text: str

    @property
    def end(self) -> int:
        return self.addr + len(self.raw)


def parse_objdump(text: str) -> list[Insn]:
    out = []
    for line in text.splitlines():
        if "\t" not in line:
            continue
        head, _, rest = line.partition("\t")
        m = re.match(r"^\s*([0-9a-f]+):\s+((?:[0-9a-f]{2}\s?)+)\s*$", head)
        if not m:
            continue
        rest = SEXY_COMMENT.sub("", rest.strip())
        mnem, _, opstr = rest.partition("\t")
        mnem = mnem.strip()
        ops = tuple(o.strip() for o in opstr.split(",")) if opstr.strip() else ()
        out.append(Insn(int(m.group(1), 16), bytes.fromhex(m.group(2).replace(" ", "")), mnem, ops,
                        f"{mnem} {', '.join(ops)}".strip()))
    return out


def written_family(insn: Insn) -> str | None:
    """Canonical register family written as a plain first operand (CALL clobbers are handled by the caller)."""
    if insn.mnem in WRITES_FIRST and insn.ops:
        return REG_TO_FAMILY.get(insn.ops[0])
    return None


def parse_imm(token: str) -> int | None:
    try:
        return int(token, 0)
    except ValueError:
        return None


def branch_targets(insns: list[Insn]) -> set[int]:
    targets = set()
    for i in insns:
        if i.mnem == "jmp" or (i.mnem.startswith("j") and len(i.mnem) <= 4):
            if i.ops:
                t = parse_imm(i.ops[0])
                if t is not None:
                    targets.add(t)
    return targets


def backward_defs(insns: list[Insn], idx: int, targets: set[int], max_back: int = 80) -> dict:
    """Last writer of each ARG_REGS family scanning back from insns[idx] within the straight-line block.
    Stops (without crossing) at a previous call/ret/jmp, at the instruction that is a branch target, or at max_back."""
    defs: dict[str, Insn | None] = {r: None for r in ARG_REGS}
    stop = "max_back"
    j = idx - 1
    steps = 0
    while j >= 0 and steps < max_back:
        ins = insns[j]
        if ins.mnem in UNCOND_FLOW_END:
            stop = f"prior {ins.mnem} at {ins.addr:#x}"
            break
        fam = written_family(ins)
        if fam in defs and defs[fam] is None:
            defs[fam] = ins
        if ins.addr in targets:
            stop = f"branch target at {ins.addr:#x}"
            break
        j -= 1
        steps += 1
    else:
        if j < 0:
            stop = "start of swept ranges"
    return {"defs": defs, "stop": stop, "insns_scanned": steps + 1}


def describe_def(ins: Insn | None) -> dict | None:
    if ins is None:
        return None
    d = {"addr": hex(ins.addr), "bytes": ins.raw.hex(), "text": ins.text}
    src = ins.ops[1] if len(ins.ops) > 1 else None
    if ins.mnem in ("mov", "movabs") and src is not None:
        imm = parse_imm(src)
        if imm is not None:
            d.update(kind="immediate", value=imm)
        elif src in REG_TO_FAMILY:
            d.update(kind="register-copy", source=REG_TO_FAMILY[src])
        else:
            d.update(kind="memory-load", source=src)
    elif ins.mnem == "lea" and src is not None:
        d.update(kind="lea", source=src)
    elif ins.mnem == "xor" and len(ins.ops) == 2 and REG_TO_FAMILY.get(ins.ops[0]) == REG_TO_FAMILY.get(ins.ops[1]):
        d.update(kind="zeroing", value=0)
    else:
        d.update(kind="other")
    return d


def lea_base_disp(src: str) -> tuple[str, int] | None:
    m = re.fullmatch(r"\[(r[a-z0-9]+)\s*(?:([+-])\s*(0x[0-9a-f]+|\d+))?\]", src)
    if not m:
        return None
    disp = 0 if not m.group(3) else int(m.group(3), 0) * (-1 if m.group(2) == "-" else 1)
    return m.group(1), disp


def mem_base_disp(op: str) -> tuple[str, int] | None:
    m = re.search(r"ptr \[(r[a-z0-9]+)\s*(?:([+-])\s*(0x[0-9a-f]+|\d+))?\]$", op)
    if not m:
        return None
    return m.group(1), (0 if not m.group(3) else int(m.group(3), 0) * (-1 if m.group(2) == "-" else 1))


def post_call_facts(insns: list[Insn], idx: int, slot: tuple[str, int] | None) -> dict:
    nxt = insns[idx + 1: idx + 9]
    window = [i for i in nxt[:3]]
    sign_check = False
    for k, i in enumerate(window):
        if i.mnem == "test" and i.ops == ("eax", "eax") and k + 1 < len(window) + 1 and k + 1 < len(nxt) \
                and nxt[k + 1].mnem in ("js", "jns", "jl", "jge"):
            sign_check = all(w.mnem == "mov" and w.ops and REG_TO_FAMILY.get(w.ops[0]) not in (None, "rax")
                             for w in window[:k])
            break
    reads_plus8 = None
    if slot is not None:
        for i in nxt:
            if i.mnem == "mov" and len(i.ops) == 2:
                mb = mem_base_disp(i.ops[1])
                if mb == (slot[0], slot[1] + 8):
                    reads_plus8 = {"addr": hex(i.addr), "text": i.text}
                    break
    return {"next_instructions": [f"{i.addr:#x}: {i.text}" for i in nxt[:6]],
            "eax_sign_checked_immediately": sign_check,
            "first_read_of_arg3_slot_plus8_in_next8": reads_plus8}


CALLEE_SAVED = {"rbx", "r12", "r13", "r14", "r15"}


def chase_register(func_insns: list[Insn], block_start_idx: int, fam: str, targets: set[int]) -> dict:
    """Find the writer of `fam` reaching insns[block_start_idx] (exclusive). Scans backward within the straight-line
    block (callee-saved families may cross calls; ret/jmp and branch-target boundaries stop it). If none is found,
    falls back to the sole writer in the whole swept function, if exactly one exists."""
    j = block_start_idx - 1
    while j >= 0:
        ins = func_insns[j]
        if ins.mnem in ("ret", "retq", "jmp", "int3", "ud2") or (ins.mnem == "call" and fam not in CALLEE_SAVED):
            break
        if written_family(ins) == fam:
            return {"resolution": "in-block", "def": describe_def(ins)}
        if ins.addr in targets:
            break
        j -= 1
    # Epilogue/prologue push-pop save/restore is not a value definition.
    writers = [i for i in func_insns if written_family(i) == fam and i.mnem != "pop"]
    if len(writers) == 1:
        return {"resolution": "function-unique-writer", "def": describe_def(writers[0])}
    return {"resolution": "unresolved", "def": None, "writers_in_function": len(writers)}


def struct_stores(func_insns: list[Insn], call_idx: int, targets: set[int], base: str, disp: int,
                  span: int = 0x200, max_back: int = 120) -> list[str]:
    """Offsets (relative to base+disp) written by mov-immediate/register or vector stores in the straight-line block
    before the call, for stores addressed as [base +/- k]."""
    out, j, steps = [], call_idx - 1, 0
    while j >= 0 and steps < max_back:
        ins = func_insns[j]
        if ins.mnem in ("call", "ret", "retq", "jmp"):
            break
        if ins.mnem in ("mov", "movups", "vmovups", "movaps", "vmovaps", "movdqu", "vmovdqu") and ins.ops:
            mb = mem_base_disp(ins.ops[0])
            if mb and mb[0] == base and 0 <= mb[1] - disp < span:
                out.append(f"+{mb[1] - disp:#x}")
        if ins.addr in targets:
            break
        j -= 1
        steps += 1
    return sorted(set(out), key=lambda t: int(t[1:], 16))


def analyze_callsite(insns: list[Insn], targets: set[int], addr: int, func_insns: list[Insn]) -> dict:
    pos = {i.addr: n for n, i in enumerate(insns)}
    if addr not in pos:
        return {"callsite": hex(addr), "instruction_boundary_in_linear_sweep": False}
    n = pos[addr]
    call = insns[n]
    tgt = parse_imm(call.ops[0]) if call.ops else None
    bd = backward_defs(insns, n, targets)
    descs = {r: describe_def(bd["defs"][r]) for r in ARG_REGS}
    chains = {}
    for r in ("rsi", "rdx"):
        d = descs[r]
        if d and d["kind"] == "register-copy":
            def_idx = [k for k, i in enumerate(insns) if i.addr == int(d["addr"], 16)][0]
            chains[r] = chase_register(func_insns, def_idx, d["source"], targets)
    def slot_of(r):
        d = descs[r]
        if d and d["kind"] == "lea":
            return lea_base_disp(d["source"])
        c = chains.get(r)
        if c and c["def"] and c["def"]["kind"] == "lea":
            return lea_base_disp(c["def"]["source"])
        return None
    slot = slot_of("rdx")
    rsi_slot = slot_of("rsi")
    arg2_base = None
    if rsi_slot:
        arg2_base = {"kind": "stack-slot", "base": rsi_slot[0], "disp": rsi_slot[1]}
    elif descs["rsi"] and descs["rsi"]["kind"] == "register-copy":
        src = [a for a in FAMILIES[descs["rsi"]["source"]] if a == descs["rsi"]["text"].split(", ")[1]]
        arg2_base = {"kind": "register-pointer", "base": src[0] if src else descs["rsi"]["source"], "disp": 0}
    rec = {
        "callsite": hex(addr),
        "instruction_boundary_in_linear_sweep": True,
        "decoded_as": call.text, "bytes": call.raw.hex(), "decoded_target": hex(tgt) if tgt is not None else None,
        "target_is_thunk": tgt == THUNK,
        "backward_scan_stop": bd["stop"],
        "args": {"rdi": descs["rdi"], "rsi": descs["rsi"], "rdx": descs["rdx"]},
        "register_copy_chains": chains,
        "arg2_slot": {"base": rsi_slot[0], "disp": rsi_slot[1]} if rsi_slot else None,
        "arg3_slot": {"base": slot[0], "disp": slot[1]} if slot else None,
        "arg2_struct_base": arg2_base,
        "arg2_struct_store_offsets_in_block": (struct_stores(func_insns, n, targets, arg2_base["base"],
                                                             arg2_base["disp"]) if arg2_base else None),
        "after_call": post_call_facts(insns, n, slot),
    }
    return rec


def sha256(b: bytes) -> str:
    return hashlib.sha256(b).hexdigest()


def decode_thunk_name(elf: bytes, target: int, by_slot: dict[int, str]) -> str | None:
    """Name of an import via `jmp qword ptr [rip+rel32]` (ff 25) at `target`, decoded from the ELF bytes and the
    imports.json first_site slot table. Returns None if the bytes are not that stub or the slot is unnamed."""
    phoff = struct.unpack_from("<Q", elf, 32)[0]
    phentsize, phnum = struct.unpack_from("<HH", elf, 54)
    for i in range(phnum):
        o = phoff + i * phentsize
        ptype, flags = struct.unpack_from("<II", elf, o)
        off, va, _pa, fsz, _msz, _al = struct.unpack_from("<QQQQQQ", elf, o + 8)
        if ptype == 1 and flags & 1 and va <= target and target + 6 <= va + fsz:
            b = elf[off + target - va: off + target - va + 6]
            if b[:2] != b"\xff\x25":
                return None
            return by_slot.get(target + 6 + struct.unpack("<i", b[2:])[0])
    return None


def slot_name_map() -> dict[int, str]:
    imp = json.loads(IMPORTS.read_bytes())
    return {int(r["first_site"], 16): r["name"] for r in imp["imports"] if r.get("first_site")}


def run_objdump(ranges: list[tuple[int, int]], objdump: str) -> tuple[str, list[dict]]:
    chunks, cmds = [], []
    for s, e in ranges:
        cmd = [objdump, "-d", "-M", "intel", f"--start-address={s:#x}", f"--stop-address={e + 1:#x}", str(ELF)]
        p = subprocess.run(cmd, capture_output=True, text=True)
        cmds.append({"argv": cmd, "exit": p.returncode, "stdout_sha256": sha256(p.stdout.encode()),
                     "stderr_sha256": sha256(p.stderr.encode())})
        if p.returncode != 0:
            raise RuntimeError(f"objdump failed {p.returncode}: {p.stderr[:300]}")
        chunks.append(p.stdout)
    return "\n".join(chunks), cmds


def main(outdir: Path) -> int:
    elf_raw = ELF.read_bytes()
    if sha256(elf_raw) != EXPECTED_ELF or sha256(IMPORTS.read_bytes()) != EXPECTED_IMPORTS:
        print("FAIL: pinned input digest mismatch", file=sys.stderr)
        return 2
    objdump = subprocess.run(["xcrun", "--find", "llvm-objdump"], capture_output=True, text=True).stdout.strip()
    ver = subprocess.run([objdump, "--version"], capture_output=True, text=True).stdout
    cands = sorted(c["address"] for c in json.loads(OFFLINE_SCAN.read_text())["raw_pattern_scan"]["call_candidates"])
    by_slot = slot_name_map()
    results, provenance = [], {}
    for fname, ranges in FUNCTIONS.items():
        text, cmds = run_objdump(ranges, objdump)
        insns = parse_objdump(text)
        provenance[fname] = {"ranges_inclusive": [[hex(s), hex(e)] for s, e in ranges], "commands": cmds,
                             "instructions": len(insns), "bad_decodes": sum(1 for i in insns if "(bad)" in i.text),
                             "decoded_bytes": sum(len(i.raw) for i in insns),
                             "range_bytes": sum(e - s + 1 for s, e in ranges)}
        targets = branch_targets(insns)
        for a in cands:
            if any(s <= a <= e for s, e in ranges):
                r = analyze_callsite(insns, targets, a, insns)
                r["function"] = fname
                # Names of the thunks called within the next 40 instructions, from imports.json thunk->slot->name.
                if r["instruction_boundary_in_linear_sweep"]:
                    n = [i.addr for i in insns].index(a)
                    seen = []
                    for i in insns[n + 1: n + 41]:
                        nm = decode_thunk_name(elf_raw, parse_imm(i.ops[0]), by_slot) if i.mnem == "call" and i.ops \
                            and parse_imm(i.ops[0]) is not None else None
                        if nm:
                            seen.append(f"{i.addr:#x}->{nm}")
                    r["named_import_calls_in_next_40_insns"] = seen
                results.append(r)
    summary = {
        "status": "OBSERVATIONS_ONLY",
        "elf_sha256": EXPECTED_ELF, "llvm_objdump_version": ver.strip().splitlines()[:2],
        "candidate_count": len(cands), "analyzed": len(results),
        "all_on_instruction_boundary": all(r["instruction_boundary_in_linear_sweep"] for r in results),
        "all_decode_to_thunk": all(r.get("target_is_thunk") for r in results),
        "sweep_provenance": provenance, "callsites": results,
    }
    outdir.mkdir(parents=True, exist_ok=True)
    (outdir / "rackquery-args.json").write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n")
    print(json.dumps({k: summary[k] for k in ("status", "candidate_count", "analyzed", "all_on_instruction_boundary",
                                              "all_decode_to_thunk")}))
    return 0 if len(results) == len(cands) and summary["all_on_instruction_boundary"] else 1


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(__doc__, file=sys.stderr)
        sys.exit(64)
    sys.exit(main(Path(sys.argv[1])))
