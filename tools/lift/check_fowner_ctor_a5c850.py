#!/usr/bin/env python3
"""ELF-tie the lifted row-9 owner constructor `0xa5c850` (`kOwnerMemberRows[9]`).

Private: needs `local/re/eboot.elf` and the characterization evidence in
`local/re/evidence/owner-ctor-a5c850/`. Read-only and offline: instructions are decoded and byte
ranges are hashed; **no guest byte is executed**. No emulator, interpreter, JIT or guest runtime.

Layers:
  1. `claims.py` (the report's own source of truth) is re-run and must pass all of its assertions.
  2. `stores.py` decodes the ordered store census; this file pins all 65 sites - 56 `this`-relative
     (55 plain + 1 atomic), 7 inside the record loop and 2 absolute - with offsets and widths.
  3. Loop bounds/stride, the constant blocks, the 5 call sites/targets, every call argument byte,
     the 24 B global split, and the `kOwnerMemberRows[9]` tuple are pinned from raw bytes.
  4. The native header/source/test must still carry the constants and the hook/failure contract.
"""

from __future__ import annotations

import contextlib
import hashlib
import io
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
EVIDENCE = ROOT / "local/re/evidence/owner-ctor-a5c850"
ELF_PATH = ROOT / "local/re/eboot.elf"

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
BODY_START, BODY_END = 0xA5C850, 0xA5CB21
BODY_SHA256 = "74b32f4fc7ffb86c4a3fc3018fef5ea3e37230e6093e1c5f95b70f837a754bb1"

HEADER = ROOT / "native/core/owner_ctor_a5c850.hpp"
SOURCE = ROOT / "native/core/owner_ctor_a5c850.cpp"
TEST = ROOT / "native/tests/test_owner_ctor_a5c850.cpp"

# 65 store sites in instruction order. The 19th entry (`0xa5c8f4`) is the `xchg`, which `stores.py`
# deliberately skips (it only classifies mov/vmovups), so it is inserted here and pinned by bytes.
# kind: "this" = [rbx+off], "loop" = [rax+off] with rax = this+0x1c50, "xchg" = the atomic,
# "abs" = a rip-relative destination (the 24 B global at 0x32f97f8).
STORES = [
    (0xA5C85D, "this", 0x0000, 1),
    (0xA5C860, "this", 0x0001, 1),
    (0xA5C864, "this", 0x0002, 1),
    (0xA5C868, "this", 0x0004, 4),
    (0xA5C86F, "this", 0x0008, 4),
    (0xA5C876, "this", 0x0010, 1),  # single byte only: +0x11..+0x17 untouched (report R1)
    (0xA5C87A, "this", 0x0020, 1),
    (0xA5C885, "this", 0x0048, 8),
    (0xA5C889, "this", 0x0030, 8),
    (0xA5C88D, "this", 0x0028, 8),
    (0xA5C898, "this", 0x0038, 8),
    (0xA5C8A0, "loop", 0x1C50, 4),
    (0xA5C8A6, "loop", 0x1C54, 1),
    (0xA5C8AA, "loop", 0x1C55, 1),
    (0xA5C8AE, "loop", 0x1C56, 2),
    (0xA5C8B4, "loop", 0x1C7C, 4),
    (0xA5C8BB, "loop", 0x1C84, 4),
    (0xA5C8C2, "loop", 0x1C88, 8),
    (0xA5C8D3, "this", 0x5C50, 4),
    (0xA5C8F4, "xchg", 0x5C60, 8),
    (0xA5C902, "this", 0x5C70, 8),
    (0xA5C90D, "this", 0x5C90, 8),
    (0xA5C918, "this", 0x5C98, 4),
    (0xA5C922, "this", 0x5C80, 8),
    (0xA5C92D, "this", 0x5C88, 8),
    (0xA5C934, "this", 0x5C9C, 4),
    (0xA5C93E, "this", 0x68C0, 8),
    (0xA5C945, "this", 0x68A8, 8),
    (0xA5C94C, "this", 0x68A0, 8),
    (0xA5C95A, "this", 0x68B0, 8),
    (0xA5C961, "this", 0x69C8, 4),
    (0xA5C982, "this", 0x69DC, 32),
    (0xA5C98A, "this", 0x69CC, 32),
    (0xA5C9A2, "this", 0x6A0C, 32),
    (0xA5C9AA, "this", 0x69FC, 32),
    (0xA5C9B6, "this", 0x6A2C, 16),
    (0xA5C9BE, "this", 0x6AE0, 8),
    (0xA5C9C5, "this", 0x6AC8, 8),
    (0xA5C9CC, "this", 0x6AC0, 8),
    (0xA5C9D3, "this", 0x6AD0, 8),
    (0xA5C9DA, "this", 0x7008, 8),
    (0xA5C9E1, "this", 0x6FF0, 8),
    (0xA5C9E8, "this", 0x6FE8, 8),
    (0xA5C9EF, "this", 0x6FF8, 8),
    (0xA5CA24, "this", 0x7288, 32),
    (0xA5CA2C, "this", 0x72C8, 8),
    (0xA5CA33, "this", 0x72B0, 8),
    (0xA5CA3A, "this", 0x72A8, 8),
    (0xA5CA41, "this", 0x72B8, 8),
    (0xA5CA48, "this", 0x8BF0, 8),
    (0xA5CA4F, "this", 0x8BD8, 8),
    (0xA5CA56, "this", 0x8BD0, 8),
    (0xA5CA6B, "this", 0x8BE0, 8),
    (0xA5CA72, "this", 0xA4F8, 8),
    (0xA5CA9E, "this", 0xA520, 4),
    (0xA5CABC, "abs", 0x32F9808, 8),
    (0xA5CAD1, "abs", 0x32F97F8, 16),
    (0xA5CAD9, "this", 0x6A3C, 4),
    (0xA5CAE3, "this", 0x6A40, 4),
    (0xA5CAED, "this", 0x6A44, 32),
    (0xA5CAF5, "this", 0x6A64, 32),
    (0xA5CAFD, "this", 0x6A84, 32),
    (0xA5CB05, "this", 0x6AA4, 8),
    (0xA5CB0C, "this", 0x6AAC, 8),
    (0xA5CB13, "this", 0x6AB4, 8),
]

