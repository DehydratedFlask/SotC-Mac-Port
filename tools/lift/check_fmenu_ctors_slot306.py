#!/usr/bin/env python3
"""ELF-tie for CPU-01-SLOT306-MENU-CTORS-01: debug-menu group / separator / command-item
constructors and the complete registration helper 0x268b0 used by startup slot 306.

Offline only: this script opens the pinned private ELF, reads bytes, and never loads, relocates or
executes an original RVA. It pins:
  * the ELF SHA-256 and the byte hashes of 0x26c10 (group), 0x26b60 (separator), 0x26f90 (command),
    0x268b0 (register), 0x272d0 (item destructor), 0xf1a0 (SharedStr(const char*)) and 0xf180;
  * every store the native lifts reproduce (link header, view copies with `lock inc`, kind, target,
    range {1.0, 10.0}, flags) at fixed instruction offsets, the tail `jmp 0x268b0` of each ctor and the
    `lea` of the parent cell 0x2dfd4b8 they load first;
  * the separator text address 0xf96530 and its content, the 0xf1a0 call and the 0xf180 call;
  * the register control flow: registered test, site A (flags & 0xc0), kind-0 skip, site B
    (flags & 0xf00), the kind-1 flag test (and 0x2c, cmp 0x20), the three assert line numbers and
    message addresses (by SHA-256), the parent branch to 0x27470, and the root list append;
  * the eight slot-306 call sites (3 group, 4 separator, 1 command), their flag immediates and the
    image-wide `call` census of each ctor;
  * the native header/source constants (RVAs, sizes, kinds, masks, lines, message RVAs) and the
    order of stores in native/core/fmenu_ctors_slot306.cpp.

Private: needs local/re/eboot.elf.
"""

from __future__ import annotations

import hashlib
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
RELEASE = 0xF590
PARENT_CELL = 0x2DFD4B8
ROOT_LIST = 0x2DFD4C0
ROOT_GUARD = 0x2DFD4E8
ATEXIT = 0xF8F390
LOG = 0x3E7E0
PARENT_INSERT = 0x27470

# (rva, size, sha256)
FUNCTIONS = {
    "group_ctor": (0x26C10, 0xC2, "cc1c2bcce3014d52d61f88a2ac0b3c6a0666198cba7527d46d3e2ebaf03114fe"),
    "separator_ctor": (0x26B60, 0xA6, "7e0c8b7101e14f02fee4e6c32d4315b2ba2a75e146a922fe9ffac79a97a83629"),
    "command_ctor": (0x26F90, 0xC0, "2f742f14ae3dc569fd1fe11f0959ac41fe48240096c45b1d1f62dddbd4933c29"),
    "register": (0x268B0, 0x2AC, "b963713d3ebb64cd2beb5ae62be4d91a4b4bd32a37f24a9911b2a8d269d46867"),
    "register_code": (0x268B0, 0x2AB, "5bb6d1c7dbf574a8274fad35c3bca0bdcaab592f3b88b92352772cb76af08823"),
    "item_dtor": (0x272D0, 0x19E, "41ea4aa9e998a114e13cd7149228ae5c542debbeb084c1d412a988f2e801c770"),
    "shared_str_ctor": (0xF1A0, 0x97, "954762abfefbff590b8ad52dddf7e1008cadd7944d828ef1159f004553d08946"),
}

# Message strings (NUL included) are pinned by hash; the text itself is not copied into the repo.
ASSERT_TEXT = {
    0xF9665D: (47, "1ac150f98058102fedfe254a4a93f6d60d70d503a8c0e33cfcdbb907cbbc7554"),  # site A
    0xF9668D: (78, "70f3c8f45549bf5a5e377927b90adc8a9e597c197062506eb4a48a66d8dbedba"),  # site B
    0xF966DC: (74, "54a1db1c3407738c4b0c4cc275a2363d7db738a479625884ab23968eda6d2930"),  # site C
}

