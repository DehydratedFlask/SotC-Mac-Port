#!/usr/bin/env python3
"""ELF-tie the lifted ranged-int debug-variable constructors `0xa624a0` / `0xa62860` (startup slot 306).

Private: needs `local/re/eboot.elf`. Read-only and offline: instructions are matched by raw bytes and
byte ranges are hashed; **no guest byte is executed**. No emulator, interpreter, JIT or guest runtime.

Layers:
  1. Whole-function SHA-256 and extent of the two wrappers (`0xa624a0`, `0xa62860`, 0x1b3 B), the two
     workers (`0xa62660`, `0xa62a20`, 0x15f B), the two destructors, the functor-table slot bodies
     (copy / clone / move / destroy per shape) and the int-class `mov eax,1; ret` slot body.
  2. The two shapes are the same code: wrapper/worker bodies are byte-identical once the rip-relative
     and call displacement fields are masked, and every masked field resolves to the expected target
     (shared: cookie `0x15fe518`, `0x20d60`, `0x20af0`, `__stack_chk_fail`; per shape: vtable, functor
     table, worker).
  3. Instruction pins on the wrapper and worker for every store / load the native model depends on
     (value `+0xd0`, default `+0x68`, min `+0x6c/+0x70`, max `+0x74/+0x78`, args `+0x80`, functor
     payload `+0xb0`, tag `+0xc0`, aux `+0xc8`, the self-pointer fix-up and the functor install).
  4. The vtables / functor tables read from the RELRO window: functor slots 0..3, class vtable slot 0
     (destructor), slot 2 (`0x20d60`), slots 3 and 6 (`mov eax,1; ret`).
  5. The three slot-306 call sites: name literal and length, default/min/max immediates, flags 1,
     the functor tag, the `0xf730` description, the wrapper call, the epilogue (functor destroy,
     `0xf590` name release, `__cxa_atexit(dtor, object, dso)`).
  6. The native header/source/test must carry the constants and the site table, and they must equal
     the ELF values.
  7. Mutation self-test (default on; `--no-mutate` to skip): every recorded pin and every resolved
     displacement is perturbed in an in-memory overlay (first and last byte) with the hash layer
     disabled; hashed functions are perturbed at first/middle/last byte; every native token is
     removed in turn; every ELF-tied constant and site field is changed. Each must fail the checker.
"""

from __future__ import annotations

import hashlib
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ELF_PATH = ROOT / "local/re/eboot.elf"

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"

HEADER = ROOT / "native/core/intvar_a624a0.hpp"
SOURCE = ROOT / "native/core/intvar_a624a0.cpp"
TEST = ROOT / "native/tests/test_intvar_a624a0.cpp"

PT_LOAD = 1
PT_SCE_RELRO = 0x61000010

COOKIE = 0x15FE518
STACK_FAIL = 0xF8F3A0
REGISTER_20D60 = 0x20D60
BASE_CTOR_20AF0 = 0x20AF0
RELEASE_F590 = 0xF590
EMPTY_STRING_F730 = 0xF730
DSO_HANDLE = 0x1604218  # __dso_handle passed to __cxa_atexit (sites 1 and 2 share the register)

SHAPE_DELTA = 0x3C0  # show_active copy sits exactly 0x3c0 B after the listener copy

