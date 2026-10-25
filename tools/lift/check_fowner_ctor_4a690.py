#!/usr/bin/env python3
"""ELF-tie the lifted row-21 owner constructor `0x4a690` (`kOwnerMemberRows[21]`).

Private: needs `local/re/eboot.elf` and the characterization evidence in
`local/re/evidence/owner-row21-4a690/`. Read-only and offline: instructions are decoded and byte
ranges are hashed; **no guest byte is executed**. No emulator, interpreter, JIT or guest runtime.

Layers:
  1. Body extent/hash, prologue/epilogue, the 0x90 padding after it, no stack canary.
  2. The private `census.py` re-derives the instruction count (104 = 100 real + 4 nop pads), the
     eight call sites/six targets, the 17 memory stores (12 object/anchor, 3 stack spills, 2
     indirect) and the 12-entry branch table.
  3. Raw-byte pins: every store encoding with its offset/width, the 0x48 node size, the key offset
     0x20 and the child offsets 0x00/0x10, the four tree-cell `lea` targets (all 0x2ea8980), the
     `strcmp` PLT identity, the `0x7d03` tag and the row-21 install/teardown site.
  4. The insert primitive `0x4b3f0` + `0x4c500`: `new(0x48)`, key copy offsets, the `lock incq`
     control acquire, the mapped zero, the `+0x40` return and the node-count increment.
  5. The destructor is a recorded contract only (extent, prologue, call order).
  6. The native header/source/test still carry the constants and the hook/failure contract.
"""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
EVIDENCE = ROOT / "local/re/evidence/owner-row21-4a690"
ELF_PATH = ROOT / "local/re/eboot.elf"

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
BODY_START, BODY_END = 0x4A690, 0x4A824
BODY_SHA256 = "c0771efb29d101f447dbdbb97a7e2a40871e46e04cab6391f76c5363792d4d24"

HEADER = ROOT / "native/core/owner_ctor_4a690.hpp"
SOURCE = ROOT / "native/core/owner_ctor_4a690.cpp"
TEST = ROOT / "native/tests/test_owner_ctor_4a690.cpp"

CALLS = [
    (0x4A6BC, 0xF180),
    (0x4A6C6, 0xF180),
    (0x4A6CB, 0x511F0),
    (0x4A6E8, 0x3B310),
    (0x4A700, 0x17070),
    (0x4A787, 0xF8F500),
    (0x4A7D1, 0xF8F500),
    (0x4A7F3, 0x4B3F0),
]
DISTINCT_TARGETS = sorted({target for _, target in CALLS})

# The 17 memory-destination instructions, in address order, exactly as `census.py` classifies
# them: 12 writes into the object/anchor, 3 frame spills and 2 indirect stores.
STORES = [
    (0x4A6AD, "vmovups", "xmmword ptr [r12]"),             # +0x00, 16 B zero
    (0x4A6B3, "mov", "qword ptr [r12 + 0x10]"),            # +0x10, qword 0
    (0x4A6D5, "mov", "qword ptr [r12 + 0x58]"),            # stamp.first
    (0x4A6DA, "mov", "qword ptr [r12 + 0x60]"),            # stamp.second
    (0x4A6DF, "mov", "dword ptr [r12 + 0x68]"),            # 0x7d03
    (0x4A6F6, "vmovups", "xmmword ptr [r12 + 0x80]"),      # anchor cell + count zeroed
    (0x4A70F, "mov", "qword ptr [r13]"),                   # anchor->left = anchor
    (0x4A713, "mov", "qword ptr [r13 + 0x8]"),             # anchor->parent = anchor
    (0x4A717, "mov", "qword ptr [r13 + 0x10]"),            # anchor->right = anchor
    (0x4A71B, "mov", "byte ptr [r13 + 0x18]"),             # colour = 1
    (0x4A720, "mov", "byte ptr [r13 + 0x19]"),             # is_anchor = 1
    (0x4A725, "mov", "qword ptr [r12 + 0x80]"),            # anchor pointer installed
    (0x4A742, "mov", "qword ptr [rbp - 0x38]"),            # spill: &this+0x78
    (0x4A746, "mov", "qword ptr [rbp - 0x40]"),            # spill: this+0x80
    (0x4A769, "mov", "qword ptr [rbp - 0x30]"),            # spill: &entry->value
    (0x4A7FF, "mov", "qword ptr [rax]"),                   # *mapped = entry
    (0x4A809, "mov", "qword ptr [rdx]"),                   # *(0x2ea8980) = next
]