# slot-306 call sites: (site, callee, flags-immediate bytes preceding the call)
GROUP_SITES = (0xA64986, 0xA64A93, 0xA65066)
SEPARATOR_SITES = (0xA64AF1, 0xA64EEA, 0xA65152, 0xA653C6)
COMMAND_SITE = 0xA65466

# Image-wide census of `E8 rel32` call encodings that land on each ctor (executable window 0..0xf8f000).
CALL_CENSUS = {0x26C10: 151, 0x26B60: 105, 0x26F90: 65, 0x268B0: 2, 0x26EA0: 505, 0x27050: 110}
TAIL_JMP_SITES_TO_REGISTER = [0x268A8, 0x26C01, 0x26CCD, 0x26D9E, 0x26E9A, 0x26F88, 0x2704B, 0x2711B, 0x271ED]


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(f"FAIL {message}")


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def const_value(text: str, name: str) -> int:
    match = re.search(rf"\b{re.escape(name)}\s*=\s*(0x[0-9a-fA-F]+|\d+)\s*;", text)
    require(match is not None, f"missing native constant {name}")
    return int(match.group(1), 0)


class Elf:
    def __init__(self) -> None:
        elf = (ROOT / "local/re/eboot.elf").read_bytes()
        require(digest(elf) == ELF_SHA256, "unexpected ELF SHA256")
        _, ph = d.program_headers(elf)
        self.img = d.Image(elf, ph)

    def read(self, addr: int, size: int) -> bytes:
        return self.img.read(addr, size)

    def rip(self, site: int, length: int, disp_off: int) -> int:
        return site + length + struct.unpack("<i", self.read(site + disp_off, 4))[0]

    def lea(self, site: int) -> int:
        b = self.read(site, 7)
        require(b[0] in (0x48, 0x4C) and b[1] == 0x8D and (b[2] & 0xC7) == 0x05, f"no lea at {site:#x}")
        return self.rip(site, 7, 3)

    def call(self, site: int) -> int:
        b = self.read(site, 5)
        require(b[0] == 0xE8, f"no call at {site:#x}")
        return site + 5 + struct.unpack("<i", b[1:5])[0]

    def jmp(self, site: int) -> int:
        b = self.read(site, 5)
        require(b[0] == 0xE9, f"no jmp at {site:#x}")
        return site + 5 + struct.unpack("<i", b[1:5])[0]

    def bytes_at(self, site: int, hexstr: str, what: str) -> None:
        want = bytes.fromhex(hexstr)
        require(self.read(site, len(want)) == want, f"{what} at {site:#x}")

    def cstring(self, addr: int, text: bytes) -> None:
        require(self.read(addr, len(text) + 1) == text + b"\0", f"string {text!r} at {addr:#x}")


LINK_RDI = ("48c70700000000" "48897f08" "48897f10" "48c7471800000000" "c6472000")
LINK_RBX = ("48c70300000000" "48895b08" "48895b10" "48c7431800000000" "c6432000")
LOCK_INC = "4885c0" "7404" "f0" "48ff00"
RANGE_AND_TARGET0_RDI = ("48c7879000000000000000" "c787980000000000803f" "c7879c00000000002041")


def seq(e: Elf, start: int, hexes: list[str], what: str) -> None:
    """Consecutive instructions starting at `start`."""
    blob = bytes.fromhex("".join(hexes))
    require(e.read(start, len(blob)) == blob, f"{what} at {start:#x}")


