#!/usr/bin/env python3
"""ELF-tie startup constructor slots 298 (0xb0e250), 299 (0xb0c7a0) and 301 (0xac9550).

Offline only: this script opens the pinned private ELF, reads bytes, and never loads, relocates or
executes an original RVA. It pins:
  * the three slot bodies (size, sha256, table words) and every decoded site the native lifts rely
    on: the stack views, the object addresses, the vtable/functor tags, the immediate stores and the
    call targets, in measured order;
  * the shared ConCmd base (0x20af0), the register guard/body (0x20d60 / 0x20d80), the manager
    constructor (0x210b0), its single installer (0x242c0 / store 0x243f5), and the census sites of
    the manager cell 0x2dfcbc8 and the debug-menu parent cell 0x2dfd4b8 (every site is a `lea`);
  * the bool, enum and float variable constructors and the destructors registered with atexit;
  * the functor vtables (RELATIVE relocations) and their four slot bodies, including the formerly
    unresolved indirect call: `call [0x15d5960 + 0x18]` is the bare `ret` at 0xb0e240;
  * the debug-menu item constructors 0x27050 / 0x26ea0, the registration 0x268b0 and the two
    virtual slots of the bool class (0xb890 -> 1, 0xb930 -> 0);
  * the native headers/sources (constants, order of stores, rodata strings) and the three
    production rows in native/core/startup_ctors.cpp.

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
TABLE_START = 0x2DD70E0
DSO = 0x1604218
ATEXIT = 0xF8F390
RELEASE = 0xF590

# (name, rva, size, sha256)
FUNCTIONS = {
    "slot298": (0xB0E250, 0x14B, "39d29b11d646143ad05449aaace5bffa9e9d30bdbb3befef448d582d832dddea"),
    "slot299": (0xB0C7A0, 0x39E, "634d3052556cc2c858c8a2ddefe95b50400df94c9a409414ef9b943c1b2f2462"),
    "slot301": (0xAC9550, 0x143, "b4b9f2d542c6b633bc05bab0e1fbb32494b86eb46334a7623c049dfa3522ea04"),
    "concmd_base_ctor": (0x20AF0, 0x5A, "e9dbf86d6bb1496a9b34cbf1968ba5cc6909e06d1e5a902cb936704efedf0b45"),
    "concmd_base_dtor": (0x20B50, 0x155, "f372ca669a915db1f4c9861bb7fcc3de546320c6ed3625bda474f4adb0dc6706"),
    "concmd_register": (0x20D60, 0x1A, "f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403"),
    "concmd_register_body": (0x20D80, 0x188, "f59869f13c36fe90ed647d1a642006ec48be8676d992175fd959a97346cc41fb"),
    "bool_dtor": (0x20B0, 0x36, "73f646e91ca2f7d8dd14cf1858c93197eddbf00a4267f6ca36ea00b2500efe42"),
    "float_dtor": (0x20F0, 0x36, "e93fb896a676df342779b680fc43ca88aa08aaab78eece0b260f1b6b93df5579"),
    "enum_dtor": (0xB0CB40, 0x36, "af00a6ff143adbaf5090011afc28191d71680cfd3f546cbce38142c419d99309"),
    "ranged_enum_ctor": (0xB0E060, 0x143, "222eca63c6368da970ca2af3a2353bd8bbdfeeba2ad412409e2d0162b87b411f"),
    "ranged_float_ctor": (0xC300, 0x160, "b0fad1ce8145048f5faf081eb6e4a03f97f9b8eb1cdaa44a76ff41e94f5f442f"),
    "enum_fn_copy": (0xB0E1F0, 0x14, "93065bdb89a418e3d5e9ef9803389ed60166b3a4f0a21036cd1f8ab851081d52"),
    "enum_fn_clone": (0xB0E210, 0x14, "7b35df9f7a7dea016098a69f926902670b796a2010c86d8e35921a0766345e1e"),
    "enum_fn_move": (0xB0E230, 0x9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    "enum_fn_destroy": (0xB0E240, 0x1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
    "float_fn_copy": (0xC4A0, 0x14, "0137c6226a7dcb380747d73f16a87fab37c63e9c2b8f67d0d1a8f875f2d44a08"),
    "float_fn_clone": (0xC4C0, 0x14, "5f93b13fd6577d64a7242762148d8d994a40ebd9480009297cce40f30caea597"),
    "float_fn_move": (0xC4E0, 0x9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    "float_fn_destroy": (0xC4F0, 0x1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
    "bool_fn_copy": (0xC2A0, 0x14, "b355d74a6edc25ac52cd52ef1de89719ec412b8ea6414e806f80cea7a60e5693"),
    "bool_fn_clone": (0xC2C0, 0x14, "047e110aeff5fe3000666c334afa7c4e6c799a779125a7608c000e1eed1a7474"),
    "bool_fn_move": (0xC2E0, 0x9, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    "bool_fn_destroy": (0xC2F0, 0x1, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
    "menu_float_ctor": (0x27050, 0xD0, "ad7ca05e3df035fa5df336f0463fb63e331566f140d6f0bf3ad16e9e2766b053"),
    "menu_toggle_ctor": (0x26EA0, 0xED, "c2c7d8c17db68783d9e94100c4f032f3bf06c1ac7d12d5f48e0e34e06a924677"),
    "menu_register": (0x268B0, 0x2AC, "b963713d3ebb64cd2beb5ae62be4d91a4b4bd32a37f24a9911b2a8d269d46867"),
    "menu_root_getter": (0x27200, 0x7F, "6b3bc6caaafa6e37a9491916fe6413c0d92d90c60f10c71b3280f7349902eb77"),
    "menu_root_dtor": (0x27280, 0x44, "4eed08318c1629ffc029af22924ed8bd818dd9f10fdca8319055410b0d072dcd"),
    "menu_item_dtor": (0x272D0, 0x19E, "41ea4aa9e998a114e13cd7149228ae5c542debbeb084c1d412a988f2e801c770"),
    "bool_slot3": (0xB890, 0x6, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
    "bool_slot6": (0xB930, 0x3, "251447ee91a9067dcd6ab96703133f617565974cd6c4819021760c4688c91abf"),
    "manager_ctor": (0x210B0, 0x111, "619fba45d65cab01645eb1f8f8015c7585aeec31d4afe7f3b35ff2422e17ff40"),
    "manager_installer": (0x242C0, 0x149, "52d9f830e08640c9d33c7f4511e7a321494f7975e2e2ad11132d1ecb36bbd7ed"),
}

# Every image reference to the manager cell 0x2dfcbc8 (28) is a 7-byte `lea r64,[rip+disp]`.
MANAGER_CELL = 0x2DFCBC8
MANAGER_CELL_SITES = [
    0x536, 0xAB9, 0x1304, 0x12553, 0x1B00A, 0x1B043, 0x1E011, 0x20B6D, 0x20D67, 0x2163C, 0x22594,
    0x227F4, 0x235EF, 0x241A7, 0x243B6, 0x29D26, 0x2AA2C, 0x4B887, 0x3ABBC1, 0x3C1666, 0x3C85DD,
    0x3C8632, 0x3C8911, 0x3C904A, 0x3E215D, 0x534923, 0x57518F, 0x68A162,
]
# Every image reference to the debug-menu parent cell 0x2dfd4b8 (18) is a `lea` as well.
PARENT_CELL = 0x2DFD4B8
PARENT_CELL_SITES = [
    0x2686A, 0x26BCA, 0x26C9E, 0x26D6E, 0x26E83, 0x26F71, 0x2701F, 0x270E3, 0x271B5, 0x29A0D, 0x2FCF0,
    0xBCE86, 0x2551B8, 0x2558BC, 0x256EE1, 0x2618E3, 0x5D4444, 0x7D8935,
]


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


def main() -> int:
    e = Elf()

    # ---------------------------------------------------------------- function pins
    for name, (rva, size, sha) in FUNCTIONS.items():
        body = e.read(rva, size)
        require(digest(body) == sha, f"{name} ({rva:#x}) sha256")

    # ---------------------------------------------------------------- table words
    for index, rva in ((298, 0xB0E250), (299, 0xB0C7A0), (301, 0xAC9550)):
        require(struct.unpack("<Q", e.read(TABLE_START - 8 * index, 8))[0] == rva, f"table word {index}")

    # ---------------------------------------------------------------- manager / parent cell census
    for site in MANAGER_CELL_SITES:
        require(e.lea(site) == MANAGER_CELL, f"manager cell site {site:#x} is not a lea of 0x2dfcbc8")
    require(len(set(MANAGER_CELL_SITES)) == 28, "28 distinct manager cell sites")
    for site in PARENT_CELL_SITES:
        require(e.lea(site) == PARENT_CELL, f"parent cell site {site:#x} is not a lea of 0x2dfd4b8")
    require(len(set(PARENT_CELL_SITES)) == 18, "18 distinct parent cell sites")
    # The only ctor-table entries among those sites: 0x235d0 (index 567) holds the manager-cell lea at
    # 0x235ef and zeroes it; 0x2fcf0 (index 565) zeroes the parent cell. Neither is below index 302.
    require(struct.unpack("<Q", e.read(TABLE_START - 8 * 567, 8))[0] == 0x235D0, "index 567 is 0x235d0")
    require(struct.unpack("<Q", e.read(TABLE_START - 8 * 565, 8))[0] == 0x2FCF0, "index 565 is 0x2fcf0")
    e.bytes_at(0x23612, "49c70600000000", "0x235d0 stores 0 into the manager cell (mov qword [r14],0)")
    e.bytes_at(0x2FD05, "48c70600000000", "0x2fcf0 stores 0 into the parent cell (mov qword [rsi],0)")
    require(e.lea(0x2FCF0) == PARENT_CELL, "0x2fcf0 lea of the parent cell")
    # The single non-zero store of the manager cell: `mov [r15], r14` inside 0x242c0, whose only
    # caller is 0x255119 (engine boot). 0x242c0 is not a ctor-table entry.
    e.bytes_at(0x241E1, "49c70700000000", "0x240f0 teardown stores 0 into the manager cell (mov qword [r15],0)")
    e.bytes_at(0x243F5, "4d8937", "manager install store mov [r15],r14")
    require(e.lea(0x243B6) == MANAGER_CELL, "installer lea of the manager cell")
    require(e.call(0x243B1) == 0x210B0, "installer calls the manager constructor 0x210b0")
    require(e.call(0x243A6) == 0x17070, "installer allocates through 0x17070")
    e.bytes_at(0x243A1, "bf60000000", "installer allocation size 0x60")
    require(e.call(0x255119) == 0x242C0, "0x242c0 has its caller at 0x255119")
    # The manager constructor drains the pending list by calling 0x20d80 for each queued node (node-8).
    require(e.call(0x21166) == 0x20D80, "manager ctor drains the pending list through 0x20d80")
    require(e.lea(0x21111) == 0x2DFCE50 and e.lea(0x21101) == 0x2DFCE60, "manager ctor pending head/guard")

    # ---------------------------------------------------------------- shared base: 0x20af0
    base = e.read(0x20AF0, 0x5A)
    require(e.lea(0x20AF0) == 0x1594D80, "base ctor vtable 0x1594d80 (+0x10 = 0x1594d90)")
    e.bytes_at(0x20AF7, "4883c010488907c6471800894f1c", "base ctor: vptr, registered=0, flags=ecx")
    e.bytes_at(0x20B05, "c5f81006" "c5f8114720" "488b4610" "48894730" "488b4618" "48894738",
               "base ctor: name view copy @+0x20/+0x30/+0x38")
    e.bytes_at(0x20B1E, "4885c0" "7404" "f0" "48ff00", "base ctor: lock inc qword [rax] on the name control counter")
    e.bytes_at(0x20B27, "c5f81002" "c5f8114740" "488b4210" "48894750" "488b4218" "48894758",
               "base ctor: description view copy @+0x40/+0x50/+0x58")
    e.bytes_at(0x20B40, "4885c0" "7404" "f0" "48ff00",
               "base ctor: lock inc qword [rax] on the description control counter")
    require(base[-1] == 0xC3, "base ctor ends in ret")
    require(e.reloc_target(0x1594D90) == 0x20B50, "base vtable slot 0 is the base destructor 0x20b50")
    require(e.reloc_target(0x1594DA0) == 0x20D60, "base vtable slot 2 is the register-once 0x20d60")

    # ---------------------------------------------------------------- register: 0x20d60 / 0x20d80
    e.bytes_at(0x20D60, "807f1800" "7401" "c3", "register: cmp byte [rdi+0x18],0; je; ret")
    e.bytes_at(0x20D6E, "c6471801", "register: registered = 1")
    require(e.lea(0x20D67) == MANAGER_CELL, "register loads the manager cell")
    require(e.jmp(0x20D75) == 0x20D80, "register jumps into the body")
    # null-manager path: guard 0x2dfce60, head 0x2dfce50/58, tail link.
    require(e.rip(0x20E7F, 6, 2) == 0x2DFCE60, "register null path guard byte")
    require(e.call(0x20E90) == 0xF8F3D0 and e.call(0x20EB9) == 0xF8F3E0, "register null path guard acquire/release PLT")
    require(e.lea(0x20E99) == 0x2DFCE50, "register null path list head")
    e.bytes_at(0x20ECC, "498d5608" "49894e10" "49894608",
               "register null path: rdx=this+8; [this+0x10]=tail; [this+8]=&head")
    require(e.rip(0x20ED8, 7, 3) == 0x2DFCE58, "register null path head.prev = node")
    e.bytes_at(0x20EDF, "488911", "register null path tail->next = node")
    # manager path markers: lock/unlock around the map insert, strcmp, operator[].
    require(e.call(0x20DE2) == 0x3B390 and e.call(0x20E71) == 0x3B3C0, "register manager path mutex")
    require(e.call(0x20E1A) == 0xF8F500, "register manager path strcmp")
    require(e.call(0x20E61) == 0x211D0, "register manager path map operator[]")
    e.bytes_at(0x20DBC, "498d7d58", "register manager path locks manager+0x58")

    # ---------------------------------------------------------------- bool variable shape (slot 301)
    s301 = 0xAC9550
    require(e.lea(s301 + 0x18) == 0x1594208, "301 final vtable 0x1594208 (+0x10 = 0x1594218)")
    require(e.lea(s301 + 0x1F) == 0x33054A0, "301 object 0x33054a0")
    require(e.lea(s301 + 0x26) == 0x12CDDE0, "301 name string")
    require(e.lea(s301 + 0x2D) == 0x12CDF29, "301 empty description")
    e.cstring(0x12CDDE0, b"rumbleEnabled")
    require(e.read(0x12CDF29, 1) == b"\0", "301 description points at a NUL byte")
    e.bytes_at(s301 + 0x62, "48c745980d000000", "301 name length 13")
    e.bytes_at(s301 + 0x6A, "c5f81145b8c5f81145a0", "301 stack view zeroing")
    e.bytes_at(s301 + 0x74, "48891dd5be8302", "301 [obj] = final vtable before the ctor call")
    require(e.rip(s301 + 0x74, 7, 3) == 0x33054A0, "301 pre-ctor vptr store target")
    require(e.call(s301 + 0x83) == 0x20AF0, "301 call base ctor 0x20af0")
    e.bytes_at(s301 + 0x40, "b901000000", "301 flags ecx = 1")
    require(e.lea(s301 + 0x88) == 0x15940A0, "301 mid vtable 0x15940a0 (+0x10 = 0x15940b0)")
    require(e.lea(s301 + 0x92) == 0x3305560, "301 value cell 0x3305560 (= obj+0xc0)")
    require(e.lea(s301 + 0x9D) == 0x15D5070, "301 functor vtable tag 0x15d5070")
    require(e.rip(s301 + 0xA8, 7, 3) == 0x33054A0, "301 [obj] = mid vptr")
    require(e.rip(s301 + 0xAF, 7, 3) == 0x3305500, "301 [obj+0x60] = &value")
    require(e.rip(s301 + 0xB6, 10, 2) == 0x3305509, "301 dword [obj+0x69] = 0")
    e.bytes_at(s301 + 0xB6, "c705f9be830200000000", "301 dword zero store")
    require(e.rip(s301 + 0xC0, 8, 4) == 0x3305510, "301 ymm zero [obj+0x70]")
    require(e.rip(s301 + 0xC8, 11, 3) == 0x3305540, "301 qword [obj+0xa0] = 0")
    require(e.rip(s301 + 0xD3, 7, 3) == 0x3305550, "301 [obj+0xb0] = functor vtable")
    require(e.rip(s301 + 0xDA, 11, 3) == 0x3305558, "301 qword [obj+0xb8] = 0")
    require(e.call(s301 + 0xE5) == RELEASE, "301 release of the description view")
    require(e.rip(s301 + 0xED, 7, 3) == 0x33054A0, "301 [obj] = final vptr")
    require(e.rip(s301 + 0xF4, 7, 2) == 0x3305560, "301 byte [obj+0xc0] = 1")
    e.bytes_at(s301 + 0xF4, "c605", "301 value store opcode")
    require(e.read(s301 + 0xFA, 1) == b"\x01", "301 value = 1")
    require(e.rip(s301 + 0xFB, 7, 2) == 0x3305508, "301 byte [obj+0x68] = 1")
    require(e.read(s301 + 0x101, 1) == b"\x01", "301 default = 1")
    require(e.call(s301 + 0x102) == 0x20D60, "301 register")
    require(e.call(s301 + 0x10A) == RELEASE, "301 release of the name view")
    require(e.lea(s301 + 0x10F) == 0x20B0, "301 atexit destructor 0x20b0")
    require(e.lea(s301 + 0x116) == DSO, "301 dso handle")
    require(e.call(s301 + 0x120) == ATEXIT, "301 __cxa_atexit")

    # ---------------------------------------------------------------- bool variable shape (slot 299 block B)
    s299 = 0xB0C7A0
    require(e.lea(0xB0C8F9) == 0x1594208 and e.lea(0xB0C900) == 0x3305B08, "299 B vtable / object")
    require(e.lea(0xB0C916) == 0x12CFB15, "299 B name")
    e.cstring(0x12CFB15, b"globalDebugTextAdjustFor4K")
    e.bytes_at(0xB0C934, "48c74424481a000000", "299 B name length 0x1a")
    require(e.call(0xB0C95D) == 0x20AF0, "299 B base ctor")
    require(e.lea(0xB0C962) == 0x15940A0 and e.lea(0xB0C96C) == 0x3305BC8 and e.lea(0xB0C977) == 0x15D5800,
            "299 B mid vtable / value cell / functor tag 0x15d5800")
    require(e.rip(0xB0C982, 7, 3) == 0x3305B08, "299 B [obj] = mid vptr")
    require(e.rip(0xB0C989, 7, 3) == 0x3305B68, "299 B [obj+0x60] = &value")
    require(e.rip(0xB0C990, 10, 2) == 0x3305B71, "299 B dword [obj+0x69] = 0")
    require(e.rip(0xB0C99A, 8, 4) == 0x3305B78, "299 B ymm zero [obj+0x70]")
    require(e.rip(0xB0C9A2, 11, 3) == 0x3305BA8, "299 B [obj+0xa0] = 0")
    require(e.rip(0xB0C9AD, 7, 3) == 0x3305BB8, "299 B [obj+0xb0] = functor vtable")
    require(e.rip(0xB0C9B4, 11, 3) == 0x3305BC0, "299 B [obj+0xb8] = 0")
    require(e.call(0xB0C9BF) == RELEASE, "299 B release description")
    require(e.rip(0xB0C9C4, 7, 3) == 0x3305B08, "299 B [obj] = final vptr")
    require(e.rip(0xB0C9CE, 7, 2) == 0x3305BC8 and e.read(0xB0C9D4, 1) == b"\x01", "299 B value = 1")
    require(e.rip(0xB0C9D5, 7, 2) == 0x3305B70 and e.read(0xB0C9DB, 1) == b"\x01", "299 B default = 1")
    require(e.call(0xB0C9DF) == 0x20D60 and e.call(0xB0C9E7) == RELEASE, "299 B register / release name")
    require(e.lea(0xB0C9EC) == 0x20B0 and e.call(0xB0C9FC) == ATEXIT, "299 B atexit 0x20b0")
    # vtable facts for the bool class
    require(e.reloc_target(0x1594218) == 0x20B0, "bool class slot 0 = 0x20b0")
    require(e.reloc_target(0x1594218 + 0x18) == 0xB890, "bool class slot 3 = 0xb890")
    require(e.reloc_target(0x1594218 + 0x30) == 0xB930, "bool class slot 6 = 0xb930")
    e.bytes_at(0xB890, "b801000000c3", "bool slot 3: mov eax,1; ret")
    e.bytes_at(0xB930, "31c0c3", "bool slot 6: xor eax,eax; ret")
    e.bytes_at(0x20B0 + 0x10, "488dbb90000000", "bool dtor: functor base this+0x90")
    e.bytes_at(0x20B0 + 0x1E, "488b83b0000000ff5018", "bool dtor: call [[this+0xb0]+0x18]")
    require(e.jmp(0x20B0 + 0x31) == 0x20B50, "bool dtor tail-calls the base destructor")

    # ---------------------------------------------------------------- functor vtables and slot bodies
    def check_fn_vtable(base_addr: int, bodies: tuple[int, int, int, int], tag: str, store_vt: int) -> None:
        for i, body in enumerate(bodies):
            require(e.reloc_target(base_addr + 8 * i) == body, f"{tag} functor vtable slot {i}")
        # copy / clone store `dst+0x10 = src+0x10; dst+0x20 = <vtable>`; move copies the payload only;
        # destroy is a bare ret.
        copy_body = e.read(bodies[0], 0x14)
        require(copy_body.startswith(bytes.fromhex("488b4610")) and copy_body.endswith(bytes.fromhex("c3")),
                f"{tag} copy body shape")
        require(e.lea(bodies[0] + 4) == store_vt, f"{tag} copy stores vtable {store_vt:#x}")
        clone_body = e.read(bodies[1], 0x14)
        require(clone_body.endswith(bytes.fromhex("c3")), f"{tag} clone body shape")
        require(e.lea(bodies[1]) == store_vt, f"{tag} clone stores vtable {store_vt:#x}")
        e.bytes_at(bodies[2], "488b461048894710c3", f"{tag} move body")
        e.bytes_at(bodies[3], "c3", f"{tag} destroy is a bare ret")

    enum_bodies = (0xB0E1F0, 0xB0E210, 0xB0E230, 0xB0E240)
    float_bodies = (0xC4A0, 0xC4C0, 0xC4E0, 0xC4F0)
    bool_bodies = (0xC2A0, 0xC2C0, 0xC2E0, 0xC2F0)
    check_fn_vtable(0x15D5960, enum_bodies, "enum 0x15d5960", 0x15D5960)
    check_fn_vtable(0x15D57E0, float_bodies, "float arg 0x15d57e0", 0x1594318)
    check_fn_vtable(0x1594318, float_bodies, "float class 0x1594318", 0x1594318)
    check_fn_vtable(0x15D5070, bool_bodies, "bool 0x15d5070", 0x1594280)
    check_fn_vtable(0x15D5800, bool_bodies, "bool 0x15d5800", 0x1594280)
    check_fn_vtable(0x1594280, bool_bodies, "bool class 0x1594280", 0x1594280)
    # all destroy slots share the same single byte
    require(len({FUNCTIONS[k][2] for k in ("enum_fn_destroy", "float_fn_destroy", "bool_fn_destroy")}) == 1,
            "the three destroy slots are the same bare ret")

    # ---------------------------------------------------------------- slot 298 and 0xb0e060
    s298 = 0xB0E250
    require(e.lea(s298 + 0x1F) == 0x15D58E8, "298 final vtable 0x15d58e8 (+0x10 = 0x15d58f8)")
    require(e.lea(s298 + 0x26) == 0x3305D20, "298 object 0x3305d20")
    require(e.lea(s298 + 0x2D) == 0x12CFBE0, "298 name string")
    e.cstring(0x12CFBE0, b"shaderTypeToLoad")
    require(e.lea(s298 + 0x3D) == 0x15D5960, "298 argument functor vtable 0x15d5960")
    require(e.lea(s298 + 0x74) == 0x12CFCE7, "298 empty description")
    require(e.read(0x12CFCE7, 1) == b"\0", "298 description points at a NUL byte")
    e.bytes_at(s298 + 0x95, "48c744240810000000", "298 name length 0x10")
    e.bytes_at(s298 + 0x51, "b901000000", "298 flags ecx = 1")
    e.bytes_at(s298 + 0x8A, "4889b42480000000", "298 [rsp+0x80] = 0x15d5960 (argument functor vtable)")
    e.bytes_at(s298 + 0xA7, "48c784248800000000000000", "298 [rsp+0x88] = 0 (argument functor aux)")
    e.bytes_at(s298 + 0x7B, "48c7442470" "00000000", "298 [rsp+0x70] = 0 (argument functor payload)")
    e.bytes_at(s298 + 0x84, "c5fc114c2440", "298 argument block zero qwords")
    e.bytes_at(s298 + 0xA4, "4c892b", "298 [obj] = r13 (final vptr) before the ctor")
    e.bytes_at(s298 + 0xB8, "488d93d0000000", "298 value cell = obj+0xd0")
    require(e.call(s298 + 0xCE) == 0xB0E060, "298 call 0xb0e060")
    require(e.call(s298 + 0xD6) == RELEASE, "298 release description view")
    e.bytes_at(s298 + 0xDB, "488b842480000000" "4c89f7" "ff5018", "298 indirect call [[rsp+0x80]+0x18]")
    e.bytes_at(s298 + 0xEC, "4c892b" "c783d000000000000000" "c7436800000000",
               "298 [obj]=r13; dword [obj+0xd0]=0; dword [obj+0x68]=0")
    require(e.call(s298 + 0x100) == 0x20D60, "298 register")
    require(e.call(s298 + 0x108) == RELEASE, "298 release name view")
    require(e.lea(s298 + 0x10D) == 0xB0CB40 and e.lea(s298 + 0x114) == DSO and e.call(s298 + 0x11E) == ATEXIT,
            "298 atexit 0xb0cb40")
    # the indirect call target, from the relocation table: slot 3 of 0x15d5960 is 0xb0e240 = ret.
    require(e.reloc_target(0x15D5960 + 0x18) == 0xB0E240, "298 indirect call resolves to 0xb0e240")
    e.bytes_at(0xB0E240, "c3", "the resolved target is a bare ret")
    # 0xb0e060
    r = 0xB0E060
    require(e.call(r + 0x2C) == 0x20AF0, "0xb0e060 base ctor call")
    require(e.lea(r + 0x31) == 0x15D5870, "0xb0e060 mid vtable 0x15d5870 (+0x10 = 0x15d5880)")
    require(e.lea(r + 0x3C) == 0x15D5960, "0xb0e060 functor vtable")
    e.bytes_at(r + 0x4A, "4c897b60" "c6436c00" "c6437000" "c6437400" "c6437800",
               "0xb0e060 [+0x60]=value cell; bytes +0x6c/+0x70/+0x74/+0x78 = 0")
    e.bytes_at(r + 0x5E, "c5fc118380000000" "48898bc0000000" "48c783b000000000000000" "48c783c800000000000000",
               "0xb0e060 ymm zero +0x80; [+0xc0]=vtable; [+0xb0]=0; [+0xc8]=0")
    e.bytes_at(r + 0x83, "c4c178104608" "c5f8118388000000" "498b06" "48898380000000" "498b4618" "48898398000000",
               "0xb0e060 argument qword copies")
    e.bytes_at(r + 0xA6, "4885c0" "740a" "4889d8" "4883e880" "488900",
               "0xb0e060 q[3] != 0 makes [+0x80] self-referential")
    e.bytes_at(r + 0xB5, "498b4648" "498d7620" "488945a0" "498b4640", "0xb0e060 reads args aux and vtable")
    e.bytes_at(r + 0xCF, "ff5008", "0xb0e060 call [vtbl+8] (clone)")
    e.bytes_at(r + 0xD2, "488b4598", "0xb0e060 loads tmp vtable")
    e.bytes_at(r + 0xE0, "ff10", "0xb0e060 slot 0 copy (tmp2 <- tmp1)")
    e.bytes_at(r + 0xF6, "ff10", "0xb0e060 slot 0 copy (tmp1 <- this+0xa0)")
    e.bytes_at(r + 0x102, "ff10", "0xb0e060 slot 0 copy (this+0xa0 <- tmp2)")
    e.bytes_at(r + 0x104, "488b83c8000000" "488b4da0" "4c89f7" "48898bc8000000" "488945a0",
               "0xb0e060 swaps aux words")
    e.bytes_at(r + 0x121, "ff5018", "0xb0e060 destroy tmp1 (slot 3 = bare ret)")
    require(e.reloc_target(0x15D58F8) == 0xB0CB40, "enum class slot 0 = 0xb0cb40")
    require(e.reloc_target(0x15D5880) == 0xB0D4F0, "enum mid class slot 0")
    e.bytes_at(0xB0CB40 + 0x1E, "488b83c0000000ff5018", "enum dtor: call [[this+0xb0+0x10]+0x18]")
    e.bytes_at(0xB0CB40 + 0x10, "488dbba0000000", "enum dtor: functor base this+0xa0")
    require(e.jmp(0xB0CB40 + 0x31) == 0x20B50, "enum dtor tail-calls the base destructor")

    # ---------------------------------------------------------------- float variable 0xc300 and slot 299 block A
    c = 0xC300
    require(e.call(c + 0x36) == 0x20AF0, "0xc300 base ctor call")
    require(e.lea(c + 0x3B) == 0x1594118, "0xc300 mid vtable 0x1594118 (+0x10 = 0x1594128)")
    require(e.lea(c + 0x46) == 0x1594318, "0xc300 functor vtable 0x1594318")
    e.bytes_at(c + 0x54, "4c896360" "c6436c00" "c6437001" "418b07" "89436c" "c6437400" "c6437801" "418b06" "894374",
               "0xc300: value cell; min byte0/engaged=1/dword min; max byte0/engaged=1/dword max")
    e.bytes_at(c + 0x74, "c5fc118380000000" "48898bc0000000" "48c783b000000000000000" "48c783c800000000000000",
               "0xc300 ymm zero +0x80; [+0xc0]=vtable; [+0xb0]=0; [+0xc8]=0")
    e.bytes_at(c + 0x99, "c4c178104508" "c5f8118388000000" "498b4500" "48898380000000" "498b4518" "48898398000000",
               "0xc300 argument qword copies")
    e.bytes_at(c + 0xBD, "4885c0" "740a" "4889d8" "4883e880" "488900", "0xc300 q[3] self-reference")
    e.bytes_at(c + 0xE6, "ff5008", "0xc300 call [vtbl+8] (clone)")
    e.bytes_at(c + 0xF7, "ff10", "0xc300 slot 0 copy (tmp2 <- tmp1)")
    e.bytes_at(c + 0x138, "ff5018", "0xc300 destroy tmp1 (slot 3 = bare ret)")
    require(e.reloc_target(0x15942B0) == 0x20F0, "float class slot 0 = 0x20f0")
    require(e.reloc_target(0x1594128) == 0xBB20, "float mid class slot 0")
    e.bytes_at(0x20F0 + 0x10, "488dbba0000000", "float dtor: functor base this+0xa0")
    e.bytes_at(0x20F0 + 0x1E, "488b83c0000000ff5018", "float dtor: call [[this+0xc0]+0x18]")

    require(e.lea(0xB0C7BF) == 0x15942A0, "299 A final vtable 0x15942a0 (+0x10 = 0x15942b0)")
    require(e.lea(0xB0C7C6) == 0x12CFB00, "299 A name")
    e.cstring(0x12CFB00, b"globalDebugTextScale")
    require(e.lea(0xB0C7D1) == 0x3305A30 and e.lea(0xB0C7ED) == 0x3305B00, "299 A object / value cell")
    require(e.lea(0xB0C7E1) == 0x12CFB84, "299 A empty description")
    require(e.read(0x12CFB84, 1) == b"\0", "299 A description points at a NUL byte")
    e.bytes_at(0xB0C819, "48c744242814000000", "299 A name length 0x14")
    require(e.lea(0xB0C82C) == 0x15D57E0, "299 A argument functor vtable 0x15d57e0")
    e.bytes_at(0xB0C84A, "c744241ccdcccc3d" "c744241800002041", "299 A min 0.1f (0x3dcccccd), max 10.0f (0x41200000)")
    e.bytes_at(0xB0C7F9, "41b901000000", "299 A flags r9d = 1")
    e.bytes_at(0xB0C86A, "488d4c241c", "299 A rcx = &min")
    e.bytes_at(0xB0C7F4, "4c8d442418", "299 A r8 = &max")
    e.bytes_at(0xB0C833, "48c784249000000000000000", "299 A argument functor payload 0")
    e.bytes_at(0xB0C875, "48898424a0000000" "48c78424a800000000000000", "299 A argument functor vtable / aux")
    e.bytes_at(0xB0C88E, "4155" "50", "299 A stack arguments: description, argument block")
    require(e.rip(0xB0C85A, 7, 3) == 0x3305A30, "299 A pre-ctor vptr store")
    require(e.call(0xB0C891) == 0xC300, "299 A call 0xc300")
    require(e.call(0xB0C89D) == RELEASE, "299 A release description")
    e.bytes_at(0xB0C8AA, "488dbc2480000000" "ff5018", "299 A indirect call on the argument functor")
    require(e.rip(0xB0C8B8, 7, 3) == 0x3305A30, "299 A [obj] = final vptr")
    require(e.rip(0xB0C8BF, 10, 2) == 0x3305B00 and e.read(0xB0C8C5, 4) == bytes.fromhex("0000803f"),
            "299 A value = 1.0f")
    require(e.rip(0xB0C8C9, 10, 2) == 0x3305A98 and e.read(0xB0C8CF, 4) == bytes.fromhex("0000803f"),
            "299 A default = 1.0f")
    require(e.call(0xB0C8D3) == 0x20D60 and e.call(0xB0C8DB) == RELEASE, "299 A register / release name")
    require(e.lea(0xB0C8E0) == 0x20F0 and e.lea(0xB0C8E7) == DSO and e.call(0xB0C8F4) == ATEXIT,
            "299 A atexit 0x20f0")

    # ---------------------------------------------------------------- debug-menu items (299 C and D)
    require(e.lea(0xB0CA01) == 0x12CFB30 and e.lea(0xB0CA08) == 0x12CFB36, "299 C label / text")
    e.cstring(0x12CFB30, b"Debug")
    e.cstring(0x12CFB36, b"Global Debug Text Scale")
    require(e.lea(0xB0CA1B) == 0x3305BD0 and e.lea(0xB0CA2F) == 0x3305A30, "299 C item / target A")
    require(e.rip(0xB0CA13, 8, 4) == 0x12CFACC and e.rip(0xB0CA5E, 8, 4) == 0x12CFAC8, "299 C range constants")
    require(e.read(0x12CFAC8, 4) == bytes.fromhex("cdcccc3d") and e.read(0x12CFACC, 4) == bytes.fromhex("0000803f"),
            "299 C range 0.1f / 1.0f")
    e.bytes_at(0xB0CA2C, "4531c0", "299 C flags r8d = 0")
    e.bytes_at(0xB0CA3B, "48c744246805000000" , "299 C label length 5")
    e.bytes_at(0xB0CA49, "48c744244817000000", "299 C text length 0x17")
    require(e.call(0xB0CA6F) == 0x27050, "299 C call 0x27050")
    require(e.call(0xB0CA77) == RELEASE and e.call(0xB0CA7F) == RELEASE, "299 C release text / label")
    e.bytes_at(0xB0CA74, "4c89f7", "299 C first release is the text view (r14)")
    e.bytes_at(0xB0CA7C, "4889df", "299 C second release is the label view (rbx)")
    require(e.lea(0xB0CA84) == 0x272D0 and e.call(0xB0CA94) == ATEXIT, "299 C atexit 0x272d0")
    require(e.lea(0xB0CA9E) == 0x3305C78 and e.lea(0xB0CAAF) == 0x12CFB4E and e.lea(0xB0CAC0) == 0x3305B08,
            "299 D item / text / target B")
    e.cstring(0x12CFB4E, b"Global Debug Text Scale Adjust for 4k")
    e.bytes_at(0xB0CABA, "41b830000000", "299 D flags r8d = 0x30")
    e.bytes_at(0xB0CADE, "48c744244825000000", "299 D text length 0x25")
    require(e.call(0xB0CAF3) == 0x26EA0, "299 D call 0x26ea0")
    require(e.call(0xB0CAFB) == RELEASE and e.call(0xB0CB03) == RELEASE, "299 D release text / label")
    e.bytes_at(0xB0CAF8, "4c89ff", "299 D first release is the text view (r15)")
    e.bytes_at(0xB0CB00, "4889df", "299 D second release is the label view (rbx)")
    require(e.call(0xB0CB11) == ATEXIT, "299 D atexit")

    # 0x27050 / 0x26ea0 stores
    m = 0x27050
    e.bytes_at(m + 0x1D, "48c70300000000" "48895b08" "48895b10" "48c7431800000000" "c6432000",
               "0x27050 link {0, this, this, 0}; registered = 0")
    e.bytes_at(m + 0x38, "c5f81006" "c5f8114328" "488b4610" "48894338" "488b4618" "48894340",
               "0x27050 label view copy @+0x28")
    e.bytes_at(m + 0x5A, "c7434802000000", "0x27050 kind = 2")
    e.bytes_at(m + 0x61, "c5f81002" "c5f8114350" "488b4210" "48894360" "488b4218" "48894368",
               "0x27050 text view copy @+0x50")
    require(e.call(m + 0x87) == 0xF180, "0x27050 call 0xf180(this+0x70)")
    e.bytes_at(m + 0x8C, "4c89bb90000000", "0x27050 [+0x90] = target")
    e.bytes_at(m + 0xA2, "c5fa118398000000", "0x27050 [+0x98] = xmm0")
    e.bytes_at(m + 0xAF, "c5fa11839c000000", "0x27050 [+0x9c] = xmm1")
    e.bytes_at(m + 0xB7, "4489b3a0000000", "0x27050 [+0xa0] = flags")
    require(e.lea(m + 0x93) == PARENT_CELL, "0x27050 loads the parent cell")
    require(e.jmp(m + 0xCB) == 0x268B0, "0x27050 tail-calls 0x268b0")
    t = 0x26EA0
    e.bytes_at(t + 0x13, "48c70300000000" "48895b08" "48895b10" "48c7431800000000" "c6432000",
               "0x26ea0 link {0, this, this, 0}; registered = 0")
    e.bytes_at(t + 0x50, "c7434802000000", "0x26ea0 kind = 2")
    require(e.call(t + 0x7D) == 0xF180, "0x26ea0 call 0xf180(this+0x70)")
    e.bytes_at(t + 0x82, "4c89b390000000", "0x26ea0 [+0x90] = target")
    e.bytes_at(t + 0x89, "c783980000000000803f" "c7839c00000000002041" "4489bba0000000",
               "0x26ea0 range {1.0f, 10.0f}; flags")
    e.bytes_at(t + 0xA4, "4c89f7" "498b06" "ff5018" "83f801" "751f" "498b06" "4c89f7" "ff5030" "83f803" "7511",
               "0x26ea0 virtual slot 3 == 1 && slot 6 == 3")
    e.bytes_at(t + 0xC0, "48b8cdcccc3d0000003f" "48898398000000", "0x26ea0 override range {0.1f, 0.5f}")
    require(e.lea(t + 0xD1) == PARENT_CELL, "0x26ea0 loads the parent cell")
    require(e.jmp(t + 0xE8) == 0x268B0, "0x26ea0 tail-calls 0x268b0")
    # 0x268b0 structure
    g = 0x268B0
    e.bytes_at(g + 0x24, "807b2000" "0f8561020000", "0x268b0 registered flag test")
    e.bytes_at(g + 0x2E, "f683a0000000c0", "0x268b0 flags & 0xc0 gate (assert path A)")
    e.bytes_at(g + 0xBD, "f683a10000000f", "0x268b0 flags & 0xf00 gate (assert path B)")
    e.bytes_at(g + 0x140, "83f801", "0x268b0 kind == 1 block")
    e.bytes_at(g + 0x1CE, "4d85f6" "7418", "0x268b0 parent test")
    e.bytes_at(g + 0x1D3, "488d7328" "4c89f7" "4c89fa", "0x268b0 parent insert args (name view, kind)")
    require(e.call(g + 0x1DD) == 0x27470, "0x268b0 parent insert 0x27470")
    e.bytes_at(g + 0x1E2, "c6432001", "0x268b0 registered = 1 on the parent path only")
    require(e.rip(g + 0x1EB, 6, 2) == 0x2DFD4E8, "0x268b0 root guard byte")
    require(e.call(g + 0x1FC) == 0xF8F3D0 and e.call(g + 0x258) == 0xF8F3E0, "0x268b0 root guard PLT")
    require(e.lea(g + 0x205) == 0x27280, "0x268b0 root destructor 0x27280")
    e.bytes_at(g + 0x225, "c4e1f96ec6" "c5f970c044", "0x268b0 root init: head.prev = head.next = &root")
    e.bytes_at(g + 0x25D, "488d05ac69dd02" "48891b" "48894310" "488b0da669dd02" "48894b08" "48891d9b69dd02"
               "488b4b08" "48895910" "ff05a569dd02" "48894318",
               "0x268b0 root append: self, next=root, prev=tail, root.prev=item, tail.next=item, ++count, owner")

    # ---------------------------------------------------------------- native sources
    def read(path: str) -> str:
        return (ROOT / path).read_text()

    base_h = read("native/core/concmd_base.hpp")
    base_c = read("native/core/concmd_base.cpp")
    vars_h = read("native/core/concmd_vars.hpp")
    vars_c = read("native/core/concmd_vars.cpp")
    menu_h = read("native/core/debug_menu_item.hpp")
    menu_c = read("native/core/debug_menu_item.cpp")
    h298 = read("native/core/ctor_slot_298_b0e250.hpp")
    c298 = read("native/core/ctor_slot_298_b0e250.cpp")
    h299 = read("native/core/ctor_slot_299_b0c7a0.hpp")
    c299 = read("native/core/ctor_slot_299_b0c7a0.cpp")
    h301 = read("native/core/ctor_slot_301_ac9550.hpp")
    c301 = read("native/core/ctor_slot_301_ac9550.cpp")

    require(const_value(base_h, "kConCmdBaseCtorRva") == 0x20AF0 and const_value(base_h, "kConCmdBaseCtorSize") == 0x5A,
            "native base ctor constants")
    require(const_value(base_h, "kConCmdRegisterRva") == 0x20D60 and const_value(base_h, "kConCmdRegisterSize") == 0x1A,
            "native register constants")
    require(const_value(base_h, "kConCmdRegisterBodyRva") == 0x20D80 and
            const_value(base_h, "kConCmdRegisterBodySize") == 0x188, "native register body constants")
    require(const_value(base_h, "kConCmdBaseDtorRva") == 0x20B50 and const_value(base_h, "kConCmdBaseDtorSize") == 0x155,
            "native base dtor constants")
    require(const_value(base_h, "kConCmdManagerCellRva") == MANAGER_CELL, "native manager cell")
    require(const_value(base_h, "kConCmdPendingHeadRva") == 0x2DFCE50, "native pending head")
    require(const_value(base_h, "kConCmdPendingGuardRva") == 0x2DFCE60, "native pending guard")
    require(const_value(base_h, "kConCmdManagerZeroIndex") == 567, "native manager zero index")
    require(const_value(base_h, "kConCmdManagerCtorRva") == 0x210B0, "native manager ctor")
    require(const_value(base_h, "kConCmdManagerInstallRva") == 0x243F5, "native manager install store")
    require(const_value(base_h, "kConCmdManagerInstallerRva") == 0x242C0, "native manager installer")
    require(const_value(base_h, "kConCmdBaseVptr") == 0x1594D90, "native base vptr")
    require(const_value(base_h, "kStartupAtexitRva") == ATEXIT and const_value(base_h, "kStartupDsoRva") == DSO,
            "native atexit / dso")
    # tail-link order in the native register mirrors 0x20ec0..0x20edf
    order = ["node->prev = tail;", "node->next = &list.head;", "list.head.prev = node;", "tail->next = node;"]
    pos = [base_c.index(token) for token in order]
    require(pos == sorted(pos), "native pending-list tail link order")
    require("cmd.registered = 1" in base_c and "registry.manager.manager != nullptr" in base_c,
            "native register sets the flag before choosing the path")

    require(const_value(vars_h, "kBoolVarMidVptr") == 0x15940B0 and const_value(vars_h, "kBoolVarFinalVptr") == 0x1594218,
            "native bool vptrs")
    require(const_value(vars_h, "kBoolVarDtorRva") == 0x20B0 and const_value(vars_h, "kBoolVarDtorSize") == 0x36,
            "native bool dtor")
    require(const_value(vars_h, "kRangedEnumCtorRva") == 0xB0E060 and const_value(vars_h, "kRangedEnumCtorSize") == 0x143,
            "native enum ctor")
    require(const_value(vars_h, "kRangedEnumMidVptr") == 0x15D5880 and const_value(vars_h, "kRangedEnumFnVtbl") == 0x15D5960,
            "native enum vtables")
    require(const_value(vars_h, "kRangedEnumFnCopyRva") == 0xB0E1F0 and const_value(vars_h, "kRangedEnumFnCloneRva") == 0xB0E210
            and const_value(vars_h, "kRangedEnumFnMoveRva") == 0xB0E230 and
            const_value(vars_h, "kRangedEnumFnDestroyRva") == 0xB0E240, "native enum functor slots")
    require(const_value(vars_h, "kRangedFloatCtorRva") == 0xC300 and const_value(vars_h, "kRangedFloatCtorSize") == 0x160,
            "native float ctor")
    require(const_value(vars_h, "kRangedFloatMidVptr") == 0x1594128 and
            const_value(vars_h, "kRangedFloatFinalVptr") == 0x15942B0 and
            const_value(vars_h, "kRangedFloatFnVtbl") == 0x1594318 and
            const_value(vars_h, "kRangedFloatArgFnVtbl") == 0x15D57E0, "native float vtables")
    require(const_value(vars_h, "kRangedFloatDtorRva") == 0x20F0 and const_value(vars_h, "kRangedFloatDtorSize") == 0x36,
            "native float dtor")
    # bool shape store order (0xac95f8..0xac9652)
    order = ["var.base.vptr = kBoolVarMidVptr", "var.value_ref = &var.value", "std::memset(var.zero69",
             "std::memset(var.zero70", "var.fn_payload = 0", "var.fn_vptr = fn_vptr_tag", "var.fn_aux = 0",
             "release_inert_view_f590(description)", "var.value = value;", "var.default_value = default_value;",
             "register_concmd_20d60", "release_inert_view_f590(name)", "atexit_log.record(kBoolVarDtorRva"]
    pos = [vars_c.index(token) for token in order]
    require(pos == sorted(pos) and len(set(pos)) == len(pos), "native bool shape store order")
    # enum ctor store order (0xb0e08c..0xb0e0d8)
    enum_part = vars_c[vars_c.index("bool init_ranged_var_b0e060"):]
    order = ["init_concmd_base_20af0", "var.base.vptr = kRangedEnumMidVptr", "var.value_ref = value_ref",
             "var.min_engaged = 0", "var.max_engaged = 0", "std::memset(var.args", "var.fn_vptr = kRangedEnumFnVtbl",
             "install_ranged_fn_from_args"]
    pos = [enum_part.index(token) for token in order]
    require(pos == sorted(pos) and len(set(pos)) == len(pos), "native enum ctor store order")

    require(const_value(menu_h, "kDebugMenuItemFloatCtorRva") == 0x27050 and
            const_value(menu_h, "kDebugMenuItemFloatCtorSize") == 0xD0, "native 0x27050 constants")
    require(const_value(menu_h, "kDebugMenuItemToggleCtorRva") == 0x26EA0 and
            const_value(menu_h, "kDebugMenuItemToggleCtorSize") == 0xED, "native 0x26ea0 constants")
    require(const_value(menu_h, "kDebugMenuRegisterRva") == 0x268B0, "native 0x268b0")
    require(const_value(menu_h, "kDebugMenuParentInsertRva") == 0x27470, "native parent insert")
    require(const_value(menu_h, "kDebugMenuParentCellRva") == PARENT_CELL, "native parent cell")
    require(const_value(menu_h, "kDebugMenuRootRva") == 0x2DFD4C0 and
            const_value(menu_h, "kDebugMenuRootGuardRva") == 0x2DFD4E8, "native root")
    require(const_value(menu_h, "kDebugMenuRootDtorRva") == 0x27280 and
            const_value(menu_h, "kDebugMenuItemDtorRva") == 0x272D0, "native menu dtors")
    require(const_value(menu_h, "kBoolVarSlot3Rva") == 0xB890 and const_value(menu_h, "kBoolVarSlot6Rva") == 0xB930 and
            const_value(menu_h, "kBoolVarSlot3Result") == 1 and const_value(menu_h, "kBoolVarSlot6Result") == 0,
            "native bool slots")
    order = ["node->self = node", "node->next = rootp", "node->prev = tail", "rootp->prev = node",
             "tail->next = node", "++root.count", "node->owner = rootp"]
    pos = [menu_c.index(token) for token in order]
    require(pos == sorted(pos) and len(set(pos)) == len(pos), "native root append order")

    require(const_value(h298, "kSlot298Rva") == 0xB0E250 and const_value(h298, "kSlot298BodySize") == 331,
            "native slot 298 constants")
    require(const_value(h298, "kSlot298ObjectRva") == 0x3305D20 and
            const_value(h298, "kSlot298FinalVptr") == 0x15D58F8 and
            const_value(h298, "kSlot298DtorRva") == 0xB0CB40 and const_value(h298, "kSlot298DtorSize") == 0x36,
            "native slot 298 object / vtable / dtor")
    require('kSlot298Name[] = "shaderTypeToLoad"' in h298, "native slot 298 name")
    order = ["obj.base.vptr = kSlot298FinalVptr", "init_ranged_var_b0e060", "release_inert_view_f590(description)",
             "out.functor_destroy_noop", "obj.value = 0", "obj.default_value = 0", "register_concmd_20d60",
             "release_inert_view_f590(name)", "atexit_log.record(kSlot298DtorRva"]
    pos = [c298.index(token) for token in order]
    require(pos == sorted(pos) and len(set(pos)) == len(pos), "native slot 298 order")

    require(const_value(h299, "kSlot299Rva") == 0xB0C7A0 and const_value(h299, "kSlot299BodySize") == 926,
            "native slot 299 constants")
    require(const_value(h299, "kSlot299ScaleRva") == 0x3305A30 and const_value(h299, "kSlot299AdjustRva") == 0x3305B08 and
            const_value(h299, "kSlot299ScaleItemRva") == 0x3305BD0 and
            const_value(h299, "kSlot299AdjustItemRva") == 0x3305C78, "native slot 299 objects")
    require(const_value(h299, "kSlot299AdjustFnVtbl") == 0x15D5800 and
            const_value(h299, "kSlot299AdjustItemFlags") == 0x30, "native slot 299 tags")
    for text in ('kSlot299ScaleName[] = "globalDebugTextScale"',
                 'kSlot299AdjustName[] = "globalDebugTextAdjustFor4K"', 'kSlot299Label[] = "Debug"',
                 'kSlot299ScaleText[] = "Global Debug Text Scale"',
                 'kSlot299AdjustText[] = "Global Debug Text Scale Adjust for 4k"'):
        require(text in h299, f"native slot 299 string {text}")
    order = ["init_ranged_float_var_c300", "release_inert_view_f590(desc_a)", "a.value = 0x3f800000u",
             "a.default_value = 0x3f800000u", "register_concmd_20d60(a.base", "kRangedFloatDtorRva",
             "init_bool_var_static(objects.adjust", "init_debug_menu_item_27050", "register_debug_menu_item_268b0(objects.scale_item",
             "kDebugMenuItemDtorRva, &objects.scale_item", "init_debug_menu_item_26ea0",
             "register_debug_menu_item_268b0(objects.adjust_item", "kDebugMenuItemDtorRva, &objects.adjust_item"]
    pos = [c299.index(token) for token in order]
    require(pos == sorted(pos) and len(set(pos)) == len(pos), "native slot 299 order")
    require("0.1f, 10.0f, 1" in c299 and "0.1f, 1.0f)" in c299, "native slot 299 ranges")

    require(const_value(h301, "kSlot301Rva") == 0xAC9550 and const_value(h301, "kSlot301BodySize") == 323,
            "native slot 301 constants")
    require(const_value(h301, "kSlot301ObjectRva") == 0x33054A0 and const_value(h301, "kSlot301FnVtbl") == 0x15D5070,
            "native slot 301 object / functor tag")
    require('kSlot301Name[] = "rumbleEnabled"' in h301, "native slot 301 name")
    require("init_bool_var_static(obj, name, description, kSlot301FnVtbl, registry)" in c301,
            "native slot 301 uses the shared bool shape")

    # ---------------------------------------------------------------- production rows
    startup_source = read("native/core/startup_ctors.cpp")
    startup_header = read("native/core/startup_ctors.hpp")
    array_match = re.search(r"static constexpr CtorSlot kSlots\[\]\s*=\s*\{(.*?)\n\s*\};", startup_source, re.DOTALL)
    require(array_match is not None, "cannot find recovered constructor array")
    block = array_match.group(1)
    rows = [(int(rva, 16), int(index)) for rva, index in re.findall(
        r"^\s*\{\s*(0x[0-9a-fA-F]+)\s*,\s*(\d+)\s*,\s*\[", block, re.MULTILINE)]
    indices = [index for _, index in rows]
    require(indices == sorted(indices) and len(indices) == len(set(indices)),
            "constructor rows must be in strictly increasing index order")
    for index, rva in ((298, 0xB0E250), (299, 0xB0C7A0), (301, 0xAC9550)):
        require((rva, index) in rows, f"missing registered row {index} / {rva:#x}")
    require("init_slot298_b0e250(s.slot298_shader_type, s.concmd)" in block, "slot 298 row callback")
    require("init_slot299_b0c7a0(s.slot299, s.concmd, s.debug_menu)" in block, "slot 299 row callback")
    require("init_slot301_ac9550(s.slot301_rumble, s.concmd)" in block, "slot 301 row callback")
    require(block.count("init_failed = true") >= 3, "298/299/301 failures are visible")
    for member in ("ConCmdRegistry concmd", "Slot298Object slot298_shader_type", "DebugMenuRegistry debug_menu",
                   "Slot299Objects slot299", "Slot301Object slot301_rumble"):
        require(member in startup_header, f"missing StartupState member: {member}")
    pos = {index: indices.index(index) for index in (297, 298, 299, 300, 301, 304)}
    require(pos[297] < pos[298] < pos[299] < pos[300] < pos[301] < pos[304], "rows sit between 297 and 304")

    print(
        "OK check_fctor_slots_298_299_301: "
        f"298 {FUNCTIONS['slot298'][2][:12]}… 299 {FUNCTIONS['slot299'][2][:12]}… "
        f"301 {FUNCTIONS['slot301'][2][:12]}…; base 0x20af0/0x20d60/0x20d80; "
        "indirect call [0x15d5960+0x18] -> 0xb0e240 (ret); manager cell 28 lea sites; parent cell 18 lea sites"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