# Whole-function (start, size, sha256).
FUNCS = {
    "wrapper_listener_a624a0": (0xA624A0, 0x1B3, "8b388e66341e8179da1dfaa08abfda08ff33031d08cd1d87da066f99c7f87125"),
    "worker_listener_a62660": (0xA62660, 0x15F, "953448cf250c21b87315c31ddc0d9208ed9f0ad50d98e78be78e9bd5eab4c81a"),
    "wrapper_show_active_a62860": (0xA62860, 0x1B3, "319374c166e2b43228308844fd24bfef631371889c0f78beda9d5b0bc1a51535"),
    "worker_show_active_a62a20": (0xA62A20, 0x15F, "3c3858dadb7f67602506e7162d432a3d539724b3f037ff169969b99b8257e0dd"),
    "dtor_listener_a5bea0": (0xA5BEA0, 0x36, "46c09fe5f28364f9c736490ba5f7a3e66e8f0bfc90eccc394756e6543ebbe0f5"),
    "dtor_show_active_a5bee0": (0xA5BEE0, 0x36, "4c21694d286e257ba5049c75423e5341816d018b7a87ba0258ff4f27c24fb4b4"),
    "fn_copy_listener_a62440": (0xA62440, 0x14, "619efe65e861dd2d06fdb1863b907b66ddfa2e80ee5b06556e328d3d535d2b2a"),
    "fn_clone_listener_a62460": (0xA62460, 0x14, "28802809bea6d5c9fbc0b9f6b503dac4d7e6dd6348ef94081dca3e47f92b05a2"),
    "fn_move_listener_a62480": (0xA62480, 0x09, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    "fn_destroy_listener_a62490": (0xA62490, 0x01, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
    "fn_copy_show_active_a62800": (0xA62800, 0x14, "7518b6f1099c471e3e458f8233611949766ab92e354df487f661cd513a245aaf"),
    "fn_clone_show_active_a62820": (0xA62820, 0x14, "79ac4bbae4a4eff4e084959e60219a42d143838d3080774ff0c6180e73b12410"),
    "fn_move_show_active_a62840": (0xA62840, 0x09, "547772c99434936d33353159cfc12801b74a62a235674217d91aa61cc05467bd"),
    "fn_destroy_show_active_a62850": (0xA62850, 0x01, "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"),
    "slot3_listener_a60ea0": (0xA60EA0, 0x06, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
    "slot6_listener_a60ef0": (0xA60EF0, 0x06, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
    "slot3_show_active_a61940": (0xA61940, 0x06, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
    "slot6_show_active_a61990": (0xA61990, 0x06, "2db31f4e09597946e56e859813bfdad7046d457c32332c3a882654d1e3ffdf67"),
}

# Raw functor-slot bodies of the two tables: (rva, hex). Displacements inside are resolved separately.
FN_SLOT_BODIES = [
    (0xA62440, "488b4610488d0d5dfbb6004889471048894f20c3"),   # copy  (listener)
    (0xA62460, "488d0541fbb60048894720488b461048894710c3"),   # clone (listener)
    (0xA62480, "488b461048894710c3"),                         # move
    (0xA62490, "c3"),                                         # destroy
    (0xA62800, "488b4610488d0d75f8b6004889471048894f20c3"),   # copy  (show_active)
    (0xA62820, "488d0559f8b60048894720488b461048894710c3"),   # clone (show_active)
    (0xA62840, "488b461048894710c3"),
    (0xA62850, "c3"),
]
# (rva of the rip-relative instruction, disp offset, length, expected target): the table tag each
# copy/clone body stores into the destination functor.
FN_SLOT_TAGS = [
    (0xA62444, 3, 7, 0x15D1FA8), (0xA62460, 3, 7, 0x15D1FA8),
    (0xA62804, 3, 7, 0x15D2080), (0xA62820, 3, 7, 0x15D2080),
]

DTOR_BODIES = [
    (0xA5BEA0, "554889e55350488d05e35fb7004889fb488dbba00000004883c010488903488b83c0000000ff50184889df4883c4085b5de97a4c5cff"),
    (0xA5BEE0, "554889e55350488d051b60b7004889fb488dbba00000004883c010488903488b83c0000000ff50184889df4883c4085b5de93a4c5cff"),
]

# (dtor rva, worker-class vtable it installs, tail-call target 0x20b50 = base-class dtor)
DTOR_FACTS = [(0xA5BEA0, 0x15D1E90, 0x20B50), (0xA5BEE0, 0x15D1F08, 0x20B50)]

SLOT_BODY = "b801000000c3"  # mov eax,1; ret

# Wrapper instruction pins at the listener addresses (the show_active copy sits at +0x3c0).
WRAPPER_PINS = [
    (0xA624B4, "488995f8feffff"), (0xA624BB, "488b15"), (0xA624C2, "4c8d35"), (0xA624C9, "44898d24ffffff"),
    (0xA624D0, "4c898500ffffff"), (0xA624D7, "48898d08ffffff"), (0xA624DE, "4889b510ffffff"),
    (0xA624E5, "488b4510"), (0xA624EC, "488db3d0000000"), (0xA624F6, "4983c610"), (0xA624FA, "488d15"),
    (0xA62505, "4c8933"), (0xA62508, "48899568ffffff"), (0xA6250F, "48c78558ffffff00000000"),
    (0xA6251A, "4889b518ffffff"), (0xA62521, "48c78570ffffff00000000"), (0xA6252C, "c5fc1000"),
    (0xA62530, "c5fc118528ffffff"), (0xA62538, "c4e37d19c001"), (0xA6253E, "c4e3f916c101"),
    (0xA62544, "4885c9"), (0xA62547, "740e"), (0xA62549, "488d8d28ffffff"), (0xA62550, "48898d28ffffff"),
    (0xA62557, "488b4848"), (0xA62562, "488d7020"), (0xA62569, "48894da0"), (0xA6256D, "488b4840"),
    (0xA62571, "ff5108"), (0xA62582, "ff10"), (0xA62584, "488b8568ffffff"), (0xA62598, "ff10"),
    (0xA6259A, "488b45c8"), (0xA625A4, "ff10"), (0xA625A6, "488b8570ffffff"), (0xA625AD, "488b4da0"),
    (0xA625B4, "488945a0"), (0xA625B8, "48898d70ffffff"), (0xA625C3, "ff5018"),
    (0xA625E9, "488d8528ffffff"), (0xA625F3, "ff7518"), (0xA625F6, "50"), (0xA625F7, "e8"),
    (0xA62600, "488b8568ffffff"), (0xA6260A, "ff5018"), (0xA6260D, "4c8933"), (0xA62613, "488b8df8feffff"),
    (0xA6261A, "8b01"), (0xA6261C, "8983d0000000"), (0xA62622, "8b01"), (0xA62624, "894368"),
    (0xA62627, "e8"), (0xA6262C, "488b05"), (0xA6264D, "c3"), (0xA6264E, "e8"),
]
WORKER_PINS = [
    (0xA62671, "488b05"), (0xA62678, "4989d4"), (0xA6267B, "488b5518"), (0xA6267F, "4c8b6d10"),
    (0xA62683, "4989cf"), (0xA62686, "4489c9"), (0xA62689, "4d89c6"), (0xA6268C, "4889fb"),
    (0xA62696, "e8"), (0xA6269B, "488d05"), (0xA626A6, "488d0d"), (0xA626AD, "4883c010"),
    (0xA626B1, "488903"), (0xA626B4, "4c896360"), (0xA626B8, "c6436c00"), (0xA626BC, "c6437001"),
    (0xA626C0, "418b07"), (0xA626C3, "89436c"), (0xA626C6, "c6437400"), (0xA626CA, "c6437801"),
    (0xA626CE, "418b06"), (0xA626D1, "894374"), (0xA626D4, "c5fc118380000000"),
    (0xA626DC, "48898bc0000000"), (0xA626E3, "48c783b000000000000000"),
    (0xA626EE, "48c783c800000000000000"), (0xA626F9, "c4c178104508"), (0xA626FF, "c5f8118388000000"),
    (0xA62707, "498b4500"), (0xA6270B, "48898380000000"), (0xA62712, "498b4518"),
    (0xA62716, "48898398000000"), (0xA6271D, "4885c0"), (0xA62720, "740a"), (0xA62722, "4889d8"),
    (0xA62725, "4883e880"), (0xA62729, "488900"), (0xA6272C, "498b4548"), (0xA62737, "498d7520"),
    (0xA6273E, "488945a0"), (0xA62742, "498b4540"), (0xA62746, "ff5008"), (0xA62757, "ff10"),
    (0xA62759, "488b83c0000000"), (0xA62760, "4c8da3a0000000"), (0xA6276D, "ff10"), (0xA6276F, "488b45c8"),
    (0xA62779, "ff10"), (0xA6277B, "488b83c8000000"), (0xA62782, "488b4da0"), (0xA62789, "48898bc8000000"),
    (0xA62790, "488945a0"), (0xA62798, "ff5018"), (0xA6279B, "488b05"), (0xA627B9, "c3"), (0xA627BA, "e8"),
]

# Masked (rip-rel32 / call-rel32) fields: (instruction rva, disp offset, instruction length) for the
# listener copy. Everything else in a wrapper/worker is byte-identical between the two shapes.
WRAPPER_MASKS = [(0xA624BB, 3, 7), (0xA624C2, 3, 7), (0xA624FA, 3, 7), (0xA6262C, 3, 7),
                 (0xA625F7, 1, 5), (0xA62627, 1, 5), (0xA6264E, 1, 5)]
WORKER_MASKS = [(0xA62671, 3, 7), (0xA62696, 1, 5), (0xA6269B, 3, 7), (0xA626A6, 3, 7),
                (0xA6279B, 3, 7), (0xA627BA, 1, 5)]

# Shape table: (name, wrapper, worker, vtable base (+0x10 is the vptr), worker vtable base, fn table,
#               destructor, slot3, slot6, fn slots (copy, clone, move, destroy))
SHAPES = {
    "listener": dict(wrapper=0xA624A0, worker=0xA62660, final_table=0x15D1FC8, worker_table=0x15D1E90,
                     fn_table=0x15D1FA8, dtor=0xA5BEA0, slot3=0xA60EA0, slot6=0xA60EF0,
                     fn_slots=(0xA62440, 0xA62460, 0xA62480, 0xA62490), delta=0),
    "show_active": dict(wrapper=0xA62860, worker=0xA62A20, final_table=0x15D20A0, worker_table=0x15D1F08,
                        fn_table=0x15D2080, dtor=0xA5BEE0, slot3=0xA61940, slot6=0xA61990,
                        fn_slots=(0xA62800, 0xA62820, 0xA62840, 0xA62850), delta=SHAPE_DELTA),
}

# Call-site pins (address, hex): "L" listener sites 1 and 2 share r14 = functor table.
SITE_PINS = [
    # site 1: sndx_listenerOrientation
    (0xA633FB, "488d05"), (0xA63406, "4c8d35"), (0xA6340D, "c744242000000000"),
    (0xA63415, "c744241c00000000"), (0xA6341D, "c744241801000000"), (0xA63431, "4889442440"),
    (0xA63436, "48c744244818000000"), (0xA6344F, "4c89b424a0000000"), (0xA63463, "e8"),
    (0xA63468, "4c8d2d"), (0xA6346F, "4c8d7c2440"), (0xA63474, "4c8d642460"), (0xA63479, "488d542420"),
    (0xA6347E, "488d4c241c"), (0xA63483, "4c8d442418"), (0xA63488, "41b901000000"), (0xA6348E, "4c89fe"),
    (0xA63491, "4c89ef"), (0xA63494, "504154"), (0xA63497, "e8"), (0xA634A0, "488b8424a0000000"),
    (0xA634A8, "488dbc2480000000"), (0xA634B0, "ff5018"), (0xA634B3, "4c89ff"), (0xA634B6, "e8"),
    (0xA634BB, "4c8d3d"), (0xA634C2, "4c89ee"), (0xA634C5, "4c8d2d"), (0xA634CC, "4c89ff"),
    (0xA634CF, "4c89ea"), (0xA634D2, "e8"),
    # site 2: sndx_listenerPosition
    (0xA634D7, "488d05"), (0xA634E2, "c744242000000000"), (0xA634EA, "c744241c00000000"),
    (0xA634F2, "c744241801000000"), (0xA63506, "4889442440"), (0xA6350B, "48c744244815000000"),
    (0xA63524, "4c89b424a0000000"), (0xA63538, "e8"), (0xA6353D, "488d1d"), (0xA63544, "4c8d742440"),
    (0xA63549, "488d542420"), (0xA6354E, "488d4c241c"), (0xA63553, "4c8d442418"),
    (0xA63558, "41b901000000"), (0xA6355E, "4c89f6"), (0xA63561, "4889df"), (0xA63564, "504154"),
    (0xA63567, "e8"), (0xA63570, "488b8424a0000000"), (0xA63578, "488dbc2480000000"),
    (0xA63580, "ff5018"), (0xA63583, "4c89f7"), (0xA63586, "e8"), (0xA6358B, "4c89ff"),
    (0xA6358E, "4889de"), (0xA63591, "4c89ea"), (0xA63594, "e8"),
    # site 3: sndx_showActive
    (0xA64024, "488d05"), (0xA6402F, "c744242000000000"), (0xA64037, "c744241c00000000"),
    (0xA6403F, "c744241802000000"), (0xA6405B, "4889442440"), (0xA64060, "48c74424480f000000"),
    (0xA64073, "488d05"), (0xA64080, "48898424a0000000"), (0xA64094, "e8"), (0xA64099, "4c8d25"),
    (0xA640A0, "488d5c2440"), (0xA640A5, "488d542420"), (0xA640AA, "488d4c241c"),
    (0xA640AF, "4c8d442418"), (0xA640B4, "41b901000000"), (0xA640BA, "4c89e7"), (0xA640BD, "4889de"),
    (0xA640C0, "50"), (0xA640C1, "488d442468"), (0xA640C6, "50"), (0xA640C7, "e8"),
    (0xA640CC, "4883c410"), (0xA640D0, "488b8424a0000000"), (0xA640D8, "4c89ff"), (0xA640DB, "ff5018"),
    (0xA640DE, "4889df"), (0xA640E1, "e8"), (0xA640E6, "488d3d"), (0xA640ED, "4c89e6"),
    (0xA640F0, "4c89f2"), (0xA640F3, "e8"),
]

# Call-site facts. name_lea: rip-lea of the literal; len_ins: `mov qword [rsp+0x48], len`; g_lea: the
# object's global; wrapper_call: the call to the shape's wrapper; imm_*: `mov dword [rsp+..], imm`.
SITES = [
    dict(name="sndx_listenerOrientation", name_lea=0xA633FB, len_ins=0xA63436, g_lea=0xA63468,
         call=0xA63497, d=0xA6340D, mn=0xA63415, mx=0xA6341D, tag_lea=0xA63406, desc_call=0xA63463,
         release_call=0xA634B6, dtor_lea=0xA634BB, dtor_len=7, atexit_call=0xA634D2, shape="listener",
         dso_lea=0xA634C5),
    dict(name="sndx_listenerPosition", name_lea=0xA634D7, len_ins=0xA6350B, g_lea=0xA6353D,
         call=0xA63567, d=0xA634E2, mn=0xA634EA, mx=0xA634F2, tag_lea=0xA63406, desc_call=0xA63538,
         release_call=0xA63586, dtor_lea=0xA634BB, dtor_len=7, atexit_call=0xA63594, shape="listener",
         dso_lea=0xA634C5),
    dict(name="sndx_showActive", name_lea=0xA64024, len_ins=0xA64060, g_lea=0xA64099,
         call=0xA640C7, d=0xA6402F, mn=0xA64037, mx=0xA6403F, tag_lea=0xA64073, desc_call=0xA64094,
         release_call=0xA640E1, dtor_lea=0xA640E6, dtor_len=7, atexit_call=0xA640F3, shape="show_active",
         dso_lea=None),
]

OVERLAY: dict[int, int] = {}
PINNED: list[tuple[int, int]] = []
HASHED: list[tuple[int, int]] = []
RECORD = False
SKIP_HASH = False


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def load_segments(elf: bytes) -> list[tuple[int, int, int]]:
    e_phoff = struct.unpack_from("<Q", elf, 0x20)[0]
    e_phentsize, e_phnum = struct.unpack_from("<HH", elf, 0x36)
    out = []
    for i in range(e_phnum):
        base = e_phoff + i * e_phentsize
        p_type = struct.unpack_from("<I", elf, base)[0]
        if p_type not in (PT_LOAD, PT_SCE_RELRO):
            continue
        p_offset, p_vaddr = struct.unpack_from("<QQ", elf, base + 8)
        p_filesz = struct.unpack_from("<Q", elf, base + 32)[0]
        out.append((p_vaddr, p_offset, p_filesz))
    return out


def read_rva(elf: bytes, segments, rva: int, size: int) -> bytes:
    for vaddr, offset, filesz in segments:
        if vaddr <= rva and rva + size <= vaddr + filesz:
            start = offset + (rva - vaddr)
            data = bytearray(elf[start:start + size])
            for i in range(size):
                if rva + i in OVERLAY:
                    data[i] = OVERLAY[rva + i]
            return bytes(data)
    raise SystemExit(f"FAIL rva {rva:#x} (+{size:#x}) is not file-backed")


def u8(elf, segments, rva):
    return read_rva(elf, segments, rva, 1)[0]


def u32(elf, segments, rva):
    return struct.unpack("<I", read_rva(elf, segments, rva, 4))[0]


def i32(elf, segments, rva):
    return struct.unpack("<i", read_rva(elf, segments, rva, 4))[0]


def u64(elf, segments, rva):
    return struct.unpack("<Q", read_rva(elf, segments, rva, 8))[0]


def record(addr: int, size: int) -> None:
    if RECORD:
        PINNED.append((addr, size))


def rel32_target(elf, segments, site):
    """Target of an `e8`/`e9` at `site`; the four displacement bytes are recorded for mutation."""
    record(site + 1, 4)
    return site + 5 + i32(elf, segments, site + 1)


def rip_target(elf, segments, site, disp_off, length):
    record(site + disp_off, 4)
    return site + length + i32(elf, segments, site + disp_off)


def pin(elf, segments, addr, pattern, what):
    size = len(pattern) // 2
    record(addr, size)
    require(read_rva(elf, segments, addr, size) == bytes.fromhex(pattern), f"{what} @ {addr:#x}")


def hashed(elf, segments, name, start, size, sha):
    body = read_rva(elf, segments, start, size)
    if RECORD:
        HASHED.append((start, size))
    require(len(body) == size, f"{name} size")
    if not SKIP_HASH:
        require(hashlib.sha256(body).hexdigest() == sha, f"{name} sha256")
    return body


def check_hashes(elf, segments) -> None:
    for name, (start, size, sha) in FUNCS.items():
        hashed(elf, segments, name, start, size, sha)


def masked(body: bytes, base: int, masks, delta: int) -> bytes:
    data = bytearray(body)
    for site, disp_off, length in masks:
        off = site + delta - base + disp_off
        end = off + (4 if length in (5, 7) else 0)
        data[off:end] = b"\0\0\0\0"
    return bytes(data)


def check_functions(elf, segments) -> None:
    check_hashes(elf, segments)
    # Padding after each body (nop run) and the next function entry (virtual deleting destructor).
    pin(elf, segments, 0xA62653, "90" * 13, "wrapper padding")
    pin(elf, segments, 0xA627BF, "90", "worker padding")
    pin(elf, segments, 0xA62A13, "90" * 13, "show_active wrapper padding")
    pin(elf, segments, 0xA62B7F, "90", "show_active worker padding")
    pin(elf, segments, 0xA624A0, "554889e54157415641554154534881ec", "wrapper prologue")
    pin(elf, segments, 0xA62660, "554889e54157415641554154534883ec68", "worker prologue")
    pin(elf, segments, 0xA62860, "554889e54157415641554154534881ec", "show_active wrapper prologue")
    pin(elf, segments, 0xA62A20, "554889e54157415641554154534883ec68", "show_active worker prologue")

    wl = read_rva(elf, segments, 0xA624A0, 0x1B3)
    ws = read_rva(elf, segments, 0xA62860, 0x1B3)
    kl = read_rva(elf, segments, 0xA62660, 0x15F)
    ks = read_rva(elf, segments, 0xA62A20, 0x15F)
    require(masked(wl, 0xA624A0, WRAPPER_MASKS, 0) == masked(ws, 0xA62860, WRAPPER_MASKS, SHAPE_DELTA),
            "wrappers differ outside their rip/call displacement fields")
    require(masked(kl, 0xA62660, WORKER_MASKS, 0) == masked(ks, 0xA62A20, WORKER_MASKS, SHAPE_DELTA),
            "workers differ outside their rip/call displacement fields")
    if RECORD:
        for site, disp_off, _ in WRAPPER_MASKS + WORKER_MASKS:
            for delta in (0, SHAPE_DELTA):
                PINNED.append((site + delta + disp_off, 4))

    for pins in (WRAPPER_PINS, WORKER_PINS):
        for addr, pattern in pins:
            for delta in (0, SHAPE_DELTA):
                pin(elf, segments, addr + delta, pattern, "body")

    for shape, info in SHAPES.items():
        d = info["delta"]
        wrapper, worker = info["wrapper"], info["worker"]
        # Wrapper rip / call targets.
        require(rip_target(elf, segments, 0xA624BB + d, 3, 7) == COOKIE, f"{shape} wrapper cookie")
        require(rip_target(elf, segments, 0xA624C2 + d, 3, 7) == info["final_table"], f"{shape} final vtable")
        require(rip_target(elf, segments, 0xA624FA + d, 3, 7) == info["fn_table"], f"{shape} functor table")
        require(rip_target(elf, segments, 0xA6262C + d, 3, 7) == COOKIE, f"{shape} wrapper cookie (exit)")
        require(rel32_target(elf, segments, 0xA625F7 + d) == worker, f"{shape} worker call")
        require(rel32_target(elf, segments, 0xA62627 + d) == REGISTER_20D60, f"{shape} 0x20d60 call")
        require(rel32_target(elf, segments, 0xA6264E + d) == STACK_FAIL, f"{shape} wrapper stack fail")
        # Worker rip / call targets.
        require(rip_target(elf, segments, 0xA62671 + d, 3, 7) == COOKIE, f"{shape} worker cookie")
        require(rel32_target(elf, segments, 0xA62696 + d) == BASE_CTOR_20AF0, f"{shape} 0x20af0 call")
        require(rip_target(elf, segments, 0xA6269B + d, 3, 7) == info["worker_table"], f"{shape} worker vtable")
        require(rip_target(elf, segments, 0xA626A6 + d, 3, 7) == info["fn_table"], f"{shape} worker functor table")
        require(rip_target(elf, segments, 0xA6279B + d, 3, 7) == COOKIE, f"{shape} worker cookie (exit)")
        require(rel32_target(elf, segments, 0xA627BA + d) == STACK_FAIL, f"{shape} worker stack fail")
        require(wrapper == 0xA624A0 + d and worker == 0xA62660 + d, f"{shape} base")


def check_tables(elf, segments) -> None:
    for shape, info in SHAPES.items():
        fn = info["fn_table"]
        slots = tuple(u64(elf, segments, fn + 8 * i) for i in range(4))
        require(slots == info["fn_slots"], f"{shape} functor slots {[hex(s) for s in slots]}")
        vt = info["final_table"] + 0x10   # the vptr
        words = [u64(elf, segments, vt + 8 * i) for i in range(7)]
        require(words[0] == info["dtor"], f"{shape} vtable slot 0")
        require(words[2] == REGISTER_20D60, f"{shape} vtable slot 2")
        require(words[3] == info["slot3"], f"{shape} vtable slot 3")
        require(words[6] == info["slot6"], f"{shape} vtable slot 6")
        # Header words below the vptr (offset-to-top, RTTI) are zero.
        require(u64(elf, segments, info["final_table"]) == 0 and u64(elf, segments, info["final_table"] + 8) == 0,
                f"{shape} vtable header")
        wvt = info["worker_table"] + 0x10
        require(u64(elf, segments, wvt + 16) == REGISTER_20D60, f"{shape} worker vtable slot 2")
        require(u64(elf, segments, wvt + 24) == info["slot3"], f"{shape} worker vtable slot 3")
        require(u64(elf, segments, wvt + 48) == info["slot6"], f"{shape} worker vtable slot 6")
        # The worker's own slot 0 / 1 differ from the final destructor: it is a distinct vtable.
        require(u64(elf, segments, wvt) != info["dtor"], f"{shape} worker vtable must have its own dtor")
    for addr, body in FN_SLOT_BODIES + DTOR_BODIES:
        pin(elf, segments, addr, body, "slot/dtor body")
    for name in ("slot3_listener_a60ea0", "slot6_listener_a60ef0", "slot3_show_active_a61940",
                 "slot6_show_active_a61990"):
        start = FUNCS[name][0]
        pin(elf, segments, start, SLOT_BODY, name)
    for site, disp_off, length, target in FN_SLOT_TAGS:
        require(rip_target(elf, segments, site, disp_off, length) == target, f"fn slot tag @ {site:#x}")
    # Destructors: `lea rax,[vtable]` at +6 (then +0x10), the object's functor destroy through
    # [tag + 0x18], then a tail `jmp` to the base-class destructor at +0x31.
    for start, vtable, tail in DTOR_FACTS:
        require(rip_target(elf, segments, start + 6, 3, 7) == vtable, f"dtor {start:#x} base vtable")
        require(rel32_target(elf, segments, start + 0x31) == tail, f"dtor {start:#x} tail jump")


def derive_sites(elf, segments) -> list[dict]:
    out = []
    for s in SITES:
        name_rva = rip_target(elf, segments, s["name_lea"], 3, 7)
        length = u32(elf, segments, s["len_ins"] + 5)
        literal = read_rva(elf, segments, name_rva, length + 1)
        require(literal[-1] == 0 and b"\0" not in literal[:-1], f"{s['name']} literal not NUL terminated")
        require(literal[:-1].decode() == s["name"], f"{s['name']} literal text")
        call_target = rel32_target(elf, segments, s["call"])
        shape = next((k for k, v in SHAPES.items() if v["wrapper"] == call_target), None)
        require(shape == s["shape"], f"{s['name']} wrapper call {call_target:#x}")
        require(rip_target(elf, segments, s["tag_lea"], 3, 7) == SHAPES[shape]["fn_table"],
                f"{s['name']} argument functor tag")
        require(rel32_target(elf, segments, s["desc_call"]) == EMPTY_STRING_F730, f"{s['name']} 0xf730")
        require(rel32_target(elf, segments, s["release_call"]) == RELEASE_F590, f"{s['name']} 0xf590")
        dtor = rip_target(elf, segments, s["dtor_lea"], 3, s["dtor_len"])
        require(dtor == SHAPES[shape]["dtor"], f"{s['name']} atexit destructor {dtor:#x}")
        atexit = rel32_target(elf, segments, s["atexit_call"])
        out.append(dict(
            name=s["name"], len=length, name_rva=name_rva, call=s["call"],
            global_rva=rip_target(elf, segments, s["g_lea"], 3, 7), shape=shape,
            default=i32(elf, segments, s["d"] + 4), min=i32(elf, segments, s["mn"] + 4),
            max=i32(elf, segments, s["mx"] + 4), atexit=atexit,
            dso=(rip_target(elf, segments, s["dso_lea"], 3, 7) if s["dso_lea"] else None)))
    require(len({o["atexit"] for o in out}) == 1, "the three sites must call the same __cxa_atexit")
    require(out[0]["dso"] == DSO_HANDLE, "dso handle")
    # The default / min / max stores are `mov dword [rsp+0x20|0x1c|0x18], imm32` for every site.
    for s in SITES:
        for key, disp in (("d", 0x20), ("mn", 0x1C), ("mx", 0x18)):
            pin(elf, segments, s[key], f"c74424{disp:02x}", f"{s['name']} {key} store")
    return out


def check_sites(elf, segments) -> None:
    for addr, pattern in SITE_PINS:
        pin(elf, segments, addr, pattern, "call site")
    sites = derive_sites(elf, segments)
    require(len(sites) == 3, "three sites")
    # The wrapper is entered with flags (r9d) 1 at all three sites (pinned above); the native model
    # states ranges as (default, min, max).
    expected = [(0, 0, 1), (0, 0, 1), (0, 0, 2)]
    for site, exp in zip(sites, expected):
        require((site["default"], site["min"], site["max"]) == exp, f"{site['name']} range")


def elf_derived(elf, segments) -> dict[str, int]:
    out: dict[str, int] = {}
    for tag, key in (("Listener", "listener"), ("ShowActive", "show_active")):
        info = SHAPES[key]
        out[f"kIntVarWrapper{tag}Rva"] = info["wrapper"]
        out[f"kIntVarWorker{tag}Rva"] = info["worker"]
        out[f"kIntVarFinalVptr{tag}"] = info["final_table"] + 0x10
        out[f"kIntVarWorkerVptr{tag}"] = info["worker_table"] + 0x10
        out[f"kIntVarFnTable{tag}"] = info["fn_table"]
        out[f"kIntVarDtor{tag}Rva"] = info["dtor"]
        out[f"kIntVarSlot3{tag}Rva"] = info["slot3"]
        out[f"kIntVarSlot6{tag}Rva"] = info["slot6"]
        for slot, kind in zip(info["fn_slots"], ("Copy", "Clone", "Move", "Destroy")):
            out[f"kIntVarFn{kind}{tag}Rva"] = slot
    out["kIntVarWrapperSize"] = FUNCS["wrapper_listener_a624a0"][1]
    out["kIntVarWorkerSize"] = FUNCS["worker_listener_a62660"][1]
    out["kIntVarDtorSize"] = FUNCS["dtor_listener_a5bea0"][1]
    # `mov eax, imm32; ret` -> imm32
    out["kIntVarSlot3Result"] = u32(elf, segments, 0xA60EA0 + 1)
    out["kIntVarSlot6Result"] = u32(elf, segments, 0xA60EF0 + 1)
    # Flags: `mov r9d, imm32` at the first site.
    out["kIntVarCtorFlags"] = u32(elf, segments, 0xA63488 + 2)
    return out


ELF_TIED = {
    "kIntVarWrapperListenerRva", "kIntVarWorkerListenerRva", "kIntVarWrapperShowActiveRva",
    "kIntVarWorkerShowActiveRva", "kIntVarFinalVptrListener", "kIntVarWorkerVptrListener",
    "kIntVarFnTableListener", "kIntVarDtorListenerRva", "kIntVarFinalVptrShowActive",
    "kIntVarWorkerVptrShowActive", "kIntVarFnTableShowActive", "kIntVarDtorShowActiveRva",
    "kIntVarSlot3ListenerRva", "kIntVarSlot6ListenerRva", "kIntVarSlot3ShowActiveRva",
    "kIntVarSlot6ShowActiveRva", "kIntVarFnCopyListenerRva", "kIntVarFnCloneListenerRva",
    "kIntVarFnMoveListenerRva", "kIntVarFnDestroyListenerRva", "kIntVarFnCopyShowActiveRva",
    "kIntVarFnCloneShowActiveRva", "kIntVarFnMoveShowActiveRva", "kIntVarFnDestroyShowActiveRva",
    "kIntVarWrapperSize", "kIntVarWorkerSize", "kIntVarDtorSize", "kIntVarSlot3Result",
    "kIntVarSlot6Result", "kIntVarCtorFlags",
}

HEADER_TOKENS = [
    "enum class IntVarShape", "struct IntVarShapeInfo", "int_var_shape_info", "int_var_fn_table_lifted",
    "struct IntVarCtorResult", "init_int_var_worker", "init_int_var(", "int_var_menu_target",
    "struct Slot306IntVarSite", "kSlot306IntVarSites", "init_slot306_int_var", "unlifted_functor",
    "installed_fn_table", "arg_functor_destroyed", "name_release_inert", "atexit_recorded",
    "fail-closed", "0xa62660", "0xa62a20", "0xa624a0", "0xa62860", "0x20d60",
    "0xf590",
]
SOURCE_TOKENS = [
    "install_functor", "init_concmd_base_20af0(var.base, name, description, flags)",
    "register_concmd_20d60(var.base, registry)", "release_inert_view_f590(name)",
    "empty_string_f730()", "registry.atexit_log.record(info.dtor_rva, &var)", "out.unlifted_functor = true",
    "var.fn_aux = 0", "var.fn_payload = 0", "var.value = static_cast<std::uint32_t>(default_value)",
    "var.default_value = static_cast<std::uint32_t>(default_value)", "var.min_engaged = 1",
    "var.max_engaged = 1", "var.value_ref = &var.value", "reinterpret_cast<std::uint64_t>(&var.args[0])",
    "reinterpret_cast<std::uint64_t>(&local.q[0])", "int_var_fn_table_lifted(block.fn_vptr)",
    "if (index >= kSlot306IntVarSites.size()) return out;", "out.ok = out.registration.ok;",
    "kIntVarSlot3Result", "kIntVarSlot6Result",
]
TEST_TOKENS = [
    "init_slot306_int_var", "init_int_var_worker", "int_var_menu_target", "int_var_fn_table_lifted",
    "unlifted_functor", "kSlot306IntVarSites", "init_debug_menu_item_26ea0", "kIntVarFinalVptrListener",
    "kIntVarFinalVptrShowActive", "kIntVarCtorFlags", "WORKER_WRITTEN", "WRAPPER_WRITTEN",
    "atexit_log", "OK test_intvar_a624a0",
]

SITE_RE = re.compile(
    r'\{"(?P<name>[A-Za-z_]+)",\s*(?P<len>\d+),\s*(?P<lit>0x[0-9a-fA-F]+),\s*(?P<call>0x[0-9a-fA-F]+),\s*'
    r'(?P<glob>0x[0-9a-fA-F]+),\s*IntVarShape::(?P<shape>\w+),\s*(?P<d>-?\d+),\s*(?P<mn>-?\d+),\s*(?P<mx>-?\d+)\}')
CONST_RE = re.compile(r"inline constexpr std::\w+ (k\w+) = (0x[0-9A-Fa-f]+|\d+)u?;")


def native_texts() -> dict[str, str]:
    for path in (HEADER, SOURCE, TEST):
        require(path.is_file(), f"{path} is absent")
    return {"header": HEADER.read_text(), "source": SOURCE.read_text(), "test": TEST.read_text()}


def parse_header_constants(header: str) -> dict[str, int]:
    return {m.group(1): int(m.group(2), 0) for m in CONST_RE.finditer(header)}


def parse_header_sites(header: str) -> list[dict]:
    return [dict(name=m["name"], len=int(m["len"]), name_rva=int(m["lit"], 0), call=int(m["call"], 0),
                 global_rva=int(m["glob"], 0), shape=m["shape"], default=int(m["d"]), min=int(m["mn"]),
                 max=int(m["mx"])) for m in SITE_RE.finditer(header)]


def check_native_sources(texts: dict[str, str], elf=None, segments=None) -> None:
    header, source, test = texts["header"], texts["source"], texts["test"]
    for token in HEADER_TOKENS:
        require(token in header, f"header token {token!r}")
    for token in SOURCE_TOKENS:
        require(token in source, f"source token {token!r}")
    for token in TEST_TOKENS:
        require(token in test, f"test token {token!r}")
    require("calloc" not in source and "malloc" not in source and "new " not in source,
            "source must not allocate by itself")
    if elf is None:
        return
    constants = parse_header_constants(header)
    for name, value in elf_derived(elf, segments).items():
        require(name in constants, f"native constant {name} missing")
        require(constants[name] == value, f"native {name} = {constants[name]:#x}, ELF says {value:#x}")
    sites = parse_header_sites(header)
    expected = derive_sites(elf, segments)
    require(len(sites) == len(expected) == 3, f"native site table has {len(sites)} entries")
    for have, want in zip(sites, expected):
        for key in ("name", "len", "name_rva", "call", "global_rva", "shape", "default", "min", "max"):
            require(have[key] == want[key], f"native site {want['name']}.{key} = {have[key]!r}, ELF says {want[key]!r}")


def run_elf_checks(elf, segments, texts) -> None:
    check_functions(elf, segments)
    check_tables(elf, segments)
    check_sites(elf, segments)
    check_native_sources(texts, elf, segments)


def expect_failure(label: str, fn) -> None:
    try:
        fn()
    except SystemExit as error:
        require(str(error).startswith("FAIL"), f"mutation {label}: unexpected exit {error!r}")
        return
    raise SystemExit(f"FAIL mutation survived: {label}")


def mutation_self_test(elf, segments, texts) -> tuple[int, int]:
    global OVERLAY, SKIP_HASH
    SKIP_HASH = True
    pins = sorted(set(PINNED))
    survivors = 0
    ran = 0
    for addr, size in pins:
        for byte_addr in sorted({addr, addr + size - 1}):
            OVERLAY = {byte_addr: u8(elf, segments, byte_addr) ^ 0x01}
            ran += 1
            try:
                run_elf_checks(elf, segments, texts)
            except SystemExit:
                continue
            survivors += 1
            print(f"mutation survived at {byte_addr:#x} (pin {addr:#x}+{size})", file=sys.stderr)
    OVERLAY = {}
    SKIP_HASH = False
    require(survivors == 0, f"{survivors} ELF mutations survived")

    # Hashed functions: first, middle and last byte.
    for start, size in sorted(set(HASHED)):
        for byte_addr in sorted({start, start + size // 2, start + size - 1}):
            OVERLAY = {byte_addr: u8(elf, segments, byte_addr) ^ 0x80}
            expect_failure(f"hash @ {byte_addr:#x}", lambda: check_hashes(elf, segments))
            ran += 1
    OVERLAY = {}

    native_ran = 0
    for key, tokens in (("header", HEADER_TOKENS), ("source", SOURCE_TOKENS), ("test", TEST_TOKENS)):
        for token in tokens:
            mutated = dict(texts)
            mutated[key] = texts[key].replace(token, "\0MUTATED\0")
            expect_failure(f"{key} token {token!r}", lambda m=mutated: check_native_sources(m, elf, segments))
            native_ran += 1
    for name, value in parse_header_constants(texts["header"]).items():
        if name not in ELF_TIED:
            continue
        pattern = re.compile(rf"({name} = )(0x[0-9A-Fa-f]+|\d+)(u?;)")
        mutated = dict(texts)
        mutated["header"] = pattern.sub(lambda m: f"{m.group(1)}{value ^ 0x4:#x}{m.group(3)}",
                                        texts["header"], count=1)
        expect_failure(f"constant {name}", lambda m=mutated: check_native_sources(m, elf, segments))
        native_ran += 1
    # Every field of every native site tuple.
    for match in SITE_RE.finditer(texts["header"]):
        for group in ("len", "lit", "call", "glob", "d", "mn", "mx"):
            text = match.group(0)
            start = match.start(group) - match.start(0)
            old = match.group(group)
            new = str(int(old, 0) + 1) if not old.startswith("0x") else f"{int(old, 0) ^ 0x8:#x}"
            mutated_site = text[:start] + new + text[start + len(old):]
            mutated = dict(texts)
            mutated["header"] = texts["header"].replace(text, mutated_site, 1)
            expect_failure(f"site {match['name']}.{group}", lambda m=mutated: check_native_sources(m, elf, segments))
            native_ran += 1
        text = match.group(0)
        swapped = text.replace("IntVarShape::listener", "IntVarShape::@").replace(
            "IntVarShape::show_active", "IntVarShape::listener").replace("IntVarShape::@", "IntVarShape::show_active")
        mutated = dict(texts)
        mutated["header"] = texts["header"].replace(text, swapped, 1)
        expect_failure(f"site {match['name']}.shape", lambda m=mutated: check_native_sources(m, elf, segments))
        native_ran += 1
    return ran, native_ran


def main() -> int:
    global RECORD
    mutate = "--no-mutate" not in sys.argv[1:]
    require(ELF_PATH.is_file(), f"{ELF_PATH} is absent (private game image required)")
    elf = ELF_PATH.read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    segments = load_segments(elf)
    texts = native_texts()

    RECORD = True
    run_elf_checks(elf, segments, texts)
    RECORD = False

    mutations = ""
    if mutate:
        elf_runs, native_runs = mutation_self_test(elf, segments, texts)
        mutations = f"; mutation self-test {elf_runs} ELF + {native_runs} native mutants all rejected"

    print(
        f"OK check_fa624a0_intvars: {len(FUNCS)} function hashes, 2 wrappers (0x1b3) + 2 workers (0x15f) "
        f"identical modulo displacement, {len(WRAPPER_PINS) + len(WORKER_PINS)} body pins x 2 shapes, "
        f"4 vtables + 2 functor tables, 3 slot-306 sites (flags 1; ranges 0..1, 0..1, 0..2); "
        f"guest bytes NOT executed{mutations}"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