def main() -> int:
    e = Elf()

    # ---------------------------------------------------------------- function pins
    for name, (rva, size, sha) in FUNCTIONS.items():
        require(digest(e.read(rva, size)) == sha, f"{name} ({rva:#x}, {size:#x} B) sha256")

    # ---------------------------------------------------------------- group ctor 0x26c10
    # this=rdi, parent view=rsi, caption view=rdx, flags=ecx.
    seq(e, 0x26C10, [LINK_RDI], "group: link header (self=0, prev=next=this, owner=0, registered=0)")
    seq(e, 0x26C2B, ["c5f81006", "c5f8114728", "488b4610", "48894738", "488b4618", "48894740", LOCK_INC],
        "group: parent view (rsi) -> +0x28..+0x47, lock inc ctl")
    seq(e, 0x26C4D, ["c7474801000000"], "group: kind = 1")
    seq(e, 0x26C54, ["c5f81002", "c5f8114750", "488b4210", "48894760", "488b4218", "48894768", LOCK_INC],
        "group: caption view (rdx) -> +0x50..+0x6f, lock inc ctl")
    seq(e, 0x26C76, ["c5f81002", "c5f8114770", "488b4210", "48898780000000", "488b4218", "48898788000000", LOCK_INC],
        "group: caption view (rdx) copied AGAIN -> +0x70..+0x8f, second lock inc")
    require(e.lea(0x26C9E) == PARENT_CELL, "group loads the parent cell address")
    seq(e, 0x26CA5, ["48c7879000000000000000", "c787980000000000803f", "c7879c00000000002041", "898fa0000000", "488b30"],
        "group: target = 0, range {1.0, 10.0}, flags = ecx, rsi = [parent cell]")
    require(e.jmp(0x26CCD) == 0x268B0, "group tail-jumps into 0x268b0")

    # ---------------------------------------------------------------- separator ctor 0x26b60
    # this=rdi->rbx, parent view=rsi, (rdx unused), flags=ecx->r14d.
    seq(e, 0x26B60, ["55", "4889e5", "4156", "53", "4889fb", "4189ce"], "separator prologue: rbx=this, r14d=flags")
    seq(e, 0x26B6D, [LINK_RBX], "separator: link header")
    seq(e, 0x26B88, ["c5f81006", "c5f8114328", "488b4610", "48894338", "488b4618", "48894340", LOCK_INC],
        "separator: parent view (rsi) -> +0x28..+0x47, lock inc ctl")
    seq(e, 0x26BAA, ["488d7b50"], "separator: rdi = this+0x50")
    require(e.lea(0x26BAE) == 0xF96530, "separator: rsi = &\"MenuSeparator\"")
    e.cstring(0xF96530, b"MenuSeparator")
    seq(e, 0x26BB5, ["c7434803000000"], "separator: kind = 3 (stored before the 0xf1a0 call)")
    require(e.call(0x26BBC) == 0xF1A0, "separator calls SharedStr(const char*) 0xf1a0")
    seq(e, 0x26BC1, ["488d7b70"], "separator: rdi = this+0x70")
    require(e.call(0x26BC5) == 0xF180, "separator calls the empty-slot init 0xf180")
    require(e.lea(0x26BCA) == PARENT_CELL, "separator loads the parent cell address")
    seq(e, 0x26BD1, ["48c7839000000000000000", "c783980000000000803f", "c7839c00000000002041", "4489b3a0000000",
                     "4889df", "488b30", "5b", "415e", "5d"],
        "separator: target = 0, range {1.0, 10.0}, flags = r14d, rsi = [parent cell], epilogue")
    require(e.jmp(0x26C01) == 0x268B0, "separator tail-jumps into 0x268b0")

    # ---------------------------------------------------------------- command ctor 0x26f90
    # this=rdi, parent view=rsi, caption view=rdx, target=rcx, command-name view=r8, flags=r9d.
    seq(e, 0x26F90, [LINK_RDI], "command: link header")
    seq(e, 0x26FAB, ["c5f81006", "c5f8114728", "488b4610", "48894738", "488b4618", "48894740", LOCK_INC],
        "command: parent view (rsi) -> +0x28..+0x47, lock inc ctl")
    seq(e, 0x26FCD, ["c7474802000000"], "command: kind = 2")
    seq(e, 0x26FD4, ["c5f81002", "c5f8114750", "488b4210", "48894760", "488b4218", "48894768", LOCK_INC],
        "command: caption view (rdx) -> +0x50..+0x6f, lock inc ctl")
    seq(e, 0x26FF6, ["c4c1781000", "c5f8114770", "498b4010", "48898780000000", "498b4018", "48898788000000", LOCK_INC],
        "command: command-name view (r8) -> +0x70..+0x8f, lock inc ctl")
    require(e.lea(0x2701F) == PARENT_CELL, "command loads the parent cell address")
    seq(e, 0x27026, ["48898f90000000", "c787980000000000803f", "c7879c00000000002041", "44898fa0000000", "488b30"],
        "command: target = rcx, range {1.0, 10.0}, flags = r9d, rsi = [parent cell]")
    require(e.jmp(0x2704B) == 0x268B0, "command tail-jumps into 0x268b0")

    # ---------------------------------------------------------------- register 0x268b0
    seq(e, 0x268D4, ["807b2000", "0f8561020000"], "register: cmp byte [this+0x20],0; jne end")
    seq(e, 0x268DE, ["f683a0000000c0", "4c8d7b48", "7477"], "register: site A test byte [flags],0xc0; r15=&kind; je 0x26962")
    require(e.lea(0x268EB) == 0x2EA5170, "register: engine-state block 0x2ea5170")
    seq(e, 0x268F2, ["488b81b0220000", "483b81b8220000", "740c", "488b80b8020000", "803800", "7454"],
        "register: site A gate: [+0x22b0]==[+0x22b8] -> log; else byte [[+0x22b0]+0x2b8]==0 -> skip")
    require(e.lea(0x2690E) == 0xF9661D and e.lea(0x26915) == 0xF9665D and e.lea(0x26937) == 0xF96543,
            "register: site A function / message / file strings")
    seq(e, 0x2694C, ["c745d0f3000000"], "register: site A line 0xf3")
    require(e.call(0x2695D) == LOG, "register: site A calls the log 0x3e7e0")
    seq(e, 0x26962, ["418b07", "85c0", "0f8411010000"], "register: eax=kind; kind == 0 -> parent test 0x26a7e")
    seq(e, 0x2696D, ["f683a10000000f", "747a"], "register: site B test byte [flags+1],0xf; je 0x269f0")
    require(e.lea(0x26976) == 0x2EA5170, "register: site B engine-state block")
    require(e.lea(0x26999) == 0xF9661D and e.lea(0x269A0) == 0xF9668D, "register: site B function / message")
    seq(e, 0x269D7, ["c745b0f8000000"], "register: site B line 0xf8")
    require(e.call(0x269E8) == LOG, "register: site B calls the log 0x3e7e0")
    seq(e, 0x269ED, ["8b4348", "83f801", "0f8585000000"], "register: reload kind; kind != 1 -> parent test (kind 2 and 3 have no branch)")
    seq(e, 0x269F9, ["8b83a0000000", "83e02c", "83f820", "7477"], "register: kind-1 flags: and 0x2c, cmp 0x20, je skip")
    require(e.lea(0x26A07) == 0x2EA5170, "register: site C engine-state block")
    require(e.lea(0x26A2A) == 0xF9661D and e.lea(0x26A31) == 0xF966DC, "register: site C function / message")
    seq(e, 0x26A68, ["c74590fe000000"], "register: site C line 0xfe")
    require(e.call(0x26A79) == LOG, "register: site C calls the log 0x3e7e0")
    seq(e, 0x26A7E, ["4d85f6", "7418", "488d7328", "4c89f7", "4c89fa"],
        "register: test parent; je root; args (parent, &item+0x28, &item+0x48)")
    require(e.call(0x26A8D) == PARENT_INSERT, "register: parent branch calls 0x27470")
    seq(e, 0x26A92, ["c6432001", "e9a4000000"], "register: parent branch sets registered = 1, then end")
    require(e.rip(0x26A9B, 6, 2) == ROOT_GUARD, "register: root guard byte 0x2dfd4e8")
    require(e.lea(0x26AA5) == ROOT_GUARD, "register: guard acquire argument")
    require(e.lea(0x26AB5) == 0x27280 and e.lea(0x26ABC) == 0x1604218 and e.lea(0x26AC3) == ROOT_LIST,
            "register: root init atexit(0x27280, root, dso)")
    require(e.call(0x26AFC) == ATEXIT, "register: root init __cxa_atexit")
    seq(e, 0x26B0D, ["488d05ac69dd02", "48891b", "48894310", "488b0da669dd02", "48894b08", "48891d9b69dd02",
                     "488b4b08", "48895910", "ff05a569dd02", "48894318"],
        "register: tail append (self, next=&root, prev=root.prev, root.prev=this, prev.next=this, ++count, owner=&root)")
    require(e.rip(0x26B0D, 7, 3) == ROOT_LIST and e.rip(0x26B35, 6, 2) == 0x2DFD4E0, "register: root and count cells")

    # ---------------------------------------------------------------- assert strings / destructor
    for addr, (length, sha) in ASSERT_TEXT.items():
        text = e.read(addr, 200).split(b"\0")[0]
        require(len(text) == length and digest(text + b"\0") == sha, f"assert text at {addr:#x}")
    # Destructor shape the call sites register (pinned, not executed): releases views C, B, A in that order.
    seq(e, 0x27438, ["488d7b70"], "item destructor: view C (+0x70) released first")
    require(e.call(0x2743C) == RELEASE, "item destructor releases view C")
    seq(e, 0x27441, ["488d7b50"], "item destructor: view B (+0x50)")
    require(e.call(0x27445) == RELEASE, "item destructor releases view B")
    seq(e, 0x2744A, ["4883c328", "4889df"], "item destructor: view A (+0x28) last")
    require(e.call(0x27451) == RELEASE, "item destructor releases view A")
    seq(e, 0x2738B, ["48833b00", "0f84"], "item destructor: unlink only when link.self != 0")

    # ---------------------------------------------------------------- slot-306 call sites
    for site in GROUP_SITES:
        require(e.call(site) == 0x26C10, f"group call site {site:#x}")
        window = e.read(site - 0x40, 0x40)
        require(bytes.fromhex("b930000000") in window, f"group site {site:#x} flags ecx = 0x30")
    for site in SEPARATOR_SITES:
        require(e.call(site) == 0x26B60, f"separator call site {site:#x}")
        window = e.read(site - 0x30, 0x30)
        require(bytes.fromhex("31d2") in window and bytes.fromhex("b930000000") in window,
                f"separator site {site:#x}: edx = 0 and ecx = 0x30")
    require(e.call(COMMAND_SITE) == 0x26F90, "command call site")
    window = e.read(COMMAND_SITE - 0x70, 0x70)
    require(bytes.fromhex("41b930000000") in window, "command site flags r9d = 0x30")
    require(e.lea(0xA653E5) == 0x2DFCBD0, "command site target pointer 0x2dfcbd0")
    e.cstring(0x12C4E58, b"Reload All Sound Data")
    e.cstring(0x12C4C31, b"sndx_reloadData")
    e.cstring(0x12C4D1B, b"Sound")
    # The eight sites are the only calls of these three ctors inside the slot-306 body [0xa633b0, 0xa654bd).
    body = e.read(0xA633B0, 0xA654BD - 0xA633B0)
    for tgt, expected in ((0x26C10, GROUP_SITES), (0x26B60, SEPARATOR_SITES), (0x26F90, (COMMAND_SITE,))):
        seen = []
        for i in range(len(body) - 4):
            if body[i] == 0xE8:
                t = 0xA633B0 + i + 5 + struct.unpack("<i", body[i + 1:i + 5])[0]
                if t == tgt:
                    seen.append(0xA633B0 + i)
        require(tuple(seen) == tuple(expected), f"slot-306 callers of {tgt:#x}: {[hex(x) for x in seen]}")

    # ---------------------------------------------------------------- image-wide census
    window = e.read(0, 0xF8F000)
    census = {t: 0 for t in CALL_CENSUS}
    i = 0
    while True:
        i = window.find(b"\xe8", i)
        if i < 0 or i + 5 > len(window):
            break
        t = i + 5 + struct.unpack("<i", window[i + 1:i + 5])[0]
        if t in census:
            census[t] += 1
        i += 1
    require(census == CALL_CENSUS, f"call census {census}")
    jumps = []
    i = 0
    while True:
        i = window.find(b"\xe9", i)
        if i < 0 or i + 5 > len(window):
            break
        if i + 5 + struct.unpack("<i", window[i + 1:i + 5])[0] == 0x268B0:
            jumps.append(i)
        i += 1
    require(jumps == TAIL_JMP_SITES_TO_REGISTER, f"tail-jump census to 0x268b0 {[hex(x) for x in jumps]}")

    # ---------------------------------------------------------------- native tie
    hpp = (ROOT / "native/core/fmenu_ctors_slot306.hpp").read_text()
    cpp = (ROOT / "native/core/fmenu_ctors_slot306.cpp").read_text()
    expected_consts = {
        "kMenuGroupCtorRva": 0x26C10, "kMenuGroupCtorSize": 0xC2,
        "kMenuSeparatorCtorRva": 0x26B60, "kMenuSeparatorCtorSize": 0xA6,
        "kMenuCommandCtorRva": 0x26F90, "kMenuCommandCtorSize": 0xC0,
        "kMenuRegisterSize": 0x2AC, "kMenuRegisterCodeEndRva": 0x26B5B,
        "kMenuSeparatorTextRva": 0xF96530,
        "kMenuKindGroup": 1, "kMenuKindCommand": 2, "kMenuKindSeparator": 3, "kMenuSlot306Flags": 0x30,
        "kMenuAssertFlagsMaskA": 0xC0, "kMenuAssertFlagsMaskB": 0xF00,
        "kMenuKind1FlagsMask": 0x2C, "kMenuKind1FlagsOk": 0x20,
        "kMenuAssertLineA": 0xF3, "kMenuAssertLineB": 0xF8, "kMenuAssertLineC": 0xFE,
        "kMenuAssertTextRvaA": 0xF9665D, "kMenuAssertTextRvaB": 0xF9668D, "kMenuAssertTextRvaC": 0xF966DC,
    }
    for name, value in expected_consts.items():
        require(const_value(hpp, name) == value, f"native constant {name}")
    require('kMenuSeparatorText[] = "MenuSeparator"' in hpp, "native separator text")
    require(0x268B0 + const_value(hpp, "kMenuRegisterSize") - 1 == const_value(hpp, "kMenuRegisterCodeEndRva"),
            "register code end == start + size - 1 pad byte")

    def order(text: str, *needles: str) -> None:
        pos = -1
        for needle in needles:
            nxt = text.find(needle, pos + 1)
            require(nxt > pos, f"native order: {needle!r} after previous")
            pos = nxt

    group_body = cpp[cpp.index("bool init_menu_group_26c10"):cpp.index("bool init_menu_separator_26b60")]
    order(group_body, "link_header(item)", "item.label = parent", "concmd_acquire_view(parent)",
          "item.kind = kMenuKindGroup", "item.text = caption", "concmd_acquire_view(caption)",
          "item.empty = caption", "concmd_acquire_view(caption)", "tail_stores(item, nullptr, flags)")
    sep_body = cpp[cpp.index("bool init_menu_separator_26b60"):cpp.index("bool init_menu_command_26f90")]
    order(sep_body, "init_shared_str_f1a0", "link_header(item)", "item.label = parent",
          "item.kind = kMenuKindSeparator", "item.text = text", "init_slot_f180(&item.empty)", "tail_stores(item, nullptr, flags)")
    cmd_body = cpp[cpp.index("bool init_menu_command_26f90"):cpp.index("// One gated-log site")]
    order(cmd_body, "link_header(item)", "item.label = parent", "item.kind = kMenuKindCommand", "item.text = caption",
          "item.empty = command", "tail_stores(item, target, flags)")
    reg_body = cpp[cpp.index("MenuRegisterResult register_menu_item_268b0"):]
    order(reg_body, "item.registered != 0", "kMenuAssertFlagsMaskA", "item.kind != 0", "kMenuAssertFlagsMaskB",
          "kMenuKindGroup && (item.flags & kMenuKind1FlagsMask)", "menu.parent.parent != nullptr",
          "kDebugMenuRootDtorRva", "++root.count", "node->owner = rootp")

    print("check_fmenu_ctors_slot306: PASS (static, ELF-tied; nothing executed)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
