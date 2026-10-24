#!/usr/bin/env python3
"""ELF-tie the lifted row-14 owner constructor `0xa597d0` (`kOwnerMemberRows[14]`).

Private: needs `local/re/eboot.elf`. Read-only and offline: instructions are decoded and byte ranges
are hashed; **no guest byte is executed**. No emulator, interpreter, JIT or guest runtime.

Layers:
  1. Body extent/hash: `0xa597d0..0xa599d2`, `0x202` B, FDE-confirmed, 14 B of `0x90` padding and
     the next function at `0xa599e0`. This also pins the report correction: the E570 walk overshoots
     into the padding and reports `0x215` / 114 instructions.
  2. The ordered store census: 30 sites (25 `this`, 1 map slot, 3 loop, 1 stack frame slot), each
     pinned by its raw instruction bytes and its decoded offset/width.
  3. Both loops: the single-trip `0x17090(0x100)` allocation loop and the 64-trip `-0x800 / 0x20`
     clear loop, with the arithmetic re-derived from the immediates.
  4. Call sites/targets in order, the 7 `0x3b5c0` arguments, the event/notifier arguments, the
     RELRO callable slot, and the `{0x958, 0xa597d0, 0x32f8b80, dtor + 0x170b0}` row tuple.
  5. The native header/source/test must still carry the constants and the hook/failure contract.
"""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ELF_PATH = ROOT / "local/re/eboot.elf"

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
BODY_START, BODY_END = 0xA597D0, 0xA599D2
BODY_SHA256 = "1c8596ab15884f169300e6712ebbcc41dee261fed36ff1369b6c9469f45142ec"

HEADER = ROOT / "native/core/owner_ctor_a597d0.hpp"
SOURCE = ROOT / "native/core/owner_ctor_a597d0.cpp"
TEST = ROOT / "native/tests/test_owner_ctor_a597d0.cpp"

# 30 store sites in instruction order:
#   (addr, kind, disp, width, disp_off, disp_size, raw bytes)
# kind: "this" = [rbx+disp], "block" = [r13] (the map slot), "loop" = [rbx+rax+disp],
# "stack" = [rsp] (the 0x3b5c0 stack argument). `disp_off == -1` means the instruction carries no
# displacement immediate.
STORES = [
    (0xA597FF, "this", 0x000, 4, -1, 0, "c70301000000"),
    (0xA59805, "this", 0x008, 16, 4, 1, "c5f8114308"),
    (0xA5980E, "this", 0x018, 8, 3, 1, "48c7431800000000"),
    (0xA59816, "this", 0x028, 4, 2, 1, "c7432800000100"),
    (0xA5981D, "this", 0x860, 32, 4, 4, "c5fc118360080000"),
    (0xA59825, "this", 0x840, 32, 4, 4, "c5fc118340080000"),
    (0xA5982D, "this", 0x838, 8, 3, 4, "48c7833808000008000000"),
    (0xA59844, "this", 0x830, 8, 3, 4, "4c89bb30080000"),
    (0xA5986A, "block", 0x000, 8, -1, 0, "49894500"),
    (0xA59886, "this", 0x858, 8, 3, 4, "4c89a358080000"),
    (0xA59894, "this", 0x848, 8, 3, 4, "48898348080000"),
    (0xA5989B, "this", 0x850, 8, 3, 4, "48898b50080000"),
    (0xA598A2, "this", 0x840, 8, 3, 4, "48898340080000"),
    (0xA598A9, "this", 0x878, 8, 3, 4, "4c89a378080000"),
    (0xA598B0, "this", 0x868, 8, 3, 4, "48898368080000"),
    (0xA598B7, "this", 0x870, 8, 3, 4, "48898b70080000"),
    (0xA598BE, "this", 0x860, 8, 3, 4, "48898360080000"),
    (0xA598C5, "this", 0x888, 8, 3, 4, "48c7838808000000000000"),
    (0xA598D0, "this", 0x8B0, 8, 3, 4, "48c783b008000000000000"),
    (0xA5990B, "stack", 0x000, 8, -1, 0, "48c7042400001000"),
    (0xA59934, "this", 0x938, 16, 4, 4, "c5fa7f8338090000"),
    (0xA5993C, "this", 0x948, 8, 3, 4, "48c7834809000000000000"),
    (0xA59950, "loop", 0x830, 1, 3, 4, "c684033008000000"),
    (0xA59958, "loop", 0x838, 16, 5, 4, "c5fa7f840338080000"),
    (0xA59961, "loop", 0x848, 4, 3, 4, "c784034808000000000000"),
    (0xA59972, "this", 0x029, 1, 2, 1, "c6432900"),
    (0xA59976, "this", 0x02B, 1, 2, 1, "c6432b01"),
    (0xA5999B, "this", 0x890, 1, 2, 4, "c6839008000000"),
    (0xA599A2, "this", 0x898, 16, 4, 4, "c5fa7f8398080000"),
    (0xA599AA, "this", 0x8A8, 4, 2, 4, "c783a808000000000000"),
]