BRANCHES = [
    (0x4A733, "je", 0x4A815),
    (0x4A74A, "jmp", 0x4A754),
    (0x4A763, "jne", 0x4A7EA),
    (0x4A78E, "jns", 0x4A7A0),
    (0x4A799, "je", 0x4A780),
    (0x4A79B, "jmp", 0x4A7B0),
    (0x4A7AA, "je", 0x4A773),
    (0x4A7AC, "jmp", 0x4A7B3),
    (0x4A7C8, "je", 0x4A7EA),
    (0x4A7ED, "jne", 0x4A805),
    (0x4A80F, "jne", 0x4A750),
    (0x4A823, "ret", None),
]

# Instruction encodings pinned byte-for-byte (site, hex, what).
PINNED = [
    (0x4A690, "55", "push rbp"),
    (0x4A691, "4889e5", "mov rbp,rsp"),
    (0x4A69D, "4883ec18", "sub rsp,0x18"),
    (0x4A6A1, "4989fc", "r12 = this"),
    (0x4A6A4, "c5f857c0", "vxorps xmm0,xmm0,xmm0"),
    (0x4A6A8, "498d7c2418", "lea rdi,[r12+0x18] (first 0xf180 slot)"),
    (0x4A6C1, "498d7c2438", "lea rdi,[r12+0x38] (second 0xf180 slot)"),
    (0x4A6D0, "498d7c2470", "lea rdi,[r12+0x70] (mutex hook argument)"),
    (0x4A6DF, "41c7442468037d0000", "dword [this+0x68] = 0x7d03"),
    (0x4A6F1, "bf48000000", "mov edi,0x48 (node size)"),
    (0x4A705, "488d1574e2e502", "lea rdx,[0x2ea8980]"),
    (0x4A72D, "4c8b32", "r14 = *(0x2ea8980) (queue head)"),
    (0x4A739, "498d442478", "lea rax,[r12+0x78] (tree handle)"),
    (0x4A73E, "4983ec80", "sub r12,-0x80 (this+0x80)"),
    (0x4A750, "4d8b2c24", "r13 = *(this+0x80) (anchor)"),
    (0x4A754, "498b4508", "root = [anchor+0x08]"),
    (0x4A758, "498d7608", "rsi = &entry->value (entry+0x08)"),
    (0x4A75F, "80781900", "cmp byte [root+0x19],0 (is_anchor)"),
    (0x4A769, "488975d0", "spill &entry->value"),
    (0x4A76D, "4d89ec", "last_left = anchor"),
    (0x4A770, "488b1e", "rbx = [&entry->value] (key text)"),
    (0x4A773, "4989c7", "node = root"),
    (0x4A780, "498b7f20", "compare arg1 = [node+0x20] (stored key)"),
    (0x4A784, "4889de", "compare arg2 = probe"),
    (0x4A78E, "7910", "jns (order >= 0 -> left)"),
    (0x4A790, "4d8b7f10", "descend [node+0x10] (right)"),
    (0x4A7A0, "498b07", "descend [node+0x00] (left)"),
    (0x4A7AA, "74c7", "je back to the right descent"),
    (0x4A7B0, "4d89e7", "candidate = last_left"),
    (0x4A7B3, "4c8b65c0", "reload this+0x80"),
    (0x4A7B7, "488d15c2e1e502", "lea rdx,[0x2ea8980]"),
    (0x4A7C5, "4d39ef", "cmp candidate, anchor"),
    (0x4A7C8, "7420", "je -> insert test (end of tree)"),
    (0x4A7CA, "498b7720", "compare arg1 = [candidate+0x20]"),
    (0x4A7CE, "4889df", "compare arg2 = probe (arguments swapped)"),
    (0x4A7DA, "488d159fe1e502", "lea rdx,[0x2ea8980]"),
    (0x4A7E3, "4d0f48fd", "cmovs candidate = anchor (not present -> insert)"),
    (0x4A7EA, "4c39e9", "cmp candidate, anchor"),
    (0x4A7ED, "7516", "jne -> skip insert (present)"),
    (0x4A7EF, "488b7dc8", "rdi = [rsp-0x38] = this+0x78 (tree handle)"),
    (0x4A7F8, "488d1581e1e502", "lea rdx,[0x2ea8980]"),
    (0x4A7FF, "4c8930", "*(mapped) = entry"),
    (0x4A805, "4d8b7628", "next = [entry+0x28]"),
    (0x4A809, "4c8932", "*(0x2ea8980) = next"),
    (0x4A815, "4883c418", "add rsp,0x18"),
    (0x4A823, "c3", "ret"),
    # insert primitive 0x4b3f0
    (0x4B41E, "80781900", "cmp byte [root+0x19],0 (empty tree)"),
    (0x4B428, "4c89eb", "rbx = anchor"),
    (0x4B430, "bf48000000", "mov edi,0x48 (node size)"),
    (0x4B443, "498b4c2408", "rcx = [tree+0x08] (anchor)"),
    (0x4B44C, "488908", "node->left = anchor"),
    (0x4B44F, "48894808", "node->parent = anchor"),
    (0x4B453, "48894810", "node->right = anchor"),
    (0x4B457, "66c740180000", "colour = red, is_anchor = 0"),
    (0x4B45D, "c4c1781007", "vmovups xmm0,[value]"),
    (0x4B462, "c5f8114020", "node[+0x20] = value[0x00..0x10]"),
    (0x4B46B, "48894830", "node[+0x30] = value[+0x10]"),
    (0x4B46F, "498b4f18", "rcx = value[+0x18] (control)"),
    (0x4B473, "48894838", "node[+0x38] = control"),
    (0x4B477, "4885c9", "test control,control"),
    (0x4B47C, "f048ff01", "lock incq [control]"),
    (0x4B480, "48c7404000000000", "node->mapped = 0"),
    (0x4B4C5, "4883c340", "return &node->mapped (+0x40)"),
    (0x4B4F0, "488b7b20", "insert search: [node+0x20]"),
    (0x4B4F7, "e80440f400", "call strcmp"),
    (0x4B500, "488b5b10", "insert search: [node+0x10]"),
    (0x4B510, "488b03", "insert search: [node+0x00]"),
    (0x4B529, "488b7320", "insert equivalent-key test: [candidate+0x20]"),
    (0x4B52D, "e8ce3ff400", "call strcmp"),
    # fixup: node count
    (0x4C7D7, "49ffc0", "inc r8"),
    (0x4C7DA, "4c894610", "tree[+0x10] = count"),
    (0x4C7DE, "49894908", "tree[+0x08] = node (anchor cell)"),
    (0x4C7E6, "48394e08", "cmp [tree+0x08],rcx"),
]

