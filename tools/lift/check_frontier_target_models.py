#!/usr/bin/env python3
"""Pin the body models of the real targets behind the eight sites withdrawn in E1095.

`docs/checkpoints/2026-11-01-frontier-fallback-correction.md` withdrew eight call sites because the
calls read `TABLE + 0x10 + slot`, whose targets had no body model: `0x47860`, `0x31900`, `0x318f0`
and `0x31520`. This checker pins those four bodies and the `0x319c0` wait that `0x31520` reaches
through `vptr+0xb0`, against the pinned ELF:

  * layer 1 (image + listing): each body's FDE range, SHA-256, the instruction count, direct-call and
    indirect-transfer set from the private disassembly (`all.dis`, bisected, skipped when absent), the exact bytes of every instruction the model
    relies on (opcode, displacement, jump condition), every direct call target (decoded from rel32)
    and its PLT name, the `0x31900` jump table, the slot qwords and their single type-8 RELA, and
    the `0x3b570` thunk into `sceKernelUsleep`;
  * layer 2 (model): the matching header constants, the `.cpp` and test files, and the CMake stanza;
  * layer 3 (policy): none of the four targets has a row in `vtable_resolved_entries` (the integrator
    decides whether table-level rows return), and the `kCorrectedVptrReads` audit still names them.

It then runs mutation probes. Each probe breaks one fact (a size, hash, byte, slot address, call
target, PLT name, header constant, a reinstated table row) and the layer that owns that fact must
reject it. Byte probes run with the SHA-256 pin disabled so the instruction-level check has to catch
them by itself. A probe that is accepted fails the checker.

What this does NOT prove: that the models are behaviourally equal to the bodies (the fixture test
`test_frontier_target_models` checks the models against the semantics stated in the headers), nor
that any caller dispatches to them (the callers' vptr argument is a hand argument, see the E1095
checkpoint). Nothing here promotes an evidence grade.

Usage:
  python3 -I tools/lift/check_frontier_target_models.py
"""

from __future__ import annotations

import copy
import hashlib
import importlib.util
import json
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as dyn  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
ELF = ROOT / "local/re/eboot.elf"
PLT = ROOT / "local/validation/registry-analysis-agent/plt_map.json"
FDE_JSON = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"
CMAKE = ROOT / "native/CMakeLists.txt"
TABLE_HEADER = ROOT / "native/core/vtable_resolved_entries.hpp"
TEST = ROOT / "native/tests/test_frontier_target_models.cpp"

