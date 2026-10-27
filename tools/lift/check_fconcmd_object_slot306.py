#!/usr/bin/env python3
"""ELF-tie the console-COMMAND object that startup slot 306 (0xa633b0..0xa654bd) builds three times.

Offline only: this script opens the pinned private ELF, reads bytes, and never loads, relocates or
executes an original RVA. It pins:
  * the slot-306 body (size, sha256, table word 306);
  * the command class: vtable 0x1594dc0 (zero header words, six RELATIVE slots), the destructor 0x4b0
    and deleting destructor 0x22a60 (structure: vptr, vector 0x80/0x88 released +0x28 then +0x08 per
    0x50 B record, storage free 0x170c0, tail jump to the base destructor 0x20b50), the invoke thunk
    0x1990 (`mov rax,[rdi+0x18]; mov rdi,rsi; mov rsi,rdx; jmp rax`) reached through the RELATIVE cell
    0x15fe5e0, and the bound-call head of the execute slot 0x21da0;
  * the base destructor 0x20b50 (manager erase under lock +0x58, pending-list guard/unlink, description
    +0x40 then name +0x20 releases) and the manager erase 0x213b0 (strcmp walk, `dec [mgr+0x28]`,
    node free through 0x42bc0/0x42f50/0xf590/0x170c0);
  * for each of the three command sites: the name/description strings and lengths, the object address,
    the handler identity, the stack staging, the base-constructor call (flags 2), the vptr and delegate
    stores, the vector zero stores, the indirect register call (`call r14` with r14 = [0x1594dc0+0x20]),
    the two inert 0xf590 releases and the `__cxa_atexit(0x4b0, obj, dso)` call, plus a sha256 over the
    whole inline sequence;
  * a decode census of every `lea` of the vtable 0x1594dc0 / the destructor 0x4b0 and every `vmovq`
    load of the thunk cell 0x15fe5e0 in the executable segment;
  * the native header (constants, site table) cross-checked against the bytes above.

Private: needs local/re/eboot.elf. The census is a pattern decode (REX.W lea / VEX vmovq with RIP-relative
operand), not a disassembly; it documents how widely the class is shared, it does not prove completeness.
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
TABLE_START = 0x2DD70E0
TEXT_END = 22624632  # phdr 0 filesz
DSO = 0x1604218
ATEXIT = 0xF8F390
RELEASE = 0xF590
VTABLE = 0x1594DC0
VPTR = 0x1594DD0
THUNK_CELL = 0x15FE5E0
MANAGER_CELL = 0x2DFCBC8

# (name, rva, size, sha256)
FUNCTIONS = {
    "slot306": (0xA633B0, 0xA654BD - 0xA633B0, "f8f707c7f80129cb79d2d54a61b6fd53caa62bf920c36f18a5b93152ff9933c8"),
    "cmd_dtor": (0x4B0, 0x71, "a15bae38e545531a8c5769a079333bd4e0385846160e10a4b768827ecb0d8c8c"),
    "cmd_deleting_dtor": (0x22A60, 0x79, "7862319966c5c08c943dccefd7d27aa28cf80abe0a83bbf08c1361577fade8fd"),
    "invoke_thunk": (0x1990, 0xC, "c5eca1d65aea48ea6feaa7c41abef0c4f4d7bc1212e753f2d12da5b89285fd12"),
    "vslot3_ret0": (0x22AE0, 0x3, "251447ee91a9067dcd6ab96703133f617565974cd6c4819021760c4688c91abf"),
    "execute_head": (0x21DA0, 0x60, "dcf59e46f1a2c3bae4f1b1dfe06fdac9434856f585d15907993097271310fb17"),
    "manager_erase": (0x213B0, 0xCD, "ce88afdd8c05549b8376739c35c0c4ddf5eb014d9cc9aee75da297c473e12f0f"),
    "base_dtor": (0x20B50, 0x155, "f372ca669a915db1f4c9861bb7fcc3de546320c6ed3625bda474f4adb0dc6706"),
    "base_ctor": (0x20AF0, 0x5A, "e9dbf86d6bb1496a9b34cbf1968ba5cc6909e06d1e5a902cb936704efedf0b45"),
    "register_guard": (0x20D60, 0x1A, "f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403"),
    "register_body": (0x20D80, 0x188, "f59869f13c36fe90ed647d1a642006ec48be8676d992175fd959a97346cc41fb"),
}

VTABLE_SLOTS = (0x4B0, 0x22A60, 0x20D60, 0x22AE0, 0x21DA0, 0x21F80)

# Census (pattern decode over the executable segment): count and a digest of the little-endian site list.
CENSUS = {
    "lea vtable 0x1594dc0": (53, "c07234f447a5b5e4"),
    "lea dtor 0x4b0": (106, "3ad0f9d80ca24bdb"),
    "vmovq thunk cell": (40, "9cbd4b15753cc463"),
}

# Per site: layout A (site 1, reloads the thunk cell and the vtable) or B (sites 2/3 reuse r12/ymm).
# Fields: name, name_len, name_addr, desc, desc_len, desc_addr, obj, handler, name_call, ctor_call, reg_call,
#         atexit_call, layout, inline-sequence sha256 over [name_call-0xc, atexit_call+5).
SITES = (
    ("sndx_reloadData", 15, 0x12C4C31,
     "Reloads all currently loaded banks and the sound table. Params: None", 0x44, 0x12C4C41,
     0x32F9F68, 0xA5BF20, 0xA64660, 0xA646BC, 0xA64725, 0xA6474D, "A",
     "4537f52864cf62fcd85b6732da996113a104b3135c0e772ede914df3efa9dcaf"),
    ("sndx_categoryMuteAll", 20, 0x12C4C86,
     "Mutes/Unmutes all categories. Params: <bool mute>", 0x31, 0x12C4C9B,
     0x32FA008, 0xA5BF50, 0xA64763, 0xA647B7, 0xA64811, 0xA64839, "B",
     "0f3160f9f87e2c4db82042157112859124d3c9ebab4f84433c1971750b07cb4b"),
    ("sndx_categoryFilterAll", 22, 0x12C4CCD,
     "Filter/Unfilter all categories. Params: <bool filter>", 0x35, 0x12C4CE4,
     0x32FA0A8, 0xA5C020, 0xA6484F, 0xA648A3, 0xA648FD, 0xA64928, "B",
     "a5743b50a9c0eb6df1279614ed39d09a9387a2b2b97a1e2201c3cf422c09fcfd"),
)


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
        self.relocs = {r["target"]: r for r in d.parse(elf)["relocs"]}

    def read(self, addr: int, size: int) -> bytes:
        return self.img.read(addr, size)

    def rip(self, site: int, length: int, disp_off: int) -> int:
        raw = self.read(site + disp_off, 4)
        return site + length + struct.unpack("<i", raw)[0]

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

    def reloc_target(self, slot_addr: int) -> int:
        r = self.relocs.get(slot_addr)
        require(r is not None and r["type"] == 8, f"no RELATIVE relocation at {slot_addr:#x}")
        return r["addend"]

    def bytes_at(self, site: int, hexstr: str, what: str) -> None:
        want = bytes.fromhex(hexstr)
        require(self.read(site, len(want)) == want, f"{what} at {site:#x}")

    def cstring(self, addr: int, text: bytes) -> None:
        require(self.read(addr, len(text) + 1) == text + b"\0", f"string {text!r} at {addr:#x}")


def census(text: bytes, pattern: bytes, length: int, disp_off: int, target: int) -> list[int]:
    sites = []
    for m in re.finditer(pattern, text, re.S):
        s = m.start()
        if s + length > len(text):
            continue
        if s + length + struct.unpack("<i", text[s + disp_off:s + disp_off + 4])[0] == target:
            sites.append(s)
    return sites


def site_digest(e: Elf, site: tuple) -> str:
    name_call, atexit_call = site[8], site[11]
    return digest(e.read(name_call - 0xC, atexit_call + 5 - (name_call - 0xC)))


def main() -> int:
    e = Elf()
    print_pins = "--print-pins" in sys.argv

    if print_pins:
        for site in SITES:
            print(site[0], site_digest(e, site))
        return 0

    # ---------------------------------------------------------------- function pins
    for name, (rva, size, sha) in FUNCTIONS.items():
        require(digest(e.read(rva, size)) == sha, f"{name} ({rva:#x}) sha256")
    require(struct.unpack("<Q", e.read(TABLE_START - 8 * 306, 8))[0] == 0xA633B0, "table word 306 is 0xa633b0")

    # ---------------------------------------------------------------- vtable and thunk cell
    require(e.read(VTABLE, 16) == b"\0" * 16, "vtable header words (offset-to-top, rtti) are zero")
    for i, slot in enumerate(VTABLE_SLOTS):
        require(e.reloc_target(VPTR + 8 * i) == slot, f"command vtable slot {i}")
    require(VTABLE + 0x10 == VPTR, "address point is vtable start + 0x10")
    require(VTABLE + 0x20 == VPTR + 0x10, "register call [0x1594dc0+0x20] is slot 2")
    require(e.reloc_target(VPTR + 8 * 2) == 0x20D60, "slot 2 is the already-native register guard 0x20d60")
    require(e.reloc_target(THUNK_CELL) == 0x1990, "thunk cell 0x15fe5e0 is a RELATIVE relocation to 0x1990")
    e.bytes_at(0x1990, "488b4718" "4889f7" "4889d6" "ffe0",
               "thunk: mov rax,[rdi+0x18]; mov rdi,rsi; mov rsi,rdx; jmp rax")
    e.bytes_at(0x22AE0, "31c0c3", "vtable slot 3: xor eax,eax; ret")
    # base class vtable and base ctor
    require(e.reloc_target(0x1594D90) == 0x20B50, "base vtable slot 0 is the base destructor 0x20b50")
    require(e.reloc_target(0x1594DA0) == 0x20D60, "base vtable slot 2 is the register guard")
    require(e.lea(0x20AF0) == 0x1594D80, "base ctor vtable 0x1594d80 (+0x10 = 0x1594d90)")
    e.bytes_at(0x20AF7, "4883c010488907c6471800894f1c", "base ctor: vptr, registered=0, flags=ecx")

    # ---------------------------------------------------------------- destructor 0x4b0 and deleting dtor
    require(e.lea(0x4BA) == VTABLE, "dtor sets vptr from 0x1594dc0")
    e.bytes_at(0x4C4, "4883c010" "498906", "dtor: rax += 0x10; [this] = rax")
    e.bytes_at(0x4CB, "498b9e80000000" "4d8bbe88000000", "dtor: begin=[this+0x80], end=[this+0x88]")
    e.bytes_at(0x4E0, "488d7b28", "dtor: record +0x28 first")
    require(e.call(0x4E4) == RELEASE, "dtor releases record +0x28 through 0xf590")
    e.bytes_at(0x4E9, "488d7b08", "dtor: record +0x08 second")
    require(e.call(0x4ED) == RELEASE, "dtor releases record +0x08 through 0xf590")
    e.bytes_at(0x4F2, "4883c350", "dtor: record stride 0x50")
    e.bytes_at(0x4FB, "498b9e80000000", "dtor: reload begin")
    e.bytes_at(0x502, "4885db" "7408", "dtor: test begin; je skip")
    require(e.call(0x50A) == 0x170C0, "dtor frees the vector storage through 0x170c0")
    require(e.jmp(0x51C) == 0x20B50, "dtor tail-jumps to the base destructor 0x20b50")
    require(e.lea(0x22A6A) == VTABLE, "deleting dtor sets vptr from 0x1594dc0")
    require(e.call(0x22A94) == RELEASE and e.call(0x22A9D) == RELEASE, "deleting dtor record releases")
    require(e.call(0x22ABA) == 0x170C0, "deleting dtor frees the vector storage")
    require(e.call(0x22AC2) == 0x20B50, "deleting dtor calls the base destructor")
    require(e.jmp(0x22AD4) == 0x170B0, "deleting dtor ends with the object free 0x170b0")

    # ---------------------------------------------------------------- base destructor 0x20b50
    require(e.lea(0x20B66) == 0x1594D80, "base dtor sets the base vptr")
    require(e.lea(0x20B6D) == MANAGER_CELL, "base dtor loads the manager cell")
    e.bytes_at(0x20B9C, "4c8d7b58", "base dtor: lock = manager + 0x58")
    require(e.call(0x20BC1) == 0x3B390 and e.call(0x20BCD) == 0x213B0 and e.call(0x20BD5) == 0x3B3C0,
            "base dtor: lock / erase 0x213b0 / unlock")
    e.bytes_at(0x20BC6, "498d7620", "base dtor: erase key = this + 0x20")
    require(e.rip(0x20BDA, 6, 2) == 0x2DFCE60, "base dtor pending-list guard byte")
    require(e.call(0x20BEB) == 0xF8F3D0 and e.call(0x20C14) == 0xF8F3E0, "base dtor guard acquire / release")
    require(e.lea(0x20BF4) == 0x2DFCE50, "base dtor pending-list head")
    e.bytes_at(0x20C42, "498d5608", "base dtor: node = this + 8")
    e.bytes_at(0x20C64, "498b4608" "498b4e10" "488901" "48894808",
               "base dtor: unlink {next=[this+8], prev=[this+0x10]}")
    e.bytes_at(0x20C73, "498d7e40", "base dtor: description at +0x40")
    require(e.call(0x20C77) == RELEASE and e.call(0x20C83) == RELEASE, "base dtor releases description then name")
    e.bytes_at(0x20C7C, "4983c620", "base dtor: name at +0x20")

    # ---------------------------------------------------------------- manager erase 0x213b0
    e.bytes_at(0x213C5, "498b4718", "erase: root = manager + 0x18")
    e.bytes_at(0x213C9, "4d8d7708", "erase: header = manager + 0x8")
    require(e.call(0x213EB) == 0xF8F500 and e.call(0x2141E) == 0xF8F500, "erase compares keys with strcmp")
    e.bytes_at(0x213E3, "498b7c2420", "erase: node key data at node + 0x20")
    e.bytes_at(0x213F4, "4d8b2424", "erase: left child at node + 0")
    e.bytes_at(0x21400, "498b4424" "08", "erase: right child at node + 8")
    e.bytes_at(0x21431, "49ff4f28", "erase: dec qword [manager + 0x28]")
    require(e.call(0x21438) == 0x42BC0 and e.call(0x21443) == 0x42F50, "erase unlinks the node (0x42bc0 / 0x42f50)")
    e.bytes_at(0x21448, "498d7c2420", "erase: key SharedStr at node + 0x20")
    require(e.call(0x2144D) == RELEASE, "erase releases the key through 0xf590")
    require(e.call(0x2145F) == 0x170C0, "erase frees the node through 0x170c0")
    e.bytes_at(0x21452, "b801000000", "erase returns 1 when a node was removed")
    e.bytes_at(0x2142A, "31c0", "erase returns 0 when absent")

    # ---------------------------------------------------------------- execute slot: bound-call head
    e.bytes_at(0x21DC6, "488b7b60" "488b4368" "4885ff" "750f", "execute: object=[+0x60], ptr=[+0x68]")
    e.bytes_at(0x21DD3, "41be01000000" "4885c0" "0f84" "70010000", "execute: both zero -> status 1")
    e.bytes_at(0x21DE2, "48037b70" "a801" "7408" "488b0f" "488b4401ff",
               "execute: add adj; ptr&1 virtual lookup")
    e.bytes_at(0x21DF2, "4889de" "4c89fa" "ffd0", "execute: rdi=object, rsi=cmd, rdx=args; call rax")

    # ---------------------------------------------------------------- per-site pins
    for (name, name_len, name_addr, desc, desc_len, desc_addr, obj, handler,
         name_call, cc, rc, ac, layout, want_digest) in SITES:
        tag = f"site {name}"
        e.cstring(name_addr, name.encode())
        require(len(name) == name_len, f"{tag} name length")
        e.cstring(desc_addr, desc.encode())
        require(len(desc) == desc_len, f"{tag} description length equals the stack immediate")
        require(e.call(name_call) == 0xF2F0, f"{tag} name view through 0xf2f0")
        require(e.lea(name_call - 0xC) == name_addr, f"{tag} name string lea")
        b = 5 if layout == "B" else 0
        require(e.lea(cc - 0x4F + b) == desc_addr, f"{tag} description lea")
        require(e.lea(cc - 0x44 + b) == obj, f"{tag} object lea (rdi)")
        e.bytes_at(cc - 0x38 + b, "b902000000", f"{tag} flags ecx = 2")
        require(e.lea(cc - 0x26 + b) == handler, f"{tag} handler identity lea {handler:#x}")
        e.bytes_at(cc - 0x1F + b, "48c744244" "8" + struct.pack("<I", desc_len).hex(),
                   f"{tag} description length immediate")
        require(e.call(cc) == 0x20AF0, f"{tag} base constructor 0x20af0")
        if layout == "A":
            require(e.rip(name_call + 5, 8, 4) == THUNK_CELL, f"{tag} loads the thunk cell")
            e.bytes_at(name_call + 5, "c5fa7e0d", f"{tag} vmovq xmm1,[cell]")
            require(e.lea(cc + 5) == VTABLE, f"{tag} vptr from vtable 0x1594dc0")
            e.bytes_at(cc + 0x12, "4c8d6010", f"{tag} r12 = vtable + 0x10")
            require(e.rip(cc + 0x16, 7, 3) == obj, f"{tag} [obj] = vptr")
            e.bytes_at(cc + 0x16, "4c8925", f"{tag} vptr store opcode")
            require(e.rip(cc + 0x1D, 8, 4) == obj + 0x60, f"{tag} ymm delegate store at obj+0x60")
            e.bytes_at(cc + 0x1D, "c5fc1105", f"{tag} vmovups ymmword store opcode")
            require(e.lea(cc + 0x36) == obj + 0x60 and e.rip(cc + 0x3D, 7, 3) == obj + 0x60,
                    f"{tag} self fix-up [obj+0x60] = &delegate")
            e.bytes_at(cc + 0x44, "4c8b7020", f"{tag} r14 = [vtable + 0x20]")
            require(e.lea(cc + 0x48) == obj, f"{tag} rdi = object for the register call")
            require(e.rip(cc + 0x56, 8, 4) == obj + 0x80, f"{tag} xmm zero store at obj+0x80")
            require(e.rip(cc + 0x5E, 11, 3) == obj + 0x90, f"{tag} qword zero store at obj+0x90")
            require(rc == cc + 0x69, f"{tag} register call position")
        else:
            require(e.rip(cc + 0x0B, 7, 3) == obj, f"{tag} [obj] = vptr (r12)")
            e.bytes_at(cc + 0x0B, "4c8925", f"{tag} vptr store opcode")
            require(e.rip(cc + 0x12, 8, 4) == obj + 0x60, f"{tag} ymm delegate store at obj+0x60")
            require(e.lea(cc + 0x2B) == obj + 0x60 and e.rip(cc + 0x32, 7, 3) == obj + 0x60,
                    f"{tag} self fix-up [obj+0x60] = &delegate")
            require(e.lea(cc + 0x39) == obj, f"{tag} rbx = object for the register call")
            require(e.rip(cc + 0x47, 8, 4) == obj + 0x80, f"{tag} xmm zero store at obj+0x80")
            require(e.rip(cc + 0x4F, 11, 3) == obj + 0x90, f"{tag} qword zero store at obj+0x90")
            require(rc == cc + 0x5A, f"{tag} register call position")
        e.bytes_at(rc, "41ffd6", f"{tag} call r14")
        require(e.call(rc + 8) == RELEASE and e.call(rc + 0x12) == RELEASE, f"{tag} inert releases")
        e.bytes_at(rc + 3, "488d7c2440", f"{tag} description view at rsp+0x40")
        e.bytes_at(rc + 0x0D, "488d7c2460", f"{tag} name view at rsp+0x60")
        if name_len == 22:
            require(e.lea(rc + 0x17) == DSO and e.lea(rc + 0x1E) == 0x4B0, f"{tag} dso / destructor leas")
        else:
            require(e.lea(rc + 0x17) == 0x4B0 and e.lea(rc + 0x1E) == DSO, f"{tag} destructor / dso leas")
        require(e.call(ac) == ATEXIT, f"{tag} __cxa_atexit")
        require(digest(e.read(name_call - 0xC, ac + 5 - (name_call - 0xC))) == want_digest,
                f"{tag} inline sequence sha256")

    # the object globals are one 0xa0 stride apart and the three leas of the vtable/thunk are shared
    objs = [s[6] for s in SITES]
    require(objs[1] - objs[0] == 0xA0 and objs[2] - objs[1] == 0xA0, "objects are 0xa0 apart")

    # ---------------------------------------------------------------- census
    text = e.read(0, TEXT_END)
    lea_pat = rb"[\x48\x4c]\x8d[\x05\x0d\x15\x1d\x25\x2d\x35\x3d]"
    found = {
        "lea vtable 0x1594dc0": census(text, lea_pat, 7, 3, VTABLE),
        "lea dtor 0x4b0": census(text, lea_pat, 7, 3, 0x4B0),
        "vmovq thunk cell": census(text, rb"\xc5\xfa\x7e[\x05\x0d\x15\x1d\x25\x2d\x35\x3d]", 8, 4, THUNK_CELL),
    }
    for key, (count, sha) in CENSUS.items():
        sites = found[key]
        got = hashlib.sha256(b"".join(struct.pack("<I", s) for s in sites)).hexdigest()[:16]
        require(len(sites) == count and got == sha, f"census {key}: {len(sites)} sites digest {got}")
    require(0xA646C1 in found["lea vtable 0x1594dc0"], "slot-306 vtable lea is in the census")
    for site in SITES:
        dtor_lea = site[10] + (0x1E if site[0] == "sndx_categoryFilterAll" else 0x17)
        require(dtor_lea in found["lea dtor 0x4b0"], f"slot-306 {site[0]} destructor lea is in the census")
    in_slot = [x for x in found["lea dtor 0x4b0"] if 0xA633B0 <= x < 0xA654BD]
    require(len(in_slot) >= 3, "slot 306 body has the three destructor leas (and more for later sites)")

    # ---------------------------------------------------------------- native header cross-check
    hpp = (ROOT / "native/core/fconcmd_object_slot306.hpp").read_text()
    for cname, value in (
        ("kFConCmdVtableStart", VTABLE), ("kFConCmdVptr", VPTR), ("kFConCmdVtableRegisterOffset", 0x20),
        ("kFConCmdDtorRva", 0x4B0), ("kFConCmdDtorSize", 0x71), ("kFConCmdDeletingDtorRva", 0x22A60),
        ("kFConCmdDeletingDtorSize", 0x79), ("kFConCmdInvokeThunkRva", 0x1990), ("kFConCmdInvokeThunkSize", 0xC),
        ("kFConCmdThunkCellRva", THUNK_CELL), ("kFConCmdExecuteRva", 0x21DA0),
        ("kFConCmdExecuteBoundCallRva", 0x21DC6), ("kConCmdManagerEraseRva", 0x213B0),
        ("kConCmdManagerEraseSize", 0xCD), ("kConCmdManagerLockOffset", 0x58),
        ("kConCmdManagerMapSizeOffset", 0x28), ("kFConCmdFreeStorageRva", 0x170C0), ("kFConCmdFlags", 2),
        ("kFConCmdObjectSize", 0xA0), ("kFConCmdArgRecordSize", 0x50),
    ):
        require(const_value(hpp, cname) == value, f"native constant {cname} == {value:#x}")
    slots = re.search(r"kFConCmdVtableSlots\{([^}]*)\}", hpp)
    require(slots is not None, "native vtable slot list")
    require(tuple(int(x, 16) for x in re.findall(r"0x[0-9a-fA-F]+", slots.group(1))) == VTABLE_SLOTS,
            "native vtable slot list equals the relocations")
    table = re.search(r"kSlot306CommandSites\{\{(.*?)\}\};", hpp, re.S)
    require(table is not None, "native site table")
    rows = re.findall(r'\{"(\w+)",\s*(\d+),\s*"([^"]*)",\s*(\d+),\s*(0x[0-9a-f]+),\s*(0x[0-9a-f]+),\s*'
                      r"(0x[0-9a-f]+),\s*(0x[0-9a-f]+),\s*(0x[0-9a-f]+),\s*(0x[0-9a-f]+),\s*(0x[0-9a-f]+),\s*"
                      r"(0x[0-9a-f]+)\}", table.group(1))
    require(len(rows) == 3, "three native site rows")
    for row, site in zip(rows, SITES):
        want = (site[0], str(site[1]), site[3], str(site[4]), site[2], site[5], site[6], site[7],
                site[8], site[9], site[10], site[11])
        got = (row[0], row[1], row[2], row[3]) + tuple(int(x, 16) for x in row[4:])
        want = (want[0], want[1], want[2], want[3]) + tuple(want[4:])
        require(got == want, f"native site row for {site[0]}: {got} != {want}")

    cpp = (ROOT / "native/core/fconcmd_object_slot306.cpp").read_text()
    require("kFConCmdFlags" in cpp and "kConCmdRegisterRva" in hpp + cpp, "native source uses the pinned constants")

    print(
        "OK check_fconcmd_object_slot306: slot 306 body "
        f"{FUNCTIONS['slot306'][2][:12]}..; vtable 0x1594dc0 (6 RELATIVE slots, slot 2 = 0x20d60); dtor 0x4b0 / "
        "deleting 0x22a60 / base 0x20b50 / erase 0x213b0 / thunk 0x1990 / execute head 0x21dc6; 3 sites "
        "(15/20/22-char names, descriptions 0x44/0x31/0x35, objects 0xa0 apart); census "
        f"{len(found['lea vtable 0x1594dc0'])} vtable leas, {len(found['lea dtor 0x4b0'])} dtor leas, "
        f"{len(found['vmovq thunk cell'])} thunk-cell loads"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