CALLS = [
    (0xA5C9F6, 0xF180),
    (0xA5CA02, 0xB09650),
    (0xA5CA7D, 0x3B3D0),
    (0xA5CA90, 0x3B3D0),
    (0xA5CAAB, 0x11BC0),
]

PT_LOAD = 1
PT_SCE_RELRO = 0x61000010


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def load_segments(elf: bytes) -> list[tuple[int, int, int]]:
    """(vaddr, file_offset, filesz) for every file-backed segment, in header order."""
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
            return elf[start:start + size]
    raise SystemExit(f"FAIL rva {rva:#x} (+{size:#x}) is not file-backed")


def u32(elf: bytes, segments, rva: int) -> int:
    return struct.unpack("<I", read_rva(elf, segments, rva, 4))[0]


def u8(elf: bytes, segments, rva: int) -> int:
    return read_rva(elf, segments, rva, 1)[0]


def rel32_target(elf: bytes, segments, site: int) -> int:
    rel = struct.unpack("<i", read_rva(elf, segments, site + 1, 4))[0]
    return site + 5 + rel


def rip_target(elf: bytes, segments, site: int, disp_off: int = 3, length: int = 7) -> int:
    disp = struct.unpack("<i", read_rva(elf, segments, site + disp_off, 4))[0]
    return site + length + disp


def pin(elf: bytes, segments, addr: int, pattern: str, what: str) -> None:
    size = len(pattern) // 2
    require(read_rva(elf, segments, addr, size) == bytes.fromhex(pattern), f"{what} @ {addr:#x}")


def run_claims() -> int:
    """Re-run the characterization's own 30+ assertions; return the number that passed."""
    sys.path.insert(0, str(EVIDENCE))
    import claims  # noqa: PLC0415

    buffer = io.StringIO()
    with contextlib.redirect_stdout(buffer):
        rc = claims.main()
    output = buffer.getvalue()
    require(rc == 0 and "ALL CHECKS PASSED" in output, f"claims.py assertions\n{output}")
    return sum(1 for line in output.splitlines() if line.startswith("PASS "))