# Each spec: rva, size, sha256, header, constants (header name -> value), slots (address, target),
# calls (offset of the e8 opcode -> absolute target, plus the PLT name when the target is a stub),
# patterns (offset -> exact bytes) and an optional jump table.
SPECS = {
    "47860": {
        "rva": 0x47860, "size": 0x80,
        "sha": "ec57d6f6821e7c9f202ffe71b27c7022bb129254319336783af1d40905073628",
        "header": "frontier_47860_locked_store",
        "constants": {"kFrontier47860Rva": 0x47860, "kFrontier47860Size": 0x80,
                      "kFrontier47860EndAddress": 0x478e0,
                      "kFrontier47860SlotAddress0": 0x1594b48, "kFrontier47860SlotAddress1": 0x1595bc0,
                      "kFrontier47860StoredFieldOffset": 0x28, "kFrontier47860LockPointerOffset": 0x70,
                      "kFrontier47860SpinThreshold": 0x2711, "kFrontier47860UsleepPlt": 0xf8fa20,
                      "kFrontier47860UsleepThunkRva": 0x3b570},
        "slots": [(0x1594b48, 0x47860), (0x1595bc0, 0x47860)],
        "calls": {0x54: (0x3b570, None)},
        "patterns": {
            0x14: "4d8b6770",                      # mov r12,[r15+0x70]   (lock pointer)
            0x18: "4d85e47450",                    # test r12,r12; je 0x6d (null: plain store)
            0x1d: "bb01000000",                    # mov ebx,1            (attempt counter)
            0x22: "49c7c5ffffffff",                # mov r13,-1           (held value)
            0x30: "f390",                          # pause
            0x32: "48ffc3",                        # inc rbx
            0x35: "49833c2400750a",                # cmp [r12],0; jne    (test before the CAS)
            0x3c: "31c0f04d0fb12c247415",          # xor eax,eax; lock cmpxchg [r12],r13; je acquired
            0x46: "4881fb1127000072e1",            # cmp rbx,0x2711; jb pause
            0x4f: "bf01000000",                    # mov edi,1 (usleep microseconds)
            0x59: "ebd5",                          # jmp pause
            0x5b: "0faee8",                        # lfence
            0x5e: "31c0",                          # xor eax,eax
            0x60: "4d897728",                      # mov [r15+0x28],r14   (the store)
            0x64: "0faef8",                        # sfence
            0x67: "49870424",                 # xchg [r12],rax        (release with 0)
            0x6b: "eb04",                          # jmp epilogue
            0x6d: "4d897728",                      # null-lock path: the same store, no atomics
        },
    },
    "31900": {
        "rva": 0x31900, "size": 0xb1,
        "sha": "15b2a7dda5337e4914c90e4003f0a4f83c765e8986bc30a61b8144e042689afc",
        "header": "frontier_31900_fios_poll",
        "constants": {"kFrontier31900Rva": 0x31900, "kFrontier31900Size": 0xb1,
                      "kFrontier31900EndAddress": 0x319b1, "kFrontier31900SlotAddress": 0x15954b0,
                      "kFrontier31900JumpTable": 0xf96ec8, "kFrontier31900JumpTableEntries": 4,
                      "kFrontier31900IsDonePlt": 0xf8f7c0, "kFrontier31900GetErrorPlt": 0xf8f7b0,
                      "kFrontier31900GetActualCountPlt": 0xf8f7a0, "kFrontier31900DeletePlt": 0xf8f790,
                      "kFrontier31900SizeVtableOffset": 0x50, "kFrontier31900ModeOffset": 0x08,
                      "kFrontier31900FlagsOffset": 0x0c, "kFrontier31900FlagsSizeMask": 0x08,
                      "kFrontier31900HandleOffset": 0x10, "kFrontier31900CursorOffset": 0x18,
                      "kFrontier31900OpOffset": 0x50, "kFrontier31900StateOffset": 0x54,
                      "kFrontier31900CountOffset": 0x58, "kFrontier31900ErrorOffset": 0x5c},
        "slots": [(0x15954b0, 0x31900)],
        "calls": {0x12: (0xf8f7c0, "sceFiosOpIsDone"), 0x1e: (0xf8f7b0, "sceFiosOpGetError"),
                  0x29: (0xf8f7a0, "sceFiosOpGetActualCount"), 0x7b: (0xf8f790, "sceFiosOpDelete")},
        "patterns": {
            0x09: "837b54007478",                  # cmp [rbx+0x54],0; je 0x87     (idle: nothing to poll)
            0x0f: "8b7b50",                        # mov edi,[rbx+0x50]            (op handle)
            0x17: "84c0746c",     # test al,al; je 0x87           (not done)
            0x1b: "8b7b50",
            0x23: "89435c",                        # mov [rbx+0x5c],eax            (error)
            0x26: "8b7b50",
            0x2e: "894358",                        # mov [rbx+0x58],eax            (actual count)
            0x31: "8b4b54ffc983f9037736",          # state: dec; cmp 3; ja 0x71
            0x3b: "488d158655f600",                # lea rdx,[rip+table]
            0x42: "48630c8a4801d1ffe1",            # movslq; add; jmp *rcx
            0x4b: "489848014318eb1e",   # cdqe; add [rbx+0x18],rax; jmp 0x71
            0x53: "837b5c00743c",                  # cmp [rbx+0x5c],0; je 0x95     (state 1: error test)
            0x59: "c7431000000000eb0f",            # mov dword [rbx+0x10],0; jmp 0x71
            0x62: "c7431000000000",                # state 2: handle = 0
            0x69: "48c7430800000000",         #          qword [rbx+8] = 0
            0x71: "c7435400000000",                # state = 0
            0x78: "8b7b50",
            0x80: "c7435000000000",                # op = 0
            0x87: "837b54000f94c0",                # cmp [rbx+0x54],0; sete al
            0x95: "c7430801000000",                # dword [rbx+8] = 1
            0x9c: "f6430c0874cf",                  # test byte [rbx+0xc],8; je 0x71
            0xa2: "488b034889dfff5050",            # mov rax,[rbx]; mov rdi,rbx; call *0x50(%rax)
            0xab: "48894318ebc0",                  # mov [rbx+0x18],rax; jmp 0x71
        },
        "jump_table": {"lea_at": 0x3b, "entries": [0x31953, 0x31962, 0x3194b, 0x3194b], "bound": 3},
    },
    "318f0": {
        "rva": 0x318f0, "size": 8, "sha": "f1b31ca6c5dcb24576ce2552e3b64bf713a1d042750da335ed8636503976a4b8",
        "header": "frontier_318f0_fios_succeeded",
        "constants": {"kFrontier318f0Rva": 0x318f0, "kFrontier318f0Size": 8,
                      "kFrontier318f0EndAddress": 0x318f8, "kFrontier318f0SlotAddress": 0x15954a8,
                      "kFrontier318f0ErrorOffset": 0x5c},
        "slots": [(0x15954a8, 0x318f0)],
        "calls": {},
        "patterns": {0x00: "837f5c000f94c0c3"},   # cmp [rdi+0x5c],0; sete al; ret
    },
    "31520": {
        "rva": 0x31520, "size": 0x38, "sha": "fcc94b202748165c9a810d72e4941076886a3213a83a3516eb725d261501480a",
        "header": "frontier_31520_fios_close",
        "constants": {"kFrontier31520Rva": 0x31520, "kFrontier31520Size": 0x38,
                      "kFrontier31520EndAddress": 0x31558, "kFrontier31520SlotAddress": 0x1595420,
                      "kFrontier31520WaitVtableOffset": 0xb0, "kFrontier31520CloseSyncPlt": 0xf8f660,
                      "kFrontier31520ModeOffset": 0x8, "kFrontier31520HandleOffset": 0x10,
                      "kFrontier31520ModeOpen": 1},
        "slots": [(0x1595420, 0x31520)],
        "calls": {0x1d: (0xf8f660, "sceFiosFHCloseSync")},
        "patterns": {
            0x09: "488b03ff90b0000000",            # mov rax,[rbx]; call *0xb0(%rax)  (wait, virtual)
            0x12: "837b08017519",                  # cmp dword [rbx+8],1; jne end
            0x18: "8b731031ff",                    # mov esi,[rbx+0x10]; xor edi,edi (attr = 0)
            0x22: "c7431000000000",                # dword [rbx+0x10] = 0
            0x29: "48c7430800000000",              # qword [rbx+8] = 0
            0x37: "c3",
        },
    },
    "319c0": {
        "rva": 0x319c0, "size": 0x3a, "sha": "753093422931abcf3ea167a9753de68422015daf7276b8d14eb0f52b0bdbcbf3",
        "header": "frontier_319c0_fios_wait",
        "constants": {"kFrontier319c0Rva": 0x319c0, "kFrontier319c0Size": 0x3a,
                      "kFrontier319c0EndAddress": 0x319fa, "kFrontier319c0SlotAddress": 0x15954b8,
                      "kFrontier319c0WaitPlt": 0xf8f7d0, "kFrontier319c0OpHandleOffset": 0x50},
        "slots": [(0x15954b8, 0x319c0)],
        "calls": {0x13: (0xf8f7d0, "sceFiosOpWait")},
        "patterns": {
            0x09: "eb0d",                          # jmp loop head
            0x10: "8b7b50",                        # mov edi,[rbx+0x50]
            0x18: "488b034889dfff90a8000000",      # call *0xa8(%rax)   (poll)
            0x24: "84c074e8",                      # test al,al; je wait
            0x28: "488b034889df",
            0x34: "ffa0a0000000",                  # jmp *0xa0(%rax)    (tail: success predicate)
        },
    },
}
THUNK = {"rva": 0x3b570, "size": 5, "target": 0xf8fa20, "plt_name": "sceKernelUsleep"}
# Slot targets that the models record but do not own, checked for the image fact only.
EXTRA_SLOTS = [(0x1595458, 0x316a0)]   # vptr+0x50 under the 0x1595408 table: sceFiosFHGetSize thunk
WITHDRAWN_TARGETS = (0x47860, 0x31900, 0x318f0, 0x31520)
# Characterisation from the private disassembly (all.dis): instruction count, direct calls
# (address -> target) and indirect transfers (address, normalised text).
DIS_PINS = {
    "47860": (45, {0x478b4: 0x3b570}, []),
    "31900": (52, {0x31912: 0xf8f7c0, 0x3191e: 0xf8f7b0, 0x31929: 0xf8f7a0, 0x3197b: 0xf8f790}, [(203081, 'jmpq *%rcx'), (203176, 'callq *0x50(%rax)')]),
    "318f0": (3, {}, []),
    "31520": (18, {0x3153d: 0xf8f660}, [(202028, 'callq *0xb0(%rax)')]),
    "319c0": (20, {0x319d3: 0xf8f7d0}, [(203230, 'callq *0xa8(%rax)'), (203252, 'jmpq *0xa0(%rax)')]),
}
DIS = ROOT / "local/validation/registry-analysis-agent/all.dis"