TREE_CELL = 0x2EA8980

# Row-21 install site in `0x2569d0` and the destructor contract.
ROW_SITE = [
    (0x25706B, "bf90000000", "mov edi,0x90"),
    (0x257070, "e8fbffdbff", "call 0x17070"),
    (0x25707B, "e81036dfff", "call 0x4a690"),
    (0x257080, "4c8d2df118c502", "lea r13,[0x2ea8978]"),
    (0x25708B, "4c39e3", "cmp rbx,r12"),
    (0x257098, "e8434ddfff", "call 0x4bde0 (dtor)"),
    (0x2570A0, "e80b00dcff", "call 0x170b0 (free)"),
    (0x2570A5, "4d896500", "mov [r13],r12"),
]

DTOR_START, DTOR_END = 0x4BDE0, 0x4BEA9
DTOR_CALLS = [
    (0x4BDEA, 0x4AC80),
    (0x4BE22, 0x4BEB0),
    (0x4BE2E, 0x170B0),
    (0x4BE37, 0x3B380),
    (0x4BE40, 0xF590),
    (0x4BE49, 0xF590),
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


def call_target(elf: bytes, segments, site: int) -> int:
    rel = struct.unpack("<i", read_rva(elf, segments, site + 1, 4))[0]
    return site + 5 + rel


def lea_target(elf: bytes, segments, site: int, disp_off: int = 3, length: int = 7) -> int:
    disp = struct.unpack("<i", read_rva(elf, segments, site + disp_off, 4))[0]
    return site + length + disp


def pin(elf: bytes, segments, addr: int, pattern: str, what: str) -> None:
    size = len(pattern) // 2
    require(read_rva(elf, segments, addr, size) == bytes.fromhex(pattern), f"{what} @ {addr:#x}")


def run_census() -> dict:
    sys.path.insert(0, str(EVIDENCE))
    import census  # noqa: PLC0415

    return census.census(BODY_START, BODY_END)


def check_census() -> int:
    data = run_census()
    require(data["insn_count"] == 104, f"instruction count {data['insn_count']} != 104")
    require(data["nop_count"] == 4, f"nop count {data['nop_count']} != 4")
    require([(a, t) for a, t in data["calls"]] == CALLS, "call sites/targets")
    require(len(DISTINCT_TARGETS) == 6, "six distinct call targets")
    require(not any(target is None for _, target in data["calls"]), "indirect call in the body")
    require([(a, m, d) for a, m, d in data["stores"]] == STORES, "17-site store census")
    require([(a, m, t) for a, m, t in data["branches"]] == BRANCHES, "12-entry branch table")
    return data["insn_count"]


def check_body(elf: bytes, segments) -> None:
    body = read_rva(elf, segments, BODY_START, BODY_END - BODY_START)
    require(len(body) == 0x194, f"body size {len(body):#x}")
    require(hashlib.sha256(body).hexdigest() == BODY_SHA256, "body sha256")
    require(body[0] == 0x55 and body[-1] == 0xC3, "push rbp / ret boundaries")
    require(read_rva(elf, segments, BODY_END, 12) == b"\x90" * 12, "12 B 0x90 padding")
    require(b"\x64\x48\x8b\x04\x25" not in body, "unexpected stack canary")
    # The next function (0x4a830, FDE size 0xdb) is a sibling constructor of the same family;
    # the body must not spill into it.
    require(read_rva(elf, segments, 0x4A830, 4) == bytes.fromhex("554889e5"), "next function")
    for addr, pattern, what in PINNED:
        pin(elf, segments, addr, pattern, what)


def check_calls_and_cells(elf: bytes, segments) -> None:
    require([(site, call_target(elf, segments, site)) for site, _ in CALLS] == CALLS,
            "call encodings do not resolve to CALLS")
    for site in (0x4A705, 0x4A7B7, 0x4A7DA, 0x4A7F8):
        require(lea_target(elf, segments, site) == TREE_CELL,
                f"tree-cell lea @ {site:#x} -> {lea_target(elf, segments, site):#x}")
    # 0xf8f500 is a PLT stub (`jmp qword ptr [rip+..]`); the symbol name is not in the stripped
    # image, but the same PLT is the comparator of the resource-pool map (E593).
    require(read_rva(elf, segments, 0xF8F500, 6) == bytes.fromhex("ff256a0b6700"),
            "0xf8f500 PLT stub")
    require(0xF8F506 + struct.unpack("<i", read_rva(elf, segments, 0xF8F502, 4))[0] == 0x1600070,
            "0xf8f500 GOT slot")
    # The stamp provider returns a 16 B out-param pair (rax/rdx) filled by PLT 0xf8fd60.
    require(read_rva(elf, segments, 0x51200, 4) == bytes.fromhex("488d7de0"), "0x511f0 out param")
    require(read_rva(elf, segments, 0x5120B, 5) == bytes.fromhex("e850ebf300"), "0x511f0 -> 0xf8fd60")
    require(call_target(elf, segments, 0x5120B) == 0xF8FD60, "0x511f0 stamp import")
    require(read_rva(elf, segments, 0x51210, 8) == bytes.fromhex("488b45e0488b55e8"),
            "0x511f0 returns the pair in rax/rdx")
    # 0x3b310: four import calls; the third carries the name "CritSec" (0xf97deb), the same
    # string pool that holds "PS4SyncEvent" (0xf97df3).
    require(read_rva(elf, segments, 0xF97DEB, 8) == b"CritSec\x00", "0x3b310 mutex name")
    require(lea_target(elf, segments, 0x3B347) == 0xF97DEB, "0x3b310 name argument")


def check_row_site(elf: bytes, segments) -> None:
    for addr, pattern, what in ROW_SITE:
        pin(elf, segments, addr, pattern, what)
    require(call_target(elf, segments, 0x257070) == 0x17070, "row site allocator")
    require(call_target(elf, segments, 0x25707B) == 0x4A690, "row site ctor")
    require(lea_target(elf, segments, 0x257080) == 0x2EA8978, "row site destination")
    require(call_target(elf, segments, 0x257098) == 0x4BDE0, "row site dtor")
    require(call_target(elf, segments, 0x2570A0) == 0x170B0, "row site free")


def check_dtor_contract(elf: bytes, segments) -> None:
    require(read_rva(elf, segments, DTOR_START, 4) == bytes.fromhex("554889e5"),
            "dtor 0x4bde0 prologue")
    require(read_rva(elf, segments, DTOR_END - 1, 1) == b"\xc3", "dtor ret")
    require([(site, call_target(elf, segments, site)) for site, _ in DTOR_CALLS] == DTOR_CALLS,
            "dtor call order")


def check_native_sources() -> None:
    header = HEADER.read_text()
    for token in (
        "kOwnerCtor4A690Rva = 0x4A690",
        "kOwnerCtor4A690Size = 0x194",
        "kOwnerCtor4A690InsnCount = 104",
        "kOwnerCtor4A690ObjSize = 0x90",
        "kOwnerCtor4A690SlotRva = 0x2EA8978",
        "kOwnerCtor4A690QueueHeadRva = 0x2EA8980",
        "kOwnerCtor4A690Tag = 0x7D03",
        "kOwnerMemberRow21 = 21",
        "kOwnerRow21NodeSize = 0x48",
        "kOwnerRow21HeadOffset = 0x00",
        "kOwnerRow21SpareOffset = 0x10",
        "kOwnerRow21Shared0Offset = 0x18",
        "kOwnerRow21Shared1Offset = 0x38",
        "kOwnerRow21Stamp0Offset = 0x58",
        "kOwnerRow21Stamp1Offset = 0x60",
        "kOwnerRow21TagOffset = 0x68",
        "kOwnerRow21MutexOffset = 0x70",
        "kOwnerRow21TreeOffset = 0x78",
        "kOwnerRow21AnchorOffset = 0x80",
        "kOwnerRow21SizeOffset = 0x88",
        "kOwnerRow21QueueValueOffset = 0x08",
        "kOwnerRow21QueueNextOffset = 0x28",
        "kOwnerRow21QueueNodeSize = 0x30",
        "kOwnerRow21KeyOffset = 0x20",
        "kOwnerRow21MappedOffset = 0x40",
        "kOwnerDtor4BDE0Rva = 0x4BDE0",
        "insert_unique",
        "**not** a tree",
        "not `0x17090`",
        "drains",
        "owner_row21_member_ctor",
        "CritSec",
        "not modelled",
    ):
        require(token in header, f"header token {token!r}")

    source = SOURCE.read_text()
    for token in (
        "init_slot_f180",
        "kOwnerCtor4A690Tag",
        "kOwnerRow21QueueHeadRva" if "kOwnerRow21QueueHeadRva" in source else "kOwnerRow21QueueValueOffset",
        "std::atomic_ref<u64>",
        "memory_order_seq_cst",
        "search_lower_bound",
        "insert_fixup",
        "refresh_extremes",
        "rotate_left",
        "rotate_right",
        '"null-object"',
        '"object-align"',
        '"stamp"',
        '"mutex_init"',
        '"anchor_alloc"',
        '"queue"',
        '"compare"',
        '"null-key"',
        '"node_alloc"',
        "kOwnerRow21KeyControlOffset",
        "kOwnerRow21SizeOffset",
        "owner_row21_set_hooks",
    ):
        require(token in source, f"source token {token!r}")
    require("memset(o, 0" not in source, "source must not zero-fill the object")
    require("owner_members.hpp" not in source, "the ctor must not depend on owner_members")

    test = TEST.read_text()
    for token in (
        "kInsertShape",
        "kInsertCompares",
        "rb_invariants",
        "in_order",
        "object_offset_written",
        "expect_prefix_bytes",
        "null-key",
        "node_alloc",
        "anchor_alloc",
        "owner_row21_member_ctor",
        "SOTC_DUMP_4A690",
        "0x40 bytes 0xa5",
    ):
        require(token in test, f"test token {token!r}")
    require("kInsertShape =\n    \"A:" not in test, "shape pin left empty")


def main() -> int:
    require(ELF_PATH.is_file(), f"{ELF_PATH} is absent (private game image required)")
    require(EVIDENCE.is_dir(), f"{EVIDENCE} is absent (private characterization required)")
    require((EVIDENCE / "census.py").is_file(), "census.py is absent")
    elf = ELF_PATH.read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    segments = load_segments(elf)

    check_body(elf, segments)
    insn_count = check_census()
    check_calls_and_cells(elf, segments)
    check_row_site(elf, segments)
    check_dtor_contract(elf, segments)
    check_native_sources()

    print(
        f"OK check_fowner_ctor_4a690: body 0x194 sha256 {BODY_SHA256[:12]}..., "
        f"{insn_count} insns (4 nop pads), 8 call sites / 6 targets "
        f"({', '.join(hex(t) for t in DISTINCT_TARGETS)}), 17 stores "
        f"(12 object/anchor + 3 spills + 2 indirect), 12 branches, "
        f"node 0x48 / key +0x20 / mapped +0x40, tree cell 0x2ea8980 x4, "
        f"row site 0x25707b -> 0x4a690 @ 0x2ea8978, dtor 0x4bde0 contract; "
        f"guest bytes NOT executed"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