CALLS = [
    (0xA59838, 0x17090),
    (0xA59865, 0x17090),
    (0xA598DB, 0x3B3D0),
    (0xA59924, 0x3B5C0),
    (0xA5998B, 0xBA40),
    (0xA599CC, 0xF8F3A0),  # __stack_chk_fail, compiler boilerplate
]

PT_LOAD = 1
PT_SCE_RELRO = 0x61000010


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
            return elf[start:start + size]
    raise SystemExit(f"FAIL rva {rva:#x} (+{size:#x}) is not file-backed")


def u8(elf: bytes, segments, rva: int) -> int:
    return read_rva(elf, segments, rva, 1)[0]


def u32(elf: bytes, segments, rva: int) -> int:
    return struct.unpack("<I", read_rva(elf, segments, rva, 4))[0]


def i32(elf: bytes, segments, rva: int) -> int:
    return struct.unpack("<i", read_rva(elf, segments, rva, 4))[0]


def u64(elf: bytes, segments, rva: int) -> int:
    return struct.unpack("<Q", read_rva(elf, segments, rva, 8))[0]


def rel32_target(elf: bytes, segments, site: int) -> int:
    return site + 5 + i32(elf, segments, site + 1)


def rel8_target(elf: bytes, segments, site: int) -> int:
    return site + 2 + struct.unpack("<b", read_rva(elf, segments, site + 1, 1))[0]


def rip_target(elf: bytes, segments, site: int, disp_off: int, length: int) -> int:
    return site + length + i32(elf, segments, site + disp_off)


def pin(elf: bytes, segments, addr: int, pattern: str, what: str) -> None:
    size = len(pattern) // 2
    require(read_rva(elf, segments, addr, size) == bytes.fromhex(pattern), f"{what} @ {addr:#x}")


def check_body(elf: bytes, segments) -> bytes:
    body = read_rva(elf, segments, BODY_START, BODY_END - BODY_START)
    require(len(body) == 0x202, f"body size {len(body):#x}")
    require(hashlib.sha256(body).hexdigest() == BODY_SHA256, "body sha256")
    require(body[0] == 0x55, "body does not start with push rbp")
    require(u8(elf, segments, 0xA599CB) == 0xC3, "no ret at 0xa599cb")
    require(read_rva(elf, segments, BODY_END, 14) == b"\x90" * 14, "14 B 0x90 padding")
    pin(elf, segments, 0xA599E0, "40887729c3", "next function after the padding")
    # The E570 walk overshoots into the padding and the next function; pin that the 0x215 window is
    # not the body so the correction stays visible in the evidence.
    require(hashlib.sha256(read_rva(elf, segments, BODY_START, 0x215)).hexdigest() != BODY_SHA256,
            "0x215 must not be the body extent")
    return body