def load_module(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


class Image:
    def __init__(self):
        elf = ELF.read_bytes()
        if hashlib.sha256(elf).hexdigest() != ELF_SHA256:
            raise SystemExit("FAIL unexpected ELF SHA256")
        _, headers = dyn.program_headers(elf)
        self.image = dyn.Image(elf, headers)
        self.relocs = {}
        for reloc in dyn.parse(elf)["relocs"]:
            self.relocs.setdefault(reloc["target"], []).append(reloc)
        self.plt = json.loads(PLT.read_text())
        census = load_module("cfg_census", ROOT / "tools/lift/cfg_census.py")
        self.fdes, _ = census.load_fde_ranges(FDE_JSON)

    def read(self, address: int, size: int) -> bytes:
        return self.image.read(address, size)

    def qword(self, address: int) -> int:
        return struct.unpack("<Q", self.read(address, 8))[0]

    def type8(self, address: int):
        return [r for r in self.relocs.get(address, []) if r["type"] == 8]


def header_constants(name: str) -> dict[str, int]:
    text = (ROOT / "native/core" / f"{name}.hpp").read_text()
    return {m.group(1): int(m.group(2), 0)
            for m in re.finditer(r"\b(k[A-Za-z0-9_]+) = (0x[0-9a-fA-F]+|\d+)(?:ULL|u)?;", text)}


# ---------------------------------------------------------------------------------------------
# Layer 1: image
# ---------------------------------------------------------------------------------------------


def rel32_target(body: bytes, rva: int, offset: int) -> int:
    assert body[offset] in (0xe8, 0xe9)
    rel = struct.unpack("<i", body[offset + 1:offset + 5])[0]
    return (rva + offset + 5 + rel) & 0xffffffff


def layer1(key: str, spec: dict, img: Image, body: bytes | None = None, check_sha: bool = True,
           slot_override: dict | None = None):
    fails = []
    rva, size = spec["rva"], spec["size"]
    fde = img.fdes.find(rva)
    if fde is None or fde[0] != rva or fde[1] != size:
        fails.append(f"{key}: FDE range is not ({rva:#x}, {size:#x}): {fde}")
    if body is None:
        body = img.read(rva, size)
    if len(body) != size:
        fails.append(f"{key}: body length {len(body)} != {size}")
    if check_sha and spec["sha"] and hashlib.sha256(body).hexdigest() != spec["sha"]:
        fails.append(f"{key}: body SHA-256 changed")
    for offset, hexbytes in spec["patterns"].items():
        want = bytes.fromhex(hexbytes)
        if body[offset:offset + len(want)] != want:
            fails.append(f"{key}: bytes at +{offset:#x} are {body[offset:offset + len(want)].hex()}, "
                         f"expected {hexbytes}")
    for offset, (target, plt_name) in spec["calls"].items():
        if body[offset] != 0xe8:
            fails.append(f"{key}: +{offset:#x} is not a call")
            continue
        got = rel32_target(body, rva, offset)
        if got != target:
            fails.append(f"{key}: call at +{offset:#x} reaches {got:#x}, expected {target:#x}")
        if plt_name is not None:
            entry = img.plt.get(hex(target))
            if entry is None or entry[0] != plt_name or entry[2] != "libSceFios2":
                fails.append(f"{key}: PLT {target:#x} is {entry}, expected {plt_name} in libSceFios2")
    for address, target in spec["slots"]:
        if slot_override and address in slot_override:
            address = slot_override[address]
        got = img.qword(address)
        if got != target:
            fails.append(f"{key}: slot {address:#x} holds {got:#x}, expected {target:#x}")
        rows = img.type8(address)
        if len(rows) != 1 or rows[0]["addend"] != target:
            fails.append(f"{key}: slot {address:#x} lacks exactly one type-8 RELA with addend {target:#x}")
    table = spec.get("jump_table")
    if table:
        lea = table["lea_at"]
        disp = struct.unpack("<i", body[lea + 3:lea + 7])[0]
        base = rva + lea + 7 + disp
        if base != 0xf96ec8:
            fails.append(f"{key}: jump table base {base:#x} != 0xf96ec8")
        for index, want in enumerate(table["entries"]):
            entry = struct.unpack("<i", img.read(base + 4 * index, 4))[0]
            if (base + entry) & 0xffffffff != want:
                fails.append(f"{key}: jump table entry {index} -> {(base + entry) & 0xffffffff:#x}, "
                             f"expected {want:#x}")
        # the bound `dec ecx; cmp ecx,3; ja` is in the patterns; every arm must lie inside the body
        for want in table["entries"]:
            if not rva <= want < rva + size:
                fails.append(f"{key}: jump table arm {want:#x} outside the body")
    return fails


def layer1_thunk(img: Image, body: bytes | None = None, plt_name: str | None = None,
                 target: int | None = None):
    fails = []
    thunk = THUNK
    fde = img.fdes.find(thunk["rva"])
    if fde is None or fde[0] != thunk["rva"] or fde[1] != thunk["size"]:
        fails.append("thunk 0x3b570: FDE range changed")
    body = body if body is not None else img.read(thunk["rva"], thunk["size"])
    if body[0] != 0xe9:
        fails.append("thunk 0x3b570 is not a jmp")
        return fails
    got = rel32_target(body, thunk["rva"], 0)
    want = target if target is not None else thunk["target"]
    if got != want:
        fails.append(f"thunk 0x3b570 jumps to {got:#x}, expected {want:#x}")
    entry = img.plt.get(hex(got))
    name = plt_name or thunk["plt_name"]
    if entry is None or entry[0] != name or entry[2] != "libkernel":
        fails.append(f"thunk target {got:#x} is {entry}, expected {name} in libkernel")
    return fails


def layer1_extra(img: Image, extra=None):
    fails = []
    for address, target in (extra or EXTRA_SLOTS):
        if img.qword(address) != target:
            fails.append(f"extra slot {address:#x} does not hold {target:#x}")
        rows = img.type8(address)
        if len(rows) != 1 or rows[0]["addend"] != target:
            fails.append(f"extra slot {address:#x} lacks exactly one type-8 RELA")
    # 0x316a0 is the FHGetSize thunk the 0x31900 size call reaches under the 0x1595408 table
    body = img.read(0x316a0, 8)
    if body != bytes.fromhex("8b7f10e9c8dff500"):
        fails.append("0x316a0 is no longer `mov edi,[rdi+0x10]; jmp PLT`")
    else:
        got = rel32_target(body, 0x316a0, 3)
        entry = img.plt.get(hex(got))
        if entry is None or entry[0] != "sceFiosFHGetSize":
            fails.append(f"0x316a0 tail-jumps to {got:#x} ({entry}), expected sceFiosFHGetSize")
    return fails



_DIS_LINE = re.compile(rb"^\s*([0-9a-f]+):\s+(.*)$")


def _address_at(handle, position: int):
    handle.seek(position)
    if position:
        handle.readline()
    here = handle.tell()
    line = handle.readline()
    while line:
        match = _DIS_LINE.match(line)
        if match:
            return int(match.group(1), 16), here
        here = handle.tell()
        line = handle.readline()
    return None, here


def dis_scan():
    """{key: [(address, text)]} for the five bodies, by bisecting the address-sorted listing."""
    import os
    ranges = sorted((spec["rva"], spec["rva"] + spec["size"], key) for key, spec in SPECS.items())
    first = ranges[0][0]
    out = {key: [] for key in SPECS}
    with open(DIS, "rb") as handle:
        low, high = 0, os.path.getsize(DIS)
        while high - low > 4096:
            mid = (low + high) // 2
            address, _ = _address_at(handle, mid)
            if address is None or address >= first:
                high = mid
            else:
                low = mid
        handle.seek(low)
        for line in handle:
            match = _DIS_LINE.match(line)
            if not match:
                continue
            address = int(match.group(1), 16)
            for start, end, key in ranges:
                if start <= address < end:
                    out[key].append((address, " ".join(match.group(2).decode().split())))
            if address >= ranges[-1][1]:
                break
    return out


def layer1_dis(key: str, pin, lines):
    count, calls, indirect = pin
    fails = []
    if len(lines) != count:
        fails.append(f"{key}: {len(lines)} instructions in the listing, pinned {count}")
    got_calls = {}
    got_indirect = []
    for address, text in lines:
        direct = re.match(r"callq 0x([0-9a-f]+)", text)
        if direct:
            got_calls[address] = int(direct.group(1), 16)
        elif re.match(r"(callq|jmpq) \*", text):
            got_indirect.append((address, re.sub(r" <.*", "", text)))
    if got_calls != calls:
        fails.append(f"{key}: direct calls {{{', '.join(f'{a:#x}: {t:#x}' for a, t in got_calls.items())}}} "
                     f"differ from the pinned set")
    pinned_indirect = [(a, t) for a, t in indirect]
    if got_indirect != pinned_indirect:
        fails.append(f"{key}: indirect transfers {got_indirect} differ from {pinned_indirect}")
    return fails


# ---------------------------------------------------------------------------------------------
# Layer 2 and 3: model and policy (no ELF needed)
# ---------------------------------------------------------------------------------------------


def layer2(key: str, spec: dict, constants: dict | None = None, cmake: str | None = None,
           files_exist: bool = True):
    fails = []
    constants = constants if constants is not None else header_constants(spec["header"])
    for name, want in spec["constants"].items():
        if constants.get(name) != want:
            fails.append(f"{key}: header constant {name} = {constants.get(name)}, expected {want:#x}")
    cmake = cmake if cmake is not None else CMAKE.read_text()
    for needle in (f"core/{spec['header']}.cpp", "sotc_frontier_targets", "test_frontier_target_models",
                   "NAME frontier_target_models"):
        if needle not in cmake:
            fails.append(f"{key}: CMake lacks {needle}")
    if files_exist:
        for path in (ROOT / "native/core" / f"{spec['header']}.hpp", ROOT / "native/core" / f"{spec['header']}.cpp"):
            if not path.is_file():
                fails.append(f"{key}: {path.name} is missing")
    if not TEST.is_file():
        fails.append("test_frontier_target_models.cpp is missing")
    return fails


def layer3(table_text: str | None = None):
    fails = []
    text = table_text if table_text is not None else TABLE_HEADER.read_text()
    start = text.index("kResolvedVtableCallSites[] = {")
    rows = text[start:text.index("};", start)]
    for target in WITHDRAWN_TARGETS:
        if re.search(rf"0x{target:x}\b", rows) or f"kResolvedVtableTarget_{target:x}Rva" in rows:
            fails.append(f"table row names withdrawn target {target:#x}")
        if re.search(rf"kResolvedVtableTarget_{target:x}Rva\s*=", text):
            fails.append(f"table header defines a constant for withdrawn target {target:#x}")
    audit = text[text.index("kCorrectedVptrReads[] = {"):]
    audit = audit[:audit.index("};")]
    for target in WITHDRAWN_TARGETS:
        if f"0x{target:x}" not in audit:
            fails.append(f"audit array lost target {target:#x}")
    return fails


# ---------------------------------------------------------------------------------------------
# Mutation probes
# ---------------------------------------------------------------------------------------------


def safe(fn, *args, **kwargs):
    """A probe that makes the checker read outside the image counts as rejected."""
    try:
        return fn(*args, **kwargs)
    except (ValueError, IndexError, struct.error, AssertionError) as exc:
        return [f"{fn.__name__} raised {type(exc).__name__}: {exc}"]


def probes(img: Image | None, listing=None):
    results = []

    def run(label: str, fails):
        results.append((label, bool(fails)))

    cmake = CMAKE.read_text()
    table = TABLE_HEADER.read_text()
    for key, spec in SPECS.items():
        # layer 2: every header constant is checked, so breaking each one must be seen
        base = header_constants(spec["header"])
        for name in spec["constants"]:
            mutated = dict(base)
            mutated[name] = base[name] + 1
            run(f"{key} header {name} + 1", layer2(key, spec, constants=mutated))
            mutated = dict(base)
            del mutated[name]
            run(f"{key} header {name} removed", layer2(key, spec, constants=mutated))
        for needle in (f"core/{spec['header']}.cpp", "NAME frontier_target_models"):
            run(f"{key} CMake without {needle}", layer2(key, spec, cmake=cmake.replace(needle, "x")))
        run(f"{key} model file missing", layer2(key, dict(spec, header="does_not_exist"), files_exist=True,
                                                 constants=base))
    # layer 3: reinstating any withdrawn target as a table row (or constant) is seen
    start = table.index("kResolvedVtableCallSites[] = {")
    first_brace = table.index("{", table.index("=", start))
    for target in WITHDRAWN_TARGETS:
        literal = table[:first_brace + 1] + f"\n    {{0x1, kResolvedVtable_1594ae8, 0x60, 0x{target:x}}}," + table[first_brace + 1:]
        run(f"table row reinstated for {target:#x} (literal)", layer3(literal))
        named = (table[:first_brace + 1] + f"\n    {{0x1, kResolvedVtable_1594ae8, 0x60, kResolvedVtableTarget_{target:x}Rva}},"
                 + table[first_brace + 1:])
        run(f"table row reinstated for {target:#x} (constant)", layer3(named))
        run(f"table constant reinstated for {target:#x}",
            layer3(table + f"\ninline constexpr std::uint64_t kResolvedVtableTarget_{target:x}Rva = 0x{target:x};\n"))
        run(f"audit array without {target:#x}", layer3(table.replace(f"0x{target:x}", "0x0")))
    if img is None:
        return results

    for key, spec in SPECS.items():
        size = spec["size"]
        body = bytearray(img.read(spec["rva"], size))
        run(f"{key} wrong size", safe(layer1, key, dict(spec, size=size + 1), img))
        run(f"{key} wrong rva", safe(layer1, key, dict(spec, rva=spec["rva"] + 0x10), img))
        if spec["sha"]:
            run(f"{key} wrong sha", safe(layer1, key, dict(spec, sha="0" * 64), img))
        # every pattern: flip each of its bytes with the SHA pin off; the instruction check must see it
        for offset, hexbytes in spec["patterns"].items():
            for i in range(len(hexbytes) // 2):
                mutated = bytearray(body)
                mutated[offset + i] ^= 0x01
                run(f"{key} flip byte +{offset + i:#x}",
                    safe(layer1, key, spec, img, body=bytes(mutated), check_sha=False))
        for offset, (target, plt_name) in spec["calls"].items():
            run(f"{key} call +{offset:#x} retargeted",
                safe(layer1, key, dict(spec, calls={**spec["calls"], offset: (target + 0x10, plt_name)}), img))
            if plt_name:
                run(f"{key} call +{offset:#x} wrong PLT name",
                    safe(layer1, key, dict(spec, calls={**spec["calls"], offset: (target, "sceFiosOpBogus")}), img))
        for address, target in spec["slots"]:
            run(f"{key} slot {address:#x} wrong target",
                safe(layer1, key, dict(spec, slots=[(address, target + 0x10)]), img))
            run(f"{key} slot {address:#x} wrong address",
                safe(layer1, key, dict(spec, slots=[(address, target)]), img, slot_override={address: address + 8}))
        if "jump_table" in spec:
            for index in range(4):
                entries = list(spec["jump_table"]["entries"])
                entries[index] += 1
                run(f"{key} jump table entry {index}",
                    safe(layer1, key, dict(spec, jump_table=dict(spec["jump_table"], entries=entries)), img))
    if listing is not None:
        for key in SPECS:
            lines = listing[key]
            pin = DIS_PINS[key]
            run(f"{key} dis: pinned count + 1", layer1_dis(key, (pin[0] + 1, pin[1], pin[2]), lines))
            run(f"{key} dis: an instruction appears", layer1_dis(key, pin, lines + [(lines[-1][0] + 1, "nop")]))
            run(f"{key} dis: an unpinned direct call appears",
                layer1_dis(key, pin, lines + [(lines[-1][0] + 1, "callq 0xf8f7d0 <x>")]))
            run(f"{key} dis: an unpinned indirect call appears",
                layer1_dis(key, pin, lines + [(lines[-1][0] + 1, "callq *0x10(%rax)")]))
            run(f"{key} dis: an unpinned indirect jump appears",
                layer1_dis(key, pin, lines + [(lines[-1][0] + 1, "jmpq *%rax")]))
            for address in pin[1]:
                run(f"{key} dis: call {address:#x} vanishes",
                    layer1_dis(key, pin, [(a, t) for a, t in lines if a != address]))
                run(f"{key} dis: call {address:#x} retargeted",
                    layer1_dis(key, pin, [(a, "callq 0xdead0 <x>" if a == address else t) for a, t in lines]))
            for address, text in pin[2]:
                run(f"{key} dis: indirect {address:#x} changes displacement",
                    layer1_dis(key, pin, [(a, t.replace("0x", "0x1") if a == address else t) for a, t in lines]))
                run(f"{key} dis: indirect {address:#x} vanishes",
                    layer1_dis(key, pin, [(a, t) for a, t in lines if a != address]))
    run("thunk wrong target", safe(layer1_thunk, img, target=0xf8fa30))
    run("thunk wrong name", safe(layer1_thunk, img, plt_name="sceKernelSleep"))
    run("thunk byte flipped", safe(layer1_thunk, img, body=b"\xe8" + img.read(THUNK["rva"], 5)[1:]))
    run("extra slot wrong target", safe(layer1_extra, img, [(0x1595458, 0x316b0)]))
    return results


def main() -> int:
    text = TABLE_HEADER.read_text()
    fails = []
    for key, spec in SPECS.items():
        fails += layer2(key, spec)
    fails += layer3(text)
    have_inputs = ELF.is_file() and PLT.is_file() and FDE_JSON.is_file()
    img = Image() if have_inputs else None
    listing = None
    if img is not None:
        for key, spec in SPECS.items():
            fails += safe(layer1, key, spec, img)
        fails += safe(layer1_thunk, img)
        fails += safe(layer1_extra, img)
    if fails:
        for line in fails:
            print(f"  {line}")
        print(f"FAIL check_frontier_target_models: {len(fails)} fact(s) differ")
        return 1
    results = probes(img, listing)
    accepted = [label for label, rejected in results if not rejected]
    for label in accepted:
        print(f"  probe ACCEPTED a broken fact: {label}")
    if accepted:
        print(f"FAIL check_frontier_target_models: {len(accepted)} of {len(results)} probes slipped")
        return 1
    scope = "ELF + model + policy" if img is not None else "SKIP ELF layer (private inputs absent); model + policy only"
    print(f"OK check_frontier_target_models: {len(SPECS)} bodies and the 0x3b570/0x316a0 thunks ({scope}); "
          f"{len(results)} mutation probes all rejected")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