def check_store_census() -> None:
    import stores  # noqa: PLC0415

    ins, decoded, calls = stores.run(BODY_START, BODY_END)
    require(len(ins) == 119, f"instruction count {len(ins)}")
    require(len(decoded) == 64, f"stores.py census {len(decoded)} (expected 64 + the xchg)")
    require(len(calls) == 5, f"call count {len(calls)}")

    # stores.py tags the loop as "this(loop)"; normalize to the table's vocabulary.
    merged = []
    for store in decoded:
        where = store["where"]
        require(where is not None, f"unclassified store at {store['addr']:#x}")
        kind = "loop" if where[0] == "this(loop)" else where[0]
        merged.append((store["addr"], kind, where[1], store["width"]))
    merged.append((0xA5C8F4, "xchg", 0x5C60, 8))
    merged.sort()
    require(merged == STORES, "65-site store census (site, kind, offset, width)")

    require(sum(1 for s in STORES if s[1] == "this") + sum(
        1 for s in STORES if s[1] == "xchg") == 56, "56 this-relative sites")
    require(sum(1 for s in STORES if s[1] == "loop") == 7, "7 loop sites")
    require(sum(1 for s in STORES if s[1] == "abs") == 2, "2 absolute sites")

    require([(c["addr"], c["target"]) for c in calls] == CALLS, "call sites/targets")