def check_stores(elf: bytes, segments) -> None:
    counts = {kind: 0 for kind in ("this", "block", "loop", "stack")}
    for _, kind, _, _, _, _, _ in STORES:
        counts[kind] += 1
    require(counts["this"] == 25, f"this store count {counts['this']}")
    require(counts["loop"] == 3, f"loop store count {counts['loop']}")
    require(counts["block"] == 1, f"map-slot store count {counts['block']}")
    require(counts["stack"] == 1, f"stack store count {counts['stack']}")
    require(len(STORES) == 30, "store census size")

    previous = BODY_START
    for addr, _, _, _, _, _, raw in STORES:
        require(addr > previous, f"store sites out of order at {addr:#x}")
        previous = addr
        pin(elf, segments, addr, raw, f"store site {addr:#x}")

    for addr, kind, disp, width, disp_off, disp_size, _ in STORES:
        if disp_off < 0:
            continue
        raw_disp = read_rva(elf, segments, addr + disp_off, disp_size)
        found = int.from_bytes(raw_disp, "little")
        require(found == disp, f"{kind} disp at {addr:#x}: {found:#x} != {disp:#x}")

    # Every object store stays below the 0x958 allocation; +0x950..+0x957 is never written.
    assert_widths = []
    for _, kind, disp, width, _, _, _ in STORES:
        if kind == "this":
            require(disp + width <= 0x958, f"store past the object at {disp:#x}")
            assert_widths.append((disp, width))
        if kind == "loop":
            # Effective address is rbx + rax + disp with rax in {-0x800 .. -0x20}; the record block
            # is therefore +0x30..+0x830, not the raw displacement.
            require(disp - 0x800 >= 0x30, f"loop store before the block at {disp:#x}")
            require(disp - 0x20 + width <= 0x830, f"loop store past its footprint at {disp:#x}")
            if disp == 0x830:
                require(disp - 0x800 == 0x30, "loop record base is not +0x30")
    top = max(disp + width for disp, width in assert_widths)
    require(top == 0x950, f"highest written object byte {top:#x} (expected 0x950)")
    require(0x950 <= 0x958, "the 0x958 size does not cover the highest store")