def check_loop(elf: bytes, segments) -> None:
    # `lea rax,[rbx+0x1c50]` .. `lea rcx,[rbx+0x5c50]`; `add rax,0x40`; `cmp rax,rcx`; `jne`.
    pin(elf, segments, 0xA5C891, "488d83501c0000", "loop cursor = this+0x1c50")
    pin(elf, segments, 0xA5C87E, "488d8b505c0000", "loop limit = this+0x5c50")
    pin(elf, segments, 0xA5C8A0, "c700ffffffff", "record +0x00 = -1")
    pin(elf, segments, 0xA5C8C2, "48c7403800000000", "record +0x38 qword 0")
    pin(elf, segments, 0xA5C8CA, "4883c040", "stride 0x40")
    pin(elf, segments, 0xA5C8CE, "4839c8", "cmp rax,rcx")
    pin(elf, segments, 0xA5C8D1, "75cd", "jne back to the loop head")

    start = u32(elf, segments, 0xA5C894)  # disp32 of the +0x1c50 lea (48 8d 83 + disp32)
    limit = u32(elf, segments, 0xA5C881)  # disp32 of the +0x5c50 lea (48 8d 8b + disp32)
    stride = u8(elf, segments, 0xA5C8CD)  # imm8 of `add rax,0x40`
    require(start == 0x1C50 and limit == 0x5C50 and stride == 0x40,
            f"loop immediates {start:#x}/{limit:#x}/{stride:#x}")
    require((limit - start) % stride == 0, "loop does not divide evenly")
    require((limit - start) // stride == 0x100, "loop trips != 256")


def check_constants(elf: bytes, segments) -> None:
    require(read_rva(elf, segments, 0xFAB6B0, 48) ==
            struct.pack("<4f", 1.0, 0.0, 0.0, 0.0) * 3, "0xfab6b0 == 3x {1,0,0,0}")
    require(read_rva(elf, segments, 0x12C49A0, 32) == struct.pack("<8f", *([1.0] * 8)),
            "0x12c49a0 == 8x 1.0f")
    require(read_rva(elf, segments, 0x12C4D1B, 6) == b"Sound\x00", '0x12c4d1b == "Sound"')
    require(read_rva(elf, segments, 0xF97DF3, 13) == b"PS4SyncEvent\x00",
            '0xf97df3 == "PS4SyncEvent"')
    # 0x3b3d0 object is 0x10 B {qword handle, u8 flag} and both calls store byte +8 = 0.
    pin(elf, segments, 0x3B3EA, "0fb6ca", "event initial pattern widened from dl")
    pin(elf, segments, 0x3B410, "488903", "store the handle at +0")
    pin(elf, segments, 0x3B413, "44887308", "store byte +8 = r14b (0 for both calls)")
    require(rel32_target(elf, segments, 0x3B407) == 0xF8F950,
            "0x3b3d0 -> 0xf8f950 (sceKernelCreateEventFlag PLT)")


def check_call_arguments(elf: bytes, segments) -> None:
    # 0xf180 / 0xb09650 targets.
    pin(elf, segments, 0xA5C8ED, "488dbb50720000", "rdi = this+0x7250 for 0xf180")
    pin(elf, segments, 0xA5C9FB, "488dbb70720000", "rdi = this+0x7270 for 0xb09650")
    # 0x3b3d0 #1: rdi = this+0xa500, esi = 0, edx = 0 (hoisted at 0xa5ca20/0xa5ca22).
    pin(elf, segments, 0xA5CA64, "488dbb00a50000", "rdi = this+0xa500")
    pin(elf, segments, 0xA5CA20, "31f6", "esi = 0 for the first event flag")
    pin(elf, segments, 0xA5CA22, "31d2", "edx = 0 (initial pattern 0)")
    # 0x3b3d0 #2: rdi = this+0xa510, esi = 0, edx = 1.
    pin(elf, segments, 0xA5CA82, "488dbb10a50000", "rdi = this+0xa510")
    pin(elf, segments, 0xA5CA89, "31f6", "esi = 0 for the second event flag")
    pin(elf, segments, 0xA5CA8B, "ba01000000", "edx = 1 (initial pattern 1)")
    # 0x11bc0: rdi = "Sound", rsi = rdi, edx = 0 (create == false), and the +0xa520 store
    # happens *before* the call (it is part of the failure prefix).
    pin(elf, segments, 0xA5CA95, "488d3d7f828600", 'rdi = "Sound"')
    pin(elf, segments, 0xA5CAA8, "4889fe", "rsi = rdi (name2 == name1)")
    pin(elf, segments, 0xA5CA9C, "31d2", "edx = 0 (create == false)")
    pin(elf, segments, 0xA5CA9E, "c78320a5000000000000", "+0xa520 = 0 before the lookup")
    # 24 B global: 8 B from record+0x10, then 16 B from record+0x00.
    pin(elf, segments, 0xA5CAB0, "488b4810", "load record+0x10")
    pin(elf, segments, 0xA5CABC, "48890d45cd8902", "store 8 B at 0x32f9808")
    pin(elf, segments, 0xA5CAC3, "c5f81000", "load record+0x00 (16 B)")
    pin(elf, segments, 0xA5CAD1, "c5f811051fcd8902", "store 16 B at 0x32f97f8")
    require(rip_target(elf, segments, 0xA5CABC) == 0x32F9808, "absolute store 1")
    require(rip_target(elf, segments, 0xA5CAD1, 4, 8) == 0x32F97F8,
            "absolute store 2 (VEX: disp32 at +4)")
    require(0xA5CABC < 0xA5CAD1, "24 B copy order: high qword first")
    # The 32 x 1.0f tail.
    pin(elf, segments, 0xA5CAD9, "c7833c6a00000000803f", "dword 0x6a3c = 1.0f")
    pin(elf, segments, 0xA5CAE3, "c783406a00000000803f", "dword 0x6a40 = 1.0f")
    pin(elf, segments, 0xA5CB05, "488983a46a0000", "qword 0x6aa4 = 2x 1.0f")
    pin(elf, segments, 0xA5CAED, "c5fc118b446a0000", "ymm 0x6a44 = 8x 1.0f")


def check_row_tuple(elf: bytes, segments) -> None:
    """`kOwnerMemberRows[9]`: {size 0xa530, ctor 0xa5c850, slot 0x32f8ff0, dtor-then-free}."""
    pin(elf, segments, 0x256CC3, "bf30a50000", "row site: mov edi, 0xa530")
    require(rel32_target(elf, segments, 0x256CC8) == 0x17070, "row site -> allocator 0x17070")
    require(rel32_target(elf, segments, 0x256CD3) == 0xA5C850, "row site -> ctor 0xa5c850")
    pin(elf, segments, 0x256CD8, "4c8d2511230a03", "row site: lea r12,[rip+...]")
    require(rip_target(elf, segments, 0x256CD8) == 0x32F8FF0, "row site -> 0x32f8ff0")
    require(rel32_target(elf, segments, 0x256CF0) == 0xA5CB30, "row teardown -> dtor 0xa5cb30")
    pin(elf, segments, 0x256CF0, "e83b5e8000", "dtor call encoding")
    require(rel32_target(elf, segments, 0x256CF8) == 0x170B0, "row teardown -> free 0x170b0")
    pin(elf, segments, 0x256CF8, "e8b303dcff", "free call encoding")
    pin(elf, segments, 0x256CFD, "4d893c24", "clear the row pointer slot")


def check_native_sources() -> None:
    header = HEADER.read_text()
    for token in (
        "kOwnerCtorA5C850Rva = 0xA5C850",
        "kOwnerCtorA5C850Size = 0x2D1",
        "kOwnerCtorA5C850ObjSize = 0xA530",
        "kOwnerCtorA5C850SlotRva = 0x32F8FF0",
        "kOwnerCtorA5C850SoundRva = 0x32F97F8",
        "kOwnerDtorA5CB30Rva = 0xA5CB30",
        "kOwnerDtorA5CB30Size = 0x252",
        "kOwnerA5C850RecCount = 0x100",
        "kOwnerA5C850RecStride = 0x40",
        "kOwnerA5C850LaneCount = 0x20",
        "kOwnerA5C850LaneStride = 0x60",
        "NOT a mutex",
        "create == false",
        "report R1",
        "report R5",
        "KernelCreateEventFlag",
    ):
        require(token in header, f"header token {token!r}")

    source = SOURCE.read_text()
    for token in (
        "kOwnerA5C850RecCount",
        "kOwnerA5C850RecStride",
        "std::atomic_ref<std::uint64_t>",
        "memory_order_seq_cst",
        "init_slot_f180",
        "kOnesConstant",
        "kQuadConstant",
        '"tree_init"',
        '"event_flag_0"',
        '"event_flag_1"',
        '"name_record"',
        '"null_payload"',
        '"null-object"',
        '"sound-record"',
        "0x69DC",
        "0x69CC",
        "kOwnerA5C850ConstZeroOffset",
        "kOwnerA5C850TreeBodyOffset",
        "kOwnerA5C850PolyOffset",
        "kOwnerA5C850TailOffset",
    ):
        require(token in source, f"source token {token!r}")
    require("memset(o, 0" not in source, "source must not zero-fill the object")
    require(source.count("put_bytes") >= 8, "source no longer writes the wide constant blocks")

    test = TEST.read_text()
    for token in (
        "std::size(kStores) == 63",
        "in_loop == 7",
        "== 56",
        "is_written",
        "0xA524",
        "0x7270",
        "null_payload",
        "event_flag_1",
        "0x5C9C",
        "0x6A3C",
    ):
        require(token in test, f"test token {token!r}")


def main() -> int:
    require(ELF_PATH.is_file(), f"{ELF_PATH} is absent (private game image required)")
    require(EVIDENCE.is_dir(), f"{EVIDENCE} is absent (private characterization report required)")
    elf = ELF_PATH.read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    segments = load_segments(elf)

    body = read_rva(elf, segments, BODY_START, BODY_END - BODY_START)
    require(len(body) == 0x2D1, f"body size {len(body):#x}")
    require(hashlib.sha256(body).hexdigest() == BODY_SHA256, "body sha256")
    require(read_rva(elf, segments, BODY_END, 15) == b"\x90" * 15, "15 B 0x90 padding")
    require(read_rva(elf, segments, 0xA5CB30, 4) == bytes.fromhex("554889e5"),
            "dtor 0xa5cb30 prologue (contract only, not modelled)")
    require(body[0] == 0x55 and body[-1] == 0xC3, "push rbp / ret boundaries")
    require(b"\x64\x48\x8b\x04\x25" not in body, "unexpected stack canary")

    claims_passed = run_claims()
    check_store_census()
    check_loop(elf, segments)
    check_constants(elf, segments)
    check_call_arguments(elf, segments)
    check_row_tuple(elf, segments)
    check_native_sources()

    print(
        f"OK check_fowner_ctor_a5c850: body 0x2d1 sha256 {BODY_SHA256[:12]}..., "
        f"{len(STORES)} store sites (56 this + 7 loop + 2 abs), 256 trips x 0x40, "
        f"5 calls / 4 targets, 2 constant blocks, 24 B global split at 0x32f97f8, "
        f"row tuple 0xa530/0x256cc8/0x32f8ff0, claims.py {claims_passed} assertions; "
        f"guest bytes NOT executed"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