def check_loops(elf: bytes, segments) -> None:
    # 0x17090(0x40) then a single-trip loop that allocates one 0x100 B chunk into map+0x18.
    pin(elf, segments, 0xA597EF, "bf40000000", "mov edi, 0x40")
    pin(elf, segments, 0xA59860, "bf00010000", "mov edi, 0x100")
    pin(elf, segments, 0xA59840, "4d8d6718", "lea r12,[r15+0x18]")
    pin(elf, segments, 0xA5984B, "4983c720", "add r15,0x20")
    pin(elf, segments, 0xA5984F, "4d89e5", "mov r13,r12")
    pin(elf, segments, 0xA59872, "4d39fd", "cmp r13,r15")
    pin(elf, segments, 0xA59875, "72e9", "jb back to the allocation")
    require(rel8_target(elf, segments, 0xA59875) == 0xA59860, "allocation loop target")
    map_size, chunk_size = u32(elf, segments, 0xA597F0), u32(elf, segments, 0xA59861)
    require(map_size == 0x40 and chunk_size == 0x100, f"alloc sizes {map_size:#x}/{chunk_size:#x}")
    require((0x20 - 0x18) // 8 == 1, "allocation loop is not single-trip")
    require(map_size // 8 == 8, "map block does not hold 8 pointers")

    # The clear loop: rax = -0x800, stride 0x20, `jne` to the body.
    pin(elf, segments, 0xA5992D, "48c7c000f8ffff", "mov rax, -0x800")
    pin(elf, segments, 0xA5996C, "4883c020", "add rax, 0x20")
    pin(elf, segments, 0xA59970, "75de", "jne back to the body")
    start = i32(elf, segments, 0xA59930)
    stride = u8(elf, segments, 0xA5996F)
    require(start == -0x800, f"clear-loop start {start}")
    require(stride == 0x20, f"clear-loop stride {stride:#x}")
    require(rel8_target(elf, segments, 0xA59970) == 0xA59950, "clear-loop target")
    require(0x830 + start == 0x30, "clear-loop base is not +0x30")
    require((-start) // stride == 64, "clear-loop trips are not 64")
    require(0x30 + 64 * stride == 0x830, "clear-loop limit is not +0x830")


def check_calls(elf: bytes, segments) -> None:
    seen = []
    for site, expect in CALLS:
        pin(elf, segments, site, "e8", f"call opcode at {site:#x}")
        target = rel32_target(elf, segments, site)
        require(target == expect, f"call {site:#x} -> {target:#x} (expected {expect:#x})")
        seen.append((site, target))
    require(seen == CALLS, "call order/targets")


def check_thread_call(elf: bytes, segments) -> None:
    # rdi = this+0x8c8, rsi = &callable, rdx = name, rcx = 0x2bc, r8 = 0x60, r9 = 1, [rsp] = 0x100000
    pin(elf, segments, 0xA598E8, "488dbbc8080000", "rdi = this+0x8c8")
    pin(elf, segments, 0xA598F6, "488d75b0", "rsi = &callable frame")
    pin(elf, segments, 0xA598EF, "488d159caf8600", "rdx = thread name")
    pin(elf, segments, 0xA598FA, "b9bc020000", "rcx = 0x2bc")
    pin(elf, segments, 0xA598FF, "41b860000000", "r8d = 0x60")
    pin(elf, segments, 0xA59905, "41b901000000", "r9d = 1")
    pin(elf, segments, 0xA5990B, "48c7042400001000", "[rsp] = 0x100000")
    require(u32(elf, segments, 0xA598FB) == 0x2BC, "thread arg3")
    require(u32(elf, segments, 0xA59901) == 0x60, "thread arg4")
    require(u32(elf, segments, 0xA59907) == 1, "thread arg5")
    require(u32(elf, segments, 0xA5990F) == 0x100000, "thread stack arg")

    # Callable frame {this, relocated slot, 0, 0}.
    pin(elf, segments, 0xA598E0, "c5fa7e052066ba00", "vmovq from the RELRO slot")
    pin(elf, segments, 0xA59913, "c5fa7f45b8", "store the qword pair")
    pin(elf, segments, 0xA59918, "48895db0", "callable[0] = this")
    pin(elf, segments, 0xA5991C, "48c745c800000000", "callable[2..3] = 0")
    slot = rip_target(elf, segments, 0xA598E0, 4, 8)
    require(slot == 0x15FFF08, f"RELRO slot {slot:#x}")
    require(u64(elf, segments, slot) == 0xB09590, "RELRO slot does not hold 0xb09590")
    require(read_rva(elf, segments, 0x12C4892, 19) == b"Trophy Task Thread\x00",
            "thread name literal")

    # Event flag: 0x3b3d0(this+0x8b8, 0, 0).
    pin(elf, segments, 0xA5987B, "488dbbb8080000", "rdi = this+0x8b8")
    pin(elf, segments, 0xA59882, "31f6", "esi = 0")
    pin(elf, segments, 0xA59884, "31d2", "edx = 0")

    # Startup-mode notifier: 0xba40(0x32f8b90, *(0x32f8bf0), &this->0x2b).
    pin(elf, segments, 0xA597F4, "4c8d732b", "r14 = this+0x2b")
    pin(elf, segments, 0xA5997A, "488d3d0ff28902", "rdi = 0x32f8b90")
    pin(elf, segments, 0xA59984, "488b3565f28902", "rsi = *(0x32f8bf0)")
    pin(elf, segments, 0xA59981, "4c89f2", "rdx = r14")
    pin(elf, segments, 0xA59976, "c6432b01", "+0x2b = 1 before the call")
    require(rip_target(elf, segments, 0xA5997A, 3, 7) == 0x32F8B90, "mode config global")
    require(rip_target(elf, segments, 0xA59984, 3, 7) == 0x32F8BF0, "mode cell global")

    # Head/store constants and the compiler's stack check.
    pin(elf, segments, 0xA597FF, "c70301000000", "dword +0x00 = 1")
    pin(elf, segments, 0xA59816, "c7432800000100", "dword +0x28 = 0x10000")
    pin(elf, segments, 0xA5982D, "48c7833808000008000000", "qword +0x838 = 8")
    pin(elf, segments, 0xA599AA, "c783a808000000000000", "dword +0x8a8 = 0")
    pin(elf, segments, 0xA599CC, "e8cf595300", "__stack_chk_fail tail")
    require(rel32_target(elf, segments, 0xA599CC) == 0xF8F3A0, "stack fail target")


def check_row_tuple(elf: bytes, segments) -> None:
    """`kOwnerMemberRows[14]`: {size 0x958, ctor 0xa597d0, slot 0x32f8b80, dtor + free}."""
    pin(elf, segments, 0x256DBC, "bf58090000", "row site: mov edi, 0x958")
    require(rel32_target(elf, segments, 0x256DC1) == 0x17070, "row site -> allocator 0x17070")
    require(rel32_target(elf, segments, 0x256DCC) == 0xA597D0, "row site -> ctor 0xa597d0")
    require(rip_target(elf, segments, 0x256DD1, 3, 7) == 0x32F8B80, "row site -> 0x32f8b80")
    pin(elf, segments, 0x256DD1, "4c8d2da81d0a03", "row site: lea r13,[rip+...]")
    require(rel32_target(elf, segments, 0x256DE9) == 0xA59A10, "row teardown -> dtor 0xa59a10")
    require(rel32_target(elf, segments, 0x256DF1) == 0x170B0, "row teardown -> free 0x170b0")
    pin(elf, segments, 0x256DF6, "4d896500", "store the new pointer into the slot")
    pin(elf, segments, 0xA59A10, "554889e5", "dtor 0xa59a10 prologue (contract only)")


def check_native_sources() -> None:
    header = HEADER.read_text()
    for token in (
        "kOwnerCtorA597D0Rva = 0xA597D0",
        "kOwnerCtorA597D0Size = 0x202",
        "kOwnerCtorA597D0ObjSize = 0x958",
        "kOwnerCtorA597D0SlotRva = 0x32F8B80",
        "kOwnerDtorA59A10Rva = 0xA59A10",
        "kOwnerDtorA59A10Size = 0x1A7",
        "kOwnerA597D0MapBytes = 0x40",
        "kOwnerA597D0MapSlots = 8",
        "kOwnerA597D0ChunkBytes = 0x100",
        "kOwnerA597D0RecordCount = 64",
        "kOwnerA597D0RecordStride = 0x20",
        "kOwnerA597D0ThreadEntryRva = 0xB09590",
        "kOwnerA597D0ThreadEntrySlotRva = 0x15FFF08",
        "kOwnerA597D0ThreadNameRva = 0x12C4892",
        "kOwnerA597D0ThreadArg3 = 0x2BC",
        "kOwnerA597D0ThreadArg4 = 0x60",
        "kOwnerA597D0ThreadArg6 = 0x100000",
        "kOwnerA597D0ModeConfigRva = 0x32F8B90",
        "kOwnerA597D0ModeCellRva = 0x32F8BF0",
        "Trophy Task Thread",
        "0x215",
        "not reproduced",
        "object is partially",
        "initialized",
    ):
        require(token in header, f"header token {token!r}")

    source = SOURCE.read_text()
    for token in (
        '"alloc_block"',
        '"alloc_chunk"',
        '"event_flag"',
        '"thread_create"',
        '"startup_mode"',
        "kOwnerA597D0RecordCount",
        "kOwnerA597D0ChunkTrips",
        "put_bytes",
        "owner_row14_ctor_hook",
        "owner_row14_bind",
        "kOwnerA597D0ThreadEntryRva",
    ):
        require(token in source, f"source token {token!r}")
    require("memset(o, 0" not in source, "source must not zero-fill the object")
    require(source.count("put_pointer") >= 10, "source no longer writes the deque header")

    test = TEST.read_text()
    for token in (
        "Prefix::kFull",
        "Prefix::kHead",
        "Prefix::kMapPointer",
        "Prefix::kIterators",
        "Prefix::kEvent",
        "Prefix::kThread",
        "guards_ok",
        "kOwnerA597D0ThreadArg6",
        "0x100000",
        "owner_row14_ctor_hook",
        "std::is_same_v<decltype(OwnerMemberHooks::ctor)",
        "0x2A",
        "0x880",
        "0x950",
    ):
        require(token in test, f"test token {token!r}")


def main() -> int:
    require(ELF_PATH.is_file(), f"{ELF_PATH} is absent (private game image required)")
    elf = ELF_PATH.read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    segments = load_segments(elf)

    check_body(elf, segments)
    check_stores(elf, segments)
    check_loops(elf, segments)
    check_calls(elf, segments)
    check_thread_call(elf, segments)
    check_row_tuple(elf, segments)
    check_native_sources()

    print(
        f"OK check_fowner_ctor_a597d0: body 0x202 sha256 {BODY_SHA256[:12]}..., "
        f"30 store sites (25 this + 1 map + 3 loop + 1 stack), 0x40/0x100 allocations "
        f"({0x40 // 8} map slots, 1 chunk trip), 64-trip clear loop, "
        f"6 calls / 5 targets, 7-arg 0x3b5c0 frame, RELRO slot 0x15fff08 -> 0xb09590, "
        f"row tuple 0x958/0x256dc1/0x32f8b80; guest bytes NOT executed"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
